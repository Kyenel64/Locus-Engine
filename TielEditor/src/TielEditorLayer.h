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

		// Temp
		Ref<Shader> m_FlatColorShader;
		Ref<Texture2D> m_CheckerboardTexture;
		Ref<Texture2D> m_SpriteSheet;
		Ref<SubTexture2D> m_Grass;
		Ref<Framebuffer> m_Framebuffer;

		glm::vec4 m_SquareColor = { 0.2f, 0.5f, 0.3f, 1.0f };
	};
}
