#include "SApch.h"
#include "SceneSerializer.h"

#include "SideA/Scene/Entity.h"
#include "SideA/Scene/Components.h"

#include <fstream>
#define YAML_CPP_STATIC_DEFINE
#include "yaml-cpp/yaml.h"

// Needed to decode and encode custom datatypes
namespace YAML {

	template<>
	struct convert<glm::vec3>
	{
		static Node encode(const glm::vec3& rhs)
		{
			Node node;
			node.push_back(rhs.x);
			node.push_back(rhs.y);
			node.push_back(rhs.z);
			node.SetStyle(EmitterStyle::Flow);
			return node;
		}

		static bool decode(const Node& node, glm::vec3& rhs)
		{
			if (!node.IsSequence() || node.size() != 3)
				return false;

			rhs.x = node[0].as<float>();
			rhs.y = node[1].as<float>();
			rhs.z = node[2].as<float>();
			return true;
		}
	};

	template<>
	struct convert<glm::vec4>
	{
		static Node encode(const glm::vec4& rhs)
		{
			Node node;
			node.push_back(rhs.x);
			node.push_back(rhs.y);
			node.push_back(rhs.z);
			node.push_back(rhs.w);
			node.SetStyle(EmitterStyle::Flow);
			return node;
		}

		static bool decode(const Node& node, glm::vec4& rhs)
		{
			if (!node.IsSequence() || node.size() != 4)
				return false;

			rhs.x = node[0].as<float>();
			rhs.y = node[1].as<float>();
			rhs.z = node[2].as<float>();
			rhs.w = node[3].as<float>();
			return true;
		}
	};

}

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
			out << YAML::Key << "Rotation" << YAML::Value << tc.GetRotationEuler();
			out << YAML::Key << "Scale" << YAML::Value << tc.Scale;
			out << YAML::EndMap; // End Transform Component
		}

		// --- Sprite Renderer Component --------------------------------------
		if (entity.HasComponent<SpriteRendererComponent>())
		{
			out << YAML::Key << "SpriteRendererComponent";

			out << YAML::BeginMap; // Sprite Renderer Component
			auto& color = entity.GetComponent<SpriteRendererComponent>().Color;
			out << YAML::Key << "Color" << YAML::Value << color;
			out << YAML::EndMap; // End Sprite Renderer Component
		}

		// --- Camera Component -----------------------------------------------
		if (entity.HasComponent<CameraComponent>())
		{
			out << YAML::Key << "CameraComponent";

			auto& cc = entity.GetComponent<CameraComponent>();
			auto& camera = entity.GetComponent<CameraComponent>().Camera;
			out << YAML::BeginMap; // Camera Component

			out << YAML::Key << "Camera" << YAML::Value;
			out << YAML::BeginMap; // Camera
			out << YAML::Key << "BackgroundColor" << YAML::Value << camera.GetBackgroundColor();
			out << YAML::Key << "ProjectionType" << YAML::Value << (int)camera.GetProjectionType();
			out << YAML::Key << "OrthographicSize" << YAML::Value << camera.GetOrthographicSize();
			out << YAML::Key << "OrthographicNearClip" << YAML::Value << camera.GetOrthographicNearClip();
			out << YAML::Key << "OrthographicFarClip" << YAML::Value << camera.GetOrthographicFarClip();
			out << YAML::Key << "PerspectiveFOV" << YAML::Value << camera.GetPerspectiveFOV();
			out << YAML::Key << "PerspectiveNearClip" << YAML::Value << camera.GetPerspectiveNearClip();
			out << YAML::Key << "PerspectiveFarClip" << YAML::Value << camera.GetPerspectiveFarClip();
			out << YAML::EndMap; // End Camera

			out << YAML::Key << "Primary" << YAML::Value << cc.Primary;
			out << YAML::Key << "FixedAspectRatio" << YAML::Value << cc.FixedAspectRatio;

			out << YAML::EndMap; // End Camera Component
		}

		out << YAML::EndMap; // End Entity
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
		YAML::Node data = YAML::LoadFile(path);
		if (!data["Scene"])
			return false;

		// Deserialize scene data
		std::string sceneName = data["Scene"].as<std::string>();
		SIDEA_CORE_TRACE("Deserializing scene '{0}'", sceneName);

		// Deserialize every entity data
		auto entities = data["Entities"];
		if (entities)
		{
			for (auto entity : entities)
			{
				// --- Entity uuid --------------------------------------------
				uint64_t uuid = entity["Entity"].as<uint64_t>();

				// --- Tag Component ------------------------------------------
				std::string name;
				auto tagComponent = entity["TagComponent"];
				if (tagComponent)
					name = tagComponent["Tag"].as<std::string>();

				SIDEA_CORE_TRACE("Deserializing Entity: {0}, ID: {1}", name, uuid);
				Entity deserializedEntity = m_Scene->CreateEntity(name);

				// --- Transform Component ------------------------------------
				auto transformComponent = entity["TransformComponent"];
				if (transformComponent)
				{
					auto& tc = deserializedEntity.GetComponent<TransformComponent>();
					tc.Translation = transformComponent["Translation"].as<glm::vec3>();
					tc.SetRotationEuler(transformComponent["Rotation"].as<glm::vec3>());
					tc.Scale = transformComponent["Scale"].as<glm::vec3>();
				}

				// --- Sprite Renderer Component ------------------------------
				auto spriteRendererComponent = entity["SpriteRendererComponent"];
				if (spriteRendererComponent)
				{
					auto& src = deserializedEntity.AddComponent<SpriteRendererComponent>();
					src.Color = spriteRendererComponent["Color"].as<glm::vec4>();
				}

				// --- Camera Component ------------------------------
				auto cameraComponent = entity["CameraComponent"];
				if (cameraComponent)
				{
					auto& cc = deserializedEntity.AddComponent<CameraComponent>();
					auto& camera = cc.Camera;
					auto& cameraProps = cameraComponent["Camera"];
					camera.SetBackgroundColor(cameraProps["BackgroundColor"].as<glm::vec4>());
					camera.SetProjectionType((SceneCamera::ProjectionType)cameraProps["ProjectionType"].as<int>());
					camera.SetOrthographicSize(cameraProps["OrthographicSize"].as<float>());
					camera.SetOrthographicNearClip(cameraProps["OrthographicNearClip"].as<float>());
					camera.SetOrthographicFarClip(cameraProps["OrthographicFarClip"].as<float>());
					camera.SetPerspectiveFOV(cameraProps["PerspectiveFOV"].as<float>());
					camera.SetPerspectiveNearClip(cameraProps["PerspectiveNearClip"].as<float>());
					camera.SetPerspectiveFarClip(cameraProps["PerspectiveFarClip"].as<float>());

					cc.Primary = cameraComponent["Primary"].as<bool>();
					cc.FixedAspectRatio = cameraComponent["FixedAspectRatio"].as<bool>();
				}
			}
		}

		return true;
	}

	bool SceneSerializer::DeserializeRunTime(const std::string& path)
	{
		// Not Implemented
		SIDEA_CORE_ASSERT(false, "Not implemented");
		return false;
	}

}