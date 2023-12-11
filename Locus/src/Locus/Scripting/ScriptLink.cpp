#include "Lpch.h"
#include "ScriptLink.h"

#include <mono/jit/jit.h>
#include <mono/metadata/assembly.h>

#include "Locus/Scripting/ScriptEngine.h"
#include "Locus/Scene/Entity.h"
#include "Locus/Scene/Components.h"

// Include last
#include "Locus/Scripting/InternalCalls.h"

namespace Locus
{
	void ScriptLink::RegisterFunctions()
	{
		RegisterComponents();

		LINK_INTERNAL_CALL(DebugLog);
		LINK_INTERNAL_CALL(CreateEntity);

		// Entity
		LINK_INTERNAL_CALL(Entity_HasComponent);
		LINK_INTERNAL_CALL(Entity_AddComponent);
		LINK_INTERNAL_CALL(Entity_GetTag);
		LINK_INTERNAL_CALL(Entity_SetTag);
		LINK_INTERNAL_CALL(Entity_GetEnabled);
		LINK_INTERNAL_CALL(Entity_SetEnabled);
		LINK_INTERNAL_CALL(Entity_Find);

		// Vec2
		LINK_INTERNAL_CALL(Vec2_Distance);
		LINK_INTERNAL_CALL(Vec2_Length);

		// Vec3
		LINK_INTERNAL_CALL(Vec3_Cross);
		LINK_INTERNAL_CALL(Vec3_Distance);
		LINK_INTERNAL_CALL(Vec3_Length);

		// Vec4
		LINK_INTERNAL_CALL(Vec4_Distance);
		LINK_INTERNAL_CALL(Vec4_Length);

		// Transform Component
		LINK_INTERNAL_CALL(TransformComponent_GetLocalTransform);
		LINK_INTERNAL_CALL(TransformComponent_GetWorldTransform);
		LINK_INTERNAL_CALL(TransformComponent_GetWorldToLocal);
		LINK_INTERNAL_CALL(TransformComponent_GetLocalPosition);
		LINK_INTERNAL_CALL(TransformComponent_SetLocalPosition);
		LINK_INTERNAL_CALL(TransformComponent_GetLocalRotationEuler);
		LINK_INTERNAL_CALL(TransformComponent_SetLocalRotationEuler);
		LINK_INTERNAL_CALL(TransformComponent_GetLocalScale);
		LINK_INTERNAL_CALL(TransformComponent_SetLocalScale);

		// Sprite Renderer Component
		LINK_INTERNAL_CALL(SpriteRendererComponent_GetColor);
		LINK_INTERNAL_CALL(SpriteRendererComponent_SetColor);
		LINK_INTERNAL_CALL(SpriteRendererComponent_GetTilingFactor);
		LINK_INTERNAL_CALL(SpriteRendererComponent_SetTilingFactor);

		// Circle Renderer Component
		LINK_INTERNAL_CALL(CircleRendererComponent_GetColor);
		LINK_INTERNAL_CALL(CircleRendererComponent_SetColor);
		LINK_INTERNAL_CALL(CircleRendererComponent_GetThickness);
		LINK_INTERNAL_CALL(CircleRendererComponent_SetThickness);
		LINK_INTERNAL_CALL(CircleRendererComponent_GetFade);
		LINK_INTERNAL_CALL(CircleRendererComponent_SetFade);

		// Rigidbody2D Component
		LINK_INTERNAL_CALL(Rigidbody2DComponent_GetBodyType);
		LINK_INTERNAL_CALL(Rigidbody2DComponent_SetBodyType);
		LINK_INTERNAL_CALL(Rigidbody2DComponent_GetMass);
		LINK_INTERNAL_CALL(Rigidbody2DComponent_SetMass);
		LINK_INTERNAL_CALL(Rigidbody2DComponent_GetGravityScale);
		LINK_INTERNAL_CALL(Rigidbody2DComponent_SetGravityScale);
		LINK_INTERNAL_CALL(Rigidbody2DComponent_GetLinearDamping);
		LINK_INTERNAL_CALL(Rigidbody2DComponent_SetLinearDamping);
		LINK_INTERNAL_CALL(Rigidbody2DComponent_GetAngularDamping);
		LINK_INTERNAL_CALL(Rigidbody2DComponent_SetAngularDamping);
		LINK_INTERNAL_CALL(Rigidbody2DComponent_GetFixedRotation);
		LINK_INTERNAL_CALL(Rigidbody2DComponent_SetFixedRotation);
		LINK_INTERNAL_CALL(Rigidbody2DComponent_AddForce);
		LINK_INTERNAL_CALL(Rigidbody2DComponent_AddLinearImpulse);
		LINK_INTERNAL_CALL(Rigidbody2DComponent_SetPosition);

		// Input
		LINK_INTERNAL_CALL(Input_IsKeyPressed);
	}

	template<typename T>
	static void RegisterComponent()
	{
		// Format type name of component and retrieve a monotype
		std::string_view typeName = typeid(T).name();
		size_t pos = typeName.find_last_of(':');
		std::string_view structName = typeName.substr(pos + 1);
		std::string managedTypename = fmt::format("Locus.{}", structName);
		MonoType* managedType = mono_reflection_type_from_name(managedTypename.data(), ScriptEngine::GetImage());
		if (!managedType)
		{
			LOCUS_CORE_ERROR("Could not find component type {}", managedTypename);
			return;
		}
		// Add to map
		s_HasComponentFunctions[managedType] = [](Entity entity) { return entity.HasComponent<T>(); };
		s_AddComponentFunctions[managedType] = [](Entity entity) { entity.AddOrReplaceComponent<T>(); };
	}

	void ScriptLink::RegisterComponents()
	{
		s_HasComponentFunctions.clear();
		s_AddComponentFunctions.clear();
		RegisterComponent<IDComponent>();
		RegisterComponent<TagComponent>();
		RegisterComponent<TransformComponent>();
		RegisterComponent<ChildComponent>();
		RegisterComponent<SpriteRendererComponent>();
		RegisterComponent<CircleRendererComponent>();
		RegisterComponent<CameraComponent>();
		RegisterComponent<Rigidbody2DComponent>();
		RegisterComponent<BoxCollider2DComponent>();
		RegisterComponent<CircleCollider2DComponent>();
		RegisterComponent<NativeScriptComponent>();
		RegisterComponent<ScriptComponent>();
	}
}
