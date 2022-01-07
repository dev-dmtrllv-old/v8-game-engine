#pragma once

#include "Initializable.hpp"
#include "Terminatable.hpp"

namespace NovaEngine
{
	template<typename... Args>
	class AbstractObject;

	template<>
	class AbstractObject<> : public Initializable<>, public Terminatable
	{
	public:
		bool isReady()
		{
			return this->isInitialized() && !this->isTerminated();
		}
	};

	template<typename... Args>
	class AbstractObject : public Initializable<Args...>, public Terminatable
	{
	public:
		bool isReady()
		{
			return this->isInitialized() && !this->isTerminated();
		}
	};
};
