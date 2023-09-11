// --- Components -------------------------------------------------------------
// Holds all component classes for Locus's ECS.
#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>
#include <entt.hpp>

#include "Locus/Core/UUID.h"
#include "Locus/Renderer/Texture.h"
#include "Locus/Scene/SceneCamera.h"
#include "Locus/Scene/Entity.h"

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

		// TODO: Combine into matrix?
		glm::vec3 WorldPosition = { 0.0f, 0.0f, 0.0f };
		glm::vec3 WorldRotation = { 0.0f, 0.0f, 0.0f };
		glm::quat WorldRotationQuat = { 0.0f, 0.0f, 0.0f, 0.0f };
		glm::vec3 WorldScale = { 1.0f, 1.0f, 1.0f };

		bool Dirty = false;

	public:
		TransformComponent() = default;
		TransformComponent(const TransformComponent&) = default;
		TransformComponent(const glm::vec3& position) : WorldPosition(position) {}

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
			glm::mat4 rotation = glm::toMat4(WorldRotationQuat);
			return glm::translate(glm::mat4(1.0f), WorldPosition)
				 * rotation
				 * glm::scale(glm::mat4(1.0f), WorldScale);
		}

		// Position
		glm::vec3 GetLocalPosition() const { return LocalPosition; }
		glm::vec3 GetWorldPosition() const { return WorldPosition; }

		void SetLocalPosition(const glm::vec3& position) 
		{
			LocalPosition = position;
			if (Parent != Entity::Null)
				WorldPosition = Parent.GetComponent<TransformComponent>().GetWorldPosition() + position;
			else
				WorldPosition = LocalPosition;

			Dirty = true;
		}

		void SetWorldPosition(const glm::vec3& position)
		{
			WorldPosition = position;
			if (Parent != Entity::Null)
				LocalPosition = position - Parent.GetComponent<TransformComponent>().GetWorldPosition();
			else
				LocalPosition = WorldPosition;
			Dirty = true;
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
			
			if (Parent != Entity::Null)
				WorldRotation = Parent.GetComponent<TransformComponent>().GetWorldRotation() + rotation;
			else
				WorldRotation = LocalRotation;
			WorldRotationQuat = glm::quat(WorldRotation);

			Dirty = true;
		}

		void SetLocalRotationQuat(const glm::quat& quat)
		{
			LocalRotationQuat = quat;
			LocalRotation = glm::eulerAngles(LocalRotationQuat);
			
			if (Parent != Entity::Null)
				WorldRotationQuat = Parent.GetComponent<TransformComponent>().GetWorldRotationQuat() + quat;
			else
				WorldRotationQuat = LocalRotationQuat;
			WorldRotation = glm::eulerAngles(WorldRotationQuat);

			Dirty = true;
		}

		void SetWorldRotation(const glm::vec3& rotation)
		{
			WorldRotation = rotation;
			WorldRotationQuat = glm::quat(WorldRotation);

			if (Parent != Entity::Null)
				LocalRotation = rotation - Parent.GetComponent<TransformComponent>().GetWorldRotation();
			else
				LocalRotation = WorldRotation;
			LocalRotationQuat = glm::quat(LocalRotation);

			Dirty = true;
		}

		void SetWorldRotationQuat(const glm::quat& quat)
		{
			WorldRotationQuat = quat;
			WorldRotation = glm::eulerAngles(WorldRotationQuat);

			if (Parent != Entity::Null)
				LocalRotationQuat = quat - Parent.GetComponent<TransformComponent>().GetWorldRotationQuat();
			else
				LocalRotationQuat = WorldRotationQuat;
			LocalRotation = glm::eulerAngles(LocalRotationQuat);

			Dirty = true;
		}

		// Scale
		glm::vec3 GetLocalScale() const { return LocalScale; }
		glm::vec3 GetWorldScale() const { return WorldScale; }

		void SetLocalScale(const glm::vec3& scale)
		{
			LocalScale = scale;
			if (Parent != Entity::Null)
				WorldScale = Parent.GetComponent<TransformComponent>().GetWorldScale() + scale;
			else
				WorldScale = LocalScale;

			Dirty = true;
		}

		void SetWorldScale(const glm::vec3& scale)
		{
			WorldScale = scale;
			if (Parent != Entity::Null)
				LocalScale = scale - Parent.GetComponent<TransformComponent>().GetWorldScale();
			else
				LocalScale = WorldScale;

			Dirty = true;
		}


		friend class PropertiesPanel;
		friend class SceneSerializer;
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