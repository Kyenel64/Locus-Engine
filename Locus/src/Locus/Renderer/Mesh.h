#pragma once

#include "Locus/Renderer/Texture.h"
#include "Locus/Renderer/VertexArray.h"

namespace Locus
{
	struct MeshVertex
	{
		glm::vec3 Position;
		glm::vec3 Normal;
		glm::vec2 TexCoord;
	};

	class Mesh
	{
	public:
		Mesh(std::vector<MeshVertex> vertices, std::vector<uint32_t> indices, std::vector<Ref<Texture2D>> textures);
		~Mesh() = default;

		void ProcessMesh(std::vector<MeshVertex> vertices);

		Ref<VertexArray> GetVertexArray() const { return m_VertexArray; }
		const std::vector<uint32_t>& GetIndices() const { return m_Indices; }
		const std::vector<Ref<Texture2D>>& GetTextures() const { return m_Textures; }

	private:
		Ref<VertexArray> m_VertexArray;
		std::vector<uint32_t> m_Indices;
		std::vector<Ref<Texture2D>> m_Textures;
	};
}