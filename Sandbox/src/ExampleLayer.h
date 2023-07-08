#pragma once

#include "SideA.h"

class ExampleLayer : public SideA::Layer
{
public:
	ExampleLayer();

	void OnUpdate(SideA::Timestep deltaTime) override;
	void OnEvent(SideA::Event& e) override;
	virtual void OnImGuiRender() override;


private:

	SideA::ShaderLibrary m_ShaderLibrary;
	SideA::Ref<SideA::Shader> m_Shader;
	SideA::Ref<SideA::VertexArray> m_VertexArray;

	SideA::Ref<SideA::Shader> m_FlatColorShader;
	SideA::Ref<SideA::VertexArray> m_SquareVA;

	SideA::Ref<SideA::Texture2D> m_Texture, m_CockatielTexture;

	SideA::OrthographicCameraController m_CameraController;

	glm::vec3 m_SquareColor = { 0.2f, 0.3f, 0.8f };
};