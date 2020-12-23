#pragma once
#pragma warning(disable:4290)
#pragma warning(disable:4996)

#include "../Leadwerks.h"

namespace Leadwerks
{
	class Texture;
	/*
    //Texture targets
    #define TEXTURE_1D 1
    #define TEXTURE_2D 2
    #define TEXTURE_3D 3
    #define TEXTURE_CUBEMAP 4

    //Texture pixel formats
    #define TEXTURE_RGBA8 1
    #define TEXTURE_RGBA16 2
    #define TEXTURE_RGBA32 3
    #define TEXTURE_RGBA_DXTC1 4
    #define TEXTURE_RGBA_DXTC3 5
    #define TEXTURE_RGBA_DXTC5 6
    #define TEXTURE_RGBA_DXT5n 20
    #define TEXTURE_RGBA TEXTURE_RGBA8
    #define TEXTURE_RGB 7
    #define TEXTURE_RGB_DXTC1 8
    #define TEXTURE_BGR 10
    #define TEXTURE_BGRA 11
    #define TEXTURE_DEPTH 12
    #define TEXTURE_DEPTH32 13
    #define TEXTURE_DEPTH24 TEXTURE_DEPTH
    #define TEXTURE_FLOAT16 14
    #define TEXTURE_FLOAT32 15
    #define TEXTURE_FLOAT TEXTURE_FLOAT32
    #define TEXTURE_SHADOW 16
    #define TEXTURE_RGB_PACKED 17
    #define TEXTURE_ALPHA 18
    #define TEXTURE_INTENSITY 19
    #define TEXTURE_3Dc 21

    //Texture creation flags
    #define TEXTURE_UNIQUE 1
    #define TEXTURE_MIPMAPS 2
    #define TEXTURE_UNMANAGED 4

    //Texture filter modes
    #define TEXTURE_FILTER_PIXEL 0
    #define TEXTURE_FILTER_SMOOTH 1

    //Texture load flags
    #define TEXTURE_NO_BACKGROUND_LOAD 1

    //Texture slots
    #define TEXTURE_DIFFUSE 0
    #define TEXTURE_NORMAL 1
    #define TEXTURE_SPECULAR 2
    #define TEXTURE_DISPLACEMENT 6
    #define TEXTURE_REFLECTION 4
    #define TEXTURE_EMISSION 5
    #define TEXTURE_LIGHT 3
    #define TEXTURE_AMBIENT 7
    #define TEXTURE_REFRACTION 8
    */

	class Texture : public Asset//lua
	{
	public:

		//Texture targets
		static const int Texture1D;
		static const int Texture2D;//lua
		static const int Texture3D;
		static const int Cubemap;//lua

		//Texture pixel formats
		static const int RGBA8;//lua
		static const int RGBA16;
		static const int RGBA32;
		static const int RGBADXTC1;
		static const int RGBADXTC3;
		static const int RGBADXTC5;
		static const int RGBADXT5n;
		static const int RGBA;//lua
		static const int RGB;//lua
		static const int RGBDXTC1;
		static const int BGR;
		static const int BGRA;
		static const int Depth;//lua
		static const int Depth32;
		static const int Depth24;
		static const int Float16;
		static const int Float32;
		static const int Float;
		static const int Intensity32UI;
		static const int Shadow;
		static const int RGBPacked;
		static const int Alpha;//lua
		static const int Intensity;//lua
		static const int Intensity16;//lua
		static const int Intensity32;//lua
		static const int EditableHeightmap;//lua
		static const int RG;//lua

		//Texture creation flags
		static const int Unique;
		static const int Mipmaps;
		static const int Unmanaged;

		//Texture filter modes
		static const int Pixel;//lua
		static const int Smooth;//lua

		//Texture load flags
		static const int NoBackgroundLoad;

