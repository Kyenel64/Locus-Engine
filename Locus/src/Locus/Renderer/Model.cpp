#include "Lpch.h"
#include "Model.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

namespace Locus
{
	Model::Model(const std::filesystem::path& filePath)
		: m_FilePath(filePath)
	{
		LoadModel();
	}

	void Model::LoadModel()
	{
		Assimp::Importer importer;
		const aiScene* scene = importer.ReadFile(m_FilePath.string(), aiProcess_Triangulate | aiProcess_FlipUVs);
		LOCUS_CORE_ASSERT(scene || scene->mFlags & AI_SCENE_FLAGS_VALIDATED || scene->mRootNode, "Model::LoadModel(): Assimp failed to load model");

		ProcessNode(scene->mRootNode, scene);
	}

	void Model::ProcessNode(aiNode* node, const aiScene* scene)
	{
		// Process each mesh in node
		for (size_t i = 0; i < node->mNumMeshes; i++)
		{
			aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
			m_Meshes.push_back(ProcessMesh(mesh, scene));
		}

		// Process children nodes
		for (size_t i = 0; i < node->mNumChildren; i++)
		{
			ProcessNode(node->mChildren[i], scene);
		}
	}

	Mesh Model::ProcessMesh(aiMesh* mesh, const aiScene* scene)
	{
		std::vector<ImportedMeshVertex> vertices;
		std::vector<uint32_t> indices;
		std::vector<Ref<Texture2D>> textures;

		// Iterate through each mesh vertex
		for (size_t i = 0; i < mesh->mNumVertices; i++)
		{
			ImportedMeshVertex vertex;

			// Position
			vertex.Position.x = mesh->mVertices[i].x;
			vertex.Position.y = mesh->mVertices[i].y;
			vertex.Position.z = mesh->mVertices[i].z;

			// Normals
			if (mesh->HasNormals())
			{
				vertex.Normal.x = mesh->mNormals[i].x;
				vertex.Normal.y = mesh->mNormals[i].y;
				vertex.Normal.z = mesh->mNormals[i].z;
			}

			// TexCoords
			if (mesh->mTextureCoords[0])
			{
				vertex.TexCoord.x = mesh->mTextureCoords[0][i].x;
				vertex.TexCoord.y = mesh->mTextureCoords[0][i].y;
			}
			else
			{
				vertex.TexCoord = { 0.0f, 0.0f };
			}
			vertices.push_back(vertex);
		}

		// Iterate through indices
		for (size_t i = 0; i < mesh->mNumFaces; i++)
		{
			aiFace face = mesh->mFaces[i];
			for (size_t j = 0; j < face.mNumIndices; j++)
				indices.push_back(face.mIndices[j]);
		}

		return Mesh(vertices, indices, textures);
	}
}