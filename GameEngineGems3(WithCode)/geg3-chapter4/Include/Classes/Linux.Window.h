#pragma once

#include <string>
#include <stdio.h>
#include <stdlib.h>

/*
#include <GL/glx.h>
#include <GL/gl.h>
*/

#include <X11/X.h>    /* X11 constant (e.g. TrueColor) */
#include <X11/keysym.h>
#include  <X11/Xatom.h>

//Interferes with Collision::None:
#undef None

#include "../Leadwerks.h"

/*
ToDo:
ADD: Remove titlebar (untested): http://help.lockergnome.com/linux/lib-titlebar--ftopict199622.html
ADD: Maximized/minimized set and detect
ADD: Change resolution / fullscreen
FIX: If window is created, then immediately hidden, it will still be visible: same thing happens with maximized window
ADD: X11 key codes: http://www.gp32x.com/board/index.php?/topic/57164-raw-x11-keycodes/
*/
namespace Leadwerks
{
	#define MOUSE_LEFT 1
	#define MOUSE_RIGHT 2

	class Window : public Object
	{
		static Cursor x_cursor;
	public:
        ::Window window;
        std::string title;
        bool closed;
        Atom wmDeleteMessage;
        Vec3 mousecoords;
        bool mousedownstate[3];
        bool mousehitstate[3];
        bool keydownstate[256];
        bool keyhitstate[256];
        int x,y;
        Context* context;
		static ::XVisualInfo *visualinfo;
		static ::Display* display;
        bool takeownership;
		int style;

		int getxkey(XEvent *event);

		Window();
		virtual ~Window();

        virtual bool Closed();
 		virtual void Show();
		virtual void Hide();
		virtual std::string Debug();

        virtual bool GetHidden();
		virtual void Activate();
        virtual bool Minimized();
        virtual bool Maximized();
        virtual void Update();
		virtual std::string GetClassName();
		virtual int GetX();
		virtual int GetY();
		virtual int GetWidth();
		virtual int GetHeight();
		virtual int GetClientWidth();
		virtual int GetClientHeight();
        virtual void SetLayout(const int x, const int y, const int width, const int height);
		virtual void SetMousePosition(const float x, const float y);
		virtual void SetMousePosition(const float x, const float y, const float z);
        virtual Vec3 GetMousePosition();
		virtual bool MouseDown(const int button=1);
		virtual bool MouseHit(const int button=1);
		virtual bool KeyDown(const int keycode);
		virtual bool KeyHit(const int keycode);		//virtual void Show();
		//virtual void Hide();
		//virtual bool KeyDown(const int keycode);
		//virtual bool KeyHit(const int keycode);
		//virtual void HideMouse();
		//virtual void ShowMouse();
		virtual void Minimize();
		virtual void Maximize();
		virtual void Restore();
		//virtual bool Minimized();
		//virtual bool Maximized();

		static const int Titlebar;
		static const int Resizable;
		static const int Center;
		static const int Hidden;
		static const int Fullscreen;
		static const int FullScreen;
		static const int VRDisplay;
        static Leadwerks::Window* current;
        static Window* Create(XID xid);

        static Atom _NET_WM_STATE_MAXIMIZED_HORZ;
        static Atom _NET_WM_STATE_MAXIMIZED_VERT;
        static Atom _NET_WM_STATE;
        static Atom _NET_WM_STATE_HIDDEN;

        static Leadwerks::Window* GetCurrent();

        /*
		virtual void Show();
		virtual void Hide();
		virtual std::string Debug();

		virtual std::string GetClassName();
		virtual int GetX();
		virtual int GetY();
		virtual int GetWidth();
		virtual int GetHeight();
		virtual int GetClientWidth();
		virtual int GetClientHeight();

		virtual void SetLayout(const int x, const int y, const int width, const int height);

		virtual void Minimize();
		virtual void Maximize();
		virtual void Restore();
		virtual bool Minimized();
		virtual bool Maximized();
		virtual bool Closed();

		virtual bool Active();
		virtual void Activate();

		virtual bool KeyDown(const int keycode);
		virtual bool KeyHit(const int keycode);*/
		virtual void FlushKeys();

		virtual void HideMouse();
		virtual void ShowMouse();

		/*virtual void SetMousePosition(const float x, const float y);
		virtual void SetMousePosition(const float x, const float y, const float z);
		virtual Vec3 GetMousePosition();
		virtual bool MouseDown(const int button=MOUSE_LEFT);
		virtual bool MouseHit(const int button=MOUSE_RIGHT);*/
		virtual int MouseX();
		virtual int MouseY();
		virtual int MouseZ();
		virtual void FlushMouse();



		virtual Vec2 GetTouchPosition(const int index);
		virtual bool TouchDown(const int index);
		virtual bool TouchHit(const int index);

		//static Window* current;
		//static Window* GetCurrent();

		static Window* Create(const std::string& title="Leadwerks",const int x=0, const int y=0, const int width=1024, const int height=768, const int style=Window::Titlebar);
	};
}
