 

#ifndef C4Image_h
#define C4Image_h


#include "C4Resources.h"
#include "C4Bounding.h"


namespace C4
{
	typedef unsigned_int8	Color1C;


	enum
	{
		kMipmapNormalize		= 1 << 0,
		kMipmapBoostAlpha		= 1 << 1,
		kMipmapDampenAlpha		= 1 << 2
	};


	//# \class	Color2C		Encapsulates an integer two-component color.
	//
	//# The $Color2C$ class encapsulates an integer two-component color.
	//
	//# \def	class Color2C
	//
	//# \ctor	Color2C();
	//# \ctor	Color2C(unsigned_int32 x, unsigned_int32 y);
	//
	//# \desc
	//# The $Color2C$ class encapsulates a color having two integer
	//# components in the range [0,&nbsp;255].
	//#
	//# The default constructor leaves the components of the color undefined.
	//
	//# \also	$@Color3C@$
	//# \also	$@Color4C@$


	class Color2C
	{
		private:

			union
			{
				unsigned_int16		color;
				unsigned_int8		channel[2];
			};

		public:

			Color2C() = default;

			Color2C(unsigned_int32 x, unsigned_int32 y)
			{
				channel[0] = (unsigned_int8) x;
				channel[1] = (unsigned_int8) y;
			}

			bool operator ==(const Color2C& c) const
			{
				return (color == c.color);
			}

			bool operator !=(const Color2C& c) const
			{
				return (color != c.color);
			}

			Color2C& Set(unsigned_int32 x, unsigned_int32 y)
			{
				channel[0] = (unsigned_int8) x;
				channel[1] = (unsigned_int8) y;
				return (*this);
			}

			Color2C& Clear(void)
			{
				color = 0;
				return (*this);
			}

			unsigned_int16 GetPackedColor(void) const
			{
				return (color);
			}

			Color2C& SetPackedColor(unsigned_int16 c)
			{
				color = c;
				return (*this);
			}

			unsigned_int32 GetRed(void) const
			{
				return (channel[0]);
			}

			Color2C& SetRed(unsigned_int32 red)
			{ 
				channel[0] = (unsigned_int8) red;
				return (*this);
			} 

			unsigned_int32 GetGreen(void) const 
			{
				return (channel[1]);
			} 

			Color2C& SetGreen(unsigned_int32 green) 
			{ 
				channel[1] = (unsigned_int8) green;
				return (*this);
			}
 
			unsigned_int32 GetLum(void) const
			{
				return (channel[0]);
			}

			Color2C& SetLum(unsigned_int32 lum)
			{
				channel[0] = (unsigned_int8) lum;
				return (*this);
			}

			unsigned_int32 GetAlpha(void) const
			{
				return (channel[1]);
			}

			Color2C& SetAlpha(unsigned_int32 alpha)
			{
				channel[1] = (unsigned_int8) alpha;
				return (*this);
			}

			const unsigned_int8 *GetChannels(void) const
			{
				return (channel);
			}
	};


	//# \class	Color3C		Encapsulates an integer RGB color.
	//
	//# The $Color3C$ class encapsulates an integer RGB color.
	//
	//# \def	class Color3C
	//
	//# \ctor	Color3C();
	//# \ctor	Color3C(unsigned_int32 red, unsigned_int32 green, unsigned_int32 blue);
	//
	//# \desc
	//# The $Color3C$ class encapsulates a color having integer red, green, and blue
	//# components in the range [0,&nbsp;255].
	//#
	//# The default constructor leaves the components of the color undefined.
	//
	//# \also	$@Color2C@$
	//# \also	$@Color4C@$


	class Color3C
	{
		private:

			union
			{
				unsigned_int32		color;
				unsigned_int8		channel[4];
			};

		public:

			Color3C() = default;

			Color3C(unsigned_int32 red, unsigned_int32 green, unsigned_int32 blue)
			{
				channel[0] = (unsigned_int8) red;
				channel[1] = (unsigned_int8) green;
				channel[2] = (unsigned_int8) blue;
				channel[3] = 0;
			}

			bool operator ==(const Color3C& c) const
			{
				return (color == c.color);
			}

			bool operator !=(const Color3C& c) const
			{
				return (color != c.color);
			}

			Color3C& Set(unsigned_int32 red, unsigned_int32 green, unsigned_int32 blue)
			{
				channel[0] = (unsigned_int8) red;
				channel[1] = (unsigned_int8) green;
				channel[2] = (unsigned_int8) blue;
				channel[3] = 0;
				return (*this);
			}

			Color3C& Clear(void)
			{
				color = 0;
				return (*this);
			}

			unsigned_int32 GetPackedColor(void) const
			{
				return (color);
			}

