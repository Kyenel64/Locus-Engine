#include "Lpch.h"
#include "OpenGLRendererAPI.h"

namespace Locus
{
	static void OpenGLMessageCallback( unsigned source, unsigned type, unsigned id, unsigned severity,
		int length, const char* message, const void* userParam)
	{
		switch (severity)
		{
			case GL_DEBUG_SEVERITY_HIGH:         LOCUS_CORE_CRITICAL(message); return;
			case GL_DEBUG_SEVERITY_MEDIUM:       LOCUS_CORE_ERROR(message); return;
			case GL_DEBUG_SEVERITY_LOW:          LOCUS_CORE_WARN(message); return;
			case GL_DEBUG_SEVERITY_NOTIFICATION: LOCUS_CORE_TRACE(message); return;
		}

		LOCUS_CORE_ASSERT(false, "Unknown severity level!");
	}

	void OpenGLRendererAPI::Init()
	{
		LOCUS_PROFILE_FUNCTION();

		#ifdef LOCUS_DEBUG
			glEnable(GL_DEBUG_OUTPUT);
			glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
			glDebugMessageCallback(OpenGLMessageCallback, nullptr);
			glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DEBUG_SEVERITY_NOTIFICATION, 0, NULL, GL_FALSE);
		#endif

		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		glEnable(GL_DEPTH_TEST);
		glEnable(GL_LINE_SMOOTH);

		glEnable(GL_CULL_FACE);
	}

	void OpenGLRendererAPI::SetClearColor(const glm::vec4 color)
	{
		glClearColor(color.r, color.g, color.b, color.a);
	}

	void OpenGLRendererAPI::Clear()
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}

	void OpenGLRendererAPI::DrawIndexed(const Ref<VertexArray>& vertexArray, uint32_t indexCount)
	{
		vertexArray->Bind();
		uint32_t count = indexCount ? indexCount : vertexArray->GetIndexBuffer()->GetCount();
		glDrawElements(GL_TRIANGLES, count, GL_UNSIGNED_INT, nullptr);
	}

	void OpenGLRendererAPI::DrawArray(const Ref<VertexArray>& vertexArray, uint32_t vertexCount)
	{
		vertexArray->Bind();
		glDrawArrays(GL_TRIANGLES, 0, vertexCount);
	}

	void OpenGLRendererAPI::DrawIndexedInstanced(const Ref<VertexArray>& vertexArray, uint32_t indexCount, uint32_t instanceCount, uint32_t instanceBase)
	{
		vertexArray->Bind();
		glDrawElementsInstancedBaseInstance(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, nullptr, instanceCount, instanceBase);
	}

	void OpenGLRendererAPI::DrawArrayInstanced(const Ref<VertexArray>& vertexArray, uint32_t vertexCount, uint32_t instanceCount, uint32_t instanceBase)
	{
		vertexArray->Bind();
		glDrawArraysInstancedBaseInstance(GL_TRIANGLES, 0, vertexCount, instanceCount, instanceBase);
	}

	void OpenGLRendererAPI::DrawLine(const Ref<VertexArray>& vertexArray, uint32_t vertexCount)
	{
		vertexArray->Bind();
		glDrawArrays(GL_LINES, 0, vertexCount);
	}

	void OpenGLRendererAPI::Resize(int x, int y, int width, int height)
	{
		glViewport(x, y, width, height);
	}

	void OpenGLRendererAPI::SetLineWidth(float width)
	{
		glLineWidth(width);
	}
}
