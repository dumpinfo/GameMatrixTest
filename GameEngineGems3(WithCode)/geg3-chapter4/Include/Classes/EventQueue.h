#pragma once

#include "../Leadwerks.h"

namespace Leadwerks
{
	class Event;
	
	class EventQueue : public Object//lua
	{
	public:
		static bool Peek();//lua
		static Event Wait();//lua
		static void Flush();//lua
		static void Emit(const Event& event);//lua

		static std::list<Event> Queue;
	};
}
