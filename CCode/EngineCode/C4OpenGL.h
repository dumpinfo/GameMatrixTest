 

#ifndef C4OpenGL_h
#define C4OpenGL_h


#include "C4Memory.h"


namespace C4
{
	#if C4WINDOWS

		#define OPENGLAPI APIENTRY

	#else

		#define OPENGLAPI

	#endif


	#ifdef C4OpenGL_cpp

		#define GLCOREFUNC(type, name, params) type (OPENGLAPI *name)params = nullptr;
		#define GLEXTFUNC(type, name, params) type (OPENGLAPI *name)params = nullptr;

		#if C4WINDOWS || C4LINUX

			void *GetCoreExtFuncAddress(bool core, const char *coreName, const char *extName);

			#define GLCOREEXTFUNC(type, name, params, version, string) type (OPENGLAPI *name)params = nullptr; inline void GetCoreExtFuncAddress_##name(unsigned_int32 ver) {*(void **) &name = GetCoreExtFuncAddress((ver >= version), #name, string);}

		#elif C4MACOS

			void *GetCoreExtFuncAddress(CFBundleRef bundle, bool core, const char *coreName, const char *extName);

			#define GLCOREEXTFUNC(type, name, params, version, string) type (*name)params = nullptr; inline void GetCoreExtFuncAddress_##name(CFBundleRef bundle, unsigned_int32 ver) {*(void **) &name = GetCoreExtFuncAddress(bundle, (ver >= version), #name, string);}

		#endif

	#else

		#define GLCOREFUNC(type, name, params) extern type (OPENGLAPI *name)params;
		#define GLEXTFUNC(type, name, params) extern type (OPENGLAPI *name)params;
		#define GLCOREEXTFUNC(type, name, params, version, string) extern type (OPENGLAPI *name)params;

	#endif


	#if C4WINDOWS && C4FASTBUILD

		typedef unsigned int	GLenum;
		typedef unsigned char	GLboolean;
		typedef unsigned int	GLbitfield;
		typedef signed char		GLbyte;
		typedef short			GLshort;
		typedef int				GLint;
		typedef int				GLsizei;
		typedef unsigned char	GLubyte;
		typedef unsigned short	GLushort;
		typedef unsigned int	GLuint;
		typedef float			GLfloat;
		typedef float			GLclampf;
		typedef double			GLdouble;
		typedef double			GLclampd;

		#define GL_NONE							0
		#define GL_NO_ERROR						0
		#define GL_ZERO							0
		#define GL_ONE							1
		#define GL_POINTS						0x0000
		#define GL_LINES						0x0001
		#define GL_LINE_LOOP					0x0002
		#define GL_LINE_STRIP					0x0003
		#define GL_TRIANGLES					0x0004
		#define GL_TRIANGLE_STRIP				0x0005
		#define GL_TRIANGLE_FAN					0x0006
		#define GL_NEVER						0x0200
		#define GL_LESS							0x0201
		#define GL_EQUAL						0x0202
		#define GL_LEQUAL						0x0203
		#define GL_GREATER						0x0204
		#define GL_NOTEQUAL						0x0205
		#define GL_GEQUAL						0x0206
		#define GL_ALWAYS						0x0207
		#define GL_SRC_COLOR					0x0300
		#define GL_ONE_MINUS_SRC_COLOR			0x0301
		#define GL_SRC_ALPHA					0x0302
		#define GL_ONE_MINUS_SRC_ALPHA			0x0303
		#define GL_DST_ALPHA					0x0304
		#define GL_ONE_MINUS_DST_ALPHA			0x0305
		#define GL_DST_COLOR					0x0306
		#define GL_ONE_MINUS_DST_COLOR			0x0307
		#define GL_FRONT						0x0404
		#define GL_BACK							0x0405
		#define GL_LEFT							0x0406
		#define GL_RIGHT						0x0407
		#define GL_FRONT_AND_BACK				0x0408
		#define GL_CW							0x0900
		#define GL_CCW							0x0901
		#define GL_LINE_SMOOTH					0x0B20
		#define GL_CULL_FACE					0x0B44 
		#define GL_DEPTH_TEST					0x0B71
		#define GL_STENCIL_TEST					0x0B90
		#define GL_ALPHA_TEST					0x0BC0 
		#define GL_BLEND						0x0BE2
		#define GL_SCISSOR_TEST					0x0C11 
		#define GL_UNPACK_ROW_LENGTH			0x0CF2
		#define GL_UNPACK_ALIGNMENT				0x0CF5
		#define GL_PACK_ROW_LENGTH				0x0D02 
		#define GL_MAX_TEXTURE_SIZE				0x0D33
		#define GL_MAX_VIEWPORT_DIMS			0x0D3A 
		#define GL_TEXTURE_2D					0x0DE1 
		#define GL_TEXTURE_BORDER_COLOR			0x1004
		#define GL_BYTE							0x1400
		#define GL_UNSIGNED_BYTE				0x1401
		#define GL_SHORT						0x1402 
		#define GL_UNSIGNED_SHORT				0x1403
		#define GL_INT							0x1404
		#define GL_UNSIGNED_INT					0x1405
		#define GL_FLOAT						0x1406
		#define GL_INVERT						0x150A
		#define GL_COLOR						0x1800
		#define GL_DEPTH						0x1801
		#define GL_STENCIL						0x1802
		#define GL_DEPTH_COMPONENT				0x1902
		#define GL_RED							0x1903
		#define GL_GREEN						0x1904
		#define GL_BLUE							0x1905
		#define GL_ALPHA						0x1906
		#define GL_RGBA							0x1908
		#define GL_POINT						0x1B00
		#define GL_LINE							0x1B01
		#define GL_FILL							0x1B02
		#define GL_KEEP							0x1E00
		#define GL_REPLACE						0x1E01
		#define GL_INCR							0x1E02
		#define GL_DECR							0x1E03
		#define GL_VENDOR						0x1F00
		#define GL_RENDERER						0x1F01
		#define GL_VERSION						0x1F02
		#define GL_EXTENSIONS					0x1F03
		#define GL_NEAREST						0x2600
		#define GL_LINEAR						0x2601
		#define GL_NEAREST_MIPMAP_NEAREST		0x2700
		#define GL_LINEAR_MIPMAP_NEAREST		0x2701
		#define GL_NEAREST_MIPMAP_LINEAR		0x2702
		#define GL_LINEAR_MIPMAP_LINEAR			0x2703
		#define GL_TEXTURE_MAG_FILTER			0x2800
		#define GL_TEXTURE_MIN_FILTER			0x2801
		#define GL_TEXTURE_WRAP_S				0x2802
		#define GL_TEXTURE_WRAP_T				0x2803
		#define GL_CLAMP						0x2900
		#define GL_REPEAT						0x2901
		#define GL_POLYGON_OFFSET_POINT			0x2A01
		#define GL_POLYGON_OFFSET_LINE			0x2A02
		#define GL_POLYGON_OFFSET_FILL			0x8037
		#define GL_RGB8							0x8051
		#define GL_RGBA8						0x8058
		#define GL_DEPTH_BUFFER_BIT				0x00000100
		#define GL_STENCIL_BUFFER_BIT			0x00000400
		#define GL_COLOR_BUFFER_BIT				0x00004000

