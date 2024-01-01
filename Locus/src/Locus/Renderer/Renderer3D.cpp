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

namespace Locus
{
	struct CubeVertex
	{
		glm::vec3 WorldPosition;
		glm::vec3 Normal;
		glm::vec2 TexCoords;
		int MaterialUniformIndex;
		int EntityID;
	};

	struct MeshVertex
	{
		glm::vec3 WorldPosition;
		glm::vec3 Normal;
		glm::vec2 TexCoords;
		int MaterialUniformIndex;
		int EntityID;
	};

	struct Renderer3DData
	{
		static const uint32_t MaxCubes = 20000;
		static const uint32_t MaxVertices = MaxCubes * 8;
		static const uint32_t MaxIndices = MaxCubes * 36;
		static const uint32_t MaxTextureSlots = 32;
		static const uint32_t MaxMaterialSlots = 32;

		// Mesh
		Ref<VertexArray> MeshVA;
		Ref<VertexBuffer> MeshVB;
		Ref<IndexBuffer> MeshIB;

		uint32_t MeshVertexCount = 0;
		MeshVertex* MeshVertexBufferBase = nullptr;
		MeshVertex* MeshVertexBufferPtr = nullptr;
		uint32_t MeshIndexCount = 0;
		uint32_t* MeshIndexBufferBase = nullptr;
		uint32_t* MeshIndexBufferPtr = nullptr;

		// Cube
		Ref<VertexArray> CubeVA;
		Ref<VertexBuffer> CubeVB;
		Ref<Shader> PBRShader;

		uint32_t CubeVertexCount = 0;
		CubeVertex* CubeVertexBufferBase = nullptr;
		CubeVertex* CubeVertexBufferPtr = nullptr;

		glm::vec4 CubeVertexPositions[36];
		glm::vec3 CubeVertexNormals[36];
		glm::vec2 CubeVertexTexCoords[36];

		// Grid
		Ref<VertexArray> GridVA;
		Ref<VertexBuffer> GridVB;
		Ref<Shader> GridShader;
		int GridVertexBuffer[6] = { 0, 1, 2, 2, 3, 0 };

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
		uint32_t TextureSlotIndex = 1;
		Ref<Texture2D> WhiteTexture;

		// Material
		struct MaterialData
		{
			Ref<Material> Material;

			int AlbedoTexIndex;
			int NormalMapTexIndex;
			int MetallicTexIndex;
			int RoughnessTexIndex;
			int AOTexIndex;
		};

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
		std::array<MaterialData, MaxMaterialSlots> MaterialSlots;
		uint32_t MaterialSlotIndex = 0;
	};

	static Renderer3DData s_R3DData;

