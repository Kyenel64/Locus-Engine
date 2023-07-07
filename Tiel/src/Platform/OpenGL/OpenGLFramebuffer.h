#pragma once
#include "Tiel/Renderer/Framebuffer.h"

namespace Tiel
{
	class OpenGLFramebuffer : public Framebuffer
	{
	public:
		OpenGLFramebuffer(const FramebufferSpecs& specs);
		virtual ~OpenGLFramebuffer();

		void Refresh();

		virtual void Bind() override;
		virtual void Unbind() override;

		virtual void Resize(uint32_t width, uint32_t height) override;

		virtual uint32_t GetColorAttachmentRendererID() const override { return m_ColorAttachment; }
		virtual const FramebufferSpecs& GetSpecification() const { return m_Specifications; }

	private:
		uint32_t m_RendererID = 0;
		uint32_t m_ColorAttachment, m_DepthAttachment;
		FramebufferSpecs m_Specifications;
	};
}