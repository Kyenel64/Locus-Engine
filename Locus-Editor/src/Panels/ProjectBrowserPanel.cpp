#include "Lpch.h"
#include "ProjectBrowserPanel.h"

#include <ImGui/imgui.h>
#include <ImGui/imgui_internal.h>

namespace Locus
{
	extern std::filesystem::path g_SelectedResourcePath = std::filesystem::path();

	ProjectBrowserPanel::ProjectBrowserPanel()
	{
		m_AssetsDirectory = Application::Get().GetProjectPath() / "Assets";
		m_CurrentDirectory = m_AssetsDirectory;
		m_FolderIcon = Texture2D::Create("resources/icons/FolderIcon.png");
		m_FileIcon = Texture2D::Create("resources/icons/FileIcon.png");
		m_PlusIcon = Texture2D::Create("resources/icons/PlusIcon.png");

		m_CurrentDirectory = m_AssetsDirectory;

		SetDirectory(m_CurrentDirectory);
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
			ImGui::OpenPopup("CreateResourcePopup");
		}
		ImGui::PopStyleVar(2);
		ImGui::PopStyleColor();

		// Popup when clicking plus button. 
		ImGui::SetNextWindowPos({ ImGui::GetWindowPos().x + ImGui::GetCursorPosX(), ImGui::GetWindowPos().y + ImGui::GetCursorPosY() - ImGui::GetStyle().ItemSpacing.y });
		if (ImGui::BeginPopup("CreateResourcePopup"))
		{
			if (ImGui::MenuItem("Create new material"))
				m_OpenCreateNewMaterialPopup = true;

			ImGui::EndPopup();
		}

		ProcessCreateNewMaterialPopup();

		ImGui::SameLine();

