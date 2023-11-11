// --- SceneHierarchyPanel ----------------------------------------------------
// Scene hierarchy panel.
#pragma once

#include <glm/glm.hpp>

#include <Locus/Core/Core.h>
#include <Locus.h>

namespace Locus
{
	class SceneHierarchyPanel
	{
	public:
		SceneHierarchyPanel() = default;
		SceneHierarchyPanel(const Ref<Scene>& context);
		~SceneHierarchyPanel() = default;

		void SetContext(const Ref<Scene>& context);

		void OnImGuiRender();

		Entity GetSelectedEntity() const { return m_SelectedEntity; }
		void SetSelectedEntity(Entity entity) { m_SelectedEntity = entity; }
	private:
		void DrawEntityNode(Entity entity);

	private:
		Ref<Scene> m_ActiveScene;
		Entity m_SelectedEntity;

		// Textures
		Ref<Texture2D> m_PlusIcon;
	};
}