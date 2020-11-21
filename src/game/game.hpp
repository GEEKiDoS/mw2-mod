#pragma once

#include "structs.hpp"
#include "launcher/launcher.hpp"

#define SELECT_VALUE(sp, mp, dedi) (game::is_sp() ? (sp) : (game::is_mp() ? (mp) : (dedi)))
#define WEAK __declspec(selectany)

namespace game
{
	namespace enviroment
	{
		bool is_mp();
		bool is_sp();
		bool is_dedi();

		void initialize(launcher::mode mode);
	}

	template <typename T>
	class Symbol
	{
	public:
		Symbol(const size_t sp_address)
			: sp_object_(reinterpret_cast<T*>(sp_address))
		{
		}

		operator T* () const
		{
			return sp_object_;
		}

		T* operator->() const
		{
			return this->operator T * ();
		}

	private:
		T* sp_object_;
	};

	WEAK Symbol<void(errorParm code, const char* message, ...)> Com_Error{ 0x43DD90 };
	WEAK Symbol<void(const char* message)> Conbuf_AppendText{ 0x4F32D0 };
	WEAK Symbol<void()> Sys_ShowConsole{ 0x42C830 };
}
