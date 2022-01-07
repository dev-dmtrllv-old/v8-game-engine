#pragma once

#include "SubSystem.hpp"
#include "framework.hpp"
#include "Utils.hpp"

namespace NovaEngine
{
	class Engine;

	class AssetManager : public SubSystem<const char*>
	{
	private:
		std::string rootDir_;

	protected:
		bool onInitialize(const char* execPath);
		bool onTerminate();

	public:
		bool loadFile(const char* assetPath, std::vector<char>& fileContents);
		bool loadTextFile(const char* assetPath, std::vector<char>& fileContents);
		bool fileExists(const char* assetPath);

	protected:
		ENGINE_SUB_SYSTEM_CTOR(AssetManager) {}

		template<typename... Parts>
		inline std::string createAbsolutePath(Parts... parts)
		{
			return Utils::Path::combine(rootDir_, parts...);
		}
	};
};
