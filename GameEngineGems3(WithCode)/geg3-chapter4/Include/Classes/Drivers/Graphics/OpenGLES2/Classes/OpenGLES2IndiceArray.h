#pragma once

#include "../OpenGLES2GraphicsDriver.h"
//#include "../../../../Leadwerks3D.h"


namespace Leadwerks
{
	class OpenGLES2IndiceArray : public IndiceArray
	{
		public:
		GLuint buffer;
        int buffersize;
        
		OpenGLES2IndiceArray(const int mode,const int type);
		virtual ~OpenGLES2IndiceArray();
		
        virtual IndiceArray* Copy();
		virtual void Lock();
		virtual void Unlock();
		virtual void Enable();
		virtual void Disable();
	};
}
