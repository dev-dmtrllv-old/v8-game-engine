#pragma once

#include "js_wrappers/JsClass.hpp"
#include "framework.hpp"
#include "ScriptManager.hpp"

namespace NovaEngine
{
	class Engine;

	namespace JsWrappers
	{

		class JsGameObject : public JsClass
		{
		private:
			CLASS_SCRIPT_METHOD(addComponent);
			CLASS_SCRIPT_METHOD(getComponent);
			CLASS_SCRIPT_METHOD(removeComponent);

			virtual void build(Engine* engine, JsClass::Builder& builder) final;
		};
	};
};
