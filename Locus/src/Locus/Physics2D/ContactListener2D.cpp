#include "Lpch.h"
#include "ContactListener2D.h"

#include "Locus/Core/Core.h"
#include "Locus/Core/UUID.h"
#include "Locus/Scripting/ScriptEngine.h"
#include "Locus/Scene/Scene.h"
#include "Locus/Scene/Components.h"

namespace Locus
{
	void ContactListener2D::BeginContact(b2Contact* contact)
	{
		Scene* scene = ScriptEngine::GetScene(); // TODO: Make Ref<Scene>
		UUID aUUID = (UUID)contact->GetFixtureA()->GetBody()->GetUserData().pointer;
		UUID bUUID = (UUID)contact->GetFixtureB()->GetBody()->GetUserData().pointer;
		Entity aEntity = scene->GetEntityByUUID(aUUID);
		Entity bEntity = scene->GetEntityByUUID(bUUID);

		// Call OnCollisionBegin() for both collided entities if the function is defined.
		if (aEntity.HasComponent<ScriptComponent>())
		{
			Ref<ScriptInstance> aInstance = ScriptEngine::GetScriptInstance(aUUID);
			MonoMethod* method = ScriptEngine::GetEntityBaseClass()->GetMethod("OnCollisionBeginInternal", 1);
			void* data = &bUUID;
			ScriptEngine::InvokeMethod(aInstance, method, 1, &data);
		}

		if (bEntity.HasComponent<ScriptComponent>())
		{
			Ref<ScriptInstance> bInstance = ScriptEngine::GetScriptInstance(bUUID);
			MonoMethod* method = ScriptEngine::GetEntityBaseClass()->GetMethod("OnCollisionBeginInternal", 1);
			void* data = &aUUID;
			ScriptEngine::InvokeMethod(bInstance, method, 1, &data);
		}
		
		//LOCUS_CORE_INFO("Contact detected! A: {0}, B:{1}", aEntity.GetComponent<TagComponent>().Tag, bEntity.GetComponent<TagComponent>().Tag);
	}

	void ContactListener2D::EndContact(b2Contact* contact)
	{
		Scene* scene = ScriptEngine::GetScene();
		UUID aUUID = (UUID)contact->GetFixtureA()->GetBody()->GetUserData().pointer;
		UUID bUUID = (UUID)contact->GetFixtureB()->GetBody()->GetUserData().pointer;
		Entity aEntity = scene->GetEntityByUUID(aUUID);
		Entity bEntity = scene->GetEntityByUUID(bUUID);

		// Call OnCollisionEnd() for both collided entities if the function is defined.
		if (aEntity.HasComponent<ScriptComponent>())
		{
			Ref<ScriptInstance> aInstance = ScriptEngine::GetScriptInstance(aUUID);
			MonoMethod* method = ScriptEngine::GetEntityBaseClass()->GetMethod("OnCollisionEndInternal", 1);
			void* data = &bUUID;
			ScriptEngine::InvokeMethod(aInstance, method, 1, &data);
		}

		if (bEntity.HasComponent<ScriptComponent>())
		{
			Ref<ScriptInstance> bInstance = ScriptEngine::GetScriptInstance(bUUID);
			MonoMethod* method = ScriptEngine::GetEntityBaseClass()->GetMethod("OnCollisionEndInternal", 1);
			void* data = &aUUID;
			ScriptEngine::InvokeMethod(bInstance, method, 1, &data);
		}

		//LOCUS_CORE_INFO("Contact ended! A: {0}, B:{1}", aEntity.GetComponent<TagComponent>().Tag, bEntity.GetComponent<TagComponent>().Tag);
	}
}