		extern "C"
		{
			WINGDIAPI void APIENTRY glBindTexture(GLenum, GLuint);
			WINGDIAPI void APIENTRY glBlendFunc(GLenum, GLenum);
			WINGDIAPI void APIENTRY glColorMask(GLboolean, GLboolean, GLboolean, GLboolean);
			WINGDIAPI void APIENTRY glCopyTexSubImage2D(GLenum, GLint, GLint, GLint, GLint, GLint, GLsizei, GLsizei);
			WINGDIAPI void APIENTRY glCullFace(GLenum);
			WINGDIAPI void APIENTRY glDeleteTextures(GLsizei, const GLuint *);
			WINGDIAPI void APIENTRY glDepthFunc(GLenum);
			WINGDIAPI void APIENTRY glDepthMask(GLboolean);
			WINGDIAPI void APIENTRY glDepthRange(GLclampd, GLclampd);
			WINGDIAPI void APIENTRY glDisable(GLenum);
			WINGDIAPI void APIENTRY glDisableClientState(GLenum);
			WINGDIAPI void APIENTRY glDrawArrays(GLenum, GLint, GLsizei);
			WINGDIAPI void APIENTRY glDrawBuffer(GLenum);
			WINGDIAPI void APIENTRY glDrawElements(GLenum, GLsizei, GLenum, const void *);
			WINGDIAPI void APIENTRY glEnable(GLenum);
			WINGDIAPI void APIENTRY glEnableClientState(GLenum);
			WINGDIAPI void APIENTRY glFinish(void);
			WINGDIAPI void APIENTRY glFlush(void);
			WINGDIAPI void APIENTRY glFrontFace(GLenum);
			WINGDIAPI void APIENTRY glGenTextures(GLsizei, GLuint *);
			WINGDIAPI GLenum APIENTRY glGetError(void);
			WINGDIAPI void APIENTRY glGetFloatv(GLenum, GLfloat *);
			WINGDIAPI void APIENTRY glGetIntegerv(GLenum, GLint *);
			WINGDIAPI const GLubyte *APIENTRY glGetString(GLenum);
			WINGDIAPI void APIENTRY glPixelStorei(GLenum, GLint);
			WINGDIAPI void APIENTRY glPointSize(GLfloat);
			WINGDIAPI void APIENTRY glPolygonMode(GLenum, GLenum);
			WINGDIAPI void APIENTRY glPolygonOffset(GLfloat, GLfloat);
			WINGDIAPI void APIENTRY glReadBuffer(GLenum);
			WINGDIAPI void APIENTRY glReadPixels(GLint, GLint, GLsizei, GLsizei, GLenum, GLenum, void *);
			WINGDIAPI void APIENTRY glScissor(GLint, GLint, GLsizei, GLsizei);
			WINGDIAPI void APIENTRY glStencilFunc(GLenum, GLint, GLuint);
			WINGDIAPI void APIENTRY glStencilMask(GLuint);
			WINGDIAPI void APIENTRY glStencilOp(GLenum, GLenum, GLenum);
			WINGDIAPI void APIENTRY glTexImage2D(GLenum, GLint, GLint, GLsizei, GLsizei, GLint, GLenum, GLenum, const void *);
			WINGDIAPI void APIENTRY glTexParameterf(GLenum, GLenum, GLfloat);
			WINGDIAPI void APIENTRY glTexParameterfv(GLenum, GLenum, const GLfloat *);
			WINGDIAPI void APIENTRY glTexParameteri(GLenum, GLenum, GLint);
			WINGDIAPI void APIENTRY glTexParameteriv(GLenum, GLenum, const GLint *);
			WINGDIAPI void APIENTRY glTexSubImage2D(GLenum, GLint, GLint, GLint, GLsizei, GLsizei, GLenum, GLenum, const void *);
			WINGDIAPI void APIENTRY glViewport(GLint, GLint, GLsizei, GLsizei);
		}

	#endif


	#if C4GLCORE

		typedef char				GLchar;
		typedef ptrdiff_t			GLintptr;
		typedef ptrdiff_t			GLsizeiptr;
		typedef int64				GLint64;
		typedef unsigned_int64		GLuint64;


		// -------------------------------------------------------------------
		//
		// OpenGL 1.2 Core Features
		//
		// -------------------------------------------------------------------

		// bgra
		#define GL_BGRA													0x80E1

		// blend_color
		#define GL_CONSTANT_COLOR										0x8001
		#define GL_ONE_MINUS_CONSTANT_COLOR								0x8002
		#define GL_CONSTANT_ALPHA										0x8003
		#define GL_ONE_MINUS_CONSTANT_ALPHA								0x8004

		GLCOREFUNC(void, glBlendColor, (GLclampf, GLclampf, GLclampf, GLclampf))

		// blend_minmax
		#define GL_FUNC_ADD												0x8006
		#define GL_MIN													0x8007
		#define GL_MAX													0x8008

		GLCOREFUNC(void, glBlendEquation, (GLenum))

		// blend_subtract
		#define GL_FUNC_SUBTRACT										0x800A
		#define GL_FUNC_REVERSE_SUBTRACT								0x800B

		// packed_pixels
		#define GL_UNSIGNED_SHORT_4_4_4_4								0x8033
		#define GL_UNSIGNED_SHORT_5_5_5_1								0x8034
		#define GL_UNSIGNED_INT_8_8_8_8									0x8035
		#define GL_UNSIGNED_INT_10_10_10_2								0x8036
		#define GL_UNSIGNED_SHORT_5_6_5									0x8363
		#define GL_UNSIGNED_SHORT_5_6_5_REV								0x8364
		#define GL_UNSIGNED_SHORT_4_4_4_4_REV							0x8365
		#define GL_UNSIGNED_SHORT_1_5_5_5_REV							0x8366
		#define GL_UNSIGNED_INT_8_8_8_8_REV								0x8367
		#define GL_UNSIGNED_INT_2_10_10_10_REV							0x8368

		// texture3D
		#define GL_PACK_SKIP_IMAGES										0x806B
		#define GL_PACK_IMAGE_HEIGHT									0x806C
		#define GL_UNPACK_SKIP_IMAGES									0x806D
		#define GL_UNPACK_IMAGE_HEIGHT									0x806E
		#define GL_TEXTURE_3D											0x806F
		#define GL_TEXTURE_DEPTH										0x8071
		#define GL_TEXTURE_WRAP_R										0x8072
		#define GL_MAX_3D_TEXTURE_SIZE									0x8073

		GLCOREFUNC(void, glTexImage3D, (GLenum, GLint, GLenum, GLsizei, GLsizei, GLsizei, GLint, GLenum, GLenum, const void *))
		GLCOREFUNC(void, glTexSubImage3D, (GLenum, GLint, GLint, GLint, GLint, GLsizei, GLsizei, GLsizei, GLenum, GLenum, const void *))
		GLCOREFUNC(void, glCopyTexSubImage3D, (GLenum, GLint, GLint, GLint, GLint, GLint, GLint, GLsizei, GLsizei))

		// texture_edge_clamp
		#define GL_CLAMP_TO_EDGE										0x812F

		// texture_lod
		#define GL_TEXTURE_MIN_LOD										0x813A
		#define GL_TEXTURE_MAX_LOD										0x813B
		#define GL_TEXTURE_BASE_LEVEL									0x813C
		#define GL_TEXTURE_MAX_LEVEL									0x813D


		// -------------------------------------------------------------------
		//
		// OpenGL 1.3 Core Features
		//
		// -------------------------------------------------------------------

		// multisample
		#define GL_MULTISAMPLE											0x809D
		#define GL_SAMPLE_ALPHA_TO_COVERAGE								0x809E
		#define GL_SAMPLE_ALPHA_TO_ONE									0x809F
		#define GL_SAMPLE_COVERAGE										0x80A0
		#define GL_SAMPLE_BUFFERS										0x80A8
		#define GL_SAMPLES												0x80A9
		#define GL_SAMPLE_COVERAGE_VALUE								0x80AA
		#define GL_SAMPLE_COVERAGE_INVERT								0x80AB

		GLCOREFUNC(void, glSampleCoverage, (GLclampf, GLboolean))

		// multitexture
		#define GL_TEXTURE0												0x84C0
		#define GL_MAX_TEXTURE_UNITS									0x84E2

		GLCOREFUNC(void, glActiveTexture, (GLenum))

		// texture_border_clamp
		#define GL_CLAMP_TO_BORDER										0x812D

		// texture_compression
		#define GL_COMPRESSED_ALPHA										0x84E9
		#define GL_COMPRESSED_RGB										0x84ED
		#define GL_COMPRESSED_RGBA										0x84EE
		#define GL_NUM_COMPRESSED_TEXTURE_FORMATS						0x86A2
		#define GL_COMPRESSED_TEXTURE_FORMATS							0x86A3

