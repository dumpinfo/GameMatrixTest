#pragma once

#include "../../../../../Leadwerks.h"

namespace Leadwerks
{
	class Material;
    
	class OpenGL4Material : public OpenGL2Material
	{
	public:
		
		OpenGL4Material();
		virtual ~OpenGL4Material();
		
		virtual Asset* Copy();
	};
}
