#include <std_include.hpp>
#include "component/command.hpp"
#include "loader/component_loader.hpp"
#include "utils/hook.hpp"
#include "game/game.hpp"

namespace
{
	game::Symbol<int> show_console{ 0x929140 };

	void toggle_console()
	{
		// possibly cls.keyCatchers?
		*show_console ^= 1;

		// g_consoleField
		game::Field_Clear(reinterpret_cast<void*>(0x88C700));

		// show console output?
		utils::hook::set(0x886FC8, 0);
	}
}

class game_console final : public component
{	
public:
	void post_load() override
	{
		utils::hook(0x44317E, toggle_console, HOOK_CALL).install()->quick();
		utils::hook(0x442E8E, toggle_console, HOOK_JUMP).install()->quick();

		command::add("toggleconsole", [](const std::vector<std::string> &args) 
		{
			toggle_console();
		});
	}
};

REGISTER_COMPONENT(game_console)
