#pragma once
#include "loader/component_loader.hpp"
#include "utils/memory.hpp"

class command final : public component
{
public:
	static void add(const std::string& name, const std::function<void(const std::vector<std::string>&)>& callback);

	void post_load() override;
	void pre_destroy() override;

private:
	static utils::memory::allocator allocator_;
	static std::mutex mutex_;
	static std::unordered_map<std::string, std::function<void(const std::vector<std::string>&)>> callbacks_;

	static void dispatcher();
};
