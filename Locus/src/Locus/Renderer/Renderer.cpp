#include "Lpch.h"
#include "Renderer.h"

#include "Locus/Renderer/Renderer2D.h"
#include "Locus/Renderer/Renderer3D.h"
#include "Locus/Renderer/UniformBuffer.h"

namespace Locus
{
	struct RendererData
	{
		struct CameraData
		{
			glm::mat4 ViewProjection;
			glm::vec4 CameraPosition;
		};
		CameraData CameraBuffer;
		Ref<UniformBuffer> CameraUniformBuffer;
	};

	static RendererData s_Data;

	void Renderer::Init()
	{
		LOCUS_PROFILE_FUNCTION();

		RenderCommand::Init();
		Renderer2D::Init();
		Renderer3D::Init();

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

	void Renderer::OnWindowResize(uint32_t width, uint32_t height)
	{
		RenderCommand::Resize(0, 0, width, height);
	}
}
