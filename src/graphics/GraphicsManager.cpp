#include "graphics/GraphicsManager.hpp"
#include "GameWindow.hpp"
#include "Engine.hpp"
#include "Logger.hpp"

namespace NovaEngine::Graphics
{
	bool GraphicsManager::onInitialize()
	{
		engine()->eventManager.on(Hasher::hash("windowCreated"), [](EventManager::EventData& args) {
			
		});

		return true;
	}

	bool GraphicsManager::onTerminate()
	{	
		return true;
	}
};
