#pragma once

#include "../Leadwerks.h"

namespace Leadwerks
{
	class Hook
	{
	public:
		Hook();
		virtual ~Hook();
		
		std::list<Hook*> list;
		std::list<Hook*>::iterator link;
		Object* extra;
		Object* (*callback)(Object* extra);
		
		Object* Call();
	};
}
