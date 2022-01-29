#pragma once

#include "js_wrappers/JsClass.hpp"
#include "framework.hpp"

namespace NovaEngine
{
	class Engine;

	namespace JsWrappers
	{
		class JsScene : public JsClass
		{
		private:
			JS_METHOD(spawn);
			JS_METHOD(methodNotImplemented);

			virtual void build(Engine* engine, JsClass::Builder& builder) final;
		};
	};
};
