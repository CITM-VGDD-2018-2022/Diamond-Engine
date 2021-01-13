#include "CO_Camera.h"
#include"Globals.h"
#include"DEJsonSupport.h"
#include"ImGui/imgui.h"

#include"MathGeoLib/include/Math/float3.h"
#include"MathGeoLib/include/Geometry/Plane.h"

#include"MO_Renderer3D.h"
#include"MO_Scene.h"

#include"GameObject.h"
#include"CO_Transform.h"
#include"OpenGL.h"
#include"MO_Window.h"

C_Camera::C_Camera() : Component(nullptr), framebuffer(0), texColorBuffer(0), rbo(0), fov(60.0f), cullingState(true), MSAA(false)
, texBufferSize(float2::zero)
{
	name = "Camera";
	camFrustrum.type = FrustumType::PerspectiveFrustum;
	camFrustrum.nearPlaneDistance = 0.1f;
	camFrustrum.farPlaneDistance = 500.f;
	camFrustrum.front = float3::unitZ;
	camFrustrum.up = float3::unitY;

	camFrustrum.verticalFov = 60.0f * DEGTORAD;
	camFrustrum.horizontalFov = 2.0f * atanf(tanf(camFrustrum.verticalFov / 2.0f) * 1.7f);

	camFrustrum.pos = float3::zero;
}

C_Camera::C_Camera(GameObject* _gm) : Component(_gm), framebuffer(0), texColorBuffer(0), rbo(0), fov(60.0f), cullingState(true),
texBufferSize(float2::zero), MSAA(false)
{
#ifdef STANDALONE
	MSAA = true;
#endif // !STANDALONE


	name = "Camera";
	camFrustrum.type = FrustumType::PerspectiveFrustum;
	camFrustrum.nearPlaneDistance = 1;
	camFrustrum.farPlaneDistance = 100.f;
	camFrustrum.front = gameObject->transform->GetForward();
	camFrustrum.up = gameObject->transform->GetUp();

	camFrustrum.verticalFov = 60.0f * DEGTORAD;
	camFrustrum.horizontalFov = 2.0f * atanf(tanf(camFrustrum.verticalFov / 2.0f) * 1.7f);
	
	camFrustrum.pos = gameObject->transform->position;
}

C_Camera::~C_Camera()
{
	if (framebuffer != 0)
		glDeleteFramebuffers(1, (GLuint*)&framebuffer);

	if (texColorBuffer != 0)
		glDeleteTextures(1, (GLuint*)&texColorBuffer);

	if (rbo != 0)
		glDeleteRenderbuffers(1, (GLuint*)&rbo);

	if (EngineExternal && EngineExternal->moduleRenderer3D->GetGameRenderTarget() == this)
		EngineExternal->moduleRenderer3D->SetGameRenderTarget(nullptr);
}

#ifndef STANDALONE
bool C_Camera::OnEditor()
{
	if (Component::OnEditor() == true)
	{
		ImGui::Separator();

		ImGui::Text("FB %i, TB %i, RBO %i", framebuffer, texColorBuffer, rbo);

		ImGui::DragFloat("Near Distance: ", &camFrustrum.nearPlaneDistance, 0.1f, 0.01f, camFrustrum.farPlaneDistance);
		ImGui::DragFloat("Far Distance: ", &camFrustrum.farPlaneDistance, 0.1f, camFrustrum.nearPlaneDistance, 10000.f);

		ImGui::Separator();
		ImGui::Text("Vertical FOV: %f", camFrustrum.verticalFov);
		ImGui::Text("Horizontal FOV: %f", camFrustrum.horizontalFov);
		ImGui::Separator();

		if (ImGui::DragFloat("FOV: ", &fov, 0.1f, 1.0f, 180.f))
		{
			camFrustrum.verticalFov = fov * DEGTORAD;
			//camFrustrum.horizontalFov = 2.0f * atanf(tanf(camFrustrum.verticalFov / 2.0f) * App->window->GetWindowWidth() / App->window->GetWindowHeight());
		}
		
		if (ImGui::BeginCombo("Frustrum Type", (camFrustrum.type == FrustumType::PerspectiveFrustum) ? "Prespective" : "Orthographic")) 
		{
			if (ImGui::Selectable("Perspective")) 
				camFrustrum.type = FrustumType::PerspectiveFrustum;

			if (ImGui::Selectable("Orthographic"))
				camFrustrum.type = FrustumType::OrthographicFrustum;

			ImGui::EndCombo();
		}

		ImGui::Text("Camera Culling: "); ImGui::SameLine();
		ImGui::Checkbox("##cameraCulling", &cullingState);

		if(ImGui::Button("Set as Game Camera")) 
		{
			EngineExternal->moduleRenderer3D->SetGameRenderTarget(this);
		}

		return true;
	}
	return false;
}
#endif // !STANDALONE

void C_Camera::Update()
{

	//Maybe dont update every frame?
	camFrustrum.pos = gameObject->transform->globalTransform.TranslatePart();
	camFrustrum.front = gameObject->transform->GetForward();
	camFrustrum.up = gameObject->transform->GetUp();

#ifndef STANDALONE
	float3 points[8];
	camFrustrum.GetCornerPoints(points);

	ModuleRenderer3D::DrawBox(points, float3(0.180f, 1.f, 0.937f));
#endif // !STANDALONE

}


