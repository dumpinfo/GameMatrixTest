#pragma once

#include "../Leadwerks.h"
//#include "../Keycodes.h"

#include "Key.h"

#include <stdio.h>
#include <string>
#include <string.h>
#include <vector>
#include <map>

#undef CreateWindow

namespace Leadwerks
{
	#define WINDOW_TITLEBAR 1
	#define WINDOW_RESIZABLE 2
	#define WINDOW_TOOL 4
	#define WINDOW_CLIENTCOORDS 8
	#define WINDOW_CENTER 16
	#define WINDOW_HIDDEN 32
	#define WINDOW_FULLSCREEN 64
		
	#define MOUSE_LEFT 1
	#define MOUSE_RIGHT 2
	#define MOUSE_MIDDLE 3
	#define MOUSE_BACK 4
	#define MOUSE_FORWARD 5
	
	class Window : public Object
	{
	public:
		int style;
		bool minimized;
		bool maximized;
		bool closed;
		bool active;
		bool keydownstate[256];
		bool keyhitstate[256];
		bool mousedownstate[5];
		bool mousehitstate[5];
		float mousex,mousey,mousez;
		int xpos,ypos;
		int windowwidth,windowheight;
		int clientwidth,clientheight;
		//int mouseVisible;
		//virtual void updateMouseVisibility();
		Context* context;
        
		Window();
		virtual ~Window();
        
		virtual void InitGraphics(const int version, const int samples)=0;
		virtual void MakeCurrent()=0;
		
		virtual void Update()=0;
		virtual void SetKey(int key,bool down)=0;
		virtual void SetMouse(float x,float y,float z,bool buttons[5])=0;
		
		virtual void Show()=0;
		virtual void Hide()=0;
		
		virtual void SetLayout(const int x, const int y, const int width, const int height)=0;
		virtual void Activate()=0;
		virtual void SetMousePosition(const int x, const int y, const int z=0)=0;
		
		virtual void Flip(const bool sync)=0;
		
		virtual int GetX();
		virtual int GetY();
		virtual int GetWidth();
		virtual int GetHeight();
		virtual int GetClientWidth();
		virtual int GetClientHeight();
        
		virtual void HideMouse()=0;
		virtual void ShowMouse()=0;
		
		virtual void Minimize();
		virtual void Maximize();
		virtual void Restore();
		virtual bool Minimized();
		virtual bool Maximized();
		virtual bool Closed();
		
		virtual bool Active();
		
		virtual bool KeyDown(const int keycode);
		virtual bool KeyHit(const int keycode);
		virtual void FlushKeys();
		
		virtual bool MouseDown(const int button=MOUSE_LEFT);
		virtual bool MouseHit(const int button=MOUSE_RIGHT);
		virtual int MouseX();
		virtual int MouseY();
		virtual int MouseZ();
        virtual Vec3 GetMousePosition();
		virtual void FlushMouse();
		
		virtual Vec2 GetTouchPosition(const int index);
		virtual bool TouchDown(const int index);
		virtual bool TouchHit(const int index);
		
        static Window* current;
        static const int Titlebar;
		static const int Resizable;
		static const int Center;
		static const int Hidden;
		static const int FullScreen;
        
        static Window* GetCurrent();
		static Window* Create(const std::string& title="Leadwerks",const int x=0, const int y=0,const int width=1024, const int height=768, const int style=Titlebar);
	};
}
