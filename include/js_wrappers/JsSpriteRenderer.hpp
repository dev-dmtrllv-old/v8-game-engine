#pragma once

#include "js_wrappers/JsClass.hpp"

namespace NovaEngine
{
	namespace JsWrappers
	{
		class JsSpriteRenderer : public JsClass
		{
			JS_METHOD(setSprite);
			JS_METHOD(getSprite);

			virtual void build(Engine* engine, JsClass::Builder& builder);
		};
	};
};
