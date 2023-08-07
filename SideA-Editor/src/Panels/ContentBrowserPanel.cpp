#include "SApch.h"
#include "ContentBrowserPanel.h"


#include <ImGui/imgui.h>
#include <ImGui/imgui_internal.h>

namespace SideA
{

	// TODO: move to project folder in the future when we create project folders
	extern const std::filesystem::path g_ProjectPath = "assets";

	ContentBrowserPanel::ContentBrowserPanel()
		: m_CurrentDirectory(g_ProjectPath)
	{
		m_FolderIcon = Texture2D::Create("resources/icons/FolderIcon.png");
		m_FileIcon = Texture2D::Create("resources/icons/FileIcon.png");
	}

	void ContentBrowserPanel::OnImGuiRender()
	{
		ImGui::Begin("Content Browser");

		ImGuiStyle& style = ImGui::GetStyle();
		style.WindowMinSize.y = 300.0f;

		static ImGuiTableFlags flags = ImGuiTableFlags_SizingStretchProp | ImGuiTableFlags_Resizable
			| ImGuiTableFlags_ContextMenuInBody | ImGuiTableFlags_RowBg;
		if (ImGui::BeginTable("CBTable", 2, flags))
		{
			ImGui::TableNextColumn();
			// --- Project view ---------------------------------------------------
			DrawProjectView();
			//ImGui::Text("TestLeft");

			ImGui::TableNextColumn();

			// --- Directory view -------------------------------------------------
			DrawDirectoryView();
			//ImGui::Text("TestRight");

			ImGui::EndTable();
		}

		ImGui::End(); // End Content Browser
	}

	void ContentBrowserPanel::DrawDirectoryView()
	{
		if (m_CurrentDirectory != g_ProjectPath)
		{
			if (ImGui::Button("<-"))
			{
				m_CurrentDirectory = m_CurrentDirectory.parent_path();
			}
		}

		static float padding = 16.0f;
		static float thumbnailSize = 64.0f;
		float cellSize = thumbnailSize + padding;

		ImGui::PushItemWidth(-1);
		float panelWidth = ImGui::GetContentRegionAvail().x;
		ImGui::PopItemWidth();
		int columnCount = (int)(panelWidth / cellSize);
		if (columnCount < 1)
			columnCount = 1;

		static ImGuiTableFlags flags = ImGuiTableFlags_RowBg;
		ImGui::PushStyleColor(ImGuiCol_TableRowBg, { 0.2f, 0.2f, 0.2f, 1.0f });
		if (ImGui::BeginTable("DVTable", columnCount, flags))
		{
			for (auto& entry : std::filesystem::directory_iterator(m_CurrentDirectory))
			{
				ImGui::TableNextColumn();

				const auto& path = entry.path();
				auto relativePath = std::filesystem::relative(path, g_ProjectPath);
				std::string filenameString = relativePath.filename().string();
				std::size_t pos = filenameString.find(".sidea");
				std::string extension;
				if (pos != std::string::npos)
					extension = filenameString.substr(pos);

				Ref<Texture2D> icon = entry.is_directory() ? m_FolderIcon : m_FileIcon;

				ImGui::PushID(filenameString.c_str());
				ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.0f, 0.0f, 0.0f, 0.0f));
				ImGui::ImageButton((ImTextureID)icon->GetRendererID(), { thumbnailSize, thumbnailSize }, { 0, 1 }, { 1, 0 });

				if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_None))
				{
					const wchar_t* itemPath = relativePath.c_str();
					SIDEA_CORE_INFO(extension);

					if (extension == ".sidea")
						ImGui::SetDragDropPayload("SCENE_ITEM_PATH", itemPath, (wcslen(itemPath) + 1) * sizeof(wchar_t));

					ImGui::EndDragDropSource();
				}

				ImGui::PopID();
				ImGui::PopStyleColor();

				if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
				{
					if (entry.is_directory())
					{
						m_CurrentDirectory /= path.filename();
					}
				}
				ImGui::TextWrapped(filenameString.c_str());
			}
			ImGui::TableNextRow();
			ImGui::DragFloat("Thumbnail Size", &thumbnailSize);

			ImGui::EndTable();
		}
		ImGui::PopStyleColor();
		
		//ImGui::TableNextColumn();
		//ImGui::SliderFloat("ThumbnailSize", &thumbnailSize, 16, 512);
	}

	void ContentBrowserPanel::DrawProjectView()
	{
		ImGui::Text("Test");
	}
}