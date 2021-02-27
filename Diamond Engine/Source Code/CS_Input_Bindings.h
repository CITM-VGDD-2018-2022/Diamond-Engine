#pragma once

//Keyboard and mouse ===============================================================================

int GetKey(MonoObject* x)
{
	if (EngineExternal != nullptr)
		return EngineExternal->moduleInput->GetKey(*(int*)mono_object_unbox(x));

	return 0;
}
int GetMouseClick(MonoObject* x)
{
	if (EngineExternal != nullptr)
		return EngineExternal->moduleInput->GetMouseButton(*(int*)mono_object_unbox(x));

	return 0;
}
int MouseX()
{
	if (EngineExternal != nullptr)
		return EngineExternal->moduleInput->GetMouseXMotion();

	return 0;
}
int MouseY()
{
	if (EngineExternal != nullptr)
		return EngineExternal->moduleInput->GetMouseYMotion();

	return 0;
}

//Gamepad ===============================================================================

int GetGamepadButton(MonoObject* x)
{
	if (EngineExternal != nullptr)
		return EngineExternal->moduleInput->GetGamePadKey(*(int*)mono_object_unbox(x));

	return 0;
}

int GetGamepadAxis(MonoObject* x)
{
	//Input.GetAxis
	return 0;
}

int GetGamepadLeftTrigger()
{
	if (EngineExternal != nullptr)
		return EngineExternal->moduleInput->GetLeftTrigger();

	return 0;
}

int GetGamepadRightTrigger()
{
	if (EngineExternal != nullptr)
		return EngineExternal->moduleInput->GetRightTrigger();

	return 0;
}

