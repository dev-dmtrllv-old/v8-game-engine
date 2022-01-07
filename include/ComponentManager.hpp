#pragma once

#include "SubSystem.hpp"
#include "framework.hpp"
#include "Logger.hpp"
#include "ComponentSystem.hpp"

namespace NovaEngine
{
	class ComponentManager : public SubSystem<>
	{
	private:
		std::unordered_map<const char*, const char*> idToNameMap_;
		std::unordered_map<const char*, const char*> nameToIdMap_;
		std::unordered_map<const char*, IComponentSystem*> componentSystems_;

	public:
		ENGINE_SUB_SYSTEM_CTOR(ComponentManager) {};

	protected:
		bool onInitialize();
		bool onTerminate();

	public:
		template<typename T>
		T* registerSystem()
		{
			const char* id = typeid(T).name();
			auto nameIter = idToNameMap_.find(id);
			if (nameIter == idToNameMap_.end())
			{
				T* system = new T();
				componentSystems_.emplace(id, system);
				Logger::get()->info("Added component type ", system->name(), " -> ", id);
				idToNameMap_.emplace(id, system->name());
				nameToIdMap_.emplace(system->name(), id);
				return system;
			}
			else
			{
				const char* name = (*nameIter).second;
				return static_cast<T*>((*componentSystems_.find(name)).second);
			}
		}
	};
};
