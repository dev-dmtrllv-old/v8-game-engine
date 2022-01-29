#include "Engine.hpp"
#include "Logger.hpp"
#include "TransformSystem.hpp"

#define CHECK_REJECT(subSystem, rejector, msg) if(!subSystem) { rejector(msg); Logger::get()->error(#subSystem ":" #rejector " -> " msg); return false; }

namespace NovaEngine
{
#pragma region Scripting Area
	namespace
	{
		v8::Global<v8::Promise::Resolver> configurePromiseResolver_;
		v8::Global<v8::Function> onLoadCallback_;
		v8::Global<v8::Object> configuredValue_;

		SCRIPT_METHOD(onEngineConfigure)
		{
			v8::Isolate* isolate = args.GetIsolate();

			if (args.Length() < 1)
			{

			}
			else if (args[0]->IsObject())
			{
				v8::Local<v8::Promise::Resolver> resolver = v8::Promise::Resolver::New(isolate);
				v8::Local<v8::Promise> p = resolver->GetPromise();
				args.GetReturnValue().Set(p);
				configurePromiseResolver_.Reset(isolate, resolver);
				configuredValue_.Reset(isolate, args[0]->ToObject(isolate));
			}
		}

		SCRIPT_METHOD(log)
		{
			v8::Isolate* isolate = args.GetIsolate();
			std::string buf = "[Game]  ->  ";
			for (int i = 0; i < args.Length(); i++)
			{
				auto a = args[i]->ToString(isolate);
				v8::String::Utf8Value val(a);
				buf += *val;
				if (i != args.Length() - 1)
					buf += ", ";
			}
			Logger::get()->info(buf);
		}

		SCRIPT_METHOD(emptyFunction)
		{
			args.GetReturnValue().Set(v8::Undefined(args.GetIsolate()));
		}

		// the game will call this to configure the engine
		SCRIPT_METHOD(onEngineLoad)
		{
			v8::Isolate* isolate_ = args.GetIsolate();
			v8::HandleScope handle_scope(isolate_);
			v8::Local<v8::Context> context = isolate_->GetCurrentContext();
			v8::Context::Scope context_scope(context);

			if (args.Length() < 1)
			{
				// printf("Nein Nein Nein!\n");
			}
			else if (args[0]->IsFunction())
			{
				onLoadCallback_.Reset(isolate_, v8::Local<v8::Function>::Cast(args[0]));
			}
			args.GetReturnValue().Set(v8::Undefined(isolate_));
		}

		SCRIPT_METHOD(onEngineStart)
		{
			Logger::get()->info("on engine start called!");
			if (args.Length() < 1)
			{

			}
			else if (args.Length() > 1)
			{

			}
			else if (!args[0]->IsString())
			{

			}
			else
			{
				v8::Local<v8::String> str = args[0].As<v8::String>();
				v8::String::Utf8Value sceneName = v8::String::Utf8Value(str);
				engine->start(*sceneName);
			}
		}

		SCRIPT_METHOD(onShowWindow)
		{
			engine->gameWindow.show();
		}

		SCRIPT_METHOD(onGetActiveScene)
		{
			v8::Local<v8::Object> scene = engine->sceneManager.activeScene()->jsScene(args.GetIsolate());
			args.GetReturnValue().Set(scene);
		}

		SCRIPT_METHOD(onAddComponent)
		{
			args.GetReturnValue().SetUndefined();
		}

		SCRIPT_METHOD(onGetComponent)
		{
			Entity* entity = ScriptManager::getInternalFromArgs<Entity*>(args, 1);
			Transform* transform = engine->componentManager.getComponent<Transform>(entity);
			if(transform == nullptr)
				args.GetIsolate()->ThrowException(v8::String::NewFromUtf8(args.GetIsolate(), "Transform is nullptr!"));
			args.GetReturnValue().Set(v8::String::NewFromUtf8(args.GetIsolate(), "Wooop you got a transform!"));
		}

		SCRIPT_METHOD(onRemoveComponent)
		{
			args.GetReturnValue().SetUndefined();
		}

