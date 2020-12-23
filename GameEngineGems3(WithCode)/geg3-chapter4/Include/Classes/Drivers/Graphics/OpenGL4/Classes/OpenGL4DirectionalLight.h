#pragma once

#include "../../../../../Leadwerks.h"

namespace Leadwerks
{
	class OpenGL4DirectionalLight : public DirectionalLight
	{
	public:
		virtual void Render(Entity* entity=NULL);
	};
}