			Color3C& SetPackedColor(unsigned_int32 c)
			{
				#if C4BIGENDIAN

					color = c & 0xFFFFFF00;

				#else

					color = c & 0x00FFFFFF;

				#endif

				return (*this);
			}

			unsigned_int32 GetRed(void) const
			{
				return (channel[0]);
			}

			Color3C& SetRed(unsigned_int32 red)
			{
				channel[0] = (unsigned_int8) red;
				return (*this);
			}

			unsigned_int32 GetGreen(void) const
			{
				return (channel[1]);
			}

			Color3C& SetGreen(unsigned_int32 green)
			{
				channel[1] = (unsigned_int8) green;
				return (*this);
			}

			unsigned_int32 GetBlue(void) const
			{
				return (channel[2]);
			}

			Color3C& SetBlue(unsigned_int32 blue)
			{
				channel[2] = (unsigned_int8) blue;
				return (*this);
			}

			const unsigned_int8 *GetChannels(void) const
			{
				return (channel);
			}
	};


	//# \class	Color4C		Encapsulates an integer RGBA color.
	//
	//# The $Color4C$ class encapsulates an integer RGBA color.
	//
	//# \def	class Color4C
	//
	//# \ctor	Color4C();
	//# \ctor	Color4C(unsigned_int32 red, unsigned_int32 green, unsigned_int32 blue, unsigned_int32 alpha);
	//# \ctor	Color4C(const Color3C& c, unsigned_int32 alpha);
	//# \ctor	Color4C(const ColorRGBA& c);
	//
	//# \desc
	//# The $Color4C$ class encapsulates a color having integer red, green, blue, and alpha
	//# components in the range [0,&nbsp;255].
	//#
	//# The default constructor leaves the components of the color undefined.
	//
	//# \also	$@Color2C@$
	//# \also	$@Color3C@$


	class Color4C
	{
		private:

			union
			{
				unsigned_int32		color;
				unsigned_int8		channel[4];
			};

		public:

			Color4C() = default;

			Color4C(unsigned_int32 red, unsigned_int32 green, unsigned_int32 blue, unsigned_int32 alpha)
			{
				channel[0] = (unsigned_int8) red;
				channel[1] = (unsigned_int8) green;
				channel[2] = (unsigned_int8) blue;
				channel[3] = (unsigned_int8) alpha;
			}

			Color4C(const Color3C& c, unsigned_int32 alpha)
			{
				#if C4BIGENDIAN

					color = c.GetPackedColor() | alpha;

				#else

					color = (alpha << 24) | c.GetPackedColor();

				#endif
			}

			Color4C(const ColorRGBA& c)
			{
				channel[0] = (unsigned_int8) (c.red * 255.0F);
				channel[1] = (unsigned_int8) (c.green * 255.0F);
				channel[2] = (unsigned_int8) (c.blue * 255.0F);
				channel[3] = (unsigned_int8) (c.alpha * 255.0F);
			}

			Color4C& operator =(const Color3C& c)
			{
				color = c.GetPackedColor();
				return (*this);
			}

			Color4C& operator =(const Color4C& c)
			{
				color = c.color;
				return (*this);
			}

			void operator =(const Color4C& c) volatile
			{
				color = c.color;
			}

			Color4C& operator =(const ColorRGB& c)
			{
				channel[0] = (unsigned_int8) (c.red * 255.0F);
				channel[1] = (unsigned_int8) (c.green * 255.0F);
				channel[2] = (unsigned_int8) (c.blue * 255.0F);
				channel[3] = 0xFF;
				return (*this);
			}

			Color4C& operator =(const ColorRGBA& c)
			{
				channel[0] = (unsigned_int8) (c.red * 255.0F);
				channel[1] = (unsigned_int8) (c.green * 255.0F);
				channel[2] = (unsigned_int8) (c.blue * 255.0F);
				channel[3] = (unsigned_int8) (c.alpha * 255.0F);
				return (*this);
			}

			bool operator ==(const Color4C& c) const
			{
				return (color == c.color);
			}

			bool operator !=(const Color4C& c) const
			{
				return (color != c.color);
			}

			Color4C& Set(unsigned_int32 red, unsigned_int32 green, unsigned_int32 blue, unsigned_int32 alpha)
			{
				channel[0] = (unsigned_int8) red;
				channel[1] = (unsigned_int8) green;
				channel[2] = (unsigned_int8) blue;
				channel[3] = (unsigned_int8) alpha;
				return (*this);
			}

			void Set(unsigned_int32 red, unsigned_int32 green, unsigned_int32 blue, unsigned_int32 alpha) volatile
			{
				channel[0] = (unsigned_int8) red;
				channel[1] = (unsigned_int8) green;
				channel[2] = (unsigned_int8) blue;
				channel[3] = (unsigned_int8) alpha;
			}

