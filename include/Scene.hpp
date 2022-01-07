#pragma once

#include "framework.hpp"

namespace NovaEngine
{
	class Engine;

	class Scene
	{
		private:
			Engine* engine_;
			v8::Global<v8::Function> loadFunc_;
			v8::Global<v8::Function> startFunc_;
			v8::Global<v8::Function> stopFunc_;

		public:
			Scene(Engine* engine, v8::Isolate* isolate, v8::Local<v8::Object> jsScene);

			Engine* engine();

			void load();
			void start();
			void stop();
	};
};
