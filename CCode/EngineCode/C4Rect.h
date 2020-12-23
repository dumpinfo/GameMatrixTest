 

#ifndef C4Rect_h
#define C4Rect_h


#include "C4Fixed.h"


namespace C4
{
	class Rect
	{
		public:

			int32	left;
			int32	top;
			int32	right;
			int32	bottom;

			Rect() = default;

			Rect(int32 l, int32 t, int32 r, int32 b)
			{
				left = l;
				top = t;
				right = r;
				bottom = b;
			}

			Rect(const Integer2D& tl, const Integer2D& br)
			{
				left = tl.x;
				top = tl.y;
				right = br.x;
				bottom = br.y;
			}

			Rect& Set(int32 l, int32 t, int32 r, int32 b)
			{
				left = l;
				top = t;
				right = r;
				bottom = b;
				return (*this);
			}

			Rect& Set(const Integer2D& tl, const Integer2D& br)
			{
				left = tl.x;
				top = tl.y;
				right = br.x;
				bottom = br.y;
				return (*this);
			}

			operator bool(void) const
			{
				return ((right > left) && (bottom > top));
			}

			Integer2D& operator [](machine k)
			{
				return (reinterpret_cast<Integer2D *>(&left)[k]);
			}

			const Integer2D& operator [](machine k) const
			{
				return (reinterpret_cast<const Integer2D *>(&left)[k]);
			}

			Rect& operator +=(const Integer2D& p)
			{
				left += p.x;
				top += p.y;
				right += p.x;
				bottom += p.y;
				return (*this);
			}

			Rect& operator -=(const Integer2D& p)
			{
				left -= p.x;
				top -= p.y;
				right -= p.x;
				bottom -= p.y;
				return (*this);
			}

			Rect& operator *=(int32 t)
			{
				left *= t;
				top *= t;
				right *= t;
				bottom *= t;
				return (*this);
			}

			Rect& operator /=(int32 t)
			{
				left /= t;
				top /= t;
				right /= t; 
				bottom /= t;
				return (*this);
			} 

			Rect& operator |=(const Integer2D& p) 
			{
				left = Min(left, p.x);
				top = Min(top, p.y); 
				right = Max(right, p.x);
				bottom = Max(bottom, p.y); 
				return (*this); 
			}

			Rect& operator |=(const Rect& r)
			{ 
				left = Min(left, r.left);
				top = Min(top, r.top);
				right = Max(right, r.right);
				bottom = Max(bottom, r.bottom);
				return (*this);
			}

			Rect& operator &=(const Rect& r)
			{
				left = Max(left, r.left);
				top = Max(top, r.top);
				right = Min(right, r.right);
				bottom = Min(bottom, r.bottom);
				return (*this);
			}

			Rect operator -(void) const
			{
				return (Rect(-right, -bottom, -left, -top));
			}

			Rect operator +(const Integer2D& p) const
			{
				return (Rect(left + p.x, top + p.y, right + p.x, bottom + p.y));
			}

			Rect operator -(const Integer2D& p) const
			{
				return (Rect(left - p.x, top - p.y, right - p.x, bottom - p.y));
			}

			Rect operator *(int32 t) const
			{
				return (Rect(left * t, top * t, right * t, bottom * t));
			}

			Rect operator /(int32 t) const
			{
				return (Rect(left / t, top / t, right / t, bottom / t));
			}

			Rect operator |(const Integer2D& p) const
			{
				return (Rect(Min(left, p.x), Min(top, p.y), Max(right, p.x), Max(bottom, p.y)));
			}

			Rect operator |(const Rect& r) const
			{
				return (Rect(Min(left, r.left), Min(top, r.top), Max(right, r.right), Max(bottom, r.bottom)));
			}

			Rect operator &(const Rect& r) const
			{
				return (Rect(Max(left, r.left), Max(top, r.top), Min(right, r.right), Min(bottom, r.bottom)));
			}

			bool operator ==(const Rect& r) const
			{
				return ((left == r.left) && (top == r.top) && (right == r.right) && (bottom == r.bottom));
			}

			bool operator !=(const Rect& r) const
			{
				return ((left != r.left) || (top != r.top) || (right != r.right) || (bottom != r.bottom));
			}

			bool Empty(void) const
			{
				return ((right <= left) || (bottom <= top));
			}

			bool Contains(const Integer2D& p) const
			{
				return (((unsigned_int32) (p.x - left) < (unsigned_int32) MaxZero(right - left)) && ((unsigned_int32) (p.y - top) < (unsigned_int32) MaxZero(bottom - top)));
			}

			int32 Width(void) const
			{
				return (right - left);
			}

			int32 Height(void) const
			{
				return (bottom - top);
			}

			void Inset(int32 x, int32 y)
			{
				left += x;
				top += y;
				right -= x;
				bottom -= y;
			}
	};
}


#endif

// ZYUQURM
