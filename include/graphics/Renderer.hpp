#pragma once

#include "AbstractObject.hpp"
#include "GameWindow.hpp"

namespace NovaEngine::Graphics
{
	class Context;

	class Renderer : public AbstractObject<>
	{
		private:
			GameWindow* window_;

		public:
			Renderer(GameWindow* window) : AbstractObject(), window_(window) {}

		protected:
			bool onInitialize();
			bool onTerminate();

			virtual void render();
	};
};
