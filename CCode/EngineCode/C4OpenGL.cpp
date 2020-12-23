 

#define C4OpenGL_cpp


#include "C4OpenGL.h"
#include "C4Graphics.h"

#if C4MACOS

	#include "C4Engine.h"

#endif


using namespace C4;


#if C4WINDOWS

	void *C4::GetCoreExtFuncAddress(bool core, const char *coreName, const char *extName)
	{
		if (core)
		{
			void *address = wglGetProcAddress(coreName);
			if (address)
			{
				return (address);
			}
		}

		return (wglGetProcAddress(extName));
	}

#elif C4MACOS

	void *C4::GetCoreExtFuncAddress(CFBundleRef bundle, bool core, const char *coreName, const char *extName)
	{
		if (core)
		{
			void *address = Engine::GetBundleFunctionAddress(bundle, coreName);
			if (address)
			{
				return (address);
			}
		}

		return (Engine::GetBundleFunctionAddress(bundle, extName));
	}

#elif C4LINUX

	void *C4::GetCoreExtFuncAddress(bool core, const char *coreName, const char *extName)
	{
		if (core)
		{
			void *address = (void *) glXGetProcAddress(reinterpret_cast<const GLubyte *>(coreName));
			if (address)
			{
				return (address);
			}
		}

		return ((void *) glXGetProcAddress(reinterpret_cast<const GLubyte *>(extName)));
	}

#endif


