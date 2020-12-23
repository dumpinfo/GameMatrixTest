 

#ifndef C4Render_h
#define C4Render_h


#include "C4Packing.h"


#if C4OPENGL

	#include "C4OpenGL.h"
	#include "C4Computation.h"

#elif C4CONSOLE //[ CONSOLE

	// -- Console code hidden --

#endif //]


namespace C4
{
	#if C4GLCORE

		#define C4RENDER_TEXTURE_RECTANGLE		1
		#define C4RENDER_TEXTURE_ARRAY			1
		#define C4RENDER_OCCLUSION_QUERY		1
		#define C4RENDER_TIMER_QUERY			1

	#elif C4GLES3 //[ MOBILE

		// -- Mobile code hidden --

	#elif C4GLES //[ MOBILE

		// -- Mobile code hidden --

	#elif C4PS4 //[ PS4

		// -- PS4 code hidden --

	#elif C4PS3 //[ PS3

		// -- PS3 code hidden --

	#endif //]


	enum
	{
		kMaxShaderTexcoordCount			= 13,
		kMaxOcclusionQueryCount			= 256,
		kMaxShaderTextureCount			= 14,
		kMaxGlobalTextureCount			= 7
	};


	#define SHADER_TEXCOORD_COUNT		"13"
	#define OCCLUSION_QUERY_COUNT		"256"


	static_assert((kMaxOcclusionQueryCount & (kMaxOcclusionQueryCount - 1)) == 0, "kMaxOcclusionQueryCount must be a power of 2");


	// If the vertex attrib order is changed, then the tables in the
	// Render::VertexDataObject::SetVertexAttribArray() function
	// must also be updated for each platform.

	enum
	{
		kVertexAttribPosition0			= 0,
		kVertexAttribTexture0			= 1,
		kVertexAttribNormal				= 2,
		kVertexAttribTangent			= 3,
		kVertexAttribRadius				= 3,
		kVertexAttribOffset				= 3,
		kVertexAttribColor0				= 4,
		kVertexAttribColor1				= 5,
		kVertexAttribColor2				= 6,
		kVertexAttribTexture1			= 7,
		kVertexAttribPosition1			= 8,
		kVertexAttribVelocity			= 9,
		kVertexAttribCount				= 10
	};


	#define VERTEX_ATTRIB_POSITION0		"0"
	#define VERTEX_ATTRIB_TEXTURE0		"1"
	#define VERTEX_ATTRIB_NORMAL		"2"
	#define VERTEX_ATTRIB_TANGENT		"3"
	#define VERTEX_ATTRIB_RADIUS		"3"
	#define VERTEX_ATTRIB_OFFSET		"3"
	#define VERTEX_ATTRIB_COLOR0		"4"
	#define VERTEX_ATTRIB_COLOR1		"5"
	#define VERTEX_ATTRIB_COLOR2		"6"
	#define VERTEX_ATTRIB_TEXTURE1		"7"
	#define VERTEX_ATTRIB_POSITION1		"8"
	#define VERTEX_ATTRIB_VELOCITY		"9"
	#define VERTEX_ATTRIB_COUNT			"10"


	struct Render 
	{
		enum
		{ 
			kMaxTextureUnitCount		= 16,
			kMaxVertexParamCount		= 48, 
			kMaxFragmentParamCount		= 12,
			kMaxGeometryParamCount		= 7,
			kMaxUniversalParamCount		= 24 
		};
 
 
		enum
		{
			kTextureTarget2D,
			kTextureTarget3D, 
			kTextureTargetRectangle,
			kTextureTargetCube,
			kTextureTargetArray2D,
			kTextureTargetCount
		};


		enum
		{
			kTextureRGBX8,
			kTextureRGBA8,
			kTextureBGRX8,
			kTextureBGRA8,
			kTextureR8,
			kTextureRG8,
			kTextureL8,
			kTextureLA8,
			kTextureI8,
			kTextureDepth,
			kTextureBC1,
			kTextureBC3,
			kTextureRenderBufferR8,
			kTextureRenderBufferRGB8,
			kTextureRenderBufferRGBA8,
			kTextureRenderBufferRG16F,
			kTextureRenderBufferRGBA16F,
			kTextureFormatCount
		};


		enum
		{
			kTextureEncodingLinear,
			kTextureEncodingSrgb,
			kTextureEncodingCount
		};


		enum
		{
			kTextureBorderTransparent,
			kTextureBorderBlack,
			kTextureBorderWhite
		};


		class VertexBufferObject;


		typedef void Decompressor(const unsigned_int8 *, unsigned_int32, void *);


		struct TextureImageData
		{
			const void				*image;
			unsigned_int32			size;
			Decompressor			*decompressor;
		};


		struct TextureUploadData
		{
			unsigned_int32			*memorySize;

			unsigned_int16			format;
			unsigned_int16			encoding;

			unsigned_int32			width;
			unsigned_int32			height;

			union
			{
				unsigned_int32		depth;
				unsigned_int32		rowLength;
			};

			int32					mipmapCount;
			TextureImageData		imageData[66];
		};


		struct TextureAllocationData
		{
			unsigned_int32			*memorySize;

			unsigned_int16			format;
			unsigned_int16			encoding;

			unsigned_int32			width;
			unsigned_int32			height;
			unsigned_int32			depth;

			bool					renderBuffer;
		};


		struct BufferUploadData
		{
			unsigned_int32			offset;
			unsigned_int32			size;
			const void				*data;
		};


		static void Initialize(void);
		static void Terminate(void);


		#if C4OPENGL

			enum
			{
				kAlphaNever						= GL_NEVER,
				kAlphaLess						= GL_LESS,
				kAlphaEqual						= GL_EQUAL,
				kAlphaLessEqual					= GL_LEQUAL,
				kAlphaGreater					= GL_GREATER,
				kAlphaNotEqual					= GL_NOTEQUAL,
				kAlphaGreaterEqual				= GL_GEQUAL,
				kAlphaAlways					= GL_ALWAYS
			};


			enum
			{
				kDepthNever						= GL_NEVER,
				kDepthLess						= GL_LESS,
				kDepthEqual						= GL_EQUAL,
				kDepthLessEqual					= GL_LEQUAL,
				kDepthGreater					= GL_GREATER,
				kDepthNotEqual					= GL_NOTEQUAL,
				kDepthGreaterEqual				= GL_GEQUAL,
				kDepthAlways					= GL_ALWAYS
			};


			enum
			{
				kStencilNever					= GL_NEVER,
				kStencilLess					= GL_LESS,
				kStencilEqual					= GL_EQUAL,
				kStencilLessEqual				= GL_LEQUAL,
				kStencilGreater					= GL_GREATER,
				kStencilNotEqual				= GL_NOTEQUAL,
				kStencilGreaterEqual			= GL_GEQUAL,
				kStencilAlways					= GL_ALWAYS
			};


