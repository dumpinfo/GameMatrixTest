 

#ifndef C4GraphicsMgr_h
#define C4GraphicsMgr_h


//# \component	Graphics Manager
//# \prefix		GraphicsMgr/


#define C4CREATE_DEBUG_CONTEXT		0


#include "C4Threads.h"
#include "C4Variables.h"
#include "C4Renderable.h"
#include "C4CameraObjects.h"
#include "C4LightObjects.h"


namespace C4
{
	typedef EngineResult	GraphicsResult;
	typedef Type			TimestampType;


	enum : GraphicsResult
	{
		kGraphicsOkay				= kEngineOkay,
		kGraphicsFormatFailed		= (kManagerGraphics << 16) | 0x0001,
		kGraphicsContextFailed		= (kManagerGraphics << 16) | 0x0002,
		kGraphicsNoHardware			= (kManagerGraphics << 16) | 0x0003
	};


	enum
	{
		kExtensionConservativeDepth,
		kExtensionDebugOutput,
		kExtensionGetProgramBinary,
		kExtensionInvalidateSubdata,
		kExtensionMultiBind,
		kExtensionQueryBufferObject,
		kExtensionSampleShading,
		kExtensionTessellationShader,
		kExtensionTextureStorage,
		kExtensionDepthBoundsTest,
		kExtensionDirectStateAccess,
		kExtensionPolygonOffsetClamp,
		kExtensionTextureCompressionS3TC,
		kExtensionTextureFilterAnisotropic,
		kExtensionTextureMirrorClamp,
		kExtensionExplicitMultisample,
		kExtensionFramebufferMultisampleCoverage,
		kExtensionShaderBufferLoad,
		kExtensionVertexBufferUnifiedMemory,
		kGraphicsExtensionCount
	};


	#if C4WINDOWS

		enum
		{
			kWindowSystemExtensionCreateContext,
			kWindowSystemExtensionPixelFormat,
			kWindowSystemExtensionSwapControl,
			kWindowSystemExtensionSwapControlTear,
			kWindowSystemExtensionCount
		};

	#elif C4LINUX

		enum
		{
			kWindowSystemExtensionCreateContextProfile,
			kWindowSystemExtensionSwapControl,
			kWindowSystemExtensionSwapControlTear,
			kWindowSystemExtensionCount
		};

	#endif


	enum
	{
		kRenderOptionParallaxMapping		= 1 << 0,
		kRenderOptionHorizonMapping			= 1 << 1,
		kRenderOptionTerrainBumps			= 1 << 2,
		kRenderOptionStructureEffects		= 1 << 3,
		kRenderOptionAmbientOcclusion		= 1 << 4,
		kRenderOptionAmbientBumps			= 1 << 5,
		kRenderOptionMotionBlur				= 1 << 6,
		kRenderOptionDistortion				= 1 << 7,
		kRenderOptionGlowBloom				= 1 << 8
	};


	enum
	{
		kGraphicsActiveTimer				= 1 << 0,
		kGraphicsActiveStructureRendering	= 1 << 1,
		kGraphicsActiveStructureEffects		= 1 << 2, 
		kGraphicsActiveVelocityRendering	= 1 << 3,
		kGraphicsActiveAmbientOcclusion		= 1 << 4,
		kGraphicsActiveGlowBloom			= 1 << 5 
	};
 

	enum
	{ 
		kStructureClearBuffer				= 1 << 0,
		kStructureZeroBackgroundVelocity	= 1 << 1, 
		kStructureRenderVelocity			= 1 << 2, 
		kStructureRenderDepth				= 1 << 3
	};

 
	enum
	{
		kWireframeColor						= 1 << 0,
		kWireframeTwoSided					= 1 << 1,
		kWireframeDepthTest					= 1 << 2
	};


	enum
	{
		kProcessGridWidth					= 16,
		kProcessGridHeight					= 12
	};


	enum
	{
		kTextureUnitColor					= 0,
		kTextureUnitStructure				= 1,
		kTextureUnitVelocity				= 2,
		kTextureUnitOcclusion				= 3,
		kTextureUnitDistortion				= 4,
		kTextureUnitGlowBloom				= 5,
		kTextureUnitNoise					= 6,
		kTextureUnitCount
	};

	static_assert(kTextureUnitCount == kMaxGlobalTextureCount, "Global texture count mismatch");


	#if C4OPENGL

		enum
		{
			kTextureUnitShadowMap				= 14,
			kTextureUnitLightProjector			= 15
		};

