#include "Engine.hpp"
#include "Logger.hpp"
#include "TransformSystem.hpp"
#include "js_wrappers/Wrappers.hpp"

#define CHECK_REJECT(subSystem, rejector, msg) if(!subSystem) { rejector(msg); Logger::get()->error(#subSystem ":" #rejector " -> " msg); return false; }

namespace NovaEngine
{
	char Engine::executablePath_[PATH_MAX];

	Engine::Engine() : AbstractObject(),
		subsystemTerminateStack_(),
		isRunning_(false),
		configurePromiseResolver_(),
		onLoadCallback_(),
		configuredValue_(),
		eventManager(this),
		assetManager(this),
		scriptManager(this),
		configManager(this),
		windowManager(this),
		graphicsManager(this),
		jobScheduler(this),
		sceneManager(this),
		componentManager(this),
		gameWindow(this)
	{
	}

	bool Engine::initSubSystem(const char* name, SubSystem<>* subSystem)
	{
		Logger* l = Logger::get();

		l->info("Initializing ", name, "...");
		if (!subSystem->initialize())
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

	bool Engine::onInitialize(const char* gameStartupScript)
	{
		Logger::get()->info("Initializing Engine...");

		CHECK(initSubSystem("Asset manager", &assetManager, executablePath()), "Failed to initialize Asset Manager!");
		CHECK(initSubSystem("Script Manager", &scriptManager), "Failed to initialie Script Manager!");

		// the script should have setup a callback with Engine.onLoad([callback])
		scriptManager.load(gameStartupScript == nullptr ? "Game.js" : gameStartupScript);

		if (onLoadCallback_.IsEmpty())
		{
			Logger::get()->error("Startup Script did not provide a Engine.onload callback!");
			return false;
		}

		bool configInitialized = false;

		// lets run the configure function
		scriptManager.run([&](const ScriptManager::RunInfo& runInfo) {
			v8::Local<v8::Object> recv = v8::Object::New(runInfo.isolate);
			v8::Local<v8::Value> argv[] = { v8::Function::New(runInfo.isolate, JsWrappers::JsEngine::onConfigure) };
			onLoadCallback_.Get(runInfo.isolate)->Call(recv, 1, argv)->ToObject(runInfo.isolate);
		});

		auto rejectGameConfig = [&](const char* message) {
			scriptManager.run([&](const ScriptManager::RunInfo& runInfo) {
				v8::Local<v8::String> reason = v8::String::NewFromUtf8(runInfo.isolate, message, v8::NewStringType::kNormal).ToLocalChecked();
				configurePromiseResolver_.Get(runInfo.isolate)->Reject(reason);
			});
		};

		// now we wait till the callback from Engine.onLoad is done running
		// check if the script called the configure function 
		if (!configuredValue_.IsEmpty())
			configInitialized = initSubSystem("Config Manager", &configManager, &configuredValue_);

		CHECK_REJECT(configInitialized, rejectGameConfig, "Could not initialize Config Manager!");
		
		CHECK_REJECT(initSubSystem("Component Manager", &componentManager), rejectGameConfig, "Could not initialize Component Manager!");

		CHECK_REJECT(initSubSystem("Window Manager", &windowManager), rejectGameConfig, "Could not initialize Window Manager!");

		CHECK_REJECT(initSubSystem("Graphics Manager", &graphicsManager), rejectGameConfig, "Could not initialize Graphics Manager!");

		CHECK_REJECT(initSubSystem("Job Scheduler", &jobScheduler), rejectGameConfig, "Could not initialize Job System!");

		CHECK_REJECT(initSubSystem("Scene Manager", &sceneManager), rejectGameConfig, "Could not initialize Scene Manager!");


		return true;
	}


	void Engine::setOnLoadCallback(v8::Local<v8::Function> callback)
	{
		if (onLoadCallback_.IsEmpty())
			onLoadCallback_.Reset(scriptManager.isolate(), callback);

	}

	void Engine::setConfigValue(v8::Local<v8::Object> config, v8::Local<v8::Promise::Resolver> resolver)
	{
		v8::Isolate* isolate = scriptManager.isolate();
		configurePromiseResolver_.Reset(isolate, resolver);
		configuredValue_.Reset(isolate, config);
	}

	const char* Engine::executablePath()
	{
		if (executablePath_[0] == '\0')
		{
			int exePathLength = readlink("/proc/self/exe", executablePath_, PATH_MAX);
			if (exePathLength == -1)
			{
				Logger::get()->error("Could not set executablePath_!");
			}
			else
			{
				for (size_t i = exePathLength; i > 0; --i)
				{
					if (executablePath_[i] == '/')
					{
						executablePath_[i] = '\0';
						break;
					}
				}
			}
		}

		return executablePath_;
	}

	void Engine::run()
	{
		if (!configurePromiseResolver_.IsEmpty()) // probably first time started (with configuration passed) 
		{
			scriptManager.run([&](const ScriptManager::RunInfo& runInfo) {
				configurePromiseResolver_.Get(runInfo.isolate)->Resolve(v8::Local<v8::Value>(v8::Undefined(runInfo.isolate)));
			});
			configurePromiseResolver_.Reset();
		}
	}

	bool Engine::isRunning()
	{
		return isRunning_;
	}

	void Engine::start(const char* startSceneName)
	{
		if (!isRunning_)
		{
			Logger::get()->info("starting engine with scene ", startSceneName, "...");

			eventManager.on(WindowManagerEvents::ALL_WINDOWS_CLOSED, [](EventManager::EventData& data) {
				Logger::get()->info("All windows closed event callback!");
				data.engine->isRunning_ = false;
			});

			isRunning_ = true;

			sceneManager.loadScene(startSceneName);

			JobSystem::JobInfo jobs[1] = {
				{ WindowManager::pollEvents },
			};

			jobScheduler.runJobs(jobs, 1, true);

			jobScheduler.exec([&] { return isRunning_; });

			isRunning_ = false;
		}
	}

	void Engine::stop()
	{
		if (isRunning_)
		{
			isRunning_ = false;
		}
	}

	bool Engine::onTerminate()
	{
		configurePromiseResolver_.Reset();
		onLoadCallback_.Reset();
		configuredValue_.Reset();

		while (!subsystemTerminateStack_.empty())
		{
			std::pair<const char*, Terminatable*> p = subsystemTerminateStack_.top();
			subsystemTerminateStack_.pop();
			p.second->terminate();
			Logger::get()->info(p.first, " terminated!");
		}

		Logger::terminate();

		return true;
	}
};
