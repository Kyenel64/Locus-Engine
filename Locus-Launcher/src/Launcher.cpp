#include "Launcher.h"

#include <imgui.h>
#include <imgui_internal.h>
#include <backends/imgui_impl_opengl3.h>
#include <backends/imgui_impl_glfw.h>
#include <GLFW/glfw3.h>

#include "Colors.h"

namespace LocusLauncher
{
	Launcher* Launcher::s_Instance = nullptr;

	Launcher::Launcher(const std::string& name, uint32_t windowWidth, uint32_t windowHeight)
		: m_WindowName(name), m_WindowWidth(windowWidth), m_WindowHeight(windowHeight)
	{
		if (!s_Instance)
			s_Instance = this;
		InitGLFW();
		InitImGui();
	}

	Launcher::~Launcher()
	{
		ImGui_ImplOpenGL3_Shutdown();
		ImGui_ImplGlfw_Shutdown();
		ImGui::DestroyContext();

		glfwTerminate();
		m_Window = nullptr;
	}

	void Launcher::InitImGui()
	{
		// Setup Dear ImGui context
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGuiIO& io = ImGui::GetIO(); (void)io;
		ImGuiStyle& style = ImGui::GetStyle();
		//io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;       // Enable Keyboard Controls
		io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;           // Enable Docking
		io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;         // Enable Multi-Viewport / Platform Windows
		io.ConfigDockingAlwaysTabBar = true;

		// Load fonts
		io.Fonts->AddFontFromFileTTF("resources/fonts/Inter/Inter-Bold.ttf", 20.0f);
		io.FontDefault = io.Fonts->AddFontFromFileTTF("resources/fonts/Inter/Inter-Regular.ttf", 18.0f);

		// Styling
		style.WindowBorderSize = 1.0f;
		style.FrameBorderSize = 1.0f;
		style.ChildBorderSize = 1.0f;
		style.TabRounding = 3.0f;
		style.WindowRounding = 5.0f;
		style.FrameRounding = 5.0f;
		style.ChildRounding = 5.0f;
		style.PopupRounding = 5.0f;
		style.TabRounding = 0.0f;
		style.IndentSpacing = 10.0f;
		style.ScrollbarSize = 5.0f;
		style.DockingSplitterSize = 12.0f;
		style.WindowMenuButtonPosition = ImGuiDir_None;
		style.ItemSpacing = { 6.0f, 6.0f };
		style.ItemInnerSpacing = { 0.0f, 6.0f };
		style.WindowPadding = { 6.0f, 6.0f };
		style.FramePadding = { 10.0f, 4.0f };

		SetDarkTheme();

		// Setup Platform/Renderer bindings
		ImGui_ImplGlfw_InitForOpenGL(m_Window, true);
		ImGui_ImplOpenGL3_Init("#version 410");
	}

	void Launcher::SetDarkTheme()
	{
		auto& colors = ImGui::GetStyle().Colors;

		// Window
		colors[ImGuiCol_WindowBg] = Colors::MediumDarkGrey;
		colors[ImGuiCol_ResizeGrip] = Colors::LightGrey;
		colors[ImGuiCol_ResizeGripHovered] = Colors::Orange;
		colors[ImGuiCol_ResizeGripActive] = Colors::Orange;

		// Headers
		colors[ImGuiCol_Header] = Colors::DarkGrey;
		colors[ImGuiCol_HeaderHovered] = Colors::Orange;
		colors[ImGuiCol_HeaderActive] = Colors::Orange;

		// Buttons
		colors[ImGuiCol_Button] = Colors::LightGrey;
		colors[ImGuiCol_ButtonHovered] = Colors::Grey;
		colors[ImGuiCol_ButtonActive] = Colors::LightGrey;
		colors[ImGuiCol_CheckMark] = Colors::White;

		// Frame BG
		colors[ImGuiCol_FrameBg] = Colors::DarkGrey;
		colors[ImGuiCol_FrameBgHovered] = { 0.02f, 0.02f, 0.02f, 1.0f };
		colors[ImGuiCol_FrameBgActive] = Colors::LightGrey;

		// Tabs
		colors[ImGuiCol_Tab] = { 0.09f, 0.09f, 0.09f, 1.0f };
		colors[ImGuiCol_TabHovered] = Colors::MediumDarkGrey;
		colors[ImGuiCol_TabActive] = Colors::MediumDarkGrey;
		colors[ImGuiCol_TabUnfocused] = { 0.09f, 0.09f, 0.09f, 1.0f };
		colors[ImGuiCol_TabUnfocusedActive] = Colors::MediumDarkGrey;

		// Title
		colors[ImGuiCol_TitleBgActive] = Colors::DarkGrey;
		colors[ImGuiCol_TitleBg] = Colors::DarkGrey;
		colors[ImGuiCol_TitleBgCollapsed] = Colors::MediumDarkGrey;

		// Border
		colors[ImGuiCol_Border] = Colors::LightGrey;

		// Popups
		colors[ImGuiCol_PopupBg] = Colors::Grey;

		// Child
		colors[ImGuiCol_ChildBg] = Colors::MediumDarkGrey;

		// Separator
		colors[ImGuiCol_Separator] = Colors::Orange;
		colors[ImGuiCol_SeparatorHovered] = Colors::Orange;

		// Docking
		colors[ImGuiCol_DockingPreview] = Colors::Orange;
	}

