#include "Lpch.h"
#include "Renderer3D.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Locus/Scene/Entity.h"
#include "Locus/Scene/Scene.h"
#include "Locus/Renderer/RendererStats.h"
#include "Locus/Renderer/RenderCommand.h"
#include "Locus/Renderer/VertexArray.h"
#include "Locus/Renderer/Shader.h"
#include "Locus/Renderer/UniformBuffer.h"
#include "Locus/Renderer/Mesh.h"

namespace Locus
{
	struct Renderer3DData
	{
		static const uint32_t MaxInstances = 20000;
		static const uint32_t MaxCubeVertices = MaxInstances * 36;
		static const uint32_t MaxTextureSlots = 32;
		static const uint32_t MaxMaterialSlots = 32;

		Ref<Shader> PBRShader;

		// Model
		std::unordered_map<Ref<VertexArray>, std::vector<InstanceData>> Models;

		// Cube
		Ref<VertexArray> CubeVA;
		Ref<VertexBuffer> CubeVB;
		Ref<VertexBuffer> IndexVB;

		// Grid
		Ref<VertexArray> GridVA;
		Ref<VertexBuffer> GridVB;
		Ref<Shader> GridShader;
		struct GridData
		{
			glm::vec4 Color;
			float Near;
			float Far;
			float GridScale;
		};
		GridData GridBuffer;
		Ref<UniformBuffer> GridUniformBuffer;

		// Lighting data
		SceneLighting SceneLightingBuffer;
		Ref<UniformBuffer> SceneLightingUniformBuffer;

		// Textures
		std::array<Ref<Texture2D>, MaxTextureSlots> TextureSlots;
		uint32_t TextureSlotIndex;
		Ref<Texture2D> WhiteTexture;

		// Material
		struct MaterialBufferData
		{
			glm::vec4 Albedo;
			float Metallic;
			float Roughness;
			float AO;

			int AlbedoTexIndex;
			int NormalMapTexIndex;
			int MetallicTexIndex;
			int RoughnessTexIndex;
			int AOTexIndex;
		};
		MaterialBufferData MaterialBuffer[MaxMaterialSlots];
		Ref<UniformBuffer> MaterialUniformBuffer;
		uint32_t MaterialSlotIndex;

		//InstanceData InstanceBuffer[MaxCubes];
		uint32_t InstanceIndex;
	};

	static Renderer3DData s_R3DData;

	uint32_t Renderer3D::GetMaxInstances() { return s_R3DData.MaxInstances; }

