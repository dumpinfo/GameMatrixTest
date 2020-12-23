 

#ifndef C4Renderable_h
#define C4Renderable_h


//# \component	Graphics Manager
//# \prefix		GraphicsMgr/

//# \import		C4MaterialObjects.h


#include "C4FragmentShaders.h"
#include "C4GeometryShaders.h"
#include "C4MaterialObjects.h"
#include "C4Computation.h"


namespace C4
{
	enum
	{
		kMaxShaderRegisterCount			= 24,
		kMaxShaderInterpolantCount		= 32,
		kMaxShaderConstantCount			= 8,
		kMaxShaderStateDataCount		= 10 + kMaxShaderConstantCount
	};


	enum
	{
		kMaxRenderParameterCount		= 6,
		kMaxTexcoordParameterCount		= 1,
		kMaxTerrainParameterCount		= 2
	};


	//# \enum	VertexBufferIndex

	enum
	{
		kVertexBufferAttributeArray		= 0,		//## The primary vertex buffer containing vertex attributes.
		kVertexBufferAttributeArray0	= 0,		//## An alternate name for the primary vertex buffer containing vertex attributes.
		kVertexBufferAttributeArray1	= 1,		//## The secondary vertex buffer containing vertex attributes.
		kVertexBufferIndexArray			= 2,		//## The vertex buffer containing vertex indexes.
		kVertexBufferCount				= 3
	};


	//# \enum	VertexBufferFlags

	enum
	{
		kVertexBufferAttribute			= 0,		//## The vertex buffer contains vertex attribute data.
		kVertexBufferIndex				= 1 << 0,	//## The vertex buffer contains vertex index data.
		kVertexBufferStatic				= 0,		//## The vertex buffer data is static and will be written only once.
		kVertexBufferDynamic			= 1 << 1	//## The vertex buffer data is dynamic and will be written multiple times.
	};


	//# \enum	RenderState

	enum
	{
		kRenderDepthTest				= 1 << 0,	//## The depth test is enabled. If the depth test is disabled, then depth is also not written.
		kRenderColorInhibit				= 1 << 1,	//## Writes to the color buffer are disabled.
		kRenderDepthInhibit				= 1 << 2,	//## Writes to the depth buffer are disabled.
		kRenderDepthOffset				= 1 << 3,	//## Apply depth offset in the projection matrix. See the $@Renderable::SetDepthOffset@$ function.
		kRenderPolygonOffset			= 1 << 4,	//## Apply minimal viewport-space polygon offset using the graphics hardware.
		kRenderWireframe				= 1 << 5	//## Render wireframe instead of filled polygons.
	};


	//# \enum	RenderableFlags

	enum
	{
		kRenderableCameraTransformInhibit	= 1 << 0,	//## Do not apply the world-space to camera-space transformation.
		kRenderableStructureBufferInhibit	= 1 << 1,	//## Do not render into the structure buffer at all.
		kRenderableStructureVelocityZero	= 1 << 2,	//## Always render zero as the velocity in the structure buffer.
		kRenderableStructureDepthZero		= 1 << 3,	//## Always render zero as the depth in the structure buffer. $kRenderableStructureVelocityZero$ must also be set.
		kRenderableMotionBlurGradient		= 1 << 4,	//## Account for the depth gradient when rendering motion blur.
		kRenderableFogInhibit				= 1 << 5,	//## Do not render with fog.
		kRenderableUnfog					= 1 << 6
	};


	//# \enum	ShaderFlags

	enum
	{
		kShaderAmbientEffect				= 1 << 0,
		kShaderNormalizeBasisVectors		= 1 << 1,	//## Normalize the vertex-space basis vectors (normal and tangent).
		kShaderCubeLightInhibit				= 1 << 2,	//## Render using point light shaders when illuminated by a cube light.
		kShaderAlphaFogFraction				= 1 << 3,
		kShaderFireArrays					= 1 << 4,
		kShaderDistortion					= 1 << 5,
		kShaderGenerateTexcoord				= 1 << 6,
		kShaderGenerateTangent				= 1 << 7,
		kShaderTerrainBorder				= 1 << 8,
		kShaderWaterElevation				= 1 << 9,
		kShaderColorArrayInhibit			= 1 << 10,
		kShaderProjectiveTexture			= 1 << 11, 
		kShaderVertexInfinite				= 1 << 16,
		kShaderVertexBillboard				= 1 << 17,
		kShaderVertexPostboard				= 1 << 18, 
		kShaderVertexPolyboard				= 1 << 19,
		kShaderLinearPolyboard				= 1 << 20, 
		kShaderOrthoPolyboard				= 1 << 21,
		kShaderScaleVertex					= 1 << 22,
		kShaderOffsetVertex					= 1 << 23, 
		kShaderNormalExpandVertex			= 1 << 24,
		kShaderTexcoordVertex				= 1 << 25 
	}; 


	enum
	{ 
		kArrayPosition				= 0,
		kArrayPosition0				= 0,
		kArrayPosition1				= 1,
		kArrayNormal				= 2,
		kArrayColor					= 3,
		kArrayColor0				= 3,
		kArrayColor1				= 4,
		kArrayColor2				= 5,
		kArrayTangent				= 6,
		kArrayRadius				= 6,
		kArrayOffset				= 6,
		kArrayTexcoord				= 8,
		kArrayTexcoord0				= 8,
		kArrayTexcoord1				= 9,
		kArrayTexcoord2				= 10,
		kArrayTexcoord3				= 11,
		kArrayPrevious				= 12,
		kArrayVelocity				= 13,
		kArrayBillboard				= 14,
		kMaxAttributeArrayCount		= 16
	};


	//# \enum	BlendFactor

	enum BlendFactor
	{
		kBlendZero					= 0,		//## Zero.
		kBlendOne					= 1,		//## One.
		kBlendSourceColor			= 2,		//## Source color.
		kBlendDestColor				= 3,		//## Destination color.
		kBlendConstColor			= 4,
		kBlendSourceAlpha			= 5,		//## Source alpha.
		kBlendDestAlpha				= 6,		//## Destination alpha.
		kBlendConstAlpha			= 7,
		kBlendInvSourceColor		= 8,		//## One minus source color.
		kBlendInvDestColor			= 9,		//## One minus destination color.
		kBlendInvConstColor			= 10,
		kBlendInvSourceAlpha		= 11,		//## One minus source alpha.
		kBlendInvDestAlpha			= 12,		//## One minus destination alpha.
		kBlendInvConstAlpha			= 13,
		kBlendFactorCount
	};


	//# \enum	BlendState

	enum : unsigned_int32
	{
		kBlendReplace				= (kBlendZero << 4) | kBlendOne,						//## Replace the destination color with the source color.
		kBlendAccumulate			= (kBlendOne << 4) | kBlendOne,							//## Add the source color to the destination color.
		kBlendModulate				= (kBlendZero << 4) | kBlendDestColor,					//## Multiply the destination color by the source color.
		kBlendInterpolate			= (kBlendInvSourceAlpha << 4) | kBlendSourceAlpha,		//## Interpolate between the source and destination colors using the source alpha.
		kBlendPremultInterp			= (kBlendInvSourceAlpha << 4) | kBlendOne,				//## Add the source color to the destination color multiplied by the source alpha.
		kBlendAlphaPreserve			= (kBlendOne << 12) | (kBlendZero << 8),
		kBlendAlphaReplace			= (kBlendZero << 12) | (kBlendOne << 8),
		kBlendAlphaAccumulate		= (kBlendOne << 12) | (kBlendOne << 8),

		kBlendColorMask				= 0x00FF,
		kBlendAlphaMask				= 0xFF00
	};


	enum ShaderType
	{
		kShaderNone = -1,
		kShaderAmbient,
		kShaderAmbientRadiosity,
		kShaderUnified,
		kShaderUnifiedRadiosity,
		kShaderInfiniteLight,
		kShaderPointLight,
		kShaderCubeLight,
		kShaderSpotLight,
		kShaderShadow,
		kShaderStructure,
		kShaderTypeCount,

		kShaderFirstAmbient = kShaderAmbient,
		kShaderLastAmbient = kShaderAmbientRadiosity,
		kShaderFirstUnified = kShaderUnified,
		kShaderLastUnified = kShaderUnifiedRadiosity,
		kShaderFirstLight = kShaderInfiniteLight,
		kShaderLastLight = kShaderSpotLight,
		kShaderFirstPointLight = kShaderPointLight,
		kShaderLastPointLight = kShaderSpotLight,
		kShaderFirstPlain = kShaderShadow,
		kShaderLastPlain = kShaderStructure
	};

	static_assert(kShaderAmbientRadiosity - kShaderAmbient == kShaderUnifiedRadiosity - kShaderUnified, "Deltas for radiosity shader types must match for ambient and unified");


	enum ShaderVariant
	{
		kShaderVariantNormal,
		kShaderVariantConstantFog,
		kShaderVariantLinearFog,
		kShaderVariantCount
	};


	//# \enum	RenderType

	enum RenderType
	{
		kRenderPointSprites,				//## A set of <i>n</i> point sprites.
		kRenderPoints,						//## A set of <i>n</i> points to be amplified in a geometry shader.
		kRenderLines,						//## A set of <i>n</i>&nbsp;/&nbsp;2 unconnected line segments.
		kRenderLineStrip,					//## A set of <i>n</i>&nbsp;&minus;&nbsp;1 connected line segments.
		kRenderLineLoop,					//## A set of <i>n</i> connected line segments forming a closed loop.
		kRenderIndexedLines,				//## A set of line segments with indexed vertices.
		kRenderTriangles,					//## A set of <i>n</i>&nbsp;/&nbsp;3 unconnected triangles.
		kRenderTriangleStrip,				//## A set of <i>n</i>&nbsp;&minus;&nbsp;2 triangles connected as a strip.
		kRenderIndexedTriangles,			//## A set of triangles with indexed vertices.
		kRenderQuads,						//## A set of <i>n</i>&nbsp;/&nbsp;4 unconnected quads.
		kRenderMultiIndexedTriangles,
		kRenderMaskedMultiIndexedTriangles
	};


