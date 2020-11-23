#include <std_include.hpp>
#include "loader/component_loader.hpp"
#include "utils/hook.hpp"
#include "game/game.hpp"

namespace
{
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
	}

	char* se_load_language_stub(bool forceEnglish)
	{
		char* error = game::SE_LoadLanguage(forceEnglish);

		load_custom_strs();

		return error;
	}

	// from http://www.json.org/JSON_checker/utf8_decode.c
	int u8_read_char(const char* str, int* numBytesConsumed)
	{
		// the first byte of the character
		int c;
		// the first continuation character
		int c1;
		// the second continuation character
		int c2;
		// the third continuation character
		int c3;
		// the result
		int r;

		c = str[0];
		// Zero continuation (0 to 127)
		if ((c & 0x80) == 0)
		{
			*numBytesConsumed = 1;
			return c;
		}

		// One continuation (128 to 2047)
		if ((c & 0xE0) == 0xC0)
		{
			c1 = str[1];
			if (c1 >= 0)
			{
				r = ((c & 0x1F) << 6) | c1;
				if (r >= 128)
				{
					*numBytesConsumed = 2;
					return r;
				}
			}
		}
		// Two continuations (2048 to 55295 and 57344 to 65535)
		else if ((c & 0xF0) == 0xE0)
		{
			c1 = str[1];
			c2 = str[2];
			if ((c1 | c2) >= 0)
			{
				r = ((c & 0x0F) << 12) | (c1 << 6) | c2;
				if (r >= 2048 && (r < 55296 || r > 57343))
				{
					*numBytesConsumed = 3;
					return r;
				}
			}
		}
		// Three continuations (65536 to 1114111)
		else if ((c & 0xF8) == 0xF0)
		{
			c1 = str[1];
			c2 = str[2];
			c3 = str[3];
			if ((c1 | c2 | c3) >= 0)
			{
				r = ((c & 0x07) << 18) | (c1 << 12) | (c2 << 6) | c3;
				if (r >= 65536 && r <= 1114111)
				{
					*numBytesConsumed = 4;
					return r;
				}
			}
		}

		// fallback to original SEH_DecodeLetter
		return game::SEH_DecodeLetter(str[0], str[1], numBytesConsumed, nullptr);
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

		char psText[5];
		int idx = 0;

		if (line->textBufSize > 0)
		{
			do
			{
				int pos = msgwnd->textBufPos + idx;
				int len = line->textBufSize - 1;

				psText[0] = msgwnd->circularTextBuffer[len & pos];
				psText[1] = msgwnd->circularTextBuffer[len & (pos + 1)];
				psText[2] = msgwnd->circularTextBuffer[len & (pos + 2)];
				psText[3] = msgwnd->circularTextBuffer[len & (pos + 3)];

				int usedCharCnt = 0;
				auto ch = u8_read_char(psText, &usedCharCnt);

				idx += usedCharCnt;
				++printedCnt;

				if (ch == '^')
				{
					char color_code = msgwnd->circularTextBuffer[(msgwnd->textBufSize - 1) & idx + line->textBufPos];
					psText[0] = color_code;

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
}

class localized_string final : public component
{
public:
	void post_load() override
	{
		utils::hook(0x61BCA7, se_load_language_stub, HOOK_CALL).install()->quick();

		// yay, replace the MBCS with UTF-8!
		utils::hook(0x416100, seh_read_char_from_string_utf8, HOOK_JUMP).install()->quick();
		utils::hook(0x508BB0, r_console_text_width_hook, HOOK_JUMP).install()->quick();
		utils::hook(0x5775E0, printable_chars_count_stub, HOOK_JUMP).install()->quick();
	}
};

REGISTER_COMPONENT(localized_string)
