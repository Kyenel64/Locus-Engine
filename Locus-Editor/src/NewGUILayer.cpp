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

		// TODO: set values on load
		m_ViewportHeight = 800.0f;
		m_HierarchyHeight = 400.0f;
		m_CenterSplitter = m_WindowSize.x * 0.8f;
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
		ImGui::ShowDemoWindow();

		static bool dockspaceOpen = true;
		static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None;

		ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoScrollbar;
		ImGuiViewport* viewport = ImGui::GetMainViewport();
		ImGui::SetNextWindowPos(viewport->Pos);
		ImGui::SetNextWindowSize(viewport->Size);
		ImGui::SetNextWindowViewport(viewport->ID);
		window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
		window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;

		if (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode)
			window_flags |= ImGuiWindowFlags_NoBackground;

		ImGui::PushStyleColor(ImGuiCol_WindowBg, { 0.13f, 0.125f, 0.12f, 1.0f });
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(8.0f, 8.0f));
		ImGui::Begin("DockSpace", &dockspaceOpen, window_flags);
		ImGui::PopStyleVar();
		ImGui::PopStyleColor();

		ImGuiStyle& style = ImGui::GetStyle();
		ImGuiIO& io = ImGui::GetIO();
		style.WindowMenuButtonPosition = ImGuiDir_None;

		m_WindowSize.x = ImGui::GetContentRegionAvail().x;
		m_WindowSize.y = ImGui::GetContentRegionAvail().y;
		ProcessResize();

		ImGui::PushStyleColor(ImGuiCol_MenuBarBg, { 0.13f, 0.125f, 0.12f, 1.0f });
		ImGui::BeginMenuBar();
		ImGui::EndMenuBar();
		ImGui::PopStyleColor();

		DrawLayoutTable();

		ImGuiWindowFlags windowFlags = ImGuiWindowFlags_NoResize;


		ImGui::PushStyleColor(ImGuiCol_WindowBg, { 0.29f, 0.28f, 0.27f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_TabUnfocusedActive, { 0.29f, 0.28f, 0.27f, 1.0f });
		ImGui::PushStyleVar(ImGuiStyleVar_ItemInnerSpacing, ImVec2(0.0f, 0.0f));
		ImGui::Begin("Viewport", false, windowFlags);
		ImGui::End();

		ImGui::Begin("ContentBrowser", false, windowFlags);
		ImGui::End();

		ImGui::Begin("SceneHierarchyFrame", false, windowFlags);
		ImGui::End();

		ImGui::Begin("Properties", false, windowFlags);
		ImGui::End();
		ImGui::PopStyleColor(2);
		ImGui::PopStyleVar();


		ImGui::End();
	}

	void NewGUILayer::ProcessResize()
	{
		if (m_PrevWindowSize != m_WindowSize)
		{
			float diffX = m_PrevWindowSize.x - m_WindowSize.x;
			float diffY = m_PrevWindowSize.y - m_WindowSize.y;
			LOCUS_CORE_INFO(diffY);

			m_ViewportHeight -= diffY;
			m_HierarchyHeight -= diffY;
			m_CenterSplitter -= diffX;
		}

		m_PrevWindowSize = m_WindowSize;
	}

	void NewGUILayer::DrawLayoutTable()
	{
		ImGuiIO io = ImGui::GetIO();
		// --- Left Table ---
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, { 5.0f, 5.0f });
		if (ImGui::BeginTable("LayoutLeft", 1, ImGuiTableFlags_None, { m_CenterSplitter, m_WindowSize.y }))
		{
			ImGui::TableNextColumn();
			ImGui::BeginChild("Frame1", { -1.0f, m_ViewportHeight }, true);

			m_FrameSizes[0].x = ImGui::GetWindowSize().x;
			m_FrameSizes[0].y = ImGui::GetWindowSize().y;
			m_FramePositions[0].x = ImGui::GetWindowPos().x;
			m_FramePositions[0].y = ImGui::GetWindowPos().y;

			ImGuiID frame1DockID = ImGui::GetID("Frame1Dock");
			ImGui::DockSpace(frame1DockID, { -1.0f, -1.0f });

			ImGui::EndChild();

			ImGui::Button("##Divider", { -1.0f, 3.0f });
			if (ImGui::IsItemActive())
			{
				m_ViewportHeight += io.MouseDelta.y;
			}

			ImGui::BeginChild("Frame2", { -1.0f, -1.0f }, true);

			m_FrameSizes[1].x = ImGui::GetWindowSize().x;
			m_FrameSizes[1].y = ImGui::GetWindowSize().y;
			m_FramePositions[1].x = ImGui::GetWindowPos().x;
			m_FramePositions[1].y = ImGui::GetWindowPos().y;

			ImGuiID frame2DockID = ImGui::GetID("Frame2Dock");
			ImGui::DockSpace(frame2DockID, { -1.0f, -1.0f });

			ImGui::EndChild();

			ImGui::EndTable();
		}

		// --- Center Splitter ---
		ImGui::SameLine();
		ImGui::Button("##CenterSplitter", { 3.0f, -1.0f });
		if (ImGui::IsItemActive())
		{
			m_CenterSplitter += io.MouseDelta.x;
		}
		ImGui::SameLine();

		// --- Right Table ---
		if (ImGui::BeginTable("LayoutRight", 1, ImGuiTableFlags_None, { -1.0, -1.0f }))
		{
			ImGui::TableNextColumn();
			ImGui::BeginChild("Frame3", { -1.0f, m_HierarchyHeight }, true);

			m_FrameSizes[2].x = ImGui::GetWindowSize().x;
			m_FrameSizes[2].y = ImGui::GetWindowSize().y;
			m_FramePositions[2].x = ImGui::GetWindowPos().x;
			m_FramePositions[2].y = ImGui::GetWindowPos().y;

			ImGuiID frame3DockID = ImGui::GetID("Frame3Dock");
			ImGui::DockSpace(frame3DockID, { -1.0f, -1.0f });

			ImGui::EndChild();

			ImGui::Button("##Divider", { -1.0f, 3.0f });
			if (ImGui::IsItemActive())
			{
				m_HierarchyHeight += io.MouseDelta.y;
			}

			ImGui::BeginChild("Frame4", { -1.0f, -1.0f }, true);

			m_FrameSizes[3].x = ImGui::GetWindowSize().x;
			m_FrameSizes[3].y = ImGui::GetWindowSize().y;
			m_FramePositions[3].x = ImGui::GetWindowPos().x;
			m_FramePositions[3].y = ImGui::GetWindowPos().y;

			ImGuiID frame4DockID = ImGui::GetID("Frame4Dock");
			ImGui::DockSpace(frame4DockID, { -1.0f, -1.0f });

			ImGui::EndChild();

			ImGui::EndTable();
		}
		ImGui::PopStyleVar(2);
	}

}
