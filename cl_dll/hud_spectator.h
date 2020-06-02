//========= Copyright � 1996-2001, Valve LLC, All rights reserved. ============
//
// Purpose: 
//
// $NoKeywords: $
//=============================================================================

#ifndef SPECTATOR_H
#define SPECTATOR_H
#pragma once

#include <string>
#include <vector>

#include "cl_entity.h"
#include "interpolation.h"


#define INSET_OFF				0
#define	INSET_CHASE_FREE		1
#define	INSET_IN_EYE			2
#define	INSET_MAP_FREE			3
#define	INSET_MAP_CHASE			4

#define MAX_SPEC_HUD_MESSAGES	8

extern void VectorAngles( const float *forward, float *angles );
extern "C" void NormalizeAngles( float *angles );

//-----------------------------------------------------------------------------
// Purpose: Handles the drawing of the spectator stuff (camera & top-down map and all the things on it )
//-----------------------------------------------------------------------------

typedef struct overviewEntity_s {

	HSPRITE					hSprite;
	struct cl_entity_s *	entity;
	double					killTime;
} overviewEntity_t;

typedef struct cameraWayPoint_s 
{
	float	time;
	vec3_t	position;
	vec3_t	angle;
	float	fov;
	int		flags;
} cameraWayPoint_t;

#define	 MAX_OVERVIEW_ENTITIES		128
#define	 MAX_CAM_WAYPOINTS			32

class CHudSpectator : public CHudBase
{
	struct OverviewLayer
	{
		std::string		imagePath;
		float			z;
		model_s*		mapSprite;

		OverviewLayer()
			: imagePath("")
			, z(0.0f)
			, mapSprite(nullptr)
		{
		}
	};

	struct Overview
	{
		std::string					map;		// cl.levelname or empty
		Vector						origin;		// center of map
		float						zoom;		// zoom of map images
		std::vector<OverviewLayer>	layers;
		qboolean					rotated;	// are map images rotated (90 degrees) ?
		int							insetWindowX;
		int							insetWindowY;
		int							insetWindowHeight;
		int							insetWindowWidth;

		Overview(const std::string& map)
			: map(map)
			, origin(Vector(0, 0, 0))
			, zoom(1.0f)
			, rotated(false)
			, insetWindowX(4)
			, insetWindowY(4)
			, insetWindowHeight(180)
			, insetWindowWidth(240)
		{
		}

		Overview() : Overview("")
		{
		}

	};

public:
	void Reset();
	int  ToggleInset(bool allowOff);
	void CheckSettings();
	void InitHUDData( void );
	bool AddOverviewEntityToList( HSPRITE sprite, cl_entity_t * ent, double killTime);
	void DeathMessage(int victim);
	bool AddOverviewEntity( int type, struct cl_entity_s *ent, const char *modelname );
	void CheckOverviewEntities();
	void DrawOverview();
	void DrawOverviewEntities();
	void GetMapPosition( float * returnvec );
	void DrawOverviewLayer();
	void LoadMapSprites();
	bool ParseOverviewFile();
	bool IsActivePlayer(cl_entity_t * ent);
	void SetModes(int iMainMode, int iInsetMode);
	void HandleButtonsDown(int ButtonPressed);
	void HandleButtonsUp(int ButtonPressed);
	void FindNextPlayer( bool bReverse );
	void FindPlayer(const char *name);
	void DirectorMessage( int iSize, void *pbuf );
	void SetSpectatorStartPosition();
	int Init();
	int VidInit();

	int Draw(float flTime);

	void	AddWaypoint( float time, vec3_t pos, vec3_t angle, float fov, int flags );
	void	SetCameraView( vec3_t pos, vec3_t angle, float fov);
	float	GetFOV();
	bool	GetDirectorCamera(vec3_t &position, vec3_t &angle);
	void	SetWayInterpolation(cameraWayPoint_t * prev, cameraWayPoint_t * start, cameraWayPoint_t * end, cameraWayPoint_t * next);
	void	GetCameraView(Vector& pos, Vector& angle);

	OverviewLayer GetCurrentLayer(Vector playerPos);
	OverviewLayer GetHighestLayer();


	int m_iDrawCycle;
	client_textmessage_t m_HUDMessages[MAX_SPEC_HUD_MESSAGES];
	char				m_HUDMessageText[MAX_SPEC_HUD_MESSAGES][128];
	int					m_lastHudMessage;
	Overview			m_OverviewData;
	overviewEntity_t	m_OverviewEntities[MAX_OVERVIEW_ENTITIES];
	int					m_iObserverFlags;
	int					m_iSpectatorNumber;
	
	float				m_mapZoom;		// zoom the user currently uses
	vec3_t				m_mapOrigin;	// origin where user rotates around
	cvar_t *			m_drawnames;
	cvar_t *			m_drawcone;
	cvar_t *			m_drawstatus;
	cvar_t *			m_autoDirector;
	cvar_t *			m_pip;
	qboolean			m_chatEnabled;
	
	qboolean			m_IsInterpolating;
	int					m_ChaseEntity;	// if != 0, follow this entity with viewangles
	int					m_WayPoint;		// current waypoint 1
	int					m_NumWayPoints;	// current number of waypoints
	vec3_t				m_cameraOrigin;	// a help camera
	vec3_t				m_cameraAngles;	// and it's angles
	CInterpolation		m_WayInterpolation;

private:
	vec3_t		m_vPlayerPos[MAX_PLAYERS];
	HSPRITE		m_hsprPlayerBlue;
	HSPRITE		m_hsprPlayerRed;
	HSPRITE		m_hsprPlayer;
	HSPRITE		m_hsprCamera;
	HSPRITE		m_hsprPlayerDead;
	HSPRITE		m_hsprViewcone;
	HSPRITE		m_hsprUnknownMap;
	HSPRITE		m_hsprBeam;
	HSPRITE		m_hCrosshair;

	wrect_t		m_crosshairRect;

	float		m_flNextObserverInput;
	float		m_FOV;
	float		m_zoomDelta;
	float		m_moveDelta;
	int			m_lastPrimaryObject;
	int			m_lastSecondaryObject;

	cameraWayPoint_t	m_CamPath[MAX_CAM_WAYPOINTS];
};

#endif // SPECTATOR_H
