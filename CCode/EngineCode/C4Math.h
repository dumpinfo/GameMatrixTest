 

#ifndef C4Math_h
#define C4Math_h


//# \component	Math Library
//# \prefix		Math/


#include "C4Memory.h"


#if C4WINDOWS && C4FASTBUILD

	extern "C"
	{
		double __cdecl fabs(double);
		double __cdecl ceil(double);
		double __cdecl floor(double);
        double __cdecl acos(double);
        double __cdecl asin(double);
        double __cdecl atan(double);
        double __cdecl atan2(double, double);
        double __cdecl exp(double);
        double __cdecl log(double);
        double __cdecl log10(double);
        double __cdecl pow(double, double);
	}

#endif


namespace C4
{
	const float kWeldEpsilon			= 1.0e-3F;
	const float kWeldEpsilonSquared		= 1.0e-6F;
	const float kBoundaryEpsilon		= 1.0e-3F;


	//# \namespace	C4		The namespace containing global mathematical functions.
	//
	//# The $C4$ namespace contains global functions that don't belong to any other class or namespace.
	//
	//# \def	namespace C4 {...}
	//
	//# \also	$@Math@$


	//# \function	C4::CosSin		Calculates the sine and cosine of an angle simultaneously.
	//
	//# \proto	Vector2D CosSin(float f);
	//
	//# \param	f	The angle for which to calculate sine and cosine, in radians.
	//
	//# \desc
	//# The $CosSin$ function returns a $@Vector2D@$ object for which the <i>x</i> coordinate contains the
	//# cosine of the angle specified by the $f$ parameter and the <i>y</i> coordinate contains its sine.
	//#
	//# This function executes with much higher performance compared to calculating the sine and cosine separately.
	//
	//# \also	$@Math::GetTrigTable@$


	struct Line
	{
		unsigned_int16		index[2];

		Line& operator =(const Line& l)
		{
			index[0] = l.index[0];
			index[1] = l.index[1];
			return (*this);
		}

		void operator =(const Line& l) volatile
		{
			index[0] = l.index[0];
			index[1] = l.index[1];
		}

		void Set(unsigned_int32 i1, unsigned_int32 i2)
		{
			index[0] = (unsigned_int16) i1;
			index[1] = (unsigned_int16) i2;
		}

		void Set(unsigned_int32 i1, unsigned_int32 i2) volatile
		{
			index[0] = (unsigned_int16) i1;
			index[1] = (unsigned_int16) i2;
		}
	};


	struct Triangle
	{
		unsigned_int16		index[3];

		Triangle& operator =(const Triangle& t)
		{
			index[0] = t.index[0];
			index[1] = t.index[1]; 
			index[2] = t.index[2];
			return (*this);
		} 

		void operator =(const Triangle& t) volatile 
		{
			index[0] = t.index[0];
			index[1] = t.index[1]; 
			index[2] = t.index[2];
		} 
 
		void Set(unsigned_int32 i1, unsigned_int32 i2, unsigned_int32 i3)
		{
			index[0] = (unsigned_int16) i1;
			index[1] = (unsigned_int16) i2; 
			index[2] = (unsigned_int16) i3;
		}

		void Set(unsigned_int32 i1, unsigned_int32 i2, unsigned_int32 i3) volatile
		{
			index[0] = (unsigned_int16) i1;
			index[1] = (unsigned_int16) i2;
			index[2] = (unsigned_int16) i3;
		}
	};


	struct Quad
	{
		unsigned_int16		index[4];

		Quad& operator =(const Quad& q)
		{
			index[0] = q.index[0];
			index[1] = q.index[1];
			index[2] = q.index[2];
			index[3] = q.index[3];
			return (*this);
		}

		void operator =(const Quad& q) volatile
		{
			index[0] = q.index[0];
			index[1] = q.index[1];
			index[2] = q.index[2];
			index[3] = q.index[3];
		}

		void Set(unsigned_int32 i1, unsigned_int32 i2, unsigned_int32 i3, unsigned_int32 i4)
		{
			index[0] = (unsigned_int16) i1;
			index[1] = (unsigned_int16) i2;
			index[2] = (unsigned_int16) i3;
			index[3] = (unsigned_int16) i4;
		}

		void Set(unsigned_int32 i1, unsigned_int32 i2, unsigned_int32 i3, unsigned_int32 i4) volatile
		{
			index[0] = (unsigned_int16) i1;
			index[1] = (unsigned_int16) i2;
			index[2] = (unsigned_int16) i3;
			index[3] = (unsigned_int16) i4;
		}
	};


