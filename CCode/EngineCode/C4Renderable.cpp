 

#include "C4Renderable.h"
#include "C4Graphics.h"
#include "C4Shaders.h"


using namespace C4;


namespace C4
{
	template <> Heap Memory<ShaderData>::heap("ShaderData", MemoryMgr::CalculatePoolSize(128, sizeof(ShaderData)), kHeapMutexless);
	template class Memory<ShaderData>;
}


int32 VertexBuffer::totalVertexBufferCount = 0;
unsigned_int32 VertexBuffer::totalVertexBufferMemory = 0;

Mutex VertexBuffer::vertexBufferMutex;
List<VertexBuffer> VertexBuffer::vertexBufferList;

List<OcclusionQuery> OcclusionQuery::occlusionQueryList;

List<ShaderData> ShaderData::shaderDataList;


const PaintEnvironment Renderable::nullPaintEnvironment = {Transform4D(1.0F, 0.0F, 0.0F, 0.0F, 0.0F, 1.0F, 0.0F, 0.0F, 0.0F, 0.0F, 1.0F, 0.0F), nullptr};
const AmbientEnvironment Renderable::nullAmbientEnvironment = {0, nullptr};

const ConstVector4D Renderable::nullRenderParameterTable[kMaxRenderParameterCount] = {{0.0F}};
const ConstVector4D Renderable::nullTexcoordParameterTable[kMaxTexcoordParameterCount] = {{1.0F, 1.0F, 0.0F, 0.0F}};
const ConstVector4D Renderable::nullTerrainParameterTable[kMaxTerrainParameterCount] = {{1.0F, 1.0F, 1.0F, 1.0F}, {1.0F, 1.0F, 1.0F, 1.0F}};


VertexBuffer::VertexBuffer(unsigned_int32 flags) : VertexBufferObject((flags & kVertexBufferIndex) ? Render::kVertexBufferTargetIndex : Render::kVertexBufferTargetAttribute, (flags & kVertexBufferDynamic) ? Render::kVertexBufferUsageDynamic : Render::kVertexBufferUsageStatic)
{
	activeFlag = false;
	bufferStorage = nullptr;
}

VertexBuffer::~VertexBuffer()
{
	if (GetOwningList())
	{
		vertexBufferMutex.Acquire();
		ListElement<VertexBuffer>::Detach();
		vertexBufferMutex.Release();
	}

	Deactivate();
	delete[] bufferStorage;
}

void VertexBuffer::Activate(const void *data)
{
	if (!activeFlag)
	{
		activeFlag = true;
		GraphicsMgr::SyncRenderTask(&VertexBufferObject::AllocateStorage, static_cast<VertexBufferObject *>(this), data);

		vertexBufferMutex.Acquire();
		totalVertexBufferCount++;
		totalVertexBufferMemory += GetVertexBufferSize();
		vertexBufferMutex.Release();
	}
}

void VertexBuffer::Deactivate(void)
{
	if (activeFlag)
	{
		activeFlag = false;

		vertexBufferMutex.Acquire();
		totalVertexBufferCount--;
		totalVertexBufferMemory -= GetVertexBufferSize();
		vertexBufferMutex.Release();

		Destruct();
	}
}

void VertexBuffer::Save(void)
{
	if (!bufferStorage)
	{
		unsigned_int32 size = GetVertexBufferSize();
		if (size != 0)
		{
			bufferStorage = new char[size];
			ReadBuffer(0, size, bufferStorage);
		}
	}
}

void VertexBuffer::Restore(void)
{
	Activate(bufferStorage);

	delete[] bufferStorage;
	bufferStorage = nullptr; 
}

void VertexBuffer::Establish(unsigned_int32 size, const void *data) 
{
	if (size != 0) 
	{
		if (!GetOwningList())
		{ 
			vertexBufferMutex.Acquire();
			vertexBufferList.Append(this); 
			vertexBufferMutex.Release(); 
		}

		if (!activeFlag)
		{ 
			SetVertexBufferSize(size);
			Activate(data);
		}
		else
		{
			unsigned_int32 previousSize = GetVertexBufferSize();
			if (previousSize != size)
			{
				SetVertexBufferSize(size);
				GraphicsMgr::SyncRenderTask(&VertexBufferObject::AllocateStorage, static_cast<VertexBufferObject *>(this), data);

				vertexBufferMutex.Acquire();
				totalVertexBufferMemory += size - previousSize;
				vertexBufferMutex.Release();
			}
		}
	}
	else
	{
		Deactivate();
		SetVertexBufferSize(0);

		if (GetOwningList())
		{
			vertexBufferMutex.Acquire();
			ListElement<VertexBuffer>::Detach();
			vertexBufferMutex.Release();
		}
	}
}

volatile void *VertexBuffer::BeginUpdateSync(void)
{
	volatile void	*ptr;

	volatile void **storage = &ptr;
	GraphicsMgr::SyncRenderTask(&VertexBufferObject::BeginUpdateSync, static_cast<VertexBufferObject *>(this), &storage);
	return (ptr);
}

void VertexBuffer::EndUpdateSync(void)
{
	GraphicsMgr::SyncRenderTask(&VertexBufferObject::EndUpdateSync, static_cast<VertexBufferObject *>(this));
}

void VertexBuffer::UpdateBufferSync(unsigned_int32 offset, unsigned_int32 size, const void *data)
{
	Render::BufferUploadData	uploadData;

	uploadData.offset = offset;
	uploadData.size = size;
	uploadData.data = data;

	GraphicsMgr::SyncRenderTask(&VertexBufferObject::UpdateBufferSync, static_cast<VertexBufferObject *>(this), &uploadData);
}

void VertexBuffer::SaveAll(void)
{
	VertexBuffer *vertexBuffer = vertexBufferList.First();
	while (vertexBuffer)
	{
		vertexBuffer->Save();
		vertexBuffer = vertexBuffer->Next();
	}
}

void VertexBuffer::DeactivateAll(void)
{
	VertexBuffer *vertexBuffer = vertexBufferList.First();
	while (vertexBuffer)
	{
		vertexBuffer->Deactivate();
		vertexBuffer = vertexBuffer->Next();
	}
}

void VertexBuffer::ReactivateAll(void)
{
	VertexBuffer *vertexBuffer = vertexBufferList.First();
	while (vertexBuffer)
	{
		vertexBuffer->Restore();
		vertexBuffer = vertexBuffer->Next();
	}
}


SharedVertexBuffer::SharedVertexBuffer(unsigned_int32 flags) : VertexBuffer(flags)
{
	referenceCount = 0;
}

SharedVertexBuffer::~SharedVertexBuffer()
{
}

int32 SharedVertexBuffer::Release(void)
{
	int32 count = --referenceCount;
	if (count == 0)
	{
		Establish(0);
	}

	return (count);
}


VertexData::VertexData()
{
	for (machine a = 0; a < kVertexAttribCount; a++)
	{
		arrayIndex[a] = -1;
	}
}

void VertexData::Update(const Renderable *renderable)
{
	for (machine a = 0; a < kVertexAttribCount; a++)
	{
		int32 index = arrayIndex[a];
		if (index >= 0)
		{
			unsigned_int32 bufferIndex = (renderable->GetVertexBufferArrayFlags() >> index) & 1;
			SetVertexAttribArray(a, renderable->GetVertexBuffer(bufferIndex), renderable->GetVertexAttributeOffset(index), renderable->GetVertexAttributeComponentCount(index), renderable->GetVertexBufferStride(bufferIndex));
		}
	}

	const VertexBuffer *buffer = renderable->GetVertexBuffer(kVertexBufferIndexArray);
	if ((buffer) && (buffer->Active()))
	{
		SetVertexIndexBuffer(buffer);
	}

	#if C4OPENGL

		else if (renderable->GetRenderType() == kRenderQuads)
		{
			SetVertexIndexBuffer(Render::GetQuadIndexBuffer());
		}

	#endif
}


OcclusionQuery::OcclusionQuery(RenderProc *proc, void *cookie)
{
	activeFlag = false;

	renderProc = proc;
	renderCookie = cookie;
}

OcclusionQuery::~OcclusionQuery()
{
	Deactivate();
}

void OcclusionQuery::Activate(void)
{
	if (!activeFlag)
	{
		activeFlag = true;
		queryIndex = 0;

		unsigned_int32 frame = Render::GetFrameCount() - 4;
		for (machine a = 0; a < 4; a++)
		{
			queryFrame[a] = frame;
			queryObject[a].Construct();
		}

		if (!GetOwningList())
		{
			occlusionQueryList.Append(this);
		}
	}
}

