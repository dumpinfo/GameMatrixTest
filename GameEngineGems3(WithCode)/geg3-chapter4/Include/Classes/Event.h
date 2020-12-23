#pragma once

#include "../Leadwerks.h"

namespace Leadwerks
{ 
	/*
	    #define EVENT_TOUCH_DOWN 1
    #define EVENT_TOUCH_MOVE 2
    #define EVENT_TOUCH_UP 3
    #define EVENT_WINDOW_CLOSE 4
    #define EVENT_WINDOW_SIZE 5
    #define EVENT_WINDOW_MOVE 13
    #define EVENT_WINDOW_ACTIVATE 6
    #define EVENT_WINDOW_DEACTIVATE 7
    #define EVENT_KEY_DOWN 8
    #define EVENT_KEY_UP 9
    #define EVENT_MOUSE_DOWN 10
    #define EVENT_MOUSE_UP 11
	#define EVENT_MOUSE_MOVE 12
		*/

	class Event : public Object//lua
	{
	public:
		int id;//lua
		//const char* source;
		Object* source;//lua
		int data;//lua
		int x;
		int y;
        int z;
        
		std::string Debug();
		
        //void Emit();
		Event(const int id=0, Object* source=NULL, const int data=0, const float x=0, const float y=0, const float z=0);//lua
		//Event(const int id, Gadget* source = NULL, const int data = 0, const int x = 0, const int y = 0);
        
		virtual std::string GetClassName();

		static const int TouchDown;//lua
		static const int TouchMove;//lua
		static const int TouchUp;//lua
		static const int WindowClose;//lua
		static const int WindowSize;//lua
		static const int WindowMove;//lua
		static const int WindowActivate;//lua
		static const int WindowDeactivate;//lua
		static const int KeyDown;//lua
		static const int KeyUp;//lua
		static const int MouseDown;//lua
		static const int  MouseUp;//lua
		static const int  MouseMove;//lua
		static const int WidgetAction;//lua
		static const int WidgetSelect;//lua
		static int AllocID();
		static int IDCounter;
		//std::string ToString();
	};
	extern vector<Event> eventqueue__;
    bool PeekEvent();
    Event WaitEvent();
	void FlushEvents();
}
