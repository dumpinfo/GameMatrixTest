 

#include "C4Graphics.h"
#include "C4World.h"
#include "C4Movies.h"


#if C4WINDOWS

	extern "C"
	{
		__declspec(dllexport) DWORD NvOptimusEnablement = 1;
	}

#endif


using namespace C4;


namespace
{
	const float kFrustumEpsilon			= 2.0e-6F;
	const float kVelocityMultiplier		= 0.142857F;
	const float kMotionBlurBoxExpand	= 8.0F;


	#if !C4MOBILE

		enum
		{
			kInfiniteShadowMapSize		= 1024,
			kPointShadowMapSize			= 1024,
			kSpotShadowMapSize			= 2048
		};

	#else //[ MOBILE

		// -- Mobile code hidden --

	#endif //]


	enum
	{
		kGraphicsDepthTestLess			= 1 << 0,
		kGraphicsAmbientLessEqual		= 1 << 1,
		kGraphicsCullFaceBack			= 1 << 2,
		kGraphicsFrontFaceCCW			= 1 << 3,
		kGraphicsClipEnabled			= 1 << 4,
		kGraphicsLightScissor			= 1 << 5,
		kGraphicsObliqueFrustum			= 1 << 6,
		kGraphicsLightDepthBounds		= 1 << 7,
		kGraphicsRenderShadow			= 1 << 11,
		kGraphicsReactivateTextures		= 1 << 12,
		kGraphicsMotionBlurAvail		= 1 << 13,
		kGraphicsDistortionAvail		= 1 << 14,
		kGraphicsGlowBloomAvail			= 1 << 15
	};


	enum
	{
		kPostColorMatrix				= 1 << 0,
		kPostMotionBlur					= 1 << 1,
		kPostMotionBlurGradient			= 1 << 2,
		kPostDistortion					= 1 << 3,
		kPostGlowBloom					= 1 << 4,
		kPostShaderCount				= 1 << 5
	};


	enum
	{
		kLocalFragmentShaderCopyZero,
		kLocalFragmentShaderCopyConstant,
		kLocalFragmentShaderCount
	};


	enum
	{
		kLocalGeometryShaderExtrudeNormalLine,
		kLocalGeometryShaderCount
	};


	const TextureHeader nullTextureHeader =
	{
		kTexture2D,
		kTextureForceHighQuality,
		kTextureSemanticNone,
		kTextureSemanticNone,
		kTextureI8,
		4, 4, 1,
		{kTextureRepeat, kTextureRepeat, kTextureRepeat},
		3
	};

	alignas(32) const unsigned_int8 nullTextureImage[21] =
	{
		0xFF, 0x00, 0xFF, 0x00, 0x00, 0xFF, 0x00, 0xFF, 0xFF, 0x00, 0xFF, 0x00, 0x00, 0xFF, 0x00, 0xFF,
		0xFF, 0x00, 0xFF, 0x00, 
		0xFF
	};
} 

 
GraphicsMgr *C4::TheGraphicsMgr = nullptr;

 
namespace C4
{ 
	template <> GraphicsMgr Manager<GraphicsMgr>::managerObject(0); 
	template <> GraphicsMgr **Manager<GraphicsMgr>::managerPointer = &TheGraphicsMgr;

	template <> const char *const Manager<GraphicsMgr>::resultString[] =
	{ 
		nullptr,
		"Graphics context format failed",
		"Graphics context initialization failed",
		"Graphics hardware insufficient"
	};

	template <> const unsigned_int32 Manager<GraphicsMgr>::resultIdentifier[] =
	{
		0, 'FORM', 'CTXT', 'HARD'
	};

	template class Manager<GraphicsMgr>;
}


GraphicsExtensionData GraphicsMgr::extensionData[kGraphicsExtensionCount] =
{
	{"GL_ARB_conservative_depth",				nullptr,							0x0420,		false,	true},
	{"GL_ARB_debug_output",						nullptr,							0xFFFF,		false,	true},
	{"GL_ARB_get_program_binary",				nullptr,							0x0410,		false,	true},
	{"GL_ARB_invalidate_subdata",				nullptr,							0x0430,		false,	true},
	{"GL_ARB_multi_bind",						nullptr,							0x0440,		false,	true},
	{"GL_ARB_query_buffer_object",				nullptr,							0x0440,		false,	true},
	{"GL_ARB_sample_shading",					nullptr,							0x0400,		false,	true},
	{"GL_ARB_tessellation_shader",				nullptr,							0x0400,		false,	true},
	{"GL_ARB_texture_storage",					nullptr,							0x0420,		false,	true},
	{"GL_EXT_depth_bounds_test",				nullptr,							0xFFFF,		false,	true},
	{"GL_EXT_direct_state_access",				nullptr,							0xFFFF,		false,	true},
	{"GL_EXT_polygon_offset_clamp",				nullptr,							0xFFFF,		false,	true},
	{"GL_EXT_texture_compression_s3tc",			nullptr,							0xFFFF,		true,	true},
	{"GL_EXT_texture_filter_anisotropic",		nullptr,							0xFFFF,		true,	true},
	{"GL_EXT_texture_mirror_clamp",				"GL_ATI_texture_mirror_once",		0xFFFF,		false,	true},
	{"GL_NV_explicit_multisample",				nullptr,							0xFFFF,		false,	true},
	{"GL_NV_framebuffer_multisample_coverage",	nullptr,							0xFFFF,		false,	true},
	{"GL_NV_shader_buffer_load",				nullptr,							0xFFFF,		false,	true},
	{"GL_NV_vertex_buffer_unified_memory",		nullptr,							0xFFFF,		false,	true}
};


#if C4WINDOWS

	WindowSystemExtensionData GraphicsMgr::windowSystemExtensionData[kWindowSystemExtensionCount] =
	{
		{"WGL_ARB_create_context", true},
		{"WGL_ARB_pixel_format", true},
		{"WGL_EXT_swap_control", true},
		{"WGL_EXT_swap_control_tear", true}
	};

#elif C4LINUX

	WindowSystemExtensionData GraphicsMgr::windowSystemExtensionData[kWindowSystemExtensionCount] =
	{
		{"GLX_ARB_create_context_profile", true},
		{"GLX_EXT_swap_control", true},
		{"GLX_EXT_swap_control_tear", true}
	};

#endif


GraphicsCapabilities::GraphicsCapabilities()
{
	#if C4OPENGL

		programBinaryFormatArray = nullptr;

	#endif
}

GraphicsCapabilities::~GraphicsCapabilities()
{
	#if C4OPENGL

		delete[] programBinaryFormatArray;

	#endif
}


NormalFrameBuffer::NormalFrameBuffer(int32 width, int32 height, unsigned_int32 mask)
{
	static const unsigned_int32 targetFormat[kRenderTargetCount] =
	{
		Render::kTextureRenderBufferRGBA8,			// kRenderTargetDisplay
		Render::kTextureRenderBufferRGBA8,			// kRenderTargetPrimary
		Render::kTextureRenderBufferRGBA8,			// kRenderTargetReflection
		Render::kTextureRenderBufferRGBA8,			// kRenderTargetRefraction
		Render::kTextureRenderBufferRGBA16F,		// kRenderTargetStructure
		Render::kTextureRenderBufferRG16F,			// kRenderTargetVelocity
		Render::kTextureRenderBufferR8,				// kRenderTargetOcclusion1
		Render::kTextureRenderBufferR8				// kRenderTargetOcclusion2
	};

	Render::FrameBufferObject::Construct();

	renderTargetMask = mask;
	for (machine a = 0; a < kRenderTargetCount; a++)
	{
		if (mask & 1)
		{
			Render::TextureAllocationData	allocationData;

			Render::TextureObject *texture = &textureObject[a];
			texture->Construct(Render::kTextureTargetRectangle);

			allocationData.memorySize = nullptr;
			allocationData.format = targetFormat[a];
			allocationData.encoding = Render::kTextureEncodingLinear;
			allocationData.width = width;
			allocationData.height = height;
			allocationData.renderBuffer = true;

			texture->AllocateStorageRect(&allocationData);

			if (a == kRenderTargetDisplay)
			{
				texture->SetSWrapMode(Render::kWrapClampToBorder);
				texture->SetTWrapMode(Render::kWrapClampToBorder);
				texture->SetMaxAnisotropy(4.0F);
			}
			else if ((a == kRenderTargetStructure) || (a == kRenderTargetVelocity))
			{
				texture->SetFilterModes(Render::kFilterNearest, Render::kFilterNearest);
			}
		}

		mask >>= 1;
	}

	depthRenderBuffer.Construct();
	depthRenderBuffer.AllocateStorage(width, height, Render::kRenderBufferDepthStencil);

	SetDepthStencilRenderBuffer(&depthRenderBuffer);
	Render::ResetFrameBuffer();
}

NormalFrameBuffer::~NormalFrameBuffer()
{
	depthRenderBuffer.Destruct();

	unsigned_int32 mask = renderTargetMask;
	for (machine a = kRenderTargetCount - 1; a >= 0; a--)
	{
		if (mask & (1 << a))
		{
			textureObject[a].Destruct();
		}
	}

	Render::FrameBufferObject::Destruct();
}

void NormalFrameBuffer::Invalidate(void)
{
	unsigned_int32 mask = renderTargetMask;
	for (machine a = 0; a < kRenderTargetCount; a++)
	{
		if (mask & 1)
		{
			textureObject[a].InvalidateImage();
		}

		mask >>= 1;
	}
}


EffectFrameBuffer::EffectFrameBuffer(int32 width, int32 height)
{
	Render::TextureAllocationData	allocationData;

	Render::FrameBufferObject::Construct();
	textureObject.Construct(Render::kTextureTargetRectangle);

	allocationData.memorySize = nullptr;
	allocationData.format = Render::kTextureRenderBufferRGBA8;
	allocationData.encoding = Render::kTextureEncodingLinear;
	allocationData.width = width;
	allocationData.height = height;
	allocationData.renderBuffer = true;

	textureObject.AllocateStorageRect(&allocationData);

	SetColorRenderTexture(&textureObject);
	Render::ResetFrameBuffer();
}

EffectFrameBuffer::~EffectFrameBuffer()
{
	textureObject.Destruct();
	Render::FrameBufferObject::Destruct();
}


ShadowFrameBuffer::ShadowFrameBuffer(unsigned_int32 targetIndex)
{
	Render::FrameBufferObject::Construct();

	textureObject.Construct(targetIndex);
	textureObject.SetCompareFunc(Render::kShadowLessEqual);
	textureObject.SetBorderColor(Render::kTextureBorderWhite);
	SetTextureAnisotropy(TheGraphicsMgr->GetTextureFilterAnisotropy());
}

ShadowFrameBuffer::~ShadowFrameBuffer()
{
	textureObject.Destruct();
	Render::FrameBufferObject::Destruct();
}

void ShadowFrameBuffer::SetTextureAnisotropy(int32 anisotropy)
{
	#if C4GLCORE

		const GraphicsCapabilities *capabilities = TheGraphicsMgr->GetCapabilities();
		textureObject.SetMaxAnisotropy(Fmin((float) anisotropy, capabilities->maxTextureAnisotropy));

	#endif
}


InfiniteShadowFrameBuffer::InfiniteShadowFrameBuffer(int32 size) : ShadowFrameBuffer(Render::kTextureTargetArray2D)
{
	Render::TextureAllocationData	allocationData;

	allocationData.memorySize = nullptr;
	allocationData.format = Render::kTextureDepth;
	allocationData.encoding = Render::kTextureEncodingLinear;
	allocationData.width = size;
	allocationData.height = size;
	allocationData.depth = kMaxShadowCascadeCount;
	allocationData.renderBuffer = true;

	textureObject.AllocateStorageArray2D(&allocationData);
	textureObject.SetSWrapMode(Render::kWrapClampToBorder);
	textureObject.SetTWrapMode(Render::kWrapClampToBorder);
}

InfiniteShadowFrameBuffer::~InfiniteShadowFrameBuffer()
{
}


PointShadowFrameBuffer::PointShadowFrameBuffer(int32 size) : ShadowFrameBuffer(Render::kTextureTargetCube)
{
	Render::TextureAllocationData	allocationData;

	allocationData.memorySize = nullptr;
	allocationData.format = Render::kTextureDepth;
	allocationData.encoding = Render::kTextureEncodingLinear;
	allocationData.width = size;
	allocationData.renderBuffer = true;

	textureObject.AllocateStorageCube(&allocationData);
	textureObject.SetSWrapMode(Render::kWrapClampToEdge);
	textureObject.SetTWrapMode(Render::kWrapClampToEdge);
}

PointShadowFrameBuffer::~PointShadowFrameBuffer()
{
}


SpotShadowFrameBuffer::SpotShadowFrameBuffer(int32 size) : ShadowFrameBuffer(Render::kTextureTarget2D)
{
	Render::TextureAllocationData	allocationData;

	allocationData.memorySize = nullptr;
	allocationData.format = Render::kTextureDepth;
	allocationData.encoding = Render::kTextureEncodingLinear;
	allocationData.width = size;
	allocationData.height = size;
	allocationData.renderBuffer = true;

	textureObject.AllocateStorage2D(&allocationData);
	textureObject.SetSWrapMode(Render::kWrapClampToBorder);
	textureObject.SetTWrapMode(Render::kWrapClampToBorder);

	SetDepthRenderTexture(&textureObject);
	Render::ResetFrameBuffer();
}

SpotShadowFrameBuffer::~SpotShadowFrameBuffer()
{
}


MultisampleFrameBuffer::MultisampleFrameBuffer(int32 width, int32 height, int32 sampleCount)
{
	Render::FrameBufferObject::Construct();

	colorRenderBuffer.Construct();
	unsigned_int32 format = Render::kRenderBufferRGBA8;
	colorRenderBuffer.AllocateMultisampleStorage(width, height, sampleCount, format);

	depthRenderBuffer.Construct();
	depthRenderBuffer.AllocateMultisampleStorage(width, height, sampleCount, Render::kRenderBufferDepthStencil);

	SetColorRenderBuffer(&colorRenderBuffer);
	SetDepthStencilRenderBuffer(&depthRenderBuffer);

	#if C4OPENGL

		GLint	samples;

		glGetIntegerv(GL_SAMPLES, &samples);
		sampleDivider = 1.0F / (float) samples;

	#else

		sampleDivider = 1.0F / (float) sampleCount;

	#endif

	Render::ResetFrameBuffer();
}

MultisampleFrameBuffer::~MultisampleFrameBuffer()
{
	depthRenderBuffer.Destruct();
	colorRenderBuffer.Destruct();
	Render::FrameBufferObject::Destruct();
}


GraphicsMgr::GraphicsMgr(int) :
		fullscreenVertexBuffer(kVertexBufferAttribute | kVertexBufferStatic),
		processGridVertexBuffer(kVertexBufferAttribute | kVertexBufferStatic),
		processGridIndexBuffer(kVertexBufferIndex | kVertexBufferDynamic),
		textureDetailLevelObserver(this, &GraphicsMgr::HandleTextureDetailLevelEvent),
		paletteDetailLevelObserver(this, &GraphicsMgr::HandlePaletteDetailLevelEvent),
		textureAnisotropyObserver(this, &GraphicsMgr::HandleTextureAnisotropyEvent),
		renderParallaxMappingObserver(this, &GraphicsMgr::HandleRenderParallaxMappingEvent),
		renderHorizonMappingObserver(this, &GraphicsMgr::HandleRenderHorizonMappingEvent),
		renderTerrainBumpsObserver(this, &GraphicsMgr::HandleRenderTerrainBumpsEvent),
		renderStructureEffectsObserver(this, &GraphicsMgr::HandleRenderStructureEffectsEvent),
		renderAmbientOcclusionObserver(this, &GraphicsMgr::HandleRenderAmbientOcclusionEvent),
		renderAmbientBumpsObserver(this, &GraphicsMgr::HandleRenderAmbientBumpsEvent),
		postBrightnessObserver(this, &GraphicsMgr::HandlePostBrightnessEvent),
		postMotionBlurObserver(this, &GraphicsMgr::HandlePostMotionBlurEvent),
		postDistortionObserver(this, &GraphicsMgr::HandlePostDistortionEvent),
		postGlowBloomObserver(this, &GraphicsMgr::HandlePostGlowBloomEvent)
{
	cameraObject = nullptr;
	cameraTransformable = nullptr;

	cameraSpaceTransform(3,0) = cameraSpaceTransform(3,1) = cameraSpaceTransform(3,2) = 0.0F;
	cameraSpaceTransform(3,3) = 1.0F;
}

GraphicsMgr::~GraphicsMgr()
{
}

EngineResult GraphicsMgr::Construct(void)
{
	static const ConstPoint2D fullscreenVertex[3] = {{-1.0F, -3.0F}, {3.0F, 1.0F}, {-1.0F, 1.0F}};

	new(capabilities) GraphicsCapabilities;
	new(syncRenderSignal) Signal;

	int32 fullFrameWidth = TheDisplayMgr->GetFullFrameWidth();
	int32 fullFrameHeight = TheDisplayMgr->GetFullFrameHeight();
	int32 displayWidth = TheDisplayMgr->GetDisplayWidth();
	int32 displayHeight = TheDisplayMgr->GetDisplayHeight();
	int32 displaySamples = TheDisplayMgr->GetDisplaySamples();
	unsigned_int32 displayFlags = TheDisplayMgr->GetDisplayFlags();

	GraphicsResult result = InitializeGraphicsContext(fullFrameWidth, fullFrameHeight, displayWidth, displayHeight, displayFlags);
	if (result != kGraphicsOkay)
	{
		return (result);
	}

	bool extensionsAvailable = InitializeOpenglExtensions();

	#if C4LOG_FILE

		UpdateLog();

	#endif

	if (!extensionsAvailable)
	{
		TerminateGraphicsContext();
		return (kGraphicsNoHardware);
	}

	#if C4OPENGL

		Render::InitializeCoreOpenGL();

	#endif

	ShaderProgram::Initialize();
	MicrofacetAttribute::Initialize();

	targetDisableMask = 0;

	syncRenderObject = nullptr;
	syncLoadFlag = false;

	fogSpaceObject = nullptr;
	fogSpaceTransformable = nullptr;
	lightObject = nullptr;
	lightTransformable = nullptr;
	currentShadowFlag = false;
	diagnosticFlags = 0;

	currentBlendState = kBlendReplace;
	currentGraphicsState = kGraphicsDepthTestLess | kGraphicsCullFaceBack | kGraphicsFrontFaceCCW | kGraphicsReactivateTextures;
	currentRenderState = 0;
	currentMaterialState = 0;
	currentShaderType = kShaderAmbient;
	currentShaderVariant = kShaderVariantNormal;
	currentAmbientMode = kAmbientNormal;
	currentOcclusionQuery = nullptr;

	colorTransformFlags = 0;
	finalColorScale[0].Set(1.0F, 1.0F, 1.0F, 1.0F);
	finalColorBias.Set(0.0F, 0.0F, 0.0F, 0.0F);

	#if C4OPENGL

		glEnable(GL_SCISSOR_TEST);
		glActiveTexture(GL_TEXTURE0);
		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

		#if C4GLCORE

			glPointParameteri(GL_POINT_SPRITE_COORD_ORIGIN, GL_LOWER_LEFT);

			if (capabilities->extensionFlag[kExtensionSampleShading])
			{
				glMinSampleShading(1.0F);
			}

		#endif

	#endif

	Render::EnableBlend();
	Render::EnableCullFace();
	Render::SetBlendFunc(Render::kBlendOne, Render::kBlendZero, Render::kBlendZero, Render::kBlendZero);
	Render::SetDepthFunc(Render::kDepthLess);
	Render::SetStencilFunc(Render::kStencilAlways, 0, ~0);
	Render::SetCullFace(Render::kCullBack);

	#if C4OPENGL && C4DEBUG && C4CREATE_DEBUG_CONTEXT

		if (capabilities->extensionFlag[kExtensionDebugOutput])
		{
			glDebugMessageCallbackARB(&DebugCallback, this);
		}

	#endif

	for (machine a = 0; a < 4; a++)
	{
		timestampCount[a] = 0;

		for (machine b = 0; b < kMaxTimestampCount; b++)
		{
			timestampQuery[a][b].Construct();
		}
	}

	InitializeVariables();

	infiniteShadowMapSize = Min(kInfiniteShadowMapSize, capabilities->maxTextureSize);
	pointShadowMapSize = Min(kPointShadowMapSize, capabilities->maxCubeTextureSize);
	spotShadowMapSize = Min(kSpotShadowMapSize, capabilities->maxTextureSize);

	currentRenderTargetType = kRenderTargetNone;
	renderTargetHeight = displayHeight;

	genericFrameBuffer.Construct();

	unsigned_int32 mask = (1 << kRenderTargetPrimary) | (1 << kRenderTargetReflection) | (1 << kRenderTargetRefraction);

	#if C4OCULUS

		if (displayFlags & kDisplayOculus)
		{
			mask |= 1 << kRenderTargetDisplay;
		}

	#endif

	if (renderOptionFlags & kRenderOptionStructureEffects)
	{
		mask |= 1 << kRenderTargetStructure;
	}

	if (renderOptionFlags & kRenderOptionAmbientOcclusion)
	{
		mask |= (1 << kRenderTargetStructure) | (1 << kRenderTargetOcclusion1) | (1 << kRenderTargetOcclusion2);
	}

	if (renderOptionFlags & kRenderOptionMotionBlur)
	{
		mask |= (1 << kRenderTargetStructure) | (1 << kRenderTargetVelocity);
	}

	normalFrameBuffer = new NormalFrameBuffer(displayWidth, displayHeight, mask);

	if (displaySamples > 1)
	{
		int32 samples = Min(displaySamples, capabilities->maxMultisampleSamples);
		multisampleFrameBuffer = new MultisampleFrameBuffer(displayWidth, displayHeight, samples);
	}
	else
	{
		multisampleFrameBuffer = nullptr;
	}

	if (renderOptionFlags & kRenderOptionGlowBloom)
	{
		glowBloomFrameBuffer = new EffectFrameBuffer(displayWidth / 2, displayHeight / 2);
	}
	else
	{
		glowBloomFrameBuffer = nullptr;
	}

	infiniteShadowFrameBuffer = new InfiniteShadowFrameBuffer(infiniteShadowMapSize);
	pointShadowFrameBuffer = new PointShadowFrameBuffer(pointShadowMapSize);
	spotShadowFrameBuffer = new SpotShadowFrameBuffer(spotShadowMapSize);

	float inverseWidth = 1.0F / (float) displayWidth;
	float f = kMotionBlurBoxExpand * inverseWidth;
	motionBlurBoxLeftOffset = 0.5F - f;
	motionBlurBoxRightOffset = 0.5F + f;

	float inverseHeight = 1.0F / (float) displayHeight;
	f = kMotionBlurBoxExpand * inverseHeight;
	motionBlurBoxBottomOffset = 0.5F - f;
	motionBlurBoxTopOffset = 0.5F + f;

	float distortionScale = (float) displayWidth * 0.03125F;

	#if C4OPENGL

		Render::SetUniversalShaderParameter(kUniversalParamFrameBufferScale, inverseWidth, inverseHeight, distortionScale, distortionScale);

	#elif C4CONSOLE //[ CONSOLE

		// -- Console code hidden --

	#endif //]

	nullTexture = Texture::Get(&nullTextureHeader, nullTextureImage);

	AmbientOutputProcess::Initialize();
	OcclusionPostProcess::Initialize();
	HorizonProcess::Initialize();

	fullscreenVertexBuffer.Establish(sizeof(Point2D) * 3, fullscreenVertex);
	processGridVertexBuffer.Establish(sizeof(Point2D) * (kProcessGridWidth + 1) * (kProcessGridHeight + 1));
	processGridIndexBuffer.Establish(sizeof(Triangle) * kProcessGridWidth * kProcessGridHeight * 2);

	new(fullscreenVertexData) Render::VertexDataObject;
	new(processGridVertexData) Render::VertexDataObject;
	new(vectorVertexData) Render::VertexDataObject;

	fullscreenVertexData->SetVertexAttribArray(kVertexAttribPosition0, &fullscreenVertexBuffer, 0, 2, sizeof(Point2D));
	processGridVertexData->SetVertexAttribArray(kVertexAttribPosition0, &processGridVertexBuffer, 0, 2, sizeof(Point2D));
	processGridVertexData->SetVertexIndexBuffer(&processGridIndexBuffer);

	InitializeProcessGrid();
	InitializeActiveFlags();

	VertexBuffer::ReactivateAll();
	OcclusionQuery::ReactivateAll();

	if (cameraObject)
	{
		SetCamera(cameraObject, cameraTransformable);
	}

	return (kEngineOkay);
}

void GraphicsMgr::Destruct(void)
{
	if (TheMovieMgr)
	{
		TheMovieMgr->StopRecording();
	}

	if (TheWorldMgr)
	{
		VertexBuffer::SaveAll();
	}

	VertexBuffer::DeactivateAll();
	OcclusionQuery::DeactivateAll();
	Texture::DeactivateAll();
	ShaderData::Purge();

	vectorVertexData->~VertexDataObject();
	processGridVertexData->~VertexDataObject();
	fullscreenVertexData->~VertexDataObject();

	processGridIndexBuffer.Establish(0);
	processGridVertexBuffer.Establish(0);
	fullscreenVertexBuffer.Establish(0);

	HorizonProcess::Terminate();
	OcclusionPostProcess::Terminate();
	AmbientOutputProcess::Terminate();

	nullTexture->Release();

	delete spotShadowFrameBuffer;
	delete pointShadowFrameBuffer;
	delete infiniteShadowFrameBuffer;
	delete glowBloomFrameBuffer;
	delete multisampleFrameBuffer;
	delete normalFrameBuffer;
	genericFrameBuffer.Destruct();

	for (machine a = 3; a >= 0; a--)
	{
		for (machine b = kMaxTimestampCount - 1; b >= 0; b--)
		{
			timestampQuery[a][b].Destruct();
		}
	}

	MicrofacetAttribute::Terminate();
	ShaderProgram::Terminate();

	segmentArray[1].Purge();
	segmentArray[0].Purge();

	#if C4OPENGL

		extensionsString.Purge();
		Render::TerminateCoreOpenGL();

	#endif

	TerminateGraphicsContext();

	syncRenderSignal->~Signal();
	capabilities->~GraphicsCapabilities();
}

