#pragma once

#include "framework.hpp"

#include "EngineConfig.hpp"

namespace NovaEngine
{
	class Engine;

	class GameWindow
	{
	private:
		GameWindow(Engine* engine);

	public:
		~GameWindow();

		bool create(const char* title, const GameWindowConfig& config);

		/** @returns true if the game window went into fullscreen mode */
		bool toggleFullScreen();

		bool isClosed();
		bool shouldClose();
		void show();
		void destroy();
		GLFWwindow* glfwWindow();
		const char* title();
		Engine* engine();

	private:
		Engine* engine_;
		GLFWwindow* window_;
		std::string title_;
		bool isDestroyed_;

		friend class Engine;
	};
}
