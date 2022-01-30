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
		std::unordered_map<BitMask::Type, ComponentAllocator*> componentAllocators_;
		std::unordered_map<BitMask::Type, std::vector<IComponentSystem*>> bitMaskToSystems_;

		BitMask::Generator bitMaskGenerator_;

		void emitComponentRegistered(Hash hash);

	public:
		ENGINE_SUB_SYSTEM_CTOR(ComponentManager),
			systems_(),
			componentToBitmask_(),
			componentEntityMap_(),
			componentAllocators_(),
			bitMaskToSystems_(),
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
			return getComponentBitMask(Hasher::hash(typeid(T).name()));
		}

		const BitMask::Type getComponentBitMask(Hash hash)
		{
			return componentToBitmask_[hash];
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

		void* addComponent(Entity* entity, BitMask::Type bitMask);

		template<typename T>
		T* addComponent(Entity* entity)
		{
			const BitMask::Type bitMask = getComponentBitMask<T>();
			return static_cast<T*>(addComponent(entity, bitMask));
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

		template<typename ...Ts>
		BitMask::Type registerComponents(IComponentSystem* componentSystem)
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
					BitMask::Type bitMask = bitMaskGenerator_.generate();
					componentToBitmask_.emplace(hash, bitMask);
					componentAllocators_.emplace(bitMask, new ComponentAllocator(sizes[i]));
					
					if(!bitMaskToSystems_.contains(bitMask))
						bitMaskToSystems_.emplace(bitMask, std::vector<IComponentSystem*>());

					bitMaskToSystems_[bitMask].push_back(componentSystem);

					Logger::get()->info("register ", hash, " id ", id, " bitMask ", bitMask);
					
					emitComponentRegistered(hash);
				}
				mask |= componentToBitmask_[hash];
				i++;
			}
			return mask;
		}

		ComponentAllocator* getAllocator(BitMask::Type bitMask)
		{
			return componentAllocators_[bitMask];
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
		ComponentSystem(ComponentManager* manager) : IComponentSystem(manager, manager->registerComponents<Ts...>(this)) {};
		virtual ~ComponentSystem() = default;

		virtual const char* name() = 0;
		virtual void update() = 0;
	};
};
