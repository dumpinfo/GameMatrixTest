 

#include "C4Shaders.h"
#include "C4Graphics.h"


using namespace C4;


char ShaderAttribute::sourceStorage[kMaxShaderSourceSize];
unsigned_int32 ShaderAttribute::signatureStorage[kMaxShaderSignatureSize];


ShaderAttribute::ShaderAttribute() : Attribute(kAttributeShader)
{
}

ShaderAttribute::ShaderAttribute(const ShaderAttribute& shaderAttribute) : Attribute(shaderAttribute)
{
	CloneShader(&shaderAttribute.shaderGraph, &shaderGraph);
}

ShaderAttribute::~ShaderAttribute()
{
}

Attribute *ShaderAttribute::Replicate(void) const
{
	return (new ShaderAttribute(*this));
}

void ShaderAttribute::Pack(Packer& data, unsigned_int32 packFlags) const
{
	Attribute::Pack(data, packFlags);

	PackHandle handle = data.BeginChunk('SHDR');
	PackShader(&shaderGraph, data, packFlags);
	data.EndChunk(handle);

	data << TerminatorChunk;
}

void ShaderAttribute::Unpack(Unpacker& data, unsigned_int32 unpackFlags)
{
	Attribute::Unpack(data, unpackFlags);
	UnpackChunkList<ShaderAttribute>(data, unpackFlags);
}

bool ShaderAttribute::UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags)
{
	switch (chunkHeader->chunkType)
	{
		case 'SHDR':

			UnpackShader(&shaderGraph, data, unpackFlags);
			return (true);
	}

	return (false);
}

void *ShaderAttribute::BeginSettingsUnpack(void)
{
	shaderGraph.Purge();
	return (Attribute::BeginSettingsUnpack());
}

void ShaderAttribute::PackShader(const ShaderGraph *graph, Packer& data, unsigned_int32 packFlags)
{
	int32 processCount = 0;
	int32 routeCount = 0;

	const Process *process = graph->GetFirstElement();
	while (process)
	{
		if ((process->GetProcessType() != kProcessSection) || (packFlags & kPackEditor))
		{
			routeCount += process->GetIncomingEdgeCount();
			process->processIndex = processCount;
			processCount++;
		}

		process = process->GetNextElement();
	}

	data << processCount;
	data << routeCount;

	process = graph->GetFirstElement();
	while (process)
	{
		if ((process->GetProcessType() != kProcessSection) || (packFlags & kPackEditor))
		{
			PackHandle section = data.BeginSection();
			process->PackType(data);
			process->Pack(data, packFlags);
			data.EndSection(section);
		}

		process = process->GetNextElement();
	}

	process = graph->GetFirstElement(); 
	while (process)
	{
		if ((process->GetProcessType() != kProcessSection) || (packFlags & kPackEditor)) 
		{
			const Route *route = process->GetFirstIncomingEdge(); 
			while (route)
			{
				data << route->GetStartElement()->processIndex; 
				data << route->GetFinishElement()->processIndex;
 
				PackHandle section = data.BeginSection(); 
				route->Pack(data, packFlags);
				data.EndSection(section);

				route = route->GetNextIncomingEdge(); 
			}
		}

		process = process->GetNextElement();
	}
}

void ShaderAttribute::UnpackShader(ShaderGraph *graph, Unpacker& data, unsigned_int32 unpackFlags)
{
	int32		processCount;
	int32		routeCount;

	data >> processCount;
	data >> routeCount;

	Process **processTable = new Process *[processCount];

	for (machine a = 0; a < processCount; a++)
	{
		unsigned_int32	size;

		data >> size;
		const void *mark = data.GetPointer();

		if ((data.GetType() != kProcessSection) || (unpackFlags & kUnpackEditor))
		{
			Process *process = Process::Create(data);
			if (process)
			{
				process->Unpack(++data, unpackFlags);
				processTable[a] = process;
				graph->AddElement(process);
				continue;
			}
		}

		data.Skip(mark, size);
		processTable[a] = nullptr;
	}

	for (machine a = 0; a < routeCount; a++)
	{
		unsigned_int32	startIndex;
		unsigned_int32	finishIndex;
		unsigned_int32	size;

		data >> startIndex;
		data >> finishIndex;

		data >> size;
		const void *mark = data.GetPointer();

		Process *startNode = processTable[startIndex];
		Process *finishNode = processTable[finishIndex];
		if ((startNode) && (finishNode))
		{
			Route *route = new Route(startNode, finishNode, 0);
			route->Unpack(data, unpackFlags);
		}
		else
		{
			data.Skip(mark, size);
		}
	}

	delete[] processTable;
}

bool ShaderAttribute::operator ==(const Attribute& attribute) const
{
	if (attribute.GetAttributeType() == kAttributeShader)
	{
		const ShaderAttribute *shaderAttribute = static_cast<const ShaderAttribute *>(&attribute);

		int32 count = 0;
		const Process *process = shaderGraph.GetFirstElement();
		while (process)
		{
			process->processIndex = count++;
			process = process->GetNextElement();
		}

		int32 attributeCount = 0;
		const Process *attributeProcess = shaderAttribute->shaderGraph.GetFirstElement();
		while (attributeProcess)
		{
			attributeProcess->processIndex = attributeCount++;
			attributeProcess = attributeProcess->GetNextElement();
		}

		if (count != attributeCount)
		{
			return (false);
		}

		process = shaderGraph.GetFirstElement();
		attributeProcess = shaderAttribute->shaderGraph.GetFirstElement();
		while (process)
		{
			if (!(*process == *attributeProcess))
			{
				return (false);
			}

			process = process->GetNextElement();
			attributeProcess = attributeProcess->GetNextElement();
		}

		return (true);
	}

	return (false);
}

void ShaderAttribute::CloneShader(const ShaderGraph *sourceGraph, ShaderGraph *destinGraph, bool reference)
{
	const Process *process = sourceGraph->GetFirstElement();
	if (reference)
	{
		while (process)
		{
			Process *clone = process->Clone();
			process->cloneProcess = clone;
			destinGraph->AddElement(clone);
			clone->ReferenceStateParams(process);

			process = process->GetNextElement();
		}
	}
	else
	{
		while (process)
		{
			Process *clone = process->Clone();
			process->cloneProcess = clone;
			destinGraph->AddElement(clone);

			process = process->GetNextElement();
		}
	}

	process = sourceGraph->GetFirstElement();
	while (process)
	{
		Process *clone = process->cloneProcess;

		const Route *route = process->GetFirstIncomingEdge();
		while (route)
		{
			new Route(*route, route->GetStartElement()->cloneProcess, clone);
			route = route->GetNextIncomingEdge();
		}

		process = process->GetNextElement();
	}
}

ShaderResult ShaderAttribute::PrepareProcessPorts(const Process *process, const ShaderCompileData *compileData)
{
	int32 level = compileData->detailLevel;

	int32 portCount = process->GetPortCount();
	for (machine a = 0; a < portCount; a++)
	{
		Route *route = process->GetPortRoute(a);
		if (route)
		{
			if ((level > 0) && (route->GetRouteFlags() & kRouteHighDetail))
			{
				delete route;
			}
		}
		else if (!(process->GetPortFlags(a) & kProcessPortOptional))
		{
			return (kShaderIncomplete);
		}
	}

	return (kShaderOkay);
}

ShaderResult ShaderAttribute::PrepareAmbientShader(const ShaderCompileData *compileData, ShaderGraph *graph, List<Process> *terminalList)
{
	OutputProcess	*outputProcess[4];

	unsigned_int32 materialState = compileData->shaderData->materialState;
	unsigned_int32 targetDisableMask = TheGraphicsMgr->GetTargetDisableMask();

	Process *ambientProcess = nullptr;
	Process *alphaProcess = nullptr;
	Process *alphaTestProcess = nullptr;
	Process *glowProcess = nullptr;
	int32 outputProcessCount = 0;

	Process *process = graph->GetFirstElement();
	while (process)
	{
		ShaderResult result = PrepareProcessPorts(process, compileData);
		if (result != kShaderOkay)
		{
			return (result);
		}

		if (process->GetBaseProcessType() == kProcessOutput)
		{
			switch (process->GetProcessType())
			{
				case kProcessAmbientOutput:

					ambientProcess = process;
					break;

				case kProcessEmissionOutput:
				case kProcessEnvironmentOutput:
				case kProcessTerrainEnvironmentOutput:

					if (process->GetPortRoute(0))
					{
						outputProcess[outputProcessCount++] = static_cast<OutputProcess *>(process);
					}

					break;

				case kProcessReflectionOutput:

					if (process->GetPortRoute(0))
					{
						if ((targetDisableMask & (1 << kRenderTargetReflection)) == 0)
						{
							outputProcess[outputProcessCount++] = static_cast<OutputProcess *>(process);
						}
						else
						{
							process->PurgeIncomingEdges();
						}
					}

					break;

				case kProcessRefractionOutput:

					if (process->GetPortRoute(0))
					{
						if ((targetDisableMask & (1 << kRenderTargetRefraction)) == 0)
						{
							outputProcess[outputProcessCount++] = static_cast<OutputProcess *>(process);
						}
						else
						{
							process->PurgeIncomingEdges();
						}
					}

					break;

				case kProcessAlphaOutput:

					if (process->GetFirstIncomingEdge())
					{
						alphaProcess = process;
					}

					break;

				case kProcessAlphaTestOutput:

					if (process->GetFirstIncomingEdge())
					{
						if (materialState & kMaterialAlphaTest)
						{
							alphaTestProcess = process;
						}
						else
						{
							process->PurgeIncomingEdges();
						}
					}

					break;

				case kProcessGlowOutput:

					if (process->GetFirstIncomingEdge())
					{
						if (materialState & kMaterialEmissionGlow)
						{
							glowProcess = process;
						}
						else
						{
							process->PurgeIncomingEdges();
						}
					}

					break;
			}
		}

		process = process->GetNextElement();
	}

	if (glowProcess)
	{
		ShaderData *shaderData = compileData->shaderData;

		unsigned_int32 blendState = shaderData->blendState & kBlendColorMask;
		if (blendState == kBlendReplace)
		{
			blendState |= kBlendAlphaReplace;
		}
		else
		{
			blendState |= kBlendAlphaAccumulate;
		}

		shaderData->blendState = blendState;

		terminalList->Append(glowProcess);
	}
	else
	{
		compileData->shaderData->materialState &= ~kMaterialEmissionGlow;

		if (alphaTestProcess)
		{
			terminalList->Append(alphaTestProcess);
			if ((alphaProcess) && (!(materialState & kMaterialAlphaCoverage)))
			{
				terminalList->Append(alphaProcess);
			}
		}
		else if (alphaProcess)
		{
			terminalList->Append(alphaProcess);
		}
	}

	if ((materialState & kMaterialVertexOcclusion) && (compileData->renderable->AttributeArrayEnabled(kArrayColor)))
	{
		VertexOcclusionOutputProcess *vertexOcclusionProcess = new VertexOcclusionOutputProcess;
		graph->AddElement(vertexOcclusionProcess);

		new Route(ambientProcess, vertexOcclusionProcess, 0);
		ambientProcess = vertexOcclusionProcess;
	}

	Process *finalAmbientProcess = ambientProcess;

	if (outputProcessCount == 1)
	{
		finalAmbientProcess = new AddOutputProcess;
		graph->AddElement(finalAmbientProcess);

		new Route(ambientProcess, finalAmbientProcess, 0);
		new Route(outputProcess[0], finalAmbientProcess, 1);
	}
	else if (outputProcessCount == 2)
	{
		Process *addProcess = new AddOutputProcess;
		graph->AddElement(addProcess);

		new Route(outputProcess[0], addProcess, 0);
		new Route(outputProcess[1], addProcess, 1);

		finalAmbientProcess = new AddOutputProcess;
		graph->AddElement(finalAmbientProcess);

		new Route(ambientProcess, finalAmbientProcess, 0);
		new Route(addProcess, finalAmbientProcess, 1);
	}
	else if (outputProcessCount == 3)
	{
		Process *addProcess1 = new AddOutputProcess;
		graph->AddElement(addProcess1);

		new Route(outputProcess[0], addProcess1, 0);
		new Route(outputProcess[1], addProcess1, 1);

		Process *addProcess2 = new AddOutputProcess;
		graph->AddElement(addProcess2);

		new Route(outputProcess[2], addProcess2, 0);
		new Route(ambientProcess, addProcess2, 1);

		finalAmbientProcess = new AddOutputProcess;
		graph->AddElement(finalAmbientProcess);

		new Route(addProcess1, finalAmbientProcess, 0);
		new Route(addProcess2, finalAmbientProcess, 1);
	}
	else if (outputProcessCount == 4)
	{
		Process *addProcess1 = new AddOutputProcess;
		graph->AddElement(addProcess1);

		new Route(outputProcess[0], addProcess1, 0);
		new Route(outputProcess[1], addProcess1, 1);

		Process *addProcess2 = new AddOutputProcess;
		graph->AddElement(addProcess2);

		new Route(outputProcess[2], addProcess2, 0);
		new Route(outputProcess[3], addProcess2, 1);

		Process *addProcess3 = new AddOutputProcess;
		graph->AddElement(addProcess3);

		new Route(addProcess1, addProcess3, 0);
		new Route(addProcess2, addProcess3, 1);

		finalAmbientProcess = new AddOutputProcess;
		graph->AddElement(finalAmbientProcess);

		new Route(ambientProcess, finalAmbientProcess, 0);
		new Route(addProcess3, finalAmbientProcess, 1);
	}

	if (compileData->shaderVariant != kShaderVariantNormal)
	{
		Process		*fogProcess;
		Process		*blendProcess;

		if (compileData->shaderVariant == kShaderVariantConstantFog)
		{
			fogProcess = new ConstantFogProcess;
		}
		else
		{
			fogProcess = new LinearFogProcess;
		}

		graph->AddElement(fogProcess);

		const Renderable *renderable = compileData->renderable;

		if (renderable->GetShaderFlags() & kShaderAlphaFogFraction)
		{
			blendProcess = new AlphaFogProcess;
		}
		else if (GetBlendDest(renderable->GetAmbientBlendState()) != kBlendOne)
		{
			blendProcess = new AmbientFogProcess;
		}
		else
		{
			blendProcess = new LightFogProcess;
		}

		graph->AddElement(blendProcess);

		new Route(finalAmbientProcess, blendProcess, 0);
		new Route(fogProcess, blendProcess, 1);
		finalAmbientProcess = blendProcess;
	}

	terminalList->Append(finalAmbientProcess);
	return (kShaderOkay);
}

ShaderResult ShaderAttribute::PrepareUnifiedShader(const ShaderCompileData *compileData, ShaderGraph *graph, List<Process> *terminalList)
{
	OutputProcess	*outputProcess[4];

	unsigned_int32 materialState = compileData->shaderData->materialState;
	unsigned_int32 targetDisableMask = TheGraphicsMgr->GetTargetDisableMask();

	Process *ambientProcess = nullptr;
	Process *lightProcess = nullptr;
	Process *alphaProcess = nullptr;
	Process *alphaTestProcess = nullptr;
	Process *glowProcess = nullptr;
	Process *bloomProcess = nullptr;
	int32 outputProcessCount = 0;

	Process *process = graph->GetFirstElement();
	while (process)
	{
		ShaderResult result = PrepareProcessPorts(process, compileData);
		if (result != kShaderOkay)
		{
			return (result);
		}

		if (process->GetBaseProcessType() == kProcessOutput)
		{
			switch (process->GetProcessType())
			{
				case kProcessAmbientOutput:

					ambientProcess = process;
					break;

				case kProcessLightOutput:

					lightProcess = process;
					break;

				case kProcessEmissionOutput:
				case kProcessEnvironmentOutput:
				case kProcessTerrainEnvironmentOutput:

					if (process->GetPortRoute(0))
					{
						outputProcess[outputProcessCount++] = static_cast<OutputProcess *>(process);
					}

					break;

				case kProcessReflectionOutput:

					if (process->GetPortRoute(0))
					{
						if ((targetDisableMask & (1 << kRenderTargetReflection)) == 0)
						{
							outputProcess[outputProcessCount++] = static_cast<OutputProcess *>(process);
						}
						else
						{
							process->PurgeIncomingEdges();
						}
					}

					break;

				case kProcessRefractionOutput:

					if (process->GetPortRoute(0))
					{
						if ((targetDisableMask & (1 << kRenderTargetRefraction)) == 0)
						{
							outputProcess[outputProcessCount++] = static_cast<OutputProcess *>(process);
						}
						else
						{
							process->PurgeIncomingEdges();
						}
					}

					break;

				case kProcessAlphaOutput:

					if (process->GetFirstIncomingEdge())
					{
						alphaProcess = process;
					}

					break;

				case kProcessAlphaTestOutput:

					if (process->GetFirstIncomingEdge())
					{
						if (materialState & kMaterialAlphaTest)
						{
							alphaTestProcess = process;
						}
						else
						{
							process->PurgeIncomingEdges();
						}
					}

					break;

				case kProcessGlowOutput:

					if (process->GetFirstIncomingEdge())
					{
						if (materialState & kMaterialEmissionGlow)
						{
							glowProcess = process;
						}
						else
						{
							process->PurgeIncomingEdges();
						}
					}

					break;

				case kProcessBloomOutput:

					if (process->GetFirstIncomingEdge())
					{
						if (materialState & kMaterialSpecularBloom)
						{
							bloomProcess = process;
						}
						else
						{
							process->PurgeIncomingEdges();
						}
					}

					break;
			}
		}

		process = process->GetNextElement();
	}

	if ((materialState & kMaterialVertexOcclusion) && (compileData->renderable->AttributeArrayEnabled(kArrayColor)))
	{
		VertexOcclusionOutputProcess *vertexOcclusionProcess = new VertexOcclusionOutputProcess;
		graph->AddElement(vertexOcclusionProcess);

		new Route(ambientProcess, vertexOcclusionProcess, 0);
		ambientProcess = vertexOcclusionProcess;
	}

	Process *finalAmbientProcess = ambientProcess;

	if (outputProcessCount == 1)
	{
		finalAmbientProcess = new AddOutputProcess;
		graph->AddElement(finalAmbientProcess);

		new Route(ambientProcess, finalAmbientProcess, 0);
		new Route(outputProcess[0], finalAmbientProcess, 1);
	}
	else if (outputProcessCount == 2)
	{
		Process *addProcess = new AddOutputProcess;
		graph->AddElement(addProcess);

		new Route(outputProcess[0], addProcess, 0);
		new Route(outputProcess[1], addProcess, 1);

		finalAmbientProcess = new AddOutputProcess;
		graph->AddElement(finalAmbientProcess);

		new Route(ambientProcess, finalAmbientProcess, 0);
		new Route(addProcess, finalAmbientProcess, 1);
	}
	else if (outputProcessCount == 3)
	{
		Process *addProcess1 = new AddOutputProcess;
		graph->AddElement(addProcess1);

		new Route(outputProcess[0], addProcess1, 0);
		new Route(outputProcess[1], addProcess1, 1);

		Process *addProcess2 = new AddOutputProcess;
		graph->AddElement(addProcess2);

		new Route(outputProcess[2], addProcess2, 0);
		new Route(ambientProcess, addProcess2, 1);

		finalAmbientProcess = new AddOutputProcess;
		graph->AddElement(finalAmbientProcess);

		new Route(addProcess1, finalAmbientProcess, 0);
		new Route(addProcess2, finalAmbientProcess, 1);
	}
	else if (outputProcessCount == 4)
	{
		Process *addProcess1 = new AddOutputProcess;
		graph->AddElement(addProcess1);

		new Route(outputProcess[0], addProcess1, 0);
		new Route(outputProcess[1], addProcess1, 1);

		Process *addProcess2 = new AddOutputProcess;
		graph->AddElement(addProcess2);

		new Route(outputProcess[2], addProcess2, 0);
		new Route(outputProcess[3], addProcess2, 1);

		Process *addProcess3 = new AddOutputProcess;
		graph->AddElement(addProcess3);

		new Route(addProcess1, addProcess3, 0);
		new Route(addProcess2, addProcess3, 1);

		finalAmbientProcess = new AddOutputProcess;
		graph->AddElement(finalAmbientProcess);

		new Route(ambientProcess, finalAmbientProcess, 0);
		new Route(addProcess3, finalAmbientProcess, 1);
	}

	Process *illuminationProcess = nullptr;

	if (compileData->shadowFlag)
	{
		illuminationProcess = new InfiniteShadowProcess;
		graph->AddElement(illuminationProcess);

		new Route(illuminationProcess, lightProcess, 2);

		Route *route = lightProcess->GetPortRoute(1);
		if (route)
		{
			route->SetFinishElement(illuminationProcess);
			route->SetRoutePort(0);
		}
	}

	Process *finalUnifiedProcess = new AddOutputProcess;
	graph->AddElement(finalUnifiedProcess);

	new Route(finalAmbientProcess, finalUnifiedProcess, 0);
	new Route(lightProcess, finalUnifiedProcess, 1);

	if (compileData->shaderVariant != kShaderVariantNormal)
	{
		Process		*fogProcess;

		if (compileData->shaderVariant == kShaderVariantConstantFog)
		{
			fogProcess = new ConstantFogProcess;
		}
		else
		{
			fogProcess = new LinearFogProcess;
		}

		graph->AddElement(fogProcess);

		Process *blendProcess = new AmbientFogProcess;
		graph->AddElement(blendProcess);

		new Route(finalUnifiedProcess, blendProcess, 0);
		new Route(fogProcess, blendProcess, 1);
		finalUnifiedProcess = blendProcess;
	}

	if ((glowProcess) || (bloomProcess))
	{
		ShaderData *shaderData = compileData->shaderData;

		unsigned_int32 blendState = shaderData->blendState & kBlendColorMask;
		if (blendState == kBlendReplace)
		{
			blendState |= kBlendAlphaReplace;
		}
		else
		{
			blendState |= kBlendAlphaAccumulate;
		}

		shaderData->blendState = blendState;

		if (glowProcess)
		{
			if (bloomProcess)
			{
				GlowBloomOutputProcess *glowBloomProcess = new GlowBloomOutputProcess;
				graph->AddElement(glowBloomProcess);
				terminalList->Append(glowBloomProcess);

				glowProcess->GetFirstIncomingEdge()->SetFinishElement(glowBloomProcess);

				Route *route = bloomProcess->GetPortRoute(0);
				route->SetFinishElement(glowBloomProcess);
				route->SetRoutePort(1);

				if (illuminationProcess)
				{
					(new Route(illuminationProcess, glowBloomProcess, 2))->SetRouteSwizzle('aaaa');
				}
			}
			else
			{
				shaderData->materialState &= ~kMaterialSpecularBloom;
				terminalList->Append(glowProcess);
			}
		}
		else
		{
			shaderData->materialState &= ~kMaterialEmissionGlow;
			terminalList->Append(bloomProcess);

			if (illuminationProcess)
			{
				(new Route(illuminationProcess, bloomProcess, 1))->SetRouteSwizzle('aaaa');
			}
		}
	}
	else
	{
		compileData->shaderData->materialState &= ~(kMaterialEmissionGlow | kMaterialSpecularBloom);

		if (alphaTestProcess)
		{
			terminalList->Append(alphaTestProcess);
			if ((alphaProcess) && (!(materialState & kMaterialAlphaCoverage)))
			{
				terminalList->Append(alphaProcess);
			}
		}
		else if (alphaProcess)
		{
			terminalList->Append(alphaProcess);
		}
	}

	terminalList->Append(finalUnifiedProcess);
	return (kShaderOkay);
}

