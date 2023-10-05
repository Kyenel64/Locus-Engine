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

		LINK_INTERNAL_CALL(Entity_HasComponent);
		LINK_INTERNAL_CALL(Entity_AddComponent);

		LINK_INTERNAL_CALL(Vec3_Cross);
		LINK_INTERNAL_CALL(Vec3_Distance);
		LINK_INTERNAL_CALL(Vec3_Length);

		LINK_INTERNAL_CALL(TagComponent_GetTag);
		LINK_INTERNAL_CALL(TagComponent_SetTag);
		LINK_INTERNAL_CALL(TagComponent_GetEnabled);
		LINK_INTERNAL_CALL(TagComponent_SetEnabled);

		LINK_INTERNAL_CALL(TransformComponent_GetLocalPosition);
		LINK_INTERNAL_CALL(TransformComponent_SetLocalPosition);
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