#pragma once
#include "BitMask.hpp"

namespace NovaEngine
{
	struct Entity
	{
		using IDCounter = long unsigned int;
		const IDCounter id;
		BitMask::Type componentsBitMask;

		Entity(const IDCounter id) : id(id), componentsBitMask(0) {}
		Entity(const IDCounter id, BitMask::Type componentsBitMask) : id(id), componentsBitMask(componentsBitMask) {}
	};
};
