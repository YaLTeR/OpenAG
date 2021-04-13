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
#include "hltv.h"

#include "pm_shared.h"
#include "pm_defs.h"
#include "pmtrace.h"
#include "parsemsg.h"
#include "entity_types.h"

// these are included for the math functions
#include "com_model.h"
#include "demo_api.h"
#include "event_api.h"
#include "studio_util.h"
#include "screenfade.h"


#ifdef _MSC_VER
#pragma warning(disable: 4244)
#endif

extern "C" float	Distance(const float * v1, const float * v2);
extern float * GetClientColor( int clientIndex );

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
int CHudNameTags::Init()
{
	gHUD.AddHudElem(this);

	m_iFlags |= HUD_ACTIVE;

    m_hud_nametags		= gEngfuncs.pfnRegisterVariable("hud_nametags","1",0);
	m_hud_nametags_type	= gEngfuncs.pfnRegisterVariable("hud_nametags_type","1",0);
	m_hud_nametags_team_always	= gEngfuncs.pfnRegisterVariable("m_hud_nametags_team_always","1",0);

	return 1;
}


//-----------------------------------------------------------------------------
// Purpose: Loads new icons
//-----------------------------------------------------------------------------
int CHudNameTags::VidInit()
{
	m_iFlags &= ~HUD_ACTIVE;
	
	return 1;
}

//-----------------------------------------------------------------------------
// Purpose: 
// Input  : flTime - 
//			intermission - 
//-----------------------------------------------------------------------------
int CHudNameTags::Draw(float flTime)
{
    // Only draw if client wants us to
    if(m_hud_nametags->value != 1) return 1;        

	int lx;

	char string[256];
	float * color;

	// make sure we have player info
	gViewPort->GetAllPlayersInfo();
	
	vec3_t			origin, angles, point, forward, right, left, up, world, screen, offset;
	float			x,y,z;
	int r,g,b;
	cl_entity_t *	ent;
	float rmatrix[3][4];	// transformation matrix
	//float			zScale = (90.0f - v_angles[0] ) / 90.0f;
	
	// get yellow/brown HUD color
	UnpackRGB(r, g, b, gHUD.m_iDefaultHUDColor);
	
	cl_entity_t *localPlayer = gEngfuncs.GetLocalPlayer();
    
	// loop through all the players and draw additional infos to their sprites on the map
	for (int i = 0; i < MAX_PLAYERS; i++)
	{
		//ent = m_OverviewEntities[i].entity;
		cl_entity_s *ent = gEngfuncs.GetEntityByIndex(i+1);
		
		if(!ent || ent->curstate.messagenum < localPlayer->curstate.messagenum)
			continue; 

		if (!ent->player)
			continue;

		// Don't draw nickname for ourselves 
		if(ent == localPlayer)
			continue;

		if (g_PlayerInfoList[i + 1].name == nullptr)
			continue;

		// Don't show an icon for dead or spectating players (ie: invisible entities).
		if(ent->curstate.effects & EF_NODRAW)
			continue;

		VectorCopy(ent->origin, origin);
		origin[2] += 45.0f; // Kinda above the head TODO: Fix somewhat?

		// calculate screen position for name and infromation in hud::draw()
		if ( gEngfuncs.pTriAPI->WorldToScreen(origin, screen) )
			continue;	// object is behind viewer

		/*char mrdka[512];
		_snprintf( mrdka, sizeof( mrdka ), "HUD_SPEC ORIGIN x = %f, ORIGIN y = %f \n", origin[0], origin[1]);
		gEngfuncs.pfnConsolePrint( mrdka );
		_snprintf( mrdka, sizeof( mrdka ), "HUD_SPEC BEFORE screen x = %f, screen y = %f \n", screen[0], screen[1]);
		gEngfuncs.pfnConsolePrint( mrdka );*/

		screen[0] = XPROJECT(screen[0]);
		screen[1] = YPROJECT(screen[1]);
		screen[2] = 0.0f;

		//_snprintf( mrdka, sizeof( mrdka ), "HUD_SPEC AFTER screen x = %f, screen y = %f \n", screen[0], screen[1]);
		//gEngfuncs.pfnConsolePrint( mrdka );

		x = screen[0];	
		y = screen[1]; //+ Length(offset);	

		color = GetClientColor( i+1 );

		// draw the players name and health underneath
		char colorless_name[256];
		color_tags::strip_color_tags(colorless_name, g_PlayerInfoList[i + 1].name, ARRAYSIZE(colorless_name));

		//sprintf(string, "%s", g_PlayerInfoList[i+1].name );
		sprintf(string, "%s", colorless_name);
		
		lx = strlen(string)*3; // 3 is avg. character length :)
		
        // TODO: Fix, this can't trace origin to origin, it's not accurate
        // TODO: e.g. when we are under the player and there are walls in the way
        // TODO: etc.
		pmtrace_t * trace = gEngfuncs.PM_TraceLine( localPlayer->origin, ent->origin, PM_TRACELINE_PHYSENTSONLY, 2, -1 );
		
        if ( trace->fraction != 1.0 )
		{
			// We didn't hit
			//gEngfuncs.pfnServerCmd(string);
		}
		else
		{	
			// If the player is 600 units away from the local player, show the nametag
			// Or if we are playing a demo, show the nametag however far away he is
            // Or if client wants teammates to always appear, regardless of their distance (obv only when in our PVS and traceable)
            // TODO: Really distance? & really 600?
            if(Distance(trace->endpos, localPlayer->origin) < 600.0f 
            || gEngfuncs.pDemoAPI->IsPlayingback()
            || (g_PlayerExtraInfo[i + 1].teamname == g_PlayerExtraInfo[localPlayer->index].teamname && m_hud_nametags_team_always->value == 1)
            ) 
			{
				char mrdka[512];
				sprintf(mrdka, "X=%.2f Y=%.2f Z=%.2f\n", trace->endpos[0], trace->endpos[1], trace->endpos[2]);
				gEngfuncs.pfnConsolePrint( mrdka );	
				sprintf(mrdka, "PL X=%.2f Y=%.2f Z=%.2f\n", localPlayer->origin[0], localPlayer->origin[1], localPlayer->origin[2]);
				gEngfuncs.pfnConsolePrint( mrdka );	
				//gEngfuncs.pfnServerCmd(string);

				if(m_hud_nametags_type->value == 1) // Looks like "amxx_csay" HUD font
				{
					gHUD.DrawConsoleStringWithColorTags(
						//m_vPlayerPos[i][0] - lx,
						//m_vPlayerPos[i][1],
						x - lx, 
						y, // - 50,
						string,
						true,
						color[0],
						color[1],
						color[2]
					);
				}
				else // Looks like chat HUD font
				{
					//gEngfuncs.pfnDrawString(x - lx, y, string, r, g, b );
					gHUD.DrawHudStringCentered(x, y, string, r, g, b);
				}
			}
			else
			{
				char mrdka[512];
				sprintf(mrdka, "HE TOO FAR AWAY");
				gEngfuncs.pfnConsolePrint( mrdka );	
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

	// reset HUD FOV
	gHUD.m_iFOV =  CVAR_GET_FLOAT("default_fov");
}
