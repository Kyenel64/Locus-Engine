#include <Tiel.h>

#include "ImGui/imgui.h"

class ExampleLayer : public Tiel::Layer
{
public:
	ExampleLayer() : Layer("Example") {}

	virtual void OnImGuiRender() override
	{
		ImGui::Begin("Test");
		ImGui::Text("Hello world");
		ImGui::End();
	}

	void OnEvent(Tiel::Event& event) override
	{
		if (event.GetEventType() == Tiel::EventType::KeyPressed)
		{
			Tiel::KeyPressedEvent e = (Tiel::KeyPressedEvent&)event;
			TIEL_TRACE("{0}", (char)e.GetKeyCode());
		}
	}
};


class Sandbox : public Tiel::Application
{
public:
	Sandbox()
	{
		PushLayer(new ExampleLayer());
	}

	~Sandbox()
	{

	}
};

Tiel::Application* Tiel::CreateApplication()
{
	return new Sandbox();
}