// --- PropertiesPanel --------------------------------------------------------
// Entity properties panel.
#pragma once

#include <glm/glm.hpp>

#include <Locus/Core/Core.h>
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

		void DrawControlLabel(const std::string& name, const glm::vec2& size = { 110.0f, 0.0f });
		void DrawFloatControl(const std::string& name, float& changeValue, float resetValue = 1.0f);
		void DrawIntControl(const std::string& name, int& changeValue, int resetValue = 1);
		void DrawUInt16Control(const std::string& name, uint16_t& changeValue, uint16_t resetValue = 1);
		void DrawColorControl(const std::string& name, glm::vec4& colorValue);
		void DrawVec2Control(const std::string& name, glm::vec2& values, float resetValue = 0.0f);
		void DrawVec3Control(const std::string& name, glm::vec3& values, float resetValue = 0.0f);
		
		template<typename T>
		void CopyComponentToClipboard(Entity selectedEntity);
		void PasteComponent(Entity selectedEntity);

	private:
		Ref<Scene> m_ActiveScene;
		Entity m_SelectedEntity;

		// Textures
		Ref<Texture2D> m_ShowMoreButton;
		Ref<Texture2D> m_FolderIcon;

		ComponentsList m_ClipboardComponent;
		ComponentType m_ClipboardComponentType = ComponentType::None;
	};
}