void OcclusionQuery::Deactivate(void)
{
	if (activeFlag)
	{
		activeFlag = false;

		for (machine a = 3; a >= 0; a--)
		{
			queryObject[a].Destruct();
		}
	}
}

void OcclusionQuery::DeactivateAll(void)
{
	OcclusionQuery *occlusionQuery = occlusionQueryList.First();
	while (occlusionQuery)
	{
		occlusionQuery->Deactivate();
		occlusionQuery = occlusionQuery->Next();
	}
}

void OcclusionQuery::ReactivateAll(void)
{
	OcclusionQuery *occlusionQuery = occlusionQueryList.First();
	while (occlusionQuery)
	{
		occlusionQuery->Activate();
		occlusionQuery = occlusionQuery->Next();
	}
}

void OcclusionQuery::BeginOcclusionQuery(void)
{
	unsigned_int32 index = queryIndex;
	queryFrame[index] = Render::GetFrameCount();
	queryObject[index].BeginOcclusionQuery();
}

void OcclusionQuery::EndOcclusionQuery(void)
{
	unsigned_int32 index = queryIndex;
	queryIndex = (index + 1) & 3;
	queryObject[index].EndOcclusionQuery();
}

void OcclusionQuery::Process(List<Renderable> *renderList, float normalizer)
{
	unsigned_int32 index = (queryIndex - 3) & 3;
	unsigned_int32 frame = Render::GetFrameCount();
	if ((unsigned_int32) (frame - queryFrame[index]) < 4U)
	{
		unsigned_int32 sampleCount = queryObject[index].GetSamplesPassed();
		if (sampleCount != 0)
		{
			unoccludedArea = (float) sampleCount * normalizer;
			(*renderProc)(this, renderList, renderCookie);
		}
	}
}


ShaderData::ShaderData(const ShaderKey& key, ShaderData **pointer, unsigned_int32 blend, unsigned_int32 material) : shaderKey(key)
{
	shaderDataPointer = pointer;

	ShaderData *next = *pointer;
	nextShaderData = next;
	*pointer = this;

	if (next)
	{
		next->shaderDataPointer = &nextShaderData;
	}

	shaderDataList.Append(this);

	blendState = blend;
	materialState = material;

	shaderProgram = nullptr;
	shaderStateDataCount = 0;
}

ShaderData::~ShaderData()
{
	ShaderData *next = nextShaderData;
	*shaderDataPointer = next;

	if (next)
	{
		next->shaderDataPointer = shaderDataPointer;
	}

	ShaderProgram *program = shaderProgram;
	if (program)
	{
		program->Release();
	}
}

void ShaderData::AddStateProc(ShaderStateProc *proc, const void *cookie)
{
	int32 count = shaderStateDataCount;
	for (machine a = 0; a < count; a++)
	{
		if (shaderStateData[a].stateProc == proc)
		{
			return;
		}
	}

	Assert(count < kMaxShaderStateDataCount, "ShaderData::AddStateProc(), state proc table overflow\n");

	shaderStateData[count].stateProc = proc;
	shaderStateData[count].stateCookie = cookie;
	shaderStateDataCount = count + 1;
}

void ShaderData::Preprocess(void)
{
	textureArray.Preprocess();
}


RenderSegment::RenderSegment(Renderable *renderable, unsigned_int32 state)
{
	nextSegment = nullptr;
	owningRenderable = renderable;

	materialState = state;
	materialObject = nullptr;
	materialAttributeList = nullptr;

	for (machine type = 0; type < kShaderTypeCount; type++)
	{
		segmentShaderData[type] = nullptr;
	}
}

RenderSegment::~RenderSegment()
{
	for (machine type = 0; type < kShaderTypeCount; type++)
	{
		ShaderData *shaderData = segmentShaderData[type];
		while (shaderData)
		{
			ShaderData *next = shaderData->nextShaderData;
			delete shaderData;
			shaderData = next;
		}
	}
}

unsigned_int32 RenderSegment::GetShaderDataMaterialState(ShaderType type)
{
	unsigned_int32 state = materialState;
	if (materialObject)
	{
		const MaterialObject *material = *materialObject;
		if (material)
		{
			state |= material->GetMaterialFlags();
		}
	}

	if (type >= kShaderFirstPlain)
	{
		state &= ~(kMaterialAlphaCoverage | kMaterialSampleShading);
	}
	else
	{
		#if C4OPENGL

			if (!TheGraphicsMgr->GetCapabilities()->extensionFlag[kExtensionSampleShading])
			{
				state &= ~kMaterialSampleShading;
			}

		#endif

		if (state & kMaterialSampleShading)
		{
			state &= ~kMaterialAlphaCoverage;
		}
	}

	return (state);
}

ShaderData *RenderSegment::InitAmbientShaderData(Renderable *renderable, ShaderType type, const ShaderKey& key)
{
	ShaderData *shaderData = new ShaderData(key, &segmentShaderData[type], renderable->GetAmbientBlendState(), GetShaderDataMaterialState(type));

	const Attribute *primaryAttribute = (materialAttributeList) ? materialAttributeList->First() : nullptr;
	const MaterialObject *object = (materialObject) ? *materialObject : nullptr;
	if (object)
	{
		const Attribute *attribute = object->GetFirstAttribute();
		if (attribute)
		{
			primaryAttribute = attribute;
		}
	}

	if ((!primaryAttribute) || (primaryAttribute->GetAttributeType() != kAttributeShader))
	{
		ShaderGraph		shaderGraph;
		Process			*process[kShaderGraphProcessCount];

		ShaderAttribute::BuildRegularShaderGraph(renderable, this, object, materialAttributeList, &shaderGraph, process);
		ShaderAttribute::CompileShader(&shaderGraph, type, key, renderable, this, shaderData);
	}
	else
	{
		static_cast<const ShaderAttribute *>(primaryAttribute)->CompileShader(type, key, renderable, this, shaderData);
	}

	return (shaderData);
}

ShaderData *RenderSegment::InitLightShaderData(Renderable *renderable, ShaderType type, const ShaderKey& key)
{
	unsigned_int32 blendState = (renderable->GetLightBlendState() & kBlendColorMask) | kBlendAlphaPreserve;
	ShaderData *shaderData = new ShaderData(key, &segmentShaderData[type], blendState, GetShaderDataMaterialState(type));

	const Attribute *primaryAttribute = (materialAttributeList) ? materialAttributeList->First() : nullptr;
	const MaterialObject *object = (materialObject) ? *materialObject : nullptr;
	if (object)
	{
		const Attribute *attribute = object->GetFirstAttribute();
		if (attribute)
		{
			primaryAttribute = attribute;
		}
	}

	if ((!primaryAttribute) || (primaryAttribute->GetAttributeType() != kAttributeShader))
	{
		ShaderGraph		shaderGraph;
		Process			*process[kShaderGraphProcessCount];

		ShaderAttribute::BuildRegularShaderGraph(renderable, this, object, materialAttributeList, &shaderGraph, process);
		ShaderAttribute::CompileShader(&shaderGraph, type, key, renderable, this, shaderData);
	}
	else
	{
		static_cast<const ShaderAttribute *>(primaryAttribute)->CompileShader(type, key, renderable, this, shaderData);
	}

	return (shaderData);
}

ShaderData *RenderSegment::InitEffectShaderData(Renderable *renderable, ShaderType type, const ShaderKey& key)
{
	unsigned_int32 blendState = (renderable->GetAmbientBlendState() & kBlendColorMask) | kBlendAlphaPreserve;
	ShaderData *shaderData = new ShaderData(key, &segmentShaderData[type], blendState, GetShaderDataMaterialState(type));

	const Attribute *primaryAttribute = (materialAttributeList) ? materialAttributeList->First() : nullptr;
	const MaterialObject *object = (materialObject) ? *materialObject : nullptr;
	if (object)
	{
		const Attribute *attribute = object->GetFirstAttribute();
		if (attribute)
		{
			primaryAttribute = attribute;
		}
	}

	if ((!primaryAttribute) || (primaryAttribute->GetAttributeType() != kAttributeShader))
	{
		ShaderGraph		shaderGraph;

		ShaderAttribute::BuildEffectShaderGraph(renderable, this, object, materialAttributeList, &shaderGraph);
		ShaderAttribute::CompileShader(&shaderGraph, type, key, renderable, this, shaderData);
	}
	else
	{
		static_cast<const ShaderAttribute *>(primaryAttribute)->CompileShader(type, key, renderable, this, shaderData);
	}

	OcclusionQuery *occlusionQuery = renderable->GetOcclusionQuery();
	if (occlusionQuery)
	{
		occlusionQuery->Activate();
		shaderData->AddStateProc(&Renderable::StateProc_SetOcclusionQuery);
	}

	return (shaderData);
}

