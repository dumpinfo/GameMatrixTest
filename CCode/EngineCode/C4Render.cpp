 

#include "C4Render.h"

#if C4CONSOLE //[ CONSOLE

	// -- Console code hidden --

#endif //]


using namespace C4;


Render::RenderState Render::renderState;


namespace
{
	#if C4OPENGL

		enum
		{
			kUpdateVertexDataObject				= 1 << 0,
			kUpdateVertexShaderParameters		= 1 << 1,
			kUpdateFragmentShaderParameters		= 1 << 2,
			kUpdateGeometryShaderParameters		= 1 << 3,
			kUpdateUniversalShaderParameters	= 1 << 4
		};


		struct TextureFormatData
		{
			GLenum				internalFormat[Render::kTextureEncodingCount];
			GLint				textureSwizzle[4];
			GLenum				pixelFormat;
			GLenum				pixelType;
			unsigned_int32		pixelSize;
		};

		const TextureFormatData textureFormatData[Render::kTextureFormatCount] =
		{
			{{GL_RGBA8, GL_SRGB8},															{GL_RED, GL_GREEN, GL_BLUE, GL_ONE},		GL_RGBA,				GL_UNSIGNED_BYTE,				4},			// kTextureRGBX8
			{{GL_RGBA8, GL_SRGB8_ALPHA8},													{GL_RED, GL_GREEN, GL_BLUE, GL_ALPHA},		GL_RGBA,				GL_UNSIGNED_BYTE,				4},			// kTextureRGBA8
			{{GL_RGBA8, GL_SRGB8},															{GL_RED, GL_GREEN, GL_BLUE, GL_ONE},		GL_BGRA,				GL_UNSIGNED_BYTE,				4},			// kTextureBGRX8
			{{GL_RGBA8, GL_SRGB8_ALPHA8},													{GL_RED, GL_GREEN, GL_BLUE, GL_ALPHA},		GL_BGRA,				GL_UNSIGNED_BYTE,				4},			// kTextureBGRA8
			{{GL_R8, GL_R8},																{GL_RED, GL_ZERO, GL_ZERO, GL_ONE},			GL_RED,					GL_UNSIGNED_BYTE,				1},			// kTextureR8
			{{GL_RG8, GL_RG8},																{GL_RED, GL_GREEN, GL_ZERO, GL_ONE},		GL_RG,					GL_UNSIGNED_BYTE,				2},			// kTextureRG8
			{{GL_R8, GL_R8},																{GL_RED, GL_RED, GL_RED, GL_ONE},			GL_RED,					GL_UNSIGNED_BYTE,				1},			// kTextureL8
			{{GL_RG8, GL_RG8},																{GL_RED, GL_RED, GL_RED, GL_GREEN},			GL_RG,					GL_UNSIGNED_BYTE,				2},			// kTextureLA8
			{{GL_R8, GL_R8},																{GL_RED, GL_RED, GL_RED, GL_RED},			GL_RED,					GL_UNSIGNED_BYTE,				1},			// kTextureI8
			{{GL_DEPTH_COMPONENT24, GL_DEPTH_COMPONENT24},									{GL_RED, GL_RED, GL_RED, GL_ONE},			GL_DEPTH_COMPONENT,		GL_UNSIGNED_INT,				4},			// kTextureDepth
			{{GL_COMPRESSED_RGB_S3TC_DXT1_EXT, GL_COMPRESSED_SRGB_S3TC_DXT1_EXT},			{GL_RED, GL_GREEN, GL_BLUE, GL_ONE},		GL_RGBA,				GL_UNSIGNED_BYTE,				8},			// kTextureBC1
			{{GL_COMPRESSED_RGBA_S3TC_DXT5_EXT, GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT5_EXT},	{GL_RED, GL_GREEN, GL_BLUE, GL_ALPHA},		GL_RGBA,				GL_UNSIGNED_BYTE,				16},		// kTextureBC3
			{{GL_R8, GL_R8},																{GL_RED, GL_RED, GL_RED, GL_RED},			GL_RED,					GL_UNSIGNED_BYTE,				1},			// kTextureRenderBufferR8
			{{GL_RGB8, GL_SRGB8},															{GL_RED, GL_GREEN, GL_BLUE, GL_ONE},		GL_RGBA,				GL_UNSIGNED_BYTE,				4},			// kTextureRenderBufferRGB8
			{{GL_RGBA8, GL_SRGB8_ALPHA8},													{GL_RED, GL_GREEN, GL_BLUE, GL_ALPHA},		GL_RGBA,				GL_UNSIGNED_BYTE,				4},			// kTextureRenderBufferRGBA8
			{{GL_RG16F, GL_RG16F},															{GL_RED, GL_GREEN, GL_ZERO, GL_ONE},		GL_RG,					GL_HALF_FLOAT,					4},			// kTextureRenderBufferRG16F
			{{GL_RGBA16F, GL_RGBA16F},														{GL_RED, GL_GREEN, GL_BLUE, GL_ALPHA},		GL_RGBA,				GL_HALF_FLOAT,					8}			// kTextureRenderBufferRGBA16F
		};


		const unsigned_int16 textureTargetEnum[Render::kTextureTargetCount] =
		{
			GL_TEXTURE_2D, GL_TEXTURE_3D, GL_TEXTURE_RECTANGLE, GL_TEXTURE_CUBE_MAP, GL_TEXTURE_2D_ARRAY
		};

	#elif C4PS4 //[ PS4

		// -- PS4 code hidden --

	#elif C4PS3 //[ PS3

		// -- PS3 code hidden --

	#endif //]
}


#if C4OPENGL

	Render::VertexBufferObject Render::quadIndexBuffer(Render::kVertexBufferTargetIndex, Render::kVertexBufferUsageStatic);

#endif


void Render::VertexBufferObject::Construct(void)
{
	#if C4OPENGL

		glGenBuffers(1, &bufferIdentifier);

		staticBuffer = nullptr;

	#endif
}

void Render::VertexBufferObject::Destruct(void)
{
	#if C4OPENGL

		delete[] staticBuffer;
		staticBuffer = nullptr; 

		glDeleteBuffers(1, &bufferIdentifier);
 
		if (renderState.attributeVertexBuffer == bufferIdentifier)
		{ 
			renderState.attributeVertexBuffer = 0;
		}
		else if (renderState.indexVertexBuffer == bufferIdentifier) 
		{
			renderState.indexVertexBuffer = 0; 
		} 

		bufferIdentifier = 0;

	#elif C4CONSOLE //[ CONSOLE 

		// -- Console code hidden --

	#endif //]
}

void Render::VertexBufferObject::AllocateStorage(const void *data)
{
	#if C4OPENGL

		if (bufferIdentifier == 0)
		{
			Construct();
		}

		if ((bufferUsage != kVertexBufferUsageStatic) || (data))
		{
			glNamedBufferDataEXT(bufferIdentifier, bufferSize, data, bufferUsage);
		}
		else
		{
			staticBuffer = new char[bufferSize];
		}

	#elif C4CONSOLE //[ CONSOLE

		// -- Console code hidden --

	#endif //]
}

#if C4CONSOLE //[ CONSOLE

	// -- Console code hidden --

#endif //]

volatile void *Render::VertexBufferObject::BeginUpdate(void)
{
	#if C4OPENGL

		if (!staticBuffer)
		{
			return (glMapNamedBufferRangeEXT(bufferIdentifier, 0, bufferSize, GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT));
		}

		return (staticBuffer);

	#elif C4CONSOLE //[ CONSOLE

		// -- Console code hidden --

	#endif //]
}

void Render::VertexBufferObject::EndUpdate(void)
{
	#if C4OPENGL

		if (!staticBuffer)
		{
			glUnmapNamedBufferEXT(bufferIdentifier);
		}
		else
		{
			glNamedBufferDataEXT(bufferIdentifier, bufferSize, staticBuffer, bufferUsage);

			delete[] staticBuffer;
			staticBuffer = nullptr;
		}

	#endif
}

void Render::VertexBufferObject::UpdateBuffer(unsigned_int32 offset, unsigned_int32 size, const void *data)
{
	#if C4OPENGL

		Assert(!staticBuffer, "Render::VertexBufferObject::UpdateBuffer(), cannot be called for static vertex buffers\n");

		glNamedBufferSubDataEXT(bufferIdentifier, offset, size, data);

	#elif C4CONSOLE //[ CONSOLE

		// -- Console code hidden --

	#endif //]
}

void Render::VertexBufferObject::ReadBuffer(unsigned_int32 offset, unsigned_int32 size, void *data) const
{
	#if C4OPENGL

		glGetNamedBufferSubDataEXT(bufferIdentifier, offset, size, data);

	#endif
}

void Render::VertexBufferObject::BeginUpdateSync(volatile void **const *ptr)
{
	#if C4OPENGL

		if (!staticBuffer)
		{
			**ptr = glMapNamedBufferRangeEXT(bufferIdentifier, 0, bufferSize, GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT);
		}
		else
		{
			**ptr = staticBuffer;
		}

	#elif C4CONSOLE //[ CONSOLE

		// -- Console code hidden --

	#endif //]
}

void Render::VertexBufferObject::EndUpdateSync(const void *)
{
	#if C4OPENGL

		if (!staticBuffer)
		{
			glUnmapNamedBufferEXT(bufferIdentifier);
		}
		else
		{
			glNamedBufferDataEXT(bufferIdentifier, bufferSize, staticBuffer, bufferUsage);

			delete[] staticBuffer;
			staticBuffer = nullptr;
		}

	#elif C4CONSOLE //[ CONSOLE

		// -- Console code hidden --

	#endif //]
}