		#define TEXTURE_UNIT_SHADOW_MAP			"14"
		#define TEXTURE_UNIT_LIGHT_PROJECTOR	"15"

	#elif C4PS4 //[ PS4

		// -- PS4 code hidden --

	#endif //]


	enum
	{
		kDiagnosticWireframe				= 1 << 0,
		kDiagnosticDepthTest				= 1 << 1,
		kDiagnosticNormals					= 1 << 2,
		kDiagnosticTangents					= 1 << 3,
		kDiagnosticTimer					= 1 << 4
	};


	enum
	{
		kMaxTimestampCount					= 256
	};


	enum
	{
		kTimestampBeginRendering			= 'BRND',
		kTimestampEndRendering				= 'ERND',
		kTimestampBeginStructure			= 'BSTR',
		kTimestampEndStructure				= 'ESTR',
		kTimestampBeginOcclusion			= 'BOCC',
		kTimestampEndOcclusion				= 'EOCC',
		kTimestampBeginShadow				= 'BSHD',
		kTimestampEndShadow					= 'ESHD',
		kTimestampBeginPost					= 'BPST',
		kTimestampEndPost					= 'EPST',
		kTimestampBeginGui					= 'BGUI',
		kTimestampEndGui					= 'EGUI'
	};


	#if C4STATS

		enum
		{
			kGraphicsCounterDirectVertices,
			kGraphicsCounterDirectPrimitives,
			kGraphicsCounterDirectCommands,
			kGraphicsCounterShadowVertices,
			kGraphicsCounterShadowPrimitives,
			kGraphicsCounterShadowCommands,
			kGraphicsCounterStructureVertices,
			kGraphicsCounterStructurePrimitives,
			kGraphicsCounterStructureCommands,
			kGraphicsCounterDistortionVertices,
			kGraphicsCounterDistortionPrimitives,
			kGraphicsCounterDistortionCommands,
			kGraphicsCounterCount
		};

	#endif


	enum RenderTargetType
	{
		kRenderTargetNone = -1,
		kRenderTargetDisplay,
		kRenderTargetPrimary,
		kRenderTargetReflection,
		kRenderTargetRefraction,
		kRenderTargetDistortion = kRenderTargetRefraction,
		kRenderTargetStructure,
		kRenderTargetVelocity,
		kRenderTargetOcclusion1,
		kRenderTargetOcclusion2,
		kRenderTargetCount
	};


	enum AmbientMode
	{
		kAmbientNormal,
		kAmbientBright,
		kAmbientDark
	};


	const float kShaderTimePeriod = 120000.0F;
	const float kInverseShaderTimePeriod = 1.0F / kShaderTimePeriod;


	class FogSpaceObject;


	struct GraphicsCapabilities
	{
		unsigned_int16		hardwareSpeedRank;
		unsigned_int16		hardwareMemoryRank;

		#if C4OPENGL

			unsigned_int32	openglVersion;

			bool			extensionFlag[kGraphicsExtensionCount];

			#if C4WINDOWS || C4LINUX

				bool		windowSystemExtensionFlag[kWindowSystemExtensionCount];

			#endif

			int32			programBinaryFormats;
			int32			*programBinaryFormatArray;

		#endif

		int32				maxTextureSize;
		int32				max3DTextureSize;
		int32				maxCubeTextureSize;
		int32				maxArrayTextureLayers;
		float				maxTextureAnisotropy;
		float				maxTextureLodBias;
		int32				maxCombinedTextureImageUnits;

		int32				maxColorAttachments;
		int32				maxRenderBufferSize;
		int32				maxMultisampleSamples;

		int32				maxVertexAttribs;
		int32				maxVaryingComponents;
		int32				maxVertexUniformComponents;
		int32				maxVertexTextureImageUnits;

		int32				maxFragmentUniformComponents;
		int32				maxFragmentTextureImageUnits;

		#if C4RENDER_OCCLUSION_QUERY

			int32			queryCounterBits;

		#endif

		#if C4GLCORE

			int32			maxVertexVaryingComponents;
			int32			maxGeometryVaryingComponents;
			int32			maxGeometryUniformComponents;
			int32			maxGeometryOutputVertices;
			int32			maxGeometryTotalOutputComponents;
			int32			maxGeometryTextureImageUnits;

		#endif

		GraphicsCapabilities();
		~GraphicsCapabilities();
	};


	struct GraphicsExtensionData
	{
		const char			*name1;
		const char			*name2;
		unsigned_int16		version;
		bool				required;
		mutable bool		enabled;
	};


	#if C4WINDOWS || C4LINUX

