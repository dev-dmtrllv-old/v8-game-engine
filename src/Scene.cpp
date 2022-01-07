#include "Scene.hpp"
#include "Engine.hpp"

namespace NovaEngine
{
	Scene::Scene(Engine* engine, v8::Isolate* isolate, v8::Local<v8::Object> jsScene) : engine_(engine), loadFunc_(), startFunc_(), stopFunc_()
	{
		loadFunc_.Reset(isolate, jsScene->Get(v8::String::NewFromUtf8(isolate, "load")).As<v8::Function>());
		startFunc_.Reset(isolate, jsScene->Get(v8::String::NewFromUtf8(isolate, "start")).As<v8::Function>());
		stopFunc_.Reset(isolate, jsScene->Get(v8::String::NewFromUtf8(isolate, "stop")).As<v8::Function>());
	}

	Engine* Scene::engine() { return engine_; }


	void Scene::load()
	{
		if (!this->loadFunc_.IsEmpty())
		{
			engine_->scriptManager.run([&](const ScriptManager::RunInfo& info) {

				v8::Local<v8::Object> recv = v8::Object::New(info.isolate);
				this->loadFunc_.Get(info.isolate)->CallAsFunction(info.context, recv, 0, nullptr);
			});
		}
	}

	void Scene::start()
	{
		if (!this->startFunc_.IsEmpty())
		{
			engine_->scriptManager.run([&](const ScriptManager::RunInfo& info) {

				v8::Local<v8::Object> recv = v8::Object::New(info.isolate);
				this->startFunc_.Get(info.isolate)->CallAsFunction(info.context, recv, 0, nullptr);
			});
		}
	}

	void Scene::stop()
	{
		if (!this->stopFunc_.IsEmpty())
		{
			engine_->scriptManager.run([&](const ScriptManager::RunInfo& info) {

				v8::Local<v8::Object> recv = v8::Object::New(info.isolate);
				this->stopFunc_.Get(info.isolate)->CallAsFunction(info.context, recv, 0, nullptr);
			});
		}
	}

};
