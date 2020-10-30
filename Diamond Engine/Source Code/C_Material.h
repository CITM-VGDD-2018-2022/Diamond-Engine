#pragma once
#include "Component.h"
class Texture;

class C_Material : public Component
{
public:
	C_Material(GameObject* _gm);
	virtual ~C_Material();

	/*void Update() override;*/
	void OnEditor() override;
	int GetTextureID();

	Texture* matTexture;

	int textureID;

	int tWidth, tHeight;

	bool viewWithCheckers;

};