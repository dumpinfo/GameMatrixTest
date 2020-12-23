#pragma once

#include "../OpenGLES2GraphicsDriver.h"
//#include "../../../../Leadwerks3D.h"

namespace Leadwerks
{
	class OpenGLES2Buffer : public Buffer
	{
	public:
		Shader* shader_blitdepth;
		Shader* shader_blitcolor;
		GLuint framebuffer;
		OpenGLES2Context* context;
		
		OpenGLES2Buffer();
		virtual ~OpenGLES2Buffer();
		
		virtual void GetPixels(const char* buf, const int component=Color);
		virtual bool Init(const int width, const int height, const int colorcomponents, const int depthbuffer, const int multisamplemode);
		virtual void Enable();
		virtual void Disable();
		virtual int GetWidth();
		virtual int GetHeight();
		virtual bool SetColorTexture(Texture* texture, const int i=0, const int cubeface=0);
		virtual bool SetDepthTexture(Texture* texture, const int cubeface=0);
		virtual void Clear(const int mode=Depth|Color);
		virtual void SetMask(const bool red, const bool green, const bool blue, const bool alpha);
		virtual void Blit(Buffer* dst, const int components);
	};
}
