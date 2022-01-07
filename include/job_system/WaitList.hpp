#pragma once

#include "job_system/Job.hpp"
#include "framework.hpp"


#define AtomicFalse 0
#define AtomicTrue 1

namespace NovaEngine::JobSystem
{
	struct Job;

	typedef std::lock_guard<std::mutex> scope_lock;
	typedef std::atomic<int> AtomicBool;


	template<typename T>
	class List
	{
		std::mutex mutex_;
		std::condition_variable cv_;
		AtomicBool isBussy_;
		std::vector<T> list_;

		inline bool checkAndSetBussy()
		{
			int trueValue = AtomicTrue;
			return !isBussy_.compare_exchange_weak(trueValue, AtomicFalse, std::memory_order::seq_cst);
		}

	public:
		List() :
			mutex_(),
			cv_(),
			isBussy_(),
			list_()
		{}

		~List() {}

		bool push(T& item)
		{
			std::unique_lock<std::mutex> lock(mutex_);

			cv_.wait(lock, [&] { return checkAndSetBussy(); });

			list_.push_back(item);

			lock.unlock();
			isBussy_.store(AtomicFalse, std::memory_order::seq_cst);
			cv_.notify_all();

			return true;
		}

		bool pushWeak(T& item)
		{
			if(!mutex_.try_lock())
				return false;

			list_.push_back(item);

			mutex_.unlock();
			isBussy_.store(AtomicFalse, std::memory_order::seq_cst);
			cv_.notify_all();

			return true;
		}

		template<typename Callback>
		void findAndRelease(Callback callback)
		{
			std::unique_lock<std::mutex> lock(mutex_);

			cv_.wait(lock, [&] { return checkAndSetBussy(); });

			std::vector<T*> toRemove;

			for (T& l : list_)
			{
				if (callback(l))
					toRemove.push_back(&l);
			}

			for (T* l : toRemove)
				list_.erase(std::remove(list_.begin(), list_.end(), *l), list_.end());

			lock.unlock();
			isBussy_.store(AtomicFalse, std::memory_order::seq_cst);
			cv_.notify_all();
		}
	};
}
