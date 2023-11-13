// --- Components -------------------------------------------------------------
// Holds all component classes for Locus's ECS.
#pragma once

#include <stack> // This could be causing a COMDAT linking error

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>
#include <glm/gtx/matrix_decompose.hpp>
#include <entt.hpp>

#include "Locus/Core/UUID.h"
#include "Locus/Renderer/Texture.h"
#include "Locus/Scene/SceneCamera.h"
#include "Locus/Scene/Entity.h"
#include "Locus/Math/Math.h"

namespace Locus
{
	struct IDComponent
	{
		UUID ID;

		IDComponent() = default;
		IDComponent(const IDComponent&) = default;
	};

	struct TagComponent
	{
		std::string Tag;
		bool Enabled = true;
		uint32_t HierarchyPos;

		TagComponent() = default;
		TagComponent(const TagComponent&) = default;
		TagComponent(const std::string& tag, bool enabled = true) : Tag(tag), Enabled(enabled) {}
	};

	struct ChildComponent
	{
		uint32_t ChildCount = 0;
		std::vector<Entity> ChildEntities;

		ChildComponent() = default;
		ChildComponent(const ChildComponent&) = default;
	};

	struct TransformComponent
	{
		Entity Self = Entity::Null;
		Entity Parent = Entity::Null;

		glm::vec3 LocalPosition = { 0.0f, 0.0f, 0.0f };
		glm::vec3 LocalScale = { 1.0f, 1.0f, 1.0f };

	private:
		// In radians
		glm::vec3 LocalRotation = { 0.0f, 0.0f, 0.0f };
		glm::quat LocalRotationQuat = { 0.0f, 0.0f, 0.0f, 0.0f };

	public:
		TransformComponent() = default;
		TransformComponent(const TransformComponent&) = default;

		// Transform
		glm::mat4 GetLocalTransform() const
		{
			glm::mat4 rotation = glm::toMat4(LocalRotationQuat);
			return glm::translate(glm::mat4(1.0f), LocalPosition)
				 * rotation
				 * glm::scale(glm::mat4(1.0f), LocalScale);
		}

		glm::vec3 GetLocalRotation() const { return LocalRotation; }
		glm::quat GetLocalRotationQuat() const { return LocalRotationQuat; }

		void SetLocalRotation(const glm::vec3& rotation)
		{
			LocalRotation = rotation;
			LocalRotationQuat = glm::quat(LocalRotation);
		}

		void SetLocalRotationDegrees(const glm::vec3& rotation)
		{
			LocalRotation = glm::radians(rotation);
			LocalRotationQuat = glm::quat(LocalRotation);
		}

		void SetLocalRotationQuat(const glm::quat& quat)
		{
			LocalRotationQuat = quat;
			LocalRotation = glm::eulerAngles(LocalRotationQuat);
		}

		friend class Scene;
		friend class SceneSerializer;
		friend class LocusEditorLayer;
		friend class PropertiesPanel;
	};

	struct SpriteRendererComponent
	{
		// TODO: Take in a material
		glm::vec4 Color{ 1.0f, 1.0f, 1.0f, 1.0f };
		Ref<Texture2D> Texture;
		std::string TexturePath;
		float TilingFactor = 1.0f;

		SpriteRendererComponent() = default;
		SpriteRendererComponent(const SpriteRendererComponent&) = default;
		SpriteRendererComponent(const glm::vec4& color) : Color(color) {}
	};

	struct CircleRendererComponent
	{
		glm::vec4 Color{ 1.0f, 1.0f, 1.0f, 1.0f };
		float Thickness = 1.0f;
		float Fade = 0.005f;

		CircleRendererComponent() = default;
		CircleRendererComponent(const CircleRendererComponent&) = default;
	};

	struct CameraComponent
	{
		SceneCamera Camera;
		bool Primary = true;
		bool FixedAspectRatio = false;

		CameraComponent() = default;
		CameraComponent(const CameraComponent&) = default;
	};

	struct Rigidbody2DComponent
	{
		enum class Rigidbody2DType { Static = 0, Dynamic = 1, Kinematic = 2};
		Rigidbody2DType BodyType = Rigidbody2DType::Dynamic;

		// Body
		float Mass = 1.0f;
		float GravityScale = 1.0f;
		float LinearDamping = 0.2f;
		float AngularDamping = 0.2f;
		bool FixedRotation = false;

		// Fixture. TODO: Put in physics material
		float Friction = 0.2f; // 0 - 1
		float Restitution = 0.0f; // 0 - 1
		float RestitutionThreshold = 1.0f;

		void* RuntimeBody = nullptr;

		Rigidbody2DComponent() = default;
		Rigidbody2DComponent(const Rigidbody2DComponent&) = default;
	};

	struct BoxCollider2DComponent
	{
		// Polygon shape
		uint16_t CollisionCategory = 0x0001;
		uint16_t CollisionMask = 0xFFFF;
		glm::vec2 Offset = { 0.0f, 0.0f };
		glm::vec2 Size = { 1.0f, 1.0f };

		void* RuntimeFixture = nullptr;

		BoxCollider2DComponent() = default;
		BoxCollider2DComponent(const BoxCollider2DComponent&) = default;
	};

	struct CircleCollider2DComponent
	{
		uint16_t CollisionCategory = 0x0001;
		uint16_t CollisionMask = 0xFFFF;
		glm::vec2 Offset = { 0.0f, 0.0f };
		float Radius = 0.5f;

		void* RuntimeFixture = nullptr;

		CircleCollider2DComponent() = default;
		CircleCollider2DComponent(const CircleCollider2DComponent&) = default;
	};

	class ScriptableEntity;

	struct NativeScriptComponent
	{
		ScriptableEntity* Instance = nullptr;

		// Creates a function ptr called InstantiateScript that takes no args and returns ScriptableEntity*
		ScriptableEntity* (*InstantiateScript)();
		// Creates a void func ptr called DestroyScript that takes in NativeScriptComponent*
		void (*DestroyScript)(NativeScriptComponent*);

		template <typename T>
		void Bind()
		{
			InstantiateScript = []() { return static_cast<ScriptableEntity*>(new T()); };
			DestroyScript = [](NativeScriptComponent* nsc) { delete nsc->Instance; nsc->Instance = nullptr; };
		}
	};

	struct ScriptComponent
	{
		std::string ScriptClass;

		ScriptComponent() = default;
		ScriptComponent(const ScriptComponent&) = default;
		ScriptComponent(const std::string& scriptClass) : ScriptClass(scriptClass) {}
	};

	// Make sure to update Scene, PropertiesPanel, and ScriptLink class when adding new components
	enum class ComponentType
	{
		None = 0,
		Tag,
		Transform,
		Child,
		SpriteRenderer,
		CircleRenderer,
		Camera,
		Rigidbody2D,
		BoxCollider2D,
		CircleCollider2D,
		NativeScript,
		Script
	};

	struct ComponentData
	{
		Entity EntityID;
		Ref<IDComponent> ID;
		Ref<TagComponent> Tag;
		Ref<ChildComponent> Child;
		Ref<TransformComponent> Transform;
		Ref<SpriteRendererComponent> SpriteRenderer;
		Ref<CircleRendererComponent> CircleRenderer;
		Ref<CameraComponent> Camera;
		Ref<Rigidbody2DComponent> Rigidbody2D;
		Ref<BoxCollider2DComponent> BoxCollider2D;
		Ref<CircleCollider2DComponent> CircleCollider2D;
		Ref<NativeScriptComponent> NativeScript;
		Ref<ScriptComponent> Script;
	};
	
}