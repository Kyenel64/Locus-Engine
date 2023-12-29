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

		// TODO: Probably put material data in uniform buffer
		glm::vec4 Albedo; 
		float Metallic;
		float Roughness;
		float AO;

		int EntityID;
	};

	struct GridVertex
	{
		int Index;
	};

	struct Renderer3DData
	{
		static const uint32_t MaxCubes = 20000;
		static const uint32_t MaxVertices = MaxCubes * 8;
		static const uint32_t MaxIndices = MaxCubes * 36;

		// Cube
		Ref<VertexArray> CubeVA;
		Ref<VertexBuffer> CubeVB;
		Ref<Shader> CubeShader;

		// Grid
		Ref<VertexArray> GridVA;
		Ref<VertexBuffer> GridVB;
		Ref<Shader> GridShader;

		uint32_t CubeVertexCount = 0;
		CubeVertex* CubeVertexBufferBase = nullptr;
		CubeVertex* CubeVertexBufferPtr = nullptr;

		uint32_t GridIndexCount = 0;
		GridVertex* GridVertexBufferBase = nullptr;
		GridVertex* GridVertexBufferPtr = nullptr;

		glm::vec4 CubeVertexPositions[36];
		glm::vec3 CubeVertexNormals[36];

		// Lighting data
		SceneLighting SceneLightingBuffer;
		Ref<UniformBuffer> SceneLightingUniformBuffer;

		// Grid
		struct GridData
		{
			glm::vec4 Color;
			float Near;
			float Far;
			float GridScale;
		};
		GridData GridBuffer;
		Ref<UniformBuffer> GridUniformBuffer;
	};

	static Renderer3DData s_R3DData;

	void Renderer3D::Init()
	{
		LOCUS_PROFILE_FUNCTION();

		// --- Cube -----------------------------------------------------------
		// Vertex array
		s_R3DData.CubeVA = VertexArray::Create();
		// Vertex buffer
		s_R3DData.CubeVB = VertexBuffer::Create(s_R3DData.MaxVertices * sizeof(CubeVertex));
		s_R3DData.CubeVB->SetLayout({
			{ ShaderDataType::Float3, "a_Position" },
			{ ShaderDataType::Float3, "a_Normal"},
			{ ShaderDataType::Float4, "a_Albedo" },
			{ ShaderDataType::Float, "a_Metallic" },
			{ ShaderDataType::Float, "a_Roughness" },
			{ ShaderDataType::Float, "a_AO"},
			{ ShaderDataType::Int, "a_EntityID" }
		});
		s_R3DData.CubeVA->AddVertexBuffer(s_R3DData.CubeVB);
		s_R3DData.CubeVertexBufferBase = new CubeVertex[s_R3DData.MaxVertices];

		// --- Grid -----------------------------------------------------------
		s_R3DData.GridVA = VertexArray::Create();
		// Create VB
		s_R3DData.GridVB = VertexBuffer::Create(4 * sizeof(GridVertex));
		s_R3DData.GridVB->SetLayout({
			{ ShaderDataType::Int, "a_Index"}
			});
		s_R3DData.GridVA->AddVertexBuffer(s_R3DData.GridVB);
		// Create IB
		uint32_t* gridIndices = new uint32_t[6];
		gridIndices[0] = 0;
		gridIndices[1] = 1;
		gridIndices[2] = 2;

		gridIndices[3] = 2;
		gridIndices[4] = 3;
		gridIndices[5] = 0;

		Ref<IndexBuffer> gridIB = IndexBuffer::Create(gridIndices, 6);
		s_R3DData.GridVA->SetIndexBuffer(gridIB);
		delete[] gridIndices;
		s_R3DData.GridVertexBufferBase = new GridVertex[4];

		// --- Initializations ------------------------------------------------
		s_R3DData.CubeShader = Shader::Create("resources/shaders/FlatColorPBR.glsl");
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

#pragma endregion Cube vertex definitions
		
		// Uniform buffers
		s_R3DData.GridUniformBuffer = UniformBuffer::Create(sizeof(Renderer3DData::GridData), 1);
		s_R3DData.SceneLightingUniformBuffer = UniformBuffer::Create(sizeof(SceneLighting), 2);
	}

	void Renderer3D::Shutdown()
	{
		LOCUS_PROFILE_FUNCTION();

		delete[] s_R3DData.CubeVertexBufferBase;
		delete[] s_R3DData.GridVertexBufferBase;
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

		s_R3DData.GridIndexCount = 0;
		s_R3DData.GridVertexBufferPtr = s_R3DData.GridVertexBufferBase;
	}

	void Renderer3D::Flush()
	{
		LOCUS_PROFILE_FUNCTION();

		if (s_R3DData.CubeVertexCount)
		{
			uint32_t dataSize = (uint32_t)((uint8_t*)s_R3DData.CubeVertexBufferPtr - (uint8_t*)s_R3DData.CubeVertexBufferBase);
			s_R3DData.CubeVB->SetData(s_R3DData.CubeVertexBufferBase, dataSize);

			s_R3DData.CubeShader->Bind();
			RenderCommand::DrawArray(s_R3DData.CubeVA, s_R3DData.CubeVertexCount);

			RendererStats::GetStats().DrawCalls++;
		}

		if (s_R3DData.GridIndexCount)
		{
			uint32_t dataSize = (uint32_t)((uint8_t*)s_R3DData.GridVertexBufferPtr - (uint8_t*)s_R3DData.GridVertexBufferBase);
			s_R3DData.GridVB->SetData(s_R3DData.GridVertexBufferBase, dataSize);

			s_R3DData.GridShader->Bind();
			RenderCommand::DrawIndexed(s_R3DData.GridVA, s_R3DData.GridIndexCount);

			RendererStats::GetStats().DrawCalls++;
		}
	}

	void Renderer3D::FlushAndReset()
	{
		LOCUS_PROFILE_FUNCTION();

		EndScene();
		
		s_R3DData.CubeVertexCount = 0;
		s_R3DData.CubeVertexBufferPtr = s_R3DData.CubeVertexBufferBase;

		s_R3DData.GridIndexCount = 0;
		s_R3DData.GridVertexBufferPtr = s_R3DData.GridVertexBufferBase;
	}

	void Renderer3D::DrawCube(const glm::mat4& transform, const CubeRendererComponent& crc, int entityID)
	{
		LOCUS_PROFILE_FUNCTION();

		if (s_R3DData.CubeVertexCount >= Renderer3DData::MaxIndices)
			FlushAndReset();

		for (uint32_t i = 0; i < 36; i++)
		{
			s_R3DData.CubeVertexBufferPtr->WorldPosition = transform * s_R3DData.CubeVertexPositions[i];
			s_R3DData.CubeVertexBufferPtr->Normal = s_R3DData.CubeVertexNormals[i];
			s_R3DData.CubeVertexBufferPtr->Albedo = crc.Albedo;
			s_R3DData.CubeVertexBufferPtr->Metallic = crc.Metallic;
			s_R3DData.CubeVertexBufferPtr->Roughness = crc.Roughness;
			s_R3DData.CubeVertexBufferPtr->AO = crc.AO;
			s_R3DData.CubeVertexBufferPtr->EntityID = entityID;
			s_R3DData.CubeVertexBufferPtr++;
		}

		s_R3DData.CubeVertexCount += 36;

		RendererStats::GetStats().CubeCount++;
	}

	void Renderer3D::DrawGrid()
	{
		LOCUS_PROFILE_FUNCTION();

		for (uint32_t i = 0; i < 4; i++)
		{
			s_R3DData.GridVertexBufferPtr->Index = i;
			s_R3DData.GridVertexBufferPtr++;
		}

		s_R3DData.GridIndexCount += 6;
	}
}