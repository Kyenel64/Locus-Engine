#include "LocalProjectsPanel.h"

#include <windows.h>
#include <shellapi.h>
#include <tchar.h>
#include <fstream>
#include <sstream>
#include <vector>

#include <imgui.h>

#include "Launcher.h"
#include "Colors.h"

namespace LocusLauncher
{
	ProjectData project1 = { "C:/Users/Kye/Desktop/TestGenProject", "TestGenProject" };
	ProjectData project2 = { "C:/Users/Kye/Desktop/SandboxProject", "SandboxProject" };
	ProjectData project3 = { "SampleProject", "Sandbox" };

	LocalProjectsPanel::LocalProjectsPanel()
	{
		m_ProjectList.push_back(project1);
		m_ProjectList.push_back(project2);
		m_ProjectList.push_back(project3);
	}

	void LocalProjectsPanel::OnImGuiRender()
	{
		m_LocusEditorPath = std::filesystem::current_path();
		ImGuiWindowFlags windowFlags = ImGuiWindowFlags_TabBarAlignLeft | ImGuiWindowFlags_DockedWindowBorder;
		ImGui::Begin("Local Projects", false, windowFlags);

		if (ImGui::Button("New Project", { 150.0f, 40.0f }))
		{

		}

		ImGui::SameLine();

		if (ImGui::Button("Import Project", { 150.0f, 40.0f }))
		{

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
				OpenProject(m_SelectedProject->ProjectPath.string(), m_SelectedProject->ProjectName);
				Launcher::Get().Close();
			}
		}
		ImGui::PopStyleColor();


		// --- Project list ---
		ImGui::PushStyleColor(ImGuiCol_ChildBg, Colors::DarkGrey);
		ImGui::BeginChild("Projects", { -1.0f, -1.0f }, true);
		ImGui::PopStyleColor();

		ImGui::PushStyleColor(ImGuiCol_Header, Colors::LightGrey);
		for (auto& projectData : m_ProjectList)
		{
			bool selected = m_SelectedProject ? m_SelectedProject->ProjectName == projectData.ProjectName : false;
			std::string label = "##" + projectData.ProjectName + " " + projectData.ProjectPath.string();
			ImVec2 textPos = ImGui::GetCursorScreenPos();

			if (ImGui::Selectable(label.c_str(), selected, ImGuiSelectableFlags_None, {0.0f, 50.0f}))
				m_SelectedProject = &projectData;

			if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(0))
			{
				if (m_SelectedProject)
				{
					OpenProject(m_SelectedProject->ProjectPath.string(), m_SelectedProject->ProjectName);
					Launcher::Get().Close();
				}
			}

			// Project info
			ImDrawList* drawList = ImGui::GetWindowDrawList();
			ImFont* boldFont = ImGui::GetIO().Fonts->Fonts[0];
			ImGui::PushFont(boldFont);
			drawList->AddText(textPos, ImGui::GetColorU32(Colors::White), projectData.ProjectName.c_str());
			ImGui::PopFont();
			drawList->AddText({ textPos.x, textPos.y + 25.0f }, ImGui::GetColorU32(Colors::White), projectData.ProjectPath.string().c_str());
		}
		ImGui::PopStyleColor();

		if (ImGui::IsMouseDown(0) && ImGui::IsWindowHovered())
			m_SelectedProject = nullptr;

		ImGui::EndChild();


		ImGui::End();
	}

	void LocalProjectsPanel::OpenProject(const std::string& projectPath, const std::string& projectName)
	{
		std::filesystem::path appPath = m_LocusEditorPath / "Locus-Editor.exe";

		std::string cla = projectPath + " " + projectName;
		std::wstring claWStr = std::wstring(cla.begin(), cla.end());
		ShellExecute(NULL, _T("open"), appPath.c_str(), claWStr.c_str(), NULL, SW_SHOWNORMAL);
	}

	void LocalProjectsPanel::GenerateProject(const std::string& projectPath, const std::string& projectName)
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

		// Replace ProjectName
		std::string projectNameFmt = "\"" + projectName + "\"\n";
		while (premakeTemplateStr.find("ProjectName") != std::string::npos)
			premakeTemplateStr.replace(premakeTemplateStr.find("ProjectName"), 12, projectNameFmt);

		// Replace LocusScriptLocation
		std::filesystem::path currentPath = m_LocusEditorPath / "resources/scripts/Locus-Script";
		std::string locusScriptLocationFmt = "\"" + currentPath.string() + "\"\n";
		std::replace(locusScriptLocationFmt.begin(), locusScriptLocationFmt.end(), '\\', '/');
		while (premakeTemplateStr.find("LocusScriptLocation") != std::string::npos)
			premakeTemplateStr.replace(premakeTemplateStr.find("LocusScriptLocation"), 20, locusScriptLocationFmt);

		// Write template to project path
		std::ofstream ofs(projectPath + "/premake5.lua");
		ofs << premakeTemplateStr;
		ofs.close();

		// Generate VS solution
		std::string cmd = "--file=" + projectPath + "/premake5.lua" + " " + "vs2022";
		std::wstring cmdWStr = std::wstring(cmd.begin(), cmd.end());
		std::string premakePath = m_LocusEditorPath.string() + "/projectGeneration/premake5.exe";
		std::wstring premakePathWStr = std::wstring(premakePath.begin(), premakePath.end());
		ShellExecute(NULL, _T("open"), premakePathWStr.c_str(), cmdWStr.c_str(), NULL, SW_SHOWNORMAL);

	}
}