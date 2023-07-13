#include "SApch.h"
#include "SceneSerializer.h"

#include "SideA/Scene/Entity.h"
#include "SideA/Scene/Components.h"

#include <fstream>
#define YAML_CPP_STATIC_DEFINE
#include "yaml-cpp/yaml.h"

namespace SideA
{
	YAML::Emitter& operator<<(YAML::Emitter& out, const glm::vec3& v)
	{
		out << YAML::Flow;
		out << YAML::BeginSeq << v.x << v.y << v.z << YAML::EndSeq;
		return out;
	}

	YAML::Emitter& operator<<(YAML::Emitter& out, const glm::vec4& v)
	{
		out << YAML::Flow;
		out << YAML::BeginSeq << v.x << v.y << v.z << v.w << YAML::EndSeq;
		return out;
	}

	SceneSerializer::SceneSerializer(const Ref<Scene>& scene)
		: m_Scene(scene)
	{
	}

	static void SerializeEntity(YAML::Emitter& out, Entity entity)
	{
		out << YAML::BeginMap; // Begin Entity
		out << YAML::Key << "Entity" << YAML::Value << "1283849483838";

		// --- Tag Component --------------------------------------------------
		if (entity.HasComponent<TagComponent>())
		{
			out << YAML::Key << "TagComponent";

			out << YAML::BeginMap; // Tag Component
			auto& tag = entity.GetComponent<TagComponent>().Tag;
			out << YAML::Key << "Tag" << YAML::Value << tag;
			out << YAML::EndMap; // End Tag Component
		}

		// --- Transform Component --------------------------------------------
		if (entity.HasComponent<TransformComponent>())
		{
			out << YAML::Key << "TransformComponent";

			out << YAML::BeginMap; // Transform Component
			auto& tc = entity.GetComponent<TransformComponent>();
			out << YAML::Key << "Translation" << YAML::Value << tc.Translation;
			out << YAML::Key << "Rotation" << YAML::Value << tc.Rotation;
			out << YAML::Key << "Scale" << YAML::Value << tc.Scale;
			out << YAML::EndMap; // End Transform Component
		}
	}

	void SceneSerializer::Serialize(const std::string& path)
	{
		YAML::Emitter out;
		out << YAML::BeginMap; // Scene
		out << YAML::Key << "Scene" << YAML::Value << "Untitled"; 

		// Array of Entities
		out << YAML::Key << "Entities" << YAML::Value << YAML::BeginSeq;
		m_Scene->m_Registry.each([&](auto entityID)
		{
				Entity entity = { entityID, m_Scene.get() };
				if (!entity)
					return;
				SerializeEntity(out, entity);
		});
		out << YAML::EndSeq;

		out << YAML::EndMap; // End Scene
		std::ofstream fout(path);
		fout << out.c_str();
	}

	void SceneSerializer::SerializeRunTime(const std::string& path)
	{
		// Not Implemented
		SIDEA_CORE_ASSERT(false, "Not implemented");
	}

	bool SceneSerializer::Deserialize(const std::string& path)
	{
		return true;
	}

	bool SceneSerializer::DeserializeRunTime(const std::string& path)
	{
		// Not Implemented
		SIDEA_CORE_ASSERT(false, "Not implemented");
		return false;
	}

}