			Color4C& Set(const Color3C& c, unsigned_int32 alpha)
			{
				#if C4BIGENDIAN

					color = c.GetPackedColor() | alpha;

				#else

					color = (alpha << 24) | c.GetPackedColor();

				#endif

				return (*this);
			}

			void Set(const Color3C& c, unsigned_int32 alpha) volatile
			{
				#if C4BIGENDIAN

					color = c.GetPackedColor() | alpha;

				#else

					color = (alpha << 24) | c.GetPackedColor();

				#endif
			}

			Color4C& Set(const ColorRGBA& c)
			{
				channel[0] = (unsigned_int8) (c.red * 255.0F);
				channel[1] = (unsigned_int8) (c.green * 255.0F);
				channel[2] = (unsigned_int8) (c.blue * 255.0F);
				channel[3] = (unsigned_int8) (c.alpha * 255.0F);
				return (*this);
			}

			void Set(const ColorRGBA& c) volatile
			{
				channel[0] = (unsigned_int8) (c.red * 255.0F);
				channel[1] = (unsigned_int8) (c.green * 255.0F);
				channel[2] = (unsigned_int8) (c.blue * 255.0F);
				channel[3] = (unsigned_int8) (c.alpha * 255.0F);
			}

			Color4C& Clear(void)
			{
				color = 0;
				return (*this);
			}

			Color4C& ClearMaxAlpha(void)
			{
				#if C4BIGENDIAN

					color = 0x000000FF;

				#else

					color = 0xFF000000;

				#endif

				return (*this);
			}

			unsigned_int32 GetPackedColor(void) const
			{
				return (color);
			}

			unsigned_int32 GetPackedRGBColor(void) const
			{
				#if C4BIGENDIAN

					return (color & 0xFFFFFF00);

				#else

					return (color & 0x00FFFFFF);

				#endif
			}

			Color4C& SetPackedColor(unsigned_int32 c)
			{
				color = c;
				return (*this);
			}

			unsigned_int32 GetRed(void) const
			{
				return (channel[0]);
			}

			Color4C& SetRed(unsigned_int32 red)
			{
				channel[0] = (unsigned_int8) red;
				return (*this);
			}

			unsigned_int32 GetGreen(void) const
			{
				return (channel[1]);
			}

			Color4C& SetGreen(unsigned_int32 green)
			{
				channel[1] = (unsigned_int8) green;
				return (*this);
			}

			unsigned_int32 GetBlue(void) const
			{
				return (channel[2]);
			}

			Color4C& SetBlue(unsigned_int32 blue)
			{
				channel[2] = (unsigned_int8) blue;
				return (*this);
			}

			unsigned_int32 GetAlpha(void) const
			{
				return (channel[3]);
			}

			Color4C& SetAlpha(unsigned_int32 alpha)
			{
				channel[3] = (unsigned_int8) alpha;
				return (*this);
			}

			Color3C GetColor3C(void) const
			{
				return (Color3C().SetPackedColor(color));
			}

			const unsigned_int8 *GetChannels(void) const
			{
				return (channel);
			}
	};


	struct ConstColor4C
	{
		unsigned_int8	red;
		unsigned_int8	green;
		unsigned_int8	blue;
		unsigned_int8	alpha;

		operator const Color4C&(void) const
		{
			return (reinterpret_cast<const Color4C&>(*this));
		}

		const Color4C *operator &(void) const
		{
			return (reinterpret_cast<const Color4C *>(this));
		}

		const Color4C *operator ->(void) const
		{
			return (reinterpret_cast<const Color4C *>(this));
		}
	};


	struct BC1Block
	{
		union
		{
			unsigned_int8	byteData[8];
			unsigned_int32	wordData[2];
		};

		BC1Block& operator =(const BC1Block& block)
		{
			wordData[0] = block.wordData[0];
			wordData[1] = block.wordData[1];
			return (*this);
		}

		bool operator ==(const BC1Block& block) const
		{
			return ((block.wordData[0] == wordData[0]) && (block.wordData[1] == wordData[1]));
		}

		bool operator !=(const BC1Block& block) const
		{
			return ((block.wordData[0] != wordData[0]) || (block.wordData[1] != wordData[1]));
		}
	};


	struct BC3Block
	{
		union
		{
			unsigned_int8	byteData[16];
			unsigned_int32	wordData[4];
		};

		BC3Block& operator =(const BC3Block& block)
		{
			wordData[0] = block.wordData[0];
			wordData[1] = block.wordData[1];
			wordData[2] = block.wordData[2];
			wordData[3] = block.wordData[3];
			return (*this);
		}

		bool operator ==(const BC3Block& block) const
		{
			return ((block.wordData[0] == wordData[0]) && (block.wordData[1] == wordData[1]) && (block.wordData[2] == wordData[2]) && (block.wordData[3] == wordData[3]));
		}

