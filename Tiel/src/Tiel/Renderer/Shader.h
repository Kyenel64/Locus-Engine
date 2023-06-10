// --- Shader -----------------------------------------------------------------
// Interface for shaders. Creates platform specific shader class.

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

		static Shader* Create(const std::string vertexSrc, const std::string fragmentSrc);

	};
}