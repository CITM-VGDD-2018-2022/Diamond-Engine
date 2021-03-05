#include "CO_Navigation.h"

#include "Application.h"
#include "MO_GUI.h"
#include "MO_Scene.h"
#include "MO_Input.h"

#include "GameObject.h"
#include "CO_Button.h"
#include "CO_Checkbox.h"

#include "ImGui/imgui.h"

#include <assert.h>

C_Navigation::C_Navigation(GameObject* gameObject, Component::TYPE type_of_ui) :Component(gameObject), is_selected(false),button_or_joystick_being_used(BUTTONSANDJOYSTICKS::NO_BUTTON_OR_JOYSTICK),
type_of_ui(type_of_ui)
{
	map_of_buttons_and_joysticks.clear();
	name = "Navigation";
}

C_Navigation::~C_Navigation()
{
	map_of_buttons_and_joysticks.clear();
}

void C_Navigation::Update()
{
	if (!is_selected || map_of_buttons_and_joysticks.size() == 0)
		return;
	KEY_STATE state;
	for (std::map<BUTTONSANDJOYSTICKS, ActionToRealize>::iterator it = map_of_buttons_and_joysticks.begin(); it != map_of_buttons_and_joysticks.end(); ++it) {
		if (it->second.uid_gameobject == 0)
			continue;
		CheckIfButtonOrJoystickIsBeingUsed(it->first,state);

		if (button_or_joystick_being_used == it->first && state == KEY_STATE::KEY_UP) {
			GameObject* gameobject = EngineExternal->moduleScene->GetGOFromUID(EngineExternal->moduleScene->root, it->second.uid_gameobject);
			DoTheAction(gameObject, it->first, it->second.action, true);
			button_or_joystick_being_used = BUTTONSANDJOYSTICKS::NO_BUTTON_OR_JOYSTICK;
			return;
		}
		else if (button_or_joystick_being_used == BUTTONSANDJOYSTICKS::NO_BUTTON_OR_JOYSTICK && state == KEY_STATE::KEY_DOWN) {
			GameObject* gameobject = EngineExternal->moduleScene->GetGOFromUID(EngineExternal->moduleScene->root, it->second.uid_gameobject);
			DoTheAction(gameObject, it->first, it->second.action, false);
			button_or_joystick_being_used = it->first;
			return;
		}

		it->second.is_key_down = false;
		it->second.is_key_up = false;
		if (state == KEY_STATE::KEY_DOWN || state == KEY_STATE::KEY_REPEAT) {
			it->second.is_key_down = true;
		}
		if (state == KEY_STATE::KEY_UP || state == KEY_STATE::KEY_IDLE) {
			it->second.is_key_up = true;
		}

	}
}

