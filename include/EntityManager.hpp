#pragma once

#include "SubSystem.hpp"
#include "framework.hpp"
#include "Entity.hpp"

namespace NovaEngine
{
	class EntityManager : public SubSystem<>
	{
	private:
		std::mutex spawnMutex_;
		Entity entityCounter_;
		std::vector<Entity> entities_;
		ENGINE_SUB_SYSTEM_CTOR(EntityManager), spawnMutex_(), entityCounter_(0), entities_() {}

	protected:
		bool onInitialize();
		bool onTerminate();

	public:
		Entity* spawn(size_t size = 1);
	};
};
