#include "Lpch.h"
#include "OpenGLUniformBuffer.h"

#include <glad/glad.h>

#include "Locus/Renderer/Renderer.h"

namespace Locus
{
	OpenGLUniformBuffer::OpenGLUniformBuffer(uint32_t size, uint32_t binding)
	{
		std::vector<int>& uboBindings = Renderer::GetUBOBindings();
		if (std::find(uboBindings.begin(), uboBindings.end(), binding) == uboBindings.end())
			uboBindings.push_back(binding);
		else
			LOCUS_CORE_ASSERT(false, "UBO binding occupied!");

		glCreateBuffers(1, &m_RendererID);
		glNamedBufferData(m_RendererID, size, nullptr, GL_DYNAMIC_DRAW);
		glBindBufferBase(GL_UNIFORM_BUFFER, binding, m_RendererID);
	}

	OpenGLUniformBuffer::~OpenGLUniformBuffer()
	{
		glDeleteBuffers(1, &m_RendererID);
	}

	void OpenGLUniformBuffer::SetData(const void* data, uint32_t size, uint32_t offset)
	{
		glNamedBufferSubData(m_RendererID, offset, size, data);
	}
}
