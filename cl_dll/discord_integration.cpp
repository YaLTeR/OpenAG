#include <cstdint>
#include <string>

#include <discord-rpc.h>

#include "wrect.h"
#include "cl_dll.h"
#include "discord_integration.h"

namespace discord_integration
{
	// From Discord developer dashboard.
	constexpr const char CLIENT_ID[] = "378412193567473674";

	// This seems to be consistent across PCs.
	constexpr const char STEAM_APP_ID[] = "17215498729465839686";

	namespace
	{
		// For tracking if we're in-game.
		bool updated_client_data = false;

		// The current state.
		state current_state;

		// The current gamemode.
		std::string current_gamemode;

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

		// void handle_joinGame(const char* join_secret)
		// {
                // 
		// }
                // 
		// void handle_spectateGame(const char* spectate_secret)
		// {
                // 
		// }
                // 
		// void handle_joinRequest(const DiscordJoinRequest* request)
		// {
                // 
		// }

		void update()
		{
			DiscordRichPresence presence{};

			switch (current_state)
			{
			case state::NOT_PLAYING:
				presence.state = "Not Playing";
				break;

			case state::PLAYING:
				presence.state = "Playing";
				break;

			case state::IN_A_MATCH:
				presence.state = "In a Match";
				break;

			case state::SPECTATING:
				presence.state = "Spectating";
				break;
			}

			if (current_state != state::NOT_PLAYING && !current_gamemode.empty())
				presence.details = current_gamemode.c_str();

			presence.largeImageKey = "default";
			// presence.largeImageText = "OpenAG";
			// presence.smallImageKey = "default";
			// presence.smallImageText = "blah";

			Discord_UpdatePresence(&presence);
		}
	}

	void initialize()
	{
		DiscordEventHandlers handlers{};
		handlers.ready = handle_ready;
		handlers.errored = handle_errored;
		handlers.disconnected = handle_disconnected;
		// handlers.joinGame = handle_joinGame;
		// handlers.spectateGame = handle_spectateGame;
		// handlers.joinRequest = handle_joinRequest;
		Discord_Initialize(CLIENT_ID, &handlers, 1, STEAM_APP_ID);

		current_state = state::NOT_PLAYING;
		update();

		Discord_RunCallbacks();
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
				current_gamemode.clear();

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

		Discord_RunCallbacks();
	}
}