void Render::VertexBufferObject::UpdateBufferSync(const BufferUploadData *uploadData)
{
	#if C4OPENGL

		Assert(!staticBuffer, "Render::VertexBufferObject::UpdateBufferSync(), cannot be called for static vertex buffers\n");

		glNamedBufferSubDataEXT(bufferIdentifier, uploadData->offset, uploadData->size, uploadData->data);

	#elif C4CONSOLE //[ CONSOLE

		// -- Console code hidden --

	#endif //]
}


Render::VertexDataObject::VertexDataObject()
{
	validFlag = false;
	vertexArrayMask = 0;
}

Render::VertexDataObject::~VertexDataObject()
{
	if (renderState.vertexDataObject == this)
	{
		renderState.vertexDataObject = nullptr;
	}
}

#if C4OPENGL

	void Render::VertexDataObject::Bind(void) const
	{
		if (renderState.vertexDataObject != this)
		{
			renderState.vertexDataObject = this;
			renderState.updateFlags |= kUpdateVertexDataObject;
		}
	}

	void Render::VertexDataObject::Invalidate(void)
	{
		validFlag = false;

		if (renderState.vertexDataObject == this)
		{
			renderState.updateFlags |= kUpdateVertexDataObject;
		}
	}

#endif

void Render::VertexDataObject::SetVertexAttribArray(unsigned_int32 index, const VertexBufferObject *buffer, unsigned_int32 offset, int32 count, int32 stride)
{
	#if C4OPENGL

		static const unsigned_int16 type[4][kVertexAttribCount] =
		{
			{kVertexFloat, kVertexFloat, kVertexSignedShort, kVertexFloat, kVertexUnsignedByte, kVertexUnsignedByte, kVertexUnsignedByte, kVertexFloat, kVertexFloat, kVertexFloat},
			{kVertexFloat, kVertexFloat, kVertexSignedShort, kVertexFloat, kVertexFloat, kVertexFloat, kVertexFloat, kVertexFloat, kVertexFloat, kVertexFloat},
			{kVertexFloat, kVertexFloat, kVertexFloat, kVertexFloat, kVertexFloat, kVertexFloat, kVertexFloat, kVertexFloat, kVertexFloat, kVertexFloat},
			{kVertexFloat, kVertexFloat, kVertexFloat, kVertexFloat, kVertexFloat, kVertexFloat, kVertexFloat, kVertexFloat, kVertexFloat, kVertexFloat}
		};

		static const unsigned_int8 size[4][kVertexAttribCount] =
		{
			{1, 1, 2, 1, 4, 4, 4, 1, 1, 1},
			{2, 2, 4, 2, 2, 2, 2, 2, 2, 2},
			{3, 3, 3, 3, 3, 3, 3, 3, 3, 3},
			{4, 4, 4, 4, 4, 4, 4, 4, 4, 4}
		};

		count--;

		VertexAttribData *attribData = &vertexAttribData[index];
		attribData->buffer = buffer->bufferIdentifier;
		attribData->offset = offset;
		attribData->type = type[count][index];
		attribData->size = size[count][index];
		attribData->stride = (unsigned_int8) stride;

	#elif C4PS4 //[ PS4

		// -- PS4 code hidden --

	#elif C4PS3 //[ PS3

		// -- PS3 code hidden --

	#endif //]

	vertexArrayMask |= (unsigned_int16) (1 << index);
}


#if C4OPENGL

	Render::ParameterBufferObject::ParameterBufferObject(unsigned_int32 index)
	{
		glGenBuffers(1, &bufferIdentifier);

		if (index == kParameterBufferUniversal)
		{
			bufferSize = kMaxUniversalParamCount * sizeof(Vector4D);
			glNamedBufferDataEXT(bufferIdentifier, bufferSize, nullptr, GL_DYNAMIC_DRAW);
		}
		else if (index == kParameterBufferQuery)
		{
			bufferSize = kMaxOcclusionQueryCount * 4;
			glNamedBufferDataEXT(bufferIdentifier, bufferSize, nullptr, GL_DYNAMIC_COPY);
		}

		glBindBufferBase(GL_UNIFORM_BUFFER, 0, bufferIdentifier);
	}

	Render::ParameterBufferObject::~ParameterBufferObject()
	{
		glDeleteBuffers(1, &bufferIdentifier);
	}

#endif


#if C4PS4 //[ PS4

	// -- PS4 code hidden --

#elif C4PS3 //[ PS3

	// -- PS3 code hidden --

#endif //]


Render::ShaderObject::ShaderObject()
{
	#if C4OPENGL

		compiledFlag = false;

	#elif C4PS4 //[ PS4

		// -- PS4 code hidden --

	#elif C4PS3 //[ PS3

		// -- PS3 code hidden --

	#endif //]
}

Render::ShaderObject::~ShaderObject()
{
	#if C4OPENGL

		glDeleteShader(shaderIdentifier);

	#elif C4PS4 //[ PS4

		// -- PS4 code hidden --

	#elif C4PS3 //[ PS3

		// -- PS3 code hidden --

	#endif //]
}

#if C4OPENGL

	void Render::ShaderObject::Compile(void)
	{
		if (!compiledFlag)
		{
			compiledFlag = true;
			glCompileShader(shaderIdentifier);
		}
	}

	#if C4DEBUG

		bool Render::ShaderObject::GetShaderStatus(const char **error, const char **source) const
		{
			GLint	status;

			glGetShaderiv(shaderIdentifier, GL_COMPILE_STATUS, &status);
			if (!status)
			{
				GLint	size;

				glGetShaderiv(shaderIdentifier, GL_INFO_LOG_LENGTH, &size);
				char *string = new char[size];
				glGetShaderInfoLog(shaderIdentifier, size, nullptr, string);
				*error = string;

				glGetShaderiv(shaderIdentifier, GL_SHADER_SOURCE_LENGTH, &size);
				string = new char[size];
				glGetShaderSource(shaderIdentifier, size, nullptr, string);
				*source = string;

				return (false);
			}

			return (true);
		}

		void Render::ShaderObject::ReleaseShaderStatus(const char *error, const char *source) const
		{
			delete[] source;
			delete[] error;
		}

	#endif

#endif


Render::VertexShaderObject::VertexShaderObject(const char *source, unsigned_int32 size)
{
	#if C4OPENGL

		shaderIdentifier = glCreateShader(GL_VERTEX_SHADER);
		glShaderSource(shaderIdentifier, 1, &source, reinterpret_cast<GLint *>(&size));

	#endif
}

#if C4PS4 //[ PS4

	// -- PS4 code hidden --

#elif C4PS3 //[ PS3

	// -- PS3 code hidden --

#endif //]


Render::FragmentShaderObject::FragmentShaderObject(const char *source, unsigned_int32 size)
{
	#if C4OPENGL

		shaderIdentifier = glCreateShader(GL_FRAGMENT_SHADER);
		glShaderSource(shaderIdentifier, 1, &source, reinterpret_cast<GLint *>(&size));

	#endif
}

#if C4PS4 //[ PS4

	// -- PS4 code hidden --

#endif //]


Render::GeometryShaderObject::GeometryShaderObject(const char *source, unsigned_int32 size)
{
	#if C4GLCORE

		shaderIdentifier = glCreateShader(GL_GEOMETRY_SHADER);
		glShaderSource(shaderIdentifier, 1, &source, reinterpret_cast<GLint *>(&size));

	#endif
}

#if C4PS4 //[ PS4

	// -- PS4 code hidden --

#endif //]


Render::ShaderProgramObject::ShaderProgramObject()
{
	#if C4OPENGL

		programIdentifier = glCreateProgram();
		programUpdateFlags = 0;

		geometryShader = nullptr;

	#endif
}

Render::ShaderProgramObject::~ShaderProgramObject()
{
	#if C4OPENGL

		if (renderState.shaderProgram == this)
		{
			renderState.shaderProgram = nullptr;
			glUseProgram(0);
		}

		glDeleteProgram(programIdentifier);

	#endif
}

#if C4OPENGL

	void Render::ShaderProgramObject::SetVertexShader(VertexShaderObject *shader)
	{
		vertexShader = shader;
		programUpdateFlags |= kUpdateVertexShaderParameters;
		glAttachShader(programIdentifier, shader->GetShaderIdentifier());
	}

	void Render::ShaderProgramObject::SetFragmentShader(FragmentShaderObject *shader)
	{
		fragmentShader = shader;
		programUpdateFlags |= kUpdateFragmentShaderParameters;
		glAttachShader(programIdentifier, shader->GetShaderIdentifier());
	}

	void Render::ShaderProgramObject::SetGeometryShader(GeometryShaderObject *shader)
	{
		geometryShader = shader;

		#if C4GLCORE

			programUpdateFlags |= kUpdateGeometryShaderParameters;
			glAttachShader(programIdentifier, shader->GetShaderIdentifier());

		#endif
	}

	void Render::ShaderProgramObject::Activate(void)
	{
		vertexShader->Compile();
		fragmentShader->Compile();

		#if C4GLCORE

			if (geometryShader)
			{
				geometryShader->Compile();
			}

		#endif

		glLinkProgram(programIdentifier);
	}

	bool Render::ShaderProgramObject::SetProgramBinary(unsigned_int32 format, const void *data, unsigned_int32 size)
	{
		GLint	status;

		glProgramBinary(programIdentifier, format, data, size);
		glGetProgramiv(programIdentifier, GL_LINK_STATUS, &status);
		return (status != 0);
	}

	#if C4DEBUG

		bool Render::ShaderProgramObject::GetProgramStatus(const char **error, const char **source) const
		{
			GLint	status;

			if (!vertexShader->GetShaderStatus(error, source))
			{
				return (false);
			}

			if (!fragmentShader->GetShaderStatus(error, source))
			{
				return (false);
			}

			#if C4GLCORE

				if ((geometryShader) && (!geometryShader->GetShaderStatus(error, source)))
				{
					return (false);
				}

			#endif

			glGetProgramiv(programIdentifier, GL_LINK_STATUS, &status);
			if (!status)
			{
				GLint	size;

				glGetProgramiv(programIdentifier, GL_INFO_LOG_LENGTH, &size);
				char *string = new char[size];
				glGetProgramInfoLog(programIdentifier, size, nullptr, string);
				*error = string;

				*source = nullptr;
				return (false);
			}

			return (true);
		}

		void Render::ShaderProgramObject::ReleaseProgramStatus(const char *error, const char *source) const
		{
			delete[] source;
			delete[] error;
		}

	#endif

