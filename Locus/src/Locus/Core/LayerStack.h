// --- LayerStack -------------------------------------------------------------
// Layer stack. Layers are contained in first half of stack, overlays are
// contained in the second half.
#pragma once

#include "Locus/Core/Layer.h"

namespace Locus
{
	class LayerStack
	{
	public:
		LayerStack() = default;
		~LayerStack();

		void PushLayer(Layer* layer);
		void PushOverlay(Layer* layer);
		void PopLayer(Layer* overlay);
		void PopOverlay(Layer* overlay);

		inline std::vector<Layer*>::iterator begin() { return m_Layers.begin(); }
		inline std::vector<Layer*>::iterator end() { return m_Layers.end(); }
		inline std::vector<Layer*>::reverse_iterator rbegin() { return m_Layers.rbegin(); }
		inline std::vector<Layer*>::reverse_iterator rend() { return m_Layers.rend(); }
		
		inline std::vector<Layer*>::const_iterator begin() const { return m_Layers.begin(); }
		inline std::vector<Layer*>::const_iterator end() const { return m_Layers.end(); }
		inline std::vector<Layer*>::const_reverse_iterator rbegin() const { return m_Layers.rbegin(); }
		inline std::vector<Layer*>::const_reverse_iterator rend() const { return m_Layers.rend(); }

	private:
		std::vector<Layer*> m_Layers;
		unsigned int m_LayerInsertIndex = 0;
	};
}
