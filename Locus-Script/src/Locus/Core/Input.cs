// --- Input ------------------------------------------------------------------

namespace Locus
{
	/// <summary>
	/// Input system for Locus.
	/// </summary>
	public class Input
	{
		/// <summary>
		/// Returns true when key is pressed.
		/// </summary>
		public static bool IsKeyPressed(KeyCode key)
		{
			return InternalCalls.Input_IsKeyPressed(key);
		}
		/// <summary>
		/// Returns true while key is held down.
		/// </summary>
		public static bool IsKeyHeld(KeyCode key)
		{
			return InternalCalls.Input_IsKeyHeld(key);
		}
		/// <summary>
		/// Returns true when key is released.
		/// </summary>
		public static bool IsKeyReleased(KeyCode key)
		{
			return InternalCalls.Input_IsKeyReleased(key);
		}
	}
}