ShaderData *RenderSegment::InitPlainShaderData(Renderable *renderable, ShaderType type, const ShaderKey& key)
{
	ShaderData *shaderData = new ShaderData(key, &segmentShaderData[type], kBlendReplace, GetShaderDataMaterialState(type));

	const Attribute *primaryAttribute = (materialAttributeList) ? materialAttributeList->First() : nullptr;
	const MaterialObject *object = (materialObject) ? *materialObject : nullptr;
	if (object)
	{
		const Attribute *attribute = object->GetFirstAttribute();
		if (attribute)
		{
			primaryAttribute = attribute;
		}
	}

	if ((!primaryAttribute) || (primaryAttribute->GetAttributeType() != kAttributeShader))
	{
		ShaderGraph		shaderGraph;

		ShaderAttribute::BuildPlainShaderGraph(type, renderable, this, object, materialAttributeList, &shaderGraph);
		ShaderAttribute::CompileShader(&shaderGraph, type, key, renderable, this, shaderData);
	}
	else
	{
		static_cast<const ShaderAttribute *>(primaryAttribute)->CompileShader(type, key, renderable, this, shaderData);
	}

	return (shaderData);
}

ShaderData *RenderSegment::InitShaderData(Renderable *renderable, ShaderType type, const ShaderKey& key)
{
	if (type <= kShaderLastUnified)
	{
		if ((type <= kShaderLastAmbient) && (renderable->GetShaderFlags() & kShaderAmbientEffect))
		{
			return (InitEffectShaderData(renderable, type, key));
		}

		return (InitAmbientShaderData(renderable, type, key));
	}
	else if (type <= kShaderLastLight)
	{
		return (InitLightShaderData(renderable, type, key));
	}

	return (InitPlainShaderData(renderable, type, key));
}

ShaderData *RenderSegment::GetShaderData(ShaderType type, const ShaderKey& key)
{
	ShaderData *firstShaderData = segmentShaderData[type];
	if (firstShaderData)
	{
		if (firstShaderData->shaderKey == key)
		{
			return (firstShaderData);
		}

		ShaderData *previous = firstShaderData;
		ShaderData *shaderData = firstShaderData->nextShaderData;
		while (shaderData)
		{
			ShaderData *next = shaderData->nextShaderData;

			if (shaderData->shaderKey == key)
			{
				previous->nextShaderData = next;
				if (next)
				{
					next->shaderDataPointer = &previous->nextShaderData;
				}

				segmentShaderData[type] = shaderData;
				shaderData->shaderDataPointer = &segmentShaderData[type];

				shaderData->nextShaderData = firstShaderData;
				firstShaderData->shaderDataPointer = &shaderData->nextShaderData;

				return (shaderData);
			}

			previous = shaderData;
			shaderData = next;
		}
	}

	return (nullptr);
}

void RenderSegment::InvalidateVertexData(void)
{
	for (machine type = 0; type < kShaderTypeCount; type++)
	{
		ShaderData *shaderData = segmentShaderData[type];
		while (shaderData)
		{
			shaderData->vertexData.Invalidate();
			shaderData = shaderData->nextShaderData;
		}
	}
}

void RenderSegment::InvalidateShaderData(void)
{
	for (machine type = 0; type < kShaderTypeCount; type++)
	{
		ShaderData *shaderData = segmentShaderData[type];
		while (shaderData)
		{
			ShaderData *next = shaderData->nextShaderData;
			delete shaderData;
			shaderData = next;
		}
	}
}

void RenderSegment::InvalidateAmbientShaderData(void)
{
	for (machine type = kShaderFirstAmbient; type <= kShaderLastUnified; type++)
	{
		ShaderData *shaderData = segmentShaderData[type];
		while (shaderData)
		{
			ShaderData *next = shaderData->nextShaderData;
			delete shaderData;
			shaderData = next;
		}
	}
}


Renderable::Renderable(RenderType type, unsigned_int32 state) : renderSegment(this)
{
	renderType = type;
	renderState = state;
	shaderFlags = 0;
	renderableFlags = 0;
	geometryShaderIndex = -1;

	ambientBlendState = kBlendReplace;
	lightBlendState = kBlendAccumulate;

	transformable = nullptr;
	previousWorldTransform = nullptr;

	paintEnvironment = &nullPaintEnvironment;
	ambientEnvironment = &nullAmbientEnvironment;

	transparentAttachment = nullptr;
	transparentPosition = nullptr;

	for (machine a = 0; a < kVertexBufferCount; a++)
	{
		vertexBuffer[a] = nullptr;
	}

	for (machine a = 0; a < kMaxAttributeArrayCount; a++)
	{
		componentCount[a] = 0;
	}

	vertexBufferArrayFlags = 0;
	primitiveIndexOffset = 0;

	renderParameter = &nullRenderParameterTable[0];
	texcoordParameter = &nullTexcoordParameterTable[0];
	terrainParameter = &nullTerrainParameterTable[0];

	occlusionQuery = nullptr;
	wireColor = nullptr;

	shaderDetailLevel = 0;
	shaderDetailParameter = 1.0F;
}

Renderable::~Renderable()
{
}

int32 Renderable::SetShaderArray(ShaderData *data, int32 shaderIndex, int32 renderIndex) const
{
	int32 count = componentCount[renderIndex];
	if (count != 0)
	{
		data->vertexData.SetArrayIndex(shaderIndex, renderIndex);

		unsigned_int32 bufferIndex = (vertexBufferArrayFlags >> renderIndex) & 1;
		data->vertexData.SetVertexAttribArray(shaderIndex, vertexBuffer[bufferIndex], attributeOffset[renderIndex], count, vertexBufferStride[bufferIndex]);
	}

	return (count);
}

