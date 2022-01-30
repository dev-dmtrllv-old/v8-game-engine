#pragma once

#include "js_wrappers/JsClass.hpp"

namespace NovaEngine
{

	namespace JsWrappers
	{
		class JsTransform : public JsClass
		{
			JS_METHOD(ctor);

			JS_GETTER(position);
			JS_SETTER(position);
			JS_GETTER(rotation);
			JS_SETTER(rotation);
			JS_GETTER(scale);
			JS_SETTER(scale);

			virtual void build(Engine* engine, JsClass::Builder& builder);
		};
	};
};
