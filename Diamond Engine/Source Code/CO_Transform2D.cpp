#include "CO_Transform2D.h"

#include "GameObject.h"

#include "MathGeoLib/include/Math/float4x4.h"

C_Transform2D::C_Transform2D(GameObject* gameObject) : Component(gameObject),
	posX(0.0f),
	posY(0.0f),
	localPosX(0.0f),
	localPosY(0.0f),
	rotation(0.0f),
	localRotation(0.0f),
	sizeX(10.0f),
	sizeY(10.0f),
	updateTransform(false)
{
}


C_Transform2D::~C_Transform2D()
{

}


void C_Transform2D::Update()
{
	if (updateTransform == true)
		UpdateTransform();
}


#ifndef STANDALONE
bool C_Transform2D::OnEditor()
{
	//TODO Add modify pos logic
	return true;
}
#endif // !STANDALONE


float4x4 C_Transform2D::GetGlobal2DTransform(int winWidth, int winHeight)
{
	float positionX = posX * winWidth / 100.f;
	float positionY = posY * winHeight / 100.f;

	float scaleX = sizeX * winWidth / 100.f;
	float scaleY = sizeY * winHeight / 100.f;

	return float4x4::FromTRS(float3(positionX, positionY, 0), Quat::FromEulerXYZ(0, 0, rotation), float3(scaleX, scaleY, 0));
}


void C_Transform2D::UpdateTransform()
{
	updateTransform = false;

	if (gameObject->parent != nullptr)
	{
		Component* parentTrans = gameObject->parent->GetComponent(Component::TYPE::TRANSFORM_2D);
		if (parentTrans != nullptr)
		{
			C_Transform2D* parentTransform = (C_Transform2D*)parentTrans;
			posX = localPosX + parentTransform->posX;
			posY = localPosY + parentTransform->posY;
			rotation = localRotation + parentTransform->rotation;
		}

		else
		{
			posX = localPosX;
			posY = localPosY;
			rotation = localRotation;
		}
	}

	int childCount = gameObject->children.size();
	for (int i = 0; i < childCount; ++i)
	{
		Component* childComponent = gameObject->children[i]->GetComponent(Component::TYPE::TRANSFORM_2D);

		if (childComponent != nullptr)
		{
			C_Transform2D* childTransform = (C_Transform2D*)childComponent;
			childTransform->UpdateTransform();
		}
	}
}


void C_Transform2D::SetTransform(float locPosX, float locPosY, float locRotation)
{
	localPosX = locPosX;
	localPosY = locPosY;
	localRotation = locRotation;

	if (gameObject->parent != nullptr)
	{
		if (gameObject->parent->GetComponent(Component::TYPE::CANVAS) == nullptr)
		{
			Component* parentTransformComp = gameObject->parent->GetComponent(Component::TYPE::TRANSFORM_2D);
			if (parentTransformComp != nullptr)
			{
				C_Transform2D* parentTransform = (C_Transform2D*)parentTransformComp;

				posX = localPosX + parentTransform->posX;
				posY = localPosY + parentTransform->posY;
				rotation = localRotation + parentTransform->rotation;
			}
		}
	}
}