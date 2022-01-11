#include "Scene.hpp"
#include "Engine.hpp"

namespace NovaEngine
{
	Scene::Scene(Engine* engine, v8::Isolate* isolate, v8::Local<v8::Object> jsScene) : engine_(engine), jsScene_()
	{
		jsScene_.Reset(isolate, jsScene);
	}

	Engine* Scene::engine() { return engine_; }

	void Scene::load()
	{
		runJSMethod("load");
	}

	void Scene::start()
	{
		runJSMethod("start");
	}

	void Scene::stop()
	{
		runJSMethod("stop");
	}

	void Scene::runJSMethod(const char* methodName)
	{
		if (!this->jsScene_.IsEmpty())
		{
			engine_->scriptManager.run([&](const ScriptManager::RunInfo& info) {
				auto s = jsScene_.Get(info.isolate);
				s->Get(v8::String::NewFromUtf8(info.isolate, methodName)).As<v8::Function>()->Call(s, 0, nullptr);
			});
		}
	}

};
