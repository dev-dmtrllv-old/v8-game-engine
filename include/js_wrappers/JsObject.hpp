#pragma once

#pragma once

#include "framework.hpp"
#include "Types.hpp"
#include "js_wrappers/JsMethodHelpers.hpp"

namespace NovaEngine
{
	class Engine;

	namespace JsWrappers
	{
		class JsObject
		{
		public:
			class Builder
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
				Builder(v8::Local<v8::Object> boundObject, v8::Isolate* isolate) :
					boundObject(boundObject),
					isolate(isolate),
					ctx(isolate->GetCurrentContext())
				{}
				Builder(v8::Isolate* isolate) :
					boundObject(v8::Object::New(isolate)),
					isolate(isolate),
					ctx(isolate->GetCurrentContext())
				{}

				Builder& set(const char* name, int num)
				{
					boundObject->Set(ctx, newString(name), v8::Number::New(isolate, num));
					return *this;
				}

				Builder& set(const char* name, unsigned int num)
				{
					boundObject->Set(ctx, newString(name), v8::Number::New(isolate, num));
					return *this;
				}

				Builder& set(const char* name, float num)
				{
					boundObject->Set(ctx, newString(name), v8::Number::New(isolate, num));
					return *this;
				}

				Builder& set(const char* name, double num)
				{
					boundObject->Set(ctx, newString(name), v8::Number::New(isolate, num));
					return *this;
				}

				Builder& set(const char* name, const char* string)
				{
					boundObject->Set(ctx, newString(name), newString(string));
					return *this;
				}

				Builder& set(const char* name, bool boolean)
				{
					boundObject->Set(ctx, newString(name), v8::Boolean::New(isolate, boolean));
					return *this;
				}

				Builder& set(const char* name, v8::FunctionCallback func)
				{
					boundObject->Set(ctx, newString(name), v8::Function::New(isolate, func));
					return *this;
				}

				template <typename T>
				Builder& set(const char* name, T param)
				{
					boundObject->Set(ctx, newString(name), param);
					return *this;
				}

				template<Types::DerivedFrom<JsObject> T>
				void set(const char* name)
				{
					T obj = T();
					boundObject->Set(ctx, newString(name), obj.create(boundObject->GetIsolate()));
				}

				Builder newObject(const char* name)
				{
					v8::Local<v8::Object> o = v8::Object::New(isolate);
					boundObject->Set(ctx, newString(name), o);
					return Builder(o, isolate);
				}

				v8::Local<v8::Object> build()
				{
					return boundObject;
				}
			};

			virtual void build(Builder& builder) = 0;

			v8::Local<v8::Object> create(v8::Isolate* isolate)
			{
				Builder builder = Builder(isolate);
				build(builder);
				return builder.build();
			}

		};
	};
};
