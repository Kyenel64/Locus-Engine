// --- Input ------------------------------------------------------------------
// Application input class. The Locus-Editor also handles ImGui inputs. See if
//    it is better to use Locus inputs for ImGui as well.
// Implementation for this will be handled in:
//    Platform/{Platform}/{Platform}Input.cpp
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