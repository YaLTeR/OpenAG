#include "hud.h"
#include "cl_util.h"
#include "parsemsg.h"

namespace steam_id
{
	namespace
	{
		std::string steam_ids[MAX_PLAYERS];

		void msgfunc_AuthID(const char* name, int size, void* buf)
		{
			BEGIN_READ(buf, size);

			auto slot = READ_BYTE();
			auto id = READ_STRING();

			if (slot >= 1 && slot <= MAX_PLAYERS) {
				auto underscore = strchr(id, '_');

				if (underscore)
					steam_ids[slot - 1].assign(underscore + 1);
			}
		}
	}

	void hook_messages()
	{
		gEngfuncs.pfnHookUserMsg("AuthID", msgfunc_AuthID);
	}

	const std::string& get_steam_id(size_t player_index)
	{
		if (player_index < MAX_PLAYERS)
			return steam_ids[player_index];
		else
			return std::string();
	}
}
