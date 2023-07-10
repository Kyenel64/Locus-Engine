#pragma once

#include "SideA/Renderer/Camera.h"

namespace SideA
{
	class SceneCamera : public Camera
	{
	public:
		SceneCamera();
		virtual ~SceneCamera() = default;

		void SetOrthographic(float size, float nearClip, float farClip);

		void SetViewportSize(uint32_t width, uint32_t height);

		void SetOrthoSize(float size) { m_OrthoSize = size; RecalculateProjection(); }
		float GetOrthoSize() const { return m_OrthoSize; }

	private:
		void RecalculateProjection();

	private:
		float m_OrthoSize = 5.0f;
		float m_OrthoNear = -1.0f, m_OrthoFar = 1.0f;

		float m_AspectRatio = 0.0f;
	};
}