#endif

void Render::ShaderProgramObject::Preprocess(void)
{
	#if C4OPENGL

		vertexUniformLocation = glGetUniformLocation(programIdentifier, "vparam");
		fragmentUniformLocation = glGetUniformLocation(programIdentifier, "fparam");

		#if C4GLCORE

			geometryUniformLocation = glGetUniformLocation(programIdentifier, "gparam");

		#endif

		unsigned_int32 blockIndex = glGetUniformBlockIndex(programIdentifier, "universal");
		if (blockIndex != GL_INVALID_INDEX)
		{
			glUniformBlockBinding(programIdentifier, blockIndex, 0);
		}

	#elif C4PS4 //[ PS4

		// -- PS4 code hidden --

	#endif //]
}


#if C4OPENGL

	void Render::PixelBufferObject::Construct(void)
	{
		glGenBuffers(1, &pixelBufferIdentifier);
	}

	void Render::PixelBufferObject::Destruct(void)
	{
		glDeleteBuffers(1, &pixelBufferIdentifier);
	}

	void Render::PixelBufferObject::AllocateStorage(unsigned_int32 size)
	{
		bufferSize = size;
		glNamedBufferDataEXT(pixelBufferIdentifier, size, nullptr, GL_STREAM_READ);
	}

#endif


void Render::TextureObject::Construct(unsigned_int32 index)
{
	targetIndex = (unsigned_int16) index;

	#if C4OPENGL

		openglTarget = textureTargetEnum[index];

		glGenTextures(1, &textureIdentifier);

	#elif C4PS4 //[ PS4

		// -- PS4 code hidden --

	#elif C4PS3 //[ PS3

		// -- PS3 code hidden --

	#endif //]
}

void Render::TextureObject::Destruct(void)
{
	#if C4OPENGL

		glDeleteTextures(1, &textureIdentifier);

		for (unsigned_machine unit = 0; unit < kMaxTextureUnitCount; unit++)
		{
			if (renderState.texture[unit][targetIndex] == textureIdentifier)
			{
				renderState.texture[unit][targetIndex] = 0;
			}
		}

	#elif C4PS4 //[ PS4

		// -- PS4 code hidden --

	#elif C4PS3 //[ PS3

		// -- PS3 code hidden --

	#endif //]
}

#if C4OPENGL

	void Render::TextureObject::Bind(unsigned_int32 unit) const
	{
		GLuint *object = &renderState.texture[unit][targetIndex];
		if (*object != textureIdentifier)
		{
			*object = textureIdentifier;
			glBindMultiTextureEXT(GL_TEXTURE0 + unit, openglTarget, textureIdentifier);
		}
	}

	void Render::TextureObject::Unbind(unsigned_int32 unit) const
	{
		GLuint *object = &renderState.texture[unit][targetIndex];
		if (*object == textureIdentifier)
		{
			*object = 0;
			glBindMultiTextureEXT(GL_TEXTURE0 + unit, openglTarget, 0);
		}
	}

	void Render::TextureObject::UnbindAll(void) const
	{
		for (machine a = 0; a < kMaxTextureUnitCount; a++)
		{
			GLuint *object = &renderState.texture[a][targetIndex];
			if (*object == textureIdentifier)
			{
				*object = 0;
				glBindMultiTextureEXT(GL_TEXTURE0 + a, openglTarget, 0);
			}
		}
	}

#elif C4PS4 //[ PS4

	// -- PS4 code hidden --

#elif C4PS3 //[ PS3

	// -- PS3 code hidden --

#endif //]

void Render::TextureObject::SetBorderColor(unsigned_int32 color)
{
	#if C4GLCORE

		static const ConstColorRGBA colorTable[3] =
		{
			{0.0F, 0.0F, 0.0F, 0.0F}, {0.0F, 0.0F, 0.0F, 1.0F}, {1.0F, 1.0F, 1.0F, 1.0F}
		};

		glTextureParameterfvEXT(textureIdentifier, openglTarget, GL_TEXTURE_BORDER_COLOR, &colorTable[color].red);

	#elif C4PS4 //[ PS4

		// -- PS4 code hidden --

	#elif C4PS3 //[ PS3

		// -- PS3 code hidden --

	#endif //]
}

void Render::TextureObject::SetImage2D(const TextureUploadData *uploadData)
{
	#if C4OPENGL

		glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);

		unsigned_int32 format = uploadData->format;
		const TextureFormatData *formatData = &textureFormatData[format];
		formatIndex = format;

		char *storage = nullptr;
		unsigned_int32 storageSize = 0;

		unsigned_int32 width = uploadData->width;
		unsigned_int32 height = uploadData->height;
		GLenum internal = formatData->internalFormat[uploadData->encoding];
		int32 count = uploadData->mipmapCount;
		const TextureImageData *imageData = uploadData->imageData;

		for (machine level = 0; level < count; level++)
		{
			unsigned_int32 pixelCount = width * height;
			unsigned_int32 size = pixelCount * formatData->pixelSize;
			storageSize += size;

			const void *image = imageData->image;
			if (imageData->decompressor)
			{
				if (!storage)
				{
					storage = new char[size];
				}

				(*imageData->decompressor)(static_cast<const unsigned_int8 *>(image), imageData->size, storage);
				image = storage;
			}

			glTextureImage2DEXT(textureIdentifier, GL_TEXTURE_2D, level, internal, width, height, 0, formatData->pixelFormat, formatData->pixelType, image);

			width = Max(width >> 1, 1);
			height = Max(height >> 1, 1);

			imageData++;
		}

		*uploadData->memorySize = storageSize;
		delete[] storage;

		SetSwizzle(formatData->textureSwizzle);

	#elif C4PS4 //[ PS4

		// -- PS4 code hidden --

	#elif C4PS3 //[ PS3

		// -- PS3 code hidden --

	#endif //]
}

void Render::TextureObject::SetImage3D(const TextureUploadData *uploadData)
{
	#if C4OPENGL

		glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);

		unsigned_int16 format = uploadData->format;
		const TextureFormatData *formatData = &textureFormatData[format];
		formatIndex = format;

		char *storage = nullptr;
		unsigned_int32 storageSize = 0;

		unsigned_int32 width = uploadData->width;
		unsigned_int32 height = uploadData->height;
		unsigned_int32 depth = uploadData->depth;
		GLenum internal = formatData->internalFormat[uploadData->encoding];
		int32 count = uploadData->mipmapCount;
		const TextureImageData *imageData = uploadData->imageData;

		for (machine level = 0; level < count; level++)
		{
			unsigned_int32 pixelCount = width * height * depth;
			unsigned_int32 size = pixelCount * formatData->pixelSize;
			storageSize += size;

			const void *image = imageData->image;
			if (imageData->decompressor)
			{
				if (!storage)
				{
					storage = new char[size];
				}

				(*imageData->decompressor)(static_cast<const unsigned_int8 *>(image), imageData->size, storage);
				image = storage;
			}

			glTextureImage3DEXT(textureIdentifier, GL_TEXTURE_3D, level, internal, width, height, depth, 0, formatData->pixelFormat, formatData->pixelType, image);

			width = Max(width >> 1, 1);
			height = Max(height >> 1, 1);
			depth = Max(depth >> 1, 1);

			imageData++;
		}

		*uploadData->memorySize = storageSize;
		delete[] storage;

		SetSwizzle(formatData->textureSwizzle);

	#elif C4PS4 //[ PS4

		// -- PS4 code hidden --

	#elif C4PS3 //[ PS3

		// -- PS3 code hidden --

	#endif //]
}

void Render::TextureObject::SetImageCube(const TextureUploadData *uploadData)
{
	#if C4OPENGL

		glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);

		unsigned_int16 format = uploadData->format;
		const TextureFormatData *formatData = &textureFormatData[format];
		formatIndex = format;

		char *storage = nullptr;
		unsigned_int32 storageSize = 0;

		unsigned_int32 width = uploadData->width;
		GLenum internal = formatData->internalFormat[uploadData->encoding];
		int32 count = uploadData->mipmapCount;
		const TextureImageData *imageData = uploadData->imageData;

		for (machine level = 0; level < count; level++)
		{
			for (machine component = 0; component < 6; component++)
			{
				unsigned_int32 pixelCount = width * width;
				unsigned_int32 size = pixelCount * formatData->pixelSize;
				storageSize += size;

				const void *image = imageData->image;
				if (imageData->decompressor)
				{
					if (!storage)
					{
						storage = new char[size];
					}

					(*imageData->decompressor)(static_cast<const unsigned_int8 *>(image), imageData->size, storage);
					image = storage;
				}

				glTextureImage2DEXT(textureIdentifier, GL_TEXTURE_CUBE_MAP_POSITIVE_X + component, level, internal, width, width, 0, formatData->pixelFormat, formatData->pixelType, image);
				imageData++;
			}

			width >>= 1;
		}

		*uploadData->memorySize = storageSize;
		delete[] storage;

		SetSwizzle(formatData->textureSwizzle);

	#elif C4PS4 //[ PS4

		// -- PS4 code hidden --

	#elif C4PS3 //[ PS3

		// -- PS3 code hidden --

	#endif //]
}

