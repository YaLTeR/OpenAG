#include<windows.h>
#include <GL/gl.h>
#include <math.h>
#include "drawingutility.h"

#ifndef M_PI
#define M_PI 3.14159265358979323846 // matches value in gcc v2 math.h
#endif

namespace drawingutility
{
	void SetDrawingMatrix(void)
	{
		glMatrixMode(GL_MODELVIEW);
		glPushMatrix();
		glLoadIdentity();
	}

	void RestoreDrawingMatrix(void) { glPopMatrix(); }

	void SetDrawingModes(void)
	{
		glDisable(GL_TEXTURE_2D);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	}

	void RestoreDrawingModes(void)
	{
		glDisable(GL_BLEND);
		glEnable(GL_TEXTURE_2D);
	}

	/* range: 0.0f->1.0f */
	void SetColor4f(float r, float g, float b, float a) { glColor4f(r, g, b, a); }

	/*todo:make line using gl quads instead.*/
	void DrawLine(float startX, float startY, float endX, float endY, float width)
	{
		glLineWidth(width);
		glBegin(GL_LINES);
		glVertex2f(startX, startY);
		glVertex2f(endX, endY);
		glEnd();

	}

	void DrawCircle(float centerX, float centerY, float radius, float width)
	{
		glLineWidth(width);

		float ux, vy = centerY + radius;
		float uy, vx = centerX;
		glBegin(GL_LINE_STRIP);

		/* todo: step should have radius as a factor */
		for (float angle = 0.0f; angle <= (2.0f*M_PI); angle += 0.01f)
		{
			ux = centerX + radius*sin(angle);
			uy = centerY + radius*cos(angle);
			glVertex2f(vx, vy);
			vy = uy;
			vx = ux;
		}
		glEnd();
	}

	void DrawRectangle(float x, float y, float length, float height)
	{
		glBegin(GL_QUADS);
		glVertex2f(x, y);
		glVertex2f(x + length, y);
		glVertex2f(x + length, y + height);
		glVertex2f(x, y + height);
		glEnd();
	}

}