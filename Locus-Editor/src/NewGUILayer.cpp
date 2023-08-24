#include "NewGUILayer.h"

#include <imgui/imgui.h>
#include <imgui/imgui_internal.h>
#include <glm/gtc/type_ptr.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/matrix_decompose.hpp>

#include "Command/Command.h"

namespace Locus
{
	NewGUILayer::NewGUILayer()
		: Layer("NewGUILayer")
	{
	}

	NewGUILayer::~NewGUILayer()
	{
		CommandHistory::Shutdown();
	}

	void NewGUILayer::OnAttach()
	{
		m_WindowSize.x = Application::Get().GetWindow().GetWidth();
		m_WindowSize.y = Application::Get().GetWindow().GetHeight();

		m_FrameSizes[0] = { m_WindowSize.x *        0.8f       - (m_WindowPadding * 2), m_WindowSize.y *        0.7f       - (m_WindowPadding * 2) - m_MenuBarHeight - m_TabBarHeight };
		m_FrameSizes[1] = { m_WindowSize.x - m_FrameSizes[0].x - (m_WindowPadding * 4), m_WindowSize.y *        0.5f       - (m_WindowPadding * 2) - m_MenuBarHeight - m_TabBarHeight };
	}

	void NewGUILayer::OnDetach()
	{

	}

	void NewGUILayer::OnUpdate(Timestep deltaTime)
	{
		m_WindowSize.x = Application::Get().GetWindow().GetWidth();
		m_WindowSize.y = Application::Get().GetWindow().GetHeight();

		if (m_LayoutStyle == LayoutStyle::Default)
		{
			m_FrameSizes[0] = { };
			m_FramePositions[0] = { m_WindowPadding, 25.0f + m_WindowPadding };
		}

	}

	void NewGUILayer::OnEvent(Event& event)
	{

	}

	void NewGUILayer::OnImGuiRender()
	{
		ImGui::ShowDemoWindow();

		static bool dockspaceOpen = true;
		static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_NoSplit;

		ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
		ImGuiViewport* viewport = ImGui::GetMainViewport();
		ImGui::SetNextWindowPos(viewport->Pos);
		ImGui::SetNextWindowSize(viewport->Size);
		ImGui::SetNextWindowViewport(viewport->ID);
		window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
		window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;

		if (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode)
			window_flags |= ImGuiWindowFlags_NoBackground;

		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
		ImGui::Begin("DockSpace", &dockspaceOpen, window_flags);
		ImGui::PopStyleVar();

		ImGui::BeginMainMenuBar();
		ImGui::EndMainMenuBar();

		bool selected = false;
		ImGui::Selectable("movebar", selected);

		ImGuiWindowFlags windowFlags =  ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoDocking;

		ImGui::Begin("ViewportWindow", false, windowFlags);
		ImGui::SetWindowSize({ m_FrameSizes[0].x, m_FrameSizes[0].y });
		ImGui::SetWindowPos({ m_FramePositions[0].x, m_FramePositions[0].y });
		ImGui::End();

		ImGui::Begin("SceneHierarchyFrame", false, windowFlags);
		ImGui::SetWindowSize({ m_FrameSizes[1].x, m_FrameSizes[1].y });
		ImGui::End();

		ImGui::Begin("Properties", false, windowFlags);
		ImGui::SetWindowSize({ m_FrameSizes[2].x, m_FrameSizes[2].y });
		ImGui::End();

		ImGui::Begin("ContentBrowser", false, windowFlags);
		ImGui::SetWindowSize({ m_FrameSizes[3].x, m_FrameSizes[3].y });
		ImGui::End();


		ImGui::End();
	}
}
