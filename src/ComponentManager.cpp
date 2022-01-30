#include "ComponentManager.hpp"
#include "Engine.hpp"
#include "TransformSystem.hpp"
#include "Transform.hpp"
#include "graphics/GameRenderer.hpp"

namespace NovaEngine
{
	bool ComponentManager::onInitialize()
	{
		registerSystem<TransformSystem>();
		registerSystem<Graphics::GameRenderer>(&engine()->gameWindow);
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

	void ComponentManager::emitComponentRegistered(Hash hash)
	{
		engine()->eventManager.emit(Hasher::hash("componentRegistered"), reinterpret_cast<void*>(hash));
	}
}
