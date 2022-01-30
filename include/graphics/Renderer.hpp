#pragma once

#include "GameWindow.hpp"
#include "ComponentManager.hpp"

namespace NovaEngine::Graphics
{
	class Context;

	template<typename... Ts>
	class Renderer : public ComponentSystem<Ts...>
	{
		private:
			GameWindow* window_;

		public:
			Renderer(ComponentManager* manager, GameWindow* window) : ComponentSystem<Ts...>(manager), window_(window) {}

		protected:
			bool onInitialize();
			bool onTerminate();
	};
};
