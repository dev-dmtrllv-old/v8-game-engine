#pragma once

#include "Terminatable.hpp"

#define ENGINE_SUB_SYSTEM_CTOR(name) friend class NovaEngine::Engine; \
name(Engine* engine) : SubSystem(engine)

namespace NovaEngine
{
	class Engine;

	template<typename... Args>
	class SubSystem;

	template<typename... Args>
	class SubSystem : public Terminatable
	{
	private:
		bool isInitialized_ = false;
		Engine* engine_;

	public:
		Engine* engine() { return engine_; };
		bool isInitialized() { return isInitialized_; }
		bool isReady() { return isInitialized_ && !isTerminated(); }

	protected:
		SubSystem(Engine* engine) : engine_(engine) {}

		virtual bool onInitialize(Args ...args) = 0;
		virtual bool onTerminate() = 0;

	private:
		bool initialize(Args ...args)
		{
			if (!isInitialized_)
			{
				if (!onInitialize(args...))
					return false;

				isInitialized_ = true;
			}
			return true;
		}

		friend class Engine;
	};

	template<>
	class SubSystem<> : public Terminatable
	{
	private:
		bool isInitialized_ = false;
		Engine* engine_;

	public:
		Engine* engine() { return engine_; };
		bool isInitialized() { return isInitialized_; }
		bool isReady() { return isInitialized_ && !isTerminated(); }

	protected:
		SubSystem(Engine* engine) : engine_(engine) {}

		virtual bool onInitialize() = 0;
		virtual bool onTerminate() = 0;
		
	private:

		bool initialize()
		{
			if (!isInitialized_)
			{
				if (!onInitialize())
					return false;

				isInitialized_ = true;
			}
			return true;
		}

		friend class Engine;
	};
};
