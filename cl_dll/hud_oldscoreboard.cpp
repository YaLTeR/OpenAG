#include "hud.h"
#include "cl_util.h"
#include <string.h>
#include <stdio.h>
#include <demo_api.h>
#include "vgui_TeamFortressViewport.h"
#include "hud_oldscoreboard.h"
#include "vgui_ScorePanel.h"
#include "steam_id.h"

// Y positions
// Those who play on killed-off 32bit Apple don't deserve old_scoreboard looking good
// as I am too lazy to build & test in a MacOS, sorry.
#ifdef LINUX
	#define ROW_RANGE_MIN (gHUD.m_scrinfo.iCharHeight + 3)

	/* The scoreboard's default width is set to 380 on Linux
	 * since it doesn't look good (Ping/loss doesn't fit and overlaps "Deaths" etc.)
	 * on the (original) 320 default, like it does on Windows,
	 * since the engine's Trebuchet MS is bigger than on Windows
	 * WINDOWS = Trebuchet MS = 9x8
	 * LINUX = Trebuchet MS = 11x10
	 */
	#define DEFAULT_WIDTH "380"
#else
	#define ROW_RANGE_MIN (gHUD.m_scrinfo.iCharHeight + 2)
	#define DEFAULT_WIDTH "320"
#endif // LINUX

#define ROW_GAP (gHUD.m_scrinfo.iCharHeight - 5)
#define ROW_RANGE_MAX ( ScreenHeight - 50 )

#define TEAM_NO             0
#define TEAM_YES            1
#define TEAM_SPECTATORS     2
#define TEAM_BLANK          3

int CHudOldScoreboard::Init(void)
{
	m_pCvarOldScoreboard = CVAR_CREATE("cl_old_scoreboard", "0", FCVAR_ARCHIVE);
	m_pCvarOldScoreboardWidth = CVAR_CREATE("cl_old_scoreboard_width", DEFAULT_WIDTH, FCVAR_ARCHIVE);

	m_iFlags = 0;

	gHUD.AddHudElem(this);
	return 1;
};

int CHudOldScoreboard::VidInit(void)
{
	m_iFlags &= ~HUD_ACTIVE; // m_iFlags |= HUD_ACTIVE;
	m_iFlags |= HUD_INTERMISSION; // is always drawn during an intermission

	int m_iSprite = 0;
	m_iSprite = gHUD.GetSpriteIndex("icon_ctf_score");
	m_IconFlagScore.spr = gHUD.GetSprite(m_iSprite);
	m_IconFlagScore.rc = gHUD.GetSpriteRect(m_iSprite);
	m_WidthScale = m_pCvarOldScoreboardWidth->value / 320.0f;

	return 1;
}

void CHudOldScoreboard::Reset(void)
{
}

bool CHudOldScoreboard::IsVisible()
{
	return m_iFlags & HUD_ACTIVE;
}

void CHudOldScoreboard::ShowScoreboard(bool bShow)
{
	if (bShow && gViewPort && gViewPort->m_pScoreBoard)
	{
		gViewPort->m_pScoreBoard->RebuildTeams();
		m_iFlags |= HUD_ACTIVE;
	}
	else
		m_iFlags &= ~HUD_ACTIVE;
}