ShaderResult ShaderAttribute::PrepareLightShader(const ShaderCompileData *compileData, ShaderGraph *graph, List<Process> *terminalList)
{
	unsigned_int32 materialState = compileData->shaderData->materialState;

	Process *lightProcess = nullptr;
	Process *alphaProcess = nullptr;
	Process *alphaTestProcess = nullptr;
	Process *bloomProcess = nullptr;

	Process *process = graph->GetFirstElement();
	while (process)
	{
		ShaderResult result = PrepareProcessPorts(process, compileData);
		if (result != kShaderOkay)
		{
			return (result);
		}

		if (process->GetBaseProcessType() == kProcessOutput)
		{
			switch (process->GetProcessType())
			{
				case kProcessLightOutput:

					lightProcess = process;
					break;

				case kProcessAlphaOutput:

					if ((process->GetFirstIncomingEdge()) && (GetBlendSource(compileData->renderable->GetLightBlendState()) != kBlendOne))
					{
						alphaProcess = process;
					}

					break;

				case kProcessAlphaTestOutput:

					if (process->GetFirstIncomingEdge())
					{
						if (materialState & kMaterialAlphaTest)
						{
							alphaTestProcess = process;
						}
						else
						{
							process->PurgeIncomingEdges();
						}
					}

					break;

				case kProcessBloomOutput:

					if (process->GetFirstIncomingEdge())
					{
						if (materialState & kMaterialSpecularBloom)
						{
							bloomProcess = process;
						}
						else
						{
							process->PurgeIncomingEdges();
						}
					}

					break;
			}
		}

		process = process->GetNextElement();
	}

	Process *illuminationProcess = nullptr;

	ShaderType shaderType = LightOutputProcess::GetLightShaderType(compileData);
	if (shaderType == kShaderInfiniteLight)
	{
		if (compileData->shadowFlag)
		{
			illuminationProcess = new InfiniteShadowProcess;

			Route *route = lightProcess->GetPortRoute(1);
			if (route)
			{
				route->SetFinishElement(illuminationProcess);
				route->SetRoutePort(0);
			}
		}
	}
	else if ((shaderType == kShaderPointLight) || (shaderType == kShaderCubeLight))
	{
		Process		*pointProcess;

		if (shaderType == kShaderPointLight)
		{
			pointProcess = new PointIlluminationProcess;
		}
		else
		{
			pointProcess = new CubeIlluminationProcess;
		}

		if (compileData->shadowFlag)
		{
			graph->AddElement(pointProcess);

			Process *shadowProcess = new PointShadowProcess;
			graph->AddElement(shadowProcess);

			illuminationProcess = new MultiplyProcess;
			new Route(pointProcess, illuminationProcess, 0);
			new Route(shadowProcess, illuminationProcess, 1);
		}
		else
		{
			illuminationProcess = pointProcess;
		}
	}
	else if (shaderType == kShaderSpotLight)
	{
		Process *spotProcess = new SpotIlluminationProcess;

		if (compileData->shadowFlag)
		{
			graph->AddElement(spotProcess);

			Process *shadowProcess = new SpotShadowProcess;
			graph->AddElement(shadowProcess);

			illuminationProcess = new MultiplyProcess;
			new Route(spotProcess, illuminationProcess, 0);
			new Route(shadowProcess, illuminationProcess, 1);
		}
		else
		{
			illuminationProcess = spotProcess;
		}
	}

	if (illuminationProcess)
	{
		new Route(illuminationProcess, lightProcess, 2);
		graph->AddElement(illuminationProcess);
	}

	Process *finalLightProcess = lightProcess;

	if (compileData->shaderVariant != kShaderVariantNormal)
	{
		Process		*fogProcess;

		if (compileData->shaderVariant == kShaderVariantConstantFog)
		{
			fogProcess = new ConstantFogProcess;
		}
		else
		{
			fogProcess = new LinearFogProcess;
		}

		graph->AddElement(fogProcess);

		Process *blendProcess = new LightFogProcess;
		graph->AddElement(blendProcess);

		new Route(finalLightProcess, blendProcess, 0);
		new Route(fogProcess, blendProcess, 1);
		finalLightProcess = blendProcess;
	}

	if (bloomProcess)
	{
		ShaderData *shaderData = compileData->shaderData;
		shaderData->blendState = (shaderData->blendState & kBlendColorMask) | kBlendAlphaAccumulate;

		terminalList->Append(bloomProcess);

		if (illuminationProcess)
		{
			(new Route(illuminationProcess, bloomProcess, 1))->SetRouteSwizzle('aaaa');
		}
	}
	else
	{
		compileData->shaderData->materialState &= ~kMaterialSpecularBloom;

		if (alphaTestProcess)
		{
			terminalList->Append(alphaTestProcess);
			if ((alphaProcess) && (!(materialState & kMaterialAlphaCoverage)))
			{
				terminalList->Append(alphaProcess);
			}
		}
		else if (alphaProcess)
		{
			terminalList->Append(alphaProcess);
		}
	}

	terminalList->Append(finalLightProcess);
	return (kShaderOkay);
}

ShaderResult ShaderAttribute::PreparePlainShader(ShaderType shaderType, const ShaderCompileData *compileData, ShaderGraph *graph, List<Process> *terminalList)
{
	if (shaderType == kShaderShadow)
	{
		graph->AddElement(new NullOutputProcess);
	}
	else if (shaderType == kShaderStructure)
	{
		graph->AddElement(new StructureOutputProcess);
	}

	Process *alphaTestOutput = nullptr;
	Process *impostorDepthOutput = nullptr;

	Process *process = graph->GetFirstElement();
	while (process)
	{
		ShaderResult result = PrepareProcessPorts(process, compileData);
		if (result != kShaderOkay)
		{
			return (result);
		}

		if (process->GetBaseProcessType() == kProcessOutput)
		{
			switch (process->GetProcessType())
			{
				case kProcessNullOutput:
				case kProcessStructureOutput:

					terminalList->Append(process);
					break;

				case kProcessAlphaTestOutput:

					if (process->GetFirstIncomingEdge())
					{
						alphaTestOutput = process;
					}

					break;

				case kProcessImpostorDepthOutput:

					if (process->GetFirstIncomingEdge())
					{
						impostorDepthOutput = process;
					}

					break;
			}
		}

		process = process->GetNextElement();
	}

	if (impostorDepthOutput)
	{
		terminalList->Append(impostorDepthOutput);
	}
	else if (alphaTestOutput)
	{
		terminalList->Append(alphaTestOutput);
	}

	return (kShaderOkay);
}

Process *ShaderAttribute::FindDerivedInterpolant(ProcessType type, int32 count, Process *const *interpolant)
{
	for (machine a = 0; a < count; a++)
	{
		Process *process = *interpolant;
		if (process->GetProcessType() == type)
		{
			return (process);
		}

		interpolant++;
	}

	return (nullptr);
}

void ShaderAttribute::OrganizeDerivedInterpolants(const ShaderCompileData *compileData, ShaderGraph *graph)
{
	Process		*interpolantProcess[20];

	int32 interpolantCount = 0;

	Process *process = graph->GetFirstElement();
	while (process)
	{
		ProcessType		interpolantType[4];

		int32 count = process->GenerateDerivedInterpolantTypes(compileData, interpolantType);
		Assert(count <= 4, "ShaderAttribute::OrganizeDerivedInterpolants(), interpolant type array overflow\n");

		for (machine a = 0; a < count; a++)
		{
			ProcessType type = interpolantType[a];
			Process *interpolant = FindDerivedInterpolant(type, interpolantCount, interpolantProcess);
			if (!interpolant)
			{
				interpolant = Process::New(type);
				graph->AddElement(interpolant);

				Assert(interpolantCount < 20, "ShaderAttribute::OrganizeDerivedInterpolants(), interpolant process array overflow\n");
				interpolantProcess[interpolantCount++] = interpolant;

				if (type == kProcessTerrainViewDirection)
				{
					compileData->terrainViewDirection = interpolant;
				}
			}

			new Route(interpolant, process, kProcessPortHiddenDependency);
		}

		if (process->GetBaseProcessType() == kProcessDerived)
		{
			Process *interpolant = FindDerivedInterpolant(process->GetProcessType(), interpolantCount, interpolantProcess);
			if (interpolant)
			{
				Process *next = process->GetNextElement();

				if (interpolant != process)
				{
					for (;;)
					{
						Route *route = process->GetFirstOutgoingEdge();
						if (!route)
						{
							break;
						}

						route->SetStartElement(interpolant);
					}

					delete process;
				}

				process = next;
				continue;
			}

			Assert(interpolantCount < 20, "ShaderAttribute::OrganizeDerivedInterpolants(), interpolant process array overflow\n");
			interpolantProcess[interpolantCount++] = process;
		}

		process = process->GetNextElement();
	}

	process = graph->GetFirstElement();
	while (process)
	{
		process->Preschedule(compileData);
		process = process->GetNextElement();
	}
}

void ShaderAttribute::OptimizeTextureMaps(const ShaderGraph *graph)
{
	List<TextureMapProcess>		textureMapList;

	Process *process = graph->GetFirstElement();
	while (process)
	{
		if (process->GetBaseProcessType() == kProcessTextureMap)
		{
			if (process->GetPortCount() > 0)
			{
				const Route *inputRoute = process->GetPortRoute(0);
				if (inputRoute)
				{
					const Process *inputProcess = inputRoute->GetStartElement();
					TextureMapProcess *textureMapProcess = static_cast<TextureMapProcess *>(process);

					TextureMapProcess *previousTextureMap = textureMapList.First();
					while (previousTextureMap)
					{
						if (*textureMapProcess == *previousTextureMap)
						{
							const Route *previousRoute = previousTextureMap->GetPortRoute(0);
							if (*inputRoute == *previousRoute)
							{
								const Process *previousInputProcess = previousRoute->GetStartElement();
								if ((inputProcess == previousInputProcess) || ((inputProcess->GetBaseProcessType() == kProcessInterpolant) && (inputProcess->GetProcessType() == previousInputProcess->GetProcessType())))
								{
									Route *outputRoute = textureMapProcess->GetFirstOutgoingEdge();
									while (outputRoute)
									{
										Route *nextRoute = outputRoute->GetNextOutgoingEdge();
										outputRoute->SetStartElement(previousTextureMap);
										outputRoute = nextRoute;
									}

									goto next;
								}
							}
						}

						previousTextureMap = previousTextureMap->ListElement<TextureMapProcess>::Next();
					}

					textureMapList.Append(textureMapProcess);
				}
			}
			else
			{
				TextureMapProcess *textureMapProcess = static_cast<TextureMapProcess *>(process);

				TextureMapProcess *previousTextureMap = textureMapList.First();
				while (previousTextureMap)
				{
					if (*textureMapProcess == *previousTextureMap)
					{
						Route *outputRoute = textureMapProcess->GetFirstOutgoingEdge();
						while (outputRoute)
						{
							Route *nextRoute = outputRoute->GetNextOutgoingEdge();
							outputRoute->SetStartElement(previousTextureMap);
							outputRoute = nextRoute;
						}

						goto next;
					}

					previousTextureMap = previousTextureMap->ListElement<TextureMapProcess>::Next();
				}

				textureMapList.Append(textureMapProcess);
			}
		}

		next:
		process = process->GetNextElement();
	}

	textureMapList.RemoveAll();
}

void ShaderAttribute::EliminateDeadCode(const ShaderGraph *graph, List<Process> *terminalList)
{
	List<Process>	deadList;

	Process *process = graph->GetFirstElement();
	while (process)
	{
		if ((!process->GetFirstOutgoingEdge()) && (!terminalList->Member(process)))
		{
			if (process->GetBaseProcessType() == kProcessTerminal)
			{
				terminalList->Append(process);
			}
			else
			{
				deadList.Append(process);
			}
		}

		process = process->GetNextElement();
	}

	for (;;)
	{
		process = deadList.First();
		if (!process)
		{
			break;
		}

		Route *route = process->GetFirstIncomingEdge();
		while (route)
		{
			Route *next = route->GetNextIncomingEdge();

			Process *predecessor = route->GetStartElement();
			delete route;

			if (!predecessor->GetFirstOutgoingEdge())
			{
				deadList.Append(predecessor);
			}

			route = next;
		}

		delete process;
	}
}

void ShaderAttribute::CalculatePathLengths(const ShaderGraph *graph, List<Process> *processList, List<Process> *readyList)
{
	Process *process = graph->GetFirstElement();
	while (process)
	{
		process->readyCount = (int16) process->GetOutgoingEdgeCount();
		process = process->GetNextElement();
	}

	for (;;)
	{
		process = processList->First();
		if (!process)
		{
			break;
		}

		int32 pathLength = 0;

		#if C4PS3 //[ PS3

			// -- PS3 code hidden --

		#endif //]

		const Route *route = process->GetFirstOutgoingEdge();
		while (route)
		{
			const Process *successor = route->GetFinishElement();
			int32 len = successor->pathLength + 1;
			pathLength = Max(pathLength, len);

			#if C4PS3 //[ PS3

				// -- PS3 code hidden --

			#endif //]

			route = route->GetNextOutgoingEdge();
		}

		process->pathLength = (int16) pathLength;

		#if C4PS3 //[ PS3

			// -- PS3 code hidden --

		#endif //]

		route = process->GetFirstIncomingEdge();
		if (route)
		{
			int32 count = 0;
			do
			{
				Process *predecessor = route->GetStartElement();
				if (--predecessor->readyCount == 0)
				{
					processList->Append(predecessor);
				}

				count++;
				route = route->GetNextIncomingEdge();
			} while (route);

			process->readyCount = (int16) count;
			processList->Remove(process);
		}
		else
		{
			Process *ready = readyList->Last();
			while (ready)
			{
				if (ready->pathLength >= pathLength)
				{
					readyList->InsertAfter(process, ready);
					goto next;
				}

				ready = ready->Previous();
			}

			readyList->Prepend(process);
		}

		next:;
	}
}

int32 ShaderAttribute::ScheduleShader(const ShaderCompileData *compileData, List<Process> *readyList, List<Process> *scheduleList, unsigned_int32 *shaderSignature)
{
	int32 processCount = 0;
	unsigned_int32 *signature = &shaderSignature[1];

	for (;;)
	{
		Process *process = readyList->First();
		if (!process)
		{
			break;
		}

		scheduleList->Append(process);
		process->processIndex = processCount;
		processCount++;

		signature += process->GenerateProcessSignature(compileData, signature);

		const Route *route = process->GetFirstOutgoingEdge();
		while (route)
		{
			Process *successor = route->GetFinishElement();
			if (--successor->readyCount == 0)
			{
				int32 pathLength = successor->pathLength;

				Process *ready = readyList->Last();
				while (ready)
				{
					if (ready->pathLength >= pathLength)
					{
						readyList->InsertAfter(successor, ready);
						goto next;
					}

					ready = ready->Previous();
				}

				readyList->Prepend(successor);
			}

			next:
			route = route->GetNextOutgoingEdge();
		}
	}

	shaderSignature[0] = (unsigned_int32) (signature - shaderSignature - 1);
	return (processCount);
}

bool ShaderAttribute::AllocateOutputRegister(ProcessData *data, unsigned_int8 *registerLive)
{
	for (machine a = 0; a < kMaxShaderRegisterCount; a++)
	{
		if (registerLive[a] == 0)
		{
			registerLive[a] = 1;
			data->outputRegister = a;
			return (true);
		}
	}

	return (false);
}

bool ShaderAttribute::AllocateInterpolant(Type type, int32 size, ShaderAllocationData *allocData, bool (*usage)[4])
{
	int32 count = allocData->interpolantCount;
	if (count == kMaxShaderInterpolantCount)
	{
		return (false);
	}

	allocData->interpolantCount = count + 1;
	InterpolantData *data = &allocData->interpolantData[count];

	if ((type != kProcessTexcoord0) || (size != 4))
	{
		if (size >= 3)
		{
			for (machine a = 0; a < kMaxShaderTexcoordCount; a++)
			{
				if (*reinterpret_cast<int32 *>(usage) == 0)
				{
					data->interpolantType = type;
					data->texcoordIndex = a;

					data->swizzleData.size = (char) size;
					data->swizzleData.negate = false;
					data->swizzleData.component[0] = 0;
					data->swizzleData.component[1] = 1;
					data->swizzleData.component[2] = 2;
					data->swizzleData.component[3] = 3;

					(*usage)[0] = true;
					(*usage)[1] = true;
					(*usage)[2] = true;

					if (size == 4)
					{
						(*usage)[3] = true;
					}

					return (true);
				}

				usage++;
			}
		}
		else if (size == 2)
		{
			for (machine a = 0; a < kMaxShaderTexcoordCount; a++)
			{
				if (*reinterpret_cast<int16 *>(usage) == 0)
				{
					data->interpolantType = type;
					data->texcoordIndex = a;

					data->swizzleData.size = 2;
					data->swizzleData.negate = false;
					data->swizzleData.component[0] = 0;
					data->swizzleData.component[1] = 1;
					data->swizzleData.component[2] = 1;
					data->swizzleData.component[3] = 1;

					(*usage)[0] = true;
					(*usage)[1] = true;
					return (true);
				}
				else if (*reinterpret_cast<int16 *>(&(*usage)[2]) == 0)
				{
					data->interpolantType = type;
					data->texcoordIndex = a;

					data->swizzleData.size = 2;
					data->swizzleData.negate = false;
					data->swizzleData.component[0] = 2;
					data->swizzleData.component[1] = 3;
					data->swizzleData.component[2] = 3;
					data->swizzleData.component[3] = 3;

					(*usage)[2] = true;
					(*usage)[3] = true;
					return (true);
				}

				usage++;
			}
		}
		else
		{
			for (machine a = 0; a < kMaxShaderTexcoordCount; a++)
			{
				if (*reinterpret_cast<unsigned_int32 *>(usage) != 0x01010101)
				{
					for (machine b = 0; b < 4; b++)
					{
						if (!(*usage)[b])
						{
							data->interpolantType = type;
							data->texcoordIndex = a;

							data->swizzleData.size = 1;
							data->swizzleData.negate = false;
							data->swizzleData.component[0] = (unsigned_int8) b;
							data->swizzleData.component[1] = (unsigned_int8) b;
							data->swizzleData.component[2] = (unsigned_int8) b;
							data->swizzleData.component[3] = (unsigned_int8) b;

							(*usage)[b] = true;
							return (true);
						}
					}
				}

				usage++;
			}
		}
	}
	else
	{
		data->interpolantType = kProcessTexcoord0;
		data->texcoordIndex = 0;

		data->swizzleData.size = 4;
		data->swizzleData.negate = false;
		data->swizzleData.component[0] = 0;
		data->swizzleData.component[1] = 1;
		data->swizzleData.component[2] = 2;
		data->swizzleData.component[3] = 3;

		return (true);
	}

	return (false);
}

