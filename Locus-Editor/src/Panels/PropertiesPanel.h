// --- PropertiesPanel --------------------------------------------------------
// Entity properties panel.
#pragma once

#include <glm/glm.hpp>

#include <Locus.h>

namespace Locus
{
	class PropertiesPanel
	{
	public:
		PropertiesPanel() = default;
		PropertiesPanel(const Ref<Scene>& context);
		~PropertiesPanel() = default;

		void SetContext(const Ref<Scene>& context);

		void OnImGuiRender();

		Entity GetSelectedEntity() const { return m_SelectedEntity; }
		void SetSelectedEntity(Entity entity) { m_SelectedEntity = entity; }

	private:
		void DrawComponents(Entity entity);
		template<typename T, typename UIFunction>
		void DrawComponentUI(const std::string& name, Entity entity, UIFunction uiFunction);

		void DrawScriptFields(Entity entity, ScriptComponent& component);

		template<typename T>
		void CopyComponentToClipboard(Entity selectedEntity);
		void PasteComponent(Entity selectedEntity);

	private:
		Ref<Scene> m_ActiveScene;
		Entity m_SelectedEntity;

		float m_LabelWidth = 110.0f;

		// Textures
		Ref<Texture2D> m_ShowMoreIcon;
		Ref<Texture2D> m_FolderIcon;

		// Script
		std::vector<std::string> m_ScriptClasses;

		ComponentData m_ClipboardComponent;
		ComponentType m_ClipboardComponentType = ComponentType::None;

	public:
		friend class LocusEditorLayer;
	};
}
