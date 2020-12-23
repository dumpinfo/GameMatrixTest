 

#include "C4Graphics.h"

#if C4LOG_FILE

	#include "C4Engine.h"

#endif


#define C4LOG_GEOMETRY_SHADERS		0


using namespace C4;


Storage<HashTable<GeometryShader>> GeometryShader::hashTable;


const GeometryAssembly GeometryShader::geometryAssembly[kGeometryShaderCount] =
{
	// kGeometryShaderExtrudeNormalLine
	{
		{1, 'ENRM'}, 2, {&VertexShader::nullTransform, &VertexShader::outputNormalTexcoord}, nullptr,

		#if C4OPENGL

			"#version 330\n"

			"layout(points) in;\n"
			"layout(line_strip, max_vertices = 2) out;\n"

			"in vresult\n"
			"{\n"
				"vec4 vcolor[2];\n"
				"vec4 texcoord[" SHADER_TEXCOORD_COUNT "];\n"
			"} gattrib[];\n"

			"uniform vec4 gparam[" GEOMETRY_PARAM_COUNT "];\n"

			"void main()\n"
			"{\n"
				"gl_Position.x = dot(gl_in[0].gl_Position, gparam[" GEOMETRY_PARAM_MATRIX_MVP0 "]);\n"
				"gl_Position.y = dot(gl_in[0].gl_Position, gparam[" GEOMETRY_PARAM_MATRIX_MVP1 "]);\n"
				"gl_Position.z = dot(gl_in[0].gl_Position, gparam[" GEOMETRY_PARAM_MATRIX_MVP2 "]);\n"
				"gl_Position.w = dot(gl_in[0].gl_Position, gparam[" GEOMETRY_PARAM_MATRIX_MVP3 "]);\n"
				"EmitVertex();\n"

				"vec3 p = gl_in[0].gl_Position.xyz + normalize(gattrib[0].texcoord[0].xyz) * 0.0625;\n"
				"gl_Position.x = dot(p, gparam[" GEOMETRY_PARAM_MATRIX_MVP0 "].xyz) + gparam[" GEOMETRY_PARAM_MATRIX_MVP0 "].w;\n"
				"gl_Position.y = dot(p, gparam[" GEOMETRY_PARAM_MATRIX_MVP1 "].xyz) + gparam[" GEOMETRY_PARAM_MATRIX_MVP1 "].w;\n"
				"gl_Position.z = dot(p, gparam[" GEOMETRY_PARAM_MATRIX_MVP2 "].xyz) + gparam[" GEOMETRY_PARAM_MATRIX_MVP2 "].w;\n"
				"gl_Position.w = dot(p, gparam[" GEOMETRY_PARAM_MATRIX_MVP3 "].xyz) + gparam[" GEOMETRY_PARAM_MATRIX_MVP3 "].w;\n"
				"EmitVertex();\n"
			"}\n"

		#else

			""

		#endif
	},

	// kGeometryShaderExpandLineSegment
	{
		{1, 'ELSG'}, 1, {&VertexShader::nullTransform}, &GeometryShader::StateProc_ExpandLineSegment,

		#if C4OPENGL

			"#version 330\n"

			"layout(lines) in;\n"
			"layout(triangle_strip, max_vertices = 4) out;\n"

			"in vresult\n"
			"{\n"
				"vec4 vcolor[2];\n"
				"vec4 texcoord[" SHADER_TEXCOORD_COUNT "];\n"
			"} gattrib[];\n"

			"out vresult\n"
			"{\n"
				"vec4 vcolor[2];\n"
				"vec4 texcoord[" SHADER_TEXCOORD_COUNT "];\n"
			"} gresult;\n"

			"uniform vec4 gparam[" GEOMETRY_PARAM_COUNT "];\n"

			"layout(std140) uniform universal\n"
			"{\n"
				"vec4 uparam[" UNIVERSAL_PARAM_COUNT "];\n"
			"};\n"

			"void main()\n"
			"{\n"
				"vec3 p1 = gl_in[0].gl_Position.xyz * gparam[" GEOMETRY_PARAM_EXPAND "].xyz;\n"
				"vec3 p2 = gl_in[1].gl_Position.xyz * gparam[" GEOMETRY_PARAM_EXPAND "].xyz;\n"

				"float d1 = dot(p1, gparam[" GEOMETRY_PARAM_CAMERA_PLANE "].xyz) + gparam[" GEOMETRY_PARAM_CAMERA_PLANE "].w;\n"
				"float d2 = dot(p2, gparam[" GEOMETRY_PARAM_CAMERA_PLANE "].xyz) + gparam[" GEOMETRY_PARAM_CAMERA_PLANE "].w;\n"
				"float r1 = (d1 * uparam[" UNIVERSAL_PARAM_CAMERA_DATA "].x + uparam[" UNIVERSAL_PARAM_CAMERA_DATA "].y) * gparam[" GEOMETRY_PARAM_EXPAND "].w;\n"
				"float r2 = (d2 * uparam[" UNIVERSAL_PARAM_CAMERA_DATA "].x + uparam[" UNIVERSAL_PARAM_CAMERA_DATA "].y) * gparam[" GEOMETRY_PARAM_EXPAND "].w;\n" 

				"vec3 dp = p2 - p1;\n"
				"vec3 m = cross(gparam[" GEOMETRY_PARAM_CAMERA_PLANE "].xyz, dp);\n" 
				"m *= inversesqrt(max(dot(m, m), 0.0000152587890625)) * gparam[" GEOMETRY_PARAM_EXPAND "].w;\n"
 
				"vec3 n1 = normalize(cross(p1 - gparam[" GEOMETRY_PARAM_CAMERA_POSITION "].xyz, dp)) * r1;\n"
				"vec3 n2 = normalize(cross(p2 - gparam[" GEOMETRY_PARAM_CAMERA_POSITION "].xyz, dp)) * r2;\n"
				"bool b = bool(uparam[" UNIVERSAL_PARAM_CAMERA_DATA "].x);\n" 
				"n1 = mix(m, n1, bvec3(b, b, b));\n"
				"n2 = mix(m, n2, bvec3(b, b, b));\n" 
 
				"vec3 v = p1 - n1;\n"
				"gl_Position.x = dot(v, gparam[" GEOMETRY_PARAM_MATRIX_MVP0 "].xyz) + gparam[" GEOMETRY_PARAM_MATRIX_MVP0 "].w;\n"
				"gl_Position.y = dot(v, gparam[" GEOMETRY_PARAM_MATRIX_MVP1 "].xyz) + gparam[" GEOMETRY_PARAM_MATRIX_MVP1 "].w;\n"
				"gl_Position.z = dot(v, gparam[" GEOMETRY_PARAM_MATRIX_MVP2 "].xyz) + gparam[" GEOMETRY_PARAM_MATRIX_MVP2 "].w;\n" 
				"gl_Position.w = dot(v, gparam[" GEOMETRY_PARAM_MATRIX_MVP3 "].xyz) + gparam[" GEOMETRY_PARAM_MATRIX_MVP3 "].w;\n"
				"gresult.texcoord[0].xyz = vec3(0.0, 0.0, r1);\n"
				"EmitVertex();\n"

				"v = p1 + n1;\n"
				"gl_Position.x = dot(v, gparam[" GEOMETRY_PARAM_MATRIX_MVP0 "].xyz) + gparam[" GEOMETRY_PARAM_MATRIX_MVP0 "].w;\n"
				"gl_Position.y = dot(v, gparam[" GEOMETRY_PARAM_MATRIX_MVP1 "].xyz) + gparam[" GEOMETRY_PARAM_MATRIX_MVP1 "].w;\n"
				"gl_Position.z = dot(v, gparam[" GEOMETRY_PARAM_MATRIX_MVP2 "].xyz) + gparam[" GEOMETRY_PARAM_MATRIX_MVP2 "].w;\n"
				"gl_Position.w = dot(v, gparam[" GEOMETRY_PARAM_MATRIX_MVP3 "].xyz) + gparam[" GEOMETRY_PARAM_MATRIX_MVP3 "].w;\n"
				"gresult.texcoord[0].xyz = vec3(r1, 0.0, r1);\n"
				"EmitVertex();\n"

				"v = p2 - n2;\n"
				"gl_Position.x = dot(v, gparam[" GEOMETRY_PARAM_MATRIX_MVP0 "].xyz) + gparam[" GEOMETRY_PARAM_MATRIX_MVP0 "].w;\n"
				"gl_Position.y = dot(v, gparam[" GEOMETRY_PARAM_MATRIX_MVP1 "].xyz) + gparam[" GEOMETRY_PARAM_MATRIX_MVP1 "].w;\n"
				"gl_Position.z = dot(v, gparam[" GEOMETRY_PARAM_MATRIX_MVP2 "].xyz) + gparam[" GEOMETRY_PARAM_MATRIX_MVP2 "].w;\n"
				"gl_Position.w = dot(v, gparam[" GEOMETRY_PARAM_MATRIX_MVP3 "].xyz) + gparam[" GEOMETRY_PARAM_MATRIX_MVP3 "].w;\n"
				"gresult.texcoord[0].xyz = vec3(0.0, 0.0, r2);\n"
				"EmitVertex();\n"

				"v = p2 + n2;\n"
				"gl_Position.x = dot(v, gparam[" GEOMETRY_PARAM_MATRIX_MVP0 "].xyz) + gparam[" GEOMETRY_PARAM_MATRIX_MVP0 "].w;\n"
				"gl_Position.y = dot(v, gparam[" GEOMETRY_PARAM_MATRIX_MVP1 "].xyz) + gparam[" GEOMETRY_PARAM_MATRIX_MVP1 "].w;\n"
				"gl_Position.z = dot(v, gparam[" GEOMETRY_PARAM_MATRIX_MVP2 "].xyz) + gparam[" GEOMETRY_PARAM_MATRIX_MVP2 "].w;\n"
				"gl_Position.w = dot(v, gparam[" GEOMETRY_PARAM_MATRIX_MVP3 "].xyz) + gparam[" GEOMETRY_PARAM_MATRIX_MVP3 "].w;\n"
				"gresult.texcoord[0].xyz = vec3(r2, 0.0, r2);\n"
				"EmitVertex();\n"
			"}\n"

		#else

			""

		#endif
	}
};


