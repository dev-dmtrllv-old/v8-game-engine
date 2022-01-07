#pragma once

#include "SubSystem.hpp"
#include "framework.hpp"
#include "Context.hpp"

namespace NovaEngine
{
	class GameWindow;

	namespace Graphics
	{
		class GraphicsManager : public SubSystem<>
		{
		private:
			Context context_;

		public:
			ENGINE_SUB_SYSTEM_CTOR(GraphicsManager), context_() {}

		protected:
			bool onInitialize();
			bool onTerminate();

			void render();
		};
	};
};
