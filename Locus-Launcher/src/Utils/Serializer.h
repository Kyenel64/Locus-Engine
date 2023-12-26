#pragma once

namespace LocusLauncher
{
	struct ProjectData;

	class Serializer
	{
	public:
		Serializer() = default;
		~Serializer() = default;

		static void SerializeProjects(const std::vector<std::shared_ptr<ProjectData>>& projectList);
		static void DeserializeProjects(std::vector<std::shared_ptr<ProjectData>>& projectList);
	};
}