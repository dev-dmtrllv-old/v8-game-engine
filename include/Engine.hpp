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
#include "ComponentManager.hpp"
#include "EventManager.hpp"

namespace NovaEngine
{
	class Engine : public AbstractObject<const char*>
	{
	private:
		static char executablePath_[PATH_MAX];

		std::stack<std::pair<const char*, Terminatable*>> subsystemTerminateStack_;
		bool isRunning_;

		v8::Global<v8::Promise::Resolver> configurePromiseResolver_;
		v8::Global<v8::Function> onLoadCallback_;
		v8::Global<v8::Object> configuredValue_;

		Engine();

	public:
		static Engine& get()
		{
			static Engine instance_;
			return instance_;
		}

		void setOnLoadCallback(v8::Local<v8::Function> callback);
		void setConfigValue(v8::Local<v8::Object> config, v8::Local<v8::Promise::Resolver> resolver);
		
		EventManager eventManager;
		AssetManager assetManager;
		ScriptManager scriptManager;
		ConfigManager configManager;
		WindowManager windowManager;
		Graphics::GraphicsManager graphicsManager;
		JobSystem::JobScheduler jobScheduler;
		SceneManager sceneManager;
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
				subsystemTerminateStack_.push(std::pair(name, dynamic_cast<Terminatable*>(subSystem)));
				l->info(name, " initialized!");
				return true;
			}
		}

		inline bool initSubSystem(const char* name, SubSystem<>* subSystem);
	};
};
