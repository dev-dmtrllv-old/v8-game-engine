#pragma once

#include "framework.hpp"

namespace NovaEngine
{
	class BitMask
	{
	public:
		using Type = size_t;

		constexpr static const bool containsBit(BitMask::Type bitMask, size_t bitIndex)
		{
			return bitMask & (1 << bitIndex);
		}

		class Generator
		{
		private:
			std::stack<size_t> freeBits_;

			size_t lastUsedBit_;
			size_t usedBits_;

		public:
			Generator() : freeBits_(), lastUsedBit_(0), usedBits_(0) {}

			const size_t generate()
			{
				usedBits_++;
				if (!freeBits_.empty())
				{
					size_t bit = freeBits_.top();
					freeBits_.pop();
					return bit;
				}
				else
				{
					lastUsedBit_ = lastUsedBit_ == 0 ? 1 : lastUsedBit_ << 1;
					return lastUsedBit_;
				}
			}

			void remove(const size_t bit)
			{
				usedBits_--;
				if (lastUsedBit_ == bit)
					lastUsedBit_ = lastUsedBit_ == 1 ? 0 : lastUsedBit_ >> 1;
				else
					freeBits_.push(bit);
			}

			const size_t usedBits()
			{
				return usedBits_;
			}

			template<typename Callback>
			void forEachBit(size_t bits, Callback callback)
			{
				for (size_t i = 0; i < lastUsedBit_; i++)
					if (containsBit(bits, i + 1))
						callback(i + 1);
			}
		};
	};
};