		GLCOREFUNC(void, glCompressedTexImage3D, (GLenum, GLint, GLenum, GLsizei, GLsizei, GLsizei, GLint, GLsizei, const void *))
		GLCOREFUNC(void, glCompressedTexImage2D, (GLenum, GLint, GLenum, GLsizei, GLsizei, GLint, GLsizei, const void *))
		GLCOREFUNC(void, glCompressedTexImage1D, (GLenum, GLint, GLenum, GLsizei, GLint, GLsizei, const void *))
		GLCOREFUNC(void, glCompressedTexSubImage3D, (GLenum, GLint, GLint, GLint, GLint, GLsizei, GLsizei, GLsizei, GLenum, GLsizei, const void *))
		GLCOREFUNC(void, glCompressedTexSubImage2D, (GLenum, GLint, GLint, GLint, GLsizei, GLsizei, GLenum, GLsizei, const void *))
		GLCOREFUNC(void, glCompressedTexSubImage1D, (GLenum, GLint, GLint, GLsizei, GLenum, GLsizei, const void *))

		// texture_cube_map
		#define GL_TEXTURE_CUBE_MAP										0x8513
		#define GL_TEXTURE_CUBE_MAP_POSITIVE_X							0x8515
		#define GL_TEXTURE_CUBE_MAP_NEGATIVE_X							0x8516
		#define GL_TEXTURE_CUBE_MAP_POSITIVE_Y							0x8517
		#define GL_TEXTURE_CUBE_MAP_NEGATIVE_Y							0x8518
		#define GL_TEXTURE_CUBE_MAP_POSITIVE_Z							0x8519
		#define GL_TEXTURE_CUBE_MAP_NEGATIVE_Z							0x851A
		#define GL_MAX_CUBE_MAP_TEXTURE_SIZE							0x851C


		// -------------------------------------------------------------------
		//
		// OpenGL 1.4 Core Features
		//
		// -------------------------------------------------------------------

		// blend_func_separate
		GLCOREFUNC(void, glBlendFuncSeparate, (GLenum, GLenum, GLenum, GLenum))

		// depth_texture
		#define GL_DEPTH_COMPONENT16									0x81A5
		#define GL_DEPTH_COMPONENT24									0x81A6
		#define GL_DEPTH_COMPONENT32									0x81A7

		// multi_draw_arrays
		GLCOREFUNC(void, glMultiDrawArrays, (GLenum, const GLint *, const GLsizei *, GLsizei))
		GLCOREFUNC(void, glMultiDrawElements, (GLenum, const GLsizei *, GLenum, const void *const *, GLsizei))

		// shadow
		#define GL_TEXTURE_COMPARE_MODE									0x884C
		#define GL_TEXTURE_COMPARE_FUNC									0x884D
		#define GL_COMPARE_REF_TO_TEXTURE								0x884E

		// stencil_wrap
		#define GL_INCR_WRAP											0x8507
		#define GL_DECR_WRAP											0x8508

		// texture_lod_bias
		#define GL_TEXTURE_FILTER_CONTROL								0x8500
		#define GL_TEXTURE_LOD_BIAS										0x8501
		#define GL_MAX_TEXTURE_LOD_BIAS									0x84FD

		// texture_mirrored_repeat
		#define GL_MIRRORED_REPEAT										0x8370


		// -------------------------------------------------------------------
		//
		// OpenGL 1.5 Core Features
		//
		// -------------------------------------------------------------------

		// occlusion_query
		#define GL_SAMPLES_PASSED										0x8914
		#define GL_QUERY_COUNTER_BITS									0x8864
		#define GL_QUERY_RESULT											0x8866
		#define GL_QUERY_RESULT_AVAILABLE								0x8867

		GLCOREFUNC(void, glGenQueries, (GLsizei, GLuint *))
		GLCOREFUNC(void, glDeleteQueries, (GLsizei, const GLuint *))
		GLCOREFUNC(void, glBeginQuery, (GLenum, GLuint))
		GLCOREFUNC(void, glEndQuery, (GLenum))
		GLCOREFUNC(void, glGetQueryiv, (GLenum, GLenum, GLint *))
		GLCOREFUNC(void, glGetQueryObjectiv, (GLuint, GLenum, GLint *))
		GLCOREFUNC(void, glGetQueryObjectuiv, (GLuint, GLenum, GLuint *))

		// vertex_buffer_object
		#define GL_ARRAY_BUFFER											0x8892
		#define GL_ELEMENT_ARRAY_BUFFER									0x8893
		#define GL_STREAM_DRAW											0x88E0
		#define GL_STREAM_READ											0x88E1
		#define GL_STREAM_COPY											0x88E2
		#define GL_STATIC_DRAW											0x88E4
		#define GL_STATIC_READ											0x88E5
		#define GL_STATIC_COPY											0x88E6
		#define GL_DYNAMIC_DRAW											0x88E8
		#define GL_DYNAMIC_READ											0x88E9
		#define GL_DYNAMIC_COPY											0x88EA
		#define GL_READ_ONLY											0x88B8
		#define GL_WRITE_ONLY											0x88B9
		#define GL_READ_WRITE											0x88BA

		GLCOREFUNC(void, glBindBuffer, (GLenum, GLuint))
		GLCOREFUNC(void, glDeleteBuffers, (GLsizei, const GLuint *))
		GLCOREFUNC(void, glGenBuffers, (GLsizei, GLuint *))
		GLCOREFUNC(void, glBufferData, (GLenum, GLsizeiptr, const void *, GLenum))
		GLCOREFUNC(void, glBufferSubData, (GLenum, GLintptr, GLsizeiptr, const void *))
		GLCOREFUNC(void *, glMapBuffer, (GLenum, GLenum))
		GLCOREFUNC(GLboolean, glUnmapBuffer, (GLenum))
		GLCOREFUNC(void, glGetBufferSubData, (GLenum, GLintptr, GLsizeiptr, void *))


		// -------------------------------------------------------------------
		//
		// OpenGL 2.0 Core Features
		//
		// -------------------------------------------------------------------

		// draw_buffers
		GLCOREFUNC(void, glDrawBuffers, (GLsizei, const GLenum *))

		// fragment_shader
		#define GL_FRAGMENT_SHADER										0x8B30
		#define GL_MAX_TEXTURE_COORDS									0x8871
		#define GL_MAX_TEXTURE_IMAGE_UNITS								0x8872
		#define GL_MAX_FRAGMENT_UNIFORM_COMPONENTS						0x8B49

		// point_sprite
		#define GL_POINT_SPRITE_COORD_ORIGIN							0x8CA0
		#define GL_LOWER_LEFT											0x8CA1
		#define GL_UPPER_LEFT											0x8CA2

		GLCOREFUNC(void, glPointParameterf, (GLenum, GLfloat))
		GLCOREFUNC(void, glPointParameterfv, (GLenum, const GLfloat *))
		GLCOREFUNC(void, glPointParameteri, (GLenum, GLint))
		GLCOREFUNC(void, glPointParameteriv, (GLenum, const GLint *))

		// separate_stencil
		GLCOREFUNC(void, glStencilOpSeparate, (GLenum, GLenum, GLenum, GLenum))
		GLCOREFUNC(void, glStencilFuncSeparate, (GLenum, GLenum, GLint, GLuint))

		// shader_objects
		#define GL_SHADER_TYPE											0x8B4F
		#define GL_COMPILE_STATUS										0x8B81
		#define GL_LINK_STATUS											0x8B82
		#define GL_INFO_LOG_LENGTH										0x8B84
		#define GL_SHADER_SOURCE_LENGTH									0x8B88
		#define GL_SHADING_LANGUAGE_VERSION								0x8B8C
		#define GL_FLOAT_VEC2											0x8B50
		#define GL_FLOAT_VEC3											0x8B51
		#define GL_FLOAT_VEC4											0x8B52
		#define GL_INT_VEC2												0x8B53
		#define GL_INT_VEC3												0x8B54
		#define GL_INT_VEC4												0x8B55
		#define GL_BOOL													0x8B56
		#define GL_BOOL_VEC2											0x8B57
		#define GL_BOOL_VEC3											0x8B58
		#define GL_BOOL_VEC4											0x8B59
		#define GL_FLOAT_MAT2											0x8B5A
		#define GL_FLOAT_MAT3											0x8B5B
		#define GL_FLOAT_MAT4											0x8B5C
		#define GL_SAMPLER_1D											0x8B5D
		#define GL_SAMPLER_2D											0x8B5E
		#define GL_SAMPLER_3D											0x8B5F
		#define GL_SAMPLER_CUBE											0x8B60
		#define GL_SAMPLER_1D_SHADOW									0x8B61
		#define GL_SAMPLER_2D_SHADOW									0x8B62

