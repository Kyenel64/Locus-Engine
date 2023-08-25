#pragma once

#include <string>

#include <glm/glm.hpp>
#include <ImGui/imgui.h>

namespace LocusUI
{
	void BeginWindow(const std::string& name, const glm::vec2& size, const glm::vec2& pos)
	{
		ImGuiWindowFlags testFlags = ImGuiWindowFlags_NoDecoration;
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, { 0.0f, 0.0f });
		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, { 0.0f, 0.0f });
		ImGui::PushStyleVar(ImGuiStyleVar_TabRounding, 0.0f);
		ImGui::PushStyleColor(ImGuiCol_TabActive, { 0.29f, 0.28f, 0.27f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_WindowBg, { 0.21f, 0.196f, 0.176f, 1.0f });
		ImGui::Begin(name.c_str(), false, testFlags);
		ImGui::SetWindowSize({ size.x, size.y });
		ImGui::SetWindowPos({ pos.x, pos.y });


		ImGui::BeginTabBar("TabBar");

		ImGui::BeginTabItem(name.c_str());

		std::string childName = name + "_Child";
		ImGui::PushStyleColor(ImGuiCol_ChildBg, { 0.29f, 0.28f, 0.27f, 1.0f });
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, { 0.0f, 0.0f });
		ImGui::BeginChild(childName.c_str(), ImGui::GetContentRegionAvail(), false);


	}

	void EndWindow()
	{
		ImGui::EndChild();
		ImGui::PopStyleColor();
		ImGui::PopStyleVar();

		ImGui::EndTabItem();

		ImGui::EndTabBar();
		ImGui::End();
		ImGui::PopStyleVar(3);
		ImGui::PopStyleColor(2);
	}
}