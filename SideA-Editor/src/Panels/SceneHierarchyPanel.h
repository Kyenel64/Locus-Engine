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


		Entity GetSelectedEntity() const { return m_SelectedEntity; }
		void SetSelectedEntity(Entity entity) { m_SelectedEntity = entity; }
	private:
		void DrawEntityNode(Entity entity);
		void DrawComponents(Entity entity);

		template<typename T, typename UIFunction>
		void DrawComponentUI(const std::string& name, Entity entity, UIFunction uiFunction);
		void DrawVec3Control(const std::string& name, glm::vec3& values, float resetValue = 0.0f, float columnWidth = 100.0f);

	private:
		Ref<Scene> m_ActiveScene;
		Entity m_SelectedEntity;
	};
}