		GLCOREFUNC(GLuint, glCreateShader, (GLenum))
		GLCOREFUNC(void, glDeleteShader, (GLuint))
		GLCOREFUNC(void, glShaderSource, (GLuint, GLsizei, const GLchar **, const GLint *))
		GLCOREFUNC(void, glCompileShader, (GLuint))
		GLCOREFUNC(void, glAttachShader, (GLuint, GLuint))
		GLCOREFUNC(GLuint, glCreateProgram, (void))
		GLCOREFUNC(void, glDeleteProgram, (GLuint))
		GLCOREFUNC(void, glLinkProgram, (GLuint))
		GLCOREFUNC(void, glUseProgram, (GLuint))
		GLCOREFUNC(void, glGetShaderiv, (GLuint, GLenum, GLint *))
		GLCOREFUNC(void, glGetProgramiv, (GLuint, GLenum, GLint *))
		GLCOREFUNC(void, glGetShaderInfoLog, (GLuint, GLsizei, GLsizei *, GLchar *))
		GLCOREFUNC(void, glGetProgramInfoLog, (GLuint, GLsizei, GLsizei *, GLchar *))
		GLCOREFUNC(void, glGetShaderSource, (GLuint, GLsizei, GLsizei *, GLchar *))
		GLCOREFUNC(void, glGetActiveUniform, (GLuint, GLuint, GLsizei, GLsizei *, GLint *, GLenum *, GLchar *))
		GLCOREFUNC(void, glGetActiveAttrib, (GLuint, GLuint, GLsizei, GLsizei *, GLint *, GLenum *, GLchar *))
		GLCOREFUNC(GLint, glGetUniformLocation, (GLuint, const GLchar *))
		GLCOREFUNC(GLint, glGetAttribLocation, (GLuint, const GLchar *))
		GLCOREFUNC(void, glBindAttribLocation, (GLuint, GLuint, const GLchar *))
		GLCOREFUNC(void, glUniform1f, (GLint, GLfloat))
		GLCOREFUNC(void, glUniform2f, (GLint, GLfloat, GLfloat))
		GLCOREFUNC(void, glUniform3f, (GLint, GLfloat, GLfloat, GLfloat))
		GLCOREFUNC(void, glUniform4f, (GLint, GLfloat, GLfloat, GLfloat, GLfloat))
		GLCOREFUNC(void, glUniform1i, (GLint, GLint))
		GLCOREFUNC(void, glUniform2i, (GLint, GLint, GLint))
		GLCOREFUNC(void, glUniform3i, (GLint, GLint, GLint, GLint))
		GLCOREFUNC(void, glUniform4i, (GLint, GLint, GLint, GLint, GLint))
		GLCOREFUNC(void, glUniform1fv, (GLint, GLsizei, const GLfloat *))
		GLCOREFUNC(void, glUniform2fv, (GLint, GLsizei, const GLfloat *))
		GLCOREFUNC(void, glUniform3fv, (GLint, GLsizei, const GLfloat *))
		GLCOREFUNC(void, glUniform4fv, (GLint, GLsizei, const GLfloat *))
		GLCOREFUNC(void, glUniform1iv, (GLint, GLsizei, const GLint *))
		GLCOREFUNC(void, glUniform2iv, (GLint, GLsizei, const GLint *))
		GLCOREFUNC(void, glUniform3iv, (GLint, GLsizei, const GLint *))
		GLCOREFUNC(void, glUniform4iv, (GLint, GLsizei, const GLint *))

		// vertex_shader
		#define GL_VERTEX_SHADER										0x8B31
		#define GL_MAX_VERTEX_UNIFORM_COMPONENTS						0x8B4A
		#define GL_MAX_VARYING_COMPONENTS								0x8B4B
		#define GL_MAX_VERTEX_ATTRIBS									0x8869
		#define GL_MAX_VERTEX_TEXTURE_IMAGE_UNITS						0x8B4C
		#define GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS						0x8B4D
		#define GL_PROGRAM_POINT_SIZE									0x8642

		GLCOREFUNC(void, glVertexAttribPointer, (GLuint, GLint, GLenum, GLboolean, GLsizei, const void *))
		GLCOREFUNC(void, glEnableVertexAttribArray, (GLuint))
		GLCOREFUNC(void, glDisableVertexAttribArray, (GLuint))


		// -------------------------------------------------------------------
		//
		// OpenGL 2.1 Core Features
		//
		// -------------------------------------------------------------------

		// pixel_buffer_object
		#define GL_PIXEL_PACK_BUFFER									0x88EB
		#define GL_PIXEL_UNPACK_BUFFER									0x88EC

		// texture_sRGB
		#define GL_SRGB													0x8C40
		#define GL_SRGB8												0x8C41
		#define GL_SRGB_ALPHA											0x8C42
		#define GL_SRGB8_ALPHA8											0x8C43
		#define GL_COMPRESSED_SRGB_S3TC_DXT1_EXT						0x8C4C
		#define GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT1_EXT					0x8C4D
		#define GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT3_EXT					0x8C4E
		#define GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT5_EXT					0x8C4F


		// -------------------------------------------------------------------
		//
		// OpenGL 3.0 Core Features
		//
		// -------------------------------------------------------------------

		GLCOREFUNC(void, glClearBufferiv, (GLenum, GLint, const GLint *))
		GLCOREFUNC(void, glClearBufferuiv, (GLenum, GLint, const GLuint *))
		GLCOREFUNC(void, glClearBufferfv, (GLenum, GLint, const GLfloat *))
		GLCOREFUNC(void, glClearBufferfi, (GLenum, GLint, GLfloat, GLint))

		// conditional_render
		#define GL_QUERY_WAIT											0x8E13
		#define GL_QUERY_NO_WAIT										0x8E14
		#define GL_QUERY_BY_REGION_WAIT									0x8E15
		#define GL_QUERY_BY_REGION_NO_WAIT								0x8E16

		GLCOREFUNC(void, glBeginConditionalRender, (GLuint, GLenum))
		GLCOREFUNC(void, glEndConditionalRender, (void))

		// framebuffer_blit
		#define GL_READ_FRAMEBUFFER										0x8CA8
		#define GL_DRAW_FRAMEBUFFER										0x8CA9

		GLCOREFUNC(void, glBlitFramebuffer, (GLint, GLint, GLint, GLint, GLint, GLint, GLint, GLint, GLbitfield, GLenum))

		// framebuffer_multisample
		#define GL_MAX_SAMPLES											0x8D57

		GLCOREFUNC(void, glRenderbufferStorageMultisample, (GLenum, GLsizei, GLenum, GLsizei, GLsizei))

		// framebuffer_object
		#define GL_FRAMEBUFFER											0x8D40
		#define GL_RENDERBUFFER											0x8D41
		#define GL_STENCIL_INDEX8										0x8D48
		#define GL_COLOR_ATTACHMENT0									0x8CE0
		#define GL_COLOR_ATTACHMENT1									0x8CE1
		#define GL_DEPTH_ATTACHMENT										0x8D00
		#define GL_STENCIL_ATTACHMENT									0x8D20
		#define GL_FRAMEBUFFER_COMPLETE									0x8CD5
		#define GL_MAX_COLOR_ATTACHMENTS								0x8CDF
		#define GL_MAX_RENDERBUFFER_SIZE								0x84E8
		#define GL_INVALID_FRAMEBUFFER_OPERATION						0x0506