void Render::TextureObject::SetImageRect(const TextureUploadData *uploadData)
{
	#if C4OPENGL

		unsigned_int16 format = uploadData->format;
		const TextureFormatData *formatData = &textureFormatData[format];
		formatIndex = format;

		char *storage = nullptr;

		unsigned_int32 width = uploadData->width;
		unsigned_int32 height = uploadData->height;
		GLenum internal = formatData->internalFormat[uploadData->encoding];
		const TextureImageData *imageData = uploadData->imageData;

		int32 pixelCount = width * height;
		unsigned_int32 storageSize = pixelCount * formatData->pixelSize;

		const void *image = imageData->image;
		if (imageData->decompressor)
		{
			if (!storage)
			{
				storage = new char[storageSize];
			}

			(*imageData->decompressor)(static_cast<const unsigned_int8 *>(image), imageData->size, storage);
			image = storage;
		}

		glPixelStorei(GL_UNPACK_ROW_LENGTH, uploadData->rowLength);
		glTextureImage2DEXT(textureIdentifier, GL_TEXTURE_RECTANGLE, 0, internal, width, height, 0, formatData->pixelFormat, formatData->pixelType, image);

		*uploadData->memorySize = storageSize;
		delete[] storage;

		SetSwizzle(formatData->textureSwizzle);

	#elif C4PS4 //[ PS4

		// -- PS4 code hidden --

	#elif C4PS3 //[ PS3

		// -- PS3 code hidden --

	#endif //]
}

void Render::TextureObject::SetImageArray2D(const TextureUploadData *uploadData)
{
	#if C4OPENGL

		glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);

		unsigned_int16 format = uploadData->format;
		const TextureFormatData *formatData = &textureFormatData[format];
		formatIndex = format;

		char *storage = nullptr;
		unsigned_int32 storageSize = 0;

		unsigned_int32 width = uploadData->width;
		unsigned_int32 height = uploadData->height;
		unsigned_int32 depth = uploadData->depth;
		GLenum internal = formatData->internalFormat[uploadData->encoding];
		int32 count = uploadData->mipmapCount;
		const TextureImageData *imageData = uploadData->imageData;

		for (machine level = 0; level < count; level++)
		{
			unsigned_int32 pixelCount = width * height * depth;
			unsigned_int32 size = pixelCount * formatData->pixelSize;
			storageSize += size;

			const void *image = imageData->image;
			if (imageData->decompressor)
			{
				if (!storage)
				{
					storage = new char[size];
				}

				(*imageData->decompressor)(static_cast<const unsigned_int8 *>(image), imageData->size, storage);
				image = storage;
			}

			glTextureImage3DEXT(textureIdentifier, GL_TEXTURE_2D_ARRAY, level, internal, width, height, depth, 0, formatData->pixelFormat, formatData->pixelType, image);

			width = Max(width >> 1, 1);
			height = Max(height >> 1, 1);

			imageData++;
		}

		*uploadData->memorySize = storageSize;
		delete[] storage;

		SetSwizzle(formatData->textureSwizzle);

	#elif C4PS4 //[ PS4

		// -- PS4 code hidden --

	#endif //]
}

void Render::TextureObject::SetCompressedImage2D(const TextureUploadData *uploadData)
{
	#if C4OPENGL

		unsigned_int16 format = uploadData->format;
		const TextureFormatData *formatData = &textureFormatData[format];
		formatIndex = format;

		char *storage = nullptr;
		unsigned_int32 storageSize = 0;

		unsigned_int32 width = uploadData->width;
		unsigned_int32 height = uploadData->height;
		GLenum internal = formatData->internalFormat[uploadData->encoding];
		int32 count = uploadData->mipmapCount;
		const TextureImageData *imageData = uploadData->imageData;

		for (machine level = 0; level < count; level++)
		{
			const void *image = imageData->image;
			unsigned_int32 size = imageData->size;

			if (imageData->decompressor)
			{
				unsigned_int32 blockCount = ((width + 3) / 4) * ((height + 3) / 4);
				size = blockCount * formatData->pixelSize;
				if (!storage)
				{
					storage = new char[size];
				}

				(*imageData->decompressor)(static_cast<const unsigned_int8 *>(image), imageData->size, storage);
				image = storage;
			}

			storageSize += size;
			glCompressedTextureImage2DEXT(textureIdentifier, GL_TEXTURE_2D, level, internal, width, height, 0, size, image);

			width = Max(width >> 1, 1);
			height = Max(height >> 1, 1);

			imageData++;
		}

		*uploadData->memorySize = storageSize;
		delete[] storage;

	#elif C4PS4 //[ PS4

		// -- PS4 code hidden --

	#elif C4PS3 //[ PS3

		// -- PS3 code hidden --

	#endif //]
}

void Render::TextureObject::SetCompressedImageCube(const TextureUploadData *uploadData)
{
	#if C4OPENGL

		unsigned_int16 format = uploadData->format;
		const TextureFormatData *formatData = &textureFormatData[format];
		formatIndex = format;

		char *storage = nullptr;
		unsigned_int32 storageSize = 0;

		unsigned_int32 width = uploadData->width;
		GLenum internal = formatData->internalFormat[uploadData->encoding];
		int32 count = uploadData->mipmapCount;
		const TextureImageData *imageData = uploadData->imageData;

		for (machine level = 0; level < count; level++)
		{
			for (machine component = 0; component < 6; component++)
			{
				const void *image = imageData->image;
				unsigned_int32 size = imageData->size;

				if (imageData->decompressor)
				{
					unsigned_int32 w = (width + 3) / 4;
					unsigned_int32 blockCount = w * w;
					size = blockCount * formatData->pixelSize;
					if (!storage)
					{
						storage = new char[size];
					}

					(*imageData->decompressor)(static_cast<const unsigned_int8 *>(image), imageData->size, storage);
					image = storage;
				}

				storageSize += size;
				glCompressedTextureImage2DEXT(textureIdentifier, GL_TEXTURE_CUBE_MAP_POSITIVE_X + component, level, internal, width, width, 0, size, image);

				imageData++;
			}

			width >>= 1;
		}

		*uploadData->memorySize = storageSize;
		delete[] storage;

	#elif C4PS4 //[ PS4

		// -- PS4 code hidden --

	#elif C4PS3 //[ PS3

		// -- PS3 code hidden --

	#endif //]
}

void Render::TextureObject::SetCompressedImageArray2D(const TextureUploadData *uploadData)
{
	#if C4OPENGL

		unsigned_int16 format = uploadData->format;
		const TextureFormatData *formatData = &textureFormatData[format];
		formatIndex = format;

		char *storage = nullptr;
		unsigned_int32 storageSize = 0;

		unsigned_int32 width = uploadData->width;
		unsigned_int32 height = uploadData->height;
		unsigned_int32 depth = uploadData->depth;
		GLenum internal = formatData->internalFormat[uploadData->encoding];
		int32 count = uploadData->mipmapCount;
		const TextureImageData *imageData = uploadData->imageData;

		for (machine level = 0; level < count; level++)
		{
			const void *image = imageData->image;
			unsigned_int32 size = imageData->size;

			if (imageData->decompressor)
			{
				unsigned_int32 blockCount = ((width + 3) / 4) * ((height + 3) / 4) * depth;
				size = blockCount * formatData->pixelSize;
				if (!storage)
				{
					storage = new char[size];
				}

				(*imageData->decompressor)(static_cast<const unsigned_int8 *>(image), imageData->size, storage);
				image = storage;
			}

			storageSize += size;
			glCompressedTextureImage3DEXT(textureIdentifier, GL_TEXTURE_2D_ARRAY, level, internal, width, height, depth, 0, size, image);

			width = Max(width >> 1, 1);
			height = Max(height >> 1, 1);

			imageData++;
		}

		*uploadData->memorySize = storageSize;
		delete[] storage;

	#elif C4PS4 //[ PS4

		// -- PS4 code hidden --

	#endif //]
}

void Render::TextureObject::AllocateStorage2D(const TextureAllocationData *allocationData)
{
	#if C4OPENGL

		unsigned_int32 format = allocationData->format;
		const TextureFormatData *formatData = &textureFormatData[format];
		formatIndex = format;

		unsigned_int32 width = allocationData->width;
		unsigned_int32 height = allocationData->height;
		GLenum internal = formatData->internalFormat[allocationData->encoding];

		glTextureImage2DEXT(textureIdentifier, GL_TEXTURE_2D, 0, internal, width, height, 0, formatData->pixelFormat, formatData->pixelType, nullptr);
		SetFilterModes(Render::kFilterLinear, Render::kFilterLinear);
		SetSwizzle(formatData->textureSwizzle);

		Unbind(0);

		unsigned_int32 *memorySize = allocationData->memorySize;
		if (memorySize)
		{
			*memorySize = width * height * formatData->pixelSize;
		}

	#elif C4PS4 //[ PS4

		// -- PS4 code hidden --

	#elif C4PS3 //[ PS3

		// -- PS3 code hidden --

	#endif //]
}

void Render::TextureObject::AllocateStorageRect(const TextureAllocationData *allocationData)
{
	#if C4OPENGL

		unsigned_int32 format = allocationData->format;
		const TextureFormatData *formatData = &textureFormatData[format];
		formatIndex = format;

		unsigned_int32 width = allocationData->width;
		unsigned_int32 height = allocationData->height;
		GLenum internal = formatData->internalFormat[allocationData->encoding];

		glTextureImage2DEXT(textureIdentifier, GL_TEXTURE_RECTANGLE, 0, internal, width, height, 0, formatData->pixelFormat, formatData->pixelType, nullptr);
		SetFilterModes(Render::kFilterLinear, Render::kFilterLinear);
		SetSwizzle(formatData->textureSwizzle);

		Unbind(0);

		unsigned_int32 *memorySize = allocationData->memorySize;
		if (memorySize)
		{
			*memorySize = width * height * formatData->pixelSize;
		}

	#elif C4PS4 //[ PS4

		// -- PS4 code hidden --

	#elif C4PS3 //[ PS3

		// -- PS3 code hidden --

	#endif //]
}

