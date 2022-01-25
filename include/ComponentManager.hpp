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

	class ComponentManager : public SubSystem<>
	{
	private:
		std::unordered_map<Hash, IComponentSystem*> systems_;
		std::unordered_map<Hash, BitMask::Type> componentToBitmask_;
		BitMask::Generator bitMaskGenerator_;

	public:
		ENGINE_SUB_SYSTEM_CTOR(ComponentManager),
			systems_(),
			bitMaskGenerator_()
		{}

	protected:
		bool onInitialize();
		bool onTerminate();

		void registerSystem(Hash name, IComponentSystem* componentSystem);

		template<typename T>
		Hash componentToHash()
		{
			return Hasher::hash(typeid(T).name());
		}

		template<typename T>
		BitMask::Type componentToBitMask()
		{
			return componentToBitmask_[componentToHash<T>()];
		}

	public:
		template<Types::DerivedFrom<IComponentSystem> T>
		T* registerSystem()
		{
			const char* id = typeid(T).name();
			const Hash hash = Hasher::hash(id);

			std::string hashString = std::to_string(hash);



			if (!systems_.contains(hash))
				registerSystem(hash, new T(this));

			std::string bitMaskString = std::to_string(static_cast<T*>(systems_[hash])->bitMask);

			Logger::get()->info("Registering Component-System with ID: ", id, " Hash: ", hashString.c_str(), " BitMask ", bitMaskString.c_str());
			return static_cast<T*>(systems_[hash]);
		}

		template<typename T>
		T* addComponent(Entity* entity)
		{
			if (entity->componentsBitMask == 0)
				entity->componentsBitMask = componentToBitMask<T>();
			else
				entity->componentsBitMask |= componentToBitMask<T>();
			
			for (const std::pair<const Entity::IDCounter, IComponentSystem*> p : systems_)
				if (p.second->bitMask & ~entity->componentsBitMask)
					p.second->addEntitiy(entity);
			
			return new T();
		}

		template<typename ...Ts>
		BitMask::Type getComponentBitMask()
		{
			std::vector<const char*> names = std::vector<const char*>();
			names.insert(names.end(), { typeid(Ts).name()... });
			BitMask::Type mask = 0;
			for (const char* id : names)
			{
				Hash hash = Hasher::hash(id);
				if (!componentToBitmask_.contains(hash))
				{
					std::string hashString = std::to_string(hash);
					Logger::get()->info("Registering Component-Data-Struct with ID: ", id, " Hash: ", hashString.c_str());
					componentToBitmask_.emplace(hash, bitMaskGenerator_.generate());
				}
				mask += componentToBitmask_[hash];
			}
			return mask;
		}
	};

	template<typename... Ts>
	class ComponentSystem : public IComponentSystem
	{
	public:
		ComponentSystem(ComponentManager* manager) : IComponentSystem(manager, manager->getComponentBitMask<Ts...>()) {};
		virtual ~ComponentSystem() = default;

		virtual const char* name() = 0;
		virtual void update() = 0;
	};
};
