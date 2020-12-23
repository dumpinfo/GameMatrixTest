#pragma once

#include "Math3D.h"

namespace Leadwerks
{
	class iVec3
	{
	public:
		union{int x,r;};
		union{int y,g;};
		union{int z,b;};
		
		iVec3();
		iVec3(const int x, const int y, const int z);
		
		/*bool operator<(const iVec2 v);
		bool operator>(const iVec2 v);
		bool operator==(const iVec2 v);*/
	};
	
	bool operator<(const iVec3 v0,const iVec3 v1);
	bool operator>(const iVec3 v0,const iVec3 v1);
}