			enum
			{
				kStencilZero					= GL_ZERO,
				kStencilInvert					= GL_INVERT,
				kStencilKeep					= GL_KEEP,
				kStencilReplace					= GL_REPLACE,
				kStencilIncr					= GL_INCR,
				kStencilDecr					= GL_DECR,
				kStencilIncrWrap				= GL_INCR_WRAP,
				kStencilDecrWrap				= GL_DECR_WRAP
			};


			enum
			{
				kBlendZero						= GL_ZERO,
				kBlendOne						= GL_ONE,
				kBlendSrcColor					= GL_SRC_COLOR,
				kBlendInvSrcColor				= GL_ONE_MINUS_SRC_COLOR,
				kBlendSrcAlpha					= GL_SRC_ALPHA,
				kBlendInvSrcAlpha				= GL_ONE_MINUS_SRC_ALPHA,
				kBlendDstAlpha					= GL_DST_ALPHA,
				kBlendInvDstAlpha				= GL_ONE_MINUS_DST_ALPHA,
				kBlendDstColor					= GL_DST_COLOR,
				kBlendInvDstColor				= GL_ONE_MINUS_DST_COLOR,
				kBlendConstColor				= GL_CONSTANT_COLOR,
				kBlendInvConstColor				= GL_ONE_MINUS_CONSTANT_COLOR,
				kBlendConstAlpha				= GL_CONSTANT_ALPHA,
				kBlendInvConstAlpha				= GL_ONE_MINUS_CONSTANT_ALPHA
			};


			enum
			{
				kBlendEquationAdd				= GL_FUNC_ADD,
				kBlendEquationMin				= GL_MIN,
				kBlendEquationMax				= GL_MAX,
				kBlendEquationSubtract			= GL_FUNC_SUBTRACT,
				kBlendEquationReverseSubtract	= GL_FUNC_REVERSE_SUBTRACT
			};


			enum
			{
				kCullFront						= GL_FRONT,
				kCullBack						= GL_BACK,
				kCullFrontAndBack				= GL_FRONT_AND_BACK
			};


			enum
			{
				kFrontCW						= GL_CW,
				kFrontCCW						= GL_CCW
			};


			enum
			{
				kShadowNever					= GL_NEVER,
				kShadowLess						= GL_LESS,
				kShadowEqual					= GL_EQUAL,
				kShadowLessEqual				= GL_LEQUAL,
				kShadowGreater					= GL_GREATER,
				kShadowNotEqual					= GL_NOTEQUAL,
				kShadowGreaterEqual				= GL_GEQUAL,
				kShadowAlways					= GL_ALWAYS
			};


			enum
			{
				kTextureCompareNone				= GL_NONE,
				kTextureCompareReference		= GL_COMPARE_REF_TO_TEXTURE
			};


			enum
			{
				kPrimitivePoints				= GL_POINTS,
				kPrimitiveLines					= GL_LINES,
				kPrimitiveLineLoop				= GL_LINE_LOOP,
				kPrimitiveLineStrip				= GL_LINE_STRIP,
				kPrimitiveTriangles				= GL_TRIANGLES,
				kPrimitiveTriangleStrip			= GL_TRIANGLE_STRIP,
				kPrimitiveTriangleFan			= GL_TRIANGLE_FAN
			};


			enum
			{
				kVertexFloat					= GL_FLOAT,
				kVertexUnsignedByte				= GL_UNSIGNED_BYTE,
				kVertexSignedShort				= GL_SHORT
			};


			enum
			{
				kRenderBufferRGBA8				= GL_RGBA8,
				kRenderBufferRGBA16F			= GL_RGBA16F,
				kRenderBufferDepth				= GL_DEPTH_COMPONENT24,
				kRenderBufferDepthStencil		= GL_DEPTH24_STENCIL8
			};


			enum
			{
				kWrapRepeat						= GL_REPEAT,
				kWrapMirrorRepeat				= GL_MIRRORED_REPEAT,
				kWrapClampToEdge				= GL_CLAMP_TO_EDGE,

				#if C4GLCORE

					kWrapClampToBorder			= GL_CLAMP_TO_BORDER,
					kWrapMirrorClampToEdge		= GL_MIRROR_CLAMP_TO_EDGE,
					kWrapMirrorClampToBorder	= GL_MIRROR_CLAMP_TO_BORDER_EXT,
					kWrapMirrorClamp			= GL_MIRROR_CLAMP_EXT

				#else //[ MOBILE

					// -- Mobile code hidden --

				#endif //]
			};


			enum
			{
				kFilterNearest					= GL_NEAREST,
				kFilterLinear					= GL_LINEAR,
				kFilterNearestMipmapNearest		= GL_NEAREST_MIPMAP_NEAREST,
				kFilterLinearMipmapNearest		= GL_LINEAR_MIPMAP_NEAREST,
				kFilterNearestMipmapLinear		= GL_NEAREST_MIPMAP_LINEAR,
				kFilterLinearMipmapLinear		= GL_LINEAR_MIPMAP_LINEAR
			};


			enum
			{
				kBlitFilterPoint				= GL_NEAREST,
				kBlitFilterBilinear				= GL_LINEAR
			};


			enum
			{
				kVertexBufferTargetAttribute	= GL_ARRAY_BUFFER,
				kVertexBufferTargetIndex		= GL_ELEMENT_ARRAY_BUFFER
			};


			enum
			{
				kVertexBufferUsageStatic		= GL_STATIC_DRAW,
				kVertexBufferUsageDynamic		= GL_DYNAMIC_DRAW
			};


			enum
			{
				kParameterBufferUniversal		= 0,
				kParameterBufferQuery			= 1
			};

		#elif C4PS4 //[ PS4

			// -- PS4 code hidden --

		#elif C4PS3 //[ PS3

			// -- PS3 code hidden --

		#endif //]


		class VertexBufferObject
		{
			friend struct Render;

			private:

				unsigned_int32		bufferTarget;
				unsigned_int32		bufferUsage;
				unsigned_int32		bufferSize;

				#if C4OPENGL

					GLuint			bufferIdentifier;
					char			*staticBuffer;

				#elif C4CONSOLE //[ CONSOLE

					// -- Console code hidden --

				#endif //]

			protected:

				void SetVertexBufferSize(unsigned_int32 size)
				{
					bufferSize = size;
				}

				void ReadBuffer(unsigned_int32 offset, unsigned_int32 size, void *data) const;

			public:

