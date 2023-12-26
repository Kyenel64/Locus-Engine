#include "LLpch.h"
#include "LocalProjectsPanel.h"

#include <imgui.h>

#include "Core/Launcher.h"
#include "Utils/Colors.h"
#include "Utils/FileDialog.h"
#include "Utils/Serializer.h"

namespace LocusLauncher
{
	LocalProjectsPanel::LocalProjectsPanel()
	{
		Serializer::DeserializeProjects(m_ProjectList);
		m_LocusEditorPath = std::filesystem::current_path();
	}

	LocalProjectsPanel::~LocalProjectsPanel()
	{
		Serializer::SerializeProjects(m_ProjectList);
	}

	void LocalProjectsPanel::OnImGuiRender()
	{
		ImGuiWindowFlags windowFlags = ImGuiWindowFlags_TabBarAlignLeft | ImGuiWindowFlags_DockedWindowBorder
			| ImGuiWindowFlags_NoMove;
		ImGui::Begin("Local Projects", false, windowFlags);

		// --- New Project button ---
		if (ImGui::Button("New Project", { 150.0f, 40.0f }))
			m_OpenCreateNewProjectPopup = true;
		ProcessCreateNewProjectPopup();

		ImGui::SameLine();

		// --- Import Project button ---
		if (ImGui::Button("Import Project", { 150.0f, 40.0f }))
		{
			// Select the visual studio solution file within the locus project 
			//	as the .sln file will have the same name as the project.
			std::filesystem::path path = Utils::OpenFile("VS Solution (*.sln)\0*.sln\0");
			if (!path.empty())
			{
				std::string projectName = path.filename().replace_extension().string();
				std::string projectPath = path.remove_filename().string();
				projectPath.pop_back();

				std::shared_ptr<ProjectData> projectData = std::make_shared<ProjectData>();
				projectData->ProjectName = projectName;
				projectData->ProjectPath = projectPath;
				m_ProjectList.push_back(projectData);
			}
		}

		ImGui::SameLine();

		// --- Open Project button ---
		ImVec4 buttonColor = ImGui::GetStyleColorVec4(ImGuiCol_Button);
		if (!m_SelectedProject)
			buttonColor = Colors::DarkGrey;
		ImGui::PushStyleColor(ImGuiCol_Button, buttonColor);
		if (ImGui::Button("Open Project", { 150.0f, 40.0f }))
		{
			if (m_SelectedProject)
			{
				if (OpenProject(m_SelectedProject->ProjectPath.string(), m_SelectedProject->ProjectName))
					Launcher::Get().Close();
			}
		}
		ImGui::PopStyleColor();


		// --- Project list ---
		ImGui::PushStyleColor(ImGuiCol_ChildBg, Colors::DarkGrey);
		ImGui::BeginChild("Projects", { -1.0f, -1.0f }, true);
		ImGui::PopStyleColor();

		bool removeProject = false;
		ImGui::PushStyleColor(ImGuiCol_Header, Colors::LightGrey);
		for (auto projectData : m_ProjectList)
		{
			bool selected = m_SelectedProject ? m_SelectedProject == projectData : false;
			std::string label = "##" + projectData->ProjectName + " " + projectData->ProjectPath.string();
			ImVec2 textPos = ImGui::GetCursorScreenPos();
			ImDrawList* drawList = ImGui::GetWindowDrawList();

			ImGui::PushID(projectData.get());
			if (ImGui::Selectable(label.c_str(), selected, ImGuiSelectableFlags_None, {0.0f, 50.0f}))
				m_SelectedProject = projectData;
			ImGui::PopID();

			// Project info
			ImFont* boldFont = ImGui::GetIO().Fonts->Fonts[0];
			ImGui::PushFont(boldFont);
			drawList->AddText(textPos, ImGui::GetColorU32(Colors::White), projectData->ProjectName.c_str());
			ImGui::PopFont();
			drawList->AddText({ textPos.x, textPos.y + 25.0f }, ImGui::GetColorU32(Colors::White), projectData->ProjectPath.string().c_str());
			drawList->AddLine({ ImGui::GetCursorScreenPos().x, ImGui::GetCursorScreenPos().y - ImGui::GetStyle().ItemSpacing.y / 2 },
				{ ImGui::GetCursorScreenPos().x + ImGui::GetItemRectSize().x, ImGui::GetCursorScreenPos().y - ImGui::GetStyle().ItemSpacing.y / 2 }, ImGui::GetColorU32(Colors::LightGrey));

			// Click events
			if (ImGui::IsItemHovered())
			{
				if (ImGui::IsMouseDoubleClicked(0))
				{
					if (m_SelectedProject)
					{
						if (OpenProject(m_SelectedProject->ProjectPath.string(), m_SelectedProject->ProjectName))
							Launcher::Get().Close();
					}
				}
				if (ImGui::IsMouseClicked(ImGuiMouseButton_Right))
				{
					m_SelectedProject = projectData;
					ImGui::OpenPopup("ProjectSettingsPopup");
				}
			}

			// Project settings popup
			if (m_SelectedProject == projectData)
			{
				if (ImGui::BeginPopup("ProjectSettingsPopup"))
				{
					if (ImGui::MenuItem("Remove from list"))
						removeProject = true;
					ImGui::EndPopup();
				}
			}
		}
		ImGui::PopStyleColor();

		if (removeProject)
			m_ProjectList.erase(std::find(m_ProjectList.begin(), m_ProjectList.end(), m_SelectedProject));

		if (ImGui::IsMouseDown(0) && ImGui::IsWindowHovered())
			m_SelectedProject = nullptr;

		ImGui::EndChild();

		ProcessProjectDoesNotExistPopup();
		ImGui::End();
	}

