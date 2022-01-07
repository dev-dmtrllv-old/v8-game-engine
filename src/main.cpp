#include "Engine.hpp"

#include "framework.hpp"
#include "Logger.hpp"

int main(int argc, const char** argv)
{
	using namespace NovaEngine;

	Engine& engine = Engine::get();

	if (!engine.initialize("Game.js"))
		return 1;

	engine.run();

	if (!engine.terminate())
	{
		Logger::get()->info("oops??");
		return 2;
	}

	return 0;
}
