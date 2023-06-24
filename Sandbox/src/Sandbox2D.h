#pragma once

#include "Tiel.h"

class Sandbox2D : public Tiel::Layer
{
public:
	Sandbox2D();
	virtual ~Sandbox2D() {}

	virtual void OnAttach() override;
	virtual void OnDetach() override;
	virtual void OnUpdate(Tiel::Timestep deltaTime) override;
	virtual void OnEvent(Tiel::Event& event) override;
	virtual void OnImGuiRender() override;

private:
	Tiel::OrthographicCameraController m_CameraController;

	// Temp
	Tiel::Ref<Tiel::Shader> m_FlatColorShader;
	Tiel::Ref<Tiel::Texture2D> m_CheckerboardTexture;
	glm::vec4 m_SquareColor = { 0.2f, 0.5f, 0.3f, 1.0f };
};