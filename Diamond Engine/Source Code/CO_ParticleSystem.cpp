#include "CO_ParticleSystem.h"
#include "ImGui/imgui.h"
#include "Application.h"

C_ParticleSystem::C_ParticleSystem(GameObject* _gm) : Component(_gm), systemActive(true)
{
	name = "Particle System";
}

C_ParticleSystem::~C_ParticleSystem()
{
	systemActive = false;
}

#ifndef STANDALONE
bool C_ParticleSystem::OnEditor()
{

	if (Component::OnEditor() == false)
		return false;

	ImGui::Separator();

	//Put particle modules onEditor here

	return true;
}
#endif // !STANDALONE

void C_ParticleSystem::Update()
{
	float dt = EngineExternal->GetDT();
	for (int i = 0; i < myEmitters.size(); ++i)
	{
		myEmitters[i].Update(dt,systemActive);
	}
}

void C_ParticleSystem::Draw()
{
	for (int i = 0; i < myEmitters.size(); ++i)
	{
		myEmitters[i].Draw();
	}
}

void C_ParticleSystem::SetActive(bool newActive)
{
	systemActive = newActive;
}

void C_ParticleSystem::SaveData(JSON_Object* nObj)
{
	Component::SaveData(nObj);

}

void C_ParticleSystem::LoadData(DEConfig& nObj)
{
	Component::LoadData(nObj);

}
