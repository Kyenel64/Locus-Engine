// --- Layer ------------------------------------------------------------------
// Layer interface. Handles attached, detached, updates and events.

#pragma once

#include "Tiel/Core.h"
#include "Tiel/Events/Event.h"

namespace Tiel
{
	class Layer
	{
	public:
		Layer(const std::string& name = "Layer");
		virtual ~Layer() {};

		virtual void OnAttach() {}
		virtual void OnDetach() {}
		virtual void OnUpdate() {}
		virtual void OnEvent(Event& event) {}
		virtual void OnImGuiRender() {}

		inline const std::string& GetName() const { return m_DebugName; }

	protected:
		std::string m_DebugName;
	};
}