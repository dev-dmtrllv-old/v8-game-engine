#pragma once

#include "ComponentManager.hpp"
#include "Transform.hpp"

namespace NovaEngine
{
	class TransformSystem : public ComponentSystem<Transform>
	{
		std::unordered_map<Transform*, std::vector<Transform*>> parentToChildrenMap_;

	public:
		TransformSystem(ComponentManager* manager);
		~TransformSystem();

		virtual const char* name() override;
		virtual void update() override;

		void setParent(Transform* child, Transform* parent)
		{
			Transform* cp = child->parent;

			if (cp != nullptr)
				std::remove(parentToChildrenMap_[cp].begin(), parentToChildrenMap_[cp].end(), child);

			if (parent != nullptr)
			{
				if (!parentToChildrenMap_.contains(parent))
					parentToChildrenMap_[parent] = std::vector<Transform*>();

				parentToChildrenMap_[parent].push_back(child);
			}
		}

		std::vector<Transform*>& getChildren(Transform* transform)
		{
			return parentToChildrenMap_[transform];
		}
	};
};
