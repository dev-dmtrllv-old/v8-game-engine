#pragma once

#include "js_wrappers/JsClass.hpp"

namespace NovaEngine
{
	namespace JsWrappers
	{

		class JsGameObject : public JsClass
		{
		private:
			JS_METHOD(addComponent);
			JS_METHOD(getComponent);
			JS_METHOD(removeComponent);

			virtual void build(Engine* engine, JsClass::Builder& builder) final;
		};
	};
};
