#pragma once

#include "framework.hpp"

namespace NovaEngine
{
	typedef float Color[4];

	namespace DefaultConfig
	{
		namespace JobSystem
		{
			constexpr size_t maxJobs = 1000;
			constexpr size_t executionThreads = 0;
		};

		namespace GameWindow
		{
			constexpr size_t minWidth = 640;
			constexpr size_t minHeight = 480;
			constexpr size_t maxWidth = std::numeric_limits<uint32_t>::max();
			constexpr size_t maxHeight = std::numeric_limits<uint32_t>::max();
			constexpr size_t width = 640;
			constexpr size_t height = 480;
			constexpr bool resizable = true;
			constexpr bool maximized = true;
			constexpr bool fullscreen = false;
			constexpr bool hidden = false;
		};
	};

	struct GraphicsConfig
	{
		Color clearColor = { 0.0f, 0.0f, 0.0f, 0.0f };
	};

	struct JobSystemConfig
	{
		size_t maxJobs = DefaultConfig::JobSystem::maxJobs;
		size_t executionThreads = DefaultConfig::JobSystem::executionThreads;
	};

	struct GameWindowConfig
	{
		size_t minWidth = DefaultConfig::GameWindow::minWidth;
		size_t minHeight = DefaultConfig::GameWindow::minHeight;
		size_t maxWidth = DefaultConfig::GameWindow::maxWidth;
		size_t maxHeight = DefaultConfig::GameWindow::maxHeight;
		size_t width = DefaultConfig::GameWindow::width;
		size_t height = DefaultConfig::GameWindow::height;
		bool resizable = DefaultConfig::GameWindow::resizable;
		bool maximized = DefaultConfig::GameWindow::maximized;
		bool fullscreen = DefaultConfig::GameWindow::fullscreen;
		bool hidden = DefaultConfig::GameWindow::hidden;
		GraphicsConfig graphics;
	};

	struct GameConfig
	{
		std::string name = "MISSING GAME NAME";
		GameWindowConfig window = GameWindowConfig();
		JobSystemConfig jobSystem = JobSystemConfig();
	};
};

#pragma once
