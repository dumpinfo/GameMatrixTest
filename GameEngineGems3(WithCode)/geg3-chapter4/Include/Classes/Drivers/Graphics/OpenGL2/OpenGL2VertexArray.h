#pragma once

#include "../../../../Leadwerks.h"

namespace Leadwerks
{
	class OpenGL2VertexArray : public VertexArray
	{
		public:

		//unsigned int buffer;
		GLuint buffer;
		int buffersize;
		
		OpenGL2VertexArray();
		OpenGL2VertexArray(const int target,const int type,const int coordinates);
		virtual ~OpenGL2VertexArray();
		
		virtual VertexArray* Copy();
		virtual void Lock();
		virtual void Unlock();
		virtual void Enable();
		virtual void Disable();
	};
}
