#pragma once

namespace Leadwerks3D
{
	class Object;
	class LinkedList;
	
	class Link : public Object
	{
	public:
		Object* value;
		Link* prev;
		Link* next;
		LinkedList* list;
		
		Link();
		virtual ~Link();
		
		virtual Link* NextLink();
		virtual Link* PrevLink();
		virtual Object* GetValue();
	};
}
