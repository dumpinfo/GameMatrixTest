 

#ifndef C4Fixed_h
#define C4Fixed_h


//# \component	Math Library
//# \prefix		Math/


#include "C4Memory.h"


namespace C4
{
	typedef int32			Fixed;
	typedef unsigned_int32	UnsignedFixed;


	class Integer2D
	{
		public:

			int32	x;
			int32	y;

			Integer2D() = default;

			Integer2D(int32 i, int32 j)
			{
				x = i;
				y = j;
			}

			Integer2D& Set(int32 i, int32 j)
			{
				x = i;
				y = j;
				return (*this);
			}

			int32& operator [](machine k)
			{
				return ((&x)[k]);
			}

			const int32& operator [](machine k) const
			{
				return ((&x)[k]);
			}

			Integer2D& operator +=(const Integer2D& v)
			{
				x += v.x;
				y += v.y;
				return (*this);
			}

			Integer2D& operator -=(const Integer2D& v)
			{
				x -= v.x;
				y -= v.y;
				return (*this);
			}
	};

	inline Integer2D operator +(const Integer2D& v1, const Integer2D& v2)
	{
		return (Integer2D(v1.x + v2.x, v1.y + v2.y));
	}

	inline Integer2D operator -(const Integer2D& v1, const Integer2D& v2)
	{
		return (Integer2D(v1.x - v2.x, v1.y - v2.y));
	}

	inline Integer2D operator *(const Integer2D& v, int32 t)
	{
		return (Integer2D(v.x * t, v.y * t));
	}

	inline Integer2D operator /(const Integer2D& v, int32 t)
	{
		return (Integer2D(v.x / t, v.y / t));
	}

	inline Integer2D operator <<(const Integer2D& v, unsigned_int32 shift)
	{
		return (Integer2D(v.x << shift, v.y << shift));
	}

	inline Integer2D operator >>(const Integer2D& v, unsigned_int32 shift)
	{
		return (Integer2D(v.x >> shift, v.y >> shift));
	}

	inline bool operator ==(const Integer2D& v1, const Integer2D& v2)
	{
		return ((v1.x == v2.x) && (v1.y == v2.y));
	}

	inline bool operator !=(const Integer2D& v1, const Integer2D& v2)
	{ 
		return ((v1.x != v2.x) || (v1.y != v2.y));
	}
 

	class Integer3D 
	{
		public:
 
			int32	x;
			int32	y; 
			int32	z; 

			Integer3D() = default;

			Integer3D(int32 i, int32 j, int32 k) 
			{
				x = i;
				y = j;
				z = k;
			}

			Integer3D& Set(int32 i, int32 j, int32 k)
			{
				x = i;
				y = j;
				z = k;
				return (*this);
			}

			int32& operator [](machine k)
			{
				return ((&x)[k]);
			}

			const int32& operator [](machine k) const
			{
				return ((&x)[k]);
			}

			Integer3D& operator +=(const Integer3D& v)
			{
				x += v.x;
				y += v.y;
				z += v.z;
				return (*this);
			}

			Integer3D& operator -=(const Integer3D& v)
			{
				x -= v.x;
				y -= v.y;
				z -= v.z;
				return (*this);
			}
	};

	inline Integer3D operator +(const Integer3D& v1, const Integer3D& v2)
	{
		return (Integer3D(v1.x + v2.x, v1.y + v2.y, v1.z + v2.z));
	}

	inline Integer3D operator -(const Integer3D& v1, const Integer3D& v2)
	{
		return (Integer3D(v1.x - v2.x, v1.y - v2.y, v1.z - v2.z));
	}

	inline Integer3D operator %(const Integer3D& v1, const Integer3D& v2)
	{
		return (Integer3D(v1.y * v2.z - v1.z * v2.y, v1.z * v2.x - v1.x * v2.z, v1.x * v2.y - v1.y * v2.x));
	}

	inline Integer3D operator *(const Integer3D& v, int32 t)
	{
		return (Integer3D(v.x * t, v.y * t, v.z * t));
	}

	inline Integer3D operator /(const Integer3D& v, int32 t)
	{
		return (Integer3D(v.x / t, v.y / t, v.z / t));
	}

	inline Integer3D operator <<(const Integer3D& v, unsigned_int32 shift)
	{
		return (Integer3D(v.x << shift, v.y << shift, v.z << shift));
	}

	inline Integer3D operator >>(const Integer3D& v, unsigned_int32 shift)
	{
		return (Integer3D(v.x >> shift, v.y >> shift, v.z >> shift));
	}

	inline bool operator ==(const Integer3D& v1, const Integer3D& v2)
	{
		return ((v1.x == v2.x) && (v1.y == v2.y) && (v1.z == v2.z));
	}

	inline bool operator !=(const Integer3D& v1, const Integer3D& v2)
	{
		return ((v1.x != v2.x) || (v1.y != v2.y) || (v1.z != v2.z));
	}


	class Fixed2D
	{
		public:

			Fixed	x;
			Fixed	y;

			Fixed2D() = default;

			Fixed2D(Fixed i, Fixed j)
			{
				x = i;
				y = j;
			}

