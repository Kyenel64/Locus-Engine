#include "Lpch.h"
#include "Mesh.h"

namespace Locus
{
	Mesh::Mesh(std::vector<MeshVertex> vertices, std::vector<uint32_t> indices, std::vector<Ref<Texture2D>> textures)
		: m_Vertices(vertices), m_Indices(indices), m_Textures(textures)
	{
		ProcessMesh();
	}

	void Mesh::ProcessMesh()
	{
		
	}
}