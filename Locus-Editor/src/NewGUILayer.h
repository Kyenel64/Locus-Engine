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
		float m_WindowPadding = 5.0f;
		float m_MenuBarHeight = 25.0f;
		float m_TabBarHeight = 25.0f;
		glm::vec2 m_FrameSizes[4];
		glm::vec2 m_FramePositions[4];
		glm::vec2 m_WindowSize = { 1920.0f, 1080.0f };

		enum class LayoutStyle { Default = 0 };
		LayoutStyle m_LayoutStyle = LayoutStyle::Default;
	};
}