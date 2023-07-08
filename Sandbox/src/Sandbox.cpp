#include <SideA.h>
// --- Entry Point ------------------------------------------------------------
#include <SideA/Core/EntryPoint.h>

#include "Sandbox2D.h"
#include "ExampleLayer.h"


class Sandbox : public SideA::Application
{
public:
	Sandbox()
	{
		//PushLayer(new ExampleLayer());
		PushLayer(new Sandbox2D());
	}

	~Sandbox() {}
};

SideA::Application* SideA::CreateApplication()
{
	return new Sandbox();
}