//========= Copyright � 1996-2001, Valve LLC, All rights reserved. ============
//
// Purpose: 
//
// $NoKeywords: $
//=============================================================================

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
	m_hud_nametags_type	= gEngfuncs.pfnRegisterVariable("hud_nametags_type", "1", FCVAR_ARCHIVE);
	m_hud_nametags_team_max_distance = gEngfuncs.pfnRegisterVariable("hud_nametags_team_max_distance", "1", FCVAR_ARCHIVE);

	return 1;
}

//-----------------------------------------------------------------------------
// Purpose: Loads new icons
//-----------------------------------------------------------------------------
int CHudNameTags::VidInit()
{
	//m_iFlags &= ~HUD_ACTIVE;
	//m_hsprPlayer = SPR_Load("sprites/ascii_table_test.spr");
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
	
	int lx;
	char string[512];
	float * color;

	vec3_t origin_above_target_head, world, screen;
	vec3_t origin_pl;
	vec3_t vecSrc;
	vec3_t vecTargetPlayer;
	vec3_t view_ofs;
	float			x,y,z;
	int 			r,g,b;

	// make sure we have player info
	gViewPort->GetAllPlayersInfo();
	// get server's gamemode 
	auto gamemode = gHUD.m_Settings.GetGamemode();
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
		
		// Target player not here, or not in our PVS
		if(!ent || ent->curstate.messagenum < localPlayer->curstate.messagenum)
			continue; 

		if (!ent->player)
			continue;

		// Don't draw nickname for ourselves 
		if(ent == localPlayer)
			continue;

		// Don't draw for player without a name (TODO: ?)
		if (g_PlayerInfoList[i + 1].name == nullptr)
			continue;

		// Don't show a nametag for dead or spectating players (ie: invisible entities).
		if(ent->curstate.effects & EF_NODRAW)
			continue;

		VectorCopy(ent->origin, origin_above_target_head);
		origin_above_target_head[2] += 45.0f; // Kinda above the head 
		// ^ TODO: change based on distance?

		// calculate screen position for name and infromation in hud::draw()
		if ( gEngfuncs.pTriAPI->WorldToScreen(origin_above_target_head, screen) )
			continue;	// object is behind viewer

		x = XPROJECT(screen[0]);
		y = YPROJECT(screen[1]);

		color = GetClientColor( i+1 ); // team color

		// draw the players name and health underneath
		char colorless_name[256];
		color_tags::strip_color_tags(colorless_name, g_PlayerInfoList[i + 1].name, ARRAYSIZE(colorless_name));

		/*if(!strcmp(g_PlayerExtraInfo[localPlayer->index].teamname, g_PlayerExtraInfo[i + 1].teamname) && gHUD.m_Teamplay)
			sprintf(string, "%s", colorless_name, health, armor, teammate);
		else */ // For playerid msg
			sprintf(string, "%s", colorless_name);

		lx = strlen(string)*4; // 3 is avg. character length :)

		VectorCopy(ent->origin, vecTargetPlayer);
		
		// Since the nametag is above the player's head, let's somewhat try to see if we can see his head
		if (ent->curstate.usehull == 1) 
			vecTargetPlayer[2] += VEC_DUCK_VIEW;
		else 
			vecTargetPlayer[2] += DEFAULT_VIEWHEIGHT;
		
		pmtrace_t * trace = gEngfuncs.PM_TraceLine( vecSrc, vecTargetPlayer, PM_TRACELINE_PHYSENTSONLY, 2, -1 );
		
		if ( trace->fraction == 1.0 )
		{
			// If the player is 600 units(?) away from the local player, show the nametag
			// Or if we are playing a demo, show the nametag however far away he is
			// Or if client wants teammates to always appear, regardless of their distance (obv only when in our PVS and traceable)
			// Or if the gamemode is Kreedz
			// Or if the localplayer is spectating
			// TODO: V_GetInEyePos( g_iUser2, origin, angles );

			if (Distance(trace->endpos, localPlayer->origin) < m_hud_nametags_team_max_distance->value && IsTeamMate(localPlayer, i) ||
				gEngfuncs.pDemoAPI->IsPlayingback() ||
				!strcmp(gamemode, "Kreedz") ||
				g_iUser1)
			{
				if(m_hud_nametags_type->value == 1) // Looks like chat HUD font
				{
					gHUD.DrawConsoleStringWithColorTags(
						x - lx,
						y,
						string,
						true,
						color[0],
						color[1],
						color[2]
					);
				}
				else // Looks like HUD font
				{
					gHUD.DrawHudStringCentered(x, y, string, r, g, b);
				}
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

void CHudNameTags::Reset()
{

}

void CHudNameTags::InitHUDData()
{
	Reset();
}

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
