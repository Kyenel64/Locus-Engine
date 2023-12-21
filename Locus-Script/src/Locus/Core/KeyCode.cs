// --- KeyCode ----------------------------------------------------------------
// Same as GLFW3's keycodes.
#pragma warning disable 1591

namespace Locus
{
	/// <summary>
	/// KeyCodes corresponding to physical keys on the keyboard. 
	/// Numerical keycodes begin with a "D" (Eg. D0, D1)
	/// </summary>
	public enum KeyCode
	{
		/// <summary>
		/// Spacebar
		/// </summary>
		Space = 32,
		/// <summary>
		/// '
		/// </summary>
		Apostrophe = 39,
		/// <summary>
		/// ,
		/// </summary>
		Comma = 44,
		/// <summary>
		/// -
		/// </summary>
		Minus = 45,
		/// <summary>
		/// .
		/// </summary>
		Period = 46,
		/// <summary>
		/// /
		/// </summary>
		Slash = 47,

		/// <summary>
		/// 0
		/// </summary>
		D0 = 48,
		/// <summary>
		/// 1
		/// </summary>
		D1 = 49,
		/// <summary>
		/// 2
		/// </summary>
		D2 = 50,
		/// <summary>
		/// 3
		/// </summary>
		D3 = 51,
		/// <summary>
		/// 4
		/// </summary>
		D4 = 52,
		/// <summary>
		/// 5
		/// </summary>
		D5 = 53,
		/// <summary>
		/// 6
		/// </summary>
		D6 = 54,
		/// <summary>
		/// 7
		/// </summary>
		D7 = 55,
		/// <summary>
		/// 8
		/// </summary>
		D8 = 56,
		/// <summary>
		/// 9
		/// </summary>
		D9 = 57,

		/// <summary>
		/// ;
		/// </summary>
		Semicolon = 59,
		/// <summary>
		/// =
		/// </summary>
		Equal = 61,

		A = 65,
		B = 66,
		C = 67,
		D = 68,
		E = 69,
		F = 70,
		G = 71,
		H = 72,
		I = 73,
		J = 74,
		K = 75,
		L = 76,
		M = 77,
		N = 78,
		O = 79,
		P = 80,
		Q = 81,
		R = 82,
		S = 83,
		T = 84,
		U = 85,
		V = 86,
		W = 87,
		X = 88,
		Y = 89,
		Z = 90,

		/// <summary>
		/// [
		/// </summary>
		LeftBracket = 91,
		/// <summary>
		/// \
		/// </summary>
		Backslash = 92,
		/// <summary>
		/// ]
		/// </summary>
		RightBracket = 93,
		/// <summary>
		/// `
		/// </summary>
		GraveAccent = 96,

		/// <summary>
		/// non-US #1
		/// </summary>
		World1 = 161,
		/// <summary>
		/// non-US #2
		/// </summary>
		World2 = 162,

		/* Function keys */
		Escape = 256,
		Enter = 257,
		Tab = 258,
		Backspace = 259,
		Insert = 260,
		Delete = 261,
		Right = 262,
		Left = 263,
		Down = 264,
		Up = 265,
		PageUp = 266,
		PageDown = 267,
		Home = 268,
		End = 269,
		CapsLock = 280,
		ScrollLock = 281,
		NumLock = 282,
		PrintScreen = 283,
		Pause = 284,
		F1 = 290,
		F2 = 291,
		F3 = 292,
		F4 = 293,
		F5 = 294,
		F6 = 295,
		F7 = 296,
		F8 = 297,
		F9 = 298,
		F10 = 299,
		F11 = 300,
		F12 = 301,
		F13 = 302,
		F14 = 303,
		F15 = 304,
		F16 = 305,
		F17 = 306,
		F18 = 307,
		F19 = 308,
		F20 = 309,
		F21 = 310,
		F22 = 311,
		F23 = 312,
		F24 = 313,
		F25 = 314,

		/* Keypad */
		KP0 = 320,
		KP1 = 321,
		KP2 = 322,
		KP3 = 323,
		KP4 = 324,
		KP5 = 325,
		KP6 = 326,
		KP7 = 327,
		KP8 = 328,
		KP9 = 329,
		KPDecimal = 330,
		KPDivide = 331,
		KPMultiply = 332,
		KPSubtract = 333,
		KPAdd = 334,
		KPEnter = 335,
		KPEqual = 336,

		LeftShift = 340,
		LeftControl = 341,
		LeftAlt = 342,
		LeftSuper = 343,
		RightShift = 344,
		RightControl = 345,
		RightAlt = 346,
		RightSuper = 347,
		Menu = 348
	}
}