int32 ShaderAttribute::AllocateTextureUnit(ShaderData *shaderData, const Render::TextureObject *textureObject)
{
	int32 count = shaderData->textureArray.textureCount;
	for (machine a = 0; a < count; a++)
	{
		if (shaderData->textureArray.textureHandle[a] == textureObject->GetTextureHandle())
		{
			return (a);
		}
	}

	if (count == kMaxShaderTextureCount)
	{
		return (-1);
	}

	shaderData->textureArray.textureCount = count + 1;
	shaderData->SetTextureObject(count, textureObject);
	return (count);
}

ShaderResult ShaderAttribute::AllocateShaderResources(const ShaderCompileData *compileData, ShaderAllocationData *allocData, int32 processCount, ProcessData *processData, const List<Process> *scheduleList)
{
	union
	{
		unsigned_int8	registerLive[kMaxShaderRegisterCount];
		unsigned_int32	registerLiveLong[kMaxShaderRegisterCount / 4];
	};

	union
	{
		bool	texcoordUsage[kMaxShaderTexcoordCount][4];
		int32	texcoordUsageLong[kMaxShaderTexcoordCount];
	};

	int32	interpolantCount[4];
	Type	interpolantType[4][kMaxShaderInterpolantCount];

	allocData->maxRegister = -1;
	allocData->temporaryCount = 0;

	#if C4PS3 //[ PS3

		// -- PS3 code hidden --

	#endif //]

	allocData->literalCount = 0;
	allocData->interpolantCount = 0;

	for (machine a = 0; a < kMaxShaderRegisterCount / 4; a++)
	{
		registerLiveLong[a] = 0;
	}

	for (machine a = 0; a < kMaxShaderTexcoordCount; a++)
	{
		texcoordUsageLong[a] = 0;
	}

	for (machine a = 0; a < 4; a++)
	{
		interpolantCount[a] = 0;
	}

	#if !C4OPENGL

		const Renderable *renderable = compileData->renderable;
		bool pointSprite = ((renderable) && (renderable->GetRenderType() == kRenderPointSprites));
		if (pointSprite)
		{
			texcoordUsageLong[0] = 0x01010101;
		}

	#endif

	const Process *process = scheduleList->First();
	while (process)
	{
		processData->registerCount = 0;
		processData->preregisterCount = 0;
		processData->temporaryCount = 0;
		processData->literalCount = 0;
		processData->interpolantCount = 0;
		processData->textureCount = 0;
		processData->passthruPort = -1;
		processData->outputRegister = -1;
		processData->outputCount = 0;

		process->processData = processData;
		process->GenerateProcessData(compileData, processData);

		if (processData->preregisterCount != 0)
		{
			if (!AllocateOutputRegister(processData, registerLive))
			{
				return (kShaderRegisterOverflow);
			}

			allocData->maxRegister = Max(allocData->maxRegister, processData->outputRegister);
			processData->outputCount = process->GetOutgoingEdgeCount();
		}

		const Route *route = process->GetFirstIncomingEdge();
		while (route)
		{
			const ProcessData *predecessorData = route->GetStartElement()->GetProcessData();

			int32 predecessorRegister = predecessorData->outputRegister;
			if ((predecessorRegister >= 0) && (--predecessorData->outputCount == 0))
			{
				registerLive[predecessorRegister]--;
			}

			if (route->GetRoutePort() == processData->passthruPort)
			{
				if (predecessorRegister >= 0)
				{
					registerLive[predecessorRegister]++;
				}

				processData->outputCount = process->GetOutgoingEdgeCount();
				processData->outputRegister = predecessorRegister;
			}

			route = route->GetNextIncomingEdge();
		}

		if (processData->registerCount != 0)
		{
			if (!AllocateOutputRegister(processData, registerLive))
			{
				return (kShaderRegisterOverflow);
			}

			allocData->maxRegister = Max(allocData->maxRegister, processData->outputRegister);
			processData->outputCount = process->GetOutgoingEdgeCount();
		}

		allocData->temporaryCount = Max(allocData->temporaryCount, processData->temporaryCount);

		int32 literalCount = processData->literalCount;
		if (literalCount > 0)
		{
			int32 start = allocData->literalCount;
			int32 count = start + literalCount;
			if (count > kMaxShaderLiteralCount)
			{
				return (kShaderLiteralOverflow);
			}

			allocData->literalCount = count;

			for (machine a = 0; a < literalCount; a++)
			{
				allocData->literalData[start + a] = processData->literalData[a];
			}
		}

		for (machine a = 0; a < processData->interpolantCount; a++)
		{
			Type type = processData->interpolantType[a];
			int32 size = InterpolantProcess::GetInterpolantSize(type) - 1;
			Assert(size >= 0, "ShaderAttribute::AllocateShaderResources(), unrecognized interpolant\n");

			#if !C4OPENGL

				if ((type == kProcessTexcoord0) && (pointSprite))
				{
					size = 3;
				}

			#endif

			int32 count = interpolantCount[size];
			for (machine b = 0; b < count; b++)
			{
				if (interpolantType[size][b] == type)
				{
					goto next;
				}
			}

			if (count == kMaxShaderInterpolantCount)
			{
				return (kShaderTexcoordOverflow);
			}

			interpolantType[size][count] = type;
			interpolantCount[size] = count + 1;

			next:;
		}

		for (machine a = 0; a < processData->textureCount; a++)
		{
			int32 unit = AllocateTextureUnit(compileData->shaderData, processData->textureObject[a]);
			if (unit < 0)
			{
				return (kShaderTextureUnitOverflow);
			}

			processData->textureUnit[a] = (unsigned_int8) unit;

			if ((a == 0) && (process->GetBaseProcessType() == kProcessTextureMap))
			{
				*static_cast<const TextureMapProcess *>(process)->signatureUnit = unit;
			}

			#if C4PS3 //[ PS3

				// -- PS3 code hidden --

			#endif //]
		}

		process = process->Next();
		processData++;
	}

	for (machine a = 3; a >= 0; a--)
	{
		int32 count = interpolantCount[a];
		for (machine b = 0; b < count; b++)
		{
			if (!AllocateInterpolant(interpolantType[a][b], a + 1, allocData, texcoordUsage))
			{
				return (kShaderTexcoordOverflow);
			}
		}
	}

	return (kShaderOkay);
}

int32 ShaderAttribute::GenerateSwizzleData(const char *code, SwizzleData *swizzleData)
{
	machine count = 0;
	for (; count < 4; count++)
	{
		int32 c = code[count] - 'a';
		if ((unsigned_int32) c >= 26U)
		{
			break;
		}

		swizzleData->component[count] = Route::swizzleTable[c];
	}

	swizzleData->size = (char) count;
	return (count);
}

int32 ShaderAttribute::GenerateLiteralConstantValue(Type type, const ShaderAllocationData *allocData, char *value)
{
	int32 count = allocData->literalCount;
	const LiteralData *literalData = allocData->literalData;
	for (machine a = 0; a < count; a++)
	{
		if (literalData->literalType == type)
		{
			return (Text::FloatToString(literalData->literalValue, value, 15));
		}

		literalData++;
	}

	Fatal("ShaderAttribute::GenerateLiteralConstantValue(), literal type not found\n");
	return (0);
}

void ShaderAttribute::GenerateShaderCode(const ShaderCompileData *compileData, const ShaderAllocationData *allocData, const ProcessData *processData, const List<Process> *scheduleList, char *shaderCode, int32 *shaderLength)
{
	#if C4OPENGL

		static const char *const textureUnitName[kMaxShaderTextureCount] =
		{
			"texture0", "texture1", "texture2", "texture3", "texture4", "texture5", "texture6", "texture7",
			"texture8", "texture9", "texture10", "texture11", "texture12", "texture13"
		};

	#elif C4PSSL //[ PS4

		// -- PS4 code hidden --

	#elif C4CG //[ PS3

		// -- PS3 code hidden --

	#endif //]

	const char *start = shaderCode;

	const Process *process = scheduleList->First();
	while (process)
	{
		const char	*codeArray[kMaxProcessCodeCount];

		int32 codeCount = process->GenerateShaderCode(compileData, codeArray);
		Assert(codeCount <= kMaxProcessCodeCount, "ShaderAttribute::GenerateShaderCode(), shader code array overflow\n");

		for (machine codeIndex = 0; codeIndex < codeCount; codeIndex++)
		{
			const char *code = codeArray[codeIndex];
			for (;;)
			{
				SwizzleData		swizzleData;

				int32 c = *code++;
				if (c == 0)
				{
					break;
				}

				if (c == '%')
				{
					c = *code++;
					int32 n = c - '0';
					if ((unsigned_int32) n < 10U)
					{
						swizzleData.size = (char) processData->inputSize[n];
						swizzleData.negate = (code[-3] == '-');
						shaderCode -= swizzleData.negate;

						if (*code != '.')
						{
							for (machine a = 0; a < 4; a++)
							{
								swizzleData.component[a] = (unsigned_int8) a;
							}
						}
						else
						{
							code++;
							code += GenerateSwizzleData(code, &swizzleData);
						}

						const Route *route = process->GetPortRoute(n);
						shaderCode += route->GenerateOutputIdentifier(compileData, allocData, &swizzleData, shaderCode);
						continue;
					}
					else
					{
						if ((c == 'I') && (code[0] == 'M') && (code[1] == 'G'))
						{
							int32 unit = processData->textureUnit[code[2] - '0'];
							shaderCode += Text::CopyText(textureUnitName[unit], shaderCode);
							code += 3;
							continue;
						}
						else if ((c == 'T') && (code[0] == 'R') && (code[1] == 'G'))
						{
							#if C4OPENGL

								shaderCode += Text::CopyText("texture", shaderCode);

							#elif C4PSSL //[ PS4

								// -- PS4 code hidden --

							#elif C4CG //[ PS3

								// -- PS3 code hidden --

							#endif //]

							code += 3;
							continue;
						}
					}
				}
				else if (c == '#')
				{
					shaderCode += process->GenerateOutputIdentifier(compileData, allocData, nullptr, shaderCode);

					c = *code;
					if (c != '#')
					{
						int32 size = processData->outputSize;
						if ((size != 4) && (c != '.'))
						{
							*shaderCode++ = '.';
							for (machine a = 0; a < size; a++)
							{
								*shaderCode++ = Route::GetSwizzleChar(a);
							}
						}
					}
					else
					{
						code++;
					}

					continue;
				}
				else if (c == '$')
				{
					Type type = (code[0] << 24) | (code[1] << 16) | (code[2] << 8) | code[3];
					code += 4;

					if (*code == '.')
					{
						swizzleData.negate = (code[-3] == '-');
						shaderCode -= swizzleData.negate;

						code++;
						code += GenerateSwizzleData(code, &swizzleData);
						shaderCode += InterpolantProcess::GetInterpolantName(type, allocData, shaderCode, &swizzleData);
					}
					else
					{
						shaderCode += InterpolantProcess::GetInterpolantName(type, allocData, shaderCode);
					}

					continue;
				}
				else if ((c == '&') && (code[0] != ' '))
				{
					Type type = (code[0] << 24) | (code[1] << 16) | (code[2] << 8) | code[3];
					code += 4;

					shaderCode += GenerateLiteralConstantValue(type, allocData, shaderCode);
					continue;
				}

				*shaderCode++ = (char) c;
			}
		}

		process = process->Next();
		processData++;
	}

	*shaderLength = (int32) (shaderCode - start);
}

int32 ShaderAttribute::GenerateShaderProlog(const ShaderCompileData *compileData, const ShaderAllocationData *allocData, char *shaderCode)
{
	#if C4OPENGL

		static const char *const samplerParams[kShaderTypeCount + 1] =
		{
			#if C4GLCORE

				// kShaderNone
				"uniform sampler2DRect colorTexture;\n"
				"uniform sampler2DRect structureTexture;\n"
				"uniform sampler2DRect velocityTexture;\n"
				"uniform sampler2DRect occlusionTexture;\n"
				"uniform sampler2DRect distortionTexture;\n"
				"uniform sampler2DRect glowBloomTexture;\n"
				"uniform sampler2D noiseTexture;\n",

				// kShaderAmbient
				"",

				// kShaderAmbientRadiosity
				"",

				// kShaderUnified
				"uniform sampler2DArrayShadow shadowTexture;\n",

				// kShaderUnifiedRadiosity
				"uniform sampler2DArrayShadow shadowTexture;\n",

				// kShaderInfiniteLight
				"uniform sampler2DArrayShadow shadowTexture;\n",

				// kShaderPointLight
				"uniform samplerCubeShadow shadowTexture;\n",

				// kShaderCubeLight
				"uniform samplerCubeShadow shadowTexture;\n"
				"uniform samplerCube projectorCUBE;\n",

				// kShaderSpotLight
				"uniform sampler2DShadow shadowTexture;\n"
				"uniform sampler2D projector2D;\n",

				// kShaderShadow
				"",

				// kShaderStructure
				""

			#else //[ MOBILE

				// -- Mobile code hidden --

			#endif //]
		};

		static const char *const samplerTarget[Render::kTextureTargetCount] =
		{
			#if C4GLCORE

				"uniform sampler2D ", "uniform sampler3D ", "uniform sampler2DRect ", "uniform samplerCube ", "uniform sampler2DArray "

			#else //[ MOBILE

				// -- Mobile code hidden --

			#endif //]
		};

		static const char *const textureName[kMaxShaderTextureCount] =
		{
			"texture0;\n", "texture1;\n", "texture2;\n", "texture3;\n", "texture4;\n", "texture5;\n", "texture6;\n", "texture7;\n",
			"texture8;\n", "texture9;\n", "texture10;\n", "texture11;\n", "texture12;\n", "texture13;\n"
		};

	#elif C4PSSL //[ PS4

		// -- PS4 code hidden --

	#elif C4CG //[ PS3

		// -- PS3 code hidden --

	#endif //]

	int32 len = Text::CopyText(FragmentShader::prolog1Text, shaderCode);

	#if C4OPENGL

		len += Text::CopyText(samplerParams[compileData->shaderType + 1], shaderCode + len);

	#endif

	const ShaderData *shaderData = compileData->shaderData;
	if (shaderData)
	{
		int32 unitCount = shaderData->textureArray.textureCount;
		for (machine a = 0; a < unitCount; a++)
		{
			len += Text::CopyText(samplerTarget[shaderData->textureArray.textureTarget[a]], shaderCode + len);
			len += Text::CopyText(textureName[a], shaderCode + len);
		}
	}

	len += Text::CopyText(FragmentShader::prolog2Text, shaderCode + len);

	#if C4PS4 //[ PS4

		// -- PS4 code hidden --

	#endif //]

	int32 count = allocData->temporaryCount;
	for (machine a = 1; a <= count; a++)
	{
		len += Text::CopyText(", tmp", shaderCode + len);
		shaderCode[len++] = (char) (a + '0');
	}

	count = allocData->maxRegister;
	for (machine a = 0; a <= count; a++)
	{
		len += Text::CopyText(", r", shaderCode + len);
		if (a < 10)
		{
			shaderCode[len++] = (char) (a + '0');
		}
		else
		{
			int32 d = a / 10;
			shaderCode[len++] = (char) (d + '0');
			shaderCode[len++] = (char) (a - d * 10 + '0');
		}
	}

	len += Text::CopyText(";\n", shaderCode + len);
	return (len);
}

int32 ShaderAttribute::GenerateVertexOutputName(Type type, const ShaderAllocationData *allocData, int32 mask, char *name)
{
	static const char *const texcoordName[kMaxShaderTexcoordCount] =
	{
		#if C4OPENGL

			"texcoord[0]", "texcoord[1]", "texcoord[2]", "texcoord[3]", "texcoord[4]", "texcoord[5]", "texcoord[6]",
			"texcoord[7]", "texcoord[8]", "texcoord[9]", "texcoord[10]", "texcoord[11]", "texcoord[12]"

		#else

			"result.texcoord[0]", "result.texcoord[1]", "result.texcoord[2]", "result.texcoord[3]", "result.texcoord[4]", "result.texcoord[5]", "result.texcoord[6]",
			"result.texcoord[7]", "result.texcoord[8]", "result.texcoord[9]", "result.texcoord[10]", "result.texcoord[11]", "result.texcoord[12]"

		#endif
	};

	int32 count = allocData->interpolantCount;
	const InterpolantData *interpolantData = allocData->interpolantData;
	for (machine a = 0; a < count; a++)
	{
		if (interpolantData->interpolantType == type)
		{
			int32 len = Text::CopyText(texcoordName[interpolantData->texcoordIndex], name);

			int32 size = interpolantData->swizzleData.size;
			if ((size < 4) && (mask >= 0))
			{
				name[len++] = '.';

				if (mask == 0)
				{
					for (machine b = 0; b < size; b++)
					{
						name[len++] = Route::GetSwizzleChar(interpolantData->swizzleData.component[b]);
					}
				}
				else
				{
					Assert((mask == 'x') || (mask == 'y'), "ShaderAttribute::GenerateVertexOutputName(), illegal write mask\n");

					name[len++] = Route::GetSwizzleChar(interpolantData->swizzleData.component[mask - 'x']);
				}
			}

			return (len);
		}

		interpolantData++;
	}

	Fatal("ShaderAttribute::GenerateVertexOutputName(), interpolant not found\n");
	return (0);
}