	void Renderer3D::Init()
	{
		LOCUS_PROFILE_FUNCTION();

		// --- Mesh -----------------------------------------------------------
		s_R3DData.MeshVA = VertexArray::Create();
		s_R3DData.MeshIB = IndexBuffer::Create(nullptr, s_R3DData.MaxIndices);
		s_R3DData.MeshVB = VertexBuffer::Create(s_R3DData.MaxVertices * sizeof(MeshVertex));
		s_R3DData.MeshVB->SetLayout({
			{ ShaderDataType::Float3, "a_Position" },
			{ ShaderDataType::Float3, "a_Normal"},
			{ ShaderDataType::Float2, "a_TexCoord"},
			{ ShaderDataType::Int, "a_MaterialUniformIndex" },
			{ ShaderDataType::Int, "a_EntityID" }
			});
		s_R3DData.MeshVA->AddVertexBuffer(s_R3DData.MeshVB);
		s_R3DData.MeshVA->SetIndexBuffer(s_R3DData.MeshIB);
		s_R3DData.MeshVertexBufferBase = new MeshVertex[s_R3DData.MaxVertices];
		s_R3DData.MeshIndexBufferBase = new uint32_t[s_R3DData.MaxIndices];

		// --- Cube -----------------------------------------------------------
		// Vertex array
		s_R3DData.CubeVA = VertexArray::Create();
		// Vertex buffer
		s_R3DData.CubeVB = VertexBuffer::Create(s_R3DData.MaxVertices * sizeof(CubeVertex));
		s_R3DData.CubeVB->SetLayout({
			{ ShaderDataType::Float3, "a_Position" },
			{ ShaderDataType::Float3, "a_Normal"},
			{ ShaderDataType::Float2, "a_TexCoord"},
			{ ShaderDataType::Int, "a_MaterialUniformIndex" },
			{ ShaderDataType::Int, "a_EntityID" }
		});
		s_R3DData.CubeVA->AddVertexBuffer(s_R3DData.CubeVB);
		s_R3DData.CubeVertexBufferBase = new CubeVertex[s_R3DData.MaxVertices];

		// --- Grid -----------------------------------------------------------
		s_R3DData.GridVA = VertexArray::Create();
		// Create VB
		s_R3DData.GridVB = VertexBuffer::Create(6 * sizeof(int));
		s_R3DData.GridVB->SetLayout({
			{ ShaderDataType::Int, "a_Index"}
			});
		s_R3DData.GridVA->AddVertexBuffer(s_R3DData.GridVB);
		s_R3DData.GridVB->SetData(&s_R3DData.GridVertexBuffer, sizeof(int) * 6);

		// --- Initializations ------------------------------------------------
		s_R3DData.PBRShader = Shader::Create("resources/shaders/PBRShader.glsl");
		s_R3DData.GridShader = Shader::Create("resources/shaders/GridShader.glsl");

		// Define cube vertices and normals
#pragma region Cube vertex definitions
		glm::vec4 vertexPos[8] = {};
		vertexPos[0] = { -0.5f, -0.5f,  0.5f, 1.0f };
		vertexPos[1] = { -0.5f,  0.5f,  0.5f, 1.0f };
		vertexPos[2] = {  0.5f,  0.5f,  0.5f, 1.0f };
		vertexPos[3] = {  0.5f, -0.5f,  0.5f, 1.0f };
		vertexPos[4] = { -0.5f, -0.5f, -0.5f, 1.0f };
		vertexPos[5] = { -0.5f,  0.5f, -0.5f, 1.0f };
		vertexPos[6] = {  0.5f,  0.5f, -0.5f, 1.0f };
		vertexPos[7] = {  0.5f, -0.5f, -0.5f, 1.0f };

		s_R3DData.CubeVertexPositions[0] = vertexPos[0];
		s_R3DData.CubeVertexPositions[1] = vertexPos[1];
		s_R3DData.CubeVertexPositions[2] = vertexPos[2];
		s_R3DData.CubeVertexPositions[3] = vertexPos[0];
		s_R3DData.CubeVertexPositions[4] = vertexPos[2];
		s_R3DData.CubeVertexPositions[5] = vertexPos[3];

		s_R3DData.CubeVertexPositions[6] = vertexPos[4];
		s_R3DData.CubeVertexPositions[7] = vertexPos[5];
		s_R3DData.CubeVertexPositions[8] = vertexPos[1];
		s_R3DData.CubeVertexPositions[9] = vertexPos[4];
		s_R3DData.CubeVertexPositions[10] = vertexPos[1];
		s_R3DData.CubeVertexPositions[11] = vertexPos[0];

		s_R3DData.CubeVertexPositions[12] = vertexPos[7];
		s_R3DData.CubeVertexPositions[13] = vertexPos[6];
		s_R3DData.CubeVertexPositions[14] = vertexPos[5];
		s_R3DData.CubeVertexPositions[15] = vertexPos[7];
		s_R3DData.CubeVertexPositions[16] = vertexPos[5];
		s_R3DData.CubeVertexPositions[17] = vertexPos[4];

		s_R3DData.CubeVertexPositions[18] = vertexPos[3];
		s_R3DData.CubeVertexPositions[19] = vertexPos[2];
		s_R3DData.CubeVertexPositions[20] = vertexPos[6];
		s_R3DData.CubeVertexPositions[21] = vertexPos[3];
		s_R3DData.CubeVertexPositions[22] = vertexPos[6];
		s_R3DData.CubeVertexPositions[23] = vertexPos[7];

		s_R3DData.CubeVertexPositions[24] = vertexPos[1];
		s_R3DData.CubeVertexPositions[25] = vertexPos[5];
		s_R3DData.CubeVertexPositions[26] = vertexPos[6];
		s_R3DData.CubeVertexPositions[27] = vertexPos[1];
		s_R3DData.CubeVertexPositions[28] = vertexPos[6];
		s_R3DData.CubeVertexPositions[29] = vertexPos[2];

		s_R3DData.CubeVertexPositions[30] = vertexPos[0];
		s_R3DData.CubeVertexPositions[31] = vertexPos[4];
		s_R3DData.CubeVertexPositions[32] = vertexPos[7];
		s_R3DData.CubeVertexPositions[33] = vertexPos[0];
		s_R3DData.CubeVertexPositions[34] = vertexPos[7];
		s_R3DData.CubeVertexPositions[35] = vertexPos[3];

		s_R3DData.CubeVertexNormals[0] = { 0, 0, 1 };
		s_R3DData.CubeVertexNormals[1] = { 0, 0, 1 };
		s_R3DData.CubeVertexNormals[2] = { 0, 0, 1 };
		s_R3DData.CubeVertexNormals[3] = { 0, 0, 1 };
		s_R3DData.CubeVertexNormals[4] = { 0, 0, 1 };
		s_R3DData.CubeVertexNormals[5] = { 0, 0, 1 };

		s_R3DData.CubeVertexNormals[6] =  { -1, 0, 0 };
		s_R3DData.CubeVertexNormals[7] =  { -1, 0, 0 };
		s_R3DData.CubeVertexNormals[8] =  { -1, 0, 0 };
		s_R3DData.CubeVertexNormals[9] =  { -1, 0, 0 };
		s_R3DData.CubeVertexNormals[10] = { -1, 0, 0 };
		s_R3DData.CubeVertexNormals[11] = { -1, 0, 0 };

		s_R3DData.CubeVertexNormals[12] = { 0, 0, -1 };
		s_R3DData.CubeVertexNormals[13] = { 0, 0, -1 };
		s_R3DData.CubeVertexNormals[14] = { 0, 0, -1 };
		s_R3DData.CubeVertexNormals[15] = { 0, 0, -1 };
		s_R3DData.CubeVertexNormals[16] = { 0, 0, -1 };
		s_R3DData.CubeVertexNormals[17] = { 0, 0, -1 };

		s_R3DData.CubeVertexNormals[18] = { 1, 0, 0 };
		s_R3DData.CubeVertexNormals[19] = { 1, 0, 0 };
		s_R3DData.CubeVertexNormals[20] = { 1, 0, 0 };
		s_R3DData.CubeVertexNormals[21] = { 1, 0, 0 };
		s_R3DData.CubeVertexNormals[22] = { 1, 0, 0 };
		s_R3DData.CubeVertexNormals[23] = { 1, 0, 0 };

		s_R3DData.CubeVertexNormals[24] = { 0, 1, 0 };
		s_R3DData.CubeVertexNormals[25] = { 0, 1, 0 };
		s_R3DData.CubeVertexNormals[26] = { 0, 1, 0 };
		s_R3DData.CubeVertexNormals[27] = { 0, 1, 0 };
		s_R3DData.CubeVertexNormals[28] = { 0, 1, 0 };
		s_R3DData.CubeVertexNormals[29] = { 0, 1, 0 };

		s_R3DData.CubeVertexNormals[30] = { 0, -1, 0 };
		s_R3DData.CubeVertexNormals[31] = { 0, -1, 0 };
		s_R3DData.CubeVertexNormals[32] = { 0, -1, 0 };
		s_R3DData.CubeVertexNormals[33] = { 0, -1, 0 };
		s_R3DData.CubeVertexNormals[34] = { 0, -1, 0 };
		s_R3DData.CubeVertexNormals[35] = { 0, -1, 0 };

		s_R3DData.CubeVertexTexCoords[0] = { 0, 0 };
		s_R3DData.CubeVertexTexCoords[1] = { 0, 1 };
		s_R3DData.CubeVertexTexCoords[2] = { 1, 1 };
		s_R3DData.CubeVertexTexCoords[3] = { 0, 0 };
		s_R3DData.CubeVertexTexCoords[4] = { 1, 1 };
		s_R3DData.CubeVertexTexCoords[5] = { 1, 0 };

		s_R3DData.CubeVertexTexCoords[6]  = { 0, 0 };
		s_R3DData.CubeVertexTexCoords[7]  = { 0, 1 };
		s_R3DData.CubeVertexTexCoords[8]  = { 1, 1 };
		s_R3DData.CubeVertexTexCoords[9]  = { 0, 0 };
		s_R3DData.CubeVertexTexCoords[10] = { 1, 1 };
		s_R3DData.CubeVertexTexCoords[11] = { 1, 0 };

		s_R3DData.CubeVertexTexCoords[12] = { 0, 0 };
		s_R3DData.CubeVertexTexCoords[13] = { 0, 1 };
		s_R3DData.CubeVertexTexCoords[14] = { 1, 1 };
		s_R3DData.CubeVertexTexCoords[15] = { 0, 0 };
		s_R3DData.CubeVertexTexCoords[16] = { 1, 1 };
		s_R3DData.CubeVertexTexCoords[17] = { 1, 0 };

		s_R3DData.CubeVertexTexCoords[18] = { 0, 0 };
		s_R3DData.CubeVertexTexCoords[19] = { 0, 1 };
		s_R3DData.CubeVertexTexCoords[20] = { 1, 1 };
		s_R3DData.CubeVertexTexCoords[21] = { 0, 0 };
		s_R3DData.CubeVertexTexCoords[22] = { 1, 1 };
		s_R3DData.CubeVertexTexCoords[23] = { 1, 0 };

		s_R3DData.CubeVertexTexCoords[24] = { 0, 0 };
		s_R3DData.CubeVertexTexCoords[25] = { 0, 1 };
		s_R3DData.CubeVertexTexCoords[26] = { 1, 1 };
		s_R3DData.CubeVertexTexCoords[27] = { 0, 0 };
		s_R3DData.CubeVertexTexCoords[28] = { 1, 1 };
		s_R3DData.CubeVertexTexCoords[29] = { 1, 0 };

		s_R3DData.CubeVertexTexCoords[30] = { 0, 0 };
		s_R3DData.CubeVertexTexCoords[31] = { 0, 1 };
		s_R3DData.CubeVertexTexCoords[32] = { 1, 1 };
		s_R3DData.CubeVertexTexCoords[33] = { 0, 0 };
		s_R3DData.CubeVertexTexCoords[34] = { 1, 1 };
		s_R3DData.CubeVertexTexCoords[35] = { 1, 0 };

#pragma endregion Cube vertex definitions
		
		// Uniform buffers
		s_R3DData.GridUniformBuffer = UniformBuffer::Create(sizeof(Renderer3DData::GridData), 1);
		s_R3DData.SceneLightingUniformBuffer = UniformBuffer::Create(sizeof(SceneLighting), 2);
		s_R3DData.MaterialUniformBuffer = UniformBuffer::Create(sizeof(Renderer3DData::MaterialBufferData) * s_R3DData.MaxMaterialSlots, 3);

		// White Texture
		s_R3DData.WhiteTexture = Texture2D::Create(1, 1);
		uint64_t whiteTextureData = 0xfffffffff;
		s_R3DData.WhiteTexture->SetData(&whiteTextureData, sizeof(uint32_t));
		s_R3DData.TextureSlots[0] = s_R3DData.WhiteTexture;
	}

