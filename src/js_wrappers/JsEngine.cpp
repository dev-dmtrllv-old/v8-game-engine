#include "js_wrappers/JsEngine.hpp"
#include "js_wrappers/JsWindow.hpp"
#include "ScriptManager.hpp"
#include "Engine.hpp"

namespace NovaEngine
{
	namespace JsWrappers
	{
		JS_METHOD_IMPL(JsEngine, onLoad)
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
				engine->setOnLoadCallback(v8::Local<v8::Function>::Cast(args[0]));
			}
			args.GetReturnValue().Set(v8::Undefined(isolate_));
		}

		JS_METHOD_IMPL(JsEngine, start)
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

		JS_METHOD_IMPL(JsEngine, log)
		{
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

		JS_METHOD_IMPL(JsEngine, getActiveScene)
		{
			args.GetReturnValue().Set(engine->sceneManager.activeScene()->jsScene(args.GetIsolate()));
		}

		JS_METHOD_IMPL(JsEngine, onConfigure)
		{
			if (args.Length() < 1)
			{

			}
			else if (args[0]->IsObject())
			{
				v8::Local<v8::Promise::Resolver> resolver = v8::Promise::Resolver::New(isolate);
				v8::Local<v8::Promise> p = resolver->GetPromise();
				args.GetReturnValue().Set(p);
				engine->setConfigValue(args[0]->ToObject(isolate), resolver);
			}
		}

		void JsEngine::build(JsObject::Builder& builder)
		{
			builder.set("onLoad", onLoad);
			builder.set("start", start);
			builder.set("log", log);
			builder.set("getActiveScene", getActiveScene);

			builder.set<JsWindow>("window");
		}
	};
};
