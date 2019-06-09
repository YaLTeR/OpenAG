#include <cstdlib>
#include <fstream>
#include <limits>
#include <string>

#include "hud.h"
#include "cl_util.h"
#include "parsemsg.h"

DECLARE_MESSAGE(m_Location, InitLoc);
DECLARE_MESSAGE(m_Location, Location);

void CHudLocation::parse_locations(const char* mapname)
{
	locations.resize(1);

	auto gamedir = gEngfuncs.pfnGetGameDirectory();

	std::ifstream i(std::string(gamedir) + "/locs/" + mapname + ".loc");
	if (!i)
		i = std::ifstream(std::string(gamedir) + "_downloads/locs/" + mapname + ".loc");

	if (!i)
		return;

	enum {
		NAME,
		X,
		Y,
		Z
	} state = NAME;

	Location current_loc;

	std::string s;
	while (std::getline(i, s, '#')) {
		switch (state) {
		case NAME:
			current_loc.name = std::move(s);
			state = X;
			break;

		case X:
			current_loc.center.x = std::strtod(s.c_str(), nullptr);
			state = Y;
			break;

		case Y:
			current_loc.center.y = std::strtod(s.c_str(), nullptr);
			state = Z;
			break;

		case Z:
			current_loc.center.z = std::strtod(s.c_str(), nullptr);

			// gEngfuncs.Con_Printf("Location %s: %.8f %.8f %.8f\n", current_loc.name.c_str(), current_loc.center.x, current_loc.center.y, current_loc.center.z);

			locations.push_back(std::move(current_loc));

			state = NAME;
			break;
		}
	}
}

void CHudLocation::update_player_location(size_t id, const Vector& pos)
{
	player_locations[id] = 0;

	#pragma push_macro("max")
	#undef max
	float min_dist_sqr = std::numeric_limits<float>::max();
	#pragma pop_macro("max")

	for (size_t i = 1; i < locations.size(); ++i) {
		const auto& loc = locations[i];
		auto distance_sqr = (pos - loc.center).LengthSqr();

		if (distance_sqr < min_dist_sqr) {
			min_dist_sqr = distance_sqr;
			player_locations[id] = i;
		}
	}
}

int CHudLocation::Init()
{
	HOOK_MESSAGE(InitLoc);
	HOOK_MESSAGE(Location);

	m_iFlags = 0;

	// The first location is always present.
	// It's used, for example, when the location file is empty.
	locations.emplace_back();

	gHUD.AddHudElem(this);
	return 0;
}

int CHudLocation::VidInit()
{
	return 1;
}

int CHudLocation::MsgFunc_InitLoc(const char* name, int size, void* buf)
{
	BEGIN_READ(buf, size);

	auto mapname = READ_STRING();
	parse_locations(mapname);

	// gEngfuncs.Con_Printf("InitLoc: %s.\n", mapname);

	return 1;
}

int CHudLocation::MsgFunc_Location(const char* name, int size, void* buf)
{
	BEGIN_READ(buf, size);

	int id = READ_BYTE();
	Vector pos;
	pos.x = READ_COORD();
	pos.y = READ_COORD();
	pos.z = READ_COORD();

	if (id >= 0 && id < MAX_PLAYERS + 1)
		update_player_location(id, pos);

	// gEngfuncs.Con_Printf("Location: %d (%.8f %.8f %.8f).\n", id, pos[0], pos[1], pos[2]);
	// gEngfuncs.Con_Printf("Player %d; location: %s.\n", id, get_player_location(id).c_str());

	return 1;
}

const std::string& CHudLocation::get_player_location(size_t id)
{
	if (id < MAX_PLAYERS + 1)
		return locations[player_locations[id]].name;

	return locations[0].name;
}