void Render::TextureObject::AllocateStorageCube(const TextureAllocationData *allocationData)
{
	#if C4OPENGL

		unsigned_int32 format = allocationData->format;
		const TextureFormatData *formatData = &textureFormatData[format];
		formatIndex = format;

		unsigned_int32 width = allocationData->width;
		GLenum internal = formatData->internalFormat[allocationData->encoding];

		for (machine component = 0; component < 6; component++)
		{
			glTextureImage2DEXT(textureIdentifier, GL_TEXTURE_CUBE_MAP_POSITIVE_X + component, 0, internal, width, width, 0, formatData->pixelFormat, formatData->pixelType, nullptr);
		}

		SetFilterModes(Render::kFilterLinear, Render::kFilterLinear);
		SetSwizzle(formatData->textureSwizzle);

		Unbind(0);

		unsigned_int32 *memorySize = allocationData->memorySize;
		if (memorySize)
		{
			*memorySize = width * width * 6 * formatData->pixelSize;
		}

	#elif C4PS4 //[ PS4

		// -- PS4 code hidden --

	#endif //]
}

void Render::TextureObject::AllocateStorageArray2D(const TextureAllocationData *allocationData)
{
	#if C4OPENGL

		unsigned_int32 format = allocationData->format;
		const TextureFormatData *formatData = &textureFormatData[format];
		formatIndex = format;

		unsigned_int32 width = allocationData->width;
		unsigned_int32 height = allocationData->height;
		unsigned_int32 depth = allocationData->depth;
		GLenum internal = formatData->internalFormat[allocationData->encoding];

		glTextureImage3DEXT(textureIdentifier, GL_TEXTURE_2D_ARRAY, 0, internal, width, height, depth, 0, formatData->pixelFormat, formatData->pixelType, nullptr);
		SetFilterModes(Render::kFilterLinear, Render::kFilterLinear);
		SetSwizzle(formatData->textureSwizzle);

		Unbind(0);

		unsigned_int32 *memorySize = allocationData->memorySize;
		if (memorySize)
		{
			*memorySize = width * height * depth * formatData->pixelSize;
		}

	#elif C4PS4 //[ PS4

		// -- PS4 code hidden --

	#endif //]
}

void Render::TextureObject::UpdateImage2D(unsigned_int32 width, unsigned_int32 height, int32 count, const void *image) const
{
	#if C4OPENGL

		glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);

		const TextureFormatData *formatData = &textureFormatData[formatIndex];

		for (machine level = 0; level < count; level++)
		{
			glTextureSubImage2DEXT(textureIdentifier, GL_TEXTURE_2D, level, 0, 0, width, height, formatData->pixelFormat, formatData->pixelType, image);

			image = static_cast<const char *>(image) + width * height * formatData->pixelSize;

			width = Max(width >> 1, 1);
			height = Max(height >> 1, 1);
		}

	#endif
}

void Render::TextureObject::UpdateImage2D(unsigned_int32 x, unsigned_int32 y, unsigned_int32 width, unsigned_int32 height, unsigned_int32 rowLength, const void *image) const
{
	#if C4OPENGL

		glPixelStorei(GL_UNPACK_ROW_LENGTH, rowLength);

		const TextureFormatData *formatData = &textureFormatData[formatIndex];
		image = static_cast<const char *>(image) + (rowLength * y + x) * formatData->pixelSize;
		glTextureSubImage2DEXT(textureIdentifier, GL_TEXTURE_2D, 0, x, y, width, height, formatData->pixelFormat, formatData->pixelType, image);

	#endif
}

void Render::TextureObject::UpdateImage3D(unsigned_int32 x, unsigned_int32 y, unsigned_int32 z, unsigned_int32 width, unsigned_int32 height, unsigned_int32 depth, const void *image) const
{
	#if C4OPENGL

		glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);

		const TextureFormatData *formatData = &textureFormatData[formatIndex];
		image = static_cast<const char *>(image) + (width * (height * z + y) + x) * formatData->pixelSize;
		glTextureSubImage3DEXT(textureIdentifier, GL_TEXTURE_3D, 0, x, y, z, width, height, depth, formatData->pixelFormat, formatData->pixelType, image);

	#endif
}

void Render::TextureObject::UpdateImageRect(unsigned_int32 x, unsigned_int32 y, unsigned_int32 width, unsigned_int32 height, unsigned_int32 rowLength, const void *image) const
{
	#if C4OPENGL

		glPixelStorei(GL_UNPACK_ROW_LENGTH, rowLength);

		const TextureFormatData *formatData = &textureFormatData[formatIndex];
		image = static_cast<const char *>(image) + (rowLength * y + x) * formatData->pixelSize;
		glTextureSubImage2DEXT(textureIdentifier, GL_TEXTURE_RECTANGLE, 0, x, y, width, height, formatData->pixelFormat, formatData->pixelType, image);

	#elif C4PS4 //[ PS4

		// -- PS4 code hidden --

	#elif C4PS3 //[ PS3

		// -- PS3 code hidden --

	#endif //]
}


#if C4PS4 //[ PS4

	// -- PS4 code hidden --

#endif //]


void Render::RenderBufferObject::Construct(void)
{
	#if C4OPENGL

		glGenRenderbuffers(1, &renderBufferIdentifier);

	#elif C4PS4 //[ PS4

		// -- PS4 code hidden --

	#endif //]
}

void Render::RenderBufferObject::Destruct(void)
{
	#if C4OPENGL

		glDeleteRenderbuffers(1, &renderBufferIdentifier);

	#elif C4PS4 //[ PS4

		// -- PS4 code hidden --

	#elif C4PS3 //[

		Render::ReleaseRenderBufferMemory(bufferOffset);

	#endif //]
}

void Render::RenderBufferObject::AllocateStorage(unsigned_int32 width, unsigned_int32 height, unsigned_int32 format, bool display)
{
	#if C4OPENGL

		glBindRenderbuffer(GL_RENDERBUFFER, renderBufferIdentifier);
		glRenderbufferStorage(GL_RENDERBUFFER, format, width, height);
		glBindRenderbuffer(GL_RENDERBUFFER, 0);

	#elif C4PS4 //[ PS4

		// -- PS4 code hidden --

	#elif C4PS3 //[ PS3

		// -- PS3 code hidden --

	#endif //]
}

void Render::RenderBufferObject::AllocateMultisampleStorage(unsigned_int32 width, unsigned_int32 height, unsigned_int32 sampleCount, unsigned_int32 format)
{
	#if C4OPENGL

		glBindRenderbuffer(GL_RENDERBUFFER, renderBufferIdentifier);
		glRenderbufferStorageMultisample(GL_RENDERBUFFER, sampleCount, format, width, height);
		glBindRenderbuffer(GL_RENDERBUFFER, 0);

	#elif C4PS4 //[ PS4

		// -- PS4 code hidden --

	#elif C4PS3 //[ PS3

		// -- PS3 code hidden --

	#endif //]
}


void Render::FrameBufferObject::Construct(void)
{
	#if C4OPENGL

		glGenFramebuffers(1, &frameBufferIdentifier);

	#elif C4PS4 //[ PS4

		// -- PS4 code hidden --

	#elif C4PS3 //[ PS3

		// -- PS3 code hidden --

	#endif //]

	currentColorTexture = nullptr;
	currentVelocityTexture = nullptr;
	currentDepthTexture = nullptr;
}

void Render::FrameBufferObject::Destruct(void)
{
	#if C4OPENGL

		glDeleteFramebuffers(1, &frameBufferIdentifier);

	#endif
}

void Render::FrameBufferObject::SetColorRenderBuffer(const RenderBufferObject *renderBuffer)
{
	#if C4OPENGL

		Bind();
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, renderBuffer->renderBufferIdentifier);

	#elif C4PS4 //[ PS4

		// -- PS4 code hidden --

	#elif C4PS3 //[ PS3

		// -- PS3 code hidden --

	#endif //]
}

void Render::FrameBufferObject::SetDepthStencilRenderBuffer(const RenderBufferObject *renderBuffer)
{
	#if C4OPENGL

		Bind();
		GLuint renderBufferIdentifier = renderBuffer->renderBufferIdentifier;
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, renderBufferIdentifier);
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_STENCIL_ATTACHMENT, GL_RENDERBUFFER, renderBufferIdentifier);

	#elif C4PS4 //[ PS4

		// -- PS4 code hidden --

	#elif C4PS3 //[ PS3

		// -- PS3 code hidden --

	#endif //]
}

void Render::FrameBufferObject::SetColorRenderTexture(TextureObject *renderTexture)
{
	if (currentColorTexture != renderTexture)
	{
		#if C4OPENGL

			currentColorTexture = renderTexture;

			Bind();
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_RECTANGLE, renderTexture->textureIdentifier, 0);

		#elif C4PS4 //[ PS4

			// -- PS4 code hidden --

		#elif C4PS3 //[ PS3

			// -- PS3 code hidden --

		#endif //]
	}
}

void Render::FrameBufferObject::SetVelocityRenderTexture(TextureObject *renderTexture)
{
	if (currentVelocityTexture != renderTexture)
	{
		#if C4OPENGL

			static const GLenum drawBuffers[2] = {GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1};

			currentVelocityTexture = renderTexture;

			Bind();
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_RECTANGLE, renderTexture->textureIdentifier, 0);
			glDrawBuffers(2, drawBuffers);

		#elif C4PS4 //[ PS4

			// -- PS4 code hidden --

		#endif //]
	}
}

