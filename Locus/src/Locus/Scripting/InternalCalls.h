#pragma once

#include <box2d/box2d.h>

#include "Locus/Core/Timer.h"
#include "Locus/Core/Input.h"
#include "Locus/Core/KeyCodes.h"
#include "Locus/Physics2D/PhysicsUtils.h"
#include "Locus/Scripting/ScriptUtils.h"

namespace Locus
{
	static std::unordered_map<MonoType*, std::function<bool(Entity)>> s_HasComponentFunctions;
	static std::unordered_map<MonoType*, std::function<void(Entity)>> s_AddComponentFunctions;

	#define LINK_INTERNAL_CALL(func) mono_add_internal_call("Locus.InternalCalls::"#func, (void*)InternalCalls::func)

	static Entity GetEntity(UUID entityID)
	{
		// Probably faster if C# Entity held entt id instead of UUID.
		Scene* scene = ScriptEngine::GetScene();
		if (!scene)
			return Entity::Null;

		Entity entity = scene->GetEntityByUUID(entityID);
		LOCUS_CORE_ASSERT(entity != Entity::Null, "GetEntity(): Invalid Entity!");
		return entity;
	}

	namespace InternalCalls
	{
		static void DebugLog(float val)
		{
			std::cout << val << std::endl;
		}

		// --- Entity ---
		static uint64_t Entity_CreateEntity()
		{
			Scene* scene = ScriptEngine::GetScene();
			Entity entity = scene->CreateEntity();
			return entity.GetUUID();
		}

		static bool Entity_HasComponent(UUID entityID, MonoReflectionType* componentType)
		{
			Entity entity = GetEntity(entityID);
			return s_HasComponentFunctions[mono_reflection_type_get_type(componentType)](entity);
		}

		static void Entity_AddComponent(UUID entityID, MonoReflectionType* componentType)
		{
			Entity entity = GetEntity(entityID);
			s_AddComponentFunctions[mono_reflection_type_get_type(componentType)](entity);

			// TODO: Think of alternative
			Scene* scene = ScriptEngine::GetScene();
			scene->CreatePhysicsData(entity);
		}

		// Tag
		static MonoString* Entity_GetTag(UUID entityID)
		{
			Entity entity = GetEntity(entityID);
			std::string tag = entity.GetComponent<TagComponent>().Tag;
			return mono_string_new(ScriptEngine::GetAppDomain(), tag.c_str());
		}
		static void Entity_SetTag(UUID entityID, MonoString* newTag)
		{
			Entity entity = GetEntity(entityID);
			std::string newTagStr = ScriptUtils::MonoStringToUTF8(newTag);
			entity.GetComponent<TagComponent>().Tag = newTagStr;
		}

		// Group
		static MonoString* Entity_GetGroup(UUID entityID)
		{
			Entity entity = GetEntity(entityID);
			std::string group = entity.GetComponent<TagComponent>().Group;
			return mono_string_new(ScriptEngine::GetAppDomain(), group.c_str());
		}
		static void Entity_SetGroup(UUID entityID, MonoString* newGroup)
		{
			Entity entity = GetEntity(entityID);
			std::string newGroupStr = ScriptUtils::MonoStringToUTF8(newGroup);
			entity.GetComponent<TagComponent>().Group = newGroupStr;
		}

		// Enabled
		static bool Entity_GetEnabled(UUID entityID)
		{
			Entity entity = GetEntity(entityID);
			return entity.GetComponent<TagComponent>().Enabled;
		}
		static void Entity_SetEnabled(UUID entityID, bool newEnabled)
		{
			Entity entity = GetEntity(entityID);
			entity.GetComponent<TagComponent>().Enabled = newEnabled;
		}

		static uint64_t Entity_Find(MonoString* tag)
		{
			std::string newTagStr = ScriptUtils::MonoStringToUTF8(tag);
			auto view = ScriptEngine::GetScene()->GetEntitiesWith<TagComponent>();
			for (auto e : view)
			{
				Entity entity = Entity(e, ScriptEngine::GetScene());
				if (entity.GetComponent<TagComponent>().Tag == newTagStr)
					return entity.GetUUID();
			}

			return 0;
		}

		static void Entity_Destroy(UUID entityID)
		{
			Scene* scene = ScriptEngine::GetScene();
			scene->DestroyEntity(scene->GetEntityByUUID(entityID));
		}