	inline float Fabs(float x)
	{
		return ((float) fabs(x));
	}

	inline float Fnabs(float x)
	{
		return (-(float) fabs(x));
	}

	inline float Fmin(const float& x, const float& y)
	{
		#if C4SIMD

			float	result;

			VecStoreX(VecMinScalar(VecLoadScalar(&x), VecLoadScalar(&y)), &result);
			return (result);

		#else

			return ((x < y) ? x : y);

		#endif
	}

	inline float Fmin(const float& x, const float& y, const float& z)
	{
		#if C4SIMD

			float	result;

			VecStoreX(VecMinScalar(VecMinScalar(VecLoadScalar(&x), VecLoadScalar(&y)), VecLoadScalar(&z)), &result);
			return (result);

		#else

			return (Fmin(Fmin(x, y), z));

		#endif
	}

	inline float Fmin(const float& x, const float& y, const float& z, const float& w)
	{
		#if C4SIMD

			float	result;

			VecStoreX(VecMinScalar(VecMinScalar(VecLoadScalar(&x), VecLoadScalar(&y)), VecMinScalar(VecLoadScalar(&z), VecLoadScalar(&w))), &result);
			return (result);

		#else

			return (Fmin(Fmin(x, y), Fmin(z, w)));

		#endif
	}

	inline float Fmax(const float& x, const float& y)
	{
		#if C4SIMD

			float	result;

			VecStoreX(VecMaxScalar(VecLoadScalar(&x), VecLoadScalar(&y)), &result);
			return (result);

		#else

			return ((x < y) ? y : x);

		#endif
	}

	inline float Fmax(const float& x, const float& y, const float& z)
	{
		#if C4SIMD

			float	result;

			VecStoreX(VecMaxScalar(VecMaxScalar(VecLoadScalar(&x), VecLoadScalar(&y)), VecLoadScalar(&z)), &result);
			return (result);

		#else

			return (Fmax(Fmax(x, y), z));

		#endif
	}

	inline float Fmax(const float& x, const float& y, const float& z, const float& w)
	{
		#if C4SIMD

			float	result;

			VecStoreX(VecMaxScalar(VecMaxScalar(VecLoadScalar(&x), VecLoadScalar(&y)), VecMaxScalar(VecLoadScalar(&z), VecLoadScalar(&w))), &result);
			return (result);

		#else

			return (Fmax(Fmax(x, y), Fmax(z, w)));

		#endif
	}

	inline float FminZero(const float& x)
	{
		#if C4SIMD

			float	result;

			VecStoreX(VecMinScalar(VecLoadScalar(&x), VecFloatGetZero()), &result);
			return (result);

		#else

			return ((x < 0.0F) ? x : 0.0F);

		#endif
	}

	inline float FmaxZero(const float& x)
	{
		#if C4SIMD

			float	result;

			VecStoreX(VecMaxScalar(VecLoadScalar(&x), VecFloatGetZero()), &result);
			return (result);

		#else

			return ((x < 0.0F) ? 0.0F : x);

		#endif
	}

	inline float Saturate(const float& x)
	{
		#if C4SIMD

			float	result;

			VecStoreX(VecMinScalar(VecMaxScalar(VecLoadScalar(&x), VecFloatGetZero()), VecLoadVectorConstant<0x3F800000>()), &result);
			return (result);

		#else

			float f = (x < 0.0F) ? 0.0F : x;
			return ((f < 1.0F) ? f : 1.0F);

		#endif
	}

	inline float Clamp(const float& x, const float& y, const float& z)
	{
		#if C4SIMD

			float	result;

			VecStoreX(VecMinScalar(VecMaxScalar(VecLoadScalar(&x), VecLoadScalar(&y)), VecLoadScalar(&z)), &result);
			return (result);

		#else

			float f = (x < y) ? y : x;
			return ((f < z) ? f : z);

		#endif
	}

	inline float Floor(const float& x)
	{
		#if C4SIMD

			float	result;

			VecStoreX(VecFloorScalar(VecLoadScalar(&x)), &result);
			return (result);

		#else

			return ((float) floor(x));

		#endif
	}

	inline float PositiveFloor(const float& x)
	{
		#if C4SIMD

			float	result;

			VecStoreX(VecPositiveFloorScalar(VecLoadScalar(&x)), &result);
			return (result);

		#else

			return ((float) floor(x));

		#endif
	}

	inline float NegativeFloor(const float& x)
	{
		#if C4SIMD

			float	result;

			VecStoreX(VecNegativeFloorScalar(VecLoadScalar(&x)), &result);
			return (result);

		#else

			return ((float) floor(x));

		#endif
	}

