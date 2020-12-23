#pragma once

#include "../../../../../Leadwerks.h"

namespace Leadwerks
{
	class OpenGL4Context : public OpenGL2Context
	{
	public:
		GLuint vao;

		OpenGL4Context();
		virtual ~OpenGL4Context();

	};
}
