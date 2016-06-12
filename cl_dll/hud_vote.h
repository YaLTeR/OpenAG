#pragma once
#include <cstdint>

class CHudVote : public CHudBase
{
	enum class VoteStatus : uint8_t
	{
		HIDDEN = 0,

		IN_PROGRESS = 1,
		ACCEPTED = 2,
		DENIED = 3
	};

	float draw_until;

	VoteStatus status;
	uint8_t count_for;
	uint8_t count_against;
	uint8_t count_undecided;

	char str1[32];
	char str2[32];
	char called_by[32];

public:
	virtual int Init();
	virtual int VidInit();
	virtual int Draw(float time);

	int MsgFunc_Vote(const char* name, int size, void* buf);
};
