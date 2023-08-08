// --- Layer ------------------------------------------------------------------
// Locus iterates through Layers to render to the application window.
// For example, Locus-Editor derives the Layer class.
#pragma once

#include "Locus/Core/Core.h"
#include "Locus/Core/Timestep.h"
#include "Locus/Events/Event.h"

namespace Locus
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