	void Renderer3D::Init()
	{
		LOCUS_PROFILE_FUNCTION();

		// --- Cube -----------------------------------------------------------
		// Vertex array
		s_R3DData.CubeVA = VertexArray::Create();
		// Vertex buffer
		s_R3DData.CubeVB = VertexBuffer::Create(s_R3DData.MaxCubeVertices * sizeof(MeshVertex));
		s_R3DData.IndexVB = VertexBuffer::Create(s_R3DData.MaxInstances * sizeof(InstanceData));
		s_R3DData.CubeVB->SetLayout({
			{ ShaderDataType::Float3, "a_Position" },
			{ ShaderDataType::Float3, "a_Normal" },
			{ ShaderDataType::Float2, "a_TexCoord" }
			});
		s_R3DData.IndexVB->SetLayout({ 
			{ ShaderDataType::Mat4, "a_InstanceModelMatrix", 1},
			{ ShaderDataType::Int, "a_MaterialIndex", 1},
			{ ShaderDataType::Int, "a_EntityID", 1}
			});
		s_R3DData.CubeVA->AddVertexBuffer(s_R3DData.CubeVB);
		s_R3DData.CubeVA->AddVertexBuffer(s_R3DData.IndexVB);

		// --- Grid -----------------------------------------------------------
		s_R3DData.GridVA = VertexArray::Create();
		// Create VB
		s_R3DData.GridVB = VertexBuffer::Create(6 * sizeof(int));
		s_R3DData.GridVB->SetLayout({
			{ ShaderDataType::Int, "a_Index"}
			});
		s_R3DData.GridVA->AddVertexBuffer(s_R3DData.GridVB);
		int gridVertexData[6] = { 0, 1, 2, 2, 3, 0 };
		s_R3DData.GridVB->SetData(&gridVertexData, sizeof(int) * 6);

		// --- Initializations ------------------------------------------------
		s_R3DData.PBRShader = Shader::Create("resources/shaders/PBRShader.glsl");
		s_R3DData.GridShader = Shader::Create("resources/shaders/GridShader.glsl");

		// Define cube vertices and normals
		MeshVertex* cubeVertices = new MeshVertex[36];
#pragma region Cube vertex definitions
		glm::vec4 vertexPos[8] = {};
		vertexPos[0] = { -0.5f, -0.5f,  0.5f, 1.0f }; // Bottom Left Front
		vertexPos[1] = {  0.5f,  0.5f,  0.5f, 1.0f }; // Top Right Front
		vertexPos[2] = {  0.5f, -0.5f,  0.5f, 1.0f }; // Bottom Right Front
		vertexPos[3] = { -0.5f,  0.5f,  0.5f, 1.0f }; // Top Left Front
		vertexPos[4] = { -0.5f, -0.5f, -0.5f, 1.0f }; // Bottom Left Back
		vertexPos[5] = {  0.5f,  0.5f, -0.5f, 1.0f }; // Top Right Back
		vertexPos[6] = {  0.5f, -0.5f, -0.5f, 1.0f }; // Bottom Right Back
		vertexPos[7] = { -0.5f,  0.5f, -0.5f, 1.0f }; // Top Left Back

		// Front
		cubeVertices[0].Position = vertexPos[0];
		cubeVertices[1].Position = vertexPos[2];
		cubeVertices[2].Position = vertexPos[1];
		cubeVertices[3].Position = vertexPos[1];
		cubeVertices[4].Position = vertexPos[3];
		cubeVertices[5].Position = vertexPos[0];
		
		// Back
		cubeVertices[6].Position = vertexPos[4];
		cubeVertices[7].Position = vertexPos[5];
		cubeVertices[8].Position = vertexPos[6];
		cubeVertices[9].Position = vertexPos[5];
		cubeVertices[10].Position = vertexPos[4];
		cubeVertices[11].Position = vertexPos[7];
		
		// Left
		cubeVertices[12].Position = vertexPos[3];
		cubeVertices[13].Position = vertexPos[7];
		cubeVertices[14].Position = vertexPos[4];
		cubeVertices[15].Position = vertexPos[4];
		cubeVertices[16].Position = vertexPos[0];
		cubeVertices[17].Position = vertexPos[3];
		
		// Right
		cubeVertices[18].Position = vertexPos[1];
		cubeVertices[19].Position = vertexPos[6];
		cubeVertices[20].Position = vertexPos[5];
		cubeVertices[21].Position = vertexPos[6];
		cubeVertices[22].Position = vertexPos[1];
		cubeVertices[23].Position = vertexPos[2];
		
		// Top
		cubeVertices[24].Position = vertexPos[7];
		cubeVertices[25].Position = vertexPos[1];
		cubeVertices[26].Position = vertexPos[5];
		cubeVertices[27].Position = vertexPos[1];
		cubeVertices[28].Position = vertexPos[7];
		cubeVertices[29].Position = vertexPos[3];
		
		// Bottom
		cubeVertices[30].Position = vertexPos[4];
		cubeVertices[31].Position = vertexPos[6];
		cubeVertices[32].Position = vertexPos[2];
		cubeVertices[33].Position = vertexPos[2];
		cubeVertices[34].Position = vertexPos[0];
		cubeVertices[35].Position = vertexPos[4];
		
		cubeVertices[0].Normal = { 0, 0, 1 };
		cubeVertices[1].Normal = { 0, 0, 1 };
		cubeVertices[2].Normal = { 0, 0, 1 };
		cubeVertices[3].Normal = { 0, 0, 1 };
		cubeVertices[4].Normal = { 0, 0, 1 };
		cubeVertices[5].Normal = { 0, 0, 1 };
		
		cubeVertices[6].Normal = { 0, 0, -1 };
		cubeVertices[7].Normal = { 0, 0, -1 };
		cubeVertices[8].Normal = { 0, 0, -1 };
		cubeVertices[9].Normal = { 0, 0, -1 };
		cubeVertices[10].Normal = { 0, 0, -1 };
		cubeVertices[11].Normal = { 0, 0, -1 };
		
		cubeVertices[12].Normal = { -1, 0, 0 };
		cubeVertices[13].Normal = { -1, 0, 0 };
		cubeVertices[14].Normal = { -1, 0, 0 };
		cubeVertices[15].Normal = { -1, 0, 0 };
		cubeVertices[16].Normal = { -1, 0, 0 };
		cubeVertices[17].Normal = { -1, 0, 0 };
		
		cubeVertices[18].Normal = { 1, 0, 0 };
		cubeVertices[19].Normal = { 1, 0, 0 };
		cubeVertices[20].Normal = { 1, 0, 0 };
		cubeVertices[21].Normal = { 1, 0, 0 };
		cubeVertices[22].Normal = { 1, 0, 0 };
		cubeVertices[23].Normal = { 1, 0, 0 };
		
		cubeVertices[24].Normal = { 0, 1, 0 };
		cubeVertices[25].Normal = { 0, 1, 0 };
		cubeVertices[26].Normal = { 0, 1, 0 };
		cubeVertices[27].Normal = { 0, 1, 0 };
		cubeVertices[28].Normal = { 0, 1, 0 };
		cubeVertices[29].Normal = { 0, 1, 0 };
		
		cubeVertices[30].Normal = { 0, -1, 0 };
		cubeVertices[31].Normal = { 0, -1, 0 };
		cubeVertices[32].Normal = { 0, -1, 0 };
		cubeVertices[33].Normal = { 0, -1, 0 };
		cubeVertices[34].Normal = { 0, -1, 0 };
		cubeVertices[35].Normal = { 0, -1, 0 };
		
		// Front
		cubeVertices[0].TexCoords = { 0, 0 };
		cubeVertices[1].TexCoords = { 1, 0 };
		cubeVertices[2].TexCoords = { 1, 1 };
		cubeVertices[3].TexCoords = { 1, 1 };
		cubeVertices[4].TexCoords = { 0, 1 };
		cubeVertices[5].TexCoords = { 0, 0 };
		
		// Back
		cubeVertices[6].TexCoords =  { 0, 0 };
		cubeVertices[7].TexCoords =  { 1, 1 };
		cubeVertices[8].TexCoords =  { 1, 0 };
		cubeVertices[9].TexCoords =  { 1, 1 };
		cubeVertices[10].TexCoords = { 0, 0 };
		cubeVertices[11].TexCoords = { 0, 1 };
		
		// Left
		cubeVertices[12].TexCoords = { 1, 0 };
		cubeVertices[13].TexCoords = { 1, 1 };
		cubeVertices[14].TexCoords = { 0, 1 };
		cubeVertices[15].TexCoords = { 0, 1 };
		cubeVertices[16].TexCoords = { 0, 0 };
		cubeVertices[17].TexCoords = { 1, 0 };
		
		// Right
		cubeVertices[18].TexCoords = { 1, 0 };
		cubeVertices[19].TexCoords = { 0, 1 };
		cubeVertices[20].TexCoords = { 1, 1 };
		cubeVertices[21].TexCoords = { 0, 1 };
		cubeVertices[22].TexCoords = { 1, 0 };
		cubeVertices[23].TexCoords = { 0, 0 };
		
		// Top
		cubeVertices[24].TexCoords = { 0, 1 };
		cubeVertices[25].TexCoords = { 1, 0 };
		cubeVertices[26].TexCoords = { 1, 1 };
		cubeVertices[27].TexCoords = { 1, 0 };
		cubeVertices[28].TexCoords = { 0, 1 };
		cubeVertices[29].TexCoords = { 0, 0 };
		
		// Bottom
		cubeVertices[30].TexCoords = { 0, 1 };
		cubeVertices[31].TexCoords = { 1, 1 };
		cubeVertices[32].TexCoords = { 1, 0 };
		cubeVertices[33].TexCoords = { 1, 0 };
		cubeVertices[34].TexCoords = { 0, 0 };
		cubeVertices[35].TexCoords = { 0, 1 };

#pragma endregion Cube vertex definitions
		s_R3DData.CubeVB->SetData(&cubeVertices[0], sizeof(MeshVertex) * 36);
		delete[] cubeVertices;

		// Uniform buffers
		s_R3DData.GridUniformBuffer = UniformBuffer::Create(sizeof(Renderer3DData::GridData), 1);
		s_R3DData.SceneLightingUniformBuffer = UniformBuffer::Create(sizeof(SceneLighting), 2);
		s_R3DData.MaterialUniformBuffer = UniformBuffer::Create(sizeof(Renderer3DData::MaterialBufferData) * s_R3DData.MaxMaterialSlots, 3);

		// White Texture
		s_R3DData.WhiteTexture = Texture2D::Create(1, 1);
		uint64_t whiteTextureData = 0xfffffffff;
		s_R3DData.WhiteTexture->SetData(&whiteTextureData, sizeof(uint32_t));
		s_R3DData.TextureSlots[0] = s_R3DData.WhiteTexture;

		// Default material
		s_R3DData.MaterialBuffer[0].Albedo = { 1.0f, 1.0f, 1.0f, 1.0f };
		s_R3DData.MaterialBuffer[0].Metallic = 0.0f;
		s_R3DData.MaterialBuffer[0].Roughness = 0.5f;
		s_R3DData.MaterialBuffer[0].AO = 1.0f;
		s_R3DData.MaterialBuffer[0].AlbedoTexIndex = 0;
		s_R3DData.MaterialBuffer[0].NormalMapTexIndex = 0;
		s_R3DData.MaterialBuffer[0].MetallicTexIndex = 0;
		s_R3DData.MaterialBuffer[0].RoughnessTexIndex = 0;
		s_R3DData.MaterialBuffer[0].AOTexIndex = 0;
	}

