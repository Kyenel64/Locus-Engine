#include "SideAExport.h"


#include "SideA/Scene/SceneSerializer.h"

namespace SideA
{
	SideAExport::SideAExport()
		: Layer("SideAExport"), m_CameraController(1920.0f / 1080.0f)
	{

	}

	void SideAExport::OnAttach()
	{
		SIDEA_PROFILE_FUNCTION();

		// Framebuffer
		FramebufferSpecs FramebufferSpecs;
		FramebufferSpecs.Width = 1920;
		FramebufferSpecs.Height = 1080;
		m_Framebuffer = Framebuffer::Create(FramebufferSpecs);


		std::string path = "assets/scenes/Example.sidea";
		if (!path.empty())
		{
			m_ActiveScene = CreateRef<Scene>();
			m_ActiveScene->OnViewportResize((uint32_t)m_ViewportSize.x, (uint32_t)m_ViewportSize.y);

			SceneSerializer serializer(m_ActiveScene);
			serializer.Deserialize(path);
		}
		else
		{
			m_ActiveScene = CreateRef<Scene>();
		}

	}

	void SideAExport::OnDetach()
	{
		SIDEA_PROFILE_FUNCTION();
	}

	void SideAExport::OnUpdate(Timestep deltaTime)
	{
		// Profiling
		SIDEA_PROFILE_FUNCTION();

		if (FramebufferSpecs spec = m_Framebuffer->GetSpecification();
			m_ViewportSize.x > 0.0f && m_ViewportSize.y > 0.0f &&
			(spec.Width != m_ViewportSize.x || spec.Height != m_ViewportSize.y))
		{
			m_Framebuffer->Resize((uint32_t)m_ViewportSize.x, (uint32_t)m_ViewportSize.y);
			m_CameraController.OnResize(m_ViewportSize.x, m_ViewportSize.y);
			m_ActiveScene->OnViewportResize((uint32_t)m_ViewportSize.x, (uint32_t)m_ViewportSize.y);
		}

		// Update
		if (m_ViewportFocused)
			m_CameraController.OnUpdate(deltaTime);

		// Render
		//m_Framebuffer->Bind();
		m_ActiveScene->OnUpdate(deltaTime);
		//m_Framebuffer->Unbind();

	}

	void SideAExport::OnEvent(Event& e)
	{
		m_CameraController.OnEvent(e);

	}
}