	enum
	{
		kShaderStateCameraPosition			= 1 << 0,
		kShaderStateCameraPosition4D		= 1 << 1,
		kShaderStateCameraDirections		= 1 << 2,
		kShaderStateCameraTransform			= 1 << 3,
		kShaderStateWorldTransform			= 1 << 4,
		kShaderStatePaintTransform			= 1 << 5,
		kShaderStateVertexScaleOffset		= 1 << 6,
		kShaderStateTerrainBorder			= 1 << 7,
		kShaderStateImpostorTransition		= 1 << 8,
		kShaderStateGeometryTransition		= 1 << 9,
		kShaderStateBaseTexcoord			= 1 << 10,
		kShaderStateTexcoordGenerate		= 1 << 11,
		kShaderStateTexcoordTransform0		= 1 << 12,
		kShaderStateTexcoordTransform1		= 1 << 13,
		kShaderStateTexcoordVelocity0		= 1 << 14,
		kShaderStateTexcoordVelocity1		= 1 << 15,
		kShaderStateTerrainTexcoordScale	= 1 << 16
	};


	class Box3D;
	class Renderable;
	class RadiositySpaceObject;


	inline unsigned_int32 BlendState(BlendFactor sc, BlendFactor dc, BlendFactor sa = kBlendZero, BlendFactor da = kBlendZero)
	{
		return ((da << 12) | (sa << 8) | (dc << 4) | sc);
	}

	inline BlendFactor GetBlendSource(unsigned_int32 state)
	{
		return (static_cast<BlendFactor>(state & 0x0F));
	}

	inline BlendFactor GetBlendDest(unsigned_int32 state)
	{
		return (static_cast<BlendFactor>((state >> 4) & 0x0F));
	}

	inline BlendFactor GetBlendSourceAlpha(unsigned_int32 state)
	{
		return (static_cast<BlendFactor>((state >> 8) & 0x0F));
	}

	inline BlendFactor GetBlendDestAlpha(unsigned_int32 state)
	{
		return (static_cast<BlendFactor>((state >> 12) & 0x0F));
	}


	struct PaintEnvironment
	{
		Transform4D						paintTransform;
		const Texture					*const *paintTexture;
	};


	struct AmbientEnvironment
	{
		int32							ambientShaderTypeDelta;
		const ColorRGBA					*ambientLightColor;

		const RadiositySpaceObject		*radiositySpaceObject;
		const Transformable				*radiositySpaceTransformable;

		const Texture					*const *environmentMap;
	};


	struct ShaderKey
	{
		private:

			unsigned_int32		keyValue;

		public:

			ShaderKey(const ShaderKey& key) = default;

			ShaderKey(ShaderVariant variant, int32 level, bool shadow = false)
			{
				keyValue = (variant << 3) | (shadow << 2) | level;
			}

			bool operator ==(const ShaderKey& key) const
			{
				return (keyValue == key.keyValue);
			}

			ShaderVariant GetShaderVariant(void) const
			{
				return (static_cast<ShaderVariant>((keyValue >> 3) & 0x03));
			}

			int32 GetDetailLevel(void) const
			{
				return (keyValue & 0x03);
			}

			bool GetShadowFlag(void) const
			{
				return ((keyValue & 0x04) != 0);
			}
	};


	//# \class	VertexBuffer		Encapsulates a buffer that stores GPU-accessible vertex data.
	//
	//# The $VertexBuffer$ class encapsulates a buffer that stores GPU-accessible vertex data.
	//
	//# \def	class VertexBuffer : public Render::VertexBufferObject, public ListElement<VertexBuffer>
	//
	//# \ctor	VertexBuffer(unsigned_int32 flags);
	//
	//# \param	flags	The vertex buffer flags.
	//
	//# \desc
	//# The $VertexBuffer$ class encapsulates a buffer that stores GPU-accessible vertex attribute data
	//# or vertex index data. The $flags$ parameter should be a combination of the following constants specifying
	//# which kind of data will be stored in the vertex buffer and how the vertex buffer will be used.
	//
	//# \table	VertexBufferFlags
	//
	//# The $flags$ parameter should be set to the logical OR of exactly two values from this table. The first
	//# value must be either $kVertexBufferAttribute$ or $kVertexBufferIndex$, and the second value must be
	//# either $kVertexBufferStatic$ or $kVertexBufferDynamic$.
	//
	//# \privbase	Render::VertexBufferObject				Used internally by the Graphics Manager.
	//# \base		Utilities/ListElement<VertexBuffer>		Used internally by the Graphics Manager.
	//
	//# \also	$@SharedVertexBuffer@$
	//# \also	$@Renderable::SetVertexBuffer@$


	//# \function	VertexBuffer::Establish		Establishes the size of a vertex buffer and initializes it.
	//
	//# \proto	void Establish(unsigned_int32 size, const void *data = nullptr);
	//
	//# \param	size	The size, in bytes, of the data that will be stored in the vertex buffer.
	//# \param	dat		A pointer to the data that should initially be loaded into the vertex buffer.
	//
	//# \desc
	//# The $Establish$ function establishes the size of a vertex buffer and initializes its internal resources.
	//# This function must be called before data is written to the vertex buffer with the $@VertexBuffer::BeginUpdate@$
	//# or $@VertexBuffer::UpdateBuffer@$ functions.
	//#
	//# If the $data$ parameter is not $nullptr$, then $size$ bytes are copied from the memory to which $data$ points
	//# into the vertex buffer. Otherwise, the contents of the vertex buffer are initially undefined. For a static
	//# vertex buffer, performance is best when the buffer data is supplied with the $Establish$ function.
	//#
	//# The $size$ parameter should specify the maximum number of bytes of data that will be stored in the buffer.
	//# If the $size$ parameter is zero, then the vertex buffer's internal resources are deallocated, and the vertex
	//# buffer returns to the state it was in before the $Establish$ function was first called. If the $size$ parameter
	//# is equal to the same value that was specified for a preceding call to the $Establish$ function for the same
	//# vertex buffer, then the function quickly returns without reallocating any internal resources.
	//#
	//# The $Establish$ function can be called from any thread, but the caller must use synchronization mechanisms to
	//# ensure that the $Establish$ function is not called for the same vertex buffer from multiple threads simultaneously.
	//
	//# \also	$@VertexBuffer::BeginUpdate@$
	//# \also	$@VertexBuffer::UpdateBuffer@$
	//# \also	$@VertexBuffer::Active@$
	//# \also	$@VertexBuffer::GetVertexBufferSize@$
	//# \also	$@VertexBuffer::GetVertexStride@$


	//# \function	VertexBuffer::Active		Returns a boolean value indicating whether a vertex buffer is active.
	//
	//# \proto	bool Active(void) const;
	//
	//# \desc
	//# The $Active$ function returns $true$ if resources have been allocated for a vertex buffer through a previous
	//# call to the $@VertexBuffer::Establish@$ function. If the $Establish$ function has not been called, or the most
	//# recent call specified zero for the $size$ parameter, then the $Active$ function returns $false$.
	//
	//# \also	$@VertexBuffer::Establish@$


	//# \function	VertexBuffer::GetVertexBufferSize		Returns the size of a vertex buffer.
	//
	//# \proto	unsigned_int32 GetVertexBufferSize(void) const;
	//
	//# \desc
	//# The $GetVertexBufferSize$ function returns the maximum size of the data that can be stored in a vertex buffer.
	//# If the $@VertexBuffer::Establish@$ function has previously been called for a vertex buffer, then the return
	//# value of the $GetVertexBufferSize$ function is the value that was passed to it as the $size$ parameter.
	//# Otherwise, the return value of the $GetVertexBufferSize$ function is zero.
	//
	//# \also	$@VertexBuffer::Establish@$
	//# \also	$@VertexBuffer::GetVertexStride@$


	//# \function	VertexBuffer::GetVertexStride		Returns the vertex stride for a vertex buffer.
	//
	//# \proto	unsigned_int32 GetVertexStride(void) const;
	//
	//# \desc
	//# The $GetVertexStride$ function returns the vertex stride that was previously specified by the
	//# $@VertexBuffer::Establish@$ function. If the $Establish$ function has not been called yet for a vertex
	//# buffer, then the return value of the $GetVertexStride$ function is undefined.
	//
	//# \also	$@VertexBuffer::Establish@$
	//# \also	$@VertexBuffer::GetVertexBufferSize@$


	//# \function	VertexBuffer::BeginUpdate		Maps a vertex buffer and returns a pointer to its storage.
	//
	//# \proto	template <typename type> volatile type *BeginUpdate(void);
	//# \proto	template <typename type> volatile type *BeginUpdateSync(void);
	//
	//# \tparam	type	The type to which the return value points.
	//
	//# \desc
	//# The $BeginUpdate$ function maps a vertex buffer's storage into CPU-accessible memory and returns a pointer
	//# to the storage buffer. This function should only be called after the $@VertexBuffer::Establish@$ function
	//# has been called for the vertex buffer with a nonzero $size$ parameter, and the maximum number of bytes that
	//# can be written to the buffer returned by $BeginUpdate$ is the number that was specified by the $size$ parameter.
	//#
	//# The $BeginUpdateSync$ variant of the $BeginUpdate$ function should normally be called to ensure that the GPU
	//# driver is being accessed properly in multithreaded contexts. The $BeginUpdate$ function should only be called
	//# if it's a certainty that it is being called from the main thread. Both functions have the same effect.
	//#
	//# When a vertex buffer is mapped into CPU-accessible memory with either the $BeginUpdate$ or $BeginUpdateSync$
	//# function, the data in the vertex buffer is completely destroyed. Any vertex data that will be later read from
	//# the vertex buffer by the GPU must be written to the buffer in its entirety. It is thus not possible to update
	//# only the texture coordinates for a vertex array without also rewriting the vertex positions, for example.
	//# In the case that vertices contain some data that is static and some data that is dynamic, two separate vertex
	//# buffers should be used. (The vertex buffer data is destroyed as an optimization that prevents an expensive
	//# copy of the existing data into the CPU-accessible buffer.)
	//#
	//# Once the data has been written to a vertex buffer, the $@VertexBuffer::EndUpdate@$ must be called to unmap
	//# the storage buffer before any rendering commands referencing the vertex buffer are issued.
	//#
	//# For a static vertex buffer, the $BeginUpdate$ function should be called at most one time and only if no buffer
	//# data was supplied with the $@VertexBuffer::Establish@$ function. Otherwise, performance may be degraded.
	//# Dynamic vertex buffers can be updated as often as needed without any performance penalty.
	//
	//# \special
	//# Data should be written to the buffer returned by the $BeginUpdate$ function contiguously and in order. This
	//# promotes the most efficient writeback performance for memory that is possibly write-combined. The pointer
	//# returned by the $BeginUpdate$ function is declared $volatile$ to prevent the compiler from changing the
	//# program order of individual writes.
	//#
	//# Since the vertex buffer cannot be accessed through any pointers other than the one returned by the $BeginUpdate$
	//# function, the variable to which it's assigned should be declared $restrict$ in order to allow the compiler to
	//# perform optimizations that it may not otherwise be able to perform with respect to reads from ordinary program data.
	//# For example, the following code should be used to cast the return value of the $BeginUpdateSync$ function, called
	//# for a $VertexBuffer$ object named $vertexBuffer$, to a pointer to $Point3D$ and declare it as restricted.
	//
	//# \source
	//# volatile Point3D *restrict vertex = vertexBuffer->BeginUpdateSync<Point3D>();
	//
	//# \also	$@VertexBuffer::EndUpdate@$
	//# \also	$@VertexBuffer::UpdateBuffer@$


