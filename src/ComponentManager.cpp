#include "ComponentManager.hpp"
#include "Engine.hpp"
#include "TransformSystem.hpp"
#include "Transform.hpp"

namespace NovaEngine
{
	bool ComponentManager::onInitialize()
	{
		registerSystem<TransformSystem>();
		return true;
	}

	bool ComponentManager::onTerminate()
	{
		return true;
	}

	void ComponentManager::registerSystem(Hash hash, IComponentSystem* componentSystem)
	{
		systems_.emplace(hash, componentSystem);
	}
}
