// --- Layer ------------------------------------------------------------------
// Layer interface. Handles attached, detached, updates and events.

#pragma once

#include "SideA/Core/Core.h"
#include "SideA/Core/Timestep.h"
#include "SideA/Events/Event.h"

namespace SideA
{
	class Layer
	{
	public:
		Layer(const std::string& name = "Layer");
		virtual ~Layer() {};

		virtual void OnAttach() {}
		virtual void OnDetach() {}
		virtual void OnUpdate(Timestep deltaTime) {}
		virtual void OnEvent(Event& e) {}
		virtual void OnImGuiRender() {}

		inline const std::string& GetName() const { return m_DebugName; }

	protected:
		std::string m_DebugName;
	};
}