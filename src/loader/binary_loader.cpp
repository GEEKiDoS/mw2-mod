#include <std_include.hpp>
#include "binary_loader.hpp"
#include "utils/nt.hpp"
#include "utils/io.hpp"
#include "utils/cryptography.hpp"
#include "utils/string.hpp"
#include "utils/compression.hpp"

#define DEDI_HASH "F271C305117B79242E254E9F64BD5AA2993CAC8E57975243EBD44CD576418D20"

namespace binary_loader
{
	std::string load_resource(const int id)
	{
		const auto res = FindResource(::utils::nt::library(), MAKEINTRESOURCE(id), RT_RCDATA);
		if (!res) return {};

		const auto handle = LoadResource(nullptr, res);
		if (!handle) return {};

		return std::string(LPSTR(LockResource(handle)), SizeofResource(nullptr, res));
	}

	std::string load(const launcher::mode mode)
	{
		return load_resource(BINARY_SP);
	}
}