		// --- Vec2 ---
		static float Vec2_Distance(glm::vec2 v1, glm::vec2 v2)
		{
			return glm::distance(v1, v2);
		}

		static float Vec2_Length(glm::vec2 vec)
		{
			return glm::length(vec);
		}

		// --- Vec3 ---
		static void Vec3_Cross(glm::vec3 v1, glm::vec3 v2, glm::vec3* output)
		{
			*output = glm::cross(v1, v2);
		}

		static float Vec3_Distance(glm::vec3 v1, glm::vec3 v2)
		{
			return glm::distance(v1, v2); 
		}

		static float Vec3_Length(glm::vec3 vec)
		{
			return glm::length(vec);
		}

		// --- Vec4 ---
		static float Vec4_Distance(glm::vec4 v1, glm::vec4 v2)
		{
			return glm::distance(v1, v2);
		}

		static float Vec4_Length(glm::vec4 vec)
		{
			return glm::length(vec);
		}

		// --- Transform Component ---
		// Local Transform
		static void TransformComponent_GetLocalTransform(UUID entityID, glm::mat4* output)
		{
			Entity entity = GetEntity(entityID);
			*output = entity.GetComponent<TransformComponent>().GetLocalTransform();
		}
		// World Transform
		static void TransformComponent_GetWorldTransform(UUID entityID, glm::mat4* output)
		{
			Entity entity = GetEntity(entityID);
			*output = ScriptEngine::GetScene()->GetWorldTransform(entity);
		}

		// Local Position
		static void TransformComponent_GetLocalPosition(UUID entityID, glm::vec3* output)
		{
			Entity entity = GetEntity(entityID);
			*output = entity.GetComponent<TransformComponent>().LocalPosition;
		}
		static void TransformComponent_SetLocalPosition(UUID entityID, glm::vec3* newPos)
		{
			Entity entity = GetEntity(entityID);
			entity.GetComponent<TransformComponent>().LocalPosition = *newPos;
		}

		// Local Rotation Euler
		static void TransformComponent_GetLocalRotationEuler(UUID entityID, glm::vec3* output)
		{
			Entity entity = GetEntity(entityID);
			*output = glm::degrees(entity.GetComponent<TransformComponent>().GetLocalRotation());
		}
		static void TransformComponent_SetLocalRotationEuler(UUID entityID, glm::vec3* newRot)
		{
			Entity entity = GetEntity(entityID);
			entity.GetComponent<TransformComponent>().SetLocalRotationDegrees(*newRot);
		}

		// Local Scale
		static void TransformComponent_GetLocalScale(UUID entityID, glm::vec3* output)
		{
			Entity entity = GetEntity(entityID);
			*output = entity.GetComponent<TransformComponent>().LocalScale;
		}
		static void TransformComponent_SetLocalScale(UUID entityID, glm::vec3* newScale)
		{
			Entity entity = GetEntity(entityID);
			entity.GetComponent<TransformComponent>().LocalScale = *newScale;
		}

		// World To Local matrix
		static void TransformComponent_GetWorldToLocal(UUID entityID, glm::mat4* output)
		{
			Entity entity = GetEntity(entityID);
			*output = glm::inverse(ScriptEngine::GetScene()->GetWorldTransform(entity));
		}

		// --- Sprite Renderer Component ---
		// Color
		static void SpriteRendererComponent_GetColor(UUID entityID, glm::vec4* output)
		{
			Entity entity = GetEntity(entityID);
			*output = entity.GetComponent<SpriteRendererComponent>().Color;
		}
		static void SpriteRendererComponent_SetColor(UUID entityID, glm::vec4* newColor)
		{
			Entity entity = GetEntity(entityID);
			entity.GetComponent<SpriteRendererComponent>().Color = *newColor;
		}

		// Tiling Factor
		static float SpriteRendererComponent_GetTilingFactor(UUID entityID)
		{
			Entity entity = GetEntity(entityID);
			return entity.GetComponent<SpriteRendererComponent>().TilingFactor;
		}
		static void SpriteRendererComponent_SetTilingFactor(UUID entityID, float newTilingFactor)
		{
			Entity entity = GetEntity(entityID);
			entity.GetComponent<SpriteRendererComponent>().TilingFactor = newTilingFactor;
		}