void Render::FrameBufferObject::SetDepthRenderTexture(TextureObject *renderTexture)
{
	if (currentDepthTexture != renderTexture)
	{
		#if C4OPENGL

			currentDepthTexture = renderTexture;

			Bind();
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, renderTexture->textureIdentifier, 0);

			glDrawBuffers(0, nullptr);
			glReadBuffer(GL_NONE);

		#elif C4PS4 //[ PS4

			// -- PS4 code hidden --

		#elif C4PS3 //[ PS3

			// -- PS3 code hidden --

		#endif //]
	}
}

void Render::FrameBufferObject::SetDepthRenderTextureLayer(TextureObject *renderTexture, int32 layer)
{
	#if C4PS4 //[ PS4

		// -- PS4 code hidden --

	#endif //]

	if (currentDepthTexture != renderTexture)
	{
		#if C4OPENGL

			currentDepthTexture = renderTexture;
			currentDepthLayer = layer;

			Bind();
			glFramebufferTextureLayer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, renderTexture->textureIdentifier, 0, layer);

			glDrawBuffers(0, nullptr);
			glReadBuffer(GL_NONE);

		#elif C4PS4 //[ PS4

			// -- PS4 code hidden --

		#endif //]
	}
	else if (currentDepthLayer != layer)
	{
		#if C4OPENGL

			currentDepthLayer = layer;
			glFramebufferTextureLayer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, renderTexture->textureIdentifier, 0, layer);

		#elif C4PS4 //[ PS4

			// -- PS4 code hidden --

		#endif //]
	}
}

#if C4OPENGL

	void Render::FrameBufferObject::SetDepthRenderTextureCubeFace(TextureObject *renderTexture, int32 face)
	{
		if (currentDepthTexture != renderTexture)
		{
			currentDepthTexture = renderTexture;
			currentDepthLayer = face;

			Bind();
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_CUBE_MAP_POSITIVE_X + face, renderTexture->textureIdentifier, 0);

			glDrawBuffers(0, nullptr);
			glReadBuffer(GL_NONE);
		}
		else if (currentDepthLayer != face)
		{
			currentDepthLayer = face;
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_CUBE_MAP_POSITIVE_X + face, renderTexture->textureIdentifier, 0);
		}
	}

#endif

void Render::FrameBufferObject::ResetColorRenderTexture(void)
{
	if (currentColorTexture)
	{
		currentColorTexture = nullptr;

		#if C4OPENGL

			Bind();
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_RECTANGLE, 0, 0);

		#elif C4PS4 //[ PS4

			// -- PS4 code hidden --

		#endif //]
	}
}

void Render::FrameBufferObject::ResetVelocityRenderTexture(void)
{
	if (currentVelocityTexture)
	{
		currentVelocityTexture = nullptr;

		#if C4OPENGL

			static const GLenum drawBuffers[1] = {GL_COLOR_ATTACHMENT0};

			Bind();
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_RECTANGLE, 0, 0);
			glDrawBuffers(1, drawBuffers);

		#elif C4PS4 //[ PS4

			// -- PS4 code hidden --

		#endif //]
	}
}

#if C4PS4 //[ PS4

	// -- PS4 code hidden --

#endif //]


void Render::Initialize(void)
{
	#if C4OPENGL

		renderState.imageUnit = 0;
		for (machine unit = 0; unit < kMaxTextureUnitCount; unit++)
		{
			for (machine target = 0; target < kTextureTargetCount; target++)
			{
				renderState.texture[unit][target] = 0;
			}
		}

		renderState.drawFrameBuffer = nullptr;
		renderState.readFrameBuffer = nullptr;

		renderState.vertexArrayObject = 0;
		renderState.attributeVertexBuffer = 0;
		renderState.indexVertexBuffer = 0;

		renderState.frameCount = 0;
		renderState.updateFlags = 0;

		renderState.vertexArrayMask = 0;
		renderState.vertexDataObject = nullptr;

		renderState.shaderProgram = nullptr;

	#elif C4PS4 //[ PS4

		// -- PS4 code hidden --

	#elif C4PS3 //[ PS3

		// -- PS3 code hidden --

	#endif //]

	for (machine a = 0; a < kMaxVertexParamCount; a++)
	{
		renderState.vertexShaderParam[a].Set(0.0F, 0.0F, 0.0F, 0.0F);
	}

	for (machine a = 0; a < kMaxFragmentParamCount; a++)
	{
		renderState.fragmentShaderParam[a].Set(0.0F, 0.0F, 0.0F, 0.0F);
	}

	for (machine a = 0; a < kMaxGeometryParamCount; a++)
	{
		renderState.geometryShaderParam[a].Set(0.0F, 0.0F, 0.0F, 0.0F);
	}

	for (machine a = 0; a < kMaxUniversalParamCount; a++)
	{
		renderState.universalShaderParam[a].Set(0.0F, 0.0F, 0.0F, 0.0F);
	}
}

void Render::Terminate(void)
{
	#if C4PS4 //[ PS4

		// -- PS4 code hidden --

	#elif C4PS3 //[ PS3

		// -- PS3 code hidden --

	#endif //]
}

