#include "ScriptManager.hpp"
#include "Engine.hpp"
#include "js_wrappers/Wrappers.hpp"
#include "Transform.hpp"
#include "SpriteRenderer.hpp"

namespace NovaEngine
{
	std::unique_ptr<v8::Platform> ScriptManager::platform_;

	bool ScriptManager::onInitialize()
	{
		platform_ = v8::platform::NewDefaultPlatform(1);
		v8::V8::InitializePlatform(platform_.get());
		v8::V8::Initialize();

		{
			createParams_.array_buffer_allocator = v8::ArrayBuffer::Allocator::NewDefaultAllocator();
			isolate_ = v8::Isolate::New(createParams_);
			isolate_->SetData(0, engine());

			v8::HandleScope handleScope(isolate_);

			v8::Local<v8::ObjectTemplate> global = v8::ObjectTemplate::New(isolate_);
			v8::Local<v8::Context> context = v8::Context::New(isolate_, NULL, global);
			componentHashSymbol_.Reset(isolate_, v8::Private::New(isolate_, v8::String::NewFromUtf8(isolate_, "__COMPONENT_BITMASK__")));
			context_.Reset(isolate_, context);
		}

		run([&](const RunInfo& info) {
			initializeGlobal(info.context->Global());
		});

		engine()->eventManager.on(Hasher::hash("componentRegistered"), [](EventManager::EventData& e) {
			Hash hash = reinterpret_cast<Hash>(e.data);
			e.engine->scriptManager.run([&](const RunInfo& info) {
				v8::Local<v8::Function> componentClass = info.scriptManager->getComponentClass(hash)->GetFunction();
				componentClass->SetPrivate(info.context, info.scriptManager->getComponentHashSymbol(), v8::External::New(info.isolate, reinterpret_cast<void*>(hash)));
				std::cout << "component registered with hash " << hash << std::endl;
			});
		});

		return true;
	}

	bool ScriptManager::onTerminate()
	{
		resetGlobalMap(registeredClasses_);
		resetGlobalMap(modules_);

		componentHashSymbol_.Reset();
		context_.Reset();
		scriptManagerReference_.Reset();

		isolate_->Dispose();

		delete createParams_.array_buffer_allocator;

		v8::V8::Dispose();
		v8::V8::ShutdownPlatform();
		Logger::get()->info("V8 Disposed!");

		return true;
	}

	void ScriptManager::initializeGlobal(v8::Local<v8::Object> globalObj)
	{
		using namespace JsWrappers;
		using Func = v8::Local<v8::Function>;

		JsWrappers::JsObject::Builder global = JsWrappers::JsObject::Builder(globalObj, isolate());

		global.set("require", v8::Function::New(isolate_, onRequire, scriptManagerReference_.Get(isolate_)));
		global.set<JsEngine>("Engine");
		global.set<JsAssetManager>("AssetManager");

		Func transformClass = registerComponent<JsWrappers::JsTransform, Transform>("Transform");
		Func spriteRendererClass = registerComponent<JsWrappers::JsSpriteRenderer, SpriteRenderer>("SpriteRenderer");
		Func gameObjectClass = registerClass<JsWrappers::JsGameObject>("GameObject");
		Func sceneClass = registerClass<JsWrappers::JsScene>("Scene");
		Func assetClass = registerClass<JsWrappers::JsAsset>("Asset");

		global.set("Transform", transformClass);
		global.set("SpriteRenderer", spriteRendererClass);
		global.set("GameObject", gameObjectClass);
		global.set("Scene", sceneClass);
		global.set("Asset", assetClass);
	}

	v8::Isolate* ScriptManager::isolate() { return isolate_; }
	v8::Local<v8::Context> ScriptManager::context() { return context_.Get(isolate_); }

	std::string ScriptManager::getRelativePath(const std::string& str)
	{
		static std::string scriptPath = Utils::Path::combine(engine()->executablePath(), "assets", "scripts");
		static size_t l = scriptPath.length();

		std::string testPath = Utils::Path::combine(engine()->executablePath(), "assets", "scripts", str);

		char* formattedPath = realpath(testPath.c_str(), NULL);

		size_t copySize = strlen(formattedPath) - l - 1;

		std::string relativePath = std::string(&formattedPath[l + 1]);

		free(formattedPath);

		return relativePath;
	}

