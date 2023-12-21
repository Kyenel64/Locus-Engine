// --- OpenGLFramebuffer ------------------------------------------------------
// OpenGL framebuffer class.
#pragma once

#include "Locus/Renderer/Framebuffer.h"

namespace Locus
{
	class OpenGLFramebuffer : public Framebuffer
	{
	public:
		OpenGLFramebuffer(const FramebufferSpecification& specs);
		virtual ~OpenGLFramebuffer();

		void Refresh();

		virtual void Bind() override;
		virtual void Unbind() override;

		virtual void Resize(uint32_t width, uint32_t height) override;
		virtual int ReadPixel(uint32_t attachmentIndex, int x, int y) override;

		virtual void ClearAttachmentInt(uint32_t attachmentIndex, int value) override;
		virtual void ClearAttachmentColor(uint32_t attachmentIndex, const glm::vec4& value) override;

		virtual inline uint32_t GetColorAttachmentRendererID(uint32_t index = 0) const override 
		{
			LOCUS_CORE_ASSERT(index < m_ColorAttachments.size(), "No Color Attachments!");
			return m_ColorAttachments[index];
		}
		virtual inline const FramebufferSpecification& GetSpecification() const { return m_Specification; }

	private:
		uint32_t m_RendererID = 0;
		FramebufferSpecification m_Specification;
		
		std::vector<FramebufferTextureSpecification> m_ColorAttachmentSpecifications;
		FramebufferTextureSpecification m_DepthAttachmentSpecification = FramebufferTextureFormat::None;

		std::vector<uint32_t> m_ColorAttachments;
		uint32_t m_DepthAttachment = 0;
	};
}