	bool LocalProjectsPanel::OpenProject(const std::string& projectPath, const std::string& projectName)
	{
		if (!std::filesystem::exists(projectPath))
		{
			m_OpenProjectDoesNotExistPopup = true;
			return false;
		}

		// Run Locus-Editor with project path
		std::filesystem::path appPath = m_LocusEditorPath / "Locus-Editor.exe";
		std::string cla = projectPath + " " + projectName;
		std::wstring claWStr = std::wstring(cla.begin(), cla.end());
		ShellExecute(NULL, _T("open"), appPath.c_str(), claWStr.c_str(), NULL, SW_SHOWNORMAL);

		return true;
	}

	bool LocalProjectsPanel::GenerateProject(const std::string& projectPath, const std::string& projectName)
	{
		// Generate directories
		std::filesystem::create_directory(projectPath);
		std::filesystem::create_directory(projectPath + "/Assets");

		// --- Generate premake ---
		// Read premakeTemplate file to string
		std::ifstream ifs(m_LocusEditorPath / "projectGeneration" / "projectPremakeTemplate.lua");
		std::stringstream premakeTemplateStream = {};
		if (ifs.is_open())
			premakeTemplateStream << ifs.rdbuf();
		ifs.close();
		std::string premakeTemplateStr = premakeTemplateStream.str();

		// Replace ProjectName in the premake template string.
		std::string projectNameFmt = "\"" + projectName + "\"\n";
		while (premakeTemplateStr.find("ProjectName") != std::string::npos)
			premakeTemplateStr.replace(premakeTemplateStr.find("ProjectName"), 12, projectNameFmt);

		// Replace LocusScriptLocation in the premake template string.
		std::filesystem::path currentPath = m_LocusEditorPath / "resources/scripts/Locus-Script";
		std::string locusScriptLocationFmt = "\"" + currentPath.string() + "\"\n";
		std::replace(locusScriptLocationFmt.begin(), locusScriptLocationFmt.end(), '\\', '/');
		while (premakeTemplateStr.find("LocusScriptLocation") != std::string::npos)
			premakeTemplateStr.replace(premakeTemplateStr.find("LocusScriptLocation"), 20, locusScriptLocationFmt);

		// Write the modified template file to the project path. 
		std::ofstream ofs(projectPath + "/premake5.lua");
		ofs << premakeTemplateStr;
		ofs.close();

		// Generate VS solution with premake
		std::string cmd = "--file=" + projectPath + "/premake5.lua" + " " + "vs2022";
		std::wstring cmdWStr = std::wstring(cmd.begin(), cmd.end());
		std::string premakePath = m_LocusEditorPath.string() + "/projectGeneration/premake5.exe";
		std::wstring premakePathWStr = std::wstring(premakePath.begin(), premakePath.end());
		ShellExecute(NULL, _T("open"), premakePathWStr.c_str(), cmdWStr.c_str(), NULL, SW_SHOWNORMAL);

		// Add project info to project list.
		std::shared_ptr<ProjectData> projectData = std::make_shared<ProjectData>();
		projectData->ProjectName = projectName;
		projectData->ProjectPath = projectPath;
		m_ProjectList.push_back(projectData);

		return true;
	}

