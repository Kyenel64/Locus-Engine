#include "Lpch.h"
#include "SceneSerializer.h"

#include <fstream>

#define YAML_CPP_STATIC_DEFINE
#include <yaml-cpp/yaml.h>

#include "Locus/Scene/Entity.h"
#include "Locus/Scene/Components.h"

// Needed to decode and encode custom datatypes
namespace YAML {

	template<>
	struct convert<glm::vec2>
	{
		static Node encode(const glm::vec2& rhs)
		{
			Node node;
			node.push_back(rhs.x);
			node.push_back(rhs.y);
			node.SetStyle(EmitterStyle::Flow);
			return node;
		}

		static bool decode(const Node& node, glm::vec2& rhs)
		{
			if (!node.IsSequence() || node.size() != 2)
				return false;

			rhs.x = node[0].as<float>();
			rhs.y = node[1].as<float>();
			return true;
		}
	};

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

namespace Locus
{
	YAML::Emitter& operator<<(YAML::Emitter& out, const glm::vec2& v)
	{
		out << YAML::Flow;
		out << YAML::BeginSeq << v.x << v.y << YAML::EndSeq;
		return out;
	}

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

	void SceneSerializer::SerializeEntity(YAML::Emitter& out, Entity entity)
	{
		LOCUS_CORE_ASSERT(entity.HasComponent<IDComponent>(), "Entity does not have ID!");
		out << YAML::BeginMap; // Begin Entity
		out << YAML::Key << "Entity" << YAML::Value << entity.GetUUID();

		// --- Tag Component --------------------------------------------------
		if (entity.HasComponent<TagComponent>())
		{
			out << YAML::Key << "TagComponent";

			out << YAML::BeginMap; // Tag Component
			auto& tag = entity.GetComponent<TagComponent>();
			out << YAML::Key << "Tag" << YAML::Value << tag.Tag;
			out << YAML::Key << "Enabled" << YAML::Value << tag.Enabled;
			out << YAML::EndMap; // End Tag Component
		}

		// --- Transform Component --------------------------------------------
		if (entity.HasComponent<TransformComponent>())
		{
			out << YAML::Key << "TransformComponent";

			out << YAML::BeginMap; // Transform Component
			auto& tc = entity.GetComponent<TransformComponent>();
			out << YAML::Key << "Self" << YAML::Value << tc.Self.GetComponent<IDComponent>().ID;
			if (tc.Parent.IsValid())
				out << YAML::Key << "Parent" << YAML::Value << tc.Parent.GetComponent<IDComponent>().ID;
			else
				out << YAML::Key << "Parent" << YAML::Value << 0;
			out << YAML::Key << "LocalPosition" << YAML::Value << tc.LocalPosition;
			out << YAML::Key << "LocalRotation" << YAML::Value << tc.LocalRotation;
			out << YAML::Key << "LocalScale" << YAML::Value << tc.LocalScale;
			out << YAML::EndMap; // End Transform Component
		}

		// --- Child Component ------------------------------------------------
		if (entity.HasComponent<ChildComponent>())
		{
			auto& cc = entity.GetComponent<ChildComponent>();
			out << YAML::Key << "ChildComponent";
			out << YAML::BeginMap; // Child Component
			out << YAML::Key << "ChildCount" << YAML::Value << cc.ChildCount;

			out << YAML::Key << "ChildEntities";
			out << YAML::BeginMap; // Child Entities
			for (uint32_t i = 0; i < cc.ChildCount; i++)
			{
				std::string childWithVal = "Child" + std::to_string(i);
				out << YAML::Key << childWithVal << YAML::Value << cc.ChildEntities[i].GetComponent<IDComponent>().ID;
			}
			out << YAML::EndMap; // End Child Entities

			out << YAML::EndMap; // End Child Component
		}

		// --- Sprite Renderer Component --------------------------------------
		if (entity.HasComponent<SpriteRendererComponent>())
		{
			auto& src = entity.GetComponent<SpriteRendererComponent>();
			out << YAML::Key << "SpriteRendererComponent";

			out << YAML::BeginMap; // Sprite Renderer Component
			out << YAML::Key << "Color" << YAML::Value << src.Color;
			out << YAML::Key << "TexturePath" << YAML::Value << src.TexturePath;
			out << YAML::Key << "TilingFactor" << YAML::Value << src.TilingFactor;
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

		// --- Rigidbody2D Component ------------------------------------------
		if (entity.HasComponent<Rigidbody2DComponent>())
		{
			out << YAML::Key << "Rigidbody2DComponent";
			auto& rb2D = entity.GetComponent<Rigidbody2DComponent>();

			out << YAML::BeginMap; // Rigidbody2D Component
			out << YAML::Key << "BodyType" << YAML::Value << (int)rb2D.BodyType;
			out << YAML::Key << "Mass" << YAML::Value << rb2D.Mass;
			out << YAML::Key << "GravityScale" << YAML::Value << rb2D.GravityScale;
			out << YAML::Key << "LinearDrag" << YAML::Value << rb2D.LinearDrag;
			out << YAML::Key << "AngularDrag" << YAML::Value << rb2D.AngularDrag;
			out << YAML::Key << "FixedRotation" << YAML::Value << rb2D.FixedRotation;
			out << YAML::Key << "Friction" << YAML::Value << rb2D.Friction;
			out << YAML::Key << "Restitution" << YAML::Value << rb2D.Restitution;
			out << YAML::Key << "RestitutionThreshold" << YAML::Value << rb2D.RestitutionThreshold;
			out << YAML::EndMap; // End Rigidbody2D Component
		}

		// --- BoxCollider2D Component ----------------------------------------
		if (entity.HasComponent<BoxCollider2DComponent>())
		{
			out << YAML::Key << "BoxCollider2DComponent";
			auto& bc2D = entity.GetComponent<BoxCollider2DComponent>();

			out << YAML::BeginMap; // BoxCollider2D Component
			out << YAML::Key << "CollisionLayer" << YAML::Value << bc2D.CollisionLayer;
			out << YAML::Key << "Offset" << YAML::Value << bc2D.Offset;
			out << YAML::Key << "Size" << YAML::Value << bc2D.Size;
			out << YAML::EndMap; // BoxCollider2D Component
		}

		out << YAML::EndMap; // End Entity
	}

	void SceneSerializer::Serialize(const std::string& path)
	{
		YAML::Emitter out;
		out << YAML::BeginMap; // Scene
		out << YAML::Key << "Scene" << YAML::Value << m_Scene->GetSceneName(); 

		// Array of Entities
		out << YAML::Key << "Entities" << YAML::Value << YAML::BeginSeq;
		m_Scene->m_Registry.each([&](auto entityID)
		{
				Entity entity = Entity(entityID, m_Scene.get());
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
		LOCUS_CORE_ASSERT(false, "Not implemented");
	}

	bool SceneSerializer::Deserialize(const std::string& path)
	{
		YAML::Node data = YAML::LoadFile(path);
		if (!data["Scene"])
			return false;

		// Deserialize scene data
		std::string sceneName = data["Scene"].as<std::string>();
		m_Scene->SetSceneName(sceneName);
		LOCUS_CORE_TRACE("Deserializing scene '{0}'", sceneName);

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
				bool enabled = true;
				auto tagComponent = entity["TagComponent"];
				if (tagComponent)
				{
					name = tagComponent["Tag"].as<std::string>();
					enabled = tagComponent["Enabled"].as<bool>();
				}

				LOCUS_CORE_TRACE("Deserializing Entity: {0}, ID: {1}", name, uuid);
				Entity deserializedEntity = m_Scene->CreateEntityWithUUID(uuid, name, enabled);

				// --- Transform Component ------------------------------------
				auto transformComponent = entity["TransformComponent"];
				if (transformComponent)
				{
					auto& tc = deserializedEntity.GetComponent<TransformComponent>();
					tc.Self = deserializedEntity;
				}

				// --- Sprite Renderer Component ------------------------------
				auto spriteRendererComponent = entity["SpriteRendererComponent"];
				if (spriteRendererComponent)
				{
					auto& src = deserializedEntity.AddComponent<SpriteRendererComponent>();
					src.Color = spriteRendererComponent["Color"].as<glm::vec4>();
					src.TexturePath = spriteRendererComponent["TexturePath"].as<std::string>();
					if (src.TexturePath != std::string())
						src.Texture = Texture2D::Create(src.TexturePath);
					src.TilingFactor = spriteRendererComponent["TilingFactor"].as<float>();
				}

				// --- Camera Component ---------------------------------------
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

				// --- Rigidbody2D Component ----------------------------------
				auto rigidBody2DComponent = entity["Rigidbody2DComponent"];
				if (rigidBody2DComponent)
				{
					auto& rb2D = deserializedEntity.AddComponent<Rigidbody2DComponent>();
					rb2D.BodyType = (Rigidbody2DComponent::Rigidbody2DType)rigidBody2DComponent["BodyType"].as<int>();
					rb2D.Mass = rigidBody2DComponent["Mass"].as<float>();
					rb2D.GravityScale = rigidBody2DComponent["GravityScale"].as<float>();
					rb2D.LinearDrag = rigidBody2DComponent["LinearDrag"].as<float>();
					rb2D.AngularDrag = rigidBody2DComponent["AngularDrag"].as<float>();
					rb2D.FixedRotation = rigidBody2DComponent["FixedRotation"].as<bool>();
					rb2D.Friction = rigidBody2DComponent["Friction"].as<float>();
					rb2D.Restitution = rigidBody2DComponent["Restitution"].as<float>();
					rb2D.RestitutionThreshold = rigidBody2DComponent["RestitutionThreshold"].as<float>();
				}

				// --- BoxCollider2D Component --------------------------------
				auto boxCollider2DComponent = entity["BoxCollider2DComponent"];
				if (boxCollider2DComponent)
				{
					auto& bc2D = deserializedEntity.AddComponent<BoxCollider2DComponent>();
					bc2D.CollisionLayer = boxCollider2DComponent["CollisionLayer"].as<int>();
					bc2D.Offset = boxCollider2DComponent["Offset"].as<glm::vec2>();
					bc2D.Size = boxCollider2DComponent["Size"].as<glm::vec2>();
				}
			}

			// Process relationships after all entities are created
			for (auto e : entities)
			{
				uint64_t uuid = e["Entity"].as<uint64_t>();
				Entity entity = m_Scene->GetEntityByUUID(uuid);

				// --- Transform Component ------------------------------------
				auto transformComponent = e["TransformComponent"];
				if (transformComponent)
				{
					auto& tc = entity.GetComponent<TransformComponent>();
					if (transformComponent["Parent"].as<uint64_t>() != 0)
					{
						Entity parent = m_Scene->GetEntityByUUID(transformComponent["Parent"].as<uint64_t>());
						tc.Parent = parent;
					}
					tc.LocalPosition = transformComponent["LocalPosition"].as<glm::vec3>();
					tc.LocalRotation = transformComponent["LocalRotation"].as<glm::vec3>();
					tc.LocalRotationQuat = glm::quat(tc.LocalRotation);
					tc.LocalScale = transformComponent["LocalScale"].as<glm::vec3>();
				}

				// --- Child Component ----------------------------------------
				auto childComponent = e["ChildComponent"];
				if (childComponent)
				{
					auto& cc = entity.AddComponent<ChildComponent>();
					cc.ChildCount = childComponent["ChildCount"].as<uint32_t>();
					auto childEntities = childComponent["ChildEntities"];
					for (uint32_t i = 0; i < cc.ChildCount; i++)
					{
						std::string childWithVal = "Child" + std::to_string(i);
						UUID uuid = childEntities[childWithVal].as<uint64_t>();
						Entity child = m_Scene->GetEntityByUUID(uuid);
						cc.ChildEntities.push_back(child);
					}
				}
				
			}
		}

		return true;
	}

	bool SceneSerializer::DeserializeRunTime(const std::string& path)
	{
		// Not Implemented
		LOCUS_CORE_ASSERT(false, "Not implemented");
		return false;
	}

}