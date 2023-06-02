#pragma once
#include <string>

namespace Tiel
{
	class Shader
	{
	public:
		Shader(const std::string vertexSrc, const std::string fragmentSrc);
		~Shader();

		void Bind() const;
		void Unbind() const;

		inline unsigned int getID() const { return m_RendererID; }

	private:
		uint32_t m_RendererID;
	};
}