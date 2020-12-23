#pragma once

#include "Math3D.h"

namespace Leadwerks
{
	//#define PI 3.1415926535897932384626433832795
	#define PI_OVER_180 0.017453292519943295769236907684886
	#define PI_OVER_360 0.0087266462599716478846184538424431
	
	class Vec3;
	
    class Math//lua
    {
    public:
        static float DegToRad(const float degrees);
        static float RadToDeg(const float radians);
        static std::string Hex(const long long i);//lua
        static float Sin(const float a);
		static double Sin(const double& a);//lua
        static double Cos(const double& a);//lua
		static double Tan(const double& a);//lua
		static float Cos(const float a);
        static float Tan(const float a);
        static float ASin(const float a);//lua
        static float ACos(const float a);//lua
        static float ATan(const float a);//lua
        static float ATan2(const float y, const float x);//lua
        static float Degrees(const float a);//lua
        static float Radians(const float a);//lua
        static float Lerp(const float start, const float stop, const float amount);//lua
        static float Log2(const float num);//lua
        //float Pow2(const float f);
		static float DeltaAngle(float angle0, float angle1);//lua
		static float IncAngle(const float angle0, const float angle1, const float increment);//lua
        static int Pow2(const int f);//lua
        static float Round(const float val);//lua
        static float Sgn(const float f);//lua
		static float Random(const float max=1);//lua
        static float Random(const float min, const float max);//lua
		static float Rnd(const float min, const float max);
        static float Clamp(const float a, const float min, const float max);//lua
        static float Inc(const float newvalue, float oldvalue, const float increments);//lua
        static float Curve(const float target, const float current, const float divisions);//lua
        static float CurveAngle(float target, float current, const float divisions);//lua
        static double Mod(const double num, const double div);//lua
        static float Min(const float n0, const float n1);//lua
        static float Max(const float n0, const float n1);//lua
        static Vec3 Min(const Vec3& v0, const Vec3& v1);
		static  Vec3 Max(const Vec3& v0, const Vec3& v1);
		static float Abs(const float f);//lua
		//static Vec3 HSL(const float r, const float g, const float b);//l ua
		//static Vec3 RGB(const float h, const float s, const float l);//l ua
    };
    
    extern int rnd_state;
}
