// --- OpenGlShader -----------------------------------------------------------
// OpenGL Shader class. Creates OpenGL shaders with GLSL source data.
#pragma once

#include "Tiel/Renderer/Shader.h"

namespace Tiel
{
	class OpenGLShader : public Shader
	{
	public:
		OpenGLShader(const std::string vertexSrc, const std::string fragmentSrc);
		~OpenGLShader();

		virtual void Bind() const override;
		virtual void Unbind() const override;

		virtual void UploadUniformMat4(const std::string& name, const glm::mat4& matrix) const override;

		virtual unsigned int getID() const override { return m_RendererID; }

	private:
		uint32_t m_RendererID;
	};
	
}