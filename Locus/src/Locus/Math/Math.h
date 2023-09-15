// --- Math -------------------------------------------------------------------
// Locus math class
#pragma once

#include <glm/glm.hpp>

namespace Locus::Math
{
	bool Decompose(const glm::mat4& transform, glm::vec3& scale, glm::quat& rotation, glm::vec3& position);

	bool DecomposeTransform(const glm::mat4& transform, glm::vec3& translation, glm::quat& rotation, glm::vec3& scale);

}