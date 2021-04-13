//========= Copyright � 1996-2001, Valve LLC, All rights reserved. ============
//
// Purpose: 
//
// $NoKeywords: $
//=============================================================================

#ifndef NAMETAGS_H
#define NAMETAGS_H
#pragma once

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
	bool AddOverviewEntityToList( HSPRITE sprite, cl_entity_t * ent, double killTime);
	void DeathMessage(int victim);
	bool AddOverviewEntity( int type, struct cl_entity_s *ent, const char *modelname );
	void CheckOverviewEntities();
	bool IsActivePlayer(cl_entity_t * ent);
	void DirectorMessage( int iSize, void *pbuf );
	int Init();
	int VidInit();

	int Draw(float flTime);

	void	AddWaypoint( float time, vec3_t pos, vec3_t angle, float fov, int flags );
	void	SetCameraView( vec3_t pos, vec3_t angle, float fov);
	float	GetFOV();
	bool	GetDirectorCamera(vec3_t &position, vec3_t &angle);
	
	int m_iDrawCycle;
	client_textmessage_t m_HUDMessages[MAX_SPEC_HUD_MESSAGES];
	char				m_HUDMessageText[MAX_SPEC_HUD_MESSAGES][128];
	
	cvar_t * 			m_hud_nametags;
	cvar_t * 			m_hud_nametags_type;	
    cvar_t *            m_hud_nametags_team_always;

private:
	vec3_t		m_vPlayerPos[MAX_PLAYERS];

	float		m_flNextObserverInput;
	float		m_FOV;
	float		m_zoomDelta;
	float		m_moveDelta;
	int			m_lastPrimaryObject;
	int			m_lastSecondaryObject;

	cameraWayPoint_t	m_CamPath[MAX_CAM_WAYPOINTS];
};

#endif // SPECTATOR_H
