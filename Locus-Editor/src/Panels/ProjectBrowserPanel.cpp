#include "Lpch.h"
#include "ProjectBrowserPanel.h"

#include <ImGui/imgui.h>
#include <ImGui/imgui_internal.h>

namespace Locus
{
	extern UUID g_SelectedResourceID = 0;
	extern ResourceType g_SelectedResourceType = ResourceType::None;

	ProjectBrowserPanel::ProjectBrowserPanel()
	{
		m_AssetsDirectory = Application::Get().GetProjectPath() / "Assets";
		m_CurrentDirectory = m_AssetsDirectory;
		m_FolderIcon = Texture2D::Create("resources/icons/FolderIcon.png");
		m_FileIcon = Texture2D::Create("resources/icons/FileIcon.png");
		m_PlusIcon = Texture2D::Create("resources/icons/PlusIcon.png");

		m_CurrentDirectory = m_AssetsDirectory / "Models" / "SciFiHelmet";

		for (auto& item : std::filesystem::directory_iterator(m_CurrentDirectory))
		{
			m_DirectoryItems.push_back(item);
		}
	}

	void ProjectBrowserPanel::OnImGuiRender()
	{
		LOCUS_PROFILE_FUNCTION();
		ImGuiWindowFlags windowFlags = ImGuiWindowFlags_TabBarAlignLeft | ImGuiWindowFlags_DockedWindowBorder | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse;
		ImGui::Begin("Project Browser", false, windowFlags);

		DrawTopBar();

		ImGuiTableFlags tableFlags = ImGuiTableFlags_SizingStretchProp | ImGuiTableFlags_Resizable
			| ImGuiTableFlags_ContextMenuInBody | ImGuiTableFlags_RowBg;
		ImGui::PushStyleColor(ImGuiCol_TableRowBg, LocusColors::Transparent);
		ImGui::PushStyleColor(ImGuiCol_SeparatorHovered, LocusColors::Transparent);
		ImGui::PushStyleColor(ImGuiCol_SeparatorActive, LocusColors::Transparent);
		ImGui::PushStyleColor(ImGuiCol_TableBorderLight, LocusColors::Transparent);
		if (ImGui::BeginTable("PBTable", 2, tableFlags))
		{
			ImGui::TableNextRow(ImGuiTableRowFlags_None, ImGui::GetContentRegionAvail().y);
			ImGui::TableNextColumn();

			// --- Project view ---
			DrawProjectDirectoryView();

			ImGui::TableNextColumn();

			// --- Directory view ---
			DrawCurrentDirectoryView();

			ImGui::EndTable();
		}
		ImGui::PopStyleColor(4);

		ImGui::End();
	}

	void ProjectBrowserPanel::DrawTopBar()
	{
		LOCUS_PROFILE_FUNCTION();
		// Plus button
		ImVec2 textSize = ImGui::CalcTextSize("A");
		ImGui::PushStyleColor(ImGuiCol_Button, LocusColors::Transparent);
		ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 0.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, { ImGui::GetStyle().FramePadding.y, ImGui::GetStyle().FramePadding.y });
		if (ImGui::ImageButton((ImTextureID)(uintptr_t)m_PlusIcon->GetRendererID(), { textSize.y, textSize.y }, { 0, 1 }, { 1, 0 }))
		{

		}
		ImGui::PopStyleVar(2);
		ImGui::PopStyleColor();

		ImGui::SameLine();

