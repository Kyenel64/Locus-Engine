// --- Color ------------------------------------------------------------------
using System;

namespace Locus
{
	/// <summary>
	/// Represents a color in RGBA format with range 0 - 1.
	/// </summary>
	public struct Color
	{
		// --- Properties ---
		/// <summary> Red channel. </summary>
		public float r;
		/// <summary> Green channel. </summary>
		public float g;
		/// <summary> Blue channel. </summary>
		public float b;
		/// <summary> Alpha channel. </summary>
		public float a;

		// --- Static Properties ---
		/// <summary> Returns a black color. </summary>
		public static Color Black => new Color(0.0f);
		/// <summary> Returns a white color. </summary>
		public static Color White => new Color(1.0f);
		/// <summary> Returns a red color. </summary>
		public static Color Red => new Color(1.0f, 0.0f, 0.0f, 1.0f);
		/// <summary> Returns a green color. </summary>
		public static Color Green => new Color(0.0f, 1.0f, 0.0f, 1.0f);
		/// <summary> Returns a blue color. </summary>
		public static Color Blue => new Color(0.0f, 0.0f, 1.0f, 1.0f);
		/// <summary> Returns a yellow color. </summary>
		public static Color Yellow => new Color(1.0f, 1.0f, 0.0f, 1.0f);

		// --- Constructors ---
		/// <summary>
		/// Sets the RGB channels to the value with alpha = 1.0f.
		/// </summary>
		public Color(float value)
		{
			r = g = b = value;
			a = 1.0f;
		}
		/// <summary>
		/// Sets each RGB channels with alpha = 1.0f.
		/// </summary>
		public Color(float R, float G, float B)
		{
			r = R; g = G; b = B; a = 1.0f;
		}
		/// <summary>
		/// Sets each RGBA channels.
		/// </summary>
		public Color(float R, float G, float B, float A)
		{
			r = R; g = G; b = B; a = A;
		}

		// --- Overrides ---
		/// <summary> Adds two colors. </summary>
		public static Color operator +(Color c1, Color c2)
		{
			return new Color(c1.r + c2.r, c1.g + c2.g, c1.b + c2.b, c1.a + c2.a);
		}
		/// <summary> Adds a color with a scalar. </summary>
		public static Color operator +(Color col, float s)
		{
			return new Color(col.r + s, col.g + s, col.b + s, col.a + s);
		}
		/// <summary> Adds a scalar value to each channel in a color. </summary>
		public static Color operator +(float s, Color col)
		{
			return new Color(s + col.r, s + col.g, s + col.b, s + col.a);
		}
		/// <summary> Subtracts c2 from c1. </summary>
		public static Color operator -(Color c1, Color c2)
		{
			return new Color(c1.r - c2.r, c1.g - c2.g, c1.b - c2.b, c1.a - c2.a);
		}
		/// <summary> Subtracts a scalar value from a color. </summary>
		public static Color operator -(Color col, float s)
		{
			return new Color(col.r - s, col.g - s, col.b - s, col.a - s);
		}
		/// <summary> Subtracts each channel in a color by a scalar value. </summary>
		public static Color operator -(float s, Color col)
		{
			return new Color(s - col.r, s - col.g, s - col.b, s - col.a);
		}
		/// <summary> Multiplies two colors. </summary>
		public static Color operator *(Color c1, Color c2)
		{
			return new Color(c1.r * c2.r, c1.g * c2.g, c1.b * c2.b, c1.a * c2.a);
		}
		/// <summary> Multiplies a color with a scalar value. </summary>
		public static Color operator *(Color col, float s)
		{
			return new Color(col.r * s, col.g * s, col.b * s, col.a * s);
		}
		/// <summary> Multiplies each channel in a color with a scalar value. </summary>
		public static Color operator *(float s, Color col)
		{
			return new Color(s * col.r, s * col.g, s * col.b, s * col.a);
		}
		/// <summary> Divides c1 by c2. </summary>
		public static Color operator /(Color c1, Color c2)
		{
			return new Color(c1.r / c2.r, c1.g / c2.g, c1.b / c2.b, c1.a / c2.a);
		}
		/// <summary> Divides a color by a scalar value. </summary>
		public static Color operator /(Color col, float s)
		{
			return new Color(col.r / s, col.g / s, col.b / s, col.a / s);
		}
		/// <summary> Divides a scalar value by each channel in a color. </summary>
		public static Color operator /(float s, Color col)
		{
			return new Color(s / col.r, s / col.g, s / col.b, s / col.a);
		}
		/// <summary> Prints each channel in a color. </summary>
		public override String ToString()
		{
			return String.Format("{0}, {1}, {2}, {3}", r, g, b, a);
		}
	}
}
