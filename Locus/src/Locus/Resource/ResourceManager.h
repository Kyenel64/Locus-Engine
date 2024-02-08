#pragma once

#include "Locus/Core/UUID.h"

namespace Locus
{
	enum class ResourceType { None = 0, Texture = 1, Material = 2, Model = 3, Scene = 4, Script = 5 };

	class ResourceManager
	{
	public:
		ResourceManager() = default;
		~ResourceManager() = default;

		static void Init();

		static void Rescan();

		static const std::vector<std::filesystem::path>& GetTexturePaths();
		static const std::vector<std::filesystem::path>& GetMaterialPaths();
		static const std::vector<std::filesystem::path>& GetModelPaths();
		static std::filesystem::path GetAssetsDirectory();
	};
}