		struct WindowSystemExtensionData
		{
			const char		*name;
			mutable bool	enabled;
		};

	#endif


	struct SegmentReference
	{
		RenderSegment		*segment;
		machine_address		groupKey;
	};


	class NormalFrameBuffer : public Render::FrameBufferObject
	{
		private:

			unsigned_int32					renderTargetMask;
			Render::TextureObject			textureObject[kRenderTargetCount];

			Render::RenderBufferObject		depthRenderBuffer;

		public:

			NormalFrameBuffer(int32 width, int32 height, unsigned_int32 mask);
			~NormalFrameBuffer();

			unsigned_int32 GetRenderTargetMask(void) const
			{
				return (renderTargetMask);
			}

			Render::TextureObject *GetRenderTargetTexture(int32 target)
			{
				return (&textureObject[target]);
			}

			void Invalidate(void);
	};


	class EffectFrameBuffer : public Render::FrameBufferObject
	{
		private:

			Render::TextureObject		textureObject;

		public:

			EffectFrameBuffer(int32 width, int32 height);
			~EffectFrameBuffer();

			Render::TextureObject *GetRenderTargetTexture(void)
			{
				return (&textureObject);
			}

			void Invalidate(void)
			{
				textureObject.InvalidateImage();
			}
	};


	class ShadowFrameBuffer : public Render::FrameBufferObject
	{
		protected:

			Render::TextureObject		textureObject;

			ShadowFrameBuffer(unsigned_int32 targetIndex);
			~ShadowFrameBuffer();

		public:

			Render::TextureObject *GetRenderTargetTexture(void)
			{
				return (&textureObject);
			}

			void Invalidate(void)
			{
				textureObject.InvalidateImage();
			}

			void SetTextureAnisotropy(int32 anisotropy);
	};


	class InfiniteShadowFrameBuffer : public ShadowFrameBuffer
	{
		public:

			InfiniteShadowFrameBuffer(int32 size);
			~InfiniteShadowFrameBuffer();

			void SetShadowLayer(int32 layer)
			{
				SetDepthRenderTextureLayer(&textureObject, layer);
			}
	};


	class PointShadowFrameBuffer : public ShadowFrameBuffer
	{
		public:

			PointShadowFrameBuffer(int32 size);
			~PointShadowFrameBuffer();

			void SetShadowCubeFace(int32 face)
			{
				SetDepthRenderTextureCubeFace(&textureObject, face);
			}
	};


	class SpotShadowFrameBuffer : public ShadowFrameBuffer
	{
		public:

			SpotShadowFrameBuffer(int32 size);
			~SpotShadowFrameBuffer();
	};


	class MultisampleFrameBuffer : public Render::FrameBufferObject
	{
		private:

			Render::RenderBufferObject		colorRenderBuffer;
			Render::RenderBufferObject		depthRenderBuffer;

			float							sampleDivider;

		public:

			MultisampleFrameBuffer(int32 width, int32 height, int32 sampleCount);
			~MultisampleFrameBuffer();

			float GetSampleDivider(void) const
			{
				return (sampleDivider);
			}
	};


	//# \class	GraphicsMgr		The Graphics Manager class.
	//
	//# \def	class GraphicsMgr : public Manager<GraphicsMgr>
	//
	//# \desc
	//# The $GraphicsMgr$ class encapsulates the 3D graphics rendering facilities of the C4 Engine.
	//# The single instance of the Graphics Manager is constructed during an application's initialization
	//# and destroyed at termination.
	//#
	//# The Graphics Manager's member functions are accessed through the global pointer $TheGraphicsMgr$.
	//
	//# \also	$@Renderable@$


	//# \function	GraphicsMgr::GetCameraObject	Returns the current camera object.
	//
	//# \proto	const CameraObject *GetCameraObject(void) const;
	//
	//# \desc
	//#
	//
	//# \also	$@GraphicsMgr::GetCameraTransformable@$
	//# \also	$@CameraObject@$


	//# \function	GraphicsMgr::GetCameraTransformable		Returns the current camera transform.
	//
	//# \proto	const Transformable *GetCameraTransformable(void) const;
	//
	//# \desc
	//#
	//
	//# \also	$@GraphicsMgr::GetCameraObject@$
	//# \also	$@Utilities/Transformable@$


	//# \function	GraphicsMgr::GetLightObject		Returns the current light object.
	//
	//# \proto	const LightObject *GetLightObject(void) const;
	//
	//# \desc
	//#
	//
	//# \also	$@GraphicsMgr::GetLightTransformable@$
	//# \also	$@LightObject@$


