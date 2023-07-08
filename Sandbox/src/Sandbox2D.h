#pragma once

#include "SideA.h"

class Sandbox2D : public SideA::Layer
{
public:
	Sandbox2D();
	virtual ~Sandbox2D() {}

	virtual void OnAttach() override;
	virtual void OnDetach() override;
	virtual void OnUpdate(SideA::Timestep deltaTime) override;
	virtual void OnEvent(SideA::Event& event) override;
	virtual void OnImGuiRender() override;

private:
	SideA::OrthographicCameraController m_CameraController;

	// Temp
	SideA::Ref<SideA::Shader> m_FlatColorShader;
	SideA::Ref<SideA::Texture2D> m_CheckerboardTexture;
	SideA::Ref<SideA::Texture2D> m_SpriteSheet;
	SideA::Ref<SideA::SubTexture2D> m_Grass;

	glm::vec4 m_SquareColor = { 0.2f, 0.5f, 0.3f, 1.0f };
};