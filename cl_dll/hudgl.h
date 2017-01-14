#pragma once

class HudGL {
public:
	HudGL();
	~HudGL();

	void color(float r, float g, float b, float a) const;
	void line_width(float width) const;
	void line(const Vector2D& start, const Vector2D& end) const;
};
