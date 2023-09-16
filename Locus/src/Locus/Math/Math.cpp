#include "Lpch.h"
#include "Math.h"

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/norm.hpp>
#include <glm/gtx/quaternion.hpp>

namespace Locus::Math
{

	glm::vec3 Scale(const glm::vec3& v, float desiredLength)
	{
		return v * desiredLength / length(v);
	}

	bool Decompose(const glm::mat4& transform, glm::vec3& scale, glm::quat& rotation, glm::vec3& position)
	{
		glm::mat4 localTransform = transform;

		// Normalize the matrix.
		if (glm::epsilonEqual(localTransform[3][3], 0.0f, glm::epsilon<float>()))
			return false;

		for (size_t i = 0; i < 4; ++i)
			for (size_t j = 0; j < 4; ++j)
				localTransform[i][j] /= localTransform[3][3];

		// Transform
		position = glm::vec3(localTransform[3]);
		localTransform[3] = glm::vec4(0, 0, 0, localTransform[3].w);

		glm::vec3 Row[3], Pdum3;

		// Scale
		for (size_t i = 0; i < 3; ++i)
			for (size_t j = 0; j < 3; ++j)
				Row[i][j] = localTransform[i][j];

		// Compute X scale factor and normalize first row.
		scale.x = length(Row[0]);// v3Length(Row[0]);
		Row[0] = Scale(Row[0], 1.0f);

		scale.y = length(Row[1]);
		Row[1] = Scale(Row[1], 1.0f);

		// Next, get Z scale and normalize 3rd row.
		scale.z = length(Row[2]);
		Row[2] = Scale(Row[2], 1.0f);

		// At this point, the matrix (in rows[]) is orthonormal.
		// Check for a coordinate system flip.  If the determinant
		// is -1, then negate the matrix and the scaling factors.
		Pdum3 = cross(Row[1], Row[2]); // v3Cross(row[1], row[2], Pdum3);
		if (dot(Row[0], Pdum3) < 0)
		{
			for (size_t i = 0; i < 3; i++)
			{
				scale[i] *= -1.0f;
				Row[i] *= -1.0f;
			}
		}

		// Rotations
		int i, j, k = 0;
		float root, trace = Row[0].x + Row[1].y + Row[2].z;
		if (trace > 0.0f)
		{
			root = sqrt(trace + 1.0f);
			rotation.w = 0.5f * root;
			root = 0.5f / root;
			rotation.x = root * (Row[1].z - Row[2].y);
			rotation.y = root * (Row[2].x - Row[0].z);
			rotation.z = root * (Row[0].y - Row[1].x);
		} // End if > 0
		else
		{
			static int Next[3] = { 1, 2, 0 };
			i = 0;
			if (Row[1].y > Row[0].x) i = 1;
			if (Row[2].z > Row[i][i]) i = 2;
			j = Next[i];
			k = Next[j];

#           ifdef GLM_FORCE_QUAT_DATA_XYZW
			int off = 0;
#           else
			int off = 1;
#           endif

			root = sqrt(Row[i][i] - Row[j][j] - Row[k][k] + 1.0f);

			rotation[i + off] = 0.5f * root;
			root = 0.5f / root;
			rotation[j + off] = root * (Row[i][j] + Row[j][i]);
			rotation[k + off] = root * (Row[i][k] + Row[k][i]);
			rotation.w = root * (Row[j][k] - Row[k][j]);
		} // End if <= 0

		return true;
	}

}