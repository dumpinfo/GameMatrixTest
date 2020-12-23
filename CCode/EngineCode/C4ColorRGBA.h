 

#ifndef C4ColorRGBA_h
#define C4ColorRGBA_h


//# \component	Math Library
//# \prefix		Math/


#include "C4Memory.h"


namespace C4
{
	//# \class	ColorRGB	Encapsulates a floating-point RGB color.
	//
	//# The $ColorRGB$ class encapsulates a floating-point RGB color.
	//
	//# \def	class ColorRGB
	//
	//# \data	ColorRGB
	//
	//# \ctor	ColorRGB();
	//# \ctor	ColorRGB(float r, float g, float b);
	//
	//# \desc
	//# The $ColorRGB$ class encapsulates a color having floating-point red, green, and blue
	//# components in the range [0.0,&nbsp;1.0]. An additional alpha component is provided by the
	//# $@ColorRGBA@$ class. When a $ColorRGB$ object is converted to a $@ColorRGBA@$ object,
	//# the alpha component is assumed to be 1.0.
	//#
	//# The default constructor leaves the components of the color undefined. If the values
	//# $r$, $g$, and $b$ are supplied, then they are assigned to the red, green, and blue
	//# components of the color, respectively.
	//
	//# \operator	float& operator [](machine k);
	//#				Returns a reference to the $k$-th component of a color.
	//#				The value of $k$ must be 0, 1, or 2.
	//
	//# \operator	const float& operator [](machine k) const;
	//#				Returns a constant reference to the $k$-th component of a color.
	//#				The value of $k$ must be 0, 1, or 2.
	//
	//# \operator	ColorRGB& operator =(float f);
	//#				Sets all three components to the value $f$.
	//
	//# \operator	ColorRGB& operator +=(const ColorRGB& c);
	//#				Adds the color $c$.
	//
	//# \operator	ColorRGB& operator -=(const ColorRGB& c);
	//#				Subtracts the color $c$.
	//
	//# \operator	ColorRGB& operator *=(const ColorRGB& c);
	//#				Multiplies by the color $c$.
	//
	//# \operator	ColorRGB& operator *=(float t);
	//#				Multiplies all three components by $t$.
	//
	//# \operator	ColorRGB& operator /=(float t);
	//#				Divides all three components by $t$.
	//
	//# \action		ColorRGB operator -(const ColorRGB& c);
	//#				Returns the negation of the color $c$.
	//
	//# \action		ColorRGB operator +(const ColorRGB& c1, const ColorRGB& c2);
	//#				Returns the sum of the colors $c1$ and $c2$.
	//
	//# \action		ColorRGB operator -(const ColorRGB& c1, const ColorRGB& c2);
	//#				Returns the difference of the colors $c1$ and $c2$.
	//
	//# \action		ColorRGB operator *(const ColorRGB& c1, const ColorRGB& c2);
	//#				Returns the product of the colors $c1$ and $c2$.
	//
	//# \action		ColorRGB operator *(const ColorRGB& c, float t);
	//#				Returns the product of the color $c$ and the scalar $t$.
	//
	//# \action		ColorRGB operator *(float t, const ColorRGB& c);
	//#				Returns the product of the color $c$ and the scalar $t$.
	//
	//# \action		ColorRGB operator /(const ColorRGB& c, float t);
	//#				Returns the product of the color $c$ and the inverse of the scalar $t$.
	//
	//# \action		bool operator ==(const ColorRGB& c1, const ColorRGB& c2);
	//#				Returns a boolean value indicating the equality of the colors $c1$ and $c2$.
	//
	//# \action		bool operator !=(const ColorRGB& c1, const ColorRGB& c2);
	//#				Returns a boolean value indicating the inequality of the colors $c1$ and $c2$.
	//
	//# \also	$@ColorRGBA@$