void C_Navigation::CheckIfButtonOrJoystickIsBeingUsed(BUTTONSANDJOYSTICKS button_or_joystick_to_check, KEY_STATE& state)
{
	switch (button_or_joystick_to_check)
	{
	case BUTTONSANDJOYSTICKS::BUTTON_A:
		state = EngineExternal->moduleInput->GetKey(SDL_CONTROLLER_BUTTON_A);
		break;
	case BUTTONSANDJOYSTICKS::BUTTON_B:
		state = EngineExternal->moduleInput->GetKey(SDL_CONTROLLER_BUTTON_B);
		break;
	case BUTTONSANDJOYSTICKS::BUTTON_X:
		state = EngineExternal->moduleInput->GetKey(SDL_CONTROLLER_BUTTON_X);
		break;
	case BUTTONSANDJOYSTICKS::BUTTON_Y:
		state = EngineExternal->moduleInput->GetKey(SDL_CONTROLLER_BUTTON_Y);
		break;
	case BUTTONSANDJOYSTICKS::BUTTON_BACK:
		state = EngineExternal->moduleInput->GetKey(SDL_CONTROLLER_BUTTON_BACK);
		break;
	case BUTTONSANDJOYSTICKS::BUTTON_GUIDE:
		state = EngineExternal->moduleInput->GetKey(SDL_CONTROLLER_BUTTON_GUIDE);
		break;
	case BUTTONSANDJOYSTICKS::BUTTON_START:
		state = EngineExternal->moduleInput->GetKey(SDL_CONTROLLER_BUTTON_START);
		break;
	case BUTTONSANDJOYSTICKS::BUTTON_LEFTTTRIGGER:
		state = EngineExternal->moduleInput->GetKey(SDL_CONTROLLER_BUTTON_LEFTSTICK);
		break;
	case BUTTONSANDJOYSTICKS::BUTTON_RIGHTTRIGGER:
		state = EngineExternal->moduleInput->GetKey(SDL_CONTROLLER_BUTTON_RIGHTSTICK);
		break;
	case BUTTONSANDJOYSTICKS::BUTTON_LEFTSHOULDER:
		state = EngineExternal->moduleInput->GetKey(SDL_CONTROLLER_BUTTON_LEFTSHOULDER);
		break;
	case BUTTONSANDJOYSTICKS::BUTTON_RIGHTSHOULDER:
		state = EngineExternal->moduleInput->GetKey(SDL_CONTROLLER_BUTTON_RIGHTSHOULDER);
		break;
	case BUTTONSANDJOYSTICKS::BUTTON_DPAD_UP:
		state = EngineExternal->moduleInput->GetKey(SDL_CONTROLLER_BUTTON_DPAD_UP);
		break;
	case BUTTONSANDJOYSTICKS::BUTTON_DPAD_DOWN:
		state = EngineExternal->moduleInput->GetKey(SDL_CONTROLLER_BUTTON_DPAD_DOWN);
		break;
	case BUTTONSANDJOYSTICKS::BUTTON_DPAD_LEFT:
		state = EngineExternal->moduleInput->GetKey(SDL_CONTROLLER_BUTTON_DPAD_LEFT);
		break;
	case BUTTONSANDJOYSTICKS::BUTTON_DPAD_RIGHT:
		state = EngineExternal->moduleInput->GetKey(SDL_CONTROLLER_BUTTON_DPAD_RIGHT);
		break;
	case BUTTONSANDJOYSTICKS::RIGHT_JOYSTICK_UP: {
		int value_of_axis = EngineExternal->moduleInput->GetRightAxisY();
		if (value_of_axis >= 0) {
			if (map_of_buttons_and_joysticks[button_or_joystick_to_check].is_key_down) {
				state = KEY_UP;
			}
			else {
				state = KEY_IDLE;
			}
		}
		else {
			if (map_of_buttons_and_joysticks[button_or_joystick_to_check].is_key_up) {
				state = KEY_DOWN;
			}
			else {
				state = KEY_REPEAT;
			}
		}
		break;
	}
	case BUTTONSANDJOYSTICKS::RIGHT_JOYSTICK_DOWN: {
		int value_of_axis = EngineExternal->moduleInput->GetRightAxisY();
		if (value_of_axis <= 0) {
			if (map_of_buttons_and_joysticks[button_or_joystick_to_check].is_key_down) {
				state = KEY_UP;
			}
			else {
				state = KEY_IDLE;
			}
		}
		else {
			if (map_of_buttons_and_joysticks[button_or_joystick_to_check].is_key_up) {
				state = KEY_DOWN;
			}
			else {
				state = KEY_REPEAT;
			}
		}
		break;
	}
	case BUTTONSANDJOYSTICKS::RIGHT_JOYSTICK_LEFT: {
		int value_of_axis = EngineExternal->moduleInput->GetRightAxisX();
		if (value_of_axis >= 0) {
			if (map_of_buttons_and_joysticks[button_or_joystick_to_check].is_key_down) {
				state = KEY_UP;
			}
			else {
				state = KEY_IDLE;
			}
		}
		else {
			if (map_of_buttons_and_joysticks[button_or_joystick_to_check].is_key_up) {
				state = KEY_DOWN;
			}
			else {
				state = KEY_REPEAT;
			}
		}
		break;
	}
	case BUTTONSANDJOYSTICKS::RIGHT_JOYSTICK_RIGHT: {
		int value_of_axis = EngineExternal->moduleInput->GetRightAxisX();
		if (value_of_axis <= 0) {
			if (map_of_buttons_and_joysticks[button_or_joystick_to_check].is_key_down) {
				state = KEY_UP;
			}
			else {
				state = KEY_IDLE;
			}
		}
		else {
			if (map_of_buttons_and_joysticks[button_or_joystick_to_check].is_key_up) {
				state = KEY_DOWN;
			}
			else {
				state = KEY_REPEAT;
			}
		}
		break;
	}
	case BUTTONSANDJOYSTICKS::LEFT_JOYSTICK_UP: {
		int value_of_axis = EngineExternal->moduleInput->GetLeftAxisY();
		if (value_of_axis >= 0) {
			if (map_of_buttons_and_joysticks[button_or_joystick_to_check].is_key_down) {
				state = KEY_UP;
			}
			else {
				state = KEY_IDLE;
			}
		}
		else {
			if (map_of_buttons_and_joysticks[button_or_joystick_to_check].is_key_up) {
				state = KEY_DOWN;
			}
			else {
				state = KEY_REPEAT;
			}
		}
		break;
	}
	case BUTTONSANDJOYSTICKS::LEFT_JOYSTICK_DOWN: {
		int value_of_axis = EngineExternal->moduleInput->GetLeftAxisY();
		if (value_of_axis <= 0) {
			if (map_of_buttons_and_joysticks[button_or_joystick_to_check].is_key_down) {
				state = KEY_UP;
			}
			else {
				state = KEY_IDLE;
			}
		}
		else {
			if (map_of_buttons_and_joysticks[button_or_joystick_to_check].is_key_up) {
				state = KEY_DOWN;
			}
			else {
				state = KEY_REPEAT;
			}
		}
		break;
	}
	case BUTTONSANDJOYSTICKS::LEFT_JOYSTICK_LEFT: {
		int value_of_axis = EngineExternal->moduleInput->GetLeftAxisX();
		if (value_of_axis >= 0) {
			if (map_of_buttons_and_joysticks[button_or_joystick_to_check].is_key_down) {
				state = KEY_UP;
			}
			else {
				state = KEY_IDLE;
			}
		}
		else {
			if (map_of_buttons_and_joysticks[button_or_joystick_to_check].is_key_up) {
				state = KEY_DOWN;
			}
			else {
				state = KEY_REPEAT;
			}
		}
		break;
	}
	case BUTTONSANDJOYSTICKS::LEFT_JOYSTICK_RIGHT: {
		int value_of_axis = EngineExternal->moduleInput->GetRightAxisX();
		if (value_of_axis <= 0) {
			if (map_of_buttons_and_joysticks[button_or_joystick_to_check].is_key_down) {
				state = KEY_UP;
			}
			else {
				state = KEY_IDLE;
			}
		}
		else {
			if (map_of_buttons_and_joysticks[button_or_joystick_to_check].is_key_up) {
				state = KEY_DOWN;
			}
			else {
				state = KEY_REPEAT;
			}
		}
		break;
	}
	}
	return;
}

