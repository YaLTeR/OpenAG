#pragma once

namespace discord_integration
{
	/**
	 * Possible game states.
	 */
	enum class state
	{
		NOT_PLAYING,
		PLAYING,
		IN_A_MATCH,
		SPECTATING,
	};

	/**
	 * Initializes the Discord integration. Call this once on game start.
	 */
	void initialize();

	/**
	 * Shuts down the Discord integration. Call this once on game end.
	 */
	void shutdown();

	/**
	 * Sets the Discord state.
	 */
	void set_state(state new_state);

	/**
	 * Sets the current gamemode.
	 */
	void set_gamemode(const char* new_gamemode);

	/**
	 * Call this in HUD_UpdateClientData.
	 */
	void on_update_client_data();

	/**
	 * Call this in HUD_Frame.
	 */
	void on_frame();

	/**
	 * Call this when a player joins or leaves.
	 */
	void on_player_count_update();
}
