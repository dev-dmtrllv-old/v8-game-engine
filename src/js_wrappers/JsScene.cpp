#include "js_wrappers/JsScene.hpp"
#include "Scene.hpp"

namespace NovaEngine
{
	namespace JsWrappers
	{	
		SCRIPT_METHOD_IMPL(JsScene, methodNotImplemented)
		{
			args.GetIsolate()->ThrowException(V8STR("Method not implemented!"));
		}
		
		void JsScene::build(Engine* engine, JsClass::Builder& builder)
		{
			builder.setInternalFieldCount(1);
			builder.set("load", methodNotImplemented);
			builder.set("start", methodNotImplemented);
			builder.set("stop", methodNotImplemented);
			builder.set("spawn", Scene::onSpawnGameObject);
		}
	};
};
