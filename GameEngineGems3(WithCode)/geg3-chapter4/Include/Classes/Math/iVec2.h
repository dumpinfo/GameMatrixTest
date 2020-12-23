#pragma once

#include "Math3D.h"

namespace Leadwerks
{
	class iVec2 : public Object//lua
	{
	public:
		union{int x,r;};
		union{int y,g;};
		//float x,y,r,g;//lua

		iVec2();//lua
		iVec2(const int x, const int y);//lua
		
		virtual std::string GetClassName();
		/*bool operator<(const iVec2 v);
		bool operator>(const iVec2 v);
		bool operator==(const iVec2 v);*/
		bool operator<(const iVec2 v);
		//bool operator>(const iVec2 v);
	};
	
	bool operator<(const iVec2& v0,const iVec2& v1);
	bool operator>(const iVec2& v0,const iVec2& v1);
}
