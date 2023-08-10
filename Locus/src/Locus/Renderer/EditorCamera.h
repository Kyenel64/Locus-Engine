// --- EditorCamera -----------------------------------------------------------
// The editor camera is only used in the editor. 
#pragma once

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>

#include "Locus/Core/Timestep.h"
#include "Locus/Events/Event.h"
#include "Locus/Events/MouseEvent.h"
#include "Locus/Renderer/Camera.h"

namespace Locus
{
	class EditorCamera : public Camera
	{
	public:
		EditorCamera() = default;
		EditorCamera(float fov, float aspectRatio, float nearClip, float farClip);
		~EditorCamera() = default;

		void OnUpdate(Timestep deltaTime);
		void OnEvent(Event& e);

		const glm::vec4& GetBackgroundColor() const { return m_BackgroundColor; }

		inline float GetDistance() const { return m_Distance; }
		inline void SetDistance(float distance) { m_Distance = distance; }

		inline void SetViewportSize(float width, float height) { m_ViewportWidth = width; m_ViewportHeight = height; CalculateProjection(); }

		const glm::mat4& GetViewMatrix() const { return m_ViewMatrix; } // Note: Usually only use const& as return if the returning value is a member or static variable.
		glm::mat4 GetViewProjectionMatrix() const { return m_Projection * m_ViewMatrix; } // Returning by value since returning value is not member or static.

		glm::vec3 GetForwardDirection() const;
		glm::vec3 GetRightDirection() const;
		glm::vec3 GetUpDirection() const;
		const glm::vec3& GetPosition() const { return m_Position; }
		glm::quat GetOrientation() const;

	private:
		void CalculateProjection();
		void CalculateView();

		bool OnMouseScroll(MouseScrolledEvent& e);

		void MouseRotate(const glm::vec2& delta);
		void MousePan(const glm::vec2& delta);
		void MouseZoom(float delta);


		glm::vec3 CalculatePosition() const;

	private:
		float m_FOV = 45.0f, m_AspectRatio = 1.778f, m_NearClip = 0.1f, m_FarClip = 1000.0f;

		glm::mat4 m_ViewMatrix; // Projection in Camera class

		glm::vec3 m_Position = { 0.0f, 0.0f, 0.0f };
		glm::vec3 m_FocalPoint = { 0.0f, 0.0f, 0.0f };

		float m_Distance = 30.0f;
		float m_Pitch = 0.0f, m_Yaw = 0.0f;

		float m_ViewportWidth = 1920.0f, m_ViewportHeight = 1080.0f;
		glm::vec2 m_InitialMousePosition = { 0.0f, 0.0f };

		glm::vec4 m_BackgroundColor = { 0.25f, 0.5f, 0.5f, 1.0f };

		float m_MouseRotateSpeed = 10.0f, m_MousePanSpeed = 10.0f, m_MouseScrollSpeed = 10.0f;
	};
}