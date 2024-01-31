#pragma once

#include "Mesh.h"

#include "Locus/Renderer/VertexArray.h"
#include "Locus/Renderer/Buffer.h"

struct aiNode;
struct aiScene;
struct aiMesh;

namespace Locus
{
	class Model
	{
	public:
		Model(const std::filesystem::path& filePath);
		~Model() = default;

		Mesh& GetMesh(int index) { return m_Meshes[index]; }
		std::vector<Mesh>& GetMeshes() { return m_Meshes; }
		const std::string& GetName() const { return m_Name; }

		Ref<VertexArray> GetVertexArray() const { return m_VertexArray; }

	private:
		void LoadModel();
		void ProcessNode(aiNode* node, const aiScene* scene);
		Mesh ProcessMesh(aiMesh* mesh, const aiScene* scene);

	private:
		std::vector<Mesh> m_Meshes;
		std::filesystem::path m_FilePath;
		std::string m_Name;

		uint32_t m_IndexOffset = 0;

		Ref<VertexArray> m_VertexArray;
		Ref<VertexBuffer> m_VertexBuffer;
		Ref<IndexBuffer> m_IndexBuffer;
		Ref<VertexBuffer> m_InstanceBuffer;

		std::vector<uint32_t> m_TotalIndices;
		std::vector<MeshVertex> m_TotalVertices;
	};
}