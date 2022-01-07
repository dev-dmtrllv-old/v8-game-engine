#pragma once

#include "SubSystem.hpp"
#include "framework.hpp"
#include "job_system/JobScheduler.hpp"

namespace NovaEngine
{
	class GameWindow;

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
