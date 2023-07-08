#include "ExampleLayer.h"

#include "imgui/imgui.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>


ExampleLayer::ExampleLayer() : Layer("Example"), m_CameraController(1920.0f / 1080.0f)
{
	// --- Rendering Triangle ---------------------------------------------

	m_VertexArray = SideA::VertexArray::Create();
	// Vertex Buffer
	float vertices[3 * 7] =
	{
		// Position          // Color
		-0.5f, -0.5f,  0.0f, 0.8f, 0.2f, 0.8f, 1.0f,
		 0.5f, -0.5f,  0.0f, 0.2f, 0.8f, 0.8f, 1.0f,
		 0.0f,  0.5f,  0.0f, 0.8f, 0.8f, 0.2f, 1.0f
	};
	SideA::Ref<SideA::VertexBuffer> vertexBuffer;
	vertexBuffer = SideA::VertexBuffer::Create(vertices, sizeof(vertices));
	// Add element here and GLSL
	SideA::BufferLayout layout =
	{
		{ SideA::ShaderDataType::Float3, "a_Position" },
		{ SideA::ShaderDataType::Float4, "a_Color" }
	};
	vertexBuffer->SetLayout(layout);
	m_VertexArray->AddVertexBuffer(vertexBuffer);

	// Index Buffer
	uint32_t indices[3] = { 0, 1, 2 };
	SideA::Ref<SideA::IndexBuffer> indexBuffer;
	indexBuffer = SideA::IndexBuffer::Create(indices, sizeof(indices) / sizeof(uint32_t));
	m_VertexArray->SetIndexBuffer(indexBuffer);


	// --- Rendering Square -----------------------------------------------

	m_SquareVA = SideA::VertexArray::Create();
	// Vertex Buffer
	float squareVertices[5 * 4] = {
		-0.5f, -0.5f, 0.0f, 0.0f, 0.0f,
		 0.5f, -0.5f, 0.0f, 1.0f, 0.0f,
		 0.5f,  0.5f, 0.0f, 1.0f, 1.0f,
		-0.5f,  0.5f, 0.0f, 0.0f, 1.0f
	};
	SideA::Ref<SideA::VertexBuffer> squareVB;
	squareVB = SideA::VertexBuffer::Create(squareVertices, sizeof(squareVertices));
	// Add element here and GLSL
	SideA::BufferLayout squareLayout =
	{
		{ SideA::ShaderDataType::Float3, "a_Position" },
		{ SideA::ShaderDataType::Float2, "a_TexCoord" }
	};
	squareVB->SetLayout(squareLayout);
	m_SquareVA->AddVertexBuffer(squareVB);
	// Index Buffer
	uint32_t squareIndices[6] = { 0, 1, 2, 2, 3, 0 };
	SideA::Ref<SideA::IndexBuffer> squareIB;
	squareIB = SideA::IndexBuffer::Create(squareIndices, sizeof(squareIndices) / sizeof(uint32_t));
	m_SquareVA->SetIndexBuffer(squareIB);



	// --- Shaders --------------------------------------------------------------
	std::string vertexSrc = R"(
		#version 460 core
		layout (location = 0) in vec3 a_Position;
		layout (location = 1) in vec4 a_Color;

		uniform mat4 u_ViewProjection;
		uniform mat4 u_Transform;

		out vec4 v_Color;
		
		void main()
		{
			gl_Position = u_ViewProjection * u_Transform * vec4(a_Position, 1.0f);
			v_Color = a_Color;
			
		}
	)";
	std::string fragmentSrc = R"(
		#version 460 core

		in vec4 v_Color;
		out vec4 color;
		
		void main()
		{
			color = vec4(v_Color);
		}
	)";

	m_Shader = SideA::Shader::Create("VertexPosColor", vertexSrc, fragmentSrc);

	// Shaders
	std::string flatColorVertexSrc = R"(
		#version 460 core
		layout (location = 0) in vec3 a_Position;

		uniform mat4 u_ViewProjection;
		uniform mat4 u_Transform;

		void main()
		{
			gl_Position = u_ViewProjection * u_Transform * vec4(a_Position, 1.0f);
			
		}
	)";
	std::string flatColorFragmentxSrc = R"(
		#version 460 core

		uniform vec3 u_Color;

		out vec4 color;
		
		void main()
		{
			color = vec4(u_Color, 1.0f);
		}
	)";

	m_FlatColorShader = SideA::Shader::Create("FlatColor", flatColorVertexSrc, flatColorFragmentxSrc);

	auto textureShader = m_ShaderLibrary.Load("assets/shaders/Texture.glsl");

	m_Texture = SideA::Texture2D::Create("assets/textures/Checkerboard.png");
	m_CockatielTexture = SideA::Texture2D::Create("assets/textures/Cockatiel.png");

	textureShader->Bind();
	textureShader->SetInt("u_Texture", 0);
}

void ExampleLayer::OnUpdate(SideA::Timestep deltaTime)
{
	// Update
	m_CameraController.OnUpdate(deltaTime);

	// Render
	SideA::RenderCommand::SetClearColor({ 0.2f, 0.2f, 0.25f, 1 });
	SideA::RenderCommand::Clear();

	SideA::Renderer::BeginScene(m_CameraController.GetCamera());

	m_FlatColorShader->Bind();
	m_FlatColorShader->SetFloat3("u_Color", m_SquareColor);

	for (int i = 0; i < 25; i++)
	{
		for (int j = 0; j < 25; j++)
		{
			glm::mat4 transform = glm::translate(glm::mat4(1.0f),
				glm::vec3((i * 1.1), (j * 1.1), 0.0f));
			SideA::Renderer::Submit(m_FlatColorShader, m_SquareVA, transform);
		}
	}
	auto textureShader = m_ShaderLibrary.Get("Texture");

	m_Texture->Bind();
	SideA::Renderer::Submit(textureShader, m_SquareVA, glm::scale(glm::mat4(1.0f), glm::vec3(1.5f)));
	m_CockatielTexture->Bind();
	SideA::Renderer::Submit(textureShader, m_SquareVA, glm::scale(glm::mat4(1.0f), glm::vec3(1.5f)));

	SideA::Renderer::EndScene();
}

void ExampleLayer::OnImGuiRender()
{
	ImGui::Begin("Settings");
	ImGui::ColorEdit3("Square Color", glm::value_ptr(m_SquareColor));
	ImGui::Text("%f", m_CameraController.GetZoomLevel());
	ImGui::End();
}

void ExampleLayer::OnEvent(SideA::Event& e)
{
	m_CameraController.OnEvent(e);
}