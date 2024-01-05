#pragma once

#include "Locus/Renderer/Texture.h"
#include "Locus/Renderer/Shader.h"

#include "Locus/Core/UUID.h"

namespace Locus
{
	class Material
	{
	public:

		Material() = default;
		Material(const std::filesystem::path& path);
		~Material() = default;

		const std::string& GetName() const { return m_Name; }
		const std::filesystem::path& GetPath() const { return m_Path; }

	public:
		std::filesystem::path m_Path;
		std::string m_Name;

		Ref<Shader> m_Shader = nullptr;

		glm::vec3 m_Albedo = { 1.0f, 1.0f, 1.0f };
		float m_Metallic = 0.5f;
		float m_Roughness = 0.5f;
		float m_AO = 0.5f;

		Ref<Texture2D> m_AlbedoTexture = nullptr;
		Ref<Texture2D> m_NormalMapTexture = nullptr;
		Ref<Texture2D> m_MetallicTexture = nullptr;
		Ref<Texture2D> m_RoughnessTexture = nullptr;
		Ref<Texture2D> m_AOTexture = nullptr;
	};
}