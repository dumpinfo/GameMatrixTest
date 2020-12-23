 

#ifndef C4VertexShaders_h
#define C4VertexShaders_h


#include "C4Programs.h"


namespace C4
{
	enum
	{
		kVertexParamMatrixMVP						= 0,		// Model-view-projection matrix
		kVertexParamMatrixWorld						= 4,		// Object to world transform
		kVertexParamMatrixCamera					= 7,		// Object to camera transform
		kVertexParamMatrixSpace						= 10,		// Object to space transform
		kVertexParamMatrixLight						= 13,		// Object to light transform
		kVertexParamMatrixShadow					= 16,		// Object to shadow transform
		kVertexParamMatrixLightProjector			= 19,		// Light projector transform

		kVertexParamMatrixVelocityA					= 12,		// Object to viewport transform, previous frame
		kVertexParamMatrixVelocityB					= 16,		// Object to viewport transform, current frame

		kVertexParamCameraPosition					= 21,		// Object-space camera position
		kVertexParamCameraRight						= 22,		// Object-space camera right direction
		kVertexParamCameraDown						= 23,		// Object-space camera down direction

		kVertexParamLightPosition					= 24,		// Object-space light position
		kVertexParamFogPlane						= 25,		// Object-space fog plane

		kVertexParamTexcoordGenerate				= 26,		// (x scale, y scale, 0.0, 0.0)
		kVertexParamTexcoordTransform0				= 27,		// (x scale, y scale, x offset, y offset)
		kVertexParamTexcoordTransform1				= 28,		// (x scale, y scale, x offset, y offset)
		kVertexParamTexcoordVelocity0				= 29,		// (v1.x, v1.y, 0.0, 0.0) or (v1.x, v1.y, v2.x, v2.y)
		kVertexParamTexcoordVelocity1				= 30,		// (v2.x, v2.y, 0.0, 0.0) or (v3.x, v3.y, 0.0, 0.0)

		kVertexParamFireParams						= 31,		// Fire params (intensity, 0.0, 0.0, 0.0)

		kVertexParamTerrainTexcoordScale			= 31,		// (scale, 0.0, 0.0, 0.0)
		kVertexParamTerrainParameter0				= 32,		// Terrain border parameters, positive faces
		kVertexParamTerrainParameter1				= 33,		// Terrain border parameters, negative faces

		kVertexParamShadowCascadePlane1				= 34,		// Shadow cascade 0-1 blend plane
		kVertexParamShadowCascadePlane2				= 35,		// Shadow cascade 1-2 blend plane
		kVertexParamShadowCascadePlane3				= 36,		// Shadow cascade 2-3 blend plane

		kVertexParamSpaceScale						= 37,		// Reciprocal radiosity space size (x, y, z, 0.0)
		kVertexParamVertexScaleOffset				= 38,		// (scale.x, scale.y, scale.z, offset)
		kVertexParamDistortCameraPlane				= 39,		// Object-space camera plane over focal length
		kVertexParamReflectionScale					= 40,		// (reflection offset scale, 0.0, 0.0, 0.0)
		kVertexParamRefractionScale					= 41,		// (refraction offset scale, 0.0, 0.0, 0.0)

		kVertexParamImpostorTransition				= 42,		// (transition scale, transition bias, 0.0, 0.0)
		kVertexParamImpostorDepth					= 43,		// (impostor depth scale, impostor depth offset, tan(elevation), 0.0)
		kVertexParamImpostorPlaneS					= 44,		// Geometry noisy blend impostor s-ccordinate generation plane
		kVertexParamImpostorPlaneT					= 45,		// Geometry noisy blend impostor t-ccordinate generation plane

		kVertexParamPaintPlaneS						= 46,		// Paint space s-ccordinate generation plane
		kVertexParamPaintPlaneT						= 47,		// Paint space t-ccordinate generation plane

		kVertexParamCount
	};

	static_assert(kVertexParamCount == Render::kMaxVertexParamCount, "Vertex param count mismatch");


	#define VERTEX_PARAM_MATRIX_MVP0				"0"
	#define VERTEX_PARAM_MATRIX_MVP1				"1"
	#define VERTEX_PARAM_MATRIX_MVP2				"2"
	#define VERTEX_PARAM_MATRIX_MVP3				"3"

	#define VERTEX_PARAM_MATRIX_WORLD0				"4"
	#define VERTEX_PARAM_MATRIX_WORLD1				"5"
	#define VERTEX_PARAM_MATRIX_WORLD2				"6"

	#define VERTEX_PARAM_MATRIX_CAMERA0				"7"
	#define VERTEX_PARAM_MATRIX_CAMERA1				"8"
	#define VERTEX_PARAM_MATRIX_CAMERA2				"9"