				VertexBufferObject(unsigned_int32 target, unsigned_int32 usage)
				{
					bufferTarget = target;
					bufferUsage = usage;
					bufferSize = 0;

					#if C4OPENGL

						bufferIdentifier = 0;

					#elif C4CONSOLE //[ CONSOLE

						// -- Console code hidden --

					#endif //]
				}

				void Construct(void);
				void Destruct(void);

				unsigned_int32 GetVertexBufferSize(void) const
				{
					return (bufferSize);
				}

				void AllocateStorage(const void *data = nullptr);

				C4API volatile void *BeginUpdate(void);
				C4API void EndUpdate(void);
				C4API void UpdateBuffer(unsigned_int32 offset, unsigned_int32 size, const void *data);

				void BeginUpdateSync(volatile void **const *ptr);
				void EndUpdateSync(const void *);
				void UpdateBufferSync(const BufferUploadData *uploadData);

				template <typename type> volatile type *BeginUpdate(void)
				{
					return (static_cast<volatile type *>(BeginUpdate()));
				}
		};


		struct VertexAttribData
		{
			#if C4OPENGL

				GLuint						buffer;
				unsigned_int32				offset;
				unsigned_int16				type;
				unsigned_int8				size;
				unsigned_int8				stride;

			#elif C4PS4 //[ PS4

				// -- PS4 code hidden --

			#elif C4PS3 //[ PS3

				// -- PS3 code hidden --

			#endif //]
		};


		class VertexDataObject
		{
			friend struct Render;

			private:

				bool							validFlag;
				unsigned_int16					vertexArrayMask;

				#if C4OPENGL

					GLuint						indexBuffer;

				#elif C4CONSOLE //[ CONSOLE

					// -- Console code hidden --

				#endif //]

				VertexAttribData				vertexAttribData[kVertexAttribCount];

			public:

				VertexDataObject();
				~VertexDataObject();

				bool GetValidFlag(void)
				{
					bool flag = validFlag;
					validFlag = true;
					return (flag);
				}

				void SetVertexIndexBuffer(const VertexBufferObject *buffer)
				{
					#if C4OPENGL

						indexBuffer = buffer->bufferIdentifier;

					#elif C4CONSOLE //[ CONSOLE

						// -- Console code hidden --

					#endif //]
				}

				#if C4OPENGL

					void Bind(void) const;
					void Invalidate(void);

				#elif C4CONSOLE //[ CONSOLE

					// -- Console code hidden --

				#endif //]

				void SetVertexAttribArray(unsigned_int32 index, const VertexBufferObject *buffer, unsigned_int32 offset, int32 count, int32 stride);
		};


		#if C4OPENGL

			class ParameterBufferObject
			{
				friend struct Render;

				private:

					GLuint				bufferIdentifier;
					unsigned_int32		bufferSize;

				public:

					ParameterBufferObject(unsigned_int32 index);
					~ParameterBufferObject();

					volatile void *BeginUpdate(void)
					{
						return (glMapNamedBufferRangeEXT(bufferIdentifier, 0, bufferSize, GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT));
					}

					void EndUpdate(void)
					{
						glUnmapNamedBufferEXT(bufferIdentifier);
					}

					void UpdateBuffer(const void *data)
					{
						glNamedBufferSubDataEXT(bufferIdentifier, 0, bufferSize, data);
					}
			};

		#endif


		class QueryObject
		{
			private:

				#if C4GLCORE

					GLuint			queryIdentifier;

				#elif C4PS4 //[ PS4

					// -- PS4 code hidden --

				#elif C4PS3 //[ PS3

					// -- PS3 code hidden --

				#endif //]

			public:

				#if !C4PS3

					void Construct(void)
					{
						#if C4GLCORE

							glGenQueries(1, &queryIdentifier);

						#elif C4PS4 //[ PS4

							// -- PS4 code hidden --

						#endif //]
					}

				#else //[ C4PS3

					void Construct(void);

				#endif //]

				void Destruct(void)
				{
					#if C4GLCORE

						glDeleteQueries(1, &queryIdentifier);

					#endif
				}

				#if C4OPENGL

					void BeginOcclusionQuery(void)
					{
						#if C4GLCORE

							glBeginQuery(GL_SAMPLES_PASSED, queryIdentifier);

						#endif
					}

					static void EndOcclusionQuery(void)
					{
						#if C4GLCORE

							glEndQuery(GL_SAMPLES_PASSED);

						#endif
					}

					void QueryTimestamp(void)
					{
						#if C4GLCORE

							glQueryCounter(queryIdentifier, GL_TIMESTAMP);

						#endif
					}

					unsigned_int32 GetSamplesPassed(void) const
					{
						#if C4GLCORE

							GLuint		result;

							glGetQueryObjectuiv(queryIdentifier, GL_QUERY_RESULT, &result);
							return (result);

						#else //[ MOBILE

							// -- Mobile code hidden --

						#endif //]
					}

					unsigned_int64 GetTimestamp(void) const
					{
						#if C4GLCORE

							GLuint64	result;

							glGetQueryObjectui64v(queryIdentifier, GL_QUERY_RESULT, &result);
							return (result);

						#else //[ MOBILE

							// -- Mobile code hidden --

						#endif //]
					}

				#elif C4CONSOLE //[ CONSOLE

					// -- Console code hidden --

				#endif //]
		};


		class ShaderObject
		{
			private:

				#if C4OPENGL

					bool				compiledFlag;

				#endif

			protected:

				#if C4OPENGL

					GLuint				shaderIdentifier;

				#elif C4PS4 //[ PS4

					// -- PS4 code hidden --

				#elif C4PS3 //[ PS3

					// -- PS3 code hidden --

				#endif //]

				ShaderObject();
				~ShaderObject();

			public:

				#if C4OPENGL

					GLuint GetShaderIdentifier(void) const
					{
						return (shaderIdentifier);
					}

					void Compile(void);

					#if C4DEBUG

						bool GetShaderStatus(const char **error, const char **source) const;
						void ReleaseShaderStatus(const char *error, const char *source) const;

					#endif

				#endif
		};


		class VertexShaderObject : public ShaderObject
		{
			#if C4PS4 //[ PS4

				// -- PS4 code hidden --

			#elif C4PS3 //[ PS3

				// -- PS3 code hidden --

			#endif //]

			public:

				VertexShaderObject(const char *source, unsigned_int32 size);

				#if C4PS4 //[ PS4

					// -- PS4 code hidden --

				#elif C4PS3 //[ PS3

					// -- PS3 code hidden --

				#endif //]
		};


		class FragmentShaderObject : public ShaderObject
		{
			#if C4PS4 //[ PS4

				// -- PS4 code hidden --

			#elif C4PS3 //[ PS3

				// -- PS3 code hidden --

			#endif //]

			public:

				FragmentShaderObject(const char *source, unsigned_int32 size);

