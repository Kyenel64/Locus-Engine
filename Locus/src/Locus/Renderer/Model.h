#pragma once

#include "Mesh.h"

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

		const std::vector<Mesh>& GetMeshes() const { return m_Meshes; }

	private:
		void LoadModel();
		void ProcessNode(aiNode* node, const aiScene* scene);
		Mesh ProcessMesh(aiMesh* mesh, const aiScene* scene);

	private:
		std::vector<Mesh> m_Meshes;
		std::filesystem::path m_FilePath;
	};
}