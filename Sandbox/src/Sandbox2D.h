#pragma once

#include "Locus.h"

class Sandbox2D : public Locus::Layer
{
public:
	Sandbox2D();
	virtual ~Sandbox2D() {}

	virtual void OnAttach() override;
	virtual void OnDetach() override;
	virtual void OnUpdate(Locus::Timestep deltaTime) override;
	virtual void OnEvent(Locus::Event& event) override;
	virtual void OnImGuiRender() override;

private:
	Locus::OrthographicCameraController m_CameraController;

	// Temp
	Locus::Ref<Locus::Shader> m_FlatColorShader;
	Locus::Ref<Locus::Texture2D> m_CheckerboardTexture;
	Locus::Ref<Locus::Texture2D> m_SpriteSheet;
	Locus::Ref<Locus::SubTexture2D> m_Grass;

	glm::vec4 m_SquareColor = { 0.2f, 0.5f, 0.3f, 1.0f };
};