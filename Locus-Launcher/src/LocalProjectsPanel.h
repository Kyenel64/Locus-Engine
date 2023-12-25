#pragma once

#include <string>
#include <filesystem>

namespace LocusLauncher
{
	struct ProjectData
	{
		std::filesystem::path ProjectPath;
		std::string ProjectName;
	};

	class LocalProjectsPanel
	{
	public:
		LocalProjectsPanel();
		~LocalProjectsPanel() = default;

		void OnImGuiRender();

	private:
		void OpenProject(const std::string& projectPath, const std::string& projectName);
		void GenerateProject(const std::string& projectPath, const std::string& projectName);

	private:
		std::filesystem::path m_LocusEditorPath;
		ProjectData* m_SelectedProject = nullptr;
		std::vector<ProjectData> m_ProjectList;
	};
}