		// Breadcrumb
		std::string curPath = m_CurrentDirectory.string();
		ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, { 2.0f, ImGui::GetStyle().FramePadding.y });
		ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 0.0f);
		ImGui::PushStyleColor(ImGuiCol_Button, LocusColors::Transparent);
		if (ImGui::Button("Assets"))
			m_CurrentDirectory = m_AssetsDirectory;
		while (curPath != m_AssetsDirectory.string())
		{
			size_t filenameBegin = curPath.find_last_of("\\\\");
			m_Breadcrumb.push(curPath.substr(filenameBegin + 1));
			curPath.erase(filenameBegin, curPath.size());
		}
		while (!m_Breadcrumb.empty())
		{
			ImGui::SameLine();
			ImGui::Text(">");
			ImGui::SameLine();
			if (ImGui::Button(m_Breadcrumb.top().c_str()))
			{
				while (m_CurrentDirectory.filename() != m_Breadcrumb.top())
					m_CurrentDirectory = m_CurrentDirectory.parent_path();
			}

			m_Breadcrumb.pop();
		}
		if (!m_SelectedResource.empty())
		{
			ImGui::SameLine();
			ImGui::Text(">");
			ImGui::SameLine();
			ImGui::Button(m_SelectedResource.filename().string().c_str());
		}
		ImGui::PopStyleColor();
		ImGui::PopStyleVar(2);

		ImGui::SameLine();

		// Search bar
		ImGuiTextFilter filter;
		bool showSearchText = true;
		float inputPosY = ImGui::GetCursorPosY();

		char buffer[256];
		memset(buffer, 0, sizeof(buffer));
		float width = ImGui::GetWindowSize().x * 0.3f;
		textSize = ImGui::CalcTextSize("Search");
		ImGui::SetCursorPosX(ImGui::GetWindowSize().x * 0.5f - (width * 0.5f));
		ImGui::PushItemWidth(width);
		if (filter.Draw("##SearchFile"))
			showSearchText = false;
		ImGui::PopItemWidth();
		// Draw "Search" text
		if (showSearchText)
		{
			ImDrawList* drawList = ImGui::GetWindowDrawList();
			drawList->AddText({ ImGui::GetWindowPos().x + (ImGui::GetWindowSize().x - textSize.x) * 0.5f, ImGui::GetWindowPos().y + inputPosY + ImGui::GetStyle().FramePadding.y },
				ImGui::GetColorU32(LocusColors::Orange), "Search");
		}

		ImGui::SameLine(ImGui::GetContentRegionAvail().x - 190.0f);

		ImGui::PushStyleColor(ImGuiCol_Button, LocusColors::Transparent);
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, LocusColors::Transparent);
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, LocusColors::Transparent);
		ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 0.0f);
		ImGui::Button("Icon", { 40.0f, 0.0f });
		ImGui::PopStyleVar();
		ImGui::PopStyleColor(3);
		ImGui::SameLine();
		ImGui::PushStyleColor(ImGuiCol_SliderGrab, LocusColors::LightGrey);
		ImGui::PushStyleColor(ImGuiCol_SliderGrabActive, LocusColors::White);
		ImGui::PushItemWidth(150.0f);
		ImGui::SliderFloat("##IconSize", &m_IconSize, 10.0f, 450.0f, "%.0f");
		ImGui::PopStyleColor(2);
	}

	void ProjectBrowserPanel::DrawCurrentDirectoryView()
	{
		LOCUS_PROFILE_FUNCTION();
#if 0
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

		static ImGuiTableFlags flags = ImGuiTableFlags_RowBg | ImGuiTableFlags_ContextMenuInBody;
		ImGui::PushStyleColor(ImGuiCol_TableRowBg, LocusColors::MediumDarkGrey);
		if (ImGui::BeginTable("DVTable", columnCount, flags))
		{
			ImGui::TableNextRow(ImGuiTableRowFlags_None, ImGui::GetContentRegionAvail().y - 40.0f);
			for (auto& item : std::filesystem::directory_iterator(m_CurrentDirectory)) // TODO: Iterate through resource manager
			{
				if (item.path().extension() != ".meta")
				{
					ImGui::TableNextColumn();

					const auto& path = item.path();
					std::string filenameString = path.stem().string() + path.extension().string();
					std::string extension = path.extension().string();

					ResourceType type = ResourceType::None;
					if (extension == ".locus")
						type = ResourceType::Scene;
					else if (extension == ".png" || extension == ".jpg" || extension == ".tiff" || extension == ".tif" || extension == ".bmp" || extension == ".tga")
						type = ResourceType::Texture;
					else if (extension == ".obj" || extension == ".fbx" || extension == ".gltf")
						type = ResourceType::Model;
					else if (extension == ".lmat")
						type = ResourceType::Material;

					Ref<Texture2D> icon = item.is_directory() ? m_FolderIcon : m_FileIcon;
					ImGui::PushID(filenameString.c_str());
					ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.0f, 0.0f, 0.0f, 0.0f));
					ImGui::ImageButton((ImTextureID)(uint64_t)icon->GetRendererID(), { thumbnailSize, thumbnailSize }, { 0, 1 }, { 1, 0 });

					if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_None))
					{
						uint64_t resourceUUID = ResourceManager::GetResourceUUID(path.string() + ".meta"); // TODO: slow

						switch (type)
						{
						case Locus::ResourceType::None: break;
						case Locus::ResourceType::Texture: ImGui::SetDragDropPayload("TEXTURE_ITEM_PATH", &resourceUUID, sizeof(uint64_t)); break;
						case Locus::ResourceType::Material: ImGui::SetDragDropPayload("MATERIAL_ITEM_PATH", &resourceUUID, sizeof(uint64_t)); break;
						case Locus::ResourceType::Model: ImGui::SetDragDropPayload("MODEL_ITEM_PATH", &resourceUUID, sizeof(uint64_t)); break;
						case Locus::ResourceType::Scene: ImGui::SetDragDropPayload("SCENE_ITEM_PATH", &resourceUUID, sizeof(uint64_t)); break;
						case Locus::ResourceType::Script: break;
						}
						ImGui::EndDragDropSource();
					}

					ImGui::PopID();
					ImGui::PopStyleColor();

					if (ImGui::IsItemHovered())
					{
						if (ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left) && item.is_directory())
							m_CurrentDirectory /= path.filename();
						else if (ImGui::IsMouseClicked(ImGuiMouseButton_Left))
						{
							g_SelectedResourceID = ResourceManager::GetResourceUUID(path.string() + ".meta");
							switch (type)
							{
							case Locus::ResourceType::None: g_SelectedResourceType = ResourceType::None; break;
							case Locus::ResourceType::Texture: g_SelectedResourceType = ResourceType::Texture; break;
							case Locus::ResourceType::Material: g_SelectedResourceType = ResourceType::Material; break;
							case Locus::ResourceType::Model: g_SelectedResourceType = ResourceType::Model; break;
							case Locus::ResourceType::Scene: g_SelectedResourceType = ResourceType::Scene; break;
							case Locus::ResourceType::Script: g_SelectedResourceType = ResourceType::Script; break;
							}
						}
					}
					ImGui::TextWrapped(filenameString.c_str());
				}
			}

			ImGui::EndTable();
		}
		ImGui::PopStyleColor();
