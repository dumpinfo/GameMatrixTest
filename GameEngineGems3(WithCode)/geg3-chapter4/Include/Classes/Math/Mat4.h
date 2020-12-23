#pragma once

#include "Math3D.h"

namespace Leadwerks
{

#define AXIS_X 1
#define AXIS_Y 2
#define AXIS_Z 3
	
class Vec3;
class Vec4;

// Classes
class Mat4 : public Object//lua
{
	public:
	
	//Attributes
	Vec4 i;//lua
	Vec4 j;//lua
	Vec4 k;//lua
	Vec4 t;//lua
	//union{Vec4 i,right;};
	//union{Vec4 j,up;};
	//union{Vec4 k,front;};
	//union{Vec4 t,trans;};
	
	//Constructors
	Mat4();//lua
	Mat4(const Vec3& i,const Vec3& j,const Vec3& k,const Vec3& t);//lua
	Mat4(const Vec4& i,const Vec4& j,const Vec4& k,const Vec4& t);//lua
	Mat4(const string & s);//lua
	Mat4(float ix, float iy=0,  float iz=0,  float iw=0,  float jx=0,  float jy=1,  float jz=0,  float jw=0,  float kx=0,  float ky=0,  float kz=1,  float kw=0,  float tx=0,  float ty=0,  float tz=0,  float tw=1);//lua
	Mat4(const float* f);
	Mat4(const dMat4& m);//lua
    
	//Mat4(const float pitch, const float yaw, const float roll);
	
	//Operators
	Mat4 operator-();
	Vec3 operator*(const Vec3& v);//lua
	Mat4 operator*(const Mat4& m);//lua
	void operator*=(const Mat4& m);
	bool operator!=(const Mat4& m);//lua
	bool operator==(const Mat4& m);//lua
	Vec4& operator[](const unsigned int n);//lua

	//Methods
	virtual void Serialize(Stream* stream);
	virtual void Deserialize(Stream* stream);
	virtual void Ortho(const float left, const float right, const float top, const float bottom, const float near, const float far);
	virtual void Perspective(const float left, const float right, const float bottom, const float top, const float near, const float far);
	//virtual void Perspective(const float aspect, const float zoom, const float znear, const float zfar);
	virtual std::string ToString();
	virtual Mat4 Multiply(const Mat4& m);
	virtual Mat4 Transpose();//lua
	virtual Mat4 Inverse();//lua
	virtual float Determinant();
	virtual Vec3 GetTranslation();//lua
	virtual Vec3 GetRotation();//lua
	virtual Vec3 GetScale();//lua
	virtual Quat GetQuaternion();//lua
	virtual Mat4 Normalize();//lua
	virtual std::string Debug();
	virtual std::string GetClassName();
	void GetQuaternion(Quat& result);

	//Make matrix from...
	virtual void MakeDir(const Vec3& dir, const char axis = 2);
	virtual void MakeIdentity();
	virtual void MakeTransRotScale(const Vec3& trans, const Quat& rot, const Vec3& scale);
	virtual void MakeTranslation(const Vec3& translation);
	virtual void MakeTranslation(const float x, const float y, const float z);
	virtual void MakeRotation(const Vec3& euler);
	virtual void MakeRotation(const float pitch, const float yaw, const float roll);
	virtual void MakeRotation(const Quat& q);
	virtual void SetRotation(const Quat& q);//lua
    virtual void MakeScale(const Vec3& scale);
	virtual void MakePitch(const float pitch);
	virtual void MakeYaw(const float yaw);
	virtual void MakeRoll(const float roll);

	//float GetPitch();
	//float GetYaw();
	//float GetRoll();

};

bool operator>(const Mat4& m0,const Mat4& m1);
bool operator<(const Mat4& m0,const Mat4& m1);

extern Mat4 IDENTITY;

}
