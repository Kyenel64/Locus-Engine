// --- Application ------------------------------------------------------------
// Main program application class.
// Contains the core run loop that iterates through each layer.
// Handles events for all layers.
// Optionally takes in command line args. The first argument takes in a path
//  to a .locus scene file which will open on startup.
#pragma once

#include <filesystem>

#include "Locus/Core/Core.h"
#include "Locus/Core/Window.h"
#include "Locus/Core/LayerStack.h"
#include "Locus/Events/Event.h"
#include "Locus/Events/ApplicationEvent.h"
#include "Locus/ImGui/ImGuiLayer.h"
#include "Locus/Renderer/Shader.h"
#include "Locus/Renderer/Buffer.h"
#include "Locus/Renderer/VertexArray.h"

namespace Locus
{
	class Application
	{
	public:
		Application(const std::string& name, const std::string& projectPath = std::string(), const std::string& projectName = std::string());
		virtual ~Application() = default;

		void PushLayer(Layer* layer);
		void PushOverlay(Layer* overlay);

		void OnEvent(Event& e);

		void Run();
		void Close();

		inline Window& GetWindow() const { return *m_Window; }
		inline static Application& Get() { return *s_Instance; }
		inline ImGuiLayer* GetImGuiLayer() const { return m_ImGuiLayer; }

		const std::filesystem::path& GetProjectPath() const { return m_ProjectPath; }
		const std::string& GetProjectName() const { return m_ProjectName; }

		inline bool IsRunning() const { return m_Running; }
	private:
		bool OnWindowClose(WindowCloseEvent& e);
		bool OnWindowResize(WindowResizeEvent& e);

	private:
		static Application* s_Instance;

		std::filesystem::path m_ProjectPath;
		std::string m_ProjectName;

		Scope<Window> m_Window;
		ImGuiLayer* m_ImGuiLayer;
		bool m_Running = true;
		bool m_Minimized = false;
		LayerStack m_LayerStack;

		float m_LastFrameTime = 0.0f;
	};
}
