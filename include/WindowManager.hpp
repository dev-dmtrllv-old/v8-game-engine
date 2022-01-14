#pragma once

#include "SubSystem.hpp"
#include "framework.hpp"
#include "job_system/JobScheduler.hpp"
#include "Hash.hpp"

namespace NovaEngine
{
	class GameWindow;

	enum class WindowManagerEvents : Hash
	{
		ALL_WINDOWS_CLOSED = Hasher::hash("ALL_WINDOWS_CLOSED"),
	};

	class WindowManager : public SubSystem<>
	{
	protected:
		static JOB(pollEvents);
		static void onWindowClose(GLFWwindow* window);

	private:
		std::vector<GameWindow*> windows_;

	public:
		ENGINE_SUB_SYSTEM_CTOR(WindowManager), windows_() {}

		bool allWindowsClosed();
		void addWindow(GameWindow* window);

	protected:
		bool onInitialize();
		bool onTerminate();
	};
};
