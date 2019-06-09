#include <unordered_set>
#include <string>
using namespace std::literals;

#include "hud.h"
#include "cl_util.h"
#include "parsemsg.h"
#include "demo_api.h"

DECLARE_MESSAGE(m_Countdown, Countdown);

int CHudCountdown::Init()
{
	HOOK_MESSAGE(Countdown);

	m_iFlags = 0;

	name1[ARRAYSIZE(name1) - 1] = '\0';
	name2[ARRAYSIZE(name2) - 1] = '\0';

	cl_autorecord = CVAR_CREATE("cl_autorecord", "0", FCVAR_ARCHIVE);

	gHUD.AddHudElem(this);
	return 0;
}

int CHudCountdown::VidInit()
{
	m_iFlags &= ~HUD_ACTIVE;

	return 1;
}

int CHudCountdown::Draw(float time)
{
	int r, g, b;
	UnpackRGB(r, g, b, gHUD.m_iDefaultHUDColor);

	char str[72];

	if (seconds_left == 50) {
		if (name1[0] != '\0')
			sprintf(str, "The last round was won by %s^0!", name1);
		else
			sprintf(str, "Waiting for players to get ready...");
	} else {
		if (gHUD.m_flTime >= draw_until) {
			m_iFlags &= ~HUD_ACTIVE;
			return 0;
		}

		if (name1[0] != '\0' && name2[0] != '\0')
			sprintf(str, "%s^0 vs. %s", name1, name2);
		else
			sprintf(str, "The match is about to start.");

		gHUD.DrawHudNumberCentered(ScreenWidth / 2, gHUD.m_scrinfo.iCharHeight * 10, seconds_left, r, g, b);
	}

	gHUD.DrawHudStringCenteredWithColorTags(ScreenWidth / 2, gHUD.m_scrinfo.iCharHeight * 7, str, r, g, b);

	return 0;
}

static const char* sound_names[] = {
	"barney/ba_bring.wav",
	"fvox/one.wav",
	"fvox/two.wav",
	"fvox/three.wav",
	"fvox/four.wav",
	"fvox/five.wav",
	"fvox/six.wav",
	"fvox/seven.wav",
	"fvox/eight.wav",
	"fvox/nine.wav",
	"fvox/ten.wav"
};

// Blacklist all round-based modes from autorecord on countdown.
static std::unordered_set<std::string> autorecord_gamemode_blacklist = {
	"AG Arena"s,
	"AG LTS"s,
	"AG LMS"s,
	"AG CTF II"s,
};

int CHudCountdown::MsgFunc_Countdown(const char* name, int size, void* buf)
{
	BEGIN_READ(buf, size);

	seconds_left = READ_BYTE();
	bool play_sound = (READ_BYTE() != 0);

	strncpy(name1, READ_STRING(), ARRAYSIZE(name1) - 1);
	strncpy(name2, READ_STRING(), ARRAYSIZE(name2) - 1);

	if (seconds_left >= 0) {
		m_iFlags |= HUD_ACTIVE;

		if (play_sound && seconds_left <= 10) {
			gEngfuncs.pfnPlaySoundByName(sound_names[seconds_left], 1.0f);

			if (!gEngfuncs.pDemoAPI->IsRecording() && cl_autorecord->value > 0.0f
			    && autorecord_gamemode_blacklist.find(gHUD.m_Settings.GetGamemode())
			       == autorecord_gamemode_blacklist.cend())
				gEngfuncs.pfnClientCmd("agrecord");

			draw_until = gHUD.m_flTime + 5.0f;
		}
	} else {
		m_iFlags &= ~HUD_ACTIVE;
	}

	//gEngfuncs.Con_Printf("Countdown: seconds_left = %hhd; play_sound = %s; name1 = `%s`; name2 = `%s`.\n", seconds_left, play_sound ? "true" : "false", name1, name2);

	return 1;
}
