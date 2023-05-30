#include <Tiel.h>

class ExampleLayer : public Tiel::Layer
{
public:
	ExampleLayer() : Layer("Example") {}

	void OnUpdate() override
	{
		//TIEL_INFO("ExampleLayer::Update");
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
		PushOverlay(new Tiel::ImGuiLayer());
	}

	~Sandbox()
	{

	}
};

Tiel::Application* Tiel::CreateApplication()
{
	return new Sandbox();
}