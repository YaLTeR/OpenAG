#ifndef CUSTOMCROSSHAIR_HPP
#define CUSTOMCROSSHAIR_HPP
#include "DrawingUtility.hpp"

namespace CustomCrosshair
{
  void RegisterCvars(void);
  void Draw(void);

  void DrawCrosshairDot(float centerX, float centerY,float size,float offsetX, float offsetY);
  void DrawCrosshairCircle(float centerX, float centerY, float width, float radius, float offsetX, float offsetY);
  void DrawCrosshairLines(float centerX, float centerY, float size,float gap, float width, float offsetX, float offsetY);
  void DrawCrosshairQLines(float centerX, float centerY, float size,float gap, float width, float offsetX, float offsetY);
}

#endif /* CUSTOMCROSSHAIR_HPP */