VertexShader *ShaderAttribute::GenerateVertexShader(const ShaderCompileData *compileData, const ShaderAllocationData *allocData, unsigned_int32 *shaderStateFlags)
{
	union
	{
		bool			snippetFlag[kVertexSnippetCount];
		unsigned_int32	flagStorage[(kVertexSnippetCount + 3) / 4];
	};

	VertexAssembly assembly(signatureStorage);

	for (machine a = 0; a < (kVertexSnippetCount + 3) / 4; a++)
	{
		flagStorage[a] = 0;
	}

	unsigned_int32 stateFlags = 0;
	const Renderable *renderable = compileData->renderable;
	unsigned_int32 shaderFlags = renderable->GetShaderFlags();

	bool normalFlag = ((shaderFlags & (kShaderWaterElevation | kShaderNormalExpandVertex)) != 0);
	bool tangentFlag = false;
	bool bitangentFlag = false;

	for (machine a = 0; a < allocData->interpolantCount; a++)
	{
		switch (allocData->interpolantData[a].interpolantType)
		{
			case 'TEX0':

				#if C4OPENGL

					stateFlags = renderable->BuildTexcoord0Transform(compileData->renderSegment, compileData->shaderData, &assembly, stateFlags);

				#else

					if (renderable->GetRenderType() != kRenderPointSprites)
					{
						stateFlags = renderable->BuildTexcoord0Transform(compileData->renderSegment, compileData->shaderData, &assembly, stateFlags);
					}

				#endif

				break;

			case 'TEX1':

				stateFlags = renderable->BuildTexcoord1Transform(compileData->renderSegment, compileData->shaderData, &assembly, stateFlags);
				break;

			case 'POSI':

				snippetFlag[kVertexSnippetOutputObjectPosition] = true;
				break;

			case 'NRML':

				snippetFlag[kVertexSnippetOutputObjectNormal] = true;
				normalFlag = true;
				break;

			case 'TANG':

				snippetFlag[kVertexSnippetOutputObjectTangent] = true;
				tangentFlag = true;
				break;

			case 'BTNG':

				snippetFlag[kVertexSnippetOutputObjectBitangent] = true;
				bitangentFlag = true;
				break;

			case 'WPOS':

				snippetFlag[kVertexSnippetOutputWorldPosition] = true;
				stateFlags |= kShaderStateWorldTransform;
				break;

			case 'WNRM':

				snippetFlag[kVertexSnippetOutputWorldNormal] = true;
				stateFlags |= kShaderStateWorldTransform;
				normalFlag = true;
				break;

			case 'WTAN':

				snippetFlag[kVertexSnippetOutputWorldTangent] = true;
				stateFlags |= kShaderStateWorldTransform;
				tangentFlag = true;
				break;

			case 'WBTN':

				snippetFlag[kVertexSnippetOutputWorldBitangent] = true;
				stateFlags |= kShaderStateWorldTransform;
				bitangentFlag = true;
				break;

			case 'NRMC':

				snippetFlag[kVertexSnippetOutputCameraNormal] = true;
				stateFlags |= kShaderStateCameraTransform;
				normalFlag = true;
				break;

			case 'GEOM':

				snippetFlag[kVertexSnippetOutputVertexGeometry] = true;
				break;

			case 'IDEP':

				snippetFlag[kVertexSnippetOutputImpostorDepth] = true;
				stateFlags |= kShaderStateCameraTransform;
				break;

			case 'IRAD':

				snippetFlag[kVertexSnippetOutputImpostorRadius] = true;
				break;

			case 'ISRD':

				snippetFlag[kVertexSnippetOutputImpostorShadowRadius] = true;
				break;

			case 'LDIR':

				if (compileData->shaderType >= kShaderFirstPointLight)
				{
					snippetFlag[kVertexSnippetCalculateObjectPointLightDirection] = true;

					if (!(shaderFlags & kShaderVertexBillboard))
					{
						snippetFlag[kVertexSnippetOutputTangentPointLightDirection] = true;
						bitangentFlag = true;
					}
					else
					{
						snippetFlag[kVertexSnippetCalculateObjectViewDirection] = true;
						snippetFlag[kVertexSnippetOutputBillboardPointLightDirection] = true;
						stateFlags |= kShaderStateCameraPosition;
					}
				}
				else
				{
					if (!(shaderFlags & kShaderVertexBillboard))
					{
						snippetFlag[kVertexSnippetOutputTangentInfiniteLightDirection] = true;
						bitangentFlag = true;
					}
					else
					{
						snippetFlag[kVertexSnippetCalculateObjectViewDirection] = true;
						snippetFlag[kVertexSnippetOutputBillboardInfiniteLightDirection] = true;
						stateFlags |= kShaderStateCameraPosition;
					}
				}

				break;

			case 'VDIR':

				snippetFlag[kVertexSnippetCalculateObjectViewDirection] = true;
				snippetFlag[kVertexSnippetOutputTangentViewDirection] = true;
				stateFlags |= kShaderStateCameraPosition;
				bitangentFlag = true;
				break;

			case 'OLDR':

				if (compileData->shaderType >= kShaderFirstPointLight)
				{
					snippetFlag[kVertexSnippetCalculateObjectPointLightDirection] = true;
					snippetFlag[kVertexSnippetOutputObjectPointLightDirection] = true;
				}
				else
				{
					snippetFlag[kVertexSnippetOutputObjectInfiniteLightDirection] = true;
				}

				break;

			case 'OVDR':

				snippetFlag[kVertexSnippetCalculateObjectViewDirection] = true;
				snippetFlag[kVertexSnippetOutputObjectViewDirection] = true;
				stateFlags |= kShaderStateCameraPosition;
				break;

			case 'TLDR':

				snippetFlag[kVertexSnippetCalculateTerrainTangentData] = true;

				if (compileData->shaderType >= kShaderFirstPointLight)
				{
					snippetFlag[kVertexSnippetOutputTerrainPointLightDirection] = true;
				}
				else
				{
					snippetFlag[kVertexSnippetOutputTerrainInfiniteLightDirection] = true;
				}

				normalFlag = true;
				break;

			case 'TVDR':

				snippetFlag[kVertexSnippetCalculateTerrainTangentData] = true;
				snippetFlag[kVertexSnippetOutputTerrainViewDirection] = true;
				stateFlags |= kShaderStateCameraPosition;
				break;

			case 'TWNM':

				snippetFlag[kVertexSnippetCalculateTerrainTangentData] = true;
				snippetFlag[kVertexSnippetOutputTerrainWorldTangentFrame] = true;
				stateFlags |= kShaderStateWorldTransform;
				break;

			case 'RTXC':

				renderable->SetShaderArray(compileData->shaderData, kVertexAttribTexture0, kArrayTexcoord0);
				snippetFlag[kVertexSnippetOutputRawTexcoords] = true;
				break;

			case 'TERA':

				snippetFlag[kVertexSnippetOutputTerrainTexcoords] = true;
				stateFlags |= kShaderStateTerrainTexcoordScale;
				break;

			case 'IMPT':

				renderable->SetShaderArray(compileData->shaderData, kVertexAttribRadius, kArrayRadius);
				renderable->SetShaderArray(compileData->shaderData, kVertexAttribTexture0, kArrayTexcoord0);
				snippetFlag[kVertexSnippetOutputImpostorTexcoords] = true;
				stateFlags |= kShaderStateCameraPosition4D;
				break;

			case 'IXBL':

				snippetFlag[kVertexSnippetOutputImpostorTransitionBlend] = true;
				stateFlags |= kShaderStateImpostorTransition;
				break;

			case 'GITX':

				snippetFlag[kVertexSnippetOutputGeometryImpostorTexcoords] = true;
				stateFlags |= kShaderStateGeometryTransition;
				break;

			case 'PTXC':

				snippetFlag[kVertexSnippetOutputPaintTexcoords] = true;
				stateFlags |= kShaderStatePaintTransform;
				break;

			case 'FIRE':

				if (shaderFlags & kShaderFireArrays)
				{
					renderable->SetShaderArray(compileData->shaderData, kVertexAttribRadius, kArrayRadius);
					renderable->SetShaderArray(compileData->shaderData, kVertexAttribTexture0, kArrayTexcoord0);
					renderable->SetShaderArray(compileData->shaderData, kVertexAttribTexture1, kArrayTexcoord1);
					snippetFlag[kVertexSnippetOutputFireArrayTexcoords] = true;
				}
				else
				{
					renderable->SetShaderArray(compileData->shaderData, kVertexAttribTexture0, kArrayTexcoord0);
					snippetFlag[kVertexSnippetOutputFireTexcoords] = true;
				}

				break;

			case 'WARP':

				snippetFlag[kVertexSnippetCalculateObjectViewDirection] = true;
				snippetFlag[kVertexSnippetCalculateCameraDistance] = true;
				snippetFlag[kVertexSnippetOutputCameraWarpFunction] = true;
				stateFlags |= kShaderStateCameraPosition | kShaderStateCameraDirections;
				normalFlag = true;
				break;

			case 'RGHT':

				snippetFlag[kVertexSnippetCalculateObjectViewDirection] = true;
				snippetFlag[kVertexSnippetCalculateCameraDistance] = true;
				snippetFlag[kVertexSnippetOutputCameraBumpWarpFunction] = true;
				stateFlags |= kShaderStateCameraPosition | kShaderStateCameraDirections;
				bitangentFlag = true;
				break;

			case 'DDEP':

				snippetFlag[kVertexSnippetOutputDistortionDepth] = true;
				break;

			case 'ATTN':

				if (compileData->shaderType != kShaderSpotLight)
				{
					snippetFlag[kVertexSnippetCalculateObjectPointLightDirection] = true;
					snippetFlag[kVertexSnippetOutputPointLightAttenuation] = true;
				}
				else
				{
					snippetFlag[kVertexSnippetOutputSpotLightAttenuation] = true;
				}

				break;

			case 'CSCD':

				snippetFlag[kVertexSnippetOutputCascadeTexcoord] = true;
				break;

			case 'PROJ':

				if (compileData->shaderType != kShaderSpotLight)
				{
					snippetFlag[kVertexSnippetOutputPointProjectTexcoord] = true;
				}
				else
				{
					snippetFlag[kVertexSnippetOutputSpotProjectTexcoord] = true;
				}

				break;

			case 'SPOT':

				snippetFlag[kVertexSnippetOutputSpotShadowTexcoord] = true;
				break;

			case 'RPOS':

				snippetFlag[kVertexSnippetOutputRadiosityPosition] = true;
				break;

			case 'RNRM':

				if (renderable->AttributeArrayEnabled(kArrayNormal))
				{
					snippetFlag[kVertexSnippetOutputRadiosityNormal] = true;
					normalFlag = true;
				}
				else
				{
					snippetFlag[kVertexSnippetCalculateObjectViewDirection] = true;
					snippetFlag[kVertexSnippetOutputRadiosityViewNormal] = true;
				}

				break;

			case 'RTNG':

				snippetFlag[kVertexSnippetOutputRadiosityTangent] = true;
				tangentFlag = true;
				break;

			case 'RTGH':

				snippetFlag[kVertexSnippetOutputRadiosityTangentHandedness] = true;
				tangentFlag = true;
				break;

			case 'FDTV':

				if (!(shaderFlags & kShaderVertexInfinite))
				{
					snippetFlag[kVertexSnippetCalculateObjectViewDirection] = true;

					if (compileData->shaderVariant == kShaderVariantConstantFog)
					{
						snippetFlag[kVertexSnippetOutputFiniteConstantFogFactors] = true;
					}
					else
					{
						snippetFlag[kVertexSnippetOutputFiniteLinearFogFactors] = true;
					}
				}
				else
				{
					if (compileData->shaderVariant == kShaderVariantConstantFog)
					{
						snippetFlag[kVertexSnippetOutputInfiniteConstantFogFactors] = true;
					}
					else
					{
						snippetFlag[kVertexSnippetOutputInfiniteLinearFogFactors] = true;
					}
				}

				break;

			case 'VELA':

				if (renderable->AttributeArrayEnabled(kArrayPrevious))
				{
					renderable->SetShaderArray(compileData->shaderData, kVertexAttribVelocity, kArrayPrevious);
					snippetFlag[kVertexSnippetDeformMotionBlurTransform] = true;
				}
				else if (renderable->AttributeArrayEnabled(kArrayVelocity))
				{
					renderable->SetShaderArray(compileData->shaderData, kVertexAttribVelocity, kArrayVelocity);
					snippetFlag[kVertexSnippetVelocityMotionBlurTransform] = true;
				}
				else
				{
					if (!(shaderFlags & kShaderVertexInfinite))
					{
						snippetFlag[kVertexSnippetMotionBlurTransform] = true;
					}
					else
					{
						snippetFlag[kVertexSnippetInfiniteMotionBlurTransform] = true;
					}
				}

				break;
		}
	}

	tangentFlag |= bitangentFlag;
	normalFlag |= tangentFlag;

	if (normalFlag)
	{
		if (shaderFlags & kShaderVertexPostboard)
		{
			assembly.AddSnippet(&VertexShader::generateImpostorFrame);
			stateFlags |= kShaderStateCameraPosition;
		}
		else
		{
			renderable->SetShaderArray(compileData->shaderData, kVertexAttribNormal, kArrayNormal);

			if (tangentFlag)
			{
				if (shaderFlags & kShaderGenerateTangent)
				{
					if (shaderFlags & kShaderNormalizeBasisVectors)
					{
						assembly.AddSnippet(&VertexShader::normalizeNormal);
					}

					assembly.AddSnippet(&VertexShader::generateTangent);
					assembly.AddSnippet(&VertexShader::calculateBitangent);
				}
				else
				{
					renderable->SetShaderArray(compileData->shaderData, kVertexAttribTangent, kArrayTangent);

					if (renderable->GetVertexAttributeComponentCount(kArrayTangent) == 3)
					{
						if (shaderFlags & kShaderNormalizeBasisVectors)
						{
							assembly.AddSnippet(&VertexShader::normalizeNormal);
							assembly.AddSnippet(&VertexShader::normalizeTangent);
						}

						assembly.AddSnippet(&VertexShader::calculateBitangent);
					}
					else
					{
						if (shaderFlags & kShaderNormalizeBasisVectors)
						{
							assembly.AddSnippet(&VertexShader::normalizeNormal);
							assembly.AddSnippet(&VertexShader::orthonormalizeTangent);
						}

						assembly.AddSnippet(&VertexShader::calculateBitangent);
						assembly.AddSnippet(&VertexShader::adjustBitangent);
					}
				}
			}
			else
			{
				if (shaderFlags & kShaderNormalizeBasisVectors)
				{
					assembly.AddSnippet(&VertexShader::normalizeNormal);
				}
			}
		}
	}

	stateFlags |= renderable->BuildVertexTransform(compileData->shaderData, &assembly);

	for (machine a = 0; a < kVertexSnippetCount; a++)
	{
		if (snippetFlag[a])
		{
			assembly.AddSnippet(&VertexShader::vertexSnippet[a]);
		}
	}

	if (compileData->shaderSourceFlags & kShaderSourcePrimaryColor)
	{
		renderable->SetShaderArray(compileData->shaderData, kVertexAttribColor0, kArrayColor0);
		assembly.AddSnippet(&VertexShader::outputPrimaryColor);

		if (compileData->shaderSourceFlags & kShaderSourceSecondaryColor)
		{
			renderable->SetShaderArray(compileData->shaderData, kVertexAttribColor1, kArrayColor1);
			assembly.AddSnippet(&VertexShader::outputSecondaryColor);
		}
	}

	if (renderable->GetRenderType() == kRenderPointSprites)
	{
		renderable->SetShaderArray(compileData->shaderData, kVertexAttribRadius, kArrayRadius);

		if (!(shaderFlags & kShaderVertexInfinite))
		{
			assembly.AddSnippet(&VertexShader::outputPointSize);
		}
		else
		{
			assembly.AddSnippet(&VertexShader::outputInfinitePointSize);
		}
	}

	int32 snippetCount = signatureStorage[0];
	unsigned_int32 *signature = &signatureStorage[snippetCount + 1];

	int32 interpolantCount = allocData->interpolantCount;
	signatureStorage[0] = snippetCount + interpolantCount * 2;

	for (machine a = 0; a < interpolantCount; a++)
	{
		const InterpolantData *data = &allocData->interpolantData[a];
		signature[0] = data->interpolantType;
		signature[1] = data->texcoordIndex | (data->swizzleData.component[0] << 8);
		signature += 2;
	}

	VertexShader *vertexShader = VertexShader::Find(signatureStorage);
	if (!vertexShader)
	{
		const char *positionText = nullptr;
		const char *normalText = nullptr;
		const char *tangentText = nullptr;

		int32 len = Text::CopyText(VertexShader::prologText, sourceStorage);
		char *string = sourceStorage + len;

		for (machine a = 0; a < snippetCount; a++)
		{
			const char *code = assembly.vertexSnippet[a]->shaderCode;
			for (;;)
			{
				int32 c = *code++;
				if (c == 0)
				{
					break;
				}

				if (c == '%')
				{
					if ((code[0] == 'O') && (code[1] == 'P') && (code[2] == 'O') && (code[3] == 'S'))
					{
						if (positionText)
						{
							string += Text::CopyText(positionText, string);
						}
						else
						{
							string += Text::CopyText(ATTRIB(VERTEX_ATTRIB_POSITION0), string);
							if (code[4] != '.')
							{
								string += Text::CopyText(".xyz", string);
							}
						}

						code += 4;
					}
					else if ((code[0] == 'N') && (code[1] == 'R') && (code[2] == 'M') && (code[3] == 'L'))
					{
						if (normalText)
						{
							string += Text::CopyText(normalText, string);
						}
						else
						{
							string += Text::CopyText(ATTRIB(VERTEX_ATTRIB_NORMAL), string);
							if (code[4] != '.')
							{
								string += Text::CopyText(".xyz", string);
							}
						}

						code += 4;
					}
					else if ((code[0] == 'T') && (code[1] == 'A') && (code[2] == 'N') && (code[3] == 'G'))
					{
						if (tangentText)
						{
							string += Text::CopyText(tangentText, string);
						}
						else
						{
							string += Text::CopyText(ATTRIB(VERTEX_ATTRIB_TANGENT), string);
							if (code[4] != '.')
							{
								string += Text::CopyText(".xyz", string);
							}
						}

						code += 4;
					}
					else if ((code[0] == 'H') && (code[1] == 'A') && (code[2] == 'N') && (code[3] == 'D'))
					{
						if (tangentText)
						{
							string += Text::CopyText("1.0", string);
						}
						else
						{
							string += Text::CopyText(ATTRIB(VERTEX_ATTRIB_TANGENT), string);
							string += Text::CopyText(".w", string);
						}

						code += 4;
					}
				}
				else if (c == '$')
				{
					Type type = (code[0] << 24) | (code[1] << 16) | (code[2] << 8) | code[3];
					code += 4;

					c = code[0];
					if (c != ':')
					{
						string += GenerateVertexOutputName(type, allocData, -(c == '.'), string);
					}
					else
					{
						string += GenerateVertexOutputName(type, allocData, code[1], string);
						code += 2;
					}
				}
				else
				{
					*string++ = (char) c;
				}
			}

			unsigned_int32 snippetFlags = assembly.vertexSnippet[a]->flags;
			if (snippetFlags & kVertexSnippetPositionFlag)
			{
				positionText = "opos";
			}

			if (snippetFlags & kVertexSnippetNormalFlag)
			{
				normalText = "nrml";
			}

			if (snippetFlags & kVertexSnippetTangentFlag)
			{
				tangentText = "tang";
			}
		}

		string += Text::CopyText(VertexShader::epilogText, string);

		unsigned_int32 size = (unsigned_int32) (string - sourceStorage);
		Assert(size < kMaxShaderSourceSize, "ShaderAttribute::GenerateVertexShader(), shader string overflow\n");

		vertexShader = VertexShader::New(sourceStorage, size, signatureStorage);
	}

	*shaderStateFlags = stateFlags;
	return (vertexShader);
}

void ShaderAttribute::BuildStateProcList(const ShaderCompileData *compileData, unsigned_int32 shaderStateFlags)
{
	ShaderData *shaderData = compileData->shaderData;

	if (compileData->renderable->GetTransformable())
	{
		if (shaderStateFlags & kShaderStateCameraPosition)
		{
			if (shaderStateFlags & kShaderStateCameraDirections)
			{
				shaderData->AddStateProc(&Renderable::StateProc_TransformCameraPositionAndDirections);
			}
			else
			{
				shaderData->AddStateProc(&Renderable::StateProc_TransformCameraPosition);
			}
		}
		else if (shaderStateFlags & kShaderStateCameraDirections)
		{
			shaderData->AddStateProc(&Renderable::StateProc_TransformCameraDirections);
		}

		if (shaderStateFlags & kShaderStateCameraPosition4D)
		{
			shaderData->AddStateProc(&Renderable::StateProc_TransformCameraPosition4D);
		}

		if (shaderStateFlags & kShaderStateCameraTransform)
		{
			shaderData->AddStateProc(&Renderable::StateProc_TransformCameraMatrix);
		}

		if (shaderStateFlags & kShaderStateWorldTransform)
		{
			shaderData->AddStateProc(&Renderable::StateProc_TransformWorldMatrix);
		}

		if (shaderStateFlags & kShaderStatePaintTransform)
		{
			shaderData->AddStateProc(&Renderable::StateProc_TransformPaintSpace);
		}

		if (shaderStateFlags & kShaderStateGeometryTransition)
		{
			shaderData->AddStateProc(&Renderable::StateProc_TransformGeometryTransition);
		}

		switch (compileData->shaderType)
		{
			case kShaderUnified:
			case kShaderUnifiedRadiosity:
			case kShaderInfiniteLight:

				if (compileData->shadowFlag)
				{
					shaderData->AddStateProc(&Renderable::StateProc_ConfigureTransformInfiniteLightShadow);
				}
				else
				{
					shaderData->AddStateProc(&Renderable::StateProc_ConfigureTransformInfiniteLight);
				}

				break;

			case kShaderPointLight:

				if (compileData->shadowFlag)
				{
					shaderData->AddStateProc(&Renderable::StateProc_ConfigureTransformCubeLight);
				}
				else
				{
					shaderData->AddStateProc(&Renderable::StateProc_ConfigureTransformPointLight);
				}

				break;

			case kShaderCubeLight:

				shaderData->AddStateProc(&Renderable::StateProc_ConfigureTransformCubeLight);
				break;

			case kShaderSpotLight:

				if (compileData->shadowFlag)
				{
					shaderData->AddStateProc(&Renderable::StateProc_ConfigureTransformSpotLightShadow);
				}
				else
				{
					shaderData->AddStateProc(&Renderable::StateProc_ConfigureTransformSpotLight);
				}

				break;
		}
	}
	else
	{
		if (shaderStateFlags & kShaderStateCameraPosition)
		{
			if (shaderStateFlags & kShaderStateCameraDirections)
			{
				shaderData->AddStateProc(&Renderable::StateProc_CopyCameraPositionAndDirections);
			}
			else
			{
				shaderData->AddStateProc(&Renderable::StateProc_CopyCameraPosition);
			}
		}
		else if (shaderStateFlags & kShaderStateCameraDirections)
		{
			shaderData->AddStateProc(&Renderable::StateProc_CopyCameraDirections);
		}

		if (shaderStateFlags & kShaderStateCameraPosition4D)
		{
			shaderData->AddStateProc(&Renderable::StateProc_CopyCameraPosition4D);
		}

		if (shaderStateFlags & kShaderStateCameraTransform)
		{
			shaderData->AddStateProc(&Renderable::StateProc_CopyCameraMatrix);
		}

		if (shaderStateFlags & kShaderStateWorldTransform)
		{
			shaderData->AddStateProc(&Renderable::StateProc_CopyWorldMatrix);
		}

		if (shaderStateFlags & kShaderStatePaintTransform)
		{
			shaderData->AddStateProc(&Renderable::StateProc_CopyPaintSpace);
		}

		if (shaderStateFlags & kShaderStateGeometryTransition)
		{
			shaderData->AddStateProc(&Renderable::StateProc_CopyGeometryTransition);
		}

		switch (compileData->shaderType)
		{
			case kShaderUnified:
			case kShaderUnifiedRadiosity:
			case kShaderInfiniteLight:

				if (compileData->shadowFlag)
				{
					shaderData->AddStateProc(&Renderable::StateProc_ConfigureInfiniteLightShadow);
				}
				else
				{
					shaderData->AddStateProc(&Renderable::StateProc_ConfigureInfiniteLight);
				}

				break;

			case kShaderPointLight:

				if (compileData->shadowFlag)
				{
					shaderData->AddStateProc(&Renderable::StateProc_ConfigureCubeLight);
				}
				else
				{
					shaderData->AddStateProc(&Renderable::StateProc_ConfigurePointLight);
				}

				break;

			case kShaderCubeLight:

				shaderData->AddStateProc(&Renderable::StateProc_ConfigureCubeLight);
				break;

			case kShaderSpotLight:

				if (compileData->shadowFlag)
				{
					shaderData->AddStateProc(&Renderable::StateProc_ConfigureSpotLightShadow);
				}
				else
				{
					shaderData->AddStateProc(&Renderable::StateProc_ConfigureSpotLight);
				}

				break;
		}
	}

	if (shaderStateFlags & (kShaderStateTexcoordTransform0 | kShaderStateTexcoordTransform1 | kShaderStateTexcoordVelocity0 | kShaderStateTexcoordVelocity1))
	{
		const MaterialObject *materialObject = *compileData->renderSegment->GetMaterialObjectPointer();
		if (materialObject)
		{
			if (shaderStateFlags & kShaderStateTexcoordGenerate)
			{
				if (shaderStateFlags & kShaderStateTexcoordTransform0)
				{
					if (shaderStateFlags & kShaderStateTexcoordVelocity0)
					{
						shaderData->AddStateProc(&Renderable::StateProc_GenerateTransformAnimateTexcoord0, materialObject);
					}
					else
					{
						shaderData->AddStateProc(&Renderable::StateProc_GenerateTransformTexcoord0, materialObject);
					}
				}
				else if (shaderStateFlags & kShaderStateTexcoordVelocity0)
				{
					if (shaderStateFlags & kShaderStateTexcoordVelocity1)
					{
						shaderData->AddStateProc(&Renderable::StateProc_GenerateAnimateDualTexcoords, materialObject);
					}
					else
					{
						shaderData->AddStateProc(&Renderable::StateProc_GenerateAnimateTexcoord0, materialObject);
					}
				}

				if (shaderStateFlags & kShaderStateTexcoordTransform1)
				{
					if (shaderStateFlags & kShaderStateTexcoordVelocity1)
					{
						shaderData->AddStateProc(&Renderable::StateProc_GenerateTransformAnimateTexcoord1, materialObject);
					}
					else
					{
						shaderData->AddStateProc(&Renderable::StateProc_GenerateTransformTexcoord1, materialObject);
					}
				}
				else if (shaderStateFlags & kShaderStateTexcoordVelocity1)
				{
					if (!(shaderStateFlags & kShaderStateTexcoordVelocity0))
					{
						shaderData->AddStateProc(&Renderable::StateProc_GenerateAnimateTexcoord1, materialObject);
					}
				}
			}
			else
			{
				if (shaderStateFlags & kShaderStateTexcoordTransform0)
				{
					if (shaderStateFlags & kShaderStateTexcoordVelocity0)
					{
						shaderData->AddStateProc(&Renderable::StateProc_TransformAnimateTexcoord0, materialObject);
					}
					else
					{
						shaderData->AddStateProc(&Renderable::StateProc_TransformTexcoord0, materialObject);
					}
				}
				else if (shaderStateFlags & kShaderStateTexcoordVelocity0)
				{
					shaderData->AddStateProc(&Renderable::StateProc_AnimateTexcoord0, materialObject);
				}

				if (shaderStateFlags & kShaderStateTexcoordTransform1)
				{
					if (shaderStateFlags & kShaderStateTexcoordVelocity1)
					{
						shaderData->AddStateProc(&Renderable::StateProc_TransformAnimateTexcoord1, materialObject);
					}
					else
					{
						shaderData->AddStateProc(&Renderable::StateProc_TransformTexcoord1, materialObject);
					}
				}
				else if (shaderStateFlags & kShaderStateTexcoordVelocity1)
				{
					shaderData->AddStateProc(&Renderable::StateProc_AnimateTexcoord1, materialObject);
				}
			}
		}
	}
	else
	{
		if (shaderStateFlags & kShaderStateTexcoordGenerate)
		{
			shaderData->AddStateProc(&Renderable::StateProc_GenerateTexcoord);
		}
	}

	if (shaderStateFlags & kShaderStateTerrainTexcoordScale)
	{
		const MaterialObject *materialObject = *compileData->renderSegment->GetMaterialObjectPointer();
		if (materialObject)
		{
			shaderData->AddStateProc(&Renderable::StateProc_ScaleTerrainTexcoord, materialObject);
		}
	}

	if (shaderStateFlags & kShaderStateVertexScaleOffset)
	{
		shaderData->AddStateProc(&Renderable::StateProc_CopyVertexScaleOffset);
	}

	if (shaderStateFlags & kShaderStateTerrainBorder)
	{
		shaderData->AddStateProc(&Renderable::StateProc_CopyTerrainParameters);
	}

	if (shaderStateFlags & kShaderStateImpostorTransition)
	{
		shaderData->AddStateProc(&Renderable::StateProc_CopyImpostorTransition);
	}
}

