#include <std_include.hpp>
#include <map>

#include "component/dvar.hpp"
#include "component/filesystem.hpp"

#include "loader/component_loader.hpp"
#include "utils/hook.hpp"
#include "game/game.hpp"

namespace
{
	static std::unordered_map<std::string, std::u8string> overrides;

	void replace_font(std::string orig_font, std::string new_font)
	{
		if (orig_font == "default")
		{
			*(char**)0x620FAD =
				*(char**)0x620FBE =
				*(char**)0x620FE0 =
				*(char**)0x620FF4 =
				*(char**)0x621005 = _strdup(new_font.data());
		}
		else if (orig_font == "hud")
		{
			*(char**)0x621027 =
				*(char**)0x621038 = _strdup(new_font.data());
		}
		else if (orig_font == "objective")
		{
			*(char**)0x621016 = _strdup(new_font.data());
		}
	}

	void read_font_config()
	{
		filesystem::file cfg_file{ "font_config.json" };

		if (cfg_file.exists())
		{
			rapidjson::Document cfg;
			cfg.Parse(cfg_file.get_buffer().data());

			for (auto iter = cfg.MemberBegin(); iter != cfg.MemberEnd(); iter++)
				replace_font(iter->name.GetString(), iter->value.GetString());
		}
	}

	void load_custom_strs()
	{
		int numFiles = 0;
		const char** files = game::FS_ListFiles("localized_strings/custom/", "str", game::FS_LIST_ALL, &numFiles);
		for (int i = 0; i < numFiles; i++)
		{
			printf("Loading custom localized string %s\n", files[i]);

			auto error_msg = game::SE_Load(("localized_strings/custom/"s + files[i]).data(), false);

			if (error_msg)
				printf("%s\n", error_msg);
		}

		game::FS_FreeFileList(files);
	}

	const char* seh_stringed_getstring_hook(const char* pszReference)
	{
		dvar::var loc_translate{ "loc_translate" };

		if (loc_translate.get<bool>())
		{
			if (overrides.find(pszReference) != overrides.end())
				return (const char *)overrides[pszReference].data();

			auto *entry = game::DB_FindXAssetHeader(game::ASSET_TYPE_LOCALIZE_ENTRY, pszReference).localize;

			return entry->value;
		}

		return pszReference;
	}

	void __fastcall set_string_hook(void* _this, void* edx, const char* psLocalReference, const char* psNewString, int bSentenceIsEnglish)
	{
		overrides[psLocalReference] = (const char8_t *)psNewString;
	}

	char* se_load_language_stub(bool forceEnglish)
	{
		char* error = game::SE_LoadLanguage(forceEnglish);

		read_font_config();

		auto hk = utils::hook(0x4759FE, set_string_hook, HOOK_CALL);
		hk.install()->quick();

		load_custom_strs();

		hk.uninstall()->quick();

		return error;
	}

#ifdef USE_UTF8
	int u8_read_char(const char* utf8Stream, int* numBytesConsumed)
	{
		int u8char = 0;

		*numBytesConsumed = 0;
		char ch = *utf8Stream;
		if (*utf8Stream >= 0)
		{
			*numBytesConsumed = 1;
			return ch;
		}
		if ((ch & 0xE0) == 0xC0u)
		{
			u8char = utf8Stream[1] & 0x7F | ((ch & 0x1F) << 6);
			if (u8char >= 128)
			{
				*numBytesConsumed = 2;
				return u8char;
			}
		}
		else if ((ch & 0xF0) == 0xE0u)
		{
			u8char = utf8Stream[2] & 0x7F | ((utf8Stream[1] & 0x7F | ((ch & 0xF) << 6)) << 6);
			if (u8char - 2048 <= 53247 || u8char >= 57344)
			{
				*numBytesConsumed = 3;
				return u8char;
			}
		}
		else if ((ch & 0xF8) == 0xF0u)
		{
			u8char = utf8Stream[3] & 0x7F | ((utf8Stream[2] & 0x7F | ((utf8Stream[1] & 0x7F | ((ch & 7) << 6)) << 6)) << 6);
			if (u8char - 65536 <= 1048574)
			{
				*numBytesConsumed = 4;
				return u8char;
			}
		}

		// fallback to original SEH_DecodeLetter
		return game::SEH_DecodeLetter(utf8Stream[0], utf8Stream[1], numBytesConsumed, nullptr);
	}

