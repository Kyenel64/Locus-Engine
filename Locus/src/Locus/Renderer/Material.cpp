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
	Material::Material(const std::filesystem::path& path)
		: m_Path(path)
	{
		m_Name = path.stem().string();

		YAML::Node data = YAML::LoadFile(path.string());
		auto colors = data["Colors"];
		m_Albedo = colors["Albedo"].as<glm::vec4>();
		m_Metallic = colors["Metallic"].as<float>();
		m_Roughness = colors["Roughness"].as<float>();
		m_AO = colors["AO"].as<float>();

		auto textures = data["Textures"];
		TextureHandle albedoTex = TextureHandle(textures["Albedo"].as<std::string>());
		m_AlbedoTexture = albedoTex;
		TextureHandle normalTex = TextureHandle(textures["Normal"].as<std::string>());
		m_NormalMapTexture = normalTex;
		TextureHandle metallicTex = TextureHandle(textures["Metallic"].as<std::string>());
		m_MetallicTexture = metallicTex;
		TextureHandle roughnessTex = TextureHandle(textures["Roughness"].as<std::string>());
		m_RoughnessTexture = roughnessTex;
		TextureHandle aoTex = TextureHandle(textures["AO"].as<std::string>());
		m_AOTexture = aoTex;

	}
}