#pragma once

#include "Locus/Core/UUID.h"
#include "Locus/Scene/Scene.h"
#include "Locus/Core/Log.h"
#include "Components.h"

#include "entt.hpp"

namespace Locus
{
	class Entity
	{
	public:
		Entity() = default;
		Entity(entt::entity handle, Scene* scene);
		Entity(const Entity& other) = default;

		// Entites doesnt really contain components. The scene contains components which holds handles to the entity.
		// An Entity is an ID. A Component is a struct of data. A System is the logic that operates on the components.
		template<typename T, typename... Args>
		T& AddComponent(Args&&... args)
		{
			LOCUS_CORE_ASSERT(!HasComponent<T>(), "Entity already has component!");
			T& component = m_Scene->m_Registry.emplace<T>(m_EntityHandle, std::forward<Args>(args)...);
			m_Scene->OnComponentAdded<T>(*this, component);
			return component;
		}

		template<typename T>
		T& GetComponent()
		{
			LOCUS_CORE_ASSERT(HasComponent<T>(), "Entity does not have component!");
			return m_Scene->m_Registry.get<T>(m_EntityHandle);
		}

		template<typename T>
		bool HasComponent()
		{
			return m_Scene->m_Registry.any_of<T>(m_EntityHandle);
		}

		template<typename T>
		void RemoveComponent()
		{
			LOCUS_CORE_ASSERT(HasComponent<T>(), "Entity does not have component!");
			m_Scene->m_Registry.remove<T>(m_EntityHandle);
		}

		bool IsValid()
		{
			if (m_EntityHandle == entt::null)
				return false;
			else
				return m_Scene->m_Registry.valid(m_EntityHandle);
		}

		// --- Overrides ---
		operator bool() const { return m_EntityHandle != entt::null; }

		operator uint32_t() const { return (uint32_t)m_EntityHandle; }
		operator entt::entity() const { return m_EntityHandle; }

		UUID GetUUID() { return GetComponent<IDComponent>().ID; }

		bool operator==(const Entity& other) const 
		{
			return m_EntityHandle == other.m_EntityHandle && m_Scene == other.m_Scene;
		}

		bool operator!=(const Entity& other) const
		{
			return !(*this == other);
		}
	private:
		entt::entity m_EntityHandle{ entt::null };
		Scene* m_Scene = nullptr;
	};
}