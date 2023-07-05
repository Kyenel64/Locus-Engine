#include "TielRPG.h"

glm::vec2 pos = { 0.0f, 0.0f };
float playerSpeed = 1.0f;

TielRPG::TielRPG() : Layer("TielRPG"), m_CameraController(1920.0f / 1080.0f)
{

}

void TielRPG::OnAttach()
{
	m_CheckerboardTexture = Tiel::Texture2D::Create("assets/textures/Checkerboard.png");
}

void TielRPG::OnDetach()
{

}

void TielRPG::OnUpdate(Tiel::Timestep deltaTime)
{
	if (Tiel::Input::IsKeyPressed(TIEL_KEY_UP))
	{
		pos.y += playerSpeed * deltaTime;
	}

	m_CameraController.OnUpdate(deltaTime);

	Tiel::RenderCommand::SetClearColor({ 0.2f, 0.2f, 0.25f, 1 });
	Tiel::RenderCommand::Clear();

	Tiel::Renderer2D::BeginScene(m_CameraController.GetCamera());

	Tiel::Renderer2D::DrawQuad(pos, { 0.2f, 0.2f }, { 0.1f, 0.1f, 0.4f, 1.0f });
	Tiel::Renderer2D::DrawQuad({ -5.0f, -5.0f, -0.1f}, { 10.0f, 10.0f }, m_CheckerboardTexture, 10.0f);

	Tiel::Renderer2D::EndScene();
}

void TielRPG::OnEvent(Tiel::Event& e)
{

}

void TielRPG::OnImGuiRender()
{

}