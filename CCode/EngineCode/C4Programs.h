 

#ifndef C4Programs_h
#define C4Programs_h


#include "C4Render.h"
#include "C4Resources.h"


namespace C4
{
	enum
	{
		kUniversalParamShaderTime						= 0,		// Time (normalized, ms, delta, 0.0)

		kUniversalParamCameraData						= 1,		// Camera data (frustum, ortho, near depth, far depth)
		kUniversalParamViewportTransform				= 2,		// Viewport (w, h, l, b)

		kUniversalParamLightColor						= 3,		// Light color (r, g, b, 1.0)
		kUniversalParamLightRange						= 4,		// Light range (r, 0.0, 0.0, 1 / r)

		kUniversalParamFogColor							= 5,		// Fog color (r, g, b, a)
		kUniversalParamFogParams1						= 6,		// Fog params (F ∧ C, F ∧ C <= 0, F ∧ C > 0, sgn(F ∧ C))
		kUniversalParamFogParams2						= 7,		// Fog params (F ∧ C, -(F ∧ C)², F ∧ C <= 0, density / ln 2)

		kUniversalParamRadiusPointFactor				= 8,		// Radius-to-point-size factor
		kUniversalParamPointCameraPlane					= 9,		// World-space camera plane over radius-to-point-size factor

		kUniversalParamShadowSample1					= 10,		// Shadow sample offsets (ds1, dt1, ds2, dt2)
		kUniversalParamShadowSample2					= 11,		// Shadow sample offsets (ds3, dt3, ds4, dt4)
		kUniversalParamShadowMapScale1					= 12,		// Shadow map (s, t, p) texcoord scale cascade 1
		kUniversalParamShadowMapScale2					= 13,		// Shadow map (s, t, p) texcoord scale, cascade 2
		kUniversalParamShadowMapScale3					= 14,		// Shadow map (s, t, p) texcoord scale, cascade 3
		kUniversalParamShadowMapOffset1					= 15,		// Shadow map (s, t, p) texcoord offset, cascade 1
		kUniversalParamShadowMapOffset2					= 16,		// Shadow map (s, t, p) texcoord offset, cascade 2
		kUniversalParamShadowMapOffset3					= 17,		// Shadow map (s, t, p) texcoord offset, cascade 3
		kUniversalParamShadowViewDirection				= 18,		// Shadow-space scaled view direction, cascade 0

		kUniversalParamImpostorCameraPosition			= 19,		// Camera position used for impostor transitions
		kUniversalParamImpostorShadowBlend				= 20,		// Impostor shadow map elevation blend
		kUniversalParamImpostorShadowScale				= 21,		// Impostor shadow map elevation scales

		kUniversalParamVelocityScale					= 22,		// Velocity scale (x scale, y scale, 0.0, 0.0)
		kUniversalParamFrameBufferScale					= 23,		// Frame buffer scale (1 / width, 1 / height, distortion x scale, distortion y scale)

		kUniversalParamCount
	};

	static_assert(kUniversalParamCount == Render::kMaxUniversalParamCount, "Universal param count mismatch");


	#define UNIVERSAL_PARAM_SHADER_TIME					"0"

	#define UNIVERSAL_PARAM_CAMERA_DATA					"1"
	#define UNIVERSAL_PARAM_VIEWPORT_TRANSFORM			"2"

	#define UNIVERSAL_PARAM_LIGHT_COLOR					"3"
	#define UNIVERSAL_PARAM_LIGHT_RANGE					"4"

	#define UNIVERSAL_PARAM_FOG_COLOR					"5"
	#define UNIVERSAL_PARAM_FOG_PARAMS1					"6"
	#define UNIVERSAL_PARAM_FOG_PARAMS2					"7"

	#define UNIVERSAL_PARAM_RADIUS_POINT_FACTOR			"8"
	#define UNIVERSAL_PARAM_POINT_CAMERA_PLANE			"9"

	#define UNIVERSAL_PARAM_SHADOW_SAMPLE1				"10"
	#define UNIVERSAL_PARAM_SHADOW_SAMPLE2				"11"
	#define UNIVERSAL_PARAM_SHADOW_MAP_SCALE1			"12"
	#define UNIVERSAL_PARAM_SHADOW_MAP_SCALE2			"13"
	#define UNIVERSAL_PARAM_SHADOW_MAP_SCALE3			"14"
	#define UNIVERSAL_PARAM_SHADOW_MAP_OFFSET1			"15"
	#define UNIVERSAL_PARAM_SHADOW_MAP_OFFSET2			"16"
	#define UNIVERSAL_PARAM_SHADOW_MAP_OFFSET3			"17"
	#define UNIVERSAL_PARAM_SHADOW_VIEW_DIRECTION		"18"

	#define UNIVERSAL_PARAM_IMPOSTOR_CAMERA_POSITION	"19"
	#define UNIVERSAL_PARAM_IMPOSTOR_SHADOW_BLEND		"20"
	#define UNIVERSAL_PARAM_IMPOSTOR_SHADOW_SCALE		"21"

