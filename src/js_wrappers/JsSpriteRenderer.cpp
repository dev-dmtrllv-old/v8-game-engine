#include "js_wrappers/JsSpriteRenderer.hpp"
#include "Engine.hpp"
#include "ScriptManager.hpp"

namespace NovaEngine
{
	namespace JsWrappers
	{
		void JsSpriteRenderer::build(Engine* engine, JsClass::Builder& builder)
		{
			builder.setConstructor([](V8CallbackArgs args) 
			{
				
			});
		}
	};
};
