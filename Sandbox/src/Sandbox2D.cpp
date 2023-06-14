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
	m_SquareVA = Tiel::VertexArray::Create();

	float squareVertices[3 * 4] = {
		-0.5f, -0.5f, 0.0f,
		 0.5f, -0.5f, 0.0f,
		 0.5f,  0.5f, 0.0f,
		-0.5f,  0.5f, 0.0f
	};

	Tiel::Ref<Tiel::VertexBuffer> squareVB;
	squareVB = Tiel::VertexBuffer::Create(squareVertices, sizeof(squareVertices));
	Tiel::BufferLayout squareLayout =
	{
		{ Tiel::ShaderDataType::Float3, "a_Position" }
	};
	squareVB->SetLayout(squareLayout);
	m_SquareVA->AddVertexBuffer(squareVB);

	uint32_t squareIndices[6] = { 0, 1, 2, 2, 3, 0 };
	Tiel::Ref<Tiel::IndexBuffer> squareIB;
	squareIB = Tiel::IndexBuffer::Create(squareIndices, sizeof(squareIndices) / sizeof(uint32_t));
	m_SquareVA->SetIndexBuffer(squareIB);

	m_FlatColorShader = Tiel::Shader::Create("assets/shaders/FlatColor.glsl");
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

	Tiel::Renderer::BeginScene(m_CameraController.GetCamera());

	std::dynamic_pointer_cast<Tiel::OpenGLShader>(m_FlatColorShader)->Bind();
	std::dynamic_pointer_cast<Tiel::OpenGLShader>(m_FlatColorShader)->UploadUniformFloat4("u_Color", m_SquareColor);


	Tiel::Renderer::Submit(m_FlatColorShader, m_SquareVA, glm::scale(glm::mat4(1.0f), glm::vec3(1.5f)));

	Tiel::Renderer::EndScene();
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