	inline float Ceil(const float& x)
	{
		#if C4SIMD

			float	result;

			VecStoreX(VecCeilScalar(VecLoadScalar(&x)), &result);
			return (result);

		#else

			return ((float) ceil(x));

		#endif
	}

	inline float PositiveCeil(const float& x)
	{
		#if C4SIMD

			float	result;

			VecStoreX(VecPositiveCeilScalar(VecLoadScalar(&x)), &result);
			return (result);

		#else

			return ((float) ceil(x));

		#endif
	}

	inline float NegativeCeil(const float& x)
	{
		#if C4SIMD

			float	result;

			VecStoreX(VecNegativeCeilScalar(VecLoadScalar(&x)), &result);
			return (result);

		#else

			return ((float) ceil(x));

		#endif
	}

	inline void FloorCeil(const float& x, float *f, float *c)
	{
		#if C4SIMD

			vec_float		vf, vc;

			VecFloorCeilScalar(VecLoadScalar(&x), &vf, &vc);
			VecStoreX(vf, f);
			VecStoreX(vc, c);

		#else

			*f = (float) floor(x);
			*c = (float) ceil(x);

		#endif
	}

	inline void PositiveFloorCeil(const float& x, float *f, float *c)
	{
		#if C4SIMD

			vec_float		vf, vc;

			VecPositiveFloorCeilScalar(VecLoadScalar(&x), &vf, &vc);
			VecStoreX(vf, f);
			VecStoreX(vc, c);

		#else

			*f = (float) floor(x);
			*c = (float) ceil(x);

		#endif
	}

	inline void NegativeFloorCeil(const float& x, float *f, float *c)
	{
		#if C4SIMD

			vec_float		vf, vc;

			VecNegativeFloorCeilScalar(VecLoadScalar(&x), &vf, &vc);
			VecStoreX(vf, f);
			VecStoreX(vc, c);

		#else

			*f = (float) floor(x);
			*c = (float) ceil(x);

		#endif
	}

	inline float Frac(const float& x)
	{
		return (x - Floor(x));
	}

	inline float PositiveFrac(const float& x)
	{
		return (x - PositiveFloor(x));
	}

	inline float NegativeFrac(const float& x)
	{
		return (x - NegativeFloor(x));
	}

	inline float Fsgn(const float& x)
	{
		#if C4SIMD

			float	result;

			VecStoreX(VecFsgn(VecLoadScalar(&x)), &result);
			return (result);

		#else

			return ((x < 0.0F) ? -1.0F : ((x > 0.0F) ? 1.0F : 0.0F));

		#endif
	}

	inline float NonzeroFsgn(const float& x)
	{
		#if C4SIMD

			float	result;

			VecStoreX(VecNonzeroFsgn(VecLoadScalar(&x)), &result);
			return (result);

		#else

			return ((x < 0.0F) ? -1.0F : 1.0F);

		#endif
	}

	inline float Asin(float x)
	{
		return ((float) asin(Clamp(x, -1.0F, 1.0F)));
	}

	inline float Acos(float x)
	{
		return ((float) acos(Clamp(x, -1.0F, 1.0F)));
	}

	inline float Atan(float x)
	{
		return ((float) atan(x));
	}

	inline float Atan(float y, float x)
	{
		return ((float) atan2(y, x));
	}

	inline float Exp(float x)
	{
		return ((float) exp(x));
	}

	inline double Exp(double x)
	{
		return (exp(x));
	}

	inline float Log(float x)
	{
		return ((float) log(x));
	}

	inline float Log10(float x)
	{
		return ((float) log10(x));
	}

	inline float Pow(float base, float exponent)
	{
		return ((float) pow(base, exponent));
	}


	class Vector2D;
	class Vector3D;
	struct ConstVector2D;


	C4API float Sqrt(float x);
	C4API float InverseSqrt(float x);

	C4API float Sin(float x);
	C4API float Cos(float x);
	C4API float Tan(float x);
	C4API void CosSin(float x, float *c, float *s);
	C4API Vector2D CosSin(float x);


	namespace Math
	{
		C4API extern const unsigned_int32 trigTable[256][2];

		inline const ConstVector2D *GetTrigTable(void)
		{
			return (reinterpret_cast<const ConstVector2D *>(trigTable));
		}

		C4API Vector3D CreatePerpendicular(const Vector3D& v);
		C4API Vector3D CreateUnitPerpendicular(const Vector3D& v);

		C4API unsigned_int32 Hash(int32 count, const float *value);
	}
}


#endif

// ZYUQURM
