// --- GraphicsContext --------------------------------------------------------
// Graphics context interface. Must implement render api specific context.
#pragma once

namespace Locus
{
	class GraphicsContext
	{
	public:
		virtual void Init() = 0;
		virtual void SwapBuffers() = 0;

		static Scope<GraphicsContext> Create(void* window);
	};
}