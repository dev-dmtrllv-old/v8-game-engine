#pragma once

#include "js_wrappers/JsObject.hpp"

namespace NovaEngine
{
	namespace JsWrappers
	{
		class JsWindow : public JsObject
		{
			JS_METHOD(show);

			virtual void build(JsObject::Builder& builder) final;
		};
	};
};