#if C4OPENGL

	void Render::InitializeCoreOpenGL(void)
	{
		quadIndexBuffer.SetVertexBufferSize(kMaxQuadPrimitiveCount * 2 * sizeof(Triangle));
		quadIndexBuffer.AllocateStorage(quadTriangle);

		glGenVertexArrays(1, &renderState.vertexArrayObject);
		glBindVertexArray(renderState.vertexArrayObject);

		new(renderState.universalParameterBuffer) ParameterBufferObject(kParameterBufferUniversal);
	}

	void Render::TerminateCoreOpenGL(void)
	{
		renderState.universalParameterBuffer->~ParameterBufferObject();

		glDeleteVertexArrays(1, &renderState.vertexArrayObject);

		quadIndexBuffer.Destruct();
	}

	void Render::BindTextureArray(const TextureArrayObject *textureArray)
	{
		int32 count = textureArray->textureCount;
		if (count != 0)
		{
			const int8 *target = textureArray->textureTarget;
			for (machine a = 0;;)
			{
				renderState.texture[a][target[a]] = textureArray->textureHandle[a];

				if (++a >= count)
				{
					break;
				}
			}

			glBindTextures(0, count, textureArray->textureHandle);
		}
	}

	void Render::SetShaderProgram(const ShaderProgramObject *shaderProgram)
	{
		if (renderState.shaderProgram != shaderProgram)
		{
			renderState.shaderProgram = shaderProgram;
			glUseProgram(shaderProgram->programIdentifier);

			renderState.updateFlags |= shaderProgram->programUpdateFlags;
		}
	}

	void Render::Update(bool indexed)
	{
		unsigned_int32 flags = renderState.updateFlags;
		renderState.updateFlags = 0;

		if (flags & kUpdateVertexDataObject)
		{
			const VertexDataObject *vertexData = renderState.vertexDataObject;
			const VertexAttribData *attribData = vertexData->vertexAttribData;

			unsigned_int32 arrayMask = vertexData->vertexArrayMask;
			unsigned_int32 currentMask = renderState.vertexArrayMask;
			renderState.vertexArrayMask = arrayMask;

			for (machine a = 0; a < kVertexAttribCount; a++)
			{
				if (arrayMask & 1)
				{
					if (!(currentMask & 1))
					{
						glEnableVertexAttribArray(a);
					}

					glVertexArrayVertexAttribOffsetEXT(renderState.vertexArrayObject, attribData->buffer, a, attribData->size, attribData->type, true, attribData->stride, attribData->offset);
				}
				else
				{
					if (currentMask & 1)
					{
						glDisableVertexAttribArray(a);
					}
				}

				arrayMask >>= 1;
				currentMask >>= 1;
				attribData++;
			}

			if (indexed)
			{
				GLuint identifier = vertexData->indexBuffer;
				if (renderState.indexVertexBuffer != identifier)
				{
					renderState.indexVertexBuffer = identifier;
					glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, identifier);
				}
			}
		}

		const ShaderProgramObject *program = renderState.shaderProgram;
		if (program)
		{
			GLuint identifier = program->programIdentifier;

			if (flags & kUpdateVertexShaderParameters)
			{
				glProgramUniform4fvEXT(identifier, program->vertexUniformLocation, kMaxVertexParamCount, &renderState.vertexShaderParam[0].x);
			}

			if (flags & kUpdateFragmentShaderParameters)
			{
				glProgramUniform4fvEXT(identifier, program->fragmentUniformLocation, kMaxFragmentParamCount, &renderState.fragmentShaderParam[0].x);
			}

			#if C4GLCORE

				if (flags & kUpdateGeometryShaderParameters)
				{
					glProgramUniform4fvEXT(identifier, program->geometryUniformLocation, kMaxGeometryParamCount, &renderState.geometryShaderParam[0].x);
				}

			#endif
		}

		if (flags & kUpdateUniversalShaderParameters)
		{
			renderState.universalParameterBuffer->UpdateBuffer(renderState.universalShaderParam);
		}
	}

	void Render::BindTextureUnit0(GLuint texture, GLenum target)
	{
		if (renderState.imageUnit != 0)
		{
			renderState.imageUnit = 0;
			glActiveTexture(GL_TEXTURE0);
		}

		machine targetIndex = 0;
		targetIndex += ((int32) (GL_TEXTURE_2D - target) >> 31) & 1;
		targetIndex += ((int32) (GL_TEXTURE_3D - target) >> 31) & 1;
		targetIndex += ((int32) (GL_TEXTURE_RECTANGLE - target) >> 31) & 1;
		targetIndex += ((int32) (GL_TEXTURE_CUBE_MAP - target) >> 31) & 1;		// kTextureTargetCount - 1

		GLuint *object = &renderState.texture[0][targetIndex];
		if (*object != texture)
		{
			*object = texture;
			glBindTexture(target, texture);
		}
	}

	void Render::BindMultiTexture(GLenum texunit, GLenum target, GLuint texture)
	{
		int32 unitIndex = texunit - GL_TEXTURE0;
		if (renderState.imageUnit != unitIndex)
		{
			renderState.imageUnit = unitIndex;
			glActiveTexture(texunit);
		}

		glBindTexture(target, texture);
	}

	void Render::TextureParameteri(GLuint texture, GLenum target, GLenum pname, GLint param)
	{
		BindTextureUnit0(texture, target);
		glTexParameteri(target, pname, param);
	}

	void Render::TextureParameteriv(GLuint texture, GLenum target, GLenum pname, const GLint *param)
	{
		BindTextureUnit0(texture, target);
		glTexParameteriv(target, pname, param);
	}

	void Render::TextureParameterf(GLuint texture, GLenum target, GLenum pname, GLfloat param)
	{
		BindTextureUnit0(texture, target);
		glTexParameterf(target, pname, param);
	}

	void Render::TextureParameterfv(GLuint texture, GLenum target, GLenum pname, const GLfloat *param)
	{
		BindTextureUnit0(texture, target);
		glTexParameterfv(target, pname, param);
	}

	void Render::TextureImage2D(GLuint texture, GLenum target, GLint level, GLint internalformat, GLsizei width, GLsizei height, GLint border, GLenum format, GLenum type, const void *pixels)
	{
		// Taking the minimum with GL_TEXTURE_CUBE_MAP remaps all of the pseudo-targets for the
		// individual cube faces to GL_TEXTURE_CUBE_MAP since their enums have greater values.

		BindTextureUnit0(texture, Min(target, GL_TEXTURE_CUBE_MAP));
		glTexImage2D(target, level, internalformat, width, height, border, format, type, pixels);
	}

	void Render::TextureSubImage2D(GLuint texture, GLenum target, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLenum type, const void *pixels)
	{
		BindTextureUnit0(texture, Min(target, GL_TEXTURE_CUBE_MAP));
		glTexSubImage2D(target, level, xoffset, yoffset, width, height, format, type, pixels);
	}

	void Render::TextureImage3D(GLuint texture, GLenum target, GLint level, GLint internalformat, GLsizei width, GLsizei height, GLsizei depth, GLint border, GLenum format, GLenum type, const void *pixels)
	{
		BindTextureUnit0(texture, target);
		glTexImage3D(target, level, internalformat, width, height, depth, border, format, type, pixels);
	}

	void Render::TextureSubImage3D(GLuint texture, GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLsizei width, GLsizei height, GLsizei depth, GLenum format, GLenum type, const void *pixels)
	{
		BindTextureUnit0(texture, Min(target, GL_TEXTURE_CUBE_MAP));
		glTexSubImage3D(target, level, xoffset, yoffset, zoffset, width, height, depth, format, type, pixels);
	}

	void Render::CompressedTextureImage2D(GLuint texture, GLenum target, GLint level, GLenum internalformat, GLsizei width, GLsizei height, GLint border, GLsizei imageSize, const void *data)
	{
		BindTextureUnit0(texture, Min(target, GL_TEXTURE_CUBE_MAP));

		#if C4GLCORE

			glCompressedTexImage2D(target, level, internalformat, width, height, border, imageSize, data);

		#else //[ MOBILE

			// -- Mobile code hidden --

		#endif //]
	}

	void Render::CompressedTextureImage3D(GLuint texture, GLenum target, GLint level, GLenum internalformat, GLsizei width, GLsizei height, GLsizei depth, GLint border, GLsizei imageSize, const void *data)
	{
		BindTextureUnit0(texture, target);

		#if C4GLCORE

			glCompressedTexImage3D(target, level, internalformat, width, height, depth, border, imageSize, data);

		#else //[ MOBILE

			// -- Mobile code hidden --

		#endif //]
	}

	void Render::CopyTextureSubImage2D(GLuint texture, GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint x, GLint y, GLsizei width, GLsizei height)
	{
		BindTextureUnit0(texture, Min(target, GL_TEXTURE_CUBE_MAP));
		glCopyTexSubImage2D(target, level, xoffset, yoffset, x, y, width, height);
	}

	void Render::NamedBufferData(GLuint buffer, GLsizeiptr size, const void *data, GLenum usage)
	{
		if (renderState.attributeVertexBuffer != buffer)
		{
			renderState.attributeVertexBuffer = buffer;
			glBindBuffer(GL_ARRAY_BUFFER, buffer);
		}

		glBufferData(GL_ARRAY_BUFFER, size, data, usage);
	}

	void Render::NamedBufferSubData(GLuint buffer, GLintptr offset, GLsizeiptr size, const void *data)
	{
		GLuint boundBuffer = renderState.attributeVertexBuffer;
		renderState.attributeVertexBuffer = 0;

		if (boundBuffer != buffer)
		{
			glBindBuffer(GL_ARRAY_BUFFER, buffer);
		}

		glBufferSubData(GL_ARRAY_BUFFER, offset, size, data);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}

	void Render::GetNamedBufferSubData(GLuint buffer, GLintptr offset, GLsizeiptr size, void *data)
	{
		GLuint boundBuffer = renderState.attributeVertexBuffer;
		renderState.attributeVertexBuffer = 0;

		if (boundBuffer != buffer)
		{
			glBindBuffer(GL_ARRAY_BUFFER, buffer);
		}

		#if C4GLCORE

			glGetBufferSubData(GL_ARRAY_BUFFER, offset, size, data);

		#else //[ MOBILE

			// -- Mobile code hidden --

		#endif //]

		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}

	void *Render::MapNamedBufferRange(GLuint buffer, GLintptr offset, GLsizeiptr length, GLbitfield access)
	{
		if (renderState.attributeVertexBuffer != buffer)
		{
			renderState.attributeVertexBuffer = buffer;
			glBindBuffer(GL_ARRAY_BUFFER, buffer);
		}

		return (glMapBufferRange(GL_ARRAY_BUFFER, offset, length, access));
	}

	GLboolean Render::UnmapNamedBuffer(GLuint buffer)
	{
		GLuint boundBuffer = renderState.attributeVertexBuffer;
		renderState.attributeVertexBuffer = 0;

		if (boundBuffer != buffer)
		{
			glBindBuffer(GL_ARRAY_BUFFER, buffer);
		}

		GLboolean result = glUnmapBuffer(GL_ARRAY_BUFFER);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		return (result);
	}

	void Render::ProgramUniform4fv(GLuint program, GLint location, GLsizei count, const GLfloat *param)
	{
		if ((renderState.shaderProgram) && (renderState.shaderProgram->programIdentifier != program))
		{
			renderState.shaderProgram = nullptr;
			glUseProgram(program);
		}

		glUniform4fv(location, count, param);
	}

	void Render::VertexArrayVertexAttribOffset(GLuint vaobj, GLuint buffer, GLuint index, GLint size, GLenum type, GLboolean normalized, GLsizei stride, GLintptr offset)
	{
		if (renderState.vertexArrayObject != vaobj)
		{
			renderState.vertexArrayObject = vaobj;
			glBindVertexArray(vaobj);
		}

		if (renderState.attributeVertexBuffer != buffer)
		{
			renderState.attributeVertexBuffer = buffer;
			glBindBuffer(GL_ARRAY_BUFFER, buffer);
		}

		glVertexAttribPointer(index, size, type, normalized, stride, (char *) 0 + offset);
	}

	void Render::VertexArrayVertexAttribDivisor(GLuint vaobj, GLuint index, GLuint divisor)
	{
		if (renderState.vertexArrayObject != vaobj)
		{
			renderState.vertexArrayObject = vaobj;
			glBindVertexArray(vaobj);
		}

		glVertexAttribDivisor(index, divisor);
	}

	void Render::EnableVertexArrayAttrib(GLuint vaobj, GLuint index)
	{
		if (renderState.vertexArrayObject != vaobj)
		{
			renderState.vertexArrayObject = vaobj;
			glBindVertexArray(vaobj);
		}

		glEnableVertexAttribArray(index);
	}

	void Render::DisableVertexArrayAttrib(GLuint vaobj, GLuint index)
	{
		if (renderState.vertexArrayObject != vaobj)
		{
			renderState.vertexArrayObject = vaobj;
			glBindVertexArray(vaobj);
		}

		glDisableVertexAttribArray(index);
	}

	void Render::PolygonOffsetClamp(GLfloat factor, GLfloat units, GLfloat clamp)
	{
		glPolygonOffset(factor, units);
	}

	void Render::TextureStorage2D(GLuint texture, GLenum target, GLsizei levels, GLenum internalformat, GLsizei width, GLsizei height)
	{
		BindTextureUnit0(texture, target);
		glTexStorage2D(target, levels, internalformat, width, height);
	}

	void Render::TextureStorage3D(GLuint texture, GLenum target, GLsizei levels, GLenum internalformat, GLsizei width, GLsizei height, GLsizei depth)
	{
		BindTextureUnit0(texture, target);
		glTexStorage3D(target, levels, internalformat, width, height, depth);
	}

	void Render::InvalidateTexImage(GLuint texture, GLint level)
	{
	}

	void Render::InvalidateBufferData(GLuint buffer)
	{
	}

	void Render::InvalidateFramebuffer(GLenum target, GLsizei numAttachments, const GLenum *attachments)
	{
	}

	void Render::BindTextures(GLuint first, GLsizei count, const GLuint *textures)
	{
		const int8 *target = reinterpret_cast<const int8 *>(textures + (kMaxShaderTextureCount - first));
		const unsigned_int16 *targetEnum = textureTargetEnum;

		GLenum endunit = GL_TEXTURE0 + count;
		for (GLenum texunit = GL_TEXTURE0 + first; texunit < endunit; texunit++)
		{
			glBindMultiTextureEXT(texunit, targetEnum[*target], *textures);
			textures++;
			target++;
		}
	}

	#if C4GLCORE

		void Render::DepthBounds(GLclampd, GLclampd)
		{
			// NOP implementation, called when DBT not available.
		}

	#endif

	#if C4GLES //[ MOBILE

		// -- Mobile code hidden --

	#endif //]

