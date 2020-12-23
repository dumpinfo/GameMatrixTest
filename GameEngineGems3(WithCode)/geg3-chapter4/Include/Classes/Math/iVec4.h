#pragma once

#include "Math3D.h"

namespace Leadwerks
{
	class iVec4
	{
	public:
		union{int x,r;};
		union{int y,g;};
		union{int z,b;};
		union{int w,a;};
		
		iVec4();
		iVec4(const int x, const int y, const int z, const int w);
		
		/*bool operator<(const iVec2 v);
		bool operator>(const iVec2 v);
		bool operator==(const iVec2 v);*/
	};
	
	bool operator<(const iVec4 v0,const iVec4 v1);
	bool operator>(const iVec4 v0,const iVec4 v1);
}
