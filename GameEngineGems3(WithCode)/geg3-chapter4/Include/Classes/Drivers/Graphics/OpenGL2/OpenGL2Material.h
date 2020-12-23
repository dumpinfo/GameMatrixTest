#pragma once

#include "../../../../Leadwerks.h"

namespace Leadwerks
{
	class Material;
    
	class OpenGL2Material : public Material
	{
	public:
		
		OpenGL2Material();
		virtual ~OpenGL2Material();
		
		virtual void Enable(const int flags=0, const int shaderindex=SHADER_DEFAULT);
		virtual void Disable();
		virtual Asset* Copy();
	};
}
