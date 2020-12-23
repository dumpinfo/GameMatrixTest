#pragma once
#include "../OpenGLES2GraphicsDriver.h"

namespace Leadwerks
{	
	class OpenGLES2Model : public Model
	{
		public:
        
		virtual ~OpenGLES2Model();
        virtual void Draw(Camera* camera, const bool recursive, const bool drawsorted);
	};
}
