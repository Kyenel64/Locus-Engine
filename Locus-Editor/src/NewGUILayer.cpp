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
		m_PrevWindowSize = m_WindowSize;

		m_ViewportHeight = 800.0f;
	}

	void NewGUILayer::OnDetach()
	{

	}

	void NewGUILayer::OnUpdate(Timestep deltaTime)
	{
	}

	void NewGUILayer::OnEvent(Event& event)
	{
	}

	void NewGUILayer::OnImGuiRender()
	{
		//ImGui::ShowDemoWindow();

		static bool dockspaceOpen = true;
		static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None;

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
		ImGuiIO io = ImGui::GetIO();

		m_WindowSize.x = ImGui::GetContentRegionAvail().x;
		m_WindowSize.y = ImGui::GetContentRegionAvail().y;
		ProcessResize();

		ImGui::BeginMenuBar();
		ImGui::EndMenuBar();

		if (ImGui::BeginTable("Layout", 1, ImGuiTableFlags_Borders, { m_WindowSize.x * 0.5f, m_WindowSize.y }))
		{
			ImGui::TableNextColumn();
			ImGui::BeginChild("Frame1", { -1.0f, m_ViewportHeight }, true);
			ImGui::EndChild();

			ImGui::Button("Divider", { -1.0f, 5.0f });
			if (ImGui::IsItemActive())
			{
				m_ViewportHeight += io.MouseDelta.y;
			}

			ImGui::BeginChild("Frame2", { -1.0f, -1.0f }, true);
			ImGui::EndChild();

			ImGui::EndTable();
		}

		//ProcessDragBars();

		/*ImGuiWindowFlags windowFlags = ImGuiWindowFlags_None;
		ImGui::Begin("ViewportWindow", false, windowFlags);
		ImGui::End();

		/*ImGui::Begin("SceneHierarchyFrame", false, windowFlags);
		ImGui::End();

		ImGui::Begin("Properties", false, windowFlags);
		ImGui::End();

		ImGui::Begin("ContentBrowser", false, windowFlags);
		ImGui::End();*/


		ImGui::End();

	}

	void NewGUILayer::ProcessDragBars()
	{
		ImGuiIO io = ImGui::GetIO();
		ImGuiWindowFlags buttonFlags = ImGuiWindowFlags_NoDecoration;
		ImGui::PushStyleVar(ImGuiStyleVar_WindowMinSize, { 0.0f, 0.0f });
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, { 0.0f, 4.0f });
		ImGui::Begin("Slider1", false, buttonFlags);
		ImGui::SetWindowSize({ 100.0f, 10.0f });
		ImGui::Button("HorizontalDividerRight", { -1.0f, 2.0f });
		if (ImGui::IsItemActive())
		{
			ImVec2 newButtonPos = { ImGui::GetWindowPos().x, ImGui::GetWindowPos().y + io.MouseDelta.y };
			ImGui::SetWindowPos(newButtonPos);

			// Modify viewport size
			ImGuiWindow* viewportWindow = ImGui::FindWindowByName("ViewportWindow");
			ImVec2 newViewportSize = { viewportWindow->Size.x, viewportWindow->Size.y + io.MouseDelta.y };
			ImGui::SetWindowSize(viewportWindow, newViewportSize);

			// Modify browser Pos + Size
			ImGuiWindow* browserWindow = ImGui::FindWindowByName("ContentBrowser");
			ImVec2 newBrowserSize = { browserWindow->Size.x, browserWindow->Size.y - io.MouseDelta.y };
			ImVec2 newBrowserPos = { browserWindow->Pos.x, browserWindow->Pos.y + io.MouseDelta.y };
			ImGui::SetWindowSize(browserWindow, newBrowserSize);
			ImGui::SetWindowPos(browserWindow, newBrowserPos);
		}
		ImGui::End();
		ImGui::PopStyleVar(2);
	}

	void NewGUILayer::ProcessResize()
	{
		if (m_PrevWindowSize != m_WindowSize)
		{
			float diffY = m_PrevWindowSize.y - m_WindowSize.y;
			LOCUS_CORE_INFO(diffY);

			m_ViewportHeight -= diffY;
		}

		m_PrevWindowSize = m_WindowSize;
	}
}
