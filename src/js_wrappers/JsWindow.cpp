#include "js_wrappers/JsWindow.hpp"
#include "Engine.hpp"
#include "ScriptManager.hpp"

namespace NovaEngine
{
	namespace JsWrappers
	{
		JS_METHOD_IMPL(JsWindow, show)
		{
			engine->gameWindow.show();
		}

		void JsWindow::build(JsObject::Builder& builder)
		{
			builder.set("show", show);
		}
	};
};
