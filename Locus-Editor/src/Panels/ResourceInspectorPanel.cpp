#include "ResourceInspectorPanel.h"

#include <ImGui/imgui.h>
#include <ImGui/imgui_internal.h>

#include "Widgets/Widgets.h"

namespace Locus
{
	extern std::filesystem::path g_SelectedResourcePath;

	void ResourceInspectorPanel::OnImGuiRender()
	{
		ImGuiWindowFlags windowFlags = ImGuiWindowFlags_TabBarAlignLeft | ImGuiWindowFlags_DockedWindowBorder;
		ImGui::Begin("Resource Inspector", false, windowFlags);

		if (MaterialManager::IsValid(g_SelectedResourcePath) && !g_SelectedResourcePath.empty())
			DrawMaterialInspector();

		ImGui::End();
	}

	void ResourceInspectorPanel::DrawMaterialInspector()
	{
		Ref<Material> material = MaterialHandle(g_SelectedResourcePath).Get();
		ImDrawList* drawList = ImGui::GetWindowDrawList();

		ImVec2 matPreviewSize = { 60.0f, 60.0f };
		ImVec2 topLeft = ImGui::GetCursorScreenPos();

		// --- Header ---
		// Temporary material display
		ImGui::InvisibleButton("##Spacer", matPreviewSize);
		if (material->m_AlbedoTexture)
		{
			drawList->AddImageRounded((ImTextureID)(uint64_t)material->m_AlbedoTexture.Get()->GetRendererID(),
				topLeft, { topLeft.x + matPreviewSize.x, topLeft.y + matPreviewSize.y }, { 0, 1 }, { 1, 0 }, ImGui::GetColorU32(0), matPreviewSize.x * 0.5f);
		}
		else
		{
			drawList->AddRectFilled(topLeft, { topLeft.x + matPreviewSize.x, topLeft.y + matPreviewSize.y },
				ImGui::GetColorU32(ToImVec4(material->m_Albedo)), matPreviewSize.x * 0.5f);
		}
		ImGui::SameLine();
		ImGui::Text(material->GetName().c_str());
		ImGui::SameLine(ImGui::GetContentRegionAvail().x - 75.0f);
		if (ImGui::Button("Locate", { 75.0f, 0.0f }))
			m_ProjectBrowserPanel->SetDirectory(material->GetPath().parent_path());

		ImGui::Separator();

		// --- Material shader ---
		// TODO: Implement
		Widgets::DrawControlLabel("Shader", { ImGui::GetContentRegionAvail().x * 0.5f, 0 });
		ImGui::SameLine();
		ImGui::Button("##Placeholder", { ImGui::GetContentRegionAvail().x, 0.0f });

		// Albedo
		Widgets::DrawTextureDropdown("Albedo Texture", material->m_AlbedoTexture);
		Widgets::DrawColorControl("Albedo", material->m_Albedo, { 1.0f, 1.0f, 1.0f, 1.0f });
		// Normal
		Widgets::DrawTextureDropdown("Normal Map", material->m_NormalMapTexture);
		// Metallic
		Widgets::DrawTextureDropdown("Metallic Texture", material->m_MetallicTexture);
		Widgets::DrawValueControl("Metallic", material->m_Metallic, 0.0f);
		// Roughness
		Widgets::DrawTextureDropdown("Roughness Texture", material->m_RoughnessTexture);
		Widgets::DrawValueControl("Roughness", material->m_Roughness, 0.5f);
		// AO
		Widgets::DrawTextureDropdown("AO Texture", material->m_AOTexture);
		Widgets::DrawValueControl("AO", material->m_AO, 1.0f);

	}
}
