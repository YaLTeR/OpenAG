#include "drawingutility.h"
#include "customcrosshair.h"

namespace customcrosshair
{
	/* crosshair center dot */
	void DrawCrosshairDot(float centerX, float centerY, float size, float offsetX, float offsetY)
	{
		drawingutility::DrawRectangle(centerX - (size)+offsetX, centerY - (size)+offsetY, 2 * size, 2 * size);
	}

	void DrawCrosshairCircle(float centerX, float centerY, float width, float radius, float offsetX, float offsetY)
	{
		drawingutility::DrawCircle(centerX + offsetX, centerY + offsetY, radius, width);
	}

	void DrawCrosshairLines(float centerX, float centerY, float size, float gap, float width, float offsetX, float offsetY)
	{
		drawingutility::DrawLine(-(size + gap) + offsetX + centerX, offsetY + centerY, centerX - (gap + offsetX), centerY + offsetY, width);
		drawingutility::DrawLine((size + gap) + offsetX + centerX, 0.0f + offsetY + centerY, centerX + offsetX + gap, centerY + offsetY, width);
		drawingutility::DrawLine(offsetX + centerX, (size + gap) + offsetY + centerY, offsetX + centerX, centerY + offsetY + gap, width);
		drawingutility::DrawLine(offsetX + centerX, -(size + gap) + offsetY + centerY, offsetX + centerX, -(gap)+centerY + offsetY, width);
	}

}