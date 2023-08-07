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

	bool DecomposeTransform(const glm::mat4& transform, glm::vec3& translation, glm::quat& rotation, glm::vec3& scale)
	{
		// from glm::decompose
		glm::mat4 LocalMatrix(transform);

		// Normalize the matrix.
		if (glm::epsilonEqual(LocalMatrix[3][3], 0.0f, glm::epsilon<float>()))
			return false;

		for (glm::length_t i = 0; i < 4; ++i)
			for (glm::length_t j = 0; j < 4; ++j)
				LocalMatrix[i][j] /= LocalMatrix[3][3];

		// No perspective.
		glm::vec4 Perspective = glm::vec4(0, 0, 0, 1);

		// Next take care of translation (easy).
		translation = glm::vec3(LocalMatrix[3]);
		LocalMatrix[3] = glm::vec4(0, 0, 0, LocalMatrix[3].w);

		glm::vec3 Row[3], Pdum3;

		// Now get scale and shear.
		for (glm::length_t i = 0; i < 3; ++i)
			for (glm::length_t j = 0; j < 3; ++j)
				Row[i][j] = LocalMatrix[i][j];

		// Compute X scale factor and normalize first row.
		scale.x = length(Row[0]);// v3Length(Row[0]);
		Row[0] = Scale(Row[0], 1.0f);

		// Now, compute Y scale and normalize 2nd row.
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
			for (glm::length_t i = 0; i < 3; i++)
			{
				scale[i] *= -1.0f;
				Row[i] *= -1.0f;
			}
		}

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