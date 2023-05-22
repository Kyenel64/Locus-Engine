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
		TIEL_TRACE("{0}", event);
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