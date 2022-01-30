#pragma once

#include "SubSystem.hpp"
#include "framework.hpp"

namespace NovaEngine
{
	class GameWindow;

	namespace Graphics
	{	
		class GraphicsManager : public SubSystem<>
		{
		public:
			ENGINE_SUB_SYSTEM_CTOR(GraphicsManager) {}

		protected:
			bool onInitialize();
			bool onTerminate();
		};
	};
};
