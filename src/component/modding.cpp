#include <std_include.hpp>
#include <filesystem>
#include "component/dvar.hpp"
#include "loader/component_loader.hpp"
#include "utils/hook.hpp"
#include "game/game.hpp"

namespace
{
	static std::vector<std::filesystem::path> custom_zones;

	void add_search_path(const char* path, const char *basepath)
	{
		auto search = reinterpret_cast<game::searchpath_s *>(game::Z_Malloc(sizeof(game::searchpath_s)));
		auto dir = reinterpret_cast<game::directory_t*>(game::Z_Malloc(sizeof(game::directory_t)));

		strcpy_s(dir->gamedir, path);
		strcpy_s(dir->path, basepath);

		search->bLocalized = false;
		search->dir = dir;
		search->ignore = false;
		search->ignorePureCheck = true;
		search->iwd = 0;
		search->language = 0;

		game::searchpath_s** pSearch = *game::fs_searchpaths;
		search->next = *pSearch;
		*pSearch = search;
	}

	void add_custom_search_path()
	{
		dvar::var fs_basepath("fs_basepath");

		auto custom_path = "custom";

		if (std::filesystem::exists(custom_path))
		{
			for (auto& entry : std::filesystem::directory_iterator(custom_path))
			{
				if (entry.is_directory())
				{
					auto search_path = entry.path();

					add_search_path(search_path.string().data(), fs_basepath.get<const char *>());
					game::FS_AddIwdFilesForGameDirectory(fs_basepath.get<const char *>(), search_path.string().data());

					auto zone_path = search_path / "zone";

					if (std::filesystem::exists(zone_path))
						custom_zones.push_back(zone_path);
				}
			}
		}
	}

	void fs_displaypath_stub(int)
	{
		add_custom_search_path();

		game::FS_DisplayPath(1);
	}

	void load_xsurface_array_fix(int shouldLoad, int count)
	{
		// read the actual count from the varXModelSurfs ptr
		auto surface = *reinterpret_cast<game::XModelSurfs**>(0x9DB05C);

		// call original read function with the correct count
		return ((void(*)(int, int))0x44C880)(shouldLoad, surface->numsurfs);
	}

	const char* get_fastfile_path_stub(const char* name)
	{
		for (auto& path : custom_zones)
		{
			if (std::filesystem::exists(path / name))
			{
				return _strdup((path.string() + "\\").data());
			}
		}

		return game::DB_GetFastfilePath(name);
	}

	void load_custom_zone(game::XZoneInfo* data, int count, int sync)
	{
		std::vector<game::XZoneInfo> infos;

		for (int i = 0; i < count; i++)
			infos.push_back(data[i]);

		for (auto& path : custom_zones)
		{
			for (auto& entry : std::filesystem::directory_iterator(path))
			{
				if (entry.is_regular_file())
				{
					auto file = entry.path();

					if (file.has_extension() && file.extension() == ".ff")
					{
						game::XZoneInfo info =
						{
							_strdup(file.filename().replace_extension("").string().data()),
							1,
							0
						};

						infos.insert(infos.begin(), info);
					}
				}
			}
		}

		return game::DB_LoadXAssets(infos.data(), infos.size(), sync);
	}
}

class modding final : public component
{
public:
	void post_load() override
	{
		// Add custom search path on start
		utils::hook(0x47B2B2, fs_displaypath_stub, HOOK_CALL).install()->quick();
		// Load zones in custom search path
		utils::hook(0x50B637, load_custom_zone, HOOK_CALL).install()->quick();
		// Find zones in custom search path
		utils::hook(0x582F9C, get_fastfile_path_stub, HOOK_CALL).install()->quick();

		// Ignore zone version missmatch
		utils::hook::set(0x4256D8, (uint8_t)0xEB);

		// Ignore 'Disc read error.'
		utils::hook::nop(0x4B7335, 2);
		utils::hook::set(0x4B7356, (uint8_t)0xEB);
		utils::hook::set(0x4256B9, (uint8_t)0xEB);

		utils::hook(0x45EE95, load_xsurface_array_fix, HOOK_CALL).install()->quick();
	}
};

REGISTER_COMPONENT(modding)
