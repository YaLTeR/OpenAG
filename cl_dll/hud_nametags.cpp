#include "hud.h"
#include "cl_util.h"
#include "cl_entity.h"
#include "triangleapi.h"
#include "vgui_TeamFortressViewport.h"
#include "vgui_SpectatorPanel.h"

#include "pm_shared.h"
#include "pm_defs.h"
#include "pmtrace.h"
#include "parsemsg.h"

// these are included for the math functions
#include "com_model.h"
#include "demo_api.h"
#include "event_api.h"
#include "screenfade.h"
#include "vgui_TeamFortressViewport.h"
#include "vgui_helpers.h"
#include "vgui_loadtga.h"

#ifdef _MSC_VER
#pragma warning(disable: 4244)
#endif

extern "C" float	Distance(const float * v1, const float * v2);
extern float * GetClientColor( int clientIndex );
extern void V_GetInEyePos(int entity, float * origin, float * angles );

//DECLARE_MESSAGE(m_NameTags, PlayerId);

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
int CHudNameTags::Init()
{
	//HOOK_MESSAGE(PlayerId);
	gHUD.AddHudElem(this);

	m_iFlags |= HUD_ACTIVE;

	m_hud_nametags		= gEngfuncs.pfnRegisterVariable("hud_nametags", "1", FCVAR_ARCHIVE);
	m_hud_nametags_team_max_distance = gEngfuncs.pfnRegisterVariable("hud_nametags_team_max_distance", "1", FCVAR_ARCHIVE);

	return 1;
}

//-----------------------------------------------------------------------------
// Purpose: Loads the ASCII table sprite
//-----------------------------------------------------------------------------
int CHudNameTags::VidInit()
{
	//m_iFlags &= ~HUD_ACTIVE;
	m_nameTagSprite = SPR_Load("sprites/ascii_table.spr"); //gEngfuncs.pfnSPR_Load("sprites/alphabet_test.spr"); //SPR_Load
	return 1;
}

bool CHudNameTags::IsTeamMate(cl_entity_t *localPlayer, int playerId)
{
	return !strcmp(g_PlayerExtraInfo[localPlayer->index].teamname,
			g_PlayerExtraInfo[playerId + 1].teamname)
			&& gHUD.m_Teamplay;
}

