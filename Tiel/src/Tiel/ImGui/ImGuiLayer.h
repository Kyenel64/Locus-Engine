// -- ImGuiLayer --------------------------------------------------------------
// ImGui layer. Displays gui. Handles ImGui events

#pragma once

#include "Tiel/Core/Layer.h"

#include "Tiel/Events/ApplicationEvent.h"
#include "Tiel/Events/KeyEvent.h"
#include "Tiel/Events/MouseEvent.h"

namespace Tiel
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

	private:
		float m_Time = 0.0f;
	};
}

