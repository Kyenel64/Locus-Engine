#include "Lpch.h"
#include "SceneSerializer.h"

#include <fstream>

#define YAML_CPP_STATIC_DEFINE
#include <yaml-cpp/yaml.h>

#include "Locus/Scene/Entity.h"
#include "Locus/Scene/Components.h"
#include "Locus/Scripting/ScriptEngine.h"

// Needed to decode and encode custom datatypes using YAML
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

	template<>
	struct convert<glm::quat>
	{
		static Node encode(const glm::quat& rhs)
		{
			Node node;
			node.push_back(rhs.x);
			node.push_back(rhs.y);
			node.push_back(rhs.z);
			node.push_back(rhs.w);
			node.SetStyle(EmitterStyle::Flow);
			return node;
		}

		static bool decode(const Node& node, glm::quat& rhs)
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

	YAML::Emitter& operator<<(YAML::Emitter& out, const glm::quat& v)
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

		// --- Tag Component ---
		if (entity.HasComponent<TagComponent>())
		{
			auto& tag = entity.GetComponent<TagComponent>();

			out << YAML::Key << "TagComponent";
			out << YAML::BeginMap; // Tag Component
			out << YAML::Key << "Tag" << YAML::Value << tag.Tag;
			out << YAML::Key << "Group" << YAML::Value << tag.Group;
			out << YAML::Key << "Enabled" << YAML::Value << tag.Enabled;
			out << YAML::EndMap; // End Tag Component
		}

		// --- Transform Component ---
		if (entity.HasComponent<TransformComponent>())
		{
			auto& tc = entity.GetComponent<TransformComponent>();

			out << YAML::Key << "TransformComponent";
			out << YAML::BeginMap; // Transform Component
			out << YAML::Key << "Self" << YAML::Value << tc.Self;
			if (tc.Parent)
				out << YAML::Key << "Parent" << YAML::Value << tc.Parent;
			else
				out << YAML::Key << "Parent" << YAML::Value << 0;
			out << YAML::Key << "LocalPosition" << YAML::Value << tc.LocalPosition;
			out << YAML::Key << "LocalRotation" << YAML::Value << tc.LocalRotation;
			out << YAML::Key << "LocalRotationQuat" << YAML::Value << tc.LocalRotationQuat;
			out << YAML::Key << "LocalScale" << YAML::Value << tc.LocalScale;
			out << YAML::EndMap; // End Transform Component
		}

		// --- Child Component ---
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
				out << YAML::Key << childWithVal << YAML::Value << cc.ChildEntities[i];
			}
			out << YAML::EndMap; // End Child Entities
			out << YAML::EndMap; // End Child Component
		}

		// --- Sprite Renderer Component ---
		if (entity.HasComponent<SpriteRendererComponent>())
		{
			auto& src = entity.GetComponent<SpriteRendererComponent>();

			out << YAML::Key << "SpriteRendererComponent";
			out << YAML::BeginMap; // Sprite Renderer Component
			out << YAML::Key << "Texture" << YAML::Value << (uint64_t)src.Texture;
			out << YAML::Key << "Color" << YAML::Value << src.Color;
			out << YAML::Key << "TilingFactor" << YAML::Value << src.TilingFactor;
			out << YAML::EndMap; // End Sprite Renderer Component
		}

		// --- Circle Renderer Component ---
		if (entity.HasComponent<CircleRendererComponent>())
		{
			auto& crc = entity.GetComponent<CircleRendererComponent>();

			out << YAML::Key << "CircleRendererComponent";
			out << YAML::BeginMap; // Circle Renderer Component
			out << YAML::Key << "Color" << YAML::Value << crc.Color;
			out << YAML::Key << "Thickness" << YAML::Value << crc.Thickness;
			out << YAML::Key << "Fade" << YAML::Value << crc.Fade;
			out << YAML::EndMap;
		}

		// --- Cube Renderer Component ---
		if (entity.HasComponent<CubeRendererComponent>())
		{
			auto& crc = entity.GetComponent<CubeRendererComponent>();

			out << YAML::Key << "CubeRendererComponent";
			out << YAML::BeginMap; // Cube Renderer Component
			out << YAML::Key << "Material" << YAML::Value << (uint64_t)crc.Material;
			out << YAML::EndMap;
		}

		// --- Point Light Component ---
		if (entity.HasComponent<PointLightComponent>())
		{
			auto& plc = entity.GetComponent<PointLightComponent>();

			out << YAML::Key << "PointLightComponent";
			out << YAML::BeginMap; // Point Light Component
			out << YAML::Key << "Color" << YAML::Value << plc.Color;
			out << YAML::Key << "Intensity" << YAML::Value << plc.Intensity;
			out << YAML::EndMap;
		}

		// --- Directional Light Component ---
		if (entity.HasComponent<DirectionalLightComponent>())
		{
			auto& dlc = entity.GetComponent<DirectionalLightComponent>();

			out << YAML::Key << "DirectionalLightComponent";
			out << YAML::BeginMap; // Directional Light Component
			out << YAML::Key << "Color" << YAML::Value << dlc.Color;
			out << YAML::Key << "Intensity" << YAML::Value << dlc.Intensity;
			out << YAML::EndMap;
		}

		// --- Spot Light Component ---
		if (entity.HasComponent<SpotLightComponent>())
		{
			auto& slc = entity.GetComponent<SpotLightComponent>();

			out << YAML::Key << "SpotLightComponent";
			out << YAML::BeginMap; // Spot Light Component
			out << YAML::Key << "Color" << YAML::Value << slc.Color;
			out << YAML::Key << "Intensity" << YAML::Value << slc.Intensity;
			out << YAML::Key << "CutOff" << YAML::Value << slc.CutOff;
			out << YAML::Key << "OuterCutOff" << YAML::Value << slc.OuterCutOff;
			out << YAML::EndMap;
		}

		// --- Camera Component ---
		if (entity.HasComponent<CameraComponent>())
		{
			auto& cc = entity.GetComponent<CameraComponent>();
			auto& camera = entity.GetComponent<CameraComponent>().Camera;

			out << YAML::Key << "CameraComponent";
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

		// --- Rigidbody2D Component ---
		if (entity.HasComponent<Rigidbody2DComponent>())
		{
			auto& rb2D = entity.GetComponent<Rigidbody2DComponent>();

			out << YAML::Key << "Rigidbody2DComponent";
			out << YAML::BeginMap; // Rigidbody2D Component
			out << YAML::Key << "BodyType" << YAML::Value << (int)rb2D.BodyType;
			out << YAML::Key << "Mass" << YAML::Value << rb2D.Mass;
			out << YAML::Key << "GravityScale" << YAML::Value << rb2D.GravityScale;
			out << YAML::Key << "LinearDamping" << YAML::Value << rb2D.LinearDamping;
			out << YAML::Key << "AngularDamping" << YAML::Value << rb2D.AngularDamping;
			out << YAML::Key << "FixedRotation" << YAML::Value << rb2D.FixedRotation;
			out << YAML::Key << "IsBullet" << YAML::Value << rb2D.IsBullet;
			out << YAML::EndMap; // End Rigidbody2D Component
		}

		// --- BoxCollider2D Component ---
		if (entity.HasComponent<BoxCollider2DComponent>())
		{
			auto& bc2D = entity.GetComponent<BoxCollider2DComponent>();

			out << YAML::Key << "BoxCollider2DComponent";
			out << YAML::BeginMap; // BoxCollider2D Component
			out << YAML::Key << "Friction" << YAML::Value << bc2D.Friction;
			out << YAML::Key << "Restitution" << YAML::Value << bc2D.Restitution;
			out << YAML::Key << "RestitutionThreshold" << YAML::Value << bc2D.RestitutionThreshold;
			out << YAML::Key << "CollisionCategory" << YAML::Value << bc2D.CollisionCategory;
			out << YAML::Key << "CollisionMask" << YAML::Value << bc2D.CollisionMask;
			out << YAML::Key << "Offset" << YAML::Value << bc2D.Offset;
			out << YAML::Key << "Size" << YAML::Value << bc2D.Size;
			out << YAML::EndMap; // BoxCollider2D Component
		}

		// --- CircleCollider2D Component ---
		if (entity.HasComponent<CircleCollider2DComponent>())
		{
			auto& c2D = entity.GetComponent<CircleCollider2DComponent>();

			out << YAML::Key << "CircleCollider2DComponent";
			out << YAML::BeginMap; // BoxCollider2D Component
			out << YAML::Key << "Friction" << YAML::Value << c2D.Friction;
			out << YAML::Key << "Restitution" << YAML::Value << c2D.Restitution;
			out << YAML::Key << "RestitutionThreshold" << YAML::Value << c2D.RestitutionThreshold;
			out << YAML::Key << "CollisionCategory" << YAML::Value << c2D.CollisionCategory;
			out << YAML::Key << "CollisionMask" << YAML::Value << c2D.CollisionMask;
			out << YAML::Key << "Radius" << YAML::Value << c2D.Radius;
			out << YAML::Key << "Offset" << YAML::Value << c2D.Offset;
			out << YAML::EndMap; // BoxCollider2D Component
		}

		// --- Script Component ---
		if (entity.HasComponent<ScriptComponent>())
		{
			auto& sc = entity.GetComponent<ScriptComponent>();

			out << YAML::Key << "ScriptComponent";
			out << YAML::BeginMap;
			out << YAML::Key << "ScriptClass" << YAML::Value << sc.ScriptClass;

			// Fields
			auto& fieldInstances = ScriptEngine::GetFieldInstances(entity.GetUUID());
			out << YAML::Key << "Fields";
			out << YAML::BeginMap;
			for (auto& [name, field] : fieldInstances)
			{
				switch (field.Field.Type)
				{
				case FieldType::SystemSingle: out << YAML::Key << name << YAML::Value << field.GetValue<float>(); break;
				case FieldType::SystemDouble: out << YAML::Key << name << YAML::Value << field.GetValue<double>(); break;
				case FieldType::SystemShort: out << YAML::Key << name << YAML::Value << field.GetValue<int16_t>(); break;
				case FieldType::SystemInt: out << YAML::Key << name << YAML::Value << field.GetValue<int>(); break;
				case FieldType::SystemLong: out << YAML::Key << name << YAML::Value << field.GetValue<int64_t>(); break;
				case FieldType::SystemUShort: out << YAML::Key << name << YAML::Value << field.GetValue<uint16_t>(); break;
				case FieldType::SystemUInt: out << YAML::Key << name << YAML::Value << field.GetValue<uint32_t>(); break;
				case FieldType::SystemULong: out << YAML::Key << name << YAML::Value << field.GetValue<uint64_t>(); break;
				case FieldType::SystemBoolean: out << YAML::Key << name << YAML::Value << field.GetValue<bool>(); break;
				case FieldType::SystemChar: out << YAML::Key << name << YAML::Value << field.GetValue<char>(); break;
				//case FieldType::SystemString: out << YAML::Key << name << YAML::Value << field.GetValue<std::string>(); break;

				case FieldType::LocusVec2: out << YAML::Key << name << YAML::Value << field.GetValue<glm::vec2>(); break;
				case FieldType::LocusVec3: out << YAML::Key << name << YAML::Value << field.GetValue<glm::vec3>(); break;
				//case FieldType::LocusVec4: out << YAML::Key << name << YAML::Value << field.GetValue<glm::vec4>(); break;
				case FieldType::LocusEntity: out << YAML::Key << name << YAML::Value << field.GetValue<UUID>(); break;

				}

			}
			out << YAML::EndMap; // Fields

			out << YAML::EndMap; // Script Component
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
				// --- Entity uuid ---
				uint64_t uuid = entity["Entity"].as<uint64_t>();

				// --- Tag Component ---
				std::string name;
				bool enabled = true;
				auto tagComponent = entity["TagComponent"];
				if (tagComponent)
				{
					name = tagComponent["Tag"].as<std::string>();
					enabled = tagComponent["Enabled"].as<bool>();
				}

				LOCUS_CORE_TRACE("Deserializing Entity: {0}, ID: {1}", name, uuid);
				Entity deserializedEntity = m_Scene->CreateEntityWithUUID(uuid, name);
				deserializedEntity.GetComponent<TagComponent>().Enabled = tagComponent["Enabled"].as<bool>();
				deserializedEntity.GetComponent<TagComponent>().Group = tagComponent["Group"].as<std::string>();

				// --- Transform Component ---
				auto transformComponent = entity["TransformComponent"];
				if (transformComponent) 
				{
					auto& tc = deserializedEntity.GetComponent<TransformComponent>();
					tc.Self = transformComponent["Self"].as<uint64_t>();
					tc.Parent = transformComponent["Parent"].as<uint64_t>();
					tc.LocalPosition = transformComponent["LocalPosition"].as<glm::vec3>();
					tc.LocalRotation = transformComponent["LocalRotation"].as<glm::vec3>();
					tc.LocalRotationQuat = transformComponent["LocalRotationQuat"].as<glm::quat>();
					tc.LocalScale = transformComponent["LocalScale"].as<glm::vec3>();
				}

				// --- Child Component ---
				auto childComponent = entity["ChildComponent"];
				if (childComponent)
				{
					auto& cc = deserializedEntity.AddComponent<ChildComponent>();
					cc.ChildCount = childComponent["ChildCount"].as<uint32_t>();
					auto childEntities = childComponent["ChildEntities"];
					for (uint32_t i = 0; i < cc.ChildCount; i++)
					{
						std::string childWithVal = "Child" + std::to_string(i);
						UUID uuid = childEntities[childWithVal].as<uint64_t>();
						cc.ChildEntities.push_back(uuid);
					}
				}

				// --- Sprite Renderer Component ---
				auto spriteRendererComponent = entity["SpriteRendererComponent"];
				if (spriteRendererComponent)
				{
					auto& src = deserializedEntity.AddComponent<SpriteRendererComponent>();
					src.Texture = TextureHandle(spriteRendererComponent["Texture"].as<uint64_t>());
					src.Color = spriteRendererComponent["Color"].as<glm::vec4>();
					src.TilingFactor = spriteRendererComponent["TilingFactor"].as<float>();
				}

				// --- Circle Renderer Component ---
				auto circleRendererComponent = entity["CircleRendererComponent"];
				if (circleRendererComponent)
				{
					auto& crc = deserializedEntity.AddComponent<CircleRendererComponent>();
					crc.Color = circleRendererComponent["Color"].as<glm::vec4>();
					crc.Thickness = circleRendererComponent["Thickness"].as<float>();
					crc.Fade = circleRendererComponent["Fade"].as<float>();
				}

				// --- Cube Renderer Component ---
				auto cubeRendererComponent = entity["CubeRendererComponent"];
				if (cubeRendererComponent)
				{
					auto& crc = deserializedEntity.AddComponent<CubeRendererComponent>();
					crc.Material = MaterialHandle(spriteRendererComponent["Material"].as<uint64_t>());
				}

				// --- Point Light Component ---
				auto pointLightComponent = entity["PointLightComponent"];
				if (pointLightComponent)
				{
					auto& plc = deserializedEntity.AddComponent<PointLightComponent>();
					plc.Color = pointLightComponent["Color"].as<glm::vec4>();
					plc.Intensity = pointLightComponent["Intensity"].as<float>();
				}

				// --- Directional Light Component ---
				auto directionalLightComponent = entity["DirectionalLightComponent"];
				if (directionalLightComponent)
				{
					auto& dlc = deserializedEntity.AddComponent<DirectionalLightComponent>();
					dlc.Color = directionalLightComponent["Color"].as<glm::vec4>();
					dlc.Intensity = directionalLightComponent["Intensity"].as<float>();
				}

				// --- Spot Light Component ---
				auto spotLightComponent = entity["SpotLightComponent"];
				if (spotLightComponent)
				{
					auto& slc = deserializedEntity.AddComponent<SpotLightComponent>();
					slc.Color = spotLightComponent["Color"].as<glm::vec4>();
					slc.Intensity = spotLightComponent["Intensity"].as<float>();
					slc.CutOff = spotLightComponent["CutOff"].as<float>();
					slc.OuterCutOff = spotLightComponent["OuterCutOff"].as<float>();
				}

				// --- Camera Component ---
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

				// --- Rigidbody2D Component ---
				auto rigidBody2DComponent = entity["Rigidbody2DComponent"];
				if (rigidBody2DComponent)
				{
					auto& rb2D = deserializedEntity.AddComponent<Rigidbody2DComponent>();
					rb2D.BodyType = (Rigidbody2DType)rigidBody2DComponent["BodyType"].as<int>();
					rb2D.Mass = rigidBody2DComponent["Mass"].as<float>();
					rb2D.GravityScale = rigidBody2DComponent["GravityScale"].as<float>();
					rb2D.LinearDamping = rigidBody2DComponent["LinearDamping"].as<float>();
					rb2D.AngularDamping = rigidBody2DComponent["AngularDamping"].as<float>();
					rb2D.FixedRotation = rigidBody2DComponent["FixedRotation"].as<bool>();
					rb2D.IsBullet = rigidBody2DComponent["IsBullet"].as<bool>();
				}

				// --- BoxCollider2D Component ---
				auto boxCollider2DComponent = entity["BoxCollider2DComponent"];
				if (boxCollider2DComponent)
				{
					auto& bc2D = deserializedEntity.AddComponent<BoxCollider2DComponent>();
					bc2D.Friction = boxCollider2DComponent["Friction"].as<float>();
					bc2D.Restitution = boxCollider2DComponent["Restitution"].as<float>();
					bc2D.RestitutionThreshold = boxCollider2DComponent["RestitutionThreshold"].as<float>();
					bc2D.CollisionCategory = boxCollider2DComponent["CollisionCategory"].as<uint16_t>();
					bc2D.CollisionMask = boxCollider2DComponent["CollisionMask"].as<uint16_t>();
					bc2D.Offset = boxCollider2DComponent["Offset"].as<glm::vec2>();
					bc2D.Size = boxCollider2DComponent["Size"].as<glm::vec2>();
				}

				// --- CircleCollider2D Component ---
				auto circleCollider2DComponent = entity["CircleCollider2DComponent"];
				if (circleCollider2DComponent)
				{
					auto& c2D = deserializedEntity.AddComponent<CircleCollider2DComponent>();
					c2D.Friction = circleCollider2DComponent["Friction"].as<float>();
					c2D.Restitution = circleCollider2DComponent["Restitution"].as<float>();
					c2D.RestitutionThreshold = circleCollider2DComponent["RestitutionThreshold"].as<float>();
					c2D.CollisionCategory = circleCollider2DComponent["CollisionCategory"].as<uint16_t>();
					c2D.CollisionMask = circleCollider2DComponent["CollisionMask"].as<uint16_t>();
					c2D.Offset = circleCollider2DComponent["Offset"].as<glm::vec2>();
					c2D.Radius = circleCollider2DComponent["Radius"].as<float>();
				}

				// --- Script Component ---
				auto scriptComponent = entity["ScriptComponent"];
				auto fields = scriptComponent["Fields"];
				if (scriptComponent)
				{
					auto& sc = deserializedEntity.AddComponent<ScriptComponent>();
					sc.ScriptClass = scriptComponent["ScriptClass"].as<std::string>();

					// Fields
					if (fields && ScriptEngine::HasClass(sc.ScriptClass))
					{
						Ref<ScriptClass> scriptClass = ScriptEngine::GetScriptClass(sc.ScriptClass);
						auto& classFields = scriptClass->GetPublicFields();
						auto& fieldInstances = ScriptEngine::GetFieldInstances(uuid);

						for (auto& [fieldName, field] : classFields)
						{
							ScriptClassFieldInstance& scriptField = fieldInstances[fieldName];
							scriptField.Field = field;

							if (fields[fieldName])
							{
								switch (field.Type)
								{
								case FieldType::SystemSingle:  scriptField.SetValue<float>(fields[fieldName].as<float>());         break;
								case FieldType::SystemDouble:  scriptField.SetValue<double>(fields[fieldName].as<double>());       break;
								case FieldType::SystemShort:   scriptField.SetValue<int16_t>(fields[fieldName].as<int16_t>());     break;
								case FieldType::SystemInt:     scriptField.SetValue<int>(fields[fieldName].as<int>());             break;
								case FieldType::SystemLong:    scriptField.SetValue<int64_t>(fields[fieldName].as<int64_t>());     break;
								case FieldType::SystemUShort:  scriptField.SetValue<uint16_t>(fields[fieldName].as<uint16_t>());   break;
								case FieldType::SystemUInt:    scriptField.SetValue<uint32_t>(fields[fieldName].as<uint32_t>());   break;
								case FieldType::SystemULong:   scriptField.SetValue<uint64_t>(fields[fieldName].as<uint64_t>());   break;
								case FieldType::SystemBoolean: scriptField.SetValue<bool>(fields[fieldName].as<bool>());           break;
								case FieldType::SystemChar:    scriptField.SetValue<char>(fields[fieldName].as<char>());           break;
								//case FieldType::SystemString: scriptField.SetValue<std::string>(fields[fieldName].as<std::string>()); break;

								case FieldType::LocusVec2:     scriptField.SetValue<glm::vec2>(fields[fieldName].as<glm::vec2>()); break;
								case FieldType::LocusVec3:     scriptField.SetValue<glm::vec3>(fields[fieldName].as<glm::vec3>()); break;
								//case FieldType::LocusVec4: scriptField.SetValue<glm::vec4>(fields[fieldName].as<glm::vec4>());     break;
								case FieldType::LocusEntity:   scriptField.SetValue<uint64_t>(fields[fieldName].as<uint64_t>());   break;
								}
							}
							else
							{
								switch (field.Type)
								{
								case FieldType::SystemSingle:  scriptField.SetValue<float>(scriptClass->GetFieldValue<float>(fieldName));         break;
								case FieldType::SystemDouble:  scriptField.SetValue<double>(scriptClass->GetFieldValue<double>(fieldName));       break;
								case FieldType::SystemShort:   scriptField.SetValue<int16_t>(scriptClass->GetFieldValue<int16_t>(fieldName));     break;
								case FieldType::SystemInt:     scriptField.SetValue<int>(scriptClass->GetFieldValue<int>(fieldName));             break;
								case FieldType::SystemLong:    scriptField.SetValue<int64_t>(scriptClass->GetFieldValue<int64_t>(fieldName));     break;
								case FieldType::SystemUShort:  scriptField.SetValue<uint16_t>(scriptClass->GetFieldValue<uint16_t>(fieldName));   break;
								case FieldType::SystemUInt:    scriptField.SetValue<uint32_t>(scriptClass->GetFieldValue<uint32_t>(fieldName));   break;
								case FieldType::SystemULong:   scriptField.SetValue<uint64_t>(scriptClass->GetFieldValue<uint64_t>(fieldName));   break;
								case FieldType::SystemBoolean: scriptField.SetValue<bool>(scriptClass->GetFieldValue<bool>(fieldName));           break;
								case FieldType::SystemChar:    scriptField.SetValue<char>(scriptClass->GetFieldValue<char>(fieldName));           break;
									//case FieldType::SystemString: scriptField.SetValue<std::string>(fields[fieldName].as<std::string>()); break;

								case FieldType::LocusVec2:     scriptField.SetValue<glm::vec2>(scriptClass->GetFieldValue<glm::vec2>(fieldName)); break;
								case FieldType::LocusVec3:     scriptField.SetValue<glm::vec3>(scriptClass->GetFieldValue<glm::vec3>(fieldName)); break;
									//case FieldType::LocusVec4: scriptField.SetValue<glm::vec4>(fields[fieldName].as<glm::vec4>()); break;
								case FieldType::LocusEntity:   scriptField.SetValue<uint64_t>(scriptClass->GetFieldValue<uint64_t>(fieldName));   break;
								}
							}
						}
					}
					else
					{
						sc.ScriptClass = std::string();
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
