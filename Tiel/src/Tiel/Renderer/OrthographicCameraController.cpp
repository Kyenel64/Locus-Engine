#include "tpch.h"
#include "OrthographicCameraController.h"

#include "Tiel/Core/Input.h"
#include "Tiel/Core/KeyCodes.h"

namespace Tiel
{
	OrthographicCameraController::OrthographicCameraController(float aspectRatio, bool rotation)
		: m_AspectRatio(aspectRatio), 
			m_Camera(-m_AspectRatio * m_ZoomLevel, m_AspectRatio* m_ZoomLevel, -m_ZoomLevel, m_ZoomLevel),
			m_Rotation(rotation)
	{

	}

	void OrthographicCameraController::OnUpdate(Timestep deltaTime)
	{
		TIEL_PROFILE_FUNCTION();

		if (Tiel::Input::IsKeyPressed(TIEL_KEY_A) || Tiel::Input::IsKeyPressed(TIEL_KEY_LEFT))
		{
			m_CameraPosition.x -= cos(glm::radians(m_CameraRotation)) * m_CameraTranslationSpeed * deltaTime;
			m_CameraPosition.y -= sin(glm::radians(m_CameraRotation)) * m_CameraTranslationSpeed * deltaTime;
		}
		else if (Tiel::Input::IsKeyPressed(TIEL_KEY_D) || Tiel::Input::IsKeyPressed(TIEL_KEY_RIGHT))
		{
			m_CameraPosition.x += cos(glm::radians(m_CameraRotation)) * m_CameraTranslationSpeed * deltaTime;
			m_CameraPosition.y += sin(glm::radians(m_CameraRotation)) * m_CameraTranslationSpeed * deltaTime;
		}

		if (Tiel::Input::IsKeyPressed(TIEL_KEY_W) || Tiel::Input::IsKeyPressed(TIEL_KEY_UP))
		{
			m_CameraPosition.x += -sin(glm::radians(m_CameraRotation)) * m_CameraTranslationSpeed * deltaTime;
			m_CameraPosition.y += cos(glm::radians(m_CameraRotation)) * m_CameraTranslationSpeed * deltaTime;
		}
			
		else if (Tiel::Input::IsKeyPressed(TIEL_KEY_S) || Tiel::Input::IsKeyPressed(TIEL_KEY_DOWN))
		{
			m_CameraPosition.x -= -sin(glm::radians(m_CameraRotation)) * m_CameraTranslationSpeed * deltaTime;
			m_CameraPosition.y -= cos(glm::radians(m_CameraRotation)) * m_CameraTranslationSpeed * deltaTime;
		}

		if (m_Rotation)
		{
			if (Tiel::Input::IsKeyPressed(TIEL_KEY_Q))
				m_CameraRotation += m_CameraRotationSpeed * deltaTime;
			if (Tiel::Input::IsKeyPressed(TIEL_KEY_E))
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
		TIEL_PROFILE_FUNCTION();

		EventDispatcher dispatcher(e);
		dispatcher.Dispatch<MouseScrolledEvent>(TIEL_BIND_EVENT_FN(OrthographicCameraController::OnMouseScrolled));
		dispatcher.Dispatch<WindowResizeEvent>(TIEL_BIND_EVENT_FN(OrthographicCameraController::OnWindowResized));
	}

	bool OrthographicCameraController::OnMouseScrolled(MouseScrolledEvent& e)
	{
		TIEL_PROFILE_FUNCTION();

		m_ZoomLevel -= e.GetYOffset() * 0.25f;
		m_ZoomLevel = std::max(m_ZoomLevel, 0.25f);
		m_Camera.SetProjection(-m_AspectRatio * m_ZoomLevel, m_AspectRatio * m_ZoomLevel, -m_ZoomLevel, m_ZoomLevel);
		return false;
	}

	bool OrthographicCameraController::OnWindowResized(WindowResizeEvent& e)
	{
		TIEL_PROFILE_FUNCTION();

		m_AspectRatio = (float)e.GetWidth() / (float)e.GetHeight();
		m_Camera.SetProjection(-m_AspectRatio * m_ZoomLevel, m_AspectRatio * m_ZoomLevel, -m_ZoomLevel, m_ZoomLevel);
		return false;
	}
}