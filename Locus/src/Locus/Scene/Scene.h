// --- Scene ------------------------------------------------------------------
// Scene class.
#pragma once

#include <entt.hpp>

#include "Locus/Core/Timestep.h"
#include "Locus/Core/UUID.h"
#include "Locus/Renderer/EditorCamera.h"

class b2World; // Forward declare here because we dont want files including Scene.h to also include b2World

namespace Locus
{
	class Entity;

	class Scene
	{
	public:
		Scene();
		~Scene() = default;

		static Ref<Scene> Copy(Ref<Scene> other);
		template<typename T>
		static void CopyComponent(Entity from, Entity to);
		static void CopyAllComponents(Entity from, Entity to);

		Entity CreateEntity(const std::string& name = std::string());
		Entity CreateEntityWithUUID(UUID uuid, const std::string& name = std::string(), bool enabled = true);
		Entity CreateEntityWithUUID(Entity copyEntity, UUID uuid, const std::string& name = std::string(), bool enabled = true);
		void DestroyEntity(Entity entity);

		void OnUpdateRuntime(Timestep deltaTime);
		void OnUpdateEditor(Timestep deltaTime, EditorCamera& camera);
		void OnUpdatePhysics(Timestep deltaTime, EditorCamera& camera);

		void OnRuntimeStart();
		void OnPhysicsStart();
		void OnRuntimeStop();

		void OnViewportResize(uint32_t width, uint32_t height);

		Entity GetPrimaryCameraEntity();
		const std::string& GetSceneName() const { return m_SceneName; }
		Entity GetEntityByUUID(UUID uuid);

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
		uint32_t m_ViewportWidth = 0, m_ViewportHeight = 0;
		uint32_t m_RootEntityCount = 1;

		b2World* m_Box2DWorld = nullptr;

		friend class Entity;
		friend class SceneSerializer;
		friend class SceneHierarchyPanel;
		friend class CreateEntityCommand;
		friend class CreateChildEntityCommand;
		friend class DestroyEntityCommand;
		friend class DuplicateEntityCommand;
	};
}