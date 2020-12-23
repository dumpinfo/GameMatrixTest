#pragma once
#pragma warning(disable:4290)
#pragma warning(disable:4996)

#include "../OpenGLES2GraphicsDriver.h"
//#include "../../../../Leadwerks3D.h"

namespace Leadwerks3D
{
	class OpenGLES2TextureReference : public TextureReference
	{
		public:
		
		//Attributes
		unsigned int gltexturehandle;
		
		//Procedures
		OpenGLES2TextureReference();
		virtual ~OpenGLES2TextureReference();
		virtual std::string GetClassName();

		virtual void GetPixels(const char* buf, const int miplevel=0, const int framenumber=0, const int cubeface=0);
		virtual void SetPixels(const char* buf, const int miplevel=0, const int framenumber=0, const int cubeface=0);
		/*
		virtual void Lock(const int miplevel=0, const int framenumber=0, const int cubeface=0);
		virtual void Unlock(const int miplevel=0, const int framenumber=0, const int cubeface=0);
		virtual void WritePixel(const int x, const int y, const unsigned char r, const unsigned char g, const unsigned char b, const unsigned char a=0, const int miplevel=0, const int framenumber=0, const int cubeface=0);
		virtual int ReadPixel(const int x, const int y, const int miplevel=0, const int framenumber=0, const int cubeface=0);
		*/
		//virtual void WritePixel(const char r,const char g,const char b,const char a);
		//virtual void WritePixel(const float i);
		//virtual void WritePixel(const float r,const float g,const float b,const float a);
//		virtual void ReadPixels(const char* pixels, const int miplevel=0, const int framenumber=0);
//		virtual void WritePixels(const char* pixels, const int miplevel=0, const int framenumber=0);
//		virtual void SetPixels(const char* pixels, const int miplevel=0, const int framenumber=0, const int x=0, const int y=0, const int width=0, const int height=0);
//		virtual void GetPixels(const char* pixels, const int miplevel=0, const int framenumber=0, const int x=0, const int y=0, const int width=0, const int height=0);
		virtual AssetReference* Copy();
		virtual void Reset();
		virtual int GetGLTarget();
		virtual bool Initialize(const int width, const int height, const int format, const int flags, const int frames, const int samples);
		virtual void Bind(const int index);
		virtual void SetFilter(const int filtermode);
		//virtual bool Locked(const int miplevel=0, const int framenumber=0);
	};
}
