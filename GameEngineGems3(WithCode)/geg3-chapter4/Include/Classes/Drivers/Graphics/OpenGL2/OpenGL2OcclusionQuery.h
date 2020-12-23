#pragma once

#include "../../../../Leadwerks.h"

namespace Leadwerks
{
	class OpenGL2OcclusionQuery : public OcclusionQuery
	{
	public:
		GLuint glquery;
		GLuint result;
		bool active;
		bool everrun;
		bool running;
#ifdef _WIN32
		HGLRC hglrc;
#endif

		OpenGL2OcclusionQuery(GraphicsDriver* graphicsdriver);
		virtual ~OpenGL2OcclusionQuery();
		
		virtual void Reset();
		virtual void Begin();
		virtual void End();
		virtual int GetState();
		virtual int GetResult();

		static bool QueryActive;
	};
}
