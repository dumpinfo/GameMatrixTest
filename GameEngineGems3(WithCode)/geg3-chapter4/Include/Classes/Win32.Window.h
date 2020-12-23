#pragma once

#include "../Leadwerks.h"

//Doesn't appear to do anything.
//http://social.msdn.microsoft.com/Forums/windowsdesktop/en-US/6c1c67a9-5548-4e9b-989f-c7dbac0b1375/getwindowrect-on-nonresizable-windows-under-aero-glass
//#define WINVER 6.0

#define NOMINMAX
namespace win32
{
	#ifdef _WIN32
	#ifdef _AFXDLL
	#include <afx.h>	
	#else
	#include <windows.h>
	#endif
	#endif
}
#include <stdio.h>
#include <string>
#include <string.h>
//#include <windows.h>
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

	class Object;

	class Window : public Object//lua
	{
		void Update();
	public:
		HWND hwnd;
		HDC hdc;
		DEVMODE devmode;
		bool mousevisible;
		int style;
		bool minimized;
		bool maximized;
		bool closed;
		bool keydownstate[256];
		bool keyhitstate[256];
		bool mousedownstate[6];
		bool mousehitstate[6];
		bool customhwnd;
		std::string text;
		Context* context;
		int mousez;

		Window();
		virtual ~Window();

		virtual bool GetHidden();//lua
		virtual void Show();//lua
		virtual void Hide();//lua
		virtual std::string Debug();

		virtual std::string GetClassName();
		virtual int GetX();//lua
		virtual int GetY();//lua
		virtual int GetWidth();//lua
		virtual int GetHeight();//lua
		virtual int GetClientWidth();//lua
		virtual int GetClientHeight();//lua

		virtual void SetLayout(const int x, const int y, const int width, const int height);//lua

		virtual void Minimize();//lua
		virtual void Maximize();//lua
		virtual void Restore();//lua
		virtual bool Minimized();//lua
		virtual bool Maximized();//lua
		virtual bool Closed();//lua

		virtual bool Active();
		virtual void Activate();

		virtual bool KeyDown(const int keycode);//lua
		virtual bool KeyHit(const int keycode);//lua
		virtual void FlushKeys();//lua

		virtual void HideMouse();//lua
		virtual void ShowMouse();//lua

		virtual void SetMousePosition(const float x, const float y);//lua
		virtual void SetMousePosition(const float x, const float y, const float z);//lua
		virtual Vec3 GetMousePosition();//lua
		virtual bool MouseDown(const int button=MOUSE_LEFT);//lua
		virtual bool MouseHit(const int button=MOUSE_RIGHT);//lua
		virtual int MouseX();//lua
		virtual int MouseY();//lua
		virtual int MouseZ();//lua
		virtual void FlushMouse();//lua

		virtual Vec2 GetTouchPosition(const int index);//lua
		virtual bool TouchDown(const int index);//lua
		virtual bool TouchHit(const int index);//lua

		static const int Titlebar;//lua
		static const int Resizable;//lua
		static const int Center;//lua
		static const int Hidden;//lua
		static const int FullScreen;//lua
		static const int Fullscreen;//lua
		static const int VRDisplay;//lua

		static Window* Create(HWND hwnd);

		static Window* current;
		static Window* GetCurrent();//lua
		static Window* Create(const std::string& title="Leadwerks",const int x=0, const int y=0, const int width=1024, const int height=768, const int style=Window::Titlebar);//lua
	};

	//Functions
	Window* CreateCustomWindow(HWND hwnd);
	LRESULT CALLBACK WndProc( HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam );

	//Globals
	extern WNDCLASS wndclass;
	extern bool wndclassinitialized;
	extern HWND fullscreenhwnd__;
	extern std::map<HWND,Window*> windowmap__;
}
