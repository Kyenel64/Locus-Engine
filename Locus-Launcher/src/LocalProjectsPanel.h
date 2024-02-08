#pragma once

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
		~LocalProjectsPanel();

		void OnImGuiRender();

	private:
		// Relaunches Locus-Editor with selected project.
		bool OpenProject(const std::string& projectPath, const std::string& projectName);

		// Creates a locus project in the given project path. 
		bool GenerateProject(const std::string& projectPath, const std::string& projectName);

		void ProcessCreateNewProjectPopup();
		void ProcessProjectDoesNotExistPopup();

	private:
		std::filesystem::path m_LocusEditorPath;
		std::shared_ptr<ProjectData> m_SelectedProject = nullptr;
		std::vector<std::shared_ptr<ProjectData>> m_ProjectList;

		std::string m_NewProjectName;
		std::string m_NewProjectPath;

		bool m_OpenCreateNewProjectPopup = false;
		bool m_OpenProjectDoesNotExistPopup = false;
	};
}