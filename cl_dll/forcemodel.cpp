#include <algorithm>
#include <cstring>
#include <vector>

#include "hud.h"
#include "cl_util.h"
#include "com_model.h"
#include "r_studioint.h"
#include "forcemodel.h"

extern engine_studio_api_t IEngineStudio;
extern extra_player_info_t g_PlayerExtraInfo[MAX_PLAYERS + 1];

namespace force_model
{
	namespace
	{
		struct model_override
		{
			char* who;
			model_t* model;

			model_override(const char* who_, model_t* model)
				: model(model)
			{
				auto len = strlen(who_);

				who = new char[len + 1];
				std::memcpy(who, who_, len + 1);
			}

			~model_override()
			{
				delete[] who;
			}
		};

		// Overrides by the team name.
		std::vector<model_override> team_model_overrides;

		// Cache for fast lookup.
		model_t* team_model_overrides_cache[MAX_PLAYERS];

		void callback_cl_forceteammodel()
		{
			if (gEngfuncs.Cmd_Argc() != 3) {
				gEngfuncs.Con_Printf("cl_forceteammodel <team name> <model name>\n");
				return;
			}

			auto model_name = gEngfuncs.Cmd_Argv(2);

			char model_path[4096];
			std::snprintf(model_path, ARRAYSIZE(model_path), "models/player/%s/%s.mdl", model_name, model_name);

			auto model = IEngineStudio.Mod_ForName(model_path, 0);
			if (!model) {
				gEngfuncs.Con_Printf("This model could not be loaded.\n");
				return;
			}

			auto team_name = gEngfuncs.Cmd_Argv(1);

			auto it = std::find_if(
				team_model_overrides.begin(),
				team_model_overrides.end(),
				[=](const model_override& entry) {
					return !strcmp(entry.who, team_name);
				});

			if (it != team_model_overrides.end())
				it->model = model;
			else
				team_model_overrides.emplace_back(team_name, model);

			update_player_teams();
		}

		void callback_cl_forceteammodel_list()
		{
			if (team_model_overrides.size() == 0) {
				gEngfuncs.Con_Printf("There are no team model overrides.\n");
				return;
			}

			for (unsigned i = 0; i < team_model_overrides.size(); ++i) {
				const auto& entry = team_model_overrides[i];
				gEngfuncs.Con_Printf("%u. %s -> %s\n", i + 1, entry.who, entry.model->name);
			}
		}

		void callback_cl_forceteammodel_remove()
		{
			if (gEngfuncs.Cmd_Argc() != 2) {
				gEngfuncs.Con_Printf("cl_forceteammodel_remove <team name>\n");
				return;
			}

			auto it = std::find_if(
				team_model_overrides.begin(),
				team_model_overrides.end(),
				[=](const model_override& entry) {
					return !strcmp(entry.who, gEngfuncs.Cmd_Argv(1));
				});

			if (it != team_model_overrides.end()) {
				team_model_overrides.erase(it);

				update_player_teams();
			}
		}
	}

	void hook_commands()
	{
		gEngfuncs.pfnAddCommand("cl_forceteammodel", callback_cl_forceteammodel);
		gEngfuncs.pfnAddCommand("cl_forceteammodel_list", callback_cl_forceteammodel_list);
		gEngfuncs.pfnAddCommand("cl_forceteammodel_remove", callback_cl_forceteammodel_remove);
	}

	void update_player_team(int player_index)
	{
		auto team_name = g_PlayerExtraInfo[player_index + 1].teamname;

		auto it = std::find_if(
			team_model_overrides.cbegin(),
			team_model_overrides.cend(),
			[=](const model_override& entry) {
				return !strcmp(entry.who, team_name);
			});

		if (it != team_model_overrides.cend())
			team_model_overrides_cache[player_index] = it->model;
		else
			team_model_overrides_cache[player_index] = nullptr;
	}

	void update_player_teams()
	{
		for (size_t i = 0; i < MAX_PLAYERS; ++i)
			update_player_team(i);
	}

	model_t* get_model_override(int player_index)
	{
		return team_model_overrides_cache[player_index];
	}
}
