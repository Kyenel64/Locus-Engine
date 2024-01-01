#pragma once

#include "Locus/Renderer/Texture.h"

namespace Locus
{
	struct ImportedMeshVertex
	{
		glm::vec3 Position;
		glm::vec3 Normal;
		glm::vec2 TexCoord;
	};

	class Mesh
	{
	public:
		Mesh(std::vector<ImportedMeshVertex> vertices, std::vector<uint32_t> indices, std::vector<Ref<Texture2D>> textures);
		~Mesh() = default;

		void ProcessMesh();

		const std::vector<ImportedMeshVertex>& GetVertices() const { return m_Vertices; }
		const std::vector<uint32_t>& GetIndices() const { return m_Indices; }
		const std::vector<Ref<Texture2D>>& GetTextures() const { return m_Textures; }

	private:
		std::vector<ImportedMeshVertex> m_Vertices;
		std::vector<uint32_t> m_Indices;
		std::vector<Ref<Texture2D>> m_Textures;

	public:
		friend class Renderer3D;
	};
}