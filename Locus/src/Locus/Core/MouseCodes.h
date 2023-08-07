#pragma once

// From glfw3.h keycodes
//#define LOCUS_MOUSE_BUTTON_1         0
//#define LOCUS_MOUSE_BUTTON_2         1
//#define LOCUS_MOUSE_BUTTON_3         2
//#define LOCUS_MOUSE_BUTTON_4         3
//#define LOCUS_MOUSE_BUTTON_5         4
//#define LOCUS_MOUSE_BUTTON_6         5
//#define LOCUS_MOUSE_BUTTON_7         6
//#define LOCUS_MOUSE_BUTTON_8         7
//#define LOCUS_MOUSE_BUTTON_LAST      LOCUS_MOUSE_BUTTON_8
//#define LOCUS_MOUSE_BUTTON_LEFT      LOCUS_MOUSE_BUTTON_1
//#define LOCUS_MOUSE_BUTTON_RIGHT     LOCUS_MOUSE_BUTTON_2
//#define LOCUS_MOUSE_BUTTON_MIDDLE    LOCUS_MOUSE_BUTTON_3

namespace Locus
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