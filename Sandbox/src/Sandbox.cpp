#include "tpch.h"
#include <Tiel.h>


class Sandbox : public Tiel::Application
{
public:
	Sandbox()
	{

	}

	~Sandbox()
	{

	}
};

Tiel::Application* Tiel::CreateApplication()
{
	return new Sandbox();
}