	void Renderer3D::Shutdown()
	{
		LOCUS_PROFILE_FUNCTION();

		delete[] s_R3DData.CubeVertexBufferBase;
		delete[] s_R3DData.MeshVertexBufferBase;
		delete[] s_R3DData.MeshIndexBufferBase;
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

		s_R3DData.CubeVertexCount = 0;
		s_R3DData.CubeVertexBufferPtr = s_R3DData.CubeVertexBufferBase;

		s_R3DData.MeshVertexCount = 0;
		s_R3DData.MeshVertexBufferPtr = s_R3DData.MeshVertexBufferBase;

		s_R3DData.MeshIndexCount = 0;
		s_R3DData.MeshIndexBufferPtr = s_R3DData.MeshIndexBufferBase;

		s_R3DData.TextureSlotIndex = 1;
		s_R3DData.MaterialSlotIndex = 0;
	}

	void Renderer3D::Flush()
	{
		LOCUS_PROFILE_FUNCTION();

		// Bind textures and uniforms
		for (uint32_t i = 0; i < s_R3DData.TextureSlotIndex; i++)
		{
			s_R3DData.TextureSlots[i]->Bind(i);
		}
		for (uint32_t i = 0; i < s_R3DData.MaterialSlotIndex; i++)
		{
			s_R3DData.MaterialBuffer[i].Albedo = glm::vec4(s_R3DData.MaterialSlots[i].Material->m_Albedo, 1.0f);
			s_R3DData.MaterialBuffer[i].Metallic = s_R3DData.MaterialSlots[i].Material->m_Metallic;
			s_R3DData.MaterialBuffer[i].Roughness = s_R3DData.MaterialSlots[i].Material->m_Roughness;
			s_R3DData.MaterialBuffer[i].AO = s_R3DData.MaterialSlots[i].Material->m_AO;
			s_R3DData.MaterialBuffer[i].AlbedoTexIndex = s_R3DData.MaterialSlots[i].AlbedoTexIndex;
			s_R3DData.MaterialBuffer[i].NormalMapTexIndex = s_R3DData.MaterialSlots[i].NormalMapTexIndex;
			s_R3DData.MaterialBuffer[i].MetallicTexIndex = s_R3DData.MaterialSlots[i].MetallicTexIndex;
			s_R3DData.MaterialBuffer[i].RoughnessTexIndex = s_R3DData.MaterialSlots[i].RoughnessTexIndex;
			s_R3DData.MaterialBuffer[i].AOTexIndex = s_R3DData.MaterialSlots[i].AOTexIndex;
		}
		s_R3DData.MaterialUniformBuffer->SetData(&s_R3DData.MaterialBuffer, sizeof(Renderer3DData::MaterialData) * s_R3DData.MaxMaterialSlots);

		// Cube
		if (s_R3DData.CubeVertexCount)
		{
			uint32_t dataSize = (uint32_t)((uint8_t*)s_R3DData.CubeVertexBufferPtr - (uint8_t*)s_R3DData.CubeVertexBufferBase);
			s_R3DData.CubeVB->SetData(s_R3DData.CubeVertexBufferBase, dataSize);

			s_R3DData.PBRShader->Bind();
			RenderCommand::DrawArray(s_R3DData.CubeVA, s_R3DData.CubeVertexCount);
			RendererStats::GetStats().DrawCalls++;
		}

		// Mesh
		if (s_R3DData.MeshIndexCount)
		{
			uint32_t vDataSize = (uint32_t)((uint8_t*)s_R3DData.MeshVertexBufferPtr - (uint8_t*)s_R3DData.MeshVertexBufferBase);
			s_R3DData.MeshVB->SetData(s_R3DData.MeshVertexBufferBase, vDataSize);
			uint32_t iDataSize = (uint32_t)((uint8_t*)s_R3DData.MeshIndexBufferPtr - (uint8_t*)s_R3DData.MeshIndexBufferBase);
			s_R3DData.MeshIB->SetData(s_R3DData.MeshIndexBufferBase, iDataSize);

			s_R3DData.PBRShader->Bind();
			RenderCommand::DrawArray(s_R3DData.MeshVA, s_R3DData.MeshIndexCount);
			RendererStats::GetStats().DrawCalls++;
		}
	}

