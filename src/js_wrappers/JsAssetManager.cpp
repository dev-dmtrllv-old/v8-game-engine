#include "js_wrappers/JsAssetManager.hpp"
#include "Engine.hpp"
#include "ScriptManager.hpp"
#include "js_wrappers/JsAsset.hpp"
#include "Hash.hpp"

namespace NovaEngine
{
	namespace JsWrappers
	{
		JS_METHOD_IMPL(JsAssetManager, load)
		{
			const size_t l = args.Length();

			if (l != 0 && args[0]->IsString())
			{
				v8::String::Utf8Value val(args[0].As<v8::String>());
				Logger::get()->info("Load asset ", *val);

				Hash hash = Hasher::hash(*val);

				// engine->assetManager.loadAsset(hash)

				v8::Local<v8::Function> assetClass = engine->scriptManager.getClass<JsAsset>()->GetFunction();
				v8::Local<v8::Value> ctorArgs[1] = { v8::Number::New(isolate, static_cast<double>(hash)) };
				args.GetReturnValue().Set(assetClass->CallAsConstructor(isolate->GetCurrentContext(), 1, ctorArgs).ToLocalChecked());
			}
			else
			{
				args.GetReturnValue().SetUndefined();
			}
		}

		void JsAssetManager::build(JsObject::Builder& builder)
		{
			builder.set("load", load);
		}
	};
};
