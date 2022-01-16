#pragma once

#include <type_traits>

namespace NovaEngine
{
	namespace Types
	{
		template<class T, class U>
		concept DerivedFrom = std::is_base_of<U, T>::value;
	};
};
