#ifndef STANDALONE

#ifndef __W_INSPECTOR_H__
#define __W_INSPECTOR_H__

#include "Window.h"
#include <vector>

class GameObject;
class W_Assets;
class Resource;

#define MAX_NAME_SIZE 50
class W_Inspector : public Window
{

public:
	W_Inspector();
	virtual ~W_Inspector();

	void Draw() override;

	void SetEditingResource(Resource* res);
	void ComponentDragAndDrop();

	GameObject* selectedGO;

	Resource* editingRes;

	char inputName[MAX_NAME_SIZE];

};

#endif //__W_INSPECTOR_H__

#endif // !STANDALONE