		// --- Circle Renderer Component ---
		// Color
		static void CircleRendererComponent_GetColor(UUID entityID, glm::vec4* output)
		{
			Entity entity = GetEntity(entityID);
			*output = entity.GetComponent<CircleRendererComponent>().Color;
		}
		static void CircleRendererComponent_SetColor(UUID entityID, glm::vec4* newColor)
		{
			Entity entity = GetEntity(entityID);
			entity.GetComponent<CircleRendererComponent>().Color = *newColor;
		}

		// Thickness
		static float CircleRendererComponent_GetThickness(UUID entityID)
		{
			Entity entity = GetEntity(entityID);
			return entity.GetComponent<CircleRendererComponent>().Thickness;
		}
		static void CircleRendererComponent_SetThickness(UUID entityID, float newThickness)
		{
			Entity entity = GetEntity(entityID);
			entity.GetComponent<CircleRendererComponent>().Thickness = newThickness;
		}

		// Fade
		static float CircleRendererComponent_GetFade(UUID entityID)
		{
			Entity entity = GetEntity(entityID);
			return entity.GetComponent<CircleRendererComponent>().Fade;
		}
		static void CircleRendererComponent_SetFade(UUID entityID, float newFade)
		{
			Entity entity = GetEntity(entityID);
			entity.GetComponent<CircleRendererComponent>().Fade = newFade;
		}

		// --- Rigidbody2D Component ---
		// Body Type
		static int Rigidbody2DComponent_GetBodyType(UUID entityID)
		{
			Entity entity = GetEntity(entityID);
			return (int)entity.GetComponent<Rigidbody2DComponent>().BodyType;
		}
		static void Rigidbody2DComponent_SetBodyType(UUID entityID, int newBodyType)
		{
			Entity entity = GetEntity(entityID);
			Rigidbody2DComponent& rb2d = entity.GetComponent<Rigidbody2DComponent>();
			rb2d.BodyType = (Rigidbody2DType)newBodyType;
		}

		// Mass
		static float Rigidbody2DComponent_GetMass(UUID entityID)
		{
			Entity entity = GetEntity(entityID);
			return entity.GetComponent<Rigidbody2DComponent>().Mass;
		}
		static void Rigidbody2DComponent_SetMass(UUID entityID, float newMass)
		{
			Entity entity = GetEntity(entityID);
			Rigidbody2DComponent& rb2d = entity.GetComponent<Rigidbody2DComponent>();
			rb2d.Mass = newMass;
		}

		// GravityScale
		static float Rigidbody2DComponent_GetGravityScale(UUID entityID)
		{
			Entity entity = GetEntity(entityID);
			return entity.GetComponent<Rigidbody2DComponent>().GravityScale;
		}
		static void Rigidbody2DComponent_SetGravityScale(UUID entityID, float newGravityScale)
		{
			Entity entity = GetEntity(entityID);
			Rigidbody2DComponent& rb2d = entity.GetComponent<Rigidbody2DComponent>();
			rb2d.GravityScale = newGravityScale;
		}

		// Linear Damping
		static float Rigidbody2DComponent_GetLinearDamping(UUID entityID)
		{
			Entity entity = GetEntity(entityID);
			return entity.GetComponent<Rigidbody2DComponent>().LinearDamping;
		}
		static void Rigidbody2DComponent_SetLinearDamping(UUID entityID, float newLinearDamping)
		{
			Entity entity = GetEntity(entityID);
			Rigidbody2DComponent& rb2d = entity.GetComponent<Rigidbody2DComponent>();
			rb2d.LinearDamping = newLinearDamping;
		}

		// Angular Damping
		static float Rigidbody2DComponent_GetAngularDamping(UUID entityID)
		{
			Entity entity = GetEntity(entityID);
			return entity.GetComponent<Rigidbody2DComponent>().AngularDamping;
		}
		static void Rigidbody2DComponent_SetAngularDamping(UUID entityID, float newAngularDamping)
		{
			Entity entity = GetEntity(entityID);
			Rigidbody2DComponent& rb2d = entity.GetComponent<Rigidbody2DComponent>();
			rb2d.AngularDamping = newAngularDamping;
		}

