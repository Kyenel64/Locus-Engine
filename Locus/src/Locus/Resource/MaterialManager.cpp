#include "Lpch.h"
#include "MaterialManager.h"

#define YAML_CPP_STATIC_DEFINE
#include <yaml-cpp/yaml.h>

#include "Locus/Resource/ResourceManager.h"

namespace Locus
{
	MaterialHandle MaterialHandle::Null = MaterialHandle(0);

	struct MaterialManagerData
	{
		std::unordered_map<MaterialHandle, Ref<Material>> Materials;
		uint32_t MaterialCount = 0;
	};

	static MaterialManagerData s_MMData;

	void MaterialManager::Init()
	{
		LOCUS_CORE_INFO("Material Manager:");

		for (auto& materialPath : ResourceManager::GetMaterialPaths())
		{
			LoadMaterial(materialPath);
		}

		
	}

	MaterialHandle MaterialManager::LoadMaterial(const std::filesystem::path& materialPath)
	{
		std::filesystem::path metadataPath = materialPath.string() + ".meta";
		if (std::filesystem::exists(metadataPath))
		{
			// Load texture from metadata path
			YAML::Node data = YAML::LoadFile(metadataPath.string());
			UUID uuid = data["UUID"].as<uint64_t>();
			MaterialHandle matHandle = MaterialHandle(uuid);
			s_MMData.Materials[matHandle] = CreateRef<Material>(materialPath.string());
			s_MMData.MaterialCount++;
			LOCUS_CORE_TRACE("  Loaded material: {0}", materialPath);
			return matHandle;
		}
		else
		{
			MaterialHandle matHandle = MaterialHandle();
			s_MMData.Materials[matHandle] = CreateRef<Material>(materialPath.string());
			s_MMData.MaterialCount++;
			YAML::Emitter out;
			out << YAML::BeginMap; // Scene
			out << YAML::Key << "UUID" << YAML::Value << (uint64_t)matHandle;
			out << YAML::Key << "Path" << YAML::Value << materialPath.string();
			out << YAML::EndMap; // End Scene
			std::ofstream fout(metadataPath);
			fout << out.c_str();
			LOCUS_CORE_TRACE("  Generated metadata: {0}", metadataPath);
			LOCUS_CORE_TRACE("  Loaded material: {0}", materialPath);
			return matHandle;
		}

		return {};
	}

	Ref<Material> MaterialManager::GetMaterial(MaterialHandle handle)
	{
		if (s_MMData.Materials.find(handle) != s_MMData.Materials.end())
			return s_MMData.Materials[handle];
		return nullptr;
	}
	const std::unordered_map<MaterialHandle, Ref<Material>> MaterialManager::GetMaterials() { return s_MMData.Materials; }

	bool MaterialManager::IsValid(MaterialHandle handle)
	{
		return s_MMData.Materials.find(handle) != s_MMData.Materials.end();
	}

	Ref<Material> MaterialHandle::Get() const
	{
		return MaterialManager::GetMaterial(Handle);
	}

	MaterialHandle::operator bool() const
	{
		return MaterialManager::IsValid(Handle);
	}
}