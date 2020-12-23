#pragma once

#include "../Leadwerks.h"

namespace Leadwerks
{
	class Face;
	
	class Edge : public Object
	{
		public:
		int vertex[2];
		Face* face[2];
		
		Edge();

		virtual Edge* Copy();
	};
}