#if C4OPENGL

	void ShaderAttribute::BindShaderUniforms(ShaderType type, ShaderProgram *shaderProgram)
	{
		static const char *const textureName[kMaxShaderTextureCount] =
		{
			"texture0", "texture1", "texture2", "texture3", "texture4", "texture5", "texture6", "texture7",
			"texture8", "texture9", "texture10", "texture11", "texture12", "texture13"
		};

		Render::SetShaderProgram(shaderProgram);

		switch (type)
		{
			case kShaderNone:

				Render::SetShaderTextureUnit(shaderProgram, "colorTexture", kTextureUnitColor);
				Render::SetShaderTextureUnit(shaderProgram, "structureTexture", kTextureUnitStructure);
				Render::SetShaderTextureUnit(shaderProgram, "velocityTexture", kTextureUnitVelocity);
				Render::SetShaderTextureUnit(shaderProgram, "occlusionTexture", kTextureUnitOcclusion);
				Render::SetShaderTextureUnit(shaderProgram, "distortionTexture", kTextureUnitDistortion);
				Render::SetShaderTextureUnit(shaderProgram, "glowBloomTexture", kTextureUnitGlowBloom);
				Render::SetShaderTextureUnit(shaderProgram, "noiseTexture", kTextureUnitNoise);
				break;

			case kShaderUnified:
			case kShaderUnifiedRadiosity:
			case kShaderInfiniteLight:

				Render::SetShaderTextureUnit(shaderProgram, "shadowTexture", kTextureUnitShadowMap);
				break;

			case kShaderPointLight:

				Render::SetShaderTextureUnit(shaderProgram, "shadowTexture", kTextureUnitShadowMap);
				break;

			case kShaderCubeLight:

				Render::SetShaderTextureUnit(shaderProgram, "projectorCUBE", kTextureUnitLightProjector);
				Render::SetShaderTextureUnit(shaderProgram, "shadowTexture", kTextureUnitShadowMap);
				break;

			case kShaderSpotLight:

				Render::SetShaderTextureUnit(shaderProgram, "projector2D", kTextureUnitLightProjector);
				Render::SetShaderTextureUnit(shaderProgram, "shadowTexture", kTextureUnitShadowMap);
				break;
		}

		for (machine a = 0; a < kMaxShaderTextureCount; a++)
		{
			Render::SetShaderTextureUnit(shaderProgram, textureName[a], a);
		}
	}

#endif

ShaderResult ShaderAttribute::CompileShader(ShaderType type, const ShaderKey& key, const Renderable *renderable, const RenderSegment *renderSegment, ShaderData *shaderData) const
{
	ShaderGraph		tempGraph;

	CloneShader(&shaderGraph, &tempGraph, true);
	return (CompileShader(&tempGraph, type, key, renderable, renderSegment, shaderData));
}

ShaderResult ShaderAttribute::CompileShader(ShaderGraph *graph, ShaderType type, const ShaderKey& key, const Renderable *renderable, const RenderSegment *renderSegment, ShaderData *shaderData)
{
	List<Process>			processList;
	List<Process>			readyList;
	ShaderCompileData		compileData;
	ShaderAllocationData	allocData;
	ShaderResult			result;

	compileData.renderable = renderable;
	compileData.renderSegment = renderSegment;
	compileData.shaderData = shaderData;

	compileData.shaderType = type;
	compileData.shaderVariant = key.GetShaderVariant();
	compileData.detailLevel = key.GetDetailLevel();
	compileData.shadowFlag = key.GetShadowFlag();

	compileData.shaderSourceFlags = 0;
	compileData.terrainViewDirection = nullptr;

	if (type <= kShaderLastAmbient)
	{
		result = PrepareAmbientShader(&compileData, graph, &processList);
	}
	else if (type <= kShaderLastUnified)
	{
		result = PrepareUnifiedShader(&compileData, graph, &processList);
	}
	else if (type <= kShaderLastLight)
	{
		result = PrepareLightShader(&compileData, graph, &processList);
	}
	else
	{
		result = PreparePlainShader(type, &compileData, graph, &processList);
	}

	if (result == kShaderOkay)
	{
		OptimizeTextureMaps(graph);
		EliminateDeadCode(graph, &processList);
		OrganizeDerivedInterpolants(&compileData, graph);

		CalculatePathLengths(graph, &processList, &readyList);
		int32 processCount = ScheduleShader(&compileData, &readyList, &processList, signatureStorage);

		if (renderable->GetRenderType() == kRenderPointSprites)
		{
			unsigned_int32 size = signatureStorage[0];
			signatureStorage[0] = size + 1;
			signatureStorage[size] = 1;
		}

		Assert(signatureStorage[0] < kMaxShaderSignatureSize, "ShaderAttribute::CompileShader(), signature overflow\n");

		ProcessData *processData = new ProcessData[processCount];

		result = AllocateShaderResources(&compileData, &allocData, processCount, processData, &processList);
		if (result == kShaderOkay)
		{
			ProgramStageTable	stageTable;
			unsigned_int32		shaderStateFlags;
			int32				geometryShaderIndex;

			FragmentShader *fragmentShader = FragmentShader::Find(signatureStorage);
			if (!fragmentShader)
			{
				int32 length = GenerateShaderProlog(&compileData, &allocData, sourceStorage);
				char *string = sourceStorage + length;

				GenerateShaderCode(&compileData, &allocData, processData, &processList, string, &length);

				string += length;
				string += Text::CopyText(FragmentShader::epilogText, string);

				unsigned_int32 size = (unsigned_int32) (string - sourceStorage);
				Assert(size < kMaxShaderSourceSize, "ShaderAttribute::CompileShader(), shader string overflow\n");

				fragmentShader = FragmentShader::New(sourceStorage, size, signatureStorage);

				#if C4PS3 //[ PS3

					// -- PS3 code hidden --

				#endif //]
			}

			stageTable.fragmentShader = fragmentShader;

			if ((!renderable) || ((geometryShaderIndex = renderable->GetGeometryShaderIndex()) < 0))
			{
				stageTable.vertexShader = GenerateVertexShader(&compileData, &allocData, &shaderStateFlags);
			}
			else
			{
				renderable->SetShaderArray(shaderData, kVertexAttribPosition0, kArrayPosition0);

				const GeometryAssembly *geometryAssembly = &GeometryShader::geometryAssembly[geometryShaderIndex];

				GeometryShader *geometryShader = GeometryShader::Find(geometryAssembly->signature);
				if (!geometryShader)
				{
					const char *source = geometryAssembly->shaderSource;
					geometryShader = GeometryShader::New(source, Text::GetTextLength(source), geometryAssembly->signature);
				}

				stageTable.geometryShader = geometryShader;

				int32 snippetCount = geometryAssembly->vertexSnippetCount;
				const VertexSnippet *const *snippet = geometryAssembly->vertexSnippet;

				VertexAssembly vertexAssembly(ShaderAttribute::signatureStorage);
				for (machine a = 0; a < snippetCount; a++)
				{
					vertexAssembly.AddSnippet(snippet[a]);
				}

				stageTable.vertexShader = VertexShader::Get(&vertexAssembly);

				ShaderStateProc *stateProc = geometryAssembly->stateProc;
				if (stateProc)
				{
					shaderData->AddStateProc(stateProc);
				}
			}

			ShaderProgram *shaderProgram = ShaderProgram::Get(stageTable);
			shaderData->shaderProgram = shaderProgram;

			stageTable.fragmentShader->Release();
			stageTable.vertexShader->Release();

			#if C4OPENGL

				BindShaderUniforms(type, shaderProgram);

			#endif

			GeometryShader *geometryShader = stageTable.geometryShader;
			if (!geometryShader)
			{
				BuildStateProcList(&compileData, shaderStateFlags);
			}
			else
			{
				geometryShader->Release();
			}

			compileData.shaderData->Preprocess();
		}

		delete[] processData;
	}

	return (result);
}

ShaderResult ShaderAttribute::TestShader(ShaderType type, const ShaderKey& key, const Renderable *renderable, const RenderSegment *renderSegment) const
{
	ShaderGraph				tempGraph;
	List<Process>			processList;
	List<Process>			readyList;
	ShaderCompileData		compileData;
	ShaderAllocationData	allocData;
	ShaderResult			result;

	ShaderData *shaderDataPtr = nullptr;
	ShaderData shaderData(key, &shaderDataPtr, kBlendReplace, 0);

	compileData.renderable = renderable;
	compileData.renderSegment = renderSegment;
	compileData.shaderData = &shaderData;

	compileData.shaderType = type;
	compileData.shaderVariant = key.GetShaderVariant();
	compileData.detailLevel = key.GetDetailLevel();
	compileData.shadowFlag = key.GetShadowFlag();

	compileData.shaderSourceFlags = 0;
	compileData.terrainViewDirection = nullptr;

	CloneShader(&shaderGraph, &tempGraph, true);

	if (type <= kShaderLastAmbient)
	{
		result = PrepareAmbientShader(&compileData, &tempGraph, &processList);
	}
	else if (type <= kShaderLastUnified)
	{
		result = PrepareUnifiedShader(&compileData, &tempGraph, &processList);
	}
	else if (type <= kShaderLastLight)
	{
		result = PrepareLightShader(&compileData, &tempGraph, &processList);
	}
	else
	{
		result = PreparePlainShader(type, &compileData, &tempGraph, &processList);
	}

	if (result == kShaderOkay)
	{
		OptimizeTextureMaps(&tempGraph);
		EliminateDeadCode(&tempGraph, &processList);
		OrganizeDerivedInterpolants(&compileData, &tempGraph);

		CalculatePathLengths(&tempGraph, &processList, &readyList);
		int32 processCount = ScheduleShader(&compileData, &readyList, &processList, signatureStorage);

		ProcessData *processData = new ProcessData[processCount];
		result = AllocateShaderResources(&compileData, &allocData, processCount, processData, &processList);
		delete[] processData;
	}

	return (result);
}

ShaderProgram *ShaderAttribute::CompilePostShader(const ShaderGraph *graph, VertexShader *vertexShader)
{
	List<Process>			processList;
	List<Process>			readyList;
	ShaderCompileData		compileData;
	ShaderAllocationData	allocData;

	compileData.renderable = nullptr;
	compileData.shaderData = nullptr;

	compileData.shaderType = kShaderNone;
	compileData.shaderVariant = kShaderVariantNormal;

	compileData.shaderSourceFlags = 0;
	compileData.terrainViewDirection = nullptr;

	Process *process = graph->GetFirstElement();
	while (process)
	{
		if (!process->GetFirstOutgoingEdge())
		{
			processList.Append(process);
		}

		process = process->GetNextElement();
	}

	CalculatePathLengths(graph, &processList, &readyList);
	int32 processCount = ScheduleShader(&compileData, &readyList, &processList, signatureStorage);

	ProcessData *processData = new ProcessData[processCount];

	ShaderProgram *shaderProgram = nullptr;
	if (AllocateShaderResources(&compileData, &allocData, processCount, processData, &processList) == kShaderOkay)
	{
		ProgramStageTable	stageTable;

		FragmentShader *fragmentShader = FragmentShader::Find(signatureStorage);
		if (!fragmentShader)
		{
			int32 length = GenerateShaderProlog(&compileData, &allocData, sourceStorage);
			char *string = sourceStorage + length;

			GenerateShaderCode(&compileData, &allocData, processData, &processList, string, &length);

			string += length;
			length = Text::CopyText(FragmentShader::epilogText, string);
			string[length] = 0;

			unsigned_int32 size = (unsigned_int32) (string + length - sourceStorage);
			fragmentShader = FragmentShader::New(sourceStorage, size, signatureStorage);
		}

		stageTable.vertexShader = vertexShader;
		stageTable.fragmentShader = fragmentShader;

		shaderProgram = ShaderProgram::Get(stageTable);
		fragmentShader->Release();

		#if C4OPENGL

			BindShaderUniforms(kShaderNone, shaderProgram);

		#endif
	}

	delete[] processData;
	processList.RemoveAll();
	return (shaderProgram);
}

Process *ShaderAttribute::BuildTextureCombiner(const MaterialObject *materialObject, ShaderGraph *graph, Process **textureMap1, Process **textureMap2, Process **textureCombiner, Process **vertexColor)
{
	if (*textureMap2)
	{
		Process		*combiner;

		TextureBlendMode mode = (materialObject) ? materialObject->GetTextureBlendMode() : kTextureBlendMultiply;
		switch (mode)
		{
			case kTextureBlendAdd:

				combiner = new AddProcess;
				new Route(*textureMap1, combiner, 0);
				new Route(*textureMap2, combiner, 1);
				break;

			case kTextureBlendAverage:

				combiner = new AverageProcess;
				new Route(*textureMap1, combiner, 0);
				new Route(*textureMap2, combiner, 1);
				break;

			case kTextureBlendMultiply:

				combiner = new MultiplyProcess;
				new Route(*textureMap1, combiner, 0);
				new Route(*textureMap2, combiner, 1);
				break;

			case kTextureBlendVertexAlpha:

				*vertexColor = new VertexColorProcess;
				graph->AddElement(*vertexColor);

				combiner = new LerpProcess;
				new Route(*textureMap1, combiner, 0);
				new Route(*textureMap2, combiner, 1);
				(new Route(*vertexColor, combiner, 2))->SetRouteSwizzle('aaaa');
				break;

			case kTextureBlendPrimaryAlpha:

				combiner = new LerpProcess;
				new Route(*textureMap1, combiner, 0);
				new Route(*textureMap2, combiner, 1);
				(new Route(*textureMap1, combiner, 2))->SetRouteSwizzle('aaaa');
				break;

			case kTextureBlendSecondaryAlpha:

				combiner = new LerpProcess;
				new Route(*textureMap1, combiner, 0);
				new Route(*textureMap2, combiner, 1);
				(new Route(*textureMap2, combiner, 2))->SetRouteSwizzle('aaaa');
				break;

			case kTextureBlendPrimaryInverseAlpha:

				combiner = new LerpProcess;
				new Route(*textureMap2, combiner, 0);
				new Route(*textureMap1, combiner, 1);
				(new Route(*textureMap1, combiner, 2))->SetRouteSwizzle('aaaa');
				break;

			case kTextureBlendSecondaryInverseAlpha:

				combiner = new LerpProcess;
				new Route(*textureMap2, combiner, 0);
				new Route(*textureMap1, combiner, 1);
				(new Route(*textureMap2, combiner, 2))->SetRouteSwizzle('aaaa');
				break;
		}

		*textureCombiner = combiner;
		graph->AddElement(combiner);
		return (combiner);
	}

	return (*textureMap1);
}

