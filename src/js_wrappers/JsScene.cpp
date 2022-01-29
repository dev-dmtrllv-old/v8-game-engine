#include "js_wrappers/JsScene.hpp"
#include "js_wrappers/JsGameObject.hpp"
#include "Scene.hpp"
#include "Engine.hpp"
#include "ScriptManager.hpp"

namespace NovaEngine
{
	namespace JsWrappers
	{
		JS_METHOD_IMPL(JsScene, spawn)
		{
			Scene* scene = ScriptManager::getInternalFromArgs<Scene*>(args, 0);
			assert(scene != nullptr);
			Entity* entity = scene->spawn();
			const size_t l = args.Length();
			v8::Local<v8::Array> argsList = v8::Array::New(isolate, l);
			for (size_t i = 0; i < l; i++)
				argsList->Set(i, args[i]);
			v8::Local<v8::Value> argsListVal = argsList.As<v8::Value>();
			v8::Local<v8::Function> goClass = engine->scriptManager.getClass<JsGameObject>()->GetFunction();
			v8::Local<v8::Value> go = goClass->CallAsConstructor(isolate->GetCurrentContext(), l, &argsListVal).ToLocalChecked();
			go.As<v8::Object>()->SetInternalField(0, v8::External::New(isolate, scene));
			go.As<v8::Object>()->SetInternalField(1, v8::External::New(isolate, entity));
			args.GetReturnValue().Set(go);
		}

		JS_METHOD_IMPL(JsScene, methodNotImplemented)
		{
			args.GetIsolate()->ThrowException(V8STR("Method not implemented!"));
		}

		void JsScene::build(Engine* engine, JsClass::Builder& builder)
		{
			builder.setInternalFieldCount(1);
			builder.set("load", methodNotImplemented);
			builder.set("start", methodNotImplemented);
			builder.set("stop", methodNotImplemented);
			builder.set("spawn", spawn);
		}
	};
};
