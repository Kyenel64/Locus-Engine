#include "Sandbox2D.h"

#include "imgui/imgui.h"

#include <glm/gtc/type_ptr.hpp>

Sandbox2D::Sandbox2D()
	: Layer("Sandbox2D"), m_CameraController(1920.0f / 1080.0f)
{

}

void Sandbox2D::OnAttach()
{
	TIEL_PROFILE_FUNCTION();

	m_CheckerboardTexture = Tiel::Texture2D::Create("assets/textures/Checkerboard.png");
}

void Sandbox2D::OnDetach()
{
	TIEL_PROFILE_FUNCTION();
}

void Sandbox2D::OnUpdate(Tiel::Timestep deltaTime)
{
	TIEL_PROFILE_FUNCTION();

	// Update
	m_CameraController.OnUpdate(deltaTime);

	// Render
	{
		TIEL_PROFILE_SCOPE("Renderer Prep");
		Tiel::RenderCommand::SetClearColor({ 0.2f, 0.2f, 0.25f, 1 });
		Tiel::RenderCommand::Clear();
	}

	{
		TIEL_PROFILE_SCOPE("Render Draws");
		Tiel::Renderer2D::BeginScene(m_CameraController.GetCamera());

		Tiel::Renderer2D::DrawQuad({ 0.2f, -0.2f }, { 2.0f, 1.0f }, m_SquareColor);
		Tiel::Renderer2D::DrawQuad({ -0.2f, 0.2f }, { 2.0f, 1.0f }, { 0.8f, 0.2f, 0.4f, 1.0f });
		Tiel::Renderer2D::DrawQuad({ 0.0f, 0.0f, -0.1f }, { 10.0f, 10.0f }, m_CheckerboardTexture);

		Tiel::Renderer2D::EndScene();
	}

	// TODO: SetMat4, SetFloat4
	//m_FlatColorShader->Bind();
	//m_FlatColorShader->SetFloat4("u_Color", m_SquareColor);


}

void Sandbox2D::OnEvent(Tiel::Event& e)
{
	m_CameraController.OnEvent(e);
}

void Sandbox2D::OnImGuiRender()
{
	TIEL_PROFILE_FUNCTION();

	ImGui::Begin("Settings");
	ImGui::ColorEdit4("Square Color", glm::value_ptr(m_SquareColor));
	ImGui::End();
}