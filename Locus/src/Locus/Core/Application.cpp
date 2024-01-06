#include "Lpch.h"
#include "Application.h"

#include "Locus/Renderer/Renderer.h"
#include "Locus/Scripting/ScriptEngine.h"
#include "Locus/Resource/ResourceManager.h"

namespace Locus
{
	Application* Application::s_Instance = nullptr;

	Application::Application(const std::string& name, const std::string& projectPath, const std::string& projectName)
		: m_ProjectPath(projectPath), m_ProjectName(projectName)
	{
		LOCUS_PROFILE_FUNCTION();

		LOCUS_CORE_ASSERT(!s_Instance, "Application already exists");
		s_Instance = this;

		Log::Init();

		m_Window = Window::Create(name);
		m_Window->SetEventCallback(LOCUS_BIND_EVENT_FN(Application::OnEvent));
		m_Window->SetVSync(false);

		m_ImGuiLayer = new ImGuiLayer();
		PushOverlay(m_ImGuiLayer);

		// Initialize subsystems if project is set. 
		Renderer::Init();
		ScriptEngine::Init();
		ResourceManager::Init();
	}

	void Application::PushLayer(Layer* layer)
	{
		LOCUS_PROFILE_FUNCTION();

		m_LayerStack.PushLayer(layer);
	}

	void Application::PushOverlay(Layer* overlay)
	{
		LOCUS_PROFILE_FUNCTION();

		m_LayerStack.PushOverlay(overlay);
	}

	void Application::OnEvent(Event& e)
	{
		LOCUS_PROFILE_FUNCTION();

		for (auto it = m_LayerStack.rbegin(); it != m_LayerStack.rend(); ++it)
		{
			if (e.m_Handled)
				break;
			(*it)->OnEvent(e);
		}

		// Bind callback functions
		EventDispatcher dispatcher(e);
		dispatcher.Dispatch<WindowResizeEvent>(LOCUS_BIND_EVENT_FN(Application::OnWindowResize));
		dispatcher.Dispatch<WindowCloseEvent>(LOCUS_BIND_EVENT_FN(Application::OnWindowClose));
	}

	void Application::Run()
	{
		LOCUS_PROFILE_FUNCTION();

		while (m_Running)
		{
			// Calculate deltaTime
			float time = m_Window->GetTime();
			Timestep timestep = time - m_LastFrameTime;
			m_LastFrameTime = time;

			// Call OnUpdate() for each layer
			if (!m_Minimized)
			{
				LOCUS_PROFILE_SCOPE("LayerStack OnUpdate()");
				for (Layer* layer : m_LayerStack)
					layer->OnUpdate(timestep);
			}

			// Call OnImGuiRender() for each layer
			m_ImGuiLayer->Begin();
			{
				LOCUS_PROFILE_SCOPE("LayerStack OnImGuiRender()");
				for (Layer* layer : m_LayerStack)
					layer->OnImGuiRender();
			}
			m_ImGuiLayer->End();

			// Call window OnUpdate() after all layers
			m_Window->OnUpdate();

			// Calculate FPS
			static int frames = 0;
			static float timer = 1.0f;
			timer -= timestep;
			frames++;
			if (timer <= 0.0f)
			{
				m_FPS = frames;
				timer = 1.0f;
				frames = 0;
			}
		}
	}

	void Application::Close()
	{
		LOCUS_PROFILE_FUNCTION();

		Renderer::Shutdown();
		ScriptEngine::Shutdown();
		m_Running = false;
	}

	bool Application::OnWindowClose(WindowCloseEvent& e)
	{
		LOCUS_PROFILE_FUNCTION();

		// Check if the WindowCloseEvent has already been handled elsewhere.
		if (!e.m_Handled)
			Close();

		return true;
	}

	bool Application::OnWindowResize(WindowResizeEvent& e)
	{
		LOCUS_PROFILE_FUNCTION();

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