		static void globalInitializer(ScriptManager* manager, const v8::Local<v8::Object>& global)
		{
			using namespace v8;

			Isolate* isolate = manager->isolate();
			Local<Context> ctx = isolate->GetCurrentContext();

			auto globalObject = ScriptManager::ObjectBuilder(global, isolate, ctx);

			auto engineObject = globalObject.newObject("Engine");
			engineObject.set("onLoad", onEngineLoad);
			engineObject.set("start", onEngineStart);
			engineObject.set("log", log);
			engineObject.set("getActiveScene", onGetActiveScene);

			auto windowObj = engineObject.newObject("window");
			windowObj.set("show", onShowWindow);

			ScriptManager::ClassBuilder sceneClass = ScriptManager::ClassBuilder(isolate, "Scene", 1);

			sceneClass.set("load", emptyFunction);
			sceneClass.set("start", emptyFunction);
			sceneClass.set("stop", emptyFunction);
			sceneClass.set("spawn", Scene::onSpawnGameObject);

			globalObject.set("Scene", sceneClass.build());

			ScriptManager::ClassBuilder vector2Class = ScriptManager::ClassBuilder(isolate, "Vector", 0, [](V8CallbackArgs args) {
				Isolate* isolate = args.GetIsolate();
				Local<Context> ctx = isolate->GetCurrentContext();
				Local<Object> this_ = args.This();

				size_t l = args.Length();
				if (l == 0)
				{
					this_->Set(ctx, String::NewFromUtf8(isolate, "x"), Number::New(isolate, 0));
					this_->Set(ctx, String::NewFromUtf8(isolate, "y"), Number::New(isolate, 0));
				}
				else if (l == 1)
				{
					this_->Set(ctx, String::NewFromUtf8(isolate, "x"), args[0]->ToNumber(isolate));
					this_->Set(ctx, String::NewFromUtf8(isolate, "y"), args[0]->ToNumber(isolate));
				}
				else if (l == 2)
				{
					this_->Set(ctx, String::NewFromUtf8(isolate, "x"), args[0]->ToNumber(isolate));
					this_->Set(ctx, String::NewFromUtf8(isolate, "y"), args[1]->ToNumber(isolate));
				}
			});

			globalObject.set("Vector2", vector2Class.build());

			ScriptManager::ClassBuilder gameObjectClass = ScriptManager::ClassBuilder(isolate, "GameObject", 2, [](V8CallbackArgs args) {
				Isolate* isolate = args.GetIsolate();
				Engine* engine = static_cast<Engine*>(args.Data().As<v8::External>()->Value());
				Scene* scene = engine->sceneManager.activeScene();

				if (scene == nullptr)
				{
					isolate->ThrowException(v8::String::NewFromUtf8(isolate, "There is no scene loaded!"));
					Logger::get()->error("Trying to spawn a GameObject while the active scene is nullptr!");
					return;
				}

				Local<Object> this_ = args.This();

				Entity* e = scene->spawn();
				this_->SetInternalField(0, External::New(isolate, scene));
				this_->SetInternalField(1, External::New(isolate, e));

				if (args.Length() == 0)
				{

				}
				else if (args[0]->IsString())
				{
					this_->Set(isolate->GetCurrentContext(), v8::String::NewFromUtf8(isolate, "name"), args[0]);
				}
			}, manager->engine());

			gameObjectClass.set("addComponent", onAddComponent);
			gameObjectClass.set("getComponent", onGetComponent);
			gameObjectClass.set("removeComponent", onRemoveComponent);

			globalObject.set("GameObject", gameObjectClass.build());

			ScriptManager::ClassBuilder transformClass = ScriptManager::ClassBuilder(isolate, "Transform", 2, [](V8CallbackArgs args) {
				// Isolate* isolate = args.GetIsolate();
				// Engine* engine = static_cast<Engine*>(args.Data().As<v8::External>()->Value());
				// Scene* scene = engine->sceneManager.activeScene();

				// if (scene == nullptr)
				// {
				// 	isolate->ThrowException(v8::String::NewFromUtf8(isolate, "There is no scene loaded!"));
				// 	Logger::get()->error("Trying to spawn a GameObject while the active scene is nullptr!");
				// 	return;
				// }

				// Local<Object> this_ = args.This();

				// Entity* e = scene->spawn();
				// this_->SetInternalField(0, External::New(isolate, scene));
				// this_->SetInternalField(1, External::New(isolate, e));

				// if (args.Length() == 0)
				// {

				// }
				// else if (args[0]->IsString())
				// {
				// 	this_->Set(isolate->GetCurrentContext(), v8::String::NewFromUtf8(isolate, "name"), args[0]);
				// }
			}, manager->engine());

			globalObject.set("Transform", transformClass.build());
		};
	};
#pragma endregion

	char Engine::executablePath_[PATH_MAX];

	Engine::Engine() : AbstractObject(),
		subsystemTerminateOrder_(),
		isRunning_(false),
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
			subsystemTerminateOrder_.push(std::pair(name, dynamic_cast<Terminatable*>(subSystem)));
			l->info(name, " initialized!");
			return true;
		}
	}

	bool Engine::onInitialize(const char* gameStartupScript)
	{
		Logger::get()->info("Initializing Engine...");

		CHECK(initSubSystem("Asset manager", &assetManager, executablePath()), "Failed to initialize Asset Manager!");
		CHECK(initSubSystem("Script Manager", &scriptManager, globalInitializer), "Failed to initialie Script Manager!");

		// the script should have setup a callback with Engine.onLoad([callback])
		scriptManager.load(gameStartupScript == nullptr ? "Game.js" : gameStartupScript);

		if (onLoadCallback_.IsEmpty())
		{
			Logger::get()->error("Startup Script did not provide a Engine.onload callback!");
			return false;
		}

		bool configInitialized = false;

		// the script call Engine.onLoad() to provide a configure callback
		scriptManager.run([&](const ScriptManager::RunInfo& runInfo) {
			v8::Local<v8::Object> recv = v8::Object::New(runInfo.isolate);
			v8::Local<v8::Value> argv[] = { v8::Function::New(runInfo.isolate, onEngineConfigure) };
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

		CHECK_REJECT(initSubSystem("Window Manager", &windowManager), rejectGameConfig, "Could not initialize Window Manager!");

		CHECK_REJECT(initSubSystem("Graphics Manager", &graphicsManager), rejectGameConfig, "Could not initialize Graphics Manager!");

		CHECK_REJECT(initSubSystem("Job Scheduler", &jobScheduler), rejectGameConfig, "Could not initialize Job System!");

		CHECK_REJECT(initSubSystem("Scene Manager", &sceneManager), rejectGameConfig, "Could not initialize Scene Manager!");

		CHECK_REJECT(initSubSystem("Component Manager", &componentManager), rejectGameConfig, "Could not initialize Component Manager!");

		return true;
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
			scriptManager.run([](const ScriptManager::RunInfo& runInfo) {
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

		while (!subsystemTerminateOrder_.empty())
		{
			std::pair<const char*, Terminatable*> p = subsystemTerminateOrder_.top();
			subsystemTerminateOrder_.pop();
			p.second->terminate();
			Logger::get()->info(p.first, " terminated!");
		}

		Logger::terminate();

		return true;
	}
};
