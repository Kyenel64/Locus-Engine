#include "Sandbox2D.h"

#include "imgui/imgui.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// Temp
#include "Platform/OpenGL/OpenGLShader.h"

Sandbox2D::Sandbox2D()
	: Layer("Sandbox2D"), m_CameraController(1920.0f / 1080.0f)
{

}

void Sandbox2D::OnAttach()
{

}

void Sandbox2D::OnDetach()
{

}

void Sandbox2D::OnUpdate(Tiel::Timestep deltaTime)
{
	// Update
	m_CameraController.OnUpdate(deltaTime);

	// Render
	Tiel::RenderCommand::SetClearColor({ 0.2f, 0.2f, 0.25f, 1 });
	Tiel::RenderCommand::Clear();

	Tiel::Renderer2D::BeginScene(m_CameraController.GetCamera());

	Tiel::Renderer2D::DrawQuad({ 0.0f, 0.5f }, { 2.0f, 1.0f }, m_SquareColor);

	Tiel::Renderer2D::EndScene();

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
	ImGui::Begin("Settings");
	ImGui::ColorEdit4("Square Color", glm::value_ptr(m_SquareColor));
	ImGui::End();
}