#include <std_include.hpp>
#include "component_loader.hpp"

std::vector<std::unique_ptr<component>>* component_loader::components_ = nullptr;

void component_loader::register_component(std::unique_ptr<component>&& component_)
{
	if (!components_)
	{
		components_ = new std::vector<std::unique_ptr<component>>();
		atexit(destroy_components);
	}

	components_->push_back(std::move(component_));
}

bool component_loader::post_start()
{
	static auto handled = false;
	if (handled || !components_) return true;
	handled = true;

	try
	{
		for (const auto& component_ : *components_)
		{
			component_->post_start();
		}
	}
	catch (premature_shutdown_trigger&)
	{
		return false;
	}

	return true;
}

bool component_loader::post_load()
{
	static auto handled = false;
	if (handled || !components_) return true;
	handled = true;

	try
	{
		for (const auto& component_ : *components_)
		{
			component_->post_load();
		}
	}
	catch (premature_shutdown_trigger&)
	{
		return false;
	}

	return true;
}

void component_loader::pre_destroy()
{
	static auto handled = false;
	if (handled || !components_) return;
	handled = true;

	for (const auto& component_ : *components_)
	{
		component_->pre_destroy();
	}
}

void* component_loader::load_import(const std::string& library, const std::string& function)
{
	void* function_ptr = nullptr;

	for (const auto& component_ : *components_)
	{
		const auto library_function_ptr = component_->load_import(library, function);
		if (library_function_ptr)
		{
			function_ptr = library_function_ptr;
		}
	}

	return function_ptr;
}

void component_loader::destroy_components()
{
	pre_destroy();

	if (!components_) return;

	delete components_;
	components_ = nullptr;
}

void component_loader::trigger_premature_shutdown()
{
	throw premature_shutdown_trigger();
}
