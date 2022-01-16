#pragma once

#include "framework.hpp"

namespace NovaEngine
{
	class BitMap
	{
	private:
		std::stack<size_t> freeBits_;

		size_t lastUsedBit_;

	public:
		BitMap() : freeBits_(), lastUsedBit_(0) {}

		const size_t generate()
		{
			if (!freeBits_.empty())
			{
				size_t bit = freeBits_.top();
				freeBits_.pop();
				return bit;
			}
			else
			{
				lastUsedBit_ = lastUsedBit_ == 0 ? 1 : (lastUsedBit_ * 2);
				return lastUsedBit_;
			}
		}

		void remove(const size_t bit)
		{
			if (lastUsedBit_ == bit)
				lastUsedBit_ = lastUsedBit_ == 1 ? 0 : (lastUsedBit_ / 2);
			else
				freeBits_.push(bit);
		}
	};
};
