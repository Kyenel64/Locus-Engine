// --- Vec2 -------------------------------------------------------------------
using System;

namespace Locus
{
    /// <summary>
    /// Vector 2 class.
    /// </summary>
    public struct Vec2
    {
        // --- Properties ---
        /// <summary>
        /// x value.
        /// </summary>
        public float x;
        /// <summary>
        /// y value.
        /// </summary>
        public float y;

        // --- Static Properties ---
        /// <summary>
        /// Returns a new zero vector.
        /// </summary>
        public static Vec2 Zero => new Vec2(0.0f);

        // --- Constructors ---
        /// <summary>
        /// Sets all values to the given value.
        /// </summary>
        public Vec2(float value)
        {
            x = y = value;
        }
        /// <summary>
        /// Sets each value.
        /// </summary>
        public Vec2(float X, float Y)
        {
            x = X; y = Y;
        }

        // --- Static Methods ---
        /// <summary>
        /// Distance between two vectors.
        /// </summary>
        public static float Distance(Vec2 v1, Vec2 v2)
        {
            return InternalCalls.Vec2_Distance(v1, v2);
        }
        /// <summary>
        /// Length of a vector.
        /// </summary>
        public static float Length(Vec2 vec)
        {
            return InternalCalls.Vec2_Length(vec);
        }

        // --- Overrides ---
        public static Vec2 operator +(Vec2 v1, Vec2 v2)
        {
            return new Vec2(v1.x + v2.x, v1.y + v2.y);
        }
        public static Vec2 operator +(Vec2 vec, float s)
        {
            return new Vec2(vec.x + s, vec.y + s);
        }
        public static Vec2 operator +(float s, Vec2 vec)
        {
            return new Vec2(s + vec.x, s + vec.y);
        }
        public static Vec2 operator -(Vec2 v1, Vec2 v2)
        {
            return new Vec2(v1.x - v2.x, v1.y - v2.y);
        }
        public static Vec2 operator -(Vec2 vec, float s)
        {
            return new Vec2(vec.x - s, vec.y - s);
        }
        public static Vec2 operator -(float s, Vec2 vec)
        {
            return new Vec2(s - vec.x, s - vec.y);
        }
        public static Vec2 operator *(Vec2 v1, Vec2 v2)
        {
            return new Vec2(v1.x * v2.x, v1.y * v2.y);
        }
        public static Vec2 operator *(Vec2 vec, float s)
        {
            return new Vec2(vec.x * s, vec.y * s);
        }
        public static Vec2 operator *(float s, Vec2 vec)
        {
            return new Vec2(s * vec.x, s * vec.y);
        }
        public static Vec2 operator /(Vec2 v1, Vec2 v2)
        {
            return new Vec2(v1.x / v2.x, v1.y / v2.y);
        }
        public static Vec2 operator /(Vec2 vec, float s)
        {
            return new Vec2(vec.x / s, vec.y / s);
        }
        public static Vec2 operator /(float s, Vec2 vec)
        {
            return new Vec2(s / vec.x, s / vec.y);
        }
        public override String ToString()
        {
            return String.Format("{0}, {1}", x, y);
        }
    }
}