#if C4GLCORE

	#if C4WINDOWS || C4LINUX

		void C4::InitializeOpenglExtensions(GraphicsCapabilities *capabilities)

	#elif C4MACOS

		void C4::InitializeOpenglExtensions(GraphicsCapabilities *capabilities, CFBundleRef openglBundle)

	#endif

	{
		const bool *extensionFlag = capabilities->extensionFlag;
		unsigned_int32 version = capabilities->openglVersion;

		GLGETCOREFUNC(glBlendColor);
		GLGETCOREFUNC(glBlendEquation);
		GLGETCOREFUNC(glTexImage3D);
		GLGETCOREFUNC(glTexSubImage3D);
		GLGETCOREFUNC(glCopyTexSubImage3D);
		GLGETCOREFUNC(glSampleCoverage);
		GLGETCOREFUNC(glActiveTexture);
		GLGETCOREFUNC(glCompressedTexImage3D);
		GLGETCOREFUNC(glCompressedTexImage2D);
		GLGETCOREFUNC(glCompressedTexImage1D);
		GLGETCOREFUNC(glCompressedTexSubImage3D);
		GLGETCOREFUNC(glCompressedTexSubImage2D);
		GLGETCOREFUNC(glCompressedTexSubImage1D);
		GLGETCOREFUNC(glBlendFuncSeparate);
		GLGETCOREFUNC(glMultiDrawArrays);
		GLGETCOREFUNC(glMultiDrawElements);
		GLGETCOREFUNC(glGenQueries);
		GLGETCOREFUNC(glDeleteQueries); 
		GLGETCOREFUNC(glBeginQuery);
		GLGETCOREFUNC(glEndQuery);
		GLGETCOREFUNC(glGetQueryiv); 
		GLGETCOREFUNC(glGetQueryObjectiv);
		GLGETCOREFUNC(glGetQueryObjectuiv); 
		GLGETCOREFUNC(glBindBuffer);
		GLGETCOREFUNC(glDeleteBuffers);
		GLGETCOREFUNC(glGenBuffers); 
		GLGETCOREFUNC(glBufferData);
		GLGETCOREFUNC(glBufferSubData); 
		GLGETCOREFUNC(glMapBuffer); 
		GLGETCOREFUNC(glUnmapBuffer);
		GLGETCOREFUNC(glGetBufferSubData);
		GLGETCOREFUNC(glDrawBuffers);
		GLGETCOREFUNC(glPointParameterf); 
		GLGETCOREFUNC(glPointParameterfv);
		GLGETCOREFUNC(glPointParameteri);
		GLGETCOREFUNC(glPointParameteriv);
		GLGETCOREFUNC(glStencilFuncSeparate);
		GLGETCOREFUNC(glStencilOpSeparate);
		GLGETCOREFUNC(glCreateShader);
		GLGETCOREFUNC(glDeleteShader);
		GLGETCOREFUNC(glShaderSource);
		GLGETCOREFUNC(glCompileShader);
		GLGETCOREFUNC(glAttachShader);
		GLGETCOREFUNC(glCreateProgram);
		GLGETCOREFUNC(glDeleteProgram);
		GLGETCOREFUNC(glLinkProgram);
		GLGETCOREFUNC(glUseProgram);
		GLGETCOREFUNC(glGetShaderiv);
		GLGETCOREFUNC(glGetProgramiv);
		GLGETCOREFUNC(glGetShaderInfoLog);
		GLGETCOREFUNC(glGetProgramInfoLog);
		GLGETCOREFUNC(glGetShaderSource);
		GLGETCOREFUNC(glGetActiveUniform);
		GLGETCOREFUNC(glGetActiveAttrib);
		GLGETCOREFUNC(glGetUniformLocation);
		GLGETCOREFUNC(glGetAttribLocation);
		GLGETCOREFUNC(glBindAttribLocation);
		GLGETCOREFUNC(glUniform1f);
		GLGETCOREFUNC(glUniform2f);
		GLGETCOREFUNC(glUniform3f);
		GLGETCOREFUNC(glUniform4f);
		GLGETCOREFUNC(glUniform1i);
		GLGETCOREFUNC(glUniform2i);
		GLGETCOREFUNC(glUniform3i);
		GLGETCOREFUNC(glUniform4i);
		GLGETCOREFUNC(glUniform1fv);
		GLGETCOREFUNC(glUniform2fv);
		GLGETCOREFUNC(glUniform3fv);
		GLGETCOREFUNC(glUniform4fv);
		GLGETCOREFUNC(glUniform1iv);
		GLGETCOREFUNC(glUniform2iv);
		GLGETCOREFUNC(glUniform3iv);
		GLGETCOREFUNC(glUniform4iv);
		GLGETCOREFUNC(glVertexAttribPointer);
		GLGETCOREFUNC(glEnableVertexAttribArray);
		GLGETCOREFUNC(glDisableVertexAttribArray);
		GLGETCOREFUNC(glClearBufferiv);
		GLGETCOREFUNC(glClearBufferuiv);
		GLGETCOREFUNC(glClearBufferfv);
		GLGETCOREFUNC(glClearBufferfi);
		GLGETCOREFUNC(glBeginConditionalRender);
		GLGETCOREFUNC(glEndConditionalRender);
		GLGETCOREFUNC(glBlitFramebuffer);
		GLGETCOREFUNC(glRenderbufferStorageMultisample);
		GLGETCOREFUNC(glBindRenderbuffer);
		GLGETCOREFUNC(glDeleteRenderbuffers);
		GLGETCOREFUNC(glGenRenderbuffers);
		GLGETCOREFUNC(glRenderbufferStorage);
		GLGETCOREFUNC(glGetRenderbufferParameteriv);
		GLGETCOREFUNC(glBindFramebuffer);
		GLGETCOREFUNC(glDeleteFramebuffers);
		GLGETCOREFUNC(glGenFramebuffers);
		GLGETCOREFUNC(glCheckFramebufferStatus);
		GLGETCOREFUNC(glFramebufferTexture1D);
		GLGETCOREFUNC(glFramebufferTexture2D);
		GLGETCOREFUNC(glFramebufferTexture3D);
		GLGETCOREFUNC(glFramebufferRenderbuffer);
		GLGETCOREFUNC(glBindFragDataLocation);
		GLGETCOREFUNC(glMapBufferRange);
		GLGETCOREFUNC(glFramebufferTextureLayer);
		GLGETCOREFUNC(glBindVertexArray);
		GLGETCOREFUNC(glDeleteVertexArrays);
		GLGETCOREFUNC(glGenVertexArrays);
		GLGETCOREFUNC(glGetUniformBlockIndex);
		GLGETCOREFUNC(glBindBufferBase);
		GLGETCOREFUNC(glUniformBlockBinding);
		GLGETCOREFUNC(glProgramParameteri);
		GLGETCOREFUNC(glBindFragDataLocationIndexed);
		GLGETCOREFUNC(glGetFragDataIndex);
		GLGETCOREFUNC(glDrawElementsInstanced);
		GLGETCOREFUNC(glVertexAttribDivisor);
		GLGETCOREFUNC(glQueryCounter);
		GLGETCOREFUNC(glGetQueryObjecti64v);
		GLGETCOREFUNC(glGetQueryObjectui64v);

		if (extensionFlag[kExtensionSampleShading])
		{
			GLGETCOREEXTFUNC(glMinSampleShading);
		}

		if (extensionFlag[kExtensionTessellationShader])
		{
			GLGETCOREEXTFUNC(glPatchParameteri);
			GLGETCOREEXTFUNC(glPatchParameterfv);
		}

		if (extensionFlag[kExtensionGetProgramBinary])
		{
			GLGETEXTFUNC(glGetProgramBinary);
			GLGETEXTFUNC(glProgramBinary);
		}

		if (extensionFlag[kExtensionTextureStorage])
		{
			GLGETCOREFUNC(glTexStorage2D);
			GLGETCOREFUNC(glTexStorage3D);

			if (extensionFlag[kExtensionDirectStateAccess])
			{
				GLGETEXTFUNC(glTextureStorage2DEXT);
				GLGETEXTFUNC(glTextureStorage3DEXT);
			}
			else
			{
				glTextureStorage2DEXT = &Render::TextureStorage2D;
				glTextureStorage3DEXT = &Render::TextureStorage3D;
			}
		}

		if (extensionFlag[kExtensionInvalidateSubdata])
		{
			GLGETCOREFUNC(glInvalidateTexImage);
			GLGETCOREFUNC(glInvalidateBufferData);
			GLGETCOREFUNC(glInvalidateFramebuffer);
		}
		else
		{
			glInvalidateTexImage = &Render::InvalidateTexImage;
			glInvalidateBufferData = &Render::InvalidateBufferData;
			glInvalidateFramebuffer = &Render::InvalidateFramebuffer;
		}

		if (extensionFlag[kExtensionMultiBind])
		{
			GLGETCOREFUNC(glBindTextures);
		}
		else
		{
			glBindTextures = &Render::BindTextures;
		}

		#if C4DEBUG

			if (extensionFlag[kExtensionDebugOutput])
			{
				GLGETEXTFUNC(glDebugMessageControlARB);
				GLGETEXTFUNC(glDebugMessageInsertARB);
				GLGETEXTFUNC(glDebugMessageCallbackARB);
				GLGETEXTFUNC(glGetDebugMessageLogARB);
				GLGETEXTFUNC(glGetPointerv);
			}

		#endif

		if (extensionFlag[kExtensionDepthBoundsTest])
		{
			GLGETEXTFUNC(glDepthBoundsEXT);
		}
		else
		{
			glDepthBoundsEXT = &Render::DepthBounds;
		}

		if (extensionFlag[kExtensionDirectStateAccess])
		{
			GLGETEXTFUNC(glBindMultiTextureEXT);
			GLGETEXTFUNC(glTextureParameteriEXT);
			GLGETEXTFUNC(glTextureParameterivEXT);
			GLGETEXTFUNC(glTextureParameterfEXT);
			GLGETEXTFUNC(glTextureParameterfvEXT);
			GLGETEXTFUNC(glTextureImage2DEXT);
			GLGETEXTFUNC(glTextureSubImage2DEXT);
			GLGETEXTFUNC(glTextureImage3DEXT);
			GLGETEXTFUNC(glTextureSubImage3DEXT);
			GLGETEXTFUNC(glCompressedTextureImage2DEXT);
			GLGETEXTFUNC(glCompressedTextureImage3DEXT);
			GLGETEXTFUNC(glCopyTextureSubImage2DEXT);
			GLGETEXTFUNC(glNamedBufferDataEXT);
			GLGETEXTFUNC(glNamedBufferSubDataEXT);
			GLGETEXTFUNC(glGetNamedBufferSubDataEXT);
			GLGETEXTFUNC(glMapNamedBufferRangeEXT);
			GLGETEXTFUNC(glUnmapNamedBufferEXT);
			GLGETEXTFUNC(glProgramUniform4fvEXT);
			GLGETEXTFUNC(glVertexArrayVertexAttribOffsetEXT);
			GLGETEXTFUNC(glVertexArrayVertexAttribDivisorEXT);
			GLGETEXTFUNC(glEnableVertexArrayAttribEXT);
			GLGETEXTFUNC(glDisableVertexArrayAttribEXT);
			GLGETEXTFUNC(glNamedRenderbufferStorageEXT);
			GLGETEXTFUNC(glNamedRenderbufferStorageMultisampleEXT);
			GLGETEXTFUNC(glNamedRenderbufferStorageMultisampleCoverageEXT);
			GLGETEXTFUNC(glCheckNamedFramebufferStatusEXT);
			GLGETEXTFUNC(glNamedFramebufferTexture2DEXT);
			GLGETEXTFUNC(glNamedFramebufferTexture3DEXT);
			GLGETEXTFUNC(glNamedFramebufferRenderbufferEXT);
			GLGETEXTFUNC(glFramebufferDrawBufferEXT);
			GLGETEXTFUNC(glFramebufferDrawBuffersEXT);
			GLGETEXTFUNC(glFramebufferReadBufferEXT);
			GLGETEXTFUNC(glNamedFramebufferTextureEXT);
			GLGETEXTFUNC(glNamedFramebufferTextureLayerEXT);
			GLGETEXTFUNC(glTextureRenderbufferEXT);

			if (!glMapNamedBufferRangeEXT)
			{
				glMapNamedBufferRangeEXT = &Render::MapNamedBufferRange;
			}
		}
		else
		{
			glBindMultiTextureEXT = &Render::BindMultiTexture;
			glTextureParameteriEXT = &Render::TextureParameteri;
			glTextureParameterivEXT = &Render::TextureParameteriv;
			glTextureParameterfEXT = &Render::TextureParameterf;
			glTextureParameterfvEXT = &Render::TextureParameterfv;
			glTextureImage2DEXT = &Render::TextureImage2D;
			glTextureSubImage2DEXT = &Render::TextureSubImage2D;
			glTextureImage3DEXT = &Render::TextureImage3D;
			glTextureSubImage3DEXT = &Render::TextureSubImage3D;
			glCompressedTextureImage2DEXT = &Render::CompressedTextureImage2D;
			glCompressedTextureImage3DEXT = &Render::CompressedTextureImage3D;
			glCopyTextureSubImage2DEXT = &Render::CopyTextureSubImage2D;
			glNamedBufferDataEXT = &Render::NamedBufferData;
			glNamedBufferSubDataEXT = &Render::NamedBufferSubData;
			glGetNamedBufferSubDataEXT = &Render::GetNamedBufferSubData;
			glMapNamedBufferRangeEXT = &Render::MapNamedBufferRange;
			glUnmapNamedBufferEXT = &Render::UnmapNamedBuffer;
			glProgramUniform4fvEXT = &Render::ProgramUniform4fv;
			glVertexArrayVertexAttribOffsetEXT = &Render::VertexArrayVertexAttribOffset;
			glVertexArrayVertexAttribDivisorEXT = &Render::VertexArrayVertexAttribDivisor;
			glEnableVertexArrayAttribEXT = &Render::EnableVertexArrayAttrib;
			glDisableVertexArrayAttribEXT = &Render::DisableVertexArrayAttrib;
		}

		if (extensionFlag[kExtensionPolygonOffsetClamp])
		{
			GLGETEXTFUNC(glPolygonOffsetClampEXT);
		}
		else
		{
			glPolygonOffsetClampEXT = &Render::PolygonOffsetClamp;
		}

		if (extensionFlag[kExtensionExplicitMultisample])
		{
			GLGETEXTFUNC(glGetBooleanIndexedvEXT);
			GLGETEXTFUNC(glGetIntegerIndexedvEXT);
			GLGETEXTFUNC(glGetMultisamplefvNV);
			GLGETEXTFUNC(glSampleMaskIndexedNV);
			GLGETEXTFUNC(glTexRenderbufferNV);
		}

		if (extensionFlag[kExtensionFramebufferMultisampleCoverage])
		{
			GLGETEXTFUNC(glRenderbufferStorageMultisampleCoverageNV);
		}

		if (extensionFlag[kExtensionShaderBufferLoad])
		{
			GLGETEXTFUNC(glMakeNamedBufferResidentNV);
			GLGETEXTFUNC(glMakeNamedBufferNonResidentNV);
			GLGETEXTFUNC(glGetNamedBufferParameterui64vNV);
			GLGETEXTFUNC(glProgramUniformui64NV);
			GLGETEXTFUNC(glProgramUniformui64vNV);
		}

		if (extensionFlag[kExtensionVertexBufferUnifiedMemory])
		{
			GLGETEXTFUNC(glBufferAddressRangeNV);
			GLGETEXTFUNC(glVertexAttribFormatNV);
			GLGETEXTFUNC(glVertexAttribIFormatNV);
			GLGETEXTFUNC(glGetIntegerui64i_vNV);
		}
	}

#endif

// ZYUQURM
