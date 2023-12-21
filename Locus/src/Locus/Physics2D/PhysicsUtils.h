#include <box2d/b2_body.h>

#include "Locus/Core/Log.h"
#include "Locus/Scene/Components.h"

namespace Locus
{
	namespace Utils
	{
		static b2BodyType Rigidbody2DTypeToBox2DType(Rigidbody2DType bodyType)
		{
			switch (bodyType)
			{
			case Rigidbody2DType::Static: return b2_staticBody;
			case Rigidbody2DType::Dynamic: return b2_dynamicBody;
			case Rigidbody2DType::Kinematic: return b2_kinematicBody;
			}

			LOCUS_CORE_ASSERT(false, "Unknown Rigidbody2DType");
			return b2_staticBody;
		}
	}
}