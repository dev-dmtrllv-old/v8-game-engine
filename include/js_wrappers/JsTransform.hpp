#pragma once

#include "js_wrappers/JsClass.hpp"
#include "Engine.hpp"
#include "ScriptManager.hpp"

namespace NovaEngine
{
	class Engine;

	namespace JsWrappers
	{
		class JsTransform : public JsClass
		{
			CLASS_SCRIPT_METHOD(getPosition);
			CLASS_SCRIPT_METHOD(setPosition);
			CLASS_SCRIPT_METHOD(getRotation);
			CLASS_SCRIPT_METHOD(setRotation);
			CLASS_SCRIPT_METHOD(getScale);
			CLASS_SCRIPT_METHOD(setScale);

			virtual void build(Engine* engine, JsClass::Builder& builder);
		};
	};
};
