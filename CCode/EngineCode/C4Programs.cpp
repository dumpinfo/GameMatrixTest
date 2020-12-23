 

#include "C4Programs.h"
#include "C4Graphics.h"

#if C4DEBUG

	#include "C4Engine.h"

#endif


using namespace C4;


Storage<HashTable<ProgramBinary>> ProgramBinary::hashTable;

Storage<HashTable<ShaderProgram>> ShaderProgram::hashTable;

#if C4CONSOLE //[ CONSOLE

	// -- Console code hidden --

#endif //]


bool ProgramStageTable::operator ==(const ProgramStageTable& x) const
{
	return ((vertexShader == x.vertexShader) && (fragmentShader == x.fragmentShader) && (geometryShader == x.geometryShader));
}


bool ShaderSignature::operator ==(const ShaderSignature& x) const
{
	if (!signature)
	{
		return (x.signature == nullptr);
	}

	if (!x.signature)
	{
		return (false);
	}

	unsigned_int32 size = signature[0];
	if (x.signature[0] != size)
	{
		return (false);
	}

	for (unsigned_machine a = 1; a <= size; a++)
	{
		if (signature[a] != x.signature[a])
		{
			return (false);
		}
	}

	return (true);
}


ProgramSignature::ProgramSignature(const ProgramStageTable& table) :
		vertexSignature(&table.vertexShader->GetKey()[0]),
		fragmentSignature(&table.fragmentShader->GetKey()[0]),
		geometrySignature((table.geometryShader) ? &table.geometryShader->GetKey()[0] : nullptr)
{
}

ProgramSignature::ProgramSignature(const unsigned_int32 *vertexSig, const unsigned_int32 *fragmentSig, const unsigned_int32 *geometrySig) :
		vertexSignature(vertexSig),
		fragmentSignature(fragmentSig),
		geometrySignature(geometrySig)
{
}

bool ProgramSignature::operator ==(const ProgramSignature& x) const
{
	return ((vertexSignature == x.vertexSignature) && (fragmentSignature == x.fragmentSignature) && (geometrySignature == x.geometrySignature));
}


ProgramBinary::ProgramBinary(const ProgramStageTable& table, unsigned_int32 format, const void *data, unsigned_int32 size)
{
	binaryFormat = format;
	binarySize = size;

	ShaderSignature vertexSignature = table.vertexShader->GetKey();
	unsigned_int32 vertexSignatureSize = vertexSignature[0] + 1;
	unsigned_int32 *vertexSignatureData = reinterpret_cast<unsigned_int32 *>(this + 1);
	MemoryMgr::CopyMemory(&vertexSignature[0], vertexSignatureData, vertexSignatureSize * 4);

	ShaderSignature fragmentSignature = table.fragmentShader->GetKey();
	unsigned_int32 fragmentSignatureSize = fragmentSignature[0] + 1;
	unsigned_int32 *fragmentSignatureData = vertexSignatureData + vertexSignatureSize;
	MemoryMgr::CopyMemory(&fragmentSignature[0], fragmentSignatureData, fragmentSignatureSize * 4);

	unsigned_int32 geometrySignatureSize = 0;
	unsigned_int32 *geometrySignatureData = nullptr;

	const GeometryShader *geometryShader = table.geometryShader;
	if (geometryShader)
	{ 
		ShaderSignature geometrySignature = geometryShader->GetKey();
		geometrySignatureSize = geometrySignature[0] + 1;
		geometrySignatureData = fragmentSignatureData + fragmentSignatureSize; 
		MemoryMgr::CopyMemory(&geometrySignature[0], geometrySignatureData, geometrySignatureSize * 4);
	} 

	signatureSize = (vertexSignatureSize + fragmentSignatureSize + geometrySignatureSize) * 4;
	new(programSignature) ProgramSignature(vertexSignatureData, fragmentSignatureData, geometrySignatureData); 

	MemoryMgr::CopyMemory(data, reinterpret_cast<char *>(this + 1) + signatureSize, size); 
} 

ProgramBinary::~ProgramBinary()
{
} 

void ProgramBinary::Initialize(void)
{
	new(hashTable) HashTable<ProgramBinary>(16, 16);
}

void ProgramBinary::Terminate(void)
{
	hashTable->~HashTable();
}

unsigned_int32 ProgramBinary::Hash(const KeyType& key)
{
	unsigned_int32 shaderMask = 0x03;

	const ShaderSignature& geometrySignature = key.GetGeometrySignature();
	if (geometrySignature)
	{
		shaderMask |= 0x04;
	}

	unsigned_int32 hash = (shaderMask << 5) | (shaderMask >> 27);

	const ShaderSignature& vertexSignature = key.GetVertexSignature();
	int32 count = vertexSignature[0];
	for (machine a = 1; a <= count; a++)
	{
		hash += vertexSignature[a];
		hash = (hash << 5) | (hash >> 27);
	}

	const ShaderSignature& fragmentSignature = key.GetFragmentSignature();
	count = fragmentSignature[0];
	for (machine a = 1; a <= count; a++)
	{
		hash += fragmentSignature[a];
		hash = (hash << 5) | (hash >> 27);
	}

	if (geometrySignature)
	{
		count = geometrySignature[0];
		for (machine a = 1; a <= count; a++)
		{
			hash += geometrySignature[a];
			hash = (hash << 5) | (hash >> 27);
		}
	}

	return (hash);
}

