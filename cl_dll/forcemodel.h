#pragma once
#include "com_model.h"

namespace force_model
{
	void hook_commands();

	model_t* get_team_model_override(const std::string& team_name);
};
