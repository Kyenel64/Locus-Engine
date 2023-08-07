#pragma once

#include "entt.hpp"

#include "Locus/Core/Timestep.h"
#include "Locus/Core/UUID.h"
#include "Locus/Renderer/EditorCamera.h"

namespace Locus
{
	class Entity;

	class Scene
	{
	public:
		Scene();
		~Scene();

		Entity CreateEntity(const std::string& name = std::string());
		Entity CreateEntityWithUUID(UUID uuid, const std::string& name = std::string());
		Entity CreateEntityWithUUID(Entity entity, UUID uuid, const std::string& name = std::string());
		void DestroyEntity(Entity entity);

		void OnUpdateRuntime(Timestep deltaTime);
		void OnUpdateEditor(Timestep deltaTime, EditorCamera& camera);

		void OnViewportResize(uint32_t width, uint32_t height);

		Entity GetPrimaryCameraEntity();
		const std::string& GetSceneName() const { return m_SceneName; }
		void SetSceneName(const std::string& name) { m_SceneName = name; }

	private:
		template<typename T>
		void OnComponentAdded(Entity entity, T& component);
	private:
		std::string m_SceneName = "Untitled";
		entt::registry m_Registry;
		uint32_t m_ViewportWidth = 0, m_ViewportHeight = 0;

		friend class Entity;
		friend class SceneSerializer;
		friend class SceneHierarchyPanel;
	};
}