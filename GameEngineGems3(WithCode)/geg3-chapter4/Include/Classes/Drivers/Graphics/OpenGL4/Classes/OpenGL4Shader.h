#pragma once

#include "../../../../../Leadwerks.h"

namespace Leadwerks
{
	class OpenGL2Shader;

	class OpenGL4Shader : public OpenGL2Shader
	{
    public:
        GLuint uniformblockindex_entitymatrix;

		virtual Asset* Copy();
		virtual void InitializeTextureUnits();
		virtual void BindData();
		virtual bool Reload(const int flags = 0);
		virtual void InitializeUniforms();
 	};
}