void C_Navigation::DoTheAction(GameObject* gameobject, BUTTONSANDJOYSTICKS button_or_joystick_used, ACTIONSNAVIGATION action, bool is_key_released)
{
	
	switch (action)
	{
	case ACTIONSNAVIGATION::MOVE: {
		C_Navigation* nav = static_cast<C_Navigation*>(gameObject->GetComponent(Component::TYPE::NAVIGATION));
		nav->Select();
		map_of_buttons_and_joysticks[button_or_joystick_being_used].is_key_down = false;
		map_of_buttons_and_joysticks[button_or_joystick_being_used].is_key_up = true;
		button_or_joystick_being_used = BUTTONSANDJOYSTICKS::NO_BUTTON_OR_JOYSTICK;
		switch (type_of_ui) {
		case Component::TYPE::BUTTON: {
			C_Button* button = static_cast<C_Button*>(gameObject->GetComponent(Component::TYPE::BUTTON));
			button->is_selected = false;
			break;
		}
		case Component::TYPE::CHECKBOX: {
			C_Checkbox* checbox = static_cast<C_Checkbox*>(gameObject->GetComponent(Component::TYPE::CHECKBOX));
			checbox->is_selected = false;
			break;
		}
		}
		if (nav->map_of_buttons_and_joysticks.count(button_or_joystick_being_used) == 0)
			return;
		nav->map_of_buttons_and_joysticks[button_or_joystick_being_used].is_key_down = true;
		nav->button_or_joystick_being_used = button_or_joystick_used;
		break; }

	case ACTIONSNAVIGATION::EXECUTE:
		switch (type_of_ui) {
		case Component::TYPE::BUTTON: {
			C_Button* button = static_cast<C_Button*>(gameObject->GetComponent(Component::TYPE::BUTTON));
			if (is_key_released) {
				button->ReleaseButton();
				return;
			}
			button->ExecuteButton();
			break;
		}
		case Component::TYPE::CHECKBOX: {
			C_Checkbox* checbox = static_cast<C_Checkbox*>(gameObject->GetComponent(Component::TYPE::CHECKBOX));
			if (is_key_released) {
				checbox->UnpressCheckbox();
				return;
			}
			checbox->PressCheckbox();
			break;
		}
		}
		break;
	}
}

