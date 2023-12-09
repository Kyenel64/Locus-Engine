// --- Color ------------------------------------------------------------------
using System;

namespace Locus
{
    /// <summary>
    /// Color class. Represents a color in RGBA format.
    /// </summary>
    public struct Color
    {
        // --- Properties ---
        /// <summary>
        /// red channel.
        /// </summary>
        public float r;
        /// <summary>
        /// green channel.
        /// </summary>
        public float g;
        /// <summary>
        /// blue channel.
        /// </summary>
        public float b;
        /// <summary>
        /// alpha channel.
        /// </summary>
        public float a;

        // --- Static Properties ---
        /// <summary>
        /// Returns a black color.
        /// </summary>
        public static Color Black => new Color(0.0f);
        public static Color White => new Color(1.0f);
        public static Color Red => new Color(1.0f, 0.0f, 0.0f, 1.0f);
        public static Color Green => new Color(0.0f, 1.0f, 0.0f, 1.0f);
        public static Color Blue => new Color(0.0f, 0.0f, 1.0f, 1.0f);
        public static Color Yellow => new Color(1.0f, 1.0f, 0.0f, 1.0f);

        // --- Constructors ---
        /// <summary>
        /// Sets the RGB channels to the given value.
        /// </summary>
        public Color(float value)
        {
            r = g = b = value;
            a = 1.0f;
        }
        /// <summary>
         /// Sets the RGB channels with alpha = 1.0f.
         /// </summary>
        public Color(float R, float G, float B)
        {
            r = R; g = G; b = B; a = 1.0f;
        }
        /// <summary>
        /// Sets each channel values.
        /// </summary>
        public Color(float R, float G, float B, float A)
        {
            r = R; g = G; b = B; a = A;
        }

        // --- Overrides ---
        public static Color operator +(Color v1, Color v2)
        {
            return new Color(v1.r + v2.r, v1.g + v2.g, v1.b + v2.b, v1.a + v2.a);
        }
        public static Color operator +(Color vec, float s)
        {
            return new Color(vec.r + s, vec.g + s, vec.b + s, vec.a + s);
        }
        public static Color operator +(float s, Color vec)
        {
            return new Color(s + vec.r, s + vec.g, s + vec.b, s + vec.a);
        }
        public static Color operator -(Color v1, Color v2)
        {
            return new Color(v1.r - v2.r, v1.g - v2.g, v1.b - v2.b, v1.a - v2.a);
        }
        public static Color operator -(Color vec, float s)
        {
            return new Color(vec.r - s, vec.g - s, vec.b - s, vec.a - s);
        }
        public static Color operator -(float s, Color vec)
        {
            return new Color(s - vec.r, s - vec.g, s - vec.b, s - vec.a);
        }
        public static Color operator *(Color v1, Color v2)
        {
            return new Color(v1.r * v2.r, v1.g * v2.g, v1.b * v2.b, v1.a * v2.a);
        }
        public static Color operator *(Color vec, float s)
        {
            return new Color(vec.r * s, vec.g * s, vec.b * s, vec.a * s);
        }
        public static Color operator *(float s, Color vec)
        {
            return new Color(s * vec.r, s * vec.g, s * vec.b, s * vec.a);
        }
        public static Color operator *(Color vec, Mat4 mat)
        {
            Color result = new Color();
            result.r = vec.r * mat.m00 + vec.g * mat.m10 + vec.b * mat.m20 + vec.a * mat.m30;
            result.g = vec.r * mat.m01 + vec.g * mat.m11 + vec.b * mat.m21 + vec.a * mat.m31;
            result.b = vec.r * mat.m02 + vec.g * mat.m12 + vec.b * mat.m22 + vec.a * mat.m32;
            result.a = vec.r * mat.m03 + vec.g * mat.m13 + vec.b * mat.m23 + vec.a * mat.m33;
            return result;
        }
        public static Color operator /(Color v1, Color v2)
        {
            return new Color(v1.r / v2.r, v1.g / v2.g, v1.b / v2.b, v1.a / v2.a);
        }
        public static Color operator /(Color vec, float s)
        {
            return new Color(vec.r / s, vec.g / s, vec.b / s, vec.a / s);
        }
        public static Color operator /(float s, Color vec)
        {
            return new Color(s / vec.r, s / vec.g, s / vec.b, s / vec.a);
        }
        public override String ToString()
        {
            return String.Format("{0}, {1}, {2}, {3}", r, g, b, a);
        }
    }
}
