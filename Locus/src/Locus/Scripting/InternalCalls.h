#pragma once

#include "Locus/Core/Input.h"
#include "Locus/Core/KeyCodes.h"

namespace Locus
{
	static std::unordered_map<MonoType*, std::function<bool(Entity)>> s_HasComponentFunctions;
	static std::unordered_map<MonoType*, std::function<void(Entity)>> s_AddComponentFunctions;

	#define LINK_INTERNAL_CALL(func) mono_add_internal_call("Locus.InternalCalls::"#func, (void*)InternalCalls::func)

	static Entity GetEntity(UUID entityID)
	{
		Scene* scene = ScriptEngine::GetScene();
		return scene->GetEntityByUUID(entityID);
	}

	namespace InternalCalls
	{
		static void DebugLog(float val)
		{
			std::cout << val << std::endl;
		}

		static uint64_t CreateEntity()
		{
			Scene* scene = ScriptEngine::GetScene();
			Entity entity = scene->CreateEntity();
			return entity.GetUUID();
		}

		// --- Entity ---------------------------------------------------------
		static bool Entity_HasComponent(UUID entityID, MonoReflectionType* componentType)
		{
			Entity entity = GetEntity(entityID);

			MonoType* type = mono_reflection_type_get_type(componentType);
			return s_HasComponentFunctions[type](entity);
		}

		static void Entity_AddComponent(UUID entityID, MonoReflectionType* componentType)
		{
			Entity entity = GetEntity(entityID);

			MonoType* type = mono_reflection_type_get_type(componentType);
			s_AddComponentFunctions[type](entity);
		}

		static MonoString* Entity_GetTag(UUID entityID)
		{
			Entity entity = GetEntity(entityID);

			std::string tag = entity.GetComponent<TagComponent>().Tag;
			return mono_string_new(ScriptEngine::GetAppDomain(), tag.c_str());
		}

		static void Entity_SetTag(UUID entityID, MonoString* newTag)
		{
			Entity entity = GetEntity(entityID);

			MonoError error;
			std::string newTagStr = mono_string_to_utf8_checked(newTag, &error);
			// TODO: Still need to check for the error
			entity.GetComponent<TagComponent>().Tag = newTagStr;
		}

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

		static UUID Entity_Find(MonoString* tag)
		{
			MonoError error;
			std::string newTagStr = mono_string_to_utf8_checked(tag, &error);
			auto view = ScriptEngine::GetScene()->GetEntitiesWith<TagComponent>();
			for (auto e : view)
			{
				Entity entity = Entity(e, ScriptEngine::GetScene());
				if (entity.GetComponent<TagComponent>().Tag == newTagStr)
					return entity.GetUUID();
			}

			return 0;
		}

		// --- Vec3 -----------------------------------------------------------
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

		// --- Transform Component --------------------------------------------
		static void TransformComponent_GetLocalTransform(UUID entityID, glm::mat4* output)
		{
			Entity entity = GetEntity(entityID);
			*output = entity.GetComponent<TransformComponent>().GetLocalTransform();
		}

		static void TransformComponent_GetWorldTransform(UUID entityID, glm::mat4* output)
		{
			Entity entity = GetEntity(entityID);
			*output = ScriptEngine::GetScene()->GetWorldTransform(entity);
		}

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

		static void TransformComponent_GetWorldToLocal(UUID entityID, glm::mat4* output)
		{
			// Not optimized
			Entity entity = GetEntity(entityID);
			*output = glm::inverse(ScriptEngine::GetScene()->GetWorldTransform(entity));
		}

		// --- Input ----------------------------------------------------------
		static bool Input_IsKeyPressed(uint16_t key)
		{
			return Input::IsKeyPressed(key);
		}
	}
}
