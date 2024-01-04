#pragma once

#include "Locus/Renderer/Texture.h"
#include "Locus/Renderer/Shader.h"

#include "Locus/Core/UUID.h"

namespace Locus
{
	class Material
	{
	public:

		Material();
		Material(const std::filesystem::path& path);
		~Material() = default;

		bool operator==(const Material& other) const
		{
			return m_UUID == other.m_UUID;
		}

	public:
		Ref<Shader> m_Shader = nullptr;

		UUID m_UUID;

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