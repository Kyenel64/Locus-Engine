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

		void SetScene(const Ref<Scene>& context);

		void OnImGuiRender();

	private:
		void DrawEntityNode(Entity entity);

	private:
		Ref<Scene> m_ActiveScene;

		// Textures
		Ref<Texture2D> m_PlusIcon;
	};
}
