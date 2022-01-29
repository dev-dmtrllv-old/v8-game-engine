#pragma once

#include "js_wrappers/JsClass.hpp"

namespace NovaEngine
{

	namespace JsWrappers
	{
		class JsTransform : public JsClass
		{
			JS_METHOD(getPosition);
			JS_METHOD(setPosition);
			JS_METHOD(getRotation);
			JS_METHOD(setRotation);
			JS_METHOD(getScale);
			JS_METHOD(setScale);

			virtual void build(Engine* engine, JsClass::Builder& builder);
		};
	};
};
