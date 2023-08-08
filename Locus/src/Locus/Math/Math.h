// --- Math -------------------------------------------------------------------
// Locus math class
#pragma once

#include <glm/glm.hpp>

namespace Locus::Math
{
	bool DecomposeTransform(const glm::mat4& transform, glm::vec3& translation, glm::quat& rotation, glm::vec3& scale);

}