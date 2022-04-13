#include <chrono>
#include <cstdint>
#include <cstring>
#include <memory>
#include <string>
#include <unordered_set>

#include <discord_rpc.h>

#include "hud.h"
#include "cl_util.h"
#include "vgui_TeamFortressViewport.h"
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
			"2bfree"s,
			"8b1_hellinashop"s,
			"ag_bhop_dungeon"s,
			"ag_bhop_master"s,
			"ag_bhop_parkour"s,
			"ag_bhop_r"s,
			"ag_jumpcrazy"s,
			"agtricks"s,
			"agtricks_2018"s,
			"b2j_bhopward"s,
			"bh_axn_ravine"s,
			"bh_axn_tunnel"s,
			"bhm_assault"s,
			"bhm_ramp"s,
			"bhop_aeonflux"s,
			"bhop_avantura"s,
			"bhop_axn_project64"s,
			"bhop_blocksponge"s,
			"bhop_crossfire"s,
			"bhop_janne"s,
			"bhop_kisu"s,
			"bhop_lava_cave"s,
			"bhop_lego"s,
			"bhop_littletown"s,
			"bhop_m_novice"s,
			"bhop_m_space"s,
			"bhop_m_temple"s,
			"bhop_m_xmas"s,
			"bhop_maratona"s,
			"bhop_medieval"s,
			"bhop_minecraft_v1"s,
			"bhop_molwiz"s,
			"bhop_platinum"s,
			"bhop_pool_day"s,
			"bhop_river_wb"s,
			"bhop_rooftops"s,
			"bhop_runners"s,
			"bhop_tunnels"s,
			"bhop_xmas_2013"s,
			"bkz_aztecbhop"s,
			"bkz_goldbhop"s,
			"bkz_junglebhop"s,
			"boot_camp"s,
			"bootbox"s,
			"bounce"s,
			"bunnyrace_beta2"s,
			"cg_cbblebhop"s,
			"cg_coldbhop_v2"s,
			"cg_industry"s,
			"cg_tuscan"s,
			"cg_wildwesthop"s,
			"cnd_speed_bhop"s,
			"cobkz_construction"s,
			"cosy_merrychristmas4"s,
			"crossfire"s,
			"d2_mario_bhop"s,
			"datacore"s,
			"daza_junglebhop"s,
			"de_racetownz"s,
			"destructo_hops"s,
			"dev_control"s,
			"dg_winterclimb"s,
			"dm_dust2"s,
			"dr0_deepbluesea"s,
			"dyd_axn_plant"s,
			"dyd_axn_sky"s,
			"dyd_cosy_cupbhop_ez"s,
			"e1m1"s,
			"eden"s,
			"elixir"s,
			"endcamp"s,
			"fof_axn_scroll_killa"s,
			"fof_chillbhop"s,
			"frenzy"s,
			"fu_darkhop"s,
			"gasworks"s,
			"gayl0rd_bhop"s,
			"havoc"s,
			"hl1_bhop_am"s,
			"hl1_bhop_bp1"s,
			"hl1_bhop_bp2"s,
			"hl1_bhop_faf"s,
			"hl1_bhop_lc"s,
			"hl1_bhop_oar"s,
			"hl1_bhop_ocwgh"s,
			"hl1_bhop_pu"s,
			"hl1_bhop_qe"s,
			"hl1_bhop_rp"s,
			"hl1_bhop_st"s,
			"hl1_bhop_uc1"s,
			"hl1_bhop_uc2"s,
			"hl_trick"s,
			"hm_castlebhop"s,
			"hm_speedwinterz"s,
			"holy_strafemaster"s,
			"hoptrack"s,
			"ins_axn_chessboard"s,
			"ins_axn_tooncastle"s,
			"ins_toontrain"s,
			"j2s_4floors"s,
			"jagkz_inferno"s,
			"ka_kart-race"s,
			"killbox"s,
			"klbk_go"s,
			"ksz_skeleton"s,
			"kz-endo_bikinihop"s,
			"kz-endo_blue"s,
			"kz-endo_congo"s,
			"kz-endo_toonbhopz"s,
			"kz-endo_topspeed"s,
			"kz_antiquity"s,
			"kz_anubis"s,
			"kz_axn_croft"s,
			"kz_cargo"s,
			"kz_cg_venice"s,
			"kz_desolate3"s,
			"kz_dojo"s,
			"kz_gigablockier_pub"s,
			"kz_j2s_tetris"s,
			"kz_kz-endo_portal"s,
			"kz_kzlt_femtobhop"s,
			"kz_kzsca_hell"s,
			"kz_kzse_aztectemple"s,
			"kz_megabhop"s,
			"kz_satomi"s,
			"kz_sola"s,
			"kz_synergy"s,
			"kz_synergy_x"s,
			"kz_world"s,
			"kzarg_darkcave"s,
			"kzarg_taringacs_bhopcity"s,
			"kzcn_riverhop"s,
			"kzfr_bhop_backalley"s,
			"kzfr_bhop_leetyard"s,
			"kzfr_speed_bhopbloc_ez"s,
			"kzls_bhop_china"s,
			"kzls_bhop_temple"s,
			"kzm_cityhops"s,
			"kzra_diaobhop_med"s,
			"kzru_mam6ahop"s,
			"kzsca_snakebhop"s,
			"kzsca_watertemple"s,
			"lost_village2"s,
			"mf_doom_e1m1_e"s,
			"outcry"s,
			"pd_shafthop"s,
			"prochallenge2_longjump"s,
			"rapidcore"s,
			"rnr_speedcrag"s,
			"scary_1"s,
			"smk_floppytown"s,
			"snark_pit"s,
			"speed_ytt_egypt"s,
			"stalkx"s,
			"stalkyard"s,
			"subtransit"s,
			"uq_axn_imoor"s,
			"uq_skrol_bhop"s,
			"urethane"s,
			"vengeance"s,
			"xjbg_bhoptemple_hard"s,
			"zjumps"s,
			"ztricks"s
		};

		// Text names of game states
		const std::string STATE_NAMES[] = {
			"Not Playing"s,
			"Playing"s,
			"Spectating"s
		};

		// For tracking if we're in-game.
		bool updated_client_data = false;

		// Possible game states.
		enum class game_state
		{
			NOT_PLAYING = 0,
			PLAYING,
			SPECTATING,
		};

		// Class that handles tracking state changes.
		class DiscordState {
		public:
			DiscordState()
				: cur_state(game_state::NOT_PLAYING)
				, gamemode()
				, player_count(0)
				, player_limit(0)
				, seconds_total(0)
				, start_timestamp(0)
				, match_is_on(false)
				, dirty(true)
			{
				update_presence();
			};

			inline void set_game_state(game_state new_game_state)
			{
				if (cur_state != new_game_state)
				{
					cur_state = new_game_state;

					if (new_game_state == game_state::NOT_PLAYING)
					{
						gamemode.clear();
						match_is_on = false;
						seconds_total = 0;
					}
					else
					{
						refresh_player_stats();
					}

					dirty = true;
				}
			}

			inline game_state get_game_state() const
			{
				return cur_state;
			}

			inline void set_gamemode(std::string new_gamemode)
			{
				if (gamemode != new_gamemode)
				{
					gamemode = std::move(new_gamemode);
					dirty = true;
				}
			}

			inline void set_player_count(int new_player_count)
			{
				if (player_count != new_player_count)
				{
					player_count = new_player_count;
					dirty = true;
				}
			}

			inline void set_player_limit(int new_player_limit)
			{
				if (player_limit != new_player_limit)
				{
					player_limit = new_player_limit;
					dirty = true;
				}
			}

			inline void set_match_is_on(bool new_match_is_on)
			{
				if (match_is_on != new_match_is_on)
				{
					match_is_on = new_match_is_on;
					dirty = true;
				}
			}

			inline void set_time_data(int new_seconds_total, int seconds_passed)
			{
				if (seconds_total != new_seconds_total)
				{
					seconds_total = new_seconds_total;
					const auto current_timestamp = std::chrono::duration_cast<std::chrono::seconds>(
						std::chrono::system_clock::now().time_since_epoch()).count();
					start_timestamp = current_timestamp - seconds_passed;
					dirty = true;
				}
			}

			inline void refresh_player_stats()
			{
				set_player_count(get_player_count());
				set_player_limit(gEngfuncs.GetMaxClients());
			}

			inline void update_presence_if_dirty()
			{
				if (dirty)
					update_presence();
			}

		protected:
			void update_presence()
			{
				dirty = false;

				DiscordRichPresence presence{};

				std::string state = STATE_NAMES[static_cast<size_t>(cur_state)];
				if (cur_state == game_state::PLAYING && match_is_on)
					state = "In a Match"s;

				// Default icon.
				presence.largeImageKey = "default";

				// Declare these outside of the following block, so they are in scope for Discord_UpdatePresence().
				char map_name[64];
				std::string party_id;

				if (cur_state != game_state::NOT_PLAYING)
				{
					// Get the gamemode.
					if (!gamemode.empty())
						state += " | "s + gamemode;

					// Get the server name.
					if (gViewPort->m_szServerName[0])
						presence.details = gViewPort->m_szServerName;

					// Get the map name and icon.
					get_map_name(map_name, ARRAYSIZE(map_name));
					if (map_name[0])
					{
						char *temp = strdup(map_name); // make a copy

						// adjust copy to lowercase
						unsigned char *tptr = (unsigned char *)temp;
						while(*tptr) {
							*tptr = tolower(*tptr);
							tptr++;
						}

						if (maps_with_thumbnails.find(temp) != maps_with_thumbnails.cend())
							presence.largeImageKey = temp;

						presence.largeImageText = temp;

						free(temp);
					}

					// Get the server address.
					const auto address = get_server_address();
					presence.joinSecret = address;

					party_id = address + "_"s + map_name;
					presence.partyId = party_id.c_str();
					presence.partySize = player_count;
					presence.partyMax = player_limit;

					if (seconds_total != 0)
					{
						presence.startTimestamp = start_timestamp;
						presence.endTimestamp = start_timestamp + seconds_total;
					}
				}

				presence.state = state.c_str();

				Discord_UpdatePresence(&presence);
			}

			game_state cur_state;
			std::string gamemode;
			int player_count;
			int player_limit;
			int seconds_total;
			int64_t start_timestamp;
			bool match_is_on;

			// Flag indicating there are some changes not sent to Discord yet.
			bool dirty;
		};

		// Pointer so the constructor doesn't run too early.
		std::unique_ptr<DiscordState> discord_state;

		// Time of the last update.
		double last_update_time;

		void handle_ready(const DiscordUser*)
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

		void handle_joinRequest(const DiscordUser* request)
		{
			Discord_Respond(request->userId, DISCORD_REPLY_YES);
		}
	}

	void initialize()
	{
		DiscordEventHandlers handlers{};
		handlers.ready = handle_ready;
		handlers.errored = handle_errored;
		handlers.disconnected = handle_disconnected;
		handlers.joinGame = handle_joinGame;
		handlers.joinRequest = handle_joinRequest;
		Discord_Initialize(CLIENT_ID, &handlers, 1, STEAM_APP_ID);

		discord_state = std::make_unique<DiscordState>();

		Discord_RunCallbacks();

		last_update_time = gEngfuncs.GetAbsoluteTime();
	}

	void shutdown()
	{
		discord_state.reset();
		Discord_Shutdown();
	}

	void set_spectating(bool spectating)
	{
		if (spectating)
			discord_state->set_game_state(game_state::SPECTATING);
		else
			discord_state->set_game_state(game_state::PLAYING);
	}

	void set_gamemode(std::string gamemode)
	{
		discord_state->set_gamemode(std::move(gamemode));
	}

	void set_match_is_on(bool match_is_on)
	{
		discord_state->set_match_is_on(match_is_on);
	}

	void set_time_data(int seconds_total, int seconds_passed)
	{
		discord_state->set_time_data(seconds_total, seconds_passed);
	}

	void on_update_client_data()
	{
		updated_client_data = true;
	}

	void on_frame()
	{
		// Check if we're still in-game.
		if (!updated_client_data)
			discord_state->set_game_state(game_state::NOT_PLAYING);
		else if (discord_state->get_game_state() == game_state::NOT_PLAYING)
			// Only set this if we weren't playing, otherwise we might overwrite some other state.
			discord_state->set_game_state(game_state::PLAYING);

		updated_client_data = false;

		// Check player counts for updates every now and then.
		const auto current_time = gEngfuncs.GetAbsoluteTime();
		if (last_update_time < current_time || last_update_time - current_time > 5)
		{
			last_update_time = current_time;
			on_player_count_update();
			discord_state->update_presence_if_dirty();
		}

		Discord_RunCallbacks();
	}

	void on_player_count_update()
	{
		discord_state->refresh_player_stats();
	}
}
