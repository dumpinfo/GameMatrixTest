#pragma once

#include "../../../../../Leadwerks.h"

namespace Leadwerks
{
	class OpenGL4Uniform : public OpenGL2Uniform
	{
	public:
        
		OpenGL4Uniform();
        virtual ~OpenGL4Uniform();

		virtual bool SetInt(const int i);
	};
}