#if C4OPENGL && C4DEBUG && C4CREATE_DEBUG_CONTEXT

	void GraphicsMgr::DebugCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar *message, void *userParam)
	{
		Engine::Report(String<>(message) += "<br/>\r\n", kReportLog);
	}

#endif

#if C4LOG_FILE

	void GraphicsMgr::LogExtensions(const char *string)
	{
		if (string)
		{
			while (*string != 0)
			{
				String<63>	line;

				const char *s = string;
				if (*s < 33)
				{
					string++;
					continue;
				}

				while (*s > 32)
				{
					s++;
				}

				int32 len = Min((int32) (s - string), 63);

				for (machine b = 0; b < len; b++)
				{
					line[b] = string[b];
				}

				line[len] = 0;
				string = s;

				Engine::Report(line, kReportLog);
				Engine::Report("<br/>\r\n", kReportLog);
			}
		}
	}

	void GraphicsMgr::UpdateLog(void) const
	{
		static bool logUpdated = false;
		if (!logUpdated)
		{
			logUpdated = true;

			Engine::Report("Graphics Manager", kReportLog | kReportHeading);

			#if C4OPENGL

				Engine::Report("<table class=\"data\" cellspacing=\"0\" cellpadding=\"0\">\r\n", kReportLog);

				Engine::Report("<tr><th>GL_VENDOR</th><td>", kReportLog);
				Engine::Report(GetOpenGLVendor(), kReportLog);
				Engine::Report("</td></tr>\r\n", kReportLog);

				Engine::Report("<tr><th>GL_RENDERER</th><td>", kReportLog);
				Engine::Report(GetOpenGLRenderer(), kReportLog);
				Engine::Report("</td></tr>\r\n", kReportLog);

				Engine::Report("<tr><th>GL_VERSION</th><td>", kReportLog);
				Engine::Report(GetOpenGLVersion(), kReportLog);
				Engine::Report("</td></tr>\r\n", kReportLog);

				Engine::Report("<tr><th>GLSL Version</th><td>", kReportLog);
				Engine::Report(GetGLSLVersion(), kReportLog);
				Engine::Report("</td></tr>\r\n", kReportLog);

				Engine::Report("<tr><th>GL_EXTENSIONS</th><td><div style=\"height: 128px; overflow: auto;\">\r\n", kReportLog);
				LogExtensions(extensionsString);

				#if C4WINDOWS

					Engine::Report("</div></td></tr>\r\n<tr><th>WGL_EXTENSIONS</th><td><div style=\"height: 128px; overflow: auto;\">\r\n", kReportLog);
					LogExtensions(windowSystemExtensionsString);

				#elif C4LINUX

					Engine::Report("</div></td></tr>\r\n<tr><th>GLX_EXTENSIONS</th><td><div style=\"height: 128px; overflow: auto;\">\r\n", kReportLog);
					LogExtensions(windowSystemExtensionsString);

				#endif

				Engine::Report("</div></td></tr>\r\n", kReportLog);

				Engine::Report("<tr><th>Texture limits</th><td>", kReportLog);
				Engine::Report("Max texture 2D size: ", kReportLog);
				Engine::Report(Text::IntegerToString(capabilities->maxTextureSize), kReportLog);
				Engine::Report("<br/>Max texture 3D size: ", kReportLog);
				Engine::Report(Text::IntegerToString(capabilities->max3DTextureSize), kReportLog);
				Engine::Report("<br/>Max texture cube size: ", kReportLog);
				Engine::Report(Text::IntegerToString(capabilities->maxCubeTextureSize), kReportLog);
				Engine::Report("<br/>Max array texture layers: ", kReportLog);
				Engine::Report(Text::IntegerToString(capabilities->maxArrayTextureLayers), kReportLog);
				Engine::Report("<br/>Max texture lod bias: ", kReportLog);
				Engine::Report(Text::FloatToString(capabilities->maxTextureLodBias), kReportLog);
				Engine::Report("<br/>Max texture anisotropy: ", kReportLog);
				Engine::Report(Text::FloatToString(capabilities->maxTextureAnisotropy), kReportLog);
				Engine::Report("<br/>Max combined texture image units: ", kReportLog);
				Engine::Report(Text::IntegerToString(capabilities->maxCombinedTextureImageUnits), kReportLog);
				Engine::Report("</td></tr>\r\n", kReportLog);

				#if C4RENDER_OCCLUSION_QUERY

					Engine::Report("<tr><th>Query object limits</th><td>", kReportLog);
					Engine::Report("Query counter bits: ", kReportLog);
					Engine::Report(Text::IntegerToString(capabilities->queryCounterBits), kReportLog);
					Engine::Report("</td></tr>\r\n", kReportLog);

				#endif

				Engine::Report("<tr><th>Frame buffer object limits</th><td>", kReportLog);
				Engine::Report("Max frame buffer color attachments: ", kReportLog);
				Engine::Report(Text::IntegerToString(capabilities->maxColorAttachments), kReportLog);
				Engine::Report("<br/>Max frame buffer render buffer size: ", kReportLog);
				Engine::Report(Text::IntegerToString(capabilities->maxRenderBufferSize), kReportLog);
				Engine::Report("<br/>Max frame buffer samples: ", kReportLog);
				Engine::Report(Text::IntegerToString(capabilities->maxMultisampleSamples), kReportLog);
				Engine::Report("</td></tr>\r\n", kReportLog);

				Engine::Report("<tr><th>Vertex shader limits</th><td>", kReportLog);
				Engine::Report("Max vertex attribs: ", kReportLog);
				Engine::Report(Text::IntegerToString(capabilities->maxVertexAttribs), kReportLog);
				Engine::Report("<br/>Max varying components: ", kReportLog);
				Engine::Report(Text::IntegerToString(capabilities->maxVaryingComponents), kReportLog);
				Engine::Report("<br/>Max vertex uniform components: ", kReportLog);
				Engine::Report(Text::IntegerToString(capabilities->maxVertexUniformComponents), kReportLog);
				Engine::Report("<br/>Max vertex texture image units: ", kReportLog);
				Engine::Report(Text::IntegerToString(capabilities->maxVertexTextureImageUnits), kReportLog);
				Engine::Report("</td></tr>\r\n", kReportLog);

				Engine::Report("<tr><th>Fragment shader limits</th><td>", kReportLog);
				Engine::Report("Max fragment uniform components: ", kReportLog);
				Engine::Report(Text::IntegerToString(capabilities->maxFragmentUniformComponents), kReportLog);
				Engine::Report("<br/>Max fragment texture image units: ", kReportLog);
				Engine::Report(Text::IntegerToString(capabilities->maxFragmentTextureImageUnits), kReportLog);
				Engine::Report("</td></tr>\r\n", kReportLog);

				#if C4GLCORE

					Engine::Report("<tr><th>Geometry shader limits</th><td>", kReportLog);
					Engine::Report("Max vertex varying components: ", kReportLog);
					Engine::Report(Text::IntegerToString(capabilities->maxVertexVaryingComponents), kReportLog);
					Engine::Report("<br/>Max geometry varying components: ", kReportLog);
					Engine::Report(Text::IntegerToString(capabilities->maxGeometryVaryingComponents), kReportLog);
					Engine::Report("<br/>Max geometry uniform components: ", kReportLog);
					Engine::Report(Text::IntegerToString(capabilities->maxGeometryUniformComponents), kReportLog);
					Engine::Report("<br/>Max geometry output vertices: ", kReportLog);
					Engine::Report(Text::IntegerToString(capabilities->maxGeometryOutputVertices), kReportLog);
					Engine::Report("<br/>Max geometry total output components: ", kReportLog);
					Engine::Report(Text::IntegerToString(capabilities->maxGeometryTotalOutputComponents), kReportLog);
					Engine::Report("<br/>Max geometry texture image units: ", kReportLog);
					Engine::Report(Text::IntegerToString(capabilities->maxGeometryTextureImageUnits), kReportLog);
					Engine::Report("</td></tr>\r\n", kReportLog);

				#endif

				if (capabilities->extensionFlag[kExtensionGetProgramBinary])
				{
					Engine::Report("<tr><th>Program binary formats</th><td>", kReportLog);

					int32 count = capabilities->programBinaryFormats;
					for (machine a = 0; a < count; a++)
					{
						Engine::Report((String<31>("0x") += Text::IntegerToHexString8(capabilities->programBinaryFormatArray[a])) += "<br/>", kReportLog);
					}

					Engine::Report("</td></tr>\r\n", kReportLog);
				}

				Engine::Report("</table>\r\n", kReportLog);

			#endif
		}
	}

#endif

GraphicsResult GraphicsMgr::InitializeGraphicsContext(int32 frameWidth, int32 frameHeight, int32 displayWidth, int32 displayHeight, unsigned_int32 displayFlags)
{
	#if !C4PS3

		Render::Initialize();

	#endif

	#if C4WINDOWS

		static const int formatAttributes[] =
		{
			WGL_SUPPORT_OPENGL_ARB, true,
			WGL_DRAW_TO_WINDOW_ARB, true,
			WGL_DOUBLE_BUFFER_ARB, true,
			WGL_ACCELERATION_ARB, WGL_FULL_ACCELERATION_ARB,
			WGL_PIXEL_TYPE_ARB, WGL_TYPE_RGBA_ARB,
			WGL_FRAMEBUFFER_SRGB_CAPABLE_ARB, true,
			WGL_COLOR_BITS_ARB, 24,
			WGL_ALPHA_BITS_ARB, 8,
			WGL_DEPTH_BITS_ARB, 0,
			WGL_STENCIL_BITS_ARB, 0,
			0, 0
		};

		static const int contextAttributes[] =
		{
			WGL_CONTEXT_MAJOR_VERSION_ARB, 3,
			WGL_CONTEXT_MINOR_VERSION_ARB, 3,
			WGL_CONTEXT_PROFILE_MASK_ARB, WGL_CONTEXT_CORE_PROFILE_BIT_ARB,

			#if C4DEBUG && C4CREATE_DEBUG_CONTEXT

				WGL_CONTEXT_FLAGS_ARB, WGL_CONTEXT_DEBUG_BIT_ARB,

			#endif

			0, 0
		};

		PIXELFORMATDESCRIPTOR	formatDescriptor;
		UINT					formatCount;
		int						pixelFormat;

		MemoryMgr::ClearMemory(&formatDescriptor, sizeof(PIXELFORMATDESCRIPTOR));
		formatDescriptor.nSize = sizeof(PIXELFORMATDESCRIPTOR);
		formatDescriptor.nVersion = 1;

		formatDescriptor.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_GENERIC_ACCELERATED | PFD_DOUBLEBUFFER | PFD_SWAP_EXCHANGE;
		if (!(displayFlags & kDisplayFullscreen))
		{
			formatDescriptor.dwFlags |= PFD_SUPPORT_COMPOSITION;
		}

		formatDescriptor.iPixelType = PFD_TYPE_RGBA;
		formatDescriptor.cColorBits = 24;
		formatDescriptor.cAlphaBits = 8;
		formatDescriptor.cDepthBits = 0;
		formatDescriptor.cStencilBits = 0;
		formatDescriptor.iLayerType = PFD_MAIN_PLANE;

		InitializeWglExtensions(&formatDescriptor);

		if ((!capabilities->windowSystemExtensionFlag[kWindowSystemExtensionCreateContext]) || (!capabilities->windowSystemExtensionFlag[kWindowSystemExtensionPixelFormat]))
		{
			return (kGraphicsContextFailed);
		}

		deviceContext = GetDC(TheDisplayMgr->GetDisplayWindow());

		if ((!wglChoosePixelFormatARB(deviceContext, formatAttributes, nullptr, 1, &pixelFormat, &formatCount)) || (formatCount == 0))
		{
			ReleaseDC(TheDisplayMgr->GetDisplayWindow(), deviceContext);
			return (kGraphicsNoHardware);
		}

		if (!SetPixelFormat(deviceContext, pixelFormat, &formatDescriptor))
		{
			ReleaseDC(TheDisplayMgr->GetDisplayWindow(), deviceContext);
			return (kGraphicsFormatFailed);
		}

		openglContext = wglCreateContextAttribsARB(deviceContext, nullptr, contextAttributes);
		if (!openglContext)
		{
			ReleaseDC(TheDisplayMgr->GetDisplayWindow(), deviceContext);
			return (kGraphicsContextFailed);
		}

		if (!wglMakeCurrent(deviceContext, openglContext))
		{
			wglDeleteContext(openglContext);
			ReleaseDC(TheDisplayMgr->GetDisplayWindow(), deviceContext);
			return (kGraphicsContextFailed);
		}

		if (wglGetExtensionsStringARB)
		{
			windowSystemExtensionsString = wglGetExtensionsStringARB(deviceContext);
		}

	#elif C4MACOS

		static NSOpenGLPixelFormatAttribute formatAttributes[] =
		{
			NSOpenGLPFAOpenGLProfile, NSOpenGLProfileVersion3_2Core,
			NSOpenGLPFAMinimumPolicy,
			NSOpenGLPFAAccelerated,
			NSOpenGLPFADoubleBuffer,
			NSOpenGLPFAColorSize, 24,
			NSOpenGLPFAAlphaSize, 8,
			NSOpenGLPFADepthSize, 0,
			NSOpenGLPFAStencilSize, 0,
			0
		};

		NSOpenGLPixelFormat *pixelFormat = [[NSOpenGLPixelFormat alloc] initWithAttributes: formatAttributes];
		if (!pixelFormat)
		{
			return (kGraphicsNoHardware);
		}

		openglContext = [[NSOpenGLContext alloc] initWithFormat: pixelFormat shareContext: nil];
		[pixelFormat release];

		if (!openglContext)
		{
			return (kGraphicsContextFailed);
		}

		if (displayFlags & kDisplayFullscreen)
		{
			CGLContextObj contextObject = (CGLContextObj) [openglContext CGLContextObj];
			GLint size[2] = {frameWidth, frameHeight};
			CGLSetParameter(contextObject, kCGLCPSurfaceBackingSize, size);
			CGLEnable(contextObject, kCGLCESurfaceBackingSize);
		}

		[openglContext makeCurrentContext];

		NSOpenGLView *displayView = TheDisplayMgr->GetDisplayView();
		[displayView setOpenGLContext: openglContext];
		[openglContext setView: displayView];

		openglBundle = TheEngine->GetOpenGLBundle();

	#elif C4LINUX

		static const int formatAttributes[] =
		{
			GLX_DOUBLEBUFFER, True,
			GLX_RED_SIZE, 8,
			GLX_GREEN_SIZE, 8,
			GLX_BLUE_SIZE, 8,
			GLX_ALPHA_SIZE, 8,
			GLX_DEPTH_SIZE, 0,
			GLX_STENCIL_SIZE, 0,
			GLX_RENDER_TYPE, GLX_RGBA_BIT,
			GLX_DRAWABLE_TYPE, GLX_WINDOW_BIT,
			GLX_X_VISUAL_TYPE, GLX_TRUE_COLOR,
			None
		};

		static const int contextAttributes[] =
		{
			GLX_CONTEXT_MAJOR_VERSION_ARB, 3,
			GLX_CONTEXT_MINOR_VERSION_ARB, 3,
			GLX_CONTEXT_PROFILE_MASK_ARB, GLX_CONTEXT_CORE_PROFILE_BIT_ARB,

			#if C4DEBUG && C4CREATE_DEBUG_CONTEXT

				GLX_CONTEXT_FLAGS_ARB, GLX_CONTEXT_DEBUG_BIT_ARB,

			#endif

			0, 0
		};

		int						configCount;
		XSetWindowAttributes	windowAttributes;

		openglDisplay = TheEngine->GetEngineDisplay();

		InitializeGlxExtensions();
		if (!capabilities->windowSystemExtensionFlag[kWindowSystemExtensionCreateContextProfile])
		{
			return (kGraphicsContextFailed);
		}

		GLXFBConfig *configList = glXChooseFBConfig(openglDisplay, DefaultScreen(openglDisplay), formatAttributes, &configCount);
		if ((!configList) || (configCount < 1))
		{
			return (kGraphicsFormatFailed);
		}

		XVisualInfo *visualInfo = glXGetVisualFromFBConfig(openglDisplay, configList[0]);
		if (!visualInfo)
		{
			return (kGraphicsFormatFailed);
		}

		::Window engineWindow = TheEngine->GetEngineWindow();
		openglColormap = XCreateColormap(openglDisplay, engineWindow, visualInfo->visual, AllocNone);

		windowAttributes.override_redirect = true;
		windowAttributes.colormap = openglColormap;
		openglWindow = XCreateWindow(openglDisplay, engineWindow, 0, 0, frameWidth, frameHeight, 0, visualInfo->depth, InputOutput, visualInfo->visual, CWOverrideRedirect | CWColormap, &windowAttributes);

		openglContext = glXCreateContextAttribsARB(openglDisplay, configList[0], nullptr, true, contextAttributes);

		XFree(visualInfo);
		XFree(configList);

		if (!openglContext)
		{
			XDestroyWindow(openglDisplay, openglWindow);
			XFreeColormap(openglDisplay, openglColormap);
			return (kGraphicsContextFailed);
		}

		if (!glXMakeCurrent(openglDisplay, openglWindow, openglContext))
		{
			XDestroyWindow(openglDisplay, openglWindow);
			XFreeColormap(openglDisplay, openglColormap);
			return (kGraphicsContextFailed);
		}

		XMapWindow(openglDisplay, openglWindow);

	#elif C4IOS //[ MOBILE

		// -- Mobile code hidden --

	#elif C4PS4 //[ PS4

		// -- PS4 code hidden --

	#endif //]

	SetDisplaySyncMode(&displayFlags);
	DetermineHardwareRank();

	return (kEngineOkay);
}

void GraphicsMgr::TerminateGraphicsContext(void)
{
	#if C4WINDOWS

		wglMakeCurrent(nullptr, nullptr);
		wglDeleteContext(openglContext);
		ReleaseDC(TheDisplayMgr->GetDisplayWindow(), deviceContext);

	#elif C4MACOS

		[NSOpenGLContext clearCurrentContext];
		[openglContext release];

	#elif C4LINUX

		glXMakeCurrent(openglDisplay, None, nullptr);
		glXDestroyContext(openglDisplay, openglContext);

		XDestroyWindow(openglDisplay, openglWindow);
		XFreeColormap(openglDisplay, openglColormap);

	#elif C4IOS //[ MOBILE

		// -- Mobile code hidden --

	#elif C4PS4 //[ PS4

		// -- PS4 code hidden --

	#elif C4PS3 //[ PS3

		// -- PS3 code hidden --

	#endif //]

	Render::Terminate();
}

void GraphicsMgr::SetDisplaySyncMode(const unsigned_int32 *displayFlags)
{
	#if C4WINDOWS

		if (capabilities->windowSystemExtensionFlag[kWindowSystemExtensionSwapControl])
		{
			unsigned_int32 flags = *displayFlags;

			int32 swapInterval = ((flags & kDisplayRefreshSync) != 0);
			if ((capabilities->windowSystemExtensionFlag[kWindowSystemExtensionSwapControlTear]) && (flags & kDisplaySyncTear))
			{
				swapInterval = -swapInterval;
			}

			wglSwapIntervalEXT(swapInterval);
		}

	#elif C4MACOS

		GLint swapInterval = ((*displayFlags & kDisplayRefreshSync) != 0);
		[openglContext setValues: &swapInterval forParameter: NSOpenGLCPSwapInterval];

	#elif C4LINUX

		if (capabilities->windowSystemExtensionFlag[kWindowSystemExtensionSwapControl])
		{
			unsigned_int32 flags = *displayFlags;

			int32 swapInterval = ((flags & kDisplayRefreshSync) != 0);
			if ((capabilities->windowSystemExtensionFlag[kWindowSystemExtensionSwapControlTear]) && (flags & kDisplaySyncTear))
			{
				swapInterval = -swapInterval;
			}

			glXSwapIntervalEXT(openglDisplay, openglWindow, swapInterval);
		}

	#elif C4PS4 //[ PS4

		// -- PS4 code hidden --

	#elif C4PS3 //[ PS3

		// -- PS3 code hidden --

	#endif //]
}

void GraphicsMgr::DetermineHardwareRank(void)
{
	#if C4DESKTOP

		capabilities->hardwareSpeedRank = 1;
		capabilities->hardwareMemoryRank = 2;

		const char *renderer = GetOpenGLRenderer();
		for (machine a = 0;; a++)
		{
			unsigned_int32 c = renderer[a];
			if (c == 0)
			{
				break;
			}

			if (c - '0' < 10U)
			{
				c = renderer[a + 1];
				if ((c - '0' < 10U) && ((unsigned_int32) renderer[a + 2] - '0' < 10U))
				{
					if (c >= '7')
					{
						capabilities->hardwareSpeedRank = 2;
					}
					else if (c <= '4')
					{
						capabilities->hardwareSpeedRank = 0;
					}

					break;
				}
			}
		}

	#elif C4MOBILE //[ MOBILE

		// -- Mobile code hidden --

	#elif C4PS4 //[ PS4

		// -- PS4 code hidden --

	#elif C4PS3 //[ PS3

		// -- PS3 code hidden --

	#endif //]
}

#if C4WINDOWS

	void GraphicsMgr::InitializeWglExtensions(PIXELFORMATDESCRIPTOR *formatDescriptor)
	{
		static const wchar_t name[] = L"wgl";

		WNDCLASSEXW		windowClass;

		HINSTANCE instance = TheEngine->GetEngineInstance();

		windowClass.cbSize = sizeof(WNDCLASSEXW);
		windowClass.style = CS_NOCLOSE | CS_OWNDC;
		windowClass.lpfnWndProc = &WglWindowProc;
		windowClass.cbClsExtra = 0;
		windowClass.cbWndExtra = 0;
		windowClass.hInstance = instance;
		windowClass.hIcon = nullptr;
		windowClass.hCursor = nullptr;
		windowClass.hbrBackground = nullptr;
		windowClass.lpszMenuName = nullptr;
		windowClass.lpszClassName = name;
		windowClass.hIconSm = nullptr;

		RegisterClassExW(&windowClass);
		HWND window = CreateWindowExW(0, name, name, WS_POPUP, 0, 0, 32, 32, nullptr, nullptr, instance, formatDescriptor);
		DestroyWindow(window);
		UnregisterClassW(name, instance);
	}

	LRESULT CALLBACK GraphicsMgr::WglWindowProc(HWND window, UINT message, WPARAM wparam, LPARAM lparam)
	{
		if (message == WM_CREATE)
		{
			TheGraphicsMgr->windowSystemExtensionsString = nullptr;
			bool *windowSystemExtensionFlag = TheGraphicsMgr->capabilities->windowSystemExtensionFlag;
			for (machine a = 0; a < kWindowSystemExtensionCount; a++)
			{
				windowSystemExtensionFlag[a] = false;
			}

			const CREATESTRUCTW *createStruct = (CREATESTRUCTW *) lparam;
			const PIXELFORMATDESCRIPTOR *formatDescriptor = (PIXELFORMATDESCRIPTOR *) createStruct->lpCreateParams;

			HDC deviceContext = GetDC(window);
			int pixelFormat = ChoosePixelFormat(deviceContext, formatDescriptor);
			if (pixelFormat != 0)
			{
				if (SetPixelFormat(deviceContext, pixelFormat, formatDescriptor))
				{
					HGLRC openglContext = wglCreateContext(deviceContext);
					if (wglMakeCurrent(deviceContext, openglContext))
					{
						GLGETEXTFUNC(wglGetExtensionsStringARB);
						if (wglGetExtensionsStringARB)
						{
							const char *string = wglGetExtensionsStringARB(deviceContext);

							const WindowSystemExtensionData *data = windowSystemExtensionData;
							for (machine a = 0; a < kWindowSystemExtensionCount; a++)
							{
								windowSystemExtensionFlag[a] = ((data->enabled) && (Text::FindText(string, data->name) >= 0));
								data++;
							}

							if (windowSystemExtensionFlag[kWindowSystemExtensionCreateContext])
							{
								GLGETEXTFUNC(wglCreateContextAttribsARB);
							}

							if (windowSystemExtensionFlag[kWindowSystemExtensionPixelFormat])
							{
								GLGETEXTFUNC(wglChoosePixelFormatARB);
							}

							if (windowSystemExtensionFlag[kWindowSystemExtensionSwapControl])
							{
								GLGETEXTFUNC(wglSwapIntervalEXT);
							}
						}

						wglMakeCurrent(nullptr, nullptr);
					}

					wglDeleteContext(openglContext);
				}
			}

			ReleaseDC(window, deviceContext);
			return (0);
		}

		return (DefWindowProcW(window, message, wparam, lparam));
	}

#elif C4LINUX

	void GraphicsMgr::InitializeGlxExtensions(void)
	{
		const char *string = glXQueryExtensionsString(openglDisplay, DefaultScreen(openglDisplay));
		windowSystemExtensionsString = string;

		bool *windowSystemExtensionFlag = capabilities->windowSystemExtensionFlag;
		const WindowSystemExtensionData *data = windowSystemExtensionData;
		for (machine a = 0; a < kWindowSystemExtensionCount; a++)
		{
			windowSystemExtensionFlag[a] = ((data->enabled) && (Text::FindText(string, data->name) >= 0));
			data++;
		}

		if (windowSystemExtensionFlag[kWindowSystemExtensionCreateContextProfile])
		{
			GLGETEXTFUNC(glXCreateContextAttribsARB);
		}

		if (windowSystemExtensionFlag[kWindowSystemExtensionSwapControl])
		{
			GLGETEXTFUNC(glXSwapIntervalEXT);
		}
	}

