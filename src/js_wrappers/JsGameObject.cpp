#include "js_wrappers/JsGameObject.hpp"

#include "Engine.hpp"
#include "Scene.hpp"
#include "ScriptManager.hpp"
#include "Entity.hpp"
#include "Logger.hpp"

namespace NovaEngine::JsWrappers
{
	JS_METHOD_IMPL(JsGameObject, addComponent)
	{
		Entity* entity = ScriptManager::getInternalFromArgs<Entity*>(args, 1);
		Hash hash = ScriptManager::getComponentHash(args);
		BitMask::Type bitMask = engine->componentManager.getComponentBitMask(hash);

		if (bitMask != 0)
		{
			void* component = engine->componentManager.addComponent(entity, bitMask);

			if (component != nullptr)
			{
				v8::Local<v8::Object> o = args[0].As<v8::Function>()->CallAsConstructor(isolate->GetCurrentContext(), 0, nullptr).ToLocalChecked().As<v8::Object>();
				size_t index = o->InternalFieldCount();
				if (index >= 2)
					index -= 2;
				o->SetInternalField(index, v8::External::New(isolate, entity));
				o->SetInternalField(index + 1, v8::External::New(isolate, component));
				args.GetReturnValue().Set(o);
			}
			else
			{
				Logger::get()->error("Could not get component!");
			}
		}
		else
		{
			std::string s = std::to_string(hash);
			Logger::get()->error("Bitmask is 0 for hash ", s.c_str());
			args.GetReturnValue().SetUndefined();
		}
	}

	JS_METHOD_IMPL(JsGameObject, getComponent)
	{
		Entity* entity = ScriptManager::getInternalFromArgs<Entity*>(args, 1);
		Hash hash = ScriptManager::getComponentHash(args);
		BitMask::Type bitMask = engine->componentManager.getComponentBitMask(hash);

		if (bitMask != 0)
		{
			void* component = engine->componentManager.getComponent(entity, bitMask);

			if (component != nullptr)
			{
				v8::Local<v8::Object> o = args[0].As<v8::Function>()->CallAsConstructor(isolate->GetCurrentContext(), 0, nullptr).ToLocalChecked().As<v8::Object>();
				size_t index = o->InternalFieldCount();
				if (index >= 2)
					index -= 2;
				o->SetInternalField(index, v8::External::New(isolate, entity));
				o->SetInternalField(index + 1, v8::External::New(isolate, component));
				args.GetReturnValue().Set(o);
			}
			else
			{
				Logger::get()->error("Could not get component!");
			}
		}
		else
		{
			std::string s = std::to_string(hash);
			Logger::get()->error("Bitmask is 0 for hash ", s.c_str());
			args.GetReturnValue().SetUndefined();
		}
	}

	JS_METHOD_IMPL(JsGameObject, removeComponent)
	{

	}

	void JsGameObject::build(Engine* engine, JsClass::Builder& builder)
	{
		builder.setConstructor([](V8CallbackArgs args) {
			using namespace v8;

			Isolate* isolate = args.GetIsolate();
			Engine* engine = static_cast<Engine*>(args.Data().As<v8::External>()->Value());
			Scene* scene = engine->sceneManager.activeScene();

			if (scene == nullptr)
			{
				isolate->ThrowException(V8STR("There is no scene loaded!"));
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
				this_->Set(isolate->GetCurrentContext(), V8STR("name"), args[0]);
			}
		}, engine);

		builder.setInternalFieldCount(2);

		builder.set("addComponent", addComponent);
		builder.set("getComponent", getComponent);
		builder.set("removeComponent", removeComponent);
	}
};
