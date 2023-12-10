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

		// Push pair of collided entities to stack if entity has a script component.
		// TODO: Also check if OnCollisionBegin() is defined.
		if (aEntity.HasComponent<ScriptComponent>())
			m_BeginContacts.push(std::make_pair(aEntity, bEntity));

		if (bEntity.HasComponent<ScriptComponent>())
			m_BeginContacts.push(std::make_pair(bEntity, aEntity));
	}

	void ContactListener2D::EndContact(b2Contact* contact)
	{
		Scene* scene = ScriptEngine::GetScene();
		UUID aUUID = (UUID)contact->GetFixtureA()->GetBody()->GetUserData().pointer;
		UUID bUUID = (UUID)contact->GetFixtureB()->GetBody()->GetUserData().pointer;
		Entity aEntity = scene->GetEntityByUUID(aUUID);
		Entity bEntity = scene->GetEntityByUUID(bUUID);

		// Push pair of collided entities to stack if entity has a script component.
		// TODO: Also check if OnCollisionEnd() is defined.
		if (aEntity.HasComponent<ScriptComponent>())
			m_EndContacts.push(std::make_pair(aEntity, bEntity));

		if (bEntity.HasComponent<ScriptComponent>())
			m_EndContacts.push(std::make_pair(bEntity, aEntity));
	}

	void ContactListener2D::Execute()
	{
		while (!m_BeginContacts.empty())
		{
			Entity aEntity = m_BeginContacts.top().first;
			Entity bEntity = m_BeginContacts.top().second;
			UUID aUUID = aEntity.GetUUID();
			UUID bUUID = bEntity.GetUUID();
			Ref<ScriptInstance> aInstance = ScriptEngine::GetScriptInstance(aUUID);
			MonoMethod* method = ScriptEngine::GetEntityBaseClass()->GetMethod("OnCollisionBeginInternal", 1);
			void* data = &bUUID;
			ScriptEngine::InvokeMethod(aInstance, method, 1, &data);
			m_BeginContacts.pop();
		}

		while (!m_EndContacts.empty())
		{
			Entity aEntity = m_EndContacts.top().first;
			Entity bEntity = m_EndContacts.top().second;
			UUID aUUID = aEntity.GetUUID();
			UUID bUUID = bEntity.GetUUID();
			Ref<ScriptInstance> aInstance = ScriptEngine::GetScriptInstance(aUUID);
			MonoMethod* method = ScriptEngine::GetEntityBaseClass()->GetMethod("OnCollisionEndInternal", 1);
			void* data = &bUUID;
			ScriptEngine::InvokeMethod(aInstance, method, 1, &data);
			m_EndContacts.pop();
		}
	}
}
