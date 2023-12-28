// --- Components -------------------------------------------------------------
// Components for Locus ECS.
// Components:
//	Tag
//	Transform
//	Child
//	SpriteRenderer
//	CircleRenderer
//	CubeRenderer
//	PointLight
//	Camera
//	Rigidbody2D
//	BoxCollider2D
//	CircleCollider2D
//	NativeScript
//	Script
// 
// Classes to edit when writing a new component:
//	Scene.cpp, PropertiesPanel.cpp, SceneSerializer.cpp, EntityCommands.h
#pragma once

#include <stack>

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
	enum class Rigidbody2DType { Static = 0, Dynamic = 1, Kinematic = 2 };

	struct IDComponent
	{
		UUID ID;

		IDComponent() = default;
		IDComponent(const IDComponent&) = default;
	};

	struct TagComponent
	{
		std::string Tag;
		std::string Group;
		bool Enabled = true;

		TagComponent() = default;
		TagComponent(const TagComponent&) = default;
		TagComponent(const std::string& tag, bool enabled = true) : Tag(tag), Enabled(enabled) {}
	};

	struct ChildComponent
	{
		uint32_t ChildCount = 0;
		std::vector<UUID> ChildEntities;

		ChildComponent() = default;
		ChildComponent(const ChildComponent&) = default;
	};

	struct TransformComponent
	{
		UUID Self = 0;
		UUID Parent = 0;

		glm::vec3 LocalPosition = { 0.0f, 0.0f, 0.0f };
		glm::vec3 LocalScale = { 1.0f, 1.0f, 1.0f };

	private:
		// Rotations are private to force using setters to sync both euler and quat.
		// In degrees
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

		// In degrees
		const glm::vec3& GetLocalRotation() const { return LocalRotation; }
		const glm::quat& GetLocalRotationQuat() const { return LocalRotationQuat; }

		void SetLocalRotation(const glm::vec3& rotation)
		{
			LocalRotation = rotation;
			LocalRotationQuat = glm::quat(glm::radians(LocalRotation));
		}

		void SetLocalRotationQuat(const glm::quat& quat)
		{
			LocalRotationQuat = quat;
			LocalRotation = glm::degrees(glm::eulerAngles(LocalRotationQuat));
		}

		friend class Scene;
		friend class SceneSerializer;
		friend class LocusEditorLayer;
		friend class PropertiesPanel;
	};

	struct SpriteRendererComponent
	{
		// TODO: Take in a material
		glm::vec4 Color = { 1.0f, 1.0f, 1.0f, 1.0f };
		Ref<Texture2D> Texture;
		std::string TexturePath;
		float TilingFactor = 1.0f;

		SpriteRendererComponent() = default;
		SpriteRendererComponent(const SpriteRendererComponent&) = default;
		SpriteRendererComponent(const glm::vec4& color) : Color(color) {}
	};

	struct CircleRendererComponent
	{
		glm::vec4 Color = { 1.0f, 1.0f, 1.0f, 1.0f };
		float Thickness = 1.0f;
		float Fade = 0.005f;

		CircleRendererComponent() = default;
		CircleRendererComponent(const CircleRendererComponent&) = default;
	};

	struct CubeRendererComponent
	{
		// TODO: Put in material
		glm::vec4 Albedo = { 0.5f, 0.5f, 0.5f, 1.0f };
		float Metallic = 0.5f;
		float Roughness = 0.5f;
		float AO = 0.5f;

		CubeRendererComponent() = default;
		CubeRendererComponent(const CubeRendererComponent&) = default;
	};

	struct PointLightComponent
	{
		glm::vec4 Color = { 1.0f, 1.0f, 1.0f, 1.0f };
		float Intensity = 1.0f;

		PointLightComponent() = default;
		PointLightComponent(const PointLightComponent&) = default;
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
		Rigidbody2DType BodyType = Rigidbody2DType::Dynamic;

		// Body
		float Mass = 1.0f;
		float GravityScale = 1.0f;
		float LinearDamping = 0.0f;
		float AngularDamping = 0.01f;
		bool FixedRotation = false;
		bool IsBullet = false;
		
		void* RuntimeBody = nullptr;

		Rigidbody2DComponent() = default;
		Rigidbody2DComponent(const Rigidbody2DComponent&) = default;
	};

	struct BoxCollider2DComponent
	{
		// Fixture. TODO: Put in physics material
		float Friction = 0.3f; // 0 - 1
		float Restitution = 0.0f; // 0 - 1
		float RestitutionThreshold = 1.0f;

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
		// Fixture. TODO: Put in physics material
		float Friction = 0.2f; // 0 - 1
		float Restitution = 0.0f; // 0 - 1
		float RestitutionThreshold = 1.0f;

		uint16_t CollisionCategory = 0x0001;
		uint16_t CollisionMask = 0xFFFF;

		glm::vec2 Offset = { 0.0f, 0.0f };
		float Radius = 0.5f;

		void* RuntimeFixture = nullptr;

		CircleCollider2DComponent() = default;
		CircleCollider2DComponent(const CircleCollider2DComponent&) = default;
	};

	struct ScriptComponent
	{
		std::string ScriptClass;

		ScriptComponent() = default;
		ScriptComponent(const ScriptComponent&) = default;
		ScriptComponent(const std::string& scriptClass) : ScriptClass(scriptClass) {}
	};

	enum class ComponentType
	{
		None = 0,
		Tag,
		Transform,
		Child,
		SpriteRenderer,
		CircleRenderer,
		CubeRenderer,
		PointLight,
		Camera,
		Rigidbody2D,
		BoxCollider2D,
		CircleCollider2D,
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
		Ref<CubeRendererComponent> CubeRenderer;
		Ref<PointLightComponent> PointLight;
		Ref<CameraComponent> Camera;
		Ref<Rigidbody2DComponent> Rigidbody2D;
		Ref<BoxCollider2DComponent> BoxCollider2D;
		Ref<CircleCollider2DComponent> CircleCollider2D;
		Ref<ScriptComponent> Script;
	};
}
