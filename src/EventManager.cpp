#include "EventManager.hpp"

namespace NovaEngine
{
	void EventManager::on(Hash eventName, EventCallback callback)
	{
		if (!eventsCallbackMap_.contains(eventName))
			eventsCallbackMap_.emplace(eventName, std::vector<EventCallback>({ callback }));
		else
			eventsCallbackMap_[eventName].push_back(callback);
	}

	void EventManager::emit(Hash eventName, void* data)
	{
		if (eventsCallbackMap_.contains(eventName))
		{
			EventData ed;
			ed.data = data;
			ed.engine = engine();
			for (const EventCallback& cb : eventsCallbackMap_[eventName])
				cb(ed);
		}
	}

	bool EventManager::onInitialize()
	{
		return true;
	}

	bool EventManager::onTerminate()
	{
		return true;
	}
};
