// --- Mat4 -------------------------------------------------------------------
using System;

namespace Locus
{
    /// <summary>
    /// Matrix 4 class.
    /// </summary>
    public struct Mat4
    {
        // --- Properties ---
        private float m00;
        private float m01;
        private float m02;
        private float m03;

        private float m10;
        private float m11;
        private float m12;
        private float m13;

        private float m20;
        private float m21;
        private float m22;
        private float m23;

        private float m30;
        private float m31;
        private float m32;
        private float m33;

        // --- Constructors ---
        /// <summary>
        /// Sets the diagonal to the given value.
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