#endif

bool GraphicsMgr::InitializeOpenglExtensions(void)
{
	#if C4OPENGL

		GLint	majorVersion;
		GLint	minorVersion;
		GLint	extensionCount;

		glGetIntegerv(GL_MAJOR_VERSION, &majorVersion);
		glGetIntegerv(GL_MINOR_VERSION, &minorVersion);

		unsigned_int32 version = (majorVersion << 8) | (minorVersion << 4);
		capabilities->openglVersion = version;

		#if C4GLCORE

			if (version < 0x0330)
			{
				return (false);
			}

		#else //[ MOBILE

			// -- Mobile code hidden --

		#endif //]

		bool *extensionFlag = capabilities->extensionFlag;
		for (machine a = 0; a < kGraphicsExtensionCount; a++)
		{
			extensionFlag[a] = false;
		}

		const GraphicsExtensionData *data = extensionData;
		for (machine a = 0; a < kGraphicsExtensionCount; a++)
		{
			if ((version >= data->version) && (data->enabled))
			{
				extensionFlag[a] = true;
			}

			data++;
		}

		#if C4GLCORE

			GLGETCOREFUNC(glGetStringi);

		#endif

		glGetIntegerv(GL_NUM_EXTENSIONS, &extensionCount);
		for (machine a = 0; a < extensionCount; a++)
		{
			const char *string = reinterpret_cast<const char *>(glGetStringi(GL_EXTENSIONS, (GLuint) a));
			(extensionsString += string) += ' ';

			data = extensionData;
			for (machine b = 0; b < kGraphicsExtensionCount; b++)
			{
				if ((!extensionFlag[b]) && (data->enabled))
				{
					const char *name1 = data->name1;
					const char *name2 = data->name2;

					if ((Text::CompareText(string, name1)) || ((name2) && (Text::CompareText(string, name2))))
					{
						extensionFlag[b] = true;
						break;
					}
				}

				data++;
			}
		}

		#if C4GLCORE

			data = extensionData;
			for (machine a = 0; a < kGraphicsExtensionCount; a++)
			{
				if ((data->required) && (!extensionFlag[a]))
				{
					return (false);
				}

				data++;
			}

		#endif

		#if C4WINDOWS || C4LINUX

			C4::InitializeOpenglExtensions(capabilities);

		#elif C4MACOS

			C4::InitializeOpenglExtensions(capabilities, openglBundle);

		#endif

		glGetIntegerv(GL_MAX_TEXTURE_SIZE, &capabilities->maxTextureSize);
		glGetIntegerv(GL_MAX_3D_TEXTURE_SIZE, &capabilities->max3DTextureSize);
		glGetIntegerv(GL_MAX_CUBE_MAP_TEXTURE_SIZE, &capabilities->maxCubeTextureSize);
		glGetIntegerv(GL_MAX_ARRAY_TEXTURE_LAYERS, &capabilities->maxArrayTextureLayers);
		glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &capabilities->maxTextureAnisotropy);
		glGetFloatv(GL_MAX_TEXTURE_LOD_BIAS, &capabilities->maxTextureLodBias);
		glGetIntegerv(GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS, &capabilities->maxCombinedTextureImageUnits);

		glGetIntegerv(GL_MAX_COLOR_ATTACHMENTS, &capabilities->maxColorAttachments);
		glGetIntegerv(GL_MAX_RENDERBUFFER_SIZE, &capabilities->maxRenderBufferSize);
		glGetIntegerv(GL_MAX_SAMPLES, &capabilities->maxMultisampleSamples);

		glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &capabilities->maxVertexAttribs);
		glGetIntegerv(GL_MAX_VARYING_COMPONENTS, &capabilities->maxVaryingComponents);
		glGetIntegerv(GL_MAX_VERTEX_UNIFORM_COMPONENTS, &capabilities->maxVertexUniformComponents);
		glGetIntegerv(GL_MAX_VERTEX_TEXTURE_IMAGE_UNITS, &capabilities->maxVertexTextureImageUnits);

		glGetIntegerv(GL_MAX_FRAGMENT_UNIFORM_COMPONENTS, &capabilities->maxFragmentUniformComponents);
		glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, &capabilities->maxFragmentTextureImageUnits);

		#if C4RENDER_OCCLUSION_QUERY

			glGetQueryiv(GL_SAMPLES_PASSED, GL_QUERY_COUNTER_BITS, &capabilities->queryCounterBits);

		#endif

		#if C4GLCORE

			glGetIntegerv(GL_MAX_VERTEX_VARYING_COMPONENTS, &capabilities->maxVertexVaryingComponents);
			glGetIntegerv(GL_MAX_GEOMETRY_VARYING_COMPONENTS, &capabilities->maxGeometryVaryingComponents);
			glGetIntegerv(GL_MAX_GEOMETRY_UNIFORM_COMPONENTS, &capabilities->maxGeometryUniformComponents);
			glGetIntegerv(GL_MAX_GEOMETRY_OUTPUT_VERTICES, &capabilities->maxGeometryOutputVertices);
			glGetIntegerv(GL_MAX_GEOMETRY_TOTAL_OUTPUT_COMPONENTS, &capabilities->maxGeometryTotalOutputComponents);
			glGetIntegerv(GL_MAX_GEOMETRY_TEXTURE_IMAGE_UNITS, &capabilities->maxGeometryTextureImageUnits);

		#endif

		if (extensionFlag[kExtensionGetProgramBinary])
		{
			glGetIntegerv(GL_NUM_PROGRAM_BINARY_FORMATS, &capabilities->programBinaryFormats);
			capabilities->programBinaryFormatArray = new int32[capabilities->programBinaryFormats];
			glGetIntegerv(GL_PROGRAM_BINARY_FORMATS, capabilities->programBinaryFormatArray);
		}

	#elif C4PS4 //[ PS4

		// -- PS4 code hidden --

	#elif C4PS3 //[ PS3

		// -- PS3 code hidden --

	#endif //]

	return (true);
}

void GraphicsMgr::InitializeVariables(void)
{
	unsigned_int32 flags = 0;
	unsigned_int32 disableFlags = kRenderOptionMotionBlur;

	unsigned_int32 speedRank = capabilities->hardwareSpeedRank;
	if (speedRank < 2)
	{
		disableFlags = kRenderOptionHorizonMapping | kRenderOptionAmbientOcclusion | kRenderOptionAmbientBumps | kRenderOptionDistortion | kRenderOptionGlowBloom;
		if (speedRank < 1)
		{
			disableFlags |= kRenderOptionParallaxMapping | kRenderOptionTerrainBumps | kRenderOptionStructureEffects;
		}
	}

	Variable *parallax = TheEngine->InitVariable("renderParallaxMapping", (disableFlags & kRenderOptionParallaxMapping) ? "0" : "1", kVariablePermanent);
	parallax->AddObserver(&renderParallaxMappingObserver);
	if (parallax->GetIntegerValue() != 0)
	{
		flags |= kRenderOptionParallaxMapping;
	}

	Variable *horizon = TheEngine->InitVariable("renderHorizonMapping", (disableFlags & kRenderOptionHorizonMapping) ? "0" : "1", kVariablePermanent);
	horizon->AddObserver(&renderHorizonMappingObserver);
	if (horizon->GetIntegerValue() != 0)
	{
		flags |= kRenderOptionHorizonMapping;
	}

	Variable *terrainBumps = TheEngine->InitVariable("renderTerrainBumps", (disableFlags & kRenderOptionTerrainBumps) ? "0" : "1", kVariablePermanent);
	terrainBumps->AddObserver(&renderTerrainBumpsObserver);
	if (terrainBumps->GetIntegerValue() != 0)
	{
		flags |= kRenderOptionTerrainBumps;
	}

	unsigned_int32 memoryRank = capabilities->hardwareMemoryRank;
	const char *defaultTextureDetailLevel = (memoryRank > 0) ? "0" : "1";

	TheEngine->InitVariable("textureDetailLevel", defaultTextureDetailLevel, kVariablePermanent, &textureDetailLevelObserver);
	TheEngine->InitVariable("paletteDetailLevel", defaultTextureDetailLevel, kVariablePermanent, &paletteDetailLevelObserver);

	Variable *anisotropy = TheEngine->InitVariable("textureAnisotropy", (speedRank > 0) ? "4" : "1", kVariablePermanent);
	textureFilterAnisotropy = anisotropy->GetIntegerValue();
	anisotropy->AddObserver(&textureAnisotropyObserver);

	Variable *structure = TheEngine->InitVariable("renderStructureEffects", (disableFlags & kRenderOptionStructureEffects) ? "0" : "1", kVariablePermanent);
	structure->AddObserver(&renderStructureEffectsObserver);
	if (structure->GetIntegerValue() != 0)
	{
		flags |= kRenderOptionStructureEffects;
	}

	Variable *occlusion = TheEngine->InitVariable("renderAmbientOcclusion", (disableFlags & kRenderOptionAmbientOcclusion) ? "0" : "1", kVariablePermanent);
	occlusion->AddObserver(&renderAmbientOcclusionObserver);
	if (occlusion->GetIntegerValue() != 0)
	{
		flags |= kRenderOptionAmbientOcclusion;
	}

	Variable *ambientBumps = TheEngine->InitVariable("renderAmbientBumps", (disableFlags & kRenderOptionAmbientBumps) ? "0" : "1", kVariablePermanent);
	ambientBumps->AddObserver(&renderAmbientBumpsObserver);
	if (ambientBumps->GetIntegerValue() != 0)
	{
		flags |= kRenderOptionAmbientBumps;
	}

	TheEngine->InitVariable("postBrightness", "1.0", kVariablePermanent, &postBrightnessObserver);

	Variable *motionBlur = TheEngine->InitVariable("postMotionBlur", "0", kVariablePermanent);
	motionBlur->AddObserver(&postMotionBlurObserver);
	if (motionBlur->GetIntegerValue() != 0)
	{
		flags |= kRenderOptionMotionBlur;
	}

	Variable *distortion = TheEngine->InitVariable("postDistortion", (disableFlags & kRenderOptionDistortion) ? "0" : "1", kVariablePermanent);
	distortion->AddObserver(&postDistortionObserver);
	if (distortion->GetIntegerValue() != 0)
	{
		flags |= kRenderOptionDistortion;
	}

	Variable *glowBloom = TheEngine->InitVariable("postGlowBloom", (disableFlags & kRenderOptionGlowBloom) ? "0" : "1", kVariablePermanent);
	glowBloom->AddObserver(&postGlowBloomObserver);
	if (glowBloom->GetIntegerValue() != 0)
	{
		flags |= kRenderOptionGlowBloom;
	}

	renderOptionFlags = flags;
}

void GraphicsMgr::HandleTextureDetailLevelEvent(Variable *variable)
{
	textureDetailLevel = MaxZero(Min(variable->GetIntegerValue(), 2));
	currentGraphicsState |= kGraphicsReactivateTextures;
	Texture::DeactivateAll();
}

void GraphicsMgr::HandlePaletteDetailLevelEvent(Variable *variable)
{
	paletteDetailLevel = MaxZero(Min(variable->GetIntegerValue(), 2));
	currentGraphicsState |= kGraphicsReactivateTextures;
	Texture::DeactivateAll();
}

void GraphicsMgr::HandleTextureAnisotropyEvent(Variable *variable)
{
	textureFilterAnisotropy = variable->GetIntegerValue();
	currentGraphicsState |= kGraphicsReactivateTextures;
	Texture::DeactivateAll();

	if (infiniteShadowFrameBuffer)
	{
		infiniteShadowFrameBuffer->SetTextureAnisotropy(textureFilterAnisotropy);
	}

	if (pointShadowFrameBuffer)
	{
		pointShadowFrameBuffer->SetTextureAnisotropy(textureFilterAnisotropy);
	}

	if (spotShadowFrameBuffer)
	{
		spotShadowFrameBuffer->SetTextureAnisotropy(textureFilterAnisotropy);
	}
}

void GraphicsMgr::HandleRenderParallaxMappingEvent(Variable *variable)
{
	unsigned_int32 flags = renderOptionFlags;

	if (variable->GetIntegerValue() != 0)
	{
		flags |= kRenderOptionParallaxMapping;
	}
	else
	{
		flags &= ~kRenderOptionParallaxMapping;
	}

	renderOptionFlags = flags;
	ResetShaders();
}

void GraphicsMgr::HandleRenderHorizonMappingEvent(Variable *variable)
{
	unsigned_int32 flags = renderOptionFlags;

	if (variable->GetIntegerValue() != 0)
	{
		flags |= kRenderOptionHorizonMapping;
	}
	else
	{
		flags &= ~kRenderOptionHorizonMapping;
	}

	renderOptionFlags = flags;
	ResetShaders();
}

void GraphicsMgr::HandleRenderTerrainBumpsEvent(Variable *variable)
{
	unsigned_int32 flags = renderOptionFlags;

	if (variable->GetIntegerValue() != 0)
	{
		flags |= kRenderOptionTerrainBumps;
	}
	else
	{
		flags &= ~kRenderOptionTerrainBumps;
	}

	renderOptionFlags = flags;
	ResetShaders();
}

void GraphicsMgr::HandleRenderStructureEffectsEvent(Variable *variable)
{
	unsigned_int32 flags = renderOptionFlags;

	if (variable->GetIntegerValue() != 0)
	{
		flags |= kRenderOptionStructureEffects;
	}
	else
	{
		flags &= ~kRenderOptionStructureEffects;
	}

	renderOptionFlags = flags;
	InitializeActiveFlags();
	ResetShaders();
}

void GraphicsMgr::HandleRenderAmbientOcclusionEvent(Variable *variable)
{
	unsigned_int32 flags = renderOptionFlags;

	if (variable->GetIntegerValue() != 0)
	{
		flags |= kRenderOptionAmbientOcclusion;
	}
	else
	{
		flags &= ~kRenderOptionAmbientOcclusion;
	}

	renderOptionFlags = flags;
	InitializeActiveFlags();
	ResetShaders();
}

void GraphicsMgr::HandleRenderAmbientBumpsEvent(Variable *variable)
{
	unsigned_int32 flags = renderOptionFlags;

	if (variable->GetIntegerValue() != 0)
	{
		flags |= kRenderOptionAmbientBumps;
	}
	else
	{
		flags &= ~kRenderOptionAmbientBumps;
	}

	renderOptionFlags = flags;
	ResetShaders();
}

void GraphicsMgr::HandlePostBrightnessEvent(Variable *variable)
{
	brightnessMultiplier = variable->GetFloatValue();
}

void GraphicsMgr::HandlePostMotionBlurEvent(Variable *variable)
{
	unsigned_int32 flags = renderOptionFlags;

	if (variable->GetIntegerValue() != 0)
	{
		flags |= kRenderOptionMotionBlur;
	}
	else
	{
		flags &= ~kRenderOptionMotionBlur;
	}

	renderOptionFlags = flags;
	InitializeActiveFlags();
}

void GraphicsMgr::HandlePostDistortionEvent(Variable *variable)
{
	unsigned_int32 flags = renderOptionFlags;

	if (variable->GetIntegerValue() != 0)
	{
		flags |= kRenderOptionDistortion;
	}
	else
	{
		flags &= ~kRenderOptionDistortion;
	}

	renderOptionFlags = flags;
	InitializeActiveFlags();
}

void GraphicsMgr::HandlePostGlowBloomEvent(Variable *variable)
{
	unsigned_int32 flags = renderOptionFlags;

	if (variable->GetIntegerValue() != 0)
	{
		flags |= kRenderOptionGlowBloom;
	}
	else
	{
		flags &= ~kRenderOptionGlowBloom;
	}

	renderOptionFlags = flags;
	InitializeActiveFlags();
}

void GraphicsMgr::InitializeProcessGrid(void)
{
	volatile Point2D *restrict vertex = processGridVertexBuffer.BeginUpdate<Point2D>();

	float w = 2.0F / (float) kProcessGridWidth;
	float h = 2.0F / (float) kProcessGridHeight;

	for (machine j = 0; j < kProcessGridHeight; j++)
	{
		float y = (float) j * h - 1.0F;

		for (machine i = 0; i < kProcessGridWidth; i++)
		{
			vertex->Set((float) i * w - 1.0F, y);
			vertex++;
		}

		vertex->Set(1.0F, y);
		vertex++;
	}

	for (machine i = 0; i < kProcessGridWidth; i++)
	{
		vertex->Set((float) i * w - 1.0F, 1.0F);
		vertex++;
	}

	vertex->Set(1.0F, 1.0F);

	processGridVertexBuffer.EndUpdate();
}

void GraphicsMgr::InitializeActiveFlags(void)
{
	unsigned_int32 flags = 0;

	if (diagnosticFlags & kDiagnosticTimer)
	{
		flags |= kGraphicsActiveTimer;
	}

	if ((renderOptionFlags & (kRenderOptionStructureEffects | kRenderOptionAmbientOcclusion | kRenderOptionMotionBlur)) != 0)
	{
		unsigned_int32 mask = normalFrameBuffer->GetRenderTargetMask();
		if ((mask & (1 << kRenderTargetStructure)) != 0)
		{
			flags |= kGraphicsActiveStructureRendering;

			if (renderOptionFlags & kRenderOptionStructureEffects)
			{
				flags |= kGraphicsActiveStructureEffects;
			}

			if ((renderOptionFlags & kRenderOptionAmbientOcclusion) && (mask & (1 << kRenderTargetOcclusion1)))
			{
				flags |= kGraphicsActiveAmbientOcclusion;
			}

			if ((renderOptionFlags & kRenderOptionMotionBlur) && (mask & (1 << kRenderTargetVelocity)))
			{
				flags |= kGraphicsActiveVelocityRendering;
			}
		}
	}

	if (renderOptionFlags & kRenderOptionGlowBloom)
	{
		flags |= kGraphicsActiveGlowBloom;
	}

	graphicsActiveFlags = flags;
}

void GraphicsMgr::SetRenderOptionFlags(unsigned_int32 flags)
{
	renderOptionFlags = flags;
	InitializeActiveFlags();
}

void GraphicsMgr::SetDiagnosticFlags(unsigned_int32 flags)
{
	diagnosticFlags = flags;
	InitializeActiveFlags();
}

void GraphicsMgr::SetShaderTime(float time, float delta)
{
	Render::SetUniversalShaderParameter(kUniversalParamShaderTime, time * kInverseShaderTimePeriod, time, delta, 0.0F);
}

void GraphicsMgr::SetImpostorDepthParams(float scale, float offset, float tangent)
{
	Render::SetVertexShaderParameter(kVertexParamImpostorDepth, scale, offset, tangent, 0.0F);
}

void GraphicsMgr::ResetShaders(void)
{
	ShaderData::Purge();
	ShaderProgram::Purge();
}

void GraphicsMgr::BeginRendering(void)
{
	Render::BeginRendering();

	#if C4PS4 //[ PS4

		// -- PS4 code hidden --

	#endif //]

	#if C4IOS //[ MOBILE

		// -- Mobile code hidden --

	#endif //]

	unsigned_int32 graphicsState = currentGraphicsState;
	currentGraphicsState = graphicsState & ~kGraphicsReactivateTextures;

	if (graphicsState & kGraphicsReactivateTextures)
	{
		Texture::ReactivateAll();
	}

	timestampCount[Render::GetFrameCount() & 3] = 0;

	if (graphicsActiveFlags & kGraphicsActiveTimer)
	{
		Timestamp(kTimestampBeginRendering);
	}

	#if C4STATS

		for (machine a = 0; a < kGraphicsCounterCount; a++)
		{
			graphicsCounter[a] = 0;
		}

	#endif

	#if C4OCULUS

		cameraLensMultiplier = 1.0F;

	#endif

	NullClass *object = syncRenderObject;
	if (object)
	{
		if (!syncLoadFlag)
		{
			syncRenderObject = nullptr;
			(object->*syncRenderFunction)(syncRenderData);
			syncRenderSignal->Trigger();
		}
		else
		{
			unsigned_int32	t;

			unsigned_int32 time = TheTimeMgr->GetMillisecondCount();
			do
			{
				if (object)
				{
					syncRenderObject = nullptr;
					(object->*syncRenderFunction)(syncRenderData);
					syncRenderSignal->Trigger();
				}
				else
				{
					Thread::Yield();
				}

				object = syncRenderObject;
				t = TheTimeMgr->GetMillisecondCount();
			} while (t - time < 16);
		}
	}
}

void GraphicsMgr::EndRendering(void)
{
	if (graphicsActiveFlags & kGraphicsActiveTimer)
	{
		Timestamp(kTimestampEndRendering);
	}

	#if C4WINDOWS

		SwapBuffers(deviceContext);

	#elif C4MACOS

		[openglContext flushBuffer];

	#elif C4LINUX

		glXSwapBuffers(openglDisplay, openglWindow);

	#elif C4IOS //[ MOBILE

		// -- Mobile code hidden --

	#endif //]

	Render::EndRendering();

	#if C4PS3 //[ PS3

		// -- PS3 code hidden --

	#endif //]
}

void GraphicsMgr::SetSyncLoadFlag(bool flag)
{
	syncLoadFlag = flag;

	unsigned_int32 displayFlags = TheDisplayMgr->GetDisplayFlags();
	if (flag)
	{
		displayFlags &= ~kDisplayRefreshSync;
	}

	SyncRenderTask(&GraphicsMgr::SetDisplaySyncMode, this, &displayFlags);
}

void GraphicsMgr::SyncRenderTask(void (NullClass::*proc)(const void *), NullClass *object, const void *data)
{
	if (Thread::MainThread())
	{
		(object->*proc)(data);
	}
	else
	{
		GraphicsMgr *graphicsMgr = TheGraphicsMgr;
		graphicsMgr->syncRenderFunction = proc;
		graphicsMgr->syncRenderData = data;

		Thread::Fence();

		graphicsMgr->syncRenderObject = object;
		graphicsMgr->syncRenderSignal->Wait();
	}
}

void GraphicsMgr::Timestamp(TimestampType type)
{
	int32 index = Render::GetFrameCount() & 3;
	int32 count = timestampCount[index];
	if (count < kMaxTimestampCount)
	{
		timestampCount[index] = count + 1;
		timestampType[index][count] = type;
		timestampQuery[index][count].QueryTimestamp();
	}
}

int32 GraphicsMgr::GetTimestampData(TimestampType *type, unsigned_int64 *stamp) const
{
	if (graphicsActiveFlags & kGraphicsActiveTimer)
	{
		int32 index = (Render::GetFrameCount() - 3) & 3;
		int32 count = timestampCount[index];
		for (machine a = 0; a < count; a++)
		{
			type[a] = timestampType[index][a];
			stamp[a] = timestampQuery[index][a].GetTimestamp();
		}

		return (count);
	}

	return (0);
}

void GraphicsMgr::SetFinalColorTransform(const ColorRGBA& scale, const ColorRGBA& bias)
{
	finalColorScale[0] = scale;
	finalColorBias = bias;
	colorTransformFlags = 0;
}

void GraphicsMgr::SetFinalColorTransform(const ColorRGBA& red, const ColorRGBA& green, const ColorRGBA& blue, const ColorRGBA& bias)
{
	finalColorScale[0] = red;
	finalColorScale[1] = green;
	finalColorScale[2] = blue;
	finalColorBias = bias;
	colorTransformFlags = kPostColorMatrix;
}

void GraphicsMgr::SetPostProcessingShader(unsigned_int32 postFlags, const VertexSnippet *snippet)
{
	static Link<ShaderProgram>		postShader[kPostShaderCount];

	postFlags |= colorTransformFlags;
	if (postFlags & kPostColorMatrix)
	{
		ColorRGBA red = finalColorScale[0] * brightnessMultiplier;
		ColorRGBA green = finalColorScale[1] * brightnessMultiplier;
		ColorRGBA blue = finalColorScale[2] * brightnessMultiplier;
		Render::SetFragmentShaderParameter(kFragmentParamConstant0, red);
		Render::SetFragmentShaderParameter(kFragmentParamConstant1, green);
		Render::SetFragmentShaderParameter(kFragmentParamConstant2, blue);
	}
	else
	{
		ColorRGBA scale = finalColorScale[0] * brightnessMultiplier;
		Render::SetFragmentShaderParameter(kFragmentParamConstant0, scale);
	}

	ColorRGBA bias = finalColorBias * brightnessMultiplier;
	Render::SetFragmentShaderParameter(kFragmentParamConstant3, bias);

	ShaderProgram *shaderProgram = postShader[postFlags];
	if (!shaderProgram)
	{
		Process			*colorProcess;
		Process			*positionProcess;
		ShaderGraph		shaderGraph;

		Process *transformProcess = new TransformPostProcess((postFlags & kPostColorMatrix) != 0);
		shaderGraph.AddElement(transformProcess);

		if (postFlags & kPostMotionBlur)
		{
			colorProcess = new MotionBlurPostProcess((postFlags & kPostMotionBlurGradient) != 0);
		}
		else
		{
			colorProcess = new ColorPostProcess;
		}

		shaderGraph.AddElement(colorProcess);

		if (postFlags & kPostDistortion)
		{
			positionProcess = new DistortPostProcess;
		}
		else
		{
			positionProcess = new FragmentPositionProcess;
		}

		shaderGraph.AddElement(positionProcess);
		new Route(positionProcess, colorProcess, 0);

		if (postFlags & kPostGlowBloom)
		{
			Process *glowBloomProcess = new GlowBloomPostProcess;
			shaderGraph.AddElement(glowBloomProcess);

			new Route(colorProcess, glowBloomProcess, 0);
			new Route(glowBloomProcess, transformProcess, 0);
		}
		else
		{
			new Route(colorProcess, transformProcess, 0);
		}

		VertexShader *vertexShader = GetLocalVertexShader(snippet);
		shaderProgram = ShaderAttribute::CompilePostShader(&shaderGraph, vertexShader);
		postShader[postFlags] = shaderProgram;
		vertexShader->Release();
	}

	Render::SetShaderProgram(shaderProgram);
}

