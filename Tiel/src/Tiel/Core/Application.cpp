#include "tpch.h"
#include "Application.h"

#include "Tiel/Renderer/Renderer.h"


namespace Tiel
{

// Binds to an event function. Ex: OnWindowClose()
//#define BIND_EVENT_FN(x) std::bind(&Application::x, this, std::placeholders::_1)

	Application* Application::s_Instance = nullptr;

	Application::Application()
	{
		TIEL_PROFILE_FUNCTION();

		TIEL_CORE_ASSERT(!s_Instance, "Application already exists");
		s_Instance = this;

		m_Window = Window::Create();
		m_Window->SetEventCallback(TIEL_BIND_EVENT_FN(Application::OnEvent));

		Renderer::Init();

		m_ImGuiLayer = new ImGuiLayer();
		PushOverlay(m_ImGuiLayer);
	}

	void Application::PushLayer(Layer* layer)
	{
		TIEL_PROFILE_FUNCTION();

		m_LayerStack.PushLayer(layer);
	}

	void Application::PushOverlay(Layer* overlay)
	{
		TIEL_PROFILE_FUNCTION();

		m_LayerStack.PushOverlay(overlay);
	}

	// Passes event function to dispatcher.
	void Application::OnEvent(Event& e)
	{
		TIEL_PROFILE_FUNCTION();

		EventDispatcher dispatcher(e);
		// Dispatch event if event class type matches event type
		dispatcher.Dispatch<WindowCloseEvent>(TIEL_BIND_EVENT_FN(Application::OnWindowClose));
		dispatcher.Dispatch<WindowResizeEvent>(TIEL_BIND_EVENT_FN(Application::OnWindowResize));

		// Iterate each layer's events backwards
		for (auto it = m_LayerStack.end(); it != m_LayerStack.begin();)
		{
			if (e.m_Handled)
				break;
			(*--it)->OnEvent(e);

		}
	}

	void Application::Run()
	{
		TIEL_PROFILE_FUNCTION();

		while (m_Running)
		{
			TIEL_PROFILE_SCOPE("Run loop");
			// Calculate deltaTime
			float time = m_Window->GetTime();
			Timestep timestep = time - m_LastFrameTime;
			m_LastFrameTime = time;

			if (!m_Minimized)
			{
				{
					TIEL_PROFILE_SCOPE("LayerStack OnUpdate");
					// Iterate through layers on update BEFORE window update.
					for (Layer* layer : m_LayerStack)
					layer->OnUpdate(timestep);
				}
				
			}

			// Render ImGui
			m_ImGuiLayer->Begin();
			{
				TIEL_PROFILE_SCOPE("LayerStack OnImGuiRender");
				for (Layer* layer : m_LayerStack)
					layer->OnImGuiRender();
			}
			m_ImGuiLayer->End();


			m_Window->OnUpdate();
		}
	}

	void Application::Close()
	{
		m_Running = false;
	}

	bool Application::OnWindowClose(WindowCloseEvent& e)
	{
		m_Running = false;
		return true;
	}

	bool Application::OnWindowResize(WindowResizeEvent& e)
	{
		TIEL_PROFILE_FUNCTION();

		if (e.GetWidth() == 0 || e.GetHeight() == 0)
		{
			m_Minimized = true;
			return false;
		}
		m_Minimized = false;
		Renderer::OnWindowResize(e.GetWidth(), e.GetHeight());

		return false;
	}
}