void C_Navigation::Select()
{
	if (EngineExternal->moduleGui->uid_gameobject_of_ui_selected != 0) {
		GameObject* previous_selected = EngineExternal->moduleScene->GetGOFromUID(EngineExternal->moduleScene->root, EngineExternal->moduleGui->uid_gameobject_of_ui_selected);
		static_cast<C_Navigation*>(previous_selected->GetComponent(Component::TYPE::NAVIGATION))->is_selected = false;
	}
	EngineExternal->moduleGui->uid_gameobject_of_ui_selected = gameObject->UID;
	is_selected = true;

	switch (type_of_ui) {
	case Component::TYPE::BUTTON: {
		C_Button* button = static_cast<C_Button*>(gameObject->GetComponent(Component::TYPE::BUTTON));
		button->is_selected = true;
		break;
	}
	case Component::TYPE::CHECKBOX: {
		C_Checkbox* checbox = static_cast<C_Checkbox*>(gameObject->GetComponent(Component::TYPE::CHECKBOX));
		checbox->is_selected = true;
		break;
	}
	}
}



#ifndef STANDALONE
bool C_Navigation::OnEditor()
{
	if (Component::OnEditor() == true)
	{
		ImGui::Separator();


		if (ImGui::Checkbox("Is this UI component selected?", &is_selected)) {
			if (is_selected)
				Select();
			else
				EngineExternal->moduleGui->uid_gameobject_of_ui_selected = 0;
		}
		ImGui::Columns(3);

		WriteButtonOrJoystickOnEditor("Button A:", BUTTONSANDJOYSTICKS::BUTTON_A);
		
		WriteButtonOrJoystickOnEditor("Button B:", BUTTONSANDJOYSTICKS::BUTTON_B);

		WriteButtonOrJoystickOnEditor("Button X:", BUTTONSANDJOYSTICKS::BUTTON_X);

		WriteButtonOrJoystickOnEditor("Button Y:", BUTTONSANDJOYSTICKS::BUTTON_Y);
		
		WriteButtonOrJoystickOnEditor("Button Back:", BUTTONSANDJOYSTICKS::BUTTON_BACK);

		WriteButtonOrJoystickOnEditor("Button Home:", BUTTONSANDJOYSTICKS::BUTTON_GUIDE);

		WriteButtonOrJoystickOnEditor("Button Start:", BUTTONSANDJOYSTICKS::BUTTON_START);
		
		WriteButtonOrJoystickOnEditor("Button Left Trigger:", BUTTONSANDJOYSTICKS::BUTTON_LEFTTTRIGGER);

		WriteButtonOrJoystickOnEditor("Button Right Trigger:", BUTTONSANDJOYSTICKS::BUTTON_RIGHTTRIGGER);

		WriteButtonOrJoystickOnEditor("Button Left Shoulder:", BUTTONSANDJOYSTICKS::BUTTON_LEFTSHOULDER);

		WriteButtonOrJoystickOnEditor("Button Right Shoulder:", BUTTONSANDJOYSTICKS::BUTTON_RIGHTSHOULDER);

		WriteButtonOrJoystickOnEditor("DPad Up:", BUTTONSANDJOYSTICKS::BUTTON_DPAD_UP);

		WriteButtonOrJoystickOnEditor("DPad Down:", BUTTONSANDJOYSTICKS::BUTTON_DPAD_DOWN);

		WriteButtonOrJoystickOnEditor("DPad Left:", BUTTONSANDJOYSTICKS::BUTTON_DPAD_LEFT);

		WriteButtonOrJoystickOnEditor("DPad Right:", BUTTONSANDJOYSTICKS::BUTTON_DPAD_RIGHT);

		WriteButtonOrJoystickOnEditor("Right Joystick Up:", BUTTONSANDJOYSTICKS::RIGHT_JOYSTICK_UP);

		WriteButtonOrJoystickOnEditor("Right Joystick Down:", BUTTONSANDJOYSTICKS::RIGHT_JOYSTICK_DOWN);

		WriteButtonOrJoystickOnEditor("Right Joystick Left:", BUTTONSANDJOYSTICKS::RIGHT_JOYSTICK_LEFT);

		WriteButtonOrJoystickOnEditor("Right Joystick Right:", BUTTONSANDJOYSTICKS::RIGHT_JOYSTICK_RIGHT);

		WriteButtonOrJoystickOnEditor("Left Joystick Up:", BUTTONSANDJOYSTICKS::LEFT_JOYSTICK_UP);

		WriteButtonOrJoystickOnEditor("Left Joystick Down:", BUTTONSANDJOYSTICKS::LEFT_JOYSTICK_DOWN);

		WriteButtonOrJoystickOnEditor("Left Joystick Left:", BUTTONSANDJOYSTICKS::LEFT_JOYSTICK_LEFT);
		
		WriteButtonOrJoystickOnEditor("Left Joystick Right:", BUTTONSANDJOYSTICKS::LEFT_JOYSTICK_RIGHT);

		ImGui::Columns(1);

	}

	return true;
}

