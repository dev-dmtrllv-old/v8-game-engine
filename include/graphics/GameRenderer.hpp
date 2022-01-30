#pragma once

#include "graphics/Renderer.hpp"
#include "Transform.hpp"
#include "SpriteRenderer.hpp"

namespace NovaEngine::Graphics
{
	class GameRenderer : public Renderer<Transform, SpriteRenderer>
	{
		public:
			GameRenderer(ComponentManager *manager, GameWindow *window);
			virtual const char* name() final override;
			virtual void update() final override;

	};
};
