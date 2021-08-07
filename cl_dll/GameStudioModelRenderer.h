//========= Copyright © 1996-2002, Valve LLC, All rights reserved. ============
//
// Purpose: 
//
// $NoKeywords: $
//=============================================================================

#if !defined( GAMESTUDIOMODELRENDERER_H )
#define GAMESTUDIOMODELRENDERER_H
#if defined( _WIN32 )
#pragma once
#endif
#include "com_model.h"
#include "studio.h"
#include "r_studioint.h"
#include "StudioModelRenderer.h"

/*
====================
CGameStudioModelRenderer

====================
*/
class CGameStudioModelRenderer : public CStudioModelRenderer
{
public:
	CGameStudioModelRenderer( void );
};

#endif // GAMESTUDIOMODELRENDERER_H