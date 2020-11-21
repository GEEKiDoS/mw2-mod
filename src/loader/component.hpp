#pragma once

class component
{
public:
	virtual ~component()
	{
	}

	virtual void post_start()
	{
	}

	virtual void post_load()
	{
	}

	virtual void pre_destroy()
	{
	}

	virtual void* load_import(const std::string& library, const std::string& function)
	{
		return nullptr;
	}
};