unsigned_int32 Renderable::BuildVertexTransform(ShaderData *data, VertexAssembly *assembly) const
{
	SetShaderArray(data, kVertexAttribPosition0, kArrayPosition0);
	SetShaderArray(data, kVertexAttribPosition1, kArrayPosition1);

	unsigned_int32 stateFlags = 0;

	if (shaderFlags & kShaderVertexBillboard)
	{
		stateFlags |= kShaderStateCameraDirections;

		int32 count = SetShaderArray(data, kVertexAttribOffset, kArrayBillboard);
		if (count != 0)
		{
			if (shaderFlags & kShaderVertexInfinite)
			{
				assembly->AddSnippet(&VertexShader::calculateBillboardPosition);
				assembly->AddSnippet(&VertexShader::modelviewProjectTransformInfinite);
			}
			else
			{
				if (shaderFlags & kShaderScaleVertex)
				{
					stateFlags |= kShaderStateVertexScaleOffset;
					assembly->AddSnippet(&VertexShader::calculateBillboardScalePosition);
				}
				else if (count == 2)
				{
					assembly->AddSnippet(&VertexShader::calculateBillboardPosition);
				}
				else
				{
					assembly->AddSnippet(&VertexShader::calculateLightedBillboardPosition);
				}

				assembly->AddSnippet(&VertexShader::modelviewProjectTransformHomogeneous);
			}
		}
		else
		{
			if (shaderFlags & kShaderVertexInfinite)
			{
				assembly->AddSnippet(&VertexShader::calculateVertexBillboardPosition);
				assembly->AddSnippet(&VertexShader::modelviewProjectTransformInfinite);
			}
			else
			{
				if (shaderFlags & kShaderScaleVertex)
				{
					stateFlags |= kShaderStateVertexScaleOffset;
					assembly->AddSnippet(&VertexShader::calculateVertexBillboardScalePosition);
				}
				else
				{
					assembly->AddSnippet(&VertexShader::calculateVertexBillboardPosition);
				}

				assembly->AddSnippet(&VertexShader::modelviewProjectTransformHomogeneous);
			}
		}
	}
	else if (shaderFlags & kShaderVertexPostboard)
	{
		stateFlags |= kShaderStateCameraPosition4D;
		SetShaderArray(data, kVertexAttribRadius, kArrayRadius);

		if (shaderFlags & kShaderScaleVertex)
		{
			stateFlags |= kShaderStateVertexScaleOffset;
			assembly->AddSnippet(&VertexShader::calculatePostboardScalePosition);
		}
		else
		{
			assembly->AddSnippet(&VertexShader::calculatePostboardPosition);
		}

		assembly->AddSnippet(&VertexShader::modelviewProjectTransformHomogeneous);
	}
	else if (shaderFlags & kShaderVertexPolyboard)
	{
		SetShaderArray(data, kVertexAttribTangent, kArrayTangent);

		if (shaderFlags & kShaderOrthoPolyboard)
		{
			stateFlags |= kShaderStateCameraPosition4D;

			if (shaderFlags & kShaderScaleVertex)
			{
				stateFlags |= kShaderStateVertexScaleOffset;
				assembly->AddSnippet(&VertexShader::scaleVertexCalculateCameraDirection4D);

				if (shaderFlags & kShaderLinearPolyboard)
				{
					assembly->AddSnippet(&VertexShader::calculateLinearPolyboardNormal);
				}
				else
				{
					assembly->AddSnippet(&VertexShader::calculatePolyboardNormal);
				}

				assembly->AddSnippet(&VertexShader::calculatePolyboardScalePosition);
			}
			else
			{
				assembly->AddSnippet(&VertexShader::calculateCameraDirection4D);

				if (shaderFlags & kShaderLinearPolyboard)
				{
					assembly->AddSnippet(&VertexShader::calculateLinearPolyboardNormal);
				}
				else
				{
					assembly->AddSnippet(&VertexShader::calculatePolyboardNormal);
				}

				assembly->AddSnippet(&VertexShader::calculatePolyboardPosition);
			}

			assembly->AddSnippet(&VertexShader::modelviewProjectTransformHomogeneous);
		}
		else
		{
			stateFlags |= kShaderStateCameraPosition;
			assembly->AddSnippet(&VertexShader::calculateCameraDirection);

			if (shaderFlags & kShaderLinearPolyboard)
			{
				assembly->AddSnippet(&VertexShader::calculateLinearPolyboardNormal);
			}
			else
			{
				assembly->AddSnippet(&VertexShader::calculatePolyboardNormal);
			}

			assembly->AddSnippet(&VertexShader::calculatePolyboardPosition);

			if (shaderFlags & kShaderVertexInfinite)
			{
				assembly->AddSnippet(&VertexShader::modelviewProjectTransformInfinite);
			}
			else
			{
				assembly->AddSnippet(&VertexShader::modelviewProjectTransformHomogeneous);
			}
		}
	}
	else if (shaderFlags & kShaderScaleVertex)
	{
		stateFlags |= kShaderStateVertexScaleOffset;

		if (shaderFlags & kShaderOffsetVertex)
		{
			SetShaderArray(data, kVertexAttribOffset, kArrayOffset);
			assembly->AddSnippet(&VertexShader::calculateScaleOffsetPosition);
		}
		else
		{
			assembly->AddSnippet(&VertexShader::calculateScalePosition);
		}

		assembly->AddSnippet(&VertexShader::modelviewProjectTransformHomogeneous);
	}
	else if (shaderFlags & kShaderNormalExpandVertex)
	{
		stateFlags |= kShaderStateVertexScaleOffset;
		assembly->AddSnippet(&VertexShader::calculateExpandNormalPosition);
		assembly->AddSnippet(&VertexShader::modelviewProjectTransformHomogeneous);
	}
	else if (shaderFlags & kShaderTerrainBorder)
	{
		stateFlags |= kShaderStateTerrainBorder;
		SetShaderArray(data, kVertexAttribColor2, kArrayColor2);

		assembly->AddSnippet(&VertexShader::calculateTerrainBorderPosition);
		assembly->AddSnippet(&VertexShader::modelviewProjectTransformHomogeneous);
	}
	else if (shaderFlags & kShaderWaterElevation)
	{
		assembly->AddSnippet(&VertexShader::calculateWaterHeightPosition);
		assembly->AddSnippet(&VertexShader::modelviewProjectTransformHomogeneous);
	}
	else if (shaderFlags & kShaderVertexInfinite)
	{
		assembly->AddSnippet(&VertexShader::modelviewProjectTransformInfinite);
	}
	else
	{
		assembly->AddSnippet(&VertexShader::modelviewProjectTransform);
	}

	const VertexBuffer *buffer = vertexBuffer[kVertexBufferIndexArray];
	if ((buffer) && (buffer->Active()))
	{
		data->vertexData.SetVertexIndexBuffer(buffer);
	}

	#if C4OPENGL

		else if (renderType == kRenderQuads)
		{
			data->vertexData.SetVertexIndexBuffer(Render::GetQuadIndexBuffer());
		}

	#endif

	return (stateFlags);
}

unsigned_int32 Renderable::BuildTexcoord0Transform(const RenderSegment *segment, ShaderData *data, VertexAssembly *assembly, unsigned_int32 stateFlags) const
{
	static const VertexSnippet *snippetTable[8] =
	{
		&VertexShader::copyPrimaryTexcoord0, &VertexShader::transformPrimaryTexcoord0,
		&VertexShader::animatePrimaryTexcoord0, &VertexShader::transformAnimatePrimaryTexcoord0,
		&VertexShader::generateTexcoord0, &VertexShader::generateTransformTexcoord0,
		&VertexShader::generateAnimateTexcoord0, &VertexShader::generateTransformAnimateTexcoord0
	};

	SetShaderArray(data, kVertexAttribTexture0, kArrayTexcoord0);
	unsigned_int32 snippetIndex = 0;

	const MaterialObject *const *materialPointer = segment->GetMaterialObjectPointer();
	if (materialPointer)
	{
		const MaterialObject *materialObject = *materialPointer;
		if (materialObject)
		{
			const Vector2D& scale = materialObject->GetTexcoordScale(0);
			const Vector2D& offset = materialObject->GetTexcoordOffset(0);
			if ((scale.x != 1.0F) || (scale.y != 1.0F) || (offset.x != 0.0F) || (offset.y != 0.0F))
			{
				snippetIndex = 1;
				stateFlags |= kShaderStateTexcoordTransform0;
			}

			if (materialObject->GetMaterialFlags() & kMaterialAnimateTexcoord0)
			{
				snippetIndex |= 2;
				stateFlags |= kShaderStateTexcoordVelocity0;
			}
		}
	}

	if (shaderFlags & kShaderGenerateTexcoord)
	{
		snippetIndex |= 4;
		stateFlags |= kShaderStateTexcoordGenerate;

		if ((snippetIndex == 6) && (!(stateFlags & kShaderStateBaseTexcoord)))
		{
			stateFlags |= kShaderStateBaseTexcoord;
			assembly->AddSnippet(&VertexShader::generateBaseTexcoord);
		}
	}

	assembly->AddSnippet(snippetTable[snippetIndex]);
	return (stateFlags);
}

unsigned_int32 Renderable::BuildTexcoord1Transform(const RenderSegment *segment, ShaderData *data, VertexAssembly *assembly, unsigned_int32 stateFlags) const
{
	static const VertexSnippet *snippetTable[12] =
	{
		&VertexShader::copyPrimaryTexcoord1, &VertexShader::transformPrimaryTexcoord1,
		&VertexShader::animatePrimaryTexcoord1, &VertexShader::transformAnimatePrimaryTexcoord1,
		&VertexShader::generateTexcoord1, &VertexShader::generateTransformTexcoord1,
		&VertexShader::generateAnimateTexcoord1, &VertexShader::generateTransformAnimateTexcoord1,
		&VertexShader::copySecondaryTexcoord1, &VertexShader::transformSecondaryTexcoord1,
		&VertexShader::animateSecondaryTexcoord1, &VertexShader::transformAnimateSecondaryTexcoord1
	};

	SetShaderArray(data, kVertexAttribTexture0, kArrayTexcoord0);
	unsigned_int32 snippetIndex = (SetShaderArray(data, kVertexAttribTexture1, kArrayTexcoord1) != 0) ? 8 : 0;

	const MaterialObject *const *materialPointer = segment->GetMaterialObjectPointer();
	if (materialPointer)
	{
		const MaterialObject *materialObject = *materialPointer;
		if (materialObject)
		{
			const Vector2D& scale = materialObject->GetTexcoordScale(1);
			const Vector2D& offset = materialObject->GetTexcoordOffset(1);
			if ((scale.x != 1.0F) || (scale.y != 1.0F) || (offset.x != 0.0F) || (offset.y != 0.0F))
			{
				snippetIndex |= 1;
				stateFlags |= kShaderStateTexcoordTransform1;
			}

			if (materialObject->GetMaterialFlags() & kMaterialAnimateTexcoord1)
			{
				snippetIndex |= 2;
				stateFlags |= kShaderStateTexcoordVelocity1;
			}
		}
	}

	if ((shaderFlags & kShaderGenerateTexcoord) && (snippetIndex < 8))
	{
		snippetIndex |= 4;
		stateFlags |= kShaderStateTexcoordGenerate;

		if ((snippetIndex == 6) && (!(stateFlags & kShaderStateBaseTexcoord)))
		{
			stateFlags |= kShaderStateBaseTexcoord;
			assembly->AddSnippet(&VertexShader::generateBaseTexcoord);
		}
	}

	assembly->AddSnippet(snippetTable[snippetIndex]);
	return (stateFlags);
}

