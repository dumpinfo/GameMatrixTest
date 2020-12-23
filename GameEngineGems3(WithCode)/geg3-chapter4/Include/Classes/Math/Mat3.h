#pragma once

#include "Math3D.h"

namespace Leadwerks
{
	class Mat4;
	
	// Classes
	class Mat3 : public Object//lua
	{
	public:
		Vec3 i;
		Vec3 j;
		Vec3 k;
		
		//Constructors
		Mat3();//lua
		Mat3(const Vec3& i,const Vec3& j,const Vec3& k);//lua
		Mat3(const Mat4& mat);//lua

		//Attributes
		//union{Vec3 i,right;};
		//union{Vec3 j,up;};
		//union{Vec3 k,front;};

		
		//Operators
		Mat3 operator*(const Mat3& m);//lua
		Vec3& operator[](unsigned int n);//lua

		//Methods
        float Determinant();
		std::string ToString() const;
		Mat3 Multiply(const Mat3& m);
		Mat3 Transpose();//lua
        virtual Mat3 Inverse();//lua
		virtual std::string GetClassName();

	};
	
	bool operator>(const Mat3 m0,const Mat3 m1);
	bool operator<(const Mat3 m0,const Mat3 m1);
}
