#include "SceneManager.hpp"
#include "Engine.hpp"
#include "Logger.hpp"

namespace NovaEngine
{
	bool SceneManager::onInitialize()
	{
		Logger* logger = Logger::get();

		Engine* e = engine();
		e->scriptManager.run([&](const ScriptManager::RunInfo& info) {
			using namespace v8;

			Local<Object> o = e->configManager.getJSConfig()->Get(info.isolate);
			Local<Object> scenes = o->Get(String::NewFromUtf8(info.isolate, "scenes"))->ToObject(info.isolate);

			ScriptManager::iterateObjectKeys(scenes, [&](const char* key, Local<Value>& sceneValue) {
				Local<Function> sceneClass = sceneValue.As<Function>();
				Local<Object> scene = sceneClass->CallAsConstructor(info.context, 0, nullptr).ToLocalChecked()->ToObject(info.isolate);
				addJSScene(key, info.isolate, scene);
			});
		});
		return true;
	}

	void SceneManager::addJSScene(const char* name, v8::Isolate* isolate, v8::Local<v8::Object> jsScene)
	{
		scenes_.emplace(name, Scene(engine(), isolate, jsScene));
	}

	bool SceneManager::onTerminate()
	{
		this->scenes_.clear();
		return true;
	}

	bool SceneManager::loadScene(const char* sceneName)
	{
		for (std::pair<const std::string, Scene>& p : scenes_)
		{
			if (strcmp(p.first.c_str(), sceneName) == 0)
			{
				if (&p.second == activeScene_)
				{
					return false;
				}
				else
				{
					p.second.load();
					activeScene_ = &p.second;
				}
			}
		}
		return false;
	}
};