void GraphicsMgr::SetDisplayWarpingShader(void)
{
	static Link<ShaderProgram>		warpShader;

	ShaderProgram *shaderProgram = warpShader;
	if (!shaderProgram)
	{
		ShaderGraph		shaderGraph;

		Process *displayWarpProcess = new DisplayWarpProcess(true);
		shaderGraph.AddElement(displayWarpProcess);

		VertexShader *vertexShader = GetLocalVertexShader(&VertexShader::nullTransform);
		shaderProgram = ShaderAttribute::CompilePostShader(&shaderGraph, vertexShader);
		warpShader = shaderProgram;
		vertexShader->Release();
	}

	Render::SetShaderProgram(shaderProgram);
}

void GraphicsMgr::SetRenderTarget(RenderTargetType type)
{
	RenderTargetType prev = currentRenderTargetType;
	if (prev != type)
	{
		currentRenderTargetType = type;

		if ((type == kRenderTargetPrimary) && (multisampleFrameBuffer))
		{
			Render::SetFrameBuffer(multisampleFrameBuffer);
		}
		else if (normalFrameBuffer->GetRenderTargetMask() & (1 << type))
		{
			Render::SetFrameBuffer(normalFrameBuffer);
			normalFrameBuffer->SetColorRenderTexture(normalFrameBuffer->GetRenderTargetTexture(type));
		}
	}
}

void GraphicsMgr::SetDisplayRenderTarget(void)
{
	if (graphicsActiveFlags & kGraphicsActiveTimer)
	{
		Timestamp(kTimestampBeginPost);
	}

	unsigned_int32 graphicsState = currentGraphicsState;
	currentGraphicsState = graphicsState & ~(kGraphicsMotionBlurAvail | kGraphicsDistortionAvail | kGraphicsGlowBloomAvail);
	if (!(renderOptionFlags & kRenderOptionGlowBloom))
	{
		graphicsState &= ~kGraphicsGlowBloomAvail;
	}

	if (multisampleFrameBuffer)
	{
		Render::SetDrawFrameBuffer(normalFrameBuffer);
		Render::SetReadFrameBuffer(multisampleFrameBuffer);

		normalFrameBuffer->SetColorRenderTexture(normalFrameBuffer->GetRenderTargetTexture(kRenderTargetPrimary));

		int32 left = cameraRect.left;
		int32 top = cameraRect.top;
		int32 right = cameraRect.right;
		int32 bottom = cameraRect.bottom;
		Render::ResolveMultisampleFrameBuffer(left, bottom, right, top);

		Render::InvalidateReadFrameBuffer();
		normalFrameBuffer->ResetColorRenderTexture();
	}

	Render::DisableBlend();
	SetRenderState(kRenderDepthInhibit);
	SetMaterialState(currentMaterialState & kMaterialTwoSided);

	const Render::TextureObject *mainTexture = normalFrameBuffer->GetRenderTargetTexture(kRenderTargetPrimary);
	Render::BindGlobalTexture(kTextureUnitColor, mainTexture);

	if (graphicsState & kGraphicsGlowBloomAvail)
	{
		static Link<ShaderProgram>		glowBloomShaderProgram;

		Render::SetFrameBuffer(glowBloomFrameBuffer);

		int32 w = viewportRect.right - viewportRect.left;
		int32 h = viewportRect.top - viewportRect.bottom;
		Render::SetViewport(0, 0, w >> 1, h >> 1);
		Render::SetScissor(0, 0, w >> 1, h >> 1);

		ShaderProgram *shaderProgram = glowBloomShaderProgram;
		if (!shaderProgram)
		{
			ShaderGraph		shaderGraph;

			shaderGraph.AddElement(new GlowBloomExtractPostProcess);

			VertexShader *vertexShader = GetLocalVertexShader(&VertexShader::extractGlowBloomTransform);
			shaderProgram = ShaderAttribute::CompilePostShader(&shaderGraph, vertexShader);
			glowBloomShaderProgram = shaderProgram;
			vertexShader->Release();
		}

		Render::SetShaderProgram(shaderProgram);

		fullscreenVertexData->Bind();
		Render::DrawPrimitives(Render::kPrimitiveTriangles, 0, 3);

		Render::SetViewport(viewportRect.left, viewportRect.bottom, w, h);
		Render::SetScissor(viewportRect.left, viewportRect.bottom, w, h);
	}

	if (normalFrameBuffer->GetRenderTargetMask() & (1 << kRenderTargetDisplay))
	{
		Render::SetFrameBuffer(normalFrameBuffer);
		normalFrameBuffer->SetColorRenderTexture(normalFrameBuffer->GetRenderTargetTexture(kRenderTargetDisplay));
	}
	else
	{
		Render::ResetFrameBuffer();

		#if C4IOS //[ MOBILE

			// -- Mobile code hidden --

		#endif //]
	}

	if (graphicsState & (kGraphicsMotionBlurAvail | kGraphicsDistortionAvail | kGraphicsGlowBloomAvail))
	{
		unsigned_int32 postFlags = 0;

		if (graphicsState & kGraphicsMotionBlurAvail)
		{
			postFlags |= kPostMotionBlur;
		}

		if (graphicsState & kGraphicsDistortionAvail)
		{
			postFlags |= kPostDistortion;
		}

		if (graphicsState & kGraphicsGlowBloomAvail)
		{
			postFlags |= kPostGlowBloom;
		}

		const Render::TextureObject *structureTexture = normalFrameBuffer->GetRenderTargetTexture(kRenderTargetStructure);
		const Render::TextureObject *velocityTexture = normalFrameBuffer->GetRenderTargetTexture(kRenderTargetVelocity);
		const Render::TextureObject *distortionTexture = normalFrameBuffer->GetRenderTargetTexture(kRenderTargetDistortion);
		const Render::TextureObject *glowBloomTexture = glowBloomFrameBuffer->GetRenderTargetTexture();

		if (postFlags & kPostDistortion)
		{
			Render::BindGlobalTexture(kTextureUnitDistortion, distortionTexture);
		}

		if (postFlags & kPostGlowBloom)
		{
			Render::BindGlobalTexture(kTextureUnitGlowBloom, glowBloomTexture);
		}

		if (postFlags & kPostMotionBlur)
		{
			volatile Triangle *restrict basicTriangle = processGridIndexBuffer.BeginUpdate<Triangle>();
			volatile Triangle *restrict gradientTriangle = basicTriangle + kProcessGridWidth * kProcessGridHeight * 2;

			int32 basicCount = 0;
			int32 gradientCount = 0;

			const bool *flag = motionGridFlag;
			machine k = 0;

			for (machine j = 0; j < kProcessGridHeight; j++)
			{
				for (machine i = 0; i < kProcessGridWidth; i++)
				{
					if (!*flag)
					{
						basicTriangle[0].Set(k, k + 1, k + kProcessGridWidth + 2);
						basicTriangle[1].Set(k, k + kProcessGridWidth + 2, k + kProcessGridWidth + 1);
						basicTriangle += 2;
						basicCount++;
					}
					else
					{
						gradientTriangle -= 2;
						gradientTriangle[0].Set(k, k + 1, k + kProcessGridWidth + 2);
						gradientTriangle[1].Set(k, k + kProcessGridWidth + 2, k + kProcessGridWidth + 1);
						gradientCount++;
					}

					flag++;
					k++;
				}

				k++;
			}

			processGridIndexBuffer.EndUpdate();

			Render::BindGlobalTexture(kTextureUnitStructure, structureTexture);
			Render::BindGlobalTexture(kTextureUnitVelocity, velocityTexture);

			if (basicCount != 0)
			{
				SetPostProcessingShader(postFlags, &VertexShader::postProcessTransform);

				processGridVertexData->Bind();
				Render::DrawIndexedPrimitives(Render::kPrimitiveTriangles, basicCount * 6, 0);
			}

			if (gradientCount != 0)
			{
				SetPostProcessingShader(postFlags | kPostMotionBlurGradient, &VertexShader::postProcessTransform);

				processGridVertexData->Bind();
				Render::DrawIndexedPrimitives(Render::kPrimitiveTriangles, gradientCount * 6, (kProcessGridWidth * kProcessGridHeight - gradientCount) * (sizeof(Triangle) * 2));
			}

			structureTexture->Unbind(kTextureUnitStructure);
			structureTexture->Unbind(kTextureUnitVelocity);

			#if C4PS3 //[ PS3

				// -- PS3 code hidden --

			#endif //]
		}
		else
		{
			SetPostProcessingShader(postFlags, &VertexShader::postProcessTransform);

			fullscreenVertexData->Bind();
			Render::DrawPrimitives(Render::kPrimitiveTriangles, 0, 3);
		}

		if (postFlags & kPostDistortion)
		{
			distortionTexture->Unbind(kTextureUnitDistortion);
		}

		if (postFlags & kPostGlowBloom)
		{
			glowBloomTexture->Unbind(kTextureUnitGlowBloom);
		}
	}
	else
	{
		SetPostProcessingShader(0, &VertexShader::nullTransform);

		fullscreenVertexData->Bind();
		Render::DrawPrimitives(Render::kPrimitiveTriangles, 0, 3);
	}

	Render::EnableBlend();

	mainTexture->Unbind(kTextureUnitColor);
	currentRenderTargetType = kRenderTargetNone;

	if (graphicsActiveFlags & kGraphicsActiveTimer)
	{
		Timestamp(kTimestampEndPost);
	}
}

void GraphicsMgr::SetFullFrameRenderTarget(void)
{
	#if C4OCULUS

		if (normalFrameBuffer->GetRenderTargetMask() & (1 << kRenderTargetDisplay))
		{
			Render::ResetFrameBuffer();
			Render::DisableBlend();
			SetRenderState(kRenderDepthInhibit);
			SetMaterialState(currentMaterialState & kMaterialTwoSided);

			const Render::TextureObject *mainTexture = normalFrameBuffer->GetRenderTargetTexture(kRenderTargetDisplay);
			Render::BindGlobalTexture(kTextureUnitColor, mainTexture);

			SetDisplayWarpingShader();

			int32 index = (cameraLensMultiplier < 0.0F);
			Render::SetFragmentShaderParameter(0, Oculus::GetFullFrameParam(index));
			Render::SetFragmentShaderParameter(1, Oculus::GetDisplayParam(index));
			Render::SetFragmentShaderParameter(2, Oculus::GetDistortionParam());
			Render::SetFragmentShaderParameter(3, Oculus::GetChromaticParam());

			int32 fullFrameWidth = Oculus::GetFullFrameWidth() / 2;
			int32 fullFrameHeight = Oculus::GetFullFrameHeight();

			int32 i = fullFrameWidth * index;
			int32 j = Oculus::GetScissorInset();
			Render::SetViewport(i, 0, fullFrameWidth, fullFrameHeight);
			Render::SetScissor(i, j, fullFrameWidth, fullFrameHeight - j * 2);

			fullscreenVertexData->Bind();
			Render::DrawPrimitives(Render::kPrimitiveTriangles, 0, 3);

			Render::EnableBlend();

			mainTexture->Unbind(kTextureUnitColor);
			currentRenderTargetType = kRenderTargetNone;
		}

	#endif

	normalFrameBuffer->Invalidate();
}

void GraphicsMgr::CopyRenderTarget(Texture *texture, const Rect& rect)
{
	if ((multisampleFrameBuffer) && (currentRenderTargetType == kRenderTargetPrimary))
	{
		Render::SetDrawFrameBuffer(&genericFrameBuffer);
		genericFrameBuffer.SetColorRenderTexture(texture);

		int32 w = rect.Width();
		int32 h = rect.Height();
		int32 left = rect.left;
		int32 bottom = renderTargetHeight - rect.bottom;
		Render::CopyFrameBuffer(left, bottom, left + w, bottom + h, 0, 0, w, h, Render::kBlitFilterPoint);

		genericFrameBuffer.ResetColorRenderTexture();
		Render::SetDrawFrameBuffer(multisampleFrameBuffer);
	}
	else
	{
		texture->BlitImageRect(rect.left, renderTargetHeight - rect.bottom, 0, 0, rect.Width(), rect.Height());
	}
}

void GraphicsMgr::ActivateOrthoCamera(void)
{
	const OrthoCameraObject *orthoCameraObject = static_cast<const OrthoCameraObject *>(cameraObject);

	float nearDepth = orthoCameraObject->GetNearDepth();
	float farDepth = orthoCameraObject->GetFarDepth();
	currentNearDepth = nearDepth;

	Render::SetUniversalShaderParameter(kUniversalParamCameraData, 0.0F, 1.0F, nearDepth, farDepth);

	currentFrustumFlags = 0;
	cameraPosition4D = -cameraTransformable->GetWorldTransform()[2];

	Matrix4D& matrix = cameraProjectionMatrix;
	matrix(0,1) = matrix(0,2) = matrix(1,0) = matrix(1,2) = matrix(2,0) = matrix(2,1) = matrix(3,0) = matrix(3,1) = matrix(3,2) = 0.0F;
	matrix(3,3) = 1.0F;

	float orthoLeft = orthoCameraObject->GetOrthoRectLeft();
	float orthoRight = orthoCameraObject->GetOrthoRectRight();
	float orthoTop = orthoCameraObject->GetOrthoRectTop();
	float orthoBottom = orthoCameraObject->GetOrthoRectBottom();

	float dx = 1.0F / (orthoRight - orthoLeft);
	float dy = 1.0F / (orthoBottom - orthoTop);
	float dz = 1.0F / (farDepth - nearDepth);

	matrix(0,0) = 2.0F * dx;
	matrix(0,3) = -(orthoRight + orthoLeft) * dx;
	matrix(1,1) = 2.0F * dy;
	matrix(1,3) = -(orthoBottom + orthoTop) * dy;
	matrix(2,2) = -2.0F * dz;
	matrix(2,3) = -(farDepth + nearDepth) * dz;

	#if C4OCULUS

		matrix(0,3) += orthoCameraObject->GetProjectionOffset() * cameraLensMultiplier;

	#endif

	currentProjectionMatrix = matrix;
	depthOffsetConstant = 0.0F;

	currentGraphicsState &= ~(kGraphicsLightScissor | kGraphicsObliqueFrustum);
	currentRenderState &= ~kRenderDepthOffset;
	disabledRenderState = kRenderDepthOffset;

	const Rect& rect = orthoCameraObject->GetViewRect();
	int32 left = rect.left;
	int32 right = rect.right;
	int32 bottom = renderTargetHeight - rect.bottom;
	int32 top = renderTargetHeight - rect.top;
	int32 width = right - left;
	int32 height = top - bottom;

	viewportRect.Set(left, top, right, bottom);
	cameraRect.Set(left, top, right, bottom);

	Render::SetUniversalShaderParameter(kUniversalParamViewportTransform, (float) width, (float) height, (float) left, (float) bottom);
	Render::SetViewport(left, bottom, width, height);

	if (currentGraphicsState & kGraphicsClipEnabled)
	{
		left = Max(left, clipRect.left);
		right = Min(right, clipRect.right);
		bottom = Max(bottom, renderTargetHeight - clipRect.bottom);
		top = Min(top, renderTargetHeight - clipRect.top);

		right = Max(left, right);
		top = Max(bottom, top);

		width = right - left;
		height = top - bottom;

		scissorRect.Set(left, top, right, bottom);
	}
	else
	{
		scissorRect = cameraRect;
	}

	Render::SetScissor(left, bottom, width, height);
}

void GraphicsMgr::ActivateFrustumCamera(void)
{
	const FrustumCameraObject *frustumCameraObject = static_cast<const FrustumCameraObject *>(cameraObject);

	float nearDepth = frustumCameraObject->GetNearDepth();
	float farDepth = frustumCameraObject->GetFarDepth();
	currentNearDepth = nearDepth;

	Render::SetUniversalShaderParameter(kUniversalParamCameraData, 1.0F, 0.0F, nearDepth, farDepth);

	unsigned_int32 flags = frustumCameraObject->GetFrustumFlags();
	currentFrustumFlags = flags;

	cameraPosition4D = cameraTransformable->GetWorldPosition();

	Matrix4D& matrix = cameraProjectionMatrix;
	matrix(0,1) = matrix(0,2) = matrix(0,3) = matrix(1,0) = matrix(1,2) = matrix(1,3) = matrix(2,0) = matrix(2,1) = matrix(3,0) = matrix(3,1) = matrix(3,3) = 0.0F;
	matrix(3,2) = -1.0F;

	float focalLength = frustumCameraObject->GetFocalLength();
	matrix(0,0) = focalLength;
	matrix(1,1) = focalLength / frustumCameraObject->GetAspectRatio();

	#if C4OCULUS

		matrix(0,2) = -frustumCameraObject->GetProjectionOffset() * cameraLensMultiplier;

	#endif

	currentGraphicsState &= ~(kGraphicsLightScissor | kGraphicsObliqueFrustum);

	if (flags & kFrustumInfinite)
	{
		matrix(2,2) = kFrustumEpsilon - 1.0F;
		matrix(2,3) = nearDepth * (kFrustumEpsilon - 2.0F);
	}
	else
	{
		float d = -1.0F / (farDepth - nearDepth);
		float k = 2.0F * farDepth * nearDepth;

		matrix(2,2) = (farDepth + nearDepth) * d;
		matrix(2,3) = k * d;
	}

	standardProjectionMatrix = matrix;
	currentProjectionMatrix = matrix;
	depthOffsetConstant = -matrix(2,3) / matrix(2,2);

	currentRenderState &= ~kRenderDepthOffset;
	disabledRenderState = 0;

	const Rect& rect = frustumCameraObject->GetViewRect();
	int32 left = rect.left;
	int32 right = rect.right;
	int32 bottom = renderTargetHeight - rect.bottom;
	int32 top = renderTargetHeight - rect.top;
	int32 width = right - left;
	int32 height = top - bottom;

	viewportRect.Set(left, top, right, bottom);
	cameraRect.Set(left, top, right, bottom);

	occlusionPlaneScale = 2.0F / ((float) width * focalLength);
	renderTargetOffsetSize = (float) width;

	Render::SetUniversalShaderParameter(kUniversalParamViewportTransform, (float) width, (float) height, (float) left, (float) bottom);
	Render::SetViewport(left, bottom, width, height);

	if (currentGraphicsState & kGraphicsClipEnabled)
	{
		left = Max(left, clipRect.left);
		right = Min(right, clipRect.right);
		bottom = Max(bottom, renderTargetHeight - clipRect.bottom);
		top = Min(top, renderTargetHeight - clipRect.top);

		right = Max(left, right);
		top = Max(bottom, top);

		width = right - left;
		height = top - bottom;

		scissorRect.Set(left, top, right, bottom);
	}
	else
	{
		scissorRect = cameraRect;
	}

	Render::SetScissor(left, bottom, width, height);
}

void GraphicsMgr::ActivateRemoteCamera(void)
{
	const RemoteCameraObject *remoteCameraObject = static_cast<const RemoteCameraObject *>(cameraObject);

	float nearDepth = remoteCameraObject->GetNearDepth();
	float farDepth = remoteCameraObject->GetFarDepth();
	currentNearDepth = nearDepth;

	Render::SetUniversalShaderParameter(kUniversalParamCameraData, 1.0F, 0.0F, nearDepth, farDepth);

	unsigned_int32 flags = remoteCameraObject->GetFrustumFlags();
	currentFrustumFlags = flags;

	cameraPosition4D = cameraTransformable->GetWorldPosition();

	Matrix4D& matrix = cameraProjectionMatrix;
	matrix(0,1) = matrix(0,2) = matrix(0,3) = matrix(1,0) = matrix(1,2) = matrix(1,3) = matrix(2,0) = matrix(2,1) = matrix(3,0) = matrix(3,1) = matrix(3,3) = 0.0F;
	matrix(3,2) = -1.0F;

	float focalLength = remoteCameraObject->GetFocalLength();
	matrix(0,0) = focalLength;
	matrix(1,1) = focalLength / remoteCameraObject->GetAspectRatio();

	#if C4OCULUS

		matrix(0,2) = -remoteCameraObject->GetProjectionOffset() * cameraLensMultiplier;

	#endif

	unsigned_int32 graphicsState = currentGraphicsState & ~(kGraphicsLightScissor | kGraphicsObliqueFrustum);

	if (flags & kFrustumInfinite)
	{
		matrix(2,2) = kFrustumEpsilon - 1.0F;
		matrix(2,3) = nearDepth * (kFrustumEpsilon - 2.0F);
	}
	else
	{
		float d = -1.0F / (farDepth - nearDepth);
		float k = 2.0F * farDepth * nearDepth;

		matrix(2,2) = (farDepth + nearDepth) * d;
		matrix(2,3) = k * d;
	}

	standardProjectionMatrix = matrix;

	if (flags & kFrustumOblique)
	{
		Antivector4D clipPlane = remoteCameraObject->GetRemoteClipPlane() * Inverse(cameraSpaceTransform);
		if (clipPlane.w < 0.0F)
		{
			float qpx = (clipPlane.x < 0.0F) ? -1.0F : 1.0F;
			float qpy = (clipPlane.y < 0.0F) ? -1.0F : 1.0F;

			float qx = (qpx + matrix(0,2)) / matrix(0,0);
			float qy = (qpy + matrix(1,2)) / matrix(1,1);
			float qw = (1.0F + matrix(2,2)) / matrix(2,3);

			float scale = 2.0F / (clipPlane.x * qx + clipPlane.y * qy - clipPlane.z + clipPlane.w * qw);

			matrix(2,0) = scale * clipPlane.x;
			matrix(2,1) = scale * clipPlane.y;
			matrix(2,2) = scale * clipPlane.z + 1.0F;
			matrix(2,3) = scale * clipPlane.w;

			graphicsState |= kGraphicsObliqueFrustum;
		}
	}

	currentGraphicsState = graphicsState;
	currentProjectionMatrix = matrix;
	depthOffsetConstant = -matrix(2,3) / matrix(2,2);

	currentRenderState &= ~kRenderDepthOffset;
	disabledRenderState = 0;

	const Rect& rect = remoteCameraObject->GetViewRect();
	int32 left = rect.left;
	int32 right = rect.right;
	int32 bottom = renderTargetHeight - rect.bottom;
	int32 top = renderTargetHeight - rect.top;
	int32 width = right - left;
	int32 height = top - bottom;

	viewportRect.Set(left, top, right, bottom);

	occlusionPlaneScale = 2.0F / ((float) width * focalLength);
	renderTargetOffsetSize = (float) width;

	Render::SetUniversalShaderParameter(kUniversalParamViewportTransform, (float) width, (float) height, (float) left, (float) bottom);
	Render::SetViewport(left, bottom, width, height);

	const ProjectionRect& frustumBoundary = remoteCameraObject->GetFrustumBoundary();
	float x1 = frustumBoundary.left * 0.5F + 0.5F;
	float x2 = frustumBoundary.right * 0.5F + 0.5F;
	float y1 = frustumBoundary.bottom * 0.5F + 0.5F;
	float y2 = frustumBoundary.top * 0.5F + 0.5F;

	float w = (float) width * 0.5F;
	float h = (float) height * 0.5F;

	int32 cameraLeft = left + (int32) (x1 * w * 2.0F);
	int32 cameraRight = left + (int32) (x2 * w * 2.0F + 0.5F);
	int32 cameraBottom = bottom + (int32) (y1 * h * 2.0F);
	int32 cameraTop = bottom + (int32) (y2 * h * 2.0F + 0.5F);

	cameraRect.Set(cameraLeft, cameraTop, cameraRight, cameraBottom);

	if (currentGraphicsState & kGraphicsClipEnabled)
	{
		left = Max(left, clipRect.left);
		right = Min(right, clipRect.right);
		bottom = Max(bottom, renderTargetHeight - clipRect.bottom);
		top = Min(top, renderTargetHeight - clipRect.top);

		right = Max(left, right);
		top = Max(bottom, top);

		scissorRect.Set(left, top, right, bottom);
		Render::SetScissor(left, bottom, right - left, top - bottom);
	}
	else
	{
		scissorRect = cameraRect;
		Render::SetScissor(cameraLeft, cameraBottom, cameraRight - cameraLeft, cameraTop - cameraBottom);
	}
}

