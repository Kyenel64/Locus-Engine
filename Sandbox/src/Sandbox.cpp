#include <Tiel.h>

#include "Platform/OpenGL/OpenGLShader.h"

#include "ImGui/imgui.h"
#include "glm/glm/gtc/matrix_transform.hpp"
#include "glm/glm/gtc/type_ptr.hpp"

class ExampleLayer : public Tiel::Layer
{
public:
	ExampleLayer() : Layer("Example"), m_Camera(-1.6f, 1.6f, -0.9f, 0.9f), m_CameraPosition({0.0f, 0.0f, 0.5f})
	{
		// --- Rendering Triangle ---------------------------------------------

		m_VertexArray.reset(Tiel::VertexArray::Create());
		// Vertex Buffer
		float vertices[3 * 7] =
		{
			// Position          // Color
			-0.5f, -0.5f,  0.0f, 0.8f, 0.2f, 0.8f, 1.0f,
			 0.5f, -0.5f,  0.0f, 0.2f, 0.8f, 0.8f, 1.0f,
			 0.0f,  0.5f,  0.0f, 0.8f, 0.8f, 0.2f, 1.0f
		};
		Tiel::Ref<Tiel::VertexBuffer> vertexBuffer;
		vertexBuffer.reset(Tiel::VertexBuffer::Create(vertices, sizeof(vertices)));
		// Add element here and GLSL
		Tiel::BufferLayout layout =
		{
			{ Tiel::ShaderDataType::Float3, "a_Position" },
			{ Tiel::ShaderDataType::Float4, "a_Color" }
		};
		vertexBuffer->SetLayout(layout);
		m_VertexArray->AddVertexBuffer(vertexBuffer);

		// Index Buffer
		uint32_t indices[3] = { 0, 1, 2 };
		Tiel::Ref<Tiel::IndexBuffer> indexBuffer;
		indexBuffer.reset(Tiel::IndexBuffer::Create(indices, sizeof(indices) / sizeof(uint32_t)));
		m_VertexArray->SetIndexBuffer(indexBuffer);


		// --- Rendering Square -----------------------------------------------

		m_SquareVA.reset(Tiel::VertexArray::Create());
		// Vertex Buffer
		float squareVertices[5 * 4] = {
			-0.5f, -0.5f, 0.0f, 0.0f, 0.0f,
			 0.5f, -0.5f, 0.0f, 1.0f, 0.0f,
			 0.5f,  0.5f, 0.0f, 1.0f, 1.0f,
			-0.5f,  0.5f, 0.0f, 0.0f, 1.0f
		};
		Tiel::Ref<Tiel::VertexBuffer> squareVB;
		squareVB.reset(Tiel::VertexBuffer::Create(squareVertices, sizeof(squareVertices)));
		// Add element here and GLSL
		Tiel::BufferLayout squareLayout =
		{
			{ Tiel::ShaderDataType::Float3, "a_Position" },
			{ Tiel::ShaderDataType::Float2, "a_TexCoord" }
		};
		squareVB->SetLayout(squareLayout);
		m_SquareVA->AddVertexBuffer(squareVB);
		// Index Buffer
		uint32_t squareIndices[6] = { 0, 1, 2, 2, 3, 0 };
		Tiel::Ref<Tiel::IndexBuffer> squareIB;
		squareIB.reset(Tiel::IndexBuffer::Create(squareIndices, sizeof(squareIndices) / sizeof(uint32_t)));
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

		m_Shader.reset(Tiel::Shader::Create(vertexSrc, fragmentSrc));

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

		m_FlatColorShader.reset(Tiel::Shader::Create(flatColorVertexSrc, flatColorFragmentxSrc));

		m_TextureShader.reset(Tiel::Shader::Create("assets/Shaders/Texture.glsl"));



		m_Texture = Tiel::Texture2D::Create("assets/Textures/Checkerboard.png");
		m_CockatielTexture = Tiel::Texture2D::Create("assets/Textures/Cockatiel.png");

		std::dynamic_pointer_cast<Tiel::OpenGLShader>(m_TextureShader)->Bind();
		std::dynamic_pointer_cast<Tiel::OpenGLShader>(m_TextureShader)->UploadUniformInt("u_Texture", 0);
	}