	//# \function	GraphicsMgr::GetLightTransformable		Returns the current light transform.
	//
	//# \proto	const Transformable *GetLightTransformable(void) const;
	//
	//# \desc
	//#
	//
	//# \also	$@GraphicsMgr::GetLightObject@$
	//# \also	$@Utilities/Transformable@$


	//# \function	GraphicsMgr::Draw		Draws a set of renderable objects.
	//
	//# \proto	void Draw(const List<Renderable> *renderList, bool group = false);
	//# \proto	void Draw(const ImmutableArray<Renderable *> *renderList, bool group = false);
	//
	//# \desc
	//#
	//
	//# \also	$@Renderable@$


	class GraphicsMgr : public Manager<GraphicsMgr>
	{
		private:

			#if C4WINDOWS

				HDC								deviceContext;
				HGLRC							openglContext;

			#elif C4MACOS

				CFBundleRef						openglBundle;
				NSOpenGLContext					*openglContext;

			#elif C4LINUX

				::Display						*openglDisplay;
				::Colormap						openglColormap;
				::Window						openglWindow;
				GLXContext						openglContext;

			#elif C4IOS //[ MOBILE

				// -- Mobile code hidden --

			#endif //]

			#if C4OPENGL

				String<>						extensionsString;

				#if C4WINDOWS || C4LINUX

					const char					*windowSystemExtensionsString;

				#endif

			#endif

			Storage<GraphicsCapabilities>		capabilities;

			unsigned_int32						targetDisableMask;

			Storage<Signal>						syncRenderSignal;
			NullClass							*volatile syncRenderObject;
			const void							*volatile syncRenderData;
			void								(NullClass::*volatile syncRenderFunction)(const void *);
			volatile bool						syncLoadFlag;

			const CameraObject					*cameraObject;
			const Transformable					*cameraTransformable;
			Vector4D							cameraPosition4D;
			Point3D								directCameraPosition;
			Transform4D							cameraSpaceTransform;
			Transform4D							previousCameraSpaceTransform;

			const CameraObject					*savedCameraObject;
			const Transformable					*savedCameraTransformable;

			const FogSpaceObject				*fogSpaceObject;
			const Transformable					*fogSpaceTransformable;
			Antivector4D						worldFogPlane;

			const LightObject					*lightObject;
			const Transformable					*lightTransformable;
			const LightShadowData				*lightShadowData;
			Matrix4D							shadowTransform;
			bool								currentShadowFlag;

			Array<SegmentReference>				segmentArray[2];

			unsigned_int32						graphicsActiveFlags;
			unsigned_int32						diagnosticFlags;

			unsigned_int32						currentBlendState;
			unsigned_int32						currentGraphicsState;
			unsigned_int32						currentRenderState;
			unsigned_int32						disabledRenderState;

			unsigned_int32						currentMaterialState;
			ShaderType							currentShaderType;
			ShaderVariant						currentShaderVariant;
			AmbientMode							currentAmbientMode;

			float								currentNearDepth;
			unsigned_int32						currentFrustumFlags;

			Matrix4D							cameraProjectionMatrix;
			Matrix4D							standardProjectionMatrix;
			Matrix4D							currentProjectionMatrix;
			Matrix4D							currentMVPMatrix;

			unsigned_int32						currentStructureFlags;
			float								occlusionPlaneScale;
			float								renderTargetOffsetSize;
			float								depthOffsetConstant;
			float								cameraLensMultiplier;

			unsigned_int32						colorTransformFlags;
			float								brightnessMultiplier;
			ColorRGBA							finalColorScale[3];
			ColorRGBA							finalColorBias;

			Rect								clipRect;
			Rect								viewportRect;
			Rect								cameraRect;
			Rect								scissorRect;
			Rect								lightRect;
			Rect								shadowRect;
			Range<float>						lightDepthBounds;

			OcclusionQuery						*currentOcclusionQuery;
			List<OcclusionQuery>				occlusionQueryList;

			Antivector4D						distortionPlane;
			float								occlusionAreaNormalizer;

			float								motionBlurBoxLeftOffset;
			float								motionBlurBoxRightOffset;
			float								motionBlurBoxBottomOffset;
			float								motionBlurBoxTopOffset;

			unsigned_int32						renderOptionFlags;
			int32								textureDetailLevel;
			int32								paletteDetailLevel;
			int32								textureFilterAnisotropy;

			int32								infiniteShadowMapSize;
			int32								pointShadowMapSize;
			int32								spotShadowMapSize;

			RenderTargetType					currentRenderTargetType;
			int32								renderTargetHeight;