	#define VERTEX_PARAM_MATRIX_SPACE0				"10"
	#define VERTEX_PARAM_MATRIX_SPACE1				"11"
	#define VERTEX_PARAM_MATRIX_SPACE2				"12"

	#define VERTEX_PARAM_MATRIX_LIGHT0				"13"
	#define VERTEX_PARAM_MATRIX_LIGHT1				"14"
	#define VERTEX_PARAM_MATRIX_LIGHT2				"15"

	#define VERTEX_PARAM_MATRIX_SHADOW0				"16"
	#define VERTEX_PARAM_MATRIX_SHADOW1				"17"
	#define VERTEX_PARAM_MATRIX_SHADOW2				"18"

	#define VERTEX_PARAM_MATRIX_LIGHT_PROJECTOR0	"19"
	#define VERTEX_PARAM_MATRIX_LIGHT_PROJECTOR1	"20"

	#define VERTEX_PARAM_MATRIX_VELOCITY_A0			"12"
	#define VERTEX_PARAM_MATRIX_VELOCITY_A1			"13"
	#define VERTEX_PARAM_MATRIX_VELOCITY_A2			"14"
	#define VERTEX_PARAM_MATRIX_VELOCITY_A3			"15"

	#define VERTEX_PARAM_MATRIX_VELOCITY_B0			"16"
	#define VERTEX_PARAM_MATRIX_VELOCITY_B1			"17"
	#define VERTEX_PARAM_MATRIX_VELOCITY_B2			"18" 
	#define VERTEX_PARAM_MATRIX_VELOCITY_B3			"19"

	#define VERTEX_PARAM_CAMERA_POSITION			"21" 
	#define VERTEX_PARAM_CAMERA_RIGHT				"22"
	#define VERTEX_PARAM_CAMERA_DOWN				"23" 

	#define VERTEX_PARAM_LIGHT_POSITION				"24"
	#define VERTEX_PARAM_FOG_PLANE					"25" 

	#define VERTEX_PARAM_TEXCOORD_GENERATE			"26" 
	#define VERTEX_PARAM_TEXCOORD_TRANSFORM0		"27" 
	#define VERTEX_PARAM_TEXCOORD_TRANSFORM1		"28"
	#define VERTEX_PARAM_TEXCOORD_VELOCITY0			"29"
	#define VERTEX_PARAM_TEXCOORD_VELOCITY1			"30"
 
	#define VERTEX_PARAM_FIRE_PARAMS				"31"

	#define VERTEX_PARAM_TERRAIN_TEXCOORD_SCALE		"31"
	#define VERTEX_PARAM_TERRAIN_PARAMETER0			"32"
	#define VERTEX_PARAM_TERRAIN_PARAMETER1			"33"

	#define VERTEX_PARAM_SHADOW_CASCADE_PLANE1		"34"
	#define VERTEX_PARAM_SHADOW_CASCADE_PLANE2		"35"
	#define VERTEX_PARAM_SHADOW_CASCADE_PLANE3		"36"

	#define VERTEX_PARAM_SPACE_SCALE				"37"
	#define VERTEX_PARAM_VERTEX_SCALE_OFFSET		"38"
	#define VERTEX_PARAM_DISTORT_CAMERA_PLANE		"39"
	#define VERTEX_PARAM_REFLECTION_SCALE			"40"
	#define VERTEX_PARAM_REFRACTION_SCALE			"41"

	#define VERTEX_PARAM_IMPOSTOR_TRANSITION		"42"
	#define VERTEX_PARAM_IMPOSTOR_DEPTH				"43"
	#define VERTEX_PARAM_IMPOSTOR_PLANE_S			"44"
	#define VERTEX_PARAM_IMPOSTOR_PLANE_T			"45"

	#define VERTEX_PARAM_PAINT_PLANE_S				"46"
	#define VERTEX_PARAM_PAINT_PLANE_T				"47"

	#define VERTEX_PARAM_COUNT						"48"


	#if C4OPENGL

		#define ATTRIB(index)			"attrib[" index "]"
		#define VPARAM(index)			"vparam[" index "]"

	#elif C4PSSL //[ PS4

		// -- PS4 code hidden --

	#elif C4CG //[ PS3

		// -- PS3 code hidden --

	#endif //]


	enum
	{
		kMaxVertexSnippetCount		= 32
	};


	enum
	{
		kVertexSnippetOutputObjectPosition,
		kVertexSnippetOutputObjectNormal,
		kVertexSnippetOutputObjectTangent,
		kVertexSnippetOutputObjectBitangent,
		kVertexSnippetOutputWorldPosition,
		kVertexSnippetOutputWorldNormal,
		kVertexSnippetOutputWorldTangent,
		kVertexSnippetOutputWorldBitangent,
		kVertexSnippetOutputCameraNormal,
		kVertexSnippetOutputVertexGeometry,

