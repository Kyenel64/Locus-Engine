#pragma once

#include "Locus/Core/UUID.h"

namespace Locus
{
	class ResourceManager
	{
	public:
		ResourceManager() = default;
		~ResourceManager() = default;

		static void Init();

		static const std::vector<std::filesystem::path>& GetTexturePaths();
		static const std::vector<std::filesystem::path>& GetMaterialPaths();
		static std::filesystem::path GetAssetsDirectory();
		static UUID GetResourceUUID(const std::filesystem::path& path);
	};
}