			Texture								*nullTexture;

			Render::FrameBufferObject			genericFrameBuffer;
			NormalFrameBuffer					*normalFrameBuffer;
			MultisampleFrameBuffer				*multisampleFrameBuffer;
			EffectFrameBuffer					*glowBloomFrameBuffer;
			InfiniteShadowFrameBuffer			*infiniteShadowFrameBuffer;
			PointShadowFrameBuffer				*pointShadowFrameBuffer;
			SpotShadowFrameBuffer				*spotShadowFrameBuffer;

			VertexBuffer						fullscreenVertexBuffer;
			VertexBuffer						processGridVertexBuffer;
			VertexBuffer						processGridIndexBuffer;

			Storage<Render::VertexDataObject>	fullscreenVertexData;
			Storage<Render::VertexDataObject>	processGridVertexData;
			Storage<Render::VertexDataObject>	vectorVertexData;

			bool								motionGridFlag[kProcessGridWidth * kProcessGridHeight];

			int32								timestampCount[4];
			TimestampType						timestampType[4][kMaxTimestampCount];
			Render::QueryObject					timestampQuery[4][kMaxTimestampCount];

			#if C4STATS

				int32							graphicsCounter[kGraphicsCounterCount];

			#endif

			VariableObserver<GraphicsMgr>		textureDetailLevelObserver;
			VariableObserver<GraphicsMgr>		paletteDetailLevelObserver;
			VariableObserver<GraphicsMgr>		textureAnisotropyObserver;
			VariableObserver<GraphicsMgr>		renderParallaxMappingObserver;
			VariableObserver<GraphicsMgr>		renderHorizonMappingObserver;
			VariableObserver<GraphicsMgr>		renderTerrainBumpsObserver;
			VariableObserver<GraphicsMgr>		renderStructureEffectsObserver;
			VariableObserver<GraphicsMgr>		renderAmbientOcclusionObserver;
			VariableObserver<GraphicsMgr>		renderAmbientBumpsObserver;
			VariableObserver<GraphicsMgr>		postBrightnessObserver;
			VariableObserver<GraphicsMgr>		postMotionBlurObserver;
			VariableObserver<GraphicsMgr>		postDistortionObserver;
			VariableObserver<GraphicsMgr>		postGlowBloomObserver;

			static GraphicsExtensionData		extensionData[kGraphicsExtensionCount];

			#if C4WINDOWS || C4LINUX

				static WindowSystemExtensionData	windowSystemExtensionData[kWindowSystemExtensionCount];

				#if C4WINDOWS

					void InitializeWglExtensions(PIXELFORMATDESCRIPTOR *formatDescriptor);
					static LRESULT CALLBACK WglWindowProc(HWND window, UINT message, WPARAM wparam, LPARAM lparam);

				#elif C4LINUX

					void InitializeGlxExtensions(void);

				#endif

			#endif

			#if C4OPENGL && C4DEBUG && C4CREATE_DEBUG_CONTEXT

				static void OPENGLAPI DebugCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar *message, void *userParam);

			#endif

			#if C4LOG_FILE

				static void LogExtensions(const char *string);
				void UpdateLog(void) const;

			#endif

			GraphicsResult InitializeGraphicsContext(int32 frameWidth, int32 frameHeight, int32 displayWidth, int32 displayHeight, unsigned_int32 displayFlags);
			void TerminateGraphicsContext(void);

			void SetDisplaySyncMode(const unsigned_int32 *displayFlags);

			void DetermineHardwareRank(void);
			bool InitializeOpenglExtensions(void);

			void InitializeVariables(void);
			void HandleTextureDetailLevelEvent(Variable *variable);
			void HandlePaletteDetailLevelEvent(Variable *variable);
			void HandleTextureAnisotropyEvent(Variable *variable);
			void HandleRenderParallaxMappingEvent(Variable *variable);
			void HandleRenderHorizonMappingEvent(Variable *variable);
			void HandleRenderTerrainBumpsEvent(Variable *variable);
			void HandleRenderStructureEffectsEvent(Variable *variable);
			void HandleRenderAmbientOcclusionEvent(Variable *variable);
			void HandleRenderAmbientBumpsEvent(Variable *variable);
			void HandlePostBrightnessEvent(Variable *variable);
			void HandlePostMotionBlurEvent(Variable *variable);
			void HandlePostDistortionEvent(Variable *variable);
			void HandlePostGlowBloomEvent(Variable *variable);

			void InitializeProcessGrid(void);
			void InitializeActiveFlags(void);