int CHudOldScoreboard::Draw(float fTime)
{
	if (!IsVisible())
		return 1;

	// This is now calculated so that the HLKreedz timer is not in the way on ANY resolution
	// Since the Y pos of the timer is changed with different resolution
	// It's also calculated dynamically since one can change the ScreenHeight by resizing the window during game
	int ROW_TOP = (ScreenHeight / 320.0f) * 30.0f;

	// Let users use 320 even on Linux, if they really want to
	int width = max(min((int)m_pCvarOldScoreboardWidth->value, ScreenWidth), 320);
	m_WidthScale = (float)width / 320.0f;

	/*
	 * X positions, relative to the side of the scoreboard
	 * This is as close to the original as we can get more or less
	 * And yes, the original scoreboard isn't perfectly centered,
	 * even that is preserved :)
	 */
	int NAME_RANGE_MIN = 20 * m_WidthScale;
	int NAME_RANGE_MAX = 145 * m_WidthScale;
	int KILLS_RANGE_MIN = 130 * m_WidthScale;
	int KILLS_RANGE_MAX = 160 * m_WidthScale;
	int DIVIDER_POS     = 180 * m_WidthScale;
	int DEATHS_RANGE_MIN = 190 * m_WidthScale; // og 185
	int DEATHS_RANGE_MAX = 220 * m_WidthScale;
	int PING_RANGE_MAX  = 295 * m_WidthScale;
	int END = 315 * m_WidthScale;

	int r, g, b;
	UnpackRGB(r,g,b, gHUD.m_iDefaultHUDColor);

	int xpos = 0;
	int ypos = 0;
	float list_slot = 0;
	int xpos_rel = (ScreenWidth - width) / 2; // scale the scoreboard based on the cvar

	// print the heading line
	ypos = ROW_TOP + ROW_RANGE_MIN + (list_slot * ROW_GAP);
	xpos = NAME_RANGE_MIN + xpos_rel;

	if (!gHUD.m_Teamplay)
		gHUD.DrawHudString(xpos, ypos, NAME_RANGE_MAX + xpos_rel, CHudTextMessage::BufferedLocaliseTextString("#PLAYERS"), r, g, b);
	else
		gHUD.DrawHudString(xpos, ypos, NAME_RANGE_MAX + xpos_rel, CHudTextMessage::BufferedLocaliseTextString("#TEAMS"), r, g, b);

	// can't use #SCORE as RightAligned is not a friend with BufferedLocaliseTextString for some reason, sorry
	gHUD.DrawHudStringRightAligned(KILLS_RANGE_MAX + xpos_rel, ypos, CHudTextMessage::BufferedLocaliseTextString("Score"), r, g, b);

	gHUD.DrawHudStringCentered(DIVIDER_POS + xpos_rel, ypos, "/", r, g, b);

	gHUD.DrawHudString(DEATHS_RANGE_MIN + xpos_rel, ypos, ScreenWidth, CHudTextMessage::BufferedLocaliseTextString("#DEATHS"), r, g, b);

	// can't use #LATENCY as RightAligned is not a friend with BufferedLocaliseTextString for some reason, sorry
	xpos = width + xpos_rel - (width - END);
	gHUD.DrawHudStringRightAligned(xpos, ypos, "Ping/loss", r, g, b);

	list_slot += 1.5;
	ypos = ROW_TOP + ROW_RANGE_MIN + (list_slot * ROW_GAP);
	xpos = NAME_RANGE_MIN + xpos_rel;
	FillRGBA(xpos, ypos, PING_RANGE_MAX, 1, r, g, b, 255); // draw the seperator line

	list_slot += 0.8;

	// draw the players, in order
	const auto scoreboard = gViewPort->GetScoreBoard();
	gViewPort->GetAllPlayersInfo();

	for (int iRow = 0; iRow < scoreboard->m_iRows; ++iRow)
	{
		const auto sorted = scoreboard->m_iSortedRows[iRow];
		int nameoffset = 0;

		if (scoreboard->m_iIsATeam[iRow] == TEAM_BLANK)
			continue;

		else if (scoreboard->m_iIsATeam[iRow] && gHUD.m_Teamplay)
		{
			// Draw team info line
			team_info_t* team_info = &g_TeamInfo[sorted];

			if (0 != iRow)
				list_slot += 0.3;
			ypos = ROW_TOP + ROW_RANGE_MIN + (list_slot * ROW_GAP);

			// check we haven't drawn too far down
			if ( ypos > ROW_RANGE_MAX )  // don't draw to close to the lower border
				break;

			xpos = NAME_RANGE_MIN + xpos_rel;

			char szTeamName[128];
			if (scoreboard->m_iIsATeam[iRow] == TEAM_SPECTATORS)
			{
				r = g = b = 100;
				snprintf(szTeamName, ARRAYSIZE(szTeamName), "%s", CHudTextMessage::BufferedLocaliseTextString( "#Spectators"));
			}
			else
			{
				r = iTeamColors[team_info->teamnumber % iNumberOfTeamColors][0];
				g = iTeamColors[team_info->teamnumber % iNumberOfTeamColors][1];
				b = iTeamColors[team_info->teamnumber % iNumberOfTeamColors][2];
				snprintf(szTeamName, ARRAYSIZE(szTeamName), "%s", team_info->name);
			}

			// Cut off the name if it'd overlap score
			// - 5 to allow for some more space between the name and kills
			while ( gHUD.GetHudStringWidth(szTeamName) + nameoffset + NAME_RANGE_MIN > KILLS_RANGE_MIN - 5 )
			{
				szTeamName[strlen(szTeamName) - 1] = '\0';
			}
			// draw their name (left to right)
			gHUD.DrawHudString(xpos, ypos, 0, szTeamName, r, g, b);

			// draw kills (right to left)
			xpos = KILLS_RANGE_MAX + xpos_rel;
			gHUD.DrawHudNumberStringFixed(xpos, ypos, team_info->frags, r, g, b);

			// draw divider
			xpos = DIVIDER_POS + xpos_rel;
			gHUD.DrawHudStringCentered(xpos, ypos, "/", r, g, b);

			// draw deaths
			xpos = DEATHS_RANGE_MAX + xpos_rel;
			gHUD.DrawHudNumberStringFixed(xpos, ypos, team_info->deaths, r, g, b);
			list_slot++;
		}
		else
		{
			// Draw player info line
			if (gHUD.m_Teamplay)
				nameoffset = 10;

			hud_player_info_t* pl_info = &g_PlayerInfoList[sorted];
			extra_player_info_t* pl_info_extra = &g_PlayerExtraInfo[sorted];

			if (pl_info->name == NULL)
			{
				gViewPort->m_pScoreBoard->RebuildTeams();
				continue;
			}

			ypos = ROW_TOP + ROW_RANGE_MIN + (list_slot * ROW_GAP);

			// check we haven't drawn too far down
			if ( ypos > ROW_RANGE_MAX )  // don't draw to close to the lower border
				break;

			r = iTeamColors[pl_info_extra->teamnumber % iNumberOfTeamColors][0];
			g = iTeamColors[pl_info_extra->teamnumber % iNumberOfTeamColors][1];
			b = iTeamColors[pl_info_extra->teamnumber % iNumberOfTeamColors][2];

			xpos = NAME_RANGE_MIN + xpos_rel; // Set xpos for name here so that the flag is at the correct position

			// If this player is carrying a CTF flag, draw the sprite for it
			if (gHUD.m_CTF.GetBlueFlagPlayerIndex() == sorted || gHUD.m_CTF.GetRedFlagPlayerIndex() == sorted)
			{
				SPR_Set(m_IconFlagScore.spr, 200, 200, 200);
				SPR_DrawAdditive(0, xpos - 10, ypos + 5, &m_IconFlagScore.rc);
			}

			if (pl_info->thisplayer) // if it's local player's name, draw it with a different color
			{
				r = g = b = 255;
				// overlay the background in blue, then draw the score text over it
				FillRGBA(NAME_RANGE_MIN + xpos_rel - 5, ypos + 5, PING_RANGE_MAX - 5, ROW_GAP, 0, 0, 255, 70);
			}

			char szName[128];
			int specoffset = 0;
			const char* name = nullptr;

			if (steam_id::is_showing_real_names())
				name = steam_id::get_real_name(scoreboard->m_iSortedRows[iRow] - 1).c_str();
			if (!name || name[0] == '\0')
				name = pl_info->name;

			snprintf(szName, ARRAYSIZE(szName), "%s", name);

			// If this player is a spectator we need to also fit " (S)" in, let's prepare for that
			if (g_IsSpectator[scoreboard->m_iSortedRows[iRow]])
			{
				std::string spec_str(" (S)");
				for( char& c : spec_str )
				{
					auto cwidth = gHUD.m_scrinfo.charWidths[ static_cast<unsigned char>(c) ];
					specoffset += cwidth;
				}
			}

			// cut off the name if it'd overlap score
			while ( gHUD.GetHudStringWidthWithColorTags(szName) + nameoffset + NAME_RANGE_MIN + specoffset > KILLS_RANGE_MIN )
			{
				szName[strlen(szName) - 1] = '\0';
			}

			// Now that we have space for it, add the (S)
			if (g_IsSpectator[scoreboard->m_iSortedRows[iRow]])
				strcat(szName, " (S)");

			gHUD.DrawHudStringWithColorTags(xpos + nameoffset, ypos, szName, r, g, b);

			// draw kills (right to left)
			xpos = KILLS_RANGE_MAX + xpos_rel;
			gHUD.DrawHudNumberStringFixed(xpos, ypos, pl_info_extra->frags, r, g, b);

			// draw divider
			xpos = DIVIDER_POS + xpos_rel;
			gHUD.DrawHudStringCentered(xpos, ypos, "/", r, g, b);

			// draw deaths
			xpos = DEATHS_RANGE_MAX + xpos_rel;
			gHUD.DrawHudNumberStringFixed(xpos, ypos, pl_info_extra->deaths, r, g, b);

			// draw ping & packetloss
			// Why 25: 320 (default width in the original AG) - 295 (PING_RANGE_MAX) = 25
			xpos = width + xpos_rel - (width - PING_RANGE_MAX);
			static char buf[64];
			snprintf(buf, ARRAYSIZE(buf), "%d/%d", (int)pl_info->ping, (int)pl_info->packetloss);
			gHUD.DrawHudStringRightAligned(xpos , ypos, buf, r, g, b);

			list_slot++;
		}
	}

	return 1;
}
