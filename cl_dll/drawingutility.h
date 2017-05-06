#pragma once
namespace drawingutility
{
	void SetDrawingMatrix(void);
	void RestoreDrawingMatrix(void);
	void SetDrawingModes(void);
	void RestoreDrawingModes(void);
	void SetColor4f(float r, float g, float b, float a);
	void DrawLine(float startX, float startY, float endX, float endY, float width);
	void DrawCircle(float centerX, float centerY, float radius, float width);
	void DrawRectangle(float x, float y, float length, float height);
};
