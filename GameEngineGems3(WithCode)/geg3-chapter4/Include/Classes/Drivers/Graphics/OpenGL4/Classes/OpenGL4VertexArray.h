#pragma once

#include "../../../../../Leadwerks.h"

namespace Leadwerks
{
	class OpenGL4VertexArray : public OpenGL2VertexArray
	{
	public:
		OpenGL4VertexArray(const int target,const int type,const int coordinates);
		virtual void Lock();
	};
}