				#if C4PS4 //[ PS4

					// -- PS4 code hidden --

				#elif C4PS3 //[ PS3

					// -- PS3 code hidden --

				#endif //]
		};


		class GeometryShaderObject : public ShaderObject
		{
			public:

				GeometryShaderObject(const char *source, unsigned_int32 size);

				#if C4PS4 //[ PS4

					// -- PS4 code hidden --

				#elif C4PS3 //[ PS3

					// -- PS3 code hidden --

				#endif //]
		};


		class ShaderProgramObject
		{
			friend struct Render;

			private:

				VertexShaderObject		*vertexShader;
				FragmentShaderObject	*fragmentShader;
				GeometryShaderObject	*geometryShader;

				#if C4OPENGL

					GLuint				programIdentifier;
					unsigned_int32		programUpdateFlags;

					int32				vertexUniformLocation;
					int32				fragmentUniformLocation;

					#if C4GLCORE

						int32			geometryUniformLocation;

					#endif

				#elif C4PS4 //[ PS4

					// -- PS4 code hidden --

				#endif //]

			public:

				ShaderProgramObject();
				~ShaderProgramObject();

				#if C4OPENGL

					GLuint GetProgramIdentifier(void) const
					{
						return (programIdentifier);
					}

					int32 QueryUniformLocation(const char *name) const
					{
						return (glGetUniformLocation(programIdentifier, name));
					}

					void SetVertexShader(VertexShaderObject *shader);
					void SetFragmentShader(FragmentShaderObject *shader);
					void SetGeometryShader(GeometryShaderObject *shader);

					void Activate(void);

					bool SetProgramBinary(unsigned_int32 format, const void *data, unsigned_int32 size);

					#if C4DEBUG

						bool GetProgramStatus(const char **error, const char **source) const;
						void ReleaseProgramStatus(const char *error, const char *source) const;

					#endif

				#elif C4CONSOLE //[ CONSOLE

					// -- Console code hidden --

				#endif //]

				void Preprocess(void);
		};


		#if C4PS3 //[ PS3

			// -- PS3 code hidden --

		#endif //]


		#if C4OPENGL

			class PixelBufferObject
			{
				private:

					GLuint				pixelBufferIdentifier;
					unsigned_int32		bufferSize;

				public:

					void Construct(void);
					void Destruct(void);

					void Bind(void)
					{
						glBindBuffer(GL_PIXEL_PACK_BUFFER, pixelBufferIdentifier);
					}

					void Unbind(void)
					{
						glBindBuffer(GL_PIXEL_PACK_BUFFER, 0);
					}

					const void *BeginRead(void)
					{
						return (glMapNamedBufferRangeEXT(pixelBufferIdentifier, 0, bufferSize, GL_MAP_READ_BIT));
					}

					void EndRead(void)
					{
						glUnmapNamedBufferEXT(pixelBufferIdentifier);
					}

					void AllocateStorage(unsigned_int32 size);
			};

		#endif


		#if C4OPENGL

			typedef unsigned_int32 TextureHandle;

		#elif C4CONSOLE //[ CONSOLE

			// -- Console code hidden --

		#endif //]


		class TextureObject
		{
			friend struct Render;

			private:

				#if C4OPENGL

					GLuint					textureIdentifier;
					unsigned_int16			openglTarget;

				#elif C4PS4 //[ PS4

					// -- PS4 code hidden --

				#endif //]

				unsigned_int16				targetIndex;
				unsigned_int16				formatIndex;

				#if C4PS4 //[ PS4

					// -- PS4 code hidden --

				#elif C4PS3 //[ PS3

					// -- PS3 code hidden --

				#endif //]

			public:

				void Construct(unsigned_int32 index);
				void Destruct(void);

				TextureHandle GetTextureHandle(void) const
				{
					#if C4OPENGL

						return (textureIdentifier);

					#elif C4CONSOLE //[ CONSOLE

						// -- Console code hidden --

					#endif //]
				}

				unsigned_int32 GetTextureTargetIndex(void) const
				{
					return (targetIndex);
				}

				void InvalidateImage(void)
				{
					#if C4OPENGL

						glInvalidateTexImage(textureIdentifier, 0);

					#endif
				}

				#if C4OPENGL

					void SetSWrapMode(unsigned_int32 mode)
					{
						glTextureParameteriEXT(textureIdentifier, openglTarget, GL_TEXTURE_WRAP_S, mode);
					}

					void SetTWrapMode(unsigned_int32 mode)
					{
						glTextureParameteriEXT(textureIdentifier, openglTarget, GL_TEXTURE_WRAP_T, mode);
					}

					void SetRWrapMode(unsigned_int32 mode)
					{
						glTextureParameteriEXT(textureIdentifier, openglTarget, GL_TEXTURE_WRAP_R, mode);
					}

					void SetCompareFunc(unsigned_int32 func)
					{
						glTextureParameteriEXT(textureIdentifier, openglTarget, GL_TEXTURE_COMPARE_FUNC, func);
					}

					void SetCompareMode(unsigned_int32 mode)
					{
						glTextureParameteriEXT(textureIdentifier, openglTarget, GL_TEXTURE_COMPARE_MODE, mode);
					}

					void SetMinLod(unsigned_int32 lod)
					{
						glTextureParameteriEXT(textureIdentifier, openglTarget, GL_TEXTURE_BASE_LEVEL, lod);
					}

					void SetMaxLod(unsigned_int32 lod)
					{
						glTextureParameteriEXT(textureIdentifier, openglTarget, GL_TEXTURE_MAX_LEVEL, lod);
					}

					void SetLodBias(float bias)
					{
						#if C4GLCORE

							glTextureParameterfEXT(textureIdentifier, openglTarget, GL_TEXTURE_LOD_BIAS, bias);

						#endif
					}

					void SetFilterModes(unsigned_int32 minMode, unsigned_int32 magMode)
					{
						glTextureParameteriEXT(textureIdentifier, openglTarget, GL_TEXTURE_MIN_FILTER, minMode);
						glTextureParameteriEXT(textureIdentifier, openglTarget, GL_TEXTURE_MAG_FILTER, magMode);
					}

					void SetSwizzle(const int32 *swizzle)
					{
						#if C4GLCORE

							glTextureParameterivEXT(textureIdentifier, openglTarget, GL_TEXTURE_SWIZZLE_RGBA, swizzle);

						#else //[ MOBILE

							// -- Mobile code hidden --

						#endif //]
					}

					void SetMaxAnisotropy(float anisotropy)
					{
						glTextureParameterfEXT(textureIdentifier, openglTarget, GL_TEXTURE_MAX_ANISOTROPY_EXT, anisotropy);
					}

