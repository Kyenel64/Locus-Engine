// -- ImGuiLayer --------------------------------------------------------------
// ImGui layer. Displays gui. Handles ImGui events

#pragma once

#include "Tiel/Layer.h"

#include "Tiel/Events/ApplicationEvent.h"
#include "Tiel/Events/KeyEvent.h"
#include "Tiel/Events/MouseEvent.h"

namespace Tiel
{
	class TIEL_API ImGuiLayer : public Layer
	{
	public:
		ImGuiLayer();
		~ImGuiLayer();

		virtual void OnAttach() override;
		virtual void OnDetach() override;
		virtual void OnImGuiRender() override;

		void Begin();
		void End();

	private:
		float m_Time = 0.0f;
	};
}

