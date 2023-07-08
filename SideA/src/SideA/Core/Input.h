// --- Input ------------------------------------------------------------------
// Input interface for input polling.
// Input polling checks for input every frame. Events are called whenever there
// is an event.

#pragma once

#include "SideA/Core/Core.h"

namespace SideA
{
	class Input
	{
	public:
		static bool IsKeyPressed(int keycode);
		static bool IsMouseButtonPressed(int button);

		static std::pair<float, float> GetMousePosition();
		static float GetMouseX();
		static float GetMouseY();
	};
}