			static void SyncRenderTask(void (NullClass::*proc)(const void *), NullClass *object, const void *data);

			void SetPostProcessingShader(unsigned_int32 postFlags, const VertexSnippet *snippet);
			void SetDisplayWarpingShader(void);

			static void GroupSegmentArray(SegmentReference *segmentArray, SegmentReference *tempArray, int32 minIndex, int32 maxIndex);
			static void SortSublist(List<Renderable> *list, float zmin, float zmax, List<Renderable> *finalList);

			void SetModelviewMatrix(const Transform4D& matrix);
			void SetGeometryModelviewMatrix(const Transform4D& matrix);

			VertexShader *GetLocalVertexShader(const VertexSnippet *snippet);
			VertexShader *GetLocalVertexShader(int32 snippetCount, const VertexSnippet *const *snippet);
			FragmentShader *GetLocalFragmentShader(int32 shaderIndex);
			GeometryShader *GetLocalGeometryShader(int32 shaderIndex, int32 *vertexSnippetCount, const VertexSnippet *const **vertexSnippet);

			void SetBlendState(unsigned_int32 newBlendState);
			void SetRenderState(unsigned_int32 newRenderState);
			void SetMaterialState(unsigned_int32 newMaterialState);
			void SetVertexData(VertexData *vertexData, const Renderable *renderable);

			template <class container> C4API void DrawContainer(const container *renderList, bool group);

			template <class container> void DrawStructureContainer(const container *renderList, bool group);
			template <class container> void DrawStructureDepthContainer(const container *renderList, bool group);

			template <class container> C4API void DrawWireframeContainer(unsigned_int32 flags, const container *renderList);
			template <class container> C4API void DrawVectorsContainer(int32 arrayIndex, const container *renderList);

		public:

			GraphicsMgr(int);
			~GraphicsMgr();

			EngineResult Construct(void);
			void Destruct(void);

			#if C4WINDOWS

				HDC GetDeviceContext(void) const
				{
					return (deviceContext);
				}

			#endif

			const GraphicsCapabilities *GetCapabilities(void) const
			{
				return (capabilities);
			}

			static const GraphicsExtensionData *GetExtensionData(void)
			{
				return (extensionData);
			}

			#if C4OPENGL

				#if C4WINDOWS || C4LINUX

					static const WindowSystemExtensionData *GetWindowSystemExtensionData(void)
					{
						return (windowSystemExtensionData);
					}

				#endif

				static const char *GetOpenGLVendor(void)
				{
					return (reinterpret_cast<const char *>(glGetString(GL_VENDOR)));
				}

				static const char *GetOpenGLRenderer(void)
				{
					return (reinterpret_cast<const char *>(glGetString(GL_RENDERER)));
				}

				static const char *GetOpenGLVersion(void)
				{
					return (reinterpret_cast<const char *>(glGetString(GL_VERSION)));
				}

				static const char *GetGLSLVersion(void)
				{
					return (reinterpret_cast<const char *>(glGetString(GL_SHADING_LANGUAGE_VERSION)));
				}

			#endif

			unsigned_int32 GetTargetDisableMask(void) const
			{
				return (targetDisableMask);
			}

			void SetTargetDisableMask(unsigned_int32 mask)
			{
				targetDisableMask = mask;
			}

			const CameraObject *GetCameraObject(void) const
			{
				return (cameraObject);
			}

			const Transformable *GetCameraTransformable(void) const
			{
				return (cameraTransformable);
			}

			const Vector4D& GetCameraPosition4D(void) const
			{
				return (cameraPosition4D);
			}

			const Point3D& GetDirectCameraPosition(void) const
			{
				return (directCameraPosition);
			}

			const Transform4D& GetCameraSpaceTransform(void) const
			{
				return (cameraSpaceTransform);
			}

			const FogSpaceObject *GetFogSpaceObject(void) const
			{
				return (fogSpaceObject);
			}

			const Transformable *GetFogSpaceTransformable(void) const
			{
				return (fogSpaceTransformable);
			}

			const Antivector4D& GetFogPlane(void) const
			{
				return (worldFogPlane);
			}

			const LightObject *GetLightObject(void) const
			{
				return (lightObject);
			}

			const Transformable *GetLightTransformable(void) const
			{
				return (lightTransformable);
			}

			const LightShadowData *GetLightShadowData(void) const
			{
				return (lightShadowData);
			}

			const Matrix4D& GetShadowTransform(void) const
			{
				return (shadowTransform);
			}

			unsigned_int32 GetGraphicsActiveFlags(void) const
			{
				return (graphicsActiveFlags);
			}

