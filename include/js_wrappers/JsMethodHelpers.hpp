#pragma once

#define JS_METHOD(name) static void name ## _caller(const v8::FunctionCallbackInfo<v8::Value>& args, Engine* engine, v8::Isolate* isolate); \
static void name(const v8::FunctionCallbackInfo<v8::Value>& args);

#define JS_METHOD_IMPL(className, name) void className::name(const v8::FunctionCallbackInfo<v8::Value>& args) { className::name ## _caller(args, ScriptManager::fetchEngineFromArgs(args), args.GetIsolate()); } \
void className::name ## _caller(const v8::FunctionCallbackInfo<v8::Value>& args, Engine* engine, v8::Isolate* isolate) 
