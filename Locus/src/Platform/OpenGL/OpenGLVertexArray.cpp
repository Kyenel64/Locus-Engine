#include "Lpch.h"
#include "OpenGLVertexArray.h"

#include <glad/glad.h>

namespace Locus
{
	static GLenum ShaderDataTypeToOpenGLBaseType(ShaderDataType type)
	{
		switch (type)
		{
			case ShaderDataType::Float:    return GL_FLOAT;
			case ShaderDataType::Float2:   return GL_FLOAT;
			case ShaderDataType::Float3:   return GL_FLOAT;
			case ShaderDataType::Float4:   return GL_FLOAT;
			case ShaderDataType::Mat3:     return GL_FLOAT;
			case ShaderDataType::Mat4:     return GL_FLOAT;
			case ShaderDataType::Int:      return GL_INT;
			case ShaderDataType::Int2:     return GL_INT;
			case ShaderDataType::Int3:     return GL_INT;
			case ShaderDataType::Int4:     return GL_INT;
			case ShaderDataType::Bool:     return GL_BOOL;
		}

		LOCUS_CORE_ASSERT(false, "Unknown ShaderDataType");
		return 0;
	}

	OpenGLVertexArray::OpenGLVertexArray()
	{
		LOCUS_PROFILE_FUNCTION();

		glGenVertexArrays(1, &m_RendererID);
	}

	OpenGLVertexArray::~OpenGLVertexArray()
	{
		LOCUS_PROFILE_FUNCTION();

		glDeleteVertexArrays(1, &m_RendererID);
	}

	void OpenGLVertexArray::Bind() const
	{
		LOCUS_PROFILE_FUNCTION();

		glBindVertexArray(m_RendererID);
	}

	void OpenGLVertexArray::Unbind() const
	{
		LOCUS_PROFILE_FUNCTION();

		glBindVertexArray(0);
	}

	void OpenGLVertexArray::AddVertexBuffer(const Ref<VertexBuffer>& vertexBuffer)
	{
		LOCUS_PROFILE_FUNCTION();

		LOCUS_CORE_ASSERT(vertexBuffer->GetLayout().GetElements().size(), "VertexBuffer has no layout!");

		glBindVertexArray(m_RendererID);
		vertexBuffer->Bind();

		const auto& layout = vertexBuffer->GetLayout();

		for (const auto& element : layout)
		{
			switch (element.Type)
			{
				case ShaderDataType::Int:
				case ShaderDataType::Int2:
				case ShaderDataType::Int3:
				case ShaderDataType::Int4:
				case ShaderDataType::Bool:
				{
					glEnableVertexAttribArray(m_VertexBufferIndex);

					glVertexAttribIPointer(m_VertexBufferIndex, element.GetComponentCount(), ShaderDataTypeToOpenGLBaseType(element.Type), 
						layout.GetStride(), (const void*)(size_t)element.Offset);
					if (element.Instanced)
						glVertexAttribDivisor(m_VertexBufferIndex, element.Instanced);
					m_VertexBufferIndex++;
					break;
				}
				case ShaderDataType::Float:
				case ShaderDataType::Float2:
				case ShaderDataType::Float3:
				case ShaderDataType::Float4:
				{
					glEnableVertexAttribArray(m_VertexBufferIndex);

					glVertexAttribPointer(m_VertexBufferIndex, element.GetComponentCount(), ShaderDataTypeToOpenGLBaseType(element.Type),
						element.Normalized ? GL_TRUE : GL_FALSE, layout.GetStride(), (const void*)(size_t)element.Offset);
					if (element.Instanced)
						glVertexAttribDivisor(m_VertexBufferIndex, element.Instanced);
					m_VertexBufferIndex++;
					break;
				}
				case ShaderDataType::Mat4:
				{
					glEnableVertexAttribArray(m_VertexBufferIndex);
					glEnableVertexAttribArray(m_VertexBufferIndex + 1);
					glEnableVertexAttribArray(m_VertexBufferIndex + 2);
					glEnableVertexAttribArray(m_VertexBufferIndex + 3);
					glVertexAttribPointer(m_VertexBufferIndex + 0, 4, ShaderDataTypeToOpenGLBaseType(element.Type),
						element.Normalized ? GL_TRUE : GL_FALSE, layout.GetStride(), (const void*)(size_t)element.Offset);
					glVertexAttribPointer(m_VertexBufferIndex + 1, 4, ShaderDataTypeToOpenGLBaseType(element.Type),
						element.Normalized ? GL_TRUE : GL_FALSE, layout.GetStride(), (const void*)((size_t)element.Offset + (sizeof(float) * 4)));
					glVertexAttribPointer(m_VertexBufferIndex + 2, 4, ShaderDataTypeToOpenGLBaseType(element.Type),
						element.Normalized ? GL_TRUE : GL_FALSE, layout.GetStride(), (const void*)((size_t)element.Offset + (sizeof(float) * 8)));
					glVertexAttribPointer(m_VertexBufferIndex + 3, 4, ShaderDataTypeToOpenGLBaseType(element.Type),
						element.Normalized ? GL_TRUE : GL_FALSE, layout.GetStride(), (const void*)((size_t)element.Offset + (sizeof(float) * 12)));

					if (element.Instanced)
					{
						glVertexAttribDivisor(m_VertexBufferIndex, element.Instanced);
						glVertexAttribDivisor(m_VertexBufferIndex + 1, element.Instanced);
						glVertexAttribDivisor(m_VertexBufferIndex + 2, element.Instanced);
						glVertexAttribDivisor(m_VertexBufferIndex + 3, element.Instanced);
					}
					
					
					m_VertexBufferIndex += 4;
					break;
				}
				default: LOCUS_CORE_ASSERT(false, "Unknown ShaderDataType!");
			}
		}
		m_VertexBuffers.push_back(vertexBuffer);
		glBindVertexArray(0);
	}

	void OpenGLVertexArray::SetIndexBuffer(const Ref<IndexBuffer>& indexBuffer)
	{
		LOCUS_PROFILE_FUNCTION();

		glBindVertexArray(m_RendererID);
		indexBuffer->Bind();

		m_IndexBuffer = indexBuffer;
		glBindVertexArray(0);
	}
}
