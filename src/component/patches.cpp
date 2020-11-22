#include <std_include.hpp>
#include "loader/component_loader.hpp"
#include "utils/hook.hpp"
#include "game/game.hpp"

class patches final : public component
{
public:
	void post_load() override
	{
		// prevent stat loading from steam
		utils::hook::set(0x43FB33, (uint8_t)0xC3);

		// remove limit on IWD file loading
		utils::hook::set(0x630FF3, (uint8_t)0xEB);

		// remove fs_game check for moddable rawfiles 
		// allows non-fs_game to modify rawfiles
		utils::hook::nop(0x612932, 2);

		// prevent matchmaking stuff
		utils::hook::set(0x43BAE0, (uint8_t)0xEB);

		// remove dvar restrictions
		// read only
		utils::hook::set(0x635841, (uint8_t)0xEB);
		// cheat protected
		utils::hook::set(0x635913, (uint8_t)0xEB);
		// write protected
		utils::hook::set(0x6358A5, (uint8_t)0xEB);
		// latched
		utils::hook::set(0x635974, (uint8_t)0xEB);

		// Ignore config problems
		utils::hook::set(0x4D3FD3, (uint8_t)0xEB);

		// No improper quit popup
		utils::hook::nop(0x4F5B3A, 2);

		// R_MAX_SKINNED_CACHE_VERTICES
		utils::hook::set(0x52046C, 0x480000 * 4);
		utils::hook::set(0x520489, 0x480000 * 4);
		utils::hook::set(0x52049C, 0x480000 * 4);
		utils::hook::set(0x520506, 0x480000 * 4);
		utils::hook::set(0x549245, 0x480000 * 4);
		utils::hook::set(0x549356, 0x480000 * 4);

		// PMem_Init, g_mem size
		utils::hook::set(0x4318ED, 0x140000 * 4);
		utils::hook::set(0x43190E, 0x140000 * 4);
		utils::hook::set(0x431922, 0x140000 * 4);
	}
};

REGISTER_COMPONENT(patches)
