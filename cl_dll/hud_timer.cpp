#include "hud.h"
#include "cl_util.h"
#include "parsemsg.h"
#include "discord_integration.h"

DECLARE_MESSAGE(m_Timer, Timer);

static void unpack_seconds(int seconds_total, int& days, int& hours, int& minutes, int& seconds)
{
	constexpr int SECONDS_PER_MINUTE = 60;
	constexpr int SECONDS_PER_HOUR = SECONDS_PER_MINUTE * 60;
	constexpr int SECONDS_PER_DAY = SECONDS_PER_HOUR * 24;

	days = seconds_total / SECONDS_PER_DAY;
	seconds_total %= SECONDS_PER_DAY;

	hours = seconds_total / SECONDS_PER_HOUR;
	seconds_total %= SECONDS_PER_HOUR;

	minutes = seconds_total / SECONDS_PER_MINUTE;
	seconds_total %= SECONDS_PER_MINUTE;

	seconds = seconds_total;
}

int CHudTimer::Init()
{
	HOOK_MESSAGE(Timer);

	m_iFlags = 0;
	hud_timer = CVAR_CREATE("hud_timer", "1", FCVAR_ARCHIVE);

	gHUD.AddHudElem(this);
	return 0;
}

int CHudTimer::VidInit()
{
	m_iFlags &= ~HUD_ACTIVE;

	return 1;
}

int CHudTimer::Draw(float time)
{
	if (gHUD.m_flTime >= draw_until) {
		m_iFlags &= ~HUD_ACTIVE;
		return 0;
	}

	if (hud_timer->value == 0.0f)
		return 0;

	char str[32];

	int seconds_to_draw;
	if (hud_timer->value == 2.0f || seconds_total == 0)
		seconds_to_draw = seconds_passed;
	else
		seconds_to_draw = seconds_total - seconds_passed;

	int days, hours, minutes, seconds;
	unpack_seconds(seconds_to_draw, days, hours, minutes, seconds);

	if (days > 0)
		sprintf(str, "%d day%s %dh %dm %ds", days, (days > 1 ? "s" : ""), hours, minutes, seconds);
	else if (hours > 0)
		sprintf(str, "%dh %dm %ds", hours, minutes, seconds);
	else if (minutes > 0)
		sprintf(str, "%d:%02d", minutes, seconds);
	else if (seconds_to_draw >= 0)
		sprintf(str, "%d", seconds);
	else
		// Overtime!
		sprintf(str, "%d", seconds_to_draw);

	int r, g, b;
	UnpackRGB(r, g, b, gHUD.m_iDefaultHUDColor);

	gHUD.DrawHudStringCentered(ScreenWidth / 2, gHUD.m_scrinfo.iCharHeight, str, r, g, b);

	return 0;
}

int CHudTimer::MsgFunc_Timer(const char* name, int size, void* buf)
{
	BEGIN_READ(buf, size);

	seconds_total = READ_LONG();
	seconds_passed = READ_LONG();

	draw_until = gHUD.m_flTime + 5.0f;

	m_iFlags |= HUD_ACTIVE;

	discord_integration::set_time_data(seconds_total, seconds_passed);

	//gEngfuncs.Con_Printf("Timer: seconds_total = %d; seconds_passed = %d.\n", seconds_total, seconds_passed);

	return 1;
}
