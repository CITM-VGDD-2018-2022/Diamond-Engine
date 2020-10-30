#pragma once
#include "Component.h"

class Mesh;

class C_MeshRenderer : public Component
{
public:
	C_MeshRenderer(GameObject* _gm);
	virtual ~C_MeshRenderer();

	void Update() override;

	void RenderMesh();

	void OnEditor() override;

//private:

	//Pointer to a mesh stored at ModuleRenderer3D
	//Does not need a delete call
	Mesh* _mesh;

	bool faceNormals, vertexNormals;

};