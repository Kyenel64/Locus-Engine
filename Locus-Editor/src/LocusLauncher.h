// --- LocusLauncher ----------------------------------------------------------
#pragma once

#include <Locus.h>

namespace Locus
{
	class LocusLauncher : public Layer
	{
	public:
		LocusLauncher();
		virtual ~LocusLauncher() = default;

		virtual void OnAttach() override;
		virtual void OnDetach() override;
		virtual void OnUpdate(Timestep deltaTime) override;
		virtual void OnEvent(Event& event) override;
		virtual void OnImGuiRender() override;

	private:
		void OpenProject(const std::string& projectPath, const std::string& projectName);
		void GenerateProject(const std::string& projectPath, const std::string& projectName);

	private:
		std::filesystem::path m_LocusEditorDirPath;
	};
}
