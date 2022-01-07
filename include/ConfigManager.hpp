#pragma once

#include "framework.hpp"
#include "SubSystem.hpp"
#include "EngineConfig.hpp"

namespace NovaEngine
{
	class Engine;

	class ConfigManager : public SubSystem<v8::Global<v8::Object>*>
	{
	private:
		v8::Global<v8::Object>* config_;
		GameConfig gameConfig_;
		bool isConfigured_;

		bool parseConfigObject(const v8::Local<v8::Object>& configuration);

		ENGINE_SUB_SYSTEM_CTOR(ConfigManager), gameConfig_(), isConfigured_(false) {}

	protected:
		bool onInitialize(v8::Global<v8::Object>* config);
		bool onTerminate();

	public:
		inline std::string& getGameName() { return gameConfig_.name; }
		inline JobSystemConfig& getJobSystemConfig() { return gameConfig_.jobSystem; }
		inline GameWindowConfig& getWindowConfig() { return gameConfig_.window; }
		inline v8::Global<v8::Object>* getJSConfig() { return config_; }
	};
};
