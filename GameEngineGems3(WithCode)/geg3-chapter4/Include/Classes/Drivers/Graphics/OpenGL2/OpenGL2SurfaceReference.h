#pragma once

#include "OpenGL2GraphicsDriver.h"

namespace Leadwerks
{
	class OpenGL2SurfaceReference : public SurfaceReference
	{
	public:
		
		OpenGL2SurfaceReference();
		virtual ~OpenGL2SurfaceReference();
		
		virtual SurfaceReference* Copy();
		virtual void Draw();
	};
}
