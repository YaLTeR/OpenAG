#include "hud.h"
#include "cl_util.h"
#include "parsemsg.h"

DECLARE_MESSAGE(m_CTF, CTF);
DECLARE_MESSAGE(m_CTF, CTFSound);
DECLARE_MESSAGE(m_CTF, CTFFlag);

int CHudCTF::Init()
{
	HOOK_MESSAGE(CTF);
	HOOK_MESSAGE(CTFSound);
	HOOK_MESSAGE(CTFFlag);

	m_iFlags = 0;

	gHUD.AddHudElem(this);
	return 0;
}

int CHudCTF::VidInit()
{
	flag_sprite_indices[0] = gHUD.GetSpriteIndex("icon_ctf_home");
	flag_sprite_indices[1] = gHUD.GetSpriteIndex("icon_ctf_stolen");
	flag_sprite_indices[2] = gHUD.GetSpriteIndex("icon_ctf_lost");
	flag_sprite_indices[3] = gHUD.GetSpriteIndex("icon_ctf_carry");

	for (int i = 0; i < 4; ++i)
		flag_sprites[i] = gHUD.GetSprite(flag_sprite_indices[i]);

	blue_flag_player_index = 0;
	red_flag_player_index = 0;

	m_iFlags &= ~HUD_ACTIVE;

	return 1;
}

int CHudCTF::Draw(float time)
{
	if (gHUD.GetGameType() != 3) {
		m_iFlags &= ~HUD_ACTIVE;
		return 0;
	}

	auto blue_flag_sprite = flag_sprites[static_cast<int>(blue_flag_state)];
	if (blue_flag_sprite) {
		auto blue_flag_sprite_index = flag_sprite_indices[static_cast<int>(blue_flag_state)];
		auto& blue_flag_sprite_rect = gHUD.GetSpriteRect(blue_flag_sprite_index);
		auto y = ScreenHeight / 2 + blue_flag_sprite_rect.top - blue_flag_sprite_rect.bottom - 5;

		SPR_Set(blue_flag_sprite, 125, 165, 210);
		SPR_DrawAdditive(0, 30, y, &blue_flag_sprite_rect);
	}

	auto red_flag_sprite = flag_sprites[static_cast<int>(red_flag_state)];
	if (red_flag_sprite) {
		auto red_flag_sprite_index = flag_sprite_indices[static_cast<int>(red_flag_state)];

		SPR_Set(red_flag_sprite, 200, 90, 70);
		SPR_DrawAdditive(0, 30, ScreenHeight / 2, &gHUD.GetSpriteRect(red_flag_sprite_index));
	}

	return 0;
}

int CHudCTF::MsgFunc_CTF(const char* name, int size, void* buf)
{
	BEGIN_READ(buf, size);

	int blue_flag = READ_BYTE();
	int red_flag = READ_BYTE();

	if (blue_flag == -1 || red_flag == -1) {
		m_iFlags &= ~HUD_ACTIVE;
		return 1;
	} else {
		m_iFlags |= HUD_ACTIVE;
	}

	blue_flag = max(0, min(4, blue_flag));
	red_flag = max(0, min(4, red_flag));

	blue_flag_state = static_cast<FlagState>(blue_flag);
	red_flag_state = static_cast<FlagState>(red_flag);

	// gEngfuncs.Con_Printf("CTF: blue_flag = %d; red_flag = %d\n", blue_flag, red_flag);

	return 1;
}

static const char* sound_names[] = {
	"ctf/youhaveflag.wav",
	"ctf/teamhaveflag.wav",
	"ctf/enemyhaveflag.wav",
	"ctf/blueflagreturned.wav",
	"ctf/redflagreturned.wav",
	"ctf/bluescores.wav",
	"ctf/redscores.wav",
	"ctf/blueflagstolen.wav",
	"ctf/redflagstolen.wav",
	"ctf/blueleads.wav",
	"ctf/redleads.wav",
	"ctf/teamsaretied.wav",
	"ctf/suddendeath.wav"
	// "ctf/stolenctf/capture" - doesn't exist at all?
};

int CHudCTF::MsgFunc_CTFSound(const char* name, int size, void* buf)
{
	BEGIN_READ(buf, size);

	int sound_index = READ_BYTE();

	if (sound_index >= 0 && sound_index < ARRAYSIZE(sound_names))
		gEngfuncs.pfnPlaySoundByName(sound_names[sound_index], 1.0f);

	return 1;
}

int CHudCTF::MsgFunc_CTFFlag(const char* name, int size, void* buf)
{
	BEGIN_READ(buf, size);

	blue_flag_player_index = READ_BYTE();
	red_flag_player_index = READ_BYTE();

	return 1;
}
