#include "LocusLauncher.h"

#include <shellapi.h>
#include <tchar.h>
#include <filesystem>
#include <string>
#include <fstream>
#include <sstream>

#include <ImGui/imgui.h>

#include "LocusEditorLayer.h"

namespace Locus
{
	LocusLauncher::LocusLauncher()
		: Layer("LocusLauncher")
	{
		m_LocusEditorDirPath = std::filesystem::current_path();
	}

	void LocusLauncher::OnAttach()
	{
		
	}

	void LocusLauncher::OnDetach()
	{

	}

	void LocusLauncher::OnUpdate(Timestep deltaTime)
	{
		
	}

	void LocusLauncher::OnEvent(Event& event)
	{

	}

	void LocusLauncher::OnImGuiRender()
	{
		// --- Dockspace ---
		ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None;
		ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoScrollbar |
			ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove |
			ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus | ImGuiWindowFlags_NoScrollWithMouse;
		ImGuiViewport* viewport = ImGui::GetMainViewport();
		ImGui::SetNextWindowPos(viewport->Pos);
		ImGui::SetNextWindowSize(viewport->Size);
		ImGui::SetNextWindowViewport(viewport->ID);

		if (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode)
			window_flags |= ImGuiWindowFlags_NoBackground;

		ImGui::Begin("Launcher", false, window_flags);
		ImGui::End();


		if (Input::IsKeyPressed(Key::Space))
		{
			OpenProject("C:/Users/Kye/Desktop/TestGenProject", "TestGenProject");
			Application::Get().Close();
		}

		if (Input::IsKeyPressed(Key::Enter))
		{
			GenerateProject("C:/Users/Kye/Desktop/TestGenProject", "TestGenProject");
		}
	}

	void LocusLauncher::OpenProject(const std::string& projectPath, const std::string& projectName)
	{
		std::filesystem::path appPath = m_LocusEditorDirPath / "Locus-Editor.exe";

		std::string cla = projectPath + " " + projectName;
		std::wstring claWStr = std::wstring(cla.begin(), cla.end());
		ShellExecute(NULL, _T("open"), appPath.c_str(), claWStr.c_str(), NULL, SW_SHOWNORMAL);
	}

	void LocusLauncher::GenerateProject(const std::string& projectPath, const std::string& projectName)
	{
		// Generate directories
		std::filesystem::create_directory(projectPath);
		std::filesystem::create_directory(projectPath + "/Assets");

		// --- Generate premake ---
		// Read premakeTemplate file to string
		std::ifstream ifs(m_LocusEditorDirPath / "projectPremakeTemplate.lua");
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
		std::filesystem::path currentPath = m_LocusEditorDirPath / "resources/scripts/Locus-Script";
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
		std::string cmdFull = "start premake5.exe " + cmd;
		std::wstring cmdWStr = std::wstring(cmd.begin(), cmd.end());
		ShellExecute(NULL, _T("open"), _T("premake5.exe"), cmdWStr.c_str(), NULL, SW_SHOWNORMAL);

	}
}