					void BlitImageRect(unsigned_int32 srcX, unsigned_int32 srcY, unsigned_int32 dstX, unsigned_int32 dstY, unsigned_int32 width, unsigned_int32 height) const
					{
						glCopyTextureSubImage2DEXT(textureIdentifier, GL_TEXTURE_RECTANGLE, 0, dstX, dstY, srcX, srcY, width, height);
					}

					void Bind(unsigned_int32 unit) const;
					void Unbind(unsigned_int32 unit) const;
					void UnbindAll(void) const;

				#elif C4PS4 //[ PS4

					// -- PS4 code hidden --

				#elif C4PS3 //[ PS3

					// -- PS3 code hidden --

				#endif //]

				void SetBorderColor(unsigned_int32 color);

				void SetImage2D(const TextureUploadData *uploadData);
				void SetImage3D(const TextureUploadData *uploadData);
				void SetImageCube(const TextureUploadData *uploadData);
				void SetImageRect(const TextureUploadData *uploadData);
				void SetImageArray2D(const TextureUploadData *uploadData);
				void SetCompressedImage2D(const TextureUploadData *uploadData);
				void SetCompressedImageCube(const TextureUploadData *uploadData);
				void SetCompressedImageArray2D(const TextureUploadData *uploadData);

				void AllocateStorage2D(const TextureAllocationData *allocationData);
				void AllocateStorageCube(const TextureAllocationData *allocationData);
				void AllocateStorageRect(const TextureAllocationData *allocationData);
				void AllocateStorageArray2D(const TextureAllocationData *allocationData);

				void UpdateImage2D(unsigned_int32 width, unsigned_int32 height, int32 count, const void *image) const;
				void UpdateImage2D(unsigned_int32 x, unsigned_int32 y, unsigned_int32 width, unsigned_int32 height, unsigned_int32 rowLength, const void *image) const;
				void UpdateImage3D(unsigned_int32 x, unsigned_int32 y, unsigned_int32 z, unsigned_int32 width, unsigned_int32 height, unsigned_int32 depth, const void *image) const;
				void UpdateImageRect(unsigned_int32 x, unsigned_int32 y, unsigned_int32 width, unsigned_int32 height, unsigned_int32 rowLength, const void *image) const;
		};


		class TextureArrayObject
		{
			#if C4PS4 //[ PS4

				// -- PS4 code hidden --

			#endif //]

			public:

				int32				textureCount;
				TextureHandle		textureHandle[kMaxShaderTextureCount];
				int8				textureTarget[kMaxShaderTextureCount];

				#if C4OPENGL || C4PS3

					void Construct(void)
					{
						textureCount = 0;
					}

					void Destruct(void)
					{
					}

					void Preprocess(void)
					{
					}

				#elif C4PS4 //[ PS4

					// -- PS4 code hidden --

				#endif //]
		};


		class RenderBufferObject
		{
			friend struct Render;
			friend class FrameBufferObject;

			private:

				#if C4OPENGL

					GLuint				renderBufferIdentifier;

				#elif C4PS4 //[ PS4

					// -- PS4 code hidden --

				#elif C4PS3 //[ PS3

					// -- PS3 code hidden --

				#endif //]

			public:

				void Construct(void);
				void Destruct(void);

				void AllocateStorage(unsigned_int32 width, unsigned_int32 height, unsigned_int32 format, bool display = false);
				void AllocateMultisampleStorage(unsigned_int32 width, unsigned_int32 height, unsigned_int32 sampleCount, unsigned_int32 format);
		};


		class FrameBufferObject
		{
			friend struct Render;
			friend class TextureObject;

			private:

				TextureObject					*currentColorTexture;
				TextureObject					*currentVelocityTexture;
				TextureObject					*currentDepthTexture;
				mutable int32					currentDepthLayer;

				#if C4OPENGL

					GLuint						frameBufferIdentifier;

					void Bind(void) const
					{
						if (renderState.drawFrameBuffer != this)
						{
							renderState.drawFrameBuffer = this;
							renderState.readFrameBuffer = this;
							glBindFramebuffer(GL_FRAMEBUFFER, frameBufferIdentifier);
						}
					}

				#elif C4PS4 //[ PS4

					// -- PS4 code hidden --

				#elif C4PS3 //[ PS3

					// -- PS3 code hidden --

				#endif //]

			public:

				void Construct(void);
				void Destruct(void);

				void SetColorRenderBuffer(const RenderBufferObject *renderBuffer);
				void SetDepthStencilRenderBuffer(const RenderBufferObject *renderBuffer);

				void SetColorRenderTexture(TextureObject *renderTexture);
				void SetVelocityRenderTexture(TextureObject *renderTexture);
				void SetDepthRenderTexture(TextureObject *renderTexture);
				void SetDepthRenderTextureLayer(TextureObject *renderTexture, int32 layer);

				#if C4OPENGL

					void SetDepthRenderTextureCubeFace(TextureObject *renderTexture, int32 face);

				#elif C4PS4 //[ PS4

					// -- PS4 code hidden --

				#endif //]

				void ResetColorRenderTexture(void);
				void ResetVelocityRenderTexture(void);
		};


		struct RenderState
		{
			#if C4OPENGL

				unsigned_int32						imageUnit;
				GLuint								texture[kMaxTextureUnitCount][kTextureTargetCount];

				const FrameBufferObject				*drawFrameBuffer;
				const FrameBufferObject				*readFrameBuffer;

				GLuint								vertexArrayObject;
				GLuint								attributeVertexBuffer;
				GLuint								indexVertexBuffer;

				unsigned_int32						frameCount;
				unsigned_int32						updateFlags;

				unsigned_int32						vertexArrayMask;
				const VertexDataObject				*vertexDataObject;

				const ShaderProgramObject			*shaderProgram;

				Vector4D							vertexShaderParam[kMaxVertexParamCount];
				Vector4D							fragmentShaderParam[kMaxFragmentParamCount];
				Vector4D							geometryShaderParam[kMaxGeometryParamCount];

				Storage<ParameterBufferObject>		universalParameterBuffer;
				Vector4D							universalShaderParam[kMaxUniversalParamCount];

			#elif C4PS4 //[ PS4

				// -- PS4 code hidden --

			#elif C4PS3 //[ PS3

				// -- PS3 code hidden --

			#endif //]
		};


		private:

			static RenderState		renderState;

			#if C4OPENGL

				enum
				{
					kMaxQuadPrimitiveCount = 16383
				};

				static VertexBufferObject		quadIndexBuffer;
				static const Triangle			quadTriangle[kMaxQuadPrimitiveCount * 2];

				static void Update(bool indexed = false);

				static void BindTextureUnit0(GLuint texture, GLenum target);

			#elif C4PS4 //[ PS4

				// -- PS4 code hidden --

