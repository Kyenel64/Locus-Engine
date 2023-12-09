// --- Vec4 -------------------------------------------------------------------
using System;

namespace Locus
{
    /// <summary>
    /// Vector 4 class. Contains static methods for common vector operations.
    /// </summary>
    public struct Vec4
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
        /// <summary>
        /// z value.
        /// </summary>
        public float z;
        /// <summary>
        /// w value.
        /// </summary>
        public float w;

        // --- Static Properties ---
        /// <summary>
        /// Returns a new zero vector.
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

        /// <summary>
        /// Distance between two vectors.
        /// </summary>
        public static float Distance(Vec4 v1, Vec4 v2)
        {
            return InternalCalls.Vec4_Distance(v1, v2);
        }

        /// <summary>
        /// Length of a vector.
        /// </summary>
        public static float Length(Vec4 vec)
        {
            return InternalCalls.Vec4_Length(vec);
        }

        // --- Overrides ---
        public static Vec4 operator +(Vec4 v1, Vec4 v2)
        {
            return new Vec4(v1.x + v2.x, v1.y + v2.y, v1.z + v2.z, v1.w + v2.w);
        }
        public static Vec4 operator +(Vec4 vec, float s)
        {
            return new Vec4(vec.x + s, vec.y + s, vec.z + s, vec.w + s);
        }
        public static Vec4 operator +(float s, Vec4 vec)
        {
            return new Vec4(s + vec.x, s + vec.y, s + vec.z, s + vec.w);
        }
        public static Vec4 operator -(Vec4 v1, Vec4 v2)
        {
            return new Vec4(v1.x - v2.x, v1.y - v2.y, v1.z - v2.z, v1.w - v2.w);
        }
        public static Vec4 operator -(Vec4 vec, float s)
        {
            return new Vec4(vec.x - s, vec.y - s, vec.z - s, vec.w - s);
        }
        public static Vec4 operator -(float s, Vec4 vec)
        {
            return new Vec4(s - vec.x, s - vec.y, s - vec.z, s - vec.w);
        }
        public static Vec4 operator *(Vec4 v1, Vec4 v2)
        {
            return new Vec4(v1.x * v2.x, v1.y * v2.y, v1.z * v2.z, v1.w * v2.w);
        }
        public static Vec4 operator *(Vec4 vec, float s)
        {
            return new Vec4(vec.x * s, vec.y * s, vec.z * s, vec.w * s);
        }
        public static Vec4 operator *(float s, Vec4 vec)
        {
            return new Vec4(s * vec.x, s * vec.y, s * vec.z, s * vec.w);
        }
        public static Vec4 operator *(Vec4 vec, Mat4 mat)
        {
            Vec4 result = new Vec4();
            result.x = vec.x * mat.m00 + vec.y * mat.m10 + vec.z * mat.m20 + vec.w * mat.m30;
            result.y = vec.x * mat.m01 + vec.y * mat.m11 + vec.z * mat.m21 + vec.w * mat.m31;
            result.z = vec.x * mat.m02 + vec.y * mat.m12 + vec.z * mat.m22 + vec.w * mat.m32;
            result.w = vec.x * mat.m03 + vec.y * mat.m13 + vec.z * mat.m23 + vec.w * mat.m33;
            return result;
        }
        public static Vec4 operator /(Vec4 v1, Vec4 v2)
        {
            return new Vec4(v1.x / v2.x, v1.y / v2.y, v1.z / v2.z, v1.w / v2.w);
        }
        public static Vec4 operator /(Vec4 vec, float s)
        {
            return new Vec4(vec.x / s, vec.y / s, vec.z / s, vec.w / s);
        }
        public static Vec4 operator /(float s, Vec4 vec)
        {
            return new Vec4(s / vec.x, s / vec.y, s / vec.z, s / vec.w);
        }
        public override String ToString()
        {
            return String.Format("{0}, {1}, {2}, {3}", x, y, z, w);
        }
    }
}