	void Renderer3D::Shutdown()
	{
		LOCUS_PROFILE_FUNCTION();
	}

	void Renderer3D::BeginScene(const EditorCamera& camera, Scene* scene)
	{
		LOCUS_PROFILE_FUNCTION();

		// Lighting
		s_R3DData.SceneLightingBuffer = scene->GetLightingData();
		s_R3DData.SceneLightingUniformBuffer->SetData(&s_R3DData.SceneLightingBuffer, sizeof(SceneLighting));

		// Editor grid
		s_R3DData.GridBuffer.Color = camera.GetGridColor();
		s_R3DData.GridBuffer.Near = camera.GetNearClip();
		s_R3DData.GridBuffer.Far = camera.GetFarClip();
		s_R3DData.GridBuffer.GridScale = camera.GetGridScale();
		s_R3DData.GridUniformBuffer->SetData(&s_R3DData.GridBuffer, sizeof(Renderer3DData::GridData));

		StartBatch();
	}

	void Renderer3D::BeginScene(const Camera& camera, const glm::mat4& transform, Scene* scene)
	{
		LOCUS_PROFILE_FUNCTION();

		// Lighting
		s_R3DData.SceneLightingBuffer = scene->GetLightingData();
		s_R3DData.SceneLightingUniformBuffer->SetData(&s_R3DData.SceneLightingBuffer, sizeof(SceneLighting));

		StartBatch();
	}