	//# \function	VertexBuffer::EndUpdate		Unmaps a vertex buffer.
	//
	//# \proto	void EndUpdate(void);
	//# \proto	void EndUpdateSync(void);
	//
	//# \desc
	//# The $EndUpdate$ function unmaps the storage for a vertex buffer that was previously mapped using the
	//# $@VertexBuffer::BeginUpdate@$ function. Once a vertex buffer is unmapped, it is no longer legal to write to
	//# the buffer returned by the $BeginUpdate$ function.
	//#
	//# The $EndUpdateSync$ variant of the $EndUpdate$ function should normally be called to ensure that the GPU
	//# driver is being accessed properly in multithreaded contexts. The $EndUpdate$ function should only be called
	//# if it's a certainty that it is being called from the main thread. Both functions have the same effect.
	//
	//# \also	$@VertexBuffer::BeginUpdate@$
	//# \also	$@VertexBuffer::UpdateBuffer@$


	//# \function	VertexBuffer::UpdateBuffer		Copies data to a vertex buffer.
	//
	//# \proto	void UpdateBuffer(unsigned_int32 offset, unsigned_int32 size, const void *data);
	//# \proto	void UpdateBufferSync(unsigned_int32 offset, unsigned_int32 size, const void *data);
	//
	//# \param	offset		The base offset within the buffer to which the data will be copied, in bytes.
	//# \param	size		The size of the data to copy into the buffer, in bytes.
	//# \param	data		A pointer to the data that will be copied into the buffer.
	//
	//# \desc
	//# The $UpdateBuffer$ function copies the data specified by the $data$ parameter into a vertex buffer.
	//# The number of bytes copied is specified by the $size$ parameter, and the base offset within the vertex
	//# buffer to which the data is copied is specified by the $offset$ parameter. The sum of the $offset$ and
	//# $size$ parameters must not exceed the total size of the vertex buffer, as specified by the value of the
	//# $size$ parameter passed to the $@VertexBuffer::Establish@$ function.
	//#
	//# The $UpdateBufferSync$ variant of the $UpdateBuffer$ function should normally be called to ensure that the GPU
	//# driver is being accessed properly in multithreaded contexts. The $UpdateBuffer$ function should only be called
	//# if it's a certainty that it is being called from the main thread. Both functions have the same effect.
	//#
	//# The $UpdateBuffer$ function cannot be called for static vertex buffers. For a vertex buffer constructed
	//# with the $kVertexBufferStatic$ flag, either the buffer data should be supplied with the $Establish$ function
	//# or the buffer data should be loaded into the buffer one time using the $@VertexBuffer::BeginUpdate@$ and
	//# $@VertexBuffer::EndUpdate@$ functions.
	//
	//# \also	$@VertexBuffer::BeginUpdate@$
	//# \also	$@VertexBuffer::EndUpdate@$


	class VertexBuffer : public Render::VertexBufferObject, public ListElement<VertexBuffer>
	{
		private:

			bool						activeFlag;
			char						*bufferStorage;

			static int32				totalVertexBufferCount;
			static unsigned_int32		totalVertexBufferMemory;

			static Mutex				vertexBufferMutex;
			static List<VertexBuffer>	vertexBufferList;

			void Activate(const void *data);
			void Deactivate(void);

			void Save(void);
			void Restore(void);

		public:

			C4API VertexBuffer(unsigned_int32 flags);
			C4API ~VertexBuffer();

			bool Active(void) const
			{
				return (activeFlag);
			}

			static int32 GetTotalVertexBufferCount(void)
			{
				return (totalVertexBufferCount);
			}

			static unsigned_int32 GetTotalVertexBufferMemory(void)
			{
				return (totalVertexBufferMemory);
			}

			C4API void Establish(unsigned_int32 size, const void *data = nullptr);

			C4API volatile void *BeginUpdateSync(void);
			C4API void EndUpdateSync(void);
			C4API void UpdateBufferSync(unsigned_int32 offset, unsigned_int32 size, const void *data);

			template <typename type> volatile type *BeginUpdateSync(void)
			{
				return (static_cast<volatile type *>(BeginUpdateSync()));
			}

			static void SaveAll(void);
			static void DeactivateAll(void);
			static void ReactivateAll(void);
	};


	//# \class	SharedVertexBuffer		Encapsulates a reference-counted vertex buffer.
	//
	//# The $SharedVertexBuffer$ class encapsulates a reference-counted vertex buffer.
	//
	//# \def	class SharedVertexBuffer : public VertexBuffer
	//
	//# \ctor	SharedVertexBuffer(unsigned_int32 flags);
	//
	//# \param	flags	The vertex buffer flags. See the $@VertexBuffer@$ class.
	//
	//# \desc
	//# The $SharedVertexBuffer$ class is a vertex buffer that has a reference count so that it
	//# can safely be shared among multiple renderable objects. A shared vertex buffer is typically
	//# used when some vertex attribute or index data is identical for multiple instances of a
	//# renderable object of some kind.
	//#
	//# When a shared vertex buffer object is first constructed, there are no references to it,
	//# and its reference count is zero. The $@SharedVertexBuffer::Retain@$ function is used to
	//# add a new reference to the vertex buffer, and the $@SharedVertexBuffer::Release@$ function
	//# is used to remove a reference. The $Retain$ function is typically called in the constructor
	//# for some class, and the $Release$ function is typically called in its destructor.
	//
	//# \base	VertexBuffer	A $SharedVertexBuffer$ is a special type of vertex buffer.


	//# \function	SharedVertexBuffer::Retain		Retains a reference to a shared vertex buffer.
	//
	//# \proto	int32 Retain(void);
	//
	//# \desc
	//# The $Retain$ function increments the reference count for a shared vertex buffer and returns
	//# the new reference count. This function takes no other action.
	//#
	//# If the returned reference count is one, then the caller has retained the first reference to the
	//# shared vertex buffer. In this case, the calling code should allocate the buffer's resources using
	//# the $@VertexBuffer::Establish@$ function and define the vertex buffer's data using either the
	//# $@VertexBuffer::BeginUpdate@$ function or $@VertexBuffer::UpdateBuffer@$ function.
	//#
	//# When the owner of the retained reference is finished using the vertex buffer, it should call the
	//# $@SharedVertexBuffer::Release@$ to release its reference, allowing the vertex buffer's resources
	//# to be deallocated if there are no other references.
	//#
	//# The initial reference count for a shared vertex buffer is zero.
	//
	//# \also	$@SharedVertexBuffer::Release@$


	//# \function	SharedVertexBuffer::Release		Releases a reference to a shared vertex buffer.
	//
	//# \proto	int32 Release(void);
	//
	//# \desc
	//# The $Release$ function decrements the reference count for a shared vertex buffer and returns
	//# the new reference count. If the new reference count becomes zero, then the $Release$ function
	//# also deallocates the vertex buffer's resources by calling the $@VertexBuffer::Establish@$ function
	//# with a $size$ parameter of zero before returning.
	//#
	//# The $Release$ function should be called to balance each previous call to the $@SharedVertexBuffer::Retain@$
	//# function for the same shared vertex buffer.
	//
	//# \also	$@SharedVertexBuffer::Retain@$


	class SharedVertexBuffer : public VertexBuffer
	{
		private:

			int32		referenceCount;

		public:

			C4API SharedVertexBuffer(unsigned_int32 flags);
			C4API ~SharedVertexBuffer();

			int32 Retain(void)
			{
				return (++referenceCount);
			}

			C4API int32 Release(void);
	};


	class VertexData : public Render::VertexDataObject
	{
		friend class GraphicsMgr;

		private:

			int8		arrayIndex[kVertexAttribCount];

		public:

			VertexData();

			void SetArrayIndex(int32 shaderIndex, int32 renderIndex)
			{
				arrayIndex[shaderIndex] = (int8) renderIndex;
			}

			void Update(const Renderable *renderable);
	};


	//# \class	OcclusionQuery		Represents an occlusion query operation.
	//
	//# The $OcclusionQuery$ class represents an occlusion query operation.
	//
	//# \def	class OcclusionQuery : public ListElement<OcclusionQuery>
	//
	//# \ctor	OcclusionQuery(RenderProc *proc, void *cookie);
	//
	//# \param	proc	A pointer to the occlusion query's callback procedure.
	//# \param	cookie	The cookie that is passed to the callback procedure as its last parameter.
	//
	//# \desc
	//# The $OcclusionQuery$ class represents an occlusion query operation that can be attached to a renderable
	//# object. When such a renderable object is rendered as an effect, the GPU counts how many fragments pass
	//# the depth test. This information is reported through a callback procedure later so that it can be used
	//# to alter the appearance of some other renderable object. The callback procedure is specified by the $proc$
	//# parameter, which must point to a function having the following signature defined by the $RenderProc$ type.
	//
	//# \code	typedef void RenderProc(OcclusionQuery *, List<Renderable> *, void *);
	//
	//# The $cookie$ parameter specifies a user-defined pointer that is passed to the callback procedure as its
	//# last parameter.
	//#
	//# An occlusion query is attached to a $@Renderable@$ object by calling the $@Renderable::SetOcclusionQuery@$
	//# function before the renderable object is first rendered. The renderable object is typically created
	//# with the render flags $kRenderDepthTest$, $kRenderColorInhibit$, and $kRenderDepthInhibit$ all set so
	//# that the depth test is performed, but nothing is written to the color buffer or depth buffer.
	//#
	//# When the callback procedure is called, the first parameter is a pointer to the occlusion query object,
	//# and the second parameter points to a list of renderable objects. The callback procedure should call the
	//# $@OcclusionQuery::GetUnoccludedArea@$ function to determine the normalized viewport area that passed the
	//# depth test when the occlusion query was rendered and optionally add items to the list of renderable
	//# objects after making adjustments to their appearance as necessary depending on the outcome of the
	//# occlusion query.
	//#
	//# The occluded area returned by the $GetUnoccludedArea$ function is reported as the fraction of the entire
	//# viewport area corresponding to the number of fragments that passed the depth test. It is always in the
	//# range [0.0,&nbsp;1.0], where a value of 0.0 means that no fragments passed the depth test, and a value
	//# of 1.0 means that the entire viewport was filled with fragments that did pass the depth test.
	//
	//# \base	Utilities/ListElement<OcclusionQuery>	Used internally by the Graphics Manager.
	//
	//# \also	$@Renderable::GetOcclusionQuery@$
	//# \also	$@Renderable::SetOcclusionQuery@$


