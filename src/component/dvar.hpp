#pragma once
#include  "game/game.hpp"
#include "loader/component_loader.hpp"

class dvar : public component
{
public:
	class flag
	{
	public:
		flag(game::dvar_flag flag) : val(flag) {};
		flag(int flag) : flag(static_cast<game::dvar_flag>(flag)) {};

		game::dvar_flag val;
	};

	class var
	{
	public:
		var() : dvar(nullptr) {};
		var(const var& obj) { this->dvar = obj.dvar; };
		var(game::dvar_t* _dvar) : dvar(_dvar) {};
		var(uint32_t ppdvar) : var(*reinterpret_cast<game::dvar_t**>(ppdvar)) {};
		var(const std::string& dvarName);

		template<typename T> T get();

		void set(char* string);
		void set(const char* string);
		void set(const std::string& string);

		void set(int integer);
		void set(float value);

		// TODO: Add others
		void set_raw(int integer);
		void set_raw(float value);

	private:
		game::dvar_t* dvar;
	};

	void post_load() override;

	// Only strings and bools use this type of declaration
	template<typename T> static var register_var(const char* name, T value, flag flag, const char* description);
	template<typename T> static var register_var(const char* name, T value, T min, T max, flag flag, const char* description);

private:
	static game::dvar_t* set_from_string_by_name_external(const char* dvar, const char* value);
	static game::dvar_t* set_from_string_by_name_safe_external(const char* dvar, const char* value);
};
