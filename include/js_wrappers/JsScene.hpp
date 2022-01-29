#pragma once

#include "js_wrappers/JsClass.hpp"
#include "framework.hpp"
#include "ScriptManager.hpp"

namespace NovaEngine
{
	class Engine;

	namespace JsWrappers
	{
		class JsScene : public JsClass
		{
		private:
			CLASS_SCRIPT_METHOD(methodNotImplemented);

			virtual void build(Engine* engine, JsClass::Builder& builder) final;
		};
	};
};
