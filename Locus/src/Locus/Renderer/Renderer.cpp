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
	struct ScreenVertex
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

		// Screen
		Ref<VertexArray> ScreenVA;
		Ref<VertexBuffer> ScreenVB;
		ScreenVertex ScreenVertexBuffer[6];

		// Debug
		std::vector<int> UBOBindings;
	};

	static RendererData s_Data;

	void Renderer::Init()
	{
		LOCUS_PROFILE_FUNCTION();

		RenderCommand::Init();
		Renderer2D::Init();
		Renderer3D::Init();

		// --- Screen ---
		// Vertex array
		s_Data.ScreenVA = VertexArray::Create();
		// Vertex buffer
		s_Data.ScreenVB = VertexBuffer::Create(6 * sizeof(ScreenVertex));
		s_Data.ScreenVB->SetLayout({
			{ ShaderDataType::Float2, "a_Position" },
			{ ShaderDataType::Float2, "a_TexCoord"}
			});
		s_Data.ScreenVA->AddVertexBuffer(s_Data.ScreenVB);
		// Define screen vertex data
#pragma region Screen vertex definitions
		s_Data.ScreenVertexBuffer[0].Position = { -1.0f, -1.0f };
		s_Data.ScreenVertexBuffer[0].TexCoords = { 0, 0 };
		s_Data.ScreenVertexBuffer[1].Position = { -1.0f, 1.0f };
		s_Data.ScreenVertexBuffer[1].TexCoords = { 0, 1 };
		s_Data.ScreenVertexBuffer[2].Position = { 1.0f, 1.0f };
		s_Data.ScreenVertexBuffer[2].TexCoords = { 1, 1 };
		s_Data.ScreenVertexBuffer[3].Position = { -1.0f, -1.0f };
		s_Data.ScreenVertexBuffer[3].TexCoords = { 0, 0 };
		s_Data.ScreenVertexBuffer[4].Position = { 1.0f, 1.0f };
		s_Data.ScreenVertexBuffer[4].TexCoords = { 1, 1 };
		s_Data.ScreenVertexBuffer[5].Position = { 1.0f, -1.0f };
		s_Data.ScreenVertexBuffer[5].TexCoords = { 1, 0 };

#pragma endregion Screen vertex definitions
		s_Data.ScreenVB->SetData(s_Data.ScreenVertexBuffer, sizeof(ScreenVertex) * 6);

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

	void Renderer::DrawPostProcess(Ref<Texture> texture, Ref<Shader> shader)
	{
		s_Data.CameraBuffer.ViewportSize = { texture->GetWidth(), texture->GetHeight() };
		s_Data.CameraUniformBuffer->SetData(&s_Data.CameraBuffer, sizeof(RendererData::CameraData));

		texture->Bind();
		shader->Bind();
		RenderCommand::DrawArray(s_Data.ScreenVA, 6);

		RendererStats::GetStats().DrawCalls++;
	}

	void Renderer::OnWindowResize(uint32_t width, uint32_t height)
	{
		RenderCommand::Resize(0, 0, width, height);
	}

	std::vector<int>& Renderer::GetUBOBindings()
	{
		return s_Data.UBOBindings;
	}
}
