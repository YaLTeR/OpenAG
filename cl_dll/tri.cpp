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

static std::vector<cl_entity_t*> trigger_entities;
char map_name[64];
static char map_name_old[64];

void UpdateServerTriggers()
{
	if (map_name[0])
	{
		if ((strcmp(map_name, map_name_old)) || (gHUD.m_pShowServerTriggersForceUpdate->value > 0))
		{
			if ((gHUD.m_pShowServerTriggers->value == 3.0f) && (gHUD.m_pShowServerTriggersForceUpdate->value < 1.0f)) // Debug
				gEngfuncs.Con_DPrintf("UpdateServerTriggersOnMapChange: map changed!\n");

			trigger_entities.clear();

			for (int e = 0; e < MAX_EDICTS; ++e)
			{
				cl_entity_t* ent = gEngfuncs.GetEntityByIndex(e);
				if (ent)
				{
					if (ent->model)
					{
						if ((ent->curstate.rendermode == kRenderTransColor) && (ent->curstate.renderfx == kRenderFxTrigger))
						{
							trigger_entities.emplace_back(ent);
						}
					}
				}
			}

			gEngfuncs.Con_DPrintf("UpdateServerTriggersOnMapChange: triggers updated!\n");
		}
	}
	else
	{
		if (gHUD.m_pShowServerTriggers->value == 3.0f) // Debug
			gEngfuncs.Con_DPrintf("UpdateServerTriggersOnMapChange: map not found, then we clear vectors!\n");

		trigger_entities.clear();
	}
}

void DrawServerTriggers()
{
	if (!trigger_entities.empty())
	{
		for (size_t i = 0; i < trigger_entities.size(); i++)
		{
			color24 rendercolor = trigger_entities[i]->curstate.rendercolor;
			if (!gHUD.IsTriggerForSinglePlayer(rendercolor))
			{
				gEngfuncs.pTriAPI->RenderMode(kRenderTransAdd);
				gEngfuncs.pTriAPI->CullFace(TRI_NONE);

				float r = rendercolor.r, g = rendercolor.g, b = rendercolor.b, a = std::min(255.0f, std::max(0.0f, gHUD.m_pShowServerTriggersAlpha->value));
				DivideRGBABy255(r, g, b, a);
				gEngfuncs.pTriAPI->Color4f(r, g, b, a);

				Vector mins = trigger_entities[i]->curstate.mins;
				Vector maxs = trigger_entities[i]->curstate.maxs;
				Vector origin = trigger_entities[i]->curstate.origin;
				Vector absmin = origin + mins;
				Vector absmax = origin + maxs;

				DrawAACuboid(gEngfuncs.pTriAPI, absmin, absmax);
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

	// Draw server-side triggers with TriAPI instead
	if (IEngineStudio.IsHardware())
	{
		glDisable(GL_TEXTURE_2D);

		if ((gHUD.m_pShowServerTriggers->value > 0) && (gHUD.m_pShowServerTriggers->value != 2.0f))
		{
			gHUD.SetMapName(map_name, ARRAYSIZE(map_name));
			UpdateServerTriggers();
			DrawServerTriggers();
		}

		glEnable(GL_TEXTURE_2D);
		gEngfuncs.pTriAPI->RenderMode(kRenderNormal);

		if ((gHUD.m_pShowServerTriggers->value > 0) && (gHUD.m_pShowServerTriggers->value != 2.0f))
		{
			// Saved old map name in static variable to differ it with new map name
			if (map_name[0])
			{
				memset(map_name_old, 0, sizeof(map_name_old));
				strncpy(map_name_old, map_name, sizeof(map_name_old) - 1);
			}
		}
	}
}
