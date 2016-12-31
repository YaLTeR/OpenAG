#ifndef DRAWINGUTILITY_HPP
#define DRAWINGUTILITY_HPP

#include <GL/gl.h> // windows:link with old opengl lib. quest: dynamic link funcs instead here
#define _USE_MATH_DEFINES 
#include <math.h>

namespace DrawingUtility
{
  void SetDrawingMatrix(void);

  void RestoreDrawingMatrix(void);

  void SetDrawingModes(void);

  void RestoreDrawingModes(void);

  /* range: 0.0f->1.0f */
  void SetColor4f(float r,float g,float b, float a);

  void DrawQLine(float startX,float startY, float endX, float endY, float width); // use this not drawline
  void DrawQCircle(float centerX,float centerY, float radius,float width);
  void DrawLine(float startX,float startY, float endX, float endY, float width);   // deprecated

  void DrawCircle(float centerX,float centerY, float radius,float width);

  void DrawRectangle(float x,float y, float length, float height);

}

#endif
