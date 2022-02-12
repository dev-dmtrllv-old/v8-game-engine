#pragma once

#include "framework.hpp"
#include "SubSystem.hpp"

#include <coroutine>
#include "job_system/Job.hpp"
#include "job_system/Queue.hpp"
#include "job_system/WaitList.hpp"
#include "EngineConfig.hpp"

#define JOB(name) NovaEngine::JobSystem::Job name(NovaEngine::JobSystem::Counter* __COROUTINE_COUNTER__, NovaEngine::JobSystem::JobScheduler* scheduler, NovaEngine::Engine* engine, void* arg)
#define JOB_AWAIT(counter) if(counter != nullptr) co_yield { counter, false };
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

		JobInfo(JobFunction function, void* arg = nullptr) : function(function), arg(arg) {}
	};

	class JobScheduler : public SubSystem<>
	{
	private:
		size_t maxJobs_;
		Queue<JobHandle>* readyQueue_;
		Queue<JobHandle>* mainThreadQueue_;
		List<JobHandle> waitList_;
		List<JobHandle> mainThreadWaitList_;
		// std::mutex jobYieldMutex_;
		// std::mutex mainThreadJobYieldMutex_;

		std::vector<std::thread> threads_;
		std::thread::id mainThreadID_;
		std::atomic<int> threadsRunning_;
		size_t executionThreads_;

		std::unordered_map<std::thread::id, size_t> threadIdMap_;

		bool isRunning_;

		ENGINE_SUB_SYSTEM_CTOR(JobScheduler),
			maxJobs_(DefaultConfig::JobSystem::maxJobs),
			readyQueue_(),
			mainThreadQueue_(),
			waitList_(),
			mainThreadWaitList_(),
			// jobYieldMutex_(),
			threads_(),
			mainThreadID_(std::this_thread::get_id()),
			threadsRunning_(),
			executionThreads_(1),
			threadIdMap_(),
			isRunning_(false)
		{
			threadsRunning_.store(0);
		}

	protected:
		bool onInitialize();
		bool onTerminate();
		bool runNextJob(JobHandle* handleOut);
		bool runNextMainThreadJob(JobHandle* handleOut);
		void threadEntry(size_t threadID);
		bool handleJobYield(JobHandle* handle);
		bool handleMainThreadJobYield(JobHandle* handle);

	public:
		Counter* runJobs(JobInfo* jobs, size_t jobsCount, bool mainThreadOnly = false);
		Counter* runJob(JobInfo jobs, bool mainThreadOnly = false);
		Counter* runJob(JobFunction func, bool mainThreadOnly = false);

		size_t getThreadID();

		void joinThreads();

		const bool isRunning();

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

			if(isRunning_)
				throw std::runtime_error("JobScheduler::exec() is already running!");

			isRunning_ = true;

			bool didStartThreads = threadsRunning_.load() == 0;
			bool didInitializeThreads = threads_.size() == 0;

			if (didInitializeThreads)
				initThreads();

			if (didStartThreads)
				runThreads();


			JobHandle jobHandle;

			while (shouldLoop())
			{
				if(runNextMainThreadJob(&jobHandle))
					handleMainThreadJobYield(&jobHandle);
				
				if (shouldLoop() && runNextJob(&jobHandle))
					handleJobYield(&jobHandle);
			}

			isRunning_ = true;

			if (didStartThreads)
				stopThreads();

			if (didInitializeThreads)
				joinThreads();
		}

		void initThreads();
		void runThreads();
		void stopThreads();
		void clearThreads();
	};
}
