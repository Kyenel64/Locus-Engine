#pragma once

#include "Tiel.h"

namespace Tiel
{
	class TielEditorLayer : public Layer
	{
	public:
		TielEditorLayer();
		virtual ~TielEditorLayer() {}

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
		Ref<Shader> m_FlatColorShader;
		Ref<Texture2D> m_CheckerboardTexture;
		Ref<Texture2D> m_SpriteSheet;
		Ref<SubTexture2D> m_Grass;

		glm::vec2 m_ViewportSize = { 0.0f, 0.0f };
		glm::vec4 m_SquareColor = { 0.2f, 0.5f, 0.3f, 1.0f };
	};
}
