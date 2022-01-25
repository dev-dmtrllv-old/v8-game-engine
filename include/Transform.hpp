#pragma once

#include "Vector.hpp"

namespace NovaEngine
{
	struct Transform
	{
		Vector2 position;

		Transform() : position() {}
		Transform(const Transform&) = delete;
	};
};
