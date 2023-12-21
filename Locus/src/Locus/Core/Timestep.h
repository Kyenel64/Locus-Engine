// --- Timestep ---------------------------------------------------------------
// Deltatime. Used in the update loop.
#pragma once

namespace Locus
{
	class Timestep
	{
	public:
		Timestep(float time = 0.0f) : m_Time(time) {}
		~Timestep() = default;

		operator float() const { return m_Time; }
		
		float GetSeconds() const { return m_Time; }
		float GetMilliseconds() const { return m_Time * 1000.f; }
	private:
		float m_Time;
	};
}
