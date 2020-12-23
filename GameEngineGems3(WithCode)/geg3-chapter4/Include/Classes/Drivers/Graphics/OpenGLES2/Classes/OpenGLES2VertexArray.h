#pragma once

#include "../OpenGLES2GraphicsDriver.h"
//#include "../../../../Leadwerks3D.h"

namespace Leadwerks
{
	class OpenGLES2VertexArray : public VertexArray
	{
		public:

		//unsigned int buffer;
		GLuint buffer;
		int buffersize;
		
		OpenGLES2VertexArray(const int target,const int type,const int coordinates);
		virtual ~OpenGLES2VertexArray();
		
        virtual VertexArray* Copy();
		virtual void Lock();
		virtual void Unlock();
		virtual void Enable();
		virtual void Disable();
	};
}
