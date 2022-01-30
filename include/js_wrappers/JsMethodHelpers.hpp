#pragma once

#define JS_METHOD(name) static void name ## _caller(const v8::FunctionCallbackInfo<v8::Value>& args, Engine* engine, v8::Isolate* isolate); \
static void name(const v8::FunctionCallbackInfo<v8::Value>& args);

#define JS_METHOD_IMPL(className, name) void className::name(const v8::FunctionCallbackInfo<v8::Value>& args) { className::name ## _caller(args, ScriptManager::fetchEngineFromArgs(args), args.GetIsolate()); } \
void className::name ## _caller(const v8::FunctionCallbackInfo<v8::Value>& args, Engine* engine, v8::Isolate* isolate) 

#define JS_GETTER(name) static void get ## name ## _caller(const v8::PropertyCallbackInfo<v8::Value>& args, Engine* engine, v8::Isolate* isolate); \
static void get ## name(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args);
#define JS_SETTER(name) static void set ## name ## _caller(const v8::PropertyCallbackInfo<void>& args, v8::Local<v8::Value> value, Engine* engine, v8::Isolate* isolate); \
static void set ## name(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& args);

#define JS_GETTER_IMPL(className, name) void className::get ## name(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args) { className::get ## name ## _caller(args, ScriptManager::fetchEngineFromArgs(args), args.GetIsolate()); } \
void className::get ## name ## _caller(const v8::PropertyCallbackInfo<v8::Value>& args, Engine* engine, v8::Isolate* isolate)

#define JS_SETTER_IMPL(className, name) void className::set ## name(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& args) { className::set ## name ## _caller(args, value, ScriptManager::fetchEngineFromArgs(args), args.GetIsolate()); } \
void className::set ## name ## _caller(const v8::PropertyCallbackInfo<void>& args, v8::Local<v8::Value> value, Engine* engine, v8::Isolate* isolate)
