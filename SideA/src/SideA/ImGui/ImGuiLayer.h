// -- ImGuiLayer --------------------------------------------------------------
// ImGui layer. Displays gui. Handles ImGui events

#pragma once

#include "SideA/Core/Layer.h"

#include "SideA/Events/ApplicationEvent.h"
#include "SideA/Events/KeyEvent.h"
#include "SideA/Events/MouseEvent.h"

namespace SideA
{
	class ImGuiLayer : public Layer
	{
	public:
		ImGuiLayer();
		virtual ~ImGuiLayer() {}

		// Initiates and sets styling for ImGui
		virtual void OnAttach() override;
		// Shuts down opengl3 and glfw for ImGui
		virtual void OnDetach() override;

		virtual void OnEvent(Event& e) override;

		// Separates rendering so OnImGuiRender can be simplified
		void Begin();
		void End();

		void BlockEvents(bool block) { m_BlockEvents = block; }

		void SetDarkTheme();

	private:
		bool m_BlockEvents = false;
	};
}

