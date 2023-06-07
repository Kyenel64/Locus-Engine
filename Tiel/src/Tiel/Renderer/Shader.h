#pragma once
#include <string>

#include "glm/glm.hpp"

namespace Tiel
{
	class Shader
	{
	public:

		virtual void Bind() const = 0;
		virtual void Unbind() const = 0;

		virtual void UploadUniformMat4(const std::string& name, const glm::mat4& matrix) const = 0;

		virtual inline unsigned int getID() const = 0;

		static Shader* Create(const std::string vertexSrc, const std::string fragmentSrc);

	};
}