		kVertexSnippetOutputObjectInfiniteLightDirection,
		kVertexSnippetCalculateObjectPointLightDirection,
		kVertexSnippetOutputObjectPointLightDirection,
		kVertexSnippetOutputTangentInfiniteLightDirection,
		kVertexSnippetOutputTangentPointLightDirection,

		kVertexSnippetCalculateObjectViewDirection,
		kVertexSnippetOutputObjectViewDirection,
		kVertexSnippetOutputTangentViewDirection,

		kVertexSnippetOutputBillboardInfiniteLightDirection,
		kVertexSnippetOutputBillboardPointLightDirection,

		kVertexSnippetCalculateTerrainTangentData,
		kVertexSnippetOutputTerrainInfiniteLightDirection,
		kVertexSnippetOutputTerrainPointLightDirection,
		kVertexSnippetOutputTerrainViewDirection,
		kVertexSnippetOutputTerrainWorldTangentFrame,

		kVertexSnippetOutputRawTexcoords,
		kVertexSnippetOutputTerrainTexcoords,
		kVertexSnippetOutputImpostorTexcoords,
		kVertexSnippetOutputImpostorTransitionBlend,
		kVertexSnippetOutputGeometryImpostorTexcoords,
		kVertexSnippetOutputPaintTexcoords,

		kVertexSnippetOutputFireTexcoords,
		kVertexSnippetOutputFireArrayTexcoords,

		kVertexSnippetCalculateCameraDistance,
		kVertexSnippetOutputCameraWarpFunction,
		kVertexSnippetOutputCameraBumpWarpFunction,

		kVertexSnippetOutputDistortionDepth,

		kVertexSnippetOutputImpostorDepth,
		kVertexSnippetOutputImpostorRadius,
		kVertexSnippetOutputImpostorShadowRadius,

		kVertexSnippetOutputPointLightAttenuation,
		kVertexSnippetOutputSpotLightAttenuation,
		kVertexSnippetOutputCascadeTexcoord,
		kVertexSnippetOutputPointProjectTexcoord,
		kVertexSnippetOutputSpotProjectTexcoord,
		kVertexSnippetOutputSpotShadowTexcoord,

		kVertexSnippetOutputRadiosityPosition,
		kVertexSnippetOutputRadiosityNormal,
		kVertexSnippetOutputRadiosityViewNormal,
		kVertexSnippetOutputRadiosityTangent,
		kVertexSnippetOutputRadiosityTangentHandedness,

		kVertexSnippetOutputFiniteConstantFogFactors,
		kVertexSnippetOutputInfiniteConstantFogFactors,
		kVertexSnippetOutputFiniteLinearFogFactors,
		kVertexSnippetOutputInfiniteLinearFogFactors,

		kVertexSnippetMotionBlurTransform,
		kVertexSnippetDeformMotionBlurTransform,
		kVertexSnippetVelocityMotionBlurTransform,
		kVertexSnippetInfiniteMotionBlurTransform,

		kVertexSnippetCount
	};


	enum
	{
		kVertexSnippetPositionFlag		= 1 << 0,
		kVertexSnippetNormalFlag		= 1 << 1,
		kVertexSnippetTangentFlag		= 1 << 2
	};


	struct VertexSnippet
	{
		Type				signature;
		unsigned_int32		flags;

		const char			*shaderCode;
	};


	struct VertexAssembly
	{
		unsigned_int32			*signatureStorage;
		const VertexSnippet		*vertexSnippet[kMaxVertexSnippetCount];

		VertexAssembly(unsigned_int32 *storage)
		{
			signatureStorage = storage;
			storage[0] = 0;
		}

		void AddSnippet(const VertexSnippet *snippet)
		{
			unsigned_int32 count = signatureStorage[0];
			Assert(count < kMaxVertexSnippetCount, "VertexAssembly::AddSnippet(), vertex snippet table overflow\n");

			vertexSnippet[count] = snippet;
			signatureStorage[++count] = snippet->signature;
			signatureStorage[0] = count;
		}
	};


	class VertexShader : public Render::VertexShaderObject, public Shared, public HashTableElement<VertexShader>
	{
		public:

			typedef ShaderSignature KeyType;

		private:

			static Storage<HashTable<VertexShader>>		hashTable;

			#if C4CONSOLE //[ CONSOLE

				// -- Console code hidden --

			#endif //]

			unsigned_int32		shaderSignature[1];

