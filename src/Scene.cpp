#include "Scene.hpp"
#include "Engine.hpp"
#include "Transform.hpp"

namespace NovaEngine
{
	Scene::Scene(Engine* engine, v8::Isolate* isolate, v8::Local<v8::Object> jsScene) :
		engine_(engine),
		gameObjectTemplate_(),
		jsScene_(),
		spawnMutex_(),
		entityCounter_(),
		entities_(),
		namedEntities_()
	{
		jsScene_.Reset(isolate, jsScene);
		jsScene->SetInternalField(0, v8::External::New(isolate, this));
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
				v8::TryCatch tryCatch = v8::TryCatch(info.isolate);
				auto s = jsScene_.Get(info.isolate);
				s->Get(v8::String::NewFromUtf8(info.isolate, methodName)).As<v8::Function>()->Call(s, 0, nullptr);
				if (tryCatch.HasCaught())
				{
					v8::Local<v8::String> str = tryCatch.Exception()->ToString(info.isolate);
					v8::String::Utf8Value val = v8::String::Utf8Value(str);
					Logger::get()->error("Got exception ", *val);
				}
			});
		}
	}

	Entity* Scene::spawn(size_t n)
	{
		assert(n > 0);
		std::lock_guard<std::mutex> guard(spawnMutex_);
		const size_t index = entities_.size();
		for (size_t i = 0; i < n; i++)
		{
			entities_.push_back(entityCounter_++);
			engine_->componentManager.addComponent<Transform>(&entities_.at(index + i));
		}
		return &entities_[index];
	}

	SCRIPT_METHOD_IMPL(Scene, onSpawnGameObject)
	{
		Scene* scene = ScriptManager::getInternalFromArgs<Scene*>(args, 0);
		assert(scene != nullptr);
		Entity* entity = scene->spawn();
		const size_t l = args.Length();
		v8::Local<v8::Array> argsList = v8::Array::New(isolate, l);
		for (size_t i = 0; i < l; i++)
			argsList->Set(i, args[i]);
		v8::Local<v8::Value> argsListVal = argsList.As<v8::Value>();
		v8::Local<v8::Function> goClass = scene->gameObjectTemplate_.Get(isolate);
		v8::Local<v8::Value> go = goClass->CallAsConstructor(isolate->GetCurrentContext(), l, &argsListVal).ToLocalChecked();
		go.As<v8::Object>()->SetInternalField(0, v8::External::New(isolate, scene));
		go.As<v8::Object>()->SetInternalField(1, v8::External::New(isolate, entity));
		args.GetReturnValue().Set(go);
	}

	v8::Local<v8::Object> Scene::jsScene(v8::Isolate* isolate)
	{
		return jsScene_.Get(isolate);
	}
};