void GraphicsMgr::SetCamera(const CameraObject *camera, const Transformable *transformable, unsigned_int32 clearMask, bool reset)
{
	cameraObject = camera;
	cameraTransformable = transformable;

	const Transform4D& worldTransform = (transformable) ? transformable->GetWorldTransform() : Identity4D;
	const Vector3D& rightDirection = worldTransform[0];
	const Vector3D& downDirection = worldTransform[1];
	const Vector3D& viewDirection = worldTransform[2];

	if (camera->GetCameraType() != kCameraOrtho)
	{
		const FrustumCameraObject *object = static_cast<const FrustumCameraObject *>(camera);

		const Rect& rect = object->GetViewRect();
		float viewWidth = (float) rect.Width();

		float focal = object->GetFocalLength();
		float factor = focal * viewWidth;
		Render::SetUniversalShaderParameter(kUniversalParamRadiusPointFactor, factor, 0.0F, 0.0F, 0.0F);

		factor = 1.0F / factor;
		float d = -(viewDirection * worldTransform.GetTranslation());
		Render::SetUniversalShaderParameter(kUniversalParamPointCameraPlane, viewDirection.x * factor, viewDirection.y * factor, viewDirection.z * factor, d * factor);

		focal = 1.0F / focal;
		distortionPlane.Set(viewDirection.x * focal, viewDirection.y * focal, viewDirection.z * focal, d * focal);

		occlusionAreaNormalizer = 1.0F / (viewWidth * (float) rect.Height());
	}

	cameraSpaceTransform(0,0) = rightDirection.x;
	cameraSpaceTransform(0,1) = rightDirection.y;
	cameraSpaceTransform(0,2) = rightDirection.z;
	cameraSpaceTransform(1,0) = -downDirection.x;
	cameraSpaceTransform(1,1) = -downDirection.y;
	cameraSpaceTransform(1,2) = -downDirection.z;
	cameraSpaceTransform(2,0) = -viewDirection.x;
	cameraSpaceTransform(2,1) = -viewDirection.y;
	cameraSpaceTransform(2,2) = -viewDirection.z;

	const Vector3D& worldOffset = worldTransform[3];
	cameraSpaceTransform(0,3) = -(cameraSpaceTransform.GetRow(0) ^ worldOffset);
	cameraSpaceTransform(1,3) = -(cameraSpaceTransform.GetRow(1) ^ worldOffset);
	cameraSpaceTransform(2,3) = -(cameraSpaceTransform.GetRow(2) ^ worldOffset);

	camera->Activate(this);
	unsigned_int32 graphicsState = currentGraphicsState;

	if (Determinant(worldTransform) > 0.0F)
	{
		if (!(graphicsState & kGraphicsFrontFaceCCW))
		{
			graphicsState |= kGraphicsFrontFaceCCW;
			Render::SetFrontFace(Render::kFrontCCW);
		}
	}
	else
	{
		if (graphicsState & kGraphicsFrontFaceCCW)
		{
			graphicsState &= ~kGraphicsFrontFaceCCW;
			Render::SetFrontFace(Render::kFrontCW);
		}
	}

	currentGraphicsState = graphicsState;

	unsigned_int32 clearFlags = camera->GetClearFlags() & clearMask;
	if (clearFlags != 0)
	{
		unsigned_int32 renderState = currentRenderState;

		if (clearFlags & kClearColorBuffer)
		{
			const ColorRGBA& color = camera->GetClearColor();

			if (renderState & kRenderColorInhibit)
			{
				Render::SetColorMask(true, true, true, true);
				renderState &= ~kRenderColorInhibit;
			}

			if (clearFlags & kClearDepthStencilBuffer)
			{
				if (renderState & kRenderDepthInhibit)
				{
					Render::SetDepthMask(true);
					renderState &= ~kRenderDepthInhibit;
				}

				Render::ClearColorDepthStencilBuffers(&color.red);
			}
			else
			{
				Render::ClearColorBuffer(&color.red);
			}
		}
		else
		{
			if (renderState & kRenderDepthInhibit)
			{
				Render::SetDepthMask(true);
				renderState &= ~kRenderDepthInhibit;
			}

			Render::ClearDepthStencilBuffers();
		}

		currentRenderState = renderState;
	}

	if (graphicsState & kGraphicsRenderShadow)
	{
		float cp = Magnitude(viewDirection.GetVector2D());
		float elevation = Atan(-viewDirection.z, cp) * K::degrees;

		float scale = 1.0F / (cp * (camera->GetFarDepth() - camera->GetNearDepth()));

		if (elevation < 30.5F)
		{
			float t = FmaxZero(elevation - 14.5F) * 0.0625F;
			Render::SetUniversalShaderParameter(kUniversalParamImpostorShadowBlend, 1.0F - t, t, 0.0F, 0.0F);
			Render::SetUniversalShaderParameter(kUniversalParamImpostorShadowScale, scale * (1.0F - t), scale * t, 0.0F, 0.0F);
		}
		else if (elevation < 45.5F)
		{
			float t = FmaxZero(elevation - 29.5F) * 0.0625F;
			Render::SetUniversalShaderParameter(kUniversalParamImpostorShadowBlend, 0.0F, 1.0F - t, t, 0.0F);
			Render::SetUniversalShaderParameter(kUniversalParamImpostorShadowScale, 0.0F, scale * (1.0F - t), scale * t, 0.0F);
		}
		else if (elevation < 60.5F)
		{
			float t = FmaxZero(elevation - 44.5F) * 0.0625F;
			Render::SetUniversalShaderParameter(kUniversalParamImpostorShadowBlend, 0.0F, 0.0F, 1.0F - t, t);
			Render::SetUniversalShaderParameter(kUniversalParamImpostorShadowScale, 0.0F, 0.0F, scale * (1.0F - t), scale * t);
		}
		else
		{
			Render::SetUniversalShaderParameter(kUniversalParamImpostorShadowBlend, 0.0F, 0.0F, 0.0F, 1.0F);
			Render::SetUniversalShaderParameter(kUniversalParamImpostorShadowScale, 0.0F, 0.0F, 0.0F, scale);
		}
	}
	else
	{
		const Point3D& position = worldTransform.GetTranslation();
		directCameraPosition = position;

		Render::SetUniversalShaderParameter(kUniversalParamImpostorCameraPosition, position);
	}

	if (reset)
	{
		fogSpaceObject = nullptr;
		fogSpaceTransformable = nullptr;

		currentShaderVariant = kShaderVariantNormal;
		currentGraphicsState &= ~kGraphicsAmbientLessEqual;

		SetAmbientLight();
	}
}

void GraphicsMgr::SetFogSpace(const FogSpaceObject *fogSpace, const Transformable *transformable)
{
	fogSpaceObject = fogSpace;
	fogSpaceTransformable = transformable;

	if (fogSpace)
	{
		float	f1, f2;

		const Transform4D& m = transformable->GetInverseWorldTransform();
		const MatrixRow4D& plane = m.GetRow(2);
		worldFogPlane = plane;

		float F_wedge_C = plane ^ cameraTransformable->GetWorldPosition();
		if (F_wedge_C > 0.0F)
		{
			f1 = 0.0F;
			f2 = 1.0F;
		}
		else
		{
			f1 = 1.0F;
			f2 = -1.0F;
		}

		float density = fogSpace->GetFogDensity();
		Render::SetUniversalShaderParameter(kUniversalParamFogParams1, F_wedge_C, f1, 1.0F - f1, f2);
		Render::SetUniversalShaderParameter(kUniversalParamFogParams2, F_wedge_C, -F_wedge_C * F_wedge_C, f1, density * K::one_over_ln_2);
		Render::SetUniversalShaderParameter(kUniversalParamFogColor, fogSpace->GetFogColor());

		currentShaderVariant = (fogSpace->GetFogFunction() == kFogFunctionLinear) ? kShaderVariantLinearFog : kShaderVariantConstantFog;
	}
	else
	{
		currentShaderVariant = kShaderVariantNormal;
	}
}

void GraphicsMgr::SetAmbientLight(void)
{
	if (lightObject)
	{
		lightObject = nullptr;

		infiniteShadowFrameBuffer->GetRenderTargetTexture()->SetCompareMode(Render::kTextureCompareNone);
		pointShadowFrameBuffer->GetRenderTargetTexture()->SetCompareMode(Render::kTextureCompareNone);
		spotShadowFrameBuffer->GetRenderTargetTexture()->SetCompareMode(Render::kTextureCompareNone);
	}

	lightTransformable = nullptr;
	currentShadowFlag = false;

	unsigned_int32 oldGraphicsState = currentGraphicsState;
	unsigned_int32 newGraphicsState = oldGraphicsState & ~(kGraphicsLightScissor | kGraphicsLightDepthBounds);
	currentGraphicsState = newGraphicsState;

	unsigned_int32 changed = (newGraphicsState ^ oldGraphicsState) & (kGraphicsLightScissor | kGraphicsLightDepthBounds);
	if (changed != 0)
	{
		if (changed & kGraphicsLightDepthBounds)
		{
			Render::DisableDepthBoundsTest();
		}

		if (changed & kGraphicsLightScissor)
		{
			int32 left = scissorRect.left;
			int32 bottom = scissorRect.bottom;
			Render::SetScissor(left, bottom, scissorRect.right - left, scissorRect.top - bottom);
		}
	}

	currentShaderType = kShaderAmbient;
}

void GraphicsMgr::SetAmbientDepthLessEqual(bool equal)
{
	if (equal)
	{
		currentGraphicsState |= kGraphicsAmbientLessEqual;
	}
	else
	{
		currentGraphicsState &= ~kGraphicsAmbientLessEqual;
	}
}

void GraphicsMgr::BeginClip(const Rect& rect)
{
	currentGraphicsState |= kGraphicsClipEnabled;
	clipRect = rect;

	int32 left = Max(cameraRect.left, rect.left);
	int32 bottom = Max(cameraRect.bottom, viewportRect.top - rect.bottom);
	int32 right = Min(cameraRect.right, rect.right);
	int32 top = Min(cameraRect.top, viewportRect.top - rect.top);

	scissorRect.Set(left, top, right, bottom);
	Render::SetScissor(left, bottom, right - left, top - bottom);
}

void GraphicsMgr::EndClip(void)
{
	currentGraphicsState &= ~kGraphicsClipEnabled;
	scissorRect = cameraRect;

	int32 left = cameraRect.left;
	int32 bottom = cameraRect.bottom;
	Render::SetScissor(left, bottom, cameraRect.right - left, cameraRect.top - bottom);
}

void GraphicsMgr::SetInfiniteLight(const InfiniteLightObject *light, const Transformable *transformable, const LightShadowData *shadowData, bool unified)
{
	lightObject = light;
	lightTransformable = transformable;
	currentShadowFlag = false;
	currentShaderType = (unified) ? kShaderUnified : kShaderInfiniteLight;

	Render::SetUniversalShaderParameter(kUniversalParamLightColor, light->GetLightColor());

	if (shadowData)
	{
		lightShadowData = shadowData;
		currentShadowFlag = true;

		Render::TextureObject *shadowTexture = infiniteShadowFrameBuffer->GetRenderTargetTexture();
		shadowTexture->SetCompareMode(Render::kTextureCompareReference);
		Render::BindGlobalTexture(kTextureUnitShadowMap, shadowTexture);

		float d = 1.5F / infiniteShadowMapSize;
		Render::SetUniversalShaderParameter(kUniversalParamShadowSample1, -0.125F * d, -0.375F * d, 0.375F * d, -0.125F * d);
		Render::SetUniversalShaderParameter(kUniversalParamShadowSample2, 0.125F * d, 0.375F * d, -0.375F * d, 0.125F * d);

		const Transform4D& inverseLightTransform = transformable->GetInverseWorldTransform();
		const Vector3D& cameraView = cameraTransformable->GetWorldTransform()[2];
		float f = InverseSqrt(cameraView.x * cameraView.x + cameraView.y * cameraView.y);
		Vector3D shadowView = inverseLightTransform[0] * (cameraView.x * f) + inverseLightTransform[1] * (cameraView.y * f);

		#if C4OPENGL

			Render::SetUniversalShaderParameter(kUniversalParamShadowViewDirection, shadowView.x * -shadowData->inverseShadowSize.x, shadowView.y * -shadowData->inverseShadowSize.y, shadowView.z * -shadowData->inverseShadowSize.z, 0.0F);

		#elif C4CONSOLE //[ CONSOLE

			// -- Console code hidden --

		#endif //]

		Vector3D scale1(shadowData[1].inverseShadowSize.x * shadowData[0].shadowSize.x, shadowData[1].inverseShadowSize.y * shadowData[0].shadowSize.y, shadowData[1].inverseShadowSize.z * shadowData[0].shadowSize.z);
		Vector3D scale2(shadowData[2].inverseShadowSize.x * shadowData[0].shadowSize.x, shadowData[2].inverseShadowSize.y * shadowData[0].shadowSize.y, shadowData[2].inverseShadowSize.z * shadowData[0].shadowSize.z);
		Vector3D scale3(shadowData[3].inverseShadowSize.x * shadowData[0].shadowSize.x, shadowData[3].inverseShadowSize.y * shadowData[0].shadowSize.y, shadowData[3].inverseShadowSize.z * shadowData[0].shadowSize.z);

		Render::SetUniversalShaderParameter(kUniversalParamShadowMapScale1, scale1);
		Render::SetUniversalShaderParameter(kUniversalParamShadowMapScale2, scale2);
		Render::SetUniversalShaderParameter(kUniversalParamShadowMapScale3, scale3);

		#if C4OPENGL

			Render::SetUniversalShaderParameter(kUniversalParamShadowMapOffset1,
					shadowData[1].shadowPosition.x * shadowData[1].inverseShadowSize.x + 0.5F - scale1.x * (shadowData[0].shadowPosition.x * shadowData[0].inverseShadowSize.x + 0.5F),
					shadowData[1].shadowPosition.y * shadowData[1].inverseShadowSize.y + 0.5F - scale1.y * (shadowData[0].shadowPosition.y * shadowData[0].inverseShadowSize.y + 0.5F),
					shadowData[1].shadowPosition.z * shadowData[1].inverseShadowSize.z - scale1.z * (shadowData[0].shadowPosition.z * shadowData[0].inverseShadowSize.z), 0.0F);
			Render::SetUniversalShaderParameter(kUniversalParamShadowMapOffset2,
					shadowData[2].shadowPosition.x * shadowData[2].inverseShadowSize.x + 0.5F - scale2.x * (shadowData[0].shadowPosition.x * shadowData[0].inverseShadowSize.x + 0.5F),
					shadowData[2].shadowPosition.y * shadowData[2].inverseShadowSize.y + 0.5F - scale2.y * (shadowData[0].shadowPosition.y * shadowData[0].inverseShadowSize.y + 0.5F),
					shadowData[2].shadowPosition.z * shadowData[2].inverseShadowSize.z - scale2.z * (shadowData[0].shadowPosition.z * shadowData[0].inverseShadowSize.z), 0.0F);
			Render::SetUniversalShaderParameter(kUniversalParamShadowMapOffset3,
					shadowData[3].shadowPosition.x * shadowData[3].inverseShadowSize.x + 0.5F - scale3.x * (shadowData[0].shadowPosition.x * shadowData[0].inverseShadowSize.x + 0.5F),
					shadowData[3].shadowPosition.y * shadowData[3].inverseShadowSize.y + 0.5F - scale3.y * (shadowData[0].shadowPosition.y * shadowData[0].inverseShadowSize.y + 0.5F),
					shadowData[3].shadowPosition.z * shadowData[3].inverseShadowSize.z - scale3.z * (shadowData[0].shadowPosition.z * shadowData[0].inverseShadowSize.z), 0.0F);

		#elif C4PS4 //[ PS4

			// -- PS4 code hidden --

		#endif //]
	}

	lightRect = viewportRect;
	lightDepthBounds.Set(0.0F, 1.0F);

	unsigned_int32 oldGraphicsState = currentGraphicsState;
	unsigned_int32 newGraphicsState = oldGraphicsState & ~(kGraphicsLightScissor | kGraphicsLightDepthBounds);
	currentGraphicsState = newGraphicsState;

	unsigned_int32 changed = (newGraphicsState ^ oldGraphicsState) & (kGraphicsLightScissor | kGraphicsLightDepthBounds);
	if (changed != 0)
	{
		if (changed & kGraphicsLightDepthBounds)
		{
			Render::DisableDepthBoundsTest();
		}

		if (changed & kGraphicsLightScissor)
		{
			int32 left = scissorRect.left;
			int32 bottom = scissorRect.bottom;
			Render::SetScissor(left, bottom, scissorRect.right - left, scissorRect.top - bottom);
		}
	}
}

ProjectionResult GraphicsMgr::CalculatePointLightBounds(const PointLightObject *light, const Transformable *transformable, Rect *lightBounds, Range<float> *depthBounds)
{
	ProjectionRect		projectionRect;

	const FrustumCameraObject *frustumCamera = static_cast<const FrustumCameraObject *>(cameraObject);
	Point3D center = cameraSpaceTransform * transformable->GetWorldPosition();

	float r = light->GetLightRange();
	ProjectionResult result = frustumCamera->ProjectSphere(center, r, standardProjectionMatrix(0,2), &projectionRect);
	if (result != kProjectionEmpty)
	{
		if (result == kProjectionPartial)
		{
			const Rect& viewRect = frustumCamera->GetViewRect();

			float viewLeft = (float) viewRect.left;
			float viewBottom = (float) (renderTargetHeight - viewRect.bottom);
			float viewWidth = (float) viewRect.Width() * 0.5F;
			float viewHeight = (float) viewRect.Height() * 0.5F;

			int32 scissorLeft = Max(scissorRect.left, (int32) (viewLeft + viewWidth * (projectionRect.left + 1.0F)));
			int32 scissorRight = Min(scissorRect.right, (int32) (viewLeft + viewWidth * (projectionRect.right + 1.0F)));
			int32 scissorBottom = Max(scissorRect.bottom, (int32) (viewBottom + viewHeight * (projectionRect.bottom + 1.0F)));
			int32 scissorTop = Min(scissorRect.top, (int32) (viewBottom + viewHeight * (projectionRect.top + 1.0F)));

			if ((scissorRight <= scissorLeft) || (scissorTop <= scissorBottom))
			{
				return (kProjectionEmpty);
			}

			lightBounds->Set(scissorLeft, scissorTop, scissorRight, scissorBottom);
		}
		else
		{
			*lightBounds = viewportRect;
		}

		float n = -currentNearDepth;
		float z1 = Fmin(center.z + r, n);
		float z2 = Fmin(center.z - r, n);

		float p33 = standardProjectionMatrix(2,2);
		float p34 = standardProjectionMatrix(2,3);
		float dmin = -0.5F * (p33 * z1 + p34) / z1 + 0.5F;
		float dmax = -0.5F * (p33 * z2 + p34) / z2 + 0.5F;

		depthBounds->Set(dmin, dmax);
	}

	return (result);
}

void GraphicsMgr::SetPointLight(const PointLightObject *light, const Transformable *transformable, float colorMultiplier, ProjectionResult projection, const Rect *lightBounds, const Range<float> *depthBounds, bool shadow)
{
	lightObject = light;
	lightTransformable = transformable;
	currentShadowFlag = false;

	if (shadow)
	{
		currentShadowFlag = true;

		Render::TextureObject *shadowTexture = pointShadowFrameBuffer->GetRenderTargetTexture();
		shadowTexture->SetCompareMode(Render::kTextureCompareReference);
		Render::BindGlobalTexture(kTextureUnitShadowMap, shadowTexture);

		float f = light->GetLightRange();
		float n = light->GetMinShadowDistance();
		float g = 1.0F / (n - f);

		float s = 2.0F / (float) pointShadowMapSize;
		Render::SetUniversalShaderParameter(kUniversalParamShadowSample1, s, (f + n) * (g * -0.5F) + 0.5F, f * n * g, 0.0F);
	}

	unsigned_int32 oldGraphicsState = currentGraphicsState;
	unsigned_int32 newGraphicsState = oldGraphicsState & ~(kGraphicsLightScissor | kGraphicsLightDepthBounds);

	lightRect = *lightBounds;
	if (projection == kProjectionPartial)
	{
		newGraphicsState |= kGraphicsLightScissor;
		Render::SetScissor(lightBounds->left, lightBounds->bottom, lightBounds->Width(), -lightBounds->Height());
	}

	lightDepthBounds = *depthBounds;
	if (!(newGraphicsState & kGraphicsObliqueFrustum))
	{
		Render::SetDepthBounds(depthBounds->min, depthBounds->max);
		newGraphicsState |= kGraphicsLightDepthBounds;
	}

	float r = light->GetLightRange();
	Render::SetUniversalShaderParameter(kUniversalParamLightColor, light->GetLightColor() * colorMultiplier);
	Render::SetUniversalShaderParameter(kUniversalParamLightRange, r, 0.0F, 0.0F, 1.0F / r);

	ShaderType shaderType = kShaderPointLight;
	if (light->GetLightType() == kLightCube)
	{
		shaderType = kShaderCubeLight;
		Render::BindGlobalTexture(kTextureUnitLightProjector, static_cast<const CubeLightObject *>(light)->GetProjectionMap());
	}

	currentShaderType = shaderType;
	currentGraphicsState = newGraphicsState;

	unsigned_int32 changed = (newGraphicsState ^ oldGraphicsState) & (kGraphicsLightScissor | kGraphicsLightDepthBounds);
	if (changed != 0)
	{
		if (changed & kGraphicsLightDepthBounds)
		{
			if (newGraphicsState & kGraphicsLightDepthBounds)
			{
				Render::EnableDepthBoundsTest();
			}
			else
			{
				Render::DisableDepthBoundsTest();
			}
		}

		if (changed & kGraphicsLightScissor)
		{
			if (!(newGraphicsState & kGraphicsLightScissor))
			{
				int32 left = scissorRect.left;
				int32 bottom = scissorRect.bottom;
				Render::SetScissor(left, bottom, scissorRect.right - left, scissorRect.top - bottom);
			}
		}
	}
}

void GraphicsMgr::SetSpotLight(const SpotLightObject *light, const Transformable *transformable, float colorMultiplier, ProjectionResult projection, const Rect *lightBounds, const Range<float> *depthBounds, bool shadow)
{
	lightObject = light;
	lightTransformable = transformable;
	currentShadowFlag = false;

	if (shadow)
	{
		currentShadowFlag = true;

		Render::TextureObject *shadowTexture = spotShadowFrameBuffer->GetRenderTargetTexture();
		shadowTexture->SetCompareMode(Render::kTextureCompareReference);
		Render::BindGlobalTexture(kTextureUnitShadowMap, shadowTexture);

		float s = 1.0F / (float) spotShadowMapSize;
		Render::SetUniversalShaderParameter(kUniversalParamShadowSample1, s, s, s, s);

		// Calculate projection from light space into shadow map space.
		//
		// ⎡  1                 1  ⎤⎡                               ⎤   ⎡  e             1                ⎤
		// ⎢ ───     0    0    ─── ⎥⎢  e    0      0           0    ⎥   ⎢ ───     0     ───          0    ⎥
		// ⎢  2                 2  ⎥⎢                               ⎥   ⎢  2             2                ⎥
		// ⎢                       ⎥⎢                               ⎥   ⎢                                 ⎥
		// ⎢         1          1  ⎥⎢       e                       ⎥   ⎢         e      1                ⎥
		// ⎢  0   - ───   0    ─── ⎥⎢  0   ───     0           0    ⎥   ⎢  0   - ────   ───          0    ⎥
		// ⎢         2          2  ⎥⎢       a                       ⎥ = ⎢         2a     2                ⎥
		// ⎢                       ⎥⎢                               ⎥   ⎢                                 ⎥
		// ⎢              1     1  ⎥⎢            f + n        2fn   ⎥   ⎢                f          fn    ⎥
		// ⎢  0      0   ───   ─── ⎥⎢  0    0   ───────   - ─────── ⎥   ⎢  0      0   ───────   - ─────── ⎥
		// ⎢              2     2  ⎥⎢            f - n       f - n  ⎥   ⎢              f - n       f - n  ⎥
		// ⎢                       ⎥⎢                               ⎥   ⎢                                 ⎥
		// ⎣  0      0    0     1  ⎦⎣  0    0      1           0    ⎦   ⎣  0      0      1           0    ⎦

		float f = light->GetLightRange();
		float n = light->GetMinShadowDistance();
		float e = light->GetApexTangent() * 0.5F;
		float a = light->GetAspectRatio();
		float d = f / (f - n);

		#if C4OPENGL

			shadowTransform.Set(e, 0.0F, 0.5F, 0.0F, 0.0F, -e / a, 0.5F, 0.0F, 0.0F, 0.0F, d, -d * n, 0.0F, 0.0F, 1.0F, 0.0F);

		#elif C4PS4 //[ PS4

			// -- PS4 code hidden --

		#endif //]
	}

	unsigned_int32 oldGraphicsState = currentGraphicsState;
	unsigned_int32 newGraphicsState = oldGraphicsState & ~(kGraphicsLightScissor | kGraphicsLightDepthBounds);

	lightRect = *lightBounds;
	if (projection == kProjectionPartial)
	{
		newGraphicsState |= kGraphicsLightScissor;
		Render::SetScissor(lightBounds->left, lightBounds->bottom, lightBounds->Width(), -lightBounds->Height());
	}

	lightDepthBounds = *depthBounds;
	if (!(newGraphicsState & kGraphicsObliqueFrustum))
	{
		Render::SetDepthBounds(depthBounds->min, depthBounds->max);
		newGraphicsState |= kGraphicsLightDepthBounds;
	}

	float r = light->GetLightRange();
	Render::SetUniversalShaderParameter(kUniversalParamLightColor, light->GetLightColor() * colorMultiplier);
	Render::SetUniversalShaderParameter(kUniversalParamLightRange, r, 0.0F, 0.0F, 1.0F / r);

	Render::BindGlobalTexture(kTextureUnitLightProjector, light->GetProjectionMap());

	currentShaderType = kShaderSpotLight;
	currentGraphicsState = newGraphicsState;

	unsigned_int32 changed = (newGraphicsState ^ oldGraphicsState) & (kGraphicsLightScissor | kGraphicsLightDepthBounds);
	if (changed != 0)
	{
		if (changed & kGraphicsLightDepthBounds)
		{
			if (newGraphicsState & kGraphicsLightDepthBounds)
			{
				Render::EnableDepthBoundsTest();
			}
			else
			{
				Render::DisableDepthBoundsTest();
			}
		}

		if (changed & kGraphicsLightScissor)
		{
			if (!(newGraphicsState & kGraphicsLightScissor))
			{
				int32 left = scissorRect.left;
				int32 bottom = scissorRect.bottom;
				Render::SetScissor(left, bottom, scissorRect.right - left, scissorRect.top - bottom);
			}
		}
	}
}