	//# \function	ColorRGB::Set		Sets all three components of a color.
	//
	//# \proto	ColorRGB& Set(float r, float g, float b);
	//
	//# \param	r	The new red component.
	//# \param	g	The new green component.
	//# \param	b	The new blue component.
	//
	//# \desc
	//# The $Set$ function sets the red, green, and blue components of a color to the values
	//# given by the $r$, $g$, and $b$ parameters, respectively. 
	//#
	//# The return value is a reference to the color object.
 

	//# \member		ColorRGB 

	class ColorRGB
	{ 
		public:
 
			float	red;		//## The red component. 
			float	green;		//## The green component.
			float	blue;		//## The blue component.

			ColorRGB() = default; 

			ColorRGB(float r, float g, float b)
			{
				red = r;
				green = g;
				blue = b;
			}

			ColorRGB& Set(float r, float g, float b)
			{
				red = r;
				green = g;
				blue = b;
				return (*this);
			}

			void Set(float r, float g, float b) volatile
			{
				red = r;
				green = g;
				blue = b;
			}

			float& operator [](machine k)
			{
				return ((&red)[k]);
			}

			const float& operator [](machine k) const
			{
				return ((&red)[k]);
			}

			ColorRGB& operator =(const ColorRGB& c)
			{
				red = c.red;
				green = c.green;
				blue = c.blue;
				return (*this);
			}

			void operator =(const ColorRGB& c) volatile
			{
				red = c.red;
				green = c.green;
				blue = c.blue;
			}

			ColorRGB& operator =(float f)
			{
				red = f;
				green = f;
				blue = f;
				return (*this);
			}

			void operator =(float f) volatile
			{
				red = f;
				green = f;
				blue = f;
			}

			ColorRGB& operator +=(const ColorRGB& c)
			{
				red += c.red;
				green += c.green;
				blue += c.blue;
				return (*this);
			}

			ColorRGB& operator -=(const ColorRGB& c)
			{
				red -= c.red;
				green -= c.green;
				blue -= c.blue;
				return (*this);
			}

			ColorRGB& operator *=(const ColorRGB& c)
			{
				red *= c.red;
				green *= c.green;
				blue *= c.blue;
				return (*this);
			}

			ColorRGB& operator *=(float t)
			{
				red *= t;
				green *= t;
				blue *= t;
				return (*this);
			}

			ColorRGB& operator /=(float t)
			{
				float f = 1.0F / t;
				red *= f;
				green *= f;
				blue *= f;
				return (*this);
			}

			C4API void GetHexString(char *string) const;
			C4API ColorRGB& SetHexString(const char *string);
	};


	inline ColorRGB operator -(const ColorRGB& c)
	{
		return (ColorRGB(-c.red, -c.green, -c.blue));
	}

	inline ColorRGB operator +(const ColorRGB& c1, const ColorRGB& c2)
	{
		return (ColorRGB(c1.red + c2.red, c1.green + c2.green, c1.blue + c2.blue));
	}

	inline ColorRGB operator -(const ColorRGB& c1, const ColorRGB& c2)
	{
		return (ColorRGB(c1.red - c2.red, c1.green - c2.green, c1.blue - c2.blue));
	}

	inline ColorRGB operator *(const ColorRGB& c1, const ColorRGB& c2)
	{
		return (ColorRGB(c1.red * c2.red, c1.green * c2.green, c1.blue * c2.blue));
	}

	inline ColorRGB operator *(const ColorRGB& c, float t)
	{
		return (ColorRGB(c.red * t, c.green * t, c.blue * t));
	}

	inline ColorRGB operator *(float t, const ColorRGB& c)
	{
		return (ColorRGB(t * c.red, t * c.green, t * c.blue));
	}

	inline ColorRGB operator /(const ColorRGB& c, float t)
	{
		float f = 1.0F / t;
		return (ColorRGB(c.red * f, c.green * f, c.blue * f));
	}

	inline bool operator ==(const ColorRGB& c1, const ColorRGB& c2)
	{
		return ((c1.red == c2.red) && (c1.green == c2.green) && (c1.blue == c2.blue));
	}

