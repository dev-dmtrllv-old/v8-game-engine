#include "ComponentManager.hpp"

#include "TransformSystem.hpp"

namespace NovaEngine
{
	bool ComponentManager::onInitialize()
	{
		// register default ComponentSystems
		registerSystem<TransformSystem>();
		return true;
	}

	bool ComponentManager::onTerminate()
	{
		return true;
	}
};
