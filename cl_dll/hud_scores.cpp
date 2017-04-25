#include <algorithm>

#include "hud.h"
#include "cl_util.h"
#include "parsemsg.h"
#include "vgui_TeamFortressViewport.h"
#include "vgui_ScorePanel.h"

#include "steam_id.h"

#ifdef max
#undef max
#endif

extern int iNumberOfTeamColors;
extern int iTeamColors[5][3];
extern hud_player_info_t   g_PlayerInfoList [MAX_PLAYERS + 1];
extern extra_player_info_t g_PlayerExtraInfo[MAX_PLAYERS + 1];
extern team_info_t g_TeamInfo[MAX_TEAMS + 1];

int CHudScores::Init()
{
	m_iFlags = HUD_ACTIVE;

	cl_scores = CVAR_CREATE("cl_scores", "0", FCVAR_ARCHIVE);
	cl_scores_pos = CVAR_CREATE("cl_scores_pos", "20 50", FCVAR_ARCHIVE);

	gHUD.AddHudElem(this);
	return 0;
}

int CHudScores::VidInit()
{
	return 1;
}

int CHudScores::Draw(float time)
{
	if (cl_scores->value < 1.0f || rows.empty())
		return 0;

	const size_t rows_to_draw = static_cast<size_t>(cl_scores->value);

	int x, y;
	if (sscanf(cl_scores_pos->string, "%d %d", &x, &y) != 2) {
		x = 0;
		y = 0;
	}

	auto last_row = rows.cend();
	if (rows_to_draw < rows.size())
		last_row = rows.cbegin() + rows_to_draw;

	// Figure out the scoreboard dimensions.
	int score_width = 0, name_width = 0;
	for (auto row = rows.cbegin(); row != last_row; ++row) {
		char buf[16];
		std::snprintf(buf, ARRAYSIZE(buf), "%d", row->score);
		score_width = std::max(score_width, gHUD.GetHudStringWidth(buf));
		name_width = std::max(name_width, gHUD.GetHudStringWidthWithColorTags(const_cast<char*>(row->name.c_str())));
	}

	// Draw the scoreboard.
	constexpr int PADDING = 10; // Extra fill space from the sides of the scoreboard.
	constexpr int GAP = 10; // Space between the scores and the names.

	for (auto row = rows.cbegin(); row != last_row; ++row) {
		int r = iTeamColors[row->color][0],
		    g = iTeamColors[row->color][1],
		    b = iTeamColors[row->color][2];

		FillRGBA(x, y, PADDING + score_width + GAP + name_width + PADDING, gHUD.m_scrinfo.iCharHeight, r, g, b, 20);

		ScaleColors(r, g, b, 135);

		char buf[16];
		std::snprintf(buf, ARRAYSIZE(buf), "%d", row->score);
		gHUD.DrawHudStringRightAligned(x + PADDING + score_width, y, buf, r, g, b);

		gHUD.DrawHudStringWithColorTags(x + PADDING + score_width + GAP,
		                                y,
		                                const_cast<char*>(row->name.c_str()),
		                                r,
		                                g,
		                                b);

		y += gHUD.m_scrinfo.iCharHeight;
	}

	return 0;
}

void CHudScores::UpdateRows()
{
	extern TeamFortressViewport* gViewPort;
	const auto scoreboard = gViewPort->GetScoreBoard();
	constexpr size_t TEAM_YES = 1; // From vgui_ScorePanel.cpp.

	rows.clear();

	if (!gHUD.m_Teamplay) {
		for (int row = 0; row < scoreboard->m_iRows; ++row) {
			if (scoreboard->m_iIsATeam[row])
				continue;

			const auto sorted = scoreboard->m_iSortedRows[row];
			if (g_IsSpectator[sorted])
				continue;

			const int score = g_PlayerExtraInfo[sorted].frags;
			const uint8_t color = g_PlayerExtraInfo[sorted].teamnumber % iNumberOfTeamColors;
			std::string name;
			if (steam_id::is_showing_real_names())
				name = steam_id::get_real_name(sorted - 1);
			if (name.empty())
				name = g_PlayerInfoList[sorted].name;

			rows.emplace_back(score, std::move(name), color);
		}
	} else {
		for (int row = 0; row < scoreboard->m_iRows; ++row) {
			if (scoreboard->m_iIsATeam[row] != TEAM_YES)
				continue;

			const auto sorted = scoreboard->m_iSortedRows[row];

			const int score = g_TeamInfo[sorted].frags;
			const uint8_t color = g_TeamInfo[sorted].teamnumber % iNumberOfTeamColors;
			const std::string name = g_TeamInfo[sorted].name;

			rows.emplace_back(score, std::move(name), color);
		}
	}
}
