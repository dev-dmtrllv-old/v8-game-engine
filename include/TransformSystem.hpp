#pragma once

#include "ComponentManager.hpp"
#include "Transform.hpp"

namespace NovaEngine
{
	class TransformSystem : public ComponentSystem<Transform>
	{
		public:
			TransformSystem(ComponentManager* manager);
			~TransformSystem();

			virtual const char* name() override;
			virtual void update() override;
	};
};
