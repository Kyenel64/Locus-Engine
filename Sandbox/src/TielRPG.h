#pragma once

#include "Tiel.h"

class TielRPG : public Tiel::Layer
{
public:
	TielRPG();
	virtual ~TielRPG() {}

	virtual void OnAttach() override;
	virtual void OnDetach() override;
	virtual void OnUpdate(Tiel::Timestep deltaTime) override;
	virtual void OnEvent(Tiel::Event& event) override;
	virtual void OnImGuiRender() override;

private:
	Tiel::OrthographicCameraController m_CameraController;

	Tiel::Ref<Tiel::Texture2D> m_CheckerboardTexture;
};