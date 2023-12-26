#include "LLpch.h"
#include "Serializer.h"

#define YAML_CPP_STATIC_DEFINE
#include <yaml-cpp/yaml.h>

#include "Core/Launcher.h"

namespace LocusLauncher
{
	void Serializer::SerializeProjects(const std::vector<std::shared_ptr<ProjectData>>& projectList)
	{
		YAML::Emitter out;
		out << YAML::BeginMap; // Scene
		// Array of Entities
		out << YAML::Key << "Projects" << YAML::Value << YAML::BeginSeq;

		for (auto projectData : projectList)
		{
			out << YAML::BeginMap;
			out << YAML::Key << "ProjectName" << YAML::Value << projectData->ProjectName;
			out << YAML::Key << "ProjectPath" << YAML::Value << projectData->ProjectPath.string();
			out << YAML::EndMap;
		}

		out << YAML::EndSeq;

		out << YAML::EndMap; // End Scene

		std::ofstream fout(std::filesystem::current_path() / "projects.config");
		fout << out.c_str();
	}

	void Serializer::DeserializeProjects(std::vector<std::shared_ptr<ProjectData>>& projectList)
	{
		std::string configPath = std::filesystem::current_path().string() + "/projects.config";
		if (!std::filesystem::exists(configPath))
			return;
		YAML::Node data = YAML::LoadFile(configPath);
		auto projects = data["Projects"];

		for (auto project : projects)
		{
			std::shared_ptr<ProjectData> projectData = std::make_shared<ProjectData>();
			projectData->ProjectName = project["ProjectName"].as<std::string>();
			projectData->ProjectPath = project["ProjectPath"].as<std::string>();

			projectList.push_back(projectData);
		}
	}
}