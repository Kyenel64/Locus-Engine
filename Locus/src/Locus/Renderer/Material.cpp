#include "Lpch.h"
#include "Material.h"

#define YAML_CPP_STATIC_DEFINE
#include <yaml-cpp/yaml.h>

#include "Locus/Resource/TextureManager.h"

namespace YAML
{
	template<>
	struct convert<glm::vec4>
	{
		static Node encode(const glm::vec4& rhs)
		{
			Node node;
			node.push_back(rhs.x);
			node.push_back(rhs.y);
			node.push_back(rhs.z);
			node.push_back(rhs.w);
			node.SetStyle(EmitterStyle::Flow);
			return node;
		}

		static bool decode(const Node& node, glm::vec4& rhs)
		{
			if (!node.IsSequence() || node.size() != 4)
				return false;

			rhs.x = node[0].as<float>();
			rhs.y = node[1].as<float>();
			rhs.z = node[2].as<float>();
			rhs.w = node[3].as<float>();
			return true;
		}
	};
}

namespace Locus
{
	Material::Material()
		: m_UUID(UUID())
	{

	}

	Material::Material(const std::filesystem::path& path)
	{
		YAML::Node data = YAML::LoadFile(path.string());
		m_UUID = data["UUID"].as<uint64_t>();
		auto colors = data["Colors"];
		m_Albedo = colors["Albedo"].as<glm::vec4>();
		m_Metallic = colors["Metallic"].as<float>();
		m_Roughness = colors["Roughness"].as<float>();
		m_AO = colors["AO"].as<float>();

		auto textures = data["Textures"];
		TextureHandle albedoTex = TextureHandle(textures["Albedo"].as<uint64_t>());
		m_AlbedoTexture = TextureManager::GetTexture(albedoTex);
		TextureHandle normalTex = TextureHandle(textures["Normal"].as<uint64_t>());
		m_NormalMapTexture = TextureManager::GetTexture(normalTex);
		TextureHandle metallicTex = TextureHandle(textures["Metallic"].as<uint64_t>());
		m_MetallicTexture = TextureManager::GetTexture(metallicTex);
		TextureHandle roughnessTex = TextureHandle(textures["Roughness"].as<uint64_t>());
		m_RoughnessTexture = TextureManager::GetTexture(roughnessTex);
		TextureHandle aoTex = TextureHandle(textures["AO"].as<uint64_t>());
		m_AOTexture = TextureManager::GetTexture(aoTex);

	}
}