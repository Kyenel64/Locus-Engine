#pragma once

#include "Tiel.h"

class ExampleLayer : public Tiel::Layer
{
public:
	ExampleLayer();

	void OnUpdate(Tiel::Timestep deltaTime) override;
	void OnEvent(Tiel::Event& e) override;
	virtual void OnImGuiRender() override;


private:

	Tiel::ShaderLibrary m_ShaderLibrary;
	Tiel::Ref<Tiel::Shader> m_Shader;
	Tiel::Ref<Tiel::VertexArray> m_VertexArray;

	Tiel::Ref<Tiel::Shader> m_FlatColorShader;
	Tiel::Ref<Tiel::VertexArray> m_SquareVA;

	Tiel::Ref<Tiel::Texture2D> m_Texture, m_CockatielTexture;

	Tiel::OrthographicCameraController m_CameraController;

	glm::vec3 m_SquareColor = { 0.2f, 0.3f, 0.8f };
};