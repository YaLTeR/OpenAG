#include "hud.h"
#include "cl_util.h"
#include "parsemsg.h"
#include "hudgl.h"

int CHudCrosshairs::Init()
{
	m_iFlags = HUD_ACTIVE;

	gHUD.AddHudElem(this);
	return 0;
}

int CHudCrosshairs::VidInit()
{
	return 1;
}

int CHudCrosshairs::Draw(float time)
{
	HudGL gl;

	gl.color(1.0f, 0.0f, 0.0f, 0.8f);
	gl.line(Vector2D(20, 20), Vector2D(20, 200));

	return 0;
}
