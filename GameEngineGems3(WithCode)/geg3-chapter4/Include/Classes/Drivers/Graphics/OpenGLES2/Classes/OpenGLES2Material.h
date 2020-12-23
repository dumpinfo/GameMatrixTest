#pragma once

#include "../OpenGLES2GraphicsDriver.h"
//#include "../../../../Leadwerks3D.h"

namespace Leadwerks
{
	class OpenGLES2Material : public Material
	{
	public:
		
		OpenGLES2Material();
		virtual ~OpenGLES2Material();
		
		virtual void Enable(const int flags=0, const int shaderindex=SHADER_DEFAULT);
		virtual void Disable();
		virtual Asset* Copy();
	};
}
