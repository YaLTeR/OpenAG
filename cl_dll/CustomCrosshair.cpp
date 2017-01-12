#include "hud.h"
#include "cl_util.h"
#include "CustomCrosshair.hpp"

namespace CustomCrosshair
{
  /* megasausageking: custom crosshair commands */
  cvar_t * cl_cross_alpha;
  cvar_t * cl_customcrosshair;
  cvar_t * cl_cross_dot;
  cvar_t * cl_cross_dot_size;
  cvar_t * cl_cross_red;
  cvar_t * cl_cross_green;
  cvar_t * cl_cross_blue;
  cvar_t * cl_cross_circle;
  cvar_t * cl_cross_drawoutline;
  cvar_t * cl_cross_circle_radius;
  cvar_t * cl_cross_circle_width;
  cvar_t * cl_cross_circle_alpha;
  cvar_t * cl_cross_size;
  cvar_t * cl_cross_gap;
  cvar_t * cl_cross_x;
  cvar_t * cl_cross_y;
  cvar_t * cl_cross_width;
  /* megasausageking end */

  void RegisterCvars(void)
  {
    /* megasausageking: custom crosshair commands */
    cl_cross_alpha=CVAR_CREATE("cl_cross_alpha","1", FCVAR_ARCHIVE);
    cl_customcrosshair=CVAR_CREATE("cl_customcrosshair","0",FCVAR_ARCHIVE);
    cl_cross_dot=CVAR_CREATE("cl_cross_dot","0", FCVAR_ARCHIVE);
    cl_cross_dot_size=CVAR_CREATE("cl_cross_dot_size","1",FCVAR_ARCHIVE);
    cl_cross_red=CVAR_CREATE("cl_cross_red","0",FCVAR_ARCHIVE);
    cl_cross_green=CVAR_CREATE("cl_cross_green","1",FCVAR_ARCHIVE);
    cl_cross_blue=CVAR_CREATE("cl_cross_blue","0",FCVAR_ARCHIVE);
    cl_cross_circle=CVAR_CREATE("cl_cross_circle","0",FCVAR_ARCHIVE);
    cl_cross_drawoutline=CVAR_CREATE("cl_cross_drawoutline","0",FCVAR_ARCHIVE);
    cl_cross_circle_radius=CVAR_CREATE("cl_cross_circle_radius","20",FCVAR_ARCHIVE);
    cl_cross_circle_width=CVAR_CREATE("cl_cross_circle_width","2",FCVAR_ARCHIVE);
    cl_cross_circle_alpha=CVAR_CREATE("cl_cross_circle_alpha","0.5",FCVAR_ARCHIVE);
    cl_cross_size=CVAR_CREATE("cl_cross_size","10",FCVAR_ARCHIVE);
    cl_cross_gap=CVAR_CREATE("cl_cross_gap","0",FCVAR_ARCHIVE);
    cl_cross_x=CVAR_CREATE("cl_cross_x","0",FCVAR_ARCHIVE);
    cl_cross_y=CVAR_CREATE("cl_cross_y","0",FCVAR_ARCHIVE);
    cl_cross_width=CVAR_CREATE("cl_cross_width","2",FCVAR_ARCHIVE);
    /* megasausageking end */
  }

  /* crosshair center dot */
  void DrawCrosshairDot(float centerX, float centerY,float size,float offsetX, float offsetY)
  {
    DrawingUtility::DrawRectangle(centerX + offsetX, centerY + offsetY,size,size);
  }

  void DrawCrosshairCircle(float centerX, float centerY, float width, float radius, float offsetX, float offsetY)
  {
    DrawingUtility::DrawCircle(centerX + offsetX, centerY + offsetY, radius, width);
  }

  void Draw(void)
  {
    /* megasausageking: custom crosshair drawing happens here */
    bool bdrawcircle=(cl_cross_circle->value != 0);
    float offsetX=cl_cross_x->value;
    float offsetY=cl_cross_y->value;
    float radius=cl_cross_circle_radius->value;
    float circlewidth=cl_cross_circle_width->value;
    float width=cl_cross_width->value;
    float size=cl_cross_size->value;
    float red=cl_cross_red->value;
    float green=cl_cross_green->value;
    float blue=cl_cross_blue->value;
    float crossalpha=cl_cross_alpha->value;
    float circlealpha=cl_cross_circle_alpha->value;
    float gap=cl_cross_gap->value;
    float dotsize=cl_cross_dot_size->value;
    bool bdrawdot=(cl_cross_dot->value != 0);

    if(CustomCrosshair::cl_customcrosshair->value != 0) {

      DrawingUtility::SetDrawingMatrix();
      DrawingUtility::SetDrawingModes();
      DrawingUtility::SetColor4f(red,green,blue,crossalpha);
      if(size)
	DrawCrosshairQLines(ScreenWidth/2.0,ScreenHeight/2.0,size,gap,width,offsetX,offsetY);
      if(bdrawdot)
	DrawCrosshairDot(ScreenWidth/2.0 + offsetX ,ScreenHeight/2.0 + offsetY,dotsize,offsetX,offsetY);
      if(bdrawcircle)
	{
	  DrawingUtility::SetColor4f(red,green,blue,circlealpha);
	  DrawingUtility::DrawQCircle(ScreenWidth/2.0 + offsetX, ScreenHeight/2.0 + offsetY, radius, circlewidth);
	  //DrawCrosshairCircle(ScreenWidth/2.0,ScreenHeight/2.0,circlewidth,radius,offsetX,offsetY);
	}
      DrawingUtility::RestoreDrawingMatrix();
      DrawingUtility::RestoreDrawingModes();
    }
  }

  
  void DrawCrosshairQLines(float centerX, float centerY, float size,float gap, float width, float offsetX, float offsetY)
  {
    DrawingUtility::DrawQLine(-(size + gap) + (offsetX + centerX), offsetY + centerY,-gap + (centerX + offsetX), centerY + offsetY, width);
    DrawingUtility::DrawQLine((size + gap) + offsetX + centerX ,0.0f + offsetY + centerY, centerX + offsetX + gap,centerY + offsetY, width);
    DrawingUtility::DrawQLine(offsetX + centerX ,(size + gap)  + offsetY + centerY,offsetX+centerX, centerY + offsetY + gap,width);
    DrawingUtility::DrawQLine(offsetX + centerX,-(size + gap) + offsetY + centerY,offsetX + centerX, -(gap) + centerY + offsetY,width);
  }
  

  void DrawCrosshairLines(float centerX, float centerY, float size,float gap, float width, float offsetX, float offsetY)
  {
    DrawingUtility::DrawLine(-(size + gap) + offsetX + centerX, offsetY + centerY,centerX - (gap + offsetX), centerY + offsetY, width);
    DrawingUtility::DrawLine((size + gap) + offsetX + centerX ,0.0f + offsetY + centerY, centerX + offsetX + gap,centerY + offsetY, width);
    DrawingUtility::DrawLine(offsetX + centerX ,(size + gap)  + offsetY + centerY,offsetX+centerX, centerY + offsetY + gap,width);
    DrawingUtility::DrawLine(offsetX + centerX,-(size + gap) + offsetY + centerY,offsetX + centerX, -(gap) + centerY + offsetY,width);
  }

}
