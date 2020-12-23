#pragma once

#include "../OpenGLES2GraphicsDriver.h"

namespace Leadwerks
{
	class OpenGLES2OcclusionQuery : public OcclusionQuery
	{
	public:
		GLuint glquery;
		GLuint result;
		bool active;
		bool everrun;
		bool running;
		
		OpenGLES2OcclusionQuery(GraphicsDriver* graphicsdriver);
		~OpenGLES2OcclusionQuery();
		
		virtual void Reset();
		virtual void Begin();
		virtual void End();
		virtual int GetState();
		virtual int GetResult();
	};
}
