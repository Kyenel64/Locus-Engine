#pragma once

#include "SideA/Core/Core.h"
#include "SideA/Scene/Scene.h"
#include "SideA/Scene/Entity.h"

#include "glm/glm.hpp"

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
		void DrawVec3Control(const std::string& name, glm::vec3& values, float columnWidth = 100.0f);
	private:
		Ref<Scene> m_Context;
		Entity m_SelectedEntity;
	};
}