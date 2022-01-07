#pragma once

#include "framework.hpp"
#include "SubSystem.hpp"

#define SCRIPT_METHOD(name) static void name(const v8::FunctionCallbackInfo<v8::Value>& args)

typedef const v8::FunctionCallbackInfo<v8::Value>& CallbackArgs;

namespace NovaEngine
{
	class ScriptManager;

	typedef void(*ScriptManagerGlobalInitializer)(ScriptManager* manager, const v8::Local<v8::Object>&);

	template<typename T>
	class PrototypeBuilder
	{
	private:
		v8::Isolate* isolate;
		v8::Local<v8::FunctionTemplate> classTemplate;
		v8::Local<v8::ObjectTemplate> prototype;
	public:
		PrototypeBuilder(v8::Isolate* isolate, v8::Local<v8::FunctionTemplate> classTemplate) : isolate(isolate), classTemplate(classTemplate), prototype(classTemplate->PrototypeTemplate()) {}

		template<typename FunctionCallback>
		void SetFunction(const char* name, FunctionCallback callback)
		{
			prototype->Set(v8::String::NewFromUtf8(isolate, name), v8::FunctionTemplate::New(isolate, callback)->GetFunction());
		}

		v8::Persistent<v8::Function> build()
		{
			return v8::Persistent<v8::Function>(isolate, classTemplate->GetFunction());
		}
	};

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

		class IClassData
		{
			public:
				IClassData() = delete;
				IClassData(const v8::FunctionCallbackInfo<v8::Value>& args) {}
				virtual ~IClassData() {};
		};

		template<typename Data>
		class ClassDataWrapper
		{
		private:
			v8::Persistent<v8::Object> obj_;
			Data* data_;

		public:
			ClassDataWrapper(v8::Isolate* isolate, v8::Local<v8::Object> obj, Data* data) : obj_(), data_(data)
			{
				obj->SetInternalField(0, v8::External::New(isolate, this));
				obj_.Reset(isolate, obj);
				obj_.SetWeak(this, [](const v8::WeakCallbackInfo<ClassDataWrapper<Data>>& data) {
					ClassDataWrapper<Data>* ctx = data.GetParameter();
					if (!ctx->obj().IsEmpty())
					{
						ctx->obj().ClearWeak();
						ctx->obj().Reset();
					}
					ctx->deleteData();
				}, v8::WeakCallbackType::kParameter);
			}

			v8::Persistent<v8::Object>& obj() { return obj_; }
			Data* data() { return data_; }

			void deleteData()
			{
				if (data_ != nullptr)
				{
					delete data_;
					data_ = nullptr;
				}
			}

			bool isDeleted() { return data_ == nullptr; }
		};

		template<typename T, typename std::enable_if<std::is_base_of<IClassData, T>::value>::type* = nullptr>
		v8::Local<v8::FunctionTemplate> createClass(v8::Isolate* isolate, const char* className)
		{
			v8::Local<v8::FunctionTemplate> funcTemp = v8::FunctionTemplate::New(isolate, [](const v8::FunctionCallbackInfo<v8::Value>& args) {
				v8::Isolate* isolate = args.GetIsolate();
				if (!args.IsConstructCall())
				{
					isolate->ThrowException(v8::String::NewFromUtf8(isolate, "Cannot call constructor as function!"));
				}
				else
				{
					ClassDataWrapper<T>* t = new ClassDataWrapper<T>(isolate, args.This(), new T(args));
				}
			});
			funcTemp->SetClassName(v8::String::NewFromUtf8(isolate, className));
			funcTemp->InstanceTemplate()->SetInternalFieldCount(1);
			return funcTemp;
		}

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
		static ScriptManager* callbackDataToScriptManager(const v8::Local<v8::Context>& ctx, const v8::Local<v8::Value>&);

		static std::unique_ptr<v8::Platform> platform_;

		v8::Isolate::CreateParams createParams_;
		v8::Isolate* isolate_;
		v8::Global<v8::Context> context_;
		v8::Global<v8::Number> scriptManagerReference_;
		std::unordered_map<std::string, v8::Global<v8::Object>> modules_;
		size_t moduleRequireCounter_;

		std::string getRelativePath(const std::string& str);

	protected:
		static std::vector<ScriptManager*> instances_;

		bool runScript(v8::Local<v8::Context> context, const char* scriptString);

		bool onInitialize(ScriptManagerGlobalInitializer globalInitializer);
		bool onTerminate();

	public:
		v8::Local<v8::String> createString(const char* string);
		v8::Local<v8::Number> createNumber(uint32_t num);
		v8::Local<v8::Number> createNumber(int32_t num);
		v8::Local<v8::Boolean> createBool(bool boolean);
		v8::Local<v8::Array> createArray();
		v8::Local<v8::Object> createObject();
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

			RunInfo runInfo(this, isolate_, context);
			callback(runInfo);
		}

		void createClass()
		{

		}

	private:
		void handleRequire(const v8::FunctionCallbackInfo<v8::Value>& args);
	};
}
