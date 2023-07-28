#pragma once
#include "SideA/Renderer/Framebuffer.h"

namespace SideA
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

		virtual uint32_t GetColorAttachmentRendererID(uint32_t index = 0) const override 
		{
			SIDEA_CORE_ASSERT(index < m_ColorAttachments.size(), "No Color Attachments!");
			return m_ColorAttachments[index];
		}
		virtual const FramebufferSpecification& GetSpecification() const { return m_Specification; }

	private:
		uint32_t m_RendererID = 0;
		FramebufferSpecification m_Specification;
		
		// Texture specifications
		std::vector<FramebufferTextureSpecification> m_ColorAttachmentSpecifications;
		FramebufferTextureSpecification m_DepthAttachmentSpecification = FramebufferTextureFormat::None;

		// IDs
		std::vector<uint32_t> m_ColorAttachments;
		uint32_t m_DepthAttachment = 0;
	};
}