	#define UNIVERSAL_PARAM_VELOCITY_SCALE				"22"
	#define UNIVERSAL_PARAM_FRAME_BUFFER_SCALE			"23"

	#define UNIVERSAL_PARAM_COUNT						"24"


	#if C4OPENGL

		#define UPARAM(index)			"uparam[" index "]"

	#elif C4PSSL //[ PS4

		// -- PS4 code hidden --

	#elif C4CG //[ PS3

		// -- PS3 code hidden --

	#endif //]


	class VertexShader; 
	class FragmentShader;
	class GeometryShader;
	class Renderable; 

 
	typedef void ShaderStateProc(const Renderable *, const void *);

	struct ShaderStateData 
	{
		ShaderStateProc		*stateProc; 
		const void			*stateCookie; 
	};


	struct ProgramStageTable 
	{
		VertexShader		*vertexShader;
		FragmentShader		*fragmentShader;
		GeometryShader		*geometryShader;

		ProgramStageTable()
		{
			geometryShader = nullptr;
		}

		bool operator ==(const ProgramStageTable& x) const;
	};


	class ShaderSignature
	{
		private:

			const unsigned_int32	*signature;

		public:

			explicit ShaderSignature(const unsigned_int32 *sig)
			{
				signature = sig;
			}

			operator bool(void) const
			{
				return (signature != nullptr);
			}

			const unsigned_int32& operator [](machine k) const
			{
				return (signature[k]);
			}

			bool operator ==(const ShaderSignature& x) const;
	};


	class ProgramSignature
	{
		private:

			ShaderSignature		vertexSignature;
			ShaderSignature		fragmentSignature;
			ShaderSignature		geometrySignature;

		public:

			ProgramSignature(const ProgramStageTable& table);
			ProgramSignature(const unsigned_int32 *vertexSig, const unsigned_int32 *fragmentSig, const unsigned_int32 *geometrySig);

			const ShaderSignature& GetVertexSignature(void) const
			{
				return (vertexSignature);
			}

			const ShaderSignature& GetFragmentSignature(void) const
			{
				return (fragmentSignature);
			}

			const ShaderSignature& GetGeometrySignature(void) const
			{
				return (geometrySignature);
			}

			bool operator ==(const ProgramSignature& x) const;
	};


	class ProgramBinary : public HashTableElement<ProgramBinary>
	{
		public:

			typedef ProgramSignature KeyType;

		private:

			static Storage<HashTable<ProgramBinary>>	hashTable;

			unsigned_int32				binaryFormat;
			unsigned_int32				binarySize;

			unsigned_int32				signatureSize;
			Storage<ProgramSignature>	programSignature;

			ProgramBinary(const ProgramStageTable& table, unsigned_int32 format, const void *data, unsigned_int32 size);

		public:

			~ProgramBinary();

			const KeyType& GetKey(void) const
			{
				return (*programSignature);
			}

			unsigned_int32 GetBinaryFormat(void) const
			{
				return (binaryFormat);
			}

			unsigned_int32 GetBinarySize(void) const
			{
				return (binarySize);
			}

			const void *GetBinaryData(void) const
			{
				return (reinterpret_cast<const char *>(this + 1) + signatureSize);
			}

			static ProgramBinary *Find(const ProgramStageTable& table)
			{
				return (hashTable->Find(ProgramSignature(table)));
			}

			static void Initialize(void);
			static void Terminate(void);

			static unsigned_int32 Hash(const KeyType& key);

			static ProgramBinary *New(const ProgramStageTable& table, unsigned_int32 binaryFormat, const void *binaryData, unsigned_int32 binarySize);
	};


	class ShaderProgram : public Render::ShaderProgramObject, public Shared, public HashTableElement<ShaderProgram>, public LinkTarget<ShaderProgram>
	{
		public:

			typedef ProgramStageTable KeyType;

		private:

			static Storage<HashTable<ShaderProgram>>	hashTable;

			#if C4CONSOLE //[ CONSOLE

				// -- Console code hidden --

			#endif //]

			ProgramStageTable		stageTable;

		public:

			ShaderProgram(const ProgramStageTable& table);
			~ShaderProgram();

			#if C4CONSOLE //[ CONSOLE

				// -- Console code hidden --

			#endif //]

			const KeyType& GetKey(void) const
			{
				return (stageTable);
			}

			VertexShader *GetVertexShader(void) const
			{
				return (stageTable.vertexShader);
			}

			FragmentShader *GetFragmentShader(void) const
			{
				return (stageTable.fragmentShader);
			}

			GeometryShader *GetGeometryShader(void) const
			{
				return (stageTable.geometryShader);
			}

			static void Initialize(void);
			static void Terminate(void);

			static unsigned_int32 Hash(const KeyType& key);

			static ShaderProgram *Get(const ProgramStageTable& table);

			static void ReleaseCache(void);
			static void Purge(void);
	};
}


#endif

// ZYUQURM
