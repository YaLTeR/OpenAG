#include <string>
#include <unordered_map>

#include "hud.h"
#include "cl_util.h"
#include "com_model.h"
#include "r_studioint.h"

extern engine_studio_api_t IEngineStudio;

namespace force_model
{
	namespace
	{
		// A map from team names to model names.
		std::unordered_map<std::string, model_t*> team_model_map;

		void callback_cl_forceteammodel()
		{
			if (gEngfuncs.Cmd_Argc() != 3) {
				gEngfuncs.Con_Printf("cl_forceteammodel <team name> <model name>\n");
				return;
			}

			auto model_name = gEngfuncs.Cmd_Argv(2);

			char model_path[PATH_MAX];
			std::snprintf(model_path, ARRAYSIZE(model_path), "models/player/%s/%s.mdl", model_name, model_name);

			auto model = IEngineStudio.Mod_ForName(model_path, 0);
			if (!model) {
				gEngfuncs.Con_Printf("This model could not be loaded.\n");
				return;
			}

			team_model_map[gEngfuncs.Cmd_Argv(1)] = model;
		}

		void callback_cl_forceteammodel_list()
		{
			if (team_model_map.size() == 0) {
				gEngfuncs.Con_Printf("There are no model overrides.\n");
				return;
			}

			for (const auto& entry : team_model_map)
				gEngfuncs.Con_Printf("%s -> %s\n", entry.first.c_str(), entry.second->name);
		}

		void callback_cl_forceteammodel_remove()
		{
			if (gEngfuncs.Cmd_Argc() != 2) {
				gEngfuncs.Con_Printf("cl_forceteammodel_remove <team name>\n");
				return;
			}

			team_model_map.erase(gEngfuncs.Cmd_Argv(1));
		}
	}

	void hook_commands()
	{
		gEngfuncs.pfnAddCommand("cl_forceteammodel", callback_cl_forceteammodel);
		gEngfuncs.pfnAddCommand("cl_forceteammodel_list", callback_cl_forceteammodel_list);
		gEngfuncs.pfnAddCommand("cl_forceteammodel_remove", callback_cl_forceteammodel_remove);
	}

	model_t* get_team_model_override(const std::string& team_name)
	{
		auto model = team_model_map.find(team_name);

		if (model != team_model_map.cend())
			return model->second;
		else
			return nullptr;
	}
}
