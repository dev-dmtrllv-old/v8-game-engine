#pragma once

#include "js_wrappers/JsObject.hpp"

namespace NovaEngine
{
	namespace JsWrappers
	{
		class JsAssetManager : public JsObject
		{
			JS_METHOD(load);

			virtual void build(JsObject::Builder& builder) final;
		};
	};
};