void GraphicsMgr::GroupSegmentArray(SegmentReference *segmentArray, SegmentReference *tempArray, int32 minIndex, int32 maxIndex)
{
	const SegmentReference& centerSegment = segmentArray[minIndex];
	machine_address center = centerSegment.groupKey;
	tempArray[minIndex] = centerSegment;

	int32 centerCount = 1;
	int32 leftCount = 0;
	int32 rightCount = 0;

	for (machine i = minIndex + 1; i <= maxIndex; i++)
	{
		const SegmentReference& segment = segmentArray[i];
		machine_address key = segment.groupKey;

		if (key == center)
		{
			tempArray[minIndex + centerCount] = segment;
			centerCount++;
		}
		else if (key < center)
		{
			segmentArray[minIndex + leftCount] = segment;
			leftCount++;
		}
		else
		{
			tempArray[maxIndex - rightCount] = segment;
			rightCount++;
		}
	}

	SegmentReference *output = &segmentArray[minIndex + leftCount];

	const SegmentReference *input = &tempArray[minIndex];
	for (machine a = 0; a < centerCount; a++)
	{
		*output = input[a];
		output++;
	}

	input = &tempArray[maxIndex];
	for (machine a = 0; a < rightCount; a++)
	{
		*output = input[-a];
		output++;
	}

	if (leftCount > 2)
	{
		GroupSegmentArray(segmentArray, tempArray, minIndex, minIndex + leftCount - 1);
	}
	else if (leftCount == 2)
	{
		SegmentReference& ref1 = segmentArray[minIndex];
		SegmentReference& ref2 = segmentArray[minIndex + 1];
		if (ref1.groupKey > ref2.groupKey)
		{
			SegmentReference x = ref1;
			ref1 = ref2;
			ref2 = x;
		}
	}

	if (rightCount > 2)
	{
		GroupSegmentArray(segmentArray, tempArray, maxIndex - rightCount + 1, maxIndex);
	}
	else if (rightCount == 2)
	{
		SegmentReference& ref1 = segmentArray[maxIndex - 1];
		SegmentReference& ref2 = segmentArray[maxIndex];
		if (ref1.groupKey > ref2.groupKey)
		{
			SegmentReference x = ref1;
			ref1 = ref2;
			ref2 = x;
		}
	}
}

void GraphicsMgr::SortSublist(List<Renderable> *list, float zmin, float zmax, List<Renderable> *finalList)
{
	float avg = (zmin + zmax) * 0.5F;

	Renderable *renderable = list->Last();
	if ((list->First() == renderable) || (!(zmax - zmin > Fabs(avg) * 0.001F)))
	{
		while (renderable)
		{
			Renderable *prev = renderable->Previous();
			finalList->Prepend(renderable);
			renderable = prev;
		}
	}
	else
	{
		List<Renderable>	farList;

		float zminFar = zmax;
		float zmaxNear = zmin;

		renderable = list->First();
		do
		{
			Renderable *next = renderable->Next();
			float z = renderable->GetTransparentDepth();
			if (z < avg)
			{
				zmaxNear = Fmax(zmaxNear, z);
			}
			else
			{
				zminFar = Fmin(zminFar, z);
				farList.Append(renderable);
			}

			renderable = next;
		} while (renderable);

		SortSublist(list, zmin, zmaxNear, finalList);
		SortSublist(&farList, zminFar, zmax, finalList);
	}
}

void GraphicsMgr::Sort(List<Renderable> *renderList)
{
	Renderable *renderable = renderList->First();
	if (renderable)
	{
		List<Renderable>	transparentList;
		List<Renderable>	attachedList;

		const Vector3D& direction = cameraTransformable->GetWorldTransform()[2];

		float zmin = K::infinity;
		float zmax = K::minus_infinity;

		do
		{
			Renderable *next = renderable->Next();

			if (renderable->GetTransparentAttachment())
			{
				attachedList.Append(renderable);
			}
			else
			{
				const Point3D *position = renderable->GetTransparentPosition();
				if (position)
				{
					float z = direction * *position;
					zmin = Fmin(zmin, z);
					zmax = Fmax(zmax, z);

					renderable->SetTransparentDepth(z);
					transparentList.Append(renderable);
				}
			}

			renderable = next;
		} while (renderable);

		if (!transparentList.Empty())
		{
			SortSublist(&transparentList, zmin, zmax, renderList);
		}

		renderable = attachedList.First();
		while (renderable)
		{
			Renderable *next = renderable->Next();

			Renderable *attachment = renderable->GetTransparentAttachment();
			if (attachment->GetOwningList() == renderList)
			{
				renderList->InsertBefore(renderable, attachment);
			}
			else
			{
				renderList->Append(renderable);
			}

			renderable = next;
		}
	}
}

void GraphicsMgr::SetModelviewMatrix(const Transform4D& matrix)
{
	Matrix4D& mvp = currentMVPMatrix;
	mvp = currentProjectionMatrix * matrix;

	Render::SetVertexShaderParameter(kVertexParamMatrixMVP, mvp);
}

void GraphicsMgr::SetGeometryModelviewMatrix(const Transform4D& matrix)
{
	Matrix4D& mvp = currentMVPMatrix;
	mvp = currentProjectionMatrix * matrix;

	Render::SetGeometryShaderParameter(kGeometryParamMatrixMVP, mvp);
}

VertexShader *GraphicsMgr::GetLocalVertexShader(const VertexSnippet *snippet)
{
	VertexAssembly assembly(ShaderAttribute::signatureStorage);
	assembly.AddSnippet(snippet);
	return (VertexShader::Get(&assembly));
}

VertexShader *GraphicsMgr::GetLocalVertexShader(int32 snippetCount, const VertexSnippet *const *snippet)
{
	VertexAssembly assembly(ShaderAttribute::signatureStorage);
	for (machine a = 0; a < snippetCount; a++)
	{
		assembly.AddSnippet(snippet[a]);
	}

	return (VertexShader::Get(&assembly));
}

FragmentShader *GraphicsMgr::GetLocalFragmentShader(int32 shaderIndex)
{
	static const unsigned_int32 shaderSignature[kLocalFragmentShaderCount][2] =
	{
		{1, '%CPZ'}, {1, '%CPC'}
	};

	const unsigned_int32 *signature = shaderSignature[shaderIndex];
	FragmentShader *fragmentShader = FragmentShader::Find(signature);
	if (!fragmentShader)
	{
		static const char *const shaderSource[kLocalFragmentShaderCount] =
		{
			FragmentShader::copyZero, FragmentShader::copyConstant
		};

		const char *source = shaderSource[shaderIndex];
		fragmentShader = FragmentShader::New(source, Text::GetTextLength(source), signature);
	}

	return (fragmentShader);
}

GeometryShader *GraphicsMgr::GetLocalGeometryShader(int32 shaderIndex, int32 *vertexSnippetCount, const VertexSnippet *const **vertexSnippet)
{
	static const GeometryAssembly *geometryAssembly[kLocalGeometryShaderCount] =
	{
		&GeometryShader::geometryAssembly[kGeometryShaderExtrudeNormalLine]
	};

	const GeometryAssembly *assembly = geometryAssembly[shaderIndex];
	GeometryShader *geometryShader = GeometryShader::Find(assembly->signature);
	if (!geometryShader)
	{
		const char *source = assembly->shaderSource;
		geometryShader = GeometryShader::New(source, Text::GetTextLength(source), assembly->signature);
	}

	*vertexSnippetCount = assembly->vertexSnippetCount;
	*vertexSnippet = assembly->vertexSnippet;
	return (geometryShader);
}

void GraphicsMgr::SetBlendState(unsigned_int32 newBlendState)
{
	unsigned_int32 oldBlendState = currentBlendState;
	if (newBlendState != oldBlendState)
	{
		static const unsigned_int32 blendFactor[kBlendFactorCount] =
		{
			Render::kBlendZero, Render::kBlendOne,
			Render::kBlendSrcColor, Render::kBlendDstColor, Render::kBlendConstColor,
			Render::kBlendSrcAlpha, Render::kBlendDstAlpha, Render::kBlendConstAlpha,
			Render::kBlendInvSrcColor, Render::kBlendInvDstColor, Render::kBlendInvConstColor,
			Render::kBlendInvSrcAlpha, Render::kBlendInvDstAlpha, Render::kBlendInvConstAlpha
		};

		currentBlendState = newBlendState;

		unsigned_int32 sourceRGB = blendFactor[GetBlendSource(newBlendState)];
		unsigned_int32 destRGB = blendFactor[GetBlendDest(newBlendState)];
		unsigned_int32 sourceAlpha = blendFactor[GetBlendSourceAlpha(newBlendState)];
		unsigned_int32 destAlpha = blendFactor[GetBlendDestAlpha(newBlendState)];

		Render::SetBlendFunc(sourceRGB, destRGB, sourceAlpha, destAlpha);
	}
}

void GraphicsMgr::SetRenderState(unsigned_int32 newRenderState)
{
	unsigned_int32 oldRenderState = currentRenderState;
	unsigned_int32 changed = newRenderState ^ oldRenderState;
	if (changed != 0)
	{
		currentRenderState = newRenderState;

		if (changed & kRenderDepthTest)
		{
			if (newRenderState & kRenderDepthTest)
			{
				Render::EnableDepthTest();
			}
			else
			{
				Render::DisableDepthTest();
			}
		}

		if (changed & kRenderColorInhibit)
		{
			if (newRenderState & kRenderColorInhibit)
			{
				Render::SetColorMask(false, false, false, false);
			}
			else
			{
				Render::SetColorMask(true, true, true, true);
			}
		}

		if (changed & kRenderDepthInhibit)
		{
			if (newRenderState & kRenderDepthInhibit)
			{
				Render::SetDepthMask(false);
			}
			else
			{
				Render::SetDepthMask(true);
			}
		}

		if ((changed & kRenderDepthOffset) && (!(newRenderState & kRenderDepthOffset)))
		{
			currentProjectionMatrix = cameraProjectionMatrix;
		}

		if (changed & kRenderPolygonOffset)
		{
			if (newRenderState & kRenderPolygonOffset)
			{
				Render::EnablePolygonFillOffset();
				Render::SetPolygonOffset(0.0F, -1.0F, 0.0F);
			}
			else
			{
				Render::DisablePolygonFillOffset();
			}
		}

		if (changed & kRenderWireframe)
		{
			if (newRenderState & kRenderWireframe)
			{
				Render::SetLinePolygonMode();
			}
			else
			{
				Render::SetFillPolygonMode();
			}
		}
	}
}

void GraphicsMgr::SetMaterialState(unsigned_int32 newMaterialState)
{
	newMaterialState &= kMaterialShaderStateMask;
	unsigned_int32 oldMaterialState = currentMaterialState;
	unsigned_int32 changed = newMaterialState ^ oldMaterialState;
	if (changed != 0)
	{
		currentMaterialState = newMaterialState;

		if (changed & kMaterialTwoSided)
		{
			if (newMaterialState & kMaterialTwoSided)
			{
				Render::DisableCullFace();
			}
			else
			{
				Render::EnableCullFace();
			}
		}

		if (changed & kMaterialAlphaCoverage)
		{
			if (newMaterialState & kMaterialAlphaCoverage)
			{
				Render::EnableAlphaCoverage();
			}
			else
			{
				Render::DisableAlphaCoverage();
			}
		}

		if (changed & kMaterialSampleShading)
		{
			if (newMaterialState & kMaterialSampleShading)
			{
				Render::EnableSampleShading();
			}
			else
			{
				Render::DisableSampleShading();
			}
		}
	}

	if (newMaterialState & (kMaterialEmissionGlow | kMaterialSpecularBloom))
	{
		currentGraphicsState |= kGraphicsGlowBloomAvail;
	}
}

void GraphicsMgr::SetVertexData(VertexData *vertexData, const Renderable *renderable)
{
	if (!vertexData->GetValidFlag())
	{
		vertexData->Update(renderable);
	}

	vertexData->Bind();
}

template <class container> void GraphicsMgr::DrawContainer(const container *renderList, bool group)
{
	if (renderList->Empty())
	{
		return;
	}

	unsigned_int32 graphicsState = currentGraphicsState;
	unsigned_int32 extraState = 0;

	if (currentShaderType >= kShaderFirstLight)
	{
		extraState = kRenderDepthInhibit;

		if (graphicsState & kGraphicsDepthTestLess)
		{
			currentGraphicsState = graphicsState & ~kGraphicsDepthTestLess;
			Render::SetDepthFunc(Render::kDepthLessEqual);
		}
	}
	else
	{
		if (!(graphicsState & kGraphicsAmbientLessEqual))
		{
			if (!(graphicsState & kGraphicsDepthTestLess))
			{
				currentGraphicsState = graphicsState | kGraphicsDepthTestLess;
				Render::SetDepthFunc(Render::kDepthLess);
			}
		}
		else if (graphicsState & kGraphicsDepthTestLess)
		{
			currentGraphicsState = graphicsState & ~kGraphicsDepthTestLess;
			Render::SetDepthFunc(Render::kDepthLessEqual);
		}
	}

	for (Renderable *renderable : *renderList)
	{
		ShaderType shaderType = currentShaderType;
		if (shaderType <= kShaderUnified)
		{
			shaderType = static_cast<ShaderType>(shaderType + renderable->GetAmbientEnvironment()->ambientShaderTypeDelta);
		}

		unsigned_int32 renderableFlags = renderable->GetRenderableFlags();
		ShaderVariant variant = (renderableFlags & (kRenderableFogInhibit | kRenderableUnfog)) ? kShaderVariantNormal : currentShaderVariant;
		ShaderKey shaderKey(variant, renderable->GetShaderDetailLevel(), currentShadowFlag);

		RenderSegment *segment = renderable->GetFirstRenderSegment();
		do
		{
			ShaderData *shaderData = segment->GetShaderData(shaderType, shaderKey);
			if (!shaderData)
			{
				shaderData = segment->InitShaderData(renderable, shaderType, shaderKey);
			}

			const ShaderProgram *program = shaderData->shaderProgram;
			if (program)
			{
				segment->currentShaderData = shaderData;
				segmentArray[0].AddElement(SegmentReference{segment, GetPointerAddress(program)});
			}

		} while ((segment = segment->GetNextRenderSegment()) != nullptr);
	}

	int32 segmentCount = segmentArray[0].GetElementCount();
	if (segmentCount == 0)
	{
		return;
	}

	if (group)
	{
		segmentArray[1].SetElementCount(segmentCount);
		GroupSegmentArray(segmentArray[0], segmentArray[1], 0, segmentCount - 1);
	}

	const Renderable *prevRenderable = nullptr;
	for (const SegmentReference& reference : segmentArray[0])
	{
		const RenderSegment *segment = reference.segment;

		Renderable *renderable = segment->owningRenderable;
		if (renderable != prevRenderable)
		{
			prevRenderable = renderable;

			unsigned_int32 newRenderState = renderable->GetRenderState() & ~disabledRenderState;
			SetRenderState(newRenderState | extraState);

			unsigned_int32 renderableFlags = renderable->GetRenderableFlags();
			const Transformable *transformable = renderable->GetTransformable();
			if (!(renderableFlags & kRenderableCameraTransformInhibit))
			{
				if (newRenderState & kRenderDepthOffset)
				{
					float z = Fmin(cameraSpaceTransform.GetRow(2) ^ renderable->GetDepthOffsetPoint(), -cameraObject->GetNearDepth());
					float delta = renderable->GetDepthOffsetDelta();
					float epsilon = depthOffsetConstant * delta / (z * (z + delta));
					epsilon = Fmax(Fabs(epsilon), 4.8e-7F) * NonzeroFsgn(epsilon);

					currentProjectionMatrix = cameraProjectionMatrix;
					currentProjectionMatrix(2,2) *= 1.0F + epsilon;
				}

				if (transformable)
				{
					SetModelviewMatrix(cameraSpaceTransform * transformable->GetWorldTransform());
				}
				else
				{
					SetModelviewMatrix(cameraSpaceTransform);
				}
			}
			else
			{
				if (transformable)
				{
					SetModelviewMatrix(transformable->GetWorldTransform());
				}
				else
				{
					SetModelviewMatrix(Identity4D);
				}
			}
		}

		ShaderData *shaderData = segment->currentShaderData;
		Render::SetShaderProgram(shaderData->shaderProgram);

		SetBlendState(shaderData->blendState);
		SetMaterialState(shaderData->materialState);
		SetVertexData(&shaderData->vertexData, renderable);

		int32 stateProcCount = shaderData->shaderStateDataCount;
		for (machine a = 0; a < stateProcCount; a++)
		{
			(*shaderData->shaderStateData[a].stateProc)(renderable, shaderData->shaderStateData[a].stateCookie);
		}

		Render::BindTextureArray(&shaderData->textureArray);

		int32 vertexCount = renderable->GetVertexCount();

		#if C4STATS

			graphicsCounter[kGraphicsCounterDirectVertices] += vertexCount;
			graphicsCounter[kGraphicsCounterDirectCommands]++;

		#endif

		switch (renderable->GetRenderType())
		{
			case kRenderPointSprites:

				Render::EnablePointSprite();
				Render::DrawPrimitives(Render::kPrimitivePoints, 0, vertexCount);
				Render::DisablePointSprite();
				break;

			case kRenderPoints:

				Render::DrawPrimitives(Render::kPrimitivePoints, 0, vertexCount);
				break;

			case kRenderLines:

				Render::DrawPrimitives(Render::kPrimitiveLines, 0, vertexCount);
				break;

			case kRenderLineStrip:

				Render::DrawPrimitives(Render::kPrimitiveLineStrip, 0, vertexCount);
				break;

			case kRenderLineLoop:

				Render::DrawPrimitives(Render::kPrimitiveLineLoop, 0, vertexCount);
				break;

			case kRenderIndexedLines:

				Render::DrawIndexedPrimitives(Render::kPrimitiveLines, segment->GetPrimitiveCount() * 2, renderable->GetPrimitiveIndexOffset() + segment->GetPrimitiveStart() * sizeof(Line));
				break;

			case kRenderTriangles:

				#if C4STATS

					graphicsCounter[kGraphicsCounterDirectPrimitives] += vertexCount / 3;

				#endif

				Render::DrawPrimitives(Render::kPrimitiveTriangles, 0, vertexCount);
				break;

			case kRenderTriangleStrip:

				#if C4STATS

					graphicsCounter[kGraphicsCounterDirectPrimitives] += vertexCount - 2;

				#endif

				Render::DrawPrimitives(Render::kPrimitiveTriangleStrip, 0, vertexCount);
				break;

			case kRenderIndexedTriangles:
			{
				int32 triangleCount = segment->GetPrimitiveCount();

				#if C4STATS

					graphicsCounter[kGraphicsCounterDirectPrimitives] += triangleCount;

				#endif

				Render::DrawIndexedPrimitives(Render::kPrimitiveTriangles, triangleCount * 3, renderable->GetPrimitiveIndexOffset() + segment->GetPrimitiveStart() * sizeof(Triangle));
				break;
			}

			case kRenderQuads:

				#if C4STATS

					graphicsCounter[kGraphicsCounterDirectPrimitives] += vertexCount >> 2;

				#endif

				Render::DrawQuads(0, vertexCount);
				break;

			case kRenderMultiIndexedTriangles:

				Render::MultiDrawIndexedPrimitives(Render::kPrimitiveTriangles, segment->GetMultiCountArray(), segment->GetMultiOffsetArray(), segment->GetMultiRenderCount());
				break;

			case kRenderMaskedMultiIndexedTriangles:
			{
				static machine_address	index[33];
				static unsigned_int32	count[33];

				unsigned_int32 size = 0;
				unsigned_int32 offset = renderable->GetPrimitiveIndexOffset();

				int32 triangleCount = segment->GetPrimitiveCount();
				if (triangleCount != 0)
				{
					#if C4STATS

						graphicsCounter[kGraphicsCounterDirectPrimitives] += triangleCount;

					#endif

					index[0] = offset;
					count[0] = triangleCount * 3;
					size = 1;
				}

				unsigned_int32 mask = segment->GetMultiRenderMask();
				const int32 *data = segment->GetMultiRenderData();
				while (mask != 0)
				{
					if (mask & 1)
					{
						triangleCount = data[1];

						#if C4STATS

							graphicsCounter[kGraphicsCounterDirectPrimitives] += triangleCount;

						#endif

						index[size] = offset + data[0] * sizeof(Triangle);
						count[size] = triangleCount * 3;
						size++;
					}

					mask >>= 1;
					data += 2;
				}

				Render::MultiDrawIndexedPrimitives(Render::kPrimitiveTriangles, count, index, size);
				break;
			}
		}

		OcclusionQuery *query = currentOcclusionQuery;
		if (query)
		{
			currentOcclusionQuery = nullptr;
			occlusionQueryList.Append(query);
			query->EndOcclusionQuery();
		}
	}

	segmentArray[0].Clear();

	#if C4DIAGS

		if ((diagnosticFlags & (kDiagnosticWireframe | kDiagnosticNormals | kDiagnosticTangents)) && (extraState == 0))
		{
			if (diagnosticFlags & kDiagnosticWireframe)
			{
				DrawWireframeContainer((diagnosticFlags & kDiagnosticDepthTest) ? kWireframeDepthTest : 0, renderList);
			}

			if (diagnosticFlags & kDiagnosticNormals)
			{
				DrawVectorsContainer(kArrayNormal, renderList);
			}

			if (diagnosticFlags & kDiagnosticTangents)
			{
				DrawVectorsContainer(kArrayTangent, renderList);
			}
		}

	#endif
}

template void GraphicsMgr::DrawContainer(const List<Renderable> *, bool);
template void GraphicsMgr::DrawContainer(const ImmutableArray<Renderable *> *, bool);

bool GraphicsMgr::BeginStructureRendering(const Transform4D& previousCameraWorldTransform, unsigned_int32 structureFlags, float velocityScale)
{
	static const float structureClearColor[4] = {0.0F, 0.0F, 65504.0F, 65504.0F};

	unsigned_int32 activeFlags = graphicsActiveFlags;
	if ((!(activeFlags & kGraphicsActiveStructureRendering)) || (currentRenderTargetType != kRenderTargetPrimary))
	{
		return (false);
	}

	if (activeFlags & kGraphicsActiveTimer)
	{
		Timestamp(kTimestampBeginStructure);
	}

	if (!(activeFlags & kGraphicsActiveVelocityRendering))
	{
		structureFlags &= ~kStructureRenderVelocity;
	}

	previousCameraSpaceTransform(3,0) = previousCameraSpaceTransform(3,1) = previousCameraSpaceTransform(3,2) = 0.0F;
	previousCameraSpaceTransform(3,3) = 1.0F;

	const Vector3D& rightDirection = previousCameraWorldTransform[0];
	const Vector3D& downDirection = previousCameraWorldTransform[1];
	const Vector3D& viewDirection = previousCameraWorldTransform[2];

	previousCameraSpaceTransform(0,0) = rightDirection.x;
	previousCameraSpaceTransform(0,1) = rightDirection.y;
	previousCameraSpaceTransform(0,2) = rightDirection.z;
	previousCameraSpaceTransform(1,0) = -downDirection.x;
	previousCameraSpaceTransform(1,1) = -downDirection.y;
	previousCameraSpaceTransform(1,2) = -downDirection.z;
	previousCameraSpaceTransform(2,0) = -viewDirection.x;
	previousCameraSpaceTransform(2,1) = -viewDirection.y;
	previousCameraSpaceTransform(2,2) = -viewDirection.z;

	const Vector3D& previousCameraWorldOffset = previousCameraWorldTransform[3];
	previousCameraSpaceTransform(0,3) = -(previousCameraSpaceTransform.GetRow(0) ^ previousCameraWorldOffset);
	previousCameraSpaceTransform(1,3) = -(previousCameraSpaceTransform.GetRow(1) ^ previousCameraWorldOffset);
	previousCameraSpaceTransform(2,3) = -(previousCameraSpaceTransform.GetRow(2) ^ previousCameraWorldOffset);

	float velocityClearColor[4] = {0.0F, 0.0F, 0.0F, 0.0F};
	unsigned_int32 graphicsState = currentGraphicsState;

	if ((structureFlags & (kStructureZeroBackgroundVelocity | kStructureRenderVelocity)) == kStructureRenderVelocity)
	{
		graphicsState |= kGraphicsMotionBlurAvail;

		Vector4D v = currentProjectionMatrix * (cameraSpaceTransform * viewDirection);
		float w = (float) (viewportRect.right - viewportRect.left) * 0.5F;
		float h = (float) (viewportRect.top - viewportRect.bottom) * 0.5F;
		float f = kVelocityMultiplier / v.w;
		float red = v.x * f * w;
		float green = v.y * f * h;

		float m = 1.0F / Fmax(Fabs(red), Fabs(green), 1.0F);
		velocityClearColor[0] = Clamp(red * m, -1.0F, 1.0F);
		velocityClearColor[1] = Clamp(green * m, -1.0F, 1.0F);
	}

	normalFrameBuffer->GetRenderTargetTexture(kRenderTargetStructure)->UnbindAll();
	SetRenderTarget(kRenderTargetStructure);

	if (graphicsState & kGraphicsMotionBlurAvail)
	{
		Render::TextureObject *velocityTexture = normalFrameBuffer->GetRenderTargetTexture(kRenderTargetVelocity);
		velocityTexture->UnbindAll();

		Render::SetFrameBuffer(normalFrameBuffer);
		normalFrameBuffer->SetVelocityRenderTexture(velocityTexture);
	}

	Render::DisableBlend();

	if (multisampleFrameBuffer)
	{
		SetRenderState(kRenderDepthTest);

		if (structureFlags & kStructureClearBuffer)
		{
			if (graphicsState & kGraphicsMotionBlurAvail)
			{
				Render::ClearColorVelocityDepthStencilBuffers(structureClearColor, velocityClearColor);
			}
			else
			{
				Render::ClearColorDepthStencilBuffers(structureClearColor);
			}
		}
		else
		{
			Render::ClearDepthStencilBuffers();
		}
	}
	else
	{
		if (structureFlags & kStructureClearBuffer)
		{
			SetRenderState(kRenderDepthTest);

			if (graphicsState & kGraphicsMotionBlurAvail)
			{
				Render::ClearColorVelocityBuffers(structureClearColor, velocityClearColor);
			}
			else
			{
				Render::ClearColorBuffer(structureClearColor);
			}
		}

		graphicsState |= kGraphicsAmbientLessEqual;
	}

	if (!(graphicsState & kGraphicsDepthTestLess))
	{
		graphicsState |= kGraphicsDepthTestLess;
		Render::SetDepthFunc(Render::kDepthLess);
	}

	currentGraphicsState = graphicsState;
	currentStructureFlags = structureFlags;

	float scale = velocityScale * kVelocityMultiplier;

	#if C4OPENGL

		Render::SetUniversalShaderParameter(kUniversalParamVelocityScale, scale, scale, 0.0F, 0.0F);

	#elif C4CONSOLE //[ CONSOLE

		// -- Console code hidden --

	#endif //]

	MemoryMgr::ClearMemory(motionGridFlag, kProcessGridWidth * kProcessGridHeight);
	return (true);
}

