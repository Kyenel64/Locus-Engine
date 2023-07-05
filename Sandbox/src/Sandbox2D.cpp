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
	Tiel::Renderer2D::ResetStats();
	{
		TIEL_PROFILE_SCOPE("Renderer Prep");
		Tiel::RenderCommand::SetClearColor({ 0.2f, 0.2f, 0.25f, 1 });
		Tiel::RenderCommand::Clear();
	}

	{
		static float rotation = 0.0f;
		rotation += deltaTime * 50.0f;

		TIEL_PROFILE_SCOPE("Render Draws");
		Tiel::Renderer2D::BeginScene(m_CameraController.GetCamera());
		Tiel::Renderer2D::DrawQuad({ 0.2f, -0.2f }, { 0.1f, 0.2f }, m_SquareColor);
		Tiel::Renderer2D::DrawQuad({ -0.2f, 0.2f }, { 0.3f, 0.3f }, { 0.8f, 0.2f, 0.4f, 1.0f });
		Tiel::Renderer2D::DrawRotatedQuad({ -0.2f, -0.2f }, rotation, { 0.3f, 0.1f }, m_SquareColor);
		Tiel::Renderer2D::DrawQuad({ 0.0f, 0.0f, -0.1f }, { 10.0f, 10.0f }, m_CheckerboardTexture, 10.0f);
		Tiel::Renderer2D::EndScene();

		Tiel::Renderer2D::BeginScene(m_CameraController.GetCamera());
		for (float y = -5.0f; y < 5.0f; y += 0.5f)
		{
			for (float x = -5.0f; x < 5.0f; x += 0.5f)
			{
				glm::vec4 color = { (x + 5.0f) / 10.0f, 0.4f, (y + 5.0f) / 10.0f, 0.7f };
				Tiel::Renderer2D::DrawQuad({ x, y }, { 0.45f, 0.45f }, color);
			}
		}
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

	auto stats = Tiel::Renderer2D::GetStats();
	ImGui::Text("Renderer2D Stats:");
	ImGui::Text("Draw Calls: %d", stats.DrawCalls);
	ImGui::Text("Quads: %d", stats.QuadCount);
	ImGui::Text("Vertices: %d", stats.GetTotalVertexCount());
	ImGui::Text("Indices: %d", stats.GetTotalIndexCount());

	ImGui::ColorEdit4("Square Color", glm::value_ptr(m_SquareColor));
	ImGui::End();
}