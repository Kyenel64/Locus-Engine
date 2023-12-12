#include "Lpch.h"
#include "Locus/Core/Input.h"

#include <GLFW/glfw3.h>

#include "Locus/Core/Application.h"

namespace Locus
{
	bool Input::IsKeyPressed(const KeyCode keycode)
	{
		return s_KeyState.find(keycode) != s_KeyState.end() && s_KeyState[keycode] == KeyState::Pressed;
	}

	bool Input::IsKeyHeld(KeyCode keycode)
	{
		return s_KeyState.find(keycode) != s_KeyState.end() && s_KeyState[keycode] == KeyState::Repeat;
	}

	bool Input::IsKeyReleased(KeyCode keycode)
	{
		return s_KeyState.find(keycode) != s_KeyState.end() && s_KeyState[keycode] == KeyState::Released;
	}

	void Input::SetKeyState(KeyCode keycode, KeyState keystate)
	{
		s_KeyState[keycode] = keystate;
		if (keystate != KeyState::None)
			s_UnhandledKeys.push(keycode);
	}

	KeyState Input::GetKeyState(KeyCode keycode)
	{
		return s_KeyState.find(keycode) != s_KeyState.end() ? s_KeyState[keycode] : KeyState::None;
	}

	void Input::ProcessKeys()
	{
		while (!s_UnhandledKeys.empty())
		{
			KeyCode key = s_UnhandledKeys.front();
			// Set keystate to repeat right after first pressed event.
			if (Input::GetKeyState(key) == KeyState::Pressed)
				Input::SetKeyState(key, KeyState::Repeat);
			if (Input::GetKeyState(key) == KeyState::Released)
				Input::SetKeyState(key, KeyState::None);
			s_UnhandledKeys.pop();
		}
	}

	bool Input::IsMouseButtonPressed(const KeyCode button)
	{
		auto* window = static_cast<GLFWwindow*>(Application::Get().GetWindow().GetNativeWindow());
		auto state = glfwGetMouseButton(window, button);
		return state == GLFW_PRESS;
	}

	glm::vec2 Input::GetMousePosition()
	{
		auto* window = static_cast<GLFWwindow*>(Application::Get().GetWindow().GetNativeWindow());
		double xpos, ypos;
		glfwGetCursorPos(window, &xpos, &ypos);
		return { (float)xpos, (float)ypos };
	}

	float Input::GetMouseX()
	{
		return GetMousePosition().x;
	}
	
	float Input::GetMouseY()
	{
		return GetMousePosition().y;
	}
}
