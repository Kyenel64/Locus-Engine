#include "Lpch.h"
#include "OpenGLFramebuffer.h"

#include <glad/glad.h>

namespace Locus
{
	static const uint32_t s_MaxFramebufferSize = 8192;

	OpenGLFramebuffer::OpenGLFramebuffer(const FramebufferSpecification& specs)
		: m_Specification(specs)
	{
		for (auto& spec : m_Specification.Attachments.Attachments)
		{
			if (spec.TextureFormat != FramebufferTextureFormat::DEPTH24STENCIL8)
				m_ColorAttachmentSpecifications.emplace_back(spec);
			else
				m_DepthAttachmentSpecification = spec;
		}

		Refresh();
	}

	OpenGLFramebuffer::~OpenGLFramebuffer()
	{
		glDeleteFramebuffers(1, &m_RendererID);
		glDeleteTextures((GLsizei)m_ColorAttachments.size(), m_ColorAttachments.data());
		glDeleteTextures(1, &m_DepthAttachment);
	}

	void OpenGLFramebuffer::Refresh()
	{
		if (m_RendererID)
		{
			glDeleteFramebuffers(1, &m_RendererID);
			glDeleteTextures((GLsizei)m_ColorAttachments.size(), m_ColorAttachments.data());
			glDeleteTextures(1, &m_DepthAttachment);

			m_ColorAttachments.clear();
			m_DepthAttachment = 0;
		}

		glCreateFramebuffers(1, &m_RendererID);
		glBindFramebuffer(GL_FRAMEBUFFER, m_RendererID);

		GLenum textureTarget = m_Specification.Samples > 1 ? GL_TEXTURE_2D_MULTISAMPLE : GL_TEXTURE_2D;

		// Color attachment
		if (m_ColorAttachmentSpecifications.size())
		{
			m_ColorAttachments.resize(m_ColorAttachmentSpecifications.size());
			glCreateTextures(textureTarget, (GLsizei)m_ColorAttachments.size(), m_ColorAttachments.data());

			for (size_t i = 0; i < m_ColorAttachments.size(); i++)
			{
				glBindTexture(textureTarget, m_ColorAttachments[i]);

				switch (m_ColorAttachmentSpecifications[i].TextureFormat)
				{
					case FramebufferTextureFormat::RGBA8:
						if (textureTarget == GL_TEXTURE_2D_MULTISAMPLE)
							glTexImage2DMultisample(textureTarget, m_Specification.Samples, GL_RGBA8, m_Specification.Width, m_Specification.Height, GL_FALSE);
						else
							glTexImage2D(textureTarget, 0, GL_RGBA8, m_Specification.Width, m_Specification.Height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
						break;

					case FramebufferTextureFormat::RED_INT:
						glTexImage2D(textureTarget, 0, GL_R32I, m_Specification.Width, m_Specification.Height, 0, GL_RED_INTEGER, GL_UNSIGNED_BYTE, nullptr);
						break; 
				}

				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

				glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + (GLenum)i, textureTarget, m_ColorAttachments[i], 0);
			}
		}

		// Depth attachment
		if (m_DepthAttachmentSpecification.TextureFormat != FramebufferTextureFormat::None)
		{
			glCreateTextures(textureTarget, 1, &m_DepthAttachment);
			glBindTexture(textureTarget, m_DepthAttachment);

			switch (m_DepthAttachmentSpecification.TextureFormat)
			{
			case FramebufferTextureFormat::DEPTH24STENCIL8:
				if (textureTarget == GL_TEXTURE_2D_MULTISAMPLE)
					glTexImage2DMultisample(textureTarget, m_Specification.Samples, GL_DEPTH24_STENCIL8, m_Specification.Width, m_Specification.Height, GL_FALSE);
				else
					glTexStorage2D(textureTarget, 1, GL_DEPTH24_STENCIL8, m_Specification.Width, m_Specification.Height);
				break;
			}

			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, textureTarget, m_DepthAttachment, 0);
		}

		if (m_ColorAttachments.size() > 1)
		{
			LOCUS_CORE_ASSERT(m_ColorAttachments.size() <= 4, "Color Attachment failed");
			GLenum buffers[4] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3 };
			glDrawBuffers((GLsizei)m_ColorAttachments.size(), buffers);
		}
		else if (m_ColorAttachments.empty())
		{
			glDrawBuffer(GL_NONE);
		}

		LOCUS_CORE_ASSERT(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE, "Framebuffer is incomplete!");

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	void OpenGLFramebuffer::Resize(uint32_t width, uint32_t height)
	{
		if (width == 0 || height == 0 || width > s_MaxFramebufferSize || height > s_MaxFramebufferSize)
		{
			LOCUS_CORE_WARN("Attempted to rezize framebuffer to {0}, {1}", width, height);
			return;
		}

		m_Specification.Width = width;
		m_Specification.Height = height;

		Refresh();
	}

	int OpenGLFramebuffer::ReadPixel(uint32_t attachmentIndex, int x, int y)
	{
		LOCUS_CORE_ASSERT(attachmentIndex < m_ColorAttachments.size(), "Attachment Index out of bounds!");
		glBindFramebuffer(GL_FRAMEBUFFER, m_RendererID);
		glReadBuffer(GL_COLOR_ATTACHMENT0 + attachmentIndex);
		int pixelData;
		glReadPixels(x, y, 1, 1, GL_RED_INTEGER, GL_INT, &pixelData);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		return pixelData;
	}

	void OpenGLFramebuffer::BindTexture(uint32_t attachmentIndex)
	{
		glBindTextureUnit(0, m_ColorAttachments[attachmentIndex]);
	}

	void OpenGLFramebuffer::ClearAttachmentInt(uint32_t attachmentIndex, int value)
	{
		LOCUS_CORE_ASSERT(attachmentIndex < m_ColorAttachments.size(), "Attachment index out of bounds!");

		glClearTexImage(m_ColorAttachments[attachmentIndex], 0, GL_RED_INTEGER, GL_INT, &value);
	}

	void OpenGLFramebuffer::ClearAttachmentColor(uint32_t attachmentIndex, const glm::vec4& value)
	{
		LOCUS_CORE_ASSERT(attachmentIndex < m_ColorAttachments.size(), "Attachment index out of bounds!");

		glClearTexImage(m_ColorAttachments[attachmentIndex], 0, GL_RGBA, GL_FLOAT , &value);
	}

	void OpenGLFramebuffer::Bind()
	{
		glBindFramebuffer(GL_FRAMEBUFFER, m_RendererID);
		glViewport(0, 0, m_Specification.Width, m_Specification.Height);
	}

	void OpenGLFramebuffer::Unbind()
	{
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}
}
