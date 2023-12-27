#include "Lpch.h"
#include "Renderer3D.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

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
		glm::vec4 Color;
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

		uint32_t CubeIndexCount = 0;
		CubeVertex* CubeVertexBufferBase = nullptr;
		CubeVertex* CubeVertexBufferPtr = nullptr;

		uint32_t GridIndexCount = 0;
		GridVertex* GridVertexBufferBase = nullptr;
		GridVertex* GridVertexBufferPtr = nullptr;

		glm::vec4 CubeVertexPositions[8];

		// Camera
		struct CameraData
		{
			glm::mat4 ViewProjection;
		};
		CameraData CameraBuffer;
		Ref<UniformBuffer> CameraUniformBuffer;

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

	static Renderer3DData s_Data;

	void Renderer3D::Init()
	{
		LOCUS_PROFILE_FUNCTION();

		// --- Cube -----------------------------------------------------------
		// Vertex array
		s_Data.CubeVA = VertexArray::Create();
		// Vertex buffer
		s_Data.CubeVB = VertexBuffer::Create(s_Data.MaxVertices * sizeof(CubeVertex));
		s_Data.CubeVB->SetLayout({
			{ ShaderDataType::Float3, "a_Position" },
			{ ShaderDataType::Float4, "a_Color" },
			{ ShaderDataType::Int, "a_EntityID" }
		});
		s_Data.CubeVA->AddVertexBuffer(s_Data.CubeVB);
		// Index buffer
		uint32_t* cubeIndices = new uint32_t[s_Data.MaxIndices];
		uint32_t offset = 0;
		for (uint32_t i = 0; i < s_Data.MaxIndices; i += 36)
		{
			// Front
			cubeIndices[i + 0] = offset + 0;
			cubeIndices[i + 1] = offset + 1;
			cubeIndices[i + 2] = offset + 2;

			cubeIndices[i + 3] = offset + 2;
			cubeIndices[i + 4] = offset + 3;
			cubeIndices[i + 5] = offset + 0;

			// Left
			cubeIndices[i + 6] = offset + 4;
			cubeIndices[i + 7] = offset + 5;
			cubeIndices[i + 8] = offset + 1;

			cubeIndices[i + 9] = offset + 1;
			cubeIndices[i + 10] = offset + 0;
			cubeIndices[i + 11] = offset + 4;

			// Back
			cubeIndices[i + 12] = offset + 7;
			cubeIndices[i + 13] = offset + 6;
			cubeIndices[i + 14] = offset + 5;

			cubeIndices[i + 15] = offset + 5;
			cubeIndices[i + 16] = offset + 4;
			cubeIndices[i + 17] = offset + 7;

			// Right
			cubeIndices[i + 18] = offset + 3;
			cubeIndices[i + 19] = offset + 2;
			cubeIndices[i + 20] = offset + 6;

			cubeIndices[i + 21] = offset + 6;
			cubeIndices[i + 22] = offset + 7;
			cubeIndices[i + 23] = offset + 3;

			// Top
			cubeIndices[i + 24] = offset + 1;
			cubeIndices[i + 25] = offset + 5;
			cubeIndices[i + 26] = offset + 6;

			cubeIndices[i + 27] = offset + 6;
			cubeIndices[i + 28] = offset + 2;
			cubeIndices[i + 29] = offset + 1;

			// Bottom
			cubeIndices[i + 30] = offset + 0;
			cubeIndices[i + 31] = offset + 4;
			cubeIndices[i + 32] = offset + 7;

			cubeIndices[i + 33] = offset + 7;
			cubeIndices[i + 34] = offset + 3;
			cubeIndices[i + 35] = offset + 0;

			offset += 8;
		}
		Ref<IndexBuffer> cubeIB = IndexBuffer::Create(cubeIndices, s_Data.MaxIndices);
		s_Data.CubeVA->SetIndexBuffer(cubeIB);
		delete[] cubeIndices;
		s_Data.CubeVertexBufferBase = new CubeVertex[s_Data.MaxVertices];

		// --- Grid -----------------------------------------------------------
		s_Data.GridVA = VertexArray::Create();
		// Create VB
		s_Data.GridVB = VertexBuffer::Create(4 * sizeof(GridVertex));
		s_Data.GridVB->SetLayout({
			{ ShaderDataType::Int, "a_Index"}
			});
		s_Data.GridVA->AddVertexBuffer(s_Data.GridVB);
		// Create IB
		uint32_t* gridIndices = new uint32_t[6];
		gridIndices[0] = 0;
		gridIndices[1] = 1;
		gridIndices[2] = 2;

		gridIndices[3] = 2;
		gridIndices[4] = 3;
		gridIndices[5] = 0;

		Ref<IndexBuffer> gridIB = IndexBuffer::Create(gridIndices, 6);
		s_Data.GridVA->SetIndexBuffer(gridIB);
		delete[] gridIndices;
		s_Data.GridVertexBufferBase = new GridVertex[4];

		// --- Initializations ------------------------------------------------
		s_Data.CubeShader = Shader::Create("resources/shaders/3DCube.glsl");
		s_Data.GridShader = Shader::Create("resources/shaders/GridShader.glsl");

		// Define cube vertices
		s_Data.CubeVertexPositions[0] = { -0.5f, -0.5f,  0.5f, 1.0f };
		s_Data.CubeVertexPositions[1] = { -0.5f,  0.5f,  0.5f, 1.0f };
		s_Data.CubeVertexPositions[2] = {  0.5f,  0.5f,  0.5f, 1.0f };
		s_Data.CubeVertexPositions[3] = {  0.5f, -0.5f,  0.5f, 1.0f };
		s_Data.CubeVertexPositions[4] = { -0.5f, -0.5f, -0.5f, 1.0f };
		s_Data.CubeVertexPositions[5] = { -0.5f,  0.5f, -0.5f, 1.0f };
		s_Data.CubeVertexPositions[6] = {  0.5f,  0.5f, -0.5f, 1.0f };
		s_Data.CubeVertexPositions[7] = {  0.5f, -0.5f, -0.5f, 1.0f };

		s_Data.CameraUniformBuffer = UniformBuffer::Create(sizeof(Renderer3DData::CameraData), 0);
		s_Data.GridUniformBuffer = UniformBuffer::Create(sizeof(Renderer3DData::GridData), 1);
	}

	void Renderer3D::Shutdown()
	{
		LOCUS_PROFILE_FUNCTION();

		delete[] s_Data.CubeVertexBufferBase;
		delete[] s_Data.GridVertexBufferBase;
	}

	void Renderer3D::BeginScene(const EditorCamera& camera)
	{
		LOCUS_PROFILE_FUNCTION();

		s_Data.CameraBuffer.ViewProjection = camera.GetViewProjectionMatrix();
		s_Data.CameraUniformBuffer->SetData(&s_Data.CameraBuffer, sizeof(Renderer3DData::CameraData));

		// Editor grid
		s_Data.GridBuffer.Color = camera.GetGridColor();
		s_Data.GridBuffer.Near = camera.GetNearClip();
		s_Data.GridBuffer.Far = camera.GetFarClip();
		s_Data.GridBuffer.GridScale = camera.GetGridScale();
		s_Data.GridUniformBuffer->SetData(&s_Data.GridBuffer, sizeof(Renderer3DData::GridData));

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

		s_Data.CubeIndexCount = 0;
		s_Data.CubeVertexBufferPtr = s_Data.CubeVertexBufferBase;

		s_Data.GridIndexCount = 0;
		s_Data.GridVertexBufferPtr = s_Data.GridVertexBufferBase;
	}

	void Renderer3D::Flush()
	{
		LOCUS_PROFILE_FUNCTION();

		if (s_Data.CubeIndexCount)
		{
			uint32_t dataSize = (uint32_t)((uint8_t*)s_Data.CubeVertexBufferPtr - (uint8_t*)s_Data.CubeVertexBufferBase);
			s_Data.CubeVB->SetData(s_Data.CubeVertexBufferBase, dataSize);

			s_Data.CubeShader->Bind();
			RenderCommand::DrawIndexed(s_Data.CubeVA, s_Data.CubeIndexCount);

			RendererStats::GetStats().DrawCalls++;
		}

		if (s_Data.GridIndexCount)
		{
			uint32_t dataSize = (uint32_t)((uint8_t*)s_Data.GridVertexBufferPtr - (uint8_t*)s_Data.GridVertexBufferBase);
			s_Data.GridVB->SetData(s_Data.GridVertexBufferBase, dataSize);

			s_Data.GridShader->Bind();
			RenderCommand::DrawIndexed(s_Data.GridVA, s_Data.GridIndexCount);

			RendererStats::GetStats().DrawCalls++;
		}
	}

	void Renderer3D::FlushAndReset()
	{
		LOCUS_PROFILE_FUNCTION();

		EndScene();
		
		s_Data.CubeIndexCount = 0;
		s_Data.CubeVertexBufferPtr = s_Data.CubeVertexBufferBase;

		s_Data.GridIndexCount = 0;
		s_Data.GridVertexBufferPtr = s_Data.GridVertexBufferBase;
	}

	void Renderer3D::DrawCube(const glm::mat4& transform, const glm::vec4& color, int entityID)
	{
		LOCUS_PROFILE_FUNCTION();

		if (s_Data.CubeIndexCount >= Renderer3DData::MaxIndices)
			FlushAndReset();

		for (uint32_t i = 0; i < 8; i++)
		{
			s_Data.CubeVertexBufferPtr->WorldPosition = transform * s_Data.CubeVertexPositions[i];
			s_Data.CubeVertexBufferPtr->Color = color;
			s_Data.CubeVertexBufferPtr->EntityID = entityID;
			s_Data.CubeVertexBufferPtr++;
		}

		s_Data.CubeIndexCount += 36;

		RendererStats::GetStats().CubeCount++;
	}

	void Renderer3D::DrawGrid()
	{
		LOCUS_PROFILE_FUNCTION();

		for (uint32_t i = 0; i < 4; i++)
		{
			s_Data.GridVertexBufferPtr->Index = i;
			s_Data.GridVertexBufferPtr++;
		}

		s_Data.GridIndexCount += 6;
	}
}