ProgramBinary *ProgramBinary::New(const ProgramStageTable& table, unsigned_int32 binaryFormat, const void *binaryData, unsigned_int32 binarySize)
{
	unsigned_int32 signatureSize = table.vertexShader->GetKey()[0] + table.fragmentShader->GetKey()[0] + 2;

	const GeometryShader *geometryShader = table.geometryShader;
	if (geometryShader)
	{
		signatureSize += geometryShader->GetKey()[0] + 1;
	}

	ProgramBinary *binary = MemoryMgr::GetMainHeap()->New<ProgramBinary>(sizeof(ProgramBinary) + signatureSize * 4 + binarySize);
	new(binary) ProgramBinary(table, binaryFormat, binaryData, binarySize);
	hashTable->Insert(binary);
	return (binary);
}


ShaderProgram::ShaderProgram(const ProgramStageTable& table)
{
	VertexShader *vertexShader = table.vertexShader;
	stageTable.vertexShader = vertexShader;

	vertexShader->Retain();
	SetVertexShader(vertexShader);

	FragmentShader *fragmentShader = table.fragmentShader;
	stageTable.fragmentShader = fragmentShader;

	fragmentShader->Retain();
	SetFragmentShader(fragmentShader);

	GeometryShader *geometryShader = table.geometryShader;
	stageTable.geometryShader = geometryShader;

	if (geometryShader)
	{
		geometryShader->Retain();
		SetGeometryShader(geometryShader);
	}

	#if C4OPENGL

		bool binaryFlag = TheGraphicsMgr->GetCapabilities()->extensionFlag[kExtensionGetProgramBinary];
		if (binaryFlag)
		{
			const ProgramBinary *programBinary = ProgramBinary::Find(table);
			if (programBinary)
			{
				if (SetProgramBinary(programBinary->GetBinaryFormat(), programBinary->GetBinaryData(), programBinary->GetBinarySize()))
				{
					Preprocess();
					return;
				}

				// The binary was rejected by the driver, so delete it from the cache.

				delete programBinary;
			}

			glProgramParameteri(GetProgramIdentifier(), GL_PROGRAM_BINARY_RETRIEVABLE_HINT, true);
		}

	#endif

	Activate();

	#if C4DEBUG && C4LOG_FILE

		const char		*error, *source;

		if (!GetProgramStatus(&error, &source))
		{
			Engine::Report("Shader error logged", kReportError);
			Engine::Report("Shader error<br/>\r\n", kReportLog | kReportError);
			Engine::Report(error, kReportLog | kReportError | kReportFormatted);

			if (source)
			{
				Engine::LogSource(source);
			}

			ReleaseProgramStatus(error, source);
		}

	#endif

	Preprocess();

	#if C4OPENGL

		if (binaryFlag)
		{
			GLint		binarySize;
			GLenum		format;

			GLuint identifier = GetProgramIdentifier();
			glGetProgramiv(identifier, GL_PROGRAM_BINARY_LENGTH, &binarySize);

			char *binaryData = new char[binarySize];
			glGetProgramBinary(identifier, binarySize, nullptr, &format, binaryData);

			ProgramBinary::New(table, format, binaryData, binarySize);

			delete[] binaryData;
		}

	#endif
}

ShaderProgram::~ShaderProgram()
{
	GeometryShader *geometryShader = stageTable.geometryShader;
	if (geometryShader)
	{
		geometryShader->Release();
	}

	stageTable.fragmentShader->Release();
	stageTable.vertexShader->Release();
}

void ShaderProgram::Initialize(void)
{
	VertexShader::Initialize();
	FragmentShader::Initialize();
	GeometryShader::Initialize();
	ProgramBinary::Initialize();

	new(hashTable) HashTable<ShaderProgram>(16, 16);
}

void ShaderProgram::Terminate(void)
{
	hashTable->~HashTable();

	ProgramBinary::Terminate();
	GeometryShader::Terminate();
	FragmentShader::Terminate();
	VertexShader::Terminate();
}

unsigned_int32 ShaderProgram::Hash(const KeyType& key)
{
	machine_address address = GetPointerAddress(key.vertexShader);
	address |= GetPointerAddress(key.fragmentShader);
	address |= GetPointerAddress(key.geometryShader);
	return ((unsigned_int32) (address >> 4));
}

ShaderProgram *ShaderProgram::Get(const ProgramStageTable& table)
{
	ShaderProgram *program = hashTable->Find(table);
	if (!program)
	{
		program = new ShaderProgram(table);
		hashTable->Insert(program);
	}

	program->Retain();
	return (program);
}

void ShaderProgram::ReleaseCache(void)
{
	int32 bucketCount = hashTable->GetBucketCount();
	for (machine a = 0; a < bucketCount; a++)
	{
		ShaderProgram *program = hashTable->GetFirstBucketElement(a);
		while (program)
		{
			ShaderProgram *next = program->Next();

			if (program->GetReferenceCount() == 1)
			{
				program->Release();
			}

			program = next;
		}
	}

	VertexShader::ReleaseCache();
	FragmentShader::ReleaseCache();
	GeometryShader::ReleaseCache();
}

void ShaderProgram::Purge(void)
{
	hashTable->Purge();

	VertexShader::ReleaseCache();
	FragmentShader::ReleaseCache();
	GeometryShader::ReleaseCache();
}

// ZYUQURM
