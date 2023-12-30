// --- EditorCamera -----------------------------------------------------------
// Controls:
//	Hold and drag middle mouse button to rotate around focal point.
//	Shift hold and drag middle mouse button to pan.
//	Scroll to zoom.
// Contains data for the editor grid.
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

		glm::vec3 GetForwardDirection() const;
		glm::vec3 GetRightDirection() const;
		glm::vec3 GetUpDirection() const;
		glm::quat GetOrientation() const;

		inline void SetDistance(float distance) { m_Distance = distance; }
		inline void SetFocalPoint(const glm::vec3& point) { m_FocalPoint = point; }
		inline void SetViewportSize(float width, float height) { m_ViewportWidth = width; m_ViewportHeight = height; CalculateProjection(); }
		inline void SetBackgroundColor(const glm::vec4& color) { m_BackgroundColor = color; }
		inline void SetNearClip(float nearClip) { m_NearClip = nearClip; }
		inline void SetFarClip(float farClip) { m_FarClip = farClip; }

		inline float GetDistance() const { return m_Distance; }
		inline const glm::mat4& GetViewMatrix() const { return m_ViewMatrix; }
		inline glm::mat4 GetViewProjectionMatrix() const { return m_Projection * m_ViewMatrix; }
		inline const glm::vec3& GetPosition() const { return m_Position; }
		inline float GetNearClip() const { return m_NearClip; }
		inline float GetFarClip() const { return m_FarClip; }
		inline const glm::vec4& GetBackgroundColor() const { return m_BackgroundColor; }
		inline glm::vec2 GetViewportSize() const { return { m_ViewportWidth, m_ViewportHeight }; }



		// --- Editor Grid ---
		inline const glm::vec4& GetGridColor() const { return m_EditorGrid.GridColor; }
		inline float GetGridScale() const { return m_EditorGrid.GridScale; }
		inline bool GetGridVisibility() const { return m_EditorGrid.GridVisibility; }

		inline void SetGridVisibility(bool visibility) { m_EditorGrid.GridVisibility = visibility; }
		inline void SetGridColor(const glm::vec4& color) { m_EditorGrid.GridColor = color; }
		inline void SetGridScale(float scale) { m_EditorGrid.GridScale = scale; }

	private:
		void CalculateProjection();
		void CalculateView();

		bool OnMouseScroll(MouseScrolledEvent& e);

		void MouseRotate(const glm::vec2& delta);
		void MousePan(const glm::vec2& delta);
		void MouseZoom(float delta);

		glm::vec3 CalculatePosition() const;

	private:
		glm::mat4 m_ViewMatrix; // Projection in Camera class

		glm::vec4 m_BackgroundColor = { 0.1f, 0.1f, 0.1f, 1.0f };

		glm::vec3 m_Position = { 0.0f, 0.0f, 0.0f };
		glm::vec3 m_FocalPoint = { 0.0f, 0.0f, 0.0f };
		float m_Pitch = 0.5f, m_Yaw = -0.785f;

		float m_Distance = 20.0f;
		float m_FOV = 45.0f, m_AspectRatio = 1.778f;
		float m_NearClip = 0.1f, m_FarClip = 10000.0f;

		float m_ViewportWidth = 1920.0f, m_ViewportHeight = 1080.0f;
		glm::vec2 m_InitialMousePosition = { 0.0f, 0.0f };
		float m_MouseRotateSpeed = 10.0f, m_MousePanSpeed = 10.0f, m_MouseScrollSpeed = 10.0f;

		struct EditorGrid
		{
			glm::vec4 GridColor = { 0.8f, 0.8f, 0.8f, 1.0f };
			bool GridVisibility = true;
			float GridScale = 1.0f;
		};

		EditorGrid m_EditorGrid;
	};
}
