#include "TransformSystem.hpp"
#include "ComponentManager.hpp"
#include "Transform.hpp"

namespace NovaEngine
{
	TransformSystem::TransformSystem(ComponentManager* manager) : ComponentSystem<Transform>(manager) {}

	TransformSystem::~TransformSystem() {}

	const char* TransformSystem::name() { return "transform"; }

	void TransformSystem::update() {}
};