void Renderable::StateProc_CopyCameraPosition(const Renderable *renderable, const void *cookie)
{
	Render::SetVertexShaderParameter(kVertexParamCameraPosition, TheGraphicsMgr->GetCameraTransformable()->GetWorldPosition());
}

void Renderable::StateProc_CopyCameraDirections(const Renderable *renderable, const void *cookie)
{
	const Transform4D& cameraTransform = TheGraphicsMgr->GetCameraTransformable()->GetWorldTransform();

	Render::SetVertexShaderParameter(kVertexParamCameraRight, cameraTransform[0]);
	Render::SetVertexShaderParameter(kVertexParamCameraDown, cameraTransform[1]);
}

void Renderable::StateProc_CopyCameraPositionAndDirections(const Renderable *renderable, const void *cookie)
{
	const Transform4D& cameraTransform = TheGraphicsMgr->GetCameraTransformable()->GetWorldTransform();

	Render::SetVertexShaderParameter(kVertexParamCameraPosition, cameraTransform.GetTranslation());
	Render::SetVertexShaderParameter(kVertexParamCameraRight, cameraTransform[0]);
	Render::SetVertexShaderParameter(kVertexParamCameraDown, cameraTransform[1]);
}

void Renderable::StateProc_TransformCameraPosition(const Renderable *renderable, const void *cookie)
{
	const Transformable *geometryTransformable = renderable->GetTransformable();

	Point3D position = geometryTransformable->GetInverseWorldTransform() * TheGraphicsMgr->GetCameraTransformable()->GetWorldPosition();
	Render::SetVertexShaderParameter(kVertexParamCameraPosition, position);
}

void Renderable::StateProc_TransformCameraDirections(const Renderable *renderable, const void *cookie)
{
	const Transformable *geometryTransformable = renderable->GetTransformable();
	const Transform4D& cameraTransform = TheGraphicsMgr->GetCameraTransformable()->GetWorldTransform();
	const Transform4D& inverseTransform = geometryTransformable->GetInverseWorldTransform();

	Render::SetVertexShaderParameter(kVertexParamCameraRight, inverseTransform * cameraTransform[0]);
	Render::SetVertexShaderParameter(kVertexParamCameraDown, inverseTransform * cameraTransform[1]);
}

void Renderable::StateProc_TransformCameraPositionAndDirections(const Renderable *renderable, const void *cookie)
{
	const Transformable *geometryTransformable = renderable->GetTransformable();
	const Transform4D& cameraTransform = TheGraphicsMgr->GetCameraTransformable()->GetWorldTransform();

	Point3D position = geometryTransformable->GetInverseWorldTransform() * cameraTransform.GetTranslation();
	Render::SetVertexShaderParameter(kVertexParamCameraPosition, position);

	const Transform4D& inverseTransform = geometryTransformable->GetInverseWorldTransform();
	Render::SetVertexShaderParameter(kVertexParamCameraRight, inverseTransform * cameraTransform[0]);
	Render::SetVertexShaderParameter(kVertexParamCameraDown, inverseTransform * cameraTransform[1]);
}

void Renderable::StateProc_CopyCameraPosition4D(const Renderable *renderable, const void *cookie)
{
	Render::SetVertexShaderParameter(kVertexParamCameraPosition, &TheGraphicsMgr->GetCameraPosition4D().x);
}

void Renderable::StateProc_TransformCameraPosition4D(const Renderable *renderable, const void *cookie)
{
	Vector4D position = renderable->GetTransformable()->GetInverseWorldTransform() * TheGraphicsMgr->GetCameraPosition4D();
	Render::SetVertexShaderParameter(kVertexParamCameraPosition, position);
}

void Renderable::StateProc_CopyCameraMatrix(const Renderable *renderable, const void *cookie)
{
	Render::SetVertexShaderParameter(kVertexParamMatrixCamera, TheGraphicsMgr->GetCameraTransformable()->GetInverseWorldTransform());
}

void Renderable::StateProc_TransformCameraMatrix(const Renderable *renderable, const void *cookie)
{
	const Transformable *geometryTransformable = renderable->GetTransformable();
	const Transform4D& cameraTransform = TheGraphicsMgr->GetCameraTransformable()->GetInverseWorldTransform();

	Render::SetVertexShaderParameter(kVertexParamMatrixCamera, cameraTransform * geometryTransformable->GetWorldTransform());
}

void Renderable::StateProc_CopyWorldMatrix(const Renderable *renderable, const void *cookie)
{
	Render::SetVertexShaderParameter(kVertexParamMatrixWorld, Identity4D);
}

void Renderable::StateProc_TransformWorldMatrix(const Renderable *renderable, const void *cookie)
{
	Render::SetVertexShaderParameter(kVertexParamMatrixWorld, renderable->GetTransformable()->GetWorldTransform());
}

void Renderable::StateProc_TransformTexcoord0(const Renderable *renderable, const void *cookie)
{
	const MaterialObject *object = static_cast<const MaterialObject *>(cookie);

	const Vector2D& scale = object->GetTexcoordScale(0);
	const Vector2D& offset = object->GetTexcoordOffset(0);
	Render::SetVertexShaderParameter(kVertexParamTexcoordTransform0, scale.x, scale.y, offset.x, offset.y);
}

void Renderable::StateProc_AnimateTexcoord0(const Renderable *renderable, const void *cookie)
{
	const MaterialObject *object = static_cast<const MaterialObject *>(cookie);

	const Vector2D& velocity = object->GetTexcoordVelocity(0);
	Render::SetVertexShaderParameter(kVertexParamTexcoordVelocity0, velocity.x, velocity.y, 0.0F, 0.0F);
}

void Renderable::StateProc_TransformAnimateTexcoord0(const Renderable *renderable, const void *cookie)
{
	const MaterialObject *object = static_cast<const MaterialObject *>(cookie);

	const Vector2D& scale = object->GetTexcoordScale(0);
	const Vector2D& offset = object->GetTexcoordOffset(0);
	Render::SetVertexShaderParameter(kVertexParamTexcoordTransform0, scale.x, scale.y, offset.x, offset.y);

	const Vector2D& velocity = object->GetTexcoordVelocity(0);
	Render::SetVertexShaderParameter(kVertexParamTexcoordVelocity0, velocity.x, velocity.y, 0.0F, 0.0F);
}

void Renderable::StateProc_TransformTexcoord1(const Renderable *renderable, const void *cookie)
{
	const MaterialObject *object = static_cast<const MaterialObject *>(cookie);

	const Vector2D& scale = object->GetTexcoordScale(1);
	const Vector2D& offset = object->GetTexcoordOffset(1);
	Render::SetVertexShaderParameter(kVertexParamTexcoordTransform1, scale.x, scale.y, offset.x, offset.y);
}

void Renderable::StateProc_AnimateTexcoord1(const Renderable *renderable, const void *cookie)
{
	const MaterialObject *object = static_cast<const MaterialObject *>(cookie);

	const Vector2D& velocity = object->GetTexcoordVelocity(1);
	Render::SetVertexShaderParameter(kVertexParamTexcoordVelocity1, velocity.x, velocity.y, 0.0F, 0.0F);
}

void Renderable::StateProc_TransformAnimateTexcoord1(const Renderable *renderable, const void *cookie)
{
	const MaterialObject *object = static_cast<const MaterialObject *>(cookie);

	const Vector2D& scale = object->GetTexcoordScale(1);
	const Vector2D& offset = object->GetTexcoordOffset(1);
	Render::SetVertexShaderParameter(kVertexParamTexcoordTransform1, scale.x, scale.y, offset.x, offset.y);

	const Vector2D& velocity = object->GetTexcoordVelocity(1);
	Render::SetVertexShaderParameter(kVertexParamTexcoordVelocity1, velocity.x, velocity.y, 0.0F, 0.0F);
}