	//# \function	OcclusionQuery::GetRenderCookie		Returns the cookie stored in an occlusion query object.
	//
	//# \proto	void *GetRenderCookie(void) const;
	//
	//# \desc
	//# The $GetRenderCookie$ function returns the cookie stored in an occlusion query object. The cookie is set
	//# by the constructor of the $OcclusionQuery$ class or by the $@OcclusionQuery::SetRenderProc@$ function.
	//
	//# \also	$@OcclusionQuery::SetRenderProc@$


	//# \function	OcclusionQuery::SetRenderProc		Sets the callback procedure for an occlusion query object.
	//
	//# \proto	void SetRenderProc(RenderProc *proc, void *cookie);
	//
	//# \param	proc	A pointer to the occlusion query's callback procedure.
	//# \param	cookie	The cookie that is passed to the callback procedure as its last parameter.
	//
	//# \desc
	//# The $SetRenderProc$ function sets the callback procedure for an occlusion query object to the function
	//# specified by the $proc$ parameter. The $RenderProc$ type is defined as follows.
	//
	//# \code	typedef void RenderProc(OcclusionQuery *, List<Renderable> *, void *);
	//
	//# The $cookie$ parameter specifies a user-defined pointer that is passed to the callback procedure as its
	//# last parameter.
	//#
	//# See the $@OcclusionQuery@$ class for information about how this callback procedure is used.
	//
	//# \also	$@OcclusionQuery::GetRenderCookie@$


	//# \function	OcclusionQuery::GetUnoccludedArea		Returns the area that was unoccluded during an occlusion query.
	//
	//# \proto	float GetUnoccludedArea(void) const;
	//
	//# \desc
	//# The $GetUnoccludedArea$ function returns the area that was unoccluded during an occlusion query as a
	//# fraction of the total viewport area for the current camera. The return value is always in the range [0.0,&nbsp;1.0],
	//# where a value of 0.0 means that no fragments passed the depth test, and a value of 1.0 means that the entire
	//# viewport was filled with fragments that did pass the depth test.
	//
	//# \also	$@OcclusionQuery::GetRenderCookie@$


	class OcclusionQuery : public ListElement<OcclusionQuery>
	{
		friend class GraphicsMgr;

		public:

			typedef void RenderProc(OcclusionQuery *, List<Renderable> *, void *);

		private:

			bool							activeFlag;
			float							unoccludedArea;

			RenderProc						*renderProc;
			void							*renderCookie;

			unsigned_int32					queryIndex;
			unsigned_int32					queryFrame[4];
			Render::QueryObject				queryObject[4];

			static List<OcclusionQuery>		occlusionQueryList;

		public:

			OcclusionQuery(RenderProc *proc, void *cookie);
			~OcclusionQuery();

			float GetUnoccludedArea(void) const
			{
				return (unoccludedArea);
			}

			void *GetRenderCookie(void) const
			{
				return (renderCookie);
			}

			void SetRenderProc(RenderProc *proc, void *cookie)
			{
				renderProc = proc;
				renderCookie = cookie;
			}

			void Activate(void);
			void Deactivate(void);

			static void DeactivateAll(void);
			static void ReactivateAll(void);

			void BeginOcclusionQuery(void);
			void EndOcclusionQuery(void);

			void Process(List<Renderable> *renderList, float normalizer);
	};


	class ShaderData : public ListElement<ShaderData>, public Memory<ShaderData>
	{
		friend class RenderSegment;

		private:

			ShaderKey					shaderKey;

			ShaderData					**shaderDataPointer;
			ShaderData					*nextShaderData;

			static List<ShaderData>		shaderDataList;

		public:

			unsigned_int32				blendState;
			unsigned_int32				materialState;

			ShaderProgram				*shaderProgram;
			VertexData					vertexData;

			TextureArray				textureArray;

			int32						shaderStateDataCount;
			ShaderStateData				shaderStateData[kMaxShaderStateDataCount];

			ShaderData(const ShaderKey& key, ShaderData **pointer, unsigned_int32 blend, unsigned_int32 material);
			~ShaderData();

			void SetTextureObject(int32 index, const Render::TextureObject *texture)
			{
				textureArray.textureHandle[index] = texture->GetTextureHandle();
				textureArray.textureTarget[index] = (int8) texture->GetTextureTargetIndex();
			}

			static void Purge(void)
			{
				shaderDataList.Purge();
			}

			void AddStateProc(ShaderStateProc *proc, const void *cookie = nullptr);

			void Preprocess(void);
	};


	//# \class	RenderSegment		Stores rendering information for one segment of a renderable object.
	//
	//# The $RenderSegment$ class stores rendering information for one segment of a renderable object.
	//
	//# \def	class RenderSegment
	//
	//# \ctor	RenderSegment(unsigned_int32 state = 0);
	//
	//# \param	state	Flags that determine various material states to be applied.
	//
	//# \desc
	//
	//# \value	kMaterialTwoSided	The material should be rendered two-sided.
	//# \value	kMaterialAlphaTest	Use alpha testing with the material.


	//# \function	RenderSegment::GetNextRenderSegment		Returns the next segment in the linked list of render segments.
	//
	//# \proto	RenderSegment *GetNextRenderSegment(void) const;
	//
	//# \desc
	//
	//# \also	$@RenderSegment::SetNextRenderSegment@$
	//# \also	$@Renderable::GetFirstRenderSegment@$


	//# \function	RenderSegment::SetNextRenderSegment		Sets the next segment in the linked list of render segments.
	//
	//# \proto	void SetNextRenderSegment(RenderSegment *segment);
	//
	//# \param	segment		The render segment will follow the segment for which this function is called.
	//
	//# \desc
	//
	//# \also	$@RenderSegment::GetNextRenderSegment@$
	//# \also	$@Renderable::GetFirstRenderSegment@$


	//# \function	RenderSegment::GetPrimitiveStart		Returns the starting index of primitives belonging to a render segment.
	//
	//# \proto	int32 GetPrimitiveStart(void) const;
	//
	//# \desc
	//
	//# \also	$@RenderSegment::GetPrimitiveCount@$
	//# \also	$@RenderSegment::SetPrimitiveRange@$


	//# \function	RenderSegment::GetPrimitiveCount		Returns the number of primitives belonging to a render segment.
	//
	//# \proto	int32 GetPrimitiveCount(void) const;
	//
	//# \desc
	//
	//# \also	$@RenderSegment::GetPrimitiveStart@$
	//# \also	$@RenderSegment::SetPrimitiveRange@$


	//# \function	RenderSegment::SetPrimitiveRange		Sets the starting index and the number of primitives belonging to a render segment.
	//
	//# \proto	void SetPrimitiveRange(int32 start, int32 count);
	//
	//# \param	start	The primitive index at which the render segment begins.
	//# \param	count	The number of primitives in the render segment.
	//
	//# \desc
	//
	//# \also	$@RenderSegment::GetPrimitiveCount@$
	//# \also	$@RenderSegment::GetPrimitiveStart@$


	//# \function	RenderSegment::GetMaterialState		Returns the material state flags.
	//
	//# \proto	unsigned_int32 GetMaterialState(void) const;
	//
	//# \desc
	//# The $GetMaterialState$ function returns the material state flags for a render segment, which can be zero
	//# or a combination (through logical OR) of the following constants.
	//
	//# \table	MaterialFlags
	//
	//# \also	$@RenderSegment::SetMaterialState@$


	//# \function	RenderSegment::SetMaterialState		Sets the material state flags.
	//
	//# \proto	void SetMaterialState(unsigned_int32 state);
	//
	//# \desc
	//# The $SetMaterialState$ function sets the material state flags for a render segment, which can be zero
	//# or a combination (through logical OR) of the following constants.
	//
	//# \table	MaterialFlags
	//
	//# \also	$@RenderSegment::GetMaterialState@$


	//# \function	RenderSegment::GetMaterialObjectPointer		Returns the material object pointer.
	//
	//# \proto	MaterialObject *const *GetMaterialObjectPointer(void) const;
	//
	//# \desc
	//# The $GetMaterialObjectPointer$ function returns the pointer to the location at which a pointer to a
	//# material object resides.
	//
	//# \also	$@RenderSegment::SetMaterialObjectPointer@$
	//# \also	$@RenderSegment::GetMaterialAttributeList@$
	//# \also	$@RenderSegment::SetMaterialAttributeList@$
	//# \also	$@MaterialObject@$


	//# \function	RenderSegment::SetMaterialObjectPointer		Sets the material object pointer.
	//
	//# \proto	void SetMaterialObjectPointer(MaterialObject *const *object);
	//
	//# \param	object		A pointer to a location holding a pointer to a material object.
	//
	//# \desc
	//# The $SetMaterialObjectPointer$ function sets the pointer to the location at which a pointer to a
	//# material object resides.
	//
	//# \also	$@RenderSegment::GetMaterialObjectPointer@$
	//# \also	$@RenderSegment::GetMaterialAttributeList@$
	//# \also	$@RenderSegment::SetMaterialAttributeList@$
	//# \also	$@MaterialObject@$


	//# \function	RenderSegment::GetMaterialAttributeList		Returns the material attribute list.
	//
	//# \proto	List<Attribute> *GetMaterialAttributeList(void) const;
	//
	//# \desc
	//# The $GetMaterialAttributeList$ function returns a pointer to the material attribute list assigned to a render segment.
	//# The presence of a material attribute list is optional, and $nullptr$ is returned if the render segment does
	//# not have an attribute list. If present, the list contains shading attributes that either augment or override
	//# the attributes stored in the material object assigned to the render segment.
	//
	//# \also	$@RenderSegment::SetMaterialAttributeList@$
	//# \also	$@RenderSegment::GetMaterialObjectPointer@$
	//# \also	$@RenderSegment::SetMaterialObjectPointer@$
	//# \also	$@Attribute@$


