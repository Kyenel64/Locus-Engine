#include "Lpch.h"
#include "Model.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "Locus/Renderer/Renderer3D.h"

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
		const aiScene* scene = importer.ReadFile(m_FilePath.string(), aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_CalcTangentSpace);
		LOCUS_CORE_ASSERT(scene, "Model::LoadModel(): Assimp failed to load model");
		LOCUS_CORE_ASSERT(scene->mFlags & AI_SCENE_FLAGS_VALIDATED || scene->mRootNode, "Model::LoadModel(): Assimp failed to load model");

		ProcessNode(scene->mRootNode, scene);

		m_VertexArray = VertexArray::Create();
		m_VertexBuffer = VertexBuffer::Create(static_cast<uint32_t>(m_TotalVertices.size() * sizeof(MeshVertex)));
		m_InstanceBuffer = VertexBuffer::Create(Renderer3D::GetMaxInstances() * sizeof(InstanceData));
		m_IndexBuffer = IndexBuffer::Create(m_TotalIndices.data(), static_cast<uint32_t>(m_TotalIndices.size()));
		m_VertexBuffer->SetLayout({
			{ ShaderDataType::Float3, "a_Position" },
			{ ShaderDataType::Float3, "a_Normal"},
			{ ShaderDataType::Float2, "a_TexCoord"},
			});
		m_InstanceBuffer->SetLayout({
			{ ShaderDataType::Mat4, "a_InstanceModelMatrix", 1},
			{ ShaderDataType::Int, "a_MaterialIndex", 1},
			{ ShaderDataType::Int, "a_EntityID", 1}
			});
		m_VertexBuffer->SetData(m_TotalVertices.data(), static_cast<uint32_t>(m_TotalVertices.size() * sizeof(MeshVertex)));
		m_VertexArray->AddVertexBuffer(m_VertexBuffer);
		m_VertexArray->AddVertexBuffer(m_InstanceBuffer);
		m_VertexArray->SetIndexBuffer(m_IndexBuffer);

		m_TotalIndices.clear();
		m_TotalVertices.clear();
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
		std::vector<MeshVertex> vertices;
		std::vector<uint32_t> indices;
		std::vector<Ref<Texture2D>> textures;

		// Iterate through each mesh vertex
		for (size_t i = 0; i < mesh->mNumVertices; i++)
		{
			MeshVertex vertex = {};

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
				vertex.TexCoords.x = mesh->mTextureCoords[0][i].x;
				vertex.TexCoords.y = mesh->mTextureCoords[0][i].y;
			}
			else
			{
				vertex.TexCoords = { 0.0f, 0.0f };
			}
			//vertex.MaterialIndex = 0; // Temp
			vertices.push_back(vertex);
			m_TotalVertices.push_back(vertex);
		}

		// Iterate through indices
		for (size_t i = 0; i < mesh->mNumFaces; i++)
		{
			aiFace face = mesh->mFaces[i];
			for (size_t j = 0; j < face.mNumIndices; j++)
			{
				m_TotalIndices.push_back(face.mIndices[j] + m_IndexOffset);
				indices.push_back(face.mIndices[j]);
			}
		}
		m_IndexOffset += static_cast<uint32_t>(vertices.size());

		return Mesh(vertices, indices, textures);
	}
}