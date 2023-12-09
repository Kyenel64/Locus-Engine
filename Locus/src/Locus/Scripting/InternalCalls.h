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
		// Probably faster if C# Entity held entt id instead of UUID.
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

		// --- Entity ---
		static bool Entity_HasComponent(UUID entityID, MonoReflectionType* componentType)
		{
			Entity entity = GetEntity(entityID);
			if (entity.IsValid())
			{
				MonoType* type = mono_reflection_type_get_type(componentType);
				return s_HasComponentFunctions[type](entity);
			}
			else
			{
				LOCUS_SCRIPT_CRITICAL("Could not find entity!");
				return false;
			}
		}

		static void Entity_AddComponent(UUID entityID, MonoReflectionType* componentType)
		{
			Entity entity = GetEntity(entityID);
			if (entity.IsValid())
			{
				MonoType* type = mono_reflection_type_get_type(componentType);
				s_AddComponentFunctions[type](entity);
			}
			else
			{
				LOCUS_SCRIPT_CRITICAL("Could not find entity!");
			}
		}

		// Tag
		static MonoString* Entity_GetTag(UUID entityID)
		{
			Entity entity = GetEntity(entityID);
			if (entity.IsValid())
			{
				std::string tag;
				tag = entity.GetComponent<TagComponent>().Tag;
				return mono_string_new(ScriptEngine::GetAppDomain(), tag.c_str());
			}
			else
			{
				LOCUS_SCRIPT_CRITICAL("Could not find entity!");
				return nullptr;
			}
		}
		static void Entity_SetTag(UUID entityID, MonoString* newTag)
		{
			Entity entity = GetEntity(entityID);
			if (entity.IsValid())
			{
				std::string newTagStr = mono_string_to_utf8(newTag);
				entity.GetComponent<TagComponent>().Tag = newTagStr;
			}
			else
			{
				LOCUS_SCRIPT_CRITICAL("Could not find entity!");
			}
		}

		// Enabled
		static bool Entity_GetEnabled(UUID entityID)
		{
			Entity entity = GetEntity(entityID);
			if (entity.IsValid())
				return entity.GetComponent<TagComponent>().Enabled;
			
			LOCUS_SCRIPT_CRITICAL("Could not find entity!");
			return false;
		}
		static void Entity_SetEnabled(UUID entityID, bool newEnabled)
		{
			Entity entity = GetEntity(entityID);
			if (entity.IsValid())
				entity.GetComponent<TagComponent>().Enabled = newEnabled;
			else
				LOCUS_SCRIPT_CRITICAL("Could not find entity!");
		}

		static uint64_t Entity_Find(MonoString* tag)
		{
			char* newTagStr = mono_string_to_utf8(tag);
			auto view = ScriptEngine::GetScene()->GetEntitiesWith<TagComponent>();
			for (auto e : view)
			{
				Entity entity = Entity(e, ScriptEngine::GetScene());
				if (entity.HasComponent<TagComponent>())
				{
					if (entity.GetComponent<TagComponent>().Tag == std::string(newTagStr))
					{
						mono_free(newTagStr);
						return entity.GetUUID();
					}
				}
			}

			LOCUS_SCRIPT_CRITICAL("Could not find entity with tag: {0}", std::string(newTagStr));
			mono_free(newTagStr);
			return 0;
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

		// --- Input ---
		static bool Input_IsKeyPressed(uint16_t key)
		{
			return Input::IsKeyPressed(key);
		}
	}
}