		bool operator !=(const BC3Block& block) const
		{
			return ((block.wordData[0] != wordData[0]) || (block.wordData[1] != wordData[1]) || (block.wordData[2] != wordData[2]) || (block.wordData[3] != wordData[3]));
		}
	};


	struct Image
	{
		private:

			static Vector3D CalculateColorAxis(int32 count, const Point3D *color, const Box3D& bounds);
			static void CalculateEndpointCandidates(int32 count, const Point3D *color, int32 *restrict candCount1, int32 *restrict candCount2, unsigned_int16 *restrict cand1, unsigned_int16 *restrict cand2);
			static void CalculateEndpointCandidates(int32 count, const float *green, int32 *restrict candCount1, int32 *restrict candCount2, unsigned_int16 *restrict cand1, unsigned_int16 *restrict cand2);
			static float EncodeColorBlock(int32 width, int32 height, unsigned_int16 color0, unsigned_int16 color1, bool black, const Point3D *image, unsigned_int8 *restrict data);
			static float EncodeGreenBlock(int32 width, int32 height, unsigned_int16 color0, unsigned_int16 color1, const float *image, unsigned_int8 *restrict data);
			static float EncodeGrayBlock(int32 width, int32 height, unsigned_int8 gray0, unsigned_int8 gray1, bool black, const float *image, unsigned_int8 *restrict data);

		public:

			C4API static void DecompressImageRLE_RGBA32(const unsigned_int8 *code, unsigned_int32 codeSize, void *restrict output);
			C4API static void DecompressImageRLE_XY16(const unsigned_int8 *code, unsigned_int32 codeSize, void *restrict output);
			C4API static void DecompressImageRLE_X8(const unsigned_int8 *code, unsigned_int32 codeSize, void *restrict output);
			C4API static void DecompressImageRLE_DEPTH16(const unsigned_int8 *code, unsigned_int32 codeSize, void *restrict output);
			C4API static void DecompressImageRLE_BC1(const unsigned_int8 *code, unsigned_int32 codeSize, void *restrict output);
			C4API static void DecompressImageRLE_BC3(const unsigned_int8 *code, unsigned_int32 codeSize, void *restrict output);

			C4API static void CompressColorBlock(int32 width, int32 height, int32 rowLength, bool alpha, const Color4C *image, unsigned_int8 *restrict data);
			C4API static void CompressGreenBlock(int32 width, int32 height, int32 rowLength, const Color4C *image, unsigned_int8 *restrict data);
			C4API static void CompressGrayBlock(int32 width, int32 height, int32 rowLength, const unsigned_int8 *image, unsigned_int8 *restrict data);

			C4API static void BleedAlphaTestMap(int32 width, int32 height, const Rect& rect, const Color4C *source, Color4C *restrict destin, unsigned_int32 testValue);
			C4API static void BleedNormalMap(int32 width, int32 height, const Rect& rect, const Color4C *source, Color4C *restrict destin);

			C4API static void CalculateNormalMap(int32 width, int32 height, const Color4C *source, Color4C *restrict destin, float scale, bool swrap = true, bool twrap = true);
			C4API static void CalculateXYNormalMap(int32 width, int32 height, const Color4C *source, Color4C *restrict destin, float scale, bool swrap = true, bool twrap = true);
			C4API static void CalculateParallaxMap(int32 width, int32 height, const Color4C *source, Color4C *restrict destin, float scale, bool swrap = true, bool twrap = true);
			C4API static void CalculateHorizonMap(int32 index, int32 width, int32 height, const Rect& rect, const Color4C *source, Color4C *restrict destin, float scale, bool swrap = true, bool twrap = true);
			C4API static void CalculateAmbientOcclusionChannel(int32 width, int32 height, const Rect& rect, const Color4C *source, Color4C *restrict destin, float scale, bool swrap = true, bool twrap = true);

			C4API static int32 CalculateMipmapChainSize2D(int32 width, int32 height, int32 *pixelCount);
			C4API static int32 CalculateBlockMipmapChainSize2D(int32 width, int32 height, int32 levelCount);
			C4API static int32 CalculateMipmapChainSize3D(int32 width, int32 height, int32 depth, int32 *pixelCount);

			C4API static void GenerateMipmaps2D(int32 count, int32 width, int32 height, Color4C *image, unsigned_int32 flags = 0);
			C4API static void GenerateMipmaps2D(int32 count, int32 width, int32 height, Color2C *image);
			C4API static void GenerateMipmaps2D(int32 count, int32 width, int32 height, Color1C *image);

			C4API static void GenerateMipmaps3D(int32 width, int32 height, int32 depth, Color4C *image);
			C4API static void GenerateMipmaps3D(int32 width, int32 height, int32 depth, Color2C *image);
			C4API static void GenerateMipmaps3D(int32 width, int32 height, int32 depth, Color1C *image);
	};
}


#endif

// ZYUQURM
