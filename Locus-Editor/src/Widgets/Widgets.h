// --- Widgets ----------------------------------------------------------------
// Locus editor widgets. 
// Each widget can take in a ScriptInstance if displaying for a script field
// during runtime of the editor.
#pragma once

#include <string>

#include <glm/glm.hpp>

#include <Locus.h>

namespace Locus::Widgets
{
	// Widget to display name on the left hand side of each property in the properties panel.
	void DrawControlLabel(const std::string& name, const glm::vec2& size = { 110.0f, 0.0f });

	// Draws the label on the left hand side and a switch on the right.
	void DrawBoolControl(const std::string& name, float labelWidth, bool& changeValue, bool resetValue = false, Ref<ScriptInstance> instance = nullptr);

	// Draws the label on the left hand side and a text input for chars on the right.
	void DrawCharControl(const std::string& name, float labelWidth, char& changeValue, char resetValue, Ref<ScriptInstance> instance = nullptr);

	// Draws the label on the left hand side and a color box on the right.
	// Clicking on the color box will open a pop up for color control.
	void DrawColorControl(const std::string& name, float labelWidth, glm::vec4& changeValue, const glm::vec4& resetValue);

	// Draws the label on the left hand side and two float controls labeled x and y on the right.
	void DrawVec2Control(const std::string& name, float labelWidth, glm::vec2& changeValue, const glm::vec2& resetValue = glm::vec2(1.0f), 
		Ref<ScriptInstance> instance = nullptr, float speed = 0.1f, const char* format = "%.3f", float min = 0.0f, float max = 0.0f);

	// Draws the label on the left hand side and three float controls labeled x, y, and z on the right.
	void DrawVec3Control(const std::string& name, float labelWidth, glm::vec3& changeValue, const glm::vec3& resetValue = glm::vec3(1.0f), 
		Ref<ScriptInstance> instance = nullptr, float speed = 0.1f, const char* format = "%.3f", float min = 0.0f, float max = 0.0f);

	// Draws a 2x8 grid of buttons for collision filters. 
	void DrawCollisionGrid(const std::string& name, float labelWidth, uint16_t& changeValue, uint16_t resetValue);

	// Draws an image button. The image is centered within the button. 
	bool DrawImageButton(const std::string& name, uint32_t imageID, const glm::vec2& size, bool clickable = true);



	// Templates can not be used in cpp file.
	
	// Control widget for floats, doubles, int16_t, int, int64_t, uint16_t, uint32_t, uint64_t.
	// If instance != nullptr, the widget is only displayed during runtime for scripts. (Weird API. Think of fix.)
	// If dragClamp == true, slider is clamped to the following ranges.
	template<typename T>
	inline void DrawValueControl(const std::string& name, float labelWidth, T& changeValue, T resetValue = 1.0f, Ref<ScriptInstance> instance = nullptr, 
		float speed = 0.1f, const char* format = nullptr, bool dragClamp = false, T min = 0.0f, T max = 0.0f)
	{
		std::function<void(T)> func = nullptr;
		if (instance)
			func = [=](T val) { instance->SetFieldValue<T>(name, val); };

		Widgets::DrawControlLabel(name, { labelWidth, 0.0f });
		ImGui::SameLine();

		// Reset value when double clicking label.
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

		// Set ImGuiDataType from template datatype
		ImGuiDataType dataType;
		if (typeid(T) == typeid(float))
			dataType = ImGuiDataType_Float;
		else if (typeid(T) == typeid(double))
			dataType = ImGuiDataType_Double;
		else if (typeid(T) == typeid(int16_t))
			dataType = ImGuiDataType_S16;
		else if (typeid(T) == typeid(int))
			dataType = ImGuiDataType_S32;
		else if (typeid(T) == typeid(int64_t))
			dataType = ImGuiDataType_S64;
		else if (typeid(T) == typeid(uint16_t))
			dataType = ImGuiDataType_U16;
		else if (typeid(T) == typeid(uint32_t))
			dataType = ImGuiDataType_U32;
		else if (typeid(T) == typeid(uint64_t))
			dataType = ImGuiDataType_U64;
		else
			LOCUS_CORE_ASSERT(false, "Datatype not supported!");

		// If format is not defined, set a default format.
		if (!format)
		{
			if (typeid(T) == typeid(float) || typeid(T) == typeid(double))
				format = "%.3f";
			else if (typeid(T) == typeid(uint16_t) || typeid(T) == typeid(uint32_t) || typeid(T) == typeid(uint64_t))
				format = "%u";
			else
				format = "%i";
		}

		// Draw slider
		T dragVal = changeValue;
		std::string label = "##" + name;
		ImGui::PushItemWidth(ImGui::GetContentRegionAvail().x);
		if (ImGui::DragScalar(label.c_str(), dataType, &dragVal, speed, dragClamp ? &min : NULL, dragClamp ? &max : NULL, format))
		{
			if (instance)
				CommandHistory::AddCommand(new ChangeFunctionValueCommand(func, dragVal, changeValue));
			else
				CommandHistory::AddCommand(new ChangeValueCommand(dragVal, changeValue));
		}
		ImGui::PopItemWidth();
		if (ImGui::IsItemHovered())
			ImGui::SetMouseCursor(ImGuiMouseCursor_ResizeEW);
	}
}