	void OnUpdate(Tiel::Timestep deltaTime) override
	{
		if (Tiel::Input::IsKeyPressed(TIEL_KEY_A) || Tiel::Input::IsKeyPressed(TIEL_KEY_LEFT))
			m_CameraPosition.x -= m_CameraMoveSpeed * deltaTime;
		else if (Tiel::Input::IsKeyPressed(TIEL_KEY_D) || Tiel::Input::IsKeyPressed(TIEL_KEY_RIGHT))
			m_CameraPosition.x += m_CameraMoveSpeed * deltaTime;

		if (Tiel::Input::IsKeyPressed(TIEL_KEY_W) || Tiel::Input::IsKeyPressed(TIEL_KEY_UP))
			m_CameraPosition.y += m_CameraMoveSpeed * deltaTime;
		else if (Tiel::Input::IsKeyPressed(TIEL_KEY_S) || Tiel::Input::IsKeyPressed(TIEL_KEY_DOWN))
			m_CameraPosition.y -= m_CameraMoveSpeed * deltaTime;
		
		if (Tiel::Input::IsKeyPressed(TIEL_KEY_Q))
			m_CameraRotation += m_CameraRotationSpeed * deltaTime;
		else if (Tiel::Input::IsKeyPressed(TIEL_KEY_E))
			m_CameraRotation -= m_CameraRotationSpeed * deltaTime;

		Tiel::RenderCommand::SetClearColor({ 0.2f, 0.2f, 0.25f, 1 });
		Tiel::RenderCommand::Clear();

		m_Camera.SetPosition(m_CameraPosition);
		m_Camera.SetRotation(m_CameraRotation);

		Tiel::Renderer::BeginScene(m_Camera);

		std::dynamic_pointer_cast<Tiel::OpenGLShader>(m_FlatColorShader)->Bind();
		std::dynamic_pointer_cast<Tiel::OpenGLShader>(m_FlatColorShader)->UploadUniformFloat3("u_Color", m_SquareColor);

		for (int i = 0; i < 25; i++)
		{
			for (int j = 0; j < 25; j++)
			{
				glm::mat4 transform = glm::translate(glm::mat4(1.0f),
					glm::vec3((i * 1.1), (j * 1.1), 0.0f));
				Tiel::Renderer::Submit(m_FlatColorShader, m_SquareVA, transform);
			}
		}

		m_Texture->Bind();
		Tiel::Renderer::Submit(m_TextureShader, m_SquareVA, glm::scale(glm::mat4(1.0f), glm::vec3(1.5f)));
		m_CockatielTexture->Bind();
		Tiel::Renderer::Submit(m_TextureShader, m_SquareVA, glm::scale(glm::mat4(1.0f), glm::vec3(1.5f)));

		// Triangle
		//Tiel::Renderer::Submit(m_Shader, m_VertexArray);

		Tiel::Renderer::EndScene();
	}

	virtual void OnImGuiRender() override
	{
		ImGui::Begin("Settings");
		ImGui::ColorEdit3("Square Color", glm::value_ptr(m_SquareColor));
		ImGui::End();
	}

	void OnEvent(Tiel::Event& event) override
	{

	}

private:

	Tiel::Ref<Tiel::Shader> m_Shader;
	Tiel::Ref<Tiel::VertexArray> m_VertexArray;

	Tiel::Ref<Tiel::Shader> m_FlatColorShader, m_TextureShader;
	Tiel::Ref<Tiel::VertexArray> m_SquareVA;

	Tiel::Ref<Tiel::Texture2D> m_Texture, m_CockatielTexture;

	Tiel::OrthographicCamera m_Camera;
	glm::vec3 m_CameraPosition;
	float m_CameraMoveSpeed = 7.0f;

	float m_CameraRotation = 0.0f;
	float m_CameraRotationSpeed = 50.0f;

	glm::vec3 m_SquareColor = { 0.2f, 0.3f, 0.8f };
};


class Sandbox : public Tiel::Application
{
public:
	Sandbox()
	{
		PushLayer(new ExampleLayer());
	}

	~Sandbox() {}
};

Tiel::Application* Tiel::CreateApplication()
{
	return new Sandbox();
}