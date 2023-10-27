#include "Lpch.h"
#include "ContentBrowserPanel.h"

#include <ImGui/imgui.h>
#include <ImGui/imgui_internal.h>

namespace Locus
{
	// TODO: move to project folder in the future when we create project folders
	extern const std::filesystem::path g_ProjectPath = "SandboxProject/Assets";

	ContentBrowserPanel::ContentBrowserPanel()
		: m_CurrentDirectory(g_ProjectPath)
	{
		m_FolderIcon = Texture2D::Create("resources/icons/FolderIcon.png");
		m_FileIcon = Texture2D::Create("resources/icons/FileIcon.png");
	}

	void ContentBrowserPanel::OnImGuiRender()
	{
		ImGuiWindowFlags windowFlags = ImGuiWindowFlags_NoScrollbar;
		ImGui::Begin(" Content Browser ", false, windowFlags);

		static ImGuiTableFlags tableFlags = ImGuiTableFlags_SizingStretchProp | ImGuiTableFlags_Resizable
			| ImGuiTableFlags_ContextMenuInBody | ImGuiTableFlags_RowBg;
		ImGui::PushStyleColor(ImGuiCol_TableRowBg, { 0.2f, 0.2f, 0.2f, 1.0f });
		if (ImGui::BeginTable("CBTable", 2, tableFlags))
		{
			ImGui::TableNextRow(ImGuiTableRowFlags_None, ImGui::GetContentRegionAvail().y);
			ImGui::TableNextColumn();
			// --- Project view ---------------------------------------------------
			DrawRootDirectoryView();

			ImGui::TableNextColumn();

			// --- Directory view -------------------------------------------------
			DrawCurrentDirectoryView();

			ImGui::EndTable();
		}
		ImGui::PopStyleColor();

		ImGui::End();
	}

	void ContentBrowserPanel::DrawCurrentDirectoryView()
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

		//ImGui::PushItemWidth(-1);
		float panelWidth = ImGui::GetContentRegionAvail().x;
		//ImGui::PopItemWidth();
		int columnCount = (int)(panelWidth / cellSize);
		if (columnCount < 1)
			columnCount = 1;

		static ImGuiTableFlags flags = ImGuiTableFlags_RowBg;
		ImGui::PushStyleColor(ImGuiCol_TableRowBg, { 0.2f, 0.2f, 0.2f, 1.0f });
		if (ImGui::BeginTable("DVTable", columnCount, flags))
		{
			ImGui::TableNextRow(ImGuiTableRowFlags_None, ImGui::GetContentRegionAvail().y - 40.0f);
			for (auto& entry : std::filesystem::directory_iterator(m_CurrentDirectory))
			{
				ImGui::TableNextColumn();

				const auto& path = entry.path();
				auto relativePath = std::filesystem::relative(path, g_ProjectPath);
				std::string filenameString = relativePath.filename().string();

				// Find extension
				size_t containsDot = filenameString.find(".");
				size_t pos = filenameString.find_last_of(".") + 1;
				std::string extension;
				if (containsDot != std::string::npos)
					extension = filenameString.substr(filenameString.find_last_of(".") + 1);

				Ref<Texture2D> icon = entry.is_directory() ? m_FolderIcon : m_FileIcon;
				ImGui::PushID(filenameString.c_str());
				ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.0f, 0.0f, 0.0f, 0.0f));
				ImGui::ImageButton((ImTextureID)(uint64_t)icon->GetRendererID(), { thumbnailSize, thumbnailSize }, { 0, 1 }, { 1, 0 });

				if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_None))
				{
					const wchar_t* itemPath = relativePath.c_str();

					if (extension == "locus")
						ImGui::SetDragDropPayload("SCENE_ITEM_PATH", itemPath, (wcslen(itemPath) + 1) * sizeof(wchar_t));
					else if (extension == "png" || extension == "jpg" || extension == "tiff" || extension == "tif"
						|| extension == "bmp" || extension == "tga")
						ImGui::SetDragDropPayload("TEXTURE_ITEM_PATH", itemPath, (wcslen(itemPath) + 1) * sizeof(wchar_t));

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
			ImGui::TableNextRow(ImGuiTableRowFlags_None, 40.0f);
			ImGui::Text("Testing Rowfffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff");

			ImGui::EndTable();
		}
		ImGui::PopStyleColor();
		
	}

	void ContentBrowserPanel::DrawRootDirectoryView()
	{
		ImGui::Text("Test");
	}
}