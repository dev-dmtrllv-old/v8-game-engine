#include "GameWindow.hpp"
#include "framework.hpp"
#include "ConfigManager.hpp"
#include "Logger.hpp"

namespace NovaEngine
{
	GameWindow::GameWindow(Engine* engine) : engine_(engine), window_(nullptr), title_(), isDestroyed_(false) {  }

	GameWindow::~GameWindow() {  }

	bool GameWindow::create(const char* title, const GameWindowConfig& config)
	{
		if (window_ == nullptr)
		{
			const size_t titleLen = strlen(title);
			title_.reserve(titleLen);
			memcpy(title_.data(), title, titleLen);

			glfwWindowHint(GLFW_RESIZABLE, config.resizable);
			glfwWindowHint(GLFW_MAXIMIZED, config.maximized);
			glfwWindowHint(GLFW_VISIBLE, !config.hidden);
			glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
			glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);

			window_ = glfwCreateWindow(config.width, config.height, title, nullptr, nullptr);

			if (window_ == nullptr)
			{
				Logger::get()->error("Failed to open GLFW window.");
				return false;
			}

			glfwSetWindowUserPointer(window_, this);
			glfwSetWindowSizeLimits(window_, config.minWidth, config.minHeight, config.maxHeight, config.maxHeight);
			glfwSwapInterval(1);

			return true;
		}
		else
		{
			Logger::get()->warn("Window already exists!");
			return false;
		}
	}

	bool GameWindow::isClosed()
	{
		return isDestroyed_ || window_ == nullptr;
	}

	void GameWindow::show()
	{
		if (window_ != nullptr)
			glfwShowWindow(window_);
	}

	void GameWindow::destroy()
	{
		if (window_ != nullptr)
		{
			isDestroyed_ = true;
			glfwDestroyWindow(window_);
			window_ = nullptr;
		}
	}

	GLFWwindow* GameWindow::glfwWindow()
	{
		return window_;
	}

	const char* GameWindow::title()
	{
		return title_.c_str();
	}

	Engine* GameWindow::engine()
	{
		return engine_;
	}
};
