#pragma once

#include "framework.hpp"

namespace NovaEngine
{
	class Engine;

	class Scene
	{
	private:
		Engine* engine_;
		v8::Global<v8::Object> jsScene_;

	protected:
		void runJSMethod(const char* methodName);

	public:
		Scene(Engine* engine, v8::Isolate* isolate, v8::Local<v8::Object> jsScene);

		Engine* engine();

		void load();
		void start();
		void stop();
	};
};
