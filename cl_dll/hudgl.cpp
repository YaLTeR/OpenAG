#include "hud.h"
#include "cl_util.h"
#include "parsemsg.h"
#include <GL/gl.h>

#include "hudgl.h"

HudGL::HudGL() {
	// Same steps as FillRGBA does.
	glDisable(GL_TEXTURE_2D);
	glEnable(GL_BLEND);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE);
}

HudGL::~HudGL() {
	// Same steps as FillRGBA does.
	glColor3f(1.0f, 1.0f, 1.0f);
	glDisable(GL_BLEND);
	glEnable(GL_TEXTURE_2D);
}

void HudGL::color(float r, float g, float b, float a) const {
	glColor4f(r, g, b, a);
}

void HudGL::line(const Vector2D& start, const Vector2D& end) const {
	glBegin(GL_LINES);
	glVertex2f(start.x, start.y);
	glVertex2f(end.x, end.y);
	glEnd();
}
