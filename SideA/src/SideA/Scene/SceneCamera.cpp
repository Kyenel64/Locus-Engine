#include "SApch.h"
#include "SceneCamera.h"

#include "glm/gtc/matrix_transform.hpp"

namespace SideA
{
	SceneCamera::SceneCamera()
	{
		RecalculateProjection();
	}

	void SceneCamera::SetOrthographic(float size, float nearClip, float farClip)
	{
		m_OrthoSize = size;
		m_OrthoNear = nearClip;
		m_OrthoFar = farClip;
		RecalculateProjection();
	}

	void SceneCamera::SetViewportSize(uint32_t width, uint32_t height)
	{
		m_AspectRatio = (float)width / (float)height;
		RecalculateProjection();
	}

	void SceneCamera::RecalculateProjection()
	{
		float orthoLeft = -m_OrthoSize * m_AspectRatio * 0.5f;
		float orthoRight = m_OrthoSize * m_AspectRatio * 0.5f;
		float orthoTop = m_OrthoSize * 0.5f;
		float orthoBottom = -m_OrthoSize * 0.5f;

		m_Projection = glm::ortho(orthoLeft, orthoRight, orthoBottom, orthoTop, m_OrthoNear, m_OrthoFar);
	}
}