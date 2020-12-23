#pragma once

#include "../../../../../Leadwerks.h"

namespace Leadwerks
{
	class OpenGL2Batch;

	class OpenGL4Batch : public OpenGL2Batch
	{
	public:
		virtual void Draw(Camera* camera, const int start);
	};
}
