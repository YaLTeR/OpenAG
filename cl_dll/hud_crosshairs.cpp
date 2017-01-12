#include "hud.h"
#include "cl_util.h"
#include "parsemsg.h"
#include <GL/gl.h>

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
	glDisable(GL_TEXTURE_2D);
	glEnable(GL_BLEND);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE);

	glColor4f(1.0f, 0.0f, 0.0f, 0.8f);

	glBegin(GL_LINES);
	glVertex2f(20, 20);
	glVertex2f(20, 200);
	glEnd();

	glColor3f(1.0f, 1.0f, 1.0f);
	glDisable(GL_BLEND);
	glEnable(GL_TEXTURE_2D);

	return 0;
}