	uint32_t seh_read_char_from_string_utf8(const char** text, int* isTrailingPunctuation)
	{
		int usedCount = 0;
		int letter = u8_read_char(*text, &usedCount);

		*text += usedCount;
		return letter;
	}

	int printable_chars_count_u8(game::MessageWindow* msgwnd, game::MessageLine* line)
	{
		int printedCnt = 0;
		int idx = 0;

		if (line->textBufSize > 0)
		{
			do
			{
				int pos = msgwnd->textBufPos + idx;
				int len = line->textBufSize - 1;

				int usedCharCnt = 0;
				auto ch = u8_read_char(msgwnd->circularTextBuffer + (len & pos), &usedCharCnt);

				idx += usedCharCnt;
				++printedCnt;

				if (ch == '^')
				{
					char color_code = msgwnd->circularTextBuffer[(msgwnd->textBufSize - 1) & idx + line->textBufPos];

					if (color_code != '^' && color_code - '0' <= 11)
						++idx;
				}
			} while (idx < line->textBufSize);
		}

		return printedCnt;
	}

	// stop it compiler!
	__declspec(naked) void printable_chars_count_stub()
	{
		__asm
		{
			push    ebp
			mov     ebp, [esp + 18h]
			push	ebx
			push	ebp
			call	printable_chars_count_u8
			add		esp, 8
			pop		ebp
			retn
		}
	}

	int r_console_text_width_hook(const char* textPool, int poolSize, int firstChar, int charCount, game::Font_s* font)
	{
		int indexMask = poolSize - 1;
		int stopPos = (poolSize - 1) & (charCount + firstChar);
		int parsePos = firstChar;
		int width = 0;

		while (parsePos != stopPos)
		{
			int usedCharCount = 0;
			int letter = u8_read_char(textPool + parsePos, &usedCharCount);
			parsePos = indexMask & (usedCharCount + parsePos);
			if (letter == 94 && 
			   (&textPool[parsePos] && 
				textPool[parsePos] != 94 && 
				textPool[parsePos] >= 48 && 
				textPool[parsePos] <= 64 || 
				&textPool[parsePos] && 
				textPool[parsePos] != 94 && 
				textPool[parsePos] == 70))
			{
				parsePos = indexMask & (parsePos + 1);
			}
			else if (letter != 94 || textPool[parsePos] != 1 && textPool[parsePos] != 2)
			{
				width += game::R_LetterWidth(letter, font);
			}
			else
			{
				width += (font->pixelHeight * (textPool[indexMask & (parsePos + 1)] - 16) + 16) / 32;
				parsePos = indexMask & (parsePos + 7);
			}
		}

		return width;
	}

	int read_char_from_string_1(const char* text)
	{
		return seh_read_char_from_string_utf8(&text, nullptr);
	}
#endif
}

class localized_string final : public component
{
public:
	void post_load() override
	{
		utils::hook(0x61BCA7, se_load_language_stub, HOOK_CALL).install()->quick();
		utils::hook(0x61BB10, seh_stringed_getstring_hook).install()->quick();

#ifdef USE_UTF8
		// yay, replace the MBCS with UTF-8!
		utils::hook(0x416100, seh_read_char_from_string_utf8, HOOK_JUMP).install()->quick();
		utils::hook(0x508BB0, r_console_text_width_hook, HOOK_JUMP).install()->quick();
		utils::hook(0x5775E0, printable_chars_count_stub, HOOK_JUMP).install()->quick();
		utils::hook(0x4B1050, read_char_from_string_1, HOOK_JUMP).install()->quick();
#endif
	}
};

REGISTER_COMPONENT(localized_string)
