// --- Components -------------------------------------------------------------
// Holds all component classes for Locus's ECS.
#pragma once

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

	private:
		// Euler
		glm::vec3 LocalPosition = { 0.0f, 0.0f, 0.0f };
		glm::vec3 LocalRotation = { 0.0f, 0.0f, 0.0f };
		glm::quat LocalRotationQuat = { 0.0f, 0.0f, 0.0f, 0.0f };
		glm::vec3 LocalScale = { 1.0f, 1.0f, 1.0f };

		glm::vec3 WorldPosition = { 0.0f, 0.0f, 0.0f };
		glm::vec3 WorldRotation = { 0.0f, 0.0f, 0.0f };
		glm::quat WorldRotationQuat = { 0.0f, 0.0f, 0.0f, 0.0f };
		glm::vec3 WorldScale = { 1.0f, 1.0f, 1.0f };

		glm::mat4 WorldTransform = glm::mat4(1.0f);
		glm::mat4 LocalToWorld = glm::mat4(1.0f);
		glm::mat4 WorldToLocal = glm::inverse(LocalToWorld);
		

		bool Dirty = false;

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

		glm::mat4 GetWorldTransform() const
		{
			return WorldTransform;
		}

		// Position
		glm::vec3 GetLocalPosition() const { return LocalPosition; }
		glm::vec3 GetWorldPosition() const { return WorldPosition; }

		void SetLocalPosition(const glm::vec3& position) 
		{
			LocalPosition = position;
			SetWorldFromLocalChange();
		}

		void SetWorldPosition(const glm::vec3& position)
		{
			WorldPosition = position;
			UpdateWorldTransform();
			SetLocalFromWorldChange();
		}

		// Rotation
		glm::vec3 GetLocalRotation() const { return LocalRotation; }
		glm::vec3 GetWorldRotation() const { return WorldRotation; }
		glm::quat GetLocalRotationQuat() const { return LocalRotationQuat; }
		glm::quat GetWorldRotationQuat() const { return WorldRotationQuat; }

		void SetLocalRotation(const glm::vec3& rotation)
		{
			LocalRotation = rotation;
			LocalRotationQuat = glm::quat(LocalRotation);
			SetWorldFromLocalChange();
		}

		void SetLocalRotationQuat(const glm::quat& quat)
		{
			LocalRotationQuat = quat;
			LocalRotation = glm::eulerAngles(LocalRotationQuat);
			SetWorldFromLocalChange();
		}

		void SetWorldRotation(const glm::vec3& rotation)
		{
			WorldRotation = rotation;
			WorldRotationQuat = glm::quat(WorldRotation);
			UpdateWorldTransform();
			SetLocalFromWorldChange();
		}

		void SetWorldRotationQuat(const glm::quat& quat)
		{
			WorldRotationQuat = quat;
			WorldRotation = glm::eulerAngles(WorldRotationQuat);
			UpdateWorldTransform();
			SetLocalFromWorldChange();
		}

		glm::vec3 GetLocalScale() const { return LocalScale; }
		glm::vec3 GetWorldScale() const { return WorldScale; }

		void SetLocalScale(const glm::vec3& scale)
		{
			LocalScale = scale;
			SetWorldFromLocalChange();
		}

		void SetWorldScale(const glm::vec3& scale)
		{
			WorldScale = scale;
			UpdateWorldTransform();
			SetLocalFromWorldChange();
		}

	private:
		void SetWorldFromLocalChange()
		{
			WorldTransform = LocalToWorld * GetLocalTransform();
			Math::Decompose(WorldTransform, WorldScale, WorldRotationQuat, WorldPosition);
			WorldRotation = glm::eulerAngles(WorldRotationQuat);

			if (Self.HasComponent<ChildComponent>())
				Dirty = true;
		}

		void UpdateWorldTransform()
		{
			WorldTransform = glm::translate(glm::mat4(1.0f), WorldPosition)
				* glm::toMat4(WorldRotationQuat)
				* glm::scale(glm::mat4(1.0f), WorldScale);
		}

		void SetLocalFromWorldChange()
		{
			glm::mat4 localMatrix = WorldToLocal * WorldTransform;
			Math::Decompose(localMatrix, LocalScale, LocalRotationQuat, LocalPosition);
			LocalRotation = glm::eulerAngles(LocalRotationQuat);

			if (Self.HasComponent<ChildComponent>())
				Dirty = true;
		}

		void Sync()
		{
			auto& parentTC = Parent.GetComponent<TransformComponent>();
			LocalToWorld = parentTC.WorldTransform;
			WorldToLocal = glm::inverse(LocalToWorld);
			SetWorldFromLocalChange();

			parentTC.Dirty = false;
			if (Self.HasComponent<ChildComponent>())
				Dirty = true;
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
		float LinearDrag = 1.0f;
		float AngularDrag = 0.05f;
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
		uint16_t CollisionLayer = 1;
		glm::vec2 Offset = { 0.0f, 0.0f };
		glm::vec2 Size = { 1.0f, 1.0f };

		void* RuntimeFixture = nullptr;

		BoxCollider2DComponent() = default;
		BoxCollider2DComponent(const BoxCollider2DComponent&) = default;
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

	struct ComponentsList
	{
		TagComponent Tag;
		TransformComponent Transform;
		SpriteRendererComponent SpriteRenderer;
		CameraComponent Camera;
		NativeScriptComponent NativeScript;
		Rigidbody2DComponent Rigidbody2D;
		BoxCollider2DComponent BoxCollider2D;
	};

	enum class ComponentType
	{
		None = 0,
		Tag,
		Transform,
		SpriteRenderer,
		Camera,
		Rigidbody2D,
		BoxCollider2D,
		NativeScript
	};
}