#elif C4PS4 //[ PS4

	// -- PS4 code hidden --

#elif C4PS3 //[ PS3

	// -- PS3 code hidden --

#endif //]

void Render::SetVertexShaderParameter(unsigned_int32 index, float x, float y, float z, float w)
{
	renderState.vertexShaderParam[index].Set(x, y, z, w);
	renderState.updateFlags |= kUpdateVertexShaderParameters;
}

void Render::SetVertexShaderParameter(unsigned_int32 index, const float *v)
{
	renderState.vertexShaderParam[index].Set(v[0], v[1], v[2], v[3]);
	renderState.updateFlags |= kUpdateVertexShaderParameters;
}

void Render::SetVertexShaderParameter(unsigned_int32 index, const Vector3D& v)
{
	renderState.vertexShaderParam[index].Set(v.x, v.y, v.z, 0.0F);
	renderState.updateFlags |= kUpdateVertexShaderParameters;
}

void Render::SetVertexShaderParameter(unsigned_int32 index, const Vector4D& v)
{
	renderState.vertexShaderParam[index].Set(v.x, v.y, v.z, v.w);
	renderState.updateFlags |= kUpdateVertexShaderParameters;
}

void Render::SetVertexShaderParameter(unsigned_int32 index, const Antivector4D& v)
{
	renderState.vertexShaderParam[index].Set(v.x, v.y, v.z, v.w);
	renderState.updateFlags |= kUpdateVertexShaderParameters;
}

void Render::SetVertexShaderParameter(unsigned_int32 index, const Point3D& p)
{
	renderState.vertexShaderParam[index].Set(p.x, p.y, p.z, 1.0F);
	renderState.updateFlags |= kUpdateVertexShaderParameters;
}

void Render::SetVertexShaderParameter(unsigned_int32 index, const Matrix4D& m)
{
	renderState.vertexShaderParam[index].Set(m(0,0), m(0,1), m(0,2), m(0,3));
	renderState.vertexShaderParam[index + 1].Set(m(1,0), m(1,1), m(1,2), m(1,3));
	renderState.vertexShaderParam[index + 2].Set(m(2,0), m(2,1), m(2,2), m(2,3));
	renderState.vertexShaderParam[index + 3].Set(m(3,0), m(3,1), m(3,2), m(3,3));
	renderState.updateFlags |= kUpdateVertexShaderParameters;
}

void Render::SetVertexShaderParameter(unsigned_int32 index, const Transform4D& m)
{
	renderState.vertexShaderParam[index].Set(m(0,0), m(0,1), m(0,2), m(0,3));
	renderState.vertexShaderParam[index + 1].Set(m(1,0), m(1,1), m(1,2), m(1,3));
	renderState.vertexShaderParam[index + 2].Set(m(2,0), m(2,1), m(2,2), m(2,3));
	renderState.updateFlags |= kUpdateVertexShaderParameters;
}

void Render::SetFragmentShaderParameter(unsigned_int32 index, float x, float y, float z, float w)
{
	renderState.fragmentShaderParam[index].Set(x, y, z, w);
	renderState.updateFlags |= kUpdateFragmentShaderParameters;
}

void Render::SetFragmentShaderParameter(unsigned_int32 index, const float *v)
{
	renderState.fragmentShaderParam[index].Set(v[0], v[1], v[2], v[3]);
	renderState.updateFlags |= kUpdateFragmentShaderParameters;
}

void Render::SetFragmentShaderParameter(unsigned_int32 index, const Vector3D& v)
{
	renderState.fragmentShaderParam[index].Set(v.x, v.y, v.z, 0.0F);
	renderState.updateFlags |= kUpdateFragmentShaderParameters;
}

void Render::SetFragmentShaderParameter(unsigned_int32 index, const Point3D& p)
{
	renderState.fragmentShaderParam[index].Set(p.x, p.y, p.z, 1.0F);
	renderState.updateFlags |= kUpdateFragmentShaderParameters;
}

void Render::SetFragmentShaderParameter(unsigned_int32 index, const Vector4D& v)
{
	renderState.fragmentShaderParam[index].Set(v.x, v.y, v.z, v.w);
	renderState.updateFlags |= kUpdateFragmentShaderParameters;
}

void Render::SetFragmentShaderParameter(unsigned_int32 index, const Antivector4D& v)
{
	renderState.fragmentShaderParam[index].Set(v.x, v.y, v.z, v.w);
	renderState.updateFlags |= kUpdateFragmentShaderParameters;
}

void Render::SetFragmentShaderParameter(unsigned_int32 index, const ColorRGB& c)
{
	renderState.fragmentShaderParam[index].Set(c.red, c.green, c.blue, 1.0F);
	renderState.updateFlags |= kUpdateFragmentShaderParameters;
}

void Render::SetFragmentShaderParameter(unsigned_int32 index, const ColorRGBA& c)
{
	renderState.fragmentShaderParam[index].Set(c.red, c.green, c.blue, c.alpha);
	renderState.updateFlags |= kUpdateFragmentShaderParameters;
}

void Render::SetGeometryShaderParameter(unsigned_int32 index, float x, float y, float z, float w)
{
	renderState.geometryShaderParam[index].Set(x, y, z, w);
	renderState.updateFlags |= kUpdateGeometryShaderParameters;
}

void Render::SetGeometryShaderParameter(unsigned_int32 index, const float *v)
{
	renderState.geometryShaderParam[index].Set(v[0], v[1], v[2], v[3]);
	renderState.updateFlags |= kUpdateGeometryShaderParameters;
}

void Render::SetGeometryShaderParameter(unsigned_int32 index, const Vector4D& v)
{
	renderState.geometryShaderParam[index].Set(v.x, v.y, v.z, v.w);
	renderState.updateFlags |= kUpdateGeometryShaderParameters;
}

void Render::SetGeometryShaderParameter(unsigned_int32 index, const Antivector4D& v)
{
	renderState.geometryShaderParam[index].Set(v.x, v.y, v.z, v.w);
	renderState.updateFlags |= kUpdateGeometryShaderParameters;
}

void Render::SetGeometryShaderParameter(unsigned_int32 index, const Matrix4D& m)
{
	renderState.geometryShaderParam[index].Set(m(0,0), m(0,1), m(0,2), m(0,3));
	renderState.geometryShaderParam[index + 1].Set(m(1,0), m(1,1), m(1,2), m(1,3));
	renderState.geometryShaderParam[index + 2].Set(m(2,0), m(2,1), m(2,2), m(2,3));
	renderState.geometryShaderParam[index + 3].Set(m(3,0), m(3,1), m(3,2), m(3,3));
	renderState.updateFlags |= kUpdateGeometryShaderParameters;
}

void Render::SetGeometryShaderParameter(unsigned_int32 index, const Transform4D& m)
{
	renderState.geometryShaderParam[index].Set(m(0,0), m(0,1), m(0,2), m(0,3));
	renderState.geometryShaderParam[index + 1].Set(m(1,0), m(1,1), m(1,2), m(1,3));
	renderState.geometryShaderParam[index + 2].Set(m(2,0), m(2,1), m(2,2), m(2,3));
	renderState.updateFlags |= kUpdateGeometryShaderParameters;
}

void Render::SetUniversalShaderParameter(unsigned_int32 index, float x, float y, float z, float w)
{
	renderState.universalShaderParam[index].Set(x, y, z, w);
	renderState.updateFlags |= kUpdateUniversalShaderParameters;
}

void Render::SetUniversalShaderParameter(unsigned_int32 index, const float *v)
{
	renderState.universalShaderParam[index].Set(v[0], v[1], v[2], v[3]);
	renderState.updateFlags |= kUpdateUniversalShaderParameters;
}

void Render::SetUniversalShaderParameter(unsigned_int32 index, const Vector3D& v)
{
	renderState.universalShaderParam[index].Set(v.x, v.y, v.z, 0.0F);
	renderState.updateFlags |= kUpdateUniversalShaderParameters;
}

void Render::SetUniversalShaderParameter(unsigned_int32 index, const Point3D& p)
{
	renderState.universalShaderParam[index].Set(p.x, p.y, p.z, 1.0F);
	renderState.updateFlags |= kUpdateUniversalShaderParameters;
}

void Render::SetUniversalShaderParameter(unsigned_int32 index, const Vector4D& v)
{
	renderState.universalShaderParam[index].Set(v.x, v.y, v.z, v.w);
	renderState.updateFlags |= kUpdateUniversalShaderParameters;
}

void Render::SetUniversalShaderParameter(unsigned_int32 index, const ColorRGB& c)
{
	renderState.universalShaderParam[index].Set(c.red, c.green, c.blue, 1.0F);
	renderState.updateFlags |= kUpdateUniversalShaderParameters;
}

void Render::SetUniversalShaderParameter(unsigned_int32 index, const ColorRGBA& c)
{
	renderState.universalShaderParam[index].Set(c.red, c.green, c.blue, c.alpha);
	renderState.updateFlags |= kUpdateUniversalShaderParameters;
}

// ZYUQURM
