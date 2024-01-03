#pragma once

#include "Locus/Renderer/Texture.h"

namespace Locus
{
	struct MeshVertex
	{
		glm::vec3 Position;
		glm::vec3 Normal;
		glm::vec2 TexCoords;
	};

	struct InstanceData
	{
		glm::mat4 ModelMatrix;
		int MaterialIndex;
		int EntityID;
	};

	class Mesh
	{
	public:
		Mesh(std::vector<MeshVertex> vertices, std::vector<uint32_t> indices, std::vector<Ref<Texture2D>> textures);
		~Mesh() = default;

		void ProcessMesh();

		const std::vector<MeshVertex>& GetVertices() const { return m_Vertices; }
		const std::vector<uint32_t>& GetIndices() const { return m_Indices; }
		const std::vector<Ref<Texture2D>>& GetTextures() const { return m_Textures; }

	private:
		// TODO: Decide if mesh should hold these data as the model already holds this data.
		std::vector<MeshVertex> m_Vertices;
		std::vector<uint32_t> m_Indices;
		std::vector<Ref<Texture2D>> m_Textures;
	};
}