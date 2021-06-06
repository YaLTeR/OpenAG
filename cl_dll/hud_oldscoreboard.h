#ifndef OLDSCOREBOARD_H
#define OLDSCOREBOARD_H
#pragma once

#include "cl_entity.h"
#include "interpolation.h"

class CHudOldScoreboard: public CHudBase
{
public:
	int Init( void );
	int VidInit( void );
	int Draw( float flTime );
	void Reset( void );

	bool IsVisible( );
	void ShowScoreboard( bool bShow );
private:
	typedef struct
	{
		HSPRITE spr;
		wrect_t rc;
	} icon_flagstatus_t;

	icon_flagstatus_t m_IconFlagScore;

	cvar_t *m_pCvarOldScoreboard;
	cvar_t *m_pCvarOldScoreboardWidth;
	cvar_t *m_pCvarOldScoreboardColorsTags;

	float m_WidthScale;
};

#endif //OLD_SCOREBOARD_H