#include <cstdint>
#include <cstring>
#include <memory>
#include <string>
#include <unordered_set>

#include <discord-rpc.h>

#include "hud.h"
#include "cl_util.h"
#include "vgui_TeamFortressViewport.h"
#include "net_api.h"
#include "discord_integration.h"

using namespace std::literals;

namespace discord_integration
{
	namespace
	{
		// From Discord developer dashboard.
		constexpr const char CLIENT_ID[] = "378412193567473674";

		// This seems to be consistent across PCs.
		constexpr const char STEAM_APP_ID[] = "17215498729465839686";

		// Maps for which we have thumbnails.
		const std::unordered_set<std::string> maps_with_thumbnails {
			"agtricks"s,
			"bkz_goldbhop"s,
			"bootbox"s,
			"boot_camp"s,
			"bounce"s,
			"bunnyrace_beta2"s,
			"crossfire"s,
			"datacore"s,
			"destructo_hops"s,
			"dm_dust2"s,
			"eden"s,
			"elixir"s,
			"endcamp"s,
			"frenzy"s,
			"gasworks"s,
			"havoc"s,
			"killbox"s,
			"lost_village2"s,
			"outcry"s,
			"rapidcore"s,
			"scary_1"s,
			"snark_pit"s,
			"stalkx"s,
			"stalkyard"s,
			"subtransit"s,
			"urethane"s,
			"vengeance"s,
			"zjumps"s,
			"ztricks"s
		};

		// For tracking if we're in-game.
		bool updated_client_data = false;

		// The current state.
		state current_state;

		// The current gamemode.
		std::string current_gamemode;

		// Current player count.
		int current_player_count;

		// Current max player count.
		int current_max_player_count;

		// Time of the last update.
		float last_update_time;

		void handle_ready()
		{
			gEngfuncs.Con_Printf("Connected to Discord.\n");
		}

		void handle_errored(int error_code, const char* message)
		{
			gEngfuncs.Con_Printf("Discord error (%d): %s\n", error_code, message);
		}

		void handle_disconnected(int error_code, const char* message)
		{
			gEngfuncs.Con_Printf("Disconnected from Discord (%d): %s\n", error_code, message);
		}

		void handle_joinGame(const char* join_secret)
		{
			std::string address(join_secret);
			sanitize_address(address);

			if (address.empty())
				return;

			auto command = "connect "s + address + "\n"s;

			std::unique_ptr<char[]> temp(new char[command.size() + 1]);
			std::memcpy(temp.get(), command.data(), command.size() + 1);
			EngineClientCmd(temp.get());
		}

		void handle_spectateGame(const char* spectate_secret)
		{
			// TODO
			handle_joinGame(spectate_secret);
		}

		void handle_joinRequest(const DiscordJoinRequest* request)
		{
			Discord_Respond(request->userId, DISCORD_REPLY_YES);
		}

		void update()
		{
			DiscordRichPresence presence{};

			std::string state;

			switch (current_state)
			{
			case state::NOT_PLAYING:
				state = "Not Playing"s;
				break;

			case state::PLAYING:
				state = "Playing"s;
				break;

			case state::IN_A_MATCH:
				state = "In a Match"s;
				break;

			case state::SPECTATING:
				state = "Spectating"s;
				break;
			}

			presence.largeImageKey = "default";

			// Declare them outside of the following block, so they are in scope for Discord_UpdatePresence().
			char map_name[64];
			net_status_t netstatus{};
			std::string spectate_secret;
			std::string party_id;

			if (current_state != state::NOT_PLAYING)
			{
				if (!current_gamemode.empty())
					state += " | "s + current_gamemode;

				if (gViewPort->m_szServerName[0])
					presence.details = gViewPort->m_szServerName;

				const auto length = get_map_name(map_name, ARRAYSIZE(map_name));

				if (map_name[0])
				{
					if (maps_with_thumbnails.find(map_name) != maps_with_thumbnails.cend())
						presence.largeImageKey = map_name;
					else
						presence.largeImageKey = "default";

					presence.largeImageText = map_name;
				}

				gEngfuncs.pNetAPI->Status(&netstatus);
				auto address = gEngfuncs.pNetAPI->AdrToString(&netstatus.remote_address);
				presence.joinSecret = address;

				spectate_secret = address + " "s;
				presence.spectateSecret = spectate_secret.c_str();

				party_id = address + "_"s; // HACK: secrets can't match party id.
				presence.partyId = party_id.c_str();
				presence.partySize = current_player_count;
				presence.partyMax = current_max_player_count;
			}

			presence.state = state.c_str();

			Discord_UpdatePresence(&presence);
		}
	}

	void initialize()
	{
		DiscordEventHandlers handlers{};
		handlers.ready = handle_ready;
		handlers.errored = handle_errored;
		handlers.disconnected = handle_disconnected;
		handlers.joinGame = handle_joinGame;
		handlers.spectateGame = handle_spectateGame;
		handlers.joinRequest = handle_joinRequest;
		Discord_Initialize(CLIENT_ID, &handlers, 1, STEAM_APP_ID);

		current_state = state::NOT_PLAYING;
		update();

		Discord_RunCallbacks();

		last_update_time = gEngfuncs.GetClientTime();
	}

	void shutdown()
	{
		Discord_Shutdown();
	}

	void set_state(state new_state)
	{
		if (current_state != new_state)
		{
			current_state = new_state;

			if (new_state == state::NOT_PLAYING)
			{
				current_gamemode.clear();
			}
			else
			{
				current_player_count = get_player_count();
				current_max_player_count = gEngfuncs.GetMaxClients();
			}

			update();
		}
	}

	void set_gamemode(const char* new_gamemode)
	{
		if (current_gamemode != new_gamemode)
		{
			current_gamemode = new_gamemode;
			update();
		}
	}

	void on_update_client_data()
	{
		updated_client_data = true;
	}

	void on_frame()
	{
		if (!updated_client_data)
			set_state(state::NOT_PLAYING);
		else if (current_state == state::NOT_PLAYING)
			set_state(state::PLAYING);

		updated_client_data = false;

		const auto current_time = gEngfuncs.GetClientTime();
		if (last_update_time < current_time || last_update_time - current_time > 5)
		{
			last_update_time = current_time;
			on_player_count_update();
		}

		Discord_RunCallbacks();
	}

	void on_player_count_update()
	{
		if (current_state != state::NOT_PLAYING) {
			auto player_count = get_player_count();
			auto max_player_count = gEngfuncs.GetMaxClients();

			if (current_player_count != player_count || current_max_player_count != max_player_count) {
				current_player_count = player_count;
				current_max_player_count = max_player_count;
				update();
			}
		}
	}
}
