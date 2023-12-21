// --- Vec4 -------------------------------------------------------------------
using System;

namespace Locus
{
	/// <summary>
	/// 4D vector of floats.
	/// </summary>
	public struct Vec4
	{
		// --- Properties ---
		/// <summary> x value. </summary>
		public float x;
		/// <summary> y value. </summary>
		public float y;
		/// <summary> z value. </summary>
		public float z;
		/// <summary> w value. </summary>
		public float w;

		// --- Static Properties ---
		/// <summary>
		/// Returns a 4D zero vector.
		/// </summary>
		public static Vec4 Zero => new Vec4(0.0f);

		// --- Constructors ---
		/// <summary>
		/// Sets all values to the given value.
		/// </summary>
		public Vec4(float value)
		{
			x = y = z = w = value;
		}
		/// <summary>
		/// Sets each value.
		/// </summary>
		public Vec4(float X, float Y, float Z, float W)
		{
			x = X; y = Y; z = Z; w = W;
		}

		// --- Static Methods ---
		/// <summary>
		/// Distance between two 4D vectors.
		/// </summary>
		public static float Distance(Vec4 v1, Vec4 v2)
		{
			return InternalCalls.Vec4_Distance(v1, v2);
		}
		/// <summary>
		/// Length of a 4D vector.
		/// </summary>
		public static float Length(Vec4 vec)
		{
			return InternalCalls.Vec4_Length(vec);
		}

		// --- Overrides ---
		/// <summary> Adds two 4D vectors. </summary>
		public static Vec4 operator +(Vec4 v1, Vec4 v2)
		{
			return new Vec4(v1.x + v2.x, v1.y + v2.y, v1.z + v2.z, v1.w + v2.w);
		}
		/// <summary> Adds a 4D vector with a scalar. </summary>
		public static Vec4 operator +(Vec4 vec, float s)
		{
			return new Vec4(vec.x + s, vec.y + s, vec.z + s, vec.w + s);
		}
		/// <summary> Adds a scalar value to each value in a 4D vector. </summary>
		public static Vec4 operator +(float s, Vec4 vec)
		{
			return new Vec4(s + vec.x, s + vec.y, s + vec.z, s + vec.w);
		}
		/// <summary> Subtracts v2 from v1. </summary>
		public static Vec4 operator -(Vec4 v1, Vec4 v2)
		{
			return new Vec4(v1.x - v2.x, v1.y - v2.y, v1.z - v2.z, v1.w - v2.w);
		}
		/// <summary> Subtracts a scalar value from a 4D vector. </summary>
		public static Vec4 operator -(Vec4 vec, float s)
		{
			return new Vec4(vec.x - s, vec.y - s, vec.z - s, vec.w - s);
		}
		/// <summary> Subtracts each value in a 4D vector by a scalar value. </summary>
		public static Vec4 operator -(float s, Vec4 vec)
		{
			return new Vec4(s - vec.x, s - vec.y, s - vec.z, s - vec.w);
		}
		/// <summary> Multiplies two 4D vectors. </summary>
		public static Vec4 operator *(Vec4 v1, Vec4 v2)
		{
			return new Vec4(v1.x * v2.x, v1.y * v2.y, v1.z * v2.z, v1.w * v2.w);
		}
		/// <summary> Multiplies a 4D vector with a scalar value. </summary>
		public static Vec4 operator *(Vec4 vec, float s)
		{
			return new Vec4(vec.x * s, vec.y * s, vec.z * s, vec.w * s);
		}
		/// <summary> Multiplies each value in a 4D vector with a scalar value. </summary>
		public static Vec4 operator *(float s, Vec4 vec)
		{
			return new Vec4(s * vec.x, s * vec.y, s * vec.z, s * vec.w);
		}
		/// <summary> Divides v1 by v2. </summary>
		public static Vec4 operator /(Vec4 v1, Vec4 v2)
		{
			return new Vec4(v1.x / v2.x, v1.y / v2.y, v1.z / v2.z, v1.w / v2.w);
		}
		/// <summary> Divides a 4D vector by a scalar value. </summary>
		public static Vec4 operator /(Vec4 vec, float s)
		{
			return new Vec4(vec.x / s, vec.y / s, vec.z / s, vec.w / s);
		}
		/// <summary> Divides a scalar value by each value in a 4D vector. </summary>
		public static Vec4 operator /(float s, Vec4 vec)
		{
			return new Vec4(s / vec.x, s / vec.y, s / vec.z, s / vec.w);
		}
		/// <summary> Multiplies a 4D vector by a 4x4 matrix. </summary>
		public static Vec4 operator *(Vec4 vec, Mat4 mat)
		{
			Vec4 result = new Vec4();
			result.x = vec.x * mat.m00 + vec.y * mat.m10 + vec.z * mat.m20 + vec.w * mat.m30;
			result.y = vec.x * mat.m01 + vec.y * mat.m11 + vec.z * mat.m21 + vec.w * mat.m31;
			result.z = vec.x * mat.m02 + vec.y * mat.m12 + vec.z * mat.m22 + vec.w * mat.m32;
			result.w = vec.x * mat.m03 + vec.y * mat.m13 + vec.z * mat.m23 + vec.w * mat.m33;
			return result;
		}
		/// <summary> Prints each value in a 4D vector. </summary>
		public override String ToString()
		{
			return String.Format("{0}, {1}, {2}, {3}", x, y, z, w);
		}
	}
}
