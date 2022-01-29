#pragma once

#include "framework.hpp"
#include "Vector.hpp"

namespace NovaEngine
{
	struct Transform
	{
		Vector2 position;
		Transform* parent;
		bool isDirty;

		Transform() : position(), parent(nullptr), isDirty(false) {}
		Transform(const Transform&) = delete;
		Transform(Transform&&) = delete;
	} PACKED;
};
