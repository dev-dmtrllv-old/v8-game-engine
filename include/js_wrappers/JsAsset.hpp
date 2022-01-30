#pragma once

#include "js_wrappers/JsClass.hpp"

namespace NovaEngine
{
	class Engine;

	namespace JsWrappers
	{
		class JsAsset : public JsClass
		{
		public:
			JS_METHOD(ctor);

			virtual void build(Engine* engine, JsClass::Builder& builder) final;
		};
	};
};