		// Breadcrumb
		std::string curPath = m_CurrentDirectory.string();
		ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, { 2.0f, ImGui::GetStyle().FramePadding.y });
		ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 0.0f);
		ImGui::PushStyleColor(ImGuiCol_Button, LocusColors::Transparent);
		if (ImGui::Button("Assets"))
			SetDirectory(m_AssetsDirectory);
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
					SetDirectory(m_CurrentDirectory.parent_path());
			}
			m_Breadcrumb.pop();
		}
		if (!g_SelectedResourcePath.empty())
		{
			ImGui::SameLine();
			ImGui::Text(">");
			ImGui::SameLine();
			ImGui::Button(g_SelectedResourcePath.filename().string().c_str());
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

		ImGui::BeginChild("CurrentDirectoryView", { -1.0f, -1.0f });

		float contentWidth = ImGui::GetContentRegionAvail().x;
		int maxPerRow = static_cast<int>(contentWidth / (m_IconSize + ImGui::GetStyle().ItemSpacing.x)) - 1;
		ImGuiTableFlags flags = ImGuiTableFlags_SizingStretchSame;
		ImGui::BeginTable("Table", maxPerRow, flags, { -1.0f, -1.0f }, -1.0f);

		for (auto& item : m_DirectoryItems)
		{
			ImGui::TableNextColumn();
			const std::filesystem::path& path = std::filesystem::relative(item, Application::Get().GetProjectPath());
		
			Ref<Texture2D> icon = nullptr;
			if (!item.has_extension())
				icon = m_FolderIcon;
			else if (TextureManager::IsValid(item))
				icon = TextureManager::GetTexture(item);
			else
				icon = m_FileIcon;
		
			DrawItem(item, icon);
		}

		// Select nothing if clicking in blank space
		if (ImGui::IsMouseDown(0) && ImGui::IsWindowHovered() || g_SelectedResourcePath.empty())
			g_SelectedResourcePath = std::filesystem::path();

		ImGui::EndTable();

		ImGui::EndChild();
	}

	void ProjectBrowserPanel::DrawProjectDirectoryView()
	{
		LOCUS_PROFILE_FUNCTION();

		ImGui::BeginChild("ProjectDirectoryView", { -1.0f, -1.0f });
		ImGui::EndChild();
	}

	void ProjectBrowserPanel::DrawItem(const std::filesystem::path& item, Ref<Texture2D> icon)
	{
		LOCUS_PROFILE_FUNCTION();

		std::string itemName = item.filename().string();
		std::string label = "##" + itemName;
		ImGui::PushID(itemName.c_str());

		ImVec4 buttonColor = LocusColors::Transparent;
		if (g_SelectedResourcePath == item)
			buttonColor = LocusColors::LightGrey;
		
		ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, { 0.0f, 0.0f });
		ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 0.0f);
		ImGui::PushStyleColor(ImGuiCol_Button, buttonColor);
		if (ImGui::ImageButton((ImTextureID)(uint64_t)icon->GetRendererID(), { m_IconSize, m_IconSize }, { 0, 1 }, { 1, 0 }))
			g_SelectedResourcePath = item.string();
		
		if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
		{
			if (!item.has_extension())
				SetDirectory(item);
		}
		
		ImGui::PushFont(ImGui::GetIO().Fonts->Fonts[1]);
		ImGui::Text(itemName.c_str());
		ImGui::PopFont();
		
		ImGui::PopID();
		ImGui::PopStyleColor();
		ImGui::PopStyleVar(2);
	}

	void ProjectBrowserPanel::SetDirectory(const std::filesystem::path& path)
	{
		m_CurrentDirectory = Application::Get().GetProjectPath() / path;

		m_DirectoryItems.clear();
		for (auto& item : std::filesystem::directory_iterator(m_CurrentDirectory))
		{
			std::filesystem::path itemPath = std::filesystem::relative(item.path(), Application::Get().GetProjectPath());
			if (TextureManager::IsValid(itemPath) || MaterialManager::IsValid(itemPath) || ModelManager::IsValid(itemPath) || item.is_directory())
				m_DirectoryItems.push_back(itemPath);
		}
	}

	void ProjectBrowserPanel::ReloadDirectory()
	{
		for (auto& item : std::filesystem::directory_iterator(m_CurrentDirectory))
		{
			std::filesystem::path itemPath = std::filesystem::relative(item.path(), Application::Get().GetProjectPath());
			std::string ext = itemPath.extension().string();
			if (ext == ".jpg" || ext == ".png" || ext == ".jpeg" || ext == ".lmat" || ext == ".obj" || ext == ".fbx" || ext == ".gltf")
			{
				if (std::find(m_DirectoryItems.begin(), m_DirectoryItems.end(), itemPath) == m_DirectoryItems.end())
					m_DirectoryItems.push_back(itemPath);
			}
		}
	}

	void ProjectBrowserPanel::ProcessCreateNewMaterialPopup()
	{
		if (m_OpenCreateNewMaterialPopup)
			ImGui::OpenPopup("Create New Material");
		if (ImGui::BeginPopup("Create New Material"))
		{
			ImGui::PushStyleColor(ImGuiCol_Button, LocusColors::Transparent);
			ImGui::PushStyleColor(ImGuiCol_ButtonHovered, LocusColors::Transparent);
			ImGui::PushStyleColor(ImGuiCol_ButtonActive, LocusColors::Transparent);
			ImGui::PushStyleColor(ImGuiCol_Border, LocusColors::Transparent);
			ImGui::PushStyleVar(ImGuiStyleVar_ButtonTextAlign, { 0, 0.5f });

			ImGui::Button("Material Name", { 130.0f, 0 });
			ImGui::SameLine();
			char newMaterialNameBuffer[256];
			memset(newMaterialNameBuffer, 0, sizeof(newMaterialNameBuffer));
			strcpy_s(newMaterialNameBuffer, sizeof(newMaterialNameBuffer), m_CreateNewMaterialName.c_str());
			ImGui::PushItemWidth(500.0f);
			if (ImGui::InputText("##Project Name", newMaterialNameBuffer, sizeof(newMaterialNameBuffer)))
				m_CreateNewMaterialName = std::string(newMaterialNameBuffer);
			

			ImGui::PopStyleColor(4);
			ImGui::PopStyleVar();
			
			ImGui::SameLine();
			if (ImGui::Button("Create Material"))
			{
				m_CreateNewMaterialName.append(".lmat");
				std::ofstream fout(m_CurrentDirectory / m_CreateNewMaterialName);
				fout << "Colors:\n  Albedo: [1, 1, 1, 1]\n  Metallic: 0.5\n  Roughness: 0.5\n  AO: 0.5\nTextures:\n  Albedo:\n  Normal:\n  Metallic:\n  Roughness:\n  AO:";
				fout.close();
				m_CreateNewMaterialName = std::string();
				m_OpenCreateNewMaterialPopup = false;
				ResourceManager::Rescan();
				ReloadDirectory();
			}
			ImGui::EndPopup();
		}
	}
}
