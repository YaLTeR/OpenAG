#include "DrawingUtility.hpp"
#include <windows.h>
#include <GL/gl.h> // windows:link with old opengl lib. quest: dynamic link funcs instead here

namespace DrawingUtility
{
  void SetDrawingMatrix(void)
  {
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();
  }

  void RestoreDrawingMatrix(void){glPopMatrix();}

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
  void SetColor4f(float r,float g,float b, float a) {glColor4f(r,g,b,a);}

  void DrawQLine(float startX,float startY, float endX, float endY, float width) // scale of width is wrong
  {
    float vdelta[2] = {endX - startX, endY - startY};
    float len = sqrt(vdelta[0]*vdelta[0] + vdelta[1]*vdelta[1]);
    if (len <= .0f) // wont happen unless start==end
      return; // draw a point instead maby? no

    vdelta[0] /= len;
    vdelta[1] /= len;

    // apply the following matrix to vdelta for +pi/2 rotate
    // 0 -1
    // 1  0
    
    float vortho[2] = {vdelta[1], -vdelta[0]}; // is a normalvector too
    float delta = width/2.0f;
    float nodes[4][2] = {
      {startX + delta * vortho[0], startY + delta * vortho[1]},
      {endX + delta * vortho[0], endY + delta * vortho[1]},
      {endX - delta * vortho[0], endY - delta * vortho[1]},
      {startX - delta * vortho[0], startY - delta * vortho[1]}
    };

    // now quads with these nodes
    glBegin(GL_QUADS);
    glVertex2f(nodes[0][0], nodes[0][1]);
    glVertex2f(nodes[1][0], nodes[1][1]);
    glVertex2f(nodes[2][0], nodes[2][1]);
    glVertex2f(nodes[3][0], nodes[3][1]);
    glEnd();
    
  }

  // deprecated. use DrawQLine instead
  void DrawLine(float startX,float startY, float endX, float endY, float width)
  {
    glLineWidth(width);
    glBegin(GL_LINES);
    glVertex2f(startX,startY);
    glVertex2f(endX,endY);
    glEnd();
  }

  void DrawQCircle(float centerX,float centerY, float radius,float width)
  {
    float ux,vy=centerY;
    float uy,vx=centerX + radius;

    for(float angle=0.0f; angle <= (2.0f*M_PI);angle+=0.01f) // quest: give better step according to radius
      {
	ux = centerX + radius * cos(angle);
	uy = centerY + radius * sin(angle);
	DrawQLine(vx,vy, ux, uy, width);
	vx = ux;
	vy = uy;
      }
  }


  void DrawCircle(float centerX,float centerY, float radius,float width) // deprecated you can use DrawQCircle now
  {
    glLineWidth(width);

    float ux,vy=centerY + radius;
    float uy,vx=centerX;
    glBegin(GL_LINE_STRIP);

    /* todo: step should have radius as a factor */
    for(float angle=0.0f; angle <= (2.0f*M_PI);angle+=0.01f)
      {
	ux=centerX + radius*sin(angle);
	uy=centerY + radius*cos(angle);
	glVertex2f(vx,vy);
	vy=uy;
	vx=ux;
      }
    glEnd();
  }

  void DrawRectangle(float x,float y, float length, float height) // careful it is centered on (x,y)
  {
    float w=length/2.0f;
    float h=height/2.0f;
    glBegin(GL_QUADS);
    glVertex2f(x - w, y - h);
    glVertex2f(x - w, y + h);
    glVertex2f(x + w, y + h);
    glVertex2f(x + w, y - h);
    glEnd();
  }

}
