#pragma once
#include <cstdint>
#include <string>
#include <utility>
#include <vector>

class CHudScores : public CHudBase
{
	cvar_t* cl_scores;
	cvar_t* cl_scores_pos;

	struct ScoreRow {
		int score;
		std::string name;
		uint8_t color; // Index into the iTeamColors array.

		ScoreRow(int score, std::string name, uint8_t color)
			: score(score)
			, name(std::move(name))
			, color(color)
		{}
	};

	std::vector<ScoreRow> rows;

public:
	virtual int Init();
	virtual int VidInit();
	virtual int Draw(float time);

	void UpdateRows();
};
