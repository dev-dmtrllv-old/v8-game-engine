#pragma once

#include "framework.hpp"

namespace NovaEngine::JobSystem
{
	typedef std::atomic<size_t> Counter;

	struct Job
	{
		struct State
		{
			Counter* counter = nullptr;
			bool isDone = false;
		};

		struct suspend_maybe
		{
			bool suspend;
			suspend_maybe(bool suspend): suspend(suspend){}

			bool await_ready() const noexcept { return suspend; }

			void await_suspend(std::coroutine_handle<>) const noexcept {}

			void await_resume() const noexcept {}
		};

		struct promise_type
		{
			State state;
			promise_type() = default;
			Job get_return_object() { return { std::coroutine_handle<promise_type>::from_promise(*this) }; }
			std::suspend_always initial_suspend() { return {}; }
			std::suspend_never final_suspend() noexcept { return {}; }
			suspend_maybe yield_value(State s)
			{
				state = s;
				if(s.counter->load(std::memory_order::acquire) == 0)
					return suspend_maybe { true };
				return suspend_maybe { false };
			};
			void unhandled_exception() {}
		};

		std::coroutine_handle<promise_type> handle_;
		Job(std::coroutine_handle<promise_type> handle) : handle_(handle) {  }

		operator std::coroutine_handle<promise_type>() const { return handle_; }
	};

	typedef std::coroutine_handle<Job::promise_type> JobHandle;
	typedef JobHandle* JobHandlePtr;
};
