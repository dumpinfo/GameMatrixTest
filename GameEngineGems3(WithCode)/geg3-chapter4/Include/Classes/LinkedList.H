#include "../Leadwerks3D.h"

namespace Leadwerks3D
{
	class Object;
	class Link;
	
	class LinkedList : public Object
	{
	public:
		Link* firstlink;
		Link* lastlink;
		
		LinkedList();
		virtual ~LinkedList();
		
		virtual Link* GetFirstLink();
		virtual Link* GetLastLink();
		virtual Link* AddLast(Object* o);
		virtual Link* AddFirst(Object* o);

		static LinkedList* Create();
	};
}
