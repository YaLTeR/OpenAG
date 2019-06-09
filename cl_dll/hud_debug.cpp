#include <cstdio>

#include "hud.h"
#include "cl_util.h"
#include "parsemsg.h"
#include "vgui_TeamFortressViewport.h"

extern hud_player_info_t   g_PlayerInfoList [MAX_PLAYERS + 1];
extern extra_player_info_t g_PlayerExtraInfo[MAX_PLAYERS + 1];

static char* fmt(const char* fmt, ...)
{
	static char buffer[2048];

	va_list list;
	va_start(list, fmt);

	vsnprintf(buffer, ARRAYSIZE(buffer), fmt, list);

	va_end(list);

	return buffer;
}

static std::vector<std::string> split(const std::string& string, char c)
{
	std::vector<std::string> result;

	for (std::string::size_type pos = string.find(c), prev = 0; pos != std::string::npos; prev = pos + 1, pos = string.find(c, pos + 1))
		result.emplace_back(string, prev, pos - prev);

	return result;
}

void CHudDebug::DumpEntityData(const cl_entity_t* ent, float time)
{
	if (output && hud_debug_filename->string[0] == '\0') {
		fclose(output);
		output = nullptr;
	}

	if (!output && hud_debug_filename->string[0] != '\0') {
		output = fopen(hud_debug_filename->string, "w");
	}

	if (!output)
		return;

	fprintf(output, "%.8f ", time);
	fprintf(output, "%.8f %.8f %.8f ", ent->origin[0], ent->origin[1], ent->origin[2]);
	fprintf(output, "%.8f %.8f %.8f %.8f ", ent->curstate.msg_time, ent->curstate.origin[0], ent->curstate.origin[1], ent->curstate.origin[2]);
	fprintf(output, "%.8f %.8f %.8f %.8f ", ent->prevstate.msg_time, ent->prevstate.origin[0], ent->prevstate.origin[1], ent->prevstate.origin[2]);

	const auto& posdata = ent->ph[ent->current_position];
	fprintf(output, "%.8f ", posdata.animtime);
	fprintf(output, "%.8f %.8f %.8f ", posdata.origin[0], posdata.origin[1], posdata.origin[2]);

	fputc('\n', output);
}

int CHudDebug::Init()
{
	hud_debug = CVAR_CREATE("hud_debug", "0", 0);
	hud_debug_filename = CVAR_CREATE("hud_debug_filename", "", 0);
	output = nullptr;

	m_iFlags = HUD_ACTIVE;

	gHUD.AddHudElem(this);
	return 0;
}

int CHudDebug::VidInit()
{
	return 1;
}

int CHudDebug::Draw(float time)
{
	if (hud_debug->value == 0.0f)
		return 0;

	int r, g, b;
	UnpackRGB(r, g, b, gHUD.m_iDefaultHUDColor);

	int y = 50;

	gHUD.DrawHudString(0, y, 0, fmt("time: %.8f", time), r, g, b);
	gHUD.DrawHudString(0, y += gHUD.m_scrinfo.iCharHeight, 0, fmt("GetClientTime: %.8f", gEngfuncs.GetClientTime()), r, g, b);
	gHUD.DrawHudString(0, y += gHUD.m_scrinfo.iCharHeight, 0, fmt("GetClientOldTime: %.8f", gEngfuncs.hudGetClientOldTime()), r, g, b);

	for (const auto& str : split(additional_data, '\n'))
		gHUD.DrawHudString(0, y += gHUD.m_scrinfo.iCharHeight, 0, const_cast<char*>(str.c_str()), r, g, b);

	additional_data.clear();

	return 0;
}

void CHudDebug::AddString(const std::string& string)
{
	additional_data += string;
}
