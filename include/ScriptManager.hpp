#pragma once

#include "framework.hpp"
#include "SubSystem.hpp"
#include "Logger.hpp"

#define SCRIPT_METHOD(name) static void name(const v8::FunctionCallbackInfo<v8::Value>& args)
#define SCRIPT_METHOD_IMPL(className, name) void className::name(const v8::FunctionCallbackInfo<v8::Value>& args)

typedef const v8::FunctionCallbackInfo<v8::Value>& CallbackArgs;

namespace NovaEngine
{
	class ScriptManager;

	typedef void(*ScriptManagerGlobalInitializer)(ScriptManager* manager, const v8::Local<v8::Object>&);

	class ScriptManager : public SubSystem<ScriptManagerGlobalInitializer>
	{
	public:
		struct RunInfo
		{
			RunInfo(ScriptManager* m, v8::Isolate* i, v8::Local<v8::Context>& c) : scriptManager(m), isolate(i), context(c) {}
			ScriptManager* scriptManager;
			v8::Isolate* isolate;
			v8::Local<v8::Context>& context;
		};

		typedef const char* Exception;

		template<typename Callback>
		static void iterateObjectKeys(v8::Local<v8::Object> obj, Callback callback)
		{
			using namespace v8;

			Local<Context> ctx = obj->CreationContext();
			Isolate* isolate = ctx->GetIsolate();

			Local<Array> keys = obj->GetOwnPropertyNames(ctx, PropertyFilter::ALL_PROPERTIES).ToLocalChecked();

			for (uint32_t i = 0, l = keys->Length(); i < l; i++)
			{
				Local<String> key = keys->Get(ctx, i).ToLocalChecked()->ToString(ctx).ToLocalChecked();
				String::Utf8Value keyVal(key);
				Local<Value> val = obj->Get(ctx, key).ToLocalChecked();
				callback(*keyVal, val);
			}
		}

		static Engine* fetchEngineFromArgs(const v8::FunctionCallbackInfo<v8::Value>& args);

		static void printObject(v8::Isolate* isolate, const v8::Local<v8::Value>& o, const char* name = nullptr);

	private:
		ENGINE_SUB_SYSTEM_CTOR(ScriptManager),
			createParams_(),
			isolate_(nullptr),
			context_(),
			scriptManagerReference_(),
			modules_(),
			moduleRequireCounter_(0)
		{}

		static void onRequire(const v8::FunctionCallbackInfo<v8::Value>& args);

		static std::unique_ptr<v8::Platform> platform_;

		v8::Isolate::CreateParams createParams_;
		v8::Isolate* isolate_;
		v8::Global<v8::Context> context_;
		v8::Global<v8::Number> scriptManagerReference_;
		std::unordered_map<std::string, v8::Global<v8::Object>> modules_;
		size_t moduleRequireCounter_;

		std::string getRelativePath(const std::string& str);

	protected:
		bool runScript(v8::Local<v8::Context> context, const char* scriptString);

		bool onInitialize(ScriptManagerGlobalInitializer globalInitializer);
		bool onTerminate();

	public:
		v8::Local<v8::String> createString(const char* string);
		v8::Local<v8::Function> createFunction(v8::FunctionCallback);

		v8::Isolate* isolate();
		v8::Local<v8::Context> context();

		void load(const char* path, bool isJsonModule = false);

		template<typename RunCallback>
		void run(RunCallback callback)
		{
			v8::Locker isolateLocker(isolate_);
			v8::Isolate::Scope isolate_scope(isolate_);

			v8::HandleScope handleScope(isolate_);
			v8::Local<v8::Context> context = v8::Local<v8::Context>::New(isolate_, context_);
			v8::Context::Scope contextScope(context);

			v8::TryCatch tryCatch = v8::TryCatch(isolate_);

			RunInfo runInfo(this, isolate_, context);

			callback(runInfo);

			if (tryCatch.HasCaught())
			{
				v8::Local<v8::String> exceptionStr = tryCatch.Exception()->ToString(isolate_);
				v8::String::Utf8Value val = v8::String::Utf8Value(isolate_, exceptionStr);
				Logger::get()->error(*val);
				throw Exception(*val);
			}
		}

	private:
		void handleRequire(const v8::FunctionCallbackInfo<v8::Value>& args);

	public:
		class ObjectBuilder
		{
		private:
			v8::Local<v8::Object> boundObject;
			v8::Isolate* isolate;
			v8::Local<v8::Context> ctx;

			inline v8::Local<v8::String> newString(const char* str)
			{
				return v8::String::NewFromUtf8(isolate, str, v8::NewStringType::kNormal).ToLocalChecked();
			}

