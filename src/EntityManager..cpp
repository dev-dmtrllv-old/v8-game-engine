#include "EntityManager.hpp"

namespace NovaEngine
{
	bool EntityManager::onInitialize()
	{
		return true;
	}

	bool EntityManager::onTerminate()
	{
		return true;
	}

	Entity* EntityManager::spawn(size_t t)
	{
		assert(t > 0);
		std::lock_guard<std::mutex> guard(spawnMutex_);
		const size_t index = entities_.size();
		for(size_t i = 0; i < t; i++)
			entities_.push_back(Entity(entityCounter_++));
		return &entities_[index];
	}
};
