// --- Widgets ----------------------------------------------------------------
// Locus editor widgets.
// Each control widget has a respective field widget which is used during
// runtime in the editor. 
#pragma once

#include <string>

#include <glm/glm.hpp>

#include <Locus.h>

namespace Locus::Widgets
{
	// Widget to display name on the left hand side of each property in the properties panel.
	void DrawControlLabel(const std::string& name, const glm::vec2& size = { 110.0f, 0.0f });

	void DrawBoolControl(const std::string& name, float labelWidth, bool& changeValue, bool resetValue = false);
	void DrawFieldBoolControl(const std::string& name, float labelWidth, bool changeValue, bool resetValue, Ref<ScriptInstance> instance);

	void DrawColorControl(const std::string& name, glm::vec4& colorValue, float labelWidth);
	void DrawVec2Control(const std::string& name, glm::vec2& values, float labelWidth, float resetValue = 1.0f, float speed = 0.1f, const char* format = "%.3f", float min = 0.0f, float max = 0.0f);
	bool DrawVec3Control(const std::string& name, glm::vec3& values, float labelWidth, float resetValue = 1.0f, float speed = 0.1f, const char* format = "%.3f", float min = 0.0f, float max = 0.0f);



	// Templates wont work in cpp file. 
	template<typename T>
	void DrawValueControl(const std::string& name, float labelWidth, T& changeValue, T resetValue = 1.0f, float speed = 0.1f, const char* format = nullptr, bool dragClamp = false, T min = 0.0f, T max = 0.0f)
	{
		Widgets::DrawControlLabel(name, { labelWidth, 0.0f });
		ImGui::SameLine();

		// Reset value when double clicking label.
		if (ImGui::IsItemHovered())
		{
			ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);
			if (ImGui::IsMouseDoubleClicked(0))
				CommandHistory::AddCommand(new ChangeValueCommand(resetValue, changeValue));
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
			CommandHistory::AddCommand(new ChangeValueCommand(dragVal, changeValue));
		ImGui::PopItemWidth();
		if (ImGui::IsItemHovered())
			ImGui::SetMouseCursor(ImGuiMouseCursor_ResizeEW);
	}

	template<typename T>
	void DrawFieldValueControl(const std::string& name, float labelWidth, T changeValue, T resetValue, Ref<ScriptInstance> instance)
	{
		std::function<void(T)> func = [=](T val) { instance->SetFieldValue<T>(name, val); };

		Widgets::DrawControlLabel(name, { labelWidth, 0.0f });
		ImGui::SameLine();

		// Reset value when double clicking label.
		if (ImGui::IsItemHovered())
		{
			ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);
			if (ImGui::IsMouseDoubleClicked(0))
				CommandHistory::AddCommand(new ChangeFunctionValueCommand(func, resetValue, changeValue));
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

		// Define parameters
		const char* format = nullptr;
		float speed = 0.1f;
		if (typeid(T) == typeid(float) || typeid(T) == typeid(double))
		{
			format = "%.3f";

		}
		else if (typeid(T) == typeid(uint16_t) || typeid(T) == typeid(uint32_t) || typeid(T) == typeid(uint64_t))
		{
			format = "%u";
			speed = 1.0f;
		}
		else
		{
			format = "%i";
			speed = 1.0f;
		}

		// Draw slider
		T dragVal = changeValue;
		std::string label = "##" + name;
		ImGui::PushItemWidth(ImGui::GetContentRegionAvail().x);
		if (ImGui::DragScalar(label.c_str(), dataType, &dragVal, speed, NULL, NULL, format))
			CommandHistory::AddCommand(new ChangeFunctionValueCommand(func, dragVal, changeValue));
		ImGui::PopItemWidth();
		if (ImGui::IsItemHovered())
			ImGui::SetMouseCursor(ImGuiMouseCursor_ResizeEW);
	}
}