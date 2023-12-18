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
		Ref<Scene> scene = ScriptEngine::GetScene();
		UUID aUUID = (UUID)contact->GetFixtureA()->GetBody()->GetUserData().pointer;
		UUID bUUID = (UUID)contact->GetFixtureB()->GetBody()->GetUserData().pointer;
		Entity aEntity = scene->GetEntityByUUID(aUUID);
		Entity bEntity = scene->GetEntityByUUID(bUUID);

		// Push pair of collided entities to stack if entity has a script component.
		if (aEntity.IsValid()) // Checks for valid as entities can be destroyed on contact.
		{
			if (aEntity.HasComponent<ScriptComponent>())
				m_BeginContacts.push(std::make_pair(aUUID, bUUID));
		}

		if (bEntity.IsValid())
		{
			if (bEntity.HasComponent<ScriptComponent>())
				m_BeginContacts.push(std::make_pair(bUUID, aUUID));
		}
	}

	void ContactListener2D::EndContact(b2Contact* contact)
	{
		Ref<Scene> scene = ScriptEngine::GetScene();
		UUID aUUID = (UUID)contact->GetFixtureA()->GetBody()->GetUserData().pointer;
		UUID bUUID = (UUID)contact->GetFixtureB()->GetBody()->GetUserData().pointer;
		Entity aEntity = scene->GetEntityByUUID(aUUID);
		Entity bEntity = scene->GetEntityByUUID(bUUID);

		// Push pair of collided entities to stack if entity has a script component.
		// TODO: Also check if OnCollisionEnd() is defined.
		if (aEntity.IsValid())
		{
			if (aEntity.HasComponent<ScriptComponent>())
				m_EndContacts.push(std::make_pair(aUUID, bUUID));
		}
		
		if (bEntity.IsValid())
		{
			if (bEntity.HasComponent<ScriptComponent>())
				m_EndContacts.push(std::make_pair(bUUID, aUUID));
		}
	}

	void ContactListener2D::Execute()
	{
		while (!m_BeginContacts.empty())
		{
			UUID aUUID = m_BeginContacts.front().first;
			UUID bUUID = m_BeginContacts.front().second;
			Entity aEntity = ScriptEngine::GetScene()->GetEntityByUUID(aUUID);
			Ref<ScriptInstance> instance = ScriptEngine::GetScriptInstance(aUUID);
			if (instance)
			{
				MonoMethod* method = ScriptEngine::GetEntityBaseClass()->GetMethod("OnCollisionBeginInternal", 1);
				void* data = &bUUID;
				ScriptEngine::InvokeMethod(instance, method, &data);
			}
			m_BeginContacts.pop();
		}

		while (!m_EndContacts.empty())
		{
			UUID aUUID = m_EndContacts.front().first;
			UUID bUUID = m_EndContacts.front().second;
			Entity aEntity = ScriptEngine::GetScene()->GetEntityByUUID(aUUID);
			Ref<ScriptInstance> instance = ScriptEngine::GetScriptInstance(aUUID);
			if (instance)
			{
				MonoMethod* method = ScriptEngine::GetEntityBaseClass()->GetMethod("OnCollisionEndInternal", 1);
				void* data = &bUUID;
				ScriptEngine::InvokeMethod(instance, method, &data);
			}
			m_EndContacts.pop();
		}
	}
}
