#pragma once

#include "drivers/GUI/GUIDriver.h"
//#include "../Leadwerks3D.h"

#define MOUSE_LEFT 1
#define MOUSE_RIGHT 2
#define MOUSE_MIDDLE 3
#define MOUSE_BACK 4
#define MOUSE_FORWARD 5

#define KEY_UP 0
#define KEY_DOWN 1
#define KEY_HIT 2

namespace Leadwerks3D
{
	class GUIDriver;
	
	class Gadget
	{
	public:
		Gadget* parent;
		vector<Gadget*> kids;
		int rect[4];
		int clientrect[4];
		bool minimized;
		bool maximized;
		GUIDriver* driver;
		int style;
		bool keystate[256];
		bool keyhitstate[256];
		bool mousestate[5];
		bool mousehitstate[5];
		int sensitivity;
		bool pointervisible;
		//int mouseposition[3];
		//int mousespeed[3];
		
		Gadget();
		virtual ~Gadget();
		
		//virtual int GetX()=0;
		//virtual int GetY()=0;
		virtual int GetMouseX()=0;
		virtual int GetMouseY()=0;
		//virtual int GetMouseZ();
		//virtual int GetMouseSpeedX();
		//virtual int GetMouseSpeedY();
		//virtual int GetMouseSpeedZ();
		virtual bool GetKeyDown(const int keycode);
		virtual bool GetMouseDown(const int button = MOUSE_LEFT);
		virtual bool GetKeyHit(const int keycode);
		virtual bool GetMouseHit(const int button = MOUSE_LEFT);
		virtual void FlushMouse();
		virtual void FlushKeys();
		virtual void MoveMouse(const int x, const int y)=0;
		virtual void Minimize()=0;
		virtual void Maximize()=0;
		virtual void Restore()=0;
		virtual void Activate()=0;
		virtual bool Minimized();
		virtual bool Maximized();
		virtual void ShowPointer();
		virtual void HidePointer();
		//SetPointer(Pointer* pointer);
		virtual bool IntersectsPoint(const int x, const int y, const bool global=true)=0;
		virtual bool ContainsPoint(const int x, const int y, const bool global=true)=0;
		virtual int GetX();
		virtual int GetY();
		virtual int GetWidth();
		virtual int GetHeight();
		virtual int GetClientWidth();
		virtual int GetClientHeight();
		virtual void Hide()=0;
		virtual void Show()=0;
		//virtual void Enable()=0;
		//virtual void Disable()=0;
		virtual void SetShape(const int x, const int y, const int width, const int height)=0;
		//virtual bool Hidden()=0;
		//virtual bool Enabled()=0;
	};
}
