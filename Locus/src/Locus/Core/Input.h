// --- Input ------------------------------------------------------------------
// Application input class. Implementation is platform dependent. The cpp
//	file is contained in the platform directory.
#pragma once

#include <glm/glm.hpp>

#include "Locus/Core/Core.h"

namespace Locus
{
	class Input
	{
	public:
		static bool IsKeyPressed(int keycode);
		static bool IsMouseButtonPressed(int button);

		static glm::vec2 GetMousePosition();
		static float GetMouseX();
		static float GetMouseY();
	};
}