	void ScriptManager::onRequire(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		fetchEngineFromArgs(args)->scriptManager.handleRequire(args);
	}

	void ScriptManager::handleRequire(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		v8::Isolate* isolate = args.GetIsolate();
		v8::Local<v8::Context> ctx = isolate->GetCurrentContext();

		v8::HandleScope scope(isolate);

		if (args.Length() < 1)
		{

		}
		else if (args[0]->IsString())
		{
			v8::Local<v8::Object> exports = ctx->Global()->Get(V8STR("exports"))->ToObject(isolate_);
			v8::Local<v8::String> absPathStr = exports->Get(V8STR("__ABSOLUTE_PATH__"))->ToString(isolate_);

			v8::String::Utf8Value absPathVal(isolate, absPathStr);

			size_t l = absPathVal.length();

			char absPathCpy[l] = {};
			strcpy(absPathCpy, *absPathVal);

			while (l > 0)
			{
				if (absPathCpy[l] == '/')
				{
					absPathCpy[l] = '\0';
					break;
				}
				l--;
			}


			std::string modulePath(l == 0 ? "" : absPathCpy);

			v8::Handle<v8::String> requirePath = v8::Handle<v8::String>::Cast(args[0]);
			v8::String::Utf8Value output(isolate, requirePath);
			std::string path(*output);

			bool isJsonModule = false;

			if (strcmp(std::filesystem::path(path.c_str()).extension().c_str(), ".json") == 0)
			{
				isJsonModule = true;
				modulePath += path;
			}
			else
			{
				modulePath += path + ".js";
			}

			std::string relativePath = getRelativePath(modulePath).c_str();
			const char* cleanModulePath = relativePath.c_str();

			if (modules_.find(cleanModulePath) == modules_.end())
			{
				moduleRequireCounter_++;
				load(cleanModulePath, isJsonModule);
				moduleRequireCounter_--;

				if (modules_.find(cleanModulePath) == modules_.end())
				{
					Logger::get()->warn("could not get exports!");
					return;
				}
				else
				{
					args.GetReturnValue().Set(modules_[cleanModulePath].Get(isolate));
					return;

				}
			}
			else
			{
				args.GetReturnValue().Set(modules_[cleanModulePath].Get(isolate));
				return;
			}
		}

		args.GetReturnValue().Set(v8::Undefined(isolate));
	}

	void ScriptManager::load(const char* path, bool isJsonModule)
	{
		if (modules_.find(path) == modules_.end())
		{
			v8::Isolate* isolate = isolate_;
			v8::Locker isolateLocker(isolate_);
			v8::Isolate::Scope isolate_scope(isolate_);

			v8::HandleScope handle_scope(isolate_);
			v8::Local<v8::Context> context = v8::Local<v8::Context>::New(isolate_, context_);
			v8::Context::Scope context_scope(context);

			std::vector<char> content = { '\0' };
			std::string scriptPath = Utils::Path::combine("scripts", path).string();

			this->engine()->assetManager.loadTextFile(scriptPath.c_str(), content);

			v8::Local<v8::Object> global = context->Global();
			v8::Local<v8::Object> exports = v8::Object::New(isolate_);
			v8::MaybeLocal<v8::Value> prevExports = global->Get(context, V8STR("exports"));

			if (!exports->Set(context, V8STR("__ABSOLUTE_PATH__"), V8STR(path)).ToChecked())
			{
				Logger::get()->warn("Failed to set __ABSOLUTE_PATH__!");
				if (!prevExports.IsEmpty() && (moduleRequireCounter_ != 0))
					global->Set(context, V8STR("exports"), prevExports.ToLocalChecked()->ToObject());
			}

			if (!global->Set(context, V8STR("exports"), exports).ToChecked())
			{
				Logger::get()->warn("Failed to set exports!");
				if (!prevExports.IsEmpty() && (moduleRequireCounter_ != 0))
					global->Set(context, V8STR("exports"), prevExports.ToLocalChecked()->ToObject());
			}

			modules_[std::string(path)].Reset(isolate_, exports);

			std::string scriptContents = isJsonModule ? std::string("exports.data = ") + content.data() : content.data();

			v8::Local<v8::String> source = v8::String::NewFromUtf8(isolate_, scriptContents.c_str(), v8::NewStringType::kNormal).ToLocalChecked();

			v8::Local<v8::Script> script = v8::Script::Compile(context, source).ToLocalChecked();

			script->Run(context);

			if (isJsonModule)
				modules_[std::string(path)].Reset(isolate_, exports->Get(context, V8STR("data")).ToLocalChecked()->ToObject());

			if (!prevExports.IsEmpty() && (moduleRequireCounter_ != 0))
				global->Set(context, V8STR("exports"), prevExports.ToLocalChecked()->ToObject());
		}
	}

