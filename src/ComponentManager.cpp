#include "ComponentManager.hpp"
#include "Engine.hpp"
#include "TransformSystem.hpp"
#include "Transform.hpp"

namespace NovaEngine
{
	struct Test
	{
		float a;
		float b;
	};

	class TestComponentSystem : public ComponentSystem<Test, Transform>
	{
	public:
		TestComponentSystem(ComponentManager* manager) : ComponentSystem<Test, Transform>(manager) {}
		~TestComponentSystem() {}

		const char* name() { return "test component"; }
		void update() {}
	};

	bool ComponentManager::onInitialize()
	{
		registerSystem<TestComponentSystem>();
		registerSystem<TransformSystem>();
		return true;
	}

	bool ComponentManager::onTerminate()
	{
		return true;
	}

	void ComponentManager::registerSystem(Hash hash, IComponentSystem* componentSystem)
	{
		systems_.emplace(hash, componentSystem);
	}
}
