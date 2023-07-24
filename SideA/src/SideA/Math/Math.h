#pragma once

#include <glm/glm.hpp>

namespace SideA::Math
{
	bool DecomposeTransform(const glm::mat4& transform, glm::vec3& translation, glm::quat& rotation, glm::vec3& scale);

}