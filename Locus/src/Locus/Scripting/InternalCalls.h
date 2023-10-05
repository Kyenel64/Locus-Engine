#pragma once

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

		// --- Tag Component --------------------------------------------------
		static MonoString* TagComponent_GetTag(UUID entityID)
		{
			Entity entity = GetEntity(entityID);

			std::string tag = entity.GetComponent<TagComponent>().Tag;
			return mono_string_new(ScriptEngine::GetAppDomain(), tag.c_str());
		}

		static void TagComponent_SetTag(UUID entityID, MonoString* newTag)
		{
			Entity entity = GetEntity(entityID);

			MonoError error;
			std::string newTagStr = mono_string_to_utf8_checked(newTag, &error);
			// TODO: Still need to check for the error
			entity.GetComponent<TagComponent>().Tag = newTagStr;
		}

		static bool TagComponent_GetEnabled(UUID entityID)
		{
			Entity entity = GetEntity(entityID);

			return entity.GetComponent<TagComponent>().Enabled;
		}

		static void TagComponent_SetEnabled(UUID entityID, bool newEnabled)
		{
			Entity entity = GetEntity(entityID);
			entity.GetComponent<TagComponent>().Enabled = newEnabled;
		}

		// --- Transform Component --------------------------------------------
		static void TransformComponent_GetLocalPosition(UUID entityID, glm::vec3* output)
		{
			Entity entity = GetEntity(entityID);
			*output = entity.GetComponent<TransformComponent>().LocalPosition;
		}

		static void TransformComponent_SetLocalPosition(UUID entityID, glm::vec3* newVec3)
		{
			Entity entity = GetEntity(entityID);
			entity.GetComponent<TransformComponent>().LocalPosition = *newVec3;
		}
	}
}