	void Launcher::InitGLFW()
	{
		if (!glfwInit())
			return;

		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

		m_Window = glfwCreateWindow(m_WindowWidth, m_WindowHeight, m_WindowName.c_str(), nullptr, nullptr);
		if (!m_Window)
		{
			glfwTerminate();
			return;
		}
		glfwMakeContextCurrent(m_Window);

		CenterGLFWWindow();
	}

	void Launcher::CenterGLFWWindow()
	{
		GLFWmonitor* monitor = glfwGetPrimaryMonitor();
		const GLFWvidmode* mode = glfwGetVideoMode(monitor);
		glfwSetWindowPos(m_Window, mode->width / 2 - (m_WindowWidth / 2), mode->height / 2 - (m_WindowHeight / 2));
	}

	void Launcher::Run()
	{
		while (m_IsRunning)
		{
			m_IsRunning = !glfwWindowShouldClose(m_Window);
			glfwPollEvents();

			ImGui_ImplOpenGL3_NewFrame();
			ImGui_ImplGlfw_NewFrame();
			ImGui::NewFrame();


			OnImGuiRender();


			ImGuiIO& io = ImGui::GetIO();
			ImGui::Render();
			ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
			if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
			{
				GLFWwindow* backup_current_context = glfwGetCurrentContext();
				ImGui::UpdatePlatformWindows();
				ImGui::RenderPlatformWindowsDefault();
				glfwMakeContextCurrent(backup_current_context);
			}

			glfwSwapBuffers(m_Window);
		}
	}

	void Launcher::OnImGuiRender()
	{
		// --- Dockspace ---
		static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None;
		ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoScrollbar |
			ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove |
			ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus | ImGuiWindowFlags_NoScrollWithMouse;
		ImGuiViewport* viewport = ImGui::GetMainViewport();
		ImGui::SetNextWindowPos(viewport->Pos);
		ImGui::SetNextWindowSize(viewport->Size);
		ImGui::SetNextWindowViewport(viewport->ID);

		if (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode)
			window_flags |= ImGuiWindowFlags_NoBackground;

		ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, { 13.0f, 13.0f });
		ImGui::PushStyleColor(ImGuiCol_WindowBg, Colors::DarkGrey);
		ImGui::Begin("LauncherLayout", false, window_flags);
		ImGui::PopStyleVar(2);

		ImGuiDockNodeFlags docknodeFlags = ImGuiDockNodeFlags_None;
		docknodeFlags |= ImGuiDockNodeFlags_WindowRounding;
		ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
		ImGui::PushStyleColor(ImGuiCol_Separator, Colors::Transparent);
		ImGui::PushStyleColor(ImGuiCol_SeparatorHovered, Colors::Transparent);
		ImGui::PushStyleColor(ImGuiCol_SeparatorActive, Colors::Transparent);
		ImGui::DockSpace(dockspace_id, { 0.0f, 0.0f }, docknodeFlags);
		ImGui::PopStyleColor(4);

		ImGui::ShowDemoWindow();
		m_LocalProjectsPanel.OnImGuiRender();

		ImGui::End();

	}
}
