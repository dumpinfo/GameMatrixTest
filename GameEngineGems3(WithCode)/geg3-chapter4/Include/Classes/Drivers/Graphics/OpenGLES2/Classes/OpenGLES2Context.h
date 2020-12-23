#pragma once

#include "../OpenGLES2GraphicsDriver.h"

namespace Leadwerks
{
	class OpenGLES2Context : public Context
	{
		public:
		
		//Attributes
		#ifdef _WIN32
			HGLRC hrc;
		#endif
		
		int version;
		
		OpenGLES2Context();
		virtual ~OpenGLES2Context();
		
		//Methods
		virtual void GetPixels(const char* buf, const int component=Color);
		virtual void Sync(const bool sync=true);
		virtual int GetWidth();
		virtual int GetHeight();
		virtual void Enable();
		virtual Texture* GetColorTexture(const int index=0);
		virtual void Clear(const int mode);
		virtual void SetAntialias(const bool mode);
		virtual void SetMask(const bool red, const bool green, const bool blue, const bool alpha);
		virtual void Blit(Buffer* dst, const int components);

		#ifdef ANDROID
		virtual void AndroidSwapBuffers();
		#endif

		};
	
}