			void SetDrawShaderType(ShaderType shaderType)
			{
				currentShaderType = shaderType;
			}

			AmbientMode GetAmbientMode(void) const
			{
				return (currentAmbientMode);
			}

			void SetAmbientMode(AmbientMode mode)
			{
				currentAmbientMode = mode;
			}

			const Matrix4D& GetCurrentMVPMatrix(void) const
			{
				return (currentMVPMatrix);
			}

			unsigned_int32 GetStructureFlags(void) const
			{
				return (currentStructureFlags);
			}

			float GetBrightnessMultiplier(void) const
			{
				return (brightnessMultiplier);
			}

			void SetBrightnessMultiplier(float brightness)
			{
				brightnessMultiplier = brightness;
			}

			const ColorRGBA& GetFinalColorScale(int32 index = 0) const
			{
				return (finalColorScale[index]);
			}

			const ColorRGBA& GetFinalColorBias(void) const
			{
				return (finalColorBias);
			}

			float GetRenderTargetOffsetSize(void) const
			{
				return (renderTargetOffsetSize);
			}

			void SetCameraLensMultiplier(float multiplier)
			{
				cameraLensMultiplier = multiplier;
			}

			int32 GetInfiniteShadowMapSize(void) const
			{
				return (infiniteShadowMapSize);
			}

			int32 GetPointShadowMapSize(void) const
			{
				return (pointShadowMapSize);
			}

			int32 GetSpotShadowMapSize(void) const
			{
				return (spotShadowMapSize);
			}

			Texture *GetNullTexture(void) const
			{
				return (nullTexture);
			}

			const Render::TextureObject *GetPrimaryTexture(void) const
			{
				return (normalFrameBuffer->GetRenderTargetTexture(kRenderTargetPrimary));
			}

			const Render::TextureObject *GetReflectionTexture(void) const
			{
				return (normalFrameBuffer->GetRenderTargetTexture(kRenderTargetReflection));
			}

			const Render::TextureObject *GetRefractionTexture(void) const
			{
				return (normalFrameBuffer->GetRenderTargetTexture(kRenderTargetRefraction));
			}

			const Render::TextureObject *GetStructureTexture(void) const
			{
				return (normalFrameBuffer->GetRenderTargetTexture(kRenderTargetStructure));
			}

			const Render::TextureObject *GetVelocityTexture(void) const
			{
				return (normalFrameBuffer->GetRenderTargetTexture(kRenderTargetVelocity));
			}

			const Render::TextureObject *GetOcclusionTexture(void) const
			{
				return (normalFrameBuffer->GetRenderTargetTexture(kRenderTargetOcclusion2));
			}

			const Render::TextureObject *GetGlowBloomTexture(void) const
			{
				return (glowBloomFrameBuffer->GetRenderTargetTexture());
			}

			const Render::TextureObject *GetInfiniteShadowTexture(void) const
			{
				return (infiniteShadowFrameBuffer->GetRenderTargetTexture());
			}

			const Render::TextureObject *GetPointShadowTexture(void) const
			{
				return (pointShadowFrameBuffer->GetRenderTargetTexture());
			}

			const Render::TextureObject *GetSpotShadowTexture(void) const
			{
				return (spotShadowFrameBuffer->GetRenderTargetTexture());
			}

			unsigned_int32 GetRenderOptionFlags(void) const
			{
				return (renderOptionFlags);
			}

			int32 GetTextureDetailLevel(void) const
			{
				return (textureDetailLevel);
			}

			int32 GetPaletteDetailLevel(void) const
			{
				return (paletteDetailLevel);
			}

			int32 GetTextureFilterAnisotropy(void) const
			{
				return (textureFilterAnisotropy);
			}

			void SetOcclusionQuery(OcclusionQuery *query)
			{
				currentOcclusionQuery = query;
				query->BeginOcclusionQuery();
			}

			const Antivector4D& GetDistortionPlane(void) const
			{
				return (distortionPlane);
			}

			unsigned_int32 GetDiagnosticFlags(void) const
			{
				return (diagnosticFlags);
			}

			#if C4STATS

				int32 GetGraphicsCounter(int32 index) const
				{
					return (graphicsCounter[index]);
				}

			#endif

			void Draw(const List<Renderable> *renderList, bool group = false)
			{
				DrawContainer(renderList, group);
			}

			void Draw(const ImmutableArray<Renderable *> *renderList, bool group = false)
			{
				DrawContainer(renderList, group);
			}

			void DrawStructure(const List<Renderable> *renderList, bool group = false)
			{
				DrawStructureContainer(renderList, group);
			}

