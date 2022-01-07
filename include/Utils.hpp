#pragma once

#include "framework.hpp"

namespace NovaEngine
{
	namespace Utils
	{
		namespace Path
		{
			template<typename T, typename T2>
			std::filesystem::path combine(T p, T2 p2)
			{
				std::filesystem::path path = p;
				path /= p2;
				return path;
			}

			template<typename T, typename... Parts>
			std::filesystem::path combine(T p, Parts... parts)
			{
				std::filesystem::path path = p;
				path /= combine(parts...);
				return path;
			}
		}
	}
}