	void Renderer3D::EndScene()
	{
		LOCUS_PROFILE_FUNCTION();

		Flush();
	}

	void Renderer3D::StartBatch()
	{
		LOCUS_PROFILE_FUNCTION();

		s_R3DData.TextureSlotIndex = 1;
		s_R3DData.MaterialSlotIndex = 1;
		s_R3DData.InstanceIndex = 0;
		s_R3DData.Models.clear();
	}

	void Renderer3D::Flush()
	{
		LOCUS_PROFILE_FUNCTION();

		// Bind textures and uniforms
		for (uint32_t i = 0; i < s_R3DData.TextureSlotIndex; i++)
			s_R3DData.TextureSlots[i]->Bind(i);
		// Materials
		s_R3DData.MaterialUniformBuffer->SetData(&s_R3DData.MaterialBuffer[0], sizeof(Renderer3DData::MaterialBufferData) * s_R3DData.MaterialSlotIndex);

		s_R3DData.PBRShader->Bind();

		for (auto& [ va, instanceData ] : s_R3DData.Models)
		{
			// VertexBuffer[1] in our model is where the instanced data is.
			va->GetVertexBuffers()[1]->SetData(instanceData.data(), static_cast<uint32_t>(instanceData.size() * sizeof(InstanceData)));
			if (va->GetIndexBuffer())
				RenderCommand::DrawIndexedInstanced(va, va->GetIndexBuffer()->GetCount(), static_cast<uint32_t>(instanceData.size()));
			else
				RenderCommand::DrawArrayInstanced(va, 36, static_cast<uint32_t>(instanceData.size())); // Temp for cubes
			RendererStats::GetStats().DrawCalls++;
		}
	}