	//# \function	RenderSegment::SetMaterialAttributeList		Sets the material attribute list.
	//
	//# \proto	void SetMaterialAttributeList(List<Attribute> *list);
	//
	//# \param	list	A pointer to a list of material attributes.
	//
	//# \desc
	//# The $SetMaterialAttributeList$ function assigns a material attribute list to a render segment. The presence
	//# of a material attribute list is optional, and the $list$ attribute may be $nullptr$ to indicate that
	//# the render segment has no material attribute list. If present, the shading attributes in the list
	//# either augment or override the attributes stored in the material object assigned to the render segment.
	//
	//# \also	$@RenderSegment::GetMaterialAttributeList@$
	//# \also	$@RenderSegment::GetMaterialObjectPointer@$
	//# \also	$@RenderSegment::SetMaterialObjectPointer@$
	//# \also	$@Attribute@$


	//# \function	RenderSegment::InvalidateShaderData		Invalidates the shader data for a render segment.
	//
	//# \proto	void InvalidateShaderData(void);
	//
	//# \desc
	//# The $InvalidateShaderData$ function causes the internal shader data for a render segment to be discarded.
	//# The shader data is rebuilt the next time the segment is rendered. It is necessary to call this function
	//# whenever a segment's material object or material attribute list is altered. However, it is usually not
	//# necessary to call this function in the case that a color or texture is changed in an existing attribute.
	//
	//# \also	$@Renderable::InvalidateShaderData@$


	class RenderSegment
	{
		friend class GraphicsMgr;

		private:

			RenderSegment			*nextSegment;
			Renderable				*owningRenderable;
			ShaderData				*currentShaderData;

			int32					primitiveStart;

			union
			{
				int32				primitiveCount;
				int32				multiRenderCount;
			};

			unsigned_int32			materialState;

			MaterialObject			*const *materialObject;
			List<Attribute>			*materialAttributeList;

			union
			{
				const unsigned_int32	*multiCountArray;
				unsigned_int32			multiRenderMask;
			};

			union
			{
				const machine_address	*multiOffsetArray;
				const int32				*multiRenderData;
			};

			ShaderData				*segmentShaderData[kShaderTypeCount];

			unsigned_int32 GetShaderDataMaterialState(ShaderType type);

			ShaderData *InitAmbientShaderData(Renderable *renderable, ShaderType type, const ShaderKey& key);
			ShaderData *InitLightShaderData(Renderable *renderable, ShaderType type, const ShaderKey& key);
			ShaderData *InitEffectShaderData(Renderable *renderable, ShaderType type, const ShaderKey& key);
			ShaderData *InitPlainShaderData(Renderable *renderable, ShaderType type, const ShaderKey& key);

		public:

			RenderSegment(Renderable *renderable, unsigned_int32 state = 0);
			~RenderSegment();

			RenderSegment *GetNextRenderSegment(void) const
			{
				return (nextSegment);
			}

			void SetNextRenderSegment(RenderSegment *segment)
			{
				nextSegment = segment;
			}

			int32 GetPrimitiveStart(void) const
			{
				return (primitiveStart);
			}

			int32 GetPrimitiveCount(void) const
			{
				return (primitiveCount);
			}

			void SetPrimitiveRange(int32 start, int32 count)
			{
				primitiveStart = start;
				primitiveCount = count;
			}

			int32 GetMultiRenderCount(void) const
			{
				return (multiRenderCount);
			}

			void SetMultiRenderCount(int32 count)
			{
				multiRenderCount = count;
			}

			unsigned_int32 GetMaterialState(void) const
			{
				return (materialState);
			}

			void SetMaterialState(unsigned_int32 state)
			{
				materialState = state;
			}

			MaterialObject *const *GetMaterialObjectPointer(void) const
			{
				return (materialObject);
			}

			void SetMaterialObjectPointer(MaterialObject *const *object)
			{
				materialObject = object;
			}

			List<Attribute> *GetMaterialAttributeList(void) const
			{
				return (materialAttributeList);
			}

			void SetMaterialAttributeList(List<Attribute> *list)
			{
				materialAttributeList = list;
			}

			const unsigned_int32 *GetMultiCountArray(void) const
			{
				return (multiCountArray);
			}

			const machine_address *GetMultiOffsetArray(void) const
			{
				return (multiOffsetArray);
			}

			void SetMultiRenderArrays(const unsigned_int32 *count, const machine_address *offset)
			{
				multiCountArray = count;
				multiOffsetArray = offset;
			}

			unsigned_int32 GetMultiRenderMask(void) const
			{
				return (multiRenderMask);
			}

			void SetMultiRenderMask(unsigned_int32 mask)
			{
				multiRenderMask = mask;
			}

			const int32 *GetMultiRenderData(void) const
			{
				return (multiRenderData);
			}

			void SetMultiRenderData(const int32 *data)
			{
				multiRenderData = data;
			}

			ShaderData *InitShaderData(Renderable *renderable, ShaderType type, const ShaderKey& key);
			ShaderData *GetShaderData(ShaderType type, const ShaderKey& key);

			C4API void InvalidateVertexData(void);
			C4API void InvalidateShaderData(void);
			C4API void InvalidateAmbientShaderData(void);
	};


	//# \class	Renderable		Stores general rendering information for a renderable object.
	//
	//# The $Renderable$ class stores general rendering information for a renderable object.
	//
	//# \def	class Renderable : public ListElement<Renderable>
	//
	//# \ctor	Renderable(RenderType type, unsigned_int32 state = 0);
	//
	//# \param	type	The primitive type of the renderable object. See below for possible values.
	//# \param	state	Flags that determine various render states to be applied. See below for possible values.
	//
	//# \desc
	//# The $Renderable$ class stores general rendering information about a single renderable object. The use of the
	//# $Renderable$ class is the sole means by which an object can be rendered by the Graphics Manager. Objects are
	//# rendered by storing their associated $Renderable$ objects (which may be base classes of more specialized structures)
	//# in a list and passing the list to the $@GraphicsMgr::DrawRenderList@$ function.
	//#
	//# The $type$ parameter passed to the constructor specifies the rendering primitive used by the object and may be any
	//# one of the following constants, where <i>n</i> represents the number of vertices.
	//
	//# \table	RenderType
	//
	//# When an object is rendered, the current light determines how the object is shaded. The $state$ parameter specifies
	//# light-independent rendering state and may be any combination of the following bit flags.
	//
	//# \table	RenderState
	//
	//# \base	Utilities/ListElement<Renderable>	$Renderable$ objects are stored in a list that is passed to the $@GraphicsMgr::DrawRenderList@$ function.


	//# \function	Renderable::GetRenderType		Returns the primitive render type.
	//
	//# \proto	RenderType GetRenderType(void) const;
	//
	//# \desc
	//# The $GetRenderType$ function returns one of the following constants, representing the primitive render type of an object,
	//# where <i>n</i> represents the number of vertices.
	//
	//# \table	RenderType
	//
	//# \also	$@Renderable::SetRenderType@$


	//# \function	Renderable::SetRenderType		Sets the primitive render type.
	//
	//# \proto	void SetRenderType(RenderType type);
	//
	//# \param	type	The primitive type of the renderable object. See below for possible values.
	//
	//# \desc
	//# The $SetRenderType$ function sets the primitive render type of an object. The $type$ parameter may be one of the
	//# following constants, where <i>n</i> represents the number of vertices.
	//
	//# \table	RenderType
	//
	//# \also	$@Renderable::GetRenderType@$


	//# \function	Renderable::GetRenderState		Returns the render state flags that pertain to rendering.
	//
	//# \proto	unsigned_int32 GetRenderState(void) const;
	//
	//# \desc
	//# The $GetRenderState$ function returns the light-independent rendering state, which can be a combination (through logical OR) of the
	//# following bit flags.
	//
	//# \table	RenderState
	//
	//# \also	$@Renderable::SetRenderState@$


	//# \function	Renderable::SetRenderState		Sets the render state flags that pertain to rendering.
	//
	//# \proto	void SetRenderState(unsigned_int32 state);
	//
	//# \param	state	The new render state flags.
	//
	//# \desc
	//# The $SetRenderState$ function sets the light-independent render state flags. The $state$ parameter may be any
	//# combination of the following bit flags.
	//
	//# \table	RenderState
	//
	//# \also	$@Renderable::GetRenderState@$


	//# \function	Renderable::GetRenderableFlags		Returns the miscellaneous renderable flags.
	//
	//# \proto	unsigned_int32 GetRenderableFlags(void) const;
	//
	//# \desc
	//# The $GetRenderableFlags$ function returns the miscellaneous renderable flags, which can be a combination (through logical OR) of the
	//# following bit flags.
	//
	//# \table	RenderableFlags
	//
	//# By default, none of a renderable object's miscellaneous renderable flags are set.
	//
	//# \also	$@Renderable::SetRenderableFlags@$


	//# \function	Renderable::SetRenderableFlags		Sets the miscellaneous renderable flags.
	//
	//# \proto	void SetRenderableFlags(unsigned_int32 flags);
	//
	//# \param	flags	The new renderable flags.
	//
	//# \desc
	//# The $SetRenderableFlags$ function sets the miscellaneous renderable flags. The $flags$ parameter may be any
	//# combination of the following bit flags.
	//
	//# \table	RenderableFlags
	//
	//# By default, none of a renderable object's miscellaneous renderable flags are set.
	//
	//# \also	$@Renderable::GetRenderableFlags@$


	//# \function	Renderable::GetShaderFlags		Returns the shader initialization flags.
	//
	//# \proto	unsigned_int32 GetShaderFlags(void) const;
	//
	//# \desc
	//# The $GetShaderFlags$ function returns the shader initialization flags, which can be a combination (through logical OR) of the
	//# following bit flags.
	//
	//# \table	ShaderFlags
	//
	//# By default, none of a renderable object's shader initialization flags are set.
	//
	//# \also	$@Renderable::SetShaderFlags@$


	//# \function	Renderable::SetShaderFlags		Sets the shader initialization flags.
	//
	//# \proto	void SetShaderFlags(unsigned_int32 flags);
	//
	//# \param	flags	The new shader initialization flags.
	//
	//# \desc
	//# The $SetShaderFlags$ function sets the shader initialization flags. The $flags$ parameter may be any
	//# combination of the following bit flags.
	//
	//# \table	ShaderFlags
	//
	//# By default, none of a renderable object's shader initialization flags are set.
	//
	//# \also	$@Renderable::GetShaderFlags@$