		GLCOREFUNC(void, glBindRenderbuffer, (GLenum, GLuint))
		GLCOREFUNC(void, glDeleteRenderbuffers, (GLsizei, const GLuint *))
		GLCOREFUNC(void, glGenRenderbuffers, (GLsizei, GLuint *))
		GLCOREFUNC(void, glRenderbufferStorage, (GLenum, GLenum, GLsizei, GLsizei))
		GLCOREFUNC(void, glGetRenderbufferParameteriv, (GLenum, GLenum, GLint *))
		GLCOREFUNC(void, glBindFramebuffer, (GLenum, GLuint))
		GLCOREFUNC(void, glDeleteFramebuffers, (GLsizei, const GLuint *))
		GLCOREFUNC(void, glGenFramebuffers, (GLsizei, GLuint *))
		GLCOREFUNC(GLenum, glCheckFramebufferStatus, (GLenum))
		GLCOREFUNC(void, glFramebufferTexture1D, (GLenum, GLenum, GLenum, GLuint, GLint))
		GLCOREFUNC(void, glFramebufferTexture2D, (GLenum, GLenum, GLenum, GLuint, GLint))
		GLCOREFUNC(void, glFramebufferTexture3D, (GLenum, GLenum, GLenum, GLuint, GLint, GLint))
		GLCOREFUNC(void, glFramebufferRenderbuffer, (GLenum, GLenum, GLenum, GLuint))

		// framebuffer_sRGB
		#define GL_FRAMEBUFFER_SRGB										0x8DB9
		#define GL_FRAMEBUFFER_SRGB_CAPABLE								0x8DBA

		// gpu_shader4
		GLCOREFUNC(void, glBindFragDataLocation, (GLuint, GLuint, const GLchar *))

		// half_float_pixel
		#define GL_HALF_FLOAT											0x140B

		// map_buffer_range
		#define GL_MAP_READ_BIT											0x0001
		#define GL_MAP_WRITE_BIT										0x0002
		#define GL_MAP_INVALIDATE_RANGE_BIT								0x0004
		#define GL_MAP_INVALIDATE_BUFFER_BIT							0x0008
		#define GL_MAP_FLUSH_EXPLICIT_BIT								0x0010
		#define GL_MAP_UNSYNCHRONIZED_BIT								0x0020

		GLCOREFUNC(void *, glMapBufferRange, (GLenum, GLintptr, GLsizeiptr, GLbitfield))

		// packed_depth_stencil
		#define GL_DEPTH_STENCIL										0x84F9
		#define GL_UNSIGNED_INT_24_8									0x84FA
		#define GL_DEPTH24_STENCIL8										0x88F0

		// texture_array
		#define GL_TEXTURE_1D_ARRAY										0x8C18
		#define GL_TEXTURE_2D_ARRAY										0x8C1A
		#define GL_MAX_ARRAY_TEXTURE_LAYERS								0x88FF
		#define GL_COMPARE_REF_DEPTH_TO_TEXTURE							0x884E

		GLCOREFUNC(void, glFramebufferTextureLayer, (GLenum, GLenum, GLuint, GLint, GLint))

		// texture_compression_rgtc
		#define GL_COMPRESSED_RED_RGTC1									0x8DBB
		#define GL_COMPRESSED_SIGNED_RED_RGTC1							0x8DBC
		#define GL_COMPRESSED_RED_GREEN_RGTC2							0x8DBD
		#define GL_COMPRESSED_SIGNED_RED_GREEN_RGTC2					0x8DBE

		// texture_float
		#define GL_RGBA32F												0x8814
		#define GL_RGB32F												0x8815
		#define GL_ALPHA32F												0x8816
		#define GL_RGBA16F												0x881A
		#define GL_RGB16F												0x881B
		#define GL_ALPHA16F												0x881C

		// texture_rg
		#define GL_R8													0x8229
		#define GL_R16													0x822A
		#define GL_RG8													0x822B
		#define GL_RG16													0x822C
		#define GL_R16F													0x822D
		#define GL_R32F													0x822E
		#define GL_RG16F												0x822F
		#define GL_RG32F												0x8230
		#define GL_R8I													0x8231
		#define GL_R8UI													0x8232
		#define GL_R16I													0x8233
		#define GL_R16UI												0x8234
		#define GL_R32I													0x8235
		#define GL_R32UI												0x8236
		#define GL_RG8I													0x8237
		#define GL_RG8UI												0x8238
		#define GL_RG16I												0x8239
		#define GL_RG16UI												0x823A
		#define GL_RG32I												0x823B
		#define GL_RG32UI												0x823C
		#define GL_RG													0x8227
		#define GL_COMPRESSED_RED										0x8225
		#define GL_COMPRESSED_RG										0x8226
		#define GL_RG_INTEGER											0x8228

		// vertex_array_object
		GLCOREFUNC(void, glBindVertexArray, (GLuint))
		GLCOREFUNC(void, glDeleteVertexArrays, (GLsizei, const GLuint *))
		GLCOREFUNC(void, glGenVertexArrays, (GLsizei, GLuint *))


		// -------------------------------------------------------------------
		//
		// OpenGL 3.1 Core Features
		//
		// -------------------------------------------------------------------

		#define GL_MAJOR_VERSION										0x821B
		#define GL_MINOR_VERSION										0x821C
		#define GL_NUM_EXTENSIONS										0x821D

		GLEXTFUNC(const GLubyte *, glGetStringi, (GLenum, GLuint))

		// texture_rectangle
		#define GL_TEXTURE_RECTANGLE									0x84F5
		#define GL_MAX_RECTANGLE_TEXTURE_SIZE							0x84F8

		// uniform_buffer_object
		#define GL_UNIFORM_BUFFER										0x8A11
		#define GL_MAX_UNIFORM_BLOCK_SIZE								0x8A30
		#define GL_INVALID_INDEX										0xFFFFFFFFU

		GLCOREFUNC(GLuint, glGetUniformBlockIndex, (GLuint, const char *))
		GLCOREFUNC(void, glBindBufferBase, (GLenum, GLuint, GLuint))
		GLCOREFUNC(void, glUniformBlockBinding, (GLuint, GLuint, GLuint))


		// -------------------------------------------------------------------
		//
		// OpenGL 3.2 Core Features
		//
		// -------------------------------------------------------------------

		// depth_clamp
		#define GL_DEPTH_CLAMP											0x864F

		// geometry_shader4
		#define GL_GEOMETRY_SHADER										0x8DD9
		#define GL_GEOMETRY_VERTICES_OUT								0x8DDA
		#define GL_GEOMETRY_INPUT_TYPE									0x8DDB
		#define GL_GEOMETRY_OUTPUT_TYPE									0x8DDC
		#define GL_MAX_GEOMETRY_TEXTURE_IMAGE_UNITS						0x8C29
		#define GL_MAX_GEOMETRY_VARYING_COMPONENTS						0x8DDD
		#define GL_MAX_VERTEX_VARYING_COMPONENTS						0x8DDE
		#define GL_MAX_GEOMETRY_UNIFORM_COMPONENTS						0x8DDF
		#define GL_MAX_GEOMETRY_OUTPUT_VERTICES							0x8DE0
		#define GL_MAX_GEOMETRY_TOTAL_OUTPUT_COMPONENTS					0x8DE1
		#define GL_LINES_ADJACENCY										0xA
		#define GL_LINE_STRIP_ADJACENCY									0xB
		#define GL_TRIANGLES_ADJACENCY									0xC
		#define GL_TRIANGLE_STRIP_ADJACENCY								0xD

		GLCOREFUNC(void, glProgramParameteri, (GLuint, GLenum, GLint))

		// seamless_cube_map
		#define GL_TEXTURE_CUBE_MAP_SEAMLESS							0x884F


		// -------------------------------------------------------------------
		//
		// OpenGL 3.3 Core Features
		//
		// -------------------------------------------------------------------

		// blend_func_extended
		#define GL_SRC1_COLOR											0x88F9
		#define GL_SRC1_ALPHA											0x8589
		#define GL_ONE_MINUS_SRC1_COLOR									0x88FA
		#define GL_ONE_MINUS_SRC1_ALPHA									0x88FB
		#define GL_MAX_DUAL_SOURCE_DRAW_BUFFERS							0x88FC

		GLCOREFUNC(void, glBindFragDataLocationIndexed, (GLuint, GLuint, GLuint, const GLchar *))
		GLCOREFUNC(int, glGetFragDataIndex, (GLuint, const GLchar *))