void C_Navigation::WriteButtonOrJoystickOnEditor(const char* text, BUTTONSANDJOYSTICKS button_or_joystick)
{
	ImGui::Text(text);
	ImGui::NextColumn();
	std::string text_string = "##";
	text_string += static_cast<int>(button_or_joystick);
	assert(static_cast<int>(ACTIONSNAVIGATION::TOTAL_ACTIONS_WITH_NONE) == 3, "Update the references of ACTIONSNAVIGATION");
	const char* items[] = { "None", "Move", "Execute" };
	const char* current_item = items[0];
	if (map_of_buttons_and_joysticks.count(button_or_joystick) != 0) {
		current_item = items[static_cast<int>(map_of_buttons_and_joysticks[button_or_joystick].action)];
	}
	if (ImGui::BeginCombo(text_string.c_str(), current_item))
	{
		for (int n = 0; n < IM_ARRAYSIZE(items); n++)
		{
			bool is_selected = (current_item == items[n]);
			if (ImGui::Selectable(items[n], is_selected)) {
				current_item = items[n];
				if (map_of_buttons_and_joysticks.count(button_or_joystick) != 0) {
					if (n == 0) {
						map_of_buttons_and_joysticks.erase(button_or_joystick);
					}
					else
						map_of_buttons_and_joysticks[button_or_joystick].action = static_cast<ACTIONSNAVIGATION>(n);
				}
				else {
					ActionToRealize action;
					action.action = static_cast<ACTIONSNAVIGATION>(n);
					map_of_buttons_and_joysticks.emplace(button_or_joystick, action);
				}
			}
			if (is_selected)
				ImGui::SetItemDefaultFocus();
		}
		ImGui::EndCombo();
	}
	ImGui::NextColumn();
	if (current_item != items[0]) {
		ImGui::Text("UID:"); 
		ImGui::SameLine();
		ImGui::Text(std::to_string(map_of_buttons_and_joysticks[button_or_joystick].uid_gameobject).c_str());
		if (ImGui::BeginDragDropTarget())
		{
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("_GAMEOBJECT"))
			{
				map_of_buttons_and_joysticks[button_or_joystick].uid_gameobject = *(int*)payload->Data;
			}
			ImGui::EndDragDropTarget();
		}
	}
	ImGui::NextColumn();
}


#endif // !STANDALONE

ActionToRealize::ActionToRealize() :action(ACTIONSNAVIGATION::NONE), uid_gameobject(0), is_key_down(false), is_key_up(true)
{
}

ActionToRealize::~ActionToRealize()
{
}
