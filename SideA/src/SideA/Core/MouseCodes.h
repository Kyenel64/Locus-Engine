#pragma once

// From glfw3.h keycodes
//#define SIDEA_MOUSE_BUTTON_1         0
//#define SIDEA_MOUSE_BUTTON_2         1
//#define SIDEA_MOUSE_BUTTON_3         2
//#define SIDEA_MOUSE_BUTTON_4         3
//#define SIDEA_MOUSE_BUTTON_5         4
//#define SIDEA_MOUSE_BUTTON_6         5
//#define SIDEA_MOUSE_BUTTON_7         6
//#define SIDEA_MOUSE_BUTTON_8         7
//#define SIDEA_MOUSE_BUTTON_LAST      SIDEA_MOUSE_BUTTON_8
//#define SIDEA_MOUSE_BUTTON_LEFT      SIDEA_MOUSE_BUTTON_1
//#define SIDEA_MOUSE_BUTTON_RIGHT     SIDEA_MOUSE_BUTTON_2
//#define SIDEA_MOUSE_BUTTON_MIDDLE    SIDEA_MOUSE_BUTTON_3

namespace SideA
{
	using MouseCode = uint16_t;

	namespace Mouse
	{
		enum : MouseCode
		{
			// From GLFW3 keycodes
			Button0 = 0,
			Button1 = 1,
			Button2 = 2,
			Button3 = 3,
			Button4 = 4,
			Button5 = 5,
			Button6 = 6,
			Button7 = 7,

			ButtonLast = Button7,
			ButtonLeft = Button0,
			ButtonRight = Button1,
			ButtonMiddle = Button2
		};
	}
}