#pragma once
#include <cstdint>
#include <string>
#include <vector>

class CHudLocation : public CHudBase
{
	struct Location
	{
		std::string name;
		Vector center;

		Location()
			: name("")
			, center(Vector(0, 0, 0))
		{
		}

		Location(std::string name, Vector center)
			: name(name)
			, center(center)
		{
		}
	};

	std::vector<Location> locations;
	size_t player_locations[MAX_PLAYERS + 1];

	void parse_locations(const char* mapname);
	void update_player_location(size_t id, const Vector& pos);

public:
	virtual int Init();
	virtual int VidInit();

	int MsgFunc_InitLoc(const char* name, int size, void* buf);
	int MsgFunc_Location(const char* name, int size, void* buf);

	const std::string& get_player_location(size_t id);
};
