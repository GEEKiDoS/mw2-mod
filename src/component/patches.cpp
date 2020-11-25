#include <std_include.hpp>
#include "component/dvar.hpp"
#include "component/scheduler.hpp"
#include "loader/component_loader.hpp"
#include "utils/hook.hpp"
#include "game/game.hpp"

namespace
{
	void reallocate_asset_pool(game::XAssetType type, unsigned int size)
	{
		int asset_size = game::DB_GetXAssetTypeSize(type);
		void* pool = malloc(size * asset_size);

		(*game::DB_XAssetPool)[type] = pool;
		(*game::g_poolSize)[type] = size;
	}

	utils::detour* sv_spawn_server_detour = nullptr;

	std::string sp_nextmap;
	int sp_nextmap_savegame;

	void sv_spawn_server_hook(const char* server, int mapIsPreloaded, int savegame)
	{
		dvar::var sv_running("sv_running");
		
		if (sv_running.get<bool>())
		{
			sp_nextmap = server;
			sp_nextmap_savegame = savegame;

			game::Cbuf_AddText(0, "disconnect\n");
		}
		else
		{
			sv_spawn_server_detour->get(sv_spawn_server_hook)(server, mapIsPreloaded, savegame);
		}
	}

	void menu_open_mainmenu_hook(int a1, const char *name)
	{
		if (sp_nextmap != "")
		{
			std::string nextmap = sp_nextmap;
			sp_nextmap = "";

			scheduler::once([nextmap]() 
			{
				sv_spawn_server_detour->get(sv_spawn_server_hook)(nextmap.data(), 0, sp_nextmap_savegame);
			});

			game::Menus_OpenByName(a1, "pregame");
		}
		else
		{
			game::Menus_OpenByName(a1, name);
		}
	}
}

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

		reallocate_asset_pool(game::ASSET_TYPE_RAWFILE, 4096);

		// fix to crash in sub_5250E0 while changing map from af_caves to af_chase with gfl mod
		utils::hook(0x4A7F07, menu_open_mainmenu_hook, HOOK_CALL).install()->quick();
		sv_spawn_server_detour = new utils::detour(0x4348E0, sv_spawn_server_hook);

		//utils::hook(0x5250E0, sub_5250E0_hook).install()->quick();
	}

	void pre_destroy() override
	{
		delete sv_spawn_server_detour;
	}
};

REGISTER_COMPONENT(patches)