		// instanced_arrays
		#define GL_VERTEX_ATTRIB_ARRAY_DIVISOR							0x88FE

		GLCOREFUNC(void, glDrawElementsInstanced, (GLenum, GLsizei, GLenum, const void *, GLsizei))
		GLCOREFUNC(void, glVertexAttribDivisor, (GLuint, GLuint))

		// texture_swizzle
		#define GL_TEXTURE_SWIZZLE_R									0x8E42
		#define GL_TEXTURE_SWIZZLE_G									0x8E43
		#define GL_TEXTURE_SWIZZLE_B									0x8E44
		#define GL_TEXTURE_SWIZZLE_A									0x8E45
		#define GL_TEXTURE_SWIZZLE_RGBA									0x8E46

		// timer_query
		#define GL_TIME_ELAPSED											0x88BF
		#define GL_TIMESTAMP											0x8E28

		GLCOREFUNC(void, glQueryCounter, (GLuint, GLenum))
		GLCOREFUNC(void, glGetQueryObjecti64v, (GLuint, GLenum, GLint64 *))
		GLCOREFUNC(void, glGetQueryObjectui64v, (GLuint, GLenum, GLuint64 *))


		// -------------------------------------------------------------------
		//
		// OpenGL 4.0 Core Features
		//
		// -------------------------------------------------------------------

		// sample_shading
		#define GL_SAMPLE_SHADING										0x8C36
		#define GL_MIN_SAMPLE_SHADING_VALUE								0x8C37

		GLCOREEXTFUNC(void, glMinSampleShading, (GLclampf), 0x0400, "glMinSampleShadingARB")

		// tessellation_shader
		#define GL_PATCHES												14
		#define GL_PATCH_VERTICES										0x8E72
		#define GL_PATCH_DEFAULT_INNER_LEVEL							0x8E73
		#define GL_PATCH_DEFAULT_OUTER_LEVEL							0x8E74
		#define GL_TESS_CONTROL_OUTPUT_VERTICES							0x8E75
		#define GL_TESS_GEN_MODE										0x8E76
		#define GL_TESS_GEN_SPACING										0x8E77
		#define GL_TESS_GEN_VERTEX_ORDER								0x8E78
		#define GL_TESS_GEN_POINT_MODE									0x8E79
		#define GL_ISOLINES												0x8E7A
		#define GL_FRACTIONAL_ODD										0x8E7B
		#define GL_FRACTIONAL_EVEN										0x8E7C
		#define GL_MAX_PATCH_VERTICES									0x8E7D
		#define GL_MAX_TESS_GEN_LEVEL									0x8E7E
		#define GL_MAX_TESS_CONTROL_UNIFORM_COMPONENTS					0x8E7F
		#define GL_MAX_TESS_EVALUATION_UNIFORM_COMPONENTS				0x8E80
		#define GL_MAX_TESS_CONTROL_TEXTURE_IMAGE_UNITS					0x8E81
		#define GL_MAX_TESS_EVALUATION_TEXTURE_IMAGE_UNITS				0x8E82
		#define GL_MAX_TESS_CONTROL_OUTPUT_COMPONENTS					0x8E83
		#define GL_MAX_TESS_PATCH_COMPONENTS							0x8E84
		#define GL_MAX_TESS_CONTROL_TOTAL_OUTPUT_COMPONENTS				0x8E85
		#define GL_MAX_TESS_EVALUATION_OUTPUT_COMPONENTS				0x8E86
		#define GL_MAX_TESS_CONTROL_UNIFORM_BLOCKS						0x8E89
		#define GL_MAX_TESS_EVALUATION_UNIFORM_BLOCKS					0x8E8A
		#define GL_MAX_TESS_CONTROL_INPUT_COMPONENTS					0x886C
		#define GL_MAX_TESS_EVALUATION_INPUT_COMPONENTS					0x886D
		#define GL_MAX_COMBINED_TESS_CONTROL_UNIFORM_COMPONENTS			0x8E1E
		#define GL_MAX_COMBINED_TESS_EVALUATION_UNIFORM_COMPONENTS		0x8E1F
		#define GL_TESS_EVALUATION_SHADER								0x8E87
		#define GL_TESS_CONTROL_SHADER									0x8E88

		GLCOREEXTFUNC(void, glPatchParameteri, (GLenum, GLint), 0x0400, "glPatchParameteriARB")
		GLCOREEXTFUNC(void, glPatchParameterfv, (GLenum, const GLfloat *), 0x0400, "glPatchParameterfvARB")


		// -------------------------------------------------------------------
		//
		// OpenGL 4.1 Core Features
		//
		// -------------------------------------------------------------------

		// get_program_binary
		#define GL_PROGRAM_BINARY_RETRIEVABLE_HINT						0x8257
		#define GL_PROGRAM_BINARY_LENGTH								0x8741
		#define GL_NUM_PROGRAM_BINARY_FORMATS							0x87FE
		#define GL_PROGRAM_BINARY_FORMATS								0x87FF

		GLEXTFUNC(void, glGetProgramBinary, (GLuint, GLsizei, GLsizei *, GLenum *, void *))
		GLEXTFUNC(void, glProgramBinary, (GLuint, GLenum, const void *, GLsizei))


		// -------------------------------------------------------------------
		//
		// OpenGL 4.2 Core Features
		//
		// -------------------------------------------------------------------

		// texture_storage
		GLCOREFUNC(void, glTexStorage2D, (GLenum, GLsizei, GLenum, GLsizei, GLsizei))
		GLCOREFUNC(void, glTexStorage3D, (GLenum, GLsizei, GLenum, GLsizei, GLsizei, GLsizei))
		GLEXTFUNC(void, glTextureStorage2DEXT, (GLuint, GLenum, GLsizei, GLenum, GLsizei, GLsizei))
		GLEXTFUNC(void, glTextureStorage3DEXT, (GLuint, GLenum, GLsizei, GLenum, GLsizei, GLsizei, GLsizei))


		// -------------------------------------------------------------------
		//
		// OpenGL 4.3 Core Features
		//
		// -------------------------------------------------------------------

		// invalidate_subdata
		GLCOREFUNC(void, glInvalidateTexImage, (GLuint, GLint))
		GLCOREFUNC(void, glInvalidateBufferData, (GLuint))
		GLCOREFUNC(void, glInvalidateFramebuffer, (GLenum, GLsizei, const GLenum *))


		// -------------------------------------------------------------------
		//
		// OpenGL 4.4 Core Features
		//
		// -------------------------------------------------------------------

		// multi_bind
		GLCOREFUNC(void, glBindTextures, (GLuint, GLsizei, const GLuint *))

		// query_buffer_object
		#define GL_QUERY_RESULT_NO_WAIT									0x9194
		#define GL_QUERY_BUFFER											0x9192
		#define GL_QUERY_BUFFER_BARRIER_BIT								0x00008000


		// -------------------------------------------------------------------
		//
		// OpenGL Extensions
		//
		// -------------------------------------------------------------------

		#if C4DEBUG

			// GL_ARB_debug_context
			#define GL_DEBUG_OUTPUT_SYNCHRONOUS_ARB                     0x8242
			#define GL_MAX_DEBUG_MESSAGE_LENGTH_ARB                     0x9143
			#define GL_MAX_DEBUG_LOGGED_MESSAGES_ARB                    0x9144
			#define GL_DEBUG_LOGGED_MESSAGES_ARB                        0x9145
			#define GL_DEBUG_NEXT_LOGGED_MESSAGE_LENGTH_ARB             0x8243
			#define GL_DEBUG_CALLBACK_FUNCTION_ARB                      0x8244
			#define GL_DEBUG_CALLBACK_USER_PARAM_ARB                    0x8245
			#define GL_DEBUG_SOURCE_API_ARB                             0x8246
			#define GL_DEBUG_SOURCE_WINDOW_SYSTEM_ARB                   0x8247
			#define GL_DEBUG_SOURCE_SHADER_COMPILER_ARB                 0x8248
			#define GL_DEBUG_SOURCE_THIRD_PARTY_ARB                     0x8249
			#define GL_DEBUG_SOURCE_APPLICATION_ARB                     0x824A
			#define GL_DEBUG_SOURCE_OTHER_ARB                           0x824B
			#define GL_DEBUG_TYPE_ERROR_ARB                             0x824C
			#define GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR_ARB               0x824D
			#define GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR_ARB                0x824E
			#define GL_DEBUG_TYPE_PORTABILITY_ARB                       0x824F
			#define GL_DEBUG_TYPE_PERFORMANCE_ARB                       0x8250
			#define GL_DEBUG_TYPE_OTHER_ARB                             0x8251
			#define GL_DEBUG_SEVERITY_HIGH_ARB                          0x9146
			#define GL_DEBUG_SEVERITY_MEDIUM_ARB                        0x9147
			#define GL_DEBUG_SEVERITY_LOW_ARB                           0x9148

