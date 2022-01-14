#pragma once

#include "SubSystem.hpp"
#include "framework.hpp"
#include "Hash.hpp"

namespace NovaEngine
{
	class Engine;
	
	class EventManager : public SubSystem<>
	{
	public:
		struct EventData
		{
			Engine* engine;
			void* data;
		};

		using EventCallback = void(*)(EventData& data);

	private:
		std::unordered_map<Hash, std::vector<EventCallback>> eventsCallbackMap_;

	protected:
		ENGINE_SUB_SYSTEM_CTOR(EventManager), eventsCallbackMap_() {}

		bool onInitialize();
		bool onTerminate();

	public:
		void on(Hash str, EventCallback callback);

		template<typename HashType>
		void on(HashType str, EventCallback callback) { on(static_cast<Hash>(str), callback); }

		void emit(Hash str, void* data);

		template<typename T>
		void emit(Hash str, T data) { emit(str, static_cast<void*>(data)); }

		template<typename HashType, typename T>
		void emit(HashType str, T data) { emit(static_cast<Hash>(str), static_cast<void*>(data)); }
	};
};
