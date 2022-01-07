#pragma once

#define ENGINE_COMPONENT_CTOR(class) class(GameObject* gameObject) : Component(gameObject)

namespace NovaEngine
{
	class GameObject;

	class Component
	{
		private:
			GameObject* gameObject_;

		public:
			Component(GameObject* gameObject);

			GameObject* gameObject() { return gameObject_; }
	};
};
