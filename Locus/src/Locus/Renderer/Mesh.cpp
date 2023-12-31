#include "Lpch.h"
#include "Mesh.h"

namespace Locus
{
	Mesh::Mesh(std::vector<MeshVertex> vertices, std::vector<uint32_t> indices, std::vector<Ref<Texture2D>> textures)
		: m_Indices(indices), m_Textures(textures)
	{
		ProcessMesh(vertices);
	}

	void Mesh::ProcessMesh(std::vector<MeshVertex> vertices)
	{
		// Vertex array
		m_VertexArray = VertexArray::Create();
		Ref<VertexBuffer> vertexBuffer = VertexBuffer::Create((uint32_t)vertices.size() * sizeof(MeshVertex));
		vertexBuffer->SetLayout({
			{ ShaderDataType::Float3, "a_Position" },
			{ ShaderDataType::Float3, "a_Normal" },
			{ ShaderDataType::Float2, "a_TexCoord" }
		});
		m_VertexArray->AddVertexBuffer(vertexBuffer);
		vertexBuffer->SetData(&vertices[0], (uint32_t)vertices.size() * sizeof(MeshVertex));
		Ref<IndexBuffer> indexBuffer = IndexBuffer::Create(&m_Indices[0], m_Indices.size());
		m_VertexArray->SetIndexBuffer(indexBuffer);
	}
}