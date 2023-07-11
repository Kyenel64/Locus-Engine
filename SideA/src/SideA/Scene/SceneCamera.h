#pragma once

#include "SideA/Renderer/Camera.h"

namespace SideA
{
	class SceneCamera : public Camera
	{
	public:
		enum class ProjectionType { Orthographic = 0, Perspective = 1 };

	public:
		SceneCamera();
		virtual ~SceneCamera() = default;

		void SetOrthographic(float size, float nearClip, float farClip);
		void SetPerspective(float verticalFOV, float nearClip, float farClip);

		// -- Both ------------------------------------------------------------
		void SetViewportSize(uint32_t width, uint32_t height);

		void SetProjectionType(ProjectionType type) { m_ProjectionType = type; RecalculateProjection(); }
		ProjectionType GetProjectionType() const { return m_ProjectionType; }

		void SetBackgroundColor(const glm::vec4& backgroundColor) { m_BackgroundColor = backgroundColor; }
		const glm::vec4& GetBackgroundColor() const { return m_BackgroundColor; }


		// --- Perspective ----------------------------------------------------
		void SetPerspectiveFOV(float verticalFOV) { m_PerspectiveFOV = verticalFOV; RecalculateProjection(); }
		float GetPerspectiveFOV() const { return m_PerspectiveFOV; }
		void SetPerspectiveNearClip(float nearClip) { m_PerspectiveNear = nearClip; RecalculateProjection(); }
		float GetPerspectiveNearClip() const { return m_PerspectiveNear; }
		void SetPerspectiveFarClip(float farClip) { m_PerspectiveFar = farClip; RecalculateProjection(); }
		float GetPerspectiveFarClip() const { return m_PerspectiveFar; }


		// --- Orthographic ---------------------------------------------------
		void SetOrthographicSize(float size) { m_OrthographicSize = size; RecalculateProjection(); }
		float GetOrthographicSize() const { return m_OrthographicSize; }
		void SetOrthographicNearClip(float nearClip) { m_OrthographicNear = nearClip; RecalculateProjection(); }
		float GetOrthographicNearClip() const { return m_OrthographicNear; }
		void SetOrthographicFarClip(float farClip) { m_OrthographicFar = farClip; RecalculateProjection(); }
		float GetOrthographicFarClip() const { return m_OrthographicFar; }

	private:
		void RecalculateProjection();

	private:
		ProjectionType m_ProjectionType = ProjectionType::Orthographic;
		float m_AspectRatio = 0.0f;
		glm::vec4 m_BackgroundColor = glm::vec4(0.25f, 0.5f, 0.5f, 1.0f);
		
		// Orthographics
		float m_OrthographicSize = 5.0f;
		float m_OrthographicNear = -1.0f, m_OrthographicFar = 1.0f;

		// Perspective
		float m_PerspectiveFOV = glm::radians(45.0f);
		float m_PerspectiveNear = 0.01f, m_PerspectiveFar = 1000.0f;

	};
}