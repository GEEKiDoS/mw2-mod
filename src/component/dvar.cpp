#include "std_include.hpp"

#include "utils/hook.hpp"
#include "utils/string.hpp"

#include "dvar.hpp"

dvar::var::var(const std::string& dvarName) : var()
{
	this->dvar = game::Dvar_FindVar(dvarName.data());

	if (!this->dvar)
	{
		// Quick-register the dvar
		game::Dvar_SetStringByName(dvarName.data(), "");
		this->dvar = game::Dvar_FindVar(dvarName.data());
	}
}

template <> game::dvar_t* dvar::var::get()
{
	return this->dvar;
}

template <> const char* dvar::var::get()
{
	if (this->dvar && this->dvar->type == game::dvar_type::DVAR_TYPE_STRING && this->dvar->current.string)
	{
		return const_cast<const char*>(this->dvar->current.string);
	}

	return "";
}

template <> char* dvar::var::get()
{
	return const_cast<char*>(this->get<const char*>());
}

template <> int dvar::var::get()
{
	if (this->dvar && this->dvar->type == game::dvar_type::DVAR_TYPE_INT)
	{
		return this->dvar->current.integer;
	}

	return 0;
}

template <> unsigned int dvar::var::get()
{
	return static_cast<unsigned int>(this->get<int>());
}

template <> float dvar::var::get()
{
	if (this->dvar && this->dvar->type == game::dvar_type::DVAR_TYPE_FLOAT)
	{
		return this->dvar->current.value;
	}

	return 0;
}

template <> float* dvar::var::get()
{
	static float val[4] = { 0 };

	if (this->dvar && (this->dvar->type == game::dvar_type::DVAR_TYPE_FLOAT_2 || this->dvar->type == game::dvar_type::DVAR_TYPE_FLOAT_3 || this->dvar->type == game::dvar_type::DVAR_TYPE_FLOAT_4))
	{
		return this->dvar->current.vector;
	}

	return val;
}

template <> bool dvar::var::get()
{
	if (this->dvar && this->dvar->type == game::dvar_type::DVAR_TYPE_BOOL)
	{
		return this->dvar->current.enabled;
	}

	return false;
}

template <> std::string dvar::var::get()
{
	return this->get<const char*>();
}

void dvar::var::set(char* string)
{
	this->set(const_cast<const char*>(string));
}

void dvar::var::set(const char* string)
{
	if (this->dvar && this->dvar->name)
	{
		game::Dvar_SetCommand(this->dvar->name, string);
	}
}

void dvar::var::set(const std::string& string)
{
	this->set(string.data());
}

void dvar::var::set(int integer)
{
	if (this->dvar && this->dvar->name)
	{
		game::Dvar_SetCommand(this->dvar->name, utils::string::va("%i", integer));
	}
}

void dvar::var::set(float value)
{
	if (this->dvar && this->dvar->name)
	{
		game::Dvar_SetCommand(this->dvar->name, utils::string::va("%f", value));
	}
}

void dvar::var::setRaw(int integer)
{
	if (this->dvar)
	{
		this->dvar->current.integer = integer;
	}
}

void dvar::var::setRaw(float value)
{
	if (this->dvar)
	{
		this->dvar->current.value = value;
	}
}

template<> static dvar::var dvar::register_var(const char* name, bool value, dvar::flag flag, const char* description)
{
	return game::Dvar_RegisterBool(name, value, flag.val, description);
}

template<> static dvar::var dvar::register_var(const char* name, const char* value, dvar::flag flag, const char* description)
{
	return game::Dvar_RegisterString(name, value, flag.val, description);
}

template<> static dvar::var dvar::register_var(const char* name, int value, int min, int max, dvar::flag flag, const char* description)
{
	return game::Dvar_RegisterInt(name, value, min, max, flag.val, description);
}

game::dvar_t* dvar::set_from_string_by_name_safe_external(const char* dvar, const char* value)
{
	static const char* exceptions[] =
	{
		"ui_showEndOfgame",
		"systemlink",
		"splitscreen",
		"onlinegame",
		"party_maxplayers",
		"xblive_privateserver",
		"xblive_rankedmatch",
		"ui_mptype",
	};

	for (int i = 0; i < ARRAYSIZE(exceptions); ++i)
	{
		if (utils::string::to_lower(dvar) == utils::string::to_lower(exceptions[i]))
		{
			return game::Dvar_SetFromStringByName(dvar, value);
		}
	}

	return dvar::set_from_string_by_name_external(dvar, value);
}

game::dvar_t* dvar::set_from_string_by_name_external(const char* dvar, const char* value)
{
	return game::Dvar_SetFromStringByNameFromSource(dvar, value, game::DvarSetSource::DVAR_SOURCE_EXTERNAL);
}

void dvar::post_load()
{
	// flag cg_fov as saved
	utils::hook::set(0x41ED35, (uint8_t)game::DVAR_FLAG_SAVED);
}

REGISTER_COMPONENT(dvar);