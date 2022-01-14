#pragma once

#include "framework.hpp"

namespace NovaEngine
{
	using Hash = size_t;

	struct Hasher
	{
	private:
		constexpr static size_t hashString(const char* str)
		{
			size_t hash = 5381;
			int c;

			while ((c = *str++))
				hash = ((hash << 5) + hash) * 33 + c;

			return hash;
		}

	public:
		constexpr static Hash hash(const char* str) { return hashString(str); }
		constexpr static bool check(Hash& hashStr, const char* str) { return hash(str) == hashStr; }
	};
};