void Renderable::StateProc_ScaleTerrainTexcoord(const Renderable *renderable, const void *cookie)
{
	const MaterialObject *object = static_cast<const MaterialObject *>(cookie);
	Render::SetVertexShaderParameter(kVertexParamTerrainTexcoordScale, object->GetTexcoordGeneration().x, 0.0F, 0.0F, 0.0F);
}

void Renderable::StateProc_GenerateTexcoord(const Renderable *renderable, const void *cookie)
{
	Render::SetVertexShaderParameter(kVertexParamTexcoordGenerate, *renderable->GetTexcoordParameterPointer());
}

void Renderable::StateProc_GenerateTransformTexcoord0(const Renderable *renderable, const void *cookie)
{
	const MaterialObject *object = static_cast<const MaterialObject *>(cookie);

	const Vector2D& scale = object->GetTexcoordScale(0);
	const Vector2D& offset = object->GetTexcoordOffset(0);
	const Vector4D *param = renderable->GetTexcoordParameterPointer();
	Render::SetVertexShaderParameter(kVertexParamTexcoordTransform0, param->x * scale.x, param->y * scale.y, offset.x, offset.y);
}

void Renderable::StateProc_GenerateAnimateTexcoord0(const Renderable *renderable, const void *cookie)
{
	const MaterialObject *object = static_cast<const MaterialObject *>(cookie);

	const Vector2D& velocity = object->GetTexcoordVelocity(0);
	Render::SetVertexShaderParameter(kVertexParamTexcoordVelocity0, velocity.x, velocity.y, 0.0F, 0.0F);
	Render::SetVertexShaderParameter(kVertexParamTexcoordGenerate, *renderable->GetTexcoordParameterPointer());
}

void Renderable::StateProc_GenerateTransformAnimateTexcoord0(const Renderable *renderable, const void *cookie)
{
	const MaterialObject *object = static_cast<const MaterialObject *>(cookie);

	const Vector2D& scale = object->GetTexcoordScale(0);
	const Vector2D& offset = object->GetTexcoordOffset(0);
	const Vector4D *param = renderable->GetTexcoordParameterPointer();
	Render::SetVertexShaderParameter(kVertexParamTexcoordTransform0, param->x * scale.x, param->y * scale.y, offset.x, offset.y);

	const Vector2D& velocity = object->GetTexcoordVelocity(0);
	Render::SetVertexShaderParameter(kVertexParamTexcoordVelocity0, velocity.x, velocity.y, 0.0F, 0.0F);
}

void Renderable::StateProc_GenerateTransformTexcoord1(const Renderable *renderable, const void *cookie)
{
	const MaterialObject *object = static_cast<const MaterialObject *>(cookie);

	const Vector2D& scale = object->GetTexcoordScale(1);
	const Vector2D& offset = object->GetTexcoordOffset(1);
	const Vector4D *param = renderable->GetTexcoordParameterPointer();
	Render::SetVertexShaderParameter(kVertexParamTexcoordTransform1, param->x * scale.x, param->y * scale.y, offset.x, offset.y);
}

void Renderable::StateProc_GenerateAnimateTexcoord1(const Renderable *renderable, const void *cookie)
{
	const MaterialObject *object = static_cast<const MaterialObject *>(cookie);

	const Vector2D& velocity = object->GetTexcoordVelocity(1);
	Render::SetVertexShaderParameter(kVertexParamTexcoordVelocity1, velocity.x, velocity.y, 0.0F, 0.0F);
	Render::SetVertexShaderParameter(kVertexParamTexcoordGenerate, *renderable->GetTexcoordParameterPointer());
}

void Renderable::StateProc_GenerateTransformAnimateTexcoord1(const Renderable *renderable, const void *cookie)
{
	const MaterialObject *object = static_cast<const MaterialObject *>(cookie);

	const Vector2D& scale = object->GetTexcoordScale(1);
	const Vector2D& offset = object->GetTexcoordOffset(1);
	const Vector4D *param = renderable->GetTexcoordParameterPointer();
	Render::SetVertexShaderParameter(kVertexParamTexcoordTransform1, param->x * scale.x, param->y * scale.y, offset.x, offset.y);

	const Vector2D& velocity = object->GetTexcoordVelocity(1);
	Render::SetVertexShaderParameter(kVertexParamTexcoordVelocity1, velocity.x, velocity.y, 0.0F, 0.0F);
}

void Renderable::StateProc_GenerateAnimateDualTexcoords(const Renderable *renderable, const void *cookie)
{
	const MaterialObject *object = static_cast<const MaterialObject *>(cookie);

	const Vector2D& velocity0 = object->GetTexcoordVelocity(0);
	const Vector2D& velocity1 = object->GetTexcoordVelocity(1);
	Render::SetVertexShaderParameter(kVertexParamTexcoordVelocity0, velocity0.x, velocity0.y, 0.0F, 0.0F);
	Render::SetVertexShaderParameter(kVertexParamTexcoordVelocity1, velocity1.x, velocity1.y, 0.0F, 0.0F);
	Render::SetVertexShaderParameter(kVertexParamTexcoordGenerate, *renderable->GetTexcoordParameterPointer());
}

void Renderable::StateProc_ConfigureInfiniteLight(const Renderable *renderable, const void *cookie)
{
	Render::SetVertexShaderParameter(kVertexParamLightPosition, TheGraphicsMgr->GetLightTransformable()->GetWorldTransform()[2]);
}

void Renderable::StateProc_ConfigureTransformInfiniteLight(const Renderable *renderable, const void *cookie)
{
	Render::SetVertexShaderParameter(kVertexParamLightPosition, renderable->GetTransformable()->GetInverseWorldTransform() * TheGraphicsMgr->GetLightTransformable()->GetWorldTransform()[2]);
}

void Renderable::StateProc_ConfigureInfiniteLightShadow(const Renderable *renderable, const void *cookie)
{
	const Transformable *lightTransformable = TheGraphicsMgr->GetLightTransformable();
	Render::SetVertexShaderParameter(kVertexParamLightPosition, lightTransformable->GetWorldTransform()[2]);

	const Transform4D& m = lightTransformable->GetInverseWorldTransform();

	const LightShadowData *shadowData = TheGraphicsMgr->GetLightShadowData();
	float w = -shadowData->inverseShadowSize.x;
	float h = -shadowData->inverseShadowSize.y;
	float r = -shadowData->inverseShadowSize.z;

	#if C4PS4

		h = -h;

	#endif

	Render::SetVertexShaderParameter(kVertexParamMatrixShadow, m(0,0) * w, m(0,1) * w, m(0,2) * w, (m(0,3) - shadowData->shadowPosition.x) * w + 0.5F);
	Render::SetVertexShaderParameter(kVertexParamMatrixShadow + 1, m(1,0) * h, m(1,1) * h, m(1,2) * h, (m(1,3) - shadowData->shadowPosition.y) * h + 0.5F);
	Render::SetVertexShaderParameter(kVertexParamMatrixShadow + 2, m(2,0) * r, m(2,1) * r, m(2,2) * r, (m(2,3) - shadowData->shadowPosition.z) * r);

	Render::SetVertexShaderParameter(kVertexParamShadowCascadePlane1, shadowData[0].cascadePlane);
	Render::SetVertexShaderParameter(kVertexParamShadowCascadePlane2, shadowData[1].cascadePlane);
	Render::SetVertexShaderParameter(kVertexParamShadowCascadePlane3, shadowData[2].cascadePlane);
}

