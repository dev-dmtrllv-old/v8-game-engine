#include "graphics/GraphicsManager.hpp"
#include "graphics/Renderer.hpp"
#include "GameWindow.hpp"
#include "Engine.hpp"
#include "Logger.hpp"

namespace NovaEngine::Graphics
{
	bool GraphicsManager::onInitialize()
	{
		setRenderer(&(engine()->gameWindow));

		engine()->eventManager.on(Hasher::hash("windowCreated"), [](EventManager::EventData& args) {
			args.engine->graphicsManager.setRenderer(static_cast<GameWindow*>(args.data));
		});

		return true;
	}

	bool GraphicsManager::onTerminate()
	{
		for(std::pair<GameWindow*, Renderer*> p : renderers_)
			if(!p.second->isTerminated() && !p.second->terminate())
				Logger::get()->error("Could not reminate renderer for window ", p.first->title());
		
		return true;
	}

	void GraphicsManager::setRenderer(GameWindow* w)
	{
		if(!renderers_.contains(w))
		{
			renderers_.emplace(w, new Renderer(w));
		}
	}
};