void GraphicsMgr::EndStructureRendering(void)
{
	if (graphicsActiveFlags & kGraphicsActiveTimer)
	{
		Timestamp(kTimestampEndStructure);
	}

	if (currentGraphicsState & kGraphicsMotionBlurAvail)
	{
		normalFrameBuffer->ResetVelocityRenderTexture();
	}

	if (graphicsActiveFlags & kGraphicsActiveAmbientOcclusion)
	{
		static Link<ShaderProgram>		occlusionShaderProgram;
		static Link<ShaderProgram>		occlusionBlurShaderProgram;

		if (graphicsActiveFlags & kGraphicsActiveTimer)
		{
			Timestamp(kTimestampBeginOcclusion);
		}

		normalFrameBuffer->GetRenderTargetTexture(kRenderTargetOcclusion1)->UnbindAll();
		SetRenderTarget(kRenderTargetOcclusion1);

		SetRenderState(0);
		SetMaterialState(currentMaterialState & kMaterialTwoSided);

		const Render::TextureObject *structureTexture = normalFrameBuffer->GetRenderTargetTexture(kRenderTargetStructure);
		Render::BindGlobalTexture(kTextureUnitStructure, structureTexture);
		Render::BindGlobalTexture(kTextureUnitNoise, OcclusionPostProcess::GetVectorNoiseTexture());

		VertexShader *vertexShader = nullptr;
		ShaderProgram *shaderProgram = occlusionShaderProgram;
		if (!shaderProgram)
		{
			ShaderGraph		shaderGraph;

			shaderGraph.AddElement(new OcclusionPostProcess);

			vertexShader = GetLocalVertexShader(&VertexShader::nullTransform);
			shaderProgram = ShaderAttribute::CompilePostShader(&shaderGraph, vertexShader);
			occlusionShaderProgram = shaderProgram;
			vertexShader->Release();
		}

		Render::SetShaderProgram(shaderProgram);
		Render::SetFragmentShaderParameter(0, occlusionPlaneScale, 2.0F, 0.1F, 0.0F);

		fullscreenVertexData->Bind();
		Render::DrawPrimitives(Render::kPrimitiveTriangles, 0, 3);

		normalFrameBuffer->GetRenderTargetTexture(kRenderTargetOcclusion2)->UnbindAll();
		SetRenderTarget(kRenderTargetOcclusion2);

		const Render::TextureObject *occlusionTexture = normalFrameBuffer->GetRenderTargetTexture(kRenderTargetOcclusion1);
		Render::BindGlobalTexture(kTextureUnitOcclusion, occlusionTexture);

		shaderProgram = occlusionBlurShaderProgram;
		if (!shaderProgram)
		{
			ShaderGraph		shaderGraph;

			shaderGraph.AddElement(new OcclusionBlurPostProcess);
			shaderProgram = ShaderAttribute::CompilePostShader(&shaderGraph, vertexShader);
			occlusionBlurShaderProgram = shaderProgram;
		}

		Render::SetShaderProgram(shaderProgram);
		Render::DrawPrimitives(Render::kPrimitiveTriangles, 0, 3);

		occlusionTexture->Unbind(kTextureUnitOcclusion);
		structureTexture->Unbind(kTextureUnitStructure);

		#if C4PS3 //[ PS3

			// -- PS3 code hidden --

		#endif //]

		if (graphicsActiveFlags & kGraphicsActiveTimer)
		{
			Timestamp(kTimestampEndOcclusion);
		}
	}

	Render::EnableBlend();
	SetRenderTarget(kRenderTargetPrimary);
}

template <class container> void GraphicsMgr::DrawStructureContainer(const container *renderList, bool group)
{
	if (!(currentStructureFlags & kStructureRenderVelocity))
	{
		DrawStructureDepthContainer(renderList, group);
		return;
	}

	Transform4D		mv;
	Transform4D		*modelview;

	for (Renderable *renderable : *renderList)
	{
		if (!(renderable->GetRenderableFlags() & kRenderableStructureBufferInhibit))
		{
			ShaderKey shaderKey(kShaderVariantNormal, renderable->GetShaderDetailLevel());

			RenderSegment *segment = renderable->GetFirstRenderSegment();
			do
			{
				ShaderData *shaderData = segment->GetShaderData(kShaderStructure, shaderKey);
				if (!shaderData)
				{
					shaderData = segment->InitShaderData(renderable, kShaderStructure, shaderKey);
				}

				const ShaderProgram *program = shaderData->shaderProgram;
				if (program)
				{
					segment->currentShaderData = shaderData;
					segmentArray[0].AddElement(SegmentReference{segment, GetPointerAddress(program)});
				}

			} while ((segment = segment->GetNextRenderSegment()) != nullptr);
		}
	}

	int32 segmentCount = segmentArray[0].GetElementCount();
	if (segmentCount == 0)
	{
		return;
	}

	if (group)
	{
		segmentArray[1].SetElementCount(segmentCount);
		GroupSegmentArray(segmentArray[0], segmentArray[1], 0, segmentCount - 1);
	}

	const Renderable *prevRenderable = nullptr;
	for (const SegmentReference& reference : segmentArray[0])
	{
		const RenderSegment *segment = reference.segment;

		Renderable *renderable = segment->owningRenderable;
		if (renderable != prevRenderable)
		{
			Matrix4D		mvp1;

			prevRenderable = renderable;

			const Transformable *transformable = renderable->GetTransformable();
			if (transformable)
			{
				mv = cameraSpaceTransform * transformable->GetWorldTransform();
				modelview = &mv;
			}
			else
			{
				modelview = &cameraSpaceTransform;
			}

			if (renderable->GetRenderableFlags() & kRenderableMotionBlurGradient)
			{
				static const int8 edgeVertexIndex[24] =
				{
					0, 1, 1, 2, 2, 3, 3, 0, 4, 5, 5, 6, 6, 7, 7, 4, 0, 4, 1, 5, 2, 6, 3, 7
				};

				Vector4D	vertex[8];
				float 		xmin, xmax, ymin, ymax;

				Matrix4D transform = standardProjectionMatrix * *modelview;
				const Box3D *box = renderable->GetMotionBlurBox();

				vertex[0] = transform * Point3D(box->min.x, box->min.y, box->min.z);
				vertex[1] = transform * Point3D(box->max.x, box->min.y, box->min.z);
				vertex[2] = transform * Point3D(box->max.x, box->max.y, box->min.z);
				vertex[3] = transform * Point3D(box->min.x, box->max.y, box->min.z);
				vertex[4] = transform * Point3D(box->min.x, box->min.y, box->max.z);
				vertex[5] = transform * Point3D(box->max.x, box->min.y, box->max.z);
				vertex[6] = transform * Point3D(box->max.x, box->max.y, box->max.z);
				vertex[7] = transform * Point3D(box->min.x, box->max.y, box->max.z);

				bool visible = false;
				const int8 *edge = edgeVertexIndex;
				for (machine a = 0; a < 12; a++, edge += 2)
				{
					Vector4D p1 = vertex[edge[0]];
					Vector4D p2 = vertex[edge[1]];

					if (p1.z < -p1.w)
					{
						if (p2.z < -p2.w)
						{
							continue;
						}

						Vector4D dp = p1 - p2;
						p1 -= dp * ((p1.z + p1.w) / (dp.w + dp.z));
					}
					else if (p2.z < -p2.w)
					{
						Vector4D dp = p2 - p1;
						p2 -= dp * ((p2.z + p2.w) / (dp.w + dp.z));
					}

					float f1 = 1.0F / p1.w;
					float f2 = 1.0F / p2.w;
					float x1 = p1.x * f1;
					float x2 = p2.x * f2;
					float y1 = p1.y * f1;
					float y2 = p2.y * f2;

					if (!visible)
					{
						visible = true;

						if (x1 < x2)
						{
							xmin = x1;
							xmax = x2;
						}
						else
						{
							xmin = x2;
							xmax = x1;
						}

						if (y1 < y2)
						{
							ymin = y1;
							ymax = y2;
						}
						else
						{
							ymin = y2;
							ymax = y1;
						}
					}
					else
					{
						if (x1 < xmin)
						{
							xmin = x1;
						}
						else if (x1 > xmax)
						{
							xmax = x1;
						}

						if (x2 < xmin)
						{
							xmin = x2;
						}
						else if (x2 > xmax)
						{
							xmax = x2;
						}

						if (y1 < ymin)
						{
							ymin = y1;
						}
						else if (y1 > ymax)
						{
							ymax = y1;
						}

						if (y2 < ymin)
						{
							ymin = y2;
						}
						else if (y2 > ymax)
						{
							ymax = y2;
						}
					}
				}

				if (!visible)
				{
					continue;
				}

				int32 left = MaxZero((int32) ((xmin * 0.5F + motionBlurBoxLeftOffset) * (float) kProcessGridWidth));
				int32 right = Min((int32) ((xmax * 0.5F + motionBlurBoxRightOffset) * (float) kProcessGridWidth), kProcessGridWidth - 1);
				int32 bottom = MaxZero((int32) ((ymin * 0.5F + motionBlurBoxBottomOffset) * (float) kProcessGridHeight));
				int32 top = Min((int32) ((ymax * 0.5F + motionBlurBoxTopOffset) * (float) kProcessGridHeight), kProcessGridHeight - 1);

				for (machine j = bottom; j <= top; j++)
				{
					machine k = j * kProcessGridWidth;
					for (machine i = left; i <= right; i++)
					{
						motionGridFlag[k + i] = true;
					}
				}
			}

			unsigned_int32 newRenderState = kRenderDepthTest | (renderable->GetRenderState() & (kRenderDepthInhibit | kRenderDepthOffset));
			SetRenderState(newRenderState);

			if (newRenderState & kRenderDepthOffset)
			{
				float z = Fmin(cameraSpaceTransform.GetRow(2) ^ renderable->GetDepthOffsetPoint(), -cameraObject->GetNearDepth());
				float delta = renderable->GetDepthOffsetDelta();
				float epsilon = depthOffsetConstant * delta / (z * (z + delta));
				epsilon = Fmax(Fabs(epsilon), 4.8e-7F) * NonzeroFsgn(epsilon);

				currentProjectionMatrix = cameraProjectionMatrix;
				currentProjectionMatrix(2,2) *= 1.0F + epsilon;
			}

			SetModelviewMatrix(*modelview);

			if (transformable)
			{
				const Transform4D *previousWorldTransform = renderable->GetPreviousWorldTransformPointer();
				if (previousWorldTransform)
				{
					mvp1 = currentProjectionMatrix * (previousCameraSpaceTransform * *previousWorldTransform);
				}
				else
				{
					mvp1 = currentProjectionMatrix * (previousCameraSpaceTransform * transformable->GetWorldTransform());
				}
			}
			else
			{
				mvp1 = currentProjectionMatrix * previousCameraSpaceTransform;
			}

			float l = (float) viewportRect.left;
			float b = (float) viewportRect.bottom;
			float w = ((float) viewportRect.right - l) * 0.5F;
			float h = ((float) viewportRect.top - b) * 0.5F;
			l += w;
			b += h;

			Render::SetVertexShaderParameter(kVertexParamMatrixVelocityA, mvp1(0,0) * w + mvp1(3,0) * l, mvp1(0,1) * w + mvp1(3,1) * l, mvp1(0,2) * w + mvp1(3,2) * l, mvp1(0,3) * w + mvp1(3,3) * l);
			Render::SetVertexShaderParameter(kVertexParamMatrixVelocityA + 1, mvp1(1,0) * h + mvp1(3,0) * b, mvp1(1,1) * h + mvp1(3,1) * b, mvp1(1,2) * h + mvp1(3,2) * b, mvp1(1,3) * h + mvp1(3,3) * b);
			Render::SetVertexShaderParameter(kVertexParamMatrixVelocityA + 2, (mvp1(2,0) + mvp1(3,0)) * 0.5F, (mvp1(2,1) + mvp1(3,1)) * 0.5F, (mvp1(2,2) + mvp1(3,2)) * 0.5F, (mvp1(2,3) + mvp1(3,3)) * 0.5F);
			Render::SetVertexShaderParameter(kVertexParamMatrixVelocityA + 3, mvp1(3,0), mvp1(3,1), mvp1(3,2), mvp1(3,3));

			const Matrix4D& mvp2 = currentMVPMatrix;
			Render::SetVertexShaderParameter(kVertexParamMatrixVelocityB, mvp2(0,0) * w + mvp2(3,0) * l, mvp2(0,1) * w + mvp2(3,1) * l, mvp2(0,2) * w + mvp2(3,2) * l, mvp2(0,3) * w + mvp2(3,3) * l);
			Render::SetVertexShaderParameter(kVertexParamMatrixVelocityB + 1, mvp2(1,0) * h + mvp2(3,0) * b, mvp2(1,1) * h + mvp2(3,1) * b, mvp2(1,2) * h + mvp2(3,2) * b, mvp2(1,3) * h + mvp2(3,3) * b);
			Render::SetVertexShaderParameter(kVertexParamMatrixVelocityB + 2, (mvp2(2,0) + mvp2(3,0)) * 0.5F, (mvp2(2,1) + mvp2(3,1)) * 0.5F, (mvp2(2,2) + mvp2(3,2)) * 0.5F, (mvp2(2,3) + mvp2(3,3)) * 0.5F);
			Render::SetVertexShaderParameter(kVertexParamMatrixVelocityB + 3, mvp2(3,0), mvp2(3,1), mvp2(3,2), mvp2(3,3));
		}

		ShaderData *shaderData = segment->currentShaderData;

		Render::SetShaderProgram(shaderData->shaderProgram);
		SetMaterialState(shaderData->materialState);
		SetVertexData(&shaderData->vertexData, renderable);

		int32 stateProcCount = shaderData->shaderStateDataCount;
		for (machine a = 0; a < stateProcCount; a++)
		{
			(*shaderData->shaderStateData[a].stateProc)(renderable, shaderData->shaderStateData[a].stateCookie);
		}

		Render::BindTextureArray(&shaderData->textureArray);

		int32 vertexCount = renderable->GetVertexCount();

		#if C4STATS

			graphicsCounter[kGraphicsCounterStructureVertices] += vertexCount;
			graphicsCounter[kGraphicsCounterStructureCommands]++;

		#endif

		switch (renderable->GetRenderType())
		{
			case kRenderTriangles:

				#if C4STATS

					graphicsCounter[kGraphicsCounterStructurePrimitives] += vertexCount / 3;

				#endif

				Render::DrawPrimitives(Render::kPrimitiveTriangles, 0, vertexCount);
				break;

			case kRenderTriangleStrip:

				#if C4STATS

					graphicsCounter[kGraphicsCounterStructurePrimitives] += vertexCount - 2;

				#endif

				Render::DrawPrimitives(Render::kPrimitiveTriangleStrip, 0, vertexCount);
				break;

			case kRenderIndexedTriangles:
			{
				int32 triangleCount = segment->GetPrimitiveCount();

				#if C4STATS

					graphicsCounter[kGraphicsCounterStructurePrimitives] += triangleCount;

				#endif

				Render::DrawIndexedPrimitives(Render::kPrimitiveTriangles, triangleCount * 3, renderable->GetPrimitiveIndexOffset() + segment->GetPrimitiveStart() * sizeof(Triangle));
				break;
			}

			case kRenderQuads:

				#if C4STATS

					graphicsCounter[kGraphicsCounterStructurePrimitives] += vertexCount >> 2;

				#endif

				Render::DrawQuads(0, vertexCount);
				break;

			case kRenderMultiIndexedTriangles:

				Render::MultiDrawIndexedPrimitives(Render::kPrimitiveTriangles, segment->GetMultiCountArray(), segment->GetMultiOffsetArray(), segment->GetMultiRenderCount());
				break;

			case kRenderMaskedMultiIndexedTriangles:
			{
				static machine_address	index[33];
				static unsigned_int32	count[33];

				unsigned_int32 size = 0;
				unsigned_int32 offset = renderable->GetPrimitiveIndexOffset();

				int32 triangleCount = segment->GetPrimitiveCount();
				if (triangleCount != 0)
				{
					#if C4STATS

						graphicsCounter[kGraphicsCounterStructurePrimitives] += triangleCount;

					#endif

					index[0] = offset;
					count[0] = triangleCount * 3;
					size = 1;
				}

				unsigned_int32 mask = segment->GetMultiRenderMask();
				const int32 *data = segment->GetMultiRenderData();
				while (mask != 0)
				{
					if (mask & 1)
					{
						triangleCount = data[1];

						#if C4STATS

							graphicsCounter[kGraphicsCounterStructurePrimitives] += triangleCount;

						#endif

						index[size] = offset + data[0] * sizeof(Triangle);
						count[size] = triangleCount * 3;
						size++;
					}

					mask >>= 1;
					data += 2;
				}

				Render::MultiDrawIndexedPrimitives(Render::kPrimitiveTriangles, count, index, size);
				break;
			}
		}
	}

	segmentArray[0].Clear();
}

template void GraphicsMgr::DrawStructureContainer(const List<Renderable> *, bool);
template void GraphicsMgr::DrawStructureContainer(const ImmutableArray<Renderable *> *, bool);

template <class container> void GraphicsMgr::DrawStructureDepthContainer(const container *renderList, bool group)
{
	Transform4D		mv;
	Transform4D		*modelview;

	for (Renderable *renderable : *renderList)
	{
		if (!(renderable->GetRenderableFlags() & kRenderableStructureBufferInhibit))
		{
			ShaderKey shaderKey(kShaderVariantNormal, renderable->GetShaderDetailLevel());

			RenderSegment *segment = renderable->GetFirstRenderSegment();
			do
			{
				ShaderData *shaderData = segment->GetShaderData(kShaderStructure, shaderKey);
				if (!shaderData)
				{
					shaderData = segment->InitShaderData(renderable, kShaderStructure, shaderKey);
				}

				const ShaderProgram *program = shaderData->shaderProgram;
				if (program)
				{
					segment->currentShaderData = shaderData;
					segmentArray[0].AddElement(SegmentReference{segment, GetPointerAddress(program)});
				}

			} while ((segment = segment->GetNextRenderSegment()) != nullptr);
		}
	}

	int32 segmentCount = segmentArray[0].GetElementCount();
	if (segmentCount == 0)
	{
		return;
	}

	if (group)
	{
		segmentArray[1].SetElementCount(segmentCount);
		GroupSegmentArray(segmentArray[0], segmentArray[1], 0, segmentCount - 1);
	}

	const Renderable *prevRenderable = nullptr;
	for (const SegmentReference& reference : segmentArray[0])
	{
		const RenderSegment *segment = reference.segment;

		Renderable *renderable = segment->owningRenderable;
		if (renderable != prevRenderable)
		{
			prevRenderable = renderable;

			const Transformable *transformable = renderable->GetTransformable();
			if (transformable)
			{
				mv = cameraSpaceTransform * transformable->GetWorldTransform();
				modelview = &mv;
			}
			else
			{
				modelview = &cameraSpaceTransform;
			}

			unsigned_int32 newRenderState = kRenderDepthTest | (renderable->GetRenderState() & (kRenderDepthInhibit | kRenderDepthOffset));
			SetRenderState(newRenderState);

			if (newRenderState & kRenderDepthOffset)
			{
				float z = Fmin(cameraSpaceTransform.GetRow(2) ^ renderable->GetDepthOffsetPoint(), -cameraObject->GetNearDepth());
				float delta = renderable->GetDepthOffsetDelta();
				float epsilon = depthOffsetConstant * delta / (z * (z + delta));
				epsilon = Fmax(Fabs(epsilon), 4.8e-7F) * NonzeroFsgn(epsilon);

				currentProjectionMatrix = cameraProjectionMatrix;
				currentProjectionMatrix(2,2) *= 1.0F + epsilon;
			}

			SetModelviewMatrix(*modelview);
		}

		ShaderData *shaderData = segment->currentShaderData;

		Render::SetShaderProgram(shaderData->shaderProgram);
		SetMaterialState(shaderData->materialState);
		SetVertexData(&shaderData->vertexData, renderable);

		int32 stateProcCount = shaderData->shaderStateDataCount;
		for (machine a = 0; a < stateProcCount; a++)
		{
			(*shaderData->shaderStateData[a].stateProc)(renderable, shaderData->shaderStateData[a].stateCookie);
		}

		Render::BindTextureArray(&shaderData->textureArray);

		int32 vertexCount = renderable->GetVertexCount();

		#if C4STATS

			graphicsCounter[kGraphicsCounterStructureVertices] += vertexCount;
			graphicsCounter[kGraphicsCounterStructureCommands]++;

		#endif

		switch (renderable->GetRenderType())
		{
			case kRenderTriangles:

				#if C4STATS

					graphicsCounter[kGraphicsCounterStructurePrimitives] += vertexCount / 3;

				#endif

				Render::DrawPrimitives(Render::kPrimitiveTriangles, 0, vertexCount);
				break;

			case kRenderTriangleStrip:

				#if C4STATS

					graphicsCounter[kGraphicsCounterStructurePrimitives] += vertexCount - 2;

				#endif

				Render::DrawPrimitives(Render::kPrimitiveTriangleStrip, 0, vertexCount);
				break;

			case kRenderIndexedTriangles:
			{
				int32 triangleCount = segment->GetPrimitiveCount();

				#if C4STATS

					graphicsCounter[kGraphicsCounterStructurePrimitives] += triangleCount;

				#endif

				Render::DrawIndexedPrimitives(Render::kPrimitiveTriangles, triangleCount * 3, renderable->GetPrimitiveIndexOffset() + segment->GetPrimitiveStart() * sizeof(Triangle));
				break;
			}

			case kRenderQuads:

				#if C4STATS

					graphicsCounter[kGraphicsCounterStructurePrimitives] += vertexCount >> 2;

				#endif

				Render::DrawQuads(0, vertexCount);
				break;

			case kRenderMultiIndexedTriangles:

				Render::MultiDrawIndexedPrimitives(Render::kPrimitiveTriangles, segment->GetMultiCountArray(), segment->GetMultiOffsetArray(), segment->GetMultiRenderCount());
				break;

			case kRenderMaskedMultiIndexedTriangles:
			{
				static machine_address	index[33];
				static unsigned_int32	count[33];

				unsigned_int32 size = 0;
				unsigned_int32 offset = renderable->GetPrimitiveIndexOffset();

				int32 triangleCount = segment->GetPrimitiveCount();
				if (triangleCount != 0)
				{
					#if C4STATS

						graphicsCounter[kGraphicsCounterStructurePrimitives] += triangleCount;

					#endif

					index[0] = offset;
					count[0] = triangleCount * 3;
					size = 1;
				}

				unsigned_int32 mask = segment->GetMultiRenderMask();
				const int32 *data = segment->GetMultiRenderData();
				while (mask != 0)
				{
					if (mask & 1)
					{
						triangleCount = data[1];

						#if C4STATS

							graphicsCounter[kGraphicsCounterStructurePrimitives] += triangleCount;

						#endif

						index[size] = offset + data[0] * sizeof(Triangle);
						count[size] = triangleCount * 3;
						size++;
					}

					mask >>= 1;
					data += 2;
				}

				Render::MultiDrawIndexedPrimitives(Render::kPrimitiveTriangles, count, index, size);
				break;
			}
		}
	}

	segmentArray[0].Clear();
}

template void GraphicsMgr::DrawStructureDepthContainer(const List<Renderable> *, bool);
template void GraphicsMgr::DrawStructureDepthContainer(const ImmutableArray<Renderable *> *, bool);

bool GraphicsMgr::BeginDistortionRendering(void)
{
	if ((renderOptionFlags & kRenderOptionDistortion) && (currentRenderTargetType == kRenderTargetPrimary))
	{
		static const float distortionClearColor[4] = {0.0F, 0.0F, 0.0F, 0.0F};

		SetRenderTarget(kRenderTargetDistortion);
		SetRenderState(kRenderDepthTest | kRenderDepthInhibit);

		Render::ClearColorBuffer(distortionClearColor);

		SetBlendState(kBlendAccumulate | kBlendAlphaAccumulate);

		unsigned_int32 graphicsState = currentGraphicsState;
		if (!(graphicsState & kGraphicsDepthTestLess))
		{
			currentGraphicsState = graphicsState | kGraphicsDepthTestLess;
			Render::SetDepthFunc(Render::kDepthLess);
		}

		currentGraphicsState |= kGraphicsDistortionAvail;
		return (true);
	}

	return (false);
}

void GraphicsMgr::EndDistortionRendering(void)
{
	SetRenderTarget(kRenderTargetPrimary);
}