//-----------------------------------------------------------------------------
// Purpose: 
// Input  : flTime - 
//			intermission - 
//-----------------------------------------------------------------------------
int CHudNameTags::Draw(float flTime)
{
	// Only draw if client wants us to
	if(m_hud_nametags->value != 1)
		return 1;

	char string[512];
	float *color;

	vec3_t origin_above_target_head, world, screen;
	vec3_t origin_pl;
	vec3_t vecSrc;
	vec3_t vecTargetPlayer;
	vec3_t view_ofs;
	float			x,y,z;
	int 			r,g,b;

	// Get the sprite pointer
	auto hSpriteModel = (struct model_s *)gEngfuncs.GetSpritePointer(m_nameTagSprite);

	// make sure we have player info
	gViewPort->GetAllPlayersInfo();

	// get yellow/brown HUD color
	UnpackRGB(r, g, b, gHUD.m_iDefaultHUDColor);

	// get local player and get his eye level
	cl_entity_t *localPlayer = gEngfuncs.GetLocalPlayer();
	VectorCopy( localPlayer->origin, origin_pl );
	
	VectorClear(view_ofs); 

	if (localPlayer->curstate.usehull == 1) // if we're ducking
		view_ofs[2] = VEC_DUCK_VIEW;
	else
		view_ofs[2] = DEFAULT_VIEWHEIGHT;

	VectorAdd( origin_pl, view_ofs, vecSrc );

	//AngleVectors( angles_pl, forward, NULL, NULL ); // For get user aim

	for (int i = 0; i < MAX_PLAYERS; i++)
	{
		cl_entity_s *ent = gEngfuncs.GetEntityByIndex(i+1);
		
		// Target player not present on the server, or not in our PVS
		if(!ent || ent->curstate.messagenum < localPlayer->curstate.messagenum)
			continue; 

		// Target entity isn't a player entity
		if (!ent->player)
			continue;

		// Don't draw nickname for ourselves 
		if(ent == localPlayer)
			continue;

		// Don't draw for player without a name (TODO: ?)
		if (g_PlayerInfoList[i + 1].name == nullptr)
			continue;

		// Don't show a nametag for dead or players that are only spectating (ie: invisible entities).
		if (ent->curstate.effects & EF_NODRAW)
			continue;

		// Don't draw a nametag for player we are spectating in first person (incl. in PoV demos)
		if (g_iUser1 && ent->index == g_iUser2)
			continue;

		VectorCopy(ent->origin, origin_above_target_head);
		origin_above_target_head[2] += 45.0f; // Kinda above the head 
		// ^ TODO: change based on distance?

		// Calculate screen position for name and infromation in hud::draw()
		if ( gEngfuncs.pTriAPI->WorldToScreen(origin_above_target_head, screen) )
			continue;	// object is behind viewer

		color = GetClientColor(i + 1); // team color

		// draw the players name and health underneath
		char colorless_name[256];
		color_tags::strip_color_tags(colorless_name, g_PlayerInfoList[i + 1].name, ARRAYSIZE(colorless_name));
		sprintf(string, "%s", colorless_name);

		VectorCopy(ent->origin, vecTargetPlayer);
		vec3_t point, up, right;
		
		// Since the nametag is above the player's head, let's somewhat try to see if we can see his head
		if (ent->curstate.usehull == 1) 
			vecTargetPlayer[2] += VEC_DUCK_VIEW; // he's crouched, draw it lower
		else 
			vecTargetPlayer[2] += DEFAULT_VIEWHEIGHT; // he's standing up (or dead, well, can't tell)
		
		pmtrace_t * trace = gEngfuncs.PM_TraceLine( vecSrc, vecTargetPlayer, PM_TRACELINE_PHYSENTSONLY, 2, -1 );
		
		if ( trace->fraction == 1.0 )
		{
			// Or if we are playing a demo, show the nametag however far away he is
			// Or if client wants teammates to always appear, regardless of their distance (obv only when in our PVS and traceable)
			// Or if the gamemode is Kreedz
			// Or if the localplayer is spectating
			// TODO: V_GetInEyePos( g_iUser2, origin, angles );
			float distanceToPlayer = Distance(trace->endpos, localPlayer->origin);

			//if (ShouldDrawEvenIfPlayerIsNotTeamMate() || (distanceToPlayer < m_hud_nametags_team_max_distance->value && IsTeamMate(localPlayer, i)))
			if (true)
			{
				float screenPoints[3];
				gEngfuncs.pTriAPI->WorldToScreen(origin_above_target_head, screenPoints);

				screenPoints[0] = XPROJECT(screenPoints[0]);
				screenPoints[1] = YPROJECT(screenPoints[1]);
				screenPoints[2] = 0.0f;

				// see R_DrawSpriteModel

				gEngfuncs.pTriAPI->SpriteTexture( hSpriteModel, 0 );
				//gEngfuncs.pTriAPI->RenderMode( kRenderTransTexture );
				gEngfuncs.pTriAPI->RenderMode( kRenderTransAdd );
				gEngfuncs.pTriAPI->CullFace( TRI_NONE );
				//glEnable(GL_TEXTURE_2D);
				auto scale = (800 - 8) / (distanceToPlayer - 8); // TODO:
				scale = std::fmin(scale, 1.25f); // TODO:
				auto length = strlen(colorless_name);

				gEngfuncs.pTriAPI->Begin ( TRI_QUADS );
					for (int i = 0; i < sizeof(colorless_name) && colorless_name[i] != '\0'; i++)
					{
						char singleChar = colorless_name[i];
						int singleCharInt = (int)singleChar;

						int singleCharIntInSpriteTable = singleCharInt - ' ';

						int letterAlphabetNumber = singleCharIntInSpriteTable % 19;
						int line = singleCharIntInSpriteTable / 19;

						if (singleCharInt < 32 || singleCharInt > 126)
						{
							gEngfuncs.Con_Printf("Skipping character %c %i\n", singleChar, singleChar);
							continue;
						}
						auto x1 = 16.0f * letterAlphabetNumber; // for the left corner of the rectangle
						auto x2 = 16.0f * (letterAlphabetNumber + 1); // for the right corner of the rectangle
						auto a1 = (x1 - 0.0f) / (384 - 0); // 448 = width of the image // left corner of the texture selection rectangle
						auto a2 = (x2 - 0.0f) / (384 - 0); // right corner of the texture selection rectangle

						auto y1 = 32.0f * line; // for the top corner of the rectangle
						auto y2 = 32.0f * (line + 1); // for the bottom corner of the rectangle
						auto b1 = (y1 - 0.0f) / (160 - 0); // 160 = height of the sprite image
						auto b2 = (y2 - 0.0f) / (160 - 0);

						auto xpos = screenPoints[0] - (8 * scale * length / 2) + (i * 8 * scale);
						auto ypos = screenPoints[1]; // + scale;
						gEngfuncs.Con_Printf("%f %f | distance = %f | scale = %f | a1 = %f | a2 = %f | char = %c | letter = %i | x1 = %f | x2 = %f | xpos = %f | i = %d\n",
						                     screenPoints[0], screenPoints[1], distanceToPlayer, scale, a1, a2, singleChar, letterAlphabetNumber, x1, x2, xpos, i);
						gEngfuncs.Con_Printf("y1 = %f | y2 = %f | b1 = %f | b2 = %f", y1, y2, b1, b2);
						gEngfuncs.pTriAPI->TexCoord2f(a1, b1);
						gEngfuncs.pTriAPI->Vertex3f(xpos, ypos, 0);
						gEngfuncs.pTriAPI->TexCoord2f(a2, b1);
						gEngfuncs.pTriAPI->Vertex3f(xpos + 8*scale, ypos, 0);
						gEngfuncs.pTriAPI->TexCoord2f(a2, b2);
						gEngfuncs.pTriAPI->Vertex3f(xpos + 8*scale, ypos + 16*scale, 0);
						gEngfuncs.pTriAPI->TexCoord2f(a1, b2);
						gEngfuncs.pTriAPI->Vertex3f(xpos, ypos + 16*scale, 0);

					}
				gEngfuncs.pTriAPI->End ();
				gEngfuncs.pTriAPI->RenderMode( kRenderNormal );
			}
		}
	}

	return 1;
}
bool CHudNameTags::IsActivePlayer(cl_entity_t * ent)
{
	return ( ent && 
			 ent->player && 
			 ent->curstate.solid != SOLID_NOT &&
			 ent != gEngfuncs.GetLocalPlayer() &&
			 g_PlayerInfoList[ent->index].name != NULL
			);
}

bool CHudNameTags::ShouldDrawEvenIfPlayerIsNotTeamMate()
{
	// get server's gamemode
	auto gamemode = gHUD.m_Settings.GetGamemode();
	gEngfuncs.Con_Printf("RETURNING %i\n", (gEngfuncs.pDemoAPI->IsPlayingback() || !strcmp(gamemode, "Kreedz") || g_iUser1 || gEngfuncs.IsSpectateOnly()));
	return (gEngfuncs.pDemoAPI->IsPlayingback() || !strcmp(gamemode, "Kreedz") || g_iUser1 || gEngfuncs.IsSpectateOnly());
}

void CHudNameTags::Reset()
{

}

void CHudNameTags::InitHUDData()
{
	Reset();
}

// dont use rn
int CHudNameTags::MsgFunc_PlayerId(const char* name, int size, void* buf)
{
	BEGIN_READ(buf, size);

	player_id = READ_BYTE();
	teammate = (READ_BYTE() == 1);
	health = READ_SHORT();
	armor = READ_SHORT();

	m_iFlags |= HUD_ACTIVE;

	return 1;
}
