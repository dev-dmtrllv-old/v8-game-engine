#include "ConfigManager.hpp"

#include "framework.hpp"
#include "EngineConfig.hpp"
#include "Engine.hpp"
#include "ScriptManager.hpp"
#include "GameWindow.hpp"

namespace NovaEngine
{
	namespace
	{
		namespace Parser
		{
			bool isUndefined(v8::Local<v8::Object> obj, const char* name)
			{
				return obj->Get(v8::String::NewFromUtf8(obj->CreationContext()->GetIsolate(), name))->IsUndefined();
			}

			v8::Local<v8::Object> parseObj(v8::Local<v8::Object> obj, const char* name, uint32_t defaultValue = 0)
			{
				return obj->Get(v8::String::NewFromUtf8(obj->CreationContext()->GetIsolate(), name))->ToObject();
			}

			uint32_t parseUint(v8::Local<v8::Object> obj, const char* name, uint32_t defaultValue = 0)
			{
				v8::Local<v8::Value> val = obj->Get(v8::String::NewFromUtf8(obj->CreationContext()->GetIsolate(), name));
				if (val->IsUndefined())
					return defaultValue;
				return val->Uint32Value();
			}

			int32_t parseInt(v8::Local<v8::Object> obj, const char* name, int32_t defaultValue = 0)
			{
				v8::Local<v8::Value> val = obj->Get(v8::String::NewFromUtf8(obj->CreationContext()->GetIsolate(), name));
				if (val->IsUndefined())
					return defaultValue;
				return val->Int32Value();
			}

			std::string parseString(v8::Local<v8::Object> obj, const char* name, const char* defaultValue = "")
			{
				v8::Local<v8::Value> val = obj->Get(v8::String::NewFromUtf8(obj->CreationContext()->GetIsolate(), name));

				if (val->IsUndefined())
					return std::string(defaultValue);

				v8::String::Utf8Value utf8Val(val->ToString());
				return std::string(*utf8Val);
			}

			bool parseBool(v8::Local<v8::Object> obj, const char* name, bool defaultValue = false)
			{
				v8::Local<v8::Value> val = obj->Get(v8::String::NewFromUtf8(obj->CreationContext()->GetIsolate(), name));

				if (val->IsUndefined())
					return defaultValue;
				return val->BooleanValue();
			}

			v8::Local<v8::Object> getArrayVal(v8::Local<v8::Object> obj, const char* name, size_t index)
			{
				return obj->Get(v8::String::NewFromUtf8(obj->CreationContext()->GetIsolate(), name))->ToObject()->Get(index)->ToObject();
			}

			size_t getArrayLength(v8::Local<v8::Object> obj, const char* name)
			{
				return obj.As<v8::Array>()->Length();
			}
		};

	};

	bool ConfigManager::parseConfigObject(const v8::Local<v8::Object>& config)
	{
		if (!isConfigured_)
		{
			using namespace v8;

			gameConfig_.name = Parser::parseString(config, "name", "MISSING NAME");

			if (!Parser::isUndefined(config, "window"))
			{
				Local<Object> windowObj = Parser::parseObj(config, "window");
				gameConfig_.window.minWidth = Parser::parseUint(windowObj, "minWidth", DefaultConfig::GameWindow::minWidth);
				gameConfig_.window.maxWidth = Parser::parseUint(windowObj, "maxWidth", DefaultConfig::GameWindow::maxWidth);
				gameConfig_.window.minHeight = Parser::parseUint(windowObj, "minHeight", DefaultConfig::GameWindow::minHeight);
				gameConfig_.window.maxHeight = Parser::parseUint(windowObj, "maxHeight", DefaultConfig::GameWindow::maxHeight);
				gameConfig_.window.height = Parser::parseUint(windowObj, "height", DefaultConfig::GameWindow::height);
				gameConfig_.window.width = Parser::parseUint(windowObj, "width", DefaultConfig::GameWindow::width);
				gameConfig_.window.resizable = Parser::parseBool(windowObj, "resizable", DefaultConfig::GameWindow::resizable);
				gameConfig_.window.fullscreen = Parser::parseBool(windowObj, "fullscreen", DefaultConfig::GameWindow::fullscreen);
				gameConfig_.window.maximized = Parser::parseBool(windowObj, "maximized", DefaultConfig::GameWindow::maximized);
				gameConfig_.window.hidden = Parser::parseBool(windowObj, "hidden", DefaultConfig::GameWindow::hidden);

				if (!Parser::isUndefined(windowObj, "graphics"))
				{
					Local<Object> graphicsObj = Parser::parseObj(windowObj, "graphics");

					if (!Parser::isUndefined(graphicsObj, "clearColor"))
					{
						gameConfig_.window.graphics.clearColor[0] = Parser::getArrayVal(graphicsObj, "clearColor", 0)->NumberValue();
						gameConfig_.window.graphics.clearColor[1] = Parser::getArrayVal(graphicsObj, "clearColor", 1)->NumberValue();
						gameConfig_.window.graphics.clearColor[2] = Parser::getArrayVal(graphicsObj, "clearColor", 2)->NumberValue();
						gameConfig_.window.graphics.clearColor[3] = Parser::getArrayVal(graphicsObj, "clearColor", 3)->NumberValue();
					}
					else
					{
						gameConfig_.window.graphics.clearColor[0] = 0.0f;
						gameConfig_.window.graphics.clearColor[1] = 0.0f;
						gameConfig_.window.graphics.clearColor[2] = 0.0f;
						gameConfig_.window.graphics.clearColor[3] = 1.0f;
					}
				}
			}

			if (!Parser::isUndefined(config, "jobSystem"))
			{
				Local<Object> jobSysObj = Parser::parseObj(config, "jobSystem");
				gameConfig_.jobSystem.maxJobs = Parser::parseUint(jobSysObj, "maxJobs", DefaultConfig::JobSystem::maxJobs);
				gameConfig_.jobSystem.executionThreads = Parser::parseUint(jobSysObj, "executionThreads", DefaultConfig::JobSystem::executionThreads);
			}

			isConfigured_ = true;
			return true;
		}

		return false;
	}

	bool ConfigManager::onInitialize(v8::Global<v8::Object>* config)
	{
		if (!isConfigured_)
		{
			config_ = config;
			engine()->scriptManager.run([&](ScriptManager::RunInfo& info) {
				isConfigured_ = parseConfigObject(config->Get(info.isolate)->ToObject(info.isolate));
			});
		}

		return isConfigured_;
	}

	bool ConfigManager::onTerminate()
	{
		return true;
	}
};
