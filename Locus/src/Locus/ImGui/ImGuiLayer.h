// -- ImGuiLayer --------------------------------------------------------------
// ImGui layer. Displays gui. Handles ImGui events
#pragma once

#include <glm/glm.hpp>

#include "Locus/Core/Layer.h"
#include "Locus/Events/ApplicationEvent.h"
#include "Locus/Events/KeyEvent.h"
#include "Locus/Events/MouseEvent.h"

struct ImVec4;

namespace Locus
{
	// GUI colors
	struct LocusColors
	{
		static ImVec4 Black;
		static ImVec4 DarkGrey;
		static ImVec4 MediumDarkGrey;
		static ImVec4 Grey;
		static ImVec4 LightGrey;
		static ImVec4 White;
		static ImVec4 Transparent;
		static ImVec4 Orange;
		static ImVec4 Tan;
		static ImVec4 Pink;
		static ImVec4 Green;
		static ImVec4 Blue;
		static ImVec4 LightBlue;
	};

	ImVec4 ToImVec4(const glm::vec4& value);
	glm::vec4 ToGLMVec4(const ImVec4& value);



	class ImGuiLayer : public Layer
	{
	public:
		ImGuiLayer();
		virtual ~ImGuiLayer() {}

		virtual void OnAttach() override;
		virtual void OnDetach() override;

		virtual void OnEvent(Event& e) override;

		void Begin();
		void End();

		void BlockEvents(bool block) { m_BlockEvents = block; }

		void SetDarkTheme();

	private:
		bool m_BlockEvents = false;
	};
}