			#elif C4PS3 //[ PS3

				// -- PS3 code hidden --

			#endif //]

		public:

			#if C4OPENGL

				static void InitializeCoreOpenGL();
				static void TerminateCoreOpenGL();

				static const VertexBufferObject *GetQuadIndexBuffer(void)
				{
					return (&quadIndexBuffer);
				}

				static void BeginRendering(void)
				{
					renderState.frameCount++;
				}

				static void EndRendering(void)
				{
				}

				static unsigned_int32 GetFrameCount(void)
				{
					return (renderState.frameCount);
				}

				static void SetColorMask(bool r, bool g, bool b, bool a)
				{
					glColorMask(r, g, b, a);
				}

				static void EnableAlphaCoverage(void)
				{
					glEnable(GL_SAMPLE_ALPHA_TO_COVERAGE);
				}

				static void DisableAlphaCoverage(void)
				{
					glDisable(GL_SAMPLE_ALPHA_TO_COVERAGE);
				}

				static void EnableSampleShading(void)
				{
					#if C4GLCORE

						glEnable(GL_SAMPLE_SHADING);

					#endif
				}

				static void DisableSampleShading(void)
				{
					#if C4GLCORE

						glDisable(GL_SAMPLE_SHADING);

					#endif
				}

				static void EnableDepthTest(void)
				{
					glEnable(GL_DEPTH_TEST);
				}

				static void DisableDepthTest(void)
				{
					glDisable(GL_DEPTH_TEST);
				}

				static void SetDepthFunc(unsigned_int32 func)
				{
					glDepthFunc(func);
				}

				static void SetDepthMask(bool mask)
				{
					glDepthMask(mask);
				}

				static void EnableDepthClamp(void)
				{
					#if C4GLCORE

						glEnable(GL_DEPTH_CLAMP);

					#endif
				}

				static void DisableDepthClamp(void)
				{
					#if C4GLCORE

						glDisable(GL_DEPTH_CLAMP);

					#endif
				}

				static void EnableDepthBoundsTest(void)
				{
					#if C4GLCORE

						glEnable(GL_DEPTH_BOUNDS_TEST_EXT);

					#endif
				}

				static void DisableDepthBoundsTest(void)
				{
					#if C4GLCORE

						glDisable(GL_DEPTH_BOUNDS_TEST_EXT);

					#endif
				}

				static void SetDepthBounds(float zmin, float zmax)
				{
					#if C4GLCORE

						glDepthBoundsEXT(zmin, zmax);

					#endif
				}

				static void EnableStencilTest(void)
				{
					glEnable(GL_STENCIL_TEST);
				}

				static void DisableStencilTest(void)
				{
					glDisable(GL_STENCIL_TEST);
				}

				static void SetStencilFunc(unsigned_int32 func, unsigned_int32 ref, unsigned_int32 mask)
				{
					glStencilFunc(func, ref, mask);
				}

				static void SetStencilOp(unsigned_int32 fail, unsigned_int32 zfail, unsigned_int32 zpass)
				{
					glStencilOp(fail, zfail, zpass);
				}

				static void SetFrontStencilOp(unsigned_int32 fail, unsigned_int32 zfail, unsigned_int32 zpass)
				{
					glStencilOpSeparate(GL_FRONT, fail, zfail, zpass);
				}

				static void SetBackStencilOp(unsigned_int32 fail, unsigned_int32 zfail, unsigned_int32 zpass)
				{
					glStencilOpSeparate(GL_BACK, fail, zfail, zpass);
				}

				static void SetStencilMask(unsigned_int32 mask)
				{
					glStencilMask(mask);
				}

				static void EnableBlend(void)
				{
					glEnable(GL_BLEND);
				}

				static void DisableBlend(void)
				{
					glDisable(GL_BLEND);
				}

				static void SetBlendFunc(unsigned_int32 srcFunc, unsigned_int32 dstFunc)
				{
					glBlendFunc(srcFunc, dstFunc);
				}

				static void SetBlendFunc(unsigned_int32 srcRgbFunc, unsigned_int32 dstRgbFunc, unsigned_int32 srcAlphaFunc, unsigned_int32 dstAlphaFunc)
				{
					glBlendFuncSeparate(srcRgbFunc, dstRgbFunc, srcAlphaFunc, dstAlphaFunc);
				}

				static void SetBlendEquation(unsigned_int32 equation)
				{
					glBlendEquation(equation);
				}

				static void SetBlendColor(float r, float g, float b, float a)
				{
					glBlendColor(r, g, b, a);
				}

				static void EnableCullFace(void)
				{
					glEnable(GL_CULL_FACE);
				}

				static void DisableCullFace(void)
				{
					glDisable(GL_CULL_FACE);
				}

				static void SetCullFace(unsigned_int32 face)
				{
					glCullFace(face);
				}

				static void SetFrontFace(unsigned_int32 front)
				{
					glFrontFace(front);
				}

				static void SetFillPolygonMode(void)
				{
					#if C4GLCORE

						glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

					#endif
				}

				static void SetLinePolygonMode(void)
				{
					#if C4GLCORE

						glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

					#endif
				}

				static void EnablePointSprite(void)
				{
					#if C4GLCORE

						glEnable(GL_PROGRAM_POINT_SIZE);

					#endif
				}

				static void DisablePointSprite(void)
				{
					#if C4GLCORE

						glDisable(GL_PROGRAM_POINT_SIZE);

					#endif
				}

				static void EnablePolygonLineOffset(void)
				{
					#if C4GLCORE

						glEnable(GL_POLYGON_OFFSET_LINE);

					#endif
				}

				static void DisablePolygonLineOffset(void)
				{
					#if C4GLCORE

						glDisable(GL_POLYGON_OFFSET_LINE);

					#endif
				}

				static void EnablePolygonFillOffset(void)
				{
					glEnable(GL_POLYGON_OFFSET_FILL);
				}

				static void DisablePolygonFillOffset(void)
				{
					glDisable(GL_POLYGON_OFFSET_FILL);
				}

				static void SetPolygonOffset(float slope, float bias, float clamp)
				{
					glPolygonOffsetClampEXT(slope, bias, clamp);
				}

				static void EnableFrameBufferSRGB(void)
				{
					#if C4GLCORE

						glEnable(GL_FRAMEBUFFER_SRGB);

					#endif
				}

				static void DisableFrameBufferSRGB(void)
				{
					#if C4GLCORE

						glDisable(GL_FRAMEBUFFER_SRGB);

					#endif
				}

				static void ClearColorBuffer(const float *color)
				{
					glClearBufferfv(GL_COLOR, 0, color);
				}

				static void ClearColorVelocityBuffers(const float *color, const float *velocity)
				{
					glClearBufferfv(GL_COLOR, 0, color);
					glClearBufferfv(GL_COLOR, 1, velocity);
				}