		//Texture slots
		static const int Diffuse;//lua
		static const int Normal;//lua
		static const int Specular;//lua
		static const int Displacement;//lua
		static const int Reflection;//lua
		static const int Emission;//lua
		static const int Lightmap;//lua
		static const int Ambient;//lua
		static const int Refraction;//lua

		//Loading mode
		static const int Instant;//lua
		static const int Deferred;//lua
		static const int Managed;//lua

		//Attributes
		int filtermode;
		int format;
		int target;
		bool hasmipmaps;
		static float anisotropy;
		int bindindex;
		int width;
		int height;
		int depth;
		bool clamp[3];
		int currentmiplevel;
		int samples;
		Stream* stream;
		int vidmemusage;
		int mapdataposition;
		std::vector<int> memusage;
		//vector<Frame*> frame;
		static bool trilinearfiltermode;
		long lastbindtime;
		std::list<Texture*>::iterator it;
		unsigned long renderiterator;
		int buffercount;

		//Constructors and destructor
		Texture();
		virtual ~Texture();

		//Methods
		/*virtual bool Locked(const int miplevel=0, const int framenumber=0)=0;
		virtual void Lock(const int miplevel=0, const int framenumber=0)=0;
		virtual void Unlock(const int miplevel=0, const int framenumber=0)=0;
		virtual void WritePixel(const int x, const int y, const unsigned char r, const unsigned char g, const unsigned char b, const unsigned char a=255, const int miplevel=0, const int framenumber=0)=0;
		virtual int ReadPixel(const int x, const int y, const int miplevel=0, const int framenumber=0)=0;
		*/
		virtual int CountFaces();
		//virtual bool Save(const std::string& path);

		//virtual void Print_();
		virtual void GetPixels(const char* buf, const int miplevel=0, const int framenumber=0, const int cubeface=0)=0;//lua
		virtual void SetPixels(const char* buf, const int miplevel=0, const int framenumber=0, const int cubeface=0)=0;//lua

		virtual void WritePixel(const int x, const int y, const char r, const char g, const char b, const char a, const int miplevel, const int framenumber)=0;
		virtual void WritePixel(const int x, const int y, const unsigned short luminosity, const int miplevel, const int framenumber)=0;

		//virtual void WritePixel(const char r,const char g,const char b,const char a)=0;
		//virtual void WritePixel(const float i)=0;
		//virtual void WritePixel(const float r,const float g,const float b,const float a)=0;

		//virtual bool GenerateMipmaps(Bank* pixels);
		virtual void ContinueLoading();
		virtual void FinishLoading();
		virtual void DumpMipmaps();

		virtual void Reset();
		virtual int CountMipmaps();//lua
		virtual int GetMultisampleMode();//lua
		virtual void ReadMipmap(const int miplevel);
		virtual int GetMipmapSize(const int miplevel);//lua
		virtual int GetMipmapWidth(const int miplevel);
		virtual int GetMipmapHeight(const int miplevel);
		virtual int GetMipmapDepth(const int miplevel);
        virtual int CountFrames();
		virtual bool Initialize(const int width, const int height, const int format, const int flags, const int frames, const int samples=0)=0;
		virtual int GetWidth(const int miplevel=0);//lua
		virtual int GetHeight(const int miplevel=0);//lua
		virtual int GetDepth(const int miplevel=0);//lua
		virtual void ReleaseMipMap(const int miplevel)=0;
        //virtual void ReadPixels(const char* pixels, const int miplevel=0, const int framenumber=0)=0;
		//virtual void WritePixels(const char* pixels, const int miplevel=0, const int framenumber=0)=0;
		//virtual Bank* Lock(const int miplevel=0, const int framenumber=0)=0;
		//virtual void Unlock(const int miplevel=0, const int framenumber=0)=0;
		//virtual void WritePixel(const int x, const int y, const char r, const char g, const char b, const char a, const int miplevel=0, const int framenumber=0)=0;
		//virtual void ReadPixel(const int x, const int y, char& r, char& g, char& b, char& a, const int miplevel=0, const int framenumber=0)=0;
		//virtual void WritePixels(const char* pixels, const int miplevel=0, const int framenumber=0, const int x=0, const int y=0, const int width=0, const int height=0)=0;
		//virtual void ReadPixels(const char* pixels, const int miplevel=0, const int framenumber=0, const int x=0, const int y=0, const int width=0, const int height=0)=0;
		virtual void SetFilter(const int filtermode);//lua
		virtual int GetDataSize(const int width, const int height, const int depth, const int format);
		virtual bool Reload(const int flags=0);
		virtual void Bind(const int index=0)=0;//lua
		//virtual Bank* Lock(miplevel,framenumber);
		//virtual void Unlock(miplevel,framenumber);

