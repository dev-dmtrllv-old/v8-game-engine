#pragma once

#include "SubSystem.hpp"
#include "Scene.hpp"

namespace NovaEngine
{
	class SceneManager : public SubSystem<>
	{
	private:
		typedef v8::Global<v8::Object> JSScene;

		std::unordered_map<std::string, Scene> scenes_;

		Scene* activeScene_;

		void addJSScene(const char* name, v8::Isolate* isolate, v8::Local<v8::Object> jsScene);

	protected:
		ENGINE_SUB_SYSTEM_CTOR(SceneManager), scenes_(), activeScene_(nullptr) {}

	public:
		bool onInitialize();
		bool onTerminate();

		bool loadScene(const char* sceneName);
	};
};
