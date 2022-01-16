#pragma once

#include "Entity.hpp"

#define ENGINE_COMPONENT_CTOR(name) name(ComponentManager* manager) : ComponentSystem(manager)

namespace NovaEngine
{
	class ComponentManager;

	struct Component
	{
		const Entity entity;
		Component(Entity entity) : entity(entity) {}
		virtual ~Component() = default;
	};

	class ComponentSystem
	{
	private:
		ComponentManager* manager_;

	public:
		ComponentSystem(ComponentManager* manager);
		virtual ~ComponentSystem() = default;

		virtual const char* name() = 0;
		virtual void update() = 0;
		virtual void onInitialize() = 0;

		ComponentManager* manager();
	};
};
