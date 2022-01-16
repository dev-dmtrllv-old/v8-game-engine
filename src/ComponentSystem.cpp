#include "ComponentSystem.hpp"
#include "ComponentManager.hpp"

namespace NovaEngine
{
	ComponentSystem::ComponentSystem(ComponentManager* manager) : manager_(manager) {}

	ComponentManager* ComponentSystem::manager() { return manager_; }
};
