#pragma once

#include "SubSystem.hpp"
#include "framework.hpp"
#include "Logger.hpp"
#include "ComponentSystem.hpp"
#include "Types.hpp"
#include "BitMap.hpp"

namespace NovaEngine
{
	class ComponentManager : public SubSystem<>
	{
	public:
		class IAllocator
		{
			virtual void* alloc(size_t size);
			virtual void free(void*, size_t size);
		};

		template<typename T>
		class Allocator
		{
			virtual T* alloc(size_t size = sizeof(T)) override { return new T(); };
			virtual void free(void* ptr, size_t size) override { ::free(ptr); };
		};

	private:
		std::unordered_map<const char*, const char*> idToNameMap_;
		std::unordered_map<const char*, const char*> nameToIdMap_;
		std::unordered_map<const char*, ComponentSystem*> componentSystems_;
		std::unordered_map<size_t, IAllocator*> componentAllocators_;
		std::unordered_map<const char*, size_t> nameToComponentBit_;
		BitMap componentBitMap_;

	public:
		ENGINE_SUB_SYSTEM_CTOR(ComponentManager),
			idToNameMap_(),
			nameToIdMap_(),
			componentSystems_(),
			componentAllocators_(),
			nameToComponentBit_(),
			componentBitMap_()
		{};

	protected:
		bool onInitialize();
		bool onTerminate();

	public:
		template<Types::DerivedFrom<ComponentSystem> T>
		T* registerSystem()
		{
			const char* id = typeid(T).name();
			if(!idToNameMap_.contains(id))
			{
				T* system = new T(this);
				componentSystems_.emplace(id, system);
				idToNameMap_.emplace(id, system->name());
				nameToIdMap_.emplace(system->name(), id);
				return system;
			}
			else
			{
				return static_cast<T*>(componentSystems_[id]);
			}
		}

		template<typename T>
		const size_t getComponentBit()
		{
			const char* id = typeid(T).name();
			if(!nameToComponentBit_.contains(id))
			{
				const size_t bit = componentBitMap_.generate();
				nameToComponentBit_[id] = bit;
				return bit;
			}
			
			return nameToComponentBit_[id];
		}

		template<Types::DerivedFrom<Component> T>
		T* addComponent(Entity entity)
		{
			const char* id = typeid(T).name();
			if(nameToComponentBit_.contains(id))
			{
				const size_t bit = nameToComponentBit_[id];
				return static_cast<Allocator<T>*>(componentAllocators_[bit])->alloc();
			}
			return nullptr;
		}
	};
};