void ShaderAttribute::BuildRegularShaderGraph(const Renderable *renderable, const RenderSegment *renderSegment, const MaterialObject *materialObject, const List<Attribute> *attributeList, ShaderGraph *graph, Process **process)
{
	const Attribute				*firstAttribute[2];
	int32						textureTexcoordIndex[2];
	int32						normalTexcoordIndex[2];
	int32						specularTexcoordIndex;
	int32						emissionTexcoordIndex;
	int32						opacityTexcoordIndex;
	int32						horizonTexcoordIndex;
	const ReflectionAttribute	*reflectionAttribute;
	const RefractionAttribute	*refractionAttribute;
	Process						*finalAmbientProduct;
	Process						*finalLightProduct;

	unsigned_int32 materialFlags = (materialObject) ? materialObject->GetMaterialFlags() : 0;
	if (renderSegment)
	{
		materialFlags |= renderSegment->GetMaterialState();
	}

	firstAttribute[0] = (materialObject) ? materialObject->GetFirstAttribute() : nullptr;
	firstAttribute[1] = (attributeList) ? attributeList->First() : nullptr;

	for (machine a = 0; a < kShaderGraphProcessCount; a++)
	{
		process[a] = nullptr;
	}

	TextureSemantic textureAlphaSemantic = kTextureSemanticNone;
	TextureSemantic normalAlphaSemantic = kTextureSemanticNone;
	const EnvironmentTextureAttribute *environmentTextureAttribute = nullptr;

	for (machine a = 0; a < 2; a++)
	{
		const Attribute *attribute = firstAttribute[a];
		while (attribute)
		{
			const Attribute *next = attribute->Next();
			for (;;)
			{
				AttributeType type = attribute->GetAttributeType();
				switch (type)
				{
					case kAttributeReference:
					{
						attribute = static_cast<const ReferenceAttribute *>(attribute)->GetReference();
						if (attribute)
						{
							continue;
						}

						break;
					}

					case kAttributeDiffuse:
					{
						if (!process[kShaderGraphDiffuseColor])
						{
							process[kShaderGraphDiffuseColor] = new ColorProcess;
							graph->AddElement(process[kShaderGraphDiffuseColor]);
						}

						ColorProcess *colorProcess = static_cast<ColorProcess *>(process[kShaderGraphDiffuseColor]);
						const DiffuseAttribute *diffuseAttribute = static_cast<const DiffuseAttribute *>(attribute);
						const ColorRGBA& diffuseColor = diffuseAttribute->GetDiffuseColor();

						colorProcess->SetColorValue(diffuseColor);
						if (diffuseAttribute->GetAttributeFlags() & kAttributeMutable)
						{
							colorProcess->SetParameterSlot(0);
							colorProcess->SetParameterData(&diffuseColor.red);
						}

						break;
					}

					case kAttributeSpecular:
					{
						if (!process[kShaderGraphMicrofacet])
						{
							const SpecularAttribute *specularAttribute = static_cast<const SpecularAttribute *>(attribute);
							const ColorRGBA& specularColor = specularAttribute->GetSpecularColor();
							const float& specularExponent = specularAttribute->GetSpecularExponent();

							if (!process[kShaderGraphSpecularExponent])
							{
								if ((specularAttribute->GetAttributeFlags() & kAttributeMutable) || (specularColor.red != 1.0F) || (specularColor.green != 1.0F) || (specularColor.blue != 1.0F))
								{
									process[kShaderGraphSpecularColor] = new ColorProcess;
									graph->AddElement(process[kShaderGraphSpecularColor]);
								}
								else
								{
									delete process[kShaderGraphSpecularColor];
									process[kShaderGraphSpecularColor] = nullptr;
								}

								process[kShaderGraphSpecularExponent] = new ScalarProcess;
								graph->AddElement(process[kShaderGraphSpecularExponent]);
							}

							ScalarProcess *exponentProcess = static_cast<ScalarProcess *>(process[kShaderGraphSpecularExponent]);
							exponentProcess->SetScalarValue(specularExponent);

							ColorProcess *colorProcess = static_cast<ColorProcess *>(process[kShaderGraphSpecularColor]);
							if (colorProcess)
							{
								colorProcess->SetColorValue(specularColor);

								if (specularAttribute->GetAttributeFlags() & kAttributeMutable)
								{
									colorProcess->SetParameterSlot(1);
									colorProcess->SetParameterData(&specularColor.red);

									exponentProcess->SetParameterSlot(2);
									exponentProcess->SetParameterData(&specularExponent);
								}
							}
						}

						break;
					}

					case kAttributeMicrofacet:
					{
						delete process[kShaderGraphSpecularColor];
						process[kShaderGraphSpecularColor] = nullptr;

						delete process[kShaderGraphSpecularExponent];
						process[kShaderGraphSpecularExponent] = nullptr;

						const MicrofacetAttribute *microfacetAttribute = static_cast<const MicrofacetAttribute *>(attribute);

						if (!process[kShaderGraphMicrofacet])
						{
							process[kShaderGraphMicrofacet] = new MicrofacetProcess;
							graph->AddElement(process[kShaderGraphMicrofacet]);

							delete process[kShaderGraphMicrofacetReflectivity];
							process[kShaderGraphMicrofacetReflectivity] = nullptr;

							float reflectivity = microfacetAttribute->GetMicrofacetReflectivity();
							if (reflectivity < 1.0F)
							{
								process[kShaderGraphMicrofacetReflectivity] = new ScalarProcess;
								graph->AddElement(process[kShaderGraphMicrofacetReflectivity]);

								static_cast<ScalarProcess *>(process[kShaderGraphMicrofacetReflectivity])->SetScalarValue(reflectivity);
							}
						}

						MicrofacetProcess *microfacetProcess = static_cast<MicrofacetProcess *>(process[kShaderGraphMicrofacet]);
						const MicrofacetAttribute::MicrofacetParams *params = microfacetAttribute->GetMicrofacetParams();

						microfacetProcess->SetMicrofacetParams(params);
						if (renderable)
						{
							microfacetProcess->SetMicrofacetData(params);
						}

						break;
					};

					case kAttributeEmission:
					{
						if (!process[kShaderGraphEmissionColor])
						{
							process[kShaderGraphEmissionColor] = new ColorProcess;
							graph->AddElement(process[kShaderGraphEmissionColor]);
						}

						ColorProcess *colorProcess = static_cast<ColorProcess *>(process[kShaderGraphEmissionColor]);
						const EmissionAttribute *emissionAttribute = static_cast<const EmissionAttribute *>(attribute);
						const ColorRGBA& emissionColor = emissionAttribute->GetEmissionColor();

						colorProcess->SetColorValue(emissionColor);
						if (emissionAttribute->GetAttributeFlags() & kAttributeMutable)
						{
							colorProcess->SetParameterSlot(3);
							colorProcess->SetParameterData(&emissionColor.red);
						}

						break;
					}

					case kAttributeReflection:
					{
						if (!process[kShaderGraphReflectionColor])
						{
							process[kShaderGraphReflectionColor] = new ColorProcess;
							graph->AddElement(process[kShaderGraphReflectionColor]);
						}

						ColorProcess *colorProcess = static_cast<ColorProcess *>(process[kShaderGraphReflectionColor]);
						reflectionAttribute = static_cast<const ReflectionAttribute *>(attribute);
						const ColorRGBA& reflectionColor = reflectionAttribute->GetReflectionColor();

						colorProcess->SetColorValue(reflectionColor);
						if (reflectionAttribute->GetAttributeFlags() & kAttributeMutable)
						{
							colorProcess->SetParameterSlot(4);
							colorProcess->SetParameterData(&reflectionColor.red);
						}

						break;
					}

					case kAttributeRefraction:
					{
						if (!process[kShaderGraphRefractionColor])
						{
							process[kShaderGraphRefractionColor] = new ColorProcess;
							graph->AddElement(process[kShaderGraphRefractionColor]);
						}

						ColorProcess *colorProcess = static_cast<ColorProcess *>(process[kShaderGraphRefractionColor]);
						refractionAttribute = static_cast<const RefractionAttribute *>(attribute);
						const ColorRGBA& refractionColor = refractionAttribute->GetRefractionColor();

						colorProcess->SetColorValue(refractionColor);
						if (refractionAttribute->GetAttributeFlags() & kAttributeMutable)
						{
							colorProcess->SetParameterSlot(5);
							colorProcess->SetParameterData(&refractionColor.red);
						}

						break;
					}

					case kAttributeEnvironment:
					{
						if (!process[kShaderGraphEnvironmentColor])
						{
							process[kShaderGraphEnvironmentColor] = new ColorProcess;
							graph->AddElement(process[kShaderGraphEnvironmentColor]);
						}

						ColorProcess *colorProcess = static_cast<ColorProcess *>(process[kShaderGraphEnvironmentColor]);
						const EnvironmentAttribute *environmentAttribute = static_cast<const EnvironmentAttribute *>(attribute);
						const ColorRGBA& environmentColor = environmentAttribute->GetEnvironmentColor();

						colorProcess->SetColorValue(environmentColor);
						if (environmentAttribute->GetAttributeFlags() & kAttributeMutable)
						{
							colorProcess->SetParameterSlot(6);
							colorProcess->SetParameterData(&environmentColor.red);
						}

						break;
					}

					case kAttributeDiffuseTexture:
					case kAttributeSpecularTexture:
					case kAttributeEmissionTexture:
					case kAttributeNormalTexture:
					case kAttributeOpacityTexture:
					case kAttributeHorizonTexture:
					{
						const TextureAttribute *textureAttribute = static_cast<const TextureAttribute *>(attribute);

						int32 texcoordIndex = textureAttribute->GetTexcoordIndex();
						Process *texcoordProcess = process[kShaderGraphTexcoord1 + texcoordIndex];
						if (!texcoordProcess)
						{
							if (texcoordIndex == 0)
							{
								const Texture *texture = textureAttribute->GetTexture();
								if ((!texture) || (!renderable) || (TextureMapProcess::GetTexcoordSize(texture) == 2))
								{
									texcoordProcess = new Texcoord0Process;
								}
								else
								{
									texcoordProcess = new RawTexcoordProcess;
								}
							}
							else
							{
								texcoordProcess = new Texcoord1Process;
							}

							process[kShaderGraphTexcoord1 + texcoordIndex] = texcoordProcess;
							graph->AddElement(texcoordProcess);
						}

						if (type == kAttributeDiffuseTexture)
						{
							if (!process[kShaderGraphTextureMap1])
							{
								process[kShaderGraphTextureMap1] = new TextureMapProcess;
								graph->AddElement(process[kShaderGraphTextureMap1]);

								TextureMapProcess *textureMapProcess = static_cast<TextureMapProcess *>(process[kShaderGraphTextureMap1]);

								const char *name = textureAttribute->GetTextureName();
								if (name[0] != 0)
								{
									textureMapProcess->SetTexture(name);
								}
								else
								{
									textureMapProcess->SetTexture(textureAttribute->GetTexture());
								}

								textureAlphaSemantic = textureMapProcess->GetTexture()->GetAlphaSemantic();
								textureTexcoordIndex[0] = texcoordIndex;
							}
							else
							{
								if (!process[kShaderGraphTextureMap2])
								{
									process[kShaderGraphTextureMap2] = new TextureMapProcess;
									graph->AddElement(process[kShaderGraphTextureMap2]);
								}

								TextureMapProcess *textureMapProcess = static_cast<TextureMapProcess *>(process[kShaderGraphTextureMap2]);

								const char *name = textureAttribute->GetTextureName();
								if (name[0] != 0)
								{
									textureMapProcess->SetTexture(name);
								}
								else
								{
									textureMapProcess->SetTexture(textureAttribute->GetTexture());
								}

								textureTexcoordIndex[1] = texcoordIndex;
							}
						}
						else if (type == kAttributeSpecularTexture)
						{
							if (!process[kShaderGraphSpecularMap])
							{
								process[kShaderGraphSpecularMap] = new TextureMapProcess;
								graph->AddElement(process[kShaderGraphSpecularMap]);
							}

							static_cast<TextureMapProcess *>(process[kShaderGraphSpecularMap])->SetTexture(textureAttribute->GetTextureName());
							specularTexcoordIndex = texcoordIndex;
						}
						else if (type == kAttributeEmissionTexture)
						{
							if (!process[kShaderGraphEmissionMap])
							{
								process[kShaderGraphEmissionMap] = new TextureMapProcess;
								graph->AddElement(process[kShaderGraphEmissionMap]);
							}

							TextureMapProcess *textureMapProcess = static_cast<TextureMapProcess *>(process[kShaderGraphEmissionMap]);

							const char *name = textureAttribute->GetTextureName();
							if (name[0] != 0)
							{
								textureMapProcess->SetTexture(name);
							}
							else
							{
								textureMapProcess->SetTexture(textureAttribute->GetTexture());
							}

							emissionTexcoordIndex = texcoordIndex;
						}
						else if (type == kAttributeNormalTexture)
						{
							if (!process[kShaderGraphNormalMap1])
							{
								process[kShaderGraphNormalMap1] = new NormalMapProcess;
								graph->AddElement(process[kShaderGraphNormalMap1]);

								NormalMapProcess *normalMapProcess = static_cast<NormalMapProcess *>(process[kShaderGraphNormalMap1]);
								normalMapProcess->SetTexture(textureAttribute->GetTextureName());

								normalAlphaSemantic = normalMapProcess->GetTexture()->GetAlphaSemantic();
								normalTexcoordIndex[0] = texcoordIndex;

								if (normalAlphaSemantic == kTextureSemanticParallax)
								{
									process[kShaderGraphParallax] = new ParallaxProcess;
									graph->AddElement(process[kShaderGraphParallax]);

									static_cast<TextureMapProcess *>(process[kShaderGraphParallax])->SetTexture(textureAttribute->GetTextureName());
								}
							}
							else
							{
								delete process[kShaderGraphParallax];
								process[kShaderGraphParallax] = nullptr;

								if (!process[kShaderGraphNormalMap2])
								{
									process[kShaderGraphNormalMap2] = new NormalMapProcess;
									graph->AddElement(process[kShaderGraphNormalMap2]);
								}

								static_cast<TextureMapProcess *>(process[kShaderGraphNormalMap2])->SetTexture(textureAttribute->GetTextureName());
								normalTexcoordIndex[1] = texcoordIndex;
							}
						}
						else if (type == kAttributeOpacityTexture)
						{
							if (!process[kShaderGraphOpacityMap])
							{
								process[kShaderGraphOpacityMap] = new TextureMapProcess;
								graph->AddElement(process[kShaderGraphOpacityMap]);
							}

							static_cast<TextureMapProcess *>(process[kShaderGraphOpacityMap])->SetTexture(textureAttribute->GetTextureName());
							opacityTexcoordIndex = texcoordIndex;
						}
						else
						{
							if (!process[kShaderGraphHorizon])
							{
								process[kShaderGraphHorizon] = new HorizonProcess;
								graph->AddElement(process[kShaderGraphHorizon]);
							}

							HorizonProcess *horizonProcess = static_cast<HorizonProcess *>(process[kShaderGraphHorizon]);
							horizonProcess->SetTexture(textureAttribute->GetTextureName());
							horizonProcess->SetHorizonFlags(static_cast<const HorizonTextureAttribute *>(attribute)->GetHorizonFlags());
							horizonTexcoordIndex = texcoordIndex;
						}

						break;
					}

					case kAttributeEnvironmentTexture:

						environmentTextureAttribute = static_cast<const EnvironmentTextureAttribute *>(attribute);
						break;

					case kAttributeDeltaDepth:
					{
						if (!process[kShaderGraphDeltaDepth])
						{
							process[kShaderGraphDeltaDepth] = new DeltaDepthProcess;
							graph->AddElement(process[kShaderGraphDeltaDepth]);
						}

						DeltaDepthProcess *deltaDepthProcess = static_cast<DeltaDepthProcess *>(process[kShaderGraphDeltaDepth]);
						const DeltaDepthAttribute *deltaDepthAttribute = static_cast<const DeltaDepthAttribute *>(attribute);
						deltaDepthProcess->SetDeltaScale(deltaDepthAttribute->GetDeltaScale());
						break;
					}

					case kAttributeDepthRamp:
					{
						if (!process[kShaderGraphDepthLinearRamp])
						{
							process[kShaderGraphDepthLinearRamp] = new LinearRampProcess;
							graph->AddElement(process[kShaderGraphDepthLinearRamp]);

							process[kShaderGraphFragmentDepth] = new FragmentDepthProcess;
							graph->AddElement(process[kShaderGraphFragmentDepth]);

							new Route(process[kShaderGraphFragmentDepth], process[kShaderGraphDepthLinearRamp], 0);
						}

						LinearRampProcess *linearRampProcess = static_cast<LinearRampProcess *>(process[kShaderGraphDepthLinearRamp]);
						const Range<float>& rampRange = static_cast<const DepthRampAttribute *>(attribute)->GetRampRange();
						linearRampProcess->SetRampCenter((rampRange.min + rampRange.max) * 0.5F);
						linearRampProcess->SetRampWidth(rampRange.max - rampRange.min);
						break;
					}
				}

				break;
			}

			attribute = next;
		}
	}

	if (process[kShaderGraphParallax])
	{
		int32 parallaxTexcoordIndex = normalTexcoordIndex[0];

		new Route(process[kShaderGraphTexcoord1 + parallaxTexcoordIndex], process[kShaderGraphParallax], 0);
		new Route(process[kShaderGraphParallax], process[kShaderGraphNormalMap1], 0);

		if (process[kShaderGraphTextureMap1])
		{
			int32 texcoordIndex = textureTexcoordIndex[0];
			if (texcoordIndex == parallaxTexcoordIndex)
			{
				new Route(process[kShaderGraphParallax], process[kShaderGraphTextureMap1], 0);
			}
			else
			{
				new Route(process[kShaderGraphTexcoord1 + texcoordIndex], process[kShaderGraphTextureMap1], 0);
			}

			if (process[kShaderGraphTextureMap2])
			{
				texcoordIndex = textureTexcoordIndex[1];
				if (texcoordIndex == parallaxTexcoordIndex)
				{
					new Route(process[kShaderGraphParallax], process[kShaderGraphTextureMap2], 0);
				}
				else
				{
					new Route(process[kShaderGraphTexcoord1 + texcoordIndex], process[kShaderGraphTextureMap2], 0);
				}
			}
		}

		if (process[kShaderGraphSpecularMap])
		{
			int32 texcoordIndex = specularTexcoordIndex;
			if (texcoordIndex == parallaxTexcoordIndex)
			{
				new Route(process[kShaderGraphParallax], process[kShaderGraphSpecularMap], 0);
			}
			else
			{
				new Route(process[kShaderGraphTexcoord1 + texcoordIndex], process[kShaderGraphSpecularMap], 0);
			}
		}

		if (process[kShaderGraphEmissionMap])
		{
			int32 texcoordIndex = emissionTexcoordIndex;
			if (texcoordIndex == parallaxTexcoordIndex)
			{
				new Route(process[kShaderGraphParallax], process[kShaderGraphEmissionMap], 0);
			}
			else
			{
				new Route(process[kShaderGraphTexcoord1 + texcoordIndex], process[kShaderGraphEmissionMap], 0);
			}
		}

		if (process[kShaderGraphOpacityMap])
		{
			int32 texcoordIndex = opacityTexcoordIndex;
			if (texcoordIndex == parallaxTexcoordIndex)
			{
				new Route(process[kShaderGraphParallax], process[kShaderGraphOpacityMap], 0);
			}
			else
			{
				new Route(process[kShaderGraphTexcoord1 + texcoordIndex], process[kShaderGraphOpacityMap], 0);
			}
		}

		if (process[kShaderGraphHorizon])
		{
			int32 texcoordIndex = horizonTexcoordIndex;
			if (texcoordIndex == parallaxTexcoordIndex)
			{
				new Route(process[kShaderGraphParallax], process[kShaderGraphHorizon], 0);
			}
			else
			{
				new Route(process[kShaderGraphTexcoord1 + texcoordIndex], process[kShaderGraphHorizon], 0);
			}
		}
	}
	else
	{
		if (process[kShaderGraphTextureMap1])
		{
			new Route(process[kShaderGraphTexcoord1 + textureTexcoordIndex[0]], process[kShaderGraphTextureMap1], 0);
			if (process[kShaderGraphTextureMap2])
			{
				new Route(process[kShaderGraphTexcoord1 + textureTexcoordIndex[1]], process[kShaderGraphTextureMap2], 0);
			}
		}

		if (process[kShaderGraphNormalMap1])
		{
			new Route(process[kShaderGraphTexcoord1 + normalTexcoordIndex[0]], process[kShaderGraphNormalMap1], 0);
			if (process[kShaderGraphNormalMap2])
			{
				new Route(process[kShaderGraphTexcoord1 + normalTexcoordIndex[1]], process[kShaderGraphNormalMap2], 0);
			}
		}

		if (process[kShaderGraphSpecularMap])
		{
			new Route(process[kShaderGraphTexcoord1 + specularTexcoordIndex], process[kShaderGraphSpecularMap], 0);
		}

		if (process[kShaderGraphEmissionMap])
		{
			new Route(process[kShaderGraphTexcoord1 + emissionTexcoordIndex], process[kShaderGraphEmissionMap], 0);
		}

		if (process[kShaderGraphOpacityMap])
		{
			new Route(process[kShaderGraphTexcoord1 + opacityTexcoordIndex], process[kShaderGraphOpacityMap], 0);
		}

		if (process[kShaderGraphHorizon])
		{
			new Route(process[kShaderGraphTexcoord1 + horizonTexcoordIndex], process[kShaderGraphHorizon], 0);
		}
	}

	Process *finalTextureMap = BuildTextureCombiner(materialObject, graph, &process[kShaderGraphTextureMap1], &process[kShaderGraphTextureMap2], &process[kShaderGraphTextureCombiner], &process[kShaderGraphVertexColor]);

	Process *finalNormalMap = process[kShaderGraphNormalMap1];
	if (process[kShaderGraphNormalMap2])
	{
		process[kShaderGraphNormalCombiner] = new CombineNormalsProcess;
		graph->AddElement(process[kShaderGraphNormalCombiner]);
		finalNormalMap = process[kShaderGraphNormalCombiner];

		new Route(process[kShaderGraphNormalMap1], process[kShaderGraphNormalCombiner], 0);
		new Route(process[kShaderGraphNormalMap2], process[kShaderGraphNormalCombiner], 1);
	}

	bool colorArrayEnabled = false;
	if (renderable)
	{
		colorArrayEnabled = renderable->AttributeArrayEnabled(kArrayColor0);
		if (colorArrayEnabled)
		{
			if (!(renderable->GetShaderFlags() & kShaderColorArrayInhibit))
			{
				if (!process[kShaderGraphVertexColor])
				{
					process[kShaderGraphVertexColor] = new VertexColorProcess;
					graph->AddElement(process[kShaderGraphVertexColor]);
				}
			}
			else
			{
				colorArrayEnabled = false;
			}
		}
	}

	if ((materialFlags & kMaterialAlphaSemanticInhibit) && (textureAlphaSemantic != kTextureSemanticNone))
	{
		textureAlphaSemantic = kTextureSemanticTransparency;
	}

	Process *finalDiffuseProduct = finalTextureMap;
	if (finalDiffuseProduct)
	{
		if (process[kShaderGraphDiffuseColor])
		{
			process[kShaderGraphDiffuseMultiply1] = new MultiplyProcess;
			graph->AddElement(process[kShaderGraphDiffuseMultiply1]);
			finalDiffuseProduct = process[kShaderGraphDiffuseMultiply1];

			if (colorArrayEnabled)
			{
				process[kShaderGraphColorMultiply] = new MultiplyProcess;
				graph->AddElement(process[kShaderGraphColorMultiply]);

				new Route(process[kShaderGraphDiffuseColor], process[kShaderGraphColorMultiply], 0);
				new Route(process[kShaderGraphVertexColor], process[kShaderGraphColorMultiply], 1);

				new Route(process[kShaderGraphColorMultiply], process[kShaderGraphDiffuseMultiply1], 0);
				new Route(finalTextureMap, process[kShaderGraphDiffuseMultiply1], 1);
			}
			else
			{
				new Route(process[kShaderGraphDiffuseColor], process[kShaderGraphDiffuseMultiply1], 0);
				new Route(finalTextureMap, process[kShaderGraphDiffuseMultiply1], 1);
			}
		}
		else if (colorArrayEnabled)
		{
			process[kShaderGraphDiffuseMultiply1] = new MultiplyProcess;
			graph->AddElement(process[kShaderGraphDiffuseMultiply1]);
			finalDiffuseProduct = process[kShaderGraphDiffuseMultiply1];

			new Route(process[kShaderGraphVertexColor], process[kShaderGraphDiffuseMultiply1], 0);
			new Route(finalTextureMap, process[kShaderGraphDiffuseMultiply1], 1);
		}

		if (textureAlphaSemantic == kTextureSemanticOcclusion)
		{
			process[kShaderGraphOcclusionMultiply] = new MultiplyProcess;
			graph->AddElement(process[kShaderGraphOcclusionMultiply]);

			new Route(finalDiffuseProduct, process[kShaderGraphOcclusionMultiply], 0);
			(new Route(process[kShaderGraphTextureMap1], process[kShaderGraphOcclusionMultiply], 1))->SetRouteSwizzle('aaaa');

			finalAmbientProduct = process[kShaderGraphOcclusionMultiply];
		}
		else
		{
			finalAmbientProduct = finalDiffuseProduct;
		}
	}
	else
	{
		if (colorArrayEnabled)
		{
			if (process[kShaderGraphDiffuseColor])
			{
				process[kShaderGraphColorMultiply] = new MultiplyProcess;
				graph->AddElement(process[kShaderGraphColorMultiply]);
				finalDiffuseProduct = process[kShaderGraphColorMultiply];

				new Route(process[kShaderGraphDiffuseColor], process[kShaderGraphColorMultiply], 0);
				new Route(process[kShaderGraphVertexColor], process[kShaderGraphColorMultiply], 1);
			}
			else
			{
				finalDiffuseProduct = process[kShaderGraphVertexColor];
			}
		}
		else if (process[kShaderGraphDiffuseColor])
		{
			finalDiffuseProduct = process[kShaderGraphDiffuseColor];
		}
		else
		{
			process[kShaderGraphDiffuseColor] = new ColorProcess;
			graph->AddElement(process[kShaderGraphDiffuseColor]);
			finalDiffuseProduct = process[kShaderGraphDiffuseColor];
		}

		finalAmbientProduct = finalDiffuseProduct;
	}

	unsigned_int32 emissionSwizzle = 'xyzw';
	Process *finalEmissionProduct = process[kShaderGraphEmissionMap];
	if (finalEmissionProduct)
	{
		if (process[kShaderGraphEmissionColor])
		{
			process[kShaderGraphEmissionMultiply] = new MultiplyProcess;
			graph->AddElement(process[kShaderGraphEmissionMultiply]);
			finalEmissionProduct = process[kShaderGraphEmissionMultiply];

			new Route(process[kShaderGraphEmissionColor], process[kShaderGraphEmissionMultiply], 0);
			new Route(process[kShaderGraphEmissionMap], process[kShaderGraphEmissionMultiply], 1);
		}
	}
	else if (textureAlphaSemantic == kTextureSemanticEmission)
	{
		if (process[kShaderGraphEmissionColor])
		{
			process[kShaderGraphEmissionMultiply] = new MultiplyProcess;
			graph->AddElement(process[kShaderGraphEmissionMultiply]);
			finalEmissionProduct = process[kShaderGraphEmissionMultiply];

			(new Route(process[kShaderGraphTextureMap1], process[kShaderGraphEmissionMultiply], 0))->SetRouteSwizzle('aaaa');
			new Route(process[kShaderGraphEmissionColor], process[kShaderGraphEmissionMultiply], 1);
		}
		else
		{
			finalEmissionProduct = process[kShaderGraphTextureMap1];
			emissionSwizzle = 'aaaa';
		}
	}
	else
	{
		finalEmissionProduct = process[kShaderGraphEmissionColor];
	}

	Process *finalEnvironmentProduct = process[kShaderGraphEnvironmentColor];
	if (finalEnvironmentProduct)
	{
		if ((process[kShaderGraphSpecularMap]) || (textureAlphaSemantic == kTextureSemanticSpecular))
		{
			process[kShaderGraphEnvironmentMultiply] = new MultiplyProcess;
			graph->AddElement(process[kShaderGraphEnvironmentMultiply]);
			finalEnvironmentProduct = process[kShaderGraphEnvironmentMultiply];

			if ((process[kShaderGraphSpecularMap]))
			{
				new Route(process[kShaderGraphSpecularMap], process[kShaderGraphEnvironmentMultiply], 0);
			}
			else
			{
				(new Route(process[kShaderGraphTextureMap1], process[kShaderGraphEnvironmentMultiply], 0))->SetRouteSwizzle('aaaa');
			}

			new Route(process[kShaderGraphEnvironmentColor], process[kShaderGraphEnvironmentMultiply], 1);
		}
	}

	Process *finalRefractionProduct = process[kShaderGraphRefractionColor];
	if (finalRefractionProduct)
	{
		if ((process[kShaderGraphOpacityMap]) || (textureAlphaSemantic == kTextureSemanticOpacity))
		{
			process[kShaderGraphOpacityMultiply] = new MultiplyProcess;
			graph->AddElement(process[kShaderGraphOpacityMultiply]);

			new Route(finalAmbientProduct, process[kShaderGraphOpacityMultiply], 0);
			if (process[kShaderGraphOpacityMap])
			{
				new Route(process[kShaderGraphOpacityMap], process[kShaderGraphOpacityMultiply], 1);
			}
			else
			{
				(new Route(process[kShaderGraphTextureMap1], process[kShaderGraphOpacityMultiply], 1))->SetRouteSwizzle('aaaa');
			}

			finalAmbientProduct = process[kShaderGraphOpacityMultiply];

			process[kShaderGraphOpacityInvert] = new InvertProcess;
			graph->AddElement(process[kShaderGraphOpacityInvert]);

			if (process[kShaderGraphOpacityMap])
			{
				new Route(process[kShaderGraphOpacityMap], process[kShaderGraphOpacityInvert], 0);
			}
			else
			{
				(new Route(process[kShaderGraphTextureMap1], process[kShaderGraphOpacityInvert], 0))->SetRouteSwizzle('aaaa');
			}

			process[kShaderGraphRefractionMultiply] = new MultiplyProcess;
			graph->AddElement(process[kShaderGraphRefractionMultiply]);
			finalRefractionProduct = process[kShaderGraphRefractionMultiply];

			new Route(process[kShaderGraphRefractionColor], process[kShaderGraphRefractionMultiply], 0);
			new Route(process[kShaderGraphOpacityInvert], process[kShaderGraphRefractionMultiply], 1);
		}
	}

	Process *finalSpecularTerm = nullptr;
	bool finalSpecularScalar = false;

	if ((!renderable) || (!(renderable->GetShaderFlags() & kShaderVertexBillboard)))
	{
		process[kShaderGraphDiffuseReflection] = new DiffuseProcess;
		graph->AddElement(process[kShaderGraphDiffuseReflection]);

		if (finalNormalMap)
		{
			new Route(finalNormalMap, process[kShaderGraphDiffuseReflection], 0);
		}

		finalSpecularTerm = process[kShaderGraphMicrofacet];
		if (finalSpecularTerm)
		{
			if (finalNormalMap)
			{
				new Route(finalNormalMap, process[kShaderGraphMicrofacet], 0);
			}

			if ((process[kShaderGraphSpecularMap]) || (textureAlphaSemantic == kTextureSemanticSpecular) || (normalAlphaSemantic == kTextureSemanticSpecular))
			{
				process[kShaderGraphSpecularMultiply2] = new MultiplyProcess;
				graph->AddElement(process[kShaderGraphSpecularMultiply2]);
				finalSpecularTerm = process[kShaderGraphSpecularMultiply2];

				if (process[kShaderGraphMicrofacetReflectivity])
				{
					process[kShaderGraphSpecularMultiply1] = new MultiplyProcess;
					graph->AddElement(process[kShaderGraphSpecularMultiply1]);

					new Route(process[kShaderGraphMicrofacetReflectivity], process[kShaderGraphSpecularMultiply1], 0);

					if (process[kShaderGraphSpecularMap])
					{
						new Route(process[kShaderGraphSpecularMap], process[kShaderGraphSpecularMultiply1], 1);
					}
					else if (textureAlphaSemantic == kTextureSemanticSpecular)
					{
						(new Route(process[kShaderGraphTextureMap1], process[kShaderGraphSpecularMultiply1], 1))->SetRouteSwizzle('aaaa');
					}
					else
					{
						(new Route(process[kShaderGraphNormalMap1], process[kShaderGraphSpecularMultiply1], 1))->SetRouteSwizzle('aaaa');
					}

					new Route(process[kShaderGraphMicrofacet], process[kShaderGraphSpecularMultiply2], 0);
					new Route(process[kShaderGraphSpecularMultiply1], process[kShaderGraphSpecularMultiply2], 1);
				}
				else
				{
					new Route(process[kShaderGraphMicrofacet], process[kShaderGraphSpecularMultiply2], 0);

					if (process[kShaderGraphSpecularMap])
					{
						new Route(process[kShaderGraphSpecularMap], process[kShaderGraphSpecularMultiply2], 1);
					}
					else if (textureAlphaSemantic == kTextureSemanticSpecular)
					{
						(new Route(process[kShaderGraphTextureMap1], process[kShaderGraphSpecularMultiply2], 1))->SetRouteSwizzle('aaaa');
					}
					else
					{
						(new Route(process[kShaderGraphNormalMap1], process[kShaderGraphSpecularMultiply2], 1))->SetRouteSwizzle('aaaa');
					}
				}
			}
			else if (process[kShaderGraphMicrofacetReflectivity])
			{
				process[kShaderGraphSpecularMultiply2] = new MultiplyProcess;
				graph->AddElement(process[kShaderGraphSpecularMultiply2]);
				finalSpecularTerm = process[kShaderGraphSpecularMultiply2];

				new Route(process[kShaderGraphMicrofacet], process[kShaderGraphSpecularMultiply2], 0);
				new Route(process[kShaderGraphMicrofacetReflectivity], process[kShaderGraphSpecularMultiply2], 1);
			}
		}
		else if (process[kShaderGraphSpecularExponent])
		{
			process[kShaderGraphSpecularReflection] = new SpecularProcess;
			graph->AddElement(process[kShaderGraphSpecularReflection]);
			finalSpecularTerm = process[kShaderGraphSpecularReflection];

			if (finalNormalMap)
			{
				new Route(finalNormalMap, process[kShaderGraphSpecularReflection], 0);
			}

			new Route(process[kShaderGraphSpecularExponent], process[kShaderGraphSpecularReflection], 1);

			if ((process[kShaderGraphSpecularMap]) || (textureAlphaSemantic == kTextureSemanticSpecular) || (normalAlphaSemantic == kTextureSemanticSpecular))
			{
				process[kShaderGraphSpecularMultiply2] = new MultiplyProcess;
				graph->AddElement(process[kShaderGraphSpecularMultiply2]);
				finalSpecularTerm = process[kShaderGraphSpecularMultiply2];

				new Route(process[kShaderGraphSpecularReflection], process[kShaderGraphSpecularMultiply2], 0);

				Process *specularMultiply = process[kShaderGraphSpecularMultiply2];
				if (process[kShaderGraphSpecularColor])
				{
					process[kShaderGraphSpecularMultiply1] = new MultiplyProcess;
					graph->AddElement(process[kShaderGraphSpecularMultiply1]);
					specularMultiply = process[kShaderGraphSpecularMultiply1];

					new Route(process[kShaderGraphSpecularColor], process[kShaderGraphSpecularMultiply1], 0);
					new Route(process[kShaderGraphSpecularMultiply1], process[kShaderGraphSpecularMultiply2], 1);
				}

				if (process[kShaderGraphSpecularMap])
				{
					new Route(process[kShaderGraphSpecularMap], specularMultiply, 1);
				}
				else if (textureAlphaSemantic == kTextureSemanticSpecular)
				{
					finalSpecularScalar = true;
					(new Route(process[kShaderGraphTextureMap1], specularMultiply, 1))->SetRouteSwizzle('aaaa');
				}
				else
				{
					finalSpecularScalar = true;
					(new Route(process[kShaderGraphNormalMap1], specularMultiply, 1))->SetRouteSwizzle('aaaa');
				}
			}
			else if (process[kShaderGraphSpecularColor])
			{
				process[kShaderGraphSpecularMultiply2] = new MultiplyProcess;
				graph->AddElement(process[kShaderGraphSpecularMultiply2]);

				new Route(process[kShaderGraphSpecularReflection], process[kShaderGraphSpecularMultiply2], 0);
				new Route(process[kShaderGraphSpecularColor], process[kShaderGraphSpecularMultiply2], 1);

				finalSpecularTerm = process[kShaderGraphSpecularMultiply2];
			}
		}
	}

	if (finalSpecularTerm)
	{
		process[kShaderGraphLightSum] = new MultiplyAddProcess;
		graph->AddElement(process[kShaderGraphLightSum]);
		finalLightProduct = process[kShaderGraphLightSum];

		new Route(finalDiffuseProduct, process[kShaderGraphLightSum], 0);
		new Route(process[kShaderGraphDiffuseReflection], process[kShaderGraphLightSum], 1);
		new Route(finalSpecularTerm, process[kShaderGraphLightSum], 2);

		if (materialFlags & kMaterialSpecularBloom)
		{
			process[kShaderGraphBloomOutput] = new BloomOutputProcess;
			graph->AddElement(process[kShaderGraphBloomOutput]);

			if (finalSpecularScalar)
			{
				new Route(finalSpecularTerm, process[kShaderGraphBloomOutput], 0);
			}
			else
			{
				process[kShaderGraphBloom1] = new MaximumProcess;
				process[kShaderGraphBloom2] = new MaximumProcess;
				graph->AddElement(process[kShaderGraphBloom1]);
				graph->AddElement(process[kShaderGraphBloom2]);

				(new Route(finalSpecularTerm, process[kShaderGraphBloom1], 0))->SetRouteSwizzle('xxxx');
				(new Route(finalSpecularTerm, process[kShaderGraphBloom1], 1))->SetRouteSwizzle('yyyy');

				(new Route(finalSpecularTerm, process[kShaderGraphBloom2], 0))->SetRouteSwizzle('zzzz');
				new Route(process[kShaderGraphBloom1], process[kShaderGraphBloom2], 1);

				new Route(process[kShaderGraphBloom2], process[kShaderGraphBloomOutput], 0);
			}
		}
	}
	else if (process[kShaderGraphDiffuseReflection])
	{
		process[kShaderGraphDiffuseMultiply2] = new MultiplyProcess;
		graph->AddElement(process[kShaderGraphDiffuseMultiply2]);
		finalLightProduct = process[kShaderGraphDiffuseMultiply2];

		new Route(finalDiffuseProduct, process[kShaderGraphDiffuseMultiply2], 0);
		new Route(process[kShaderGraphDiffuseReflection], process[kShaderGraphDiffuseMultiply2], 1);
	}
	else
	{
		finalLightProduct = finalDiffuseProduct;
	}

	if (process[kShaderGraphHorizon])
	{
		new Route(finalLightProduct, process[kShaderGraphHorizon], 1);
		finalLightProduct = process[kShaderGraphHorizon];
	}

	process[kShaderGraphAmbientOutput] = new AmbientOutputProcess;
	graph->AddElement(process[kShaderGraphAmbientOutput]);

	new Route(finalAmbientProduct, process[kShaderGraphAmbientOutput], 0);

	if (finalNormalMap)
	{
		new Route(finalNormalMap, process[kShaderGraphAmbientOutput], 1);
	}

	process[kShaderGraphLightOutput] = new LightOutputProcess;
	graph->AddElement(process[kShaderGraphLightOutput]);

	new Route(finalLightProduct, process[kShaderGraphLightOutput], 0);

	process[kShaderGraphAlphaOutput] = new AlphaOutputProcess;
	graph->AddElement(process[kShaderGraphAlphaOutput]);

	if ((process[kShaderGraphDeltaDepth]) || (process[kShaderGraphDepthLinearRamp]))
	{
		Process		*finalDepthProduct;

		if (process[kShaderGraphDeltaDepth])
		{
			if (process[kShaderGraphDepthLinearRamp])
			{
				finalDepthProduct = new MultiplyProcess;
				process[kShaderGraphDepthMultiply] = finalDepthProduct;
				graph->AddElement(process[kShaderGraphDepthMultiply]);

				new Route(process[kShaderGraphDeltaDepth], finalDepthProduct, 0);
				new Route(process[kShaderGraphDepthLinearRamp], finalDepthProduct, 1);
			}
			else
			{
				finalDepthProduct = process[kShaderGraphDeltaDepth];
			}
		}
		else
		{
			finalDepthProduct = process[kShaderGraphDepthLinearRamp];
		}

		process[kShaderGraphAlphaMultiply] = new MultiplyProcess;
		graph->AddElement(process[kShaderGraphAlphaMultiply]);

		new Route(finalDepthProduct, process[kShaderGraphAlphaMultiply], 0);
		(new Route(finalDiffuseProduct, process[kShaderGraphAlphaMultiply], 1))->SetRouteSwizzle('aaaa');

		new Route(process[kShaderGraphAlphaMultiply], process[kShaderGraphAlphaOutput], 0);
	}
	else
	{
		(new Route(finalDiffuseProduct, process[kShaderGraphAlphaOutput], 0))->SetRouteSwizzle('aaaa');
	}

	if (materialFlags & kMaterialAlphaTest)
	{
		process[kShaderGraphAlphaTestOutput] = new AlphaTestOutputProcess;
		graph->AddElement(process[kShaderGraphAlphaTestOutput]);

		(new Route(finalDiffuseProduct, process[kShaderGraphAlphaTestOutput], 0))->SetRouteSwizzle('aaaa');
	}

	if (finalEmissionProduct)
	{
		process[kShaderGraphEmissionOutput] = new EmissionOutputProcess;
		graph->AddElement(process[kShaderGraphEmissionOutput]);

		(new Route(finalEmissionProduct, process[kShaderGraphEmissionOutput], 0))->SetRouteSwizzle(emissionSwizzle);

		if (materialFlags & kMaterialEmissionGlow)
		{
			process[kShaderGraphGlowOutput] = new GlowOutputProcess;
			graph->AddElement(process[kShaderGraphGlowOutput]);

			(new Route(finalEmissionProduct, process[kShaderGraphGlowOutput], 0))->SetRouteSwizzle('aaaa');
		}
	}
	else if ((textureAlphaSemantic == kTextureSemanticGlow) && (materialFlags & kMaterialEmissionGlow))
	{
		process[kShaderGraphGlowOutput] = new GlowOutputProcess;
		graph->AddElement(process[kShaderGraphGlowOutput]);

		(new Route(process[kShaderGraphTextureMap1], process[kShaderGraphGlowOutput], 0))->SetRouteSwizzle('aaaa');
	}

	if (process[kShaderGraphReflectionColor])
	{
		process[kShaderGraphReflectionOutput] = new ReflectionOutputProcess;
		graph->AddElement(process[kShaderGraphReflectionOutput]);

		new Route(process[kShaderGraphReflectionColor], process[kShaderGraphReflectionOutput], 0);

		if (finalNormalMap)
		{
			new Route(finalNormalMap, process[kShaderGraphReflectionOutput], 1);
		}

		ReflectionOutputProcess *reflectionProcess = static_cast<ReflectionOutputProcess *>(process[kShaderGraphReflectionOutput]);
		const ReflectionAttribute::ReflectionParams *params = reflectionAttribute->GetReflectionParams();

		reflectionProcess->SetReflectionParams(params);
		if (renderable)
		{
			reflectionProcess->SetReflectionData(params);
		}
	}

	if (finalRefractionProduct)
	{
		process[kShaderGraphRefractionOutput] = new RefractionOutputProcess;
		graph->AddElement(process[kShaderGraphRefractionOutput]);

		new Route(finalRefractionProduct, process[kShaderGraphRefractionOutput], 0);
		if (finalNormalMap)
		{
			new Route(finalNormalMap, process[kShaderGraphRefractionOutput], 1);
		}

		RefractionOutputProcess *refractionProcess = static_cast<RefractionOutputProcess *>(process[kShaderGraphRefractionOutput]);
		const RefractionAttribute::RefractionParams *params = refractionAttribute->GetRefractionParams();

		refractionProcess->SetRefractionParams(params);
		if (renderable)
		{
			refractionProcess->SetRefractionData(params);
		}
	}

	if (finalEnvironmentProduct)
	{
		process[kShaderGraphEnvironmentOutput] = new EnvironmentOutputProcess;
		graph->AddElement(process[kShaderGraphEnvironmentOutput]);

		if (environmentTextureAttribute)
		{
			static_cast<EnvironmentOutputProcess *>(process[kShaderGraphEnvironmentOutput])->SetTexture(environmentTextureAttribute->GetTextureName());
		}

		new Route(finalEnvironmentProduct, process[kShaderGraphEnvironmentOutput], 0);

		if (finalNormalMap)
		{
			new Route(finalNormalMap, process[kShaderGraphEnvironmentOutput], 1);
		}
	}
}

