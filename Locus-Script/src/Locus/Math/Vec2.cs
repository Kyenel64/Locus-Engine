// --- Vec2 -------------------------------------------------------------------
using System;

namespace Locus
{
	/// <summary>
	/// 2D vector of floats.
	/// </summary>
	public struct Vec2
	{
		// --- Properties ---
		/// <summary> x value. </summary>
		public float x;
		/// <summary> y value. </summary>
		public float y;

		// --- Static Properties ---
		/// <summary>
		/// Returns a 2D zero vector.
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
		/// Distance between two 2D vectors.
		/// </summary>
		public static float Distance(Vec2 v1, Vec2 v2)
		{
			return InternalCalls.Vec2_Distance(v1, v2);
		}
		/// <summary>
		/// Length of a 2D vector.
		/// </summary>
		public static float Length(Vec2 vec)
		{
			return InternalCalls.Vec2_Length(vec);
		}

		// --- Overrides ---
		/// <summary> Adds two 2D vectors. </summary>
		public static Vec2 operator +(Vec2 v1, Vec2 v2)
		{
			return new Vec2(v1.x + v2.x, v1.y + v2.y);
		}
		/// <summary> Adds a 2D vector with a scalar. </summary>
		public static Vec2 operator +(Vec2 vec, float s)
		{
			return new Vec2(vec.x + s, vec.y + s);
		}
		/// <summary> Adds a scalar value to each value in a 2D vector. </summary>
		public static Vec2 operator +(float s, Vec2 vec)
		{
			return new Vec2(s + vec.x, s + vec.y);
		}
		/// <summary> Subtracts v2 from v1. </summary>
		public static Vec2 operator -(Vec2 v1, Vec2 v2)
		{
			return new Vec2(v1.x - v2.x, v1.y - v2.y);
		}
		/// <summary> Subtracts a scalar value from a 2D vector. </summary>
		public static Vec2 operator -(Vec2 vec, float s)
		{
			return new Vec2(vec.x - s, vec.y - s);
		}
		/// <summary> Subtracts each value in a 2D vector by a scalar value. </summary>
		public static Vec2 operator -(float s, Vec2 vec)
		{
			return new Vec2(s - vec.x, s - vec.y);
		}
		/// <summary> Multiplies two 2D vectors. </summary>
		public static Vec2 operator *(Vec2 v1, Vec2 v2)
		{
			return new Vec2(v1.x * v2.x, v1.y * v2.y);
		}
		/// <summary> Multiplies a 2D vector with a scalar value. </summary>
		public static Vec2 operator *(Vec2 vec, float s)
		{
			return new Vec2(vec.x * s, vec.y * s);
		}
		/// <summary> Multiplies each value in a 2D vector with a scalar value. </summary>
		public static Vec2 operator *(float s, Vec2 vec)
		{
			return new Vec2(s * vec.x, s * vec.y);
		}
		/// <summary> Divides v1 by v2. </summary>
		public static Vec2 operator /(Vec2 v1, Vec2 v2)
		{
			return new Vec2(v1.x / v2.x, v1.y / v2.y);
		}
		/// <summary> Divides a 2D vector by a scalar value. </summary>
		public static Vec2 operator /(Vec2 vec, float s)
		{
			return new Vec2(vec.x / s, vec.y / s);
		}
		/// <summary> Divides a scalar value by each value in a 2D vector. </summary>
		public static Vec2 operator /(float s, Vec2 vec)
		{
			return new Vec2(s / vec.x, s / vec.y);
		}
		/// <summary> Prints each value in a 2D vector. </summary>
		public override String ToString()
		{
			return String.Format("{0}, {1}", x, y);
		}
	}
}