			typedef void (OPENGLAPI *GLdebugProcARB)(GLenum, GLenum, GLuint, GLenum, GLsizei, const GLchar *, void *);

			GLEXTFUNC(void, glDebugMessageControlARB, (GLenum, GLenum, GLenum, GLsizei, const GLuint *, GLboolean))
			GLEXTFUNC(void, glDebugMessageInsertARB, (GLenum, GLenum, GLuint, GLenum, GLsizei, const GLchar *))
			GLEXTFUNC(void, glDebugMessageCallbackARB, (GLdebugProcARB, void *))
			GLEXTFUNC(GLuint, glGetDebugMessageLogARB, (GLuint, GLsizei, GLenum *, GLenum *, GLuint *, GLenum *, GLsizei *, GLchar *))
			GLEXTFUNC(void, glGetPointerv, (GLenum, void **))

		#endif

		// GL_EXT_depth_bounds_test
		#define GL_DEPTH_BOUNDS_TEST_EXT								0x8890

		GLEXTFUNC(void, glDepthBoundsEXT, (GLclampd, GLclampd))

		// GL_EXT_direct_state_access
		GLEXTFUNC(void, glBindMultiTextureEXT, (GLenum, GLenum, GLuint))
		GLEXTFUNC(void, glTextureParameteriEXT, (GLuint, GLenum, GLenum, GLint))
		GLEXTFUNC(void, glTextureParameterivEXT, (GLuint, GLenum, GLenum, const GLint *))
		GLEXTFUNC(void, glTextureParameterfEXT, (GLuint, GLenum, GLenum, GLfloat))
		GLEXTFUNC(void, glTextureParameterfvEXT, (GLuint, GLenum, GLenum, const GLfloat *))
		GLEXTFUNC(void, glTextureImage2DEXT, (GLuint, GLenum, GLint, GLint, GLsizei, GLsizei, GLint, GLenum, GLenum, const void *))
		GLEXTFUNC(void, glTextureSubImage2DEXT, (GLuint, GLenum, GLint, GLint, GLint, GLsizei, GLsizei, GLenum, GLenum, const void *))
		GLEXTFUNC(void, glTextureImage3DEXT, (GLuint, GLenum, GLint, GLint, GLsizei, GLsizei, GLsizei, GLint, GLenum, GLenum, const void *))
		GLEXTFUNC(void, glTextureSubImage3DEXT, (GLuint, GLenum, GLint, GLint, GLint, GLint, GLsizei, GLsizei, GLsizei, GLenum, GLenum, const void *))
		GLEXTFUNC(void, glCompressedTextureImage2DEXT, (GLuint, GLenum, GLint, GLenum, GLsizei, GLsizei, GLint, GLsizei, const void *))
		GLEXTFUNC(void, glCompressedTextureImage3DEXT, (GLuint, GLenum, GLint, GLenum, GLsizei, GLsizei, GLsizei, GLint, GLsizei, const void *))
		GLEXTFUNC(void, glCopyTextureSubImage2DEXT, (GLuint, GLenum, GLint, GLint, GLint, GLint, GLint, GLsizei, GLsizei))
		GLEXTFUNC(void, glNamedBufferDataEXT, (GLuint, GLsizeiptr, const void *, GLenum))
		GLEXTFUNC(void, glNamedBufferSubDataEXT, (GLuint, GLintptr, GLsizeiptr, const void *))
		GLEXTFUNC(void, glGetNamedBufferSubDataEXT, (GLuint, GLintptr, GLsizeiptr, void *))
		GLEXTFUNC(void *, glMapNamedBufferRangeEXT, (GLuint, GLintptr, GLsizeiptr, GLbitfield))
		GLEXTFUNC(GLboolean, glUnmapNamedBufferEXT, (GLuint))
		GLEXTFUNC(void, glProgramUniform4fvEXT, (GLuint, GLint, GLsizei, const GLfloat *))
		GLEXTFUNC(void, glVertexArrayVertexAttribOffsetEXT, (GLuint, GLuint, GLuint, GLint, GLenum, GLboolean, GLsizei, GLintptr))
		GLEXTFUNC(void, glVertexArrayVertexAttribDivisorEXT, (GLuint, GLuint, GLuint))
		GLEXTFUNC(void, glEnableVertexArrayAttribEXT, (GLuint, GLuint))
		GLEXTFUNC(void, glDisableVertexArrayAttribEXT, (GLuint, GLuint))
		GLEXTFUNC(void, glNamedRenderbufferStorageEXT, (GLuint, GLenum, GLsizei, GLsizei))
		GLEXTFUNC(void, glNamedRenderbufferStorageMultisampleEXT, (GLuint, GLsizei, GLenum, GLsizei, GLsizei))
		GLEXTFUNC(void, glNamedRenderbufferStorageMultisampleCoverageEXT, (GLuint, GLsizei, GLsizei, GLenum, GLsizei, GLsizei))
		GLEXTFUNC(GLenum, glCheckNamedFramebufferStatusEXT, (GLuint, GLenum))
		GLEXTFUNC(void, glNamedFramebufferTexture2DEXT, (GLuint, GLenum, GLenum, GLuint, GLint))
		GLEXTFUNC(void, glNamedFramebufferTexture3DEXT, (GLuint, GLenum, GLenum, GLuint, GLint, GLint))
		GLEXTFUNC(void, glNamedFramebufferRenderbufferEXT, (GLuint, GLenum, GLenum, GLuint))
		GLEXTFUNC(void, glFramebufferDrawBufferEXT, (GLuint, GLenum))
		GLEXTFUNC(void, glFramebufferDrawBuffersEXT, (GLuint, GLsizei, const GLenum *))
		GLEXTFUNC(void, glFramebufferReadBufferEXT, (GLuint, GLenum))
		GLEXTFUNC(void, glNamedFramebufferTextureEXT, (GLuint, GLenum, GLuint, GLint))
		GLEXTFUNC(void, glNamedFramebufferTextureLayerEXT, (GLuint, GLenum, GLuint, GLint, GLint))
		GLEXTFUNC(void, glTextureRenderbufferEXT, (GLuint, GLenum, GLuint))

		// GL_EXT_polygon_offset_clamp
		#define GL_POLYGON_OFFSET_CLAMP_EXT								0x8E1B

		GLEXTFUNC(void, glPolygonOffsetClampEXT, (GLfloat, GLfloat, GLfloat))

		// GL_EXT_texture_compression_s3tc
		#define GL_COMPRESSED_RGB_S3TC_DXT1_EXT							0x83F0
		#define GL_COMPRESSED_RGBA_S3TC_DXT1_EXT						0x83F1
		#define GL_COMPRESSED_RGBA_S3TC_DXT3_EXT						0x83F2
		#define GL_COMPRESSED_RGBA_S3TC_DXT5_EXT						0x83F3

		// GL_EXT_texture_filter_anisotropic
		#define GL_TEXTURE_MAX_ANISOTROPY_EXT							0x84FE
		#define GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT						0x84FF

		// GL_EXT_texture_mirror_clamp
		#define GL_MIRROR_CLAMP_EXT										0x8742
		#define GL_MIRROR_CLAMP_TO_EDGE									0x8743
		#define GL_MIRROR_CLAMP_TO_BORDER_EXT							0x8912

