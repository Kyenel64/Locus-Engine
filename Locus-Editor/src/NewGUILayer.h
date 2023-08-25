#pragma once

#include <Locus.h>

namespace Locus
{
	class NewGUILayer : public Layer
	{
	public:
		NewGUILayer();
		virtual ~NewGUILayer();

		virtual void OnAttach() override;
		virtual void OnDetach() override;
		virtual void OnUpdate(Timestep deltaTime) override;
		virtual void OnEvent(Event& event) override;
		virtual void OnImGuiRender() override;

	private:
		void ProcessResize();
		void DrawLayoutTable();

	private:
		float m_ViewportHeight;
		float m_HierarchyHeight;
		float m_CenterSplitter;
		glm::vec2 m_FrameSizes[4];
		glm::vec2 m_FramePositions[4];
		glm::vec2 m_WindowSize;
		glm::vec2 m_PrevWindowSize;

		enum class LayoutStyle { Default = 0 };
		LayoutStyle m_LayoutStyle = LayoutStyle::Default;
	};
}