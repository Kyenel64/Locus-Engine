// --- Layer ------------------------------------------------------------------
// Locus iterates through Layers within the main application run-loop. 
// To use Locus, users need to create a class derived from Layer.
//	See LocusEditorLayer.h for a reference.
#pragma once

#include "Locus/Core/Timestep.h"
#include "Locus/Events/Event.h"

namespace Locus
{
	class Layer
	{
	public:
		Layer(const std::string& debugName = "Layer") : m_DebugName(debugName) {}
		virtual ~Layer() = default;

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
