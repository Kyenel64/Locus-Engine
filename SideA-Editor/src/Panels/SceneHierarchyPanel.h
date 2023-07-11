#pragma once

#include "SideA/Core/Core.h"
#include "SideA/Scene/Scene.h"
#include "SideA/Scene/Entity.h"

namespace SideA
{
	class SceneHierarchyPanel
	{
	public:
		SceneHierarchyPanel() = default;
		SceneHierarchyPanel(const Ref<Scene>& context);

		void SetContext(const Ref<Scene>& context);

		void OnImGuiRender();
	private:
		void DrawEntityNode(Entity entity);
		void DrawComponents(Entity entity);
	private:
		Ref<Scene> m_Context;
		Entity m_SelectedEntity;
	};
}