GeometryShader::GeometryShader(const char *source, unsigned_int32 size, const unsigned_int32 *signature) : GeometryShaderObject(source, size)
{
	MemoryMgr::CopyMemory(signature, shaderSignature, signature[0] * 4 + 4);

	#if C4LOG_FILE && C4LOG_GEOMETRY_SHADERS

		Engine::LogSource(source);

	#endif
}

GeometryShader::~GeometryShader()
{
}

void GeometryShader::Initialize(void)
{
	new(hashTable) HashTable<GeometryShader>(16, 16);
}

void GeometryShader::Terminate(void)
{
	hashTable->~HashTable();
}

unsigned_int32 GeometryShader::Hash(const KeyType& key)
{
	unsigned_int32 hash = 0;

	int32 count = key[0];
	for (machine a = 1; a <= count; a++)
	{
		hash += key[a];
		hash = (hash << 5) | (hash >> 27);
	}

	return (hash);
}

GeometryShader *GeometryShader::Find(const unsigned_int32 *signature)
{
	GeometryShader *shader = hashTable->Find(ShaderSignature(signature));
	if (shader)
	{
		shader->Retain();
	}

	return (shader);
}

GeometryShader *GeometryShader::New(const char *source, unsigned_int32 size, const unsigned_int32 *signature)
{
	GeometryShader *shader = MemoryMgr::GetMainHeap()->New<GeometryShader>(sizeof(GeometryShader) + signature[0] * 4);
	new(shader) GeometryShader(source, size, signature);
	hashTable->Insert(shader);

	shader->Retain();
	return (shader);
}

