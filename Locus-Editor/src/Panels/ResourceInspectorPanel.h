#pragma once

#include <Locus.h>

#include "Panels/ProjectBrowserPanel.h"

namespace Locus
{
	class ResourceInspectorPanel
	{
	public:
		ResourceInspectorPanel() = default;
		ResourceInspectorPanel(Ref<ProjectBrowserPanel> ProjectBrowserPanel) : m_ProjectBrowserPanel(ProjectBrowserPanel) {}
		~ResourceInspectorPanel() = default;

		void OnImGuiRender();

	private:
		void DrawMaterialInspector();

	private:
		Ref<ProjectBrowserPanel> m_ProjectBrowserPanel;
	};
}