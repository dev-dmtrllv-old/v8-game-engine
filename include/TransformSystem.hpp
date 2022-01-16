#pragma once

#include "ComponentSystem.hpp"

namespace NovaEngine
{
	class TransformSystem : public ComponentSystem
	{
		public:
			ENGINE_COMPONENT_CTOR(TransformSystem) {}
			~TransformSystem();

			virtual const char* name() override;

			virtual void onInitialize() override;

			virtual void update() override;
	};
};
