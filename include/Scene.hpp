#pragma once

#include "framework.hpp"
#include "Entity.hpp"
#include "ScriptManager.hpp"
#include "Hash.hpp"

namespace NovaEngine
{
	class Engine;

	class Scene
	{
	private:
		Engine* engine_;
		v8::Global<v8::Function> gameObjectTemplate_;
		v8::Global<v8::Object> jsScene_;
		std::mutex spawnMutex_;
		Entity::IDCounter entityCounter_;
		std::vector<Entity> entities_;
		std::unordered_map<Hash, Entity*> namedEntities_;

	protected:
		void runJSMethod(const char* methodName);

	public:
		Scene(Engine* engine, v8::Isolate* isolate, v8::Local<v8::Object> jsScene);

		Engine* engine();

		void load();
		void start();
		void stop();

		Entity* spawn(size_t n = 1);

		CLASS_SCRIPT_METHOD(onSpawnGameObject);

		v8::Local<v8::Object> jsScene(v8::Isolate* isolate);
	};
};
