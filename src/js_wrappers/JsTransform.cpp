#include "js_wrappers/JsTransform.hpp"

namespace NovaEngine
{
	namespace JsWrappers
	{
		SCRIPT_METHOD_IMPL(JsTransform, getPosition)
		{

		}
		
		SCRIPT_METHOD_IMPL(JsTransform, setPosition)
		{

		}
		
		SCRIPT_METHOD_IMPL(JsTransform, getRotation)
		{

		}
		
		SCRIPT_METHOD_IMPL(JsTransform, setRotation)
		{

		}
		
		SCRIPT_METHOD_IMPL(JsTransform, getScale)
		{

		}
		
		SCRIPT_METHOD_IMPL(JsTransform, setScale)
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
