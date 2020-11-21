#include <std_include.hpp>
#include "loader/component_loader.hpp"
#include "game/game.hpp"
#include "utils/hook.hpp"

class security final : public component
{
public:
	void post_load() override
	{
		if (game::is_mp())
		{
			utils::hook(0x4AECD4, read_p2p_auth_ticket_stub, HOOK_JUMP).install()->quick();
		}
	}

private:
	static void read_p2p_auth_ticket_stub(game::native::msg_t* msg, void* data, const int len)
	{
		if (len < 0) return;
		return game::native::MSG_ReadData(msg, data, std::min(len, 200));
	}
};

REGISTER_COMPONENT(security)
