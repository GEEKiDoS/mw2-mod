#include <std_include.hpp>
#include "game.hpp"

namespace game
{
	namespace enviroment
	{
		launcher::mode mode = launcher::mode::none;

		launcher::mode get_mode()
		{
			if (mode == launcher::mode::none)
			{
				throw std::runtime_error("Launcher mode not valid. Something must be wrong.");
			}

			return mode;
		}

		bool is_mp()
		{
			return get_mode() == launcher::mode::multiplayer;
		}

		bool is_sp()
		{
			return get_mode() == launcher::mode::singleplayer;
		}

		bool is_dedi()
		{
			return get_mode() == launcher::mode::server;
		}

		void initialize(const launcher::mode _mode)
		{
			mode = _mode;
		}
	}

	Glyph* R_GetCharacterGlyph(Font_s* font, unsigned int letter)
	{
		Glyph* result = nullptr;

		if (letter < 0x20 || letter > 0x7F)
		{
			int top = font->glyphCount - 1;
			int bottom = 96;
			while (bottom <= top)
			{
				int mid = (bottom + top) / 2;
				if (font->glyphs[mid].letter == letter)
					return &font->glyphs[mid];
				if (font->glyphs[mid].letter >= letter)
					top = mid - 1;
				else
					bottom = mid + 1;
			}
			result = font->glyphs + 14;
		}
		else
		{
			result = &font->glyphs[letter - 32];
		}
		return result;
	}

	int R_LetterWidth(unsigned int letter, Font_s* font)
	{
		return R_GetCharacterGlyph(font, letter)->dx;
	}
}
