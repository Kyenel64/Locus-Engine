#include "SApch.h"
#include "ImGuiLayer.h"

#include "imgui.h"
#include "GLFW/glfw3.h"

#include "SideA/Core/Application.h"

#include "backends/imgui_impl_opengl3.h"
#include "backends/imgui_impl_glfw.h"

namespace SideA
{
	ImGuiLayer::ImGuiLayer() : Layer("ImGuiLayer")
	{

	}

	void ImGuiLayer::OnAttach()
	{
		SIDEA_PROFILE_FUNCTION();

		// Setup Dear ImGui context
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGuiIO& io = ImGui::GetIO(); (void)io;
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;       // Enable Keyboard Controls
		//io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
		io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;           // Enable Docking
		io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;         // Enable Multi-Viewport / Platform Windows
		//io.ConfigFlags |= ImGuiConfigFlags_ViewportsNoTaskBarIcons;
		//io.ConfigFlags |= ImGuiConfigFlags_ViewportsNoMerge;

		// Setup Dear ImGui style
		ImGui::StyleColorsDark();
		//ImGui::StyleColorsClassic();

		// When viewports are enabled we tweak WindowRounding/WindowBg so platform windows can look identical to regular ones.
		ImGuiStyle& style = ImGui::GetStyle();
		if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
		{
			style.WindowRounding = 0.0f;
			style.Colors[ImGuiCol_WindowBg].w = 1.0f;
		}

		io.Fonts->AddFontFromFileTTF("assets/fonts/Inter/Inter-Bold.ttf", 20.0f);
		io.FontDefault = io.Fonts->AddFontFromFileTTF("assets/fonts/Inter/Inter-Regular.ttf", 20.0f);

		SetDarkTheme();

		Application& app = Application::Get();
		GLFWwindow* window = static_cast<GLFWwindow*>(app.GetWindow().GetNativeWindow());

		// Setup Platform/Renderer bindings
		ImGui_ImplGlfw_InitForOpenGL(window, true);
		ImGui_ImplOpenGL3_Init("#version 410");
	}

	void ImGuiLayer::OnDetach()
	{
		SIDEA_PROFILE_FUNCTION();

		ImGui_ImplOpenGL3_Shutdown();
		ImGui_ImplGlfw_Shutdown();
		ImGui::DestroyContext();
	}

	void ImGuiLayer::OnEvent(Event& e)
	{
		if (m_BlockEvents)
		{
			ImGuiIO& io = ImGui::GetIO();
			e.m_Handled |= e.IsInCategory(EventCategoryMouse) & io.WantCaptureMouse;
			e.m_Handled |= e.IsInCategory(EventCategoryKeyboard) & io.WantCaptureKeyboard;
		}
	}

	void ImGuiLayer::Begin()
	{
		SIDEA_PROFILE_FUNCTION();

		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();
	}

	void ImGuiLayer::End()
	{
		SIDEA_PROFILE_FUNCTION();

		ImGuiIO& io = ImGui::GetIO();
		Application& app = Application::Get();
		io.DisplaySize = ImVec2((float)app.GetWindow().GetWidth(), (float)app.GetWindow().GetHeight());

		// Rendering
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

	void ImGuiLayer::SetDarkTheme()
	{
		enum colorNames { DarkTeal, LightTeal, DarkGrey, MediumDarkGrey, Grey, LightGrey, Pink };
		ImVec4 colorPallete[] =
		{
			{ 0.0f,  0.34f, 0.3f,  1.0f }, // DarkTeal
			{ 0.0f,  0.54f, 0.48f, 1.0f }, // LightTeal
			{ 0.16f, 0.16f, 0.16f, 1.0f }, // DarkGrey
			{ 0.23f, 0.23f, 0.23f, 1.0f }, // MediumDarkGrey
			{ 0.37f, 0.37f, 0.37f, 1.0f }, // Grey
			{ 0.59f, 0.59f, 0.59f, 1.0f }, // LightGrey
			{ 0.9f,  0.0f,  0.9f,  1.0f }  // Pink
		};

		auto& colors = ImGui::GetStyle().Colors;
		colors[ImGuiCol_WindowBg] = colorPallete[colorNames::MediumDarkGrey];

		// Headers
		colors[ImGuiCol_Header] = colorPallete[colorNames::LightTeal];
		colors[ImGuiCol_HeaderHovered] = colorPallete[colorNames::Grey];
		colors[ImGuiCol_HeaderActive] = colorPallete[colorNames::LightGrey];

		// Buttons
		colors[ImGuiCol_Button] = colorPallete[colorNames::Grey];
		colors[ImGuiCol_ButtonHovered] = colorPallete[colorNames::LightGrey];
		colors[ImGuiCol_ButtonActive] = colorPallete[colorNames::Grey];

		// Frame BG
		colors[ImGuiCol_FrameBg] = colorPallete[colorNames::DarkGrey];
		colors[ImGuiCol_FrameBgHovered] = colorPallete[colorNames::Grey];
		colors[ImGuiCol_FrameBgActive] = colorPallete[colorNames::LightGrey];

		// Tabs
		colors[ImGuiCol_Tab] = colorPallete[colorNames::DarkGrey];
		colors[ImGuiCol_TabHovered] = colorPallete[colorNames::Grey];
		colors[ImGuiCol_TabActive] = colorPallete[colorNames::MediumDarkGrey];
		colors[ImGuiCol_TabUnfocused] = colorPallete[colorNames::MediumDarkGrey];
		colors[ImGuiCol_TabUnfocusedActive] = colorPallete[colorNames::MediumDarkGrey];

		// Title
		colors[ImGuiCol_TitleBg] = colorPallete[colorNames::DarkGrey];
		colors[ImGuiCol_TitleBgActive] = colorPallete[colorNames::DarkGrey];
		colors[ImGuiCol_TitleBgCollapsed] = colorPallete[colorNames::DarkGrey];
	}
}