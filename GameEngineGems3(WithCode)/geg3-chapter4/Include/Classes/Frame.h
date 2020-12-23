#pragma once
#pragma warning(disable:4290)
#pragma warning(disable:4996)

#include "../Leadwerks3D.h"

namespace Leadwerks3D
{
	class Mipmap;

	class Frame
	{
		public:
		vector<Mipmap*> mipmap;
		
		virtual ~Frame();
	};	
}
