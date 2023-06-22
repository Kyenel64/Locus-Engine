// --- Input ------------------------------------------------------------------
// Input interface for input polling.
// Input polling checks for input every frame. Events are called whenever there
// is an event.
// Implementaions differ between platforms so will be overwritten.
// Input is a singleton and each function calls its respective impl function.

#pragma once

#include "Tiel/Core/Core.h"

namespace Tiel
{
	class Input
	{
	public:
		// Input is static but must run platform specific code. The private functions
		// are overriden in a platform specific input class.
		inline static bool IsKeyPressed(int keycode) { return s_Instance->IsKeyPressedImpl(keycode); }
		inline static bool IsMouseButtonPressed(int button) { return s_Instance->IsMouseButtonPressedImpl(button); }

		inline static std::pair<float, float> GetMousePosition() { return s_Instance->GetMousePositionImpl(); }
		inline static float GetMouseX() { return s_Instance->GetMouseXImpl(); }
		inline static float GetMouseY() { return s_Instance->GetMouseYImpl(); }

	protected:
		virtual bool IsKeyPressedImpl(int keycode) = 0;
		virtual bool IsMouseButtonPressedImpl(int button) = 0;

		virtual std::pair<float, float> GetMousePositionImpl() = 0;
		virtual float GetMouseXImpl() = 0;
		virtual float GetMouseYImpl() = 0;
	private:
		static Input* s_Instance;
	};
}