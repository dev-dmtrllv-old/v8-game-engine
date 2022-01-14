#pragma once

#include "framework.hpp"
#include "ScriptManager.hpp"
#include "ConfigManager.hpp"
#include "AbstractObject.hpp"
#include "AssetManager.hpp"
#include "GameWindow.hpp"
#include "WindowManager.hpp"
#include "Logger.hpp"
#include "job_system/JobScheduler.hpp"
#include "graphics/GraphicsManager.hpp"
#include "SceneManager.hpp"
#include "EntityManager.hpp"
#include "ComponentManager.hpp"
#include "EventManager.hpp"

namespace NovaEngine
{
	class Engine : public AbstractObject<const char*>
	{
	private:
		static char executablePath_[PATH_MAX];

		std::stack<std::pair<const char*, Terminatable*>> subsystemTerminateOrder_;
		bool isRunning_;

		Engine();

	public:
		static Engine& get()
		{
			static Engine instance_;
			return instance_;
		}

		EventManager eventManager;
		AssetManager assetManager;
		ScriptManager scriptManager;
		ConfigManager configManager;
		WindowManager windowManager;
		Graphics::GraphicsManager graphicsManager;
		JobSystem::JobScheduler jobScheduler;
		SceneManager sceneManager;
		EntityManager entityManager;
		ComponentManager componentManager;
		GameWindow gameWindow;

		static const char* executablePath();

		bool isRunning();

		void run();

		/* gets called from the game to start and stop the engine */
		void start(const char* startSceneName);

		/* gets called from the game to start and stop the engine */
		void stop();

	protected:

		bool onInitialize(const char*);
		bool onTerminate();

	private:
		template<typename... Args>
		bool initSubSystem(const char* name, SubSystem<Args...>* subSystem, Args... args)
		{
			Logger* l = Logger::get();

			l->info("Initializing ", name, "...");
			if (!subSystem->initialize(args...))
			{
				l->error("Failed to initialize ", name, "!");
				return false;
			}
			else
			{
				subsystemTerminateOrder_.push(std::pair(name, dynamic_cast<Terminatable*>(subSystem)));
				l->info(name, " initialized!");
				return true;
			}
		}

		inline bool initSubSystem(const char* name, SubSystem<>* subSystem);
	};
};
