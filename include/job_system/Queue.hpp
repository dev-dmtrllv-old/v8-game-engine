#pragma once

// #include "framework.hpp"

#define AtomicFalse 0
#define AtomicTrue 1

namespace NovaEngine::JobSystem
{
	typedef std::lock_guard<std::mutex> scope_lock;
	typedef std::atomic<int> AtomicBool;

	template<typename T>
	class Queue
	{
	private:
		std::mutex mutex_;
		std::condition_variable cv_;
		AtomicBool isBussy_;
		std::queue<T> queue_;

		inline bool checkAndSetBussy()
		{
			int trueValue = AtomicTrue;
			return !isBussy_.compare_exchange_weak(trueValue, AtomicFalse, std::memory_order::seq_cst);
		}

	public:
		Queue() : mutex_(), cv_(), isBussy_(), queue_()
		{
			isBussy_.store(AtomicFalse, std::memory_order::seq_cst);
		}

		bool isEmpty()
		{
			std::unique_lock<std::mutex> lock(mutex_);
			int f = AtomicTrue;
			cv_.wait(lock, [&] { return checkAndSetBussy(); });

			bool wasEmpty = queue_.empty();

			lock.unlock();
			isBussy_.store(AtomicFalse, std::memory_order::seq_cst);
			cv_.notify_all();

			return wasEmpty;
		}

		bool isEmptyWeak()
		{
			if (!mutex_.try_lock())
				return false;

			bool wasEmpty = queue_.empty();

			mutex_.unlock();
			cv_.notify_all();

			return wasEmpty;
		}


		bool push(T&& item)
		{
			std::unique_lock<std::mutex> lock(mutex_);
			int f = AtomicTrue;
			cv_.wait(lock, [&] { return checkAndSetBussy(); });

			queue_.push(std::move(item));

			lock.unlock();
			isBussy_.store(AtomicFalse, std::memory_order::seq_cst);
			cv_.notify_all();

			return true;
		}

		bool pushWeak(T&& item)
		{
			if (!mutex_.try_lock())
				return false;

			queue_.push(std::move(item));

			mutex_.unlock();
			cv_.notify_all();

			return true;
		}

		bool push(T& item)
		{
			std::unique_lock<std::mutex> lock(mutex_);
			int f = AtomicTrue;
			cv_.wait(lock, [&] { return checkAndSetBussy(); });

			queue_.push(item);

			lock.unlock();
			isBussy_.store(AtomicFalse, std::memory_order::seq_cst);
			cv_.notify_all();

			return true;
		}

		bool pushWeak(T& item)
		{
			if (!mutex_.try_lock())
				return false;

			queue_.push(item);

			mutex_.unlock();
			cv_.notify_all();

			return true;
		}

		bool pop(T* itemPtr)
		{
			std::unique_lock<std::mutex> lock(mutex_);
			cv_.wait(lock, [&] { return checkAndSetBussy(); });

			if (queue_.empty())
			{
				lock.unlock();
				isBussy_.store(AtomicFalse, std::memory_order::seq_cst);
				cv_.notify_all();
				return false;
			}

			*itemPtr = queue_.front();
			queue_.pop();

			lock.unlock();
			isBussy_.store(AtomicFalse, std::memory_order::seq_cst);
			cv_.notify_all();

			return true;
		}

		bool popWeak(T* itemPtr)
		{
			if (!mutex_.try_lock())
				return false;

			if (queue_.empty())
			{
				mutex_.unlock();
				cv_.notify_all();
				return false;
			}

			*itemPtr = queue_.back();
			queue_.pop();

			mutex_.unlock();
			cv_.notify_all();

			return true;
		}
	};
}
