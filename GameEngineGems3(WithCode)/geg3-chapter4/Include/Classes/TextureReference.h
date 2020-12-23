#pragma once
#pragma warning(disable:4290)
#pragma warning(disable:4996)

#include "../Leadwerks.h"

namespace Leadwerks
{
	class Texture;
	//class Frame;
	//class Mipmap;

	class TextureReference : public AssetReference
	{
	public:
		
		//Attributes	
		int filtermode;
		int format;
		int target;
		bool hasmipmaps;
		float anisotropy;
		int bindindex;
		int width;
		int height;
		int depth;
		bool clamp[3];
		int currentmiplevel;
		int samples;
		Stream* stream;
		int vidmemusage;
		//vector<Frame*> frame;
		
		//Constructors and destructor
		TextureReference();
		virtual ~TextureReference();
		
		//Methods
		/*virtual bool Locked(const int miplevel=0, const int framenumber=0)=0;
		virtual void Lock(const int miplevel=0, const int framenumber=0)=0;
		virtual void Unlock(const int miplevel=0, const int framenumber=0)=0;
		virtual void WritePixel(const int x, const int y, const unsigned char r, const unsigned char g, const unsigned char b, const unsigned char a=255, const int miplevel=0, const int framenumber=0)=0;
		virtual int ReadPixel(const int x, const int y, const int miplevel=0, const int framenumber=0)=0;
		*/
		virtual int CountFaces();
		virtual bool Save(const std::string& path);
		
		//virtual void Print_();

		virtual void GetPixels(const char* buf, const int miplevel=0, const int framenumber=0, const int cubeface=0)=0;
		virtual void SetPixels(const char* buf, const int miplevel=0, const int framenumber=0, const int cubeface=0)=0;
		
		//virtual void WritePixel(const char r,const char g,const char b,const char a)=0;
		//virtual void WritePixel(const float i)=0;
		//virtual void WritePixel(const float r,const float g,const float b,const float a)=0;
		
		virtual bool GenerateMipmaps(Bank* pixels);
		virtual void ContinueLoading();
		virtual void FinishLoading();
		virtual void Reset();
		virtual int CountMipmaps();
		virtual void ReadMipmap(const int miplevel);
		virtual int GetMipmapSize(const int miplevel);
		virtual int GetMipmapWidth(const int miplevel);
		virtual int GetMipmapHeight(const int miplevel);
		virtual int GetMipmapDepth(const int miplevel);
        virtual int CountFrames();
		virtual bool Initialize(const int width, const int height, const int format, const int flags, const int frames, const int samples=0)=0;
		virtual int GetWidth(const int miplevel=0);
		virtual int GetHeight(const int miplevel=0);
		virtual int GetDepth(const int miplevel=0);
        //virtual void ReadPixels(const char* pixels, const int miplevel=0, const int framenumber=0)=0;
		//virtual void WritePixels(const char* pixels, const int miplevel=0, const int framenumber=0)=0;
		//virtual Bank* Lock(const int miplevel=0, const int framenumber=0)=0;
		//virtual void Unlock(const int miplevel=0, const int framenumber=0)=0;
		//virtual void WritePixel(const int x, const int y, const char r, const char g, const char b, const char a, const int miplevel=0, const int framenumber=0)=0;
		//virtual void ReadPixel(const int x, const int y, char& r, char& g, char& b, char& a, const int miplevel=0, const int framenumber=0)=0;
		//virtual void WritePixels(const char* pixels, const int miplevel=0, const int framenumber=0, const int x=0, const int y=0, const int width=0, const int height=0)=0;
		//virtual void ReadPixels(const char* pixels, const int miplevel=0, const int framenumber=0, const int x=0, const int y=0, const int width=0, const int height=0)=0;
		virtual void SetFilter(const int filtermode);
		virtual int GetDataSize(const int width, const int height, const int format);
		virtual Asset* Instance();
		virtual bool Reload(const int flags=0);
		virtual void Bind(const int index)=0;
		//virtual Bank* Lock(miplevel,framenumber);
		//virtual void Unlock(miplevel,framenumber);
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
		int dwMipMapCount;
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
	
	//extern map<std::string,TextureReference*> LoadedTexturesMap__;
	extern TextureReference* BoundTexture__[32];
}
