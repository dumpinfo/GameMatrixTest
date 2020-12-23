#pragma once

#include "../../../../../Leadwerks.h"

namespace Leadwerks
{
	class OpenGL4Surface : public OpenGL2Surface
	{
	public:
		
		OpenGL4Surface();
		virtual Surface* Copy();
		void Draw(const int instance_count=0);
	};
}