void Renderable::StateProc_ConfigureTransformInfiniteLightShadow(const Renderable *renderable, const void *cookie)
{
	const Transformable *lightTransformable = TheGraphicsMgr->GetLightTransformable();
	const Transformable *geometryTransformable = renderable->GetTransformable();
	Render::SetVertexShaderParameter(kVertexParamLightPosition, geometryTransformable->GetInverseWorldTransform() * lightTransformable->GetWorldTransform()[2]);

	Transform4D m = lightTransformable->GetInverseWorldTransform() * geometryTransformable->GetWorldTransform();

	const LightShadowData *shadowData = TheGraphicsMgr->GetLightShadowData();
	float w = -shadowData->inverseShadowSize.x;
	float h = -shadowData->inverseShadowSize.y;
	float r = -shadowData->inverseShadowSize.z;

	#if C4PS4

		h = -h;

	#endif

	Render::SetVertexShaderParameter(kVertexParamMatrixShadow, m(0,0) * w, m(0,1) * w, m(0,2) * w, (m(0,3) - shadowData->shadowPosition.x) * w + 0.5F);
	Render::SetVertexShaderParameter(kVertexParamMatrixShadow + 1, m(1,0) * h, m(1,1) * h, m(1,2) * h, (m(1,3) - shadowData->shadowPosition.y) * h + 0.5F);
	Render::SetVertexShaderParameter(kVertexParamMatrixShadow + 2, m(2,0) * r, m(2,1) * r, m(2,2) * r, (m(2,3) - shadowData->shadowPosition.z) * r);

	const Transform4D& geometryTransform = geometryTransformable->GetWorldTransform();
	Render::SetVertexShaderParameter(kVertexParamShadowCascadePlane1, shadowData[0].cascadePlane * geometryTransform);
	Render::SetVertexShaderParameter(kVertexParamShadowCascadePlane2, shadowData[1].cascadePlane * geometryTransform);
	Render::SetVertexShaderParameter(kVertexParamShadowCascadePlane3, shadowData[2].cascadePlane * geometryTransform);
}

void Renderable::StateProc_ConfigurePointLight(const Renderable *renderable, const void *cookie)
{
	Render::SetVertexShaderParameter(kVertexParamLightPosition, TheGraphicsMgr->GetLightTransformable()->GetWorldPosition());
}

void Renderable::StateProc_ConfigureTransformPointLight(const Renderable *renderable, const void *cookie)
{
	Render::SetVertexShaderParameter(kVertexParamLightPosition, renderable->GetTransformable()->GetInverseWorldTransform() * TheGraphicsMgr->GetLightTransformable()->GetWorldPosition());
}

void Renderable::StateProc_ConfigureCubeLight(const Renderable *renderable, const void *cookie)
{
	const Transformable *lightTransformable = TheGraphicsMgr->GetLightTransformable();
	Render::SetVertexShaderParameter(kVertexParamLightPosition, lightTransformable->GetWorldPosition());
	Render::SetVertexShaderParameter(kVertexParamMatrixLight, lightTransformable->GetInverseWorldTransform());
}

void Renderable::StateProc_ConfigureTransformCubeLight(const Renderable *renderable, const void *cookie)
{
	const Transformable *lightTransformable = TheGraphicsMgr->GetLightTransformable();
	const Transformable *geometryTransformable = renderable->GetTransformable();

	Render::SetVertexShaderParameter(kVertexParamLightPosition, geometryTransformable->GetInverseWorldTransform() * TheGraphicsMgr->GetLightTransformable()->GetWorldPosition());
	Render::SetVertexShaderParameter(kVertexParamMatrixLight, lightTransformable->GetInverseWorldTransform() * geometryTransformable->GetWorldTransform());
}

void Renderable::StateProc_ConfigureSpotLight(const Renderable *renderable, const void *cookie)
{
	const Transformable *lightTransformable = TheGraphicsMgr->GetLightTransformable();
	Render::SetVertexShaderParameter(kVertexParamLightPosition, lightTransformable->GetWorldPosition());

	const Transform4D& m = lightTransformable->GetInverseWorldTransform();
	Render::SetVertexShaderParameter(kVertexParamMatrixLight, m);

	const SpotLightObject *lightObject = static_cast<const SpotLightObject *>(TheGraphicsMgr->GetLightObject());
	float x = lightObject->GetApexTangent();
	float y = -x / lightObject->GetAspectRatio();

	Render::SetVertexShaderParameter(kVertexParamMatrixLightProjector, (x * m(0,0) + m(2,0)) * 0.5F, (x * m(0,1) + m(2,1)) * 0.5F, (x * m(0,2) + m(2,2)) * 0.5F, (x * m(0,3) + m(2,3)) * 0.5F);
	Render::SetVertexShaderParameter(kVertexParamMatrixLightProjector + 1, (y * m(1,0) + m(2,0)) * 0.5F, (y * m(1,1) + m(2,1)) * 0.5F, (y * m(1,2) + m(2,2)) * 0.5F, (y * m(1,3) + m(2,3)) * 0.5F);
}

void Renderable::StateProc_ConfigureTransformSpotLight(const Renderable *renderable, const void *cookie)
{
	const Transformable *lightTransformable = TheGraphicsMgr->GetLightTransformable();
	const Transformable *geometryTransformable = renderable->GetTransformable();

	Render::SetVertexShaderParameter(kVertexParamLightPosition, geometryTransformable->GetInverseWorldTransform() * TheGraphicsMgr->GetLightTransformable()->GetWorldPosition());

	Transform4D m = lightTransformable->GetInverseWorldTransform() * geometryTransformable->GetWorldTransform();
	Render::SetVertexShaderParameter(kVertexParamMatrixLight, m);

	const SpotLightObject *lightObject = static_cast<const SpotLightObject *>(TheGraphicsMgr->GetLightObject());
	float x = lightObject->GetApexTangent();
	float y = -x / lightObject->GetAspectRatio();

	Render::SetVertexShaderParameter(kVertexParamMatrixLightProjector, (x * m(0,0) + m(2,0)) * 0.5F, (x * m(0,1) + m(2,1)) * 0.5F, (x * m(0,2) + m(2,2)) * 0.5F, (x * m(0,3) + m(2,3)) * 0.5F);
	Render::SetVertexShaderParameter(kVertexParamMatrixLightProjector + 1, (y * m(1,0) + m(2,0)) * 0.5F, (y * m(1,1) + m(2,1)) * 0.5F, (y * m(1,2) + m(2,2)) * 0.5F, (y * m(1,3) + m(2,3)) * 0.5F);
}

void Renderable::StateProc_ConfigureSpotLightShadow(const Renderable *renderable, const void *cookie)
{
	const Transformable *lightTransformable = TheGraphicsMgr->GetLightTransformable();
	Render::SetVertexShaderParameter(kVertexParamLightPosition, lightTransformable->GetWorldPosition());

	const Transform4D& m = lightTransformable->GetInverseWorldTransform();
	Render::SetVertexShaderParameter(kVertexParamMatrixLight, m);

	const SpotLightObject *lightObject = static_cast<const SpotLightObject *>(TheGraphicsMgr->GetLightObject());
	float x = lightObject->GetApexTangent();
	float y = -x / lightObject->GetAspectRatio();

	Render::SetVertexShaderParameter(kVertexParamMatrixLightProjector, (x * m(0,0) + m(2,0)) * 0.5F, (x * m(0,1) + m(2,1)) * 0.5F, (x * m(0,2) + m(2,2)) * 0.5F, (x * m(0,3) + m(2,3)) * 0.5F);
	Render::SetVertexShaderParameter(kVertexParamMatrixLightProjector + 1, (y * m(1,0) + m(2,0)) * 0.5F, (y * m(1,1) + m(2,1)) * 0.5F, (y * m(1,2) + m(2,2)) * 0.5F, (y * m(1,3) + m(2,3)) * 0.5F);

	const Matrix4D& p = TheGraphicsMgr->GetShadowTransform();
	Render::SetVertexShaderParameter(kVertexParamMatrixShadow, p(0,0) * m(0,0) + p(0,2) * m(2,0), p(0,0) * m(0,1) + p(0,2) * m(2,1), p(0,0) * m(0,2) + p(0,2) * m(2,2), p(0,0) * m(0,3) + p(0,2) * m(2,3));
	Render::SetVertexShaderParameter(kVertexParamMatrixShadow + 1, p(1,1) * m(1,0) + p(1,2) * m(2,0), p(1,1) * m(1,1) + p(1,2) * m(2,1), p(1,1) * m(1,2) + p(1,2) * m(2,2), p(1,1) * m(1,3) + p(1,2) * m(2,3));
	Render::SetVertexShaderParameter(kVertexParamMatrixShadow + 2, p(2,2) * m(2,0) + p(2,3) * m(3,0), p(2,2) * m(2,1) + p(2,3) * m(3,1), p(2,2) * m(2,2) + p(2,3) * m(3,2), p(2,2) * m(2,3) + p(2,3) * m(3,3));
}