		// GL_NV_explicit_multisample
		#define GL_SAMPLE_POSITION_NV									0x8E50
		#define GL_SAMPLE_MASK_NV										0x8E51
		#define GL_SAMPLE_MASK_VALUE_NV									0x8E52
		#define GL_MAX_SAMPLE_MASK_WORDS_NV								0x8E59
		#define GL_TEXTURE_RENDERBUFFER_NV								0x8E55
		#define GL_SAMPLER_RENDERBUFFER_NV								0x8E56
		#define GL_INT_SAMPLER_RENDERBUFFER_NV							0x8E57
		#define GL_UNSIGNED_INT_SAMPLER_RENDERBUFFER_NV					0x8E58

		GLEXTFUNC(void, glGetBooleanIndexedvEXT, (GLenum, GLuint, GLboolean *))
		GLEXTFUNC(void, glGetIntegerIndexedvEXT, (GLenum, GLuint, GLint *))
		GLEXTFUNC(void, glGetMultisamplefvNV, (GLenum, GLuint, GLfloat *))
		GLEXTFUNC(void, glSampleMaskIndexedNV, (GLuint, GLbitfield))
		GLEXTFUNC(void, glTexRenderbufferNV, (GLenum, GLuint))

		// GL_NV_framebuffer_multisample_coverage
		#define GL_RENDERBUFFER_COVERAGE_SAMPLES_NV						0x8CAB
		#define GL_RENDERBUFFER_COLOR_SAMPLES_NV						0x8E10
		#define GL_MAX_MULTISAMPLE_COVERAGE_MODES_NV					0x8E11
		#define GL_MULTISAMPLE_COVERAGE_MODES_NV						0x8E12

		GLEXTFUNC(void, glRenderbufferStorageMultisampleCoverageNV, (GLenum, GLsizei, GLsizei, GLenum, GLsizei, GLsizei))

		// GL_NV_shader_buffer_load
		#define GL_BUFFER_GPU_ADDRESS_NV								0x8F1D
		#define GL_GPU_ADDRESS_NV										0x8F34
		#define GL_MAX_SHADER_BUFFER_ADDRESS_NV							0x8F35

		GLEXTFUNC(void, glMakeNamedBufferResidentNV, (GLuint, GLenum))
		GLEXTFUNC(void, glMakeNamedBufferNonResidentNV, (GLuint))
		GLEXTFUNC(void, glGetNamedBufferParameterui64vNV, (GLuint, GLenum, GLuint64 *))
		GLEXTFUNC(void, glProgramUniformui64NV, (GLuint, GLint, GLuint64))
		GLEXTFUNC(void, glProgramUniformui64vNV, (GLuint, GLint, GLsizei, const GLuint64 *))

		// GL_NV_vertex_buffer_unified_memory
		#define GL_VERTEX_ATTRIB_ARRAY_UNIFIED_NV						0x8F1E
		#define GL_ELEMENT_ARRAY_UNIFIED_NV								0x8F1F
		#define GL_VERTEX_ATTRIB_ARRAY_ADDRESS_NV						0x8F20
		#define GL_ELEMENT_ARRAY_ADDRESS_NV								0x8F29

		GLEXTFUNC(void, glBufferAddressRangeNV, (GLenum, GLuint, GLuint64, GLsizeiptr))
		GLEXTFUNC(void, glVertexAttribFormatNV, (GLuint, GLint, GLenum, GLboolean, GLsizei))
		GLEXTFUNC(void, glVertexAttribIFormatNV, (GLuint, GLint, GLenum, GLsizei))
		GLEXTFUNC(void, glGetIntegerui64i_vNV, (GLenum, GLuint, GLuint64 *))

	#endif


	#if C4GLES //[ MOBILE

		// -- Mobile code hidden --

	#endif //]


	#if C4WINDOWS

		// WGL_ARB_extensions_string
		GLEXTFUNC(const char *, wglGetExtensionsStringARB, (HDC))

		// WGL_ARB_create_context
		#define WGL_CONTEXT_MAJOR_VERSION_ARB							0x2091
		#define WGL_CONTEXT_MINOR_VERSION_ARB							0x2092
		#define WGL_CONTEXT_FLAGS_ARB									0x2094
		#define WGL_CONTEXT_PROFILE_MASK_ARB							0x9126
		#define WGL_CONTEXT_DEBUG_BIT_ARB								0x0001
		#define WGL_CONTEXT_CORE_PROFILE_BIT_ARB						0x00000001

		GLEXTFUNC(HGLRC, wglCreateContextAttribsARB, (HDC, HGLRC, const int *))

		// WGL_ARB_pixel_format
		#define WGL_DRAW_TO_WINDOW_ARB									0x2001
		#define WGL_ACCELERATION_ARB									0x2003
		#define WGL_SUPPORT_OPENGL_ARB									0x2010
		#define WGL_DOUBLE_BUFFER_ARB									0x2011
		#define WGL_PIXEL_TYPE_ARB										0x2013
		#define WGL_COLOR_BITS_ARB										0x2014
		#define WGL_ALPHA_BITS_ARB										0x201B
		#define WGL_DEPTH_BITS_ARB										0x2022
		#define WGL_STENCIL_BITS_ARB									0x2023
		#define WGL_FULL_ACCELERATION_ARB								0x2027
		#define WGL_TYPE_RGBA_ARB										0x202B

		GLEXTFUNC(BOOL, wglChoosePixelFormatARB, (HDC, const int *, const FLOAT *, UINT, int *, UINT *))

		// WGL_ARB_framebuffer_sRGB
		#define WGL_FRAMEBUFFER_SRGB_CAPABLE_ARB						0x20A9

		// WGL_EXT_swap_control
		GLEXTFUNC(BOOL, wglSwapIntervalEXT, (int))

	#elif C4LINUX

		// GLX_ARB_create_context_profile
		#define GLX_CONTEXT_MAJOR_VERSION_ARB							0x2091
		#define GLX_CONTEXT_MINOR_VERSION_ARB							0x2092
		#define GLX_CONTEXT_FLAGS_ARB									0x2094
		#define GLX_CONTEXT_PROFILE_MASK_ARB							0x9126
		#define GLX_CONTEXT_CORE_PROFILE_BIT_ARB						0x00000001

		GLEXTFUNC(GLXContext, glXCreateContextAttribsARB, (::Display *, GLXFBConfig, GLXContext, Bool, const int *))

		// GLX_EXT_swap_control
		GLEXTFUNC(void, glXSwapIntervalEXT, (::Display *, GLXDrawable, int))

	#endif


	struct GraphicsCapabilities;


	#if C4WINDOWS

		#define GLGETCOREFUNC(name) *reinterpret_cast<PROC *>(&name) = wglGetProcAddress(#name)
		#define GLGETEXTFUNC(name) *reinterpret_cast<PROC *>(&name) = wglGetProcAddress(#name)
		#define GLGETCOREEXTFUNC(name) GetCoreExtFuncAddress_##name(version)

		void InitializeOpenglExtensions(GraphicsCapabilities *capabilities);

	#elif C4MACOS

		#define GLGETCOREFUNC(name) *reinterpret_cast<void **>(&name) = Engine::GetBundleFunctionAddress(openglBundle, #name)
		#define GLGETEXTFUNC(name) *reinterpret_cast<void **>(&name) = Engine::GetBundleFunctionAddress(openglBundle, #name)
		#define GLGETCOREEXTFUNC(name) GetCoreExtFuncAddress_##name(openglBundle, version)

		void InitializeOpenglExtensions(GraphicsCapabilities *capabilities, CFBundleRef openglBundle);

	#elif C4LINUX

		#define GLGETCOREFUNC(name) *reinterpret_cast<void (**)()>(&name) = glXGetProcAddress(reinterpret_cast<const GLubyte *>(#name))
		#define GLGETEXTFUNC(name) *reinterpret_cast<void (**)()>(&name) = glXGetProcAddress(reinterpret_cast<const GLubyte *>(#name))
		#define GLGETCOREEXTFUNC(name) GetCoreExtFuncAddress_##name(version)

		void InitializeOpenglExtensions(GraphicsCapabilities *capabilities);

	#endif
}


#endif

// ZYUQURM