			Fixed2D(const Integer2D& v)
			{
				x = v.x;
				y = v.y;
			}

			Fixed2D& Set(Fixed i, Fixed j)
			{
				x = i;
				y = j;
				return (*this);
			}

			Fixed& operator [](machine k)
			{
				return ((&x)[k]);
			}

			const Fixed& operator [](machine k) const
			{
				return ((&x)[k]);
			}

			Fixed2D& operator =(const Integer2D& v)
			{
				x = v.x;
				y = v.y;
				return (*this);
			}

			Fixed2D& operator +=(const Fixed2D& v)
			{
				x += v.x;
				y += v.y;
				return (*this);
			}

			Fixed2D& operator -=(const Fixed2D& v)
			{
				x -= v.x;
				y -= v.y;
				return (*this);
			}
	};

	inline Fixed2D operator +(const Fixed2D& v1, const Fixed2D& v2)
	{
		return (Fixed2D(v1.x + v2.x, v1.y + v2.y));
	}

	inline Fixed2D operator -(const Fixed2D& v1, const Fixed2D& v2)
	{
		return (Fixed2D(v1.x - v2.x, v1.y - v2.y));
	}

	inline Fixed2D operator *(const Fixed2D& v, Fixed t)
	{
		return (Fixed2D(v.x * t, v.y * t));
	}

	inline Fixed2D operator /(const Fixed2D& v, Fixed t)
	{
		return (Fixed2D(v.x / t, v.y / t));
	}

	inline Fixed2D operator <<(const Fixed2D& v, unsigned_int32 shift)
	{
		return (Fixed2D(v.x << shift, v.y << shift));
	}

	inline Fixed2D operator >>(const Fixed2D& v, unsigned_int32 shift)
	{
		return (Fixed2D(v.x >> shift, v.y >> shift));
	}

	inline bool operator ==(const Fixed2D& v1, const Fixed2D& v2)
	{
		return ((v1.x == v2.x) && (v1.y == v2.y));
	}

	inline bool operator !=(const Fixed2D& v1, const Fixed2D& v2)
	{
		return ((v1.x != v2.x) || (v1.y != v2.y));
	}


	class Fixed3D
	{
		public:

			Fixed	x;
			Fixed	y;
			Fixed	z;

			Fixed3D() = default;

			Fixed3D(Fixed i, Fixed j, Fixed k)
			{
				x = i;
				y = j;
				z = k;
			}

			Fixed3D(const Integer3D& v)
			{
				x = v.x;
				y = v.y;
				z = v.z;
			}

			Fixed3D& Set(Fixed i, Fixed j, Fixed k)
			{
				x = i;
				y = j;
				z = k;
				return (*this);
			}

			Fixed& operator [](machine k)
			{
				return ((&x)[k]);
			}

			const Fixed& operator [](machine k) const
			{
				return ((&x)[k]);
			}

			Fixed3D& operator =(const Integer3D& v)
			{
				x = v.x;
				y = v.y;
				z = v.z;
				return (*this);
			}

			Fixed3D& operator +=(const Fixed3D& v)
			{
				x += v.x;
				y += v.y;
				z += v.z;
				return (*this);
			}

			Fixed3D& operator -=(const Fixed3D& v)
			{
				x -= v.x;
				y -= v.y;
				z -= v.z;
				return (*this);
			}
	};

	inline Fixed3D operator +(const Fixed3D& v1, const Fixed3D& v2)
	{
		return (Fixed3D(v1.x + v2.x, v1.y + v2.y, v1.z + v2.z));
	}

	inline Fixed3D operator -(const Fixed3D& v1, const Fixed3D& v2)
	{
		return (Fixed3D(v1.x - v2.x, v1.y - v2.y, v1.z - v2.z));
	}

	inline Fixed3D operator %(const Fixed3D& v1, const Fixed3D& v2)
	{
		return (Fixed3D(v1.y * v2.z - v1.z * v2.y, v1.z * v2.x - v1.x * v2.z, v1.x * v2.y - v1.y * v2.x));
	}

	inline Fixed3D operator *(const Fixed3D& v, Fixed t)
	{
		return (Fixed3D(v.x * t, v.y * t, v.z * t));
	}

	inline Fixed3D operator /(const Fixed3D& v, Fixed t)
	{
		return (Fixed3D(v.x / t, v.y / t, v.z / t));
	}

	inline Fixed3D operator <<(const Fixed3D& v, unsigned_int32 shift)
	{
		return (Fixed3D(v.x << shift, v.y << shift, v.z << shift));
	}

	inline Fixed3D operator >>(const Fixed3D& v, unsigned_int32 shift)
	{
		return (Fixed3D(v.x >> shift, v.y >> shift, v.z >> shift));
	}

	inline bool operator ==(const Fixed3D& v1, const Fixed3D& v2)
	{
		return ((v1.x == v2.x) && (v1.y == v2.y) && (v1.z == v2.z));
	}

	inline bool operator !=(const Fixed3D& v1, const Fixed3D& v2)
	{
		return ((v1.x != v2.x) || (v1.y != v2.y) || (v1.z != v2.z));
	}
}


#endif

// ZYUQURM
