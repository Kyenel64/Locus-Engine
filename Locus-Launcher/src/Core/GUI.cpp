#include "LLpch.h"
#include "GUI.h"

#include <imgui.h>
#include <imgui_internal.h>
#include <backends/imgui_impl_opengl3.h>
#include <backends/imgui_impl_glfw.h>
#include <GLFW/glfw3.h>

#include "Utils/Colors.h"
#include "Launcher.h"

namespace LocusLauncher
{
	GUI::GUI()
	{
		Init();
	}

	GUI::~GUI()
	{
		ImGui_ImplOpenGL3_Shutdown();
		ImGui_ImplGlfw_Shutdown();
		ImGui::DestroyContext();
	}

	bool GUI::Init()
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
		ImGui_ImplGlfw_InitForOpenGL(Launcher::Get().GetWindow().GetNativeWindow(), true);
		ImGui_ImplOpenGL3_Init("#version 410");
		return true;
	}

	void GUI::Begin()
	{
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

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
	}

	void GUI::End()
	{
		ImGui::End();

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
	}

	void GUI::SetDarkTheme()
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
		colors[ImGuiCol_PopupBg] = Colors::MediumDarkGrey;

		// Child
		colors[ImGuiCol_ChildBg] = Colors::MediumDarkGrey;

		// Separator
		colors[ImGuiCol_Separator] = Colors::Orange;
		colors[ImGuiCol_SeparatorHovered] = Colors::Orange;

		// Docking
		colors[ImGuiCol_DockingPreview] = Colors::Orange;
	}
}