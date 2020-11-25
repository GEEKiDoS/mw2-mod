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

	// rewritted functions
	Glyph* R_GetCharacterGlyph(Font_s* font, unsigned int letter);
	int R_LetterWidth(unsigned int letter, Font_s* font);

	// functions
	WEAK Symbol<void(errorParm code, const char* message, ...)> Com_Error{ 0x43DD90 };

	WEAK Symbol<void(const char* message)> Conbuf_AppendText{ 0x4F32D0 };

	WEAK Symbol<void()> Sys_ShowConsole{ 0x42C830 };

	WEAK Symbol<void(const char* cmdName, void(*function), cmd_function* allocedCmd, bool isKey)> Cmd_AddCommand{ 0x4478A0 };

	WEAK Symbol<void(int localClientNum, const char* text)> Cbuf_AddText{ 0x4A1090 };

	WEAK Symbol<const char* (const char* name)> DB_GetFastfilePath{ 0x4429F0 };
	WEAK Symbol<int(XAssetType type)> DB_GetXAssetTypeSize{ 0x40F720 };
	WEAK Symbol <XAssetHeader(XAssetType type, const char* name)> DB_FindXAssetHeader{ 0x40B200 };
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
	WEAK Symbol<int(const char* qpath, void** buffer)> FS_ReadFile{ 0x4A5480 };
	WEAK Symbol<int(const char* filename, int* file)> FS_FOpenFileRead{ 0x48DD10 };
	WEAK Symbol<int(int h)> FS_FCloseFile{ 0x44E0A0 };
	WEAK Symbol<int(void* buffer, int len, int h)> FS_Read{ 0x42EDC0 };
	WEAK Symbol<const char** (const char* path, const char* extension, FsListBehavior_e behavior, int* numfiles)>FS_ListFiles{ 0x4448F0 };
	WEAK Symbol<void(const char* path, const char* pszGameFolder)> FS_AddIwdFilesForGameDirectory{ 0x630DB0 };
	WEAK Symbol<void(const char** list)> FS_FreeFileList{ 0x41C7A0 };
	WEAK Symbol<void(void* buffer)> FS_FreeFile{ 0x4A7510 };
	WEAK Symbol<int(const char* filename, const void* buffer, int size)> FS_WriteFile{ 0x422380 };

	WEAK Symbol<int(int a1, const char* name)> Menus_OpenByName{ 0x4470B0 };

	WEAK Symbol<char* (const char* psFileName, bool forceEnglish)> SE_Load{ 0x409910 };
	WEAK Symbol<char* (bool forceEnglish)> SE_LoadLanguage{ 0x4F6F40 };
	WEAK Symbol<int(unsigned int firstChar, unsigned int secondChar, int* usedCount, int* pbIsTrailingPunctuation)> SEH_DecodeLetter{ 0x461B80 };

	WEAK Symbol<void __fastcall(void* _this, void* edx, const char* psLocalReference, const char* psNewString, int bSentenceIsEnglish)>CStringEdPackage_SetString {0x4BF570};
	WEAK Symbol<const char* (const char* psLocalReference)>CStringEdPackage_GetString {0x61BB10};

	WEAK Symbol<void()> Disconnect_f{ 0x57D8E0 };

	WEAK Symbol<void* (size_t size)> Z_Malloc{ 0x4BF110 };

	// variables
	WEAK Symbol<CmdArgs> cmd_args{ 0x144FEF0 };
	WEAK Symbol<searchpath_s**> fs_searchpaths{ 0x195AA50 };
	WEAK Symbol<void*[]> DB_XAssetPool{ 0x7337F8 };
	WEAK Symbol<unsigned int[]> g_poolSize{ 0x733510 };
}
