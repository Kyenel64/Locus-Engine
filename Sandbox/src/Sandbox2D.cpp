#include "Sandbox2D.h"

#include "imgui/imgui.h"

#include <glm/gtc/type_ptr.hpp>

#include <chrono>

template<typename Fn>
class Timer
{
public:
	Timer(const char* name, Fn&& func)
		: m_Name(name), m_Func(func), m_Stopped(false)
	{
		m_StartTimepoint = std::chrono::high_resolution_clock::now();
	}

	~Timer()
	{
		if (!m_Stopped)
			Stop();
	}

	void Stop()
	{
		auto endTimepoint = std::chrono::high_resolution_clock::now();

		long long start = std::chrono::time_point_cast<std::chrono::microseconds>(m_StartTimepoint).time_since_epoch().count();
		long long end = std::chrono::time_point_cast<std::chrono::microseconds>(endTimepoint).time_since_epoch().count();

		m_Stopped = true;

		float duration = (end - start) * 0.001f;
		m_Func({ m_Name, duration });
	}
private:
	const char* m_Name;
	Fn m_Func;
	std::chrono::time_point<std::chrono::steady_clock> m_StartTimepoint;
	bool m_Stopped;
};

#define PROFILE_SCOPE(name) Timer timer##__LINE__(name, [&](ProfileResult profileResult) { m_ProfileResults.push_back(profileResult); })


Sandbox2D::Sandbox2D()
	: Layer("Sandbox2D"), m_CameraController(1920.0f / 1080.0f)
{

}

void Sandbox2D::OnAttach()
{
	m_CheckerboardTexture = Tiel::Texture2D::Create("assets/textures/Checkerboard.png");
}

void Sandbox2D::OnDetach()
{

}

void Sandbox2D::OnUpdate(Tiel::Timestep deltaTime)
{

	PROFILE_SCOPE("Sandbox2D::OnUpdate");

	// Update
	{
		PROFILE_SCOPE("CameraController::OnUpdate");
		m_CameraController.OnUpdate(deltaTime);
	}

	// Render
	{
		PROFILE_SCOPE("Renderer Prep");
		Tiel::RenderCommand::SetClearColor({ 0.2f, 0.2f, 0.25f, 1 });
		Tiel::RenderCommand::Clear();
	}

	{
		PROFILE_SCOPE("Renderer Draws");
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
	ImGui::Begin("Settings");
	ImGui::ColorEdit4("Square Color", glm::value_ptr(m_SquareColor));

	for (auto& result : m_ProfileResults)
	{
		char label[50];
		strcpy_s(label, "%.3fms ");
		strcat_s(label, result.Name);
		ImGui::Text(label, result.Time);
	}
	m_ProfileResults.clear();

	ImGui::End();
}