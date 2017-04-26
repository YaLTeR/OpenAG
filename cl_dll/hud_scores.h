#pragma once
#include <cstdint>

class CHudScores : public CHudBase
{
	cvar_t* cl_scores;
	cvar_t* cl_scores_pos;
	cvar_t* cl_scores_alpha;

	struct ScoreRow {
		int score;
		char* name;
		uint8_t color; // Index into the iTeamColors array.
	};

public:
	virtual int Init();
	virtual int VidInit();
	virtual int Draw(float time);
};
