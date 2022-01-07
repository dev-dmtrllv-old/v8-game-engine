#pragma once

#include "Transform.hpp"
#include "ComponentSystem.hpp"

namespace NovaEngine
{
	class TransformSystem : public ComponentSystem<Transform>
	{
		public:
			TransformSystem() : ComponentSystem() {}

			const char* name() { return "transform"; }
	};
};
