#pragma once

class CHudWatermark : public CHudBase
{
	float draw_until;
	bool refresh_draw_until;

public:
	virtual int Init() override;
	virtual int VidInit() override;
	virtual int Draw(float time) override;
};
