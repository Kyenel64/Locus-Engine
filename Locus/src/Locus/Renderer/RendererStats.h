#pragma once

namespace Locus
{
	struct RendererStatisticsData
	{
		// Render stats
		uint32_t DrawCalls = 0;
		uint32_t QuadCount = 0;
		uint32_t CubeCount = 0;

		uint32_t GetTotalVertexCount() { return QuadCount * 4; }
		uint32_t GetTotalIndexCount() { return QuadCount * 6; }

		// Frame stats
		std::chrono::time_point<std::chrono::steady_clock> StartTime;
		std::chrono::time_point<std::chrono::steady_clock> EndTime;
		float FrameTime = 0;
		float FramesPerSecond = 0;
	};

	class RendererStats
	{
	public:
		static void ResetStats();
		static RendererStatisticsData& GetStats();
		static void StatsStartFrame();
		static void StatsEndFrame();
	};
}