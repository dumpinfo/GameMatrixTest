#pragma once
#include "OpenGL2GraphicsDriver.h"

namespace Leadwerks
{
	class OpenGL2Model : public Model
	{
		public:

        OpenGL2Model();
		virtual ~OpenGL2Model();

        virtual void Draw(Camera* camera, const bool recursive, const bool drawsorted);
	};
}
