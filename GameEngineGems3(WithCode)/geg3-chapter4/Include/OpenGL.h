#pragma once

#include <string>
#include <string.h>
#include <sstream>

#ifdef OPENGLES
	#ifdef ANDROID
		#import <GLES/gl.h>
		#import <GLES/glext.h>
		#import <GLES2/gl2.h>
		#import <GLES2/gl2ext.h>
	#else
		//#import <OpenGLES/ES1/gl.h>
		//#import <OpenGLES/ES1/glext.h>
		#import <OpenGLES/ES2/gl.h>
		#import <OpenGLES/ES2/glext.h>
	#endif
#endif
#ifdef _WIN32
	#include "Win32.OpenGL.h"
#endif
#ifdef PLATFORM_MACOS
	#include "MacOS.OpenGL.h"
#endif
#ifdef __linux__
    #ifndef __ANDROID__
		#include <GL/glew.h>
        #include <GL/glx.h>
        #include <GL/gl.h>
        #undef None
	#endif
#endif

#include "Leadwerks.h"

namespace Leadwerks
{
	std::string glErrorName(const int errorcode);
	//bool glExtensionSupported(const std::string& name);
	void glCheckError();
	int glGetInteger(const int name);
}