	void Renderer3D::FlushAndReset()
	{
		LOCUS_PROFILE_FUNCTION();

		EndScene();
		
		s_R3DData.TextureSlotIndex = 1;
		s_R3DData.MaterialSlotIndex = 1;
		s_R3DData.InstanceIndex = 0;
		s_R3DData.Models.clear();
	}

	void Renderer3D::DrawCube(const glm::mat4& transform, Ref<Material> material, int entityID)
	{
		DrawModel(transform, s_R3DData.CubeVA, material, entityID);
	}

	void Renderer3D::DrawModel(const glm::mat4& transform, Ref<VertexArray> va, Ref<Material> material, int entityID)
	{
		LOCUS_PROFILE_FUNCTION();

		if (!va)
			return;

		if (s_R3DData.InstanceIndex >= s_R3DData.MaxInstances)
			FlushAndReset();

		// Add texture to texture slot. Handles duplicates.
		int materialIndex = 0;
		if (material)
		{
			int albedoTexIndex = ProcessTextureSlot(material->m_AlbedoTexture.Get());
			int normalTexIndex = ProcessTextureSlot(material->m_NormalMapTexture.Get());
			int metallicTexIndex = ProcessTextureSlot(material->m_MetallicTexture.Get());
			int roughnessTexIndex = ProcessTextureSlot(material->m_RoughnessTexture.Get());
			int aoTexIndex = ProcessTextureSlot(material->m_AOTexture.Get());

			// Material uniform data
			materialIndex = ProcessMaterialSlot(material, albedoTexIndex, normalTexIndex, metallicTexIndex, roughnessTexIndex, aoTexIndex);
		}

		// Instance data
		InstanceData data = {};
		data.ModelMatrix = transform;
		data.MaterialIndex = materialIndex;
		data.EntityID = entityID;
		s_R3DData.Models[va].push_back(data);
		
		s_R3DData.InstanceIndex++;
	}

