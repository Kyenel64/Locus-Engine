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
		ImGui::PushStyleVar(ImGuiStyleVar_ButtonTextAlign, { 0.0f, 0.5f });
		ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 0.0f);
		ImGui::PushStyleColor(ImGuiCol_Button, LocusColors::Transparent);
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, LocusColors::Transparent);
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, LocusColors::Transparent);
		ImGui::Button(name.c_str(), { size.x, size.y });
		ImGui::PopStyleColor(3);
		ImGui::PopStyleVar(2);
	}

	void DrawBoolControl(const std::string& name, bool& changeValue, bool resetValue, float labelWidth, Ref<ScriptInstance> instance)
	{
		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, { 0.0f, ImGui::GetStyle().ItemSpacing.y });

		std::function<void(bool)> func = nullptr;
		if (instance)
			func = [=](bool val) { instance->SetFieldValue<bool>(name, val); };

		if (labelWidth == -1)
			labelWidth = ImGui::GetContentRegionAvail().x * 0.5f;

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

		ImGui::PopStyleVar();
	}

	void DrawCharControl(const std::string& name, char& changeValue, char resetValue, float labelWidth, float inputWidth, Ref<ScriptInstance> instance)
	{
		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, { 0.0f, ImGui::GetStyle().ItemSpacing.y });

		std::function<void(char)> func = nullptr;
		if (instance)
			func = [=](char val) { instance->SetFieldValue<char>(name, val); };

		if (labelWidth == -1)
			labelWidth = ImGui::GetContentRegionAvail().x * 0.5f;

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
		ImGui::PushItemWidth(inputWidth);
		if (ImGui::InputText("##Tag", buffer, sizeof(buffer), flags))
		{
			if (instance)
				CommandHistory::AddCommand(new ChangeFunctionValueCommand(func, buffer[0], changeValue));
			else
				CommandHistory::AddCommand(new ChangeValueCommand(buffer[0], changeValue));
		}

		if (!ImGui::TempInputIsActive(ImGui::GetActiveID()))
			Application::Get().GetImGuiLayer()->BlockEvents(true);
		else
			Application::Get().GetImGuiLayer()->BlockEvents(false);
		
		ImGui::PopItemWidth();

		ImGui::PopStyleVar();
	}

	void DrawStringControl(const std::string& name, std::string& changeValue, const std::string& resetValue, float labelWidth, float inputWidth)
	{
		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, { 0.0f, ImGui::GetStyle().ItemSpacing.y });

		if (labelWidth == -1)
			labelWidth = ImGui::GetContentRegionAvail().x * 0.5f;

		Widgets::DrawControlLabel(name, { labelWidth, 0.0f });

		ImGui::SameLine();

		if (ImGui::IsItemHovered())
		{
			ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);
			if (ImGui::IsMouseDoubleClicked(0))
				CommandHistory::AddCommand(new ChangeValueCommand(resetValue, changeValue));
		}

		// For some reason ImGui::InputText doesnt like a single char*
		char buffer[256];
		memset(buffer, 0, sizeof(buffer));
		strcpy_s(buffer, sizeof(buffer), changeValue.c_str());
		ImGuiInputTextFlags flags = ImGuiInputTextFlags_EnterReturnsTrue;
		std::string label = "##" + name;
		ImGui::PushItemWidth(inputWidth);
		if (ImGui::InputText(label.c_str(), buffer, sizeof(buffer), flags))
		{
				CommandHistory::AddCommand(new ChangeValueCommand(std::string(buffer), changeValue));
		}

		if (!ImGui::TempInputIsActive(ImGui::GetActiveID()))
			Application::Get().GetImGuiLayer()->BlockEvents(true);
		else
			Application::Get().GetImGuiLayer()->BlockEvents(false);

		ImGui::PopItemWidth();

		ImGui::PopStyleVar();
	}

	void DrawColorControl(const std::string& name, glm::vec4& changeValue, const glm::vec4& resetValue, float labelWidth, float inputWidth)
	{
		ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 0.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, { 0.0f, ImGui::GetStyle().ItemSpacing.y });

		if (labelWidth == -1)
			labelWidth = ImGui::GetContentRegionAvail().x * 0.5f;

		Widgets::DrawControlLabel("Color", { labelWidth, 0.0f });
		ImGui::SameLine();

		if (ImGui::IsItemHovered())
		{
			ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);
			if (ImGui::IsMouseDoubleClicked(0))
			{
				CommandHistory::AddCommand(new ChangeValueCommand(resetValue, changeValue));
			}
		}

		glm::vec4 color = changeValue;
		static glm::vec4 backupColor;
		ImGui::PushStyleColor(ImGuiCol_Button, ToImVec4(color));
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ToImVec4(color));
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ToImVec4(color));
		if (ImGui::Button("##Color", { inputWidth, 0.0f }))
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
				CommandHistory::AddCommand(new ChangeValueCommand(color, changeValue));
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

		ImGui::PopStyleVar(2);
	}

	void DrawVec2Control(const std::string& name, glm::vec2& changeValue, const glm::vec2& resetValue, float speed, const char* format,
		float labelWidth, float inputWidth, const glm::vec2& min, const glm::vec2& max, Ref<ScriptInstance> instance)
	{
		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, { 0.0f, ImGui::GetStyle().ItemSpacing.y });

		std::function<void(glm::vec2)> func = nullptr;
		if (instance)
			func = [=](glm::vec2 val) { instance->SetFieldValue<glm::vec2>(name, val); };

		if (labelWidth == -1)
			labelWidth = ImGui::GetContentRegionAvail().x * 0.5f - (20.0f + ImGui::GetStyle().FramePadding.x);

		glm::vec2 dragValues = changeValue;
		if (name == "Rotation")
			dragValues = glm::degrees(dragValues);

		ImGui::PushID(name.c_str());
		DrawControlLabel(name.c_str(), { labelWidth, 0 });
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

		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, { ImGui::GetStyle().ItemSpacing.x, 0.0f });
		ImGui::SameLine();
		DrawValueControl("X", changeValue.x, resetValue.x, speed, format, 0, inputWidth, min.x, max.x, instance);
		ImGui::PopStyleVar();

		DrawControlLabel("##Invisible", { labelWidth, 0 });
		ImGui::SameLine();
		DrawValueControl("Y", changeValue.y, resetValue.y, speed, format, 0, inputWidth, min.y, max.y, instance);

		ImGui::PopID();

		ImGui::PopStyleVar();
	}

	void DrawVec3Control(const std::string& name, glm::vec3& changeValue, const glm::vec3& resetValue, float speed, const char* format,
		float labelWidth, float inputWidth, const glm::vec3& min, const glm::vec3& max, Ref<ScriptInstance> instance)
	{
		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, { 0.0f, ImGui::GetStyle().ItemSpacing.y });

		std::function<void(glm::vec3)> func = nullptr;
		if (instance)
			func = [=](glm::vec3 val) { instance->SetFieldValue<glm::vec3>(name, val); };

		if (labelWidth == -1)
			labelWidth = ImGui::GetContentRegionAvail().x * 0.5f - (20.0f + ImGui::GetStyle().FramePadding.x);

		glm::vec3 dragValues = changeValue;
		if (name == "Rotation")
			dragValues = glm::degrees(dragValues);

		ImGui::PushID(name.c_str());
		DrawControlLabel(name.c_str(), {labelWidth, 0});
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

		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, { ImGui::GetStyle().ItemSpacing.x, 0.0f });
		ImGui::SameLine();
		DrawValueControl("X", changeValue.x, resetValue.x, speed, format, 0, inputWidth, min.x, max.x, instance);

		DrawControlLabel("##Invisible", { labelWidth, 0 });
		ImGui::SameLine();
		DrawValueControl("Y", changeValue.y, resetValue.y, speed, format, 0, inputWidth, min.y, max.y, instance);
		ImGui::PopStyleVar();

		DrawControlLabel("##Invisible", { labelWidth, 0 });
		ImGui::SameLine();
		DrawValueControl("Z", changeValue.z, resetValue.z, speed, format, 0, inputWidth, min.z, max.z, instance);

		ImGui::PopID();

		ImGui::PopStyleVar();
	}
	
	void Widgets::DrawCollisionGrid(const std::string& name, uint16_t& changeValue, uint16_t resetValue, float labelWidth, float inputWidth)
	{
		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, { 0.0f, ImGui::GetStyle().ItemSpacing.y });

		if (labelWidth == -1)
			labelWidth = ImGui::GetContentRegionAvail().x * 0.5f;

		Widgets::DrawControlLabel(name, { labelWidth, 0.0f });

		// Reset value when double clicking label.
		if (ImGui::IsItemHovered())
		{
			ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);
			if (ImGui::IsMouseDoubleClicked(0))
			{
				CommandHistory::AddCommand(new ChangeValueCommand(resetValue, changeValue));
			}
		}

		ImGui::SameLine();

		// --- Popup ---
		std::string label = "##" + name;
		std::string layers;
		uint16_t bitValues[] = { 0x0001, 0x0002, 0x0004, 0x0008,
								 0x0010, 0x0020, 0x0040, 0x0080,
								 0x0100, 0x0200, 0x0400, 0x0800,
								 0x1000, 0x2000, 0x4000, 0x8000 };

		// Button label
		if (changeValue == 0xFFFF)
		{
			layers.append("All Layers");
		}
		else if (changeValue == 0x0000)
		{
			layers.append("None");
		}
		else
		{
			for (int i = 0; i < 16; i++)
			{
				if ((changeValue & bitValues[i]) != 0)
				{
					layers.append("Layer " + std::to_string(i) + " ");
				}
			}
		}

		// Button
		ImGui::PushStyleColor(ImGuiCol_Button, ImGui::GetStyleColorVec4(ImGuiCol_FrameBg));
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImGui::GetStyleColorVec4(ImGuiCol_FrameBgHovered));
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImGui::GetStyleColorVec4(ImGuiCol_FrameBgActive));
		ImGui::PushID(label.c_str());
		if (ImGui::Button(layers.c_str(), {inputWidth, 0.0f}))
			ImGui::OpenPopup("CollisionPopup");
		ImGui::PopStyleColor(3);
		ImVec2 buttonSize = ImGui::GetItemRectSize();

		// Popup
		ImGui::PushStyleColor(ImGuiCol_Header, LocusColors::DarkGrey);
		ImGui::SetNextWindowPos({ ImGui::GetWindowPos().x + ImGui::GetCursorPosX() + labelWidth, ImGui::GetWindowPos().y + ImGui::GetCursorPosY() - ImGui::GetStyle().ItemSpacing.y });
		ImGui::SetNextWindowSize({ buttonSize.x, 200.0f });
		if (ImGui::BeginPopup("CollisionPopup"))
		{
			for (int i = 0; i < 16; i++)
			{
				std::string selectableLabel = "Layer " + std::to_string(i);
				bool selected = false;
				if ((changeValue & bitValues[i]) != 0)
					selected = true;
				if (ImGui::Selectable(selectableLabel.c_str(), selected, ImGuiSelectableFlags_DontClosePopups))
				{
					if (selected)
						CommandHistory::AddCommand(new ChangeValueCommand((uint16_t)(changeValue - bitValues[i]), changeValue));
					else
						CommandHistory::AddCommand(new ChangeValueCommand((uint16_t)(changeValue + bitValues[i]), changeValue));
				}
			}
			ImGui::EndPopup();
		}
		ImGui::PopStyleColor();

		ImGui::PopID();
		ImGui::PopStyleVar();
	}
}
