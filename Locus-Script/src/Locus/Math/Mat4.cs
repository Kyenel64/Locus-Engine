// --- Mat4 -------------------------------------------------------------------
using System;

#pragma warning disable 1591

namespace Locus
{
	/// <summary>
	/// 4x4 matrix of floats.
	/// </summary>
	public struct Mat4
	{
		// --- Properties ---
		public float m00;
		public float m01;
		public float m02;
		public float m03;

		public float m10;
		public float m11;
		public float m12;
		public float m13;

		public float m20;
		public float m21;
		public float m22;
		public float m23;

		public float m30;
		public float m31;
		public float m32;
		public float m33;

		// --- Constructors ---
		/// <summary>
		/// Sets a diagonal matrix.
		/// </summary>
		public Mat4(float value)
		{
			m00 = value; m01 = 0.0f;  m02 = 0.0f;  m03 = 0.0f;
			m10 = 0.0f;  m11 = value; m12 = 0.0f;  m13 = 0.0f;
			m20 = 0.0f;  m21 = 0.0f;  m22 = value; m23 = 0.0f;
			m30 = 0.0f;  m31 = 0.0f;  m32 = 0.0f;  m33 = value;
		}
	}
}