	inline bool operator !=(const ColorRGB& c1, const ColorRGB& c2)
	{
		return ((c1.red != c2.red) || (c1.green != c2.green) || (c1.blue != c2.blue));
	}


	//# \class	ColorRGBA	Encapsulates a floating-point RGBA color.
	//
	//# The $ColorRGBA$ class encapsulates a floating-point RGBA color.
	//
	//# \def	class ColorRGBA
	//
	//# \data	ColorRGBA
	//
	//# \ctor	ColorRGBA();
	//# \ctor	ColorRGBA(const ColorRGB& c, float a = 1.0F);
	//# \ctor	ColorRGBA(float r, float g, float b, float a = 1.0F);
	//
	//# \desc
	//# The $ColorRGBA$ class encapsulates a color having floating-point red, green, blue, and
	//# alpha components in the range [0.0,&nbsp;1.0]. When a $@ColorRGB@$ object is converted
	//# to a $ColorRGBA$ object, the alpha component is assumed to be 1.0.
	//#
	//# The default constructor leaves the components of the color undefined. If the values
	//# $r$, $g$, $b$, and $a$ are supplied, then they are assigned to the red, green, blue,
	//# and alpha components of the color, respectively.
	//
	//# \operator	float& operator [](machine k);
	//#				Returns a reference to the $k$-th component of a color.
	//#				The value of $k$ must be 0, 1, 2, or 3.
	//
	//# \operator	const float& operator [](machine k) const;
	//#				Returns a constant reference to the $k$-th component of a color.
	//#				The value of $k$ must be 0, 1, 2, or 3.
	//
	//# \operator	ColorRGBA& operator =(const ColorRGB& c);
	//#				Copies the red, green, and blue components of $c$, and assigns
	//#				the alpha component a value of 1.
	//
	//# \operator	ColorRGBA& operator =(float f);
	//#				Assigns the value $f$ to the red, green, and blue components, and
	//#				assigns the alpha component a value of 1.
	//
	//# \operator	ColorRGBA& operator +=(const ColorRGBA& c);
	//#				Adds the color $c$.
	//
	//# \operator	ColorRGBA& operator +=(const ColorRGB& c);
	//#				Adds the color $c$. The alpha component is not modified.
	//
	//# \operator	ColorRGBA& operator -=(const ColorRGBA& c);
	//#				Subtracts the color $c$.
	//
	//# \operator	ColorRGBA& operator -=(const ColorRGB& c);
	//#				Subtracts the color $c$. The alpha component is not modified.
	//
	//# \operator	ColorRGBA& operator *=(const ColorRGBA& c);
	//#				Multiplies by the color $c$.
	//
	//# \operator	ColorRGBA& operator *=(const ColorRGB& c);
	//#				Multiplies by the color $c$. The alpha component is not modified.
	//
	//# \operator	ColorRGBA& operator *=(float t);
	//#				Multiplies all four components by $t$.
	//
	//# \operator	ColorRGBA& operator /=(float t);
	//#				Divides all four components by $t$.
	//
	//# \action		ColorRGBA operator -(const ColorRGBA& c);
	//#				Returns the negation of the color $c$.
	//
	//# \action		ColorRGBA operator +(const ColorRGBA& c1, const ColorRGBA& c2);
	//#				Returns the sum of the colors $c1$ and $c2$.
	//
	//# \action		ColorRGBA operator +(const ColorRGBA& c1, const ColorRGB& c2);
	//#				Returns the sum of the colors $c1$ and $c2$. The alpha component of $c2$ is assumed to be 0.
	//
	//# \action		ColorRGBA operator -(const ColorRGBA& c1, const ColorRGBA& c2);
	//#				Returns the difference of the colors $c1$ and $c2$.
	//
	//# \action		ColorRGBA operator -(const ColorRGBA& c1, const ColorRGB& c2);
	//#				Returns the difference of the colors $c1$ and $c2$. The alpha component of $c2$ is assumed to be 0.
	//
	//# \action		ColorRGBA operator *(const ColorRGBA& c1, const ColorRGBA& c2);
	//#				Returns the product of the colors $c1$ and $c2$.
	//
	//# \action		ColorRGBA operator *(const ColorRGBA& c1, const ColorRGB& c2);
	//#				Returns the product of the colors $c1$ and $c2$. The alpha component of $c2$ is assumed to be 1.
	//
	//# \action		ColorRGBA operator *(const ColorRGB& c1, const ColorRGBA& c2);
	//#				Returns the product of the colors $c1$ and $c2$. The alpha component of $c1$ is assumed to be 1.
	//
	//# \action		ColorRGBA operator *(const ColorRGBA& c, float t);
	//#				Returns the product of the color $c$ and the scalar $t$.
	//
	//# \action		ColorRGBA operator *(float t, const ColorRGBA& c);
	//#				Returns the product of the color $c$ and the scalar $t$.
	//
	//# \action		ColorRGBA operator /(const ColorRGBA& c, float t);
	//#				Returns the product of the color $c$ and the inverse of the scalar $t$.
	//
	//# \action		bool operator ==(const ColorRGBA& c1, const ColorRGBA& c2);
	//#				Returns a boolean value indicating the equality of the two colors $c1$ and $c2$.
	//
	//# \action		bool operator ==(const ColorRGBA& c1, const ColorRGB& c2);
	//#				Returns a boolean value indicating the equality of the two colors $c1$ and $c2$. The alpha component of $c2$ is assumed to be 1.
	//
	//# \action		bool operator !=(const ColorRGBA& c1, const ColorRGBA& c2);
	//#				Returns a boolean value indicating the inequality of the two colors $c1$ and $c2$.
	//
	//# \action		bool operator !=(const ColorRGBA& c1, const ColorRGB& c2);
	//#				Returns a boolean value indicating the inequality of the two colors $c1$ and $c2$. The alpha component of $c2$ is assumed to be 1.
	//
	//# \also	$@ColorRGB@$


