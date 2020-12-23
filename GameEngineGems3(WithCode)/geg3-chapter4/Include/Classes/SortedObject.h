#include "../Leadwerks.h"

namespace Leadwerks
{
	class Entity;
	class Surface;

	class SortedObject
	{
	public:
		Entity* entity;
		Surface* surface;
		float distance;
		
		static bool Compare(SortedObject first, SortedObject second);
	};
}