void C_Camera::SaveData(JSON_Object* nObj)
{
	Component::SaveData(nObj);

	DEJson::WriteInt(nObj, "fType", (int)FrustumType::PerspectiveFrustum);

	DEJson::WriteFloat(nObj, "nearPlaneDist", camFrustrum.nearPlaneDistance);
	DEJson::WriteFloat(nObj, "farPlaneDist", camFrustrum.farPlaneDistance);

	DEJson::WriteFloat(nObj, "vFOV", camFrustrum.verticalFov);
	DEJson::WriteFloat(nObj, "hFOV", camFrustrum.horizontalFov);
}

void C_Camera::LoadData(DEConfig& nObj)
{
	Component::LoadData(nObj);

	camFrustrum.type = (FrustumType)nObj.ReadInt("fType");

	camFrustrum.nearPlaneDistance = nObj.ReadFloat("nearPlaneDist");
	camFrustrum.farPlaneDistance = nObj.ReadFloat("farPlaneDist");

	camFrustrum.verticalFov = nObj.ReadFloat("vFOV");
	camFrustrum.horizontalFov = nObj.ReadFloat("hFOV");

	EngineExternal->moduleScene->SetGameCamera(this);
}

void C_Camera::StartDraw()
{
	glEnable(GL_DEPTH_TEST);
	glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);

	glLoadIdentity();
	glMatrixMode(GL_PROJECTION);
	glLoadMatrixf((GLfloat*)ProjectionMatrixOpenGL().v);

	glMatrixMode(GL_MODELVIEW);
	glLoadMatrixf((GLfloat*)ViewMatrixOpenGL().v);

	glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);

	glClearColor(0.08f, 0.08f, 0.08f, 1.f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//glEnable(GL_DEPTH_TEST);

	//glLoadIdentity();

	//glMatrixMode(GL_MODELVIEW);
	//glLoadMatrixf(camFrustrum.ViewMatrix().ptr());
}

void C_Camera::EndDraw()
{
	//Is this important?

	glBindTexture(GL_TEXTURE_2D, texColorBuffer);
	glGenerateMipmap(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, 0);


	/*TODO: IMPORTANT This is the MSAA resolving to screen, to resolve a MSAA FBO to a Normal FBO we need to do the same but
	add the normal fbo as the GL_DRAW_FRAMEBUFFER*/
#ifdef STANDALONE 
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
	glBindFramebuffer(GL_READ_FRAMEBUFFER, framebuffer);
	glBlitFramebuffer(0, 0, texBufferSize.x, texBufferSize.y, 0, 0, texBufferSize.x, texBufferSize.y, GL_COLOR_BUFFER_BIT, GL_NEAREST);

	glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);
#endif // !STANDALONE


	glBindFramebuffer(GL_FRAMEBUFFER, 0);


	glDisable(GL_DEPTH_TEST);
}

void C_Camera::ReGenerateBuffer(int w, int h)
{
	SetAspectRatio((float)w / (float)h);
	texBufferSize = float2(w, h);

	if (framebuffer != 0)
		glDeleteFramebuffers(1, (GLuint*)&framebuffer);

	if (texColorBuffer != 0)
		glDeleteTextures(1, (GLuint*)&texColorBuffer);

	if (rbo != 0)
		glDeleteRenderbuffers(1, (GLuint*)&rbo);


	glGenFramebuffers(1, (GLuint*)&framebuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);

	glGenTextures(1, (GLuint*)&texColorBuffer);

	auto textureTypr = (MSAA) ? GL_TEXTURE_2D_MULTISAMPLE : GL_TEXTURE_2D;
	glBindTexture(textureTypr, texColorBuffer);
	(MSAA) ? glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, 4, GL_RGB, w, h, GL_TRUE) : glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, w, h, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(textureTypr, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(textureTypr, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, 0);

	// attach it to currently bound framebuffer object
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, textureTypr, texColorBuffer, 0);



	glGenRenderbuffers(1, (GLuint*)&rbo);
	glBindRenderbuffer(GL_RENDERBUFFER, rbo);
	if(MSAA)
		glRenderbufferStorageMultisample(GL_RENDERBUFFER, 4, GL_DEPTH24_STENCIL8, w, h);
	else
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, w, h);

	glBindRenderbuffer(GL_RENDERBUFFER, 0);

	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		LOG(LogType::L_ERROR, "ERROR::FRAMEBUFFER:: Framebuffer is not complete!");
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void C_Camera::LookAt(const float3& Spot)
{
	/*Reference = Spot;*/
	camFrustrum.front = (Spot - camFrustrum.pos).Normalized();
	float3 X = float3(0, 1, 0).Cross(camFrustrum.front).Normalized();
	camFrustrum.up = camFrustrum.front.Cross(X);
}

void C_Camera::Move(const float3& Movement)
{
	camFrustrum.pos += Movement;
}

float4x4 C_Camera::ViewMatrixOpenGL() const
{
	math::float4x4 mat = camFrustrum.ViewMatrix();
	return mat.Transposed();
}

float4x4 C_Camera::ProjectionMatrixOpenGL() const
{
	return camFrustrum.ProjectionMatrix().Transposed();
}

void C_Camera::SetAspectRatio(float aspectRatio)
{
	camFrustrum.horizontalFov = 2.f * atanf(tanf(camFrustrum.verticalFov * 0.5f) * aspectRatio);
}