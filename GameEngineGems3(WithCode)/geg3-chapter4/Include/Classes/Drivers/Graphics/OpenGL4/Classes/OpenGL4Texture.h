#pragma once

#include "../../../../../Leadwerks.h"

namespace Leadwerks
{
	class OpenGL2Texture;

	class OpenGL4Texture : public OpenGL2Texture
	{
    public:
		virtual Asset* Copy();
	};
}
