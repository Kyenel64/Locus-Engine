// --- LayerStack -------------------------------------------------------------
// Layer stack. Layers are contained in first half of stack, overlays are
// contained in the second half.

#pragma once

#include "Tiel/Core.h"
#include "Layer.h"

namespace Tiel
{
	class TIEL_API LayerStack
	{
	public:
		LayerStack();
		~LayerStack();

		void PushLayer(Layer* layer);
		void PushOverlay(Layer* layer);
		void PopLayer(Layer* overlay);
		void PopOverlay(Layer* overlay);

		std::vector<Layer*>::iterator begin() { return m_Layers.begin(); }
		std::vector<Layer*>::iterator end() { return m_Layers.end(); }

	private:
		std::vector<Layer*> m_Layers;
		std::vector<Layer*>::iterator m_LayerInsert;
	};
}