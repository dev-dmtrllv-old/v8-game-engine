#include "WindowManager.hpp"
#include "GameWindow.hpp"
#include "Engine.hpp"

namespace NovaEngine
{
	STATIC_JOB(WindowManager, pollEvents)
	{
		glfwPollEvents();
		scheduler->runJob(pollEvents, true);
		JOB_RETURN;
	}

	void WindowManager::onWindowClose(GLFWwindow* window)
	{
		GameWindow* gameWindow = reinterpret_cast<GameWindow*>(glfwGetWindowUserPointer(window));
		Engine* engine = gameWindow->engine();
		gameWindow->destroy();
		if (engine->windowManager.allWindowsClosed())
			engine->eventManager.emit(WindowManagerEvents::ALL_WINDOWS_CLOSED, nullptr);
	}

	bool WindowManager::onInitialize()
	{
		if (glfwInit() == GLFW_FALSE)
		{
			Logger::get()->error("Could not initialize GLFW!");
			return false;
		}

		Engine* e = engine();

		if (!e->gameWindow.create(e->configManager.getGameName().c_str(), e->configManager.getWindowConfig()))
		{
			Logger::get()->error("Could not create window!");
			return false;
		}

		addWindow(&(engine()->gameWindow));

		return true;
	}

	bool WindowManager::onTerminate()
	{
		glfwTerminate();
		return true;
	}

	void WindowManager::addWindow(GameWindow* window)
	{
		windows_.push_back(window);
		glfwSetWindowCloseCallback(window->glfwWindow(), onWindowClose);
	}

	bool WindowManager::allWindowsClosed()
	{
		for (auto w : windows_)
			if (!w->isClosed())
				return false;
		return true;
	}
};
