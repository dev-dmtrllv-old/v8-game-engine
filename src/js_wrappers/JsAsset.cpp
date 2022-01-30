#include "js_wrappers/JsAsset.hpp"
#include "Engine.hpp"
#include "ScriptManager.hpp"
#include "js_wrappers/JsObject.hpp"

namespace NovaEngine::JsWrappers
{
	JS_METHOD_IMPL(JsAsset, ctor)
	{
		JsObject::Builder b(args.This(), isolate);
		b.set("hash", args[0]);
	}

	void JsAsset::build(Engine* engine, JsClass::Builder& builder)
	{
		builder.setConstructor(ctor);
	}
};
