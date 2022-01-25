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

		ScriptManager::ClassBuilder goTemplate = ScriptManager::ClassBuilder(isolate, "GameObject", 2, [](V8CallbackArgs args) {
			assert(args.Length() == 1);
			assert(args[0]->IsArray());

			v8::Isolate* isolate = args.GetIsolate();
			v8::Local<v8::Context> ctx = isolate->GetCurrentContext();

			v8::Local<v8::Array> arguments = args[0].As<v8::Array>();
			
			if (arguments->Length() == 0)
			{

			}
			else
			{
				auto first = arguments->Get(0);

				/**
				 * arg options:
				 * - (...) "empty"
				 *  - (name: string)
				 *  - (position: vector2 | vector3)
				 * 	- (name: string, position: vector2 | vector3)
				 */

				if (first->IsString())
				{
					args.This()->Set(ctx, v8::String::NewFromUtf8(isolate, "name"), first);
					if(arguments->Length() == 2)
					{
						
					}
				}
				else if (first->IsObject())
				{
					args.This()->Set(ctx, v8::String::NewFromUtf8(isolate, "name"), v8::String::NewFromUtf8(isolate, "Nameless GameObject"));
				}
			}
		});
		gameObjectTemplate_.Reset(isolate, goTemplate.build());
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

	Entity* Scene::spawn(size_t n)
	{
		assert(n > 0);
		std::lock_guard<std::mutex> guard(spawnMutex_);
		const size_t index = entities_.size();
		for (size_t i = 0; i < n; i++)
		{
			entities_.push_back(entityCounter_++);
		}
		return &entities_[index];
	}

	SCRIPT_METHOD_IMPL(Scene, onSpawnGameObject)
	{
		v8::Isolate* isolate = args.GetIsolate();
		Engine* engine = ScriptManager::fetchEngineFromArgs(args);
		Scene* scene = static_cast<Scene*>(args.This()->GetInternalField(0).As<v8::External>()->Value());
		assert(scene != nullptr);
		Entity* entity = scene->spawn();
		engine->componentManager.addComponent<Transform>(entity);
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
};
