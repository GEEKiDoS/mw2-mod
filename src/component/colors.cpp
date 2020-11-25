#include <std_include.hpp>
#include "loader/component_loader.hpp"
#include "utils/hook.hpp"
#include "game/game.hpp"

namespace
{
	static unsigned int color_table[]
	{
		RGB(0, 0, 0),
		RGB(255, 49, 49),
		RGB(134, 192, 0),
		RGB(255, 173, 34),
		RGB(0, 135, 193),
		RGB(32, 197, 255),
		RGB(255, 1, 1),
		RGB(255, 255, 255),
		RGB(67, 96, 0),
		RGB(128, 128, 128),
	};

	void cl_lookup_color(unsigned int* color, unsigned char code)
	{
		unsigned char lookup = code - '0';

		if (lookup > (sizeof(color_table) / 4))
			lookup = 7;

		*color = color_table[lookup];
	}

	// Why you do that compiler?
	__declspec(naked) void cl_lookup_color_stub()
	{
		__asm 
		{
			push ebx
			mov ebx, [esp + 8]
			push ebx
			push esi
			call cl_lookup_color
			add esp, 8
			pop ebx
			retn
		}
	}
}

class colors final : public component
{
public:
	void post_load() override
	{
		utils::hook(0x537EB0, cl_lookup_color_stub).install()->quick();
	}
};

REGISTER_COMPONENT(colors)