		public:
			ObjectBuilder(v8::Local<v8::Object> boundObject, v8::Isolate* isolate, v8::Local<v8::Context> ctx) :
				boundObject(boundObject),
				isolate(isolate),
				ctx(ctx)
			{}

			ObjectBuilder& set(const char* name, int num)
			{
				boundObject->Set(ctx, newString(name), v8::Number::New(isolate, num));
				return *this;
			}

			ObjectBuilder& set(const char* name, unsigned int num)
			{
				boundObject->Set(ctx, newString(name), v8::Number::New(isolate, num));
				return *this;
			}

			ObjectBuilder& set(const char* name, float num)
			{
				boundObject->Set(ctx, newString(name), v8::Number::New(isolate, num));
				return *this;
			}

			ObjectBuilder& set(const char* name, double num)
			{
				boundObject->Set(ctx, newString(name), v8::Number::New(isolate, num));
				return *this;
			}

			ObjectBuilder& set(const char* name, const char* string)
			{
				boundObject->Set(ctx, newString(name), newString(string));
				return *this;
			}

			ObjectBuilder& set(const char* name, bool boolean)
			{
				boundObject->Set(ctx, newString(name), v8::Boolean::New(isolate, boolean));
				return *this;
			}

			ObjectBuilder& set(const char* name, v8::FunctionCallback func)
			{
				boundObject->Set(ctx, newString(name), v8::Function::New(isolate, func));
				return *this;
			}

			template <typename T>
			ObjectBuilder& set(const char* name, T param)
			{
				boundObject->Set(ctx, newString(name), param);
				return *this;
			}

			ObjectBuilder newObject(const char* name)
			{
				v8::Local<v8::Object> o = v8::Object::New(isolate);
				boundObject->Set(ctx, newString(name), o);
				return ObjectBuilder(o, isolate, ctx);
			}
		};

		class ClassBuilder
		{
		private:
			static void onConstructCallback(const v8::FunctionCallbackInfo<v8::Value>& args) {
				v8::Isolate* isolate = args.GetIsolate();
				if (!args.IsConstructCall())
					isolate->ThrowException(v8::String::NewFromUtf8(isolate, "Cannot call constructor as function!"));
			}

			v8::Isolate* isolate;
			v8::Local<v8::FunctionTemplate> funcTemp;
			v8::Local<v8::ObjectTemplate> protoTemp;

		public:
			ClassBuilder(v8::Isolate* isolate, const char* name, size_t internalFieldCount = 0, void(*constructCallback)(V8CallbackArgs) = ClassBuilder::onConstructCallback) : isolate(isolate), funcTemp(v8::FunctionTemplate::New(isolate))
			{
				funcTemp->SetCallHandler(constructCallback);

				if (name != nullptr)
					funcTemp->SetClassName(v8::String::NewFromUtf8(isolate, name));

				protoTemp = funcTemp->PrototypeTemplate();

				if (internalFieldCount > 0)
					setInternalFieldCount(internalFieldCount);
			}

			ClassBuilder& set(const char* name, int num)
			{
				protoTemp->Set(isolate, name, v8::Number::New(isolate, num));
				return *this;
			}

			ClassBuilder& set(const char* name, unsigned int num)
			{
				protoTemp->Set(isolate, name, v8::Number::New(isolate, num));
				return *this;
			}

			ClassBuilder& set(const char* name, float num)
			{
				protoTemp->Set(isolate, name, v8::Number::New(isolate, num));
				return *this;
			}

			ClassBuilder& set(const char* name, double num)
			{
				protoTemp->Set(isolate, name, v8::Number::New(isolate, num));
				return *this;
			}

			ClassBuilder& set(const char* name, const char* string)
			{
				protoTemp->Set(isolate, name, v8::String::NewFromUtf8(isolate, string, v8::NewStringType::kNormal).ToLocalChecked());
				return *this;
			}

			ClassBuilder& set(const char* name, bool boolean)
			{
				protoTemp->Set(isolate, name, v8::Boolean::New(isolate, boolean));
				return *this;
			}

			ClassBuilder& set(const char* name, v8::FunctionCallback func)
			{
				protoTemp->Set(isolate, name, v8::FunctionTemplate::New(isolate, func));
				return *this;
			}

			ClassBuilder& setInternalFieldCount(size_t count)
			{
				funcTemp->InstanceTemplate()->SetInternalFieldCount(count);
				return *this;
			}

			v8::Local<v8::Function> build()
			{
				return funcTemp->GetFunction();
			}
		};
	};
}
