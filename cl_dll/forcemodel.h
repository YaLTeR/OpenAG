#pragma once
#include "com_model.h"

namespace force_model
{
	void hook_commands();

	void update_player_team(int player_index);
	void update_player_teams();
	void update_player_steam_id(int player_index);
	void update_player_steam_ids();
	model_t* get_model_override(int player_index);
};