	void Renderer3D::FlushAndReset()
	{
		LOCUS_PROFILE_FUNCTION();

		EndScene();
		
		s_R3DData.CubeVertexCount = 0;
		s_R3DData.CubeVertexBufferPtr = s_R3DData.CubeVertexBufferBase;

		s_R3DData.MeshVertexCount = 0;
		s_R3DData.MeshVertexBufferPtr = s_R3DData.MeshVertexBufferBase;

		s_R3DData.MeshIndexCount = 0;
		s_R3DData.MeshIndexBufferPtr = s_R3DData.MeshIndexBufferBase;

		s_R3DData.TextureSlotIndex = 1;
		s_R3DData.MaterialSlotIndex = 0;
	}

	void Renderer3D::DrawCube(const glm::mat4& transform, const CubeRendererComponent& crc, int entityID)
	{
		LOCUS_PROFILE_FUNCTION();

		if (s_R3DData.CubeVertexCount >= Renderer3DData::MaxVertices)
			FlushAndReset();

		// Add texture to texture slot. Handles duplicates.
		int albedoTexIndex = ProcessTextureSlot(crc.AlbedoTexture);
		int normalTexIndex = ProcessTextureSlot(crc.NormalTexture);
		int metallicTexIndex = ProcessTextureSlot(crc.MetallicTexture);
		int roughnessTexIndex = ProcessTextureSlot(crc.RoughnessTexture);
		int aoTexIndex = ProcessTextureSlot(crc.AOTexture);

		// Material uniform data
		// Temp: Create material data until cube renderer component has materials.
		Ref<Material> material = CreateRef<Material>();
		material->m_Albedo = crc.Albedo;
		material->m_AlbedoTexture = crc.AlbedoTexture;
		material->m_NormalMapTexture = crc.NormalTexture;
		material->m_Metallic = crc.Metallic;
		material->m_MetallicTexture = crc.MetallicTexture;
		material->m_Roughness = crc.Roughness;
		material->m_RoughnessTexture = crc.RoughnessTexture;
		material->m_AO = crc.AO;
		material->m_AOTexture = crc.AOTexture;
		int materialUniformIndex = ProcessMaterialSlot(material, albedoTexIndex, normalTexIndex, metallicTexIndex, roughnessTexIndex, aoTexIndex);
		
		for (uint32_t i = 0; i < 36; i++)
		{
			s_R3DData.CubeVertexBufferPtr->WorldPosition = transform * s_R3DData.CubeVertexPositions[i];
			s_R3DData.CubeVertexBufferPtr->Normal = s_R3DData.CubeVertexNormals[i];
			s_R3DData.CubeVertexBufferPtr->TexCoords = s_R3DData.CubeVertexTexCoords[i];
			s_R3DData.CubeVertexBufferPtr->MaterialUniformIndex = materialUniformIndex;
			s_R3DData.CubeVertexBufferPtr->EntityID = entityID;
			s_R3DData.CubeVertexBufferPtr++;
		}

		s_R3DData.CubeVertexCount += 36;

		RendererStats::GetStats().CubeCount++;
	}

