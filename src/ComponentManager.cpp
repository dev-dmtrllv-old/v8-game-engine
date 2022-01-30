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

	void* ComponentManager::addComponent(Entity* entity, BitMask::Type bitMask)
	{
		if (entity->componentsBitMask == 0)
			entity->componentsBitMask = bitMask;
		else
			entity->componentsBitMask |= bitMask;

		std::vector<IComponentSystem*>& systems = bitMaskToSystems_[bitMask];

		for(IComponentSystem* s : systems)
			s->addEntitiy(entity);


		if (!componentEntityMap_.contains(entity))
			componentEntityMap_.emplace(entity, std::vector<EntityComponentInfo>());

		void* component = getAllocator(bitMask)->alloc();

		componentEntityMap_[entity].push_back(EntityComponentInfo(bitMask, component));

		return component;
	}
}
