// -- ImGuiLayer --------------------------------------------------------------
// ImGui layer. Displays gui. Handles ImGui events
#pragma once

#include "Locus/Core/Layer.h"
#include "Locus/Events/ApplicationEvent.h"
#include "Locus/Events/KeyEvent.h"
#include "Locus/Events/MouseEvent.h"

namespace Locus
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