	void Renderer3D::DrawMesh(const glm::mat4& transform, const Mesh& mesh, Ref<Material> material, int entityID)
	{
		LOCUS_PROFILE_FUNCTION();

		if (s_R3DData.MeshIndexCount + mesh.GetIndices().size() >= Renderer3DData::MaxIndices || 
			s_R3DData.MeshVertexCount + mesh.GetVertices().size() >= Renderer3DData::MaxVertices)
			FlushAndReset();

		// If texture is not in GPU memory, add texture.
		// If texture is already in GPU memory, return the slot index of that texture.
		// If texture limit is reached, flush and reset.
		int albedoTexIndex = ProcessTextureSlot(material->m_AlbedoTexture);
		int normalTexIndex = ProcessTextureSlot(material->m_NormalMapTexture);
		int metallicTexIndex = ProcessTextureSlot(material->m_MetallicTexture);
		int roughnessTexIndex = ProcessTextureSlot(material->m_RoughnessTexture);
		int aoTexIndex = ProcessTextureSlot(material->m_AOTexture);

		// Material uniform data
		int materialUniformIndex = ProcessMaterialSlot(material, albedoTexIndex, normalTexIndex, metallicTexIndex, roughnessTexIndex, aoTexIndex);

		const std::vector<ImportedMeshVertex>& vertices = mesh.GetVertices();
		const std::vector<uint32_t>& indices = mesh.GetIndices();
		{
			LOCUS_PROFILE_SCOPE("Set Vertexbuffer data");
				
			for (uint32_t i = 0; i < vertices.size(); i++)
			{
				//s_R3DData.MeshVertexBufferPtr->WorldPosition = transform * glm::vec4(vertices[i].Position, 1.0f);
				s_R3DData.MeshVertexBufferPtr->WorldPosition = vertices[i].Position;
				s_R3DData.MeshVertexBufferPtr->Normal = vertices[i].Normal;
				s_R3DData.MeshVertexBufferPtr->TexCoords = vertices[i].TexCoord;
				s_R3DData.MeshVertexBufferPtr->MaterialUniformIndex = materialUniformIndex;
				s_R3DData.MeshVertexBufferPtr->EntityID = entityID;
				s_R3DData.MeshVertexBufferPtr++;
			}
		}
		{
			LOCUS_PROFILE_SCOPE("Set Indexbuffer data");
			for (uint32_t i = 0; i < indices.size(); i++)
			{
				*s_R3DData.MeshIndexBufferPtr = indices[i];
				s_R3DData.MeshIndexBufferPtr++;
			}
		}
		
		s_R3DData.MeshVertexCount += vertices.size();
		s_R3DData.MeshIndexCount += indices.size();
	}

