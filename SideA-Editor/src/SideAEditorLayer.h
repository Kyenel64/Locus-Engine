#pragma once

#include "SideA.h"

namespace SideA
{
	class SideAEditorLayer : public Layer
	{
	public:
		SideAEditorLayer();
		virtual ~SideAEditorLayer() {}

		virtual void OnAttach() override;
		virtual void OnDetach() override;
		virtual void OnUpdate(Timestep deltaTime) override;
		virtual void OnEvent(Event& event) override;
		virtual void OnImGuiRender() override;

	private:
		OrthographicCameraController m_CameraController;
		Ref<Framebuffer> m_Framebuffer;
		bool m_ViewportFocused = false; bool m_ViewportHovered = false;

		// Temp
		Ref<Texture2D> m_CheckerboardTexture;
		Ref<Texture2D> m_SpriteSheet;
		Ref<SubTexture2D> m_Grass;

		Ref<Scene> m_ActiveScene;
		//Temp
		Entity m_SquareEntity;
		Entity m_CameraEntity;
		Entity m_SecondCamera;
		bool m_PrimaryCamera = true;


		glm::vec2 m_ViewportSize = { 0.0f, 0.0f };
		glm::vec4 m_SquareColor = { 0.2f, 0.5f, 0.3f, 1.0f };
	};
}
