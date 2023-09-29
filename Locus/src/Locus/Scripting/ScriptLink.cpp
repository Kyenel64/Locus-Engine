#include "Lpch.h"
#include "ScriptLink.h"

#include <mono/jit/jit.h>

#include "Locus/Scripting/ScriptEngine.h"
#include "Locus/Scene/Entity.h"
#include "Locus/Scene/Components.h"

namespace Locus
{
	static std::unordered_map<MonoType*, std::function<bool(Entity)>> s_HasComponentFunctions;

	// --- Functions ----------------------------------------------------------
	static void Log(float val)
	{
		std::cout << val << std::endl;
	}

	static bool Entity_HasComponent(UUID entityID, MonoReflectionType* componentType)
	{
		Scene* scene = ScriptEngine::GetScene();
		Entity entity = scene->GetEntityByUUID(entityID);

		MonoType* type = mono_reflection_type_get_type(componentType);
		return s_HasComponentFunctions[type](entity);
	}



	// --- ScriptLink ---------------------------------------------------------
	void ScriptLink::RegisterFunctions()
	{
		RegisterComponents();

		mono_add_internal_call("Locus.InternalCalls::Log", Log);
		mono_add_internal_call("Locus.InternalCalls::Entity_HasComponent", Entity_HasComponent);
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