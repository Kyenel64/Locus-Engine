#include "Lpch.h"
#include "RendererStats.h"

namespace Locus
{

	static RendererStatisticsData s_Data;

	void RendererStats::ResetStats()
	{
		//memset(&s_Data.Stats, 0, sizeof(Statistics));
		s_Data.DrawCalls = 0;
		s_Data.QuadCount = 0;
		s_Data.CubeCount = 0;
		s_Data.FrameTime = 0;
	}

	RendererStatisticsData& RendererStats::GetStats()
	{
		return s_Data;
	}

	void RendererStats::StatsStartFrame()
	{
		s_Data.StartTime = std::chrono::steady_clock().now();
	}

	void RendererStats::StatsEndFrame()
	{
		s_Data.EndTime = std::chrono::steady_clock().now();
		long long start = std::chrono::time_point_cast<std::chrono::microseconds>(s_Data.StartTime).time_since_epoch().count();
		long long end = std::chrono::time_point_cast<std::chrono::microseconds>(s_Data.EndTime).time_since_epoch().count();
		s_Data.FrameTime = (end - start) * 0.001f;
		s_Data.FramesPerSecond = 1.0f / s_Data.FrameTime * 1000.0f;

	}
}
