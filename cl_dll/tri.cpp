//========= Copyright © 1996-2002, Valve LLC, All rights reserved. ============
//
// Purpose: 
//
// $NoKeywords: $
//=============================================================================

// Triangle rendering, if any

#include <algorithm>

#ifdef _WIN32
#include <winsani_in.h>
#include <Windows.h>
#include <winsani_out.h>
#endif

#ifdef __APPLE__
#include <OpenGL/gl.h>
#else
#include <GL/gl.h>
#endif

#include "hud.h"
#include "cl_util.h"

// Triangle rendering apis are in gEngfuncs.pTriAPI

#include "const.h"
#include "entity_state.h"
#include "cl_entity.h"
#include "triangleapi.h"
#include "Exports.h"

#include "particleman.h"
#include "tri.h"
extern IParticleMan *g_pParticleMan;

#include "com_model.h"
#include "r_studioint.h"

#undef min
#undef max

/*
=================
HUD_DrawNormalTriangles

Non-transparent triangles-- add them here
=================
*/
void CL_DLLEXPORT HUD_DrawNormalTriangles( void )
{
//	RecClDrawNormalTriangles();

	gHUD.m_Spectator.DrawOverview();
}

#if defined( _TFC )
void RunEventList( void );
#endif

void DivideRGBABy255(float &r, float &g, float &b, float &a)
{
	r /= 255.0f;
	g /= 255.0f;
	b /= 255.0f;
	a /= 255.0f;
}

void DrawAACuboid(triangleapi_s *pTriAPI, Vector corner1, Vector corner2)
{
	pTriAPI->Begin(TRI_QUADS);

	pTriAPI->Vertex3f(corner1.x, corner1.y, corner1.z);
	pTriAPI->Vertex3f(corner1.x, corner2.y, corner1.z);
	pTriAPI->Vertex3f(corner2.x, corner2.y, corner1.z);
	pTriAPI->Vertex3f(corner2.x, corner1.y, corner1.z);

	pTriAPI->Vertex3f(corner1.x, corner1.y, corner1.z);
	pTriAPI->Vertex3f(corner1.x, corner1.y, corner2.z);
	pTriAPI->Vertex3f(corner1.x, corner2.y, corner2.z);
	pTriAPI->Vertex3f(corner1.x, corner2.y, corner1.z);

	pTriAPI->Vertex3f(corner1.x, corner1.y, corner1.z);
	pTriAPI->Vertex3f(corner2.x, corner1.y, corner1.z);
	pTriAPI->Vertex3f(corner2.x, corner1.y, corner2.z);
	pTriAPI->Vertex3f(corner1.x, corner1.y, corner2.z);

	pTriAPI->Vertex3f(corner2.x, corner2.y, corner2.z);
	pTriAPI->Vertex3f(corner1.x, corner2.y, corner2.z);
	pTriAPI->Vertex3f(corner1.x, corner1.y, corner2.z);
	pTriAPI->Vertex3f(corner2.x, corner1.y, corner2.z);

	pTriAPI->Vertex3f(corner2.x, corner2.y, corner2.z);
	pTriAPI->Vertex3f(corner2.x, corner1.y, corner2.z);
	pTriAPI->Vertex3f(corner2.x, corner1.y, corner1.z);
	pTriAPI->Vertex3f(corner2.x, corner2.y, corner1.z);

	pTriAPI->Vertex3f(corner2.x, corner2.y, corner2.z);
	pTriAPI->Vertex3f(corner2.x, corner2.y, corner1.z);
	pTriAPI->Vertex3f(corner1.x, corner2.y, corner1.z);
	pTriAPI->Vertex3f(corner1.x, corner2.y, corner2.z);

	pTriAPI->End();
}

void DrawServerTriggers()
{
	if ((gHUD.m_pShowServerTriggers->value > 0) && (gHUD.m_pShowServerTriggers->value != 2.0f))
	{
		for (int e = 0; e < MAX_EDICTS; ++e)
		{
			cl_entity_t* ent = gEngfuncs.GetEntityByIndex(e);
			if (ent)
			{
				if (ent->model)
				{
					if ((ent->curstate.rendermode == kRenderTransColor) && (ent->curstate.renderfx == kRenderFxTrigger))
					{
						color24 colors = ent->curstate.rendercolor;
						if (!gHUD.IsTriggerForSinglePlayer(colors))
						{
							gEngfuncs.pTriAPI->RenderMode(kRenderTransAdd);
							gEngfuncs.pTriAPI->CullFace(TRI_NONE);

							float r = colors.r, g = colors.g, b = colors.b, a = std::min(255.0f, std::max(0.0f, gHUD.m_pShowServerTriggersAlpha->value));
							DivideRGBABy255(r, g, b, a);
							gEngfuncs.pTriAPI->Color4f(r, g, b, a);

							Vector mins = ent->curstate.mins;
							Vector maxs = ent->curstate.maxs;
							Vector origin = ent->curstate.origin;
							Vector absmin = origin + mins;
							Vector absmax = origin + maxs;

							DrawAACuboid(gEngfuncs.pTriAPI, absmin, absmax);
						}
					}
				}
			}
		}
	}
}

extern engine_studio_api_t IEngineStudio;

/*
=================
HUD_DrawTransparentTriangles

Render any triangles with transparent rendermode needs here
=================
*/
void CL_DLLEXPORT HUD_DrawTransparentTriangles( void )
{
//	RecClDrawTransparentTriangles();

#if defined( _TFC )
	RunEventList();
#endif

	if ( g_pParticleMan )
		 g_pParticleMan->Update();

	if (!gHUD.white_sprite)
		return;

	if (gEngfuncs.pTriAPI->SpriteTexture(const_cast<model_s*>(gEngfuncs.GetSpritePointer(gHUD.white_sprite)), 0))
	{
		DrawServerTriggers();

		gEngfuncs.pTriAPI->RenderMode(kRenderNormal);
	}
}
