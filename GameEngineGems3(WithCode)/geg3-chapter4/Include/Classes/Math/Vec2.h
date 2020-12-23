#pragma once

#include "Math3D.h"

namespace Leadwerks
{
	
	// Classes
	class Vec2 : public Object//lua
	{
		public:

		//Attributes
		union{float x,r;};
		union{float y,g;};
		//float x,y,r,g;//lua
		
		//Constructors
		Vec2();//lua
		Vec2(const float x);//lua
		Vec2(const float x,const float y);//lua
		Vec2(const std::string& s);//lua
		
		//Operators
		Vec2 operator*(const float f);
		bool operator<(const Vec2 v);
		void operator+=(const Vec2& v);
		void operator/=(const float f);
		void operator*=(const Vec2& v);
		float& operator[](unsigned int n);
		
		Vec2 Normalize();//lua
		void Serialize(Stream* stream);
		std::string ToString() const;//lua
		float Length();//lua
		float DistanceToPoint(const Vec2& v);//lua
		virtual void Deserialize(Stream* stream);
		virtual std::string GetClassName();
		virtual std::string Debug();
	};
	
	bool operator<(const Vec2& v0,const Vec2& v1);
	bool operator>(const Vec2& v0,const Vec2& v1);

}
