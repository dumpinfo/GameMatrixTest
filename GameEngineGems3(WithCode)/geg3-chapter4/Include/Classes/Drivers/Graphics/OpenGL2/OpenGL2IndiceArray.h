#pragma once

#include "../../../../Leadwerks.h"

namespace Leadwerks
{
	class OpenGL2IndiceArray : public IndiceArray
	{
	public:
		GLuint buffer;
        int buffersize;
		
		OpenGL2IndiceArray();
		OpenGL2IndiceArray(const int mode,const int type);
		virtual ~OpenGL2IndiceArray();
		
		virtual IndiceArray* Copy();
		virtual void Lock();
		virtual void Unlock();
		virtual void Enable();
		virtual void Disable();
	};
}
