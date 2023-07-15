#pragma once

#include "SideA.h"

namespace SideA
{
	class SideAExport : public Layer
	{
	public:
		SideAExport();
		virtual ~SideAExport() {}

		virtual void OnAttach() override;
		virtual void OnDetach() override;
		virtual void OnUpdate(Timestep deltaTime) override;
		virtual void OnEvent(Event& event) override;

	private:
		OrthographicCameraController m_CameraController;
		Ref<Framebuffer> m_Framebuffer;
		bool m_ViewportFocused = false; bool m_ViewportHovered = false;

		Ref<Scene> m_ActiveScene;
		glm::vec2 m_ViewportSize = { 0.0f, 0.0f };

		// Panels
		std::string m_SavePath;
	};
}
