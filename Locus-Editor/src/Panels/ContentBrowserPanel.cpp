#include "Lpch.h"
#include "ContentBrowserPanel.h"

#include <ImGui/imgui.h>
#include <ImGui/imgui_internal.h>

namespace Locus
{
	ContentBrowserPanel::ContentBrowserPanel()
	{
		m_ProjectDirectory = Application::Get().GetProjectPath() / "Assets";
		m_CurrentDirectory = m_ProjectDirectory;
		m_FolderIcon = Texture2D::Create("resources/icons/FolderIcon.png");
		m_FileIcon = Texture2D::Create("resources/icons/FileIcon.png");
	}

	void ContentBrowserPanel::OnImGuiRender()
	{
		ImGuiWindowFlags windowFlags = ImGuiWindowFlags_TabBarAlignLeft | ImGuiWindowFlags_DockedWindowBorder;
		ImGui::Begin("Content Browser", false, windowFlags);

		ImGuiTableFlags tableFlags = ImGuiTableFlags_SizingStretchProp | ImGuiTableFlags_Resizable
			| ImGuiTableFlags_ContextMenuInBody | ImGuiTableFlags_RowBg;
		ImGui::PushStyleColor(ImGuiCol_TableRowBg, LocusColors::Transparent);
		if (ImGui::BeginTable("CBTable", 2, tableFlags))
		{
			ImGui::TableNextRow(ImGuiTableRowFlags_None, ImGui::GetContentRegionAvail().y);
			ImGui::TableNextColumn();

			// --- Project view ---
			DrawRootDirectoryView();

			ImGui::TableNextColumn();

			// --- Directory view ---
			DrawCurrentDirectoryView();

			ImGui::EndTable();
		}
		ImGui::PopStyleColor();

		ImGui::End();
	}

	void ContentBrowserPanel::DrawCurrentDirectoryView()
	{
		if (m_CurrentDirectory != m_ProjectDirectory)
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
		ImGui::PushStyleColor(ImGuiCol_TableRowBg, LocusColors::MediumDarkGrey);
		if (ImGui::BeginTable("DVTable", columnCount, flags))
		{
			ImGui::TableNextRow(ImGuiTableRowFlags_None, ImGui::GetContentRegionAvail().y - 40.0f);
			for (auto& item : std::filesystem::directory_iterator(m_CurrentDirectory))
			{
				if (item.path().extension() != ".meta")
				{
					ImGui::TableNextColumn();

					const auto& path = item.path();
					std::string filenameString = path.stem().string() + path.extension().string();
					std::string extension = path.extension().string();

					Ref<Texture2D> icon = item.is_directory() ? m_FolderIcon : m_FileIcon;
					ImGui::PushID(filenameString.c_str());
					ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.0f, 0.0f, 0.0f, 0.0f));
					ImGui::ImageButton((ImTextureID)(uint64_t)icon->GetRendererID(), { thumbnailSize, thumbnailSize }, { 0, 1 }, { 1, 0 });

					if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_None))
					{
						uint64_t resourceUUID = ResourceManager::GetResourceUUID(path.string() + ".meta"); // TODO: slow

						if (extension == ".locus")
							ImGui::SetDragDropPayload("SCENE_ITEM_PATH", &resourceUUID, sizeof(uint64_t));
						else if (extension == ".png" || extension == ".jpg" || extension == ".tiff" || extension == ".tif" || extension == ".bmp" || extension == ".tga")
							ImGui::SetDragDropPayload("TEXTURE_ITEM_PATH", &resourceUUID, sizeof(uint64_t));
						else if (extension == ".obj" || extension == ".fbx")
							ImGui::SetDragDropPayload("MESH_ITEM_PATH", &resourceUUID, sizeof(uint64_t));
						else if (extension == ".lmat")
							ImGui::SetDragDropPayload("MATERIAL_ITEM_PATH", &resourceUUID, sizeof(uint64_t));

						ImGui::EndDragDropSource();
					}

					ImGui::PopID();
					ImGui::PopStyleColor();

					if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
					{
						if (item.is_directory())
						{
							m_CurrentDirectory /= path.filename();
						}
					}
					ImGui::TextWrapped(filenameString.c_str());
				}
			}
			ImGui::TableNextRow(ImGuiTableRowFlags_None, 40.0f);
			ImGui::Text("Testing Row");

			ImGui::EndTable();
		}
		ImGui::PopStyleColor();
		
	}

	void ContentBrowserPanel::DrawRootDirectoryView()
	{
		ImGui::Text("Test");
	}
}