        //Constructor/destructor

        //Procedures
        virtual int GetTarget();
        virtual int GetFormat();//lua
        virtual void SetClampMode(const bool x, const bool y=false, const bool z=false);//lua
        //virtual bool Locked(const int miplevel=0, const int framenumber=0);
        virtual bool GetClampMode(const int axis=0);//lua
        static void SetAnisotropy(const float anisotropy);//lua
		static float GetAnisotropy();//lua
		static float GetMaxAnisotropy();//lua
        virtual int GetFilter();//lua
		static void SetTrilinearFilterMode(const bool trilinearfilter);//lua
		static bool GetTrilinearFilterMode();//lua
        /*virtual void WritePixel(const int x, const int y, const unsigned char r, const unsigned char g, const unsigned char b, const unsigned char a=255, const int miplevel=0, const int framenumber=0);
         virtual int ReadPixel(const int x, const int y, const int miplevel=0, const int framenumber=0);
         virtual void Lock(const int miplevel=0, const int framenumber=0);
         virtual void Unlock(const int miplevel=0, const int framenumber=0);*/
        virtual void Draw(const int x, const int y, const int width=0, const int height=0);//lua
        virtual bool BuildMipmaps()=0;

		//static bool AllowMipmapLoad;

        static Texture* Create(const int width, const int height, const int format=Texture::RGBA, const int flags=0, const int frames=1, const int samples=0);//lua
		static Texture* Load(const std::string& path, const int flags = 0, const uint64_t fileid=0);//lua
		static Texture* CubeMap(const int width, const int height, const int format=Texture::RGBA, const int flags=0, const int frames=1, const int samples=0);//lua

		static void SetLoadingMode(const int mode);//lua

		virtual std::string GetClassName();

		static void UpdateManager();
		static bool CompareTime(Texture* first, Texture* second);
		static std::list<Texture*> List;
		static int MinAsyncMipLoadLevel;
		static int DeferredLoadingMode;
		static Bank* MipmapLoadBuffer;
		static int Detail;
	};

	/*
	class IDDSInfo
	{
	public:
		int dwSize;
		int dwFlags;
		int dwHeight;
		int dwWidth;
		int lPitch;
		int dwDepth;
		int dwMipmapCount;
		int dwAlphaBitDepth;
		int dwReserved;
		int lpSurface;
		long long dddckCKDestOverlay;
		long long ddckCKDestBlt;
		long long ddckCKSrcOverlay;
		long long ddckCKSrcBlt;
		int ddpf_dwSize;
		int ddpf_dwFlags;
		int ddpf_dwFourCC;
		int ddpf_BitCount;
		int ddpf_BitMask_0;
		int ddpf_BitMask_1;
		int ddpf_BitMask_2;
		int ddpf_BitMask_3;
		int ddsCaps1;
		int ddsCaps2;
		int ddsCaps3;
		int ddsCaps4;
		int dwTextureStage;
	};
	*/

	class DownsampleMipmapJob : public Object
	{
	public:
		int x,y,width,height,format,sectionwidth,sectionheight;
		Bank* pixels[2];
		bool finished;
	};

	Object* DownsampleMipmap(Object* o);

	//extern map<std::string,Texture*> LoadedTexturesMap__;
	extern Texture* BoundTexture__[32];
}
