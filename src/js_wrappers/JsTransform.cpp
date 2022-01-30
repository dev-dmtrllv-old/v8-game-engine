#include "js_wrappers/JsTransform.hpp"
#include "Engine.hpp"
#include "ScriptManager.hpp"

namespace NovaEngine
{
	namespace JsWrappers
	{
		JS_METHOD_IMPL(JsTransform, ctor)
		{
			
		}

		JS_GETTER_IMPL(JsTransform, position)
		{

		}

		JS_SETTER_IMPL(JsTransform, position)
		{

		}

		JS_GETTER_IMPL(JsTransform, rotation)
		{

		}

		JS_SETTER_IMPL(JsTransform, rotation)
		{

		}

		JS_GETTER_IMPL(JsTransform, scale)
		{

		}

		JS_SETTER_IMPL(JsTransform, scale)
		{

		}

		void JsTransform::build(Engine* engine, JsClass::Builder& builder)
		{
			builder.setConstructor(ctor);
			builder.setAccessors("position", getposition, setposition);
			builder.setAccessors("rotation", getrotation, setrotation);
			builder.setAccessors("scale", getscale, setscale);
		}
	};
};
