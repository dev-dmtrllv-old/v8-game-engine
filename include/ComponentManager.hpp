#pragma once

#include "SubSystem.hpp"
#include "framework.hpp"
#include "Logger.hpp"
#include "Types.hpp"
#include "Hash.hpp"
#include "BitMask.hpp"
#include "Entity.hpp"

namespace NovaEngine
{
	class ComponentManager;

	class IComponentSystem
	{
	private:
		ComponentManager* manager_;

	public:
		const size_t bitMask;

		IComponentSystem(ComponentManager* manager, BitMask::Type bitMask) : manager_(manager), bitMask(bitMask) {};
		virtual ~IComponentSystem() = default;

		ComponentManager* manager() { return manager_; }

		virtual const char* name() = 0;
		virtual void update() = 0;

	protected:
		std::vector<Entity*> entities_;

		void addEntitiy(Entity* entity)
		{
			entities_.push_back(entity);
			Logger::get()->info("Add Entity To Component System ", name());
		}

		friend class ComponentManager;
	};


	template<typename... Ts>
	class ComponentSystem;

	class ComponentAllocator
	{
	public:
		const size_t size;
		
		ComponentAllocator(const size_t size) : size(size) {}
		
		void* alloc() { return ::malloc(size); }
		void free(void* ptr) { ::free(ptr); }
	};

	class ComponentManager : public SubSystem<>
	{
	private:
		struct EntityComponentInfo
		{
			const BitMask::Type bitMask;
			void* componentPtr;
			EntityComponentInfo(BitMask::Type bitMask, void* componentPtr) : bitMask(bitMask), componentPtr(componentPtr) {}
			template<typename T>
			EntityComponentInfo(BitMask::Type bitMask, T* componentPtr) : bitMask(bitMask), componentPtr(static_cast<void*>(componentPtr)) {}
		};

		std::unordered_map<Hash, IComponentSystem*> systems_;
		std::unordered_map<Hash, BitMask::Type> componentToBitmask_;
		std::unordered_map<Entity*, std::vector<EntityComponentInfo>> componentEntityMap_;
		std::unordered_map<Hash, ComponentAllocator*> componentAllocators_;

		BitMask::Generator bitMaskGenerator_;

		void emitComponentRegistered(Hash hash);

	public:
		ENGINE_SUB_SYSTEM_CTOR(ComponentManager),
			systems_(),
			componentToBitmask_(),
			componentEntityMap_(),
			componentAllocators_(),
			bitMaskGenerator_()
		{}

	protected:
		bool onInitialize();
		bool onTerminate();

		void registerSystem(Hash name, IComponentSystem* componentSystem);

	public:
		template<typename T>
		const BitMask::Type getComponentBitMask()
		{
			return componentToBitmask_[Hasher::hash(typeid(T).name())];
		}

		template<typename T>
		const Hash getSystemHash()
		{
			return Hasher::hash(typeid(T).name());
		}

		template<Types::DerivedFrom<IComponentSystem> T>
		T* registerSystem()
		{
			const Hash hash = Hasher::hash(typeid(T).name());

			if (!systems_.contains(hash))
				registerSystem(hash, new T(this));

			return static_cast<T*>(systems_[hash]);
		}

		template<Types::DerivedFrom<IComponentSystem> T, typename... Args>
		T* registerSystem(Args... args)
		{
			const Hash hash = Hasher::hash(typeid(T).name());

			if (!systems_.contains(hash))
				registerSystem(hash, new T(this, args...));

			return static_cast<T*>(systems_[hash]);
		}

		template<typename T>
		T* addComponent(Entity* entity)
		{
			const BitMask::Type bitMask = getComponentBitMask<T>();
			if (entity->componentsBitMask == 0)
				entity->componentsBitMask = bitMask;
			else
				entity->componentsBitMask |= bitMask;

			for (const std::pair<Hash, IComponentSystem*> p : systems_)
				if (BitMask::contains(entity->componentsBitMask, p.second->bitMask))
					p.second->addEntitiy(entity);

			if (!componentEntityMap_.contains(entity))
				componentEntityMap_.emplace(entity, std::vector<EntityComponentInfo>());

			T* component = new T();

			componentEntityMap_[entity].push_back(EntityComponentInfo(bitMask, component));

			return component;
		}

		void* getComponent(Entity* entity, const BitMask::Type bitMask)
		{
			if (componentEntityMap_.contains(entity))
			{
				const std::vector<EntityComponentInfo>& list = componentEntityMap_[entity];
				for (const EntityComponentInfo& info : list)
					if (info.bitMask == bitMask)
						return info.componentPtr;
			}

			return nullptr;
		}

		template<typename T>
		T* getComponent(Entity* entity)
		{
			return static_cast<T*>(getComponent(entity, getComponentBitMask<T>()));
		}

		template<typename ...Ts>
		BitMask::Type registerComponents()
		{
			std::vector<const char*> names = std::vector<const char*>();
			names.insert(names.end(), { typeid(Ts).name()... });

			std::vector<size_t> sizes = std::vector<size_t>();
			sizes.insert(sizes.end(), { sizeof(Ts)... });

			size_t i = 0;

			BitMask::Type mask = 0;
			for (const char* id : names)
			{
				Hash hash = Hasher::hash(id);
				if (!componentToBitmask_.contains(hash))
				{
					componentToBitmask_.emplace(hash, bitMaskGenerator_.generate());
					componentAllocators_.emplace(hash, new ComponentAllocator(sizes[i]));
					std::cout << "register " << id << std::endl;
					emitComponentRegistered(hash);
				}
				mask |= componentToBitmask_[hash];
				i++;
			}
			return mask;
		}

		ComponentAllocator* getAllocator(Hash hash)
		{
			return componentAllocators_[hash];
		}

		template<Types::DerivedFrom<IComponentSystem> T>
		T* getSystem()
		{
			Hash h = getSystemHash<T>();
			assert(systems_.contains(h));
			return static_cast<T*>(systems_[h]);
		}
	};

	template<typename... Ts>
	class ComponentSystem : public IComponentSystem
	{
	public:
		ComponentSystem(ComponentManager* manager) : IComponentSystem(manager, manager->registerComponents<Ts...>()) {};
		virtual ~ComponentSystem() = default;

		virtual const char* name() = 0;
		virtual void update() = 0;
	};
};
