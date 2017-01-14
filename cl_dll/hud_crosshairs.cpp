#include "hud.h"
#include "cl_util.h"
#include "parsemsg.h"
#include "hudgl.h"

int CHudCrosshairs::Init()
{
	m_iFlags = HUD_ACTIVE;
	hud_crosshairs_test = CVAR_CREATE("hud_crosshairs_test", "1", 0);

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

	gl.color(1.0f, 0.0f, 0.0f, 1.0f);
	// gl.line_width(hud_crosshairs_test->value);
	gl.line(Vector2D(20.5, 20.5), Vector2D(20.5, 21.5));

	gl.circle(Vector2D(50.5, 50.5), HudGL::compute_circle(hud_crosshairs_test->value));

	return 0;
}
