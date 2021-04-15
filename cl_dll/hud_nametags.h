//========= Copyright � 1996-2001, Valve LLC, All rights reserved. ============
//
// Purpose: 
//
// $NoKeywords: $
//=============================================================================

#ifndef NAMETAGS_H
#define NAMETAGS_H
#pragma once

#define	DEFAULT_VIEWHEIGHT	28
#define VEC_DUCK_VIEW 12

#include <string>
#include <vector>

#include "cl_entity.h"
#include "interpolation.h"

extern void VectorAngles( const float *forward, float *angles );
extern "C" void NormalizeAngles( float *angles );

#define	 MAX_OVERVIEW_ENTITIES		128
#define	 MAX_CAM_WAYPOINTS			32

class CHudNameTags : public CHudBase
{

public:
	void Reset();
	void InitHUDData( void );
	bool IsActivePlayer(cl_entity_t * ent);

	int MsgFunc_PlayerId(const char* name, int size, void* buf);
	//void EV_GetGunPosition(cl_entity_s *args, float *pos, float *origin);
	int Init();
	int VidInit();

	int Draw(float flTime);

	int player_id;
	bool teammate;
	int health;
	int armor;
	
	cvar_t * 			m_hud_nametags;
	cvar_t * 			m_hud_nametags_type;
	cvar_t *            m_hud_nametags_team_max_distance;

private:
	vec3_t		m_vPlayerPos[MAX_PLAYERS];

    bool IsTeamMate(cl_entity_t *localPlayer, int playerId);
};

#endif // SPECTATOR_H
