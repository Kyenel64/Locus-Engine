#include "tpch.h"
#include "OrthographicCamera.h"

#include <glm/gtc/matrix_transform.hpp>

namespace Tiel
{
	OrthographicCamera::OrthographicCamera(float left, float right, float bottom, float top)
		: m_ProjectionMatrix(glm::ortho(left, right, bottom, top, -1.0f, 1.0f)), m_ViewMatrix(1.0f)
	{
		m_ViewProjectionMatrix = m_ProjectionMatrix * m_ViewMatrix;
	}

	void OrthographicCamera::RecalculateViewMatrix()
	{
		glm::mat4 transform = glm::lookAt(m_Position, 
			glm::vec3(m_Position.x, m_Position.y, m_Position.z - 1),
			glm::vec3(0.0f, 1.0f, 0.0f));
		transform *= glm::rotate(glm::mat4(1.0f), glm::radians(-m_Rotation), glm::vec3(0, 0, 1));

		m_ViewMatrix = transform;
		m_ViewProjectionMatrix = m_ProjectionMatrix * m_ViewMatrix;
	}
}