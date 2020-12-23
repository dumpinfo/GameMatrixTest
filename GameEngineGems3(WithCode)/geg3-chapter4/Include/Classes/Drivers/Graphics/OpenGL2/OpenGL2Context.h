#pragma once

#include "../../../../Leadwerks.h"

namespace Leadwerks
{
	class OpenGL2Context : public Context
	{
		public:

#ifdef _WIN32
		HGLRC hrc;
#endif
#ifdef __linux__
		GLXContext cx;
#endif
		int version;

		OpenGL2Context();
		virtual ~OpenGL2Context();

		//Methods
		virtual void Blit(Buffer* dst, const int components);
		virtual void SetMask(const bool red, const bool green, const bool blue, const bool alpha);//lua
		virtual void GetPixels(const char* buf, const int component=Color);
		virtual void Sync(const bool sync=true);
		virtual int GetWidth();
		virtual int GetHeight();
		virtual void Enable();
		virtual Texture* GetColorTexture(const int index=0);
		virtual void Clear(const int mode);
		virtual void SetAntialias(const bool mode);
		virtual bool Screenshot(const std::string& path = "");//lua

		#ifdef ANDROID
		virtual void AndroidSwapBuffers();
		#endif

		};

}