			void DrawStructure(const ImmutableArray<Renderable *> *renderList, bool group = false)
			{
				DrawStructureContainer(renderList, group);
			}

			void DrawWireframe(unsigned_int32 flags, const List<Renderable> *renderList)
			{
				DrawWireframeContainer(flags, renderList);
			}

			void DrawWireframe(unsigned_int32 flags, const ImmutableArray<Renderable *> *renderList)
			{
				DrawWireframeContainer(flags, renderList);
			}

			void DrawVectors(int32 arrayIndex, const List<Renderable> *renderList)
			{
				DrawVectorsContainer(arrayIndex, renderList);
			}

			void DrawVectors(int32 arrayIndex, const ImmutableArray<Renderable *> *renderList)
			{
				DrawVectorsContainer(arrayIndex, renderList);
			}

			template <typename objectType, typename dataType> static void SyncRenderTask(void (objectType::*proc)(const dataType *), objectType *object, const dataType *data = nullptr)
			{
				SyncRenderTask(reinterpret_cast<void (NullClass::*)(const void *)>(proc), reinterpret_cast<NullClass *>(object), data);
			}

			void SetSyncLoadFlag(bool flag);

			C4API static void SetShaderTime(float time, float delta);
			C4API static void SetImpostorDepthParams(float scale, float offset, float tangent);

			static void ResetShaders(void);

			void BeginRendering(void);
			void EndRendering(void);

			void Timestamp(TimestampType type);
			int32 GetTimestampData(TimestampType *type, unsigned_int64 *stamp) const;

			void SetFinalColorTransform(const ColorRGBA& scale, const ColorRGBA& bias);
			void SetFinalColorTransform(const ColorRGBA& red, const ColorRGBA& green, const ColorRGBA& blue, const ColorRGBA& bias);

			void SetRenderTarget(RenderTargetType type);
			void SetDisplayRenderTarget(void);
			void SetFullFrameRenderTarget(void);

			void CopyRenderTarget(Texture *texture, const Rect& rect);

			void ActivateOrthoCamera(void);
			void ActivateFrustumCamera(void);
			void ActivateRemoteCamera(void);

			C4API void SetRenderOptionFlags(unsigned_int32 flags);
			C4API void SetDiagnosticFlags(unsigned_int32 flags);

			void SetCamera(const CameraObject *camera, const Transformable *transformable, unsigned_int32 clearMask = ~0, bool reset = true);
			void SetFogSpace(const FogSpaceObject *fogSpace, const Transformable *transformable);

			void SetAmbientLight(void);
			void SetAmbientDepthLessEqual(bool equal);

			void BeginClip(const Rect& rect);
			void EndClip(void);

			void SetInfiniteLight(const InfiniteLightObject *light, const Transformable *transformable, const LightShadowData *shadowData, bool unified = false);

			ProjectionResult CalculatePointLightBounds(const PointLightObject *light, const Transformable *transformable, Rect *lightBounds, Range<float> *depthBounds);
			void SetPointLight(const PointLightObject *light, const Transformable *transformable, float colorMultiplier, ProjectionResult projection, const Rect *lightBounds, const Range<float> *depthBounds, bool shadow);
			void SetSpotLight(const SpotLightObject *light, const Transformable *transformable, float colorMultiplier, ProjectionResult projection, const Rect *lightBounds, const Range<float> *depthBounds, bool shadow);

			C4API void Sort(List<Renderable> *renderList);

			bool BeginStructureRendering(const Transform4D& previousCameraWorldTransform, unsigned_int32 structureFlags, float velocityScale);
			void EndStructureRendering(void);

			bool BeginDistortionRendering(void);
			void EndDistortionRendering(void);
			void DrawDistortion(const List<Renderable> *renderList);

			void BeginInfiniteShadow(void);
			void EndInfiniteShadow(void);
			void SetInfiniteShadowCascade(int32 cascade);
			void DiscardInfiniteShadow(void);

			void BeginPointShadow(void);
			void EndPointShadow(void);
			void SetPointShadowFace(int32 face);
			void DiscardPointShadow(void);

			void BeginSpotShadow(void);
			void EndSpotShadow(void);
			void DiscardSpotShadow(void);

			void DrawShadow(const ImmutableArray<Renderable *>& shadowArray);

			void ProcessOcclusionQueries(void);

			C4API static void ReadImageBuffer(const Rect& rect, Color4C *image, int32 rowPixels);
	};


	C4API extern GraphicsMgr *TheGraphicsMgr;
}


#endif

// ZYUQURM
