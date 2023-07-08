#include "SApch.h"
#include "OrthographicCameraController.h"

#include "SideA/Core/Input.h"
#include "SideA/Core/KeyCodes.h"

namespace SideA
{
	OrthographicCameraController::OrthographicCameraController(float aspectRatio, bool rotation)
		: m_AspectRatio(aspectRatio), 
			m_Camera(-m_AspectRatio * m_ZoomLevel, m_AspectRatio* m_ZoomLevel, -m_ZoomLevel, m_ZoomLevel),
			m_Rotation(rotation)
	{

	}

	void OrthographicCameraController::OnUpdate(Timestep deltaTime)
	{
		SIDEA_PROFILE_FUNCTION();

		if (SideA::Input::IsKeyPressed(SIDEA_KEY_A) || SideA::Input::IsKeyPressed(SIDEA_KEY_LEFT))
		{
			m_CameraPosition.x -= cos(glm::radians(m_CameraRotation)) * m_CameraTranslationSpeed * deltaTime;
			m_CameraPosition.y -= sin(glm::radians(m_CameraRotation)) * m_CameraTranslationSpeed * deltaTime;
		}
		else if (SideA::Input::IsKeyPressed(SIDEA_KEY_D) || SideA::Input::IsKeyPressed(SIDEA_KEY_RIGHT))
		{
			m_CameraPosition.x += cos(glm::radians(m_CameraRotation)) * m_CameraTranslationSpeed * deltaTime;
			m_CameraPosition.y += sin(glm::radians(m_CameraRotation)) * m_CameraTranslationSpeed * deltaTime;
		}

		if (SideA::Input::IsKeyPressed(SIDEA_KEY_W) || SideA::Input::IsKeyPressed(SIDEA_KEY_UP))
		{
			m_CameraPosition.x += -sin(glm::radians(m_CameraRotation)) * m_CameraTranslationSpeed * deltaTime;
			m_CameraPosition.y += cos(glm::radians(m_CameraRotation)) * m_CameraTranslationSpeed * deltaTime;
		}
			
		else if (SideA::Input::IsKeyPressed(SIDEA_KEY_S) || SideA::Input::IsKeyPressed(SIDEA_KEY_DOWN))
		{
			m_CameraPosition.x -= -sin(glm::radians(m_CameraRotation)) * m_CameraTranslationSpeed * deltaTime;
			m_CameraPosition.y -= cos(glm::radians(m_CameraRotation)) * m_CameraTranslationSpeed * deltaTime;
		}

		if (m_Rotation)
		{
			if (SideA::Input::IsKeyPressed(SIDEA_KEY_Q))
				m_CameraRotation += m_CameraRotationSpeed * deltaTime;
			if (SideA::Input::IsKeyPressed(SIDEA_KEY_E))
				m_CameraRotation -= m_CameraRotationSpeed * deltaTime;

			if (m_CameraRotation > 180.0f)
				m_CameraRotation -= 360.0f;
			else if (m_CameraRotation <= -180.0f)
				m_CameraRotation += 360.0f;
		}

		m_Camera.SetPosition(m_CameraPosition);
		m_Camera.SetRotation(m_CameraRotation);

		m_CameraTranslationSpeed = m_ZoomLevel;
		
	}

	void OrthographicCameraController::OnEvent(Event& e)
	{
		SIDEA_PROFILE_FUNCTION();

		EventDispatcher dispatcher(e);
		dispatcher.Dispatch<MouseScrolledEvent>(SIDEA_BIND_EVENT_FN(OrthographicCameraController::OnMouseScrolled));
		dispatcher.Dispatch<WindowResizeEvent>(SIDEA_BIND_EVENT_FN(OrthographicCameraController::OnWindowResized));
	}

	void OrthographicCameraController::OnResize(float width, float height)
	{
		m_AspectRatio = width / height;
		m_Camera.SetProjection(-m_AspectRatio * m_ZoomLevel, m_AspectRatio
			* m_ZoomLevel, -m_ZoomLevel, m_ZoomLevel);
	}

	bool OrthographicCameraController::OnMouseScrolled(MouseScrolledEvent& e)
	{
		SIDEA_PROFILE_FUNCTION();

		m_ZoomLevel -= e.GetYOffset() * 0.25f;
		m_ZoomLevel = std::max(m_ZoomLevel, 0.25f);
		m_Camera.SetProjection(-m_AspectRatio * m_ZoomLevel, m_AspectRatio * m_ZoomLevel, -m_ZoomLevel, m_ZoomLevel);
		return false;
	}

	bool OrthographicCameraController::OnWindowResized(WindowResizeEvent& e)
	{
		SIDEA_PROFILE_FUNCTION();

		OnResize((float)e.GetWidth(), (float)e.GetHeight());
		return false;
	}
}