#include "Lpch.h"
#include "ResourceManager.h"

#define YAML_CPP_STATIC_DEFINE
#include <yaml-cpp/yaml.h>

#include "Locus/Core/Application.h"
#include "Locus/Resource/TextureManager.h"
#include "Locus/Resource/MaterialManager.h"
#include "Locus/Resource/ModelManager.h"

namespace Locus
{
	struct ResourceManagerData
	{
		std::vector<std::filesystem::path> TexturePaths;
		std::vector<std::filesystem::path> MaterialPaths;
		std::vector<std::filesystem::path> ModelPaths;

		std::filesystem::path ProjectDirectory;
	};

	static ResourceManagerData s_RMData;

	void ResourceManager::Init()
	{
		LOCUS_CORE_INFO("Initializing resource manager");
		s_RMData.ProjectDirectory = Application::Get().GetProjectPath();

		for (const auto& dirEntry : std::filesystem::recursive_directory_iterator(s_RMData.ProjectDirectory / "Assets"))
		{
			std::string ext = dirEntry.path().extension().string();
			if (ext == ".jpg" || ext == ".png" || ext == ".jpeg")
				s_RMData.TexturePaths.push_back(std::filesystem::relative(dirEntry, s_RMData.ProjectDirectory));
			else if (ext == ".lmat")
				s_RMData.MaterialPaths.push_back(std::filesystem::relative(dirEntry, s_RMData.ProjectDirectory));
			else if (ext == ".obj" || ext == ".fbx" || ext == ".gltf")
				s_RMData.ModelPaths.push_back(std::filesystem::relative(dirEntry, s_RMData.ProjectDirectory));
		}

		// Initialize sub resource managers
		TextureManager::Init();
		MaterialManager::Init();
		ModelManager::Init();
	}

	void ResourceManager::Rescan()
	{
		LOCUS_CORE_INFO("Rescanning for resources");
		for (const auto& dirEntry : std::filesystem::recursive_directory_iterator(s_RMData.ProjectDirectory / "Assets"))
		{
			std::string ext = dirEntry.path().extension().string();
			std::filesystem::path relativePath = std::filesystem::relative(dirEntry, s_RMData.ProjectDirectory);
			if (ext == ".jpg" || ext == ".png" || ext == ".jpeg")
			{
				if (std::find(s_RMData.TexturePaths.begin(), s_RMData.TexturePaths.end(), relativePath) == s_RMData.TexturePaths.end())
				{
					s_RMData.TexturePaths.push_back(relativePath);
					TextureManager::LoadTexture(relativePath);
				}
			}
			else if (ext == ".lmat")
			{
				if (std::find(s_RMData.MaterialPaths.begin(), s_RMData.MaterialPaths.end(), relativePath) == s_RMData.MaterialPaths.end())
				{
					s_RMData.MaterialPaths.push_back(relativePath);
					MaterialManager::LoadMaterial(relativePath);
				}
			}
			else if (ext == ".obj" || ext == ".fbx" || ext == ".gltf")
			{
				if (std::find(s_RMData.ModelPaths.begin(), s_RMData.ModelPaths.end(), relativePath) == s_RMData.ModelPaths.end())
				{
					s_RMData.ModelPaths.push_back(relativePath);
					ModelManager::LoadModel(relativePath);
				}
			}
		}
	}

	// Getters
	const std::vector<std::filesystem::path>& ResourceManager::GetTexturePaths() { return s_RMData.TexturePaths; }
	const std::vector<std::filesystem::path>& ResourceManager::GetMaterialPaths() { return s_RMData.MaterialPaths; }
	const std::vector<std::filesystem::path>& ResourceManager::GetModelPaths() { return s_RMData.ModelPaths; }
	std::filesystem::path ResourceManager::GetAssetsDirectory() { return s_RMData.ProjectDirectory / "Assets"; }
}
