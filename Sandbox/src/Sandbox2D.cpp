#include "Sandbox2D.h"

#include "imgui/imgui.h"

#include <glm/gtc/type_ptr.hpp>

Sandbox2D::Sandbox2D()
	: Layer("Sandbox2D"), m_CameraController(1920.0f / 1080.0f)
{

}

void Sandbox2D::OnAttach()
{
	SIDEA_PROFILE_FUNCTION();

	m_CheckerboardTexture = SideA::Texture2D::Create("assets/textures/Checkerboard.png");
	m_SpriteSheet = SideA::Texture2D::Create("assets/textures/TX_Tileset_Grass.png");
	m_Grass = SideA::SubTexture2D::CreateFromCoords(m_SpriteSheet, { 1, 0 }, { 32, 32 }, {1, 2});

}

void Sandbox2D::OnDetach()
{
	SIDEA_PROFILE_FUNCTION();
}

void Sandbox2D::OnUpdate(SideA::Timestep deltaTime)
{
	SIDEA_PROFILE_FUNCTION();
	SideA::Renderer2D::StatsStartFrame();

	// Update
	m_CameraController.OnUpdate(deltaTime);

	// Render
	SideA::Renderer2D::ResetStats();
	{
		SIDEA_PROFILE_SCOPE("Renderer Prep");
		
		SideA::RenderCommand::SetClearColor({ 0.2f, 0.2f, 0.25f, 1 });
		SideA::RenderCommand::Clear();
	}

	{
		static float rotation = 0.0f;
		rotation += deltaTime * 50.0f;

		SIDEA_PROFILE_SCOPE("Render Draws");
		SideA::Renderer2D::BeginScene(m_CameraController.GetCamera());
		SideA::Renderer2D::DrawQuad({ 0.2f, -0.2f }, { 0.1f, 0.2f }, m_SquareColor);
		SideA::Renderer2D::DrawQuad({ -0.2f, 0.2f }, { 0.3f, 0.3f }, { 0.8f, 0.2f, 0.4f, 1.0f });
		SideA::Renderer2D::DrawRotatedQuad({ -0.2f, -0.2f }, rotation, { 0.3f, 0.1f }, m_SquareColor);
		SideA::Renderer2D::DrawQuad({ 0.0f, 0.0f, -0.1f }, { 10.0f, 10.0f }, m_CheckerboardTexture, 10.0f);
		SideA::Renderer2D::EndScene();

		SideA::Renderer2D::BeginScene(m_CameraController.GetCamera());
		for (float y = -5.0f; y < 5.0f; y += 0.5f)
		{
			for (float x = -5.0f; x < 5.0f; x += 0.5f)
			{
				glm::vec4 color = { (x + 5.0f) / 10.0f, 0.4f, (y + 5.0f) / 10.0f, 0.7f };
				SideA::Renderer2D::DrawQuad({ x, y }, { 0.45f, 0.45f }, color);
			}
		}

		SideA::Renderer2D::DrawQuad({ 1.0f, 0.0f, 0.5f }, { 1.0f, 2.0f }, m_Grass);

		SideA::Renderer2D::EndScene();
	}

	SideA::Renderer2D::StatsEndFrame();
}

void Sandbox2D::OnEvent(SideA::Event& e)
{
	m_CameraController.OnEvent(e);
}

void Sandbox2D::OnImGuiRender()
{
	SIDEA_PROFILE_FUNCTION();

	ImGui::Begin("Settings");
	auto stats = SideA::Renderer2D::GetStats();
	ImGui::Text("Renderer2D Stats:");
	ImGui::Text("Draw Calls: %d", stats.DrawCalls);
	ImGui::Text("Quads: %d", stats.QuadCount);
	ImGui::Text("Vertices: %d", stats.GetTotalVertexCount());
	ImGui::Text("Indices: %d", stats.GetTotalIndexCount());
	ImGui::Text("Frame Time: %f", stats.FrameTime);
	ImGui::Text("FPS: %f", stats.FramesPerSecond);
	ImGui::ColorEdit4("Square Color", glm::value_ptr(m_SquareColor));

	uint32_t textureID = m_CheckerboardTexture->GetRendererID();
	ImGui::Image((void*)textureID, ImVec2{ 256.0f, 256.0f });
	ImGui::End();
}