	//# \function	Renderable::GetAmbientBlendState		Returns the ambient pass blend state.
	//
	//# \proto	unsigned_int32 GetAmbientBlendState(void) const;
	//
	//# \desc
	//# The $GetAmbientBlendState$ function returns the blending function used when an object is rendered in the ambient
	//# pass. See the $@Renderable::SetAmbientBlendState@$ function for more information about the value returned.
	//
	//# \also	$@Renderable::SetAmbientBlendState@$


	//# \function	Renderable::SetAmbientBlendState		Sets the ambient pass blend state.
	//
	//# \proto	void SetAmbientBlendState(unsigned_int32 state);
	//
	//# \param	state	The new ambient pass blend state.
	//
	//# \desc
	//# The $SetAmbientBlendState$ function sets the blending function used when an object is rendered in the ambient
	//# pass. The $state$ parameter encodes the blending factors and can be constructed using the $BlendState$
	//# function. The first two parameters of the $BlendState$ function specify one of the following constants
	//# for the source blend factor and the destination blend factor, respectively.
	//
	//# \table	BlendFactor
	//
	//# For example, $BlendState(kBlendOne, kBlendZero)$ returns the blend state corresponding to replacement
	//# of the destination color by the source color. $BlendState(kBlendOne, kBlendOne)$ adds the source color
	//# to the destination color.
	//#
	//# There are several predefined constants that can also be passed to the $state$ parameter, listed below.
	//
	//# \table	BlendState
	//
	//# \also	$@Renderable::GetAmbientBlendState@$


	//# \function	Renderable::GetDepthOffsetDelta		Returns the depth offset delta value.
	//
	//# \proto	float GetDepthOffsetDelta(void) const;
	//
	//# \desc
	//#
	//
	//# \also	$@Renderable::GetDepthOffsetPoint@$
	//# \also	$@Renderable::SetDepthOffset@$


	//# \function	Renderable::GetDepthOffsetPoint		Returns the depth offset center point.
	//
	//# \proto	const Point3D& GetDepthOffsetPoint(void) const;
	//
	//# \desc
	//#
	//
	//# \also	$@Renderable::GetDepthOffsetDelta@$
	//# \also	$@Renderable::SetDepthOffset@$


	//# \function	Renderable::SetDepthOffset		Sets the depth offset parameters.
	//
	//# \proto	void SetDepthOffset(float delta, const Point3D *point);
	//
	//# \param	delta	The depth offset delta value.
	//# \param	point	A pointer to the depth offset center point, in world-space coordinates.
	//
	//# \desc
	//#
	//
	//# \also	$@Renderable::GetDepthOffsetDelta@$
	//# \also	$@Renderable::GetDepthOffsetPoint@$


	//# \div
	//# \function	Renderable::GetTransformable		Returns a pointer to the $@Utilities/Transformable@$ object.
	//
	//# \proto	const Transformable *GetTransformable(void) const;
	//
	//# \desc
	//# The $GetTransformable$ function returns a pointer to the $@Utilities/Transformable@$ object that determines the
	//# transformation from object space to world space. If the renderable object has no $@Utilities/Transformable@$ object
	//# assigned to it (the default state), then the return value is $nullptr$. In this case, the renderable object's vertices
	//# are assumed to exist in world space.
	//
	//# \also	$@Renderable::SetTransformable@$


	//# \function	Renderable::SetTransformable		Sets a pointer to the $@Utilities/Transformable@$ object.
	//
	//# \proto	void SetTransformable(const Transformable *transform);
	//
	//# \param	transform	A pointer to a $@Utilities/Transformable@$ object. Specifying $nullptr$ indicates that the
	//#						transformation from object space to world space is the identity transform.
	//
	//# \desc
	//# The $SetTransformable$ function sets a pointer to the $@Utilities/Transformable@$ object that determines the
	//# transformation from object space to world space. By default, a renderable object has no $@Utilities/Transformable@$
	//# object assigned to it, meaning that the object's vertices exist in world space.
	//
	//# \special
	//# The 3&nbsp;&times;&nbsp;3 rotation portion of the object-to-world transformation contained within the
	//# $@Utilities/Transformable@$ object should be orthogonal and should have a determinant of +1. That is, it must
	//# represent only a rotation and contain no scale, skew, or mirroring of any kind. Failure to meet this requirement
	//# will result in incorrect lighting for the renderable object.
	//
	//# \also	$@Renderable::GetTransformable@$


	//# \function	Renderable::GetTransparentAttachment		Returns the transparent attachment.
	//
	//# \proto	Renderable *GetTransparentAttachment(void) const;
	//
	//# \desc
	//# The $GetTransparentAttachment$ function returns the object that is attached to a renderable to enforce
	//# a particular transparency sorting order. For more information about transparent attachments, see the
	//# $@Renderable::SetTransparentAttachment@$ function.
	//#
	//# Initially, the transparent attachment is $nullptr$.
	//
	//# \also	$@Renderable::SetTransparentAttachment@$
	//# \also	$@Renderable::GetTransparentPosition@$
	//# \also	$@Renderable::SetTransparentPosition@$


	//# \function	Renderable::SetTransparentAttachment		Sets the transparent attachment.
	//
	//# \proto	void SetTransparentAttachment(Renderable *attachment);
	//
	//# \param	attachment		The renderable object that will be attached.
	//
	//# \desc
	//# A renderable object may have another renderable object attached to it for the purpose of specifying a
	//# rendering order. The presence of an attachment qualifies a renderable object for transparent sorting, and an
	//# object is always rendered immediately before its attachment. This is useful when two transparent objects
	//# occupy the same space and one should always be behind the other.
	//#
	//# The $SetTransparentAttachment$ function sets the object that is attached to a renderable. If the
	//# $attachment$ parameter is $nullptr$, then the attachment is cleared.
	//#
	//# Initially, the transparent attachment is $nullptr$.
	//
	//# \also	$@Renderable::GetTransparentAttachment@$
	//# \also	$@Renderable::GetTransparentPosition@$
	//# \also	$@Renderable::SetTransparentPosition@$


	//# \function	Renderable::GetTransparentPosition		Returns the transparent position pointer.
	//
	//# \proto	const Point3D *GetTransparentPosition(void) const;
	//
	//# \desc
	//# The $GetTransparentPosition$ function returns the pointer to a renderable object's transparent
	//# position. This position is specified in world-space coordinates and represents the general location
	//# of a transparent object. Renderable objects having a transparent position are sorted and rendered
	//# from back to front with respect to the camera view direction.
	//#
	//# If a renderable object has an attachment specified with the $@Renderable::SetTransparentAttachment@$
	//# function, then the transparent position is ignored, and the object is always rendered immediately
	//# before its attachment.
	//#
	//# Initially, the transparent position is $nullptr$.
	//
	//# \also	$@Renderable::SetTransparentPosition@$
	//# \also	$@Renderable::GetTransparentAttachment@$
	//# \also	$@Renderable::SetTransparentAttachment@$


	//# \function	Renderable::SetTransparentPosition		Sets the transparent position pointer.
	//
	//# \proto	void SetTransparentPosition(const Point3D *position);
	//
	//# \param	position	A pointer to a world-space position.
	//
	//# \desc
	//# The $SetTransparentPosition$ function sets the pointer to a renderable object's transparent
	//# position. This position is specified in world-space coordinates and represents the general location
	//# of a transparent object. Renderable objects having a transparent position are sorted and rendered
	//# from back to front with respect to the camera view direction.
	//#
	//# If a renderable object has an attachment specified with the $@Renderable::SetTransparentAttachment@$
	//# function, then the transparent position is ignored, and the object is always rendered immediately
	//# before its attachment.
	//#
	//# Initially, the transparent position is $nullptr$.
	//
	//# \also	$@Renderable::SetTransparentPosition@$
	//# \also	$@Renderable::GetTransparentAttachment@$
	//# \also	$@Renderable::SetTransparentAttachment@$


	//# \div
	//# \function	Renderable::SetVertexBuffer		Assigns a vertex buffer to a renderable.
	//
	//# \proto	void SetVertexBuffer(unsigned_int32 index, VertexBuffer *buffer, unsigned_int32 stride = 0);
	//
	//# \param	index		The index of the vertex buffer. See below for possible values.
	//# \param	buffer		A pointer to the vertex buffer to assign.
	//# \param	stride		The stride between two consecutive vertices, in bytes.
	//
	//# \desc
	//# The $SetVertexBuffer$ function assigns the vertex buffer specified by the $buffer$ parameter to
	//# the vertex buffer slot specified by the $index$ parameter. The $index$ parameter can be one of
	//# the following constants.
	//
	//# \table	VertexBufferIndex
	//
	//# For vertex buffers storing vertex attribute data, the $stride$ parameter specifies the stride from the
	//# beginning of any particular vertex to the beginning to the next consecutive vertex, in bytes. For a vertex
	//# buffer storing index data, the $stride$ parameter should be the default value of zero.
	//
	//# By default, all vertex attributes are read from the vertex buffer with the index $kVertexBufferAttributeArray0$.
	//# The $@Renderable::SetVertexBufferArrayFlags@$ function can be used to indicate that some attributes
	//# are read from the vertex buffer with the index $kVertexBufferAttributeArray1$.
	//
	//# \also	$@Renderable::SetVertexBufferArrayFlags@$
	//# \also	$@Renderable::SetVertexCount@$


	//# \function	Renderable::SetVertexBufferArrayFlags		Sets the vertex buffer array flags.
	//
	//# \proto	void SetVertexBufferArrayFlags(unsigned_int32 flags);
	//
	//# \param	flags	The new vertex buffer array flags.
	//
	//# \desc
	//# The $SetVertexBufferArrayFlags$ function sets the flags that determine from which vertex buffer
	//# each vertex attribute array is read. For each bit position <i>n</i> in the $flags$ parameter,
	//# a bit value of zero indicates that the data for vertex attribute array <i>n</i> should be read
	//# from the primary vertex buffer having index $kVertexBufferAttributeArray0$, and a bit value of
	//# one indicates that the data should be read from the secondary vertex buffer having index
	//# $kVertexBufferAttributeArray1$.
	//#
	//# A value containing a one in the correct bit position for a particular vertex attribute array can
	//# be obtained by shifting the number one left by the number of bits equal to the array's index.
	//# For example, to indicate that the vertex attribute with index $kArrayColor$ be read from the
	//# secondary vertex buffer, use the value <code>(1 << kArrayColor)</code>.
	//#
	//# By default, the value of the vertex buffer array flags is all zeros.
	//
	//# \also	$@Renderable::SetVertexBuffer@$
	//# \also	$@Renderable::SetVertexCount@$