	Engine* ScriptManager::fetchEngineFromArgs(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		return static_cast<Engine*>(args.GetIsolate()->GetData(0));
	}

	Engine* ScriptManager::fetchEngineFromArgs(const v8::PropertyCallbackInfo<v8::Value>& args)
	{
		return static_cast<Engine*>(args.GetIsolate()->GetData(0));
	}

	Engine* ScriptManager::fetchEngineFromArgs(const v8::PropertyCallbackInfo<void>& args)
	{
		return static_cast<Engine*>(args.GetIsolate()->GetData(0));
	}

	Hash ScriptManager::getComponentHash(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		Engine* engine = fetchEngineFromArgs(args);
		v8::Local<v8::Value> val = args[0].As<v8::Object>()->GetPrivate(args.GetIsolate()->GetCurrentContext(), engine->scriptManager.getComponentHashSymbol()).ToLocalChecked();
		Hash bitMask = reinterpret_cast<Hash>(val.As<v8::External>()->Value());
		return bitMask;
	}

	v8::Local<v8::String> ScriptManager::objectToString(v8::Isolate* isolate, const v8::Local<v8::Value>& o)
	{
		return v8::JSON::Stringify(isolate->GetCurrentContext(), o).ToLocalChecked();
	}

	v8::Local<v8::FunctionTemplate> ScriptManager::getComponentClass(Hash hash)
	{
		assert(registeredComponents_.contains(hash));
		return registeredComponents_[hash]->Get(isolate());
	}

	v8::Local<v8::Private> ScriptManager::getComponentHashSymbol()
	{
		return componentHashSymbol_.Get(isolate());
	}

	void ScriptManager::printObject(v8::Isolate* isolate, const v8::Local<v8::Value>& o, const char* name)
	{
		using namespace v8;

		static size_t tabs = 0;

		std::string buf;

		if (tabs == 0)
		{
			puts("{");
			tabs = 1;
		}

		for (size_t i = 0; i < tabs; i++)
			printf("    ");


		String::Utf8Value typeVal(o->TypeOf(isolate));

		if (name != nullptr)
		{
			std::string n = std::string(name);
			n += std::string(": ") + *typeVal;
			printf("%s", n.c_str());
		}

		tabs++;

		printf(",\n");

		if (o->IsObject())
		{
			for (size_t i = 0; i < tabs - 1; i++)
				printf("    ");
			puts("{");
			Local<Object> obj = o->ToObject();
			ScriptManager::iterateObjectKeys(obj, [&](char* k, const Local<Value> val) {
				printObject(isolate, val, k);
			});
			for (size_t i = 0; i < tabs - 1; i++)
				printf("    ");
			puts("},");
		}
		else if (o->IsFunction())
		{
			String::Utf8Value val(o->ToString(isolate));
			printf("%s,", *val);
		}
		else if (o->IsArray())
		{
			for (size_t i = 0; i < tabs - 1; i++)
				printf("    ");
			puts("[");
			Local<Object> obj = o->ToObject();
			ScriptManager::iterateObjectKeys(obj, [&](char* k, const Local<Value> val) {
				printObject(isolate, val, nullptr);
			});
			for (size_t i = 0; i < tabs - 1; i++)
				printf("    ");
			puts("],");
		}

		tabs--;

		if (tabs == 1)
		{
			puts("}");
		}
	}

};
