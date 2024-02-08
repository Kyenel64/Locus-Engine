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
		PropertiesPanel();
		~PropertiesPanel() = default;

		void SetScene(const Ref<Scene>& context);

		void OnImGuiRender();

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

		std::filesystem::path m_ProjectDirectory;

		// Textures
		Ref<Texture2D> m_MenuIcon;
		Ref<Texture2D> m_FolderIcon;

		// Script
		std::vector<std::string> m_ScriptClasses;

		ComponentData m_ClipboardComponent;
		ComponentType m_ClipboardComponentType = ComponentType::None;

	public:
		friend class LocusEditorLayer;
	};
}
