#include <std_include.hpp>
#include "command.hpp"
#include "utils/string.hpp"
#include "game/structs.hpp"
#include "game/game.hpp"
#include "scheduler.hpp"

utils::memory::allocator command::allocator_;
std::mutex command::mutex_;
std::unordered_map<std::string, std::function<void(const std::vector<std::string>&)>> command::callbacks_;

void command::add(const std::string& name, const std::function<void(const std::vector<std::string>&)>& callback)
{
	std::lock_guard _(mutex_);
	callbacks_[utils::string::to_lower(name)] = callback;

	const auto cmd_name = allocator_.duplicate_string(name);
	const auto cmd_function = allocator_.allocate<game::cmd_function>();

	game::Cmd_AddCommand(cmd_name, dispatcher, cmd_function, false);
}

void command::dispatcher()
{
	const auto cmd_index = game::cmd_args->nesting;
	const auto arg_count = game::cmd_args->argc[cmd_index];

	if (arg_count < 1) return;

	const auto command = utils::string::to_lower(game::cmd_args->argv[cmd_index][0]);
	const auto handler = callbacks_.find(command);
	if (handler == callbacks_.end()) return;

	std::vector<std::string> arguments;
	arguments.reserve(arg_count);

	for (auto i = 0; i < game::cmd_args->argc[cmd_index]; ++i)
	{
		arguments.emplace_back(game::cmd_args->argv[cmd_index][i]);
	}

	handler->second(arguments);
}

void command::pre_destroy()
{
	std::lock_guard _(mutex_);
	if (!callbacks_.empty())
	{
		callbacks_.clear();
	}
}

void command::post_load()
{
	command::add("crash_test", [](const std::vector<std::string>& args)
	{
		*(int*)0x0 = 1;
	});
}

REGISTER_COMPONENT(command);
