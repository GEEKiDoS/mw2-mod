#include <std_include.hpp>
#include "launcher/launcher.hpp"
#include "loader/loader.hpp"
#include "loader/binary_loader.hpp"
#include "loader/component_loader.hpp"
#include "game/game.hpp"
#include "utils/string.hpp"
#include "utils/flags.hpp"

//#define GENERATE_DIFFS

#ifdef GENERATE_DIFFS
#include "loader/binary_loader.hpp"
#endif

DECLSPEC_NORETURN void WINAPI exit_hook(const int code)
{
	component_loader::pre_destroy();
	exit(code);
}

void verify_tls()
{
	const utils::nt::library self;
	const auto self_tls = reinterpret_cast<PIMAGE_TLS_DIRECTORY>(self.get_ptr()
		+ self.get_optional_header()->DataDirectory[IMAGE_DIRECTORY_ENTRY_TLS].VirtualAddress);

	const auto ref = DWORD(&tls_data);
	const auto tls_index = *reinterpret_cast<DWORD*>(self_tls->AddressOfIndex);
	const auto tls_vector = *reinterpret_cast<DWORD*>(__readfsdword(0x2C) + 4 * tls_index);
	const auto offset = ref - tls_vector;

	if (offset != 0 && offset != 8) // Actually 8 is bad, but I think msvc places custom stuff before
	{
		throw std::runtime_error(utils::string::va("TLS payload is at offset 0x%X, but should be at 0!",
		                                           offset));
	}
}

FARPROC load_binary(const launcher::mode mode)
{
	loader loader(mode);
	utils::nt::library self;

	loader.set_import_resolver([self](const std::string& library, const std::string& function) -> FARPROC
	{
		if (library == "steam_api.dll")
		{
			return self.get_proc<FARPROC>(function);
		}
		else if (function == "ExitProcess")
		{
			return FARPROC(exit_hook);
		}

		return FARPROC(component_loader::load_import(library, function));
	});

	return loader.load(self);
}

int main()
{
	FARPROC entry_point;

	{
		auto premature_shutdown = true;
		const auto _ = gsl::finally([&premature_shutdown]()
		{
			if (premature_shutdown)
			{
				component_loader::pre_destroy();
			}
		});

		try
		{
			verify_tls();
			if (!component_loader::post_start()) return 0;

			entry_point = load_binary(launcher::mode::singleplayer);
			if (!entry_point)
			{
				throw std::runtime_error("Unable to load binary into memory");
			}

			game::enviroment::initialize(launcher::mode::singleplayer);
			if (!component_loader::post_load()) return 0;

			premature_shutdown = false;
		}
		catch (std::exception& e)
		{
			MessageBoxA(nullptr, e.what(), "ERROR", MB_ICONERROR);
			return 1;
		}
	}

	return entry_point();
}
