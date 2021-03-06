#include "CO_DirectionalLight.h"
#include "OpenGL.h"
#include "Application.h"
#include"MO_Renderer3D.h"

#include"GameObject.h"
#include"CO_Transform.h"
#include"CO_Camera.h"
#include"ImGui/imgui.h"
#include"MO_ResourceManager.h"
#include"RE_Shader.h"

#include"RE_Material.h"
#include"RE_Shader.h"
#include"MO_Window.h"

const unsigned int SHADOW_WIDTH = 2048, SHADOW_HEIGHT = 2048;
C_DirectionalLight::C_DirectionalLight(GameObject* _gm) : Component(_gm), 
	orthoSize(10.0f, 10.0f),
	calculateShadows(true),
	lightColor(float3::one), 
	ambientLightColor(float3::one),
	lightIntensity(1.0f),
	specularValue(64.0f)
{
	name = "Directional Light";

	orthoFrustum.type = FrustumType::OrthographicFrustum;
	orthoFrustum.nearPlaneDistance = 0.1f;
	orthoFrustum.farPlaneDistance = 500.f;
	orthoFrustum.front = gameObject->transform->GetForward();
	orthoFrustum.up = gameObject->transform->GetUp();
	orthoFrustum.pos = gameObject->transform->position;
	orthoFrustum.orthographicWidth = 100;
	orthoFrustum.orthographicHeight = 100;

	glGenFramebuffers(1, &depthMapFBO);

	glGenTextures(1, &depthMap);
	glBindTexture(GL_TEXTURE_2D, depthMap);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

	glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap, 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glBindTexture(GL_TEXTURE_2D, 0);

	depthShader = dynamic_cast<ResourceShader*>(EngineExternal->moduleResources->RequestResource(248150058, Resource::Type::SHADER));
	EngineExternal->moduleRenderer3D->AddDirectionalLight(this);
}


C_DirectionalLight::~C_DirectionalLight()
{
	if (depthMapFBO != 0)
		glDeleteFramebuffers(1, (GLuint*)&depthMapFBO);

	if (depthMap != 0)
		glDeleteTextures(1, (GLuint*)&depthMap);

	EngineExternal->moduleResources->UnloadResource(depthShader->GetUID());
	EngineExternal->moduleRenderer3D->RemoveDirectionalLight(this);
}


void C_DirectionalLight::Update()
{
	//Maybe dont update every frame?
	//if (gameObject->transform->updateTransform) 
	//{
	orthoFrustum.pos = gameObject->transform->globalTransform.TranslatePart();
	orthoFrustum.front = gameObject->transform->GetForward();
	orthoFrustum.up = gameObject->transform->GetUp();

	C_Camera::LookAt(orthoFrustum, float3::zero);
	spaceMatrixOpenGL = (orthoFrustum.ProjectionMatrix() * orthoFrustum.ViewMatrix()).Transposed();
	//}

#ifndef STANDALONE
	float3 points[8];
	orthoFrustum.GetCornerPoints(points);

	ModuleRenderer3D::DrawBox(points, float3(0.0f, 1.f, 0.0f));
#endif // !STANDALONE

}


#ifndef STANDALONE
bool C_DirectionalLight::OnEditor()
{
	if (Component::OnEditor() == true)
	{
		ImGui::ColorPicker3("Color", lightColor.ptr());

		ImGui::NewLine();

		ImGui::ColorPicker3("Ambient color", ambientLightColor.ptr());

		ImGui::NewLine();

		ImGui::DragFloat("Light intensity", &lightIntensity, 0.05, 0.0f);
		ImGui::DragFloat("Specular value", &specularValue, 0.1, 0.0f);

		ImGui::NewLine();

		if (ImGui::Button("Set light source", ImVec2(0, 50)))
			EngineExternal->moduleRenderer3D->AddDirectionalLight(this);

		ImGui::Image((ImTextureID)depthMap, ImVec2(250, 250), ImVec2(0, 1), ImVec2(1, 0));

		ImGui::Checkbox("Calculate shadows", &calculateShadows);

		if (ImGui::DragFloat2("Ortho size", orthoSize.ptr(), 0.001f))
		{
			orthoFrustum.orthographicWidth = SHADOW_WIDTH / orthoSize.x;
			orthoFrustum.orthographicHeight = SHADOW_HEIGHT / orthoSize.y;
		}

		return true;
	}
	return false;
}
#endif // !STANDALONE


void C_DirectionalLight::SaveData(JSON_Object* nObj)
{
	Component::SaveData(nObj);

	DEConfig data(nObj);
	data.WriteVector2("orthoSize", orthoSize.ptr());
	data.WriteVector3("lightColor", lightColor.ptr());
	data.WriteVector3("ambientLightColor", ambientLightColor.ptr());
	data.WriteFloat("lightIntensity", lightIntensity);
	data.WriteFloat("specularValue", specularValue);
	data.WriteBool("calculateShadows", calculateShadows);
}


