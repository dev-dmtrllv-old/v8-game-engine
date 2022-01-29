#include "js_wrappers/JsTransform.hpp"
#include "Engine.hpp"
#include "ScriptManager.hpp"

namespace NovaEngine
{
	namespace JsWrappers
	{
		JS_METHOD_IMPL(JsTransform, getPosition)
		{

		}
		
		JS_METHOD_IMPL(JsTransform, setPosition)
		{

		}
		
		JS_METHOD_IMPL(JsTransform, getRotation)
		{

		}
		
		JS_METHOD_IMPL(JsTransform, setRotation)
		{

		}
		
		JS_METHOD_IMPL(JsTransform, getScale)
		{

		}
		
		JS_METHOD_IMPL(JsTransform, setScale)
		{

		}
		
		void JsTransform::build(Engine* engine, JsClass::Builder& builder)
		{
			builder.setConstructor([](V8CallbackArgs args) 
			{
				
			});
		}
	};
};