	//# \function	Renderable::SetVertexAttributeArray		Sets the offset and size of a vertex attribute array.
	//
	//# \proto	void SetVertexAttributeArray(int32 index, unsigned_int32 offset, int32 count);
	//
	//# \param	index		The index of the vertex attribute array.
	//# \param	offset		The offset of the first element of the attribute array, in bytes.
	//# \param	count		The number of 32-bit components composing the attribute.
	//
	//# \desc
	//# The $SetAttributeOffset$ function sets the offset within the vertex buffer to the first element
	//# of the vertex attribute array specified by the $index$ parameter. The $offset$ parameter should
	//# typically be set to the cumulative size of the vertex attributes preceding the attribute having
	//# the index given by the $index$ parameter for a single vertex. The $count$ parameter specifies
	//# the number of 32-bit floating-point components used by the attribute, or the number of 32-bit
	//# words in the case that the attribute is composed of integer data.
	//
	//# \also	$@Renderable::SetVertexBuffer@$
	//# \also	$@Renderable::SetVertexBufferArrayFlags@$
	//# \also	$@Renderable::SetVertexCount@$


	//# \function	Renderable::GetVertexCount		Returns the vertex count.
	//
	//# \proto	int32 GetVertexCount(void) const;
	//
	//# \desc
	//#
	//
	//# \also	$@Renderable::SetVertexCount@$


	//# \function	Renderable::SetVertexCount		Sets the vertex count.
	//
	//# \proto	void SetVertexCount(int32 count);
	//
	//# \param	count	The vertex count.
	//
	//# \desc
	//#
	//
	//# \also	$@Renderable::GetVertexCount@$


	//# \function	Renderable::GetVertexAttributeComponentCount		Returns the number of components used by one of the vertex attribute arrays.
	//
	//# \proto	int8 GetVertexAttributeComponentCount(int32 index) const;
	//
	//# \param	index	The index of the attribute array.
	//
	//# \desc
	//#
	//
	//# \also	$@Renderable::SetAttributeOffset@$


	//# \function	Renderable::GetPrimitiveCount		Returns the primitive count.
	//
	//# \proto	int32 GetPrimitiveCount(void) const;
	//
	//# \desc
	//#
	//
	//# \also	$@Renderable::SetPrimitiveCount@$
	//# \also	$@Renderable::SetPrimitiveIndexOffset@$


	//# \function	Renderable::SetPrimitiveCount		Sets the primitive count.
	//
	//# \proto	void SetPrimitiveCount(int32 count);
	//
	//# \param	count	The primitive count.
	//
	//# \desc
	//#
	//
	//# \also	$@Renderable::GetPrimitiveCount@$
	//# \also	$@Renderable::SetPrimitiveIndexOffset@$


	//# \function	Renderable::SetPrimitiveIndexOffset		Sets the offset of the primitive index array.
	//
	//# \proto	void SetPrimitiveIndexOffset(unsigned_int32 offset);
	//
	//# \param	offset	The offset of the first element of the primitive array, in bytes.
	//
	//# \desc
	//#
	//
	//# \also	$@Renderable::GetPrimitiveCount@$
	//# \also	$@Renderable::SetPrimitiveCount@$


	//# \function	Renderable::GetOcclusionQuery		Returns the pointer to an occlusion query object.
	//
	//# \proto	OcclusionQuery *GetOcclusionQuery(void) const;
	//
	//# \desc
	//# The $GetOcclusionQuery$ function returns a pointer to the occlusion query object attached to a renderable object.
	//# If a renderable object has no occlusion object attached to it (the default case), then the return value is $nullptr$.
	//
	//# \also	$@Renderable::SetOcclusionQuery@$
	//# \also	$@OcclusionQuery@$


	//# \function	Renderable::SetOcclusionQuery		Sets the pointer to an occlusion query object.
	//
	//# \proto	void SetOcclusionQuery(OcclusionQuery *query);
	//
	//# \param	query	The pointer to an occlusion query object.
	//
	//# \desc
	//# The $SetOcclusionQuery$ function attaches the occlusion query object specified by the $query$ parameter to a
	//# renderable object. See the $@OcclusionQuery@$ class for information about how occlusion queries are used.
	//
	//# \also	$@Renderable::GetOcclusionQuery@$
	//# \also	$@OcclusionQuery@$


	//# \function	Renderable::GetFirstRenderSegment	Returns the first render segment belonging to a renderable object.
	//
	//# \proto	RenderSegment *GetFirstRenderSegment(void);
	//# \proto	const RenderSegment *GetFirstRenderSegment(void) const;
	//
	//# \desc
	//# The $GetFirstRenderSegment$ function returns the first render segment belonging to a renderable object.
	//# Every renderable object has at least one render segment. Additional render segments can be added to a
	//# renderable object using the $@RenderSegment::SetNextRenderSegment@$ function.
	//
	//# \also	$@RenderSegment@$
	//# \also	$@RenderSegment::GetNextRenderSegment@$
	//# \also	$@RenderSegment::SetNextRenderSegment@$


	//# \function	Renderable::InvalidateShaderData	Invalidates the shader data for all render segments belonging to a renderable object.
	//
	//# \proto	void InvalidateShaderData(void);
	//
	//# \desc
	//# The $InvalidateShaderData$ function causes the internal shader data for all render segments belonging to
	//# a renderable object to be discarded.
	//
	//# \also	$@RenderSegment::InvalidateShaderData@$


	class Renderable : public ListElement<Renderable>
	{
		friend class RenderSegment;
		friend class ShaderAttribute;

		private:

			RenderType					renderType;
			unsigned_int32				renderState;
			unsigned_int32				shaderFlags;
			unsigned_int16				renderableFlags;
			int16						geometryShaderIndex;

			unsigned_int32				ambientBlendState;
			unsigned_int32				lightBlendState;

			const Transformable			*transformable;
			const Transform4D			*previousWorldTransform;

			const PaintEnvironment		*paintEnvironment;
			const AmbientEnvironment	*ambientEnvironment;
			const Box3D					*motionBlurBox;

			Renderable					*transparentAttachment;
			const Point3D				*transparentPosition;
			float						transparentDepth;

			float						depthOffsetDelta;
			const Point3D				*depthOffsetPoint;

			const VertexBuffer			*vertexBuffer[kVertexBufferCount];
			unsigned_int8				vertexBufferStride[kVertexBufferCount];
			unsigned_int32				vertexBufferArrayFlags;

			int32						vertexCount;
			unsigned_int32				primitiveIndexOffset;
			unsigned_int32				attributeOffset[kMaxAttributeArrayCount];
			int8						componentCount[kMaxAttributeArrayCount];

			const Vector4D				*renderParameter;
			const Vector4D				*texcoordParameter;
			const Vector4D				*terrainParameter;

			OcclusionQuery				*occlusionQuery;
			const ColorRGBA				*wireColor;

			int32						shaderDetailLevel;
			float						shaderDetailParameter;

			RenderSegment				renderSegment;

			C4API static const PaintEnvironment		nullPaintEnvironment;
			C4API static const AmbientEnvironment	nullAmbientEnvironment;

			C4API static const ConstVector4D		nullRenderParameterTable[kMaxRenderParameterCount];
			C4API static const ConstVector4D		nullTexcoordParameterTable[kMaxTexcoordParameterCount];
			C4API static const ConstVector4D		nullTerrainParameterTable[kMaxTerrainParameterCount];

			int32 SetShaderArray(ShaderData *data, int32 shaderIndex, int32 renderIndex) const;

			unsigned_int32 BuildVertexTransform(ShaderData *data, VertexAssembly *assembly) const;
			unsigned_int32 BuildTexcoord0Transform(const RenderSegment *segment, ShaderData *data, VertexAssembly *assembly, unsigned_int32 stateFlags) const;
			unsigned_int32 BuildTexcoord1Transform(const RenderSegment *segment, ShaderData *data, VertexAssembly *assembly, unsigned_int32 stateFlags) const;

			static void StateProc_CopyCameraPosition(const Renderable *renderable, const void *cookie);
			static void StateProc_CopyCameraDirections(const Renderable *renderable, const void *cookie);
			static void StateProc_CopyCameraPositionAndDirections(const Renderable *renderable, const void *cookie);
			static void StateProc_TransformCameraPosition(const Renderable *renderable, const void *cookie);
			static void StateProc_TransformCameraDirections(const Renderable *renderable, const void *cookie);
			static void StateProc_TransformCameraPositionAndDirections(const Renderable *renderable, const void *cookie);

			static void StateProc_CopyCameraPosition4D(const Renderable *renderable, const void *cookie);
			static void StateProc_TransformCameraPosition4D(const Renderable *renderable, const void *cookie);

			static void StateProc_CopyCameraMatrix(const Renderable *renderable, const void *cookie);
			static void StateProc_TransformCameraMatrix(const Renderable *renderable, const void *cookie);

			static void StateProc_CopyWorldMatrix(const Renderable *renderable, const void *cookie);
			static void StateProc_TransformWorldMatrix(const Renderable *renderable, const void *cookie);

			static void StateProc_TransformTexcoord0(const Renderable *renderable, const void *cookie);
			static void StateProc_AnimateTexcoord0(const Renderable *renderable, const void *cookie);
			static void StateProc_TransformAnimateTexcoord0(const Renderable *renderable, const void *cookie);
			static void StateProc_TransformTexcoord1(const Renderable *renderable, const void *cookie);
			static void StateProc_AnimateTexcoord1(const Renderable *renderable, const void *cookie);
			static void StateProc_TransformAnimateTexcoord1(const Renderable *renderable, const void *cookie);
			static void StateProc_ScaleTerrainTexcoord(const Renderable *renderable, const void *cookie);

			static void StateProc_GenerateTexcoord(const Renderable *renderable, const void *cookie);
			static void StateProc_GenerateTransformTexcoord0(const Renderable *renderable, const void *cookie);
			static void StateProc_GenerateAnimateTexcoord0(const Renderable *renderable, const void *cookie);
			static void StateProc_GenerateTransformAnimateTexcoord0(const Renderable *renderable, const void *cookie);
			static void StateProc_GenerateTransformTexcoord1(const Renderable *renderable, const void *cookie);
			static void StateProc_GenerateAnimateTexcoord1(const Renderable *renderable, const void *cookie);
			static void StateProc_GenerateTransformAnimateTexcoord1(const Renderable *renderable, const void *cookie);
			static void StateProc_GenerateAnimateDualTexcoords(const Renderable *renderable, const void *cookie);

