#include "CO_ParticleSystem.h"
#include "Application.h"
#include "MO_Renderer3D.h"

#include "GameObject.h"
#include "CO_Billboard.h"
#include "CO_Material.h"

#include "RE_Material.h"
#include "RE_Shader.h"

#include "OpenGL.h"
#include "ImGui/imgui.h"

C_ParticleSystem::C_ParticleSystem(GameObject* _gm) : Component(_gm), systemActive(true), myEmitters()
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
	ImGui::Checkbox("SystemActive", &systemActive);

	ImGui::Spacing();
	std::string guiName = "";
	std::string suffixLabel = "";
	for (int i = 0; i < myEmitters.size(); ++i)
	{
		suffixLabel = "##";
		suffixLabel += i;
		ImGui::Separator();
		ImGui::Spacing();

		myEmitters[i].OnEditor(i);

		guiName = "Delete Emitter" + suffixLabel;
		if (ImGui::Button(guiName.c_str()))
		{
			myEmitters[i].toDelete = true;
		}
	}

	ImGui::Spacing();
	ImGui::Spacing();

	if (ImGui::Button("Add Emitter"))
	{
		myEmitters.push_back(Emitter());
	}
	return true;
}
#endif // !STANDALONE

void C_ParticleSystem::Update()
{
	float dt = EngineExternal->GetDT();
	//delete emitters
	for (int i = myEmitters.size() - 1; i >= 0; --i)
	{
		if (myEmitters[i].toDelete)
		{
			myEmitters.erase(myEmitters.begin() + i);
		}
	}

	for (int i = 0; i < myEmitters.size(); ++i)
	{
		myEmitters[i].Update(dt, systemActive);
	}

	EngineExternal->moduleRenderer3D->particleSystemQueue.push_back(gameObject);
}

void C_ParticleSystem::Draw()
{
	Component* mat = gameObject->GetComponent(Component::TYPE::MATERIAL);
	Component* bill = gameObject->GetComponent(Component::TYPE::BILLBOARD);

	if (mat != nullptr)
	{

		ResourceMaterial* material = static_cast<C_Material*>(mat)->material;
		material->shader->Bind();
		material->PushUniforms();

		for (int i = 0; i < myEmitters.size(); ++i)
		{
			myEmitters[i].Draw(material->shader->GetUID());

			//Draw instanced arrays
		}

		material->shader->Unbind();
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