void GeometryShader::ReleaseCache(void)
{
	int32 bucketCount = hashTable->GetBucketCount();
	for (machine a = 0; a < bucketCount; a++)
	{
		GeometryShader *shader = hashTable->GetFirstBucketElement(a);
		while (shader)
		{
			GeometryShader *next = shader->Next();
			if (shader->GetReferenceCount() == 1)
			{
				shader->Release();
			}

			shader = next;
		}
	}
}

void GeometryShader::StateProc_ExpandLineSegment(const Renderable *renderable, const void *cookie)
{
	Render::SetGeometryShaderParameter(kGeometryParamMatrixMVP, TheGraphicsMgr->GetCurrentMVPMatrix());

	const Transform4D& cameraTransform = TheGraphicsMgr->GetCameraTransformable()->GetWorldTransform();
	Render::SetGeometryShaderParameter(kGeometryParamCameraPosition, renderable->GetTransformable()->GetInverseWorldTransform() * cameraTransform.GetTranslation());

	Antivector4D plane(cameraTransform[2], cameraTransform.GetTranslation());
	Render::SetGeometryShaderParameter(kGeometryParamCameraPlane, plane * renderable->GetTransformable()->GetWorldTransform());

	Render::SetGeometryShaderParameter(kGeometryParamExpand, *renderable->GetRenderParameterPointer());
}

// ZYUQURM
