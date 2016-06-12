#pragma once

class CHudWatermark : public CHudBase
{
	float draw_until;
	bool refresh_draw_until;

public:
	virtual int Init();
	virtual int VidInit();
	virtual int Draw(float time);
};