#endif
		ImGui::BeginChild("CurrentDirectoryView", { -1.0f, -1.0f });
		float contentWidth = ImGui::GetContentRegionAvail().x;
		int maxPerRow = static_cast<int>(contentWidth / (m_IconSize + ImGui::GetStyle().ItemSpacing.x)) - 1;
		ImGuiTableFlags flags = ImGuiTableFlags_SizingStretchSame | ImGuiTableFlags_Resizable;
		ImGui::BeginTable("Table", maxPerRow, flags, { -1.0f, -1.0f }, -1.0f);

		for (auto& item : std::filesystem::directory_iterator(m_CurrentDirectory))
		{
			if (item.path().extension() != ".meta")
			{
				ImGui::TableNextColumn();
				const std::filesystem::path& path = item.path();
				std::string filenameString = path.filename().string() + path.extension().string();
				std::string extension = path.extension().string();
		
				ResourceType type = ResourceType::None;
				if (extension == ".locus")
					type = ResourceType::Scene;
				else if (extension == ".png" || extension == ".jpg" || extension == ".tiff" || extension == ".tif" || extension == ".bmp" || extension == ".tga")
					type = ResourceType::Texture;
				else if (extension == ".obj" || extension == ".fbx" || extension == ".gltf")
					type = ResourceType::Model;
				else if (extension == ".lmat")
					type = ResourceType::Material;
		
				Ref<Texture2D> icon = nullptr;
				if (item.is_directory())
					icon = m_FolderIcon;
				//else if (type == ResourceType::Texture)
				//	icon = TextureHandle(ResourceManager::GetResourceUUID(item.path().string() + ".meta")).Get(); // TODO: slow
				else
					icon = m_FileIcon;
		
				DrawItem(item, icon, type);
			}
		}

		ImGui::EndTable();

		ImGui::EndChild();
	}

	void ProjectBrowserPanel::DrawProjectDirectoryView()
	{
		LOCUS_PROFILE_FUNCTION();
		ImGui::BeginChild("ProjectDirectoryView", { -1.0f, -1.0f });
		ImGui::EndChild();
	}

	void ProjectBrowserPanel::DrawItem(const std::filesystem::path& item, Ref<Texture2D> icon, ResourceType type)
	{
		LOCUS_PROFILE_FUNCTION();
		std::string itemName = item.filename().string();
		std::string label = "##" + itemName;
		ImGui::PushID(itemName.c_str());

		ImVec4 buttonColor = LocusColors::Transparent;
		if (m_SelectedResource == item)
			buttonColor = LocusColors::LightGrey;

		ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, { 0.0f, 0.0f });
		ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 0.0f);
		ImGui::PushStyleColor(ImGuiCol_Button, buttonColor);
		if (ImGui::ImageButton((ImTextureID)(uint64_t)icon->GetRendererID(), { m_IconSize, m_IconSize }, { 0, 1 }, { 1, 0 }))
		{
			m_SelectedResource = item;
			g_SelectedResourceID = ResourceManager::GetResourceUUID(item.string() + ".meta");
			switch (type)
			{
			case Locus::ResourceType::None: g_SelectedResourceType = ResourceType::None; break;
			case Locus::ResourceType::Texture: g_SelectedResourceType = ResourceType::Texture; break;
			case Locus::ResourceType::Material: g_SelectedResourceType = ResourceType::Material; break;
			case Locus::ResourceType::Model: g_SelectedResourceType = ResourceType::Model; break;
			case Locus::ResourceType::Scene: g_SelectedResourceType = ResourceType::Scene; break;
			case Locus::ResourceType::Script: g_SelectedResourceType = ResourceType::Script; break;
			}
		}
		//char buffer[256];
		//memset(buffer, 0, sizeof(buffer));
		//strcpy_s(buffer, sizeof(buffer), itemName.c_str());
		//
		//ImGui::PushFont(ImGui::GetIO().Fonts->Fonts[1]);
		//ImVec2 textSize = ImGui::CalcTextSize(itemName.c_str());
		//ImGui::PushItemWidth(-1);
		//ImGui::InputText(label.c_str(), buffer, sizeof(buffer), ImGuiInputTextFlags_EnterReturnsTrue);
		//ImGui::PopFont();

		ImGui::PushFont(ImGui::GetIO().Fonts->Fonts[1]);
		ImGui::TextWrapped(itemName.c_str());
		ImGui::PopFont();

		ImGui::PopID();
		ImGui::PopStyleColor();
		ImGui::PopStyleVar(2);
	}
}
