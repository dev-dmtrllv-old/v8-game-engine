#include "job_system/JobScheduler.hpp"
#include "Logger.hpp"
#include "Engine.hpp"

namespace NovaEngine::JobSystem
{
	static std::atomic<size_t> threadIdCounter = 0;

	bool JobScheduler::onInitialize()
	{
		JobSystemConfig& config = engine()->configManager.getJobSystemConfig();
		maxJobs_ = config.maxJobs == 0 ? DefaultConfig::JobSystem::maxJobs : config.maxJobs;
		executionThreads_ = (config.executionThreads == 0 ? std::thread::hardware_concurrency() : config.executionThreads) - 1;
		return true;
	}

	bool JobScheduler::onTerminate()
	{
		if (threadsRunning_.load(std::memory_order::acquire) == 1)
			stopThreads();

		joinThreads();

		return true;
	}

	void JobScheduler::threadEntry(size_t threadID)
	{
		while (threadsRunning_.load(std::memory_order::acquire) != 1)
			; // wait (spin lock)

		Logger::get()->info("Thread with threadID ", std::to_string(threadID), " started...");

		JobHandle jobHandle;
		while (threadsRunning_.load(std::memory_order::acquire) == 1)
		{
			if (runNextJob(&jobHandle))
				handleJobYield(&jobHandle);
		}
	}

	bool JobScheduler::runNextJob(JobHandlePtr handleOut)
	{
		if (readyQueue_.pop(handleOut))
		{
			if (handleOut != nullptr && !handleOut->done())
			{
				handleOut->resume();
				return true;
			}
			return false;
		}

		return false;
	}

	bool JobScheduler::runNextMainThreadJob(JobHandlePtr handleOut)
	{
		if (mainThreadQueue_.pop(handleOut))
		{
			if (handleOut != nullptr && !handleOut->done())
			{
				handleOut->resume();
				return true;
			}
		}
		return false;
	}

	Counter* JobScheduler::runJobs(JobInfo* jobs, size_t jobsCount, bool mainThreadOnly)
	{
		Counter* c = new Counter(0);
		c->store(jobsCount, std::memory_order::relaxed);

		Queue<JobHandle>* targetQueue = mainThreadOnly ? &mainThreadQueue_ : &readyQueue_;

		for (size_t i = 0; i < jobsCount; i++)
			targetQueue->push(jobs[i].function(c, this, this->engine(), jobs[i].arg));

		return c;
	}

	Counter* JobScheduler::runJob(JobInfo job, bool mainThreadOnly)
	{
		return runJobs(&job, 1, mainThreadOnly);
	}

	Counter* JobScheduler::runJob(JobFunction function, bool mainThreadOnly)
	{
		return runJob({ function, 0 }, mainThreadOnly);
	}

	const bool JobScheduler::isRunning() { return isRunning_; }

	void JobScheduler::runThreads()
	{
		threadsRunning_.store(1, std::memory_order::release);
	}

	void JobScheduler::stopThreads()
	{
		threadsRunning_.store(0, std::memory_order::release);
	}

	void JobScheduler::joinThreads()
	{
		for (auto& thread : threads_)
			thread.join();
		threads_.clear();
	}

	void JobScheduler::initThreads()
	{
		if (threads_.size() == 0)
			for (size_t i = 0; i < executionThreads_; i++)
				threads_.push_back(std::thread([&] { threadEntry(threadIdCounter.fetch_add(1, std::memory_order::acq_rel)); }));
	}

	bool JobScheduler::handleJobYield(JobHandlePtr handle)
	{
		if (handle == nullptr)
		{
			return false;
		}
		else
		{
			std::unique_lock<std::mutex> s(jobYieldMutex_);

			auto [counter, isDone] = handle->promise().state;

			if (!isDone)
			{
				size_t c = counter->load(std::memory_order::seq_cst);
				if (c == 0)
				{
					readyQueue_.push(*handle);
				}
				else
				{
					while (!waitList_.pushWeak(*handle))
					{
						c = counter->load(std::memory_order::seq_cst);
						if (c == 0)
						{
							readyQueue_.push(*handle);
							return true;
						}
					}
				}
			}
			else
			{
				size_t index = counter->fetch_sub(1, std::memory_order::acq_rel) - 1;

				if (index == 0)
				{
					waitList_.findAndRelease([&](JobHandle handle) {
						if (handle.promise().state.counter == counter)
						{
							readyQueue_.push(handle);
							return true;
						}
						return false;
					});
					handle->destroy();

					delete counter;
				}
			}
		}
		return true;
	}

	bool JobScheduler::handleMainThreadJobYield(JobHandlePtr handle)
	{
		if (handle == nullptr)
		{
			return false;
		}
		else
		{
			auto [counter, isDone] = handle->promise().state;

			if (!isDone)
			{
				size_t c = counter->load(std::memory_order::acquire);
				if (c == 0)
				{
					mainThreadQueue_.push(*handle);
				}
				else
				{
					while (!mainThreadWaitList_.pushWeak(*handle))
					{
						c = counter->load(std::memory_order::acquire);
						if (c == 0)
						{
							mainThreadQueue_.push(*handle);
							return true;
						}
					}
				}
			}
			else
			{
				size_t index = counter->fetch_sub(1, std::memory_order::acq_rel) - 1;

				if (index == 0)
				{
					mainThreadWaitList_.findAndRelease([&](JobHandle handle) {
						if (handle.promise().state.counter == counter)
						{
							mainThreadQueue_.push(handle);
							return true;
						}
						return false;
					});
					handle->destroy();

					delete counter;
				}
			}
		}
		return true;
	}
}
