#pragma once

#include "../OpenGLES2GraphicsDriver.h"
//#include "../../../../Leadwerks3D.h"

namespace Leadwerks3D
{
	class MaterialReference;

	class OpenGLES2MaterialReference : public MaterialReference
	{
	public:
		
		OpenGLES2MaterialReference();
		virtual ~OpenGLES2MaterialReference();
		
		virtual void Enable(const int flags=0, const int shaderindex=0);
		virtual void Disable();
		virtual AssetReference* Copy();
	};
}