	void LocalProjectsPanel::ProcessCreateNewProjectPopup()
	{
		if (m_OpenCreateNewProjectPopup)
			ImGui::OpenPopup("Create New Project");
		ImVec2 center = { ImGui::GetWindowPos().x + Launcher::Get().GetWindow().GetWidth() / 2, ImGui::GetWindowPos().y + Launcher::Get().GetWindow().GetHeight() / 2 };
		ImGui::SetNextWindowPos(center, ImGuiCond_None, { 0.5f, 0.5f });
		if (ImGui::BeginPopupModal("Create New Project", &m_OpenCreateNewProjectPopup, ImGuiWindowFlags_AlwaysAutoResize))
		{
			ImGui::PushStyleColor(ImGuiCol_Button, Colors::Transparent);
			ImGui::PushStyleColor(ImGuiCol_ButtonHovered, Colors::Transparent);
			ImGui::PushStyleColor(ImGuiCol_ButtonActive, Colors::Transparent);
			ImGui::PushStyleColor(ImGuiCol_Border, Colors::Transparent);
			ImGui::PushStyleVar(ImGuiStyleVar_ButtonTextAlign, { 0, 0.5f });

			// Project Name
			ImGui::Button("Project Name", { 130.0f, 0 });
			ImGui::SameLine();
			char projectNameBuffer[256];
			memset(projectNameBuffer, 0, sizeof(projectNameBuffer));
			strcpy_s(projectNameBuffer, sizeof(projectNameBuffer), m_NewProjectName.c_str());
			ImGui::PushItemWidth(500.0f);
			if (ImGui::InputText("##Project Name", projectNameBuffer, sizeof(projectNameBuffer)))
				m_NewProjectName = std::string(projectNameBuffer);

			// Project Path
			ImGui::Button("Project Path", { 130.0f, 0 });
			ImGui::SameLine();
			char projectPathBuffer[256];
			memset(projectPathBuffer, 0, sizeof(projectPathBuffer));
			strcpy_s(projectPathBuffer, sizeof(projectPathBuffer), m_NewProjectPath.c_str());
			ImGui::PushItemWidth(500.0f);
			if (ImGui::InputText("##Project Path", projectPathBuffer, sizeof(projectPathBuffer)))
				m_NewProjectPath = std::string(projectPathBuffer);

			ImGui::PopStyleVar();
			ImGui::PopStyleColor(4);

			ImGui::SameLine();

			// Open windows folder dialog window.
			if (ImGui::Button("Browse"))
				m_NewProjectPath = Utils::SelectPath();

			std::string projectPath = m_NewProjectPath + "/" + m_NewProjectName;

			ImGui::Text("Project will be generated in: %s", projectPath.c_str());

			// Create and Open button
			ImGui::SetCursorPosX(ImGui::GetWindowSize().x / 2 - 150.0f - ImGui::GetStyle().ItemSpacing.x / 2);
			if (ImGui::Button("Create and Open", { 150.0f, 40.0f }))
			{
				GenerateProject(projectPath, m_NewProjectName);
				OpenProject(projectPath, m_NewProjectName);
				Launcher::Get().Close();
			}

			ImGui::SameLine();

			// Cancel button
			if (ImGui::Button("Cancel", { 150.0f, 40.0f }))
				m_OpenCreateNewProjectPopup = false;

			ImGui::EndPopup();
		}
	}

	void LocalProjectsPanel::ProcessProjectDoesNotExistPopup()
	{
		if (m_OpenProjectDoesNotExistPopup)
			ImGui::OpenPopup("Project doesn't exist");
		ImVec2 center = { ImGui::GetWindowPos().x + Launcher::Get().GetWindow().GetWidth() / 2, ImGui::GetWindowPos().y + Launcher::Get().GetWindow().GetHeight() / 2 };
		ImGui::SetNextWindowPos(center, ImGuiCond_None, { 0.5f, 0.5f });
		if (ImGui::BeginPopupModal("Project doesn't exist", &m_OpenProjectDoesNotExistPopup, ImGuiWindowFlags_AlwaysAutoResize))
		{
			ImGui::Text("Could not locate project directory.");
			
			if (ImGui::Button("Remove from list", { 150.0f, 40.0f }))
			{
				if (m_SelectedProject)
				{
					m_ProjectList.erase(std::find(m_ProjectList.begin(), m_ProjectList.end(), m_SelectedProject));
					m_OpenProjectDoesNotExistPopup = false;
				}
			}

			ImGui::SameLine();

			if (ImGui::Button("Cancel", { 150.0f, 40.0f }))
				m_OpenProjectDoesNotExistPopup = false;

			ImGui::EndPopup();
		}
	}
}