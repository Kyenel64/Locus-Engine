// --- GraphicsContext --------------------------------------------------------
// Graphics context interface. Must implement render api specific subclasses.

#pragma once

namespace Tiel
{
	class GraphicsContext
	{
	public:
		virtual void Init() = 0;
		virtual void SwapBuffers() = 0;

		static Scope<GraphicsContext> Create(void* window);
	};
}