void Renderable::StateProc_ConfigureTransformSpotLightShadow(const Renderable *renderable, const void *cookie)
{
	const Transformable *lightTransformable = TheGraphicsMgr->GetLightTransformable();
	const Transformable *geometryTransformable = renderable->GetTransformable();

	Render::SetVertexShaderParameter(kVertexParamLightPosition, geometryTransformable->GetInverseWorldTransform() * TheGraphicsMgr->GetLightTransformable()->GetWorldPosition());

	Transform4D m = lightTransformable->GetInverseWorldTransform() * geometryTransformable->GetWorldTransform();
	Render::SetVertexShaderParameter(kVertexParamMatrixLight, m);

	const SpotLightObject *lightObject = static_cast<const SpotLightObject *>(TheGraphicsMgr->GetLightObject());
	float x = lightObject->GetApexTangent();
	float y = -x / lightObject->GetAspectRatio();

	Render::SetVertexShaderParameter(kVertexParamMatrixLightProjector, (x * m(0,0) + m(2,0)) * 0.5F, (x * m(0,1) + m(2,1)) * 0.5F, (x * m(0,2) + m(2,2)) * 0.5F, (x * m(0,3) + m(2,3)) * 0.5F);
	Render::SetVertexShaderParameter(kVertexParamMatrixLightProjector + 1, (y * m(1,0) + m(2,0)) * 0.5F, (y * m(1,1) + m(2,1)) * 0.5F, (y * m(1,2) + m(2,2)) * 0.5F, (y * m(1,3) + m(2,3)) * 0.5F);

	const Matrix4D& p = TheGraphicsMgr->GetShadowTransform();
	Render::SetVertexShaderParameter(kVertexParamMatrixShadow, p(0,0) * m(0,0) + p(0,2) * m(2,0), p(0,0) * m(0,1) + p(0,2) * m(2,1), p(0,0) * m(0,2) + p(0,2) * m(2,2), p(0,0) * m(0,3) + p(0,2) * m(2,3));
	Render::SetVertexShaderParameter(kVertexParamMatrixShadow + 1, p(1,1) * m(1,0) + p(1,2) * m(2,0), p(1,1) * m(1,1) + p(1,2) * m(2,1), p(1,1) * m(1,2) + p(1,2) * m(2,2), p(1,1) * m(1,3) + p(1,2) * m(2,3));
	Render::SetVertexShaderParameter(kVertexParamMatrixShadow + 2, p(2,2) * m(2,0) + p(2,3) * m(3,0), p(2,2) * m(2,1) + p(2,3) * m(3,1), p(2,2) * m(2,2) + p(2,3) * m(3,2), p(2,2) * m(2,3) + p(2,3) * m(3,3));
}

void Renderable::StateProc_CopyVertexScaleOffset(const Renderable *renderable, const void *cookie)
{
	Render::SetVertexShaderParameter(kVertexParamVertexScaleOffset, *renderable->GetRenderParameterPointer());
}

void Renderable::StateProc_CopyTerrainParameters(const Renderable *renderable, const void *cookie)
{
	const Vector4D *param = renderable->GetTerrainParameterPointer();
	Render::SetVertexShaderParameter(kVertexParamTerrainParameter0, param[0]);
	Render::SetVertexShaderParameter(kVertexParamTerrainParameter1, param[1]);
}

void Renderable::StateProc_CopyImpostorTransition(const Renderable *renderable, const void *cookie)
{
	Render::SetVertexShaderParameter(kVertexParamImpostorTransition, *renderable->GetRenderParameterPointer());
}

void Renderable::StateProc_CopyGeometryTransition(const Renderable *renderable, const void *cookie)
{
	const Vector4D *param = renderable->GetRenderParameterPointer();
	const Point3D& cameraPosition = TheGraphicsMgr->GetDirectCameraPosition();

	const Point2D& impostorPosition = param[0].GetPoint3D().GetPoint2D();
	Vector2D direction = impostorPosition - cameraPosition.GetPoint2D();
	float distance = SquaredMag(direction);
	float r = InverseSqrt(distance);
	distance *= r;

	float inverseDiameter = param[1].z;
	float inverseHeight = param[1].w;
	float dx = direction.x * inverseDiameter * r;
	float dy = direction.y * inverseDiameter * r;

	Render::SetVertexShaderParameter(kVertexParamImpostorPlaneS, dy, -dx, 0.0F, dx * impostorPosition.y - dy * impostorPosition.x + 0.5F);
	Render::SetVertexShaderParameter(kVertexParamImpostorPlaneT, 0.0F, 0.0F, inverseHeight, -param[0].w * inverseHeight);
	Render::SetFragmentShaderParameter(kFragmentParamImpostorDistance, distance * param[1].x + param[1].y, 0.0F, 0.0F, 0.0F);
}

void Renderable::StateProc_TransformGeometryTransition(const Renderable *renderable, const void *cookie)
{
	const Vector4D *param = renderable->GetRenderParameterPointer();
	const Point3D& cameraPosition = TheGraphicsMgr->GetDirectCameraPosition();

	const Point2D& impostorPosition = param[0].GetPoint3D().GetPoint2D();
	Vector2D direction = impostorPosition - cameraPosition.GetPoint2D();
	float distance = SquaredMag(direction);
	float r = InverseSqrt(distance);
	distance *= r;

	float inverseDiameter = param[1].z;
	float inverseHeight = param[1].w;
	float dx = direction.x * inverseDiameter * r;
	float dy = direction.y * inverseDiameter * r;

	const Transform4D& transform = renderable->GetTransformable()->GetWorldTransform();

	Render::SetVertexShaderParameter(kVertexParamImpostorPlaneS, dy * transform(0,0) - dx * transform(1,0), dy * transform(0,1) - dx * transform(1,1), dy * transform(0,2) - dx * transform(1,2), dy * transform(0,3) - dx * transform(1,3) + dx * impostorPosition.y - dy * impostorPosition.x + 0.5F);
	Render::SetVertexShaderParameter(kVertexParamImpostorPlaneT, inverseHeight * transform(2,0), inverseHeight * transform(2,1), inverseHeight * transform(2,2), inverseHeight * transform(2,3) - param[0].w * inverseHeight);
	Render::SetFragmentShaderParameter(kFragmentParamImpostorDistance, distance * param[1].x + param[1].y, 0.0F, 0.0F, 0.0F);
}

void Renderable::StateProc_CopyPaintSpace(const Renderable *renderable, const void *cookie)
{
	const Transform4D& paintTransform = renderable->paintEnvironment->paintTransform;

	Render::SetVertexShaderParameter(kVertexParamPaintPlaneS, paintTransform(0,0), paintTransform(0,1), paintTransform(0,2), paintTransform(0,3));
	Render::SetVertexShaderParameter(kVertexParamPaintPlaneT, paintTransform(1,0), paintTransform(1,1), paintTransform(1,2), paintTransform(1,3));
}

void Renderable::StateProc_TransformPaintSpace(const Renderable *renderable, const void *cookie)
{
	const Transform4D& worldTransform = renderable->GetTransformable()->GetWorldTransform();
	const Transform4D& paintTransform = renderable->paintEnvironment->paintTransform;

	const MatrixRow4D& x = paintTransform.GetRow(0);
	const MatrixRow4D& y = paintTransform.GetRow(1);

	Render::SetVertexShaderParameter(kVertexParamPaintPlaneS, x ^ worldTransform[0], x ^ worldTransform[1], x ^ worldTransform[2], x ^ worldTransform.GetTranslation());
	Render::SetVertexShaderParameter(kVertexParamPaintPlaneT, y ^ worldTransform[0], y ^ worldTransform[1], y ^ worldTransform[2], y ^ worldTransform.GetTranslation());
}

void Renderable::StateProc_SetOcclusionQuery(const Renderable *renderable, const void *cookie)
{
	TheGraphicsMgr->SetOcclusionQuery(renderable->occlusionQuery);
}

const float *Renderable::GetShaderParameterPointer(int32 slot) const
{
	return (nullptr);
}

void Renderable::InvalidateVertexData(void)
{
	RenderSegment *segment = &renderSegment;
	do
	{
		segment->InvalidateVertexData();
		segment = segment->GetNextRenderSegment();
	} while (segment);
}

void Renderable::InvalidateShaderData(void)
{
	RenderSegment *segment = &renderSegment;
	do
	{
		segment->InvalidateShaderData();
		segment = segment->GetNextRenderSegment();
	} while (segment);
}

void Renderable::InvalidateAmbientShaderData(void)
{
	RenderSegment *segment = &renderSegment;
	do
	{
		segment->InvalidateAmbientShaderData();
		segment = segment->GetNextRenderSegment();
	} while (segment);
}

// ZYUQURM
