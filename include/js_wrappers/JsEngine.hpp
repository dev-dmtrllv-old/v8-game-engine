#pragma once

#include "js_wrappers/JsObject.hpp"

namespace NovaEngine
{
	namespace JsWrappers
	{
		class JsEngine : public JsObject
		{
			JS_METHOD(onLoad);
			JS_METHOD(start);
			JS_METHOD(log);
			JS_METHOD(getActiveScene);
		public:
			JS_METHOD(onConfigure);

			virtual void build(JsObject::Builder& builder) final;
		};
	};
};
