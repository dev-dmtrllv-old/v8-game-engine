#pragma once

#include "framework.hpp"
#include "BitMask.hpp"
#include "js_wrappers/JsMethodHelpers.hpp"

namespace NovaEngine
{
	class Engine;

	namespace JsWrappers
	{
		class JsClass
		{
			static void onConstructCallback(const v8::FunctionCallbackInfo<v8::Value>& args)
			{
				v8::Isolate* isolate = args.GetIsolate();
				if (!args.IsConstructCall())
					isolate->ThrowException(v8::String::NewFromUtf8(isolate, "Cannot call constructor as function!"));
			}

		public:
			class Builder
			{
				static void emptyCtor(V8CallbackArgs) {}

			protected:
				Builder() = delete;
				Builder(const Builder&) = delete;
				Builder(Builder&&) = delete;

				v8::Isolate* isolate;
				v8::Local<v8::FunctionTemplate> funcTemp;
				v8::Local<v8::ObjectTemplate> protoTemp;

			public:
				Builder(v8::Isolate* isolate, const char* name) : isolate(isolate), funcTemp(v8::FunctionTemplate::New(isolate))
				{
					setConstructor(emptyCtor);

					if (name != nullptr)
						funcTemp->SetClassName(v8::String::NewFromUtf8(isolate, name));

					protoTemp = funcTemp->PrototypeTemplate();
				}

				Builder& setConstructor(v8::FunctionCallback constructCallback)
				{
					funcTemp->SetCallHandler(constructCallback);
					return *this;
				}

				Builder& setConstructor(v8::FunctionCallback constructCallback, void* data)
				{
					funcTemp->SetCallHandler(constructCallback, v8::External::New(isolate, data));
					return *this;
				}

				Builder& set(const char* name, int num)
				{
					protoTemp->Set(isolate, name, v8::Number::New(isolate, num));
					return *this;
				}

				Builder& set(const char* name, unsigned int num)
				{
					protoTemp->Set(isolate, name, v8::Number::New(isolate, num));
					return *this;
				}

				Builder& set(const char* name, float num)
				{
					protoTemp->Set(isolate, name, v8::Number::New(isolate, num));
					return *this;
				}

				Builder& set(const char* name, double num)
				{
					protoTemp->Set(isolate, name, v8::Number::New(isolate, num));
					return *this;
				}

				Builder& set(const char* name, const char* string)
				{
					protoTemp->Set(isolate, name, v8::String::NewFromUtf8(isolate, string, v8::NewStringType::kNormal).ToLocalChecked());
					return *this;
				}

				Builder& set(const char* name, bool boolean)
				{
					protoTemp->Set(isolate, name, v8::Boolean::New(isolate, boolean));
					return *this;
				}

				Builder& set(const char* name, v8::FunctionCallback func)
				{
					protoTemp->Set(isolate, name, v8::FunctionTemplate::New(isolate, func));
					return *this;
				}

				Builder& setInternalFieldCount(size_t count)
				{
					funcTemp->InstanceTemplate()->SetInternalFieldCount(count);
					return *this;
				}

				v8::Local<v8::FunctionTemplate> build()
				{
					return funcTemp;
				}

				friend class NovaEngine::JsWrappers::JsClass;
			};

			virtual void build(Engine* engine, Builder& builder) = 0;

			v8::Local<v8::FunctionTemplate> create(Engine* engine, v8::Isolate* isolate, const char* name)
			{
				Builder builder = Builder(isolate, name);
				build(engine, builder);
				return builder.build();
			}
		};
	};
};
