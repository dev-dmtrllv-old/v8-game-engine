#include "js_wrappers/JsGameObject.hpp"

#include "Engine.hpp"
#include "Scene.hpp"
#include "ScriptManager.hpp"
#include "Entity.hpp"

namespace NovaEngine::JsWrappers
{
	JS_METHOD_IMPL(JsGameObject, addComponent)
	{
		// Entity* entity = ScriptManager::getInternalFromArgs<Entity*>(args, 1);
		// BitMask::Type bitMask = ScriptManager::getComponentBitMask(args.GetIsolate(), args[0]);

		// if (bitMask != 0)
		// {
		// 	void* component = engine->componentManager.getComponent<void*>(entity, bitMask);

		// 	if (component != nullptr)
		// 	{
		// 		auto v = componentClassMap_[bitMask].Get(args.GetIsolate());
		// 		args.GetReturnValue().Set(v->CallAsConstructor(args.GetIsolate()->GetCurrentContext(), 0, nullptr).ToLocalChecked());
		// 	}
		// }
		// else
		// {
			args.GetReturnValue().SetUndefined();
		// }
	}

	JS_METHOD_IMPL(JsGameObject, getComponent)
	{

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
