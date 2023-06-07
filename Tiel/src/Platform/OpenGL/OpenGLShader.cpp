#include "tpch.h"
#include "OpenGLShader.h"

#include <glad/glad.h>

#include <glm/gtc/type_ptr.hpp>

namespace Tiel
{
	OpenGLShader::OpenGLShader(const std::string vertexSrc, const std::string fragmentSrc)
	{
		// --- Create vertex shader -------------------------------------------------
		GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);

		// Bind shader to source
		const GLchar* source = vertexSrc.c_str();
		glShaderSource(vertexShader, 1, &source, 0);

		// Compile the vertex shader
		glCompileShader(vertexShader);

		// Check compilation errors
		GLint isCompiled = 0;
		glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &isCompiled);
		if (isCompiled == GL_FALSE)
		{
			GLint maxLength = 0;
			glGetShaderiv(vertexShader, GL_INFO_LOG_LENGTH, &maxLength);
			// The maxLength includes the NULL character
			std::vector<GLchar> infoLog(maxLength);
			glGetShaderInfoLog(vertexShader, maxLength, &maxLength, &infoLog[0]);
			// We don't need the shader anymore.
			glDeleteShader(vertexShader);

			TIEL_CORE_ERROR("{0}", infoLog.data());
			TIEL_CORE_ASSERT(false, "Vertex shader failed to compile!");
			return;
		}

		// --- Create fragment shader -----------------------------------------
		GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);

		// Bind shader to source
		source = fragmentSrc.c_str();
		glShaderSource(fragmentShader, 1, &source, 0);

		// Compile the fragment shader
		glCompileShader(fragmentShader);

		// Check for compilation errors
		glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &isCompiled);
		if (isCompiled == GL_FALSE)
		{
			GLint maxLength = 0;
			glGetShaderiv(fragmentShader, GL_INFO_LOG_LENGTH, &maxLength);

			// The maxLength includes the NULL character
			std::vector<GLchar> infoLog(maxLength);
			glGetShaderInfoLog(fragmentShader, maxLength, &maxLength, &infoLog[0]);

			// We don't need the shader anymore. Don't leak shaders
			glDeleteShader(fragmentShader);
			glDeleteShader(vertexShader);

			TIEL_CORE_ERROR("{0}", infoLog.data());
			TIEL_CORE_ASSERT(false, "Fragment shader failed to compile!");
			return;
		}

		// --- Create shader program ------------------------------------------
		m_RendererID = glCreateProgram();

		// Attach shaders to program
		glAttachShader(m_RendererID, vertexShader);
		glAttachShader(m_RendererID, fragmentShader);
		// Link programs
		glLinkProgram(m_RendererID);

		// Check for linking errors
		GLint isLinked = 0;
		glGetProgramiv(m_RendererID, GL_LINK_STATUS, (int*)&isLinked);
		if (isLinked == GL_FALSE)
		{
			GLint maxLength = 0;
			glGetProgramiv(m_RendererID, GL_INFO_LOG_LENGTH, &maxLength);

			// The maxLength includes the NULL character
			std::vector<GLchar> infoLog(maxLength);
			glGetProgramInfoLog(m_RendererID, maxLength, &maxLength, &infoLog[0]);

			// We don't need the program anymore.
			glDeleteProgram(m_RendererID);
			// Don't leak shaders either.
			glDeleteShader(vertexShader);
			glDeleteShader(fragmentShader);

			TIEL_CORE_ERROR("{0}", infoLog.data());
			TIEL_CORE_ASSERT(false, "Shader program failed to link!");
			return;
		}

		// Detach shaders after linking into program
		glDetachShader(m_RendererID, vertexShader);
		glDetachShader(m_RendererID, fragmentShader);
	}

	OpenGLShader::~OpenGLShader()
	{
		glDeleteProgram(m_RendererID);
	}

	void OpenGLShader::Bind() const
	{
		glUseProgram(m_RendererID);
	}

	void OpenGLShader::Unbind() const
	{
		glUseProgram(0);
	}

	void OpenGLShader::UploadUniformMat4(const std::string& name, const glm::mat4& matrix) const
	{
		GLint location = glGetUniformLocation(m_RendererID, name.c_str());
		glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(matrix));
	}
}