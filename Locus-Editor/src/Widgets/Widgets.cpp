#include "Widgets.h"

#include <ImGui/imgui.h>
#include <ImGui/imgui_internal.h>
#include <glm/gtc/type_ptr.hpp>


#include "Command/CommandHistory.h"
#include "Command/ValueCommands.h"

namespace Locus::Widgets
{
	void DrawControlLabel(const std::string& name, const glm::vec2& size)
	{
		ImGui::PushStyleColor(ImGuiCol_Button, LocusColors::Transparent);
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, LocusColors::Transparent);
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, LocusColors::Transparent);
		ImGui::Button(name.c_str(), { size.x, size.y });
		ImGui::PopStyleColor(3);
	}

	void DrawBoolControl(const std::string& name, float labelWidth, bool& changeValue, bool resetValue)
	{
		Widgets::DrawControlLabel(name, { labelWidth, 0.0f });
		ImGui::SameLine();

		if (ImGui::IsItemHovered())
		{
			ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);
			if (ImGui::IsMouseDoubleClicked(0))
				CommandHistory::AddCommand(new ChangeValueCommand(resetValue, changeValue));
		}

		bool val = changeValue;
		std::string label = "##" + name;
		ImGui::PushItemWidth(ImGui::GetContentRegionAvail().x);
		if (ImGui::Checkbox(label.c_str(), &val))
			CommandHistory::AddCommand(new ChangeValueCommand(val, changeValue));
		ImGui::PopItemWidth();
	}

	void DrawFieldBoolControl(const std::string& name, float labelWidth, bool changeValue, bool resetValue, Ref<ScriptInstance> instance)
	{
		std::function<void(bool)> func = [=](bool val) { instance->SetFieldValue<bool>(name, val); };
		Widgets::DrawControlLabel(name, { labelWidth, 0.0f });
		ImGui::SameLine();

		if (ImGui::IsItemHovered())
		{
			ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);
			if (ImGui::IsMouseDoubleClicked(0))
				CommandHistory::AddCommand(new ChangeFunctionValueCommand(func, resetValue, changeValue));
		}

		bool val = changeValue;
		std::string label = "##" + name;
		ImGui::PushItemWidth(ImGui::GetContentRegionAvail().x);
		if (ImGui::Checkbox(label.c_str(), &val))
			CommandHistory::AddCommand(new ChangeFunctionValueCommand(func, val, changeValue));
		ImGui::PopItemWidth();
	}

	void DrawCharControl(const std::string& name, float labelWidth, char& changeValue, char resetValue)
	{
		Widgets::DrawControlLabel(name, { labelWidth, 0.0f });
		ImGui::SameLine();

		if (ImGui::IsItemHovered())
		{
			ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);
			if (ImGui::IsMouseDoubleClicked(0))
				CommandHistory::AddCommand(new ChangeValueCommand(resetValue, changeValue));
		}

		// For some reason ImGui::InputText doesnt like a single char*
		char buffer[2];
		memset(buffer, 0, sizeof(buffer));
		buffer[0] = changeValue;
		std::string label = "##" + name;
		ImGuiInputTextFlags flags = ImGuiInputTextFlags_EnterReturnsTrue;
		ImGui::PushItemWidth(ImGui::GetContentRegionAvail().x);
		if (ImGui::InputText("##Tag", buffer, sizeof(buffer), flags))
			CommandHistory::AddCommand(new ChangeValueCommand(buffer[0], changeValue));
		
		ImGui::PopItemWidth();
	}

	void DrawFieldCharControl(const std::string& name, float labelWidth, char changeValue, char resetValue, Ref<ScriptInstance> instance)
	{
		std::function<void(char)> func = [=](char val) { instance->SetFieldValue<char>(name, val); };

		Widgets::DrawControlLabel(name, { labelWidth, 0.0f });
		ImGui::SameLine();

		if (ImGui::IsItemHovered())
		{
			ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);
			if (ImGui::IsMouseDoubleClicked(0))
				CommandHistory::AddCommand(new ChangeFunctionValueCommand(func, resetValue, changeValue));
		}

		char buffer[2];
		memset(buffer, 0, sizeof(buffer));
		buffer[0] = changeValue;
		std::string label = "##" + name;
		ImGuiInputTextFlags flags = ImGuiInputTextFlags_EnterReturnsTrue;
		ImGui::PushItemWidth(ImGui::GetContentRegionAvail().x);
		if (ImGui::InputText("##Tag", buffer, sizeof(buffer), flags))
			CommandHistory::AddCommand(new ChangeFunctionValueCommand(func, buffer[0], changeValue));

		ImGui::PopItemWidth();
	}

	void DrawColorControl(const std::string& name, glm::vec4& colorValue, float labelWidth)
	{
		Widgets::DrawControlLabel("Color", { labelWidth, 0.0f });
		ImGui::SameLine();

		glm::vec4 color = colorValue;
		static glm::vec4 backupColor;
		ImGui::PushStyleColor(ImGuiCol_Button, ToImVec4(color));
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ToImVec4(color));
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ToImVec4(color));
		if (ImGui::Button("##Color", { ImGui::GetContentRegionAvail().x, 0.0f }))
		{
			ImGui::OpenPopup("mypicker");
			backupColor = color;
		}
		ImGui::PopStyleColor(3);
		if (ImGui::IsItemHovered())
		{
			ImGui::ColorTooltip("Color", glm::value_ptr(color), ImGuiColorEditFlags_None);
		}
		if (ImGui::BeginPopup("mypicker"))
		{
			ImGui::Text("Color");
			ImGui::Separator();
			if (ImGui::ColorPicker4("##picker", glm::value_ptr(color), ImGuiColorEditFlags_NoSidePreview | ImGuiColorEditFlags_NoSmallPreview))
				CommandHistory::AddCommand(new ChangeValueCommand(color, colorValue));
			ImGui::SameLine();

			ImGui::BeginGroup(); // Lock X position
			ImGui::Text("Current");
			ImGui::ColorButton("##current", ToImVec4(color), ImGuiColorEditFlags_NoPicker | ImGuiColorEditFlags_AlphaPreviewHalf, ImVec2(60, 40));
			ImGui::Text("Previous");
			if (ImGui::ColorButton("##previous", ToImVec4(backupColor), ImGuiColorEditFlags_NoPicker | ImGuiColorEditFlags_AlphaPreviewHalf, ImVec2(60, 40)))
				color = backupColor;
			ImGui::EndGroup();
			ImGui::EndPopup();
		}
	}

	void DrawVec2Control(const std::string& name, glm::vec2& values, float labelWidth, float resetValue, float speed, const char* format, float min, float max)
	{
		ImGui::PushStyleColor(ImGuiCol_Button, LocusColors::Transparent);
		ImGui::PushStyleVar(ImGuiStyleVar_CellPadding, { 0.0f, 1.0f });

		glm::vec2 dragValues = values;
		if (name == "Rotation")
			dragValues = glm::degrees(dragValues);

		Widgets::DrawControlLabel(name.c_str(), { labelWidth, 0.0f });

		ImGui::SameLine();

		ImGui::BeginTable("Vec3Control", 2);

		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, { 0.0f, 0.0f });
		// X
		ImGui::TableNextColumn();
		ImGui::Button("X", { 0.0f, 0.0f });
		if (ImGui::IsItemHovered())
		{
			ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);
			if (ImGui::IsMouseDoubleClicked(0))
				CommandHistory::AddCommand(new ChangeValueCommand(resetValue, values.x));
		}
		ImGui::SameLine();
		ImGui::PushItemWidth(ImGui::GetContentRegionAvail().x);
		if (ImGui::DragFloat("##X", &dragValues.x, 0.1f, 0.0f, 0.0f, "%.2f"))
		{
			if (name == "Rotation")
				CommandHistory::AddCommand(new ChangeValueCommand(glm::radians(dragValues.x), values.x));
			else
				CommandHistory::AddCommand(new ChangeValueCommand(dragValues.x, values.x));
		}
		if (ImGui::IsItemHovered())
			ImGui::SetMouseCursor(ImGuiMouseCursor_ResizeEW);
		ImGui::PopItemWidth();

		// Y
		ImGui::TableNextColumn();
		ImGui::Button("Y", { 0.0f, 0.0f });
		if (ImGui::IsItemHovered())
		{
			ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);
			if (ImGui::IsMouseDoubleClicked(0))
				CommandHistory::AddCommand(new ChangeValueCommand(resetValue, values.y));
		}
		ImGui::SameLine();
		ImGui::PushItemWidth(ImGui::GetContentRegionAvail().x);
		if (ImGui::DragFloat("##Y", &dragValues.y, 0.1f, 0.0f, 0.0f, "%.2f"))
		{
			if (name == "Rotation")
				CommandHistory::AddCommand(new ChangeValueCommand(glm::radians(dragValues.y), values.y));
			else
				CommandHistory::AddCommand(new ChangeValueCommand(dragValues.y, values.y));
		}
		if (ImGui::IsItemHovered())
			ImGui::SetMouseCursor(ImGuiMouseCursor_ResizeEW);
		ImGui::PopItemWidth();

		ImGui::PopStyleVar();

		ImGui::EndTable();
		ImGui::PopStyleColor();
		ImGui::PopStyleVar();
	}

	bool DrawVec3Control(const std::string& name, glm::vec3& values, float labelWidth, float resetValue, float speed, const char* format, float min, float max)
	{
		bool isChanged = false;
		ImGui::PushStyleColor(ImGuiCol_Button, LocusColors::Transparent);
		ImGui::PushStyleVar(ImGuiStyleVar_CellPadding, { 0.0f, 1.0f });

		glm::vec3 dragValues = values;
		if (name == "Rotation")
			dragValues = glm::degrees(dragValues);

		Widgets::DrawControlLabel(name.c_str(), { labelWidth, 0.0f });

		ImGui::SameLine();

		ImGui::BeginTable("Vec3Control", 3);

		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, { 0.0f, 0.0f });
		// X
		ImGui::TableNextColumn();
		ImGui::Button("X", { 0.0f, 0.0f });
		if (ImGui::IsItemHovered())
		{
			ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);
			if (ImGui::IsMouseDoubleClicked(0))
			{
				CommandHistory::AddCommand(new ChangeValueCommand(resetValue, values.x));
				isChanged = true;
			}
		}
		ImGui::SameLine();
		ImGui::PushItemWidth(ImGui::GetContentRegionAvail().x);
		if (ImGui::DragFloat("##X", &dragValues.x, speed, min, max, format))
		{
			if (name == "Rotation")
			{
				CommandHistory::AddCommand(new ChangeValueCommand(glm::radians(dragValues.x), values.x));
				isChanged = true;
			}
			else
			{
				CommandHistory::AddCommand(new ChangeValueCommand(dragValues.x, values.x));
				isChanged = true;
			}
		}
		if (ImGui::IsItemHovered())
			ImGui::SetMouseCursor(ImGuiMouseCursor_ResizeEW);
		ImGui::PopItemWidth();

		// Y
		ImGui::TableNextColumn();
		ImGui::Button("Y", { 0.0f, 0.0f });
		if (ImGui::IsItemHovered())
		{
			ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);
			if (ImGui::IsMouseDoubleClicked(0))
			{
				CommandHistory::AddCommand(new ChangeValueCommand(resetValue, values.y));
				isChanged = true;
			}
		}
		ImGui::SameLine();
		ImGui::PushItemWidth(ImGui::GetContentRegionAvail().x);
		if (ImGui::DragFloat("##Y", &dragValues.y, speed, min, max, format))
		{
			if (name == "Rotation")
			{
				CommandHistory::AddCommand(new ChangeValueCommand(glm::radians(dragValues.y), values.y));
				isChanged = true;
			}
			else
			{
				CommandHistory::AddCommand(new ChangeValueCommand(dragValues.y, values.y));
				isChanged = true;
			}
		}
		if (ImGui::IsItemHovered())
			ImGui::SetMouseCursor(ImGuiMouseCursor_ResizeEW);
		ImGui::PopItemWidth();

		// Z
		ImGui::TableNextColumn();
		ImGui::Button("Z", { 0.0f, 0.0f });
		if (ImGui::IsItemHovered())
		{
			ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);
			if (ImGui::IsMouseDoubleClicked(0))
			{
				CommandHistory::AddCommand(new ChangeValueCommand(resetValue, values.z));
				isChanged = true;
			}
		}
		ImGui::SameLine();
		ImGui::PushItemWidth(ImGui::GetContentRegionAvail().x);
		if (ImGui::DragFloat("##Z", &dragValues.z, speed, min, max, format))
		{
			if (name == "Rotation")
			{
				CommandHistory::AddCommand(new ChangeValueCommand(glm::radians(dragValues.z), values.z));
				isChanged = true;
			}
			else
			{
				CommandHistory::AddCommand(new ChangeValueCommand(dragValues.z, values.z));
				isChanged = true;
			}
		}
		if (ImGui::IsItemHovered())
			ImGui::SetMouseCursor(ImGuiMouseCursor_ResizeEW);
		ImGui::PopItemWidth();

		ImGui::PopStyleVar();

		ImGui::EndTable();
		ImGui::PopStyleColor();
		ImGui::PopStyleVar();

		if (Input::IsKeyPressed(Key::Z) && Input::IsKeyPressed(Key::LeftControl))
			isChanged = true;
		return isChanged;
	}
}