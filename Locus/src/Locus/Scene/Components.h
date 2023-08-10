// --- Components -------------------------------------------------------------
// Holds all component classes for Locus's ECS.
#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>

#include "Locus/Core/UUID.h"
#include "Locus/Renderer/Texture.h"
#include "Locus/Scene/SceneCamera.h"

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

		TagComponent() = default;
		TagComponent(const TagComponent&) = default;
		TagComponent(const std::string& tag) : Tag(tag) {}
	};

	struct TransformComponent
	{
		glm::vec3 Translation = { 0.0f, 0.0f, 0.0f };
		glm::vec3 Scale = { 1.0f, 1.0f, 1.0f };

	private:
		// Setting rotations to private forces the use of setters to avoid being able to 
		// modify one rotation without modifing the other.
		glm::vec3 RotationEuler = { 0.0f, 0.0f, 0.0f };
		glm::quat RotationQuat = { 0.0f, 0.0f, 0.0f, 0.0f };

	public:
		TransformComponent() = default;
		TransformComponent(const TransformComponent&) = default;
		TransformComponent(const glm::vec3& translation) : Translation(translation) {}

		glm::mat4 GetTransform() const
		{
			glm::mat4 rotation = glm::toMat4(RotationQuat);
			return glm::translate(glm::mat4(1.0f), Translation)
				* rotation
				* glm::scale(glm::mat4(1.0f), Scale);
		}

		glm::vec3 GetRotationEuler() const { return RotationEuler; }
		glm::vec3& GetRotationEuler() { return RotationEuler; }

		void SetRotationEuler(const glm::vec3& euler)
		{
			RotationEuler = euler;
			RotationQuat = glm::quat(RotationEuler);
		}

		glm::quat GetRotationQuat() const { return RotationQuat; }

		void SetRotationQuat(const glm::quat& quat)
		{
			RotationQuat = quat;
			RotationEuler = glm::eulerAngles(RotationQuat);
		}

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

	struct RigidBody2DComponent
	{
		enum class RigidBody2DType { Static = 0, Dynamic = 1, Kinematic = 2};
		RigidBody2DType BodyType = RigidBody2DType::Dynamic;

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

		RigidBody2DComponent() = default;
		RigidBody2DComponent(const RigidBody2DComponent&) = default;
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

}