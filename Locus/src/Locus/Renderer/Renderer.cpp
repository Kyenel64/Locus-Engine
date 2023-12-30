#include "Lpch.h"
#include "Renderer.h"

#include "Locus/Renderer/RendererStats.h"
#include "Locus/Renderer/Renderer2D.h"
#include "Locus/Renderer/Renderer3D.h"
#include "Locus/Renderer/UniformBuffer.h"
#include "Locus/Renderer/Shader.h"
#include "Locus/Scene/Scene.h"

namespace Locus
{
	struct OutlineVertex
	{
		glm::vec2 Position;
		glm::vec2 TexCoords;
	};

	struct RendererData
	{
		// Camera
		struct CameraData
		{
			glm::mat4 ViewProjection;
			glm::vec4 CameraPosition;
			glm::vec2 ViewportSize;
		};
		CameraData CameraBuffer;
		Ref<UniformBuffer> CameraUniformBuffer;

		// Outline
		Ref<VertexArray> OutlineVA;
		Ref<VertexBuffer> OutlineVB;
		Ref<Shader> OutlinePostProcessShader;
	};

	static RendererData s_Data;

	void Renderer::Init()
	{
		LOCUS_PROFILE_FUNCTION();

		RenderCommand::Init();
		Renderer2D::Init();
		Renderer3D::Init();

		// Vertex array
		s_Data.OutlineVA = VertexArray::Create();
		// Vertex buffer
		s_Data.OutlineVB = VertexBuffer::Create(6 * sizeof(OutlineVertex));
		s_Data.OutlineVB->SetLayout({
			{ ShaderDataType::Float2, "a_Position" },
			{ ShaderDataType::Float2, "a_TexCoord"}
			});
		s_Data.OutlineVA->AddVertexBuffer(s_Data.OutlineVB);
		s_Data.OutlinePostProcessShader = Shader::Create("resources/shaders/OutlinePostProcessShader.glsl");

		// Initialize uniform buffer object used in both Renderer2D and Renderer3D
		s_Data.CameraUniformBuffer = UniformBuffer::Create(sizeof(RendererData::CameraData), 0);
	}

	void Renderer::Shutdown()
	{
		Renderer2D::Shutdown();
	}

	void Renderer::BeginScene(const EditorCamera& camera)
	{
		RenderCommand::SetClearColor(camera.GetBackgroundColor());
		RenderCommand::Clear();

		s_Data.CameraBuffer.ViewProjection = camera.GetViewProjectionMatrix();
		s_Data.CameraBuffer.CameraPosition = { camera.GetPosition(), 1.0f };
		s_Data.CameraBuffer.ViewportSize = camera.GetViewportSize();
		s_Data.CameraUniformBuffer->SetData(&s_Data.CameraBuffer, sizeof(RendererData::CameraData));
	}

	void Renderer::BeginScene(const SceneCamera& camera, const glm::mat4& transform)
	{
		RenderCommand::SetClearColor(camera.GetBackgroundColor());
		RenderCommand::Clear();

		s_Data.CameraBuffer.ViewProjection = camera.GetProjection() * glm::inverse(transform);
		s_Data.CameraBuffer.CameraPosition = { transform[3].x, transform[3].y, transform[3].z, 1.0f };
		s_Data.CameraUniformBuffer->SetData(&s_Data.CameraBuffer, sizeof(RendererData::CameraData));
	}

	void Renderer::EndScene()
	{

	}

	void Renderer::DrawOutlinePostProcess(const glm::vec2& viewportSize, Ref<Framebuffer> frameBuffer)
	{
		s_Data.CameraBuffer.ViewportSize = viewportSize;
		s_Data.CameraUniformBuffer->SetData(&s_Data.CameraBuffer, sizeof(RendererData::CameraData));

		if (frameBuffer)
			frameBuffer->BindTexture(0);

		OutlineVertex vertexBuffer[6];

		glm::vec2 vertexPos[6] = { { -1.0f, -1.0f, }, { -1.0f, 1.0f }, { 1.0f, 1.0f }, { -1.0f, -1.0f }, { 1.0f, 1.0f }, { 1.0f, -1.0f} };
		glm::vec2 texCoords[6] = { { 0, 0 }, { 0, 1 }, { 1, 1 }, { 0, 0 }, { 1, 1 }, { 1, 0 } };

		for (uint32_t i = 0; i < 6; i++)
		{
			vertexBuffer[i].Position = vertexPos[i];
			vertexBuffer[i].TexCoords = texCoords[i];
		}

		s_Data.OutlineVB->SetData(vertexBuffer, sizeof(OutlineVertex) * 6);

		s_Data.OutlinePostProcessShader->Bind();
		RenderCommand::DrawArray(s_Data.OutlineVA, 6);

		RendererStats::GetStats().DrawCalls++;
	}

	void Renderer::OnWindowResize(uint32_t width, uint32_t height)
	{
		RenderCommand::Resize(0, 0, width, height);
	}
}