	//# \function	ColorRGBA::Set		Sets all four components of a color.
	//
	//# \proto	ColorRGBA& Set(float r, float g, float b, float a = 1.0F);
	//
	//# \param	r	The new red component.
	//# \param	g	The new green component.
	//# \param	b	The new blue component.
	//# \param	a	The new alpha component.
	//
	//# \desc
	//# The $Set$ function sets the red, green, blue, and alpha components of a color to the values
	//# given by the $r$, $g$, $b$, and $a$ parameters, respectively.
	//#
	//# The return value is a reference to the color object.


	//# \function	ColorRGBA::GetColorRGB		Returns a reference to a $@ColorRGB@$ object.
	//
	//# \proto	ColorRGB& GetColorRGB(void);
	//# \proto	const ColorRGB& GetColorRGB(void) const;
	//
	//# \desc
	//# The $GetColorRGB$ function returns a reference to a $@ColorRGB@$ object that refers to
	//# the same data contained within the $ColorRGBA$ object.


	//# \member		ColorRGBA

	class ColorRGBA
	{
		public:

			float	red;		//## The red component.
			float	green;		//## The green component.
			float	blue;		//## The blue component.
			float	alpha;		//## The alpha component.

			ColorRGBA() = default;

			ColorRGBA(const ColorRGB& c, float a = 1.0F)
			{
				red = c.red;
				green = c.green;
				blue = c.blue;
				alpha = a;
			}

			ColorRGBA(float r, float g, float b, float a = 1.0F)
			{
				red = r;
				green = g;
				blue = b;
				alpha = a;
			}

			ColorRGBA& Set(const ColorRGB& c, float a)
			{
				red = c.red;
				green = c.green;
				blue = c.blue;
				alpha = a;
				return (*this);
			}

			void Set(const ColorRGB& c, float a) volatile
			{
				red = c.red;
				green = c.green;
				blue = c.blue;
				alpha = a;
			}

