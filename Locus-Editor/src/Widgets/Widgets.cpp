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

	void DrawVec2Control(const std::string& name, float labelWidth, glm::vec2& changeValue, const glm::vec2& resetValue, Ref<ScriptInstance> instance, float speed, const char* format, float min, float max)
	{
		std::function<void(glm::vec2)> func = nullptr;
		if (instance)
			func = [=](glm::vec2 val) { instance->SetFieldValue<glm::vec2>(name, val); };

		ImGui::PushStyleColor(ImGuiCol_Button, LocusColors::Transparent);
		ImGui::PushStyleVar(ImGuiStyleVar_CellPadding, { 0.0f, 1.0f });

		glm::vec2 dragValues = changeValue;
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
					CommandHistory::AddCommand(new ChangeFunctionValueCommand(func, { resetValue.x, changeValue.y }, changeValue));
				else
					CommandHistory::AddCommand(new ChangeValueCommand({ resetValue.x, changeValue.y }, changeValue));
			}
				
		}
		ImGui::SameLine();
		ImGui::PushItemWidth(ImGui::GetContentRegionAvail().x);
		if (ImGui::DragFloat("##X", &dragValues.x, 0.1f, 0.0f, 0.0f, "%.2f"))
		{
			if (name == "Rotation")
			{
				if (instance)
					CommandHistory::AddCommand(new ChangeFunctionValueCommand(func, { glm::radians(dragValues.x), changeValue.y }, changeValue));
				else
					CommandHistory::AddCommand(new ChangeValueCommand({ glm::radians(dragValues.x), changeValue.y }, changeValue));
			}
			else
			{
				if (instance)
					CommandHistory::AddCommand(new ChangeFunctionValueCommand(func, { dragValues.x, changeValue.y }, changeValue));
				else
					CommandHistory::AddCommand(new ChangeValueCommand({ dragValues.x, changeValue.y }, changeValue));
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
					CommandHistory::AddCommand(new ChangeFunctionValueCommand(func, { changeValue.x, resetValue.y }, changeValue));
				else
					CommandHistory::AddCommand(new ChangeValueCommand({ changeValue.x, resetValue.y }, changeValue));
			}
		}
		ImGui::SameLine();
		ImGui::PushItemWidth(ImGui::GetContentRegionAvail().x);
		if (ImGui::DragFloat("##Y", &dragValues.y, 0.1f, 0.0f, 0.0f, "%.2f"))
		{
			if (name == "Rotation")
			{
				if (instance)
					CommandHistory::AddCommand(new ChangeFunctionValueCommand(func, { changeValue.x, glm::radians(dragValues.x) }, changeValue));
				else
					CommandHistory::AddCommand(new ChangeValueCommand({ changeValue.x, glm::radians(dragValues.x) }, changeValue));
			}
			else
			{
				if (instance)
					CommandHistory::AddCommand(new ChangeFunctionValueCommand(func, { changeValue.x, dragValues.y }, changeValue));
				else
					CommandHistory::AddCommand(new ChangeValueCommand({ changeValue.x, dragValues.y }, changeValue));
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

	void DrawVec3Control(const std::string& name, float labelWidth, glm::vec3& changeValue, const glm::vec3& resetValue, Ref<ScriptInstance> instance, float speed, const char* format, float min, float max)
	{
		std::function<void(glm::vec3)> func = nullptr;
		if (instance)
			func = [=](glm::vec3 val) { instance->SetFieldValue<glm::vec3>(name, val); };

		ImGui::PushStyleColor(ImGuiCol_Button, LocusColors::Transparent);
		ImGui::PushStyleVar(ImGuiStyleVar_CellPadding, { 0.0f, 1.0f });

		glm::vec3 dragValues = changeValue;
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
					CommandHistory::AddCommand(new ChangeFunctionValueCommand(func, { resetValue.x, changeValue.y, changeValue.z}, changeValue));
				else
					CommandHistory::AddCommand(new ChangeValueCommand({ resetValue.x, changeValue.y, changeValue.z }, changeValue));
			}
		}
		ImGui::SameLine();
		ImGui::PushItemWidth(ImGui::GetContentRegionAvail().x);
		if (ImGui::DragFloat("##X", &dragValues.x, speed, min, max, format))
		{
			if (name == "Rotation")
			{
				if (instance)
					CommandHistory::AddCommand(new ChangeFunctionValueCommand(func, { glm::radians(dragValues.x), changeValue.y, changeValue.z }, changeValue));
				else
					CommandHistory::AddCommand(new ChangeValueCommand({ glm::radians(dragValues.x), changeValue.y, changeValue.z }, changeValue));
			}
			else
			{
				if (instance)
					CommandHistory::AddCommand(new ChangeFunctionValueCommand(func, { dragValues.x, changeValue.y, changeValue.z }, changeValue));
				else
					CommandHistory::AddCommand(new ChangeValueCommand({ dragValues.x, changeValue.y, changeValue.z }, changeValue));
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
					CommandHistory::AddCommand(new ChangeFunctionValueCommand(func, { changeValue.x, resetValue.y, changeValue.z }, changeValue));
				else
					CommandHistory::AddCommand(new ChangeValueCommand({ changeValue.x, resetValue.y, changeValue.z }, changeValue));
			}
		}
		ImGui::SameLine();
		ImGui::PushItemWidth(ImGui::GetContentRegionAvail().x);
		if (ImGui::DragFloat("##Y", &dragValues.y, speed, min, max, format))
		{
			if (name == "Rotation")
			{
				if (instance)
					CommandHistory::AddCommand(new ChangeFunctionValueCommand(func, { changeValue.x, glm::radians(dragValues.y), changeValue.z }, changeValue));
				else
					CommandHistory::AddCommand(new ChangeValueCommand({ changeValue.x, glm::radians(dragValues.y), changeValue.z }, changeValue));
			}
			else
			{
				if (instance)
					CommandHistory::AddCommand(new ChangeFunctionValueCommand(func, { changeValue.x, dragValues.y, changeValue.z }, changeValue));
				else
					CommandHistory::AddCommand(new ChangeValueCommand({ changeValue.x, dragValues.y, changeValue.z }, changeValue));
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
					CommandHistory::AddCommand(new ChangeFunctionValueCommand(func, { changeValue.x, changeValue.y, resetValue.z }, changeValue));
				else
					CommandHistory::AddCommand(new ChangeValueCommand({ changeValue.x, changeValue.y, resetValue.z }, changeValue));
			}
		}
		ImGui::SameLine();
		ImGui::PushItemWidth(ImGui::GetContentRegionAvail().x);
		if (ImGui::DragFloat("##Z", &dragValues.z, speed, min, max, format))
		{
			if (name == "Rotation")
			{
				if (instance)
					CommandHistory::AddCommand(new ChangeFunctionValueCommand(func, { changeValue.x, changeValue.y, glm::radians(dragValues.z) }, changeValue));
				else
					CommandHistory::AddCommand(new ChangeValueCommand({ changeValue.x, changeValue.y, glm::radians(dragValues.z) }, changeValue));
			}
			else
			{
				if (instance)
					CommandHistory::AddCommand(new ChangeFunctionValueCommand(func, { changeValue.x, changeValue.y, dragValues.z }, changeValue));
				else
					CommandHistory::AddCommand(new ChangeValueCommand({ changeValue.x, changeValue.y, dragValues.z }, changeValue));
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
	
	void Widgets::DrawCollisionGrid(const std::string& name, float labelWidth, uint16_t& changeValue, uint16_t resetValue)
	{
		Widgets::DrawControlLabel(name, { labelWidth, 84.0f });
		ImGui::SameLine();

		// Reset value when double clicking label.
		if (ImGui::IsItemHovered())
		{
			ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);
			if (ImGui::IsMouseDoubleClicked(0))
			{
				CommandHistory::AddCommand(new ChangeValueCommand(resetValue, changeValue));
			}
		}

		// Draw grid
		ImGui::PushItemWidth(ImGui::GetContentRegionAvail().x);

		int rowCount = 8;
		if (ImGui::GetContentRegionAvail().x < 325.0f)
			rowCount = 4;
		float buttonWidth = 40.0f;
		float spacing = 2.0f;
		float windowWidth = rowCount * (buttonWidth + spacing) + (2 * spacing);
		float windowHeight = 2 * (buttonWidth + spacing) + (2 * spacing);

		uint16_t bitValues[] = { 0x0001, 0x0002, 0x0004, 0x0008, 
								 0x0010, 0x0020, 0x0040, 0x0080, 
								 0x0100, 0x0200, 0x0400, 0x0800,
								 0x1000, 0x2000, 0x4000, 0x8000 };

		ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 0.0f);
		ImGui::BeginChild(name.c_str(), { windowWidth, windowHeight}, true);
		ImGui::PushStyleVar(ImGuiStyleVar_CellPadding, { 0.0f, 2.0f });
		if (ImGui::BeginTable("split", rowCount, ImGuiTableFlags_NoSavedSettings))
		{
			for (uint16_t i = 0; i < 16; i++)
			{
				ImGui::TableNextColumn();

				bool selected;
				ImVec4 buttonCol;
				if ((changeValue & bitValues[i]) != 0)
				{
					selected = true;
					buttonCol = LocusColors::Orange;
				}
				else
				{
					selected = false;
					buttonCol = LocusColors::Grey;
				}

				std::string label = std::to_string(i);
				ImGui::PushStyleColor(ImGuiCol_Button, buttonCol);
				if (ImGui::Button(label.c_str(), { buttonWidth, buttonWidth }))
				{
					if (selected)
						CommandHistory::AddCommand(new ChangeValueCommand((uint16_t)(changeValue - bitValues[i]), changeValue));
					else
						CommandHistory::AddCommand(new ChangeValueCommand((uint16_t)(changeValue + bitValues[i]), changeValue));
				}
				ImGui::PopStyleColor();
			}
			ImGui::EndTable();
		}
		ImGui::PopStyleVar(2);
		ImGui::EndChild();

		ImGui::PopItemWidth();
	}

	bool Widgets::DrawImageButton(const std::string& name, uint32_t imageID, const glm::vec2& size, bool clickable)
	{
		std::string label = "##" + name;
		bool pressed = false;
		ImDrawList* draw_list = ImGui::GetWindowDrawList();
		// Center of window + center of button + image offset
		ImVec2 topLeft = { ImGui::GetCursorPosX() + ImGui::GetWindowPos().x + (size.x * 0.5f) - (size.y * 0.5f), ImGui::GetCursorPosY() + ImGui::GetWindowPos().y};
		ImVec2 botRight = { topLeft.x + size.y, topLeft.y + size.y };
		
		ImVec4 tint;
		if (clickable == true)
			tint = { 1.0f, 1.0f, 1.0f, 1.0f };
		else
			tint = { 0.7f, 0.7f, 0.7f, 1.0f };
		
		pressed = ImGui::Button(label.c_str(), {size.x, size.y});
		draw_list->AddImage((ImTextureID)(uint64_t)imageID, topLeft, botRight, { 0, 0 }, { 1, 1 }, ImGui::GetColorU32(tint));

		return pressed;
	}
}