				static void ClearDepthBuffer(void)
				{
					static const float depth = 1.0F;
					glClearBufferfv(GL_DEPTH, 0, &depth);
				}

				static void ClearStencilBuffer(void)
				{
					static const int stencil = 0;
					glClearBufferiv(GL_STENCIL, 0, &stencil);
				}

				static void ClearDepthStencilBuffers(void)
				{
					glClearBufferfi(GL_DEPTH_STENCIL, 0, 1.0F, 0);
				}

				static void ClearColorDepthStencilBuffers(const float *color)
				{
					glClearBufferfv(GL_COLOR, 0, color);
					glClearBufferfi(GL_DEPTH_STENCIL, 0, 1.0F, 0);
				}

				static void ClearColorVelocityDepthStencilBuffers(const float *color, const float *velocity)
				{
					glClearBufferfv(GL_COLOR, 0, color);
					glClearBufferfv(GL_COLOR, 1, velocity);
					glClearBufferfi(GL_DEPTH_STENCIL, 0, 1.0F, 0);
				}

				static void SetViewport(int32 x, int32 y, int32 width, int32 height)
				{
					glViewport(x, y, width, height);
				}

				static void SetScissor(int32 x, int32 y, int32 width, int32 height)
				{
					glScissor(x, y, width, height);
				}

				static void SetDepthRange(float z1, float z2)
				{
					glDepthRange(z1, z2);
				}

				static void ResetFrameBuffer(void)
				{
					renderState.drawFrameBuffer = nullptr;
					renderState.readFrameBuffer = nullptr;
					glBindFramebuffer(GL_FRAMEBUFFER, 0);
				}

				static void SetFrameBuffer(const FrameBufferObject *frameBuffer)
				{
					frameBuffer->Bind();
				}

				static void SetDrawFrameBuffer(const FrameBufferObject *frameBuffer)
				{
					if (renderState.drawFrameBuffer != frameBuffer)
					{
						renderState.drawFrameBuffer = frameBuffer;
						glBindFramebuffer(GL_DRAW_FRAMEBUFFER, frameBuffer->frameBufferIdentifier);
					}
				}

				static void SetReadFrameBuffer(const FrameBufferObject *frameBuffer)
				{
					if (renderState.readFrameBuffer != frameBuffer)
					{
						renderState.readFrameBuffer = frameBuffer;
						glBindFramebuffer(GL_READ_FRAMEBUFFER, frameBuffer->frameBufferIdentifier);
					}
				}

				static void InvalidateDrawFrameBuffer(void)
				{
					static const GLenum attachment[3] = {GL_COLOR_ATTACHMENT0, GL_DEPTH_ATTACHMENT, GL_STENCIL_ATTACHMENT};
					glInvalidateFramebuffer(GL_DRAW_FRAMEBUFFER, 3, attachment);
				}

				static void InvalidateReadFrameBuffer(void)
				{
					static const GLenum attachment = GL_COLOR_ATTACHMENT0;
					glInvalidateFramebuffer(GL_READ_FRAMEBUFFER, 1, &attachment);
				}

				static void ResolveMultisampleFrameBuffer(unsigned_int32 left, unsigned_int32 bottom, unsigned_int32 right, unsigned_int32 top)
				{
					glBlitFramebuffer(left, bottom, right, top, left, bottom, right, top, GL_COLOR_BUFFER_BIT, GL_LINEAR);
				}

				static void CopyFrameBuffer(unsigned_int32 srcX1, unsigned_int32 srcY1, unsigned_int32 srcX2, unsigned_int32 srcY2, unsigned_int32 dstX1, unsigned_int32 dstY1, unsigned_int32 dstX2, unsigned_int32 dstY2, unsigned_int32 filter)
				{
					glBlitFramebuffer(srcX1, srcY1, srcX2, srcY2, dstX1, dstY1, dstX2, dstY2, GL_COLOR_BUFFER_BIT, filter);
				}

				static void BindGlobalTexture(unsigned_int32 unit, const TextureObject *texture)
				{
					texture->Bind(unit);
				}

				static void SetShaderTextureUnit(const ShaderProgramObject *shaderProgram, const char *name, int32 unit)
				{
					glUniform1i(shaderProgram->QueryUniformLocation(name), unit);
				}

				static void DrawPrimitives(unsigned_int32 prim, unsigned_int32 start, unsigned_int32 count)
				{
					Update();
					glDrawArrays(prim, start, count);
				}

				static void MultiDrawPrimitives(unsigned_int32 prim, const unsigned_int32 *startArray, const unsigned_int32 *countArray, unsigned_int32 size)
				{
					Update();

					#if C4GLCORE

						glMultiDrawArrays(prim, reinterpret_cast<const GLint *>(startArray), reinterpret_cast<const GLsizei *>(countArray), size);

					#else //[ MOBILE

						// -- Mobile code hidden --

					#endif //]
				}

				static void DrawIndexedPrimitives(unsigned_int32 prim, unsigned_int32 count, unsigned_int32 offset)
				{
					Update(true);
					glDrawElements(prim, count, GL_UNSIGNED_SHORT, (char *) 0 + offset);
				}

				static void MultiDrawIndexedPrimitives(unsigned_int32 prim, const unsigned_int32 *countArray, const machine_address *offsetArray, unsigned_int32 size)
				{
					Update(true);

					#if C4GLCORE

						glMultiDrawElements(prim, reinterpret_cast<const GLsizei *>(countArray), GL_UNSIGNED_SHORT, reinterpret_cast<const void *const *>(offsetArray), size);

					#else //[ MOBILE

						// -- Mobile code hidden --

					#endif //]
				}

				static void DrawQuads(unsigned_int32 start, unsigned_int32 count)
				{
					Update(true);
					glDrawElements(GL_TRIANGLES, (count >> 2) * 6, GL_UNSIGNED_SHORT, (char *) 0 + (start >> 2) * 12);
				}

				static void BindTextureArray(const TextureArrayObject *textureArray);
				static void SetShaderProgram(const ShaderProgramObject *shaderProgram);

				// If GL_EXT_direct_state_access is not available, then the DSA functions used by the engine
				// get remapped to the following functions in the Render namespace.

				#if C4WINDOWS

					#define OPENGLCALL APIENTRY

				#else

					#define OPENGLCALL

				#endif

