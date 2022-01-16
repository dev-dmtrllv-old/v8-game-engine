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
		scenes_[Hasher::hash(name)] = new Scene(engine(), isolate, jsScene);
	}

	bool SceneManager::onTerminate()
	{
		this->scenes_.clear();
		return true;
	}

	bool SceneManager::loadScene(const char* sceneName)
	{
		Hash id = Hasher::hash(sceneName);
		if(scenes_.contains(id))
		{
			scenes_[id]->load();
			activeScene_ = scenes_[id];
			return true;
		}

		return false;
	}
};
