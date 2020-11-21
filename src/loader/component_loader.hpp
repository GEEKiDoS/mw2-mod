#pragma once
#include "component.hpp"

class component_loader final
{
public:
	class premature_shutdown_trigger final : public std::exception
	{
		const char* what() const noexcept override
		{
			return "Premature shutdown requested";
		}
	};

	template <typename T>
	class installer final
	{
		static_assert(std::is_base_of<component, T>::value, "Module has invalid base class");

	public:
		installer()
		{
			register_component(std::make_unique<T>());
		}
	};

	template <typename T>
	static T* get()
	{
		for (const auto& component_ : *components_)
		{
			if (typeid(*component_.get()) == typeid(T))
			{
				return reinterpret_cast<T*>(component_.get());
			}
		}

		return nullptr;
	}

	static void register_component(std::unique_ptr<component>&& component);

	static bool post_start();
	static bool post_load();
	static void pre_destroy();

	static void* load_import(const std::string& library, const std::string& function);

	static void trigger_premature_shutdown();

private:
	static std::vector<std::unique_ptr<component>>* components_;

	static void destroy_components();
};

#define REGISTER_COMPONENT(name)                       \
namespace                                           \
{                                                   \
	static component_loader::installer<name> $_##name; \
}