	void Renderer3D::DrawCubeMask(const glm::mat4& transform, Ref<Shader> shader)
	{
		//s_R3DData.CubeVertexCount = 0;
		//s_R3DData.CubeVertexBufferPtr = s_R3DData.CubeVertexBufferBase;
		//
		//for (uint32_t i = 0; i < 36; i++)
		//{
		//	s_R3DData.CubeVertexBufferPtr->WorldPosition = transform * s_R3DData.CubeVertexPositions[i];
		//	s_R3DData.CubeVertexBufferPtr++;
		//}
		//s_R3DData.CubeVB->SetData(s_R3DData.CubeVertexBufferBase, sizeof(CubeVertex) * 36);
		//
		//shader->Bind();
		//RenderCommand::DrawArray(s_R3DData.CubeVA, 36);
		//
		//RendererStats::GetStats().DrawCalls++;
	}

	void Renderer3D::DrawGrid()
	{
		LOCUS_PROFILE_FUNCTION();

		s_R3DData.GridShader->Bind();
		RenderCommand::DrawArray(s_R3DData.GridVA, 6);

		RendererStats::GetStats().DrawCalls++;
	}

	int Renderer3D::ProcessTextureSlot(Ref<Texture2D> texture)
	{
		LOCUS_PROFILE_FUNCTION();
		if (texture)
		{
			for (uint32_t i = 1; i < s_R3DData.TextureSlotIndex; i++)
			{
				if (*s_R3DData.TextureSlots[i].get() == *texture.get())
					return i;
			}

			if (s_R3DData.TextureSlotIndex >= Renderer3DData::MaxTextureSlots)
				FlushAndReset();

			int slot = s_R3DData.TextureSlotIndex;
			s_R3DData.TextureSlots[slot] = texture;
			s_R3DData.TextureSlotIndex++;
			return slot;
		}

		return 0;
	}

	int Renderer3D::ProcessMaterialSlot(Ref<Material> material, int albedoIndex, int normalIndex, int metallicIndex, int roughnessIndex, int aoIndex)
	{
		LOCUS_PROFILE_FUNCTION();
		if (material)
		{
			for (uint32_t i = 1; i < s_R3DData.MaterialSlotIndex; i++)
			{
				if (s_R3DData.MaterialBuffer[i].Albedo == material->m_Albedo && s_R3DData.MaterialBuffer[i].Metallic == material->m_Metallic && 
					s_R3DData.MaterialBuffer[i].Roughness == material->m_Roughness && s_R3DData.MaterialBuffer[i].AO == material->m_AO && 
					s_R3DData.MaterialBuffer[i].AlbedoTexIndex == albedoIndex && s_R3DData.MaterialBuffer[i].NormalMapTexIndex == normalIndex && 
					s_R3DData.MaterialBuffer[i].MetallicTexIndex == metallicIndex && s_R3DData.MaterialBuffer[i].RoughnessTexIndex == roughnessIndex && s_R3DData.MaterialBuffer[i].AOTexIndex == aoIndex)
					return i;
			}
			if (s_R3DData.MaterialSlotIndex >= Renderer3DData::MaxMaterialSlots)
				FlushAndReset();

			int slot = s_R3DData.MaterialSlotIndex;
			s_R3DData.MaterialBuffer[slot].Albedo = material->m_Albedo;
			s_R3DData.MaterialBuffer[slot].Metallic = material->m_Metallic;
			s_R3DData.MaterialBuffer[slot].Roughness = material->m_Roughness;
			s_R3DData.MaterialBuffer[slot].AO = material->m_AO;
			s_R3DData.MaterialBuffer[slot].AlbedoTexIndex = albedoIndex;
			s_R3DData.MaterialBuffer[slot].NormalMapTexIndex = normalIndex;
			s_R3DData.MaterialBuffer[slot].MetallicTexIndex = metallicIndex;
			s_R3DData.MaterialBuffer[slot].RoughnessTexIndex = roughnessIndex;
			s_R3DData.MaterialBuffer[slot].AOTexIndex = aoIndex;
			s_R3DData.MaterialSlotIndex++;
			return slot;
		}

		return 0;
	}
}