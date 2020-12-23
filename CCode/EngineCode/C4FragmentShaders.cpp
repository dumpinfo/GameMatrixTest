 

#include "C4FragmentShaders.h"

#if C4LOG_FILE

	#include "C4Engine.h"

#endif


#define C4LOG_FRAGMENT_SHADERS		0


using namespace C4;


Storage<HashTable<FragmentShader>> FragmentShader::hashTable;


#if C4OPENGL

	const char FragmentShader::prolog1Text[] =
	{
		#if C4GLCORE

			"#version 330\n"

		#else //[ MOBILE

			// -- Mobile code hidden --

		#endif //]

		"uniform vec4 fparam[" FRAGMENT_PARAM_COUNT "];\n"

		"layout(std140) uniform universal\n"
		"{\n"
			"vec4 uparam[" UNIVERSAL_PARAM_COUNT "];\n"
		"};\n"

		"in vresult\n"
		"{\n"
			"vec4 vcolor[2];\n"
			"vec4 texcoord[" SHADER_TEXCOORD_COUNT "];\n"
		"};\n"

		"layout(location = 0) out vec4 fcolor;\n"
		"layout(location = 1) out vec2 velocity;\n"
	};

	const char FragmentShader::prolog2Text[] =
	{
		"void main()\n"
		"{\n"
			"vec4 temp"
	};

	const char FragmentShader::epilogText[] =
	{
		"}\n"
	};

#elif C4PSSL //[ PS4

	// -- PS4 code hidden --

#elif C4CG //[ PS3

	// -- PS3 code hidden --

#endif //]


const char FragmentShader::copyZero[] =
{
	#if C4OPENGL

		#if C4GLCORE

			"#version 330\n"

		#else //[ MOBILE

			// -- Mobile code hidden --

		#endif //]

		"layout(location = 0) out vec4 fcolor;\n"

		"void main()\n"
		"{\n"
			"fcolor = vec4(0.0, 0.0, 0.0, 0.0);\n"
		"}\n"

	#elif C4PSSL //[ PS4

		// -- PS4 code hidden --

	#elif C4CG //[ PS3

		// -- PS3 code hidden --
 
	#endif //]
};
 
const char FragmentShader::copyConstant[] =
{ 
	#if C4OPENGL

		#if C4GLCORE 

			"#version 330\n" 
 
		#else //[ MOBILE

			// -- Mobile code hidden --
 
		#endif //]

		"uniform vec4 fparam[" FRAGMENT_PARAM_COUNT "];\n"
		"layout(location = 0) out vec4 fcolor;\n"

		"void main()\n"
		"{\n"
			"fcolor = " FPARAM(FRAGMENT_PARAM_CONSTANT7) ";\n"
		"}\n"

	#elif C4PSSL //[ PS4

		// -- PS4 code hidden --

	#elif C4CG //[ PS3

		// -- PS3 code hidden --

	#endif //]
};


FragmentShader::FragmentShader(const char *source, unsigned_int32 size, const unsigned_int32 *signature) : FragmentShaderObject(source, size)
{
	MemoryMgr::CopyMemory(signature, shaderSignature, signature[0] * 4 + 4);

	#if C4LOG_FILE && C4LOG_FRAGMENT_SHADERS

		Engine::LogSource(source);

	#endif

	#if C4CONSOLE //[ CONSOLE

		// -- Console code hidden --

	#endif //]
}

#if C4CONSOLE //[ CONSOLE

	// -- Console code hidden --

#endif //]

FragmentShader::~FragmentShader()
{
	#if C4CONSOLE //[ CONSOLE

		// -- Console code hidden --

	#endif //]
}

void FragmentShader::Initialize(void)
{
	new(hashTable) HashTable<FragmentShader>(16, 16);

	#if C4CONSOLE //[ CONSOLE

		// -- Console code hidden --

	#endif //]
}

void FragmentShader::Terminate(void)
{
	hashTable->~HashTable();
}

unsigned_int32 FragmentShader::Hash(const KeyType& key)
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

FragmentShader *FragmentShader::Find(const unsigned_int32 *signature)
{
	FragmentShader *shader = hashTable->Find(ShaderSignature(signature));
	if (shader)
	{
		shader->Retain();
	}

	return (shader);
}

FragmentShader *FragmentShader::New(const char *source, unsigned_int32 size, const unsigned_int32 *signature)
{
	FragmentShader *shader = MemoryMgr::GetMainHeap()->New<FragmentShader>(sizeof(FragmentShader) + signature[0] * 4);
	new(shader) FragmentShader(source, size, signature);
	hashTable->Insert(shader);

	shader->Retain();
	return (shader);
}

void FragmentShader::ReleaseCache(void)
{
	int32 bucketCount = hashTable->GetBucketCount();
	for (machine a = 0; a < bucketCount; a++)
	{
		FragmentShader *shader = hashTable->GetFirstBucketElement(a);
		while (shader)
		{
			FragmentShader *next = shader->Next();

			if (shader->GetReferenceCount() == 1)
			{
				shader->Release();
			}

			shader = next;
		}
	}
}

#if C4CONSOLE //[ CONSOLE

	// -- Console code hidden --

#endif //]

// ZYUQURM