			ColorRGBA& Set(float r, float g, float b, float a = 1.0F)
			{
				red = r;
				green = g;
				blue = b;
				alpha = a;
				return (*this);
			}

			void Set(float r, float g, float b, float a = 1.0F) volatile
			{
				red = r;
				green = g;
				blue = b;
				alpha = a;
			}

			float& operator [](machine k)
			{
				return ((&red)[k]);
			}

			const float& operator [](machine k) const
			{
				return ((&red)[k]);
			}

			operator const ColorRGB&(void) const
			{
				return (reinterpret_cast<const ColorRGB&>(*this));
			}

			ColorRGB& GetColorRGB(void)
			{
				return (reinterpret_cast<ColorRGB&>(*this));
			}

			const ColorRGB& GetColorRGB(void) const
			{
				return (reinterpret_cast<const ColorRGB&>(*this));
			}

			ColorRGBA& operator =(const ColorRGBA& c)
			{
				red = c.red;
				green = c.green;
				blue = c.blue;
				alpha = c.alpha;
				return (*this);
			}

			void operator =(const ColorRGBA& c) volatile
			{
				red = c.red;
				green = c.green;
				blue = c.blue;
				alpha = c.alpha;
			}

			ColorRGBA& operator =(const ColorRGB& c)
			{
				red = c.red;
				green = c.green;
				blue = c.blue;
				alpha = 1.0F;
				return (*this);
			}

			void operator =(const ColorRGB& c) volatile
			{
				red = c.red;
				green = c.green;
				blue = c.blue;
				alpha = 1.0F;
			}

			ColorRGBA& operator =(float f)
			{
				red = f;
				green = f;
				blue = f;
				alpha = 1.0F;
				return (*this);
			}

			void operator =(float f) volatile
			{
				red = f;
				green = f;
				blue = f;
				alpha = 1.0F;
			}

			ColorRGBA& operator +=(const ColorRGBA& c)
			{
				red += c.red;
				green += c.green;
				blue += c.blue;
				alpha += c.alpha;
				return (*this);
			}

			ColorRGBA& operator +=(const ColorRGB& c)
			{
				red += c.red;
				green += c.green;
				blue += c.blue;
				return (*this);
			}

			ColorRGBA& operator -=(const ColorRGBA& c)
			{
				red -= c.red;
				green -= c.green;
				blue -= c.blue;
				alpha -= c.alpha;
				return (*this);
			}

			ColorRGBA& operator -=(const ColorRGB& c)
			{
				red -= c.red;
				green -= c.green;
				blue -= c.blue;
				return (*this);
			}

			ColorRGBA& operator *=(const ColorRGBA& c)
			{
				red *= c.red;
				green *= c.green;
				blue *= c.blue;
				alpha *= c.alpha;
				return (*this);
			}

			ColorRGBA& operator *=(const ColorRGB& c)
			{
				red *= c.red;
				green *= c.green;
				blue *= c.blue;
				return (*this);
			}

			ColorRGBA& operator *=(float t)
			{
				red *= t;
				green *= t;
				blue *= t;
				alpha *= t;
				return (*this);
			}

			ColorRGBA& operator /=(float t)
			{
				float f = 1.0F / t;
				red *= f;
				green *= f;
				blue *= f;
				alpha *= f;
				return (*this);
			}

			C4API void GetHexString(char *string) const;
			C4API ColorRGBA& SetHexString(const char *string);
	};


	inline ColorRGBA operator -(const ColorRGBA& c)
	{
		return (ColorRGBA(-c.red, -c.green, -c.blue, -c.alpha));
	}

	inline ColorRGBA operator +(const ColorRGBA& c1, const ColorRGBA& c2)
	{
		return (ColorRGBA(c1.red + c2.red, c1.green + c2.green, c1.blue + c2.blue, c1.alpha + c2.alpha));
	}

