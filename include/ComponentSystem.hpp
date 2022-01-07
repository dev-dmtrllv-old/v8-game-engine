#pragma once

namespace NovaEngine
{
	class IComponentSystem
	{
	public:
		virtual const char* name() = 0;
		virtual ~IComponentSystem() = default;
	};

	template<typename ...Ts>
	class ComponentSystem : public IComponentSystem
	{
	public:
		ComponentSystem() : IComponentSystem() {}
		
		virtual const char* name() = 0;
		virtual ~ComponentSystem() = default;
	};
};
