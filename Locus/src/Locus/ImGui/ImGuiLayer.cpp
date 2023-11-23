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
	ImVec4 LocusColors::DarkGrey = { 0.05f, 0.05f, 0.05f, 1.0f };
	ImVec4 LocusColors::MediumDarkGrey = { 0.11f, 0.11f, 0.11f, 1.0f };
	ImVec4 LocusColors::Grey = { 0.14f, 0.14f, 0.14f, 1.0f };
	ImVec4 LocusColors::LightGrey = { 0.17f, 0.17f, 0.17f, 1.0f };
	ImVec4 LocusColors::White = { 1.0f, 1.0f, 1.0f, 1.0f };
	ImVec4 LocusColors::Pink = { 0.9f, 0.4f, 0.4f, 1.0f };
	ImVec4 LocusColors::Transparent = { 0.0f, 0.0f, 0.0f, 0.0f };
	ImVec4 LocusColors::Tan = { 0.40f, 0.40f, 0.40f, 1.0f };
	ImVec4 LocusColors::Orange = { 0.35f, 0.35f, 0.35f, 1.0f };
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
		io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;           // Enable Docking
		io.ConfigDockingNoSplit = true;
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
		style.IndentSpacing = 10.0f;
		style.ScrollbarSize = 5.0f;
		style.WindowMenuButtonPosition = ImGuiDir_None;
		style.ItemSpacing = { 6.0f, 6.0f };
		style.WindowPadding = { 10.0f, 10.0f };
		style.FramePadding = { 10.0f, 4.0f };

		SetDarkTheme();

		// Setup Platform/Renderer bindings
		Application& app = Application::Get();
		GLFWwindow* window = static_cast<GLFWwindow*>(app.GetWindow().GetNativeWindow());
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
		colors[ImGuiCol_WindowBg] = LocusColors::DarkGrey;

		// Headers
		colors[ImGuiCol_Header] = LocusColors::DarkGrey;
		colors[ImGuiCol_HeaderHovered] = LocusColors::Orange;
		colors[ImGuiCol_HeaderActive] = LocusColors::Orange;

		// Buttons
		colors[ImGuiCol_Button] = LocusColors::LightGrey;
		colors[ImGuiCol_ButtonHovered] = LocusColors::Grey;
		colors[ImGuiCol_ButtonActive] = LocusColors::LightGrey;

		// Frame BG
		colors[ImGuiCol_FrameBg] = LocusColors::DarkGrey;
		colors[ImGuiCol_FrameBgHovered] = { 0.02f, 0.02f, 0.02f, 1.0f };
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
		colors[ImGuiCol_Border] = LocusColors::LightGrey;

		// Popups
		colors[ImGuiCol_PopupBg] = LocusColors::Grey;

		// Child
		colors[ImGuiCol_ChildBg] = LocusColors::MediumDarkGrey;
	}
}
