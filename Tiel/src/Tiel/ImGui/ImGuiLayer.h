// -- ImGuiLayer --------------------------------------------------------------
// ImGui layer. Displays gui

#pragma once

#include "Tiel/Layer.h"

namespace Tiel
{
	class TIEL_API ImGuiLayer : public Layer
	{
	public:
		ImGuiLayer();
		~ImGuiLayer();

		void OnAttach();
		void OnDetach();
		void OnUpdate();
		void OnEvent(Event& event);

	private:
		float m_Time = 0.0f;
	};
}