			VertexShader(const char *source, unsigned_int32 size, const unsigned_int32 *signature);

			#if C4CONSOLE //[ CONSOLE

				// -- Console code hidden --

			#endif //]

			~VertexShader();

		public:

			static const char				prologText[];
			static const char				epilogText[];

			static const VertexSnippet		nullTransform;
			static const VertexSnippet		modelviewProjectTransform;
			static const VertexSnippet		modelviewProjectTransformInfinite;
			static const VertexSnippet		modelviewProjectTransformHomogeneous;

			static const VertexSnippet		calculateCameraDirection;
			static const VertexSnippet		calculateCameraDirection4D;
			static const VertexSnippet		scaleVertexCalculateCameraDirection;
			static const VertexSnippet		scaleVertexCalculateCameraDirection4D;

			static const VertexSnippet		calculateBillboardPosition;
			static const VertexSnippet		calculateBillboardScalePosition;
			static const VertexSnippet		calculateVertexBillboardPosition;
			static const VertexSnippet		calculateVertexBillboardScalePosition;
			static const VertexSnippet		calculateLightedBillboardPosition;
			static const VertexSnippet		calculatePostboardPosition;
			static const VertexSnippet		calculatePostboardScalePosition;
			static const VertexSnippet		calculatePolyboardNormal;
			static const VertexSnippet		calculateLinearPolyboardNormal;
			static const VertexSnippet		calculatePolyboardPosition;
			static const VertexSnippet		calculatePolyboardScalePosition;

			static const VertexSnippet		calculateScalePosition;
			static const VertexSnippet		calculateScaleOffsetPosition;
			static const VertexSnippet		calculateExpandNormalPosition;

			static const VertexSnippet		calculateTerrainBorderPosition;
			static const VertexSnippet		calculateWaterHeightPosition;

			static const VertexSnippet		texcoordVertexTransform;
			static const VertexSnippet		extractGlowBloomTransform;
			static const VertexSnippet		postProcessTransform;

			static const VertexSnippet		outputNormalTexcoord;
			static const VertexSnippet		outputPrimaryColor;
			static const VertexSnippet		outputSecondaryColor;
			static const VertexSnippet		outputPointSize;
			static const VertexSnippet		outputInfinitePointSize;

			static const VertexSnippet		copyPrimaryTexcoord0;
			static const VertexSnippet		copyPrimaryTexcoord1;
			static const VertexSnippet		copySecondaryTexcoord1;
			static const VertexSnippet		transformPrimaryTexcoord0;
			static const VertexSnippet		transformPrimaryTexcoord1;
			static const VertexSnippet		transformSecondaryTexcoord1;
			static const VertexSnippet		animatePrimaryTexcoord0;
			static const VertexSnippet		animatePrimaryTexcoord1;
			static const VertexSnippet		animateSecondaryTexcoord1;
			static const VertexSnippet		transformAnimatePrimaryTexcoord0;
			static const VertexSnippet		transformAnimatePrimaryTexcoord1;
			static const VertexSnippet		transformAnimateSecondaryTexcoord1;
			static const VertexSnippet		generateTexcoord0;
			static const VertexSnippet		generateTexcoord1;
			static const VertexSnippet		generateTransformTexcoord0;
			static const VertexSnippet		generateTransformTexcoord1;
			static const VertexSnippet		generateBaseTexcoord;
			static const VertexSnippet		generateAnimateTexcoord0;
			static const VertexSnippet		generateAnimateTexcoord1;
			static const VertexSnippet		generateTransformAnimateTexcoord0;
			static const VertexSnippet		generateTransformAnimateTexcoord1;

			static const VertexSnippet		normalizeNormal;
			static const VertexSnippet		normalizeTangent;
			static const VertexSnippet		orthonormalizeTangent;
			static const VertexSnippet		generateTangent;
			static const VertexSnippet		generateImpostorFrame;
			static const VertexSnippet		calculateBitangent;
			static const VertexSnippet		adjustBitangent;

			static const VertexSnippet		vertexSnippet[kVertexSnippetCount];

			KeyType GetKey(void) const
			{
				return (ShaderSignature(shaderSignature));
			}

			static void Initialize(void);
			static void Terminate(void);

			static unsigned_int32 Hash(const KeyType& key);

			static VertexShader *Find(const unsigned_int32 *signature);
			static VertexShader *Get(const VertexAssembly *assembly);
			static VertexShader *New(const char *source, unsigned_int32 size, const unsigned_int32 *signature);

			static void ReleaseCache(void);

			#if C4CONSOLE //[ CONSOLE

				// -- Console code hidden --

			#endif //]
	};
}


#endif

// ZYUQURM