		// Fixed Rotation
		static bool Rigidbody2DComponent_GetFixedRotation(UUID entityID)
		{
			Entity entity = GetEntity(entityID);
			return entity.GetComponent<Rigidbody2DComponent>().FixedRotation;
		}
		static void Rigidbody2DComponent_SetFixedRotation(UUID entityID, bool newFixedRotation)
		{
			Entity entity = GetEntity(entityID);
			Rigidbody2DComponent& rb2d = entity.GetComponent<Rigidbody2DComponent>();
			rb2d.FixedRotation = newFixedRotation;
		}

		// IsBullet
		static bool Rigidbody2DComponent_GetIsBullet(UUID entityID)
		{
			Entity entity = GetEntity(entityID);
			return entity.GetComponent<Rigidbody2DComponent>().IsBullet;
		}
		static void Rigidbody2DComponent_SetIsBullet(UUID entityID, bool newIsBullet)
		{
			Entity entity = GetEntity(entityID);
			Rigidbody2DComponent& rb2d = entity.GetComponent<Rigidbody2DComponent>();
			rb2d.IsBullet = newIsBullet;
		}

		// Position
		static void Rigidbody2DComponent_GetPosition(UUID entityID, glm::vec2* output)
		{
			Entity entity = GetEntity(entityID);
			Rigidbody2DComponent& rb2d = entity.GetComponent<Rigidbody2DComponent>();
			b2Body* runtimeBody = (b2Body*)rb2d.RuntimeBody;
			glm::vec2 position = { runtimeBody->GetPosition().x, runtimeBody->GetPosition().y };
			*output = position;
		}
		static void Rigidbody2DComponent_SetPosition(UUID entityID, glm::vec2* newPos)
		{
			Entity entity = GetEntity(entityID);
			Rigidbody2DComponent& rb2d = entity.GetComponent<Rigidbody2DComponent>();
			b2Body* runtimeBody = (b2Body*)rb2d.RuntimeBody;
			runtimeBody->SetTransform({ newPos->x, newPos->y }, runtimeBody->GetAngle());
		}

		// Velocity
		static void Rigidbody2DComponent_GetVelocity(UUID entityID, glm::vec2* output)
		{
			Entity entity = GetEntity(entityID);
			Rigidbody2DComponent& rb2d = entity.GetComponent<Rigidbody2DComponent>();
			b2Body* runtimeBody = (b2Body*)rb2d.RuntimeBody;
			glm::vec2 velocity = { runtimeBody->GetLinearVelocity().x, runtimeBody->GetLinearVelocity().y };
			*output = velocity;
		}
		static void Rigidbody2DComponent_SetVelocity(UUID entityID, glm::vec2* newVelocity)
		{
			Entity entity = GetEntity(entityID);
			Rigidbody2DComponent& rb2d = entity.GetComponent<Rigidbody2DComponent>();
			b2Body* runtimeBody = (b2Body*)rb2d.RuntimeBody;
			runtimeBody->SetLinearVelocity({ newVelocity->x, newVelocity->y });
		}

		// Add Force
		static void Rigidbody2DComponent_AddForce(UUID entityID, glm::vec2* force)
		{
			Entity entity = GetEntity(entityID);
			Rigidbody2DComponent& rb2d = entity.GetComponent<Rigidbody2DComponent>();
			b2Body* runtimeBody = (b2Body*)rb2d.RuntimeBody;
			runtimeBody->ApplyForce({ force->x, force->y }, runtimeBody->GetWorldCenter(), true);
		}

		// Add Linear Impulse
		static void Rigidbody2DComponent_AddLinearImpulse(UUID entityID, glm::vec2* impulse)
		{
			Entity entity = GetEntity(entityID);
			Rigidbody2DComponent& rb2d = entity.GetComponent<Rigidbody2DComponent>();
			b2Body* runtimeBody = (b2Body*)rb2d.RuntimeBody;
			runtimeBody->ApplyLinearImpulse({ impulse->x, impulse->y }, runtimeBody->GetWorldCenter(), true);
		}

		// --- Input ---
		static bool Input_IsKeyPressed(uint16_t key)
		{
			return Input::IsKeyPressed(key);
		}

		static bool Input_IsKeyHeld(uint16_t key)
		{
			return Input::IsKeyHeld(key);
		}

		static bool Input_IsKeyReleased(uint16_t key)
		{
			return Input::IsKeyReleased(key);
		}
	}
}
