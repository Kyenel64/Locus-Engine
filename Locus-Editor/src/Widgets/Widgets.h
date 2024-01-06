// --- Widgets ----------------------------------------------------------------
// Locus editor widgets. 
// For public script fields during scene runtime, widget needs to take in a 
//	ScriptInstance pointer.
#pragma once

#include <string>

#include <glm/glm.hpp>

#include <Locus.h>

#include "Command/CommandHistory.h"
#include "Command/ValueCommands.h"

namespace Locus::Widgets
{
	// Displays a label with given size.
	void DrawControlLabel(const std::string& name, const glm::vec2& size = { 0.0f, 0.0f });

	void DrawBoolControl(const std::string& name, bool& changeValue, bool resetValue = false, float labelWidth = -1.0f, Ref<ScriptInstance> instance = nullptr);

	void DrawCharControl(const std::string& name, char& changeValue, char resetValue = ' ', float labelWidth = -1.0f, float inputWidth = -1.0f, Ref<ScriptInstance> instance = nullptr);

	void DrawStringControl(const std::string& name, std::string& changeValue, const std::string& resetValue = std::string(), float labelWidth = -1.0f, float inputWidth = -1.0f);

	void DrawColorControl(const std::string& name, glm::vec4& changeValue, const glm::vec4& resetValue = glm::vec4(1.0f), float labelWidth = -1.0f, float inputWidth = -1.0f);

	void DrawVec2Control(const std::string& name, glm::vec2& changeValue, const glm::vec2& resetValue = glm::vec2(1.0f), float speed = 0.1f, const char* format = nullptr,
		float labelWidth = -1.0f, float inputWidth = -1.0f, const glm::vec2& min = glm::vec2(0.0f), const glm::vec2& max = glm::vec2(0.0f), Ref<ScriptInstance> instance = nullptr);
	void DrawVec3Control(const std::string& name, glm::vec3& changeValue, const glm::vec3& resetValue = glm::vec3(1.0f), float speed = 0.1f, const char* format = nullptr,
		float labelWidth = -1.0f, float inputWidth = -1.0f, const glm::vec3& min = glm::vec3(0.0f), const glm::vec3& max = glm::vec3(0.0f), Ref<ScriptInstance> instance = nullptr);

	// Displays a 8x2 grid for each collision layer.
	void DrawCollisionGrid(const std::string& name, uint16_t& changeValue, uint16_t resetValue = 0, float labelWidth = -1.0f, float inputWidth = -1.0f);

	void DrawTextureDropdown(const std::string& name, TextureHandle& textureHandle, float labelWidth = -1.0f, float inputWidth = -1.0f);
	void DrawMaterialDropdown(const std::string& name, MaterialHandle& textureHandle, float labelWidth = -1.0f, float inputWidth = -1.0f);

	// Control widget for float, double, int16_t, int, int64_t, uint16_t, uint32_t, uint64_t.
	template<typename T>
	void DrawValueControl(const std::string& name, T& changeValue, T resetValue = 0, float speed = 0.1f, const char* format = nullptr,
		float labelWidth = -1.0f, float inputWidth = -1.0f, T min = 0, T max = 0, Ref<ScriptInstance> instance = nullptr)
	{
		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, { 0.0f, ImGui::GetStyle().ItemSpacing.y });

		std::function<void(T)> func = nullptr;
		if (instance)
			func = [=](T val) { instance->SetFieldValue<T>(name, val); };

		if (labelWidth == -1)
			labelWidth = ImGui::GetContentRegionAvail().x * 0.5f;

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
		ImGuiDataType dataType = ImGuiDataType_Float;
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
		ImGui::PushItemWidth(inputWidth);
		if (ImGui::DragScalar(label.c_str(), dataType, &dragVal, speed, &min, &max, format))
		{
			if (instance)
				CommandHistory::AddCommand(new ChangeFunctionValueCommand(func, dragVal, changeValue));
			else
				CommandHistory::AddCommand(new ChangeValueCommand(dragVal, changeValue));
		}
		ImGui::PopItemWidth();
		if (ImGui::IsItemHovered())
			ImGui::SetMouseCursor(ImGuiMouseCursor_ResizeEW);

		ImGui::PopStyleVar();
	}
}