void GraphicsMgr::DrawDistortion(const List<Renderable> *renderList)
{
	for (Renderable *renderable = renderList->First(); renderable; renderable = renderable->Next())
	{
		unsigned_int32 newRenderState = renderable->GetRenderState();
		SetRenderState(newRenderState);

		unsigned_int32 renderableFlags = renderable->GetRenderableFlags();
		const Transformable *transformable = renderable->GetTransformable();
		if (!(renderableFlags & kRenderableCameraTransformInhibit))
		{
			if (newRenderState & kRenderDepthOffset)
			{
				float z = Fmin(cameraSpaceTransform.GetRow(2) ^ renderable->GetDepthOffsetPoint(), -cameraObject->GetNearDepth());
				float delta = renderable->GetDepthOffsetDelta();
				float epsilon = depthOffsetConstant * delta / (z * (z + delta));
				epsilon = Fmax(Fabs(epsilon), 4.8e-7F) * NonzeroFsgn(epsilon);

				currentProjectionMatrix = cameraProjectionMatrix;
				currentProjectionMatrix(2,2) *= 1.0F + epsilon;
			}

			if (transformable)
			{
				SetModelviewMatrix(cameraSpaceTransform * transformable->GetWorldTransform());
			}
			else
			{
				SetModelviewMatrix(cameraSpaceTransform);
			}
		}
		else
		{
			if (transformable)
			{
				SetModelviewMatrix(transformable->GetWorldTransform());
			}
			else
			{
				SetModelviewMatrix(Identity4D);
			}
		}

		ShaderKey shaderKey(kShaderVariantNormal, renderable->GetShaderDetailLevel());

		RenderSegment *segment = renderable->GetFirstRenderSegment();
		do
		{
			ShaderData *shaderData = segment->GetShaderData(kShaderAmbient, shaderKey);
			if (!shaderData)
			{
				shaderData = segment->InitShaderData(renderable, kShaderAmbient, shaderKey);
			}

			const ShaderProgram *program = shaderData->shaderProgram;
			if (!program)
			{
				continue;
			}

			Render::SetShaderProgram(program);
			SetMaterialState(shaderData->materialState & kMaterialTwoSided);
			SetVertexData(&shaderData->vertexData, renderable);

			int32 stateProcCount = shaderData->shaderStateDataCount;
			for (machine a = 0; a < stateProcCount; a++)
			{
				(*shaderData->shaderStateData[a].stateProc)(renderable, shaderData->shaderStateData[a].stateCookie);
			}

			Render::BindTextureArray(&shaderData->textureArray);

			int32 vertexCount = renderable->GetVertexCount();

			#if C4STATS

				graphicsCounter[kGraphicsCounterDistortionVertices] += vertexCount;
				graphicsCounter[kGraphicsCounterDistortionCommands]++;

			#endif

			switch (renderable->GetRenderType())
			{
				case kRenderTriangles:

					#if C4STATS

						graphicsCounter[kGraphicsCounterDistortionPrimitives] += vertexCount / 3;

					#endif

					Render::DrawPrimitives(Render::kPrimitiveTriangles, 0, vertexCount);
					break;

				case kRenderTriangleStrip:

					#if C4STATS

						graphicsCounter[kGraphicsCounterDistortionPrimitives] += vertexCount - 2;

					#endif

					Render::DrawPrimitives(Render::kPrimitiveTriangleStrip, 0, vertexCount);
					break;

				case kRenderIndexedTriangles:
				{
					int32 triangleCount = segment->GetPrimitiveCount();

					#if C4STATS

						graphicsCounter[kGraphicsCounterDistortionPrimitives] += triangleCount;

					#endif

					Render::DrawIndexedPrimitives(Render::kPrimitiveTriangles, triangleCount * 3, renderable->GetPrimitiveIndexOffset() + segment->GetPrimitiveStart() * sizeof(Triangle));
					break;
				}

				case kRenderQuads:

					#if C4STATS

						graphicsCounter[kGraphicsCounterDistortionPrimitives] += vertexCount >> 2;

					#endif

					Render::DrawQuads(0, vertexCount);
					break;
			}

		} while ((segment = segment->GetNextRenderSegment()) != nullptr);
	}
}

void GraphicsMgr::BeginInfiniteShadow(void)
{
	if (graphicsActiveFlags & kGraphicsActiveTimer)
	{
		Timestamp(kTimestampBeginShadow);
	}

	savedCameraObject = cameraObject;
	savedCameraTransformable = cameraTransformable;

	unsigned_int32 graphicsState = currentGraphicsState;
	if (!(graphicsState & kGraphicsDepthTestLess))
	{
		Render::SetDepthFunc(Render::kDepthLess);
	}

	currentGraphicsState = (graphicsState | (kGraphicsDepthTestLess | kGraphicsRenderShadow)) & ~kGraphicsLightScissor;

	SetAmbientLight();

	infiniteShadowFrameBuffer->GetRenderTargetTexture()->Unbind(kTextureUnitShadowMap);
	Render::SetFrameBuffer(infiniteShadowFrameBuffer);

	SetBlendState(kBlendReplace);
	SetRenderState((currentRenderState & kRenderDepthTest) | kRenderColorInhibit);
	SetMaterialState(currentMaterialState & kMaterialTwoSided);

	renderTargetHeight = infiniteShadowMapSize;
	Render::SetScissor(0, 0, infiniteShadowMapSize, infiniteShadowMapSize);

	Render::SetPolygonOffset(1.0F, 1.0F, 0.0009765625F);
	Render::EnablePolygonFillOffset();
	Render::EnableDepthClamp();
}

void GraphicsMgr::EndInfiniteShadow(void)
{
	if ((currentRenderTargetType == kRenderTargetPrimary) && (multisampleFrameBuffer))
	{
		Render::SetFrameBuffer(multisampleFrameBuffer);
	}
	else
	{
		Render::SetFrameBuffer(normalFrameBuffer);
	}

	Render::DisablePolygonFillOffset();
	Render::DisableDepthClamp();

	currentGraphicsState &= ~kGraphicsRenderShadow;
	renderTargetHeight = TheDisplayMgr->GetDisplayHeight();

	SetCamera(savedCameraObject, savedCameraTransformable, 0, false);

	if (graphicsActiveFlags & kGraphicsActiveTimer)
	{
		Timestamp(kTimestampEndShadow);
	}
}

void GraphicsMgr::SetInfiniteShadowCascade(int32 cascade)
{
	infiniteShadowFrameBuffer->SetShadowLayer(cascade);
	Render::ClearDepthBuffer();
}

void GraphicsMgr::DiscardInfiniteShadow(void)
{
	#if C4OPENGL

		#if C4GLCORE

			infiniteShadowFrameBuffer->Invalidate();

		#else //[ MOBILE

			// -- Mobile code hidden --

		#endif //]

	#endif
}

void GraphicsMgr::BeginPointShadow(void)
{
	if (graphicsActiveFlags & kGraphicsActiveTimer)
	{
		Timestamp(kTimestampBeginShadow);
	}

	savedCameraObject = cameraObject;
	savedCameraTransformable = cameraTransformable;

	unsigned_int32 graphicsState = currentGraphicsState;
	if (!(graphicsState & kGraphicsDepthTestLess))
	{
		Render::SetDepthFunc(Render::kDepthLess);
	}

	currentGraphicsState = (graphicsState | (kGraphicsDepthTestLess | kGraphicsRenderShadow)) & ~kGraphicsLightScissor;

	SetAmbientLight();

	pointShadowFrameBuffer->GetRenderTargetTexture()->Unbind(kTextureUnitShadowMap);
	Render::SetFrameBuffer(pointShadowFrameBuffer);

	SetBlendState(kBlendReplace);
	SetRenderState((currentRenderState & kRenderDepthTest) | kRenderColorInhibit);
	SetMaterialState(currentMaterialState & kMaterialTwoSided);

	renderTargetHeight = pointShadowMapSize;
	Render::SetScissor(0, 0, pointShadowMapSize, pointShadowMapSize);

	Render::SetPolygonOffset(3.0F, 0.0F, 0.0009765625F);
	Render::EnablePolygonFillOffset();
	Render::EnableDepthClamp();
	Render::SetDepthRange(0.00390625F, 1.0F);
}

void GraphicsMgr::EndPointShadow(void)
{
	if ((currentRenderTargetType == kRenderTargetPrimary) && (multisampleFrameBuffer))
	{
		Render::SetFrameBuffer(multisampleFrameBuffer);
	}
	else
	{
		Render::SetFrameBuffer(normalFrameBuffer);
	}

	Render::DisablePolygonFillOffset();
	Render::DisableDepthClamp();
	Render::SetDepthRange(0.0F, 1.0F);

	currentGraphicsState &= ~kGraphicsRenderShadow;
	renderTargetHeight = TheDisplayMgr->GetDisplayHeight();

	SetCamera(savedCameraObject, savedCameraTransformable, 0, false);

	if (graphicsActiveFlags & kGraphicsActiveTimer)
	{
		Timestamp(kTimestampEndShadow);
	}
}

void GraphicsMgr::SetPointShadowFace(int32 face)
{
	pointShadowFrameBuffer->SetShadowCubeFace(face);
	Render::ClearDepthBuffer();
}

void GraphicsMgr::DiscardPointShadow(void)
{
	#if C4OPENGL

		#if C4GLCORE

			pointShadowFrameBuffer->Invalidate();

		#else //[ MOBILE

			// -- Mobile code hidden --

		#endif //]

	#endif
}

void GraphicsMgr::BeginSpotShadow(void)
{
	if (graphicsActiveFlags & kGraphicsActiveTimer)
	{
		Timestamp(kTimestampBeginShadow);
	}

	savedCameraObject = cameraObject;
	savedCameraTransformable = cameraTransformable;

	unsigned_int32 graphicsState = currentGraphicsState;
	if (!(graphicsState & kGraphicsDepthTestLess))
	{
		Render::SetDepthFunc(Render::kDepthLess);
	}

	currentGraphicsState = (graphicsState | (kGraphicsDepthTestLess | kGraphicsRenderShadow)) & ~kGraphicsLightScissor;

	SetAmbientLight();

	spotShadowFrameBuffer->GetRenderTargetTexture()->Unbind(kTextureUnitShadowMap);
	Render::SetFrameBuffer(spotShadowFrameBuffer);

	SetBlendState(kBlendReplace);
	SetRenderState((currentRenderState & kRenderDepthTest) | kRenderColorInhibit);
	SetMaterialState(currentMaterialState & kMaterialTwoSided);

	renderTargetHeight = spotShadowMapSize;
	Render::SetScissor(0, 0, spotShadowMapSize, spotShadowMapSize);
	Render::ClearDepthBuffer();

	Render::SetPolygonOffset(3.0F, 0.0F, 0.0009765625F);
	Render::EnablePolygonFillOffset();
	Render::EnableDepthClamp();
	Render::SetDepthRange(0.00390625F, 1.0F);
}

void GraphicsMgr::EndSpotShadow(void)
{
	if ((currentRenderTargetType == kRenderTargetPrimary) && (multisampleFrameBuffer))
	{
		Render::SetFrameBuffer(multisampleFrameBuffer);
	}
	else
	{
		Render::SetFrameBuffer(normalFrameBuffer);
	}

	Render::DisablePolygonFillOffset();
	Render::DisableDepthClamp();
	Render::SetDepthRange(0.0F, 1.0F);

	currentGraphicsState &= ~kGraphicsRenderShadow;
	renderTargetHeight = TheDisplayMgr->GetDisplayHeight();

	SetCamera(savedCameraObject, savedCameraTransformable, 0, false);

	if (graphicsActiveFlags & kGraphicsActiveTimer)
	{
		Timestamp(kTimestampEndShadow);
	}
}

void GraphicsMgr::DiscardSpotShadow(void)
{
	#if C4OPENGL

		#if C4GLCORE

			spotShadowFrameBuffer->Invalidate();

		#else //[ MOBILE

			// -- Mobile code hidden --

		#endif //]

	#endif
}

void GraphicsMgr::DrawShadow(const ImmutableArray<Renderable *>& shadowArray)
{
	for (Renderable *renderable : shadowArray)
	{
		SetRenderState((renderable->GetRenderState() & kRenderDepthTest) | (kRenderColorInhibit | kRenderDepthOffset));

		const Transformable *transformable = renderable->GetTransformable();
		if (transformable)
		{
			SetModelviewMatrix(cameraSpaceTransform * transformable->GetWorldTransform());
		}
		else
		{
			SetModelviewMatrix(cameraSpaceTransform);
		}

		ShaderKey shaderKey(kShaderVariantNormal, renderable->GetShaderDetailLevel());

		RenderSegment *segment = renderable->GetFirstRenderSegment();
		do
		{
			ShaderData *shaderData = segment->GetShaderData(kShaderShadow, shaderKey);
			if (!shaderData)
			{
				shaderData = segment->InitShaderData(renderable, kShaderShadow, shaderKey);
			}

			const ShaderProgram *program = shaderData->shaderProgram;
			if (!program)
			{
				continue;
			}

			Render::SetShaderProgram(program);
			SetMaterialState(shaderData->materialState & kMaterialTwoSided);
			SetVertexData(&shaderData->vertexData, renderable);

			int32 stateProcCount = shaderData->shaderStateDataCount;
			for (machine a = 0; a < stateProcCount; a++)
			{
				(*shaderData->shaderStateData[a].stateProc)(renderable, shaderData->shaderStateData[a].stateCookie);
			}

			Render::BindTextureArray(&shaderData->textureArray);

			int32 vertexCount = renderable->GetVertexCount();

			#if C4STATS

				graphicsCounter[kGraphicsCounterShadowVertices] += vertexCount;
				graphicsCounter[kGraphicsCounterShadowCommands]++;

			#endif

			switch (renderable->GetRenderType())
			{
				case kRenderTriangles:

					#if C4STATS

						graphicsCounter[kGraphicsCounterShadowPrimitives] += vertexCount / 3;

					#endif

					Render::DrawPrimitives(Render::kPrimitiveTriangles, 0, vertexCount);
					break;

				case kRenderTriangleStrip:

					#if C4STATS

						graphicsCounter[kGraphicsCounterShadowPrimitives] += vertexCount - 2;

					#endif

					Render::DrawPrimitives(Render::kPrimitiveTriangleStrip, 0, vertexCount);
					break;

				case kRenderIndexedTriangles:
				{
					int32 triangleCount = segment->GetPrimitiveCount();

					#if C4STATS

						graphicsCounter[kGraphicsCounterShadowPrimitives] += triangleCount;

					#endif

					Render::DrawIndexedPrimitives(Render::kPrimitiveTriangles, triangleCount * 3, renderable->GetPrimitiveIndexOffset() + segment->GetPrimitiveStart() * sizeof(Triangle));
					break;
				}

				case kRenderQuads:

					#if C4STATS

						graphicsCounter[kGraphicsCounterShadowPrimitives] += vertexCount >> 2;

					#endif

					Render::DrawQuads(0, vertexCount);
					break;

				case kRenderMultiIndexedTriangles:

					Render::MultiDrawIndexedPrimitives(Render::kPrimitiveTriangles, segment->GetMultiCountArray(), segment->GetMultiOffsetArray(), segment->GetMultiRenderCount());
					break;

				case kRenderMaskedMultiIndexedTriangles:
				{
					static machine_address	index[33];
					static unsigned_int32	count[33];

					unsigned_int32 size = 0;
					unsigned_int32 offset = renderable->GetPrimitiveIndexOffset();

					int32 triangleCount = segment->GetPrimitiveCount();
					if (triangleCount != 0)
					{
						#if C4STATS

							graphicsCounter[kGraphicsCounterShadowPrimitives] += triangleCount;

						#endif

						index[0] = offset;
						count[0] = triangleCount * 3;
						size = 1;
					}

					unsigned_int32 mask = segment->GetMultiRenderMask();
					const int32 *data = segment->GetMultiRenderData();
					while (mask != 0)
					{
						if (mask & 1)
						{
							triangleCount = data[1];

							#if C4STATS

								graphicsCounter[kGraphicsCounterShadowPrimitives] += triangleCount;

							#endif

							index[size] = offset + data[0] * sizeof(Triangle);
							count[size] = triangleCount * 3;
							size++;
						}

						mask >>= 1;
						data += 2;
					}

					Render::MultiDrawIndexedPrimitives(Render::kPrimitiveTriangles, count, index, size);
					break;
				}
			}

		} while ((segment = segment->GetNextRenderSegment()) != nullptr);
	}
}

template <class container> void GraphicsMgr::DrawWireframeContainer(unsigned_int32 flags, const container *renderList)
{
	if (renderList->Empty())
	{
		return;
	}

	unsigned_int32 state = (currentRenderState & kRenderDepthInhibit) | kRenderWireframe;

	if (flags & kWireframeDepthTest)
	{
		state |= kRenderDepthTest;
		Render::EnablePolygonLineOffset();
		Render::SetPolygonOffset(0.0F, -2.0F, 0.0F);
	}

	SetRenderState(state);
	SetBlendState(kBlendReplace);

	unsigned_int32 newMaterialState = currentMaterialState & kMaterialTwoSided;
	SetMaterialState(newMaterialState);

	Render::SetFragmentShaderParameter(kFragmentParamConstant7, K::white);

	for (Renderable *renderable : *renderList)
	{
		if (renderable->GetRenderType() >= kRenderTriangles)
		{
			const Transformable *transformable = renderable->GetTransformable();
			if (!(renderable->GetRenderableFlags() & kRenderableCameraTransformInhibit))
			{
				if (transformable)
				{
					SetModelviewMatrix(cameraSpaceTransform * transformable->GetWorldTransform());
				}
				else
				{
					SetModelviewMatrix(cameraSpaceTransform);
				}
			}
			else
			{
				if (transformable)
				{
					SetModelviewMatrix(transformable->GetWorldTransform());
				}
				else
				{
					SetModelviewMatrix(Identity4D);
				}
			}

			if (flags & kWireframeColor)
			{
				const ColorRGBA *wireColor = renderable->GetWireframeColorPointer();
				Render::SetFragmentShaderParameter(kFragmentParamConstant7, (wireColor) ? *wireColor : K::white);
			}

			ShaderKey shaderKey(kShaderVariantNormal, renderable->GetShaderDetailLevel());

			RenderSegment *segment = renderable->GetFirstRenderSegment();
			do
			{
				ShaderData *shaderData = segment->GetShaderData(kShaderShadow, shaderKey);
				if (!shaderData)
				{
					shaderData = segment->InitShaderData(renderable, kShaderShadow, shaderKey);
				}

				if (!shaderData->shaderProgram)
				{
					continue;
				}

				if ((shaderData->materialState & kMaterialTwoSided) || (flags & kWireframeTwoSided))
				{
					if (!(newMaterialState & kMaterialTwoSided))
					{
						newMaterialState |= kMaterialTwoSided;
						Render::DisableCullFace();
					}
				}
				else
				{
					if (newMaterialState & kMaterialTwoSided)
					{
						newMaterialState &= ~kMaterialTwoSided;
						Render::EnableCullFace();
					}
				}

				ProgramStageTable	stageTable;

				FragmentShader *fragmentShader = GetLocalFragmentShader(kLocalFragmentShaderCopyConstant);

				stageTable.vertexShader = shaderData->shaderProgram->GetVertexShader();
				stageTable.fragmentShader = fragmentShader;

				ShaderProgram *program = ShaderProgram::Get(stageTable);
				Render::SetShaderProgram(program);
				program->Release();

				fragmentShader->Release();

				SetVertexData(&shaderData->vertexData, renderable);

				int32 stateProcCount = shaderData->shaderStateDataCount;
				for (machine a = 0; a < stateProcCount; a++)
				{
					(*shaderData->shaderStateData[a].stateProc)(renderable, shaderData->shaderStateData[a].stateCookie);
				}

				int32 vertexCount = renderable->GetVertexCount();
				switch (renderable->GetRenderType())
				{
					case kRenderTriangles:

						Render::DrawPrimitives(Render::kPrimitiveTriangles, 0, vertexCount);
						break;

					case kRenderTriangleStrip:

						Render::DrawPrimitives(Render::kPrimitiveTriangleStrip, 0, vertexCount);
						break;

					case kRenderIndexedTriangles:

						Render::DrawIndexedPrimitives(Render::kPrimitiveTriangles, segment->GetPrimitiveCount() * 3, renderable->GetPrimitiveIndexOffset() + segment->GetPrimitiveStart() * sizeof(Triangle));
						break;

					case kRenderQuads:

						Render::DrawQuads(0, vertexCount);
						break;

					case kRenderMultiIndexedTriangles:

						Render::MultiDrawIndexedPrimitives(Render::kPrimitiveTriangles, segment->GetMultiCountArray(), segment->GetMultiOffsetArray(), segment->GetMultiRenderCount());
						break;

					case kRenderMaskedMultiIndexedTriangles:
					{
						static machine_address	index[33];
						static unsigned_int32	count[33];

						unsigned_int32 size = 0;
						unsigned_int32 offset = renderable->GetPrimitiveIndexOffset();

						int32 triangleCount = segment->GetPrimitiveCount();
						if (triangleCount != 0)
						{
							index[0] = offset;
							count[0] = triangleCount * 3;
							size = 1;
						}

						unsigned_int32 mask = segment->GetMultiRenderMask();
						const int32 *data = segment->GetMultiRenderData();
						while (mask != 0)
						{
							if (mask & 1)
							{
								triangleCount = data[1];

								index[size] = offset + data[0] * sizeof(Triangle);
								count[size] = triangleCount * 3;
								size++;
							}

							mask >>= 1;
							data += 2;
						}

						Render::MultiDrawIndexedPrimitives(Render::kPrimitiveTriangles, count, index, size);
						break;
					}
				}
			} while ((segment = segment->GetNextRenderSegment()) != nullptr);
		}
	}

	currentMaterialState = newMaterialState;

	if (flags & kWireframeDepthTest)
	{
		Render::DisablePolygonLineOffset();
	}
}

template C4API void GraphicsMgr::DrawWireframeContainer(unsigned_int32, const List<Renderable> *);
template C4API void GraphicsMgr::DrawWireframeContainer(unsigned_int32, const Array<Renderable *> *);

template <class container> void GraphicsMgr::DrawVectorsContainer(int32 arrayIndex, const container *renderList)
{
	if (renderList->Empty())
	{
		return;
	}

	static Link<ShaderProgram>		vectorShaderProgram;

	SetBlendState(kBlendReplace);
	SetRenderState(currentRenderState & kRenderDepthInhibit);
	SetMaterialState(currentMaterialState & kMaterialTwoSided);

	Render::SetFragmentShaderParameter(kFragmentParamConstant7, K::white);

	ShaderProgram *program = vectorShaderProgram;
	if (!program)
	{
		int32					vertexSnippetCount;
		const VertexSnippet		*const *vertexSnippet;
		ProgramStageTable		stageTable;

		FragmentShader *fragmentShader = GetLocalFragmentShader(kLocalFragmentShaderCopyConstant);
		GeometryShader *geometryShader = GetLocalGeometryShader(kLocalGeometryShaderExtrudeNormalLine, &vertexSnippetCount, &vertexSnippet);
		VertexShader *vertexShader = GetLocalVertexShader(vertexSnippetCount, vertexSnippet);

		stageTable.vertexShader = vertexShader;
		stageTable.fragmentShader = fragmentShader;
		stageTable.geometryShader = geometryShader;

		program = ShaderProgram::Get(stageTable);
		vectorShaderProgram = program;

		geometryShader->Release();
		fragmentShader->Release();
		vertexShader->Release();
	}

	Render::SetShaderProgram(program);

	vectorVertexData->Bind();

	for (Renderable *renderable : *renderList)
	{
		if (renderable->GetRenderType() >= kRenderTriangles)
		{
			int32 positionComponentCount = renderable->GetVertexAttributeComponentCount(kArrayPosition);
			int32 vectorComponentCount = renderable->GetVertexAttributeComponentCount(arrayIndex);
			if ((positionComponentCount >= 2) && (vectorComponentCount >= 3))
			{
				const Transformable *transformable = renderable->GetTransformable();
				if (!(renderable->GetRenderableFlags() & kRenderableCameraTransformInhibit))
				{
					if (transformable)
					{
						SetGeometryModelviewMatrix(cameraSpaceTransform * transformable->GetWorldTransform());
					}
					else
					{
						SetGeometryModelviewMatrix(cameraSpaceTransform);
					}
				}
				else
				{
					if (transformable)
					{
						SetGeometryModelviewMatrix(transformable->GetWorldTransform());
					}
					else
					{
						SetGeometryModelviewMatrix(Identity4D);
					}
				}

				unsigned_int32 bufferIndex = ((renderable->GetVertexBufferArrayFlags() & (1 << kArrayPosition)) != 0);
				vectorVertexData->SetVertexAttribArray(kVertexAttribPosition0, renderable->GetVertexBuffer(bufferIndex), renderable->GetVertexAttributeOffset(kArrayPosition), positionComponentCount, renderable->GetVertexBufferStride(bufferIndex));

				bufferIndex = ((renderable->GetVertexBufferArrayFlags() & (1 << arrayIndex)) != 0);
				vectorVertexData->SetVertexAttribArray(kVertexAttribNormal, renderable->GetVertexBuffer(bufferIndex), renderable->GetVertexAttributeOffset(arrayIndex), vectorComponentCount, renderable->GetVertexBufferStride(bufferIndex));

				vectorVertexData->Invalidate();
				Render::DrawPrimitives(Render::kPrimitivePoints, 0, renderable->GetVertexCount());
			}
		}

		renderable = renderable->Next();
	}
}

template void GraphicsMgr::DrawVectorsContainer(int32, const List<Renderable> *);
template void GraphicsMgr::DrawVectorsContainer(int32, const ImmutableArray<Renderable *> *);

void GraphicsMgr::ProcessOcclusionQueries(void)
{
	List<Renderable>	renderList;

	float normalizer = occlusionAreaNormalizer;
	if ((multisampleFrameBuffer) && (currentRenderTargetType == kRenderTargetPrimary))
	{
		normalizer *= multisampleFrameBuffer->GetSampleDivider();
	}

	for (;;)
	{
		OcclusionQuery *query = occlusionQueryList.First();
		if (!query)
		{
			break;
		}

		OcclusionQuery::occlusionQueryList.Append(query);
		query->Process(&renderList, normalizer);
	}

	DrawContainer(&renderList, false);
	renderList.RemoveAll();
}

void GraphicsMgr::ReadImageBuffer(const Rect& rect, Color4C *image, int32 rowPixels)
{
	#if C4OPENGL

		glPixelStorei(GL_PACK_ROW_LENGTH, rowPixels);
		glReadPixels(rect.left, TheDisplayMgr->GetDisplayHeight() - rect.bottom, rect.Width(), rect.Height(), GL_RGBA, GL_UNSIGNED_BYTE, image);

	#endif
}

// ZYUQURM
