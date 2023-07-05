#include <Tiel.h>
// --- Entry Point ------------------------------------------------------------
#include <Tiel/Core/EntryPoint.h>

#include "Sandbox2D.h"
#include "ExampleLayer.h"
#include "TielRPG.h"


class Sandbox : public Tiel::Application
{
public:
	Sandbox()
	{
		//PushLayer(new ExampleLayer());
		PushLayer(new Sandbox2D());
		//PushLayer(new TielRPG());
	}

	~Sandbox() {}
};

Tiel::Application* Tiel::CreateApplication()
{
	return new Sandbox();
}