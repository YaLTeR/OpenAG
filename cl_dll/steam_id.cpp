#include <algorithm> 
#include <cctype>
#include <chrono>
#include <functional> 
#include <fstream>
#include <locale>
#include <unordered_map>

#include "hud.h"
#include "cl_util.h"
#include "parsemsg.h"
#include "forcemodel.h"

namespace steam_id
{
	namespace
	{
		std::string steam_ids[MAX_PLAYERS];

		bool showing_real_names;
		std::string real_names[MAX_PLAYERS];
		std::unordered_map<std::string, std::string> steam_id_to_real_name;

		// String trimming functions from http://stackoverflow.com/a/217605
		inline std::string& ltrim(std::string& s) {
			s.erase(
				s.begin(),
				std::find_if(s.begin(), s.end(), [](auto c) { return !std::isspace(c); })
			);

			return s;
		}

		inline std::string& rtrim(std::string& s) {
			s.erase(
				std::find_if(s.rbegin(), s.rend(), [](auto c) { return !std::isspace(c); }).base(),
				s.end()
			);

			return s;
		}

		inline std::string& trim(std::string& s) {
			return ltrim(rtrim(s));
		}

		void parse_realnames()
		{
			steam_id_to_real_name.clear();

			auto gamedir = gEngfuncs.pfnGetGameDirectory();

			std::ifstream i(std::string(gamedir) + "/realnames.txt");
			if (!i)
				return;

			std::string s;
			while (std::getline(i, s)) {
				trim(s);

				if (s.empty())
					continue;

				// Skip past comments.
				if (s.length() >= 2 && s[0] == '/' && s[1] == '/')
					continue;

				// Find the first space, this is where the SteamID ends.
				auto first_space = std::find_if(
					s.cbegin(),
					s.cend(),
					[](auto c) { return std::isspace(c); }
				);

				// Skip past incorrect lines.
				if (first_space == s.cend())
					continue;

				/*
				 * Find the beginning of the name.
				 * This should always find something because otherwise trim would remove the space.
				 */
				auto real_name_start = std::find_if(
					first_space,
					s.cend(),
					[](auto c) { return !std::isspace(c); }
				);

				steam_id_to_real_name[std::string(s.cbegin(), first_space)] =
					std::string(real_name_start, s.cend());
			}
		}

		void update_real_names()
		{
			for (size_t i = 0; i < MAX_PLAYERS; ++i) {
				if (steam_ids[i].empty()) {
					real_names[i].clear();
				} else {
					auto entry = steam_id_to_real_name.find(steam_ids[i]);

					if (entry == steam_id_to_real_name.cend())
						real_names[i].clear();
					else
						real_names[i] = entry->second;
				} 
			}
		}

		int msgfunc_AuthID(const char* name, int size, void* buf)
		{
			BEGIN_READ(buf, size);

			auto slot = READ_BYTE();
			auto id = READ_STRING();

			if (slot >= 1 && slot <= MAX_PLAYERS) {
				auto underscore = strchr(id, '_');

				if (underscore) {
					steam_ids[slot - 1].assign(underscore + 1);

					if (showing_real_names)
						update_real_names();

					force_model::update_player_steam_id(slot - 1);
				}
			}

			return 1;
		}

		void callback_loadauthid()
		{
			auto start = std::chrono::steady_clock::now();
			parse_realnames();
			auto end = std::chrono::steady_clock::now();

			update_real_names();

			showing_real_names = true;

			gEngfuncs.Con_Printf(
				"Loaded %llu real names.\n",
				static_cast<unsigned long long>(steam_id_to_real_name.size())
			);

			auto parsing_took = std::chrono::duration<double, std::milli>(end - start);
			gEngfuncs.Con_DPrintf("Parsing took %.2f ms.\n", parsing_took.count());
		}

		void callback_unloadauthid()
		{
			auto print_message = (steam_id_to_real_name.size() > 0);
			steam_id_to_real_name.clear();

			showing_real_names = false;

			if (print_message)
				gEngfuncs.Con_Printf("Unloaded all real names.\n");
		}
	}

	void hook_messages()
	{
		gEngfuncs.pfnHookUserMsg("AuthID", msgfunc_AuthID);

		gEngfuncs.pfnAddCommand("loadauthid", callback_loadauthid);
		gEngfuncs.pfnAddCommand("unloadauthid", callback_unloadauthid);
	}

	const std::string& get_steam_id(size_t player_index)
	{
		return steam_ids[player_index];
	}

	bool is_showing_real_names()
	{
		return showing_real_names;
	}

	const std::string& get_real_name(size_t player_index)
	{
		return real_names[player_index];
	}
}
