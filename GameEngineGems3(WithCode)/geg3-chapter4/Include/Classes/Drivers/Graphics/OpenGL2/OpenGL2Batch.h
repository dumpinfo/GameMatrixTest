#pragma once

#include "../../../../Leadwerks.h"

namespace Leadwerks
{
	class OpenGL2Batch : public Batch
	{
	public:
		virtual void Draw(Camera* camera, const int start=0);
	};
}
