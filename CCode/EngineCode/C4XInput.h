 

#ifndef C4XInput_h
#define C4XInput_h


#include "C4Defines.h"


#define XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE		7849
#define XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE		8689
#define XINPUT_GAMEPAD_TRIGGER_THRESHOLD		30
#define XINPUT_FLAG_GAMEPAD						0x00000001

struct XINPUT_GAMEPAD
{
	WORD					wButtons;
	BYTE					bLeftTrigger;
	BYTE					bRightTrigger;
	SHORT					sThumbLX;
	SHORT					sThumbLY;
	SHORT					sThumbRX;
	SHORT					sThumbRY;
};

struct XINPUT_STATE
{
	DWORD					dwPacketNumber;
	XINPUT_GAMEPAD			Gamepad;
};

struct XINPUT_VIBRATION
{
	WORD					wLeftMotorSpeed;
	WORD					wRightMotorSpeed;
};

struct XINPUT_CAPABILITIES
{
	BYTE					Type;
	BYTE					SubType;
	WORD					Flags;
	XINPUT_GAMEPAD			Gamepad;
	XINPUT_VIBRATION		Vibration;
};

extern "C"
{
	DWORD WINAPI XInputGetState(DWORD, XINPUT_STATE *);
	DWORD WINAPI XInputGetCapabilities(DWORD, DWORD, XINPUT_CAPABILITIES *);
}


#endif

// ZYUQURM
