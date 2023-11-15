#pragma once

#include "Locus.h"

class Sandbox2D : public Locus::Layer
{
public:
	Sandbox2D();
	virtual ~Sandbox2D() {}

	virtual void OnAttach() override;
	virtual void OnDetach() override;
	virtual void OnUpdate(Locus::Timestep deltaTime) override;
	virtual void OnEvent(Locus::Event& event) override;
	virtual void OnImGuiRender() override;

};
