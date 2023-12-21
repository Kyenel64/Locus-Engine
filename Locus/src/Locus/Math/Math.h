// --- Math -------------------------------------------------------------------
// Math class
#pragma once

#include <glm/glm.hpp>

namespace Locus::Math
{
	// Decompose the translation, rotation, scale of a 4x4 matrix.
	bool Decompose(const glm::mat4& transform, glm::vec3& scale, glm::quat& rotation, glm::vec3& position);
}