			static void StateProc_ConfigureInfiniteLight(const Renderable *renderable, const void *cookie);
			static void StateProc_ConfigureTransformInfiniteLight(const Renderable *renderable, const void *cookie);
			static void StateProc_ConfigureInfiniteLightShadow(const Renderable *renderable, const void *cookie);
			static void StateProc_ConfigureTransformInfiniteLightShadow(const Renderable *renderable, const void *cookie);
			static void StateProc_ConfigurePointLight(const Renderable *renderable, const void *cookie);
			static void StateProc_ConfigureTransformPointLight(const Renderable *renderable, const void *cookie);
			static void StateProc_ConfigureCubeLight(const Renderable *renderable, const void *cookie);
			static void StateProc_ConfigureTransformCubeLight(const Renderable *renderable, const void *cookie);
			static void StateProc_ConfigureSpotLight(const Renderable *renderable, const void *cookie);
			static void StateProc_ConfigureTransformSpotLight(const Renderable *renderable, const void *cookie);
			static void StateProc_ConfigureSpotLightShadow(const Renderable *renderable, const void *cookie);
			static void StateProc_ConfigureTransformSpotLightShadow(const Renderable *renderable, const void *cookie);

			static void StateProc_CopyVertexScaleOffset(const Renderable *renderable, const void *cookie);
			static void StateProc_CopyTerrainParameters(const Renderable *renderable, const void *cookie);
			static void StateProc_CopyImpostorTransition(const Renderable *renderable, const void *cookie);
			static void StateProc_CopyGeometryTransition(const Renderable *renderable, const void *cookie);
			static void StateProc_TransformGeometryTransition(const Renderable *renderable, const void *cookie);

			static void StateProc_CopyPaintSpace(const Renderable *renderable, const void *cookie);
			static void StateProc_TransformPaintSpace(const Renderable *renderable, const void *cookie);

			static void StateProc_SetOcclusionQuery(const Renderable *renderable, const void *cookie);

		public:

			C4API Renderable(RenderType type, unsigned_int32 state = 0);
			C4API virtual ~Renderable();

			RenderType GetRenderType(void) const
			{
				return (renderType);
			}

			void SetRenderType(RenderType type)
			{
				renderType = type;
			}

			unsigned_int32 GetRenderState(void) const
			{
				return (renderState);
			}

			void SetRenderState(unsigned_int32 state)
			{
				renderState = state;
			}

			unsigned_int32 GetShaderFlags(void) const
			{
				return (shaderFlags);
			}

			void SetShaderFlags(unsigned_int32 flags)
			{
				shaderFlags = flags;
			}

			unsigned_int32 GetRenderableFlags(void) const
			{
				return (renderableFlags);
			}

			void SetRenderableFlags(unsigned_int32 flags)
			{
				renderableFlags = (unsigned_int16) flags;
			}

			int32 GetGeometryShaderIndex(void) const
			{
				return (geometryShaderIndex);
			}

			void SetGeometryShaderIndex(int32 index)
			{
				geometryShaderIndex = (int16) index;
			}

			unsigned_int32 GetAmbientBlendState(void) const
			{
				return (ambientBlendState);
			}

			void SetAmbientBlendState(unsigned_int32 state)
			{
				ambientBlendState = state;
			}

			unsigned_int32 GetLightBlendState(void) const
			{
				return (lightBlendState);
			}

			void SetLightBlendState(unsigned_int32 state)
			{
				lightBlendState = state;
			}

			const Transformable *GetTransformable(void) const
			{
				return (transformable);
			}

			void SetTransformable(const Transformable *transform)
			{
				transformable = transform;
			}

			const Transform4D *GetPreviousWorldTransformPointer(void) const
			{
				return (previousWorldTransform);
			}

			void SetPreviousWorldTransformPointer(const Transform4D *transform)
			{
				previousWorldTransform = transform;
			}

			const PaintEnvironment *GetPaintEnvironment(void) const
			{
				return (paintEnvironment);
			}

			void SetPaintEnvironment(const PaintEnvironment *environment)
			{
				paintEnvironment = environment;
			}

			void SetNullPaintEnvironment(void)
			{
				paintEnvironment = &nullPaintEnvironment;
			}

			const AmbientEnvironment *GetAmbientEnvironment(void) const
			{
				return (ambientEnvironment);
			}

			void SetAmbientEnvironment(const AmbientEnvironment *environment)
			{
				ambientEnvironment = environment;
			}

			void SetNullAmbientEnvironment(void)
			{
				ambientEnvironment = &nullAmbientEnvironment;
			}

			const Box3D *GetMotionBlurBox(void) const
			{
				return (motionBlurBox);
			}

			void SetMotionBlurBox(const Box3D *box)
			{
				motionBlurBox = box;
			}

			Renderable *GetTransparentAttachment(void) const
			{
				return (transparentAttachment);
			}

			void SetTransparentAttachment(Renderable *attachment)
			{
				transparentAttachment = attachment;
			}

			const Point3D *GetTransparentPosition(void) const
			{
				return (transparentPosition);
			}

			void SetTransparentPosition(const Point3D *position)
			{
				transparentPosition = position;
			}

			float GetTransparentDepth(void) const
			{
				return (transparentDepth);
			}

			void SetTransparentDepth(float depth)
			{
				transparentDepth = depth;
			}

			float GetDepthOffsetDelta(void) const
			{
				return (depthOffsetDelta);
			}

			const Point3D& GetDepthOffsetPoint(void) const
			{
				return (*depthOffsetPoint);
			}

			void SetDepthOffset(float delta, const Point3D *point)
			{
				depthOffsetDelta = delta;
				depthOffsetPoint = point;
			}

			const VertexBuffer *GetVertexBuffer(unsigned_int32 index) const
			{
				return (vertexBuffer[index]);
			}

			unsigned_int32 GetVertexBufferStride(unsigned_int32 index) const
			{
				return (vertexBufferStride[index]);
			}

			void SetVertexBuffer(unsigned_int32 index, const VertexBuffer *buffer, unsigned_int32 stride = 0)
			{
				vertexBuffer[index] = buffer;
				vertexBufferStride[index] = stride;
			}

			unsigned_int32 GetVertexBufferArrayFlags(void) const
			{
				return (vertexBufferArrayFlags);
			}

			void SetVertexBufferArrayFlags(unsigned_int32 flags)
			{
				vertexBufferArrayFlags = flags;
			}

			int32 GetVertexCount(void) const
			{
				return (vertexCount);
			}

			void SetVertexCount(int32 count)
			{
				vertexCount = count;
			}

			unsigned_int32 GetPrimitiveIndexOffset(void) const
			{
				return (primitiveIndexOffset);
			}

			void SetPrimitiveIndexOffset(unsigned_int32 offset)
			{
				primitiveIndexOffset = offset;
			}

			unsigned_int32 GetVertexAttributeOffset(int32 index) const
			{
				return (attributeOffset[index]);
			}

			void SetVertexAttributeArray(int32 index, unsigned_int32 offset, int32 count)
			{
				attributeOffset[index] = offset;
				componentCount[index] = (int8) count;
			}

			int8 GetVertexAttributeComponentCount(int32 index) const
			{
				return (componentCount[index]);
			}

			bool AttributeArrayEnabled(int32 index) const
			{
				return (componentCount[index] != 0);
			}

			bool TangentAvailable(void) const
			{
				return ((AttributeArrayEnabled(kArrayTangent)) || (shaderFlags & kShaderGenerateTangent));
			}

			const Vector4D *GetRenderParameterPointer(void) const
			{
				return (renderParameter);
			}

			void SetRenderParameterPointer(const Vector4D *param)
			{
				renderParameter = param;
			}

			void SetNullRenderParameterPointer(void)
			{
				renderParameter = &nullRenderParameterTable[0];
			}

			const Vector4D *GetTexcoordParameterPointer(void) const
			{
				return (texcoordParameter);
			}

			void SetTexcoordParameterPointer(const Vector4D *param)
			{
				texcoordParameter = param;
			}

			void SetNullTexcoordParameterPointer(void)
			{
				texcoordParameter = &nullTexcoordParameterTable[0];
			}

			const Vector4D *GetTerrainParameterPointer(void) const
			{
				return (terrainParameter);
			}

			void SetTerrainParameterPointer(const Vector4D *param)
			{
				terrainParameter = param;
			}

			void SetNullTerrainParameterPointer(void)
			{
				terrainParameter = &nullTerrainParameterTable[0];
			}

			OcclusionQuery *GetOcclusionQuery(void) const
			{
				return (occlusionQuery);
			}

			void SetOcclusionQuery(OcclusionQuery *query)
			{
				occlusionQuery = query;
			}

			const ColorRGBA *GetWireframeColorPointer(void) const
			{
				return (wireColor);
			}

			void SetWireframeColorPointer(const ColorRGBA *color)
			{
				wireColor = color;
			}

			int32 GetShaderDetailLevel(void) const
			{
				return (shaderDetailLevel);
			}

			void SetShaderDetailLevel(int32 level)
			{
				shaderDetailLevel = level;
			}

			float GetShaderDetailParameter(void) const
			{
				return (shaderDetailParameter);
			}

			void SetShaderDetailParameter(float parameter)
			{
				shaderDetailParameter = parameter;
			}

			int32 GetPrimitiveCount(void) const
			{
				return (renderSegment.GetPrimitiveCount());
			}

			void SetPrimitiveCount(int32 count)
			{
				renderSegment.SetPrimitiveRange(0, count);
			}

			MaterialObject *const *GetMaterialObjectPointer(void) const
			{
				return (renderSegment.GetMaterialObjectPointer());
			}

			void SetMaterialObjectPointer(MaterialObject *const *object)
			{
				renderSegment.SetMaterialObjectPointer(object);
			}

			List<Attribute> *GetMaterialAttributeList(void) const
			{
				return (renderSegment.GetMaterialAttributeList());
			}

			void SetMaterialAttributeList(List<Attribute> *list)
			{
				renderSegment.SetMaterialAttributeList(list);
			}

			RenderSegment *GetFirstRenderSegment(void)
			{
				return (&renderSegment);
			}

			const RenderSegment *GetFirstRenderSegment(void) const
			{
				return (&renderSegment);
			}

			C4API virtual const float *GetShaderParameterPointer(int32 slot) const;

			C4API void InvalidateVertexData(void);
			C4API void InvalidateShaderData(void);
			C4API void InvalidateAmbientShaderData(void);
	};
}


#endif

// ZYUQURM
