 

#ifndef C4GeometryShaders_h
#define C4GeometryShaders_h


#include "C4VertexShaders.h"


namespace C4
{
	enum
	{
		kGeometryParamMatrixMVP					= 0,		// Model-view-projection matrix
		kGeometryParamCameraPosition			= 4,		// Object-space camera position
		kGeometryParamCameraPlane				= 5,		// Object-space camera plane
		kGeometryParamExpand					= 6,		// Expansion parameters (x scale, y scale, z scale, radius)

		kGeometryParamCount						= 7
	};

	static_assert(kGeometryParamCount == Render::kMaxGeometryParamCount, "Geometry param count mismatch");


	#define GEOMETRY_PARAM_MATRIX_MVP0			"0"
	#define GEOMETRY_PARAM_MATRIX_MVP1			"1"
	#define GEOMETRY_PARAM_MATRIX_MVP2			"2"
	#define GEOMETRY_PARAM_MATRIX_MVP3			"3"

	#define GEOMETRY_PARAM_CAMERA_POSITION		"4"
	#define GEOMETRY_PARAM_CAMERA_PLANE			"5"
	#define GEOMETRY_PARAM_EXPAND				"6"

	#define GEOMETRY_PARAM_COUNT				"7"


	enum
	{
		kGeometryShaderExtrudeNormalLine,
		kGeometryShaderExpandLineSegment,
		kGeometryShaderCount
	};


	struct GeometryAssembly
	{
		unsigned_int32			signature[2];

		int32					vertexSnippetCount;
		const VertexSnippet		*vertexSnippet[4];

		ShaderStateProc			*stateProc;
		const char				*shaderSource;
	};


	class GeometryShader : public Render::GeometryShaderObject, public Shared, public HashTableElement<GeometryShader>
	{
		public:

			typedef ShaderSignature KeyType;

		private:

			static Storage<HashTable<GeometryShader>>	hashTable;

			unsigned_int32			shaderSignature[1];

			GeometryShader(const char *source, unsigned_int32 size, const unsigned_int32 *signature);
			~GeometryShader();

		public:

			static const GeometryAssembly	geometryAssembly[kGeometryShaderCount];

			KeyType GetKey(void) const
			{
				return (ShaderSignature(shaderSignature));
			}

			static void Initialize(void);
			static void Terminate(void);

			static unsigned_int32 Hash(const KeyType& key);

			static GeometryShader *Find(const unsigned_int32 *signature);
			static GeometryShader *New(const char *source, unsigned_int32 size, const unsigned_int32 *signature);

			static void ReleaseCache(void);

			static void StateProc_ExpandLineSegment(const Renderable *renderable, const void *cookie);
	};
}


#endif

// ZYUQURM
