#pragma once

#include "framework.hpp"
#include "SubSystem.hpp"

#include <coroutine>
#include "job_system/Job.hpp"
#include "job_system/Queue.hpp"
#include "job_system/WaitList.hpp"
#include "EngineConfig.hpp"

#define JOB(name) NovaEngine::JobSystem::Job name(NovaEngine::JobSystem::Counter* __COROUTINE_COUNTER__, NovaEngine::JobSystem::JobScheduler* scheduler, NovaEngine::Engine* engine, void* arg)
#define STATIC_JOB(class, name) NovaEngine::JobSystem::Job class::name(NovaEngine::JobSystem::Counter* __COROUTINE_COUNTER__, NovaEngine::JobSystem::JobScheduler* scheduler, NovaEngine::Engine* engine, void* arg)
#define awaitCounter(counter) co_yield { counter, false }
#define JOB_RETURN co_yield { __COROUTINE_COUNTER__, true }

namespace NovaEngine::JobSystem
{
	class JobScheduler;

	typedef Job(*JobFunction)(Counter* counter, NovaEngine::JobSystem::JobScheduler* scheduler, NovaEngine::Engine* engine, void* arg);

	struct JobInfo
	{
		JobFunction function = nullptr;
		void* arg = nullptr;

		template<typename T>
		JobInfo(JobFunction function, T arg) : function(function), arg(reinterpret_cast<void*>(arg)) {}

		JobInfo(JobFunction function = nullptr, void* arg = nullptr) : function(function), arg(arg) {}
	};

	class JobScheduler : public SubSystem<>
	{
	private:
		size_t maxJobs_;
		Queue<JobHandle> readyQueue_;
		List<JobHandle> waitList_;
		std::mutex jobYieldMutex_;

		std::vector<std::thread> threads_;
		std::thread::id mainThreadID_;
		std::atomic<int> threadsRunning_;
		size_t executionThreads_;

		ENGINE_SUB_SYSTEM_CTOR(JobScheduler),
			maxJobs_(DefaultConfig::JobSystem::maxJobs),
			readyQueue_(),
			waitList_(),
			jobYieldMutex_(),
			threads_(),
			mainThreadID_(std::this_thread::get_id()),
			threadsRunning_(),
			executionThreads_(1)
		{
			threadsRunning_.store(0);
		}

	protected:
		bool onInitialize();
		bool onTerminate();
		bool runNextJob(JobHandle* handleOut);
		void threadEntry(size_t threadID);
		bool handleJobYield(JobHandle* handle);

	public:
		Counter* runJobs(JobInfo* jobs, size_t jobsCount);
		Counter* runJob(JobInfo jobs);
		Counter* runJob(JobFunction func);

		void joinThreads();

		void execNext()
		{
			JobHandle jobHandle;
			if (runNextJob(&jobHandle))
				handleJobYield(&jobHandle);
			else
				std::this_thread::sleep_for(std::chrono::milliseconds(10));
		}

		template<typename LoopConditionCallback>
		void exec(LoopConditionCallback shouldLoop)
		{
			if (mainThreadID_ != std::this_thread::get_id())
				throw std::runtime_error("Cannot call JobScheduler::exec() from another thread than the main thread!");

			bool didStartThreads = threadsRunning_.load() == 0;
			bool didInitializeThreads = threads_.size() == 0;
			
			if (didInitializeThreads)
				initThreads();

			if (didStartThreads)
				runThreads();

			JobHandle jobHandle;

			while (shouldLoop())
				if (runNextJob(&jobHandle))
					handleJobYield(&jobHandle);
			
			if (didStartThreads)
				stopThreads();

			if(didInitializeThreads)
				joinThreads();
		}

		void initThreads();
		void runThreads();
		void stopThreads();
		void clearThreads();
	};
}
