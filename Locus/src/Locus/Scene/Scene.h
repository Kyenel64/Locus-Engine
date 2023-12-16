// --- Scene ------------------------------------------------------------------
// Scene class.
#pragma once

#include <entt.hpp>

#include "Locus/Core/Timestep.h"
#include "Locus/Core/UUID.h"
#include "Locus/Renderer/EditorCamera.h"

class b2World;

namespace Locus
{
	class Entity;
	class ContactListener2D;

	class Scene
	{
	public:
		Scene();
		Scene(const std::string& sceneName) : m_SceneName(sceneName) {}
		~Scene() = default;

		// Creates an entity with a new UUID.
		Entity CreateEntity(const std::string& name = std::string());
		// Creates an entity with an existing UUID.
		Entity CreateEntityWithUUID(UUID uuid, const std::string& name = std::string());

		void DestroyEntity(Entity entity);

		static Ref<Scene> Copy(Ref<Scene> other);
		template<typename T>
		static void CopyComponent(Entity from, Entity to);
		static void CopyAllComponents(Entity from, Entity to);

		void OnUpdateRuntime(Timestep deltaTime);
		void OnUpdateEditor(Timestep deltaTime, EditorCamera& camera);
		void OnUpdatePhysics(Timestep deltaTime, EditorCamera& camera);

		void OnRuntimeStart();
		void OnPhysicsStart();
		void OnRuntimeStop();

		// Create new box2d physics data for newly added physics components.
		void CreatePhysicsData(Entity entity);
		// Update physics data during runtime.
		void UpdatePhysicsData(Entity entity);

		void OnViewportResize(uint32_t width, uint32_t height);

		Entity GetPrimaryCameraEntity();
		Entity GetEntityByUUID(UUID uuid);
		const std::string& GetSceneName() const { return m_SceneName; }

		template<typename... T>
		auto GetEntitiesWith()
		{
			return m_Registry.view<T...>();
		}

		glm::mat4 Scene::GetWorldTransform(Entity entity);

		void SetSceneName(const std::string& name) { m_SceneName = name; }

	private:
		template<typename T>
		void OnComponentAdded(Entity entity, T& component);
	private:
		std::string m_SceneName = "Untitled";
		entt::registry m_Registry;
		std::unordered_map<UUID, Entity> m_Entities;
		uint32_t m_ViewportWidth = 0, m_ViewportHeight = 0;

		b2World* m_Box2DWorld = nullptr;
		Ref<ContactListener2D> m_ContactListener;

		friend class Entity;
		friend class SceneSerializer;
	};
}
