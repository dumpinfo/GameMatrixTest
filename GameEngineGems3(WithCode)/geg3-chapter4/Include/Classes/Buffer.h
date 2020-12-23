#pragma once
#include "../Leadwerks.h"

namespace Leadwerks
{
	/*#define BUFFER_DEPTH 1
	#define BUFFER_COLOR 2
	#define BUFFER_COLOR0 BUFFER_COLOR
	#define BUFFER_COLOR1 3
	#define BUFFER_COLOR2 4
	#define BUFFER_COLOR3 5
	#define BUFFER_COLOR4 6
	#define BUFFER_COLOR5 7
	#define BUFFER_COLOR6 8
	#define BUFFER_COLOR7 9*/

	class Buffer : public Object//lua
	{
	public:
		GraphicsDriver* graphicsdriver;
		Texture* colorcomponent[8];
		Texture* depthcomponent;
		bool isbackbuffer;
		int rendermode;
		//int CurrentBlendMode;
		//Vec4 CurrentColor;
		std::list<Buffer*>::iterator it;

		Buffer();
		virtual ~Buffer();
		
		virtual void SetMask(const bool red, const bool green, const bool blue, const bool alpha)=0;//lua
		virtual void SetColor(const Vec4& color);//lua
        virtual void SetColor(const float r, const float g, const float b);//lua
		virtual void SetColor(const float r, const float g, const float b, const float a);//lua
		virtual std::string GetClassName();
		virtual void GetPixels(const char* buf, const int component)=0;
		virtual int CountColorTextures();//lua
		virtual void Clear(const int mode=Depth|Color)=0;//lua
		virtual void Enable();//lua
		virtual void Disable();//lua
		virtual int GetWidth()=0;//lua
		virtual int GetHeight()=0;//lua
		virtual bool SetColorTexture(Texture* texture, const int i=0, const int cubeface=0, const int miplevel=0)=0;//lua
		virtual bool SetDepthTexture(Texture* texture, const int cubeface=0)=0;//lua
		virtual Texture* GetColorTexture(const int index=0);//lua
		virtual Texture* GetDepthTexture();//lua
		//virtual Clear(const int flags)=0;
		virtual void Blit(Buffer* dst, const int components)=0;//lua
		virtual bool Screenshot(const std::string& path="") = 0;//lua

		static const int Depth;//lua
		static const int Color;//lua
		static const int Color0;//lua
		static const int Color1;//lua
		static const int Color2;//lua
		static const int Color3;//lua
		static const int Color4;//lua
		static const int Color5;//lua
		static const int Color6;//lua
		static const int Color7;//lua

		static std::list<Buffer*> list;
		static void SetCurrent(Buffer* buffer);//lua
		static Buffer* GetCurrent();//lua
		static Buffer* Create(const int width, const int height, const int colorcomponents=1, const int depthbuffer=1, const int multisamplemode=0);//lua
	};
	
	//Buffer* GetBuffer();
	
	/*class DefaultBuffer : public Buffer
	{
		virtual int GetWidth();
		virtual int GetHeight();
		virtual void Enable();
		virtual void Disable();
	};*/
}


