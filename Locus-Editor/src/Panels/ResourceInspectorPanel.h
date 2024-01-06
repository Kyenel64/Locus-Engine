#pragma once

#include <Locus.h>

#include "Panels/ContentBrowserPanel.h"

namespace Locus
{
	class ResourceInspectorPanel
	{
	public:
		ResourceInspectorPanel() = default;
		ResourceInspectorPanel(Ref<ContentBrowserPanel> contentBrowserPanel) : m_ContentBrowserPanel(contentBrowserPanel) {}
		~ResourceInspectorPanel() = default;

		void OnImGuiRender();

	private:
		void DrawMaterialInspector();

	private:
		Ref<ContentBrowserPanel> m_ContentBrowserPanel;
	};
}