
namespace Locus
{
    /// <summary>
    /// Input system for Locus.
    /// </summary>
    public class Input
    {
        /// <summary>
        /// Checks if a key is pressed.
        /// </summary>
        public static bool IsKeyPressed(KeyCode key)
        {
            return InternalCalls.Input_IsKeyPressed(key);
        }
        public static bool IsKeyHeld(KeyCode key)
        {
            return InternalCalls.Input_IsKeyHeld(key);
        }
        public static bool IsKeyReleased(KeyCode key)
        {
            return InternalCalls.Input_IsKeyReleased(key);
        }
    }
}
