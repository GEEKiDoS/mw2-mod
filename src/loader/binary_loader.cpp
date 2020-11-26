#include <std_include.hpp>
#include "binary_loader.hpp"
#include "utils/nt.hpp"
#include "utils/io.hpp"
#include "utils/cryptography.hpp"
#include "utils/string.hpp"
#include "utils/compression.hpp"

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
		return utils::compression::zstd::decompress(load_resource(BINARY_SP_ZSTD));
	}
}
