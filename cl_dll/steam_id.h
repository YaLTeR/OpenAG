#pragma once
#include <cstdint>
#include <string>

namespace steam_id
{
	void hook_messages();
	const std::string& get_steam_id(size_t player_index);
}
