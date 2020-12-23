#pragma once

#include "OpenGL2GraphicsDriver.h"
//#include "../../../../Leadwerks.h"

namespace Leadwerks
{
	class MaterialReference;

	class OpenGL2MaterialReference : public MaterialReference
	{
	public:
		
		OpenGL2MaterialReference();
		virtual ~OpenGL2MaterialReference();
		
		virtual void Enable(const int flags=0, const int shaderindex=SHADER_DEFAULT);
		virtual void Disable();
		virtual AssetReference* Copy();
	};
}
