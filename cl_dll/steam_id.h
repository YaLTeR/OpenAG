#pragma once
#include <cstdint>
#include <string>

namespace steam_id
{
	void hook_messages();

	const std::string& get_steam_id(size_t player_index);
	bool is_showing_real_names();
	const std::string& get_real_name(size_t player_index);
}
