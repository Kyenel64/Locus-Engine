// --- Application ------------------------------------------------------------
// Main program application. 
// Handles command line arguments.
// Keeps track of saved status.

#pragma once

#include "Core.h"

#include "Locus/Core/Window.h"
#include "Locus/Core/LayerStack.h"
#include "Locus/Events/Event.h"
#include "Locus/Events/ApplicationEvent.h"
#include "Locus/ImGui/ImGuiLayer.h"
#include "Locus/Renderer/Shader.h"
#include "Locus/Renderer/Buffer.h"
#include "Locus/Renderer/VertexArray.h"
#include "Locus/Renderer/OrthographicCamera.h"

namespace Locus
{
	struct ApplicationCommandLineArgs
	{
		int Count = 0;
		char** Args = nullptr;

		const char* operator[](int index) const
		{
			LOCUS_CORE_ASSERT(index < Count, "CommandLineArgs failed");
			return Args[index];
		}
	};

	class Application
	{
	public:
		// Creates window, sets event callbacks, and creates ImGui layer.
		Application(const std::string& name = "Locus App", ApplicationCommandLineArgs args = ApplicationCommandLineArgs());
		virtual ~Application() {}

		void PushLayer(Layer* layer);
		void PushOverlay(Layer* overlay);

		void OnEvent(Event& e);
		// --- Main Engine loop ---
		void Run();
		void Close();

		inline Window& GetWindow() const { return *m_Window; }
		inline static Application& Get() { return *s_Instance; }
		ImGuiLayer* GetImGuiLayer() const { return m_ImGuiLayer; }

		void SetSaveChangesPopupStatus(bool status) { m_OpenSaveChangesPopup = status; }
		bool GetSaveChangesPopupStatus() const { return m_OpenSaveChangesPopup; }

		void SetIsSavedStatus(bool isSaved) { m_IsSaved = isSaved; }
		bool GetIsSavedStatus() const { return m_IsSaved; }

		ApplicationCommandLineArgs GetCommandLineArgs() const { return m_CommandLineArgs; }
	private:
		bool OnWindowClose(WindowCloseEvent& e);
		bool OnWindowResize(WindowResizeEvent& e);

	private:
		static Application* s_Instance;

		ApplicationCommandLineArgs m_CommandLineArgs;

		Scope<Window> m_Window;
		ImGuiLayer* m_ImGuiLayer;
		bool m_Running = true;
		bool m_Minimized = false;
		LayerStack m_LayerStack;

		bool m_OpenSaveChangesPopup = false;
		bool m_IsSaved = true;

		float m_LastFrameTime = 0.0f;
	};

	// To be defined in client
	Application* CreateApplication(ApplicationCommandLineArgs args);
}