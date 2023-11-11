#include "Lpch.h"
#include "ImGuiLayer.h"

#include <imgui.h>
#include <backends/imgui_impl_opengl3.h>
#include <backends/imgui_impl_glfw.h>
#include <ImGuizmo.h>
#include <GLFW/glfw3.h>

#include "Locus/Core/Application.h"

namespace Locus
{
	ImVec4 LocusColors::Black =	{ 0.0f, 0.0f, 0.0f, 1.0f };
	ImVec4 LocusColors::DarkGrey = { 0.132f, 0.125f, 0.117f, 1.0f };
	ImVec4 LocusColors::MediumDarkGrey = { 0.207f, 0.195f, 0.176f, 1.0f };
	ImVec4 LocusColors::Grey = { 0.246f, 0.234f, 0.211f, 1.0f };
	ImVec4 LocusColors::LightGrey = { 0.289f, 0.281f, 0.266f, 1.0f };
	ImVec4 LocusColors::White = { 1.0f, 1.0f, 1.0f, 1.0f };
	ImVec4 LocusColors::Pink = { 0.9f, 0.4f, 0.4f, 1.0f };
	ImVec4 LocusColors::Transparent = { 0.0f, 0.0f, 0.0f, 0.0f };
	ImVec4 LocusColors::Tan = { 0.41f, 0.375f, 0.313f, 1.0f };
	ImVec4 LocusColors::Orange = { 0.585f, 0.332f, 0.18f, 1.0f };
	ImVec4 LocusColors::Green = { 0.0f, 1.0f, 0.0f, 1.0f };
	ImVec4 LocusColors::Blue = { 0.0f, 0.0f, 1.0f, 1.0f };
	ImVec4 LocusColors::LightBlue = { 0.4f, 0.8f, 1.0f, 1.0f };

	ImVec4 ToImVec4(const glm::vec4& value)
	{
		return { value.x, value.y, value.z, value.w };
	}

	glm::vec4 ToGLMVec4(const ImVec4& value)
	{
		return { value.x, value.y, value.z, value.w };
	}

	ImGuiLayer::ImGuiLayer() : Layer("ImGuiLayer")
	{

	}

	void ImGuiLayer::OnAttach()
	{
		LOCUS_PROFILE_FUNCTION();

		// Setup Dear ImGui context
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGuiIO& io = ImGui::GetIO(); (void)io;
		ImGuiStyle& style = ImGui::GetStyle();
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;       // Enable Keyboard Controls
		//io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
		io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;           // Enable Docking
		io.ConfigDockingNoSplit = true;
		io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;         // Enable Multi-Viewport / Platform Windows
		//io.ConfigFlags |= ImGuiConfigFlags_ViewportsNoTaskBarIcons;
		//io.ConfigFlags |= ImGuiConfigFlags_ViewportsNoMerge;
		io.ConfigDockingAlwaysTabBar = true;

		style.ChildBorderSize = 0.0f;
		style.TabRounding = 3.0f;
		style.FrameRounding = 3.0f;
		style.ChildRounding = 3.0f;
		style.WindowMenuButtonPosition = ImGuiDir_None;
		style.WindowBorderSize = 1.0f;

		// Setup Dear ImGui style
		ImGui::StyleColorsDark();

		// When viewports are enabled we tweak WindowRounding/WindowBg so platform windows can look identical to regular ones.
		if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
		{
			style.WindowRounding = 0.0f;
			style.Colors[ImGuiCol_WindowBg].w = 1.0f;
		}

		io.Fonts->AddFontFromFileTTF("resources/fonts/Inter/Inter-Bold.ttf", 20.0f);
		io.FontDefault = io.Fonts->AddFontFromFileTTF("resources/fonts/Inter/Inter-Regular.ttf", 16.0f);

		SetDarkTheme();

		Application& app = Application::Get();
		GLFWwindow* window = static_cast<GLFWwindow*>(app.GetWindow().GetNativeWindow());

		// Setup Platform/Renderer bindings
		ImGui_ImplGlfw_InitForOpenGL(window, true);
		ImGui_ImplOpenGL3_Init("#version 410");
	}

	void ImGuiLayer::OnDetach()
	{
		LOCUS_PROFILE_FUNCTION();

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
		LOCUS_PROFILE_FUNCTION();

		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();
		ImGuizmo::BeginFrame();
	}

	void ImGuiLayer::End()
	{
		LOCUS_PROFILE_FUNCTION();

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
		auto& colors = ImGui::GetStyle().Colors;
		colors[ImGuiCol_WindowBg] = LocusColors::LightGrey;

		// Headers
		colors[ImGuiCol_Header] = LocusColors::Orange;
		colors[ImGuiCol_HeaderHovered] = LocusColors::Orange;
		colors[ImGuiCol_HeaderActive] = LocusColors::Orange;

		// Buttons
		colors[ImGuiCol_Button] = LocusColors::MediumDarkGrey;
		colors[ImGuiCol_ButtonHovered] = LocusColors::LightGrey;
		colors[ImGuiCol_ButtonActive] = LocusColors::LightGrey;

		// Frame BG
		colors[ImGuiCol_FrameBg] = LocusColors::MediumDarkGrey;
		colors[ImGuiCol_FrameBgHovered] = LocusColors::Orange;
		colors[ImGuiCol_FrameBgActive] = LocusColors::LightGrey;

		// Tabs
		colors[ImGuiCol_Tab] = LocusColors::Grey;
		colors[ImGuiCol_TabHovered] = LocusColors::LightGrey;
		colors[ImGuiCol_TabActive] = LocusColors::LightGrey;
		colors[ImGuiCol_TabUnfocused] = LocusColors::Grey;
		colors[ImGuiCol_TabUnfocusedActive] = LocusColors::LightGrey;

		// Title
		colors[ImGuiCol_TitleBgActive] = LocusColors::MediumDarkGrey;
		colors[ImGuiCol_TitleBg] = LocusColors::MediumDarkGrey;
		colors[ImGuiCol_TitleBgCollapsed] = LocusColors::MediumDarkGrey;

		// Border
		colors[ImGuiCol_Border] = LocusColors::Orange;
	}
}