void ShaderAttribute::BuildEffectShaderGraph(const Renderable *renderable, const RenderSegment *renderSegment, const MaterialObject *materialObject, const List<Attribute> *attributeList, ShaderGraph *graph)
{
	Process				*process[kEffectGraphProcessCount];
	const Attribute		*firstAttribute[2];

	unsigned_int32 materialFlags = (materialObject) ? materialObject->GetMaterialFlags() : 0;
	if (renderSegment)
	{
		materialFlags |= renderSegment->GetMaterialState();
	}

	firstAttribute[0] = (materialObject) ? materialObject->GetFirstAttribute() : nullptr;
	firstAttribute[1] = (attributeList) ? attributeList->First() : nullptr;

	for (machine a = 0; a < kEffectGraphProcessCount; a++)
	{
		process[a] = nullptr;
	}

	for (machine a = 0; a < 2; a++)
	{
		const Attribute *attribute = firstAttribute[a];
		while (attribute)
		{
			const Attribute *next = attribute->Next();
			for (;;)
			{
				AttributeType type = attribute->GetAttributeType();
				switch (type)
				{
					case kAttributeReference:
					{
						attribute = static_cast<const ReferenceAttribute *>(attribute)->GetReference();
						if (attribute)
						{
							continue;
						}

						break;
					}

					case kAttributeDiffuse:
					{
						if (!process[kEffectGraphEffectColor])
						{
							process[kEffectGraphEffectColor] = new ColorProcess;
							graph->AddElement(process[kEffectGraphEffectColor]);
						}

						ColorProcess *colorProcess = static_cast<ColorProcess *>(process[kEffectGraphEffectColor]);
						const DiffuseAttribute *diffuseAttribute = static_cast<const DiffuseAttribute *>(attribute);
						const ColorRGBA& diffuseColor = diffuseAttribute->GetDiffuseColor();

						colorProcess->SetColorValue(diffuseColor);
						if (diffuseAttribute->GetAttributeFlags() & kAttributeMutable)
						{
							colorProcess->SetParameterSlot(0);
							colorProcess->SetParameterData(&diffuseColor.red);
						}

						break;
					}

					case kAttributeDiffuseTexture:
					{
						const TextureAttribute *textureAttribute = static_cast<const TextureAttribute *>(attribute);

						if (!process[kEffectGraphTextureMap])
						{
							if (renderable->GetShaderFlags() & kShaderDistortion)
							{
								process[kEffectGraphTextureMap] = new DistortionProcess;
							}
							else
							{
								if (!(renderable->GetShaderFlags() & kShaderProjectiveTexture))
								{
									process[kEffectGraphTextureMap] = new TextureMapProcess;
								}
								else
								{
									process[kEffectGraphTextureMap] = new ProjectiveTextureMapProcess;
								}
							}

							graph->AddElement(process[kEffectGraphTextureMap]);

							static_cast<TextureMapProcess *>(process[kEffectGraphTextureMap])->SetTexture(textureAttribute->GetTexture());
						}
						else if (renderable->GetShaderFlags() & kShaderFireArrays)
						{
							if (!process[kEffectGraphFire])
							{
								process[kEffectGraphFire] = new FireProcess;
								graph->AddElement(process[kEffectGraphFire]);
							}

							static_cast<TextureMapProcess *>(process[kEffectGraphFire])->SetTexture(textureAttribute->GetTexture());
						}

						break;
					}

					case kAttributeDeltaDepth:
					{
						if (!process[kEffectGraphDeltaDepth])
						{
							process[kEffectGraphDeltaDepth] = new DeltaDepthProcess;
							graph->AddElement(process[kEffectGraphDeltaDepth]);
						}

						DeltaDepthProcess *deltaDepthProcess = static_cast<DeltaDepthProcess *>(process[kEffectGraphDeltaDepth]);
						const DeltaDepthAttribute *deltaDepthAttribute = static_cast<const DeltaDepthAttribute *>(attribute);
						deltaDepthProcess->SetDeltaScale(deltaDepthAttribute->GetDeltaScale());
						break;
					}

					case kAttributeDepthRamp:
					{
						if (!process[kEffectGraphDepthLinearRamp])
						{
							process[kEffectGraphDepthLinearRamp] = new LinearRampProcess;
							graph->AddElement(process[kEffectGraphDepthLinearRamp]);

							process[kEffectGraphFragmentDepth] = new FragmentDepthProcess;
							graph->AddElement(process[kEffectGraphFragmentDepth]);

							new Route(process[kEffectGraphFragmentDepth], process[kEffectGraphDepthLinearRamp], 0);
						}

						LinearRampProcess *linearRampProcess = static_cast<LinearRampProcess *>(process[kEffectGraphDepthLinearRamp]);
						const Range<float>& rampRange = static_cast<const DepthRampAttribute *>(attribute)->GetRampRange();
						linearRampProcess->SetRampCenter((rampRange.min + rampRange.max) * 0.5F);
						linearRampProcess->SetRampWidth(rampRange.max - rampRange.min);
						break;
					}

					case kAttributeFireTexture:
					{
						if (!process[kEffectGraphFire])
						{
							process[kEffectGraphFire] = new FireProcess;
							graph->AddElement(process[kEffectGraphFire]);
						}

						FireProcess *fireProcess = static_cast<FireProcess *>(process[kEffectGraphFire]);
						const FireTextureAttribute *fireAttribute = static_cast<const FireTextureAttribute *>(attribute);
						const FireTextureAttribute::FireParams *params = fireAttribute->GetFireParams();

						fireProcess->SetFireParams(params);
						if (renderable)
						{
							fireProcess->SetFireData(params);
						}

						break;
					}
				}

				break;
			}

			attribute = next;
		}
	}

	Process *finalEffectProduct = process[kEffectGraphEffectColor];

	if (renderable->AttributeArrayEnabled(kArrayColor))
	{
		process[kEffectGraphVertexColor] = new VertexColorProcess;
		graph->AddElement(process[kEffectGraphVertexColor]);

		if (finalEffectProduct)
		{
			process[kEffectGraphColorMultiply] = new MultiplyProcess;
			graph->AddElement(process[kEffectGraphColorMultiply]);
			finalEffectProduct = process[kEffectGraphColorMultiply];

			new Route(process[kEffectGraphEffectColor], process[kEffectGraphColorMultiply], 0);
			new Route(process[kEffectGraphVertexColor], process[kEffectGraphColorMultiply], 1);
		}
		else
		{
			finalEffectProduct = process[kEffectGraphVertexColor];
		}
	}

	if (process[kEffectGraphTextureMap])
	{
		if (process[kEffectGraphFire])
		{
			new Route(process[kEffectGraphFire], process[kEffectGraphTextureMap], 0);
		}
		else
		{
			if (!(renderable->GetShaderFlags() & kShaderProjectiveTexture))
			{
				process[kEffectGraphTexcoord] = new Texcoord0Process;
			}
			else
			{
				process[kEffectGraphTexcoord] = new RawTexcoordProcess;
			}

			graph->AddElement(process[kEffectGraphTexcoord]);

			new Route(process[kEffectGraphTexcoord], process[kEffectGraphTextureMap], 0);
		}

		if (finalEffectProduct)
		{
			process[kEffectGraphEffectMultiply] = new MultiplyProcess;
			graph->AddElement(process[kEffectGraphEffectMultiply]);

			if (renderable->GetShaderFlags() & kShaderDistortion)
			{
				if (process[kEffectGraphDeltaDepth])
				{
					process[kEffectGraphAlphaMultiply] = new MultiplyProcess;
					graph->AddElement(process[kEffectGraphAlphaMultiply]);

					new Route(process[kEffectGraphDeltaDepth], process[kEffectGraphAlphaMultiply], 0);
					(new Route(finalEffectProduct, process[kEffectGraphAlphaMultiply], 1))->SetRouteSwizzle('aaaa');

					new Route(process[kEffectGraphAlphaMultiply], process[kEffectGraphEffectMultiply], 0);
				}
				else
				{
					(new Route(finalEffectProduct, process[kEffectGraphEffectMultiply], 0))->SetRouteSwizzle('aaaa');
				}
			}
			else
			{
				new Route(finalEffectProduct, process[kEffectGraphEffectMultiply], 0);
			}

			new Route(process[kEffectGraphTextureMap], process[kEffectGraphEffectMultiply], 1);

			finalEffectProduct = process[kEffectGraphEffectMultiply];
		}
		else
		{
			finalEffectProduct = process[kEffectGraphTextureMap];
		}
	}

	process[kEffectGraphColorOutput] = new AmbientOutputProcess;
	graph->AddElement(process[kEffectGraphColorOutput]);

	process[kEffectGraphAlphaOutput] = new AlphaOutputProcess;
	graph->AddElement(process[kEffectGraphAlphaOutput]);

	if (!finalEffectProduct)
	{
		finalEffectProduct = new ColorProcess;
		process[kEffectGraphEffectColor] = finalEffectProduct;
		graph->AddElement(finalEffectProduct);
	}

	new Route(finalEffectProduct, process[kEffectGraphColorOutput], 0);

	if (!(renderable->GetShaderFlags() & kShaderDistortion))
	{
		if ((process[kEffectGraphDeltaDepth]) || (process[kEffectGraphDepthLinearRamp]))
		{
			Process		*finalDepthProduct;

			if (process[kEffectGraphDeltaDepth])
			{
				if (process[kEffectGraphDepthLinearRamp])
				{
					finalDepthProduct = new MultiplyProcess;
					process[kEffectGraphDepthMultiply] = finalDepthProduct;
					graph->AddElement(process[kEffectGraphDepthMultiply]);

					new Route(process[kEffectGraphDeltaDepth], finalDepthProduct, 0);
					new Route(process[kEffectGraphDepthLinearRamp], finalDepthProduct, 1);
				}
				else
				{
					finalDepthProduct = process[kEffectGraphDeltaDepth];
				}
			}
			else
			{
				finalDepthProduct = process[kEffectGraphDepthLinearRamp];
			}

			process[kEffectGraphAlphaMultiply] = new MultiplyProcess;
			graph->AddElement(process[kEffectGraphAlphaMultiply]);

			new Route(finalDepthProduct, process[kEffectGraphAlphaMultiply], 0);
			(new Route(finalEffectProduct, process[kEffectGraphAlphaMultiply], 1))->SetRouteSwizzle('aaaa');

			new Route(process[kEffectGraphAlphaMultiply], process[kEffectGraphAlphaOutput], 0);
			return;
		}
		else if (materialFlags & kMaterialAlphaTest)
		{
			process[kEffectGraphAlphaTestOutput] = new AlphaTestOutputProcess;
			graph->AddElement(process[kEffectGraphAlphaTestOutput]);

			(new Route(finalEffectProduct, process[kEffectGraphAlphaTestOutput], 0))->SetRouteSwizzle('aaaa');
			return;
		}
	}

	(new Route(finalEffectProduct, process[kEffectGraphAlphaOutput], 0))->SetRouteSwizzle('aaaa');
}

