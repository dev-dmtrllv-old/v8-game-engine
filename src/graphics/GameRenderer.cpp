#include "graphics/GameRenderer.hpp"

namespace NovaEngine::Graphics
{
	GameRenderer::GameRenderer(ComponentManager *manager, NovaEngine::GameWindow *window) : Renderer<Transform, SpriteRenderer>(manager, window) {}
	const char* GameRenderer::name() { return "sprite renderer"; }
	void GameRenderer::update() {  }
};
