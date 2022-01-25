#pragma once

#include "SubSystem.hpp"
#include "framework.hpp"

namespace NovaEngine
{
	class GameWindow;

	namespace Graphics
	{
		class Renderer;
		
		class GraphicsManager : public SubSystem<>
		{
		private:
			std::unordered_map<GameWindow*, Renderer*> renderers_;

		public:
			ENGINE_SUB_SYSTEM_CTOR(GraphicsManager), renderers_() {}

		protected:
			bool onInitialize();
			bool onTerminate();

			void setRenderer(GameWindow* w);
		};
	};
};