void C_DirectionalLight::LoadData(DEConfig& nObj)
{
	Component::LoadData(nObj);

	orthoSize = nObj.ReadVector2("orthoSize");
	lightColor = nObj.ReadVector3("lightColor");
	ambientLightColor = nObj.ReadVector3("ambientLightColor");
	lightIntensity = nObj.ReadFloat("lightIntensity");
	specularValue = nObj.ReadFloat("specularValue");
	calculateShadows = nObj.ReadBool("calculateShadows");
}


void C_DirectionalLight::StartPass()
{

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);

	glDisable(GL_CULL_FACE);

	//glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);

	// 1. first render to depth map
	glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
	//glLoadIdentity();
	//glMatrixMode(GL_PROJECTION);
	//glLoadMatrixf((GLfloat*)orthoFrustum.ProjectionMatrix().Transposed().v);

	//math::float4x4 mat = orthoFrustum.ViewMatrix();
	//mat.Transposed();
	//glMatrixMode(GL_MODELVIEW);
	//glLoadMatrixf((GLfloat*)mat.v);

	glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
	//glClearColor(0.08f, 0.08f, 0.08f, 1.f);
	glClear(GL_DEPTH_BUFFER_BIT);

	depthShader->Bind();
}

void C_DirectionalLight::PushLightUniforms(ResourceMaterial* material, int lightNumber)
{
	char buffer[64];
	sprintf(buffer, "lightInfo[%i].lightSpaceMatrix", lightNumber);

	GLint modelLoc = glGetUniformLocation(material->shader->shaderProgramID, buffer);
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, this->spaceMatrixOpenGL.ptr());

	sprintf(buffer, "lightInfo[%i].lightPos", lightNumber);
	modelLoc = glGetUniformLocation(material->shader->shaderProgramID, buffer);
	glUniform3fv(modelLoc, 1, &gameObject->transform->position.x);

	sprintf(buffer, "lightInfo[%i].lightPosition", lightNumber);
	modelLoc = glGetUniformLocation(material->shader->shaderProgramID, buffer);
	glUniform3fv(modelLoc, 1, &gameObject->transform->position.x);

	sprintf(buffer, "lightInfo[%i].viewPos", lightNumber);
	modelLoc = glGetUniformLocation(material->shader->shaderProgramID, buffer);
	glUniform3fv(modelLoc, 1, EngineExternal->moduleRenderer3D->activeRenderCamera->GetPosition().ptr());

	sprintf(buffer, "lightInfo[%i].lightColor", lightNumber);
	modelLoc = glGetUniformLocation(material->shader->shaderProgramID, buffer);
	glUniform3fv(modelLoc, 1, &lightColor.x);

	sprintf(buffer, "lightInfo[%i].ambientLightColor", lightNumber);
	modelLoc = glGetUniformLocation(material->shader->shaderProgramID, buffer);
	glUniform3fv(modelLoc, 1, &ambientLightColor.x);

	sprintf(buffer, "lightInfo[%i].lightIntensity", lightNumber);
	modelLoc = glGetUniformLocation(material->shader->shaderProgramID, buffer);
	glUniform1f(modelLoc, lightIntensity);

	sprintf(buffer, "lightInfo[%i].specularValue", lightNumber);
	modelLoc = glGetUniformLocation(material->shader->shaderProgramID, buffer);
	glUniform1f(modelLoc, specularValue);

	//glUniform1i(glGetUniformLocation(material->shader->shaderProgramID, shadowMap), used_textures);
	sprintf(buffer, "lightInfo[%i].calculateShadows", lightNumber);
	modelLoc = glGetUniformLocation(material->shader->shaderProgramID, buffer);
	glUniform1i(modelLoc, calculateShadows);

	if (calculateShadows == true)
	{
		glActiveTexture(GL_TEXTURE5);
		modelLoc = glGetUniformLocation(material->shader->shaderProgramID, "shadowMap");
		glUniform1i(modelLoc, 5);
		glBindTexture(GL_TEXTURE_2D, depthMap);
	}
}

void C_DirectionalLight::EndPass()
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glActiveTexture(GL_TEXTURE5);
	glBindTexture(GL_TEXTURE_2D, 0);

	glActiveTexture(GL_TEXTURE0);

	//glBindTexture(GL_TEXTURE_2D, depthMap);

	depthShader->Unbind();

	glEnable(GL_CULL_FACE);
	glViewport(0, 0, EngineExternal->moduleWindow->s_width, EngineExternal->moduleWindow->s_height);
}


float3 C_DirectionalLight::GetPosition() const
{
	return orthoFrustum.pos;
}