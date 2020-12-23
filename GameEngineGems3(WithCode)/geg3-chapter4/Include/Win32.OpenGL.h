#pragma once

#include "OpenGL.h"

#define NOMINMAX

//#include <windows.h>
//#include <glew.h>
//#include <wglew.h>

#include "Libraries/glew-1.6.0/include/GL/glew.h"
#include "Libraries/glew-1.6.0/include/GL/wglew.h"

namespace Leadwerks
{
	extern int OpenGLStencilBits;

	HGLRC glInitialize(HWND hwnd, HDC hdc);
	HGLRC glCreateContext(HWND hwnd, HDC hdc, HGLRC sharedcontext, const int majorversion, const int minorversion, const int multisamplemode=0);
}