	void Renderer3D::DrawModel(const glm::mat4& transform, Ref<Model> model, Ref<Material> material, int entityID)
	{
		std::vector<Mesh>& meshes = model->GetMeshes();
		for (int i = 0; i < meshes.size(); i++)
			DrawMesh(transform, meshes[i], material, entityID);
	}

	void Renderer3D::DrawCubeMask(const glm::mat4& transform, Ref<Shader> shader)
	{
		s_R3DData.CubeVertexCount = 0;
		s_R3DData.CubeVertexBufferPtr = s_R3DData.CubeVertexBufferBase;

		for (uint32_t i = 0; i < 36; i++)
		{
			s_R3DData.CubeVertexBufferPtr->WorldPosition = transform * s_R3DData.CubeVertexPositions[i];
			s_R3DData.CubeVertexBufferPtr++;
		}
		s_R3DData.CubeVB->SetData(s_R3DData.CubeVertexBufferBase, sizeof(CubeVertex) * 36);

		shader->Bind();
		RenderCommand::DrawArray(s_R3DData.CubeVA, 36);

		RendererStats::GetStats().DrawCalls++;
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
				if (*s_R3DData.MaterialSlots[i].Material.get() == *material.get())
					return i;
			}
			if (s_R3DData.MaterialSlotIndex >= Renderer3DData::MaxMaterialSlots - 1)
				FlushAndReset();

			int slot = s_R3DData.MaterialSlotIndex;
			s_R3DData.MaterialSlots[slot].Material = material;
			s_R3DData.MaterialSlots[slot].AlbedoTexIndex = albedoIndex;
			s_R3DData.MaterialSlots[slot].NormalMapTexIndex = normalIndex;
			s_R3DData.MaterialSlots[slot].MetallicTexIndex = metallicIndex;
			s_R3DData.MaterialSlots[slot].RoughnessTexIndex = roughnessIndex;
			s_R3DData.MaterialSlots[slot].AOTexIndex = aoIndex;
			s_R3DData.MaterialSlotIndex++;
		}

		return 0;
	}
}