void ShaderAttribute::BuildPlainShaderGraph(ShaderType shaderType, const Renderable *renderable, const RenderSegment *renderSegment, const MaterialObject *materialObject, const List<Attribute> *attributeList, ShaderGraph *graph)
{
	unsigned_int32 materialFlags = (materialObject) ? materialObject->GetMaterialFlags() : 0;
	if (renderSegment)
	{
		materialFlags |= renderSegment->GetMaterialState();
	}

	if (materialFlags & kMaterialAlphaTest)
	{
		Process				*process[kPlainGraphProcessCount];
		const Attribute		*firstAttribute[2];
		int32				textureTexcoordIndex[2];
		int32				parallaxTexcoordIndex;

		firstAttribute[0] = (materialObject) ? materialObject->GetFirstAttribute() : nullptr;
		firstAttribute[1] = (attributeList) ? attributeList->First() : nullptr;

		for (machine a = 0; a < kPlainGraphProcessCount; a++)
		{
			process[a] = nullptr;
		}

		bool normalMapFlag = false;

		for (machine a = 0; a < 2; a++)
		{
			const Attribute *attribute = firstAttribute[a];
			while (attribute)
			{
				AttributeType type = attribute->GetAttributeType();
				switch (type)
				{
					case kAttributeDiffuse:
					{
						if (!process[kPlainGraphDiffuseColor])
						{
							process[kPlainGraphDiffuseColor] = new ColorProcess;
							graph->AddElement(process[kPlainGraphDiffuseColor]);
						}

						ColorProcess *colorProcess = static_cast<ColorProcess *>(process[kPlainGraphDiffuseColor]);
						const DiffuseAttribute *diffuseAttribute = static_cast<const DiffuseAttribute *>(attribute);
						const ColorRGBA& diffuseColor = diffuseAttribute->GetDiffuseColor();

						colorProcess->SetColorValue(diffuseColor);
						if (diffuseAttribute->GetAttributeFlags() & kAttributeMutable)
						{
							colorProcess->SetParameterSlot(0);
							colorProcess->SetParameterData(&diffuseColor.red);
						}

						break;
					}

					case kAttributeDiffuseTexture:
					case kAttributeNormalTexture:
					{
						const TextureAttribute *textureAttribute = static_cast<const TextureAttribute *>(attribute);

						int32 texcoordIndex = textureAttribute->GetTexcoordIndex();
						Process *texcoordProcess = process[kPlainGraphTexcoord1 + texcoordIndex];
						if (!texcoordProcess)
						{
							if (texcoordIndex == 0)
							{
								texcoordProcess = new Texcoord0Process;
							}
							else
							{
								texcoordProcess = new Texcoord1Process;
							}

							process[kPlainGraphTexcoord1 + texcoordIndex] = texcoordProcess;
							graph->AddElement(texcoordProcess);
						}

						if (type == kAttributeDiffuseTexture)
						{
							if (!process[kPlainGraphTextureMap1])
							{
								process[kPlainGraphTextureMap1] = new TextureMapProcess;
								graph->AddElement(process[kPlainGraphTextureMap1]);

								static_cast<TextureMapProcess *>(process[kPlainGraphTextureMap1])->SetTexture(textureAttribute->GetTextureName());
								textureTexcoordIndex[0] = texcoordIndex;
							}
							else
							{
								if (!process[kPlainGraphTextureMap2])
								{
									process[kPlainGraphTextureMap2] = new TextureMapProcess;
									graph->AddElement(process[kPlainGraphTextureMap2]);
								}

								static_cast<TextureMapProcess *>(process[kPlainGraphTextureMap2])->SetTexture(textureAttribute->GetTextureName());
								textureTexcoordIndex[1] = texcoordIndex;
							}
						}
						else if (shaderType == kShaderStructure)
						{
							if (!normalMapFlag)
							{
								normalMapFlag = true;

								if (textureAttribute->GetTexture()->GetAlphaSemantic() == kTextureSemanticParallax)
								{
									parallaxTexcoordIndex = texcoordIndex;

									process[kPlainGraphParallax] = new ParallaxProcess;
									graph->AddElement(process[kPlainGraphParallax]);

									static_cast<TextureMapProcess *>(process[kPlainGraphParallax])->SetTexture(textureAttribute->GetTextureName());
								}
							}
							else
							{
								delete process[kPlainGraphParallax];
								process[kPlainGraphParallax] = nullptr;
							}
						}

						break;
					}
				}

				attribute = attribute->Next();
			}
		}

		if (process[kPlainGraphTextureMap1])
		{
			if (process[kPlainGraphParallax])
			{
				new Route(process[kPlainGraphTexcoord1 + parallaxTexcoordIndex], process[kPlainGraphParallax], 0);

				int32 texcoordIndex = textureTexcoordIndex[0];
				if (texcoordIndex == parallaxTexcoordIndex)
				{
					new Route(process[kPlainGraphParallax], process[kPlainGraphTextureMap1], 0);
				}
				else
				{
					new Route(process[kPlainGraphTexcoord1 + texcoordIndex], process[kPlainGraphTextureMap1], 0);
				}

				if (process[kPlainGraphTextureMap2])
				{
					texcoordIndex = textureTexcoordIndex[1];
					if (texcoordIndex == parallaxTexcoordIndex)
					{
						new Route(process[kPlainGraphParallax], process[kPlainGraphTextureMap2], 0);
					}
					else
					{
						new Route(process[kPlainGraphTexcoord1 + texcoordIndex], process[kPlainGraphTextureMap2], 0);
					}
				}
			}
			else
			{
				new Route(process[kPlainGraphTexcoord1 + textureTexcoordIndex[0]], process[kPlainGraphTextureMap1], 0);

				if (process[kPlainGraphTextureMap2])
				{
					new Route(process[kPlainGraphTexcoord1 + textureTexcoordIndex[1]], process[kPlainGraphTextureMap2], 0);
				}
			}
		}

		Process *finalTextureMap = BuildTextureCombiner(materialObject, graph, &process[kPlainGraphTextureMap1], &process[kPlainGraphTextureMap2], &process[kPlainGraphTextureCombiner], &process[kPlainGraphVertexColor]);

		bool colorArrayEnabled = (renderable) ? renderable->AttributeArrayEnabled(kArrayColor) : false;
		if ((colorArrayEnabled) && (!process[kPlainGraphVertexColor]))
		{
			process[kPlainGraphVertexColor] = new VertexColorProcess;
			graph->AddElement(process[kPlainGraphVertexColor]);
		}

		Process *finalDiffuseProduct = finalTextureMap;
		if (finalDiffuseProduct)
		{
			if (process[kPlainGraphDiffuseColor])
			{
				process[kPlainGraphDiffuseMultiply1] = new MultiplyProcess;
				graph->AddElement(process[kPlainGraphDiffuseMultiply1]);
				finalDiffuseProduct = process[kPlainGraphDiffuseMultiply1];

				if (colorArrayEnabled)
				{
					process[kPlainGraphColorMultiply] = new MultiplyProcess;
					graph->AddElement(process[kPlainGraphColorMultiply]);

					(new Route(process[kPlainGraphDiffuseColor], process[kPlainGraphColorMultiply], 0))->SetRouteSwizzle('aaaa');
					(new Route(process[kPlainGraphVertexColor], process[kPlainGraphColorMultiply], 1))->SetRouteSwizzle('aaaa');

					(new Route(finalTextureMap, process[kPlainGraphDiffuseMultiply1], 0))->SetRouteSwizzle('aaaa');
					(new Route(process[kPlainGraphColorMultiply], process[kPlainGraphDiffuseMultiply1], 1))->SetRouteSwizzle('aaaa');
				}
				else
				{
					(new Route(finalTextureMap, process[kPlainGraphDiffuseMultiply1], 0))->SetRouteSwizzle('aaaa');
					(new Route(process[kPlainGraphDiffuseColor], process[kPlainGraphDiffuseMultiply1], 1))->SetRouteSwizzle('aaaa');
				}
			}
			else if (colorArrayEnabled)
			{
				process[kPlainGraphDiffuseMultiply1] = new MultiplyProcess;
				graph->AddElement(process[kPlainGraphDiffuseMultiply1]);
				finalDiffuseProduct = process[kPlainGraphDiffuseMultiply1];

				(new Route(finalTextureMap, process[kPlainGraphDiffuseMultiply1], 0))->SetRouteSwizzle('aaaa');
				(new Route(process[kPlainGraphVertexColor], process[kPlainGraphDiffuseMultiply1], 1))->SetRouteSwizzle('aaaa');
			}
		}
		else
		{
			if (colorArrayEnabled)
			{
				if (process[kPlainGraphDiffuseColor])
				{
					process[kPlainGraphDiffuseMultiply1] = new MultiplyProcess;
					graph->AddElement(process[kPlainGraphDiffuseMultiply1]);
					finalDiffuseProduct = process[kPlainGraphDiffuseMultiply1];

					(new Route(process[kPlainGraphDiffuseColor], process[kPlainGraphDiffuseMultiply1], 0))->SetRouteSwizzle('aaaa');
					(new Route(process[kPlainGraphVertexColor], process[kPlainGraphDiffuseMultiply1], 1))->SetRouteSwizzle('aaaa');
				}
				else
				{
					finalDiffuseProduct = process[kPlainGraphVertexColor];
				}
			}
			else
			{
				finalDiffuseProduct = process[kPlainGraphDiffuseColor];
			}
		}

		if (finalDiffuseProduct)
		{
			process[kPlainGraphAlphaTestOutput] = new AlphaTestOutputProcess;
			graph->AddElement(process[kPlainGraphAlphaTestOutput]);

			(new Route(finalDiffuseProduct, process[kPlainGraphAlphaTestOutput], 0))->SetRouteSwizzle('aaaa');
		}
	}
}

void ShaderAttribute::SetParameterValue(int32 slot, const Vector4D& param)
{
	Process *process = shaderGraph.GetFirstElement();
	while (process)
	{
		if (process->GetBaseProcessType() == kProcessConstant)
		{
			ConstantProcess *constantProcess = static_cast<ConstantProcess *>(process);
			if (constantProcess->GetParameterSlot() == slot)
			{
				constantProcess->SetParameterValue(param);
			}
		}

		process = process->GetNextElement();
	}
}

// ZYUQURM
