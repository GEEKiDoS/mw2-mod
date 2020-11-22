#pragma once

#include "structs.hpp"
#include "launcher/launcher.hpp"

#define SELECT_VALUE(sp, mp, dedi) (game::is_sp() ? (sp) : (game::is_mp() ? (mp) : (dedi)))
#define WEAK __declspec(selectany)

namespace game
{
	namespace enviroment
	{
		bool is_mp();
		bool is_sp();
		bool is_dedi();

		void initialize(launcher::mode mode);
	}

	template <typename T>
	class Symbol
	{
	public:
		Symbol(const size_t sp_address)
			: sp_object_(reinterpret_cast<T*>(sp_address))
		{
		}

		operator T* () const
		{
			return sp_object_;
		}

		T* operator->() const
		{
			return this->operator T * ();
		}

	private:
		T* sp_object_;
	};

	// functions
	WEAK Symbol<void(errorParm code, const char* message, ...)> Com_Error{ 0x43DD90 };

	WEAK Symbol<void(const char* message)> Conbuf_AppendText{ 0x4F32D0 };

	WEAK Symbol<void()> Sys_ShowConsole{ 0x42C830 };

	WEAK Symbol<void(const char* cmdName, void(*function), cmd_function* allocedCmd, bool isKey)> Cmd_AddCommand{ 0x4478A0 };

	WEAK Symbol<const char* (const char* name)> DB_GetFastfilePath{ 0x4429F0 };
	WEAK Symbol<void(XZoneInfo* data, int count, int sync)> DB_LoadXAssets{ 0x4CFC90 };

	WEAK Symbol<dvar_t* (const char* dvar)> Dvar_FindVar{ 0x4B29D0 };
	WEAK Symbol<dvar_t* (const char* name, const char* value)> Dvar_SetCommand{ 0x4959D0 };
	WEAK Symbol<dvar_t* (const char* name, bool default_val, int flags, const char* description)> Dvar_RegisterBool{ 0x429390 };
	WEAK Symbol<dvar_t* (const char* name, int default_val, int min, int max, int flags, const char* description)> Dvar_RegisterInt{ 0x4E9490 };
	WEAK Symbol<dvar_t* (const char* name, const char* default_val, int, const char*)> Dvar_RegisterString{ 0x49E0B0 };
	WEAK Symbol<int(const char* name, const char* value)> Dvar_SetStringByName{ 0x440C60 };
	WEAK Symbol<dvar_t* (const char* cvar, const char* value)> Dvar_SetFromStringByName{ 0x4F52E0 };
	WEAK Symbol<dvar_t* (const char* cvar, const char* value, DvarSetSource source)> Dvar_SetFromStringByNameFromSource{ 0x4774E0 };

	WEAK Symbol<void(void* field) > Field_Clear{ 0x45C350 };

	WEAK Symbol<void(int bLanguageCull)>FS_DisplayPath{ 0x41AC20 };
	WEAK Symbol<void(const char* path, const char* pszGameFolder)> FS_AddIwdFilesForGameDirectory{ 0x630DB0 };

	WEAK Symbol<void* (size_t size)> Z_Malloc{ 0X4BF110 };

	// variables
	WEAK Symbol<CmdArgs> cmd_args{ 0x144FEF0 };
	WEAK Symbol<searchpath_s**> fs_searchpaths{ 0x195AA50 };
}
