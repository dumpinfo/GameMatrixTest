#pragma once

#include "Math3D.h"

namespace Leadwerks
{

#define AXIS_X 1
#define AXIS_Y 2
#define AXIS_Z 3
	
class dVec4;
class dVec4;

// Classes
class dMat4 : public Object//lua
{
	public:
	
	//Attributes
	dVec4 i;//lua
	dVec4 j;//lua
	dVec4 k;//lua
	dVec4 t;//lua
	//union{dVec4 i,right;};
	//union{dVec4 j,up;};
	//union{dVec4 k,front;};
	//union{dVec4 t,trans;};
	
	//Constructors
	dMat4();//lua
	dMat4(const dVec3& i, const dVec3& j, const dVec3& k, const dVec3& t);//lua
	dMat4(const dVec4& i,const dVec4& j,const dVec4& k,const dVec4& t);//lua
	dMat4(const string & s);//lua
	dMat4(double ix, double iy=0,  double iz=0,  double iw=0,  double jx=0,  double jy=1,  double jz=0,  double jw=0,  double kx=0,  double ky=0,  double kz=1,  double kw=0,  double tx=0,  double ty=0,  double tz=0,  double tw=1);//lua
	dMat4(const double* f);
    
	//dMat4(const double pitch, const double yaw, const double roll);
	
	//Operators
	dMat4 operator-();
	dVec3 operator*(const dVec3& v);//lua
	dMat4 operator*(const dMat4& m);//lua
	void operator*=(const dMat4& m);
	bool operator!=(const dMat4& m);//lua
	bool operator==(const dMat4& m);//lua
	dVec4& operator[](const unsigned int n);//lua
	
	//Methods
	virtual void Serialize(Stream* stream);
	virtual void Deserialize(Stream* stream);
	virtual void Ortho(const double left, const double right, const double top, const double bottom, const double near, const double far);
	virtual void Perspective(const double left, const double right, const double bottom, const double top, const double near, const double far);
	//virtual void Perspective(const double aspect, const double zoom, const double znear, const double zfar);
	virtual std::string ToString();
	virtual dMat4 Multiply(const dMat4& m);
	virtual dMat4 Transpose();//lua
	virtual dMat4 Inverse();//lua
	virtual double Determinant();
	virtual dVec3 GetTranslation();//lua
	virtual dVec3 GetRotation();//lua
	virtual dVec3 GetScale();//lua
	virtual Quat GetQuaternion();//lua
	virtual dMat4 Normalize();//lua
	virtual std::string Debug();
	virtual std::string GetClassName();
	void GetQuaternion(Quat& result);

	//Make matrix from...
	virtual void MakeDir(const dVec3& dir, const char axis = 2);
	virtual void MakeIdentity();
	virtual void MakeTransRotScale(const dVec3& trans, const Quat& rot, const dVec3& scale);
	virtual void MakeTranslation(const dVec3& translation);
	virtual void MakeTranslation(const double x, const double y, const double z);
	virtual void MakeRotation(const dVec3& euler);
	virtual void MakeRotation(const double pitch, const double yaw, const double roll);
	virtual void MakeRotation(const Quat& q);
	virtual void SetRotation(const Quat& q);
    virtual void MakeScale(const dVec3& scale);
	virtual void MakePitch(const double pitch);
	virtual void MakeYaw(const double yaw);
	virtual void MakeRoll(const double roll);

	//double GetPitch();
	//double GetYaw();
	//double GetRoll();

};

bool operator>(const dMat4& m0,const dMat4& m1);
bool operator<(const dMat4& m0,const dMat4& m1);

extern dMat4 dIDENTITY;

}
