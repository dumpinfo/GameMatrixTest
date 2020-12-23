#pragma once

#include "OpenGL2GraphicsDriver.h"

namespace Leadwerks
{
	class OpenGL2Surface : public Surface
	{
	public:
		
		OpenGL2Surface();
		virtual ~OpenGL2Surface();
		
		virtual Surface* Copy();
		virtual void Draw(const int instance_count=0);
	};
}
