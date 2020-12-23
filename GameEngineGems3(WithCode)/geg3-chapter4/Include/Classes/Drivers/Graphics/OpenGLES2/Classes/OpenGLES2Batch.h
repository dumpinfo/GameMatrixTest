#pragma once

#include "../OpenGLES2GraphicsDriver.h"
//#include "../../../../Leadwerks3D.h"

namespace Leadwerks
{
	class OpenGLES2Batch : public Batch
	{
	public:
		virtual void Draw(Camera* camera, const int start=0);
	};
}
