#pragma once

#include "../../../../Leadwerks.h"

namespace Leadwerks
{
	class OpenGL2Buffer : public Buffer
	{
	public:
		int boundmiplevel;
		GLuint framebuffer;
		OpenGL2Context* context;
		Shader* shader_blitdepth;
		Shader* shader_blitcolor;
		Shader* shader_blitcoloranddepth;

		OpenGL2Buffer();
		virtual ~OpenGL2Buffer();
		
		virtual void SetMask(const bool red, const bool green, const bool blue, const bool alpha);//lua
		virtual void GetPixels(const char* buf, const int component=Color);
		virtual bool Init(const int width, const int height, const int colorcomponents, const int depthbuffer, const int multisamplemode);
		virtual void Enable();
		virtual void Disable();
		virtual int GetWidth();
		virtual int GetHeight();
		virtual bool SetColorTexture(Texture* texture, const int i=0, const int cubeface=0, const int miplevel=0);
		virtual bool SetDepthTexture(Texture* texture, const int cubeface=0);
		virtual void Clear(const int mode=Depth|Color);
		virtual void Blit(Buffer* dst, const int components);//lua
		virtual bool Screenshot(const std::string& path);
	};
}