	inline ColorRGBA operator +(const ColorRGBA& c1, const ColorRGB& c2)
	{
		return (ColorRGBA(c1.red + c2.red, c1.green + c2.green, c1.blue + c2.blue, c1.alpha));
	}

	inline ColorRGBA operator -(const ColorRGBA& c1, const ColorRGBA& c2)
	{
		return (ColorRGBA(c1.red - c2.red, c1.green - c2.green, c1.blue - c2.blue, c1.alpha - c2.alpha));
	}

	inline ColorRGBA operator -(const ColorRGBA& c1, const ColorRGB& c2)
	{
		return (ColorRGBA(c1.red - c2.red, c1.green - c2.green, c1.blue - c2.blue, c1.alpha));
	}

	inline ColorRGBA operator *(const ColorRGBA& c1, const ColorRGBA& c2)
	{
		return (ColorRGBA(c1.red * c2.red, c1.green * c2.green, c1.blue * c2.blue, c1.alpha * c2.alpha));
	}

	inline ColorRGBA operator *(const ColorRGBA& c1, const ColorRGB& c2)
	{
		return (ColorRGBA(c1.red * c2.red, c1.green * c2.green, c1.blue * c2.blue, c1.alpha));
	}

	inline ColorRGBA operator *(const ColorRGB& c1, const ColorRGBA& c2)
	{
		return (ColorRGBA(c1.red * c2.red, c1.green * c2.green, c1.blue * c2.blue, c2.alpha));
	}

	inline ColorRGBA operator *(const ColorRGBA& c, float t)
	{
		return (ColorRGBA(c.red * t, c.green * t, c.blue * t, c.alpha * t));
	}

	inline ColorRGBA operator *(float t, const ColorRGBA& c)
	{
		return (ColorRGBA(t * c.red, t * c.green, t * c.blue, t * c.alpha));
	}

	inline ColorRGBA operator /(const ColorRGBA& c, float t)
	{
		float f = 1.0F / t;
		return (ColorRGBA(c.red * f, c.green * f, c.blue * f, c.alpha * f));
	}

	inline bool operator ==(const ColorRGBA& c1, const ColorRGBA& c2)
	{
		return ((c1.red == c2.red) && (c1.green == c2.green) && (c1.blue == c2.blue) && (c1.alpha == c2.alpha));
	}

	inline bool operator ==(const ColorRGBA& c1, const ColorRGB& c2)
	{
		return ((c1.red == c2.red) && (c1.green == c2.green) && (c1.blue == c2.blue) && (c1.alpha == 1.0F));
	}

	inline bool operator !=(const ColorRGBA& c1, const ColorRGBA& c2)
	{
		return ((c1.red != c2.red) || (c1.green != c2.green) || (c1.blue != c2.blue) || (c1.alpha != c2.alpha));
	}

	inline bool operator !=(const ColorRGBA& c1, const ColorRGB& c2)
	{
		return ((c1.red != c2.red) || (c1.green != c2.green) || (c1.blue != c2.blue) || (c1.alpha != 1.0F));
	}


	struct ConstColorRGB
	{
		float	red;
		float	green;
		float	blue;

		operator const ColorRGB&(void) const
		{
			return (reinterpret_cast<const ColorRGB&>(*this));
		}

		const ColorRGB *operator &(void) const
		{
			return (reinterpret_cast<const ColorRGB *>(this));
		}

		const ColorRGB *operator ->(void) const
		{
			return (reinterpret_cast<const ColorRGB *>(this));
		}
	};


	struct ConstColorRGBA
	{
		float	red;
		float	green;
		float	blue;
		float	alpha;

		operator const ColorRGBA&(void) const
		{
			return (reinterpret_cast<const ColorRGBA&>(*this));
		}

		const ColorRGBA *operator &(void) const
		{
			return (reinterpret_cast<const ColorRGBA *>(this));
		}

		const ColorRGBA *operator ->(void) const
		{
			return (reinterpret_cast<const ColorRGBA *>(this));
		}
	};
}


#endif

// ZYUQURM
