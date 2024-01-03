// --- Input ------------------------------------------------------------------
// Application input class. Implementation is platform dependent. The cpp
//	file is contained in the platform directory.
#pragma once

#include <queue>
#include <map>

#include <glm/glm.hpp>

#include "Locus/Core/KeyCodes.h"
#include "Locus/Core/MouseCodes.h"
#include "Locus/Core/Core.h"

namespace Locus
{
	class Input
	{
	public:
		// --- Key Input ---
		static bool IsKeyPressed(KeyCode keycode);
		static bool IsKeyHeld(KeyCode keycode);
		static bool IsKeyReleased(KeyCode keycode);

		static void SetKeyState(KeyCode keycode, KeyState keystate);
		static KeyState GetKeyState(KeyCode keycode);

		// Called OnUpdate() to process keystates
		static void ProcessKeys();

		static bool IsMouseButtonPressed(MouseCode button);

		// --- Mouse Input ---
		static glm::vec2 GetMousePosition();
		static float GetMouseX();
		static float GetMouseY();

	private:
		inline static std::map<KeyCode, KeyState> s_KeyState;
		inline static std::queue<KeyCode> s_UnhandledKeys;
	};
}
