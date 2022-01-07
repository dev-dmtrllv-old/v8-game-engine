#include "ScriptManager.hpp"
#include "Engine.hpp"

namespace NovaEngine
{
	std::vector<ScriptManager*> ScriptManager::instances_;
	std::unique_ptr<v8::Platform> ScriptManager::platform_;

	bool ScriptManager::onInitialize(ScriptManagerGlobalInitializer globalInitializer)
	{
		if (instances_.size() == 0)
		{
			platform_ = v8::platform::NewDefaultPlatform();
			v8::V8::InitializePlatform(platform_.get());
			v8::V8::Initialize();
		}

		{
			createParams_.array_buffer_allocator = v8::ArrayBuffer::Allocator::NewDefaultAllocator();
			isolate_ = v8::Isolate::New(createParams_);
			isolate_->SetData(0, engine());

			v8::HandleScope handleScope(isolate_);

			v8::Local<v8::ObjectTemplate> global = v8::ObjectTemplate::New(isolate_);
			v8::Local<v8::Context> context = v8::Context::New(isolate_, NULL, global);

			context_.Reset(isolate_, context);
		}

		run([&](const RunInfo& info) {
			uint32_t index = static_cast<uint32_t>(std::distance(instances_.begin(), std::find(instances_.begin(), instances_.end(), this)));
			scriptManagerReference_.Reset(isolate_, v8::Number::New(isolate_, index));
			v8::Local<v8::Object> global = info.context->Global();
			globalInitializer(this, global);
			global->Set(info.context, createString("require"), createFunction(onRequire));
		});

		instances_.push_back(this);

		return true;
	}

	bool ScriptManager::onTerminate()
	{
		for (std::pair<const std::string, v8::Global<v8::Object>>& mod : modules_)
			mod.second.Reset();

		instances_.erase(std::find(instances_.begin(), instances_.end(), this));

		context_.Reset();
		scriptManagerReference_.Reset();

		isolate_->Dispose();

		delete createParams_.array_buffer_allocator;

		if (instances_.size() == 0)
		{
			v8::V8::Dispose();
			v8::V8::ShutdownPlatform();
			Logger::get()->info("V8 Disposed!");
		}

		return true;
	}

#pragma region v8 type creators

	v8::Local<v8::String> ScriptManager::createString(const char* string)
	{
		return v8::String::NewFromUtf8(isolate_, string, v8::NewStringType::kNormal).ToLocalChecked();
	}

	v8::Local<v8::Number> ScriptManager::createNumber(uint32_t num)
	{
		return v8::Number::New(isolate_, num);
	}

	v8::Local<v8::Number> ScriptManager::createNumber(int32_t num)
	{
		return v8::Number::New(isolate_, num);
	}

	v8::Local<v8::Boolean> ScriptManager::createBool(bool boolean)
	{
		return v8::Boolean::New(isolate_, boolean);
	}

	v8::Local<v8::Array> ScriptManager::createArray()
	{
		return v8::Array::New(isolate_);
	}

	v8::Local<v8::Object> ScriptManager::createObject()
	{
		return v8::Object::New(isolate_);
	}

	v8::Local<v8::Function> ScriptManager::createFunction(v8::FunctionCallback cb)
	{
		return v8::Function::New(isolate_, cb, scriptManagerReference_.Get(isolate_));
	}

#pragma endregion

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

	ScriptManager* ScriptManager::callbackDataToScriptManager(const v8::Local<v8::Context>& ctx, const v8::Local<v8::Value>& data)
	{
		return instances_.at(data.As<v8::Number>()->Uint32Value(ctx).ToChecked());
	}

	void ScriptManager::onRequire(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		v8::Isolate* isolate = args.GetIsolate();
		v8::Local<v8::Context> ctx = isolate->GetCurrentContext();
		callbackDataToScriptManager(ctx, args.Data())->handleRequire(args);
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
			v8::Local<v8::Object> exports = ctx->Global()->Get(createString("exports"))->ToObject(isolate_);
			v8::Local<v8::String> absPathStr = exports->Get(createString("__ABSOLUTE_PATH__"))->ToString(isolate_);

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
			v8::MaybeLocal<v8::Value> prevExports = global->Get(context, createString("exports"));

			if (!exports->Set(context, createString("__ABSOLUTE_PATH__"), createString(path)).ToChecked())
			{
				Logger::get()->warn("Failed to set __ABSOLUTE_PATH__!");
				if (!prevExports.IsEmpty() && (moduleRequireCounter_ != 0))
					global->Set(context, createString("exports"), prevExports.ToLocalChecked()->ToObject());
			}

			if (!global->Set(context, createString("exports"), exports).ToChecked())
			{
				Logger::get()->warn("Failed to set exports!");
				if (!prevExports.IsEmpty() && (moduleRequireCounter_ != 0))
					global->Set(context, createString("exports"), prevExports.ToLocalChecked()->ToObject());
			}

			modules_[std::string(path)].Reset(isolate_, exports);

			std::string scriptContents = isJsonModule ? std::string("exports.data = ") + content.data() : content.data();

			v8::Local<v8::String> source = v8::String::NewFromUtf8(isolate_, scriptContents.c_str(), v8::NewStringType::kNormal).ToLocalChecked();

			v8::Local<v8::Script> script = v8::Script::Compile(context, source).ToLocalChecked();

			script->Run(context);

			if (isJsonModule)
				modules_[std::string(path)].Reset(isolate_, exports->Get(context, createString("data")).ToLocalChecked()->ToObject());

			if (!prevExports.IsEmpty() && (moduleRequireCounter_ != 0))
				global->Set(context, createString("exports"), prevExports.ToLocalChecked()->ToObject());
		}
	}

	Engine* ScriptManager::fetchEngineFromArgs(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		return static_cast<Engine*>(args.GetIsolate()->GetData(0));
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
