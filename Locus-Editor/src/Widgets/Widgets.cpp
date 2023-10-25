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

	void DrawBoolControl(const std::string& name, float labelWidth, bool& changeValue, bool resetValue, Ref<ScriptInstance> instance)
	{
		std::function<void(bool)> func = nullptr;
		if (instance)
			func = [=](bool val) { instance->SetFieldValue<bool>(name, val); };

		Widgets::DrawControlLabel(name, { labelWidth, 0.0f });
		ImGui::SameLine();

		if (ImGui::IsItemHovered())
		{
			ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);
			if (ImGui::IsMouseDoubleClicked(0))
			{
				if (instance)
					CommandHistory::AddCommand(new ChangeFunctionValueCommand(func, resetValue, changeValue));
				else
					CommandHistory::AddCommand(new ChangeValueCommand(resetValue, changeValue));
			}
		}

		bool val = changeValue;
		std::string label = "##" + name;
		ImGui::PushItemWidth(ImGui::GetContentRegionAvail().x);
		if (ImGui::Checkbox(label.c_str(), &val))
		{
			if (instance)
				CommandHistory::AddCommand(new ChangeFunctionValueCommand(func, val, changeValue));
			else
				CommandHistory::AddCommand(new ChangeValueCommand(val, changeValue));
		}
		ImGui::PopItemWidth();
	}

	void DrawCharControl(const std::string& name, float labelWidth, char& changeValue, char resetValue, Ref<ScriptInstance> instance)
	{
		std::function<void(char)> func = nullptr;
		if (instance)
			func = [=](char val) { instance->SetFieldValue<char>(name, val); };

		Widgets::DrawControlLabel(name, { labelWidth, 0.0f });
		ImGui::SameLine();

		if (ImGui::IsItemHovered())
		{
			ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);
			if (ImGui::IsMouseDoubleClicked(0))
			{
				if (instance)
					CommandHistory::AddCommand(new ChangeFunctionValueCommand(func, resetValue, changeValue));
				else
					CommandHistory::AddCommand(new ChangeValueCommand(resetValue, changeValue));
			}
		}

		// For some reason ImGui::InputText doesnt like a single char*
		char buffer[2];
		memset(buffer, 0, sizeof(buffer));
		buffer[0] = changeValue;
		std::string label = "##" + name;
		ImGuiInputTextFlags flags = ImGuiInputTextFlags_EnterReturnsTrue;
		ImGui::PushItemWidth(ImGui::GetContentRegionAvail().x);
		if (ImGui::InputText("##Tag", buffer, sizeof(buffer), flags))
		{
			if (instance)
				CommandHistory::AddCommand(new ChangeFunctionValueCommand(func, buffer[0], changeValue));
			else
				CommandHistory::AddCommand(new ChangeValueCommand(buffer[0], changeValue));
		}
		
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

	void DrawVec2Control(const std::string& name, float labelWidth, glm::vec2& values, const glm::vec2& resetValue, Ref<ScriptInstance> instance, float speed, const char* format, float min, float max)
	{
		std::function<void(glm::vec2)> func = nullptr;
		if (instance)
			func = [=](glm::vec2 val) { instance->SetFieldValue<glm::vec2>(name, val); };

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
			{
				if (instance)
					CommandHistory::AddCommand(new ChangeFunctionValueCommand(func, { resetValue.x, values.y }, values));
				else
					CommandHistory::AddCommand(new ChangeValueCommand({ resetValue.x, values.y }, values));
			}
				
		}
		ImGui::SameLine();
		ImGui::PushItemWidth(ImGui::GetContentRegionAvail().x);
		if (ImGui::DragFloat("##X", &dragValues.x, 0.1f, 0.0f, 0.0f, "%.2f"))
		{
			if (name == "Rotation")
			{
				if (instance)
					CommandHistory::AddCommand(new ChangeFunctionValueCommand(func, { glm::radians(dragValues.x), values.y }, values));
				else
					CommandHistory::AddCommand(new ChangeValueCommand({ glm::radians(dragValues.x), values.y }, values));
			}
			else
			{
				if (instance)
					CommandHistory::AddCommand(new ChangeFunctionValueCommand(func, { dragValues.x, values.y }, values));
				else
					CommandHistory::AddCommand(new ChangeValueCommand({ dragValues.x, values.y }, values));
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
				if (instance)
					CommandHistory::AddCommand(new ChangeFunctionValueCommand(func, { values.x, resetValue.y }, values));
				else
					CommandHistory::AddCommand(new ChangeValueCommand({ values.x, resetValue.y }, values));
			}
		}
		ImGui::SameLine();
		ImGui::PushItemWidth(ImGui::GetContentRegionAvail().x);
		if (ImGui::DragFloat("##Y", &dragValues.y, 0.1f, 0.0f, 0.0f, "%.2f"))
		{
			if (name == "Rotation")
			{
				if (instance)
					CommandHistory::AddCommand(new ChangeFunctionValueCommand(func, { values.x, glm::radians(dragValues.x) }, values));
				else
					CommandHistory::AddCommand(new ChangeValueCommand({ values.x, glm::radians(dragValues.x) }, values));
			}
			else
			{
				if (instance)
					CommandHistory::AddCommand(new ChangeFunctionValueCommand(func, { values.x, dragValues.y }, values));
				else
					CommandHistory::AddCommand(new ChangeValueCommand({ values.x, dragValues.y }, values));
			}
		}
		if (ImGui::IsItemHovered())
			ImGui::SetMouseCursor(ImGuiMouseCursor_ResizeEW);
		ImGui::PopItemWidth();

		ImGui::PopStyleVar();

		ImGui::EndTable();
		ImGui::PopStyleColor();
		ImGui::PopStyleVar();
	}

	void DrawVec3Control(const std::string& name, float labelWidth, glm::vec3& values, const glm::vec3& resetValue, Ref<ScriptInstance> instance, float speed, const char* format, float min, float max)
	{
		std::function<void(glm::vec3)> func = nullptr;
		if (instance)
			func = [=](glm::vec3 val) { instance->SetFieldValue<glm::vec3>(name, val); };

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
				if (instance)
					CommandHistory::AddCommand(new ChangeFunctionValueCommand(func, { resetValue.x, values.y, values.z}, values));
				else
					CommandHistory::AddCommand(new ChangeValueCommand({ resetValue.x, values.y, values.z }, values));
			}
		}
		ImGui::SameLine();
		ImGui::PushItemWidth(ImGui::GetContentRegionAvail().x);
		if (ImGui::DragFloat("##X", &dragValues.x, speed, min, max, format))
		{
			if (name == "Rotation")
			{
				if (instance)
					CommandHistory::AddCommand(new ChangeFunctionValueCommand(func, { glm::radians(dragValues.x), values.y, values.z }, values));
				else
					CommandHistory::AddCommand(new ChangeValueCommand({ glm::radians(dragValues.x), values.y, values.z }, values));
			}
			else
			{
				if (instance)
					CommandHistory::AddCommand(new ChangeFunctionValueCommand(func, { dragValues.x, values.y, values.z }, values));
				else
					CommandHistory::AddCommand(new ChangeValueCommand({ dragValues.x, values.y, values.z }, values));
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
				if (instance)
					CommandHistory::AddCommand(new ChangeFunctionValueCommand(func, { values.x, resetValue.y, values.z }, values));
				else
					CommandHistory::AddCommand(new ChangeValueCommand({ values.x, resetValue.y, values.z }, values));
			}
		}
		ImGui::SameLine();
		ImGui::PushItemWidth(ImGui::GetContentRegionAvail().x);
		if (ImGui::DragFloat("##Y", &dragValues.y, speed, min, max, format))
		{
			if (name == "Rotation")
			{
				if (instance)
					CommandHistory::AddCommand(new ChangeFunctionValueCommand(func, { values.x, glm::radians(dragValues.y), values.z }, values));
				else
					CommandHistory::AddCommand(new ChangeValueCommand({ values.x, glm::radians(dragValues.y), values.z }, values));
			}
			else
			{
				if (instance)
					CommandHistory::AddCommand(new ChangeFunctionValueCommand(func, { values.x, dragValues.y, values.z }, values));
				else
					CommandHistory::AddCommand(new ChangeValueCommand({ values.x, dragValues.y, values.z }, values));
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
				if (instance)
					CommandHistory::AddCommand(new ChangeFunctionValueCommand(func, { values.x, values.y, resetValue.z }, values));
				else
					CommandHistory::AddCommand(new ChangeValueCommand({ values.x, values.y, resetValue.z }, values));
			}
		}
		ImGui::SameLine();
		ImGui::PushItemWidth(ImGui::GetContentRegionAvail().x);
		if (ImGui::DragFloat("##Z", &dragValues.z, speed, min, max, format))
		{
			if (name == "Rotation")
			{
				if (instance)
					CommandHistory::AddCommand(new ChangeFunctionValueCommand(func, { values.x, values.y, glm::radians(dragValues.z) }, values));
				else
					CommandHistory::AddCommand(new ChangeValueCommand({ values.x, values.y, glm::radians(dragValues.z) }, values));
			}
			else
			{
				if (instance)
					CommandHistory::AddCommand(new ChangeFunctionValueCommand(func, { values.x, values.y, dragValues.z }, values));
				else
					CommandHistory::AddCommand(new ChangeValueCommand({ values.x, values.y, dragValues.z }, values));
			}
		}
		if (ImGui::IsItemHovered())
			ImGui::SetMouseCursor(ImGuiMouseCursor_ResizeEW);
		ImGui::PopItemWidth();

		ImGui::PopStyleVar();

		ImGui::EndTable();
		ImGui::PopStyleColor();
		ImGui::PopStyleVar();
	}
}