				static void OPENGLCALL BindMultiTexture(GLenum texunit, GLenum target, GLuint texture);
				static void OPENGLCALL TextureParameteri(GLuint texture, GLenum target, GLenum pname, GLint param);
				static void OPENGLCALL TextureParameteriv(GLuint texture, GLenum target, GLenum pname, const GLint *param);
				static void OPENGLCALL TextureParameterf(GLuint texture, GLenum target, GLenum pname, GLfloat param);
				static void OPENGLCALL TextureParameterfv(GLuint texture, GLenum target, GLenum pname, const GLfloat *param);
				static void OPENGLCALL TextureImage2D(GLuint texture, GLenum target, GLint level, GLint internalformat, GLsizei width, GLsizei height, GLint border, GLenum format, GLenum type, const void *pixels);
				static void OPENGLCALL TextureSubImage2D(GLuint texture, GLenum target, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLenum type, const void *pixels);
				static void OPENGLCALL TextureImage3D(GLuint texture, GLenum target, GLint level, GLint internalformat, GLsizei width, GLsizei height, GLsizei depth, GLint border, GLenum format, GLenum type, const void *pixels);
				static void OPENGLCALL TextureSubImage3D(GLuint texture, GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLsizei width, GLsizei height, GLsizei depth, GLenum format, GLenum type, const void *pixels);
				static void OPENGLCALL CompressedTextureImage2D(GLuint texture, GLenum target, GLint level, GLenum internalformat, GLsizei width, GLsizei height, GLint border, GLsizei imageSize, const void *data);
				static void OPENGLCALL CompressedTextureImage3D(GLuint texture, GLenum target, GLint level, GLenum internalformat, GLsizei width, GLsizei height, GLsizei depth, GLint border, GLsizei imageSize, const void *data);
				static void OPENGLCALL CopyTextureSubImage2D(GLuint texture, GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint x, GLint y, GLsizei width, GLsizei height);
				static void OPENGLCALL NamedBufferData(GLuint buffer, GLsizeiptr size, const void *data, GLenum usage);
				static void OPENGLCALL NamedBufferSubData(GLuint buffer, GLintptr offset, GLsizeiptr size, const void *data);
				static void OPENGLCALL GetNamedBufferSubData(GLuint buffer, GLintptr offset, GLsizeiptr size, void *data);
				static void *OPENGLCALL MapNamedBufferRange(GLuint buffer, GLintptr offset, GLsizeiptr length, GLbitfield access);
				static GLboolean OPENGLCALL UnmapNamedBuffer(GLuint buffer);
				static void OPENGLCALL ProgramUniform4fv(GLuint program, GLint location, GLsizei count, const GLfloat *param);
				static void OPENGLCALL VertexArrayVertexAttribOffset(GLuint vaobj, GLuint buffer, GLuint index, GLint size, GLenum type, GLboolean normalized, GLsizei stride, GLintptr offset);
				static void OPENGLCALL VertexArrayVertexAttribDivisor(GLuint vaobj, GLuint index, GLuint divisor);
				static void OPENGLCALL EnableVertexArrayAttrib(GLuint vaobj, GLuint index);
				static void OPENGLCALL DisableVertexArrayAttrib(GLuint vaobj, GLuint index);
				static void OPENGLCALL PolygonOffsetClamp(GLfloat factor, GLfloat units, GLfloat clamp);
				static void OPENGLCALL TextureStorage2D(GLuint texture, GLenum target, GLsizei levels, GLenum internalformat, GLsizei width, GLsizei height);
				static void OPENGLCALL TextureStorage3D(GLuint texture, GLenum target, GLsizei levels, GLenum internalformat, GLsizei width, GLsizei height, GLsizei depth);
				static void OPENGLCALL InvalidateTexImage(GLuint texture, GLint level);
				static void OPENGLCALL InvalidateBufferData(GLuint buffer);
				static void OPENGLCALL InvalidateFramebuffer(GLenum target, GLsizei numAttachments, const GLenum *attachments);
				static void OPENGLCALL BindTextures(GLuint first, GLsizei count, const GLuint *textures);

				#if C4GLCORE

					static void OPENGLCALL DepthBounds(GLclampd, GLclampd);

				#else //[ MOBILE

					// -- Mobile code hidden --

				#endif //]

			#elif C4PS4 //[ PS4

				// -- PS4 code hidden --

			#elif C4PS3 //[ PS3

				// -- PS3 code hidden --

			#endif //]

			static void SetVertexShaderParameter(unsigned_int32 index, float x, float y, float z, float w);
			static void SetVertexShaderParameter(unsigned_int32 index, const float *v);
			static void SetVertexShaderParameter(unsigned_int32 index, const Vector3D& v);
			static void SetVertexShaderParameter(unsigned_int32 index, const Point3D& p);
			static void SetVertexShaderParameter(unsigned_int32 index, const Vector4D& v);
			static void SetVertexShaderParameter(unsigned_int32 index, const Antivector4D& v);
			static void SetVertexShaderParameter(unsigned_int32 index, const Matrix4D& m);
			static void SetVertexShaderParameter(unsigned_int32 index, const Transform4D& m);
			static void SetFragmentShaderParameter(unsigned_int32 index, float x, float y, float z, float w);
			static void SetFragmentShaderParameter(unsigned_int32 index, const float *v);
			static void SetFragmentShaderParameter(unsigned_int32 index, const Vector3D& v);
			static void SetFragmentShaderParameter(unsigned_int32 index, const Point3D& p);
			static void SetFragmentShaderParameter(unsigned_int32 index, const Vector4D& v);
			static void SetFragmentShaderParameter(unsigned_int32 index, const Antivector4D& v);
			static void SetFragmentShaderParameter(unsigned_int32 index, const ColorRGB& c);
			static void SetFragmentShaderParameter(unsigned_int32 index, const ColorRGBA& c);
			static void SetGeometryShaderParameter(unsigned_int32 index, float x, float y, float z, float w);
			static void SetGeometryShaderParameter(unsigned_int32 index, const float *v);
			static void SetGeometryShaderParameter(unsigned_int32 index, const Vector4D& v);
			static void SetGeometryShaderParameter(unsigned_int32 index, const Antivector4D& v);
			static void SetGeometryShaderParameter(unsigned_int32 index, const Matrix4D& m);
			static void SetGeometryShaderParameter(unsigned_int32 index, const Transform4D& m);
			static void SetUniversalShaderParameter(unsigned_int32 index, float x, float y, float z, float w);
			static void SetUniversalShaderParameter(unsigned_int32 index, const float *v);
			static void SetUniversalShaderParameter(unsigned_int32 index, const Vector3D& v);
			static void SetUniversalShaderParameter(unsigned_int32 index, const Point3D& p);
			static void SetUniversalShaderParameter(unsigned_int32 index, const Vector4D& v);
			static void SetUniversalShaderParameter(unsigned_int32 index, const ColorRGB& c);
			static void SetUniversalShaderParameter(unsigned_int32 index, const ColorRGBA& c);
	};
}


#endif

// ZYUQURM
