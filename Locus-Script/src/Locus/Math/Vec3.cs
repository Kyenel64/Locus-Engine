// --- Vec3 -------------------------------------------------------------------
using System;

namespace Locus
{
	/// <summary>
	/// 3D vector of floats.
	/// </summary>
	public struct Vec3
	{
		// --- Properties ---
		/// <summary> x value. </summary>
		public float x;
		/// <summary> y value. </summary>
		public float y;
		/// <summary> z value. </summary>
		public float z;

		// --- Static Properties ---
		/// <summary>
		/// Returns a 3D zero vector.
		/// </summary>
		public static Vec3 Zero => new Vec3(0.0f);

		// --- Constructors ---
		/// <summary>
		/// Sets all values to the given value.
		/// </summary>
		public Vec3(float value)
		{
			x = y = z = value;
		}
		/// <summary>
		/// Sets each value.
		/// </summary>
		public Vec3(float X, float Y, float Z)
		{
			x = X; y = Y; z = Z;
		}

		// --- Static Methods ---
		/// <summary>
		/// Cross product of two 3D vectors.
		/// </summary>
		public static Vec3 Cross(Vec3 v1, Vec3 v2)
		{
			InternalCalls.Vec3_Cross(v1, v2, out Vec3 result);
			return result;
		}
		/// <summary>
		/// Distance between two 3D vectors.
		/// </summary>
		public static float Distance(Vec3 v1, Vec3 v2)
		{
			return InternalCalls.Vec3_Distance(v1, v2);
		}
		/// <summary>
		/// Length of a 3D vector.
		/// </summary>
		public static float Length(Vec3 vec)
		{
			return InternalCalls.Vec3_Length(vec);
		}

		// --- Overrides ---
		/// <summary> Adds two 3D vectors. </summary>
		public static Vec3 operator +(Vec3 v1, Vec3 v2)
		{
			return new Vec3(v1.x + v2.x, v1.y + v2.y, v1.z + v2.z);
		}
		/// <summary> Adds a 3D vector with a scalar. </summary>
		public static Vec3 operator +(Vec3 vec, float s)
		{
			return new Vec3(vec.x + s, vec.y + s, vec.z + s);
		}
		/// <summary> Adds a scalar value to each value in a 3D vector. </summary>
		public static Vec3 operator +(float s, Vec3 vec)
		{
			return new Vec3(s + vec.x, s + vec.y, s + vec.z);
		}
		/// <summary> Subtracts v2 from v1. </summary>
		public static Vec3 operator -(Vec3 v1, Vec3 v2)
		{
			return new Vec3(v1.x - v2.x, v1.y - v2.y, v1.z - v2.z);
		}
		/// <summary> Subtracts a scalar value from a 3D vector. </summary>
		public static Vec3 operator -(Vec3 vec, float s)
		{
			return new Vec3(vec.x - s, vec.y - s, vec.z - s);
		}
		/// <summary> Subtracts each value in a 3D vector by a scalar value. </summary>
		public static Vec3 operator -(float s, Vec3 vec)
		{
			return new Vec3(s - vec.x, s - vec.y, s - vec.z);
		}
		/// <summary> Multiplies two 3D vectors. </summary>
		public static Vec3 operator *(Vec3 v1, Vec3 v2)
		{
			return new Vec3(v1.x * v2.x, v1.y * v2.y, v1.z * v2.z);
		}
		/// <summary> Multiplies a 3D vector with a scalar value. </summary>
		public static Vec3 operator *(Vec3 vec, float s)
		{
			return new Vec3(vec.x * s, vec.y * s, vec.z * s);
		}
		/// <summary> Multiplies each value in a 3D vector with a scalar value. </summary>
		public static Vec3 operator *(float s, Vec3 vec)
		{
			return new Vec3(s * vec.x, s * vec.y, s * vec.z);
		}
		/// <summary> Divides v1 by v2. </summary>
		public static Vec3 operator /(Vec3 v1, Vec3 v2)
		{
			return new Vec3(v1.x / v2.x, v1.y / v2.y, v1.z / v2.z);
		}
		/// <summary> Divides a 3D vector by a scalar value. </summary>
		public static Vec3 operator /(Vec3 vec, float s)
		{
			return new Vec3(vec.x / s, vec.y / s, vec.z / s);
		}
		/// <summary> Divides a scalar value by each value in a 3D vector. </summary>
		public static Vec3 operator /(float s, Vec3 vec)
		{
			return new Vec3(s / vec.x, s / vec.y, s / vec.z);
		}
		/// <summary> Multiplies a 3D vector by a 4x4 matrix. </summary>
		public static Vec3 operator *(Vec3 vec, Mat4 mat)
		{
			Vec3 result = new Vec3();
			result.x = vec.x * mat.m00 + vec.y * mat.m10 + vec.z * mat.m20 + 1.0f * mat.m30;
			result.y = vec.x * mat.m01 + vec.y * mat.m11 + vec.z * mat.m21 + 1.0f * mat.m31;
			result.z = vec.x * mat.m02 + vec.y * mat.m12 + vec.z * mat.m22 + 1.0f * mat.m32;
			return result;
		}
		/// <summary> Prints each value in a 3D vector. </summary>
		public override String ToString()
		{
			return String.Format("{0}, {1}, {2}", x, y, z);
		}

	}
}
