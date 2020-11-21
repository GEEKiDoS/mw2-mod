#include <std_include.hpp>
#include "scheduler.hpp"
#include "utils/string.hpp"
#include "game/game.hpp"
#include "utils/hook.hpp"

std::mutex scheduler::mutex_;
std::queue<std::pair<std::string, int>> scheduler::errors_;
utils::concurrent_list<std::function<void()>> scheduler::callbacks_;
utils::concurrent_list<std::function<void()>> scheduler::single_callbacks_;

void scheduler::on_frame(const std::function<void()>& callback)
{
	std::lock_guard _(mutex_);
	callbacks_.add(callback);
}

void scheduler::once(const std::function<void()>& callback)
{
	std::lock_guard _(mutex_);
	single_callbacks_.add(callback);
}

void scheduler::error(const std::string& message, int level)
{
	std::lock_guard _(mutex_);
	errors_.emplace(message, level);
}

void scheduler::frame_stub()
{
	execute();
	reinterpret_cast<void(*)()>(0x4D3FC0)();
}

__declspec(naked) void scheduler::execute()
{
	__asm
	{
		call execute_error
		call execute_safe
		retn
	}
}

void scheduler::execute_safe()
{
	for (auto callback : callbacks_)
	{
		(*callback)();
	}

	for (auto callback : single_callbacks_)
	{
		single_callbacks_.remove(callback);
		(*callback)();
	}
}

void scheduler::execute_error()
{
	const char* message = nullptr;
	game::errorParm level = game::ERR_FATAL;

	if (get_next_error(&message, (int *)&level) && message)
	{
		game::Com_Error(level, "%s", message);
	}
}

bool scheduler::get_next_error(const char** error_message, int* error_level)
{
	std::lock_guard _(mutex_);
	if (errors_.empty())
	{
		*error_message = nullptr;
		return false;
	}

	const auto error = errors_.front();
	errors_.pop();

	*error_level = error.second;
	*error_message = utils::string::va("%s", error.first.data());

	return true;
}

void scheduler::post_load()
{
	utils::hook(0x49C3AF, frame_stub, HOOK_CALL).install()->quick();
}

void scheduler::pre_destroy()
{
	std::lock_guard _(mutex_);
	callbacks_.clear();
	single_callbacks_.clear();
}

REGISTER_COMPONENT(scheduler);
