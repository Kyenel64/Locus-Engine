#include "SApch.h"
#include "EditorCamera.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "SideA/Core/Input.h"
#include "SideA/Core/KeyCodes.h"
#include "SideA/Core/MouseCodes.h"


namespace SideA
{
	EditorCamera::EditorCamera(float fov, float aspectRatio, float nearClip, float farClip)
		: m_FOV(fov), m_AspectRatio(aspectRatio), m_NearClip(nearClip), m_FarClip(farClip), 
		Camera(glm::perspective(glm::radians(fov), aspectRatio, nearClip, farClip))
	{
		CalculateView();
	}

	void EditorCamera::CalculateProjection()
	{
		m_AspectRatio = m_ViewportWidth / m_ViewportHeight;
		m_Projection = glm::perspective(glm::radians(m_FOV), m_AspectRatio, m_NearClip, m_FarClip);
	}

	void EditorCamera::CalculateView()
	{
		m_Position = CalculatePosition();

		glm::quat orientation = GetOrientation();
		m_ViewMatrix = glm::translate(glm::mat4(1.0f), m_Position) * glm::toMat4(orientation);
		m_ViewMatrix = glm::inverse(m_ViewMatrix);

	}

	void EditorCamera::OnUpdate(Timestep deltaTime)
	{	
		glm::vec2 mouse = Input::GetMousePosition();
		glm::vec2 delta = mouse - m_InitialMousePosition;
		m_InitialMousePosition = mouse;

		if (Input::IsMouseButtonPressed(Mouse::ButtonMiddle))
		{
			if (Input::IsKeyPressed(Key::LeftShift))
				MousePan(delta);
			else
				MouseRotate(delta);
		}
			
		CalculateView();
	}

	void EditorCamera::OnEvent(Event& e)
	{
		EventDispatcher dispatcher(e);

		dispatcher.Dispatch<MouseScrolledEvent>(SIDEA_BIND_EVENT_FN(EditorCamera::OnMouseScroll));
	}

	void EditorCamera::MouseRotate(const glm::vec2& delta)
	{
		float yawSign = GetUpDirection().y < 0 ? -1.0f : 1.0f;
		m_Yaw += yawSign * delta.x * 0.01f; // TODO:: better speed
		m_Pitch += delta.y * 0.01f;
	}

	void EditorCamera::MousePan(const glm::vec2& delta)
	{
		m_FocalPoint += -GetRightDirection() * delta.x * 0.01f; // TODO: better speed
		m_FocalPoint += GetUpDirection() * delta.y * 0.01f;
	}

	void EditorCamera::MouseZoom(float delta)
	{
		m_Zoom -= delta * 5.0f;
		if (m_Zoom < 0.3f)
		{
			//m_FocalPoint += GetForwardDirection();
			m_Zoom = 0.3f;
		}
	}

	bool EditorCamera::OnMouseScroll(MouseScrolledEvent& e)
	{
		float delta = e.GetYOffset() * 0.1f;
		MouseZoom(delta);
		CalculateView();
		return false;
	}

	glm::quat EditorCamera::GetOrientation() const
	{
		return glm::quat(glm::vec3(-m_Pitch, -m_Yaw, 0.0f));
	}

	glm::vec3 EditorCamera::GetForwardDirection() const
	{
		return glm::rotate(GetOrientation(), glm::vec3(0.0f, 0.0f, -1.0f));
	}

	glm::vec3 EditorCamera::GetRightDirection() const
	{
		return glm::rotate(GetOrientation(), glm::vec3(1.0f, 0.0f, 0.0f));
	}

	glm::vec3 EditorCamera::GetUpDirection() const
	{
		return glm::rotate(GetOrientation(), glm::vec3(0.0f, 1.0f, 0.0f));
	}

	glm::vec3 EditorCamera::CalculatePosition() const
	{
		return m_FocalPoint - GetForwardDirection() * m_Zoom;
	}

}