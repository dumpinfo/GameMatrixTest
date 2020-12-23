 

#include "C4Processes.h"
#include "C4Graphics.h"
#include "C4Horizon.h"
#include "C4Paint.h"


using namespace C4;


#if C4OPENGL

	#define COLOR_TEXTURE			"colorTexture"
	#define STRUCTURE_TEXTURE		"structureTexture"
	#define VELOCITY_TEXTURE		"velocityTexture"
	#define OCCLUSION_TEXTURE		"occlusionTexture"
	#define DISTORTION_TEXTURE		"distortionTexture"
	#define GLOW_BLOOM_TEXTURE		"glowBloomTexture"
	#define NOISE_TEXTURE			"noiseTexture"

	#define COLOR_TARGET			"texture"
	#define STRUCTURE_TARGET		"texture"
	#define VELOCITY_TARGET			"texture"
	#define OCCLUSION_TARGET		"texture"
	#define DISTORTION_TARGET		"texture"
	#define GLOW_BLOOM_TARGET		"texture"
	#define NOISE_TARGET			"texture"

#elif C4PSSL //[ PS4

	// -- PS4 code hidden --

#elif C4CG //[ PS3

	// -- PS3 code hidden --

#endif //]


namespace
{
	const TextureHeader vectorNoiseTextureHeader =
	{
		kTexture2D,
		kTextureFilterInhibit,
		kTextureSemanticNone,
		kTextureSemanticNone,
		kTextureRGBA8,
		4, 4, 1,
		{kTextureRepeat, kTextureRepeat, kTextureRepeat},
		1
	};

	alignas(32) const unsigned_int8 vectorNoiseTextureImage[64] =
	{
		0xFD, 0x2F, 0x00, 0x00, 0x8C, 0xFE, 0x00, 0x00, 0x0B, 0x5C, 0x00, 0x00, 0xDE, 0xAC, 0x00, 0x00,
		0x48, 0xE4, 0x00, 0x00, 0xB9, 0xE4, 0x00, 0x00, 0x23, 0xAC, 0x00, 0x00, 0xF6, 0x5C, 0x00, 0x00,
		0x75, 0xFE, 0x00, 0x00, 0x04, 0x2F, 0x00, 0x00, 0xA3, 0xF5, 0x00, 0x00, 0x34, 0xCB, 0x00, 0x00,
		0xEC, 0x86, 0x00, 0x00, 0x15, 0x86, 0x00, 0x00, 0xCD, 0xCB, 0x00, 0x00, 0x5E, 0xF5, 0x00, 0x00
	};
}


const unsigned_int8 Route::swizzleTable[26] =
{
	3, 2, 4, 4, 4, 4, 1, 4, 4, 4, 4, 4, 4, 4, 4, 2, 3, 0, 0, 1, 4, 4, 3, 0, 1, 2
};


const char *const ConstantProcess::constantIdentifier[kMaxShaderConstantCount] =
{
	FPARAM(FRAGMENT_PARAM_CONSTANT0), FPARAM(FRAGMENT_PARAM_CONSTANT1), FPARAM(FRAGMENT_PARAM_CONSTANT2), FPARAM(FRAGMENT_PARAM_CONSTANT3),
	FPARAM(FRAGMENT_PARAM_CONSTANT4), FPARAM(FRAGMENT_PARAM_CONSTANT5), FPARAM(FRAGMENT_PARAM_CONSTANT6), FPARAM(FRAGMENT_PARAM_CONSTANT7)
};

ShaderStateProc *const ConstantProcess::scalarStateProc[kMaxShaderConstantCount] =
{
	&StateProc_LoadScalar0, &StateProc_LoadScalar1, &StateProc_LoadScalar2, &StateProc_LoadScalar3,
	&StateProc_LoadScalar4, &StateProc_LoadScalar5, &StateProc_LoadScalar6, &StateProc_LoadScalar7
};

ShaderStateProc *const ConstantProcess::vectorStateProc[kMaxShaderConstantCount] =
{
	&StateProc_LoadVector0, &StateProc_LoadVector1, &StateProc_LoadVector2, &StateProc_LoadVector3,
	&StateProc_LoadVector4, &StateProc_LoadVector5, &StateProc_LoadVector6, &StateProc_LoadVector7
};


Texture *AmbientOutputProcess::radiosityColorTexture = nullptr;
Texture *OcclusionPostProcess::vectorNoiseTexture = nullptr;


namespace C4
{
	template <> Heap EngineMemory<Process>::heap("Process", 65536);
	template class EngineMemory<Process>;
}


ProcessRegistration::ProcessRegistration(ProcessType type, const char *name, ProcessGroup group) : Registration<Process, ProcessRegistration>(type)
{
	processName = name; 
	processGroup = group;
}
 
ProcessRegistration::~ProcessRegistration()
{ 
}

 
Route::Route(Process *start, Process *finish, int32 port) : GraphEdge<Process, Route>(start, finish)
{ 
	routeFlags = 0; 
	routePort = port;
	routeNegation = false;
	routeSwizzle = 'xyzw';
} 

Route::Route(const Route& route, Process *start, Process *finish) : GraphEdge<Process, Route>(start, finish)
{
	routeFlags = route.routeFlags;
	routePort = route.routePort;
	routeNegation = route.routeNegation;
	routeSwizzle = route.routeSwizzle;
}

Route::~Route()
{
}

void Route::Pack(Packer& data, unsigned_int32 packFlags) const
{
	data << ChunkHeader('FLAG', 4);
	data << routeFlags;

	data << ChunkHeader('PORT', 4);
	data << routePort;

	data << ChunkHeader('NEGA', 4);
	data << routeNegation;

	data << ChunkHeader('SWIZ', 4);
	data << routeSwizzle;

	data << TerminatorChunk;
}

void Route::Unpack(Unpacker& data, unsigned_int32 unpackFlags)
{
	UnpackChunkList<Route>(data, unpackFlags);
}

bool Route::UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags)
{
	switch (chunkHeader->chunkType)
	{
		case 'FLAG':

			data >> routeFlags;
			return (true);

		case 'PORT':

			data >> routePort;
			return (true);

		case 'NEGA':

			data >> routeNegation;
			return (true);

		case 'SWIZ':

			data >> routeSwizzle;
			return (true);
	}

	return (false);
}

int32 Route::GetSettingCount(void) const
{
	return (2);
}

Setting *Route::GetSetting(int32 index) const
{
	const StringTable *table = TheInterfaceMgr->GetStringTable();

	if (index == 0)
	{
		const char *title = table->GetString(StringID('ROUT', 'NEGA'));
		return (new BooleanSetting('NEGA', routeNegation, title));
	}
	else if (index == 1)
	{
		const char *title = table->GetString(StringID('ROUT', 'SWIZ'));

		String<4> string = Text::TypeToString(routeSwizzle);
		char c = string[0];

		if ((string[1] == c) && (string[2] == c) && (string[3] == c))
		{
			string[1] = 0;
		}

		auto filter = [](unsigned_int32 code) -> bool
		{
			code -= 'A';
			if (code < 26U)
			{
				return (swizzleTable[code] < 4);
			}

			code -= 0x0020;
			if (code < 26U)
			{
				return (swizzleTable[code] < 4);
			}

			return (false);
		};

		return (new TextSetting('SWIZ', string, title, 4, filter));
	}

	return (nullptr);
}

void Route::SetSetting(const Setting *setting)
{
	Type identifier = setting->GetSettingIdentifier();

	if (identifier == 'NEGA')
	{
		routeNegation = static_cast<const BooleanSetting *>(setting)->GetBooleanValue();
	}
	else if (identifier == 'SWIZ')
	{
		const char *text = static_cast<const TextSetting *>(setting)->GetText();
		unsigned_int32 swizzle = *text++;
		if (swizzle != 0)
		{
			if (swizzle < 'a')
			{
				swizzle += 32;
			}

			unsigned_int32 last = swizzle;

			for (machine a = 0; a < 3; a++)
			{
				unsigned_int32 c = *text;
				if (c != 0)
				{
					if (c < 'a')
					{
						c += 32;
					}

					swizzle = (swizzle << 8) | c;
					last = c;
					text++;
				}
				else
				{
					swizzle = (swizzle << 8) | last;
				}
			}

			routeSwizzle = swizzle;
		}
		else
		{
			routeSwizzle = 'xyzw';
		}
	}
}

bool Route::operator ==(const Route& route) const
{
	return ((routePort == route.routePort) && (routeNegation == route.routeNegation) && (routeSwizzle == route.routeSwizzle));
}

int32 Route::GenerateOutputSize(void) const
{
	unsigned_int8 c1 = swizzleTable[(routeSwizzle >> 24) - 'a'];
	unsigned_int8 c2 = swizzleTable[((routeSwizzle >> 16) & 0xFF) - 'a'];
	unsigned_int8 c3 = swizzleTable[((routeSwizzle >> 8) & 0xFF) - 'a'];
	unsigned_int8 c4 = swizzleTable[(routeSwizzle & 0xFF) - 'a'];

	if ((c1 == c2) && (c1 == c3) && (c1 == c4))
	{
		return (1);
	}

	return (GetStartElement()->GetProcessData()->outputSize);
}

int32 Route::GenerateOutputIdentifier(const ShaderCompileData *compileData, const ShaderAllocationData *allocData, SwizzleData *swizzleData, char *name) const
{
	swizzleData->negate ^= routeNegation;

	const Process *process = GetStartElement();
	int32 maxComponent = process->GetProcessData()->outputSize - 1;

	int32 size = swizzleData->size;
	for (machine a = 0; a < size; a++)
	{
		unsigned_int32 c = (routeSwizzle >> (24 - swizzleData->component[a] * 8)) & 0xFF;
		swizzleData->component[a] = (unsigned_int8) Min(swizzleTable[c - 'a'], maxComponent);
	}

	return (process->GenerateOutputIdentifier(compileData, allocData, swizzleData, name));
}

unsigned_int32 Route::GenerateRouteSignature(void) const
{
	unsigned_int8 c1 = swizzleTable[(routeSwizzle >> 24) - 'a'];
	unsigned_int8 c2 = swizzleTable[((routeSwizzle >> 16) & 0xFF) - 'a'];
	unsigned_int8 c3 = swizzleTable[((routeSwizzle >> 8) & 0xFF) - 'a'];
	unsigned_int8 c4 = swizzleTable[(routeSwizzle & 0xFF) - 'a'];

	return ((routePort << 16) | (routeNegation << 8) | (c1 << 6) | (c2 << 4) | (c3 << 2) | c4);
}


Process::Process(ProcessType type)
{
	processType = type;
	baseProcessType = 0;

	processFlags = 0;

	processPosition.Set(0.0F, 0.0F);
}

Process::Process(const Process& process) : processComment(process.processComment)
{
	processType = process.processType;
	baseProcessType = process.baseProcessType;

	processFlags = process.processFlags;

	processPosition = process.processPosition;
}

Process::~Process()
{
}

Process *Process::New(ProcessType type)
{
	Type	data[2];

	switch (type)
	{
		case kProcessRawTexcoord:

			return (new RawTexcoordProcess);

		case kProcessImpostorTexcoord:

			return (new ImpostorTexcoordProcess);

		case kProcessImpostorBlend:

			return (new ImpostorBlendProcess);

		case kProcessTerrainTexcoord:

			return (new TerrainTexcoordProcess);

		case kProcessTriplanarBlend:

			return (new TriplanarBlendProcess);

		case kProcessTerrainLightDirection:

			return (new TerrainLightDirectionProcess);

		case kProcessTerrainViewDirection:

			return (new TerrainViewDirectionProcess);

		case kProcessTerrainHalfwayDirection:

			return (new TerrainHalfwayDirectionProcess);
	}

	data[0] = type;
	data[1] = 0;

	Unpacker unpacker(data);
	return (Create(unpacker));
}

void Process::RegisterStandardProcesses(void)
{
	const StringTable *table = TheInterfaceMgr->GetStringTable();

	static ProcessReg<SectionProcess> sectionRegistration(kProcessSection, "");

	static ProcessReg<ScalarProcess> scalarRegistration(kProcessScalar, table->GetString(StringID('PROC', kProcessScalar)), 'BASC');
	static ProcessReg<VectorProcess> vectorRegistration(kProcessVector, table->GetString(StringID('PROC', kProcessVector)), 'BASC');
	static ProcessReg<ColorProcess> colorRegistration(kProcessColor, table->GetString(StringID('PROC', kProcessColor)), 'BASC');
	static ProcessReg<TimeProcess> timeRegistration(kProcessTime, table->GetString(StringID('PROC', kProcessTime)), 'BASC');
	static ProcessReg<DetailLevelProcess> detailLevelRegistration(kProcessDetailLevel, table->GetString(StringID('PROC', kProcessDetailLevel)), 'BASC');
	static ProcessReg<FragmentDepthProcess> fragmentDepthRegistration(kProcessFragmentDepth, table->GetString(StringID('PROC', kProcessFragmentDepth)), 'BASC');
	static ProcessReg<PrimitiveFacingProcess> primitiveFacingRegistration(kProcessPrimitiveFacing, table->GetString(StringID('PROC', kProcessPrimitiveFacing)), 'BASC');
	static ProcessReg<TextureMapProcess> textureMapRegistration(kProcessTextureMap, table->GetString(StringID('PROC', kProcessTextureMap)), 'BASC');
	static ProcessReg<NormalMapProcess> normalMapRegistration(kProcessNormalMap, table->GetString(StringID('PROC', kProcessNormalMap)), 'BASC');
	static ProcessReg<ImpostorTextureProcess> impostorTextureRegistration(kProcessImpostorTexture, table->GetString(StringID('PROC', kProcessImpostorTexture)), 'BASC');
	static ProcessReg<ImpostorNormalProcess> impostorNormalRegistration(kProcessImpostorNormal, table->GetString(StringID('PROC', kProcessImpostorNormal)), 'BASC');
	static ProcessReg<TerrainTextureProcess> terrainTextureRegistration(kProcessTerrainTexture, table->GetString(StringID('PROC', kProcessTerrainTexture)), 'BASC');
	static ProcessReg<TerrainNormalProcess> terrainNormalRegistration(kProcessTerrainNormal, table->GetString(StringID('PROC', kProcessTerrainNormal)), 'BASC');
	static ProcessReg<TerrainNormal2Process> terrainNormal2Registration(kProcessTerrainNormal2, table->GetString(StringID('PROC', kProcessTerrainNormal2)), 'BASC');
	static ProcessReg<TerrainNormal3Process> terrainNormal3Registration(kProcessTerrainNormal3, table->GetString(StringID('PROC', kProcessTerrainNormal3)), 'BASC');
	static ProcessReg<PaintTextureProcess> paintTextureRegistration(kProcessPaintTexture, table->GetString(StringID('PROC', kProcessPaintTexture)), 'BASC');
	static ProcessReg<Merge2Process> merge2Registration(kProcessMerge2, table->GetString(StringID('PROC', kProcessMerge2)), 'BASC');
	static ProcessReg<Merge3Process> merge3Registration(kProcessMerge3, table->GetString(StringID('PROC', kProcessMerge3)), 'BASC');
	static ProcessReg<Merge4Process> merge4Registration(kProcessMerge4, table->GetString(StringID('PROC', kProcessMerge4)), 'BASC');

	static ProcessReg<VertexColorProcess> vertexColorRegistration(kProcessVertexColor, table->GetString(StringID('PROC', kProcessVertexColor)), 'TERP');
	static ProcessReg<Texcoord0Process> texcoord0Registration(kProcessTexcoord0, table->GetString(StringID('PROC', kProcessTexcoord0)), 'TERP');
	static ProcessReg<Texcoord1Process> texcoord1Registration(kProcessTexcoord1, table->GetString(StringID('PROC', kProcessTexcoord1)), 'TERP');
	static ProcessReg<PaintTexcoordProcess> paintTexcoordRegistration(kProcessPaintTexcoord, table->GetString(StringID('PROC', kProcessPaintTexcoord)), 'TERP');
	static ProcessReg<VertexGeometryProcess> vertexGeometryRegistration(kProcessVertexGeometry, table->GetString(StringID('PROC', kProcessVertexGeometry)), 'TERP');
	static ProcessReg<ObjectPositionProcess> objectPositionRegistration(kProcessObjectPosition, table->GetString(StringID('PROC', kProcessObjectPosition)), 'TERP');
	static ProcessReg<WorldPositionProcess> worldPositionRegistration(kProcessWorldPosition, table->GetString(StringID('PROC', kProcessWorldPosition)), 'TERP');
	static ProcessReg<ObjectNormalProcess> objectNormalRegistration(kProcessObjectNormal, table->GetString(StringID('PROC', kProcessObjectNormal)), 'TERP');
	static ProcessReg<ObjectTangentProcess> objectTangentRegistration(kProcessObjectTangent, table->GetString(StringID('PROC', kProcessObjectTangent)), 'TERP');
	static ProcessReg<ObjectBitangentProcess> objectBitangentRegistration(kProcessObjectBitangent, table->GetString(StringID('PROC', kProcessObjectBitangent)), 'TERP');
	static ProcessReg<WorldNormalProcess> worldNormalRegistration(kProcessWorldNormal, table->GetString(StringID('PROC', kProcessWorldNormal)), 'TERP');
	static ProcessReg<WorldTangentProcess> worldTangentRegistration(kProcessWorldTangent, table->GetString(StringID('PROC', kProcessWorldTangent)), 'TERP');
	static ProcessReg<WorldBitangentProcess> worldBitangentRegistration(kProcessWorldBitangent, table->GetString(StringID('PROC', kProcessWorldBitangent)), 'TERP');
	static ProcessReg<TangentLightDirectionProcess> tangentLightDirectionRegistration(kProcessTangentLightDirection, table->GetString(StringID('PROC', kProcessTangentLightDirection)), 'TERP');
	static ProcessReg<TangentViewDirectionProcess> tangentViewDirectionRegistration(kProcessTangentViewDirection, table->GetString(StringID('PROC', kProcessTangentViewDirection)), 'TERP');
	static ProcessReg<TangentHalfwayDirectionProcess> tangentHalfwayDirectionRegistration(kProcessTangentHalfwayDirection, table->GetString(StringID('PROC', kProcessTangentHalfwayDirection)), 'TERP');
	static ProcessReg<ObjectLightDirectionProcess> objectLightDirectionRegistration(kProcessObjectLightDirection, table->GetString(StringID('PROC', kProcessObjectLightDirection)), 'TERP');
	static ProcessReg<ObjectViewDirectionProcess> objectViewDirectionRegistration(kProcessObjectViewDirection, table->GetString(StringID('PROC', kProcessObjectViewDirection)), 'TERP');
	static ProcessReg<ObjectHalfwayDirectionProcess> objectHalfwayDirectionRegistration(kProcessObjectHalfwayDirection, table->GetString(StringID('PROC', kProcessObjectHalfwayDirection)), 'TERP');
	static ProcessReg<TangentLightVectorProcess> tangentLightVectorRegistration(kProcessTangentLightVector, table->GetString(StringID('PROC', kProcessTangentLightVector)), 'TERP');
	static ProcessReg<TangentViewVectorProcess> tangentViewVectorRegistration(kProcessTangentViewVector, table->GetString(StringID('PROC', kProcessTangentViewVector)), 'TERP');
	static ProcessReg<ObjectLightVectorProcess> objectLightVectorRegistration(kProcessObjectLightVector, table->GetString(StringID('PROC', kProcessObjectLightVector)), 'TERP');
	static ProcessReg<ObjectViewVectorProcess> objectViewVectorRegistration(kProcessObjectViewVector, table->GetString(StringID('PROC', kProcessObjectViewVector)), 'TERP');

	static ProcessReg<AbsoluteProcess> absoluteRegistration(kProcessAbsolute, table->GetString(StringID('PROC', kProcessAbsolute)), 'MATH');
	static ProcessReg<InvertProcess> invertRegistration(kProcessInvert, table->GetString(StringID('PROC', kProcessInvert)), 'MATH');
	static ProcessReg<ExpandProcess> expandRegistration(kProcessExpand, table->GetString(StringID('PROC', kProcessExpand)), 'MATH');
	static ProcessReg<ReciprocalProcess> reciprocalRegistration(kProcessReciprocal, table->GetString(StringID('PROC', kProcessReciprocal)), 'MATH');
	static ProcessReg<ReciprocalSquareRootProcess> reciprocalSquareRootRegistration(kProcessReciprocalSquareRoot, table->GetString(StringID('PROC', kProcessReciprocalSquareRoot)), 'MATH');
	static ProcessReg<SquareRootProcess> squareRootRegistration(kProcessSquareRoot, table->GetString(StringID('PROC', kProcessSquareRoot)), 'MATH');
	static ProcessReg<MagnitudeProcess> magnitudeRegistration(kProcessMagnitude, table->GetString(StringID('PROC', kProcessMagnitude)), 'MATH');
	static ProcessReg<NormalizeProcess> normalizeRegistration(kProcessNormalize, table->GetString(StringID('PROC', kProcessNormalize)), 'MATH');
	static ProcessReg<FloorProcess> floorRegistration(kProcessFloor, table->GetString(StringID('PROC', kProcessFloor)), 'MATH');
	static ProcessReg<RoundProcess> roundRegistration(kProcessRound, table->GetString(StringID('PROC', kProcessRound)), 'MATH');
	static ProcessReg<FractionProcess> fractionRegistration(kProcessFraction, table->GetString(StringID('PROC', kProcessFraction)), 'MATH');
	static ProcessReg<SaturateProcess> saturateRegistration(kProcessSaturate, table->GetString(StringID('PROC', kProcessSaturate)), 'MATH');
	static ProcessReg<SineProcess> sineRegistration(kProcessSine, table->GetString(StringID('PROC', kProcessSine)), 'MATH');
	static ProcessReg<CosineProcess> cosineRegistration(kProcessCosine, table->GetString(StringID('PROC', kProcessCosine)), 'MATH');
	static ProcessReg<Exp2Process> exp2Registration(kProcessExp2, table->GetString(StringID('PROC', kProcessExp2)), 'MATH');
	static ProcessReg<Log2Process> log2Registration(kProcessLog2, table->GetString(StringID('PROC', kProcessLog2)), 'MATH');

	static ProcessReg<AddProcess> addRegistration(kProcessAdd, table->GetString(StringID('PROC', kProcessAdd)), 'MATH');
	static ProcessReg<SubtractProcess> subtractRegistration(kProcessSubtract, table->GetString(StringID('PROC', kProcessSubtract)), 'MATH');
	static ProcessReg<AverageProcess> averageRegistration(kProcessAverage, table->GetString(StringID('PROC', kProcessAverage)), 'MATH');
	static ProcessReg<MultiplyProcess> multiplyRegistration(kProcessMultiply, table->GetString(StringID('PROC', kProcessMultiply)), 'MATH');
	static ProcessReg<DivideProcess> divideRegistration(kProcessDivide, table->GetString(StringID('PROC', kProcessDivide)), 'MATH');
	static ProcessReg<Dot3Process> dot3Registration(kProcessDot3, table->GetString(StringID('PROC', kProcessDot3)), 'MATH');
	static ProcessReg<Dot4Process> dot4Registration(kProcessDot4, table->GetString(StringID('PROC', kProcessDot4)), 'MATH');
	static ProcessReg<CrossProcess> crossRegistration(kProcessCross, table->GetString(StringID('PROC', kProcessCross)), 'MATH');
	static ProcessReg<MinimumProcess> minimumRegistration(kProcessMinimum, table->GetString(StringID('PROC', kProcessMinimum)), 'MATH');
	static ProcessReg<MaximumProcess> maximumRegistration(kProcessMaximum, table->GetString(StringID('PROC', kProcessMaximum)), 'MATH');
	static ProcessReg<SetLessThanProcess> setLessThanRegistration(kProcessSetLessThan, table->GetString(StringID('PROC', kProcessSetLessThan)), 'MATH');
	static ProcessReg<SetGreaterThanProcess> setGreaterThanRegistration(kProcessSetGreaterThan, table->GetString(StringID('PROC', kProcessSetGreaterThan)), 'MATH');
	static ProcessReg<SetLessEqualProcess> setLessEqualRegistration(kProcessSetLessEqual, table->GetString(StringID('PROC', kProcessSetLessEqual)), 'MATH');
	static ProcessReg<SetGreaterEqualProcess> setGreaterEqualRegistration(kProcessSetGreaterEqual, table->GetString(StringID('PROC', kProcessSetGreaterEqual)), 'MATH');
	static ProcessReg<SetEqualProcess> setEqualRegistration(kProcessSetEqual, table->GetString(StringID('PROC', kProcessSetEqual)), 'MATH');
	static ProcessReg<SetNotEqualProcess> setNotEqualRegistration(kProcessSetNotEqual, table->GetString(StringID('PROC', kProcessSetNotEqual)), 'MATH');
	static ProcessReg<PowerProcess> powerRegistration(kProcessPower, table->GetString(StringID('PROC', kProcessPower)), 'MATH');

	static ProcessReg<MultiplyAddProcess> multiplyAddRegistration(kProcessMultiplyAdd, table->GetString(StringID('PROC', kProcessMultiplyAdd)), 'MATH');
	static ProcessReg<LerpProcess> lerpRegistration(kProcessLerp, table->GetString(StringID('PROC', kProcessLerp)), 'MATH');

	static ProcessReg<DiffuseProcess> diffuseRegistration(kProcessDiffuse, table->GetString(StringID('PROC', kProcessDiffuse)), 'COMP');
	static ProcessReg<SpecularProcess> specularRegistration(kProcessSpecular, table->GetString(StringID('PROC', kProcessSpecular)), 'COMP');
	static ProcessReg<MicrofacetProcess> microfacetRegistration(kProcessMicrofacet, table->GetString(StringID('PROC', kProcessMicrofacet)), 'COMP');
	static ProcessReg<TerrainDiffuseProcess> terrainDiffuseRegistration(kProcessTerrainDiffuse, table->GetString(StringID('PROC', kProcessTerrainDiffuse)), 'COMP');
	static ProcessReg<TerrainSpecularProcess> terrainSpecularRegistration(kProcessTerrainSpecular, table->GetString(StringID('PROC', kProcessTerrainSpecular)), 'COMP');
	static ProcessReg<GenerateImpostorNormalProcess> generateImpostorNormalRegistration(kProcessGenerateImpostorNormal, table->GetString(StringID('PROC', kProcessGenerateImpostorNormal)), 'COMP');
	static ProcessReg<ImpostorDepthProcess> impostorDepthRegistration(kProcessImpostorDepth, table->GetString(StringID('PROC', kProcessImpostorDepth)), 'COMP');
	static ProcessReg<CombineNormalsProcess> combineNormalsRegistration(kProcessCombineNormals, table->GetString(StringID('PROC', kProcessCombineNormals)), 'COMP');
	static ProcessReg<FrontNormalProcess> frontNormalRegistration(kProcessFrontNormal, table->GetString(StringID('PROC', kProcessFrontNormal)), 'COMP');
	static ProcessReg<ReflectVectorProcess> reflectVectorRegistration(kProcessReflectVector, table->GetString(StringID('PROC', kProcessReflectVector)), 'COMP');
	static ProcessReg<LinearRampProcess> linearRampRegistration(kProcessLinearRamp, table->GetString(StringID('PROC', kProcessLinearRamp)), 'COMP');
	static ProcessReg<SmoothParameterProcess> smoothParameterRegistration(kProcessSmoothParameter, table->GetString(StringID('PROC', kProcessSmoothParameter)), 'COMP');
	static ProcessReg<SteepParameterProcess> steepParameterRegistration(kProcessSteepParameter, table->GetString(StringID('PROC', kProcessSteepParameter)), 'COMP');
	static ProcessReg<WorldTransformProcess> worldTransformRegistration(kProcessWorldTransform, table->GetString(StringID('PROC', kProcessWorldTransform)), 'COMP');
	static ProcessReg<DeltaDepthProcess> deltaDepthRegistration(kProcessDeltaDepth, table->GetString(StringID('PROC', kProcessDeltaDepth)), 'COMP');
	static ProcessReg<ParallaxProcess> parallaxRegistration(kProcessParallax, table->GetString(StringID('PROC', kProcessParallax)), 'COMP');
	static ProcessReg<HorizonProcess> horizonRegistration(kProcessHorizon, table->GetString(StringID('PROC', kProcessHorizon)), 'COMP');
	static ProcessReg<KillProcess> killRegistration(kProcessKill, table->GetString(StringID('PROC', kProcessKill)), 'COMP');
	static ProcessReg<ImpostorTransitionProcess> impostorTransitionRegistration(kProcessImpostorTransition, table->GetString(StringID('PROC', kProcessImpostorTransition)), 'COMP');
	static ProcessReg<GeometryTransitionProcess> geometryTransitionRegistration(kProcessGeometryTransition, table->GetString(StringID('PROC', kProcessGeometryTransition)), 'COMP');

	static ProcessReg<AlphaOutputProcess> alphaOutputRegistration(kProcessAlphaOutput, table->GetString(StringID('PROC', kProcessAlphaOutput)));
	static ProcessReg<AlphaTestOutputProcess> alphaTestOutputRegistration(kProcessAlphaTestOutput, table->GetString(StringID('PROC', kProcessAlphaTestOutput)));
	static ProcessReg<AmbientOutputProcess> ambientOutputRegistration(kProcessAmbientOutput, table->GetString(StringID('PROC', kProcessAmbientOutput)));
	static ProcessReg<EmissionOutputProcess> emissionOutputRegistration(kProcessEmissionOutput, table->GetString(StringID('PROC', kProcessEmissionOutput)));
	static ProcessReg<ReflectionOutputProcess> reflectionOutputRegistration(kProcessReflectionOutput, table->GetString(StringID('PROC', kProcessReflectionOutput)));
	static ProcessReg<RefractionOutputProcess> refractionOutputRegistration(kProcessRefractionOutput, table->GetString(StringID('PROC', kProcessRefractionOutput)));
	static ProcessReg<EnvironmentOutputProcess> environmentOutputRegistration(kProcessEnvironmentOutput, table->GetString(StringID('PROC', kProcessEnvironmentOutput)));
	static ProcessReg<TerrainEnvironmentOutputProcess> terrainEnvironmentOutputRegistration(kProcessTerrainEnvironmentOutput, table->GetString(StringID('PROC', kProcessTerrainEnvironmentOutput)));
	static ProcessReg<ImpostorDepthOutputProcess> impostorDepthOutputRegistration(kProcessImpostorDepthOutput, table->GetString(StringID('PROC', kProcessImpostorDepthOutput)));
	static ProcessReg<LightOutputProcess> lightOutputRegistration(kProcessLightOutput, table->GetString(StringID('PROC', kProcessLightOutput)));
	static ProcessReg<GlowOutputProcess> glowOutputRegistration(kProcessGlowOutput, table->GetString(StringID('PROC', kProcessGlowOutput)));
	static ProcessReg<BloomOutputProcess> bloomOutputRegistration(kProcessBloomOutput, table->GetString(StringID('PROC', kProcessBloomOutput)));
}

void Process::PackType(Packer& data) const
{
	data << processType;
}

void Process::Pack(Packer& data, unsigned_int32 packFlags) const
{
	data << ChunkHeader('FLAG', 4);
	data << processFlags;

	data << ChunkHeader('POSI', sizeof(Point2D));
	data << processPosition;

	if ((packFlags & kPackEditor) && (processComment.Length() != 0))
	{
		PackHandle handle = data.BeginChunk('CMNT');
		data << processComment;
		data.EndChunk(handle);
	}

	data << TerminatorChunk;
}

void Process::Unpack(Unpacker& data, unsigned_int32 unpackFlags)
{
	UnpackChunkList<Process>(data, unpackFlags);
}

bool Process::UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags)
{
	switch (chunkHeader->chunkType)
	{
		case 'FLAG':

			data >> processFlags;
			return (true);

		case 'POSI':

			data >> processPosition;
			return (true);

		case 'CMNT':

			if (unpackFlags & kUnpackEditor)
			{
				data >> processComment;
				return (true);
			}
	}

	return (false);
}

void *Process::BeginSettingsUnpack(void)
{
	processComment.Purge();
	return (nullptr);
}

int32 Process::GetSettingCount(void) const
{
	return (1);
}

Setting *Process::GetSetting(int32 index) const
{
	if (index == 0)
	{
		const StringTable *table = TheInterfaceMgr->GetStringTable();
		const char *title = table->GetString(StringID('PROC', 'CMNT'));
		const char *string = processComment;
		return (new TextSetting('CMNT', (string) ? string : "", title, 255));
	}

	return (nullptr);
}

void Process::SetSetting(const Setting *setting)
{
	Type identifier = setting->GetSettingIdentifier();

	if (identifier == 'CMNT')
	{
		processComment = static_cast<const TextSetting *>(setting)->GetText();
	}
}

bool Process::operator ==(const Process& process) const
{
	if (processType != process.processType)
	{
		return (false);
	}

	if (processFlags != process.processFlags)
	{
		return (false);
	}

	int32 portCount = GetPortCount();
	for (machine port = 0; port < portCount; port++)
	{
		const Route *route = GetPortRoute(port);
		const Route *processRoute = process.GetPortRoute(port);

		if (route)
		{
			if (!processRoute)
			{
				return (false);
			}

			if (route->GetStartElement()->GetProcessIndex() != processRoute->GetStartElement()->GetProcessIndex())
			{
				return (false);
			}

			if (!(*route == *processRoute))
			{
				return (false);
			}
		}
		else if (processRoute)
		{
			return (false);
		}
	}

	return (true);
}

Route *Process::GetPortRoute(int32 port) const
{
	Route *route = GetFirstIncomingEdge();
	while (route)
	{
		if (route->GetRoutePort() == port)
		{
			return (route);
		}

		route = route->GetNextIncomingEdge();
	}

	return (nullptr);
}

int32 Process::GetPortCount(void) const
{
	return (0);
}

int32 Process::GetInternalPortCount(void) const
{
	return (0);
}

unsigned_int32 Process::GetPortFlags(int32 index) const
{
	return (0);
}

const char *Process::GetPortName(int32 index) const
{
	return (nullptr);
}

#if C4PS3 //[ PS3

	// -- PS3 code hidden --

#endif //]

void Process::ReferenceStateParams(const Process *process)
{
}

void Process::Preschedule(const ShaderCompileData *compileData)
{
}

int32 Process::GenerateProcessSignature(const ShaderCompileData *compileData, unsigned_int32 *signature) const
{
	signature[0] = processType;
	int32 count = 1;

	int32 portCount = GetPortCount() + GetInternalPortCount();
	for (machine port = 0; port < portCount; port++)
	{
		const Route *route = GetPortRoute(port);
		if (route)
		{
			signature[count++] = (port << 24) | route->GetStartElement()->GetProcessIndex();

			unsigned_int32 routeSignature = route->GenerateRouteSignature();
			if ((routeSignature & 0xFFFF) != 0x001B)
			{
				signature[count++] = routeSignature;
			}
		}
	}

	return (count);
}

int32 Process::GenerateDerivedInterpolantTypes(const ShaderCompileData *compileData, ProcessType *type) const
{
	return (0);
}

void Process::GenerateProcessData(const ShaderCompileData *compileData, ProcessData *data) const
{
}

int32 Process::PregenerateOutputIdentifier(const SwizzleData *swizzleData, char *name)
{
	if (swizzleData->negate)
	{
		name[0] = '-';
		return (1);
	}

	return (0);
}

int32 Process::PostgenerateOutputIdentifier(const SwizzleData *swizzleData, char *name)
{
	int32 len = 0;
	int32 size = swizzleData->size;

	unsigned_int8 c1 = swizzleData->component[0];

	if (size == 1)
	{
		name[0] = '.';
		name[1] = Route::GetSwizzleChar(c1);
		len = 2;
	}
	else
	{
		unsigned_int8 c2 = swizzleData->component[1];

		if (size == 2)
		{
			name[0] = '.';
			name[1] = Route::GetSwizzleChar(c1);
			name[2] = Route::GetSwizzleChar(c2);
			len = 3;
		}
		else
		{
			unsigned_int8 c3 = swizzleData->component[2];

			if (size == 3)
			{
				name[0] = '.';
				name[1] = Route::GetSwizzleChar(c1);
				name[2] = Route::GetSwizzleChar(c2);
				name[3] = Route::GetSwizzleChar(c3);
				len = 4;
			}
			else
			{
				unsigned_int8 c4 = swizzleData->component[3];

				if ((c1 != 0) || (c2 != 1) || (c3 != 2) || (c4 != 3))
				{
					name[0] = '.';
					name[1] = Route::GetSwizzleChar(c1);
					name[2] = Route::GetSwizzleChar(c2);
					name[3] = Route::GetSwizzleChar(c3);
					name[4] = Route::GetSwizzleChar(c4);
					len = 5;
				}
			}
		}
	}

	return (len);
}

int32 Process::GenerateOutputIdentifier(const ShaderCompileData *compileData, const ShaderAllocationData *allocData, SwizzleData *swizzleData, char *name) const
{
	int32 reg = GetProcessData()->outputRegister;
	if (reg < 0)
	{
		return (0);
	}

	if (swizzleData)
	{
		int32 len = PregenerateOutputIdentifier(swizzleData, name);
		name += len;

		name[0] = 'r';
		if (reg < 10)
		{
			name[1] = (char) (reg + 48);
			return (PostgenerateOutputIdentifier(swizzleData, name + 2) + len + 2);
		}

		int32 d = reg / 10;
		name[1] = (char) (d + 48);
		name[2] = (char) (reg - d * 10 + 48);
		return (PostgenerateOutputIdentifier(swizzleData, name + 3) + len + 3);
	}

	name[0] = 'r';
	if (reg < 10)
	{
		name[1] = (char) (reg + 48);
		return (2);
	}

	int32 d = reg / 10;
	name[1] = (char) (d + 48);
	name[2] = (char) (reg - d * 10 + 48);
	return (3);
}

int32 Process::GenerateShaderCode(const ShaderCompileData *compileData, const char **shaderCode) const
{
	return (0);
}


SectionProcess::SectionProcess() : Process(kProcessSection)
{
	sectionWidth = 0.0F;
	sectionHeight = 0.0F;

	sectionColor.Set(0.96875F, 0.96875F, 0.96875F);
}

SectionProcess::SectionProcess(const SectionProcess& sectionProcess) : Process(sectionProcess)
{
	sectionWidth = sectionProcess.sectionWidth;
	sectionHeight = sectionProcess.sectionHeight;

	sectionColor = sectionProcess.sectionColor;
}

SectionProcess::~SectionProcess()
{
}

Process *SectionProcess::Replicate(void) const
{
	return (new SectionProcess(*this));
}

void SectionProcess::Pack(Packer& data, unsigned_int32 packFlags) const
{
	Process::Pack(data, packFlags);

	data << ChunkHeader('SIZE', 8);
	data << sectionWidth;
	data << sectionHeight;

	data << ChunkHeader('COLR', sizeof(ColorRGBA));
	data << sectionColor;

	data << TerminatorChunk;
}

void SectionProcess::Unpack(Unpacker& data, unsigned_int32 unpackFlags)
{
	Process::Unpack(data, unpackFlags);
	UnpackChunkList<SectionProcess>(data, unpackFlags);
}

bool SectionProcess::UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags)
{
	switch (chunkHeader->chunkType)
	{
		case 'SIZE':

			data >> sectionWidth;
			data >> sectionHeight;
			return (true);

		case 'COLR':

			data >> sectionColor;
			return (true);
	}

	return (false);
}

int32 SectionProcess::GetSettingCount(void) const
{
	return (Process::GetSettingCount() + 1);
}

Setting *SectionProcess::GetSetting(int32 index) const
{
	int32 count = Process::GetSettingCount();
	if (index < count)
	{
		return (Process::GetSetting(index));
	}

	if (index == count)
	{
		const StringTable *table = TheInterfaceMgr->GetStringTable();
		const char *title = table->GetString(StringID('PROC', kProcessSection, 'COLR'));
		const char *picker = table->GetString(StringID('PROC', kProcessSection, 'PICK'));
		return (new ColorSetting('COLR', sectionColor, title, picker));
	}

	return (nullptr);
}

void SectionProcess::SetSetting(const Setting *setting)
{
	Type identifier = setting->GetSettingIdentifier();

	if (identifier == 'COLR')
	{
		sectionColor = static_cast<const ColorSetting *>(setting)->GetColor();
	}
	else
	{
		Process::SetSetting(setting);
	}
}


ConstantProcess::ConstantProcess(ProcessType type) : Process(type)
{
	SetBaseProcessType(kProcessConstant);

	parameterSlot = -1;
}

ConstantProcess::ConstantProcess(const ConstantProcess& constantProcess) : Process(constantProcess)
{
	parameterSlot = constantProcess.parameterSlot;
}

ConstantProcess::~ConstantProcess()
{
}

void ConstantProcess::Pack(Packer& data, unsigned_int32 packFlags) const
{
	Process::Pack(data, packFlags);

	data << parameterSlot;
}

void ConstantProcess::Unpack(Unpacker& data, unsigned_int32 unpackFlags)
{
	Process::Unpack(data, unpackFlags);

	data >> parameterSlot;
}

int32 ConstantProcess::GetSettingCount(void) const
{
	return (Process::GetSettingCount() + 1);
}

Setting *ConstantProcess::GetSetting(int32 index) const
{
	int32 count = Process::GetSettingCount();
	if (index < count)
	{
		return (Process::GetSetting(index));
	}

	if (index == count)
	{
		const StringTable *table = TheInterfaceMgr->GetStringTable();
		const char *title = table->GetString(StringID('PROC', kProcessConstant, 'SLOT'));
		MenuSetting *menu = new MenuSetting('SLOT', parameterSlot + 1, title, 9);

		menu->SetMenuItemString(0, table->GetString(StringID('PROC', kProcessConstant, 'SLOT', 'CNST')));
		for (machine a = 0; a < 8; a++)
		{
			menu->SetMenuItemString(a + 1, table->GetString(StringID('PROC', kProcessConstant, 'SLOT', 'PRM0' + a)));
		}

		return (menu);
	}

	return (nullptr);
}

void ConstantProcess::SetSetting(const Setting *setting)
{
	Type identifier = setting->GetSettingIdentifier();

	if (identifier == 'SLOT')
	{
		parameterSlot = static_cast<const MenuSetting *>(setting)->GetMenuSelection() - 1;
	}
	else
	{
		Process::SetSetting(setting);
	}
}

bool ConstantProcess::operator ==(const Process& process) const
{
	if (Process::operator ==(process))
	{
		const ConstantProcess& constantProcess = static_cast<const ConstantProcess&>(process);
		return (parameterSlot == constantProcess.parameterSlot);
	}

	return (false);
}

int32 ConstantProcess::GenerateProcessSignature(const ShaderCompileData *compileData, unsigned_int32 *signature) const
{
	int32 count = Process::GenerateProcessSignature(compileData, signature);
	signature[count] = parameterSlot;
	return (count + 1);
}

void ConstantProcess::StateProc_LoadScalar0(const Renderable *renderable, const void *cookie)
{
	float f = *static_cast<const float *>(cookie);
	Render::SetFragmentShaderParameter(0, f, f, f, f);
}

void ConstantProcess::StateProc_LoadScalar1(const Renderable *renderable, const void *cookie)
{
	float f = *static_cast<const float *>(cookie);
	Render::SetFragmentShaderParameter(1, f, f, f, f);
}

void ConstantProcess::StateProc_LoadScalar2(const Renderable *renderable, const void *cookie)
{
	float f = *static_cast<const float *>(cookie);
	Render::SetFragmentShaderParameter(2, f, f, f, f);
}

void ConstantProcess::StateProc_LoadScalar3(const Renderable *renderable, const void *cookie)
{
	float f = *static_cast<const float *>(cookie);
	Render::SetFragmentShaderParameter(3, f, f, f, f);
}

void ConstantProcess::StateProc_LoadScalar4(const Renderable *renderable, const void *cookie)
{
	float f = *static_cast<const float *>(cookie);
	Render::SetFragmentShaderParameter(4, f, f, f, f);
}

void ConstantProcess::StateProc_LoadScalar5(const Renderable *renderable, const void *cookie)
{
	float f = *static_cast<const float *>(cookie);
	Render::SetFragmentShaderParameter(5, f, f, f, f);
}

void ConstantProcess::StateProc_LoadScalar6(const Renderable *renderable, const void *cookie)
{
	float f = *static_cast<const float *>(cookie);
	Render::SetFragmentShaderParameter(6, f, f, f, f);
}

void ConstantProcess::StateProc_LoadScalar7(const Renderable *renderable, const void *cookie)
{
	float f = *static_cast<const float *>(cookie);
	Render::SetFragmentShaderParameter(7, f, f, f, f);
}

void ConstantProcess::StateProc_LoadVector0(const Renderable *renderable, const void *cookie)
{
	Render::SetFragmentShaderParameter(0, static_cast<const float *>(cookie));
}

void ConstantProcess::StateProc_LoadVector1(const Renderable *renderable, const void *cookie)
{
	Render::SetFragmentShaderParameter(1, static_cast<const float *>(cookie));
}

void ConstantProcess::StateProc_LoadVector2(const Renderable *renderable, const void *cookie)
{
	Render::SetFragmentShaderParameter(2, static_cast<const float *>(cookie));
}

void ConstantProcess::StateProc_LoadVector3(const Renderable *renderable, const void *cookie)
{
	Render::SetFragmentShaderParameter(3, static_cast<const float *>(cookie));
}

void ConstantProcess::StateProc_LoadVector4(const Renderable *renderable, const void *cookie)
{
	Render::SetFragmentShaderParameter(4, static_cast<const float *>(cookie));
}

void ConstantProcess::StateProc_LoadVector5(const Renderable *renderable, const void *cookie)
{
	Render::SetFragmentShaderParameter(5, static_cast<const float *>(cookie));
}

void ConstantProcess::StateProc_LoadVector6(const Renderable *renderable, const void *cookie)
{
	Render::SetFragmentShaderParameter(6, static_cast<const float *>(cookie));
}

void ConstantProcess::StateProc_LoadVector7(const Renderable *renderable, const void *cookie)
{
	Render::SetFragmentShaderParameter(7, static_cast<const float *>(cookie));
}


ScalarProcess::ScalarProcess() : ConstantProcess(kProcessScalar)
{
	scalarValue = 1.0F;
	parameterData = &scalarValue;
}

ScalarProcess::ScalarProcess(const ScalarProcess& scalarProcess) : ConstantProcess(scalarProcess)
{
	scalarValue = scalarProcess.scalarValue;
	parameterData = &scalarValue;
}

ScalarProcess::~ScalarProcess()
{
}

Process *ScalarProcess::Replicate(void) const
{
	return (new ScalarProcess(*this));
}

void ScalarProcess::SetParameterValue(const Vector4D& param)
{
	scalarValue = param.x;
}

void ScalarProcess::Pack(Packer& data, unsigned_int32 packFlags) const
{
	ConstantProcess::Pack(data, packFlags);

	data << scalarValue;
}

void ScalarProcess::Unpack(Unpacker& data, unsigned_int32 unpackFlags)
{
	ConstantProcess::Unpack(data, unpackFlags);

	data >> scalarValue;
}

int32 ScalarProcess::GetSettingCount(void) const
{
	return (ConstantProcess::GetSettingCount() + 1);
}

Setting *ScalarProcess::GetSetting(int32 index) const
{
	int32 count = ConstantProcess::GetSettingCount();
	if (index < count)
	{
		return (ConstantProcess::GetSetting(index));
	}

	if (index == count)
	{
		const StringTable *table = TheInterfaceMgr->GetStringTable();
		const char *title = table->GetString(StringID('PROC', kProcessScalar, 'VALU'));
		return (new TextSetting('VALU', scalarValue, title));
	}

	return (nullptr);
}

void ScalarProcess::SetSetting(const Setting *setting)
{
	Type identifier = setting->GetSettingIdentifier();

	if (identifier == 'VALU')
	{
		const char *text = static_cast<const TextSetting *>(setting)->GetText();
		scalarValue = Text::StringToFloat(text);
	}
	else
	{
		ConstantProcess::SetSetting(setting);
	}
}

bool ScalarProcess::operator ==(const Process& process) const
{
	if (ConstantProcess::operator ==(process))
	{
		if (GetParameterSlot() < 0)
		{
			const ScalarProcess& scalarProcess = static_cast<const ScalarProcess&>(process);
			return (scalarValue == scalarProcess.scalarValue);
		}

		return (true);
	}

	return (false);
}

void ScalarProcess::ReferenceStateParams(const Process *process)
{
	parameterData = static_cast<const ScalarProcess *>(process)->parameterData;
}

int32 ScalarProcess::GenerateProcessSignature(const ShaderCompileData *compileData, unsigned_int32 *signature) const
{
	int32 count = ConstantProcess::GenerateProcessSignature(compileData, signature);

	if (GetParameterSlot() < 0)
	{
		signature[count++] = *reinterpret_cast<const unsigned_int32 *>(&scalarValue);
	}

	return (count);
}

void ScalarProcess::GenerateProcessData(const ShaderCompileData *compileData, ProcessData *data) const
{
	data->outputSize = 1;

	int32 slot = GetParameterSlot();
	if (slot >= 0)
	{
		const float *param = compileData->renderable->GetShaderParameterPointer(slot);
		if (!param)
		{
			param = parameterData;
		}

		compileData->shaderData->AddStateProc(scalarStateProc[slot], param);
	}
}

int32 ScalarProcess::GenerateOutputIdentifier(const ShaderCompileData *compileData, const ShaderAllocationData *allocData, SwizzleData *swizzleData, char *name) const
{
	int32 slot = GetParameterSlot();
	if (slot < 0)
	{
		float v = scalarValue;
		if (swizzleData->negate)
		{
			v = -v;
		}

		String<15> string = Text::FloatToString(v);

		int32 size = swizzleData->size;
		if (size > 1)
		{
			char *start = name;

			#if C4OPENGL

				name += Text::CopyText("vec", name);

			#else

				name += Text::CopyText("float", name);

			#endif

			name[0] = (char) (size + '0');
			name[1] = '(';
			name += 2;

			for (machine a = 0; a < size - 1; a++)
			{
				name += Text::CopyText(string, name);
				name[0] = ',';
				name[1] = ' ';
				name += 2;
			}

			name += Text::CopyText(string, name);

			name[0] = ')';
			name[1] = 0;
			name++;

			return ((int32) (name - start));
		}

		return (Text::CopyText(string, name));
	}

	int32 len = PregenerateOutputIdentifier(swizzleData, name);
	len += Text::CopyText(constantIdentifier[slot], name + len);
	return (PostgenerateOutputIdentifier(swizzleData, name + len) + len);
}


VectorProcess::VectorProcess() : ConstantProcess(kProcessVector)
{
	vectorValue.Set(0.0F, 0.0F, 0.0F, 0.0F);
	parameterData = &vectorValue.x;
}

VectorProcess::VectorProcess(const VectorProcess& vectorProcess) : ConstantProcess(vectorProcess)
{
	vectorValue = vectorProcess.vectorValue;
	parameterData = &vectorValue.x;
}

VectorProcess::~VectorProcess()
{
}

Process *VectorProcess::Replicate(void) const
{
	return (new VectorProcess(*this));
}

void VectorProcess::SetParameterValue(const Vector4D& param)
{
	vectorValue = param;
}

void VectorProcess::Pack(Packer& data, unsigned_int32 packFlags) const
{
	ConstantProcess::Pack(data, packFlags);

	data << vectorValue;
}

void VectorProcess::Unpack(Unpacker& data, unsigned_int32 unpackFlags)
{
	ConstantProcess::Unpack(data, unpackFlags);

	data >> vectorValue;
}

int32 VectorProcess::GetSettingCount(void) const
{
	return (ConstantProcess::GetSettingCount() + 4);
}

Setting *VectorProcess::GetSetting(int32 index) const
{
	int32 count = ConstantProcess::GetSettingCount();
	if (index < count)
	{
		return (ConstantProcess::GetSetting(index));
	}

	const StringTable *table = TheInterfaceMgr->GetStringTable();

	if (index == count)
	{
		const char *title = table->GetString(StringID('PROC', kProcessVector, 'XXXX'));
		return (new TextSetting('XXXX', vectorValue.x, title));
	}

	if (index == count + 1)
	{
		const char *title = table->GetString(StringID('PROC', kProcessVector, 'YYYY'));
		return (new TextSetting('YYYY', vectorValue.y, title));
	}

	if (index == count + 2)
	{
		const char *title = table->GetString(StringID('PROC', kProcessVector, 'ZZZZ'));
		return (new TextSetting('ZZZZ', vectorValue.z, title));
	}

	if (index == count + 3)
	{
		const char *title = table->GetString(StringID('PROC', kProcessVector, 'WWWW'));
		return (new TextSetting('WWWW', vectorValue.w, title));
	}

	return (nullptr);
}

void VectorProcess::SetSetting(const Setting *setting)
{
	Type identifier = setting->GetSettingIdentifier();

	if (identifier == 'XXXX')
	{
		const char *text = static_cast<const TextSetting *>(setting)->GetText();
		vectorValue.x = Text::StringToFloat(text);
	}
	else if (identifier == 'YYYY')
	{
		const char *text = static_cast<const TextSetting *>(setting)->GetText();
		vectorValue.y = Text::StringToFloat(text);
	}
	else if (identifier == 'ZZZZ')
	{
		const char *text = static_cast<const TextSetting *>(setting)->GetText();
		vectorValue.z = Text::StringToFloat(text);
	}
	else if (identifier == 'WWWW')
	{
		const char *text = static_cast<const TextSetting *>(setting)->GetText();
		vectorValue.w = Text::StringToFloat(text);
	}
	else
	{
		ConstantProcess::SetSetting(setting);
	}
}

bool VectorProcess::operator ==(const Process& process) const
{
	if (ConstantProcess::operator ==(process))
	{
		if (GetParameterSlot() < 0)
		{
			const VectorProcess& vectorProcess = static_cast<const VectorProcess&>(process);
			return (vectorValue == vectorProcess.vectorValue);
		}

		return (true);
	}

	return (false);
}

void VectorProcess::ReferenceStateParams(const Process *process)
{
	parameterData = static_cast<const VectorProcess *>(process)->parameterData;
}

int32 VectorProcess::GenerateProcessSignature(const ShaderCompileData *compileData, unsigned_int32 *signature) const
{
	int32 count = ConstantProcess::GenerateProcessSignature(compileData, signature);

	if (GetParameterSlot() < 0)
	{
		signature += count;
		signature[0] = *reinterpret_cast<const unsigned_int32 *>(&vectorValue.x);
		signature[1] = *reinterpret_cast<const unsigned_int32 *>(&vectorValue.y);
		signature[2] = *reinterpret_cast<const unsigned_int32 *>(&vectorValue.z);
		signature[3] = *reinterpret_cast<const unsigned_int32 *>(&vectorValue.w);
		count += 4;
	}

	return (count);
}

void VectorProcess::GenerateProcessData(const ShaderCompileData *compileData, ProcessData *data) const
{
	data->outputSize = 4;

	int32 slot = GetParameterSlot();
	if (slot >= 0)
	{
		const float *param = compileData->renderable->GetShaderParameterPointer(slot);
		if (!param)
		{
			param = parameterData;
		}

		compileData->shaderData->AddStateProc(vectorStateProc[slot], param);
	}
}

int32 VectorProcess::GenerateOutputIdentifier(const ShaderCompileData *compileData, const ShaderAllocationData *allocData, SwizzleData *swizzleData, char *name) const
{
	int32 slot = GetParameterSlot();
	if (slot < 0)
	{
		int32 size = swizzleData->size;
		unsigned_int8 c1 = swizzleData->component[0];
		for (machine a = 1; a < size; a++)
		{
			if (swizzleData->component[a] != c1)
			{
				Vector4D v = vectorValue;
				if (swizzleData->negate)
				{
					v = -v;
				}

				char *start = name;
				if (size > 1)
				{
					#if C4OPENGL

						name += Text::CopyText("vec", name);

					#else

						name += Text::CopyText("float", name);

					#endif

					name[0] = (char) (size + '0');
					name[1] = '(';
					name += 2;

					for (machine b = 0; b < size - 1; b++)
					{
						name += Text::CopyText(Text::FloatToString(v[swizzleData->component[b]]), name);
						name[0] = ',';
						name[1] = ' ';
						name += 2;
					}

					name += Text::CopyText(Text::FloatToString(v[swizzleData->component[size - 1]]), name);

					name[0] = ')';
					name[1] = 0;
					name++;
				}
				else
				{
					name += Text::CopyText(Text::FloatToString(v[c1]), name);
				}

				return ((int32) (name - start));
			}
		}

		float v = vectorValue[c1];
		if (swizzleData->negate)
		{
			v = -v;
		}

		return (Text::CopyText(Text::FloatToString(v), name));
	}

	int32 len = PregenerateOutputIdentifier(swizzleData, name);
	len += Text::CopyText(constantIdentifier[slot], name + len);
	return (PostgenerateOutputIdentifier(swizzleData, name + len) + len);
}


ColorProcess::ColorProcess() : ConstantProcess(kProcessColor)
{
	colorValue.Set(1.0F, 1.0F, 1.0F, 1.0F);
	parameterData = &colorValue.red;
}

ColorProcess::ColorProcess(const ColorProcess& colorProcess) : ConstantProcess(colorProcess)
{
	colorValue = colorProcess.colorValue;
	parameterData = &colorValue.red;
}

ColorProcess::~ColorProcess()
{
}

Process *ColorProcess::Replicate(void) const
{
	return (new ColorProcess(*this));
}

void ColorProcess::SetParameterValue(const Vector4D& param)
{
	colorValue.Set(param.x, param.y, param.z, param.w);
}

void ColorProcess::Pack(Packer& data, unsigned_int32 packFlags) const
{
	ConstantProcess::Pack(data, packFlags);

	data << colorValue;
}

void ColorProcess::Unpack(Unpacker& data, unsigned_int32 unpackFlags)
{
	ConstantProcess::Unpack(data, unpackFlags);

	data >> colorValue;
}

int32 ColorProcess::GetSettingCount(void) const
{
	return (ConstantProcess::GetSettingCount() + 1);
}

Setting *ColorProcess::GetSetting(int32 index) const
{
	int32 count = ConstantProcess::GetSettingCount();
	if (index < count)
	{
		return (ConstantProcess::GetSetting(index));
	}

	if (index == count)
	{
		const StringTable *table = TheInterfaceMgr->GetStringTable();
		const char *title = table->GetString(StringID('PROC', kProcessColor, 'COLR'));
		const char *picker = table->GetString(StringID('PROC', kProcessColor, 'PICK'));
		return (new ColorSetting('COLR', colorValue, title, picker, kColorPickerAlpha));
	}

	return (nullptr);
}

void ColorProcess::SetSetting(const Setting *setting)
{
	if (setting->GetSettingIdentifier() == 'COLR')
	{
		colorValue = static_cast<const ColorSetting *>(setting)->GetColor();
	}
	else
	{
		ConstantProcess::SetSetting(setting);
	}
}

bool ColorProcess::operator ==(const Process& process) const
{
	if (ConstantProcess::operator ==(process))
	{
		if (GetParameterSlot() < 0)
		{
			const ColorProcess& colorProcess = static_cast<const ColorProcess&>(process);
			return (colorValue == colorProcess.colorValue);
		}

		return (true);
	}

	return (false);
}

void ColorProcess::ReferenceStateParams(const Process *process)
{
	parameterData = static_cast<const ColorProcess *>(process)->parameterData;
}

int32 ColorProcess::GenerateProcessSignature(const ShaderCompileData *compileData, unsigned_int32 *signature) const
{
	int32 count = ConstantProcess::GenerateProcessSignature(compileData, signature);

	if (GetParameterSlot() < 0)
	{
		signature += count;
		signature[0] = *reinterpret_cast<const unsigned_int32 *>(&colorValue.red);
		signature[1] = *reinterpret_cast<const unsigned_int32 *>(&colorValue.green);
		signature[2] = *reinterpret_cast<const unsigned_int32 *>(&colorValue.blue);
		signature[3] = *reinterpret_cast<const unsigned_int32 *>(&colorValue.alpha);
		count += 4;
	}

	return (count);
}

void ColorProcess::GenerateProcessData(const ShaderCompileData *compileData, ProcessData *data) const
{
	data->outputSize = 4;

	int32 slot = GetParameterSlot();
	if (slot >= 0)
	{
		const float *param = compileData->renderable->GetShaderParameterPointer(slot);
		if (!param)
		{
			param = parameterData;
		}

		compileData->shaderData->AddStateProc(vectorStateProc[slot], param);
	}
}

int32 ColorProcess::GenerateOutputIdentifier(const ShaderCompileData *compileData, const ShaderAllocationData *allocData, SwizzleData *swizzleData, char *name) const
{
	int32 slot = GetParameterSlot();
	if (slot < 0)
	{
		int32 size = swizzleData->size;
		unsigned_int8 c1 = swizzleData->component[0];
		for (machine a = 1; a < size; a++)
		{
			if (swizzleData->component[a] != c1)
			{
				ColorRGBA v = colorValue;
				if (swizzleData->negate)
				{
					v = -v;
				}

				char *start = name;
				if (size > 1)
				{
					#if C4OPENGL

						name += Text::CopyText("vec", name);

					#else

						name += Text::CopyText("float", name);

					#endif

					name[0] = (char) (size + '0');
					name[1] = '(';
					name += 2;

					for (machine b = 0; b < size - 1; b++)
					{
						name += Text::CopyText(Text::FloatToString(v[swizzleData->component[b]]), name);
						name[0] = ',';
						name[1] = ' ';
						name += 2;
					}

					name += Text::CopyText(Text::FloatToString(v[swizzleData->component[size - 1]]), name);

					name[0] = ')';
					name[1] = 0;
					name++;
				}
				else
				{
					name += Text::CopyText(Text::FloatToString(v[c1]), name);
				}

				return ((int32) (name - start));
			}
		}

		float v = colorValue[c1];
		if (swizzleData->negate)
		{
			v = -v;
		}

		return (Text::CopyText(Text::FloatToString(v), name));
	}

	int32 len = PregenerateOutputIdentifier(swizzleData, name);
	len += Text::CopyText(constantIdentifier[slot], name + len);
	return (PostgenerateOutputIdentifier(swizzleData, name + len) + len);
}


TimeProcess::TimeProcess() : Process(kProcessTime)
{
	SetBaseProcessType(kProcessParameter);
}

TimeProcess::TimeProcess(const TimeProcess& timeProcess) : Process(timeProcess)
{
}

TimeProcess::~TimeProcess()
{
}

Process *TimeProcess::Replicate(void) const
{
	return (new TimeProcess(*this));
}

void TimeProcess::GenerateProcessData(const ShaderCompileData *compileData, ProcessData *data) const
{
	data->outputSize = 1;
}

int32 TimeProcess::GenerateOutputIdentifier(const ShaderCompileData *compileData, const ShaderAllocationData *allocData, SwizzleData *swizzleData, char *name) const
{
	int32 len = PregenerateOutputIdentifier(swizzleData, name);
	len += Text::CopyText(UPARAM(UNIVERSAL_PARAM_SHADER_TIME), name + len);
	return (PostgenerateOutputIdentifier(swizzleData, name + len) + len);
}


DetailLevelProcess::DetailLevelProcess() : Process(kProcessDetailLevel)
{
	SetBaseProcessType(kProcessParameter);
}

DetailLevelProcess::DetailLevelProcess(const DetailLevelProcess& detailLevelProcess) : Process(detailLevelProcess)
{
}

DetailLevelProcess::~DetailLevelProcess()
{
}

Process *DetailLevelProcess::Replicate(void) const
{
	return (new DetailLevelProcess(*this));
}

void DetailLevelProcess::GenerateProcessData(const ShaderCompileData *compileData, ProcessData *data) const
{
	data->outputSize = 1;

	compileData->shaderData->AddStateProc(&StateProc_SetDetailLevelParam);
}

int32 DetailLevelProcess::GenerateOutputIdentifier(const ShaderCompileData *compileData, const ShaderAllocationData *allocData, SwizzleData *swizzleData, char *name) const
{
	int32 len = PregenerateOutputIdentifier(swizzleData, name);
	len += Text::CopyText("" FPARAM(FRAGMENT_PARAM_DETAIL_LEVEL) "", name + len);
	return (PostgenerateOutputIdentifier(swizzleData, name + len) + len);
}

void DetailLevelProcess::StateProc_SetDetailLevelParam(const Renderable *renderable, const void *cookie)
{
	Render::SetFragmentShaderParameter(kFragmentParamDetailLevel, renderable->GetShaderDetailParameter(), 0.0F, 0.0F, 0.0F);
}


VertexColorProcess::VertexColorProcess() : Process(kProcessVertexColor)
{
	SetBaseProcessType(kProcessParameter);
}

VertexColorProcess::VertexColorProcess(const VertexColorProcess& vertexColorProcess) : Process(vertexColorProcess)
{
}

VertexColorProcess::~VertexColorProcess()
{
}

Process *VertexColorProcess::Replicate(void) const
{
	return (new VertexColorProcess(*this));
}

void VertexColorProcess::GenerateProcessData(const ShaderCompileData *compileData, ProcessData *data) const
{
	compileData->shaderSourceFlags |= kShaderSourcePrimaryColor;

	data->outputSize = 4;
}

int32 VertexColorProcess::GenerateOutputIdentifier(const ShaderCompileData *compileData, const ShaderAllocationData *allocData, SwizzleData *swizzleData, char *name) const
{
	int32 len = PregenerateOutputIdentifier(swizzleData, name);
	len += Text::CopyText(FRAGMENT_COLOR0, name + len);
	return (PostgenerateOutputIdentifier(swizzleData, name + len) + len);
}


FragmentPositionProcess::FragmentPositionProcess() : Process(kProcessFragmentPosition)
{
	SetBaseProcessType(kProcessParameter);
}

FragmentPositionProcess::FragmentPositionProcess(const FragmentPositionProcess& fragmentPositionProcess) : Process(fragmentPositionProcess)
{
}

FragmentPositionProcess::~FragmentPositionProcess()
{
}

Process *FragmentPositionProcess::Replicate(void) const
{
	return (new FragmentPositionProcess(*this));
}

void FragmentPositionProcess::GenerateProcessData(const ShaderCompileData *compileData, ProcessData *data) const
{
	data->outputSize = 4;
}

int32 FragmentPositionProcess::GenerateOutputIdentifier(const ShaderCompileData *compileData, const ShaderAllocationData *allocData, SwizzleData *swizzleData, char *name) const
{
	int32 len = PregenerateOutputIdentifier(swizzleData, name);
	len += Text::CopyText(FRAGMENT_POSITION, name + len);
	return (PostgenerateOutputIdentifier(swizzleData, name + len) + len);
}


FragmentDepthProcess::FragmentDepthProcess() : Process(kProcessFragmentDepth)
{
}

FragmentDepthProcess::FragmentDepthProcess(const FragmentDepthProcess& fragmentDepthProcess) : Process(fragmentDepthProcess)
{
}

FragmentDepthProcess::~FragmentDepthProcess()
{
}

Process *FragmentDepthProcess::Replicate(void) const
{
	return (new FragmentDepthProcess(*this));
}

void FragmentDepthProcess::GenerateProcessData(const ShaderCompileData *compileData, ProcessData *data) const
{
	data->registerCount = 1;
	data->outputSize = 1;
}

int32 FragmentDepthProcess::GenerateShaderCode(const ShaderCompileData *compileData, const char **shaderCode) const
{
	static const char code[] =
	{
		#if C4OPENGL

			"# = 1.0 / gl_FragCoord.w;\n"

		#elif C4PSSL //[ PS4

			// -- PS4 code hidden --

		#elif C4CG //[ PS3

			// -- PS3 code hidden --

		#endif //]
	};

	shaderCode[0] = code;
	return (1);
}


PrimitiveFacingProcess::PrimitiveFacingProcess() : Process(kProcessPrimitiveFacing)
{
}

PrimitiveFacingProcess::PrimitiveFacingProcess(const PrimitiveFacingProcess& primitiveFacingProcess) : Process(primitiveFacingProcess)
{
}

PrimitiveFacingProcess::~PrimitiveFacingProcess()
{
}

Process *PrimitiveFacingProcess::Replicate(void) const
{
	return (new PrimitiveFacingProcess(*this));
}

void PrimitiveFacingProcess::GenerateProcessData(const ShaderCompileData *compileData, ProcessData *data) const
{
	data->registerCount = 1;
	data->outputSize = 1;
}

int32 PrimitiveFacingProcess::GenerateShaderCode(const ShaderCompileData *compileData, const char **shaderCode) const
{
	static const char code[] =
	{
		#if C4OPENGL

			"# = float(gl_FrontFacing) * 2.0 - 1.0;\n"

		#elif C4PSSL //[ PS4

			// -- PS4 code hidden --

		#elif C4CG //[ PS3

			// -- PS3 code hidden --

		#endif //]
	};

	shaderCode[0] = code;
	return (1);
}


TextureMapProcess::TextureMapProcess() : Process(kProcessTextureMap)
{
	SetBaseProcessType(kProcessTextureMap);

	textureName[0] = 0;
	textureObject = nullptr;
}

TextureMapProcess::TextureMapProcess(ProcessType type) : Process(type)
{
	SetBaseProcessType(kProcessTextureMap);

	textureName[0] = 0;
	textureObject = nullptr;
}

TextureMapProcess::TextureMapProcess(const TextureMapProcess& textureMapProcess) : Process(textureMapProcess)
{
	textureName = textureMapProcess.textureName;

	Texture *texture = textureMapProcess.GetTexture();
	textureObject = texture;

	if (texture)
	{
		texture->Retain();
	}
}

TextureMapProcess::~TextureMapProcess()
{
	if (textureObject)
	{
		textureObject->Release();
	}
}

Process *TextureMapProcess::Replicate(void) const
{
	return (new TextureMapProcess(*this));
}

void TextureMapProcess::Pack(Packer& data, unsigned_int32 packFlags) const
{
	Process::Pack(data, packFlags);

	data << textureName;
}

void TextureMapProcess::Unpack(Unpacker& data, unsigned_int32 unpackFlags)
{
	Process::Unpack(data, unpackFlags);

	data >> textureName;
	SetTexture(textureName);
}

void *TextureMapProcess::BeginSettingsUnpack(void)
{
	if (textureObject)
	{
		textureObject->Release();
		textureObject = nullptr;
	}

	return (Process::BeginSettingsUnpack());
}

int32 TextureMapProcess::GetSettingCount(void) const
{
	return (Process::GetSettingCount() + 1);
}

Setting *TextureMapProcess::GetSetting(int32 index) const
{
	int32 count = Process::GetSettingCount();
	if (index < count)
	{
		return (Process::GetSetting(index));
	}

	if (index == count)
	{
		const StringTable *table = TheInterfaceMgr->GetStringTable();
		const char *title = table->GetString(StringID('PROC', kProcessTextureMap, 'TNAM'));
		const char *picker = table->GetString(StringID('PROC', kProcessTextureMap, 'PICK'));
		return (new ResourceSetting('TNAM', textureName, title, picker, TextureResource::GetDescriptor()));
	}

	return (nullptr);
}

void TextureMapProcess::SetSetting(const Setting *setting)
{
	Type identifier = setting->GetSettingIdentifier();

	if (identifier == 'TNAM')
	{
		SetTexture(static_cast<const ResourceSetting *>(setting)->GetResourceName());
	}
	else
	{
		Process::SetSetting(setting);
	}
}

bool TextureMapProcess::operator ==(const Process& process) const
{
	if (Process::operator ==(process))
	{
		const TextureMapProcess& textureMapProcess = static_cast<const TextureMapProcess&>(process);
		return (textureName == textureMapProcess.textureName);
	}

	return (false);
}

int32 TextureMapProcess::GetTexcoordSize(const Render::TextureObject *texture)
{
	static const char texcoordSize[Render::kTextureTargetCount] =
	{
		2, 3, 2, 3, 3
	};

	return (texcoordSize[texture->GetTextureTargetIndex()]);
}

Texture *TextureMapProcess::GetTexture(void) const
{
	if (textureObject)
	{
		return (textureObject);
	}

	textureObject = Texture::Get("");
	return (textureObject);
}

void TextureMapProcess::SetTexture(const char *name)
{
	Texture *object = textureObject;

	if (name)
	{
		if (name != &textureName[0])
		{
			textureName = name;
		}

		textureObject = Texture::Get(name);
	}
	else
	{
		textureName[0] = 0;
		textureObject = nullptr;
	}

	if (object)
	{
		object->Release();
	}
}

void TextureMapProcess::SetTexture(Texture *texture)
{
	Texture *object = textureObject;
	textureObject = texture;

	if (texture)
	{
		texture->Retain();
	}

	if (object)
	{
		object->Release();
	}

	textureName[0] = 0;
}

void TextureMapProcess::SetTexture(const TextureHeader *header, const void *image)
{
	Texture *object = textureObject;
	textureName[0] = 0;

	if (header)
	{
		textureObject = Texture::Get(header, image);
	}
	else
	{
		textureObject = nullptr;
	}

	if (object)
	{
		object->Release();
	}
}

int32 TextureMapProcess::GetPortCount(void) const
{
	return (2);
}

unsigned_int32 TextureMapProcess::GetPortFlags(int32 index) const
{
	return ((index == 0) ? 0 : kProcessPortOptional);
}

const char *TextureMapProcess::GetPortName(int32 index) const
{
	static const char *const portName[2] =
	{
		"TEXC", "BIAS"
	};

	return (portName[index]);
}

#if C4PS3 //[ PS3

	// -- PS3 code hidden --

#endif //]

int32 TextureMapProcess::GenerateProcessSignature(const ShaderCompileData *compileData, unsigned_int32 *signature) const
{
	int32 count = Process::GenerateProcessSignature(compileData, signature);

	const Texture *texture = GetTexture();
	TextureType type = texture->GetTextureType();

	if (texture->GetAlphaSemantic() == kTextureSemanticNormal)
	{
		type |= 0x80000000;
	}

	signature += count;
	signature[0] = type;
	signature[1] = 0;
	signatureUnit = &signature[1];

	return (count + 2);
}

void TextureMapProcess::GenerateProcessData(const ShaderCompileData *compileData, ProcessData *data) const
{
	data->registerCount = 1;
	data->textureCount = 1;
	data->outputSize = 4;

	const Texture *texture = GetTexture();
	data->inputSize[0] = GetTexcoordSize(texture);
	data->inputSize[1] = 1;
	data->textureObject[0] = texture;
}

int32 TextureMapProcess::GenerateShaderCode(const ShaderCompileData *compileData, const char **shaderCode) const
{
	static const char textureCode[] =
	{
		"# = %TRG0(%IMG0, %0);\n"
	};

	static const char biasCode[] =
	{
		#if C4OPENGL

			"# = %TRG0(%IMG0, %0, %1);\n"

		#elif C4PSSL //[ PS4

			// -- PS4 code hidden --

		#elif C4CG //[ PS3

			// -- PS3 code hidden --

		#endif //]
	};

	shaderCode[0] = (!GetPortRoute(1)) ? textureCode : biasCode;
	return (1);
}


ProjectiveTextureMapProcess::ProjectiveTextureMapProcess() : TextureMapProcess(kProcessProjectiveTextureMap)
{
}

ProjectiveTextureMapProcess::ProjectiveTextureMapProcess(const ProjectiveTextureMapProcess& projectiveTextureMapProcess) : TextureMapProcess(projectiveTextureMapProcess)
{
}

ProjectiveTextureMapProcess::~ProjectiveTextureMapProcess()
{
}

Process *ProjectiveTextureMapProcess::Replicate(void) const
{
	return (new ProjectiveTextureMapProcess(*this));
}

void ProjectiveTextureMapProcess::GenerateProcessData(const ShaderCompileData *compileData, ProcessData *data) const
{
	data->registerCount = 1;
	data->textureCount = 1;
	data->outputSize = 4;

	data->inputSize[0] = 3;
	data->inputSize[1] = 1;
	data->textureObject[0] = GetTexture();
}

int32 ProjectiveTextureMapProcess::GenerateShaderCode(const ShaderCompileData *compileData, const char **shaderCode) const
{
	static const char textureCode[] =
	{
		"# = %TRG0(%IMG0, %0.xy / %0.z);\n"
	};

	static const char biasCode[] =
	{
		#if C4OPENGL

			"# = %TRG0(%IMG0, %0.xy / %0.z, %1);\n"

		#elif C4PSSL //[ PS4

			// -- PS4 code hidden --

		#elif C4CG //[ PS3

			// -- PS3 code hidden --

		#endif //]
	};

	shaderCode[0] = (!GetPortRoute(1)) ? textureCode : biasCode;
	return (1);
}


NormalMapProcess::NormalMapProcess() : TextureMapProcess(kProcessNormalMap)
{
}

NormalMapProcess::NormalMapProcess(const NormalMapProcess& normalMapProcess) : TextureMapProcess(normalMapProcess)
{
}

NormalMapProcess::~NormalMapProcess()
{
}

Process *NormalMapProcess::Replicate(void) const
{
	return (new NormalMapProcess(*this));
}

int32 NormalMapProcess::GenerateShaderCode(const ShaderCompileData *compileData, const char **shaderCode) const
{
	static const char normalizeCode[] =
	{
		"# = %TRG0(%IMG0, %0);\n"
		"#.xyz = normalize(##.xyz * 2.0 - 1.0);\n"
	};

	static const char compressedCode[] =
	{
		"#.xy = %TRG0(%IMG0, %0).wy * 2.0 - 1.0;\n"
		"#.z = sqrt(max(1.0 - dot(##.xy, ##.xy), 0.03125));\n"
	};

	if (GetTexture()->GetImageFormat() == kTextureBC13)
	{
		shaderCode[0] = compressedCode;
	}
	else
	{
		shaderCode[0] = normalizeCode;
	}

	return (1);
}


ImpostorTextureProcess::ImpostorTextureProcess() : TextureMapProcess(kProcessImpostorTexture)
{
	// Hold onto the screen texture here because the ImpostorBlendProcess that really needs it
	// is always a temporary node in the graph undergoing compilation.

	screenTextureObject = Texture::Get("C4/screen");
}

ImpostorTextureProcess::ImpostorTextureProcess(ProcessType type) : TextureMapProcess(type)
{
	screenTextureObject = Texture::Get("C4/screen");
}

ImpostorTextureProcess::ImpostorTextureProcess(const ImpostorTextureProcess& impostorTextureProcess) : TextureMapProcess(impostorTextureProcess)
{
	screenTextureObject = impostorTextureProcess.screenTextureObject;
	screenTextureObject->Retain();
}

ImpostorTextureProcess::~ImpostorTextureProcess()
{
	screenTextureObject->Release();
}

Process *ImpostorTextureProcess::Replicate(void) const
{
	return (new ImpostorTextureProcess(*this));
}

int32 ImpostorTextureProcess::GetPortCount(void) const
{
	return (0);
}

int32 ImpostorTextureProcess::GenerateDerivedInterpolantTypes(const ShaderCompileData *compileData, ProcessType *type) const
{
	type[0] = kProcessImpostorBlend;
	return (1);
}

void ImpostorTextureProcess::GenerateProcessData(const ShaderCompileData *compileData, ProcessData *data) const
{
	data->registerCount = 1;
	data->temporaryCount = 1;
	data->outputSize = 4;

	data->textureCount = 1;
	data->textureObject[0] = GetTexture();
}

int32 ImpostorTextureProcess::GenerateShaderCode(const ShaderCompileData *compileData, const char **shaderCode) const
{
	static const char code[] =
	{
		"temp = %TRG0(%IMG0, $IMPT.zy);\n"
		"tmp1 = %TRG0(%IMG0, $IMPT.wy);\n"
		"# = " LERP "(temp, tmp1, ibld.x);\n"
	};

	shaderCode[0] = code;
	return (1);
}


ImpostorNormalProcess::ImpostorNormalProcess() : ImpostorTextureProcess(kProcessImpostorNormal)
{
}

ImpostorNormalProcess::ImpostorNormalProcess(const ImpostorNormalProcess& impostorNormalProcess) : ImpostorTextureProcess(impostorNormalProcess)
{
}

ImpostorNormalProcess::~ImpostorNormalProcess()
{
}

Process *ImpostorNormalProcess::Replicate(void) const
{
	return (new ImpostorNormalProcess(*this));
}

int32 ImpostorNormalProcess::GenerateShaderCode(const ShaderCompileData *compileData, const char **shaderCode) const
{
	static const char code[] =
	{
		"temp = %TRG0(%IMG0, $IMPT.zy);\n"
		"tmp1 = %TRG0(%IMG0, $IMPT.wy);\n"
		"# = " LERP "(temp, tmp1, ibld.x);\n"
		"#.xyz = ##.xyz * 2.0 - 1.0;\n"
	};

	shaderCode[0] = code;
	return (1);
}


TerrainTextureProcess::TerrainTextureProcess() : TextureMapProcess(kProcessTerrainTexture)
{
	blendMode = kTerrainBlendFull;
}

TerrainTextureProcess::TerrainTextureProcess(ProcessType type) : TextureMapProcess(type)
{
	blendMode = kTerrainBlendFull;
}

TerrainTextureProcess::TerrainTextureProcess(const TerrainTextureProcess& terrainTextureProcess) : TextureMapProcess(terrainTextureProcess)
{
	blendMode = terrainTextureProcess.blendMode;
}

TerrainTextureProcess::~TerrainTextureProcess()
{
}

Process *TerrainTextureProcess::Replicate(void) const
{
	return (new TerrainTextureProcess(*this));
}

void TerrainTextureProcess::Pack(Packer& data, unsigned_int32 packFlags) const
{
	TextureMapProcess::Pack(data, packFlags);

	data << blendMode;
}

void TerrainTextureProcess::Unpack(Unpacker& data, unsigned_int32 unpackFlags)
{
	TextureMapProcess::Unpack(data, unpackFlags);

	data >> blendMode;
}

int32 TerrainTextureProcess::GetSettingCount(void) const
{
	return (TextureMapProcess::GetSettingCount() + 1);
}

Setting *TerrainTextureProcess::GetSetting(int32 index) const
{
	int32 count = TextureMapProcess::GetSettingCount();
	if (index < count)
	{
		return (TextureMapProcess::GetSetting(index));
	}

	if (index == count)
	{
		const StringTable *table = TheInterfaceMgr->GetStringTable();
		const char *title = table->GetString(StringID('PROC', kProcessTerrainTexture, 'BMOD'));
		MenuSetting *menu = new MenuSetting('BMOD', blendMode, title, 3);

		menu->SetMenuItemString(0, table->GetString(StringID('PROC', kProcessTerrainTexture, 'BMOD', 'BLND')));
		menu->SetMenuItemString(1, table->GetString(StringID('PROC', kProcessTerrainTexture, 'BMOD', 'TEXA')));
		menu->SetMenuItemString(2, table->GetString(StringID('PROC', kProcessTerrainTexture, 'BMOD', 'TEXB')));

		return (menu);
	}

	return (nullptr);
}

void TerrainTextureProcess::SetSetting(const Setting *setting)
{
	Type identifier = setting->GetSettingIdentifier();

	if (identifier == 'BMOD')
	{
		blendMode = static_cast<const MenuSetting *>(setting)->GetMenuSelection();
	}
	else
	{
		TextureMapProcess::SetSetting(setting);
	}
}

bool TerrainTextureProcess::operator ==(const Process& process) const
{
	if (TextureMapProcess::operator ==(process))
	{
		const TerrainTextureProcess& terrainTextureProcess = static_cast<const TerrainTextureProcess&>(process);
		return (blendMode == terrainTextureProcess.blendMode);
	}

	return (false);
}

int32 TerrainTextureProcess::GetPortCount(void) const
{
	return (0);
}

int32 TerrainTextureProcess::GenerateProcessSignature(const ShaderCompileData *compileData, unsigned_int32 *signature) const
{
	int32 count = TextureMapProcess::GenerateProcessSignature(compileData, signature);
	signature[count] = blendMode;
	return (count + 1);
}

int32 TerrainTextureProcess::GenerateDerivedInterpolantTypes(const ShaderCompileData *compileData, ProcessType *type) const
{
	type[0] = kProcessTerrainTexcoord;
	type[1] = kProcessTriplanarBlend;
	return (2);
}

void TerrainTextureProcess::GenerateProcessData(const ShaderCompileData *compileData, ProcessData *data) const
{
	data->registerCount = 1;
	data->temporaryCount = 5;
	data->outputSize = 4;

	data->textureCount = 1;
	data->textureObject[0] = GetTexture();
}

int32 TerrainTextureProcess::GenerateShaderCode(const ShaderCompileData *compileData, const char **shaderCode) const
{
	static const char fullCode[] =
	{
		"temp = %TRG0(%IMG0, trc1.xzw);\n"
		"tmp1 = %TRG0(%IMG0, trc1.yzw);\n"
		"tmp2 = %TRG0(%IMG0, trc3.xyz);\n"

		"tmp3 = %TRG0(%IMG0, trc2.xzw);\n"
		"tmp4 = %TRG0(%IMG0, trc2.yzw);\n"
		"tmp5 = %TRG0(%IMG0, trc3.xyw);\n"

		"temp = temp * tbld.x + tmp1 * tbld.y + tmp2 * tbld.z;\n"
		"tmp3 = tmp3 * tbld.x + tmp4 * tbld.y + tmp5 * tbld.z;\n"
		"# = " LERP "(temp, tmp3, " FRAGMENT_COLOR0 ".z);\n"
	};

	static const char primaryCode[] =
	{
		"temp = %TRG0(%IMG0, trc1.xzw);\n"
		"tmp1 = %TRG0(%IMG0, trc1.yzw);\n"
		"tmp2 = %TRG0(%IMG0, trc3.xyz);\n"

		"# = temp * tbld.x + tmp1 * tbld.y + tmp2 * tbld.z;\n"
	};

	static const char secondaryCode[] =
	{
		"tmp3 = %TRG0(%IMG0, trc2.xzw);\n"
		"tmp4 = %TRG0(%IMG0, trc2.yzw);\n"
		"tmp5 = %TRG0(%IMG0, trc3.xyw);\n"

		"# = tmp3 * tbld.x + tmp4 * tbld.y + tmp5 * tbld.z;\n"
	};

	if (blendMode == kTerrainBlendFull)
	{
		shaderCode[0] = fullCode;
	}
	else if (blendMode == kTerrainBlendPrimary)
	{
		shaderCode[0] = primaryCode;
	}
	else
	{
		shaderCode[0] = secondaryCode;
	}

	return (1);
}


TerrainNormalProcess::TerrainNormalProcess() : TerrainTextureProcess(kProcessTerrainNormal)
{
}

TerrainNormalProcess::TerrainNormalProcess(const TerrainNormalProcess& terrainNormalProcess) : TerrainTextureProcess(terrainNormalProcess)
{
}

TerrainNormalProcess::~TerrainNormalProcess()
{
}

Process *TerrainNormalProcess::Replicate(void) const
{
	return (new TerrainNormalProcess(*this));
}

void TerrainNormalProcess::GenerateProcessData(const ShaderCompileData *compileData, ProcessData *data) const
{
	data->registerCount = 1;
	data->temporaryCount = 1;
	data->outputSize = 3;

	data->textureCount = 1;
	data->textureObject[0] = GetTexture();
}

int32 TerrainNormalProcess::GenerateShaderCode(const ShaderCompileData *compileData, const char **shaderCode) const
{
	static const char fullCode[] =
	{
		"temp.xyz = %TRG0(%IMG0, trc1.xzw).xyz * 2.0 - 1.0;\n"
		"tmp1.xyz = %TRG0(%IMG0, trc2.xzw).xyz * 2.0 - 1.0;\n"

		"# = " LERP "(temp.xyz, tmp1.xyz, " FRAGMENT_COLOR0 ".z);\n"
	};

	static const char primaryCode[] =
	{
		"# = %TRG0(%IMG0, trc1.xzw).xyz * 2.0 - 1.0;\n"
	};

	static const char secondaryCode[] =
	{
		"# = %TRG0(%IMG0, trc2.xzw).xyz * 2.0 - 1.0;\n"
	};

	static const char fullCompressedCode[] =
	{
		"temp.xy = %TRG0(%IMG0, trc1.xzw).wy * 2.0 - 1.0;\n"
		"temp.z = sqrt(max(1.0 - dot(temp.xy, temp.xy), 0.03125));\n"
		"tmp1.xy = %TRG0(%IMG0, trc2.xzw).wy * 2.0 - 1.0;\n"
		"tmp1.z = sqrt(max(1.0 - dot(tmp1.xy, tmp1.xy), 0.03125));\n"

		"# = " LERP "(temp.xyz, tmp1.xyz, " FRAGMENT_COLOR0 ".z);\n"
	};

	static const char primaryCompressedCode[] =
	{
		"#.xy = %TRG0(%IMG0, trc1.xzw).wy * 2.0 - 1.0;\n"
		"#.z = sqrt(max(1.0 - dot(##.xy, ##.xy), 0.03125));\n"
	};

	static const char secondaryCompressedCode[] =
	{
		"#.xy = %TRG0(%IMG0, trc2.xzw).wy * 2.0 - 1.0;\n"
		"#.z = sqrt(max(1.0 - dot(##.xy, ##.xy), 0.03125));\n"
	};

	const Texture *texture = GetTexture();
	if (texture->GetImageFormat() == kTextureBC13)
	{
		if (blendMode == kTerrainBlendFull)
		{
			shaderCode[0] = fullCompressedCode;
		}
		else if (blendMode == kTerrainBlendPrimary)
		{
			shaderCode[0] = primaryCompressedCode;
		}
		else
		{
			shaderCode[0] = secondaryCompressedCode;
		}
	}
	else
	{
		if (blendMode == kTerrainBlendFull)
		{
			shaderCode[0] = fullCode;
		}
		else if (blendMode == kTerrainBlendPrimary)
		{
			shaderCode[0] = primaryCode;
		}
		else
		{
			shaderCode[0] = secondaryCode;
		}
	}

	return (1);
}


TerrainNormal2Process::TerrainNormal2Process() : TerrainTextureProcess(kProcessTerrainNormal2)
{
}

TerrainNormal2Process::TerrainNormal2Process(const TerrainNormal2Process& terrainNormal2Process) : TerrainTextureProcess(terrainNormal2Process)
{
}

TerrainNormal2Process::~TerrainNormal2Process()
{
}

Process *TerrainNormal2Process::Replicate(void) const
{
	return (new TerrainNormal2Process(*this));
}

void TerrainNormal2Process::GenerateProcessData(const ShaderCompileData *compileData, ProcessData *data) const
{
	data->registerCount = 1;
	data->temporaryCount = 1;
	data->outputSize = 3;

	data->textureCount = 1;
	data->textureObject[0] = GetTexture();
}

int32 TerrainNormal2Process::GenerateShaderCode(const ShaderCompileData *compileData, const char **shaderCode) const
{
	static const char fullCode[] =
	{
		"temp.xyz = %TRG0(%IMG0, trc1.yzw).xyz * 2.0 - 1.0;\n"
		"tmp1.xyz = %TRG0(%IMG0, trc2.yzw).xyz * 2.0 - 1.0;\n"

		"# = " LERP "(temp.xyz, tmp1.xyz, " FRAGMENT_COLOR0 ".z);\n"
	};

	static const char primaryCode[] =
	{
		"# = %TRG0(%IMG0, trc1.yzw).xyz * 2.0 - 1.0;\n"
	};

	static const char secondaryCode[] =
	{
		"# = %TRG0(%IMG0, trc2.yzw).xyz * 2.0 - 1.0;\n"
	};

	static const char fullCompressedCode[] =
	{
		"temp.xy = %TRG0(%IMG0, trc1.yzw).wy * 2.0 - 1.0;\n"
		"temp.z = sqrt(max(1.0 - dot(temp.xy, temp.xy), 0.03125));\n"
		"tmp1.xy = %TRG0(%IMG0, trc2.yzw).wy * 2.0 - 1.0;\n"
		"tmp1.z = sqrt(max(1.0 - dot(tmp1.xy, tmp1.xy), 0.03125));\n"

		"# = " LERP "(temp.xyz, tmp1.xyz, " FRAGMENT_COLOR0 ".z);\n"
	};

	static const char primaryCompressedCode[] =
	{
		"#.xy = %TRG0(%IMG0, trc1.yzw).wy * 2.0 - 1.0;\n"
		"#.z = sqrt(max(1.0 - dot(##.xy, ##.xy), 0.03125));\n"
	};

	static const char secondaryCompressedCode[] =
	{
		"#.xy = %TRG0(%IMG0, trc2.yzw).wy * 2.0 - 1.0;\n"
		"#.z = sqrt(max(1.0 - dot(##.xy, ##.xy), 0.03125));\n"
	};

	const Texture *texture = GetTexture();
	if (texture->GetImageFormat() == kTextureBC13)
	{
		if (blendMode == kTerrainBlendFull)
		{
			shaderCode[0] = fullCompressedCode;
		}
		else if (blendMode == kTerrainBlendPrimary)
		{
			shaderCode[0] = primaryCompressedCode;
		}
		else
		{
			shaderCode[0] = secondaryCompressedCode;
		}
	}
	else
	{
		if (blendMode == kTerrainBlendFull)
		{
			shaderCode[0] = fullCode;
		}
		else if (blendMode == kTerrainBlendPrimary)
		{
			shaderCode[0] = primaryCode;
		}
		else
		{
			shaderCode[0] = secondaryCode;
		}
	}

	return (1);
}


TerrainNormal3Process::TerrainNormal3Process() : TerrainTextureProcess(kProcessTerrainNormal3)
{
}

TerrainNormal3Process::TerrainNormal3Process(const TerrainNormal3Process& terrainNormal3Process) : TerrainTextureProcess(terrainNormal3Process)
{
}

TerrainNormal3Process::~TerrainNormal3Process()
{
}

Process *TerrainNormal3Process::Replicate(void) const
{
	return (new TerrainNormal3Process(*this));
}

void TerrainNormal3Process::GenerateProcessData(const ShaderCompileData *compileData, ProcessData *data) const
{
	data->registerCount = 1;
	data->temporaryCount = 1;
	data->outputSize = 3;

	data->textureCount = 1;
	data->textureObject[0] = GetTexture();
}

int32 TerrainNormal3Process::GenerateShaderCode(const ShaderCompileData *compileData, const char **shaderCode) const
{
	static const char fullCode[] =
	{
		"temp.xyz = %TRG0(%IMG0, trc3.xyz).xyz * 2.0 - 1.0;\n"
		"tmp1.xyz = %TRG0(%IMG0, trc3.xyw).xyz * 2.0 - 1.0;\n"

		"# = " LERP "(temp.xyz, tmp1.xyz, " FRAGMENT_COLOR0 ".z);\n"
	};

	static const char primaryCode[] =
	{
		"# = %TRG0(%IMG0, trc3.xyz).xyz * 2.0 - 1.0;\n"
	};

	static const char secondaryCode[] =
	{
		"# = %TRG0(%IMG0, trc3.xyw).xyz * 2.0 - 1.0;\n"
	};

	static const char fullCompressedCode[] =
	{
		"temp.xy = %TRG0(%IMG0, trc3.xyz).wy * 2.0 - 1.0;\n"
		"temp.z = sqrt(max(1.0 - dot(temp.xy, temp.xy), 0.03125));\n"
		"tmp1.xy = %TRG0(%IMG0, trc3.xyw).wy * 2.0 - 1.0;\n"
		"tmp1.z = sqrt(max(1.0 - dot(tmp1.xy, tmp1.xy), 0.03125));\n"

		"# = " LERP "(temp.xyz, tmp1.xyz, " FRAGMENT_COLOR0 ".z);\n"
	};

	static const char primaryCompressedCode[] =
	{
		"#.xy = %TRG0(%IMG0, trc3.xyz).wy * 2.0 - 1.0;\n"
		"#.z = sqrt(max(1.0 - dot(##.xy, ##.xy), 0.03125));\n"
	};

	static const char secondaryCompressedCode[] =
	{
		"#.xy = %TRG0(%IMG0, trc3.xyw).wy * 2.0 - 1.0;\n"
		"#.z = sqrt(max(1.0 - dot(##.xy, ##.xy), 0.03125));\n"
	};

	const Texture *texture = GetTexture();
	if (texture->GetImageFormat() == kTextureBC13)
	{
		if (blendMode == kTerrainBlendFull)
		{
			shaderCode[0] = fullCompressedCode;
		}
		else if (blendMode == kTerrainBlendPrimary)
		{
			shaderCode[0] = primaryCompressedCode;
		}
		else
		{
			shaderCode[0] = secondaryCompressedCode;
		}
	}
	else
	{
		if (blendMode == kTerrainBlendFull)
		{
			shaderCode[0] = fullCode;
		}
		else if (blendMode == kTerrainBlendPrimary)
		{
			shaderCode[0] = primaryCode;
		}
		else
		{
			shaderCode[0] = secondaryCode;
		}
	}

	return (1);
}


Merge2Process::Merge2Process() : Process(kProcessMerge2)
{
}

Merge2Process::Merge2Process(const Merge2Process& merge2Process) : Process(merge2Process)
{
}

Merge2Process::~Merge2Process()
{
}

Process *Merge2Process::Replicate(void) const
{
	return (new Merge2Process(*this));
}

int32 Merge2Process::GetPortCount(void) const
{
	return (2);
}

const char *Merge2Process::GetPortName(int32 index) const
{
	static const char *const portName[2] =
	{
		"x", "y"
	};

	return (portName[index]);
}

void Merge2Process::GenerateProcessData(const ShaderCompileData *compileData, ProcessData *data) const
{
	data->preregisterCount = 1;
	data->outputSize = 2;

	data->inputSize[0] = 1;
	data->inputSize[1] = 1;
}

int32 Merge2Process::GenerateShaderCode(const ShaderCompileData *compileData, const char **shaderCode) const
{
	static const char code[] =
	{
		"#.x = %0.x;\n"
		"#.y = %1.y;\n"
	};

	shaderCode[0] = code;
	return (1);
}


Merge3Process::Merge3Process() : Process(kProcessMerge3)
{
}

Merge3Process::Merge3Process(const Merge3Process& merge3Process) : Process(merge3Process)
{
}

Merge3Process::~Merge3Process()
{
}

Process *Merge3Process::Replicate(void) const
{
	return (new Merge3Process(*this));
}

int32 Merge3Process::GetPortCount(void) const
{
	return (3);
}

const char *Merge3Process::GetPortName(int32 index) const
{
	static const char *const portName[3] =
	{
		"x", "y", "z"
	};

	return (portName[index]);
}

void Merge3Process::GenerateProcessData(const ShaderCompileData *compileData, ProcessData *data) const
{
	data->preregisterCount = 1;
	data->outputSize = 3;

	data->inputSize[0] = 1;
	data->inputSize[1] = 1;
	data->inputSize[2] = 1;
}

int32 Merge3Process::GenerateShaderCode(const ShaderCompileData *compileData, const char **shaderCode) const
{
	static const char code[] =
	{
		"#.x = %0.x;\n"
		"#.y = %1.y;\n"
		"#.z = %2.z;\n"
	};

	shaderCode[0] = code;
	return (1);
}


Merge4Process::Merge4Process() : Process(kProcessMerge4)
{
}

Merge4Process::Merge4Process(const Merge4Process& merge4Process) : Process(merge4Process)
{
}

Merge4Process::~Merge4Process()
{
}

Process *Merge4Process::Replicate(void) const
{
	return (new Merge4Process(*this));
}

int32 Merge4Process::GetPortCount(void) const
{
	return (4);
}

const char *Merge4Process::GetPortName(int32 index) const
{
	static const char *const portName[4] =
	{
		"x", "y", "z", "w"
	};

	return (portName[index]);
}

void Merge4Process::GenerateProcessData(const ShaderCompileData *compileData, ProcessData *data) const
{
	data->preregisterCount = 1;
	data->outputSize = 4;

	data->inputSize[0] = 1;
	data->inputSize[1] = 1;
	data->inputSize[2] = 1;
	data->inputSize[3] = 1;
}

int32 Merge4Process::GenerateShaderCode(const ShaderCompileData *compileData, const char **shaderCode) const
{
	static const char code[] =
	{
		"#.x = %0.x;\n"
		"#.y = %1.y;\n"
		"#.z = %2.z;\n"
		"#.w = %3.w;\n"
	};

	shaderCode[0] = code;
	return (1);
}


InterpolantProcess::InterpolantProcess(ProcessType type) : Process(type)
{
	SetBaseProcessType(kProcessInterpolant);
}

InterpolantProcess::InterpolantProcess(const InterpolantProcess& interpolantProcess) : Process(interpolantProcess)
{
}

InterpolantProcess::~InterpolantProcess()
{
}

int32 InterpolantProcess::GetInterpolantSize(Type type)
{
	switch (type)
	{
		case 'FOGK':
		case 'FDTV':
		case 'FDTP':
		case 'DDEP':
		case 'IDEP':
		case 'IBLD':
		case 'IXBL':
		case 'TL2X':
		case 'TL2Y':
		case 'TV2X':
		case 'TV2Y':

			return (1);

		case 'TEX0':
		case 'TEX1':
		case 'PTXC':
		case 'FIR2':
		case 'IRAD':
		case 'ISRD':
		case 'GITX':

			return (2);

		case 'RTXC':
		case 'TERA':
		case 'POSI':
		case 'NRML':
		case 'TANG':
		case 'BTNG':
		case 'WPOS':
		case 'WNRM':
		case 'WTAN':
		case 'WBTN':
		case 'NRMC':
		case 'GEOM':
		case 'LDIR':
		case 'VDIR':
		case 'OLDR':
		case 'OVDR':
		case 'TLDR':
		case 'TVDR':
		case 'TWNM':
		case 'TWT1':
		case 'TWT2':
		case 'FIRE':
		case 'ATTN':
		case 'PROJ':
		case 'RPOS':
		case 'RNRM':
		case 'RTNG':
		case 'SHAD':
		case 'CSCD':
		case 'SPOT':

			return (3);

		case 'RTGH':
		case 'IMPT':
		case 'FIR1':
		case 'WARP':
		case 'RGHT':
		case 'DOWN':
		case 'VELA':
		case 'VELB':

			return (4);
	}

	return (0);
}

int32 InterpolantProcess::GetInterpolantName(Type type, const ShaderAllocationData *allocData, char *name, SwizzleData *swizzleData)
{
	static const char *const texcoordName[kMaxShaderTexcoordCount] =
	{
		#if C4OPENGL

			"texcoord[0]", "texcoord[1]", "texcoord[2]", "texcoord[3]", "texcoord[4]", "texcoord[5]", "texcoord[6]",
			"texcoord[7]", "texcoord[8]", "texcoord[9]", "texcoord[10]", "texcoord[11]", "texcoord[12]"

		#elif C4PSSL //[ PS4

			// -- PS4 code hidden --

		#elif C4CG //[ PS3

			// -- PS3 code hidden --

		#endif //]
	};

	int32 count = allocData->interpolantCount;
	const InterpolantData *interpolantData = allocData->interpolantData;

	for (machine a = 0; a < count; a++)
	{
		if (interpolantData->interpolantType == type)
		{
			int32 len = Text::CopyText(texcoordName[interpolantData->texcoordIndex], name);
			name += len;

			if (swizzleData)
			{
				int32 size = swizzleData->size;
				for (machine b = 0; b < size; b++)
				{
					swizzleData->component[b] = interpolantData->swizzleData.component[swizzleData->component[b]];
				}

				return (Process::PostgenerateOutputIdentifier(swizzleData, name) + len);
			}

			return (Process::PostgenerateOutputIdentifier(&interpolantData->swizzleData, name) + len);
		}

		interpolantData++;
	}

	Fatal("InterpolantProcess::GetInterpolantName(), missing interpolant\n");
	return (0);
}

void InterpolantProcess::GenerateProcessData(const ShaderCompileData *compileData, ProcessData *data) const
{
	data->outputSize = GetInterpolantSize(GetProcessType());

	data->interpolantCount = 1;
	data->interpolantType[0] = GetProcessType();
}

int32 InterpolantProcess::GenerateOutputIdentifier(const ShaderCompileData *compileData, const ShaderAllocationData *allocData, SwizzleData *swizzleData, char *name) const
{
	int32 len = PregenerateOutputIdentifier(swizzleData, name);
	return (GetInterpolantName(GetProcessType(), allocData, name + len, swizzleData) + len);
}


Texcoord0Process::Texcoord0Process() : InterpolantProcess(kProcessTexcoord0)
{
}

Texcoord0Process::Texcoord0Process(const Texcoord0Process& texcoord0Process) : InterpolantProcess(texcoord0Process)
{
}

Texcoord0Process::~Texcoord0Process()
{
}

Process *Texcoord0Process::Replicate(void) const
{
	return (new Texcoord0Process(*this));
}

#if C4OPENGL

	void Texcoord0Process::GenerateProcessData(const ShaderCompileData *compileData, ProcessData *data) const
	{
		if (compileData->renderable->GetRenderType() != kRenderPointSprites)
		{
			InterpolantProcess::GenerateProcessData(compileData, data);
		}
		else
		{
			data->outputSize = 2;
		}
	}

	int32 Texcoord0Process::GenerateOutputIdentifier(const ShaderCompileData *compileData, const ShaderAllocationData *allocData, SwizzleData *swizzleData, char *name) const
	{
		if (compileData->renderable->GetRenderType() != kRenderPointSprites)
		{
			return (InterpolantProcess::GenerateOutputIdentifier(compileData, allocData, swizzleData, name));
		}

		int32 len = PregenerateOutputIdentifier(swizzleData, name);
		len += Text::CopyText("gl_PointCoord", name + len);
		return (Process::PostgenerateOutputIdentifier(swizzleData, name + len) + len);
	}

#endif


Texcoord1Process::Texcoord1Process() : InterpolantProcess(kProcessTexcoord1)
{
}

Texcoord1Process::Texcoord1Process(const Texcoord1Process& texcoord1Process) : InterpolantProcess(texcoord1Process)
{
}

Texcoord1Process::~Texcoord1Process()
{
}

Process *Texcoord1Process::Replicate(void) const
{
	return (new Texcoord1Process(*this));
}


RawTexcoordProcess::RawTexcoordProcess() : InterpolantProcess(kProcessRawTexcoord)
{
}

RawTexcoordProcess::RawTexcoordProcess(const RawTexcoordProcess& rawTexcoordProcess) : InterpolantProcess(rawTexcoordProcess)
{
}

RawTexcoordProcess::~RawTexcoordProcess()
{
}

Process *RawTexcoordProcess::Replicate(void) const
{
	return (new RawTexcoordProcess(*this));
}


ImpostorTexcoordProcess::ImpostorTexcoordProcess() : InterpolantProcess(kProcessImpostorTexcoord)
{
}

ImpostorTexcoordProcess::ImpostorTexcoordProcess(const ImpostorTexcoordProcess& impostorTexcoordProcess) : InterpolantProcess(impostorTexcoordProcess)
{
}

ImpostorTexcoordProcess::~ImpostorTexcoordProcess()
{
}

Process *ImpostorTexcoordProcess::Replicate(void) const
{
	return (new ImpostorTexcoordProcess(*this));
}


ImpostorBlendProcess::ImpostorBlendProcess() : InterpolantProcess(kProcessImpostorBlend)
{
	SetBaseProcessType(kProcessDerived);

	textureObject = Texture::Get("C4/screen");
}

ImpostorBlendProcess::ImpostorBlendProcess(const ImpostorBlendProcess& impostorBlendProcess) : InterpolantProcess(impostorBlendProcess)
{
	textureObject = impostorBlendProcess.textureObject;
	textureObject->Retain();
}

ImpostorBlendProcess::~ImpostorBlendProcess()
{
	textureObject->Release();
}

Process *ImpostorBlendProcess::Replicate(void) const
{
	return (new ImpostorBlendProcess(*this));
}

void ImpostorBlendProcess::GenerateProcessData(const ShaderCompileData *compileData, ProcessData *data) const
{
	data->outputSize = 2;

	data->interpolantCount = 3;
	data->interpolantType[0] = 'IMPT';
	data->interpolantType[1] = 'IBLD';
	data->interpolantType[2] = 'IXBL';

	data->textureCount = 1;
	data->textureObject[0] = textureObject;
}

int32 ImpostorBlendProcess::GenerateShaderCode(const ShaderCompileData *compileData, const char **shaderCode) const
{
	static const char code[] =
	{
		FLOAT2 " ibld;\n"

		"ibld.y = %TRG0(%IMG0, $IMPT.xy).x;\n"
		"ibld.x = float(ibld.y < $IBLD);\n"
	};

	shaderCode[0] = code;
	return (1);
}


TerrainTexcoordProcess::TerrainTexcoordProcess() : InterpolantProcess(kProcessTerrainTexcoord)
{
	SetBaseProcessType(kProcessDerived);
}

TerrainTexcoordProcess::TerrainTexcoordProcess(const TerrainTexcoordProcess& terrainTexcoordProcess) : InterpolantProcess(terrainTexcoordProcess)
{
}

TerrainTexcoordProcess::~TerrainTexcoordProcess()
{
}

Process *TerrainTexcoordProcess::Replicate(void) const
{
	return (new TerrainTexcoordProcess(*this));
}

void TerrainTexcoordProcess::GenerateProcessData(const ShaderCompileData *compileData, ProcessData *data) const
{
	compileData->shaderSourceFlags |= kShaderSourcePrimaryColor | kShaderSourceSecondaryColor;

	data->temporaryCount = 4;

	data->interpolantCount = 2;
	data->interpolantType[0] = 'TERA';
	data->interpolantType[1] = 'NRML';
}

int32 TerrainTexcoordProcess::GenerateShaderCode(const ShaderCompileData *compileData, const char **shaderCode) const
{
	static const char selectCode[] =
	{
		#if C4OPENGL

			"vec4 trc1, trc2, trc3;\n"

			"temp.xz = vec2($NRML.x < 0.0, $NRML.z < 0.0);\n"
			"temp.y = float($NRML.y >= 0.0);\n"
			"vec2 vert = mix(" FRAGMENT_COLOR1 ".xz, " FRAGMENT_COLOR1 ".yw, temp.z);\n"

		#elif C4PSSL //[ PS4

			// -- PS4 code hidden --

		#endif //]
	};

	static const char arrayCode[] =
	{
		"trc1.xy = " LERP "($TERA.yx, -$TERA.yx, temp.xy);\n"
		"trc1.z = $TERA.z;\n"
		"trc1.w = " FRAGMENT_COLOR0 ".x * 255.0;\n"

		"trc2.xyz = trc1.xyz;\n"
		"trc2.w = " FRAGMENT_COLOR0 ".y * 255.0;\n"

		"trc3.x = " LERP "($TERA.x, -$TERA.x, temp.z);\n"
		"trc3.y = $TERA.y;\n"
		"trc3.zw = vert * 255.0;\n"
	};

	shaderCode[0] = selectCode;
	shaderCode[1] = arrayCode;
	return (2);
}


TriplanarBlendProcess::TriplanarBlendProcess() : InterpolantProcess(kProcessTriplanarBlend)
{
	SetBaseProcessType(kProcessDerived);
}

TriplanarBlendProcess::TriplanarBlendProcess(const TriplanarBlendProcess& triplanarBlendProcess) : InterpolantProcess(triplanarBlendProcess)
{
}

TriplanarBlendProcess::~TriplanarBlendProcess()
{
}

Process *TriplanarBlendProcess::Replicate(void) const
{
	return (new TriplanarBlendProcess(*this));
}

void TriplanarBlendProcess::GenerateProcessData(const ShaderCompileData *compileData, ProcessData *data) const
{
	data->interpolantCount = 1;
	data->interpolantType[0] = 'NRML';
}

int32 TriplanarBlendProcess::GenerateShaderCode(const ShaderCompileData *compileData, const char **shaderCode) const
{
	static const char code[] =
	{
		#if C4OPENGL

			"vec3 tbld = clamp(abs(normalize($NRML)) - 0.5, 0.0, 1.0);\n"

		#elif C4PSSL //[ PS4

			// -- PS4 code hidden --

		#endif //]

		"tbld *= tbld;\n"
		"tbld *= tbld;\n"
		"tbld /= dot(tbld, " FLOAT3 "(1.0, 1.0, 1.0));\n"
	};

	shaderCode[0] = code;
	return (1);
}


VertexGeometryProcess::VertexGeometryProcess() : InterpolantProcess(kProcessVertexGeometry)
{
}

VertexGeometryProcess::VertexGeometryProcess(const VertexGeometryProcess& vertexGeometryProcess) : InterpolantProcess(vertexGeometryProcess)
{
}

VertexGeometryProcess::~VertexGeometryProcess()
{
}

Process *VertexGeometryProcess::Replicate(void) const
{
	return (new VertexGeometryProcess(*this));
}


ObjectPositionProcess::ObjectPositionProcess() : InterpolantProcess(kProcessObjectPosition)
{
}

ObjectPositionProcess::ObjectPositionProcess(const ObjectPositionProcess& objectPositionProcess) : InterpolantProcess(objectPositionProcess)
{
}

ObjectPositionProcess::~ObjectPositionProcess()
{
}

Process *ObjectPositionProcess::Replicate(void) const
{
	return (new ObjectPositionProcess(*this));
}


WorldPositionProcess::WorldPositionProcess() : InterpolantProcess(kProcessWorldPosition)
{
}

WorldPositionProcess::WorldPositionProcess(const WorldPositionProcess& worldPositionProcess) : InterpolantProcess(worldPositionProcess)
{
}

WorldPositionProcess::~WorldPositionProcess()
{
}

Process *WorldPositionProcess::Replicate(void) const
{
	return (new WorldPositionProcess(*this));
}


ObjectNormalProcess::ObjectNormalProcess() : InterpolantProcess(kProcessObjectNormal)
{
	SetBaseProcessType(kProcessDerived);
}

ObjectNormalProcess::ObjectNormalProcess(const ObjectNormalProcess& objectNormalProcess) : InterpolantProcess(objectNormalProcess)
{
}

ObjectNormalProcess::~ObjectNormalProcess()
{
}

Process *ObjectNormalProcess::Replicate(void) const
{
	return (new ObjectNormalProcess(*this));
}

int32 ObjectNormalProcess::GenerateOutputIdentifier(const ShaderCompileData *compileData, const ShaderAllocationData *allocData, SwizzleData *swizzleData, char *name) const
{
	int32 len = PregenerateOutputIdentifier(swizzleData, name);
	name += len;

	Text::CopyText("onrm", name);
	return (PostgenerateOutputIdentifier(swizzleData, name + 4) + (len + 4));
}

int32 ObjectNormalProcess::GenerateShaderCode(const ShaderCompileData *compileData, const char **shaderCode) const
{
	static const char code[] =
	{
		HALF3 " onrm = normalize($NRML);\n"
	};

	shaderCode[0] = code;
	return (1);
}


ObjectTangentProcess::ObjectTangentProcess() : InterpolantProcess(kProcessObjectTangent)
{
	SetBaseProcessType(kProcessDerived);
}

ObjectTangentProcess::ObjectTangentProcess(const ObjectTangentProcess& objectTangentProcess) : InterpolantProcess(objectTangentProcess)
{
}

ObjectTangentProcess::~ObjectTangentProcess()
{
}

Process *ObjectTangentProcess::Replicate(void) const
{
	return (new ObjectTangentProcess(*this));
}

int32 ObjectTangentProcess::GenerateOutputIdentifier(const ShaderCompileData *compileData, const ShaderAllocationData *allocData, SwizzleData *swizzleData, char *name) const
{
	int32 len = PregenerateOutputIdentifier(swizzleData, name);
	name += len;

	Text::CopyText("otan", name);
	return (PostgenerateOutputIdentifier(swizzleData, name + 4) + (len + 4));
}

int32 ObjectTangentProcess::GenerateShaderCode(const ShaderCompileData *compileData, const char **shaderCode) const
{
	static const char code[] =
	{
		HALF3 " otan = normalize($TANG);\n"
	};

	shaderCode[0] = code;
	return (1);
}


ObjectBitangentProcess::ObjectBitangentProcess() : InterpolantProcess(kProcessObjectBitangent)
{
	SetBaseProcessType(kProcessDerived);
}

ObjectBitangentProcess::ObjectBitangentProcess(const ObjectBitangentProcess& objectBitangentProcess) : InterpolantProcess(objectBitangentProcess)
{
}

ObjectBitangentProcess::~ObjectBitangentProcess()
{
}

Process *ObjectBitangentProcess::Replicate(void) const
{
	return (new ObjectBitangentProcess(*this));
}

int32 ObjectBitangentProcess::GenerateOutputIdentifier(const ShaderCompileData *compileData, const ShaderAllocationData *allocData, SwizzleData *swizzleData, char *name) const
{
	int32 len = PregenerateOutputIdentifier(swizzleData, name);
	name += len;

	Text::CopyText("obtn", name);
	return (PostgenerateOutputIdentifier(swizzleData, name + 4) + (len + 4));
}

int32 ObjectBitangentProcess::GenerateShaderCode(const ShaderCompileData *compileData, const char **shaderCode) const
{
	static const char code[] =
	{
		HALF3 " obtn = normalize($BTNG);\n"
	};

	shaderCode[0] = code;
	return (1);
}


WorldNormalProcess::WorldNormalProcess() : InterpolantProcess(kProcessWorldNormal)
{
	SetBaseProcessType(kProcessDerived);
}

WorldNormalProcess::WorldNormalProcess(const WorldNormalProcess& worldNormalProcess) : InterpolantProcess(worldNormalProcess)
{
}

WorldNormalProcess::~WorldNormalProcess()
{
}

Process *WorldNormalProcess::Replicate(void) const
{
	return (new WorldNormalProcess(*this));
}

int32 WorldNormalProcess::GenerateOutputIdentifier(const ShaderCompileData *compileData, const ShaderAllocationData *allocData, SwizzleData *swizzleData, char *name) const
{
	int32 len = PregenerateOutputIdentifier(swizzleData, name);
	name += len;

	Text::CopyText("wnrm", name);
	return (PostgenerateOutputIdentifier(swizzleData, name + 4) + (len + 4));
}

int32 WorldNormalProcess::GenerateShaderCode(const ShaderCompileData *compileData, const char **shaderCode) const
{
	static const char code[] =
	{
		HALF3 " wnrm = normalize($WNRM);\n"
	};

	shaderCode[0] = code;
	return (1);
}


WorldTangentProcess::WorldTangentProcess() : InterpolantProcess(kProcessWorldTangent)
{
	SetBaseProcessType(kProcessDerived);
}

WorldTangentProcess::WorldTangentProcess(const WorldTangentProcess& worldTangentProcess) : InterpolantProcess(worldTangentProcess)
{
}

WorldTangentProcess::~WorldTangentProcess()
{
}

Process *WorldTangentProcess::Replicate(void) const
{
	return (new WorldTangentProcess(*this));
}

int32 WorldTangentProcess::GenerateOutputIdentifier(const ShaderCompileData *compileData, const ShaderAllocationData *allocData, SwizzleData *swizzleData, char *name) const
{
	int32 len = PregenerateOutputIdentifier(swizzleData, name);
	name += len;

	Text::CopyText("wtan", name);
	return (PostgenerateOutputIdentifier(swizzleData, name + 4) + (len + 4));
}

int32 WorldTangentProcess::GenerateShaderCode(const ShaderCompileData *compileData, const char **shaderCode) const
{
	static const char code[] =
	{
		HALF3 " wtan = normalize($WTAN);\n"
	};

	shaderCode[0] = code;
	return (1);
}


WorldBitangentProcess::WorldBitangentProcess() : InterpolantProcess(kProcessWorldBitangent)
{
	SetBaseProcessType(kProcessDerived);
}

WorldBitangentProcess::WorldBitangentProcess(const WorldBitangentProcess& worldBitangentProcess) : InterpolantProcess(worldBitangentProcess)
{
}

WorldBitangentProcess::~WorldBitangentProcess()
{
}

Process *WorldBitangentProcess::Replicate(void) const
{
	return (new WorldBitangentProcess(*this));
}

int32 WorldBitangentProcess::GenerateOutputIdentifier(const ShaderCompileData *compileData, const ShaderAllocationData *allocData, SwizzleData *swizzleData, char *name) const
{
	int32 len = PregenerateOutputIdentifier(swizzleData, name);
	name += len;

	Text::CopyText("wbtn", name);
	return (PostgenerateOutputIdentifier(swizzleData, name + 4) + (len + 4));
}

int32 WorldBitangentProcess::GenerateShaderCode(const ShaderCompileData *compileData, const char **shaderCode) const
{
	static const char code[] =
	{
		HALF3 " wbtn = normalize($WBTN);\n"
	};

	shaderCode[0] = code;
	return (1);
}


TangentLightDirectionProcess::TangentLightDirectionProcess() : InterpolantProcess(kProcessTangentLightDirection)
{
	SetBaseProcessType(kProcessDerived);
}

TangentLightDirectionProcess::TangentLightDirectionProcess(const TangentLightDirectionProcess& tangentLightDirectionProcess) : InterpolantProcess(tangentLightDirectionProcess)
{
}

TangentLightDirectionProcess::~TangentLightDirectionProcess()
{
}

Process *TangentLightDirectionProcess::Replicate(void) const
{
	return (new TangentLightDirectionProcess(*this));
}

int32 TangentLightDirectionProcess::GenerateOutputIdentifier(const ShaderCompileData *compileData, const ShaderAllocationData *allocData, SwizzleData *swizzleData, char *name) const
{
	int32 len = PregenerateOutputIdentifier(swizzleData, name);
	name += len;

	Text::CopyText("ldir", name);
	return (PostgenerateOutputIdentifier(swizzleData, name + 4) + (len + 4));
}

int32 TangentLightDirectionProcess::GenerateShaderCode(const ShaderCompileData *compileData, const char **shaderCode) const
{
	static const char code[] =
	{
		HALF3 " ldir = normalize($LDIR);\n"
	};

	shaderCode[0] = code;
	return (1);
}


TangentViewDirectionProcess::TangentViewDirectionProcess() : InterpolantProcess(kProcessTangentViewDirection)
{
	SetBaseProcessType(kProcessDerived);
}

TangentViewDirectionProcess::TangentViewDirectionProcess(const TangentViewDirectionProcess& tangentViewDirectionProcess) : InterpolantProcess(tangentViewDirectionProcess)
{
}

TangentViewDirectionProcess::~TangentViewDirectionProcess()
{
}

Process *TangentViewDirectionProcess::Replicate(void) const
{
	return (new TangentViewDirectionProcess(*this));
}

int32 TangentViewDirectionProcess::GenerateDerivedInterpolantTypes(const ShaderCompileData *compileData, ProcessType *type) const
{
	compileData->shaderSourceFlags |= kShaderSourceTangentViewDirection;
	return (0);
}

int32 TangentViewDirectionProcess::GenerateOutputIdentifier(const ShaderCompileData *compileData, const ShaderAllocationData *allocData, SwizzleData *swizzleData, char *name) const
{
	int32 len = PregenerateOutputIdentifier(swizzleData, name);
	name += len;

	Text::CopyText("vdir", name);
	return (PostgenerateOutputIdentifier(swizzleData, name + 4) + (len + 4));
}

int32 TangentViewDirectionProcess::GenerateShaderCode(const ShaderCompileData *compileData, const char **shaderCode) const
{
	static const char code[] =
	{
		HALF3 " vdir = normalize($VDIR);\n"
	};

	shaderCode[0] = code;
	return (1);
}


TangentHalfwayDirectionProcess::TangentHalfwayDirectionProcess() : InterpolantProcess(kProcessTangentHalfwayDirection)
{
	SetBaseProcessType(kProcessDerived);
}

TangentHalfwayDirectionProcess::TangentHalfwayDirectionProcess(const TangentHalfwayDirectionProcess& tangentHalfwayDirectionProcess) : InterpolantProcess(tangentHalfwayDirectionProcess)
{
}

TangentHalfwayDirectionProcess::~TangentHalfwayDirectionProcess()
{
}

Process *TangentHalfwayDirectionProcess::Replicate(void) const
{
	return (new TangentHalfwayDirectionProcess(*this));
}

int32 TangentHalfwayDirectionProcess::GenerateDerivedInterpolantTypes(const ShaderCompileData *compileData, ProcessType *type) const
{
	type[0] = kProcessTangentLightDirection;
	type[1] = kProcessTangentViewDirection;
	return (2);
}

void TangentHalfwayDirectionProcess::GenerateProcessData(const ShaderCompileData *compileData, ProcessData *data) const
{
	data->outputSize = 3;
}

int32 TangentHalfwayDirectionProcess::GenerateOutputIdentifier(const ShaderCompileData *compileData, const ShaderAllocationData *allocData, SwizzleData *swizzleData, char *name) const
{
	int32 len = PregenerateOutputIdentifier(swizzleData, name);
	name += len;

	Text::CopyText("hdir", name);
	return (PostgenerateOutputIdentifier(swizzleData, name + 4) + (len + 4));
}

int32 TangentHalfwayDirectionProcess::GenerateShaderCode(const ShaderCompileData *compileData, const char **shaderCode) const
{
	static const char code[] =
	{
		HALF3 " hdir = normalize(ldir + vdir);\n"
	};

	shaderCode[0] = code;
	return (1);
}


ObjectLightDirectionProcess::ObjectLightDirectionProcess() : InterpolantProcess(kProcessObjectLightDirection)
{
	SetBaseProcessType(kProcessDerived);
}

ObjectLightDirectionProcess::ObjectLightDirectionProcess(const ObjectLightDirectionProcess& objectLightDirectionProcess) : InterpolantProcess(objectLightDirectionProcess)
{
}

ObjectLightDirectionProcess::~ObjectLightDirectionProcess()
{
}

Process *ObjectLightDirectionProcess::Replicate(void) const
{
	return (new ObjectLightDirectionProcess(*this));
}

int32 ObjectLightDirectionProcess::GenerateOutputIdentifier(const ShaderCompileData *compileData, const ShaderAllocationData *allocData, SwizzleData *swizzleData, char *name) const
{
	int32 len = PregenerateOutputIdentifier(swizzleData, name);
	name += len;

	Text::CopyText("oldr", name);
	return (PostgenerateOutputIdentifier(swizzleData, name + 4) + (len + 4));
}

int32 ObjectLightDirectionProcess::GenerateShaderCode(const ShaderCompileData *compileData, const char **shaderCode) const
{
	static const char code[] =
	{
		HALF3 " oldr = normalize($OLDR);\n"
	};

	shaderCode[0] = code;
	return (1);
}


ObjectViewDirectionProcess::ObjectViewDirectionProcess() : InterpolantProcess(kProcessObjectViewDirection)
{
	SetBaseProcessType(kProcessDerived);
}

ObjectViewDirectionProcess::ObjectViewDirectionProcess(const ObjectViewDirectionProcess& objectViewDirectionProcess) : InterpolantProcess(objectViewDirectionProcess)
{
}

ObjectViewDirectionProcess::~ObjectViewDirectionProcess()
{
}

Process *ObjectViewDirectionProcess::Replicate(void) const
{
	return (new ObjectViewDirectionProcess(*this));
}

int32 ObjectViewDirectionProcess::GenerateOutputIdentifier(const ShaderCompileData *compileData, const ShaderAllocationData *allocData, SwizzleData *swizzleData, char *name) const
{
	int32 len = PregenerateOutputIdentifier(swizzleData, name);
	name += len;

	Text::CopyText("ovdr", name);
	return (PostgenerateOutputIdentifier(swizzleData, name + 4) + (len + 4));
}

int32 ObjectViewDirectionProcess::GenerateShaderCode(const ShaderCompileData *compileData, const char **shaderCode) const
{
	static const char code[] =
	{
		HALF3 " ovdr = normalize($OVDR);\n"
	};

	shaderCode[0] = code;
	return (1);
}


ObjectHalfwayDirectionProcess::ObjectHalfwayDirectionProcess() : InterpolantProcess(kProcessObjectHalfwayDirection)
{
	SetBaseProcessType(kProcessDerived);
}

ObjectHalfwayDirectionProcess::ObjectHalfwayDirectionProcess(const ObjectHalfwayDirectionProcess& objectHalfwayDirectionProcess) : InterpolantProcess(objectHalfwayDirectionProcess)
{
}

ObjectHalfwayDirectionProcess::~ObjectHalfwayDirectionProcess()
{
}

Process *ObjectHalfwayDirectionProcess::Replicate(void) const
{
	return (new ObjectHalfwayDirectionProcess(*this));
}

int32 ObjectHalfwayDirectionProcess::GenerateDerivedInterpolantTypes(const ShaderCompileData *compileData, ProcessType *type) const
{
	type[0] = kProcessObjectLightDirection;
	type[1] = kProcessObjectViewDirection;
	return (2);
}

void ObjectHalfwayDirectionProcess::GenerateProcessData(const ShaderCompileData *compileData, ProcessData *data) const
{
	data->outputSize = 3;
}

int32 ObjectHalfwayDirectionProcess::GenerateOutputIdentifier(const ShaderCompileData *compileData, const ShaderAllocationData *allocData, SwizzleData *swizzleData, char *name) const
{
	int32 len = PregenerateOutputIdentifier(swizzleData, name);
	name += len;

	Text::CopyText("ohdr", name);
	return (PostgenerateOutputIdentifier(swizzleData, name + 4) + (len + 4));
}

int32 ObjectHalfwayDirectionProcess::GenerateShaderCode(const ShaderCompileData *compileData, const char **shaderCode) const
{
	static const char code[] =
	{
		HALF3 " ohdr = normalize(oldr + ovdr);\n"
	};

	shaderCode[0] = code;
	return (1);
}


TerrainLightDirectionProcess::TerrainLightDirectionProcess() : InterpolantProcess(kProcessTerrainLightDirection)
{
	SetBaseProcessType(kProcessDerived);
}

TerrainLightDirectionProcess::TerrainLightDirectionProcess(const TerrainLightDirectionProcess& terrainLightDirectionProcess) : InterpolantProcess(terrainLightDirectionProcess)
{
}

TerrainLightDirectionProcess::~TerrainLightDirectionProcess()
{
}

Process *TerrainLightDirectionProcess::Replicate(void) const
{
	return (new TerrainLightDirectionProcess(*this));
}

void TerrainLightDirectionProcess::GenerateProcessData(const ShaderCompileData *compileData, ProcessData *data) const
{
	data->interpolantCount = 3;
	data->interpolantType[0] = 'TLDR';
	data->interpolantType[1] = 'TL2X';
	data->interpolantType[2] = 'TL2Y';
}

int32 TerrainLightDirectionProcess::GenerateShaderCode(const ShaderCompileData *compileData, const char **shaderCode) const
{
	static const char code[] =
	{
		HALF3 " tldr = normalize($TLDR);\n"
		HALF3 " tld2 = normalize(" FLOAT3 "($TL2X, $TL2Y, $TLDR.z));\n"
	};

	shaderCode[0] = code;
	return (1);
}


TerrainViewDirectionProcess::TerrainViewDirectionProcess() : InterpolantProcess(kProcessTerrainViewDirection)
{
	SetBaseProcessType(kProcessDerived);
}

TerrainViewDirectionProcess::TerrainViewDirectionProcess(const TerrainViewDirectionProcess& terrainViewDirectionProcess) : InterpolantProcess(terrainViewDirectionProcess)
{
}

TerrainViewDirectionProcess::~TerrainViewDirectionProcess()
{
}

Process *TerrainViewDirectionProcess::Replicate(void) const
{
	return (new TerrainViewDirectionProcess(*this));
}

int32 TerrainViewDirectionProcess::GenerateDerivedInterpolantTypes(const ShaderCompileData *compileData, ProcessType *type) const
{
	compileData->terrainViewDirection = this;
	return (0);
}

void TerrainViewDirectionProcess::GenerateProcessData(const ShaderCompileData *compileData, ProcessData *data) const
{
	data->interpolantCount = 3;
	data->interpolantType[0] = 'TVDR';
	data->interpolantType[1] = 'TV2X';
	data->interpolantType[2] = 'TV2Y';
}

int32 TerrainViewDirectionProcess::GenerateShaderCode(const ShaderCompileData *compileData, const char **shaderCode) const
{
	static const char code[] =
	{
		HALF3 " tvdr = normalize($TVDR);\n"
		FLOAT3 " TVD2 = " FLOAT3 "($TV2X, $TV2Y, $TVDR.z);\n"
		HALF3 " tvd2 = normalize(TVD2);\n"
	};

	shaderCode[0] = code;
	return (1);
}


TerrainHalfwayDirectionProcess::TerrainHalfwayDirectionProcess() : InterpolantProcess(kProcessTerrainHalfwayDirection)
{
	SetBaseProcessType(kProcessDerived);
}

TerrainHalfwayDirectionProcess::TerrainHalfwayDirectionProcess(const TerrainHalfwayDirectionProcess& terrainHalfwayDirectionProcess) : InterpolantProcess(terrainHalfwayDirectionProcess)
{
}

TerrainHalfwayDirectionProcess::~TerrainHalfwayDirectionProcess()
{
}

Process *TerrainHalfwayDirectionProcess::Replicate(void) const
{
	return (new TerrainHalfwayDirectionProcess(*this));
}

int32 TerrainHalfwayDirectionProcess::GenerateDerivedInterpolantTypes(const ShaderCompileData *compileData, ProcessType *type) const
{
	type[0] = kProcessTerrainLightDirection;
	type[1] = kProcessTerrainViewDirection;
	return (2);
}

void TerrainHalfwayDirectionProcess::GenerateProcessData(const ShaderCompileData *compileData, ProcessData *data) const
{
	data->outputSize = 3;
}

int32 TerrainHalfwayDirectionProcess::GenerateShaderCode(const ShaderCompileData *compileData, const char **shaderCode) const
{
	static const char code[] =
	{
		HALF3 " thdr = normalize(tldr + tvdr);\n"
		HALF3 " thd2 = normalize(tld2 + tvd2);\n"
	};

	shaderCode[0] = code;
	return (1);
}


TangentLightVectorProcess::TangentLightVectorProcess() : InterpolantProcess(kProcessTangentLightVector)
{
}

TangentLightVectorProcess::TangentLightVectorProcess(const TangentLightVectorProcess& tangentLightVectorProcess) : InterpolantProcess(tangentLightVectorProcess)
{
}

TangentLightVectorProcess::~TangentLightVectorProcess()
{
}

Process *TangentLightVectorProcess::Replicate(void) const
{
	return (new TangentLightVectorProcess(*this));
}

void TangentLightVectorProcess::GenerateProcessData(const ShaderCompileData *compileData, ProcessData *data) const
{
	data->outputSize = 3;

	data->interpolantCount = 1;
	data->interpolantType[0] = kProcessTangentLightDirection;
}

int32 TangentLightVectorProcess::GenerateOutputIdentifier(const ShaderCompileData *compileData, const ShaderAllocationData *allocData, SwizzleData *swizzleData, char *name) const
{
	int32 len = PregenerateOutputIdentifier(swizzleData, name);
	return (GetInterpolantName(kProcessTangentLightDirection, allocData, name + len, swizzleData) + len);
}


TangentViewVectorProcess::TangentViewVectorProcess() : InterpolantProcess(kProcessTangentViewVector)
{
}

TangentViewVectorProcess::TangentViewVectorProcess(const TangentViewVectorProcess& tangentViewVectorProcess) : InterpolantProcess(tangentViewVectorProcess)
{
}

TangentViewVectorProcess::~TangentViewVectorProcess()
{
}

Process *TangentViewVectorProcess::Replicate(void) const
{
	return (new TangentViewVectorProcess(*this));
}

int32 TangentViewVectorProcess::GenerateDerivedInterpolantTypes(const ShaderCompileData *compileData, ProcessType *type) const
{
	compileData->shaderSourceFlags |= kShaderSourceTangentViewDirection;
	return (0);
}

void TangentViewVectorProcess::GenerateProcessData(const ShaderCompileData *compileData, ProcessData *data) const
{
	data->outputSize = 3;

	data->interpolantCount = 1;
	data->interpolantType[0] = kProcessTangentViewDirection;
}

int32 TangentViewVectorProcess::GenerateOutputIdentifier(const ShaderCompileData *compileData, const ShaderAllocationData *allocData, SwizzleData *swizzleData, char *name) const
{
	int32 len = PregenerateOutputIdentifier(swizzleData, name);
	return (GetInterpolantName(kProcessTangentViewDirection, allocData, name + len, swizzleData) + len);
}


ObjectLightVectorProcess::ObjectLightVectorProcess() : InterpolantProcess(kProcessObjectLightVector)
{
}

ObjectLightVectorProcess::ObjectLightVectorProcess(const ObjectLightVectorProcess& objectLightVectorProcess) : InterpolantProcess(objectLightVectorProcess)
{
}

ObjectLightVectorProcess::~ObjectLightVectorProcess()
{
}

Process *ObjectLightVectorProcess::Replicate(void) const
{
	return (new ObjectLightVectorProcess(*this));
}

void ObjectLightVectorProcess::GenerateProcessData(const ShaderCompileData *compileData, ProcessData *data) const
{
	data->outputSize = 3;

	data->interpolantCount = 1;
	data->interpolantType[0] = kProcessObjectLightDirection;
}

int32 ObjectLightVectorProcess::GenerateOutputIdentifier(const ShaderCompileData *compileData, const ShaderAllocationData *allocData, SwizzleData *swizzleData, char *name) const
{
	int32 len = PregenerateOutputIdentifier(swizzleData, name);
	return (GetInterpolantName(kProcessObjectLightDirection, allocData, name + len, swizzleData) + len);
}


ObjectViewVectorProcess::ObjectViewVectorProcess() : InterpolantProcess(kProcessObjectViewVector)
{
}

ObjectViewVectorProcess::ObjectViewVectorProcess(const ObjectViewVectorProcess& objectViewVectorProcess) : InterpolantProcess(objectViewVectorProcess)
{
}

ObjectViewVectorProcess::~ObjectViewVectorProcess()
{
}

Process *ObjectViewVectorProcess::Replicate(void) const
{
	return (new ObjectViewVectorProcess(*this));
}

void ObjectViewVectorProcess::GenerateProcessData(const ShaderCompileData *compileData, ProcessData *data) const
{
	data->outputSize = 3;

	data->interpolantCount = 1;
	data->interpolantType[0] = kProcessObjectViewDirection;
}

int32 ObjectViewVectorProcess::GenerateOutputIdentifier(const ShaderCompileData *compileData, const ShaderAllocationData *allocData, SwizzleData *swizzleData, char *name) const
{
	int32 len = PregenerateOutputIdentifier(swizzleData, name);
	return (GetInterpolantName(kProcessObjectViewDirection, allocData, name + len, swizzleData) + len);
}


UnaryProcess::UnaryProcess(ProcessType type) : Process(type)
{
}

UnaryProcess::UnaryProcess(const UnaryProcess& unaryProcess) : Process(unaryProcess)
{
}

UnaryProcess::~UnaryProcess()
{
}

int32 UnaryProcess::GetPortCount(void) const
{
	return (1);
}

const char *UnaryProcess::GetPortName(int32 index) const
{
	return ("A");
}

void UnaryProcess::GenerateProcessData(const ShaderCompileData *compileData, ProcessData *data) const
{
	data->registerCount = 1;

	int32 size = GetPortRoute(0)->GenerateOutputSize();
	data->outputSize = size;
	data->inputSize[0] = size;
}


BinaryProcess::BinaryProcess(ProcessType type) : Process(type)
{
}

BinaryProcess::BinaryProcess(const BinaryProcess& binaryProcess) : Process(binaryProcess)
{
}

BinaryProcess::~BinaryProcess()
{
}

int32 BinaryProcess::GetPortCount(void) const
{
	return (2);
}

const char *BinaryProcess::GetPortName(int32 index) const
{
	static const char *const portName[2] =
	{
		"A", "B"
	};

	return (portName[index]);
}

void BinaryProcess::GenerateProcessData(const ShaderCompileData *compileData, ProcessData *data) const
{
	int32	size;

	const Route *routeA = GetPortRoute(0);
	const Route *routeB = GetPortRoute(1);

	if (!routeA)
	{
		data->passthruPort = 1;
		size = routeB->GenerateOutputSize();
	}
	else if ((!routeB) && (GetPortFlags(1) & kProcessPortOmissible))
	{
		data->passthruPort = 0;
		size = routeA->GenerateOutputSize();
	}
	else
	{
		data->registerCount = 1;

		size = routeA->GenerateOutputSize();
		if (routeB)
		{
			size = Max(size, routeB->GenerateOutputSize());
		}
	}

	data->outputSize = size;
	data->inputSize[0] = size;
	data->inputSize[1] = size;
}

int32 BinaryProcess::GenerateOutputIdentifier(const ShaderCompileData *compileData, const ShaderAllocationData *allocData, SwizzleData *swizzleData, char *name) const
{
	int32 port = GetProcessData()->passthruPort;
	if (port >= 0)
	{
		return (GetPortRoute(port)->GenerateOutputIdentifier(compileData, allocData, swizzleData, name));
	}

	return (Process::GenerateOutputIdentifier(compileData, allocData, swizzleData, name));
}


TrinaryProcess::TrinaryProcess(ProcessType type) : Process(type)
{
}

TrinaryProcess::TrinaryProcess(const TrinaryProcess& trinaryProcess) : Process(trinaryProcess)
{
}

TrinaryProcess::~TrinaryProcess()
{
}

int32 TrinaryProcess::GetPortCount(void) const
{
	return (3);
}

void TrinaryProcess::GenerateProcessData(const ShaderCompileData *compileData, ProcessData *data) const
{
	data->registerCount = 1;

	int32 size1 = GetPortRoute(0)->GenerateOutputSize();
	int32 size2 = GetPortRoute(1)->GenerateOutputSize();
	int32 size3 = GetPortRoute(2)->GenerateOutputSize();
	int32 size = Max(Max(size1, size2), size3);

	data->outputSize = size;
	data->inputSize[0] = size;
	data->inputSize[1] = size;
	data->inputSize[2] = size;
}

int32 TrinaryProcess::GenerateOutputIdentifier(const ShaderCompileData *compileData, const ShaderAllocationData *allocData, SwizzleData *swizzleData, char *name) const
{
	int32 port = GetProcessData()->passthruPort;
	if (port >= 0)
	{
		return (GetPortRoute(port)->GenerateOutputIdentifier(compileData, allocData, swizzleData, name));
	}

	return (Process::GenerateOutputIdentifier(compileData, allocData, swizzleData, name));
}


AbsoluteProcess::AbsoluteProcess() : UnaryProcess(kProcessAbsolute)
{
}

AbsoluteProcess::AbsoluteProcess(const AbsoluteProcess& absoluteProcess) : UnaryProcess(absoluteProcess)
{
}

AbsoluteProcess::~AbsoluteProcess()
{
}

Process *AbsoluteProcess::Replicate(void) const
{
	return (new AbsoluteProcess(*this));
}

int32 AbsoluteProcess::GenerateShaderCode(const ShaderCompileData *compileData, const char **shaderCode) const
{
	static const char code[] =
	{
		"# = abs(%0);\n"
	};

	shaderCode[0] = code;
	return (1);
}


InvertProcess::InvertProcess() : UnaryProcess(kProcessInvert)
{
}

InvertProcess::InvertProcess(const InvertProcess& invertProcess) : UnaryProcess(invertProcess)
{
}

InvertProcess::~InvertProcess()
{
}

Process *InvertProcess::Replicate(void) const
{
	return (new InvertProcess(*this));
}

int32 InvertProcess::GenerateShaderCode(const ShaderCompileData *compileData, const char **shaderCode) const
{
	static const char code[] =
	{
		"# = 1.0 - %0;\n"
	};

	shaderCode[0] = code;
	return (1);
}


ExpandProcess::ExpandProcess() : UnaryProcess(kProcessExpand)
{
}

ExpandProcess::ExpandProcess(const ExpandProcess& expandProcess) : UnaryProcess(expandProcess)
{
}

ExpandProcess::~ExpandProcess()
{
}

Process *ExpandProcess::Replicate(void) const
{
	return (new ExpandProcess(*this));
}

int32 ExpandProcess::GenerateShaderCode(const ShaderCompileData *compileData, const char **shaderCode) const
{
	static const char code[] =
	{
		"# = %0 * 2.0 - 1.0;\n"
	};

	shaderCode[0] = code;
	return (1);
}


ReciprocalProcess::ReciprocalProcess() : UnaryProcess(kProcessReciprocal)
{
}

ReciprocalProcess::ReciprocalProcess(const ReciprocalProcess& reciprocalProcess) : UnaryProcess(reciprocalProcess)
{
}

ReciprocalProcess::~ReciprocalProcess()
{
}

Process *ReciprocalProcess::Replicate(void) const
{
	return (new ReciprocalProcess(*this));
}

void ReciprocalProcess::GenerateProcessData(const ShaderCompileData *compileData, ProcessData *data) const
{
	data->registerCount = 1;
	data->outputSize = 1;
	data->inputSize[0] = 1;
}

int32 ReciprocalProcess::GenerateShaderCode(const ShaderCompileData *compileData, const char **shaderCode) const
{
	static const char code[] =
	{
		"# = 1.0 / %0;\n"
	};

	shaderCode[0] = code;
	return (1);
}


ReciprocalSquareRootProcess::ReciprocalSquareRootProcess() : UnaryProcess(kProcessReciprocalSquareRoot)
{
}

ReciprocalSquareRootProcess::ReciprocalSquareRootProcess(const ReciprocalSquareRootProcess& reciprocalSquareRootProcess) : UnaryProcess(reciprocalSquareRootProcess)
{
}

ReciprocalSquareRootProcess::~ReciprocalSquareRootProcess()
{
}

Process *ReciprocalSquareRootProcess::Replicate(void) const
{
	return (new ReciprocalSquareRootProcess(*this));
}

void ReciprocalSquareRootProcess::GenerateProcessData(const ShaderCompileData *compileData, ProcessData *data) const
{
	data->registerCount = 1;
	data->outputSize = 1;
	data->inputSize[0] = 1;
}

int32 ReciprocalSquareRootProcess::GenerateShaderCode(const ShaderCompileData *compileData, const char **shaderCode) const
{
	static const char code[] =
	{
		"# = " RSQRT "(%0);\n"
	};

	shaderCode[0] = code;
	return (1);
}


SquareRootProcess::SquareRootProcess() : UnaryProcess(kProcessSquareRoot)
{
}

SquareRootProcess::SquareRootProcess(const SquareRootProcess& squareRootProcess) : UnaryProcess(squareRootProcess)
{
}

SquareRootProcess::~SquareRootProcess()
{
}

Process *SquareRootProcess::Replicate(void) const
{
	return (new SquareRootProcess(*this));
}

int32 SquareRootProcess::GenerateShaderCode(const ShaderCompileData *compileData, const char **shaderCode) const
{
	static const char code[] =
	{
		"# = sqrt(%0);\n"
	};

	shaderCode[0] = code;
	return (1);
}

void SquareRootProcess::GenerateProcessData(const ShaderCompileData *compileData, ProcessData *data) const
{
	data->registerCount = 1;
	data->outputSize = 1;
	data->inputSize[0] = 1;
}


MagnitudeProcess::MagnitudeProcess() : UnaryProcess(kProcessMagnitude)
{
}

MagnitudeProcess::MagnitudeProcess(const MagnitudeProcess& magnitudeProcess) : UnaryProcess(magnitudeProcess)
{
}

MagnitudeProcess::~MagnitudeProcess()
{
}

Process *MagnitudeProcess::Replicate(void) const
{
	return (new MagnitudeProcess(*this));
}

void MagnitudeProcess::GenerateProcessData(const ShaderCompileData *compileData, ProcessData *data) const
{
	data->registerCount = 1;
	data->outputSize = 1;
	data->inputSize[0] = 3;
}

int32 MagnitudeProcess::GenerateShaderCode(const ShaderCompileData *compileData, const char **shaderCode) const
{
	static const char code[] =
	{
		"# = length(%0);\n"
	};

	shaderCode[0] = code;
	return (1);
}


NormalizeProcess::NormalizeProcess() : UnaryProcess(kProcessNormalize)
{
}

NormalizeProcess::NormalizeProcess(const NormalizeProcess& normalizeProcess) : UnaryProcess(normalizeProcess)
{
}

NormalizeProcess::~NormalizeProcess()
{
}

Process *NormalizeProcess::Replicate(void) const
{
	return (new NormalizeProcess(*this));
}

void NormalizeProcess::GenerateProcessData(const ShaderCompileData *compileData, ProcessData *data) const
{
	data->registerCount = 1;
	data->outputSize = 3;
	data->inputSize[0] = 3;
}

int32 NormalizeProcess::GenerateShaderCode(const ShaderCompileData *compileData, const char **shaderCode) const
{
	static const char code[] =
	{
		"# = normalize(%0);\n"
	};

	shaderCode[0] = code;
	return (1);
}


FloorProcess::FloorProcess() : UnaryProcess(kProcessFloor)
{
}

FloorProcess::FloorProcess(const FloorProcess& floorProcess) : UnaryProcess(floorProcess)
{
}

FloorProcess::~FloorProcess()
{
}

Process *FloorProcess::Replicate(void) const
{
	return (new FloorProcess(*this));
}

int32 FloorProcess::GenerateShaderCode(const ShaderCompileData *compileData, const char **shaderCode) const
{
	static const char code[] =
	{
		"# = floor(%0);\n"
	};

	shaderCode[0] = code;
	return (1);
}


RoundProcess::RoundProcess() : UnaryProcess(kProcessRound)
{
}

RoundProcess::RoundProcess(const RoundProcess& roundProcess) : UnaryProcess(roundProcess)
{
}

RoundProcess::~RoundProcess()
{
}

Process *RoundProcess::Replicate(void) const
{
	return (new RoundProcess(*this));
}

int32 RoundProcess::GenerateShaderCode(const ShaderCompileData *compileData, const char **shaderCode) const
{
	static const char code[] =
	{
		#if C4OPENGL

			"# = floor(%0 + 0.5);\n"

		#else

			"# = round(%0);\n"

		#endif
	};

	shaderCode[0] = code;
	return (1);
}


FractionProcess::FractionProcess() : UnaryProcess(kProcessFraction)
{
}

FractionProcess::FractionProcess(const FractionProcess& fractionProcess) : UnaryProcess(fractionProcess)
{
}

FractionProcess::~FractionProcess()
{
}

Process *FractionProcess::Replicate(void) const
{
	return (new FractionProcess(*this));
}

int32 FractionProcess::GenerateShaderCode(const ShaderCompileData *compileData, const char **shaderCode) const
{
	static const char code[] =
	{
		"# = " FRAC "(%0);\n"
	};

	shaderCode[0] = code;
	return (1);
}


SaturateProcess::SaturateProcess() : UnaryProcess(kProcessSaturate)
{
}

SaturateProcess::SaturateProcess(const SaturateProcess& saturateProcess) : UnaryProcess(saturateProcess)
{
}

SaturateProcess::~SaturateProcess()
{
}

Process *SaturateProcess::Replicate(void) const
{
	return (new SaturateProcess(*this));
}

int32 SaturateProcess::GenerateShaderCode(const ShaderCompileData *compileData, const char **shaderCode) const
{
	static const char code[] =
	{
		#if C4OPENGL

			"# = clamp(%0, 0.0, 1.0);\n"

		#else

			"# = saturate(%0);\n"

		#endif
	};

	shaderCode[0] = code;
	return (1);
}


SineProcess::SineProcess() : UnaryProcess(kProcessSine)
{
}

SineProcess::SineProcess(const SineProcess& sineProcess) : UnaryProcess(sineProcess)
{
}

SineProcess::~SineProcess()
{
}

Process *SineProcess::Replicate(void) const
{
	return (new SineProcess(*this));
}

void SineProcess::GenerateProcessData(const ShaderCompileData *compileData, ProcessData *data) const
{
	data->registerCount = 1;
	data->outputSize = 1;
	data->inputSize[0] = 1;
}

int32 SineProcess::GenerateShaderCode(const ShaderCompileData *compileData, const char **shaderCode) const
{
	static const char code[] =
	{
		"# = sin(%0);\n"
	};

	shaderCode[0] = code;
	return (1);
}


CosineProcess::CosineProcess() : UnaryProcess(kProcessCosine)
{
}

CosineProcess::CosineProcess(const CosineProcess& cosineProcess) : UnaryProcess(cosineProcess)
{
}

CosineProcess::~CosineProcess()
{
}

Process *CosineProcess::Replicate(void) const
{
	return (new CosineProcess(*this));
}

void CosineProcess::GenerateProcessData(const ShaderCompileData *compileData, ProcessData *data) const
{
	data->registerCount = 1;
	data->outputSize = 1;
	data->inputSize[0] = 1;
}

int32 CosineProcess::GenerateShaderCode(const ShaderCompileData *compileData, const char **shaderCode) const
{
	static const char code[] =
	{
		"# = cos(%0);\n"
	};

	shaderCode[0] = code;
	return (1);
}


Exp2Process::Exp2Process() : UnaryProcess(kProcessExp2)
{
}

Exp2Process::Exp2Process(const Exp2Process& exp2Process) : UnaryProcess(exp2Process)
{
}

Exp2Process::~Exp2Process()
{
}

Process *Exp2Process::Replicate(void) const
{
	return (new Exp2Process(*this));
}

void Exp2Process::GenerateProcessData(const ShaderCompileData *compileData, ProcessData *data) const
{
	data->registerCount = 1;
	data->outputSize = 1;
	data->inputSize[0] = 1;
}

int32 Exp2Process::GenerateShaderCode(const ShaderCompileData *compileData, const char **shaderCode) const
{
	static const char code[] =
	{
		"# = exp2(%0);\n"
	};

	shaderCode[0] = code;
	return (1);
}


Log2Process::Log2Process() : UnaryProcess(kProcessLog2)
{
}

Log2Process::Log2Process(const Log2Process& log2Process) : UnaryProcess(log2Process)
{
}

Log2Process::~Log2Process()
{
}

Process *Log2Process::Replicate(void) const
{
	return (new Log2Process(*this));
}

void Log2Process::GenerateProcessData(const ShaderCompileData *compileData, ProcessData *data) const
{
	data->registerCount = 1;
	data->outputSize = 1;
	data->inputSize[0] = 1;
}

int32 Log2Process::GenerateShaderCode(const ShaderCompileData *compileData, const char **shaderCode) const
{
	static const char code[] =
	{
		"# = log2(%0);\n"
	};

	shaderCode[0] = code;
	return (1);
}


AddProcess::AddProcess() : BinaryProcess(kProcessAdd)
{
}

AddProcess::AddProcess(const AddProcess& addProcess) : BinaryProcess(addProcess)
{
}

AddProcess::~AddProcess()
{
}

Process *AddProcess::Replicate(void) const
{
	return (new AddProcess(*this));
}

unsigned_int32 AddProcess::GetPortFlags(int32 index) const
{
	return (kProcessPortOmissible);
}

int32 AddProcess::GenerateShaderCode(const ShaderCompileData *compileData, const char **shaderCode) const
{
	if ((GetPortRoute(0)) && (GetPortRoute(1)))
	{
		static const char code[] =
		{
			"# = %0 + %1;\n"
		};

		shaderCode[0] = code;
		return (1);
	}

	return (0);
}


SubtractProcess::SubtractProcess() : BinaryProcess(kProcessSubtract)
{
}

SubtractProcess::SubtractProcess(const SubtractProcess& subtractProcess) : BinaryProcess(subtractProcess)
{
}

SubtractProcess::~SubtractProcess()
{
}

Process *SubtractProcess::Replicate(void) const
{
	return (new SubtractProcess(*this));
}

unsigned_int32 SubtractProcess::GetPortFlags(int32 index) const
{
	return (kProcessPortOmissible);
}

int32 SubtractProcess::GenerateShaderCode(const ShaderCompileData *compileData, const char **shaderCode) const
{
	if ((GetPortRoute(0)) && (GetPortRoute(1)))
	{
		static const char code[] =
		{
			"# = %0 - %1;\n"
		};

		shaderCode[0] = code;
		return (1);
	}

	return (0);
}


AverageProcess::AverageProcess() : BinaryProcess(kProcessAverage)
{
}

AverageProcess::AverageProcess(const AverageProcess& averageProcess) : BinaryProcess(averageProcess)
{
}

AverageProcess::~AverageProcess()
{
}

Process *AverageProcess::Replicate(void) const
{
	return (new AverageProcess(*this));
}

unsigned_int32 AverageProcess::GetPortFlags(int32 index) const
{
	return (kProcessPortOmissible);
}

int32 AverageProcess::GenerateShaderCode(const ShaderCompileData *compileData, const char **shaderCode) const
{
	if ((GetPortRoute(0)) && (GetPortRoute(1)))
	{
		static const char code[] =
		{
			"# = (%0 + %1) * 0.5;\n"
		};

		shaderCode[0] = code;
		return (1);
	}

	return (0);
}


MultiplyProcess::MultiplyProcess() : BinaryProcess(kProcessMultiply)
{
}

MultiplyProcess::MultiplyProcess(const MultiplyProcess& multiplyProcess) : BinaryProcess(multiplyProcess)
{
}

MultiplyProcess::~MultiplyProcess()
{
}

Process *MultiplyProcess::Replicate(void) const
{
	return (new MultiplyProcess(*this));
}

unsigned_int32 MultiplyProcess::GetPortFlags(int32 index) const
{
	return (kProcessPortOmissible);
}

int32 MultiplyProcess::GenerateShaderCode(const ShaderCompileData *compileData, const char **shaderCode) const
{
	if ((GetPortRoute(0)) && (GetPortRoute(1)))
	{
		static const char code[] =
		{
			"# = %0 * %1;\n"
		};

		shaderCode[0] = code;
		return (1);
	}

	return (0);
}


DivideProcess::DivideProcess() : BinaryProcess(kProcessDivide)
{
}

DivideProcess::DivideProcess(const DivideProcess& divideProcess) : BinaryProcess(divideProcess)
{
}

DivideProcess::~DivideProcess()
{
}

Process *DivideProcess::Replicate(void) const
{
	return (new DivideProcess(*this));
}

void DivideProcess::GenerateProcessData(const ShaderCompileData *compileData, ProcessData *data) const
{
	data->registerCount = 1;

	int32 size = GetPortRoute(0)->GenerateOutputSize();

	data->outputSize = size;
	data->inputSize[0] = size;
	data->inputSize[1] = 1;
}

int32 DivideProcess::GenerateShaderCode(const ShaderCompileData *compileData, const char **shaderCode) const
{
	static const char code[] =
	{
		"# = %0 / %1;\n"
	};

	shaderCode[0] = code;
	return (1);
}


Dot3Process::Dot3Process() : BinaryProcess(kProcessDot3)
{
}

Dot3Process::Dot3Process(const Dot3Process& dot3Process) : BinaryProcess(dot3Process)
{
}

Dot3Process::~Dot3Process()
{
}

Process *Dot3Process::Replicate(void) const
{
	return (new Dot3Process(*this));
}

void Dot3Process::GenerateProcessData(const ShaderCompileData *compileData, ProcessData *data) const
{
	data->registerCount = 1;
	data->outputSize = 1;
	data->inputSize[0] = 3;
	data->inputSize[1] = 3;
}

int32 Dot3Process::GenerateShaderCode(const ShaderCompileData *compileData, const char **shaderCode) const
{
	static const char code[] =
	{
		"# = dot(%0, %1);\n"
	};

	shaderCode[0] = code;
	return (1);
}


Dot4Process::Dot4Process() : BinaryProcess(kProcessDot4)
{
}

Dot4Process::Dot4Process(const Dot4Process& dot4Process) : BinaryProcess(dot4Process)
{
}

Dot4Process::~Dot4Process()
{
}

Process *Dot4Process::Replicate(void) const
{
	return (new Dot4Process(*this));
}

void Dot4Process::GenerateProcessData(const ShaderCompileData *compileData, ProcessData *data) const
{
	data->registerCount = 1;
	data->outputSize = 1;
	data->inputSize[0] = 4;
	data->inputSize[1] = 4;
}

int32 Dot4Process::GenerateShaderCode(const ShaderCompileData *compileData, const char **shaderCode) const
{
	static const char code[] =
	{
		"# = dot(%0, %1);\n"
	};

	shaderCode[0] = code;
	return (1);
}


CrossProcess::CrossProcess() : BinaryProcess(kProcessCross)
{
}

CrossProcess::CrossProcess(const CrossProcess& crossProcess) : BinaryProcess(crossProcess)
{
}

CrossProcess::~CrossProcess()
{
}

Process *CrossProcess::Replicate(void) const
{
	return (new CrossProcess(*this));
}

void CrossProcess::GenerateProcessData(const ShaderCompileData *compileData, ProcessData *data) const
{
	data->registerCount = 1;
	data->outputSize = 3;
	data->inputSize[0] = 3;
	data->inputSize[1] = 3;
}

int32 CrossProcess::GenerateShaderCode(const ShaderCompileData *compileData, const char **shaderCode) const
{
	static const char code[] =
	{
		"# = cross(%0, %1);\n"
	};

	shaderCode[0] = code;
	return (1);
}


MinimumProcess::MinimumProcess() : BinaryProcess(kProcessMinimum)
{
}

MinimumProcess::MinimumProcess(const MinimumProcess& minimumProcess) : BinaryProcess(minimumProcess)
{
}

MinimumProcess::~MinimumProcess()
{
}

Process *MinimumProcess::Replicate(void) const
{
	return (new MinimumProcess(*this));
}

unsigned_int32 MinimumProcess::GetPortFlags(int32 index) const
{
	return (kProcessPortOmissible);
}

int32 MinimumProcess::GenerateShaderCode(const ShaderCompileData *compileData, const char **shaderCode) const
{
	if ((GetPortRoute(0)) && (GetPortRoute(1)))
	{
		static const char code[] =
		{
			"# = min(%0, %1);\n"
		};

		shaderCode[0] = code;
		return (1);
	}

	return (0);
}


MaximumProcess::MaximumProcess() : BinaryProcess(kProcessMaximum)
{
}

MaximumProcess::MaximumProcess(const MaximumProcess& maximumProcess) : BinaryProcess(maximumProcess)
{
}

MaximumProcess::~MaximumProcess()
{
}

Process *MaximumProcess::Replicate(void) const
{
	return (new MaximumProcess(*this));
}

unsigned_int32 MaximumProcess::GetPortFlags(int32 index) const
{
	return (kProcessPortOmissible);
}

int32 MaximumProcess::GenerateShaderCode(const ShaderCompileData *compileData, const char **shaderCode) const
{
	if ((GetPortRoute(0)) && (GetPortRoute(1)))
	{
		static const char code[] =
		{
			"# = max(%0, %1);\n"
		};

		shaderCode[0] = code;
		return (1);
	}

	return (0);
}


SetLessThanProcess::SetLessThanProcess() : BinaryProcess(kProcessSetLessThan)
{
}

SetLessThanProcess::SetLessThanProcess(const SetLessThanProcess& setLessThanProcess) : BinaryProcess(setLessThanProcess)
{
}

SetLessThanProcess::~SetLessThanProcess()
{
}

Process *SetLessThanProcess::Replicate(void) const
{
	return (new SetLessThanProcess(*this));
}

unsigned_int32 SetLessThanProcess::GetPortFlags(int32 index) const
{
	return ((index == 0) ? 0 : kProcessPortOptional);
}

int32 SetLessThanProcess::GenerateShaderCode(const ShaderCompileData *compileData, const char **shaderCode) const
{
	#if C4OPENGL

		static const char *fullCode[4] =
		{
			"# = float(%0 < %1);\n",
			"# = vec2(%0.x < %1.x, %0.y < %1.y);\n",
			"# = vec3(%0.x < %1.x, %0.y < %1.y, %0.z < %1.z);\n",
			"# = vec4(%0.x < %1.x, %0.y < %1.y, %0.z < %1.z, %0.w < %1.w);\n"
		};

		static const char *zeroCode[4] =
		{
			"# = float(%0 < 0.0);\n",
			"# = vec2(%0.x < 0.0, %0.y < 0.0);\n",
			"# = vec3(%0.x < 0.0, %0.y < 0.0, %0.z < 0.0);\n",
			"# = vec4(%0.x < 0.0, %0.y < 0.0, %0.z < 0.0, %0.w < 0.0);\n"
		};

		const Route *routeA = GetPortRoute(0);
		const Route *routeB = GetPortRoute(1);

		if (routeB)
		{
			int32 sizeA = routeA->GenerateOutputSize();
			int32 sizeB = routeB->GenerateOutputSize();
			shaderCode[0] = fullCode[Max(sizeA, sizeB) - 1];
		}
		else
		{
			shaderCode[0] = zeroCode[routeA->GenerateOutputSize() - 1];
		}

	#else

		static const char fullCode[] =
		{
			"# = (%0 < %1);\n"
		};

		static const char zeroCode[] =
		{
			"# = (%0 < 0.0);\n"
		};

		if (GetPortRoute(1))
		{
			shaderCode[0] = fullCode;
		}
		else
		{
			shaderCode[0] = zeroCode;
		}

	#endif

	return (1);
}


SetGreaterThanProcess::SetGreaterThanProcess() : BinaryProcess(kProcessSetGreaterThan)
{
}

SetGreaterThanProcess::SetGreaterThanProcess(const SetGreaterThanProcess& setGreaterThanProcess) : BinaryProcess(setGreaterThanProcess)
{
}

SetGreaterThanProcess::~SetGreaterThanProcess()
{
}

Process *SetGreaterThanProcess::Replicate(void) const
{
	return (new SetGreaterThanProcess(*this));
}

unsigned_int32 SetGreaterThanProcess::GetPortFlags(int32 index) const
{
	return ((index == 0) ? 0 : kProcessPortOptional);
}

int32 SetGreaterThanProcess::GenerateShaderCode(const ShaderCompileData *compileData, const char **shaderCode) const
{
	#if C4OPENGL

		static const char *fullCode[4] =
		{
			"# = float(%0 > %1);\n",
			"# = vec2(%0.x > %1.x, %0.y > %1.y);\n",
			"# = vec3(%0.x > %1.x, %0.y > %1.y, %0.z > %1.z);\n",
			"# = vec4(%0.x > %1.x, %0.y > %1.y, %0.z > %1.z, %0.w > %1.w);\n"
		};

		static const char *zeroCode[4] =
		{
			"# = float(%0 > 0.0);\n",
			"# = vec2(%0.x > 0.0, %0.y > 0.0);\n",
			"# = vec3(%0.x > 0.0, %0.y > 0.0, %0.z > 0.0);\n",
			"# = vec4(%0.x > 0.0, %0.y > 0.0, %0.z > 0.0, %0.w > 0.0);\n"
		};

		const Route *routeA = GetPortRoute(0);
		const Route *routeB = GetPortRoute(1);

		if (routeB)
		{
			int32 sizeA = routeA->GenerateOutputSize();
			int32 sizeB = routeB->GenerateOutputSize();
			shaderCode[0] = fullCode[Max(sizeA, sizeB) - 1];
		}
		else
		{
			shaderCode[0] = zeroCode[routeA->GenerateOutputSize() - 1];
		}

	#else

		static const char fullCode[] =
		{
			"# = (%0 > %1);\n"
		};

		static const char zeroCode[] =
		{
			"# = (%0 > 0.0);\n"
		};

		if (GetPortRoute(1))
		{
			shaderCode[0] = fullCode;
		}
		else
		{
			shaderCode[0] = zeroCode;
		}

	#endif

	return (1);
}


SetLessEqualProcess::SetLessEqualProcess() : BinaryProcess(kProcessSetLessEqual)
{
}

SetLessEqualProcess::SetLessEqualProcess(const SetLessEqualProcess& setLessEqualProcess) : BinaryProcess(setLessEqualProcess)
{
}

SetLessEqualProcess::~SetLessEqualProcess()
{
}

Process *SetLessEqualProcess::Replicate(void) const
{
	return (new SetLessEqualProcess(*this));
}

unsigned_int32 SetLessEqualProcess::GetPortFlags(int32 index) const
{
	return ((index == 0) ? 0 : kProcessPortOptional);
}

int32 SetLessEqualProcess::GenerateShaderCode(const ShaderCompileData *compileData, const char **shaderCode) const
{
	#if C4OPENGL

		static const char *fullCode[4] =
		{
			"# = float(%0 <= %1);\n",
			"# = vec2(%0.x <= %1.x, %0.y <= %1.y);\n",
			"# = vec3(%0.x <= %1.x, %0.y <= %1.y, %0.z <= %1.z);\n",
			"# = vec4(%0.x <= %1.x, %0.y <= %1.y, %0.z <= %1.z, %0.w <= %1.w);\n"
		};

		static const char *zeroCode[4] =
		{
			"# = float(%0 <= 0.0);\n",
			"# = vec2(%0.x <= 0.0, %0.y <= 0.0);\n",
			"# = vec3(%0.x <= 0.0, %0.y <= 0.0, %0.z <= 0.0);\n",
			"# = vec4(%0.x <= 0.0, %0.y <= 0.0, %0.z <= 0.0, %0.w <= 0.0);\n"
		};

		const Route *routeA = GetPortRoute(0);
		const Route *routeB = GetPortRoute(1);

		if (routeB)
		{
			int32 sizeA = routeA->GenerateOutputSize();
			int32 sizeB = routeB->GenerateOutputSize();
			shaderCode[0] = fullCode[Max(sizeA, sizeB) - 1];
		}
		else
		{
			shaderCode[0] = zeroCode[routeA->GenerateOutputSize() - 1];
		}

	#else

		static const char fullCode[] =
		{
			"# = (%0 <= %1);\n"
		};

		static const char zeroCode[] =
		{
			"# = (%0 <= 0.0);\n"
		};

		if (GetPortRoute(1))
		{
			shaderCode[0] = fullCode;
		}
		else
		{
			shaderCode[0] = zeroCode;
		}

	#endif

	return (1);
}


SetGreaterEqualProcess::SetGreaterEqualProcess() : BinaryProcess(kProcessSetGreaterEqual)
{
}

SetGreaterEqualProcess::SetGreaterEqualProcess(const SetGreaterEqualProcess& setGreaterEqualProcess) : BinaryProcess(setGreaterEqualProcess)
{
}

SetGreaterEqualProcess::~SetGreaterEqualProcess()
{
}

Process *SetGreaterEqualProcess::Replicate(void) const
{
	return (new SetGreaterEqualProcess(*this));
}

unsigned_int32 SetGreaterEqualProcess::GetPortFlags(int32 index) const
{
	return ((index == 0) ? 0 : kProcessPortOptional);
}

int32 SetGreaterEqualProcess::GenerateShaderCode(const ShaderCompileData *compileData, const char **shaderCode) const
{
	#if C4OPENGL

		static const char *fullCode[4] =
		{
			"# = float(%0 >= %1);\n",
			"# = vec2(%0.x >= %1.x, %0.y >= %1.y);\n",
			"# = vec3(%0.x >= %1.x, %0.y >= %1.y, %0.z >= %1.z);\n",
			"# = vec4(%0.x >= %1.x, %0.y >= %1.y, %0.z >= %1.z, %0.w >= %1.w);\n"
		};

		static const char *zeroCode[4] =
		{
			"# = float(%0 >= 0.0);\n",
			"# = vec2(%0.x >= 0.0, %0.y >= 0.0);\n",
			"# = vec3(%0.x >= 0.0, %0.y >= 0.0, %0.z >= 0.0);\n",
			"# = vec4(%0.x >= 0.0, %0.y >= 0.0, %0.z >= 0.0, %0.w >= 0.0);\n"
		};

		const Route *routeA = GetPortRoute(0);
		const Route *routeB = GetPortRoute(1);

		if (routeB)
		{
			int32 sizeA = routeA->GenerateOutputSize();
			int32 sizeB = routeB->GenerateOutputSize();
			shaderCode[0] = fullCode[Max(sizeA, sizeB) - 1];
		}
		else
		{
			shaderCode[0] = zeroCode[routeA->GenerateOutputSize() - 1];
		}

	#else

		static const char fullCode[] =
		{
			"# = (%0 >= %1);\n"
		};

		static const char zeroCode[] =
		{
			"# = (%0 >= 0.0);\n"
		};

		if (GetPortRoute(1))
		{
			shaderCode[0] = fullCode;
		}
		else
		{
			shaderCode[0] = zeroCode;
		}

	#endif

	return (1);
}


SetEqualProcess::SetEqualProcess() : BinaryProcess(kProcessSetEqual)
{
}

SetEqualProcess::SetEqualProcess(const SetEqualProcess& setEqualProcess) : BinaryProcess(setEqualProcess)
{
}

SetEqualProcess::~SetEqualProcess()
{
}

Process *SetEqualProcess::Replicate(void) const
{
	return (new SetEqualProcess(*this));
}

unsigned_int32 SetEqualProcess::GetPortFlags(int32 index) const
{
	return ((index == 0) ? 0 : kProcessPortOptional);
}

int32 SetEqualProcess::GenerateShaderCode(const ShaderCompileData *compileData, const char **shaderCode) const
{
	#if C4OPENGL

		static const char *fullCode[4] =
		{
			"# = float(%0 == %1);\n",
			"# = vec2(%0.x == %1.x, %0.y == %1.y);\n",
			"# = vec3(%0.x == %1.x, %0.y == %1.y, %0.z == %1.z);\n",
			"# = vec4(%0.x == %1.x, %0.y == %1.y, %0.z == %1.z, %0.w == %1.w);\n"
		};

		static const char *zeroCode[4] =
		{
			"# = float(%0 == 0.0);\n",
			"# = vec2(%0.x == 0.0, %0.y == 0.0);\n",
			"# = vec3(%0.x == 0.0, %0.y == 0.0, %0.z == 0.0);\n",
			"# = vec4(%0.x == 0.0, %0.y == 0.0, %0.z == 0.0, %0.w == 0.0);\n"
		};

		const Route *routeA = GetPortRoute(0);
		const Route *routeB = GetPortRoute(1);

		if (routeB)
		{
			int32 sizeA = routeA->GenerateOutputSize();
			int32 sizeB = routeB->GenerateOutputSize();
			shaderCode[0] = fullCode[Max(sizeA, sizeB) - 1];
		}
		else
		{
			shaderCode[0] = zeroCode[routeA->GenerateOutputSize() - 1];
		}

	#else

		static const char fullCode[] =
		{
			"# = (%0 == %1);\n"
		};

		static const char zeroCode[] =
		{
			"# = (%0 == 0.0);\n"
		};

		if (GetPortRoute(1))
		{
			shaderCode[0] = fullCode;
		}
		else
		{
			shaderCode[0] = zeroCode;
		}

	#endif

	return (1);
}


SetNotEqualProcess::SetNotEqualProcess() : BinaryProcess(kProcessSetNotEqual)
{
}

SetNotEqualProcess::SetNotEqualProcess(const SetNotEqualProcess& setNotEqualProcess) : BinaryProcess(setNotEqualProcess)
{
}

SetNotEqualProcess::~SetNotEqualProcess()
{
}

Process *SetNotEqualProcess::Replicate(void) const
{
	return (new SetNotEqualProcess(*this));
}

unsigned_int32 SetNotEqualProcess::GetPortFlags(int32 index) const
{
	return ((index == 0) ? 0 : kProcessPortOptional);
}

int32 SetNotEqualProcess::GenerateShaderCode(const ShaderCompileData *compileData, const char **shaderCode) const
{
	#if C4OPENGL

		static const char *fullCode[4] =
		{
			"# = float(%0 != %1);\n",
			"# = vec2(%0.x != %1.x, %0.y != %1.y);\n",
			"# = vec3(%0.x != %1.x, %0.y != %1.y, %0.z != %1.z);\n",
			"# = vec4(%0.x != %1.x, %0.y != %1.y, %0.z != %1.z, %0.w != %1.w);\n"
		};

		static const char *zeroCode[4] =
		{
			"# = float(%0 != 0.0);\n",
			"# = vec2(%0.x != 0.0, %0.y != 0.0);\n",
			"# = vec3(%0.x != 0.0, %0.y != 0.0, %0.z != 0.0);\n",
			"# = vec4(%0.x != 0.0, %0.y != 0.0, %0.z != 0.0, %0.w != 0.0);\n"
		};

		const Route *routeA = GetPortRoute(0);
		const Route *routeB = GetPortRoute(1);

		if (routeB)
		{
			int32 sizeA = routeA->GenerateOutputSize();
			int32 sizeB = routeB->GenerateOutputSize();
			shaderCode[0] = fullCode[Max(sizeA, sizeB) - 1];
		}
		else
		{
			shaderCode[0] = zeroCode[routeA->GenerateOutputSize() - 1];
		}

	#else

		static const char fullCode[] =
		{
			"# = (%0 != %1);\n"
		};

		static const char zeroCode[] =
		{
			"# = (%0 != 0.0);\n"
		};

		if (GetPortRoute(1))
		{
			shaderCode[0] = fullCode;
		}
		else
		{
			shaderCode[0] = zeroCode;
		}

	#endif

	return (1);
}


PowerProcess::PowerProcess() : BinaryProcess(kProcessPower)
{
}

PowerProcess::PowerProcess(const PowerProcess& powerProcess) : BinaryProcess(powerProcess)
{
}

PowerProcess::~PowerProcess()
{
}

Process *PowerProcess::Replicate(void) const
{
	return (new PowerProcess(*this));
}

void PowerProcess::GenerateProcessData(const ShaderCompileData *compileData, ProcessData *data) const
{
	data->registerCount = 1;
	data->outputSize = 1;
	data->inputSize[0] = 1;
	data->inputSize[1] = 1;
}

int32 PowerProcess::GenerateShaderCode(const ShaderCompileData *compileData, const char **shaderCode) const
{
	static const char code[] =
	{
		"# = pow(%0, %1);\n"
	};

	shaderCode[0] = code;
	return (1);
}


MultiplyAddProcess::MultiplyAddProcess() : TrinaryProcess(kProcessMultiplyAdd)
{
}

MultiplyAddProcess::MultiplyAddProcess(const MultiplyAddProcess& multiplyAddProcess) : TrinaryProcess(multiplyAddProcess)
{
}

MultiplyAddProcess::~MultiplyAddProcess()
{
}

Process *MultiplyAddProcess::Replicate(void) const
{
	return (new MultiplyAddProcess(*this));
}

unsigned_int32 MultiplyAddProcess::GetPortFlags(int32 index) const
{
	return (kProcessPortOmissible);
}

const char *MultiplyAddProcess::GetPortName(int32 index) const
{
	static const char *const portName[3] =
	{
		"A", "B", "C"
	};

	return (portName[index]);
}

void MultiplyAddProcess::GenerateProcessData(const ShaderCompileData *compileData, ProcessData *data) const
{
	int32	size;

	const Route *routeA = GetPortRoute(0);
	const Route *routeB = GetPortRoute(1);
	const Route *routeC = GetPortRoute(2);

	if (routeA)
	{
		size = routeA->GenerateOutputSize();

		if (routeB)
		{
			data->registerCount = 1;

			size = Max(size, routeB->GenerateOutputSize());
			if (routeC)
			{
				size = Max(size, routeC->GenerateOutputSize());
			}
		}
		else
		{
			if (routeC)
			{
				data->registerCount = 1;
				size = Max(size, routeC->GenerateOutputSize());
			}
			else
			{
				data->passthruPort = 0;
			}
		}
	}
	else
	{
		if (routeB)
		{
			size = routeB->GenerateOutputSize();

			if (routeC)
			{
				data->registerCount = 1;
				size = Max(size, routeC->GenerateOutputSize());
			}
			else
			{
				data->passthruPort = 1;
			}
		}
		else
		{
			size = routeC->GenerateOutputSize();
			data->passthruPort = 2;
		}
	}

	data->outputSize = size;
	data->inputSize[0] = size;
	data->inputSize[1] = size;
	data->inputSize[2] = size;
}

int32 MultiplyAddProcess::GenerateShaderCode(const ShaderCompileData *compileData, const char **shaderCode) const
{
	const Route *routeA = GetPortRoute(0);
	const Route *routeB = GetPortRoute(1);
	const Route *routeC = GetPortRoute(2);

	if (routeA)
	{
		if (routeB)
		{
			if (routeC)
			{
				static const char code[] =
				{
					"# = %0 * %1 + %2;\n"
				};

				shaderCode[0] = code;
				return (1);
			}

			static const char code[] =
			{
				"# = %0 * %1;\n"
			};

			shaderCode[0] = code;
			return (1);
		}

		if (routeC)
		{
			static const char code[] =
			{
				"# = %0 + %2;\n"
			};

			shaderCode[0] = code;
			return (1);
		}
	}
	else
	{
		if ((routeB) && (routeC))
		{
			static const char code[] =
			{
				"# = %1 + %2;\n"
			};

			shaderCode[0] = code;
			return (1);
		}
	}

	return (0);
}


LerpProcess::LerpProcess() : TrinaryProcess(kProcessLerp)
{
}

LerpProcess::LerpProcess(const LerpProcess& lerpProcess) : TrinaryProcess(lerpProcess)
{
}

LerpProcess::~LerpProcess()
{
}

Process *LerpProcess::Replicate(void) const
{
	return (new LerpProcess(*this));
}

unsigned_int32 LerpProcess::GetPortFlags(int32 index) const
{
	return ((index < 2) ? kProcessPortOmissible : 0);
}

const char *LerpProcess::GetPortName(int32 index) const
{
	static const char *const portName[3] =
	{
		"A", "B", "t"
	};

	return (portName[index]);
}

void LerpProcess::GenerateProcessData(const ShaderCompileData *compileData, ProcessData *data) const
{
	int32	size;

	const Route *routeA = GetPortRoute(0);
	const Route *routeB = GetPortRoute(1);

	if (routeA)
	{
		size = routeA->GenerateOutputSize();

		if (routeB)
		{
			data->registerCount = 1;
			size = Max(Max(size, routeB->GenerateOutputSize()), GetPortRoute(2)->GenerateOutputSize());
		}
		else
		{
			data->passthruPort = 0;
		}
	}
	else
	{
		size = routeB->GenerateOutputSize();
		data->passthruPort = 1;
	}

	data->outputSize = size;
	data->inputSize[0] = size;
	data->inputSize[1] = size;
	data->inputSize[2] = size;
}

int32 LerpProcess::GenerateShaderCode(const ShaderCompileData *compileData, const char **shaderCode) const
{
	if ((GetPortRoute(0)) && (GetPortRoute(1)))
	{
		static const char code[] =
		{
			"# = " LERP "(%0, %1, %2);\n"
		};

		shaderCode[0] = code;
		return (1);
	}

	return (0);
}


DiffuseProcess::DiffuseProcess() : Process(kProcessDiffuse)
{
}

DiffuseProcess::DiffuseProcess(const DiffuseProcess& diffuseProcess) : Process(diffuseProcess)
{
}

DiffuseProcess::~DiffuseProcess()
{
}

Process *DiffuseProcess::Replicate(void) const
{
	return (new DiffuseProcess(*this));
}

int32 DiffuseProcess::GetPortCount(void) const
{
	return (1);
}

unsigned_int32 DiffuseProcess::GetPortFlags(int32 index) const
{
	return (kProcessPortOptional);
}

const char *DiffuseProcess::GetPortName(int32 index) const
{
	return ("N");
}

int32 DiffuseProcess::GenerateProcessSignature(const ShaderCompileData *compileData, unsigned_int32 *signature) const
{
	int32 count = Process::GenerateProcessSignature(compileData, signature);
	signature[count] = ((GetPortRoute(0)) || (compileData->renderable->TangentAvailable()));
	return (count + 1);
}

int32 DiffuseProcess::GenerateDerivedInterpolantTypes(const ShaderCompileData *compileData, ProcessType *type) const
{
	if ((GetPortRoute(0)) || (compileData->renderable->TangentAvailable()))
	{
		type[0] = kProcessTangentLightDirection;
		return (1);
	}

	type[0] = kProcessObjectNormal;
	type[1] = kProcessObjectLightDirection;
	return (2);
}

void DiffuseProcess::GenerateProcessData(const ShaderCompileData *compileData, ProcessData *data) const
{
	data->registerCount = 1;
	data->outputSize = 1;
	data->inputSize[0] = 3;
}

int32 DiffuseProcess::GenerateShaderCode(const ShaderCompileData *compileData, const char **shaderCode) const
{
	#if C4OPENGL

		static const char tangentCode[] =
		{
			"# = clamp(ldir.z, 0.0, 1.0);\n"
		};

		static const char normalCode[] =
		{
			"# = clamp(dot(%0, ldir), 0.0, 1.0);\n"
		};

		static const char objectCode[] =
		{
			"# = clamp(dot(onrm, oldr), 0.0, 1.0);\n"
		};

	#else

		static const char tangentCode[] =
		{
			"# = saturate(ldir.z);\n"
		};

		static const char normalCode[] =
		{
			"# = saturate(dot(%0, ldir));\n"
		};

		static const char objectCode[] =
		{
			"# = saturate(dot(onrm, oldr));\n"
		};

	#endif

	if (GetPortRoute(0))
	{
		shaderCode[0] = normalCode;
	}
	else if (compileData->renderable->TangentAvailable())
	{
		shaderCode[0] = tangentCode;
	}
	else
	{
		shaderCode[0] = objectCode;
	}

	return (1);
}


SpecularProcess::SpecularProcess() : Process(kProcessSpecular)
{
}

SpecularProcess::SpecularProcess(const SpecularProcess& specularProcess) : Process(specularProcess)
{
}

SpecularProcess::~SpecularProcess()
{
}

Process *SpecularProcess::Replicate(void) const
{
	return (new SpecularProcess(*this));
}

int32 SpecularProcess::GetPortCount(void) const
{
	return (2);
}

unsigned_int32 SpecularProcess::GetPortFlags(int32 index) const
{
	return ((index == 0) ? kProcessPortOptional : 0);
}

const char *SpecularProcess::GetPortName(int32 index) const
{
	static const char *const portName[2] =
	{
		"N", "p"
	};

	return (portName[index]);
}

int32 SpecularProcess::GenerateProcessSignature(const ShaderCompileData *compileData, unsigned_int32 *signature) const
{
	int32 count = Process::GenerateProcessSignature(compileData, signature);
	signature[count] = ((GetPortRoute(0)) || (compileData->renderable->TangentAvailable()));
	return (count + 1);
}

int32 SpecularProcess::GenerateDerivedInterpolantTypes(const ShaderCompileData *compileData, ProcessType *type) const
{
	if ((GetPortRoute(0)) || (compileData->renderable->TangentAvailable()))
	{
		type[0] = kProcessTangentHalfwayDirection;
		return (1);
	}

	type[0] = kProcessObjectNormal;
	type[1] = kProcessObjectHalfwayDirection;
	return (2);
}

void SpecularProcess::GenerateProcessData(const ShaderCompileData *compileData, ProcessData *data) const
{
	data->registerCount = 1;
	data->outputSize = 1;
	data->inputSize[0] = 3;
	data->inputSize[1] = 1;
}

int32 SpecularProcess::GenerateShaderCode(const ShaderCompileData *compileData, const char **shaderCode) const
{
	#if C4OPENGL

		static const char tangentCode[] =
		{
			"# = pow(clamp(hdir.z, 0.0, 1.0), %1);\n"
		};

		static const char normalCode[] =
		{
			"# = pow(clamp(dot(%0, hdir), 0.0, 1.0), %1);\n"
		};

		static const char objectCode[] =
		{
			"# = pow(clamp(dot(onrm, ohdr), 0.0, 1.0), %1);\n"
		};

	#else

		static const char tangentCode[] =
		{
			"# = pow(saturate(hdir.z), %1);\n"
		};

		static const char normalCode[] =
		{
			"# = pow(saturate(dot(%0, hdir)), %1);\n"
		};

		static const char objectCode[] =
		{
			"# = pow(saturate(dot(onrm, ohdr)), %1);\n"
		};

	#endif

	if (GetPortRoute(0))
	{
		shaderCode[0] = normalCode;
	}
	else if (compileData->renderable->TangentAvailable())
	{
		shaderCode[0] = tangentCode;
	}
	else
	{
		shaderCode[0] = objectCode;
	}

	return (1);
}


MicrofacetProcess::MicrofacetProcess() : Process(kProcessMicrofacet)
{
	microfacetParams.microfacetColor = K::white;
	microfacetParams.microfacetSlope.Set(0.5F, 0.5F);

	microfacetData = &microfacetParams;
}

MicrofacetProcess::MicrofacetProcess(const MicrofacetProcess& microfacetProcess) : Process(microfacetProcess)
{
	microfacetParams.microfacetColor = microfacetProcess.microfacetParams.microfacetColor;
	microfacetParams.microfacetSlope = microfacetProcess.microfacetParams.microfacetSlope;

	microfacetData = &microfacetParams;
}

MicrofacetProcess::~MicrofacetProcess()
{
	MicrofacetAttribute::MicrofacetTexture *texture = microfacetParams.microfacetTexture;
	if (texture)
	{
		texture->Release();
	}
}

Process *MicrofacetProcess::Replicate(void) const
{
	return (new MicrofacetProcess(*this));
}

void MicrofacetProcess::Pack(Packer& data, unsigned_int32 packFlags) const
{
	Process::Pack(data, packFlags);

	data << microfacetParams.microfacetColor;
	data << microfacetParams.microfacetSlope;
}

void MicrofacetProcess::Unpack(Unpacker& data, unsigned_int32 unpackFlags)
{
	Process::Unpack(data, unpackFlags);

	data >> microfacetParams.microfacetColor;
	data >> microfacetParams.microfacetSlope;
}

int32 MicrofacetProcess::GetSettingCount(void) const
{
	return (Process::GetSettingCount() + 3);
}

Setting *MicrofacetProcess::GetSetting(int32 index) const
{
	int32 count = Process::GetSettingCount();
	if (index < count)
	{
		return (Process::GetSetting(index));
	}

	const StringTable *table = TheInterfaceMgr->GetStringTable();

	if (index == count)
	{
		const char *title = table->GetString(StringID('PROC', kProcessMicrofacet, 'COLR'));
		const char *picker = table->GetString(StringID('PROC', kProcessMicrofacet, 'PICK'));
		return (new ColorSetting('COLR', microfacetParams.microfacetColor, title, picker));
	}

	if (index == count + 1)
	{
		const char *title = table->GetString(StringID('PROC', kProcessMicrofacet, 'SLPX'));
		return (new FloatSetting('SLPX', microfacetParams.microfacetSlope.x, title, 0.01F, 0.5F, 0.01F));
	}

	if (index == count + 2)
	{
		const char *title = table->GetString(StringID('PROC', kProcessMicrofacet, 'SLPY'));
		return (new FloatSetting('SLPY', microfacetParams.microfacetSlope.y, title, 0.01F, 0.5F, 0.01F));
	}

	return (nullptr);
}

void MicrofacetProcess::SetSetting(const Setting *setting)
{
	Type identifier = setting->GetSettingIdentifier();

	if (identifier == 'COLR')
	{
		microfacetParams.microfacetColor = static_cast<const ColorSetting *>(setting)->GetColor();
		microfacetParams.Invalidate();
	}
	else if (identifier == 'SLPX')
	{
		microfacetParams.microfacetSlope.x = static_cast<const FloatSetting *>(setting)->GetFloatValue();
		microfacetParams.Invalidate();
	}
	else if (identifier == 'SLPY')
	{
		microfacetParams.microfacetSlope.y = static_cast<const FloatSetting *>(setting)->GetFloatValue();
		microfacetParams.Invalidate();
	}
	else
	{
		Process::SetSetting(setting);
	}
}

bool MicrofacetProcess::operator ==(const Process& process) const
{
	if (Process::operator ==(process))
	{
		const MicrofacetProcess& microfacetProcess = static_cast<const MicrofacetProcess&>(process);
		return ((microfacetParams.microfacetColor == microfacetProcess.microfacetParams.microfacetColor) && (microfacetParams.microfacetSlope == microfacetProcess.microfacetParams.microfacetSlope));
	}

	return (false);
}

int32 MicrofacetProcess::GetPortCount(void) const
{
	return (1);
}

unsigned_int32 MicrofacetProcess::GetPortFlags(int32 index) const
{
	return (kProcessPortOptional);
}

const char *MicrofacetProcess::GetPortName(int32 index) const
{
	return ("N");
}

void MicrofacetProcess::ReferenceStateParams(const Process *process)
{
	microfacetData = static_cast<const MicrofacetProcess *>(process)->microfacetData;
}

int32 MicrofacetProcess::GenerateProcessSignature(const ShaderCompileData *compileData, unsigned_int32 *signature) const
{
	int32 count = Process::GenerateProcessSignature(compileData, signature);
	signature += count;

	signature[0] = *reinterpret_cast<const unsigned_int32 *>(&microfacetParams.microfacetColor.red);
	signature[1] = *reinterpret_cast<const unsigned_int32 *>(&microfacetParams.microfacetColor.green);
	signature[2] = *reinterpret_cast<const unsigned_int32 *>(&microfacetParams.microfacetColor.blue);
	signature[3] = *reinterpret_cast<const unsigned_int32 *>(&microfacetParams.microfacetSlope.x);
	signature[4] = *reinterpret_cast<const unsigned_int32 *>(&microfacetParams.microfacetSlope.y);

	return (count + 5);
}

int32 MicrofacetProcess::GenerateDerivedInterpolantTypes(const ShaderCompileData *compileData, ProcessType *type) const
{
	type[0] = kProcessTangentHalfwayDirection;
	return (1);
}

void MicrofacetProcess::GenerateProcessData(const ShaderCompileData *compileData, ProcessData *data) const
{
	data->registerCount = 1;
	data->outputSize = 3;
	data->inputSize[0] = 3;
	data->inputSize[1] = 1;

	data->textureCount = 1;
	data->textureObject[0] = MicrofacetAttribute::GetTextureObject(microfacetData);

	// The literal constants must be stored after the MicrofacetAttribute::GetTextureObject()
	// function is called because that's where microfacetThreshold is calculated.

	float value = 1.0F / (1.0F - microfacetData->microfacetThreshold);

	data->literalCount = 2;
	data->literalData[0].literalType = 'MTH1';
	data->literalData[0].literalValue = value;
	data->literalData[1].literalType = 'MTH2';
	data->literalData[1].literalValue = 1.0F - value;
}

int32 MicrofacetProcess::GenerateShaderCode(const ShaderCompileData *compileData, const char **shaderCode) const
{
	if (microfacetParams.microfacetSlope.x == microfacetParams.microfacetSlope.y)
	{
		static const char flatCode[] =
		{
			"temp.x = hdir.z * &MTH1 + &MTH2;\n"
			"temp.y = dot(ldir, hdir);\n"
			"# = %TRG0(%IMG0, temp.xy).xyz / vdir.z;\n"
		};

		static const char bumpCode[] =
		{
			"temp.x = dot(%0, hdir) * &MTH1 + &MTH2;\n"
			"temp.y = dot(ldir, hdir);\n"
			"# = %TRG0(%IMG0, temp.xy).xyz / dot(%0, vdir);\n"
		};

		if (GetPortRoute(0))
		{
			shaderCode[0] = bumpCode;
		}
		else
		{
			shaderCode[0] = flatCode;
		}
	}
	else
	{
		static const char flatCode[] =
		{
			"temp.x = hdir.z * &MTH1 + &MTH2;\n"
			"temp.y = dot(ldir, hdir);\n"
			"temp.w = hdir.x * hdir.x;\n"
			"temp.z = temp.w / (temp.w + hdir.y * hdir.y);\n"
			"# = %TRG0(%IMG0, temp.xyz).xyz / vdir.z;\n"
		};

		static const char bumpCode[] =
		{
			"temp.x = dot(%0, hdir) * &MTH1 + &MTH2;\n"
			"temp.y = dot(ldir, hdir);\n"
			"temp.w = hdir.x * hdir.x;\n"
			"temp.z = temp.w / (temp.w + hdir.y * hdir.y);\n"
			"# = %TRG0(%IMG0, temp.xyz).xyz / dot(%0, vdir);\n"
		};

		if (GetPortRoute(0))
		{
			shaderCode[0] = bumpCode;
		}
		else
		{
			shaderCode[0] = flatCode;
		}
	}

	return (1);
}


TerrainDiffuseProcess::TerrainDiffuseProcess() : Process(kProcessTerrainDiffuse)
{
}

TerrainDiffuseProcess::TerrainDiffuseProcess(const TerrainDiffuseProcess& terrainDiffuseProcess) : Process(terrainDiffuseProcess)
{
}

TerrainDiffuseProcess::~TerrainDiffuseProcess()
{
}

Process *TerrainDiffuseProcess::Replicate(void) const
{
	return (new TerrainDiffuseProcess(*this));
}

int32 TerrainDiffuseProcess::GetPortCount(void) const
{
	return (3);
}

unsigned_int32 TerrainDiffuseProcess::GetPortFlags(int32 index) const
{
	return (kProcessPortOptional);
}

const char *TerrainDiffuseProcess::GetPortName(int32 index) const
{
	static const char *const portName[3] =
	{
		"N1", "N2", "N3"
	};

	return (portName[index]);
}

bool TerrainDiffuseProcess::BumpEnabled(void) const
{
	if (!(TheGraphicsMgr->GetRenderOptionFlags() & kRenderOptionTerrainBumps))
	{
		return (false);
	}

	return ((GetPortRoute(0)) && (GetPortRoute(1)) && (GetPortRoute(2)));
}

int32 TerrainDiffuseProcess::GenerateProcessSignature(const ShaderCompileData *compileData, unsigned_int32 *signature) const
{
	int32 count = Process::GenerateProcessSignature(compileData, signature);
	signature[count] = BumpEnabled();
	return (count + 1);
}

int32 TerrainDiffuseProcess::GenerateDerivedInterpolantTypes(const ShaderCompileData *compileData, ProcessType *type) const
{
	if (BumpEnabled())
	{
		type[0] = kProcessTriplanarBlend;
		type[1] = kProcessTerrainLightDirection;
		return (2);
	}

	type[0] = kProcessObjectLightDirection;
	return (1);
}

void TerrainDiffuseProcess::GenerateProcessData(const ShaderCompileData *compileData, ProcessData *data) const
{
	data->registerCount = 1;
	data->outputSize = 1;
	data->inputSize[0] = 3;
	data->inputSize[1] = 3;
	data->inputSize[2] = 3;

	if (!BumpEnabled())
	{
		data->interpolantCount = 1;
		data->interpolantType[0] = 'NRML';
	}
}

int32 TerrainDiffuseProcess::GenerateShaderCode(const ShaderCompileData *compileData, const char **shaderCode) const
{
	#if C4OPENGL

		static const char flatCode[] =
		{
			"# = clamp(dot($NRML, oldr), 0.0, 1.0);\n"
		};

		static const char bumpCode[] =
		{
			"temp.x = clamp(dot(%0, tldr), 0.0, 1.0);\n"
			"temp.y = clamp(dot(%1, tldr), 0.0, 1.0);\n"
			"temp.z = clamp(dot(%2, tld2), 0.0, 1.0);\n"
			"# = dot(temp.xyz, tbld);\n"
		};

	#else

		static const char flatCode[] =
		{
			"# = saturate(dot($NRML, oldr));\n"
		};

		static const char bumpCode[] =
		{
			"temp.x = saturate(dot(%0, tldr));\n"
			"temp.y = saturate(dot(%1, tldr));\n"
			"temp.z = saturate(dot(%2, tld2));\n"
			"# = dot(temp.xyz, tbld);\n"
		};

	#endif

	if (BumpEnabled())
	{
		shaderCode[0] = bumpCode;
	}
	else
	{
		shaderCode[0] = flatCode;
	}

	return (1);
}


TerrainSpecularProcess::TerrainSpecularProcess() : Process(kProcessTerrainSpecular)
{
}

TerrainSpecularProcess::TerrainSpecularProcess(const TerrainSpecularProcess& terrainSpecularProcess) : Process(terrainSpecularProcess)
{
}

TerrainSpecularProcess::~TerrainSpecularProcess()
{
}

Process *TerrainSpecularProcess::Replicate(void) const
{
	return (new TerrainSpecularProcess(*this));
}

int32 TerrainSpecularProcess::GetPortCount(void) const
{
	return (4);
}

unsigned_int32 TerrainSpecularProcess::GetPortFlags(int32 index) const
{
	return ((index < 3) ? kProcessPortOptional : 0);
}

const char *TerrainSpecularProcess::GetPortName(int32 index) const
{
	static const char *const portName[4] =
	{
		"N1", "N2", "N3", "p"
	};

	return (portName[index]);
}

bool TerrainSpecularProcess::BumpEnabled(void) const
{
	if (!(TheGraphicsMgr->GetRenderOptionFlags() & kRenderOptionTerrainBumps))
	{
		return (false);
	}

	return ((GetPortRoute(0)) && (GetPortRoute(1)) && (GetPortRoute(2)));
}

int32 TerrainSpecularProcess::GenerateProcessSignature(const ShaderCompileData *compileData, unsigned_int32 *signature) const
{
	int32 count = Process::GenerateProcessSignature(compileData, signature);
	signature[count] = BumpEnabled();
	return (count + 1);
}

int32 TerrainSpecularProcess::GenerateDerivedInterpolantTypes(const ShaderCompileData *compileData, ProcessType *type) const
{
	if (BumpEnabled())
	{
		type[0] = kProcessTriplanarBlend;
		type[1] = kProcessTerrainHalfwayDirection;
		return (2);
	}

	type[0] = kProcessObjectHalfwayDirection;
	return (1);
}

void TerrainSpecularProcess::GenerateProcessData(const ShaderCompileData *compileData, ProcessData *data) const
{
	data->registerCount = 1;
	data->outputSize = 1;
	data->inputSize[0] = 3;
	data->inputSize[1] = 3;
	data->inputSize[2] = 3;
	data->inputSize[3] = 1;

	if (!BumpEnabled())
	{
		data->interpolantCount = 1;
		data->interpolantType[0] = 'NRML';
	}
}

int32 TerrainSpecularProcess::GenerateShaderCode(const ShaderCompileData *compileData, const char **shaderCode) const
{
	#if C4OPENGL

		static const char flatCode[] =
		{
			"# = pow(clamp(dot($NRML, ohdr), 0.0, 1.0), %3);\n"
		};

		static const char bumpCode[] =
		{
			"temp.x = clamp(dot(%0, thdr), 0.0, 1.0);\n"
			"temp.y = clamp(dot(%1, thdr), 0.0, 1.0);\n"
			"temp.z = clamp(dot(%2, thd2), 0.0, 1.0);\n"
			"# = pow(dot(temp.xyz, tbld), %3);\n"
		};

	#else

		static const char flatCode[] =
		{
			"# = pow(saturate(dot($NRML, ohdr)), %3);\n"
		};

		static const char bumpCode[] =
		{
			"temp.x = saturate(dot(%0, thdr));\n"
			"temp.y = saturate(dot(%1, thdr));\n"
			"temp.z = saturate(dot(%2, thd2));\n"
			"# = pow(dot(temp.xyz, tbld), %3);\n"
		};

	#endif

	if (BumpEnabled())
	{
		shaderCode[0] = bumpCode;
	}
	else
	{
		shaderCode[0] = flatCode;
	}

	return (1);
}


GenerateImpostorNormalProcess::GenerateImpostorNormalProcess() : Process(kProcessGenerateImpostorNormal)
{
}

GenerateImpostorNormalProcess::GenerateImpostorNormalProcess(const GenerateImpostorNormalProcess& generateImpostorNormalProcess) : Process(generateImpostorNormalProcess)
{
}

GenerateImpostorNormalProcess::~GenerateImpostorNormalProcess()
{
}

Process *GenerateImpostorNormalProcess::Replicate(void) const
{
	return (new GenerateImpostorNormalProcess(*this));
}

void GenerateImpostorNormalProcess::GenerateProcessData(const ShaderCompileData *compileData, ProcessData *data) const
{
	data->registerCount = 1;
	data->outputSize = 3;

	data->interpolantCount = 1;
	data->interpolantType[0] = 'NRMC';
}

int32 GenerateImpostorNormalProcess::GenerateShaderCode(const ShaderCompileData *compileData, const char **shaderCode) const
{
	static const char code[] =
	{
		"# = normalize($NRMC) * " FLOAT3 "(0.5, -0.5, -0.5) + " FLOAT3 "(0.5, 0.5, 0.5);\n"
	};

	shaderCode[0] = code;
	return (1);
}


ImpostorDepthProcess::ImpostorDepthProcess() : InterpolantProcess(kProcessImpostorDepth)
{
}

ImpostorDepthProcess::ImpostorDepthProcess(const ImpostorDepthProcess& impostorDepthProcess) : InterpolantProcess(impostorDepthProcess)
{
}

ImpostorDepthProcess::~ImpostorDepthProcess()
{
}

Process *ImpostorDepthProcess::Replicate(void) const
{
	return (new ImpostorDepthProcess(*this));
}


CombineNormalsProcess::CombineNormalsProcess() : Process(kProcessCombineNormals)
{
}

CombineNormalsProcess::CombineNormalsProcess(const CombineNormalsProcess& combineNormalsProcess) : Process(combineNormalsProcess)
{
}

CombineNormalsProcess::~CombineNormalsProcess()
{
}

Process *CombineNormalsProcess::Replicate(void) const
{
	return (new CombineNormalsProcess(*this));
}

int32 CombineNormalsProcess::GetPortCount(void) const
{
	return (3);
}

unsigned_int32 CombineNormalsProcess::GetPortFlags(int32 index) const
{
	return ((index < 2) ? 0 : kProcessPortOptional);
}

const char *CombineNormalsProcess::GetPortName(int32 index) const
{
	static const char *const portName[3] =
	{
		"N1", "N2", "t"
	};

	return (portName[index]);
}

void CombineNormalsProcess::GenerateProcessData(const ShaderCompileData *compileData, ProcessData *data) const
{
	data->registerCount = 1;
	if (GetPortRoute(2))
	{
		data->temporaryCount = 1;
	}

	data->outputSize = 3;
	data->inputSize[0] = 3;
	data->inputSize[1] = 3;
	data->inputSize[2] = 1;
}

int32 CombineNormalsProcess::GenerateShaderCode(const ShaderCompileData *compileData, const char **shaderCode) const
{
	if (GetPortRoute(2))
	{
		static const char code[] =
		{
			"temp.xy = " LERP "(%0.xy * %1.z, %1.xy * %0.z, %2);\n"
			"temp.z = %0.z * %1.z;\n"
			"# = normalize(temp.xyz);\n"
		};

		shaderCode[0] = code;
	}
	else
	{
		static const char code[] =
		{
			"temp.xy = %0.xy * %1.z + %1.xy * %0.z;\n"
			"temp.z = %0.z * %1.z;\n"
			"# = normalize(temp.xyz);\n"
		};

		shaderCode[0] = code;
	}

	return (1);
}


FrontNormalProcess::FrontNormalProcess() : Process(kProcessFrontNormal)
{
}

FrontNormalProcess::FrontNormalProcess(const FrontNormalProcess& frontNormalProcess) : Process(frontNormalProcess)
{
}

FrontNormalProcess::~FrontNormalProcess()
{
}

Process *FrontNormalProcess::Replicate(void) const
{
	return (new FrontNormalProcess(*this));
}

int32 FrontNormalProcess::GetPortCount(void) const
{
	return (1);
}

const char *FrontNormalProcess::GetPortName(int32 index) const
{
	return ("N");
}

int32 FrontNormalProcess::GenerateDerivedInterpolantTypes(const ShaderCompileData *compileData, ProcessType *type) const
{
	type[0] = kProcessTangentLightDirection;
	return (1);
}

void FrontNormalProcess::GenerateProcessData(const ShaderCompileData *compileData, ProcessData *data) const
{
	const Route *route = GetFirstIncomingEdge();
	ProcessType type = route->GetStartElement()->GetBaseProcessType();

	if ((route->GetPreviousOutgoingEdge()) || (route->GetNextOutgoingEdge()) || (type == kProcessConstant) || (type == kProcessParameter) || (type == kProcessInterpolant) || (type == kProcessDerived) || (route->GetRouteNegation()) || (route->GetRouteSwizzle() != 'xyzw'))
	{
		data->registerCount = 1;
	}
	else
	{
		data->passthruPort = 0;
	}

	data->outputSize = 3;
	data->inputSize[0] = 3;
}

int32 FrontNormalProcess::GenerateShaderCode(const ShaderCompileData *compileData, const char **shaderCode) const
{
	static const char passthruCode[] =
	{
		"if (ldir.z < 0.0) #.z = -%0.z;\n"
	};

	static const char moveCode[] =
	{
		"#.xy = %0.xy;\n"
		"#.z = (ldir.z < 0.0) ? -%0.z : %0.z;\n"
	};

	if (GetProcessData()->passthruPort >= 0)
	{
		shaderCode[0] = passthruCode;
	}
	else
	{
		shaderCode[0] = moveCode;
	}

	return (1);
}


ReflectVectorProcess::ReflectVectorProcess() : Process(kProcessReflectVector)
{
}

ReflectVectorProcess::ReflectVectorProcess(const ReflectVectorProcess& reflectVectorProcess) : Process(reflectVectorProcess)
{
}

ReflectVectorProcess::~ReflectVectorProcess()
{
}

Process *ReflectVectorProcess::Replicate(void) const
{
	return (new ReflectVectorProcess(*this));
}

int32 ReflectVectorProcess::GetPortCount(void) const
{
	return (2);
}

unsigned_int32 ReflectVectorProcess::GetPortFlags(int32 index) const
{
	return ((index == 0) ? 0 : kProcessPortOptional);
}

const char *ReflectVectorProcess::GetPortName(int32 index) const
{
	static const char *const portName[2] =
	{
		"V", "N"
	};

	return (portName[index]);
}

void ReflectVectorProcess::GenerateProcessData(const ShaderCompileData *compileData, ProcessData *data) const
{
	data->registerCount = 1;
	data->outputSize = 3;
	data->inputSize[0] = 3;
	data->inputSize[1] = 3;
}

int32 ReflectVectorProcess::GenerateShaderCode(const ShaderCompileData *compileData, const char **shaderCode) const
{
	static const char flatCode[] =
	{
		"# = %0 * " FLOAT3 "(-1.0, -1.0, 1.0);\n"
	};

	static const char bumpCode[] =
	{
		"# = %1 * (dot(%0, %1) * 2.0) - %0;\n"
	};

	if (GetPortRoute(1))
	{
		shaderCode[0] = bumpCode;
	}
	else
	{
		shaderCode[0] = flatCode;
	}

	return (1);
}


LinearRampProcess::LinearRampProcess() : UnaryProcess(kProcessLinearRamp)
{
	rampCenter = 0.5F;
	rampWidth = 0.1F;
}

LinearRampProcess::LinearRampProcess(const LinearRampProcess& linearRampProcess) : UnaryProcess(linearRampProcess)
{
	rampCenter = linearRampProcess.rampCenter;
	rampWidth = linearRampProcess.rampWidth;
}

LinearRampProcess::~LinearRampProcess()
{
}

Process *LinearRampProcess::Replicate(void) const
{
	return (new LinearRampProcess(*this));
}

void LinearRampProcess::Pack(Packer& data, unsigned_int32 packFlags) const
{
	UnaryProcess::Pack(data, packFlags);

	data << rampCenter;
	data << rampWidth;
}

void LinearRampProcess::Unpack(Unpacker& data, unsigned_int32 unpackFlags)
{
	UnaryProcess::Unpack(data, unpackFlags);

	data >> rampCenter;
	data >> rampWidth;
}

int32 LinearRampProcess::GetSettingCount(void) const
{
	return (UnaryProcess::GetSettingCount() + 2);
}

Setting *LinearRampProcess::GetSetting(int32 index) const
{
	int32 count = UnaryProcess::GetSettingCount();
	if (index < count)
	{
		return (UnaryProcess::GetSetting(index));
	}

	const StringTable *table = TheInterfaceMgr->GetStringTable();

	if (index == count)
	{
		const char *title = table->GetString(StringID('PROC', kProcessLinearRamp, 'CENT'));
		return (new TextSetting('CENT', rampCenter, title));
	}

	if (index == count + 1)
	{
		const char *title = table->GetString(StringID('PROC', kProcessLinearRamp, 'WIDE'));
		return (new FloatSetting('WIDE', rampWidth, title, 0.01F, 0.99F, 0.01F));
	}

	return (nullptr);
}

void LinearRampProcess::SetSetting(const Setting *setting)
{
	Type identifier = setting->GetSettingIdentifier();

	if (identifier == 'CENT')
	{
		rampCenter = Text::StringToFloat(static_cast<const TextSetting *>(setting)->GetText());
	}
	else if (identifier == 'WIDE')
	{
		rampWidth = static_cast<const FloatSetting *>(setting)->GetFloatValue();
	}
	else
	{
		UnaryProcess::SetSetting(setting);
	}
}

bool LinearRampProcess::operator ==(const Process& process) const
{
	if (UnaryProcess::operator ==(process))
	{
		const LinearRampProcess& linearRampProcess = static_cast<const LinearRampProcess&>(process);
		return ((rampCenter == linearRampProcess.rampCenter) && (rampWidth == linearRampProcess.rampWidth));
	}

	return (false);
}

int32 LinearRampProcess::GenerateProcessSignature(const ShaderCompileData *compileData, unsigned_int32 *signature) const
{
	int32 count = UnaryProcess::GenerateProcessSignature(compileData, signature);
	signature[count] = *reinterpret_cast<const unsigned_int32 *>(&rampCenter);
	signature[count + 1] = *reinterpret_cast<const unsigned_int32 *>(&rampWidth);
	return (count + 2);
}

void LinearRampProcess::GenerateProcessData(const ShaderCompileData *compileData, ProcessData *data) const
{
	UnaryProcess::GenerateProcessData(compileData, data);

	float scale = 1.0F / rampWidth;

	data->literalCount = 2;
	data->literalData[0].literalType = 'RMUL';
	data->literalData[0].literalValue = scale;
	data->literalData[1].literalType = 'RADD';
	data->literalData[1].literalValue = 0.5F - rampCenter * scale;
}

int32 LinearRampProcess::GenerateShaderCode(const ShaderCompileData *compileData, const char **shaderCode) const
{
	static const char code[] =
	{
		#if C4OPENGL

			"# = clamp(%0 * &RMUL + &RADD, 0.0, 1.0);\n"

		#else

			"# = saturate(%0 * &RMUL + &RADD);\n"

		#endif
	};

	shaderCode[0] = code;
	return (1);
}


SmoothParameterProcess::SmoothParameterProcess() : UnaryProcess(kProcessSmoothParameter)
{
}

SmoothParameterProcess::SmoothParameterProcess(const SmoothParameterProcess& smoothParameterProcess) : UnaryProcess(smoothParameterProcess)
{
}

SmoothParameterProcess::~SmoothParameterProcess()
{
}

Process *SmoothParameterProcess::Replicate(void) const
{
	return (new SmoothParameterProcess(*this));
}

const char *SmoothParameterProcess::GetPortName(int32 index) const
{
	return ("t");
}

int32 SmoothParameterProcess::GenerateShaderCode(const ShaderCompileData *compileData, const char **shaderCode) const
{
	static const char code[] =
	{
		"# = (3.0 - %0 * 2.0) * (%0 * %0);\n"
	};

	shaderCode[0] = code;
	return (1);
}


SteepParameterProcess::SteepParameterProcess() : UnaryProcess(kProcessSteepParameter)
{
}

SteepParameterProcess::SteepParameterProcess(const SteepParameterProcess& steepParameterProcess) : UnaryProcess(steepParameterProcess)
{
}

SteepParameterProcess::~SteepParameterProcess()
{
}

Process *SteepParameterProcess::Replicate(void) const
{
	return (new SteepParameterProcess(*this));
}

const char *SteepParameterProcess::GetPortName(int32 index) const
{
	return ("t");
}

int32 SteepParameterProcess::GenerateShaderCode(const ShaderCompileData *compileData, const char **shaderCode) const
{
	static const char code[] =
	{
		"# = %0 * 2.0 - %0 * %0;\n"
	};

	shaderCode[0] = code;
	return (1);
}


WorldTransformProcess::WorldTransformProcess() : Process(kProcessWorldTransform)
{
}

WorldTransformProcess::WorldTransformProcess(const WorldTransformProcess& worldTransformProcess) : Process(worldTransformProcess)
{
}

WorldTransformProcess::~WorldTransformProcess()
{
}

Process *WorldTransformProcess::Replicate(void) const
{
	return (new WorldTransformProcess(*this));
}

int32 WorldTransformProcess::GetPortCount(void) const
{
	return (1);
}

const char *WorldTransformProcess::GetPortName(int32 index) const
{
	return ("V");
}

void WorldTransformProcess::GenerateProcessData(const ShaderCompileData *compileData, ProcessData *data) const
{
	data->registerCount = 1;
	data->outputSize = 3;
	data->inputSize[0] = 3;

	data->interpolantCount = 3;
	data->interpolantType[0] = 'WTAN';
	data->interpolantType[1] = 'WBTN';
	data->interpolantType[2] = 'WNRM';
}

int32 WorldTransformProcess::GenerateShaderCode(const ShaderCompileData *compileData, const char **shaderCode) const
{
	static const char code[] =
	{
		"# = $WTAN * %0.x + $WBTN * %0.y + $WNRM * %0.z;\n"
	};

	shaderCode[0] = code;
	return (1);
}


DeltaDepthProcess::DeltaDepthProcess() : Process(kProcessDeltaDepth)
{
	deltaScale = 1.0F;
}

DeltaDepthProcess::DeltaDepthProcess(const DeltaDepthProcess& deltaDepthProcess) : Process(deltaDepthProcess)
{
	deltaScale = deltaDepthProcess.deltaScale;
}

DeltaDepthProcess::~DeltaDepthProcess()
{
}

Process *DeltaDepthProcess::Replicate(void) const
{
	return (new DeltaDepthProcess(*this));
}

void DeltaDepthProcess::Pack(Packer& data, unsigned_int32 packFlags) const
{
	Process::Pack(data, packFlags);

	data << deltaScale;
}

void DeltaDepthProcess::Unpack(Unpacker& data, unsigned_int32 unpackFlags)
{
	Process::Unpack(data, unpackFlags);

	data >> deltaScale;
}

int32 DeltaDepthProcess::GetSettingCount(void) const
{
	return (Process::GetSettingCount() + 1);
}

Setting *DeltaDepthProcess::GetSetting(int32 index) const
{
	int32 count = Process::GetSettingCount();
	if (index < count)
	{
		return (Process::GetSetting(index));
	}

	if (index == count)
	{
		const StringTable *table = TheInterfaceMgr->GetStringTable();
		const char *title = table->GetString(StringID('PROC', kProcessDeltaDepth, 'SCAL'));
		return (new TextSetting('SCAL', deltaScale, title));
	}

	return (nullptr);
}

void DeltaDepthProcess::SetSetting(const Setting *setting)
{
	Type identifier = setting->GetSettingIdentifier();

	if (identifier == 'SCAL')
	{
		deltaScale = Text::StringToFloat(static_cast<const TextSetting *>(setting)->GetText());
	}
	else
	{
		Process::SetSetting(setting);
	}
}

bool DeltaDepthProcess::operator ==(const Process& process) const
{
	if (Process::operator ==(process))
	{
		const DeltaDepthProcess& deltaDepthProcess = static_cast<const DeltaDepthProcess&>(process);
		return (deltaScale == deltaDepthProcess.deltaScale);
	}

	return (false);
}

bool DeltaDepthProcess::StructureEffectsEnabled(void)
{
	return ((TheGraphicsMgr->GetGraphicsActiveFlags() & kGraphicsActiveStructureEffects) != 0);
}

int32 DeltaDepthProcess::GenerateProcessSignature(const ShaderCompileData *compileData, unsigned_int32 *signature) const
{
	int32 count = Process::GenerateProcessSignature(compileData, signature);
	signature[count] = (StructureEffectsEnabled()) ? *reinterpret_cast<const unsigned_int32 *>(&deltaScale) : ~0;
	return (count + 1);
}

void DeltaDepthProcess::GenerateProcessData(const ShaderCompileData *compileData, ProcessData *data) const
{
	data->registerCount = 1;
	data->outputSize = 1;

	if (StructureEffectsEnabled())
	{
		data->temporaryCount = 1;

		data->literalCount = 1;
		data->literalData[0].literalType = 'DSCL';
		data->literalData[0].literalValue = deltaScale;

		data->textureCount = 1;
		data->textureObject[0] = TheGraphicsMgr->GetStructureTexture();
	}
}

int32 DeltaDepthProcess::GenerateShaderCode(const ShaderCompileData *compileData, const char **shaderCode) const
{
	static const char enabledCode[] =
	{
		#if C4OPENGL

			"temp.zw = %TRG0(%IMG0, gl_FragCoord.xy).zw" FRAGMENT_XY_MULTIPLIER ";\n"
			"# = clamp((temp.z + temp.w * 0.25 - 1.0 / gl_FragCoord.w) * &DSCL, 0.0, 1.0);\n"

		#elif C4PSSL //[ PS4

			// -- PS4 code hidden --

		#elif C4CG //[ PS3

			// -- PS3 code hidden --

		#endif //]
	};

	static const char disabledCode[] =
	{
		"# = 1.0;\n"
	};

	if (StructureEffectsEnabled())
	{
		shaderCode[0] = enabledCode;
	}
	else
	{
		shaderCode[0] = disabledCode;
	}

	return (1);
}


ParallaxProcess::ParallaxProcess() : TextureMapProcess(kProcessParallax)
{
}

ParallaxProcess::ParallaxProcess(const ParallaxProcess& parallaxProcess) : TextureMapProcess(parallaxProcess)
{
}

ParallaxProcess::~ParallaxProcess()
{
}

Process *ParallaxProcess::Replicate(void) const
{
	return (new ParallaxProcess(*this));
}

int32 ParallaxProcess::GetSettingCount(void) const
{
	return (TextureMapProcess::GetSettingCount() + 1);
}

Setting *ParallaxProcess::GetSetting(int32 index) const
{
	int32 count = TextureMapProcess::GetSettingCount();
	if (index < count)
	{
		return (TextureMapProcess::GetSetting(index));
	}

	if (index == count)
	{
		const StringTable *table = TheInterfaceMgr->GetStringTable();
		const char *title = table->GetString(StringID('PROC', kProcessParallax, 'DETL'));
		return (new BooleanSetting('DETL', ((GetProcessFlags() & kProcessHighDetail) != 0), title));
	}

	return (nullptr);
}

void ParallaxProcess::SetSetting(const Setting *setting)
{
	Type identifier = setting->GetSettingIdentifier();

	if (identifier == 'DETL')
	{
		if (static_cast<const BooleanSetting *>(setting)->GetBooleanValue())
		{
			SetProcessFlags(GetProcessFlags() | kProcessHighDetail);
		}
		else
		{
			SetProcessFlags(GetProcessFlags() & ~kProcessHighDetail);
		}
	}
	else
	{
		TextureMapProcess::SetSetting(setting);
	}
}

bool ParallaxProcess::ProcessEnabled(const ShaderCompileData *compileData) const
{
	if ((compileData->detailLevel > 0) && (GetProcessFlags() & kProcessHighDetail))
	{
		return (false);
	}

	return (TheGraphicsMgr->GetRenderOptionFlags() & kRenderOptionParallaxMapping);
}

int32 ParallaxProcess::GenerateProcessSignature(const ShaderCompileData *compileData, unsigned_int32 *signature) const
{
	int32 count = TextureMapProcess::GenerateProcessSignature(compileData, signature);
	signature[count] = ProcessEnabled(compileData);
	return (count + 1);
}

int32 ParallaxProcess::GenerateDerivedInterpolantTypes(const ShaderCompileData *compileData, ProcessType *type) const
{
	if (ProcessEnabled(compileData))
	{
		type[0] = kProcessTangentViewDirection;
		return (1);
	}

	return (0);
}

void ParallaxProcess::GenerateProcessData(const ShaderCompileData *compileData, ProcessData *data) const
{
	if (ProcessEnabled(compileData))
	{
		data->registerCount = 1;
		data->temporaryCount = 1;

		data->textureCount = 1;
		const Texture *texture = GetTexture();
		data->textureObject[0] = texture;

		compileData->shaderData->AddStateProc(&StateProc_CalculateParallaxScale, &texture->GetParallaxScale());
	}
	else
	{
		data->passthruPort = 0;
	}

	data->outputSize = 2;
	data->inputSize[0] = 2;
}

int32 ParallaxProcess::GenerateOutputIdentifier(const ShaderCompileData *compileData, const ShaderAllocationData *allocData, SwizzleData *swizzleData, char *name) const
{
	if (ProcessEnabled(compileData))
	{
		return (TextureMapProcess::GenerateOutputIdentifier(compileData, allocData, swizzleData, name));
	}

	return (GetPortRoute(0)->GenerateOutputIdentifier(compileData, allocData, swizzleData, name));
}

int32 ParallaxProcess::GenerateShaderCode(const ShaderCompileData *compileData, const char **shaderCode) const
{
	if (ProcessEnabled(compileData))
	{
		static const char code[] =
		{
			"temp = %TRG0(%IMG0, %0) * 2.0 - 1.0;\n"
			"tmp1.xy = " FPARAM(FRAGMENT_PARAM_PARALLAX_SCALE) ".xy * temp.w;\n"
			"# = %0 + vdir.xy * (tmp1.xy / max(dot(temp.xyz, vdir), 0.5));\n"
		};

		shaderCode[0] = code;
		return (1);
	}

	return (0);
}

void ParallaxProcess::StateProc_CalculateParallaxScale(const Renderable *renderable, const void *cookie)
{
	const Vector2D *parallaxScale = static_cast<const Vector2D *>(cookie);

	float scale = renderable->GetShaderDetailParameter() * 0.5F;
	float sx = parallaxScale->x * scale;
	float sy = parallaxScale->y * scale;
	Render::SetFragmentShaderParameter(kFragmentParamParallaxScale, sx, sy, 0.0F, 0.0F);
}


C4::KillProcess::KillProcess() : Process(kProcessKill)
{
	SetBaseProcessType(kProcessTerminal);
}

C4::KillProcess::KillProcess(const KillProcess& killProcess) : Process(killProcess)
{
}

C4::KillProcess::~KillProcess()
{
}

Process *C4::KillProcess::Replicate(void) const
{
	return (new KillProcess(*this));
}

int32 C4::KillProcess::GetPortCount(void) const
{
	return (2);
}

unsigned_int32 C4::KillProcess::GetPortFlags(int32 index) const
{
	return ((index == 0) ? 0 : kProcessPortOptional);
}

const char *C4::KillProcess::GetPortName(int32 index) const
{
	static const char *const portName[2] =
	{
		"A", "B"
	};

	return (portName[index]);
}

void C4::KillProcess::GenerateProcessData(const ShaderCompileData *compileData, ProcessData *data) const
{
	data->inputSize[0] = 1;
	data->inputSize[1] = 1;
}

int32 C4::KillProcess::GenerateShaderCode(const ShaderCompileData *compileData, const char **shaderCode) const
{
	static const char zeroCode[] =
	{
		"if (%0 < 0.0) discard;\n"
	};

	static const char fullCode[] =
	{
		"if (%0 < %1) discard;\n"
	};

	if (GetPortRoute(1))
	{
		shaderCode[0] = fullCode;
	}
	else
	{
		shaderCode[0] = zeroCode;
	}

	return (1);
}


ImpostorTransitionProcess::ImpostorTransitionProcess() : Process(kProcessImpostorTransition)
{
	SetBaseProcessType(kProcessTerminal);
}

ImpostorTransitionProcess::ImpostorTransitionProcess(const ImpostorTransitionProcess& impostorTransitionProcess) : Process(impostorTransitionProcess)
{
}

ImpostorTransitionProcess::~ImpostorTransitionProcess()
{
}

Process *ImpostorTransitionProcess::Replicate(void) const
{
	return (new ImpostorTransitionProcess(*this));
}

int32 ImpostorTransitionProcess::GenerateDerivedInterpolantTypes(const ShaderCompileData *compileData, ProcessType *type) const
{
	type[0] = kProcessImpostorBlend;
	return (1);
}

void ImpostorTransitionProcess::GenerateProcessData(const ShaderCompileData *compileData, ProcessData *data) const
{
	data->interpolantCount = 1;
	data->interpolantType[0] = 'IXBL';
}

int32 ImpostorTransitionProcess::GenerateShaderCode(const ShaderCompileData *compileData, const char **shaderCode) const
{
	static const char code[] =
	{
		"if ($IXBL < ibld.y) discard;\n"
	};

	shaderCode[0] = code;
	return (1);
}


GeometryTransitionProcess::GeometryTransitionProcess() : Process(kProcessGeometryTransition)
{
	SetBaseProcessType(kProcessTerminal);

	textureObject = Texture::Get("C4/screen");
}

GeometryTransitionProcess::GeometryTransitionProcess(const GeometryTransitionProcess& geometryTransitionProcess) : Process(geometryTransitionProcess)
{
	textureObject = geometryTransitionProcess.textureObject;
	textureObject->Retain();
}

GeometryTransitionProcess::~GeometryTransitionProcess()
{
	textureObject->Release();
}

Process *GeometryTransitionProcess::Replicate(void) const
{
	return (new GeometryTransitionProcess(*this));
}

int32 GeometryTransitionProcess::GetSettingCount(void) const
{
	return (Process::GetSettingCount() + 1);
}

Setting *GeometryTransitionProcess::GetSetting(int32 index) const
{
	int32 count = Process::GetSettingCount();
	if (index < count)
	{
		return (Process::GetSetting(index));
	}

	if (index == count)
	{
		const StringTable *table = TheInterfaceMgr->GetStringTable();
		const char *title = table->GetString(StringID('PROC', kProcessGeometryTransition, 'DETL'));
		return (new BooleanSetting('DETL', ((GetProcessFlags() & kProcessLowDetail) != 0), title));
	}

	return (nullptr);
}

void GeometryTransitionProcess::SetSetting(const Setting *setting)
{
	Type identifier = setting->GetSettingIdentifier();

	if (identifier == 'DETL')
	{
		unsigned_int32 flags = GetProcessFlags();
		if (static_cast<const BooleanSetting *>(setting)->GetBooleanValue())
		{
			SetProcessFlags(flags | kProcessLowDetail);
		}
		else
		{
			SetProcessFlags(flags & ~kProcessLowDetail);
		}
	}
	else
	{
		Process::SetSetting(setting);
	}
}

bool GeometryTransitionProcess::ProcessEnabled(const ShaderCompileData *compileData) const
{
	return ((compileData->detailLevel > 0) || (!(GetProcessFlags() & kProcessLowDetail)));
}

int32 GeometryTransitionProcess::GenerateProcessSignature(const ShaderCompileData *compileData, unsigned_int32 *signature) const
{
	int32 count = Process::GenerateProcessSignature(compileData, signature);
	signature[count] = ProcessEnabled(compileData);
	return (count + 1);
}

void GeometryTransitionProcess::GenerateProcessData(const ShaderCompileData *compileData, ProcessData *data) const
{
	if (ProcessEnabled(compileData))
	{
		data->interpolantCount = 1;
		data->interpolantType[0] = 'GITX';

		data->textureCount = 1;
		data->textureObject[0] = textureObject;
	}
}

int32 GeometryTransitionProcess::GenerateShaderCode(const ShaderCompileData *compileData, const char **shaderCode) const
{
	if (ProcessEnabled(compileData))
	{
		static const char code[] =
		{
			"temp.x = %TRG0(%IMG0, $GITX).x;\n"
			"if (" FPARAM(FRAGMENT_PARAM_IMPOSTOR_DISTANCE) ".x >= temp.x + 0.25) discard;\n"
		};

		shaderCode[0] = code;
		return (1);
	}

	return (0);
}


FireProcess::FireProcess() : TextureMapProcess(kProcessFire)
{
	SetTexture("C4/noise");

	fireParams.fireIntensity = 0.25F;
	fireParams.noiseVelocity[0].Set(0.0F, 0.0F);
	fireParams.noiseVelocity[1].Set(0.0F, 0.0F);
	fireParams.noiseVelocity[2].Set(0.0F, 0.0F);

	fireData = &fireParams;
}

FireProcess::FireProcess(const FireProcess& fireProcess) : TextureMapProcess(fireProcess)
{
	fireParams.fireIntensity = fireProcess.fireParams.fireIntensity;
	fireParams.noiseVelocity[0] = fireProcess.fireParams.noiseVelocity[0];
	fireParams.noiseVelocity[1] = fireProcess.fireParams.noiseVelocity[1];
	fireParams.noiseVelocity[2] = fireProcess.fireParams.noiseVelocity[2];

	fireData = &fireParams;
}

FireProcess::~FireProcess()
{
}

Process *FireProcess::Replicate(void) const
{
	return (new FireProcess(*this));
}

void FireProcess::Pack(Packer& data, unsigned_int32 packFlags) const
{
	TextureMapProcess::Pack(data, packFlags);

	data << fireParams.fireIntensity;
	data << fireParams.noiseVelocity[0];
	data << fireParams.noiseVelocity[1];
	data << fireParams.noiseVelocity[2];
}

void FireProcess::Unpack(Unpacker& data, unsigned_int32 unpackFlags)
{
	TextureMapProcess::Unpack(data, unpackFlags);

	data >> fireParams.fireIntensity;
	data >> fireParams.noiseVelocity[0];
	data >> fireParams.noiseVelocity[1];
	data >> fireParams.noiseVelocity[2];
}

int32 FireProcess::GetPortCount(void) const
{
	return (0);
}

void FireProcess::GenerateProcessData(const ShaderCompileData *compileData, ProcessData *data) const
{
	data->registerCount = 1;
	data->outputSize = 2;

	data->interpolantCount = 3;
	data->interpolantType[0] = 'FIRE';
	data->interpolantType[1] = 'FIR1';
	data->interpolantType[2] = 'FIR2';

	data->textureCount = 1;
	data->textureObject[0] = GetTexture();

	if (!(compileData->renderable->GetShaderFlags() & kShaderFireArrays))
	{
		compileData->shaderData->AddStateProc(&StateProc_SetFireParams, fireData);
	}
}

int32 FireProcess::GenerateShaderCode(const ShaderCompileData *compileData, const char **shaderCode) const
{
	static const char code[] =
	{
		"temp.xy = %TRG0(%IMG0, $FIR1.xy).xy * 2.0 - 3.0;\n"
		"temp.xy += %TRG0(%IMG0, $FIR1.zw).xy * 2.0;\n"
		"temp.xy += %TRG0(%IMG0, $FIR2).xy * 2.0;\n"
		"# = $FIRE.xy + temp.xy * $FIRE.z;\n"
	};

	shaderCode[0] = code;
	return (1);
}

void FireProcess::StateProc_SetFireParams(const Renderable *renderable, const void *cookie)
{
	const FireTextureAttribute::FireParams *params = static_cast<const FireTextureAttribute::FireParams *>(cookie);

	const Vector2D& velocity1 = params->noiseVelocity[0];
	const Vector2D& velocity2 = params->noiseVelocity[1];
	const Vector2D& velocity3 = params->noiseVelocity[2];

	Render::SetVertexShaderParameter(kVertexParamTexcoordVelocity0, velocity1.x, velocity1.y, velocity2.x, velocity2.y);
	Render::SetVertexShaderParameter(kVertexParamTexcoordVelocity1, velocity3.x, velocity3.y, 0.0F, 0.0F);
	Render::SetVertexShaderParameter(kVertexParamFireParams, params->fireIntensity, 0.0F, 0.0F, 0.0F);
}


DistortionProcess::DistortionProcess() : TextureMapProcess(kProcessDistortion)
{
}

DistortionProcess::DistortionProcess(const DistortionProcess& distortionProcess) : TextureMapProcess(distortionProcess)
{
}

DistortionProcess::~DistortionProcess()
{
}

Process *DistortionProcess::Replicate(void) const
{
	return (new DistortionProcess(*this));
}

void DistortionProcess::GenerateProcessData(const ShaderCompileData *compileData, ProcessData *data) const
{
	data->registerCount = 1;
	data->outputSize = 4;
	data->inputSize[0] = 2;

	data->textureCount = 1;
	data->textureObject[0] = GetTexture();

	data->interpolantCount = 1;
	data->interpolantType[0] = 'DDEP';

	if (compileData->renderable->GetTransformable())
	{
		compileData->shaderData->AddStateProc(&StateProc_TransformDistortionPlane);
	}
	else
	{
		compileData->shaderData->AddStateProc(&StateProc_CopyDistortionPlane);
	}
}

int32 DistortionProcess::GenerateShaderCode(const ShaderCompileData *compileData, const char **shaderCode) const
{
	static const char code[] =
	{
		"temp.xy = %TRG0(%IMG0, %0).xy * 2.0 - 1.0;\n"
		"temp.xy /= $DDEP;\n"
		"#.xy = temp.xy;\n"
		"#.zw = -temp.xy;\n"
	};

	shaderCode[0] = code;
	return (1);
}

void DistortionProcess::StateProc_CopyDistortionPlane(const Renderable *renderable, const void *cookie)
{
	Render::SetVertexShaderParameter(kVertexParamDistortCameraPlane, TheGraphicsMgr->GetDistortionPlane());
}

void DistortionProcess::StateProc_TransformDistortionPlane(const Renderable *renderable, const void *cookie)
{
	Render::SetVertexShaderParameter(kVertexParamDistortCameraPlane, TheGraphicsMgr->GetDistortionPlane() * renderable->GetTransformable()->GetWorldTransform());
}


FrameBufferProcess::FrameBufferProcess(unsigned_int32 index) : Process(kProcessFrameBuffer)
{
	frameBufferIndex = index;
}

FrameBufferProcess::FrameBufferProcess(const FrameBufferProcess& frameBufferProcess) : Process(frameBufferProcess)
{
	frameBufferIndex = frameBufferProcess.frameBufferIndex;
}

FrameBufferProcess::~FrameBufferProcess()
{
}

Process *FrameBufferProcess::Replicate(void) const
{
	return (new FrameBufferProcess(*this));
}

int32 FrameBufferProcess::GetPortCount(void) const
{
	return (1);
}

int32 FrameBufferProcess::GenerateProcessSignature(const ShaderCompileData *compileData, unsigned_int32 *signature) const
{
	int32 count = Process::GenerateProcessSignature(compileData, signature);
	signature[count] = frameBufferIndex;
	return (count + 1);
}

void FrameBufferProcess::GenerateProcessData(const ShaderCompileData *compileData, ProcessData *data) const
{
	data->registerCount = 1;
	data->textureCount = 1;
	data->outputSize = 4;

	const Render::TextureObject *texture = nullptr;
	switch (frameBufferIndex)
	{
		case kFrameBufferReflection:

			texture = TheGraphicsMgr->GetReflectionTexture();
			break;

		case kFrameBufferRefraction:

			texture = TheGraphicsMgr->GetRefractionTexture();
			break;

		case kFrameBufferStructure:

			if ((TheGraphicsMgr->GetGraphicsActiveFlags() & kGraphicsActiveStructureRendering) != 0)
			{
				texture = TheGraphicsMgr->GetStructureTexture();
			}

			break;

		case kFrameBufferVelocity:

			if ((TheGraphicsMgr->GetGraphicsActiveFlags() & kGraphicsActiveVelocityRendering) != 0)
			{
				texture = TheGraphicsMgr->GetVelocityTexture();
			}

			break;

		case kFrameBufferOcclusion:

			if ((TheGraphicsMgr->GetGraphicsActiveFlags() & kGraphicsActiveAmbientOcclusion) != 0)
			{
				texture = TheGraphicsMgr->GetOcclusionTexture();
			}

			break;

		case kFrameBufferGlowBloom:

			if ((TheGraphicsMgr->GetGraphicsActiveFlags() & kGraphicsActiveAmbientOcclusion) != 0)
			{
				texture = TheGraphicsMgr->GetGlowBloomTexture();
			}

			break;

		case kFrameBufferInfiniteShadow:

			texture = TheGraphicsMgr->GetInfiniteShadowTexture();
			break;

		case kFrameBufferPointShadow:

			texture = TheGraphicsMgr->GetPointShadowTexture();
			break;

		case kFrameBufferSpotShadow:

			texture = TheGraphicsMgr->GetSpotShadowTexture();
			break;
	}

	if (!texture)
	{
		texture = TheGraphicsMgr->GetPrimaryTexture();
	}

	data->textureObject[0] = texture;
	data->inputSize[0] = TextureMapProcess::GetTexcoordSize(texture);
}

int32 FrameBufferProcess::GenerateShaderCode(const ShaderCompileData *compileData, const char **shaderCode) const
{
	static const char regularCode[] =
	{
		"# = %TRG0(%IMG0, %0);\n"
	};

	static const char structureCode[] =
	{
		"temp = %TRG0(%IMG0, %0);\n"
		"# = " FLOAT4 "(abs(temp.x), abs(temp.y), temp.z * 0.03125, temp.w);\n"
	};

	static const char velocityCode[] =
	{
		"temp.xy = %TRG0(%IMG0, %0).xy;\n"
		"# = " FLOAT4 "(abs(temp.x), abs(temp.y), 0.0, 0.0);\n"
	};

	if (frameBufferIndex == kFrameBufferStructure)
	{
		shaderCode[0] = structureCode;
	}
	else if (frameBufferIndex == kFrameBufferVelocity)
	{
		shaderCode[0] = velocityCode;
	}
	else
	{
		shaderCode[0] = regularCode;
	}

	return (1);
}


InputProcess::InputProcess() : Process(kProcessInput)
{
}

InputProcess::InputProcess(const char *name) : Process(kProcessInput)
{
	inputPortName = name;
}

InputProcess::InputProcess(const InputProcess& inputProcess) : Process(inputProcess)
{
}

InputProcess::~InputProcess()
{
}

Process *InputProcess::Replicate(void) const
{
	return (new InputProcess(*this));
}

void InputProcess::Pack(Packer& data, unsigned_int32 packFlags) const
{
	Process::Pack(data, packFlags);

	PackHandle handle = data.BeginChunk('PORT');
	data << inputPortName;
	data.EndChunk(handle);

	data << TerminatorChunk;
}

void InputProcess::Unpack(Unpacker& data, unsigned_int32 unpackFlags)
{
	Process::Unpack(data, unpackFlags);
	UnpackChunkList<InputProcess>(data, unpackFlags);
}

int32 InputProcess::GetSettingCount(void) const
{
	return (Process::GetSettingCount() + 1);
}

Setting *InputProcess::GetSetting(int32 index) const
{
	int32 count = Process::GetSettingCount();
	if (index < count)
	{
		return (Process::GetSetting(index));
	}

	const StringTable *table = TheInterfaceMgr->GetStringTable();

	if (index == count)
	{
		const char *title = table->GetString(StringID('PROC', kProcessInput, 'PORT'));
		return (new TextSetting('PORT', inputPortName, title, 4));
	}

	return (nullptr);
}

void InputProcess::SetSetting(const Setting *setting)
{
	Type identifier = setting->GetSettingIdentifier();

	if (identifier == 'PORT')
	{
		inputPortName = static_cast<const TextSetting *>(setting)->GetText();
	}
	else
	{
		Process::SetSetting(setting);
	}
}

bool InputProcess::operator ==(const Process& process) const
{
	if (Process::operator ==(process))
	{
		const InputProcess& inputProcess = static_cast<const InputProcess&>(process);
		return (inputPortName == inputProcess.inputPortName);
	}

	return (false);
}


OutputProcess::OutputProcess() : Process(kProcessOutput)
{
	SetBaseProcessType(kProcessOutput);
}

OutputProcess::OutputProcess(ProcessType type) : Process(type)
{
	SetBaseProcessType(kProcessOutput);
}

OutputProcess::OutputProcess(const OutputProcess& outputProcess) : Process(outputProcess)
{
}

OutputProcess::~OutputProcess()
{
}

Process *OutputProcess::Replicate(void) const
{
	return (new OutputProcess(*this));
}


NullOutputProcess::NullOutputProcess() : OutputProcess(kProcessNullOutput)
{
}

NullOutputProcess::NullOutputProcess(const NullOutputProcess& nullOutputProcess) : OutputProcess(nullOutputProcess)
{
}

NullOutputProcess::~NullOutputProcess()
{
}

Process *NullOutputProcess::Replicate(void) const
{
	return (new NullOutputProcess(*this));
}

int32 NullOutputProcess::GenerateShaderCode(const ShaderCompileData *compileData, const char **shaderCode) const
{
	static const char code[] =
	{
		#if C4OPENGL

			RESULT_COLOR ".xyz = vec3(0.0, 0.0, 0.0);\n"

		#else

			RESULT_COLOR ".xyz = 0.0;\n"

		#endif
	};

	shaderCode[0] = code;
	return (1);
}


AddOutputProcess::AddOutputProcess() : OutputProcess(kProcessAddOutput)
{
}

AddOutputProcess::AddOutputProcess(const AddOutputProcess& addOutputProcess) : OutputProcess(addOutputProcess)
{
}

AddOutputProcess::~AddOutputProcess()
{
}

Process *AddOutputProcess::Replicate(void) const
{
	return (new AddOutputProcess(*this));
}

int32 AddOutputProcess::GetPortCount(void) const
{
	return (2);
}

const char *AddOutputProcess::GetPortName(int32 index) const
{
	static const char *const portName[2] =
	{
		"A", "B"
	};

	return (portName[index]);
}

void AddOutputProcess::GenerateProcessData(const ShaderCompileData *compileData, ProcessData *data) const
{
	if (GetFirstOutgoingEdge())
	{
		data->registerCount = 1;
	}

	int32 size1 = GetPortRoute(0)->GenerateOutputSize();
	int32 size2 = GetPortRoute(1)->GenerateOutputSize();
	int32 size = Max(size1, size2);

	data->outputSize = size;
	data->inputSize[0] = size;
	data->inputSize[1] = size;
}

int32 AddOutputProcess::GenerateOutputIdentifier(const ShaderCompileData *compileData, const ShaderAllocationData *allocData, SwizzleData *swizzleData, char *name) const
{
	if (GetFirstOutgoingEdge())
	{
		return (OutputProcess::GenerateOutputIdentifier(compileData, allocData, swizzleData, name));
	}

	return (Text::CopyText(RESULT_COLOR, name));
}

int32 AddOutputProcess::GenerateShaderCode(const ShaderCompileData *compileData, const char **shaderCode) const
{
	static const char code[] =
	{
		"# = %0 + %1;\n"
	};

	shaderCode[0] = code;
	return (1);
}


AlphaOutputProcess::AlphaOutputProcess() : OutputProcess(kProcessAlphaOutput)
{
}

AlphaOutputProcess::AlphaOutputProcess(const AlphaOutputProcess& alphaOutputProcess) : OutputProcess(alphaOutputProcess)
{
}

AlphaOutputProcess::~AlphaOutputProcess()
{
}

Process *AlphaOutputProcess::Replicate(void) const
{
	return (new AlphaOutputProcess(*this));
}

int32 AlphaOutputProcess::GetPortCount(void) const
{
	return (1);
}

unsigned_int32 AlphaOutputProcess::GetPortFlags(int32 index) const
{
	return (kProcessPortOptional);
}

const char *AlphaOutputProcess::GetPortName(int32 index) const
{
	return ("A");
}

void AlphaOutputProcess::GenerateProcessData(const ShaderCompileData *compileData, ProcessData *data) const
{
	data->inputSize[0] = 1;
}

int32 AlphaOutputProcess::GenerateShaderCode(const ShaderCompileData *compileData, const char **shaderCode) const
{
	static const char code[] =
	{
		RESULT_COLOR ".w = %0;\n"
	};

	shaderCode[0] = code;
	return (1);
}


AlphaTestOutputProcess::AlphaTestOutputProcess() : OutputProcess(kProcessAlphaTestOutput)
{
}

AlphaTestOutputProcess::AlphaTestOutputProcess(const AlphaTestOutputProcess& alphaTestOutputProcess) : OutputProcess(alphaTestOutputProcess)
{
}

AlphaTestOutputProcess::~AlphaTestOutputProcess()
{
}

Process *AlphaTestOutputProcess::Replicate(void) const
{
	return (new AlphaTestOutputProcess(*this));
}

int32 AlphaTestOutputProcess::GetPortCount(void) const
{
	return (1);
}

unsigned_int32 AlphaTestOutputProcess::GetPortFlags(int32 index) const
{
	return (kProcessPortOptional);
}

const char *AlphaTestOutputProcess::GetPortName(int32 index) const
{
	return ("A");
}

int32 AlphaTestOutputProcess::GenerateProcessSignature(const ShaderCompileData *compileData, unsigned_int32 *signature) const
{
	int32 count = OutputProcess::GenerateProcessSignature(compileData, signature);

	signature[count] = ((compileData->shaderData->materialState & kMaterialAlphaCoverage) != 0);
	return (count + 1);
}

void AlphaTestOutputProcess::GenerateProcessData(const ShaderCompileData *compileData, ProcessData *data) const
{
	data->inputSize[0] = 1;
}

int32 AlphaTestOutputProcess::GenerateShaderCode(const ShaderCompileData *compileData, const char **shaderCode) const
{
	static const char killCode[] =
	{
		"if (%0 < 0.5) discard;\n"
	};

	static const char alphaCode[] =
	{
		RESULT_COLOR ".w = %0;\n"
	};

	shaderCode[0] = killCode;

	if (compileData->shaderData->materialState & kMaterialAlphaCoverage)
	{
		shaderCode[1] = alphaCode;
		return (2);
	}

	return (1);
}


AmbientOutputProcess::AmbientOutputProcess() : OutputProcess(kProcessAmbientOutput)
{
}

AmbientOutputProcess::AmbientOutputProcess(const AmbientOutputProcess& ambientOutputProcess) : OutputProcess(ambientOutputProcess)
{
}

AmbientOutputProcess::~AmbientOutputProcess()
{
}

Process *AmbientOutputProcess::Replicate(void) const
{
	return (new AmbientOutputProcess(*this));
}

void AmbientOutputProcess::Initialize(void)
{
	radiosityColorTexture = Texture::Get("C4/radiosity");
}

void AmbientOutputProcess::Terminate(void)
{
	radiosityColorTexture->Release();
}

int32 AmbientOutputProcess::GetPortCount(void) const
{
	return (2);
}

unsigned_int32 AmbientOutputProcess::GetPortFlags(int32 index) const
{
	return ((index == 0) ? 0 : kProcessPortOptional);
}

const char *AmbientOutputProcess::GetPortName(int32 index) const
{
	static const char *const portName[2] =
	{
		"RGB", "N"
	};

	return (portName[index]);
}

ShaderType AmbientOutputProcess::GetAmbientShaderType(const ShaderCompileData *compileData)
{
	AmbientMode mode = TheGraphicsMgr->GetAmbientMode();
	if (mode != kAmbientBright)
	{
		if (mode != kAmbientDark)
		{
			ShaderType shaderType = compileData->shaderType;
			if ((shaderType == kShaderAmbientRadiosity) || (shaderType == kShaderUnifiedRadiosity))
			{
				if (compileData->shaderData->materialState & kMaterialRadiositySpaceInhibit)
				{
					shaderType = (shaderType == kShaderAmbientRadiosity) ? kShaderAmbient : kShaderUnified;
				}
			}
			else if (((shaderType == kShaderAmbient) || (shaderType == kShaderUnified)) && (!compileData->renderable->GetAmbientEnvironment()->ambientLightColor))
			{
				shaderType = kShaderNone;
			}

			return (shaderType);
		}

		return ((compileData->renderable->GetAmbientEnvironment()->ambientLightColor) ? kShaderAmbient : kShaderNone);
	}

	return (kShaderNone);
}

bool AmbientOutputProcess::AmbientOcclusionEnabled(const ShaderCompileData *compileData)
{
	if (compileData->renderable->GetRenderableFlags() & kRenderableStructureBufferInhibit)
	{
		return (false);
	}

	if (compileData->shaderData->materialState & kMaterialOcclusionInhibit)
	{
		return (false);
	}

	return ((TheGraphicsMgr->GetGraphicsActiveFlags() & kGraphicsActiveAmbientOcclusion) != 0);
}

bool AmbientOutputProcess::TwoSidedMaterial(const ShaderCompileData *compileData)
{
	const RenderSegment *segment = compileData->renderSegment;
	unsigned_int32 flags = segment->GetMaterialState();

	const MaterialObject *const *materialObjectPointer = segment->GetMaterialObjectPointer();
	if (materialObjectPointer)
	{
		const MaterialObject *materialObject = *materialObjectPointer;
		if (materialObject)
		{
			flags |= materialObject->GetMaterialFlags();
		}
	}

	return (flags & kMaterialTwoSided);
}

bool AmbientOutputProcess::AmbientBumpsEnabled(void) const
{
	if (!GetPortRoute(1))
	{
		return (false);
	}

	return ((TheGraphicsMgr->GetRenderOptionFlags() & kRenderOptionAmbientBumps) != 0);
}

int32 AmbientOutputProcess::GenerateProcessSignature(const ShaderCompileData *compileData, unsigned_int32 *signature) const
{
	int32 count = OutputProcess::GenerateProcessSignature(compileData, signature);

	int32 shaderType = GetAmbientShaderType(compileData);
	if ((shaderType >= kShaderFirstAmbient) && (shaderType <= kShaderLastUnified))
	{
		if (AmbientOcclusionEnabled(compileData))
		{
			shaderType |= 0x80000000;
		}

		if ((shaderType == kShaderAmbientRadiosity) || (shaderType == kShaderUnifiedRadiosity))
		{
			if (AmbientBumpsEnabled())
			{
				shaderType |= 0x40000000;
			}

			if (TwoSidedMaterial(compileData))
			{
				shaderType |= 0x20000000;
			}
		}
	}

	signature[count] = shaderType;
	return (count + 1);
}

void AmbientOutputProcess::GenerateProcessData(const ShaderCompileData *compileData, ProcessData *data) const
{
	const Renderable *renderable = compileData->renderable;
	const AmbientEnvironment *environment = renderable->GetAmbientEnvironment();

	switch (GetAmbientShaderType(compileData))
	{
		case kShaderAmbient:
		case kShaderUnified:
		{
			if (GetFirstOutgoingEdge())
			{
				data->registerCount = 1;
			}

			if (AmbientOcclusionEnabled(compileData))
			{
				data->textureCount = 1;
				data->textureObject[0] = TheGraphicsMgr->GetOcclusionTexture();
			}

			if (TheGraphicsMgr->GetAmbientMode() != kAmbientDark)
			{
				compileData->shaderData->AddStateProc(&StateProc_ConfigureAmbientLight, environment->ambientLightColor);
			}
			else
			{
				compileData->shaderData->AddStateProc(&StateProc_ConfigureDarkAmbientLight);
			}

			break;
		}

		case kShaderAmbientRadiosity:
		case kShaderUnifiedRadiosity:
		{
			if (GetFirstOutgoingEdge())
			{
				data->registerCount = 1;
			}

			data->temporaryCount = 2;
			data->textureCount = 3;

			data->interpolantCount = 2;
			data->interpolantType[0] = 'RPOS';
			data->interpolantType[1] = 'RNRM';

			if (AmbientBumpsEnabled())
			{
				data->interpolantCount = 3;
				data->interpolantType[2] = (renderable->GetShaderFlags() & kShaderGenerateTangent) ? 'RTNG' : 'RTGH';
			}

			const RadiositySpaceObject *object = environment->radiositySpaceObject;
			data->textureObject[0] = object->GetRadiosityTexture(0);
			data->textureObject[1] = object->GetRadiosityTexture(1);
			data->textureObject[2] = radiosityColorTexture;

			if (AmbientOcclusionEnabled(compileData))
			{
				data->textureCount = 4;
				data->textureObject[3] = TheGraphicsMgr->GetOcclusionTexture();
			}

			if (renderable->GetTransformable())
			{
				compileData->shaderData->AddStateProc(&StateProc_ConfigureTransformRadiositySpace, environment);
			}
			else
			{
				compileData->shaderData->AddStateProc(&StateProc_ConfigureRadiositySpace, environment);
			}

			break;
		}

		default:
		{
			if (GetFirstOutgoingEdge())
			{
				data->passthruPort = 0;
			}

			break;
		}
	}

	data->outputSize = 3;
	data->inputSize[0] = 3;
	data->inputSize[1] = 3;
}

int32 AmbientOutputProcess::GenerateOutputIdentifier(const ShaderCompileData *compileData, const ShaderAllocationData *allocData, SwizzleData *swizzleData, char *name) const
{
	if (GetFirstOutgoingEdge())
	{
		if (GetAmbientShaderType(compileData) != kShaderNone)
		{
			return (OutputProcess::GenerateOutputIdentifier(compileData, allocData, swizzleData, name));
		}

		return (GetPortRoute(0)->GenerateOutputIdentifier(compileData, allocData, swizzleData, name));
	}

	return (Text::CopyText(RESULT_COLOR, name));
}

int32 AmbientOutputProcess::GenerateShaderCode(const ShaderCompileData *compileData, const char **shaderCode) const
{
	switch (GetAmbientShaderType(compileData))
	{
		case kShaderAmbient:
		case kShaderUnified:
		{
			static const char constantCode[] =
			{
				"# = %0 * " FPARAM(FRAGMENT_PARAM_AMBIENT_COLOR) ".xyz;\n"
			};

			static const char occlusionCode[] =
			{
				"# = %0 * " FPARAM(FRAGMENT_PARAM_AMBIENT_COLOR) ".xyz * %TRG0(%IMG0, " FRAGMENT_POSITION ".xy" FRAGMENT_XY_MULTIPLIER ").x;\n"
			};

			shaderCode[0] = (AmbientOcclusionEnabled(compileData)) ? occlusionCode : constantCode;
			return (1);
		}

		case kShaderAmbientRadiosity:
		case kShaderUnifiedRadiosity:
		{
			static const char flatCode[] =
			{
				"tmp1 = %TRG0(%IMG0, $RPOS);\n"
				"tmp2 = %TRG1(%IMG1, $RPOS);\n"
				FLOAT3 " rdcl = %TRG2(%IMG2, " FLOAT2 "(tmp1.w, tmp2.w)).xyz;\n"
				FLOAT3 " rnrm = normalize($RNRM);\n"
			};

			static const char bumpCode[] =
			{
				"tmp1 = %TRG0(%IMG0, $RPOS);\n"
				"tmp2 = %TRG1(%IMG1, $RPOS);\n"
				FLOAT3 " rdcl = %TRG2(%IMG2, " FLOAT2 "(tmp1.w, tmp2.w)).xyz;\n"
				FLOAT3 " rnrm = normalize($RNRM);\n"
				FLOAT3 " rtng = normalize($RTNG.xyz);\n"
				FLOAT3 " rbtg = cross(rnrm, rtng.xyz);\n"
			};

			static const char bumpHandCode[] =
			{
				"tmp1 = %TRG0(%IMG0, $RPOS);\n"
				"tmp2 = %TRG1(%IMG1, $RPOS);\n"
				FLOAT3 " rdcl = %TRG2(%IMG2, " FLOAT2 "(tmp1.w, tmp2.w)).xyz;\n"
				FLOAT3 " rnrm = normalize($RNRM);\n"
				FLOAT3 " rtng = normalize($RTGH.xyz);\n"
				FLOAT3 " rbtg = cross(rnrm, rtng.xyz) * $RTGH.w;\n"
			};

			static const char frontCode[] =
			{
				#if C4OPENGL

					"if (!gl_FrontFacing) rnrm.z = -rnrm.z;\n"

				#elif C4PSSL //[ PS4

					// -- PS4 code hidden --

				#elif C4CG //[ PS3

					// -- PS3 code hidden --

				#endif //]
			};

			static const char transformCode[] =
			{
				"rnrm = rtng.xyz * %1.x + rbtg * %1.y + rnrm * %1.z;\n"
			};

			static const char blendCode[] =
			{
				#if C4OPENGL

					"tmp1.w = dot(tmp1.xyz, clamp(abs(rnrm) * rnrm, 0.0, 1.0));\n"
					"tmp2.w = dot(tmp2.xyz, clamp(abs(rnrm) * -rnrm, 0.0, 1.0));\n"

				#else

					"tmp1.w = dot(tmp1.xyz, saturate(abs(rnrm) * rnrm));\n"
					"tmp2.w = dot(tmp2.xyz, saturate(abs(rnrm) * -rnrm));\n"

				#endif
			};

			static const char constantCode[] =
			{
				"# = %0 * (rdcl * (tmp1.w + tmp2.w) + " FPARAM(FRAGMENT_PARAM_AMBIENT_COLOR) ".xyz);\n"
			};

			static const char occlusionCode[] =
			{
				"# = %0 * (rdcl * (tmp1.w + tmp2.w) + " FPARAM(FRAGMENT_PARAM_AMBIENT_COLOR) ".xyz) * %TRG3(%IMG3, " FRAGMENT_POSITION ".xy" FRAGMENT_XY_MULTIPLIER ").x;\n"
			};

			bool bumpsEnabled = AmbientBumpsEnabled();
			if (bumpsEnabled)
			{
				shaderCode[0] = (compileData->renderable->GetShaderFlags() & kShaderGenerateTangent) ? bumpCode : bumpHandCode;
			}
			else
			{
				shaderCode[0] = flatCode;
			}

			int32 count = 1;

			if (TwoSidedMaterial(compileData))
			{
				shaderCode[count++] = frontCode;
			}

			if (bumpsEnabled)
			{
				shaderCode[count++] = transformCode;
			}

			shaderCode[count] = blendCode;
			shaderCode[count + 1] = (AmbientOcclusionEnabled(compileData)) ? occlusionCode : constantCode;
			return (count + 2);
		}

		default:
		{
			if (!GetFirstOutgoingEdge())
			{
				static const char code[] =
				{
					"# = %0;\n"
				};

				shaderCode[0] = code;
				return (1);
			}

			break;
		}
	}

	return (0);
}

void AmbientOutputProcess::StateProc_ConfigureAmbientLight(const Renderable *renderable, const void *cookie)
{
	Render::SetFragmentShaderParameter(kFragmentParamAmbientColor, *static_cast<const ColorRGBA *>(cookie));
}

void AmbientOutputProcess::StateProc_ConfigureDarkAmbientLight(const Renderable *renderable, const void *cookie)
{
	Render::SetFragmentShaderParameter(kFragmentParamAmbientColor, 0.0F, 0.0F, 0.0F, 0.0F);
}

void AmbientOutputProcess::StateProc_ConfigureRadiositySpace(const Renderable *renderable, const void *cookie)
{
	const AmbientEnvironment *environment = static_cast<const AmbientEnvironment *>(cookie);
	const RadiositySpaceObject *radiositySpace = environment->radiositySpaceObject;

	const Vector3D& size = radiositySpace->GetBoxSize();
	const Vector3D& scale = radiositySpace->GetTexcoordScale();
	const Vector3D& offset = radiositySpace->GetTexcoordOffset();
	const Transform4D& m = environment->radiositySpaceTransformable->GetInverseWorldTransform();

	Render::SetVertexShaderParameter(kVertexParamMatrixSpace, m(0,0), m(0,1), m(0,2), m(0,3) + size.x * offset.x);
	Render::SetVertexShaderParameter(kVertexParamMatrixSpace + 1, m(1,0), m(1,1), m(1,2), m(1,3) + size.y * offset.y);
	Render::SetVertexShaderParameter(kVertexParamMatrixSpace + 2, m(2,0), m(2,1), m(2,2), m(2,3) + size.z * offset.z);
	Render::SetVertexShaderParameter(kVertexParamSpaceScale, scale.x / size.x, scale.y / size.y, scale.z / size.z, 0.0F);

	Render::SetFragmentShaderParameter(kFragmentParamAmbientColor, *environment->ambientLightColor);
}

void AmbientOutputProcess::StateProc_ConfigureTransformRadiositySpace(const Renderable *renderable, const void *cookie)
{
	const AmbientEnvironment *environment = static_cast<const AmbientEnvironment *>(cookie);
	const RadiositySpaceObject *radiositySpace = environment->radiositySpaceObject;

	const Vector3D& size = radiositySpace->GetBoxSize();
	const Vector3D& scale = radiositySpace->GetTexcoordScale();
	const Vector3D& offset = radiositySpace->GetTexcoordOffset();
	Transform4D m = environment->radiositySpaceTransformable->GetInverseWorldTransform() * renderable->GetTransformable()->GetWorldTransform();

	Render::SetVertexShaderParameter(kVertexParamMatrixSpace, m(0,0), m(0,1), m(0,2), m(0,3) + size.x * offset.x);
	Render::SetVertexShaderParameter(kVertexParamMatrixSpace + 1, m(1,0), m(1,1), m(1,2), m(1,3) + size.y * offset.y);
	Render::SetVertexShaderParameter(kVertexParamMatrixSpace + 2, m(2,0), m(2,1), m(2,2), m(2,3) + size.z * offset.z);
	Render::SetVertexShaderParameter(kVertexParamSpaceScale, scale.x / size.x, scale.y / size.y, scale.z / size.z, 0.0F);

	Render::SetFragmentShaderParameter(kFragmentParamAmbientColor, *environment->ambientLightColor);
}


VertexOcclusionOutputProcess::VertexOcclusionOutputProcess() : OutputProcess(kProcessVertexOcclusionOutput)
{
}

VertexOcclusionOutputProcess::VertexOcclusionOutputProcess(const VertexOcclusionOutputProcess& vertexOcclusionOutputProcess) : OutputProcess(vertexOcclusionOutputProcess)
{
}

VertexOcclusionOutputProcess::~VertexOcclusionOutputProcess()
{
}

Process *VertexOcclusionOutputProcess::Replicate(void) const
{
	return (new VertexOcclusionOutputProcess(*this));
}

int32 VertexOcclusionOutputProcess::GetPortCount(void) const
{
	return (1);
}

const char *VertexOcclusionOutputProcess::GetPortName(int32 index) const
{
	return ("A");
}

void VertexOcclusionOutputProcess::GenerateProcessData(const ShaderCompileData *compileData, ProcessData *data) const
{
	compileData->shaderSourceFlags |= kShaderSourcePrimaryColor;

	if (GetFirstOutgoingEdge())
	{
		data->registerCount = 1;
	}

	int32 size = GetPortRoute(0)->GenerateOutputSize();
	data->outputSize = size;
	data->inputSize[0] = size;
}

int32 VertexOcclusionOutputProcess::GenerateOutputIdentifier(const ShaderCompileData *compileData, const ShaderAllocationData *allocData, SwizzleData *swizzleData, char *name) const
{
	if (GetFirstOutgoingEdge())
	{
		return (OutputProcess::GenerateOutputIdentifier(compileData, allocData, swizzleData, name));
	}

	return (Text::CopyText(RESULT_COLOR, name));
}

int32 VertexOcclusionOutputProcess::GenerateShaderCode(const ShaderCompileData *compileData, const char **shaderCode) const
{
	static const char code[] =
	{
		"# = %0 * " FRAGMENT_COLOR0 ".w;\n"
	};

	shaderCode[0] = code;
	return (1);
}


EmissionOutputProcess::EmissionOutputProcess() : OutputProcess(kProcessEmissionOutput)
{
}

EmissionOutputProcess::EmissionOutputProcess(const EmissionOutputProcess& emissionOutputProcess) : OutputProcess(emissionOutputProcess)
{
}

EmissionOutputProcess::~EmissionOutputProcess()
{
}

Process *EmissionOutputProcess::Replicate(void) const
{
	return (new EmissionOutputProcess(*this));
}

int32 EmissionOutputProcess::GetPortCount(void) const
{
	return (1);
}

unsigned_int32 EmissionOutputProcess::GetPortFlags(int32 index) const
{
	return (kProcessPortOptional);
}

const char *EmissionOutputProcess::GetPortName(int32 index) const
{
	return ("RGB");
}

void EmissionOutputProcess::GenerateProcessData(const ShaderCompileData *compileData, ProcessData *data) const
{
	data->passthruPort = 0;
	data->outputSize = 3;
	data->inputSize[0] = 3;
}

int32 EmissionOutputProcess::GenerateOutputIdentifier(const ShaderCompileData *compileData, const ShaderAllocationData *allocData, SwizzleData *swizzleData, char *name) const
{
	return (GetPortRoute(0)->GenerateOutputIdentifier(compileData, allocData, swizzleData, name));
}


ReflectionOutputProcess::ReflectionOutputProcess() : OutputProcess(kProcessReflectionOutput)
{
	reflectionParams.normalIncidenceReflectivity = 1.0F;
	reflectionParams.reflectionOffsetScale = 1.0F;

	reflectionData = &reflectionParams;
}

ReflectionOutputProcess::ReflectionOutputProcess(const ReflectionOutputProcess& reflectionOutputProcess) : OutputProcess(reflectionOutputProcess)
{
	reflectionParams.normalIncidenceReflectivity = reflectionOutputProcess.reflectionParams.normalIncidenceReflectivity;
	reflectionParams.reflectionOffsetScale = reflectionOutputProcess.reflectionParams.reflectionOffsetScale;

	reflectionData = &reflectionParams;
}

ReflectionOutputProcess::~ReflectionOutputProcess()
{
}

Process *ReflectionOutputProcess::Replicate(void) const
{
	return (new ReflectionOutputProcess(*this));
}

void ReflectionOutputProcess::Pack(Packer& data, unsigned_int32 packFlags) const
{
	OutputProcess::Pack(data, packFlags);

	data << reflectionParams.normalIncidenceReflectivity;
	data << reflectionParams.reflectionOffsetScale;
}

void ReflectionOutputProcess::Unpack(Unpacker& data, unsigned_int32 unpackFlags)
{
	OutputProcess::Unpack(data, unpackFlags);

	data >> reflectionParams.normalIncidenceReflectivity;
	data >> reflectionParams.reflectionOffsetScale;
}

int32 ReflectionOutputProcess::GetSettingCount(void) const
{
	return (OutputProcess::GetSettingCount() + 2);
}

Setting *ReflectionOutputProcess::GetSetting(int32 index) const
{
	int32 count = OutputProcess::GetSettingCount();
	if (index < count)
	{
		return (OutputProcess::GetSetting(index));
	}

	const StringTable *table = TheInterfaceMgr->GetStringTable();

	if (index == count)
	{
		const char *title = table->GetString(StringID('PROC', kProcessReflectionOutput, 'NINC'));
		return (new IntegerSetting('NINC', (int32) (reflectionParams.normalIncidenceReflectivity * 100.0F + 0.5F), title, 0, 100, 1));
	}

	if (index == count + 1)
	{
		const char *title = table->GetString(StringID('PROC', kProcessReflectionOutput, 'RFLO'));
		return (new TextSetting('RFLO', reflectionParams.reflectionOffsetScale, title));
	}

	return (nullptr);
}

void ReflectionOutputProcess::SetSetting(const Setting *setting)
{
	Type identifier = setting->GetSettingIdentifier();

	if (identifier == 'NINC')
	{
		reflectionParams.normalIncidenceReflectivity = (float) static_cast<const IntegerSetting *>(setting)->GetIntegerValue() * 0.01F;
	}
	else if (identifier == 'RFLO')
	{
		reflectionParams.reflectionOffsetScale = Text::StringToFloat(static_cast<const TextSetting *>(setting)->GetText());
	}
	else
	{
		OutputProcess::SetSetting(setting);
	}
}

bool ReflectionOutputProcess::operator ==(const Process& process) const
{
	if (OutputProcess::operator ==(process))
	{
		const ReflectionOutputProcess& reflectionOutputProcess = static_cast<const ReflectionOutputProcess&>(process);

		if (reflectionParams.normalIncidenceReflectivity != reflectionOutputProcess.reflectionParams.normalIncidenceReflectivity)
		{
			return (false);
		}

		return (reflectionParams.reflectionOffsetScale == reflectionOutputProcess.reflectionParams.reflectionOffsetScale);
	}

	return (false);
}

int32 ReflectionOutputProcess::GetPortCount(void) const
{
	return (2);
}

unsigned_int32 ReflectionOutputProcess::GetPortFlags(int32 index) const
{
	return (kProcessPortOptional);
}

const char *ReflectionOutputProcess::GetPortName(int32 index) const
{
	static const char *const portName[2] =
	{
		"RGB", "N"
	};

	return (portName[index]);
}

void ReflectionOutputProcess::ReferenceStateParams(const Process *process)
{
	reflectionData = static_cast<const ReflectionOutputProcess *>(process)->reflectionData;
}

int32 ReflectionOutputProcess::GenerateProcessSignature(const ShaderCompileData *compileData, unsigned_int32 *signature) const
{
	int32 count = OutputProcess::GenerateProcessSignature(compileData, signature);
	signature[count] = *reinterpret_cast<const unsigned_int32 *>(&reflectionData->normalIncidenceReflectivity);
	return (count + 1);
}

int32 ReflectionOutputProcess::GenerateDerivedInterpolantTypes(const ShaderCompileData *compileData, ProcessType *type) const
{
	type[0] = kProcessTangentViewDirection;
	return (1);
}

void ReflectionOutputProcess::GenerateProcessData(const ShaderCompileData *compileData, ProcessData *data) const
{
	data->registerCount = 1;
	data->temporaryCount = 1;

	data->outputSize = 3;
	data->inputSize[0] = 3;
	data->inputSize[1] = 3;

	if (GetPortRoute(1))
	{
		data->interpolantCount = 2;
		data->interpolantType[0] = 'RGHT';
		data->interpolantType[1] = 'DOWN';
	}
	else
	{
		data->interpolantCount = 1;
		data->interpolantType[0] = 'WARP';
	}

	data->textureCount = 1;
	data->textureObject[0] = TheGraphicsMgr->GetReflectionTexture();

	float value = reflectionData->normalIncidenceReflectivity;

	data->literalCount = 2;
	data->literalData[0].literalType = 'NIR1';
	data->literalData[0].literalValue = value;
	data->literalData[1].literalType = 'NIR2';
	data->literalData[1].literalValue = 1.0F - value;

	compileData->shaderData->AddStateProc(&StateProc_CalculateReflectionScale, reflectionData);
}

int32 ReflectionOutputProcess::GenerateShaderCode(const ShaderCompileData *compileData, const char **shaderCode) const
{
	static const char flatCode[] =
	{
		"temp.z = 1.0 - vdir.z;\n"
		"temp.w = temp.z * temp.z;\n"
		"temp.w = temp.w * temp.w * (temp.z * &NIR2) + &NIR1;\n"
		"temp.xyz = %0 * temp.w;\n"

		"tmp1.xy = max(min($WARP.xy * $WARP.z, 8.0), -8.0) + " FRAGMENT_POSITION ".xy;\n"
		"# = %TRG0(%IMG0, tmp1.xy" FRAGMENT_XY_MULTIPLIER ").xyz * temp.xyz;\n"
	};

	static const char bumpCode[] =
	{
		"temp = " FLOAT4 "(%1, -1);\n"
		"tmp1.xy = " FLOAT2 "(dot($RGHT.xyzz, temp), dot($DOWN.xyzz, temp));\n"

		"temp.z = 1.0 - dot(%1, vdir);\n"
		"temp.w = temp.z * temp.z;\n"
		"temp.w = temp.w * temp.w * (temp.z * &NIR2) + &NIR1;\n"
		"temp.xyz = %0 * temp.w;\n"

		"tmp1.xy = tmp1.xy * $RGHT.w + " FRAGMENT_POSITION ".xy;\n"
		"# = %TRG0(%IMG0, tmp1.xy" FRAGMENT_XY_MULTIPLIER ").xyz * temp.xyz;\n"
	};

	if (GetPortRoute(1))
	{
		shaderCode[0] = bumpCode;
	}
	else
	{
		shaderCode[0] = flatCode;
	}

	return (1);
}

void ReflectionOutputProcess::StateProc_CalculateReflectionScale(const Renderable *renderable, const void *cookie)
{
	const ReflectionAttribute::ReflectionParams *params = static_cast<const ReflectionAttribute::ReflectionParams *>(cookie);

	float x = params->reflectionOffsetScale * TheGraphicsMgr->GetRenderTargetOffsetSize();
	Render::SetVertexShaderParameter(kVertexParamReflectionScale, x, 0.0F, 0.0F, 0.0F);
}


RefractionOutputProcess::RefractionOutputProcess() : OutputProcess(kProcessRefractionOutput)
{
	refractionParams.refractionOffsetScale = 1.0F;

	refractionData = &refractionParams;
}

RefractionOutputProcess::RefractionOutputProcess(const RefractionOutputProcess& refractionOutputProcess) : OutputProcess(refractionOutputProcess)
{
	refractionParams.refractionOffsetScale = refractionOutputProcess.refractionParams.refractionOffsetScale;

	refractionData = &refractionParams;
}

RefractionOutputProcess::~RefractionOutputProcess()
{
}

Process *RefractionOutputProcess::Replicate(void) const
{
	return (new RefractionOutputProcess(*this));
}

void RefractionOutputProcess::Pack(Packer& data, unsigned_int32 packFlags) const
{
	OutputProcess::Pack(data, packFlags);

	data << refractionParams.refractionOffsetScale;
}

void RefractionOutputProcess::Unpack(Unpacker& data, unsigned_int32 unpackFlags)
{
	OutputProcess::Unpack(data, unpackFlags);

	data >> refractionParams.refractionOffsetScale;
}

int32 RefractionOutputProcess::GetSettingCount(void) const
{
	return (OutputProcess::GetSettingCount() + 1);
}

Setting *RefractionOutputProcess::GetSetting(int32 index) const
{
	int32 count = OutputProcess::GetSettingCount();
	if (index < count)
	{
		return (OutputProcess::GetSetting(index));
	}

	if (index == count)
	{
		const StringTable *table = TheInterfaceMgr->GetStringTable();
		const char *title = table->GetString(StringID('PROC', kProcessRefractionOutput, 'RFRO'));
		return (new TextSetting('RFRO', refractionParams.refractionOffsetScale, title));
	}

	return (nullptr);
}

void RefractionOutputProcess::SetSetting(const Setting *setting)
{
	Type identifier = setting->GetSettingIdentifier();

	if (identifier == 'RFRO')
	{
		refractionParams.refractionOffsetScale = Text::StringToFloat(static_cast<const TextSetting *>(setting)->GetText());
	}
	else
	{
		OutputProcess::SetSetting(setting);
	}
}

bool RefractionOutputProcess::operator ==(const Process& process) const
{
	if (OutputProcess::operator ==(process))
	{
		const RefractionOutputProcess& refractionOutputProcess = static_cast<const RefractionOutputProcess&>(process);
		return (refractionParams.refractionOffsetScale == refractionOutputProcess.refractionParams.refractionOffsetScale);
	}

	return (false);
}

int32 RefractionOutputProcess::GetPortCount(void) const
{
	return (2);
}

unsigned_int32 RefractionOutputProcess::GetPortFlags(int32 index) const
{
	return (kProcessPortOptional);
}

const char *RefractionOutputProcess::GetPortName(int32 index) const
{
	static const char *const portName[2] =
	{
		"RGB", "N"
	};

	return (portName[index]);
}

void RefractionOutputProcess::ReferenceStateParams(const Process *process)
{
	refractionData = static_cast<const RefractionOutputProcess *>(process)->refractionData;
}

void RefractionOutputProcess::GenerateProcessData(const ShaderCompileData *compileData, ProcessData *data) const
{
	data->registerCount = 1;
	data->temporaryCount = 1;

	data->outputSize = 3;
	data->inputSize[0] = 3;
	data->inputSize[1] = 3;

	if (GetPortRoute(1))
	{
		data->interpolantCount = 2;
		data->interpolantType[0] = 'RGHT';
		data->interpolantType[1] = 'DOWN';
	}
	else
	{
		data->interpolantCount = 1;
		data->interpolantType[0] = 'WARP';
	}

	data->textureCount = 1;
	data->textureObject[0] = TheGraphicsMgr->GetRefractionTexture();

	compileData->shaderData->AddStateProc(&StateProc_CalculateRefractionParams, refractionData);
}

int32 RefractionOutputProcess::GenerateShaderCode(const ShaderCompileData *compileData, const char **shaderCode) const
{
	static const char flatCode[] =
	{
		"temp.xy = max(min($WARP.xy * $WARP.w, 8.0), -8.0) + " FRAGMENT_POSITION ".xy;\n"
		"# = %TRG0(%IMG0, temp.xy" FRAGMENT_XY_MULTIPLIER ").xyz * %0;\n"
	};

	static const char bumpCode[] =
	{
		"temp = " FLOAT4 "(%1, -1);\n"
		"tmp1.xy = " FLOAT2 "(dot($RGHT.xyzz, temp), dot($DOWN.xyzz, temp));\n"

		"temp.xy = tmp1.xy * $DOWN.w + " FRAGMENT_POSITION ".xy;\n"
		"# = %TRG0(%IMG0, temp.xy" FRAGMENT_XY_MULTIPLIER ").xyz * %0;\n"
	};

	if (GetPortRoute(1))
	{
		shaderCode[0] = bumpCode;
	}
	else
	{
		shaderCode[0] = flatCode;
	}

	return (1);
}

void RefractionOutputProcess::StateProc_CalculateRefractionParams(const Renderable *renderable, const void *cookie)
{
	const RefractionAttribute::RefractionParams *params = static_cast<const RefractionAttribute::RefractionParams *>(cookie);

	float x = params->refractionOffsetScale * TheGraphicsMgr->GetRenderTargetOffsetSize();
	Render::SetVertexShaderParameter(kVertexParamRefractionScale, x, 0.0F, 0.0F, 0.0F);
}


EnvironmentOutputProcess::EnvironmentOutputProcess() : OutputProcess(kProcessEnvironmentOutput)
{
	textureName[0] = 0;
	textureObject = nullptr;
}

EnvironmentOutputProcess::EnvironmentOutputProcess(const EnvironmentOutputProcess& environmentOutputProcess) : OutputProcess(environmentOutputProcess)
{
	textureName = environmentOutputProcess.textureName;

	Texture *texture = environmentOutputProcess.textureObject;
	textureObject = texture;
	if (texture)
	{
		texture->Retain();
	}
}

EnvironmentOutputProcess::~EnvironmentOutputProcess()
{
	if (textureObject)
	{
		textureObject->Release();
	}
}

Process *EnvironmentOutputProcess::Replicate(void) const
{
	return (new EnvironmentOutputProcess(*this));
}

void EnvironmentOutputProcess::Pack(Packer& data, unsigned_int32 packFlags) const
{
	OutputProcess::Pack(data, packFlags);

	data << textureName;
}

void EnvironmentOutputProcess::Unpack(Unpacker& data, unsigned_int32 unpackFlags)
{
	OutputProcess::Unpack(data, unpackFlags);

	data >> textureName;
	SetTexture(textureName);
}

void *EnvironmentOutputProcess::BeginSettingsUnpack(void)
{
	if (textureObject)
	{
		textureObject->Release();
		textureObject = nullptr;
	}

	return (OutputProcess::BeginSettingsUnpack());
}

int32 EnvironmentOutputProcess::GetSettingCount(void) const
{
	return (OutputProcess::GetSettingCount() + 1);
}

Setting *EnvironmentOutputProcess::GetSetting(int32 index) const
{
	int32 count = OutputProcess::GetSettingCount();
	if (index < count)
	{
		return (OutputProcess::GetSetting(index));
	}

	if (index == count)
	{
		const StringTable *table = TheInterfaceMgr->GetStringTable();
		const char *title = table->GetString(StringID('PROC', kProcessEnvironmentOutput, 'TNAM'));
		const char *picker = table->GetString(StringID('PROC', kProcessEnvironmentOutput, 'PICK'));
		return (new ResourceSetting('TNAM', textureName, title, picker, TextureResource::GetDescriptor()));
	}

	return (nullptr);
}

void EnvironmentOutputProcess::SetSetting(const Setting *setting)
{
	Type identifier = setting->GetSettingIdentifier();

	if (identifier == 'TNAM')
	{
		SetTexture(static_cast<const ResourceSetting *>(setting)->GetResourceName());
	}
	else
	{
		OutputProcess::SetSetting(setting);
	}
}

bool EnvironmentOutputProcess::operator ==(const Process& process) const
{
	if (OutputProcess::operator ==(process))
	{
		const EnvironmentOutputProcess& environmentOutputProcess = static_cast<const EnvironmentOutputProcess&>(process);
		return (textureName == environmentOutputProcess.textureName);
	}

	return (false);
}

void EnvironmentOutputProcess::SetTexture(const char *name)
{
	Texture *object = textureObject;

	if ((name) && (name[0] != 0))
	{
		if (name != &textureName[0])
		{
			textureName = name;
		}

		textureObject = Texture::Get(name);
	}
	else
	{
		textureName[0] = 0;
		textureObject = nullptr;
	}

	if (object)
	{
		object->Release();
	}
}

int32 EnvironmentOutputProcess::GetPortCount(void) const
{
	return (3);
}

unsigned_int32 EnvironmentOutputProcess::GetPortFlags(int32 index) const
{
	return (kProcessPortOptional);
}

const char *EnvironmentOutputProcess::GetPortName(int32 index) const
{
	static const char *const portName[3] =
	{
		"RGB", "N", "BIAS"
	};

	return (portName[index]);
}

int32 EnvironmentOutputProcess::GenerateProcessSignature(const ShaderCompileData *compileData, unsigned_int32 *signature) const
{
	int32 count = OutputProcess::GenerateProcessSignature(compileData, signature);
	signature[count] = (GetPortRoute(1)) ? 0 : ((compileData->renderable->TangentAvailable()) ? 1 : 2);
	signature[count + 1] = (textureObject) ? textureObject->GetTextureType() : kTextureCube;
	return (count + 2);
}

int32 EnvironmentOutputProcess::GenerateDerivedInterpolantTypes(const ShaderCompileData *compileData, ProcessType *type) const
{
	if ((GetPortRoute(1)) || (compileData->renderable->TangentAvailable()))
	{
		compileData->shaderSourceFlags |= kShaderSourceTangentViewDirection;
	}

	return (0);
}

void EnvironmentOutputProcess::GenerateProcessData(const ShaderCompileData *compileData, ProcessData *data) const
{
	data->registerCount = 1;

	data->outputSize = 3;
	data->inputSize[0] = 3;
	data->inputSize[1] = 3;
	data->inputSize[2] = 1;

	if ((GetPortRoute(1)) || (compileData->renderable->TangentAvailable()))
	{
		data->interpolantCount = 4;
		data->interpolantType[0] = 'VDIR';
		data->interpolantType[1] = 'WTAN';
		data->interpolantType[2] = 'WBTN';
		data->interpolantType[3] = 'WNRM';
	}
	else
	{
		data->interpolantCount = 2;
		data->interpolantType[0] = 'NRML';
		data->interpolantType[1] = 'OVDR';
	}

	data->textureCount = 1;
	if (textureObject)
	{
		data->textureObject[0] = textureObject;
	}
	else
	{
		data->textureObject[0] = *compileData->renderable->GetAmbientEnvironment()->environmentMap;
	}
}

int32 EnvironmentOutputProcess::GenerateShaderCode(const ShaderCompileData *compileData, const char **shaderCode) const
{
	static const char tangentCode[] =
	{
		"temp.xyz = $WTAN * -$VDIR.x + $WBTN * -$VDIR.y + $WNRM * $VDIR.z;\n"
	};

	static const char normalCode[] =
	{
		"temp.xyz = %1 * (dot(%1, $VDIR) * 2.0) - $VDIR;\n"
		"temp.xyz = $WTAN * temp.x + $WBTN * temp.y + $WNRM * temp.z;\n"
	};

	static const char objectCode[] =
	{
		"temp.xyz = $NRML * (dot($NRML, $OVDR) * 2.0) - $OVDR;\n"
	};

	static const char textureCode[] =
	{
		"# = %TRG0(%IMG0, temp.xyz).xyz * %0;\n"
	};

	static const char biasCode[] =
	{
		#if C4OPENGL

			"# = %TRG0(%IMG0, temp.xyz, %2).xyz * %0;\n"

		#elif C4PSSL //[ PS4

			// -- PS4 code hidden --

		#elif C4CG //[ PS3

			// -- PS3 code hidden --

		#endif //]
	};

	if (GetPortRoute(1))
	{
		shaderCode[0] = normalCode;
	}
	else if (compileData->renderable->TangentAvailable())
	{
		shaderCode[0] = tangentCode;
	}
	else
	{
		shaderCode[0] = objectCode;
	}

	shaderCode[1] = (!GetPortRoute(2)) ? textureCode : biasCode;
	return (2);
}


TerrainEnvironmentOutputProcess::TerrainEnvironmentOutputProcess() : OutputProcess(kProcessTerrainEnvironmentOutput)
{
	textureName[0] = 0;
	textureObject = nullptr;
}

TerrainEnvironmentOutputProcess::TerrainEnvironmentOutputProcess(const TerrainEnvironmentOutputProcess& terrainEnvironmentOutputProcess) : OutputProcess(terrainEnvironmentOutputProcess)
{
	textureName = terrainEnvironmentOutputProcess.textureName;

	Texture *texture = terrainEnvironmentOutputProcess.textureObject;
	textureObject = texture;
	if (texture)
	{
		texture->Retain();
	}
}

TerrainEnvironmentOutputProcess::~TerrainEnvironmentOutputProcess()
{
	if (textureObject)
	{
		textureObject->Release();
	}
}

Process *TerrainEnvironmentOutputProcess::Replicate(void) const
{
	return (new TerrainEnvironmentOutputProcess(*this));
}

void TerrainEnvironmentOutputProcess::Pack(Packer& data, unsigned_int32 packFlags) const
{
	OutputProcess::Pack(data, packFlags);

	data << textureName;
}

void TerrainEnvironmentOutputProcess::Unpack(Unpacker& data, unsigned_int32 unpackFlags)
{
	OutputProcess::Unpack(data, unpackFlags);

	data >> textureName;
	SetTextureName(textureName);
}

void *TerrainEnvironmentOutputProcess::BeginSettingsUnpack(void)
{
	if (textureObject)
	{
		textureObject->Release();
		textureObject = nullptr;
	}

	return (OutputProcess::BeginSettingsUnpack());
}

int32 TerrainEnvironmentOutputProcess::GetSettingCount(void) const
{
	return (OutputProcess::GetSettingCount() + 1);
}

Setting *TerrainEnvironmentOutputProcess::GetSetting(int32 index) const
{
	int32 count = OutputProcess::GetSettingCount();
	if (index < count)
	{
		return (OutputProcess::GetSetting(index));
	}

	if (index == count)
	{
		const StringTable *table = TheInterfaceMgr->GetStringTable();
		const char *title = table->GetString(StringID('PROC', kProcessTerrainEnvironmentOutput, 'TNAM'));
		const char *picker = table->GetString(StringID('PROC', kProcessTerrainEnvironmentOutput, 'PICK'));
		return (new ResourceSetting('TNAM', textureName, title, picker, TextureResource::GetDescriptor()));
	}

	return (nullptr);
}

void TerrainEnvironmentOutputProcess::SetSetting(const Setting *setting)
{
	Type identifier = setting->GetSettingIdentifier();

	if (identifier == 'TNAM')
	{
		SetTextureName(static_cast<const ResourceSetting *>(setting)->GetResourceName());
	}
	else
	{
		OutputProcess::SetSetting(setting);
	}
}

bool TerrainEnvironmentOutputProcess::operator ==(const Process& process) const
{
	if (OutputProcess::operator ==(process))
	{
		const TerrainEnvironmentOutputProcess& terrainEnvironmentOutputProcess = static_cast<const TerrainEnvironmentOutputProcess&>(process);
		return (textureName == terrainEnvironmentOutputProcess.textureName);
	}

	return (false);
}

void TerrainEnvironmentOutputProcess::SetTextureName(const char *name)
{
	Texture *object = textureObject;

	if ((name) && (name[0] != 0))
	{
		if (name != &textureName[0])
		{
			textureName = name;
		}

		textureObject = Texture::Get(name);
	}
	else
	{
		textureName[0] = 0;
		textureObject = nullptr;
	}

	if (object)
	{
		object->Release();
	}
}

int32 TerrainEnvironmentOutputProcess::GetPortCount(void) const
{
	return (4);
}

unsigned_int32 TerrainEnvironmentOutputProcess::GetPortFlags(int32 index) const
{
	return (kProcessPortOptional);
}

const char *TerrainEnvironmentOutputProcess::GetPortName(int32 index) const
{
	static const char *const portName[4] =
	{
		"RGB", "N1", "N2", "N3"
	};

	return (portName[index]);
}

bool TerrainEnvironmentOutputProcess::BumpEnabled(void) const
{
	if (!(TheGraphicsMgr->GetRenderOptionFlags() & kRenderOptionTerrainBumps))
	{
		return (false);
	}

	return ((GetPortRoute(1)) && (GetPortRoute(2)) && (GetPortRoute(3)));
}

int32 TerrainEnvironmentOutputProcess::GenerateProcessSignature(const ShaderCompileData *compileData, unsigned_int32 *signature) const
{
	int32 count = OutputProcess::GenerateProcessSignature(compileData, signature);
	signature[count] = BumpEnabled();
	signature[count + 1] = (textureObject) ? textureObject->GetTextureType() : kTextureCube;
	return (count + 2);
}

int32 TerrainEnvironmentOutputProcess::GenerateDerivedInterpolantTypes(const ShaderCompileData *compileData, ProcessType *type) const
{
	type[0] = kProcessTerrainViewDirection;
	type[1] = kProcessTriplanarBlend;
	return (2);
}

void TerrainEnvironmentOutputProcess::GenerateProcessData(const ShaderCompileData *compileData, ProcessData *data) const
{
	data->registerCount = 1;
	data->temporaryCount = 2;

	data->outputSize = 3;
	data->inputSize[0] = 3;
	data->inputSize[1] = 3;
	data->inputSize[2] = 3;
	data->inputSize[3] = 3;

	data->interpolantCount = 3;
	data->interpolantType[0] = 'TWNM';
	data->interpolantType[1] = 'TWT1';
	data->interpolantType[2] = 'TWT2';

	data->textureCount = 1;
	if (textureObject)
	{
		data->textureObject[0] = textureObject;
	}
	else
	{
		data->textureObject[0] = *compileData->renderable->GetAmbientEnvironment()->environmentMap;
	}
}

int32 TerrainEnvironmentOutputProcess::GenerateShaderCode(const ShaderCompileData *compileData, const char **shaderCode) const
{
	static const char flatCode[] =
	{
		FLOAT3 " twb1 = cross($TWNM, $TWT1);\n"
		FLOAT3 " twb2 = cross($TWNM, $TWT2);\n"

		"tmp1.xyz = tvdr * (tbld.x + tbld.y);\n"

		"tmp1.xyz = $TWNM * tmp1.z - $TWT1 * tmp1.x - twb1 * tmp1.y;\n"
		"tmp2.xyz = $TWNM * tvd2.z - $TWT2 * tvd2.x - twb2 * tvd2.y;\n"

		"tmp1.xyz += tmp2.xyz * tbld.z;\n"
		"# = %TRG0(%IMG0, tmp1.xyz).xyz * %0;\n"
	};

	static const char bumpCode[] =
	{
		FLOAT3 " twb1 = cross($TWNM, $TWT1);\n"
		FLOAT3 " twb2 = cross($TWNM, $TWT2);\n"

		"temp.xyz = %1 * (dot(%1, tvdr) * 2.0) - tvdr;\n"
		"tmp1.xyz = %2 * (dot(%2, tvdr) * 2.0) - tvdr;\n"
		"tmp2.xyz = %3 * (dot(%3, tvd2) * 2.0) - tvd2;\n"

		"tmp1.xyz = temp.xyz * tbld.x + tmp1.xyz * tbld.y;\n"

		"tmp1.xyz = $TWT1 * tmp1.x + twb1 * tmp1.y + $TWNM * tmp1.z;\n"
		"tmp2.xyz = $TWT2 * tmp2.x + twb2 * tmp2.y + $TWNM * tmp2.z;\n"

		"tmp1.xyz += tmp2.xyz * tbld.z;\n"
		"# = %TRG0(%IMG0, tmp1.xyz).xyz * %0;\n"
	};

	if (BumpEnabled())
	{
		shaderCode[0] = bumpCode;
	}
	else
	{
		shaderCode[0] = flatCode;
	}

	return (1);
}


ImpostorDepthOutputProcess::ImpostorDepthOutputProcess() : OutputProcess(kProcessImpostorDepthOutput)
{
}

ImpostorDepthOutputProcess::ImpostorDepthOutputProcess(const ImpostorDepthOutputProcess& impostorDepthOutputProcess) : OutputProcess(impostorDepthOutputProcess)
{
}

ImpostorDepthOutputProcess::~ImpostorDepthOutputProcess()
{
}

Process *ImpostorDepthOutputProcess::Replicate(void) const
{
	return (new ImpostorDepthOutputProcess(*this));
}

int32 ImpostorDepthOutputProcess::GetPortCount(void) const
{
	return (1);
}

unsigned_int32 ImpostorDepthOutputProcess::GetPortFlags(int32 index) const
{
	return (kProcessPortOptional);
}

const char *ImpostorDepthOutputProcess::GetPortName(int32 index) const
{
	return ("SHAD");
}

void ImpostorDepthOutputProcess::GenerateProcessData(const ShaderCompileData *compileData, ProcessData *data) const
{
	data->inputSize[0] = 4;

	data->interpolantCount = 1;
	data->interpolantType[0] = 'ISRD';
}

int32 ImpostorDepthOutputProcess::GenerateShaderCode(const ShaderCompileData *compileData, const char **shaderCode) const
{
	static const char code[] =
	{
		"if (dot(%0, " UPARAM(UNIVERSAL_PARAM_IMPOSTOR_SHADOW_BLEND) ") > 0.5) discard;\n"

		RESULT_DEPTH " = dot(%0 * $ISRD.x + $ISRD.y, " UPARAM(UNIVERSAL_PARAM_IMPOSTOR_SHADOW_SCALE) ") + " FRAGMENT_POSITION ".z;\n"
	};

	shaderCode[0] = code;
	return (1);
}


LightOutputProcess::LightOutputProcess() : OutputProcess(kProcessLightOutput)
{
}

LightOutputProcess::LightOutputProcess(const LightOutputProcess& lightOutputProcess) : OutputProcess(lightOutputProcess)
{
}

LightOutputProcess::~LightOutputProcess()
{
}

Process *LightOutputProcess::Replicate(void) const
{
	return (new LightOutputProcess(*this));
}

int32 LightOutputProcess::GetPortCount(void) const
{
	return (2);
}

int32 LightOutputProcess::GetInternalPortCount(void) const
{
	return (1);
}

unsigned_int32 LightOutputProcess::GetPortFlags(int32 index) const
{
	return (kProcessPortOptional);
}

const char *LightOutputProcess::GetPortName(int32 index) const
{
	static const char *const portName[3] =
	{
		"RGB", "Z", "LGHT"
	};

	return (portName[index]);
}

ShaderType LightOutputProcess::GetLightShaderType(const ShaderCompileData *compileData)
{
	ShaderType shaderType = compileData->shaderType;
	if ((shaderType == kShaderCubeLight) && (compileData->renderable->GetShaderFlags() & kShaderCubeLightInhibit))
	{
		return (kShaderPointLight);
	}

	return (shaderType);
}

int32 LightOutputProcess::GenerateProcessSignature(const ShaderCompileData *compileData, unsigned_int32 *signature) const
{
	int32 count = OutputProcess::GenerateProcessSignature(compileData, signature);
	signature[count] = GetLightShaderType(compileData);
	return (count + 1);
}

void LightOutputProcess::GenerateProcessData(const ShaderCompileData *compileData, ProcessData *data) const
{
	if (GetFirstOutgoingEdge())
	{
		data->registerCount = 1;
	}

	data->outputSize = 3;
	data->inputSize[0] = 3;
	data->inputSize[1] = 1;
	data->inputSize[2] = 3;
}

int32 LightOutputProcess::GenerateOutputIdentifier(const ShaderCompileData *compileData, const ShaderAllocationData *allocData, SwizzleData *swizzleData, char *name) const
{
	const Route *route = GetFirstOutgoingEdge();
	if ((route) && (route->GetRoutePort() != kProcessPortHiddenDependency))
	{
		return (OutputProcess::GenerateOutputIdentifier(compileData, allocData, swizzleData, name));
	}

	return (Text::CopyText(RESULT_COLOR, name));
}

int32 LightOutputProcess::GenerateShaderCode(const ShaderCompileData *compileData, const char **shaderCode) const
{
	if (GetPortRoute(0))
	{
		if (GetLightShaderType(compileData) <= kShaderInfiniteLight)
		{
			static const char lightCode[] =
			{
				"# = %0 * " UPARAM(UNIVERSAL_PARAM_LIGHT_COLOR) ".xyz;\n"
			};

			static const char shadowCode[] =
			{
				"# = %0 * " UPARAM(UNIVERSAL_PARAM_LIGHT_COLOR) ".xyz * %2;\n"
			};

			shaderCode[0] = (compileData->shadowFlag) ? shadowCode : lightCode;
			return (1);
		}

		static const char code[] =
		{
			"# = %0 * " UPARAM(UNIVERSAL_PARAM_LIGHT_COLOR) ".xyz * %2;\n"
		};

		shaderCode[0] = code;
		return (1);
	}

	static const char code[] =
	{
		"# = " FLOAT3 "(0.0, 0.0, 0.0);\n"
	};

	shaderCode[0] = code;
	return (1);
}


GlowOutputProcess::GlowOutputProcess() : OutputProcess(kProcessGlowOutput)
{
}

GlowOutputProcess::GlowOutputProcess(const GlowOutputProcess& glowOutputProcess) : OutputProcess(glowOutputProcess)
{
}

GlowOutputProcess::~GlowOutputProcess()
{
}

Process *GlowOutputProcess::Replicate(void) const
{
	return (new GlowOutputProcess(*this));
}

int32 GlowOutputProcess::GetPortCount(void) const
{
	return (1);
}

unsigned_int32 GlowOutputProcess::GetPortFlags(int32 index) const
{
	return (kProcessPortOptional);
}

const char *GlowOutputProcess::GetPortName(int32 index) const
{
	return ("A");
}

void GlowOutputProcess::GenerateProcessData(const ShaderCompileData *compileData, ProcessData *data) const
{
	data->inputSize[0] = 1;
}

int32 GlowOutputProcess::GenerateShaderCode(const ShaderCompileData *compileData, const char **shaderCode) const
{
	static const char code[] =
	{
		RESULT_COLOR ".w = %0;\n"
	};

	shaderCode[0] = code;
	return (1);
}


BloomOutputProcess::BloomOutputProcess() : OutputProcess(kProcessBloomOutput)
{
}

BloomOutputProcess::BloomOutputProcess(const BloomOutputProcess& bloomOutputProcess) : OutputProcess(bloomOutputProcess)
{
}

BloomOutputProcess::~BloomOutputProcess()
{
}

Process *BloomOutputProcess::Replicate(void) const
{
	return (new BloomOutputProcess(*this));
}

int32 BloomOutputProcess::GetPortCount(void) const
{
	return (1);
}

int32 BloomOutputProcess::GetInternalPortCount(void) const
{
	return (1);
}

unsigned_int32 BloomOutputProcess::GetPortFlags(int32 index) const
{
	return (kProcessPortOptional);
}

const char *BloomOutputProcess::GetPortName(int32 index) const
{
	static const char *const portName[2] =
	{
		"A", "LGHT"
	};

	return (portName[index]);
}

void BloomOutputProcess::GenerateProcessData(const ShaderCompileData *compileData, ProcessData *data) const
{
	data->inputSize[0] = 1;
	data->inputSize[1] = 1;
}

int32 BloomOutputProcess::GenerateShaderCode(const ShaderCompileData *compileData, const char **shaderCode) const
{
	static const char regularCode[] =
	{
		RESULT_COLOR ".w = %0;\n"
	};

	static const char lightCode[] =
	{
		RESULT_COLOR ".w = %0 * %1;\n"
	};

	shaderCode[0] = (GetPortRoute(1)) ? lightCode : regularCode;
	return (1);
}


GlowBloomOutputProcess::GlowBloomOutputProcess() : OutputProcess(kProcessGlowBloomOutput)
{
}

GlowBloomOutputProcess::GlowBloomOutputProcess(const GlowBloomOutputProcess& glowBloomOutputProcess) : OutputProcess(glowBloomOutputProcess)
{
}

GlowBloomOutputProcess::~GlowBloomOutputProcess()
{
}

Process *GlowBloomOutputProcess::Replicate(void) const
{
	return (new GlowBloomOutputProcess(*this));
}

int32 GlowBloomOutputProcess::GetPortCount(void) const
{
	return (3);
}

const char *GlowBloomOutputProcess::GetPortName(int32 index) const
{
	static const char *const portName[3] =
	{
		"GLOW", "BLOM", "LGHT"
	};

	return (portName[index]);
}

void GlowBloomOutputProcess::GenerateProcessData(const ShaderCompileData *compileData, ProcessData *data) const
{
	data->inputSize[0] = 1;
	data->inputSize[1] = 1;
	data->inputSize[2] = 1;
}

int32 GlowBloomOutputProcess::GenerateShaderCode(const ShaderCompileData *compileData, const char **shaderCode) const
{
	static const char regularCode[] =
	{
		RESULT_COLOR ".w = %0 + %1;\n"
	};

	static const char lightCode[] =
	{
		RESULT_COLOR ".w = %0 + %1 * %2;\n"
	};

	shaderCode[0] = (GetPortRoute(2)) ? lightCode : regularCode;
	return (1);
}


StructureOutputProcess::StructureOutputProcess() : OutputProcess(kProcessStructureOutput)
{
}

StructureOutputProcess::StructureOutputProcess(const StructureOutputProcess& structureOutputProcess) : OutputProcess(structureOutputProcess)
{
}

StructureOutputProcess::~StructureOutputProcess()
{
}

Process *StructureOutputProcess::Replicate(void) const
{
	return (new StructureOutputProcess(*this));
}

unsigned_int32 StructureOutputProcess::GetStructureRenderFlags(unsigned_int32 renderableFlags)
{
	unsigned_int32 flags = kStructureRenderVelocity | kStructureRenderDepth;

	if (renderableFlags & kRenderableStructureVelocityZero)
	{
		flags &= ~kStructureRenderVelocity;
	}

	if (renderableFlags & kRenderableStructureDepthZero)
	{
		flags &= ~kStructureRenderDepth;
	}

	return (flags & TheGraphicsMgr->GetStructureFlags());
}

int32 StructureOutputProcess::GenerateProcessSignature(const ShaderCompileData *compileData, unsigned_int32 *signature) const
{
	int32 count = OutputProcess::GenerateProcessSignature(compileData, signature);
	signature[count] = GetStructureRenderFlags(compileData->renderable->GetRenderableFlags());
	return (count + 1);
}

void StructureOutputProcess::GenerateProcessData(const ShaderCompileData *compileData, ProcessData *data) const
{
	unsigned_int32 flags = GetStructureRenderFlags(compileData->renderable->GetRenderableFlags());
	if (flags & kStructureRenderVelocity)
	{
		data->temporaryCount = 2;

		data->interpolantCount = 2;
		data->interpolantType[0] = 'VELA';
		data->interpolantType[1] = 'VELB';
	}
}

int32 StructureOutputProcess::GenerateShaderCode(const ShaderCompileData *compileData, const char **shaderCode) const
{
	static const char velocityCode[] =
	{
		"tmp1.xy = $VELA.xy / $VELA.w;\n"
		"tmp2.xy = $VELB.xy / $VELB.w;\n"
		"temp.xy = (tmp2.xy - tmp1.xy) * " UPARAM(UNIVERSAL_PARAM_VELOCITY_SCALE) ".xy;\n"
		RESULT_VELOCITY " = temp.xy / max(max(abs(temp.x), abs(temp.y)), 1.0);\n"

		RESULT_COLOR ".xy = " FLOAT2 "(" DDX "($VELB.w), " DDY "($VELB.w));\n"

		"float z = $VELB.w * 4.0;\n"
		"float f = floor(z);\n"
		RESULT_COLOR ".zw = " FLOAT2 "(f * 0.25, z - f);\n"
	};

	static const char depthCode[] =
	{
		#if C4OPENGL

			"float w = 1.0 / " FRAGMENT_POSITION ".w;\n"

		#elif C4PSSL //[ PS4

			// -- PS4 code hidden --

		#elif C4CG //[ PS3

			// -- PS3 code hidden --

		#endif //]

		RESULT_COLOR ".xy = " FLOAT2 "(" DDX "(w), " DDY "(w));\n"

		"float z = w * 4.0;\n"
		"float f = floor(z);\n"
		RESULT_COLOR ".zw = " FLOAT2 "(f * 0.25, z - f);\n"
	};

	static const char zeroCode[] =
	{
		RESULT_COLOR " = " FLOAT4 "(0.0, 0.0, 0.0, 0.0);\n"
	};

	unsigned_int32 flags = GetStructureRenderFlags(compileData->renderable->GetRenderableFlags());
	if (flags & kStructureRenderVelocity)
	{
		shaderCode[0] = velocityCode;
	}
	else if (flags & kStructureRenderDepth)
	{
		shaderCode[0] = depthCode;
	}
	else
	{
		shaderCode[0] = zeroCode;
	}

	return (1);
}


InfiniteShadowProcess::InfiniteShadowProcess() : Process(kProcessInfiniteShadow)
{
}

InfiniteShadowProcess::InfiniteShadowProcess(const InfiniteShadowProcess& infiniteShadowProcess) : Process(infiniteShadowProcess)
{
}

InfiniteShadowProcess::~InfiniteShadowProcess()
{
}

Process *InfiniteShadowProcess::Replicate(void) const
{
	return (new InfiniteShadowProcess(*this));
}

int32 InfiniteShadowProcess::GetPortCount(void) const
{
	return (1);
}

unsigned_int32 InfiniteShadowProcess::GetPortFlags(int32 index) const
{
	return (kProcessPortOptional);
}

const char *InfiniteShadowProcess::GetPortName(int32 index) const
{
	return ("Z");
}

void InfiniteShadowProcess::GenerateProcessData(const ShaderCompileData *compileData, ProcessData *data) const
{
	data->registerCount = 1;
	data->outputSize = 1;
	data->inputSize[0] = 1;

	data->interpolantType[0] = 'SHAD';
	data->interpolantType[1] = 'CSCD';

	if (!GetPortRoute(0))
	{
		data->interpolantCount = 2;
	}
	else
	{
		data->interpolantCount = 3;
		data->interpolantType[2] = 'IRAD';
	}
}

int32 InfiniteShadowProcess::GenerateShaderCode(const ShaderCompileData *compileData, const char **shaderCode) const
{
	static const char regularCode[] =
	{
		FLOAT3 " crd1 = $SHAD * " UPARAM(UNIVERSAL_PARAM_SHADOW_MAP_SCALE1) ".xyz + " UPARAM(UNIVERSAL_PARAM_SHADOW_MAP_OFFSET1) ".xyz;\n"
		FLOAT3 " crd2 = $SHAD * " UPARAM(UNIVERSAL_PARAM_SHADOW_MAP_SCALE2) ".xyz + " UPARAM(UNIVERSAL_PARAM_SHADOW_MAP_OFFSET2) ".xyz;\n"
		FLOAT3 " crd3 = $SHAD * " UPARAM(UNIVERSAL_PARAM_SHADOW_MAP_SCALE3) ".xyz + " UPARAM(UNIVERSAL_PARAM_SHADOW_MAP_OFFSET3) ".xyz;\n"
	};

	static const char impostorCode[] =
	{
		"temp.w = %0 * $IRAD.x + $IRAD.y;\n"
		"temp.xyz = " UPARAM(UNIVERSAL_PARAM_SHADOW_VIEW_DIRECTION) ".xyz * temp.w + $SHAD;\n"

		FLOAT3 " crd1 = temp.xyz * " UPARAM(UNIVERSAL_PARAM_SHADOW_MAP_SCALE1) ".xyz + " UPARAM(UNIVERSAL_PARAM_SHADOW_MAP_OFFSET1) ".xyz;\n"
		FLOAT3 " crd2 = temp.xyz * " UPARAM(UNIVERSAL_PARAM_SHADOW_MAP_SCALE2) ".xyz + " UPARAM(UNIVERSAL_PARAM_SHADOW_MAP_OFFSET2) ".xyz;\n"
		FLOAT3 " crd3 = temp.xyz * " UPARAM(UNIVERSAL_PARAM_SHADOW_MAP_SCALE3) ".xyz + " UPARAM(UNIVERSAL_PARAM_SHADOW_MAP_OFFSET3) ".xyz;\n"
	};

	static const char shadowCode[] =
	{
		"bool cas2 = ($CSCD.y >= 0.0);\n"
		"bool cas3 = ($CSCD.z >= 0.0);\n"
		FLOAT2 " cen1 = (cas2) ? crd2.xy : $SHAD.xy;\n"
		FLOAT2 " cen2 = (cas3) ? crd3.xy : crd1.xy;\n"

		FLOAT4 " lnd1, lnd2, shd1, shd2;\n"

		"lnd1.w = (cas2) ? crd2.z : $SHAD.z;\n"
		"lnd2.w = (cas3) ? crd3.z : crd1.z;\n"
		"lnd1.z = float(cas2) * 2.0;\n"
		"lnd2.z = float(cas3) * 2.0 + 1.0;\n"

		#if C4OPENGL

			"vec3 wght = clamp($CSCD, 0.0, 1.0);\n"
			"wght.x = (cas2) ? wght.y - wght.z : 1.0 - wght.x;\n"

			"lnd1.xy = cen1 + " UPARAM(UNIVERSAL_PARAM_SHADOW_SAMPLE1) ".xy;\n"
			"shd1.x = texture(shadowTexture, lnd1);\n"
			"lnd1.xy = cen1 + " UPARAM(UNIVERSAL_PARAM_SHADOW_SAMPLE1) ".zw;\n"
			"shd1.y = texture(shadowTexture, lnd1);\n"
			"lnd1.xy = cen1 + " UPARAM(UNIVERSAL_PARAM_SHADOW_SAMPLE2) ".xy;\n"
			"shd1.z = texture(shadowTexture, lnd1);\n"
			"lnd1.xy = cen1 + " UPARAM(UNIVERSAL_PARAM_SHADOW_SAMPLE2) ".zw;\n"
			"shd1.w = texture(shadowTexture, lnd1);\n"
			"shd1.x = ((shd1.x + shd1.y) + (shd1.z + shd1.w)) * 0.25;\n"

			"lnd2.xy = cen2 + " UPARAM(UNIVERSAL_PARAM_SHADOW_SAMPLE1) ".xy;\n"
			"shd2.x = texture(shadowTexture, lnd2);\n"
			"lnd2.xy = cen2 + " UPARAM(UNIVERSAL_PARAM_SHADOW_SAMPLE1) ".zw;\n"
			"shd2.y = texture(shadowTexture, lnd2);\n"
			"lnd2.xy = cen2 + " UPARAM(UNIVERSAL_PARAM_SHADOW_SAMPLE2) ".xy;\n"
			"shd2.z = texture(shadowTexture, lnd2);\n"
			"lnd2.xy = cen2 + " UPARAM(UNIVERSAL_PARAM_SHADOW_SAMPLE2) ".zw;\n"
			"shd2.w = texture(shadowTexture, lnd2);\n"
			"shd2.x = ((shd2.x + shd2.y) + (shd2.z + shd2.w)) * 0.25;\n"

		#elif C4PSSL //[ PS4

			// -- PS4 code hidden --

		#elif C4CG //[ PS3

			// -- PS3 code hidden --

		#endif //]

		"# = " LERP "(shd2.x, shd1.x, wght.x);\n"
	};

	shaderCode[0] = (!GetPortRoute(0)) ? regularCode : impostorCode;
	shaderCode[1] = shadowCode;
	return (2);
}


PointShadowProcess::PointShadowProcess() : Process(kProcessPointShadow)
{
}

PointShadowProcess::PointShadowProcess(const PointShadowProcess& pointShadowProcess) : Process(pointShadowProcess)
{
}

PointShadowProcess::~PointShadowProcess()
{
}

Process *PointShadowProcess::Replicate(void) const
{
	return (new PointShadowProcess(*this));
}

void PointShadowProcess::GenerateProcessData(const ShaderCompileData *compileData, ProcessData *data) const
{
	data->registerCount = 1;
	data->outputSize = 1;
	data->inputSize[0] = 1;

	data->interpolantCount = 1;
	data->interpolantType[0] = 'PROJ';
}

int32 PointShadowProcess::GenerateShaderCode(const ShaderCompileData *compileData, const char **shaderCode) const
{
	static const char shadowCode[] =
	{
		FLOAT3 " pmag = abs($PROJ);\n"
		"float mxy = max(pmag.x, pmag.y);\n"
		"float mxyz = max(mxy, pmag.z);\n"

		"float offs = " UPARAM(UNIVERSAL_PARAM_SHADOW_SAMPLE1) ".x * mxyz;\n"
		"float dpth = " UPARAM(UNIVERSAL_PARAM_SHADOW_SAMPLE1) ".y + " UPARAM(UNIVERSAL_PARAM_SHADOW_SAMPLE1) ".z / mxyz;\n"

		"float mz = float(pmag.z > mxy);\n"
		"float imz = 1.0 - mz;\n"
		"float mx = float(pmag.x > pmag.y) * imz;\n"

		FLOAT2 " offx = " FLOAT2 "(1.0 - mx, mx) * offs;\n"
		FLOAT2 " offy = " FLOAT2 "(mz, imz) * offs;\n"

		#if C4OPENGL

			"vec4 cord = vec4($PROJ, dpth);\n"
			"float shad = texture(shadowTexture, cord);\n"
			"cord.xy = $PROJ.xy - offx;\n"
			"shad += texture(shadowTexture, cord);\n"
			"cord.xy = $PROJ.xy + offx;\n"
			"shad += texture(shadowTexture, cord);\n"
			"cord.xyz = vec3($PROJ.x, $PROJ.yz - offy);\n"
			"shad += texture(shadowTexture, cord);\n"
			"cord.yz = $PROJ.yz + offy;\n"
			"shad += texture(shadowTexture, cord);\n"

		#elif C4PSSL //[ PS4

			// -- PS4 code hidden --

		#elif C4CG //[ PS3

			// -- PS3 code hidden --

		#endif //]

		"# = shad * 0.2;\n"
	};

	shaderCode[0] = shadowCode;
	return (1);
}


SpotShadowProcess::SpotShadowProcess() : Process(kProcessSpotShadow)
{
}

SpotShadowProcess::SpotShadowProcess(const SpotShadowProcess& spotShadowProcess) : Process(spotShadowProcess)
{
}

SpotShadowProcess::~SpotShadowProcess()
{
}

Process *SpotShadowProcess::Replicate(void) const
{
	return (new SpotShadowProcess(*this));
}

void SpotShadowProcess::GenerateProcessData(const ShaderCompileData *compileData, ProcessData *data) const
{
	data->registerCount = 1;
	data->outputSize = 1;
	data->inputSize[0] = 1;

	data->interpolantCount = 2;
	data->interpolantType[0] = 'PROJ';
	data->interpolantType[1] = 'SPOT';
}

int32 SpotShadowProcess::GenerateShaderCode(const ShaderCompileData *compileData, const char **shaderCode) const
{
	static const char shadowCode[] =
	{
		FLOAT2 " spot = $SPOT.xy / $PROJ.z;\n"
		FLOAT3 " cord = " FLOAT3 "(spot, $SPOT.z / $PROJ.z);\n"

		"float offs = " UPARAM(UNIVERSAL_PARAM_SHADOW_SAMPLE1) ".x;\n"

		#if C4OPENGL

			"float shad = texture(shadowTexture, cord);\n"
			"cord.xy -= offs;\n"
			"shad += texture(shadowTexture, cord);\n"
			"cord.x += offs;\n"
			"shad += texture(shadowTexture, cord);\n"
			"cord.x += offs;\n"
			"shad += texture(shadowTexture, cord);\n"
			"cord.y += offs;\n"
			"shad += texture(shadowTexture, cord);\n"
			"cord.y += offs;\n"
			"shad += texture(shadowTexture, cord);\n"
			"cord.x -= offs;\n"
			"shad += texture(shadowTexture, cord);\n"
			"cord.x -= offs;\n"
			"shad += texture(shadowTexture, cord);\n"
			"cord.y -= offs;\n"
			"shad += texture(shadowTexture, cord);\n"

		#elif C4PSSL //[ PS4

			// -- PS4 code hidden --

		#elif C4CG //[ PS3

			// -- PS3 code hidden --

		#endif //]

		"# = shad * 0.111111111;\n"
	};

	shaderCode[0] = shadowCode;
	return (1);
}


PointIlluminationProcess::PointIlluminationProcess() : Process(kProcessPointIllumination)
{
}

PointIlluminationProcess::PointIlluminationProcess(const PointIlluminationProcess& pointIlluminationProcess) : Process(pointIlluminationProcess)
{
}

PointIlluminationProcess::~PointIlluminationProcess()
{
}

Process *PointIlluminationProcess::Replicate(void) const
{
	return (new PointIlluminationProcess(*this));
}

void PointIlluminationProcess::GenerateProcessData(const ShaderCompileData *compileData, ProcessData *data) const
{
	data->registerCount = 1;
	data->outputSize = 1;

	data->interpolantCount = 1;
	data->interpolantType[0] = 'ATTN';
}

int32 PointIlluminationProcess::GenerateShaderCode(const ShaderCompileData *compileData, const char **shaderCode) const
{
	static const char code[] =
	{
		#if C4OPENGL

			"# = clamp(exp2(dot($ATTN, $ATTN) * -5.77078) * 1.01865736 - 0.01865736, 0.0, 1.0);\n"

		#else

			"# = saturate(exp2(dot($ATTN, $ATTN) * -5.77078) * 1.01865736 - 0.01865736);\n"

		#endif
	};

	shaderCode[0] = code;
	return (1);
}


CubeIlluminationProcess::CubeIlluminationProcess() : Process(kProcessCubeIllumination)
{
}

CubeIlluminationProcess::CubeIlluminationProcess(const CubeIlluminationProcess& cubeIlluminationProcess) : Process(cubeIlluminationProcess)
{
}

CubeIlluminationProcess::~CubeIlluminationProcess()
{
}

Process *CubeIlluminationProcess::Replicate(void) const
{
	return (new CubeIlluminationProcess(*this));
}

void CubeIlluminationProcess::GenerateProcessData(const ShaderCompileData *compileData, ProcessData *data) const
{
	data->registerCount = 1;
	data->outputSize = 4;

	data->interpolantCount = 2;
	data->interpolantType[0] = 'PROJ';
	data->interpolantType[1] = 'ATTN';
}

int32 CubeIlluminationProcess::GenerateShaderCode(const ShaderCompileData *compileData, const char **shaderCode) const
{
	static const char code[] =
	{
		#if C4OPENGL

			"temp.x = clamp(exp2(dot($ATTN, $ATTN) * -5.77078) * 1.01865736 - 0.01865736, 0.0, 1.0);\n"
			"# = texture(projectorCUBE, $PROJ) * temp.x;\n"

		#elif C4PSSL //[ PS4

			// -- PS4 code hidden --

		#elif C4CG //[ PS3

			// -- PS3 code hidden --

		#endif //]
	};

	shaderCode[0] = code;
	return (1);
}


SpotIlluminationProcess::SpotIlluminationProcess() : Process(kProcessSpotIllumination)
{
}

SpotIlluminationProcess::SpotIlluminationProcess(const SpotIlluminationProcess& spotIlluminationProcess) : Process(spotIlluminationProcess)
{
}

SpotIlluminationProcess::~SpotIlluminationProcess()
{
}

Process *SpotIlluminationProcess::Replicate(void) const
{
	return (new SpotIlluminationProcess(*this));
}

void SpotIlluminationProcess::GenerateProcessData(const ShaderCompileData *compileData, ProcessData *data) const
{
	data->registerCount = 1;
	data->outputSize = 4;

	data->interpolantCount = 2;
	data->interpolantType[0] = 'PROJ';
	data->interpolantType[1] = 'ATTN';
}

int32 SpotIlluminationProcess::GenerateShaderCode(const ShaderCompileData *compileData, const char **shaderCode) const
{
	static const char code[] =
	{
		#if C4OPENGL

			"temp.x = clamp(exp2(dot($ATTN, $ATTN) * -5.77078) * 1.01865736 - 0.01865736, 0.0, 1.0) * float($ATTN.z >= 0.0);\n"
			"# = texture(projector2D, $PROJ.xy / $PROJ.z) * temp.x;\n"

		#elif C4PSSL //[ PS4

			// -- PS4 code hidden --

		#elif C4CG //[ PS3

			// -- PS3 code hidden --

		#endif //]
	};

	shaderCode[0] = code;
	return (1);
}


ConstantFogProcess::ConstantFogProcess() : Process(kProcessConstantFog)
{
}

ConstantFogProcess::ConstantFogProcess(const ConstantFogProcess& constantFogProcess) : Process(constantFogProcess)
{
}

ConstantFogProcess::~ConstantFogProcess()
{
}

Process *ConstantFogProcess::Replicate(void) const
{
	return (new ConstantFogProcess(*this));
}

void ConstantFogProcess::Preschedule(const ShaderCompileData *compileData)
{
	if ((!(compileData->renderable->GetShaderFlags() & kShaderVertexInfinite)) && (!(compileData->shaderSourceFlags & kShaderSourceTangentViewDirection)))
	{
		const Process *process = compileData->terrainViewDirection;
		if (process)
		{
			new Route(const_cast<Process *>(process), this, kProcessPortHiddenDependency);
		}
	}
}

int32 ConstantFogProcess::GenerateProcessSignature(const ShaderCompileData *compileData, unsigned_int32 *signature) const
{
	int32 count = Process::GenerateProcessSignature(compileData, signature);
	signature[count] = compileData->renderable->GetShaderFlags() & kShaderVertexInfinite;
	return (count + 1);
}

void ConstantFogProcess::GenerateProcessData(const ShaderCompileData *compileData, ProcessData *data) const
{
	data->registerCount = 1;
	data->outputSize = 1;

	data->interpolantType[0] = 'FDTV';

	if (!(compileData->renderable->GetShaderFlags() & kShaderVertexInfinite))
	{
		data->interpolantType[1] = 'FDTP';

		if ((compileData->shaderSourceFlags & kShaderSourceTangentViewDirection) || (compileData->terrainViewDirection))
		{
			data->interpolantCount = 2;
		}
		else
		{
			data->interpolantCount = 3;
			data->interpolantType[2] = kProcessObjectViewDirection;
		}
	}
	else
	{
		data->interpolantCount = 2;
		data->interpolantType[1] = kProcessObjectPosition;
	}

	if (compileData->renderable->GetTransformable())
	{
		compileData->shaderData->AddStateProc(&StateProc_TransformFogPlane);
	}
	else
	{
		compileData->shaderData->AddStateProc(&StateProc_CopyFogPlane);
	}
}

int32 ConstantFogProcess::GenerateShaderCode(const ShaderCompileData *compileData, const char **shaderCode) const
{
	if (!(compileData->renderable->GetShaderFlags() & kShaderVertexInfinite))
	{
		if (compileData->shaderSourceFlags & kShaderSourceTangentViewDirection)
		{
			static const char code[] =
			{
				#if C4OPENGL

					"temp.x = clamp(" UPARAM(UNIVERSAL_PARAM_FOG_PARAMS2) ".z - $FDTP / abs($FDTV), 0.0, 1.0);\n"
					"# = clamp(exp2(-temp.x * length($VDIR) * " UPARAM(UNIVERSAL_PARAM_FOG_PARAMS2) ".w), 0.0, 1.0);\n"

				#else

					"temp.x = saturate(" UPARAM(UNIVERSAL_PARAM_FOG_PARAMS2) ".z - $FDTP / abs($FDTV));\n"
					"# = saturate(exp2(-temp.x * length($VDIR) * " UPARAM(UNIVERSAL_PARAM_FOG_PARAMS2) ".w));\n"

				#endif
			};

			shaderCode[0] = code;
		}
		else if (compileData->terrainViewDirection)
		{
			static const char code[] =
			{
				#if C4OPENGL

					"temp.x = clamp(" UPARAM(UNIVERSAL_PARAM_FOG_PARAMS2) ".z - $FDTP / abs($FDTV), 0.0, 1.0);\n"
					"# = clamp(exp2(-temp.x * max(length($TVDR), length(TVD2)) * " UPARAM(UNIVERSAL_PARAM_FOG_PARAMS2) ".w), 0.0, 1.0);\n"

				#else

					"temp.x = saturate(" UPARAM(UNIVERSAL_PARAM_FOG_PARAMS2) ".z - $FDTP / abs($FDTV));\n"
					"# = saturate(exp2(-temp.x * max(length($TVDR), length(TVD2)) * " UPARAM(UNIVERSAL_PARAM_FOG_PARAMS2) ".w));\n"

				#endif
			};

			shaderCode[0] = code;
		}
		else
		{
			static const char code[] =
			{
				#if C4OPENGL

					"temp.x = clamp(" UPARAM(UNIVERSAL_PARAM_FOG_PARAMS2) ".z - $FDTP / abs($FDTV), 0.0, 1.0);\n"
					"# = clamp(exp2(-temp.x * length($OVDR) * " UPARAM(UNIVERSAL_PARAM_FOG_PARAMS2) ".w), 0.0, 1.0);\n"

				#else

					"temp.x = saturate(" UPARAM(UNIVERSAL_PARAM_FOG_PARAMS2) ".z - $FDTP / abs($FDTV));\n"
					"# = saturate(exp2(-temp.x * length($OVDR) * " UPARAM(UNIVERSAL_PARAM_FOG_PARAMS2) ".w));\n"

				#endif
			};

			shaderCode[0] = code;
		}
	}
	else
	{
		static const char code[] =
		{
			"temp.x = min(" UPARAM(UNIVERSAL_PARAM_FOG_PARAMS2) ".x / $FDTV, 0.0) * length($POSI);\n"
			"# = min(exp2(temp.x * " UPARAM(UNIVERSAL_PARAM_FOG_PARAMS2) ".w), float($FDTV > 0.0));\n"
		};

		shaderCode[0] = code;
	}

	return (1);
}

void ConstantFogProcess::StateProc_CopyFogPlane(const Renderable *renderable, const void *cookie)
{
	Render::SetVertexShaderParameter(kVertexParamFogPlane, TheGraphicsMgr->GetFogPlane());
}

void ConstantFogProcess::StateProc_TransformFogPlane(const Renderable *renderable, const void *cookie)
{
	Render::SetVertexShaderParameter(kVertexParamFogPlane, TheGraphicsMgr->GetFogPlane() * renderable->GetTransformable()->GetWorldTransform());
}


LinearFogProcess::LinearFogProcess() : Process(kProcessLinearFog)
{
}

LinearFogProcess::LinearFogProcess(const LinearFogProcess& linearFogProcess) : Process(linearFogProcess)
{
}

LinearFogProcess::~LinearFogProcess()
{
}

Process *LinearFogProcess::Replicate(void) const
{
	return (new LinearFogProcess(*this));
}

void LinearFogProcess::Preschedule(const ShaderCompileData *compileData)
{
	if ((!(compileData->renderable->GetShaderFlags() & kShaderVertexInfinite)) && (!(compileData->shaderSourceFlags & kShaderSourceTangentViewDirection)))
	{
		const Process *process = compileData->terrainViewDirection;
		if (process)
		{
			new Route(const_cast<Process *>(process), this, kProcessPortHiddenDependency);
		}
	}
}

void LinearFogProcess::GenerateProcessData(const ShaderCompileData *compileData, ProcessData *data) const
{
	data->registerCount = 1;
	data->outputSize = 1;

	data->interpolantType[0] = 'FOGK';
	data->interpolantType[1] = 'FDTV';

	if (!(compileData->renderable->GetShaderFlags() & kShaderVertexInfinite))
	{
		data->interpolantType[2] = 'FDTP';

		if ((compileData->shaderSourceFlags & kShaderSourceTangentViewDirection) || (compileData->terrainViewDirection))
		{
			data->interpolantCount = 3;
		}
		else
		{
			data->interpolantCount = 4;
			data->interpolantType[3] = kProcessObjectViewDirection;
		}
	}
	else
	{
		data->interpolantCount = 3;
		data->interpolantType[2] = kProcessObjectPosition;
	}

	if (compileData->renderable->GetTransformable())
	{
		compileData->shaderData->AddStateProc(&ConstantFogProcess::StateProc_TransformFogPlane);
	}
	else
	{
		compileData->shaderData->AddStateProc(&ConstantFogProcess::StateProc_CopyFogPlane);
	}
}

int32 LinearFogProcess::GenerateShaderCode(const ShaderCompileData *compileData, const char **shaderCode) const
{
	if (!(compileData->renderable->GetShaderFlags() & kShaderVertexInfinite))
	{
		if (compileData->shaderSourceFlags & kShaderSourceTangentViewDirection)
		{
			static const char code[] =
			{
				#if C4OPENGL

					"temp.w = min($FDTP, 0.0);\n"
					"temp.x = $FOGK - temp.w * temp.w / abs($FDTV);\n"
					"# = clamp(exp2(temp.x * length($VDIR) * " UPARAM(UNIVERSAL_PARAM_FOG_PARAMS2) ".w), 0.0, 1.0);\n"

				#else

					"temp.w = min($FDTP, 0.0);\n"
					"temp.x = $FOGK - temp.w * temp.w / abs($FDTV);\n"
					"# = saturate(exp2(temp.x * length($VDIR) * " UPARAM(UNIVERSAL_PARAM_FOG_PARAMS2) ".w));\n"

				#endif
			};

			shaderCode[0] = code;
		}
		else if (compileData->terrainViewDirection)
		{
			static const char code[] =
			{
				#if C4OPENGL

					"temp.w = min($FDTP, 0.0);\n"
					"temp.x = $FOGK - temp.w * temp.w / abs($FDTV);\n"
					"# = clamp(exp2(temp.x * max(length($TVDR), length(TVD2)) * " UPARAM(UNIVERSAL_PARAM_FOG_PARAMS2) ".w), 0.0, 1.0);\n"

				#else

					"temp.w = min($FDTP, 0.0);\n"
					"temp.x = $FOGK - temp.w * temp.w / abs($FDTV);\n"
					"# = saturate(exp2(temp.x * max(length($TVDR), length(TVD2)) * " UPARAM(UNIVERSAL_PARAM_FOG_PARAMS2) ".w));\n"

				#endif
			};

			shaderCode[0] = code;
		}
		else
		{
			static const char code[] =
			{
				#if C4OPENGL

					"temp.w = min($FDTP, 0.0);\n"
					"temp.x = $FOGK - temp.w * temp.w / abs($FDTV);\n"
					"# = clamp(exp2(temp.x * length($OVDR) * " UPARAM(UNIVERSAL_PARAM_FOG_PARAMS2) ".w), 0.0, 1.0);\n"

				#else

					"temp.w = min($FDTP, 0.0);\n"
					"temp.x = $FOGK - temp.w * temp.w / abs($FDTV);\n"
					"# = saturate(exp2(temp.x * length($OVDR) * " UPARAM(UNIVERSAL_PARAM_FOG_PARAMS2) ".w));\n"

				#endif
			};

			shaderCode[0] = code;
		}
	}
	else
	{
		static const char code[] =
		{
			"temp.x = min(" UPARAM(UNIVERSAL_PARAM_FOG_PARAMS2) ".y / $FDTV, 0.0) * length($POSI);\n"
			"# = clamp(exp2(temp.x * " UPARAM(UNIVERSAL_PARAM_FOG_PARAMS2) ".w), float($FOGK > 0.0), float($FDTV > 0.0));\n"
		};

		shaderCode[0] = code;
	}

	return (1);
}


AmbientFogProcess::AmbientFogProcess() : Process(kProcessAmbientFog)
{
}

AmbientFogProcess::AmbientFogProcess(const AmbientFogProcess& ambientFogProcess) : Process(ambientFogProcess)
{
}

AmbientFogProcess::~AmbientFogProcess()
{
}

Process *AmbientFogProcess::Replicate(void) const
{
	return (new AmbientFogProcess(*this));
}

void AmbientFogProcess::GenerateProcessData(const ShaderCompileData *compileData, ProcessData *data) const
{
	data->inputSize[0] = 3;
	data->inputSize[1] = 1;
}

int32 AmbientFogProcess::GenerateShaderCode(const ShaderCompileData *compileData, const char **shaderCode) const
{
	static const char code[] =
	{
		RESULT_COLOR ".xyz = " LERP "(" UPARAM(UNIVERSAL_PARAM_FOG_COLOR) ".xyz, %0, %1);\n"
	};

	shaderCode[0] = code;
	return (1);
}


LightFogProcess::LightFogProcess() : Process(kProcessLightFog)
{
}

LightFogProcess::LightFogProcess(const LightFogProcess& lightFogProcess) : Process(lightFogProcess)
{
}

LightFogProcess::~LightFogProcess()
{
}

Process *LightFogProcess::Replicate(void) const
{
	return (new LightFogProcess(*this));
}

void LightFogProcess::GenerateProcessData(const ShaderCompileData *compileData, ProcessData *data) const
{
	data->inputSize[0] = 3;
	data->inputSize[1] = 1;
}

int32 LightFogProcess::GenerateShaderCode(const ShaderCompileData *compileData, const char **shaderCode) const
{
	static const char code[] =
	{
		RESULT_COLOR ".xyz = %0 * %1;\n"
	};

	shaderCode[0] = code;
	return (1);
}


AlphaFogProcess::AlphaFogProcess() : Process(kProcessAlphaFog)
{
}

AlphaFogProcess::AlphaFogProcess(const AlphaFogProcess& alphaFogProcess) : Process(alphaFogProcess)
{
}

AlphaFogProcess::~AlphaFogProcess()
{
}

Process *AlphaFogProcess::Replicate(void) const
{
	return (new AlphaFogProcess(*this));
}

void AlphaFogProcess::GenerateProcessData(const ShaderCompileData *compileData, ProcessData *data) const
{
	data->inputSize[0] = 3;
	data->inputSize[1] = 1;
}

int32 AlphaFogProcess::GenerateShaderCode(const ShaderCompileData *compileData, const char **shaderCode) const
{
	static const char code[] =
	{
		RESULT_COLOR ".xyz = " LERP "(" UPARAM(UNIVERSAL_PARAM_FOG_COLOR) ".xyz, %0, %1);\n"
		RESULT_COLOR ".w = 1.0 - %1;\n"
	};

	shaderCode[0] = code;
	return (1);
}


ColorPostProcess::ColorPostProcess() : Process(kProcessColorPost)
{
}

ColorPostProcess::ColorPostProcess(const ColorPostProcess& colorPostProcess) : Process(colorPostProcess)
{
}

ColorPostProcess::~ColorPostProcess()
{
}

Process *ColorPostProcess::Replicate(void) const
{
	return (new ColorPostProcess(*this));
}

int32 ColorPostProcess::GetPortCount(void) const
{
	return (1);
}

const char *ColorPostProcess::GetPortName(int32 index) const
{
	return ("P");
}

void ColorPostProcess::GenerateProcessData(const ShaderCompileData *compileData, ProcessData *data) const
{
	data->registerCount = 1;
	data->outputSize = 4;
	data->inputSize[0] = 2;
}

int32 ColorPostProcess::GenerateShaderCode(const ShaderCompileData *compileData, const char **shaderCode) const
{
	static const char code[] =
	{
		"# = " COLOR_TARGET "(" COLOR_TEXTURE ", %0" FRAGMENT_XY_MULTIPLIER ");\n"
	};

	shaderCode[0] = code;
	return (1);
}


DistortPostProcess::DistortPostProcess() : Process(kProcessDistortPost)
{
}

DistortPostProcess::DistortPostProcess(const DistortPostProcess& distortPostProcess) : Process(distortPostProcess)
{
}

DistortPostProcess::~DistortPostProcess()
{
}

Process *DistortPostProcess::Replicate(void) const
{
	return (new DistortPostProcess(*this));
}

void DistortPostProcess::GenerateProcessData(const ShaderCompileData *compileData, ProcessData *data) const
{
	data->registerCount = 1;
	data->outputSize = 2;
}

int32 DistortPostProcess::GenerateShaderCode(const ShaderCompileData *compileData, const char **shaderCode) const
{
	static const char code[] =
	{
		"temp = " DISTORTION_TARGET "(" DISTORTION_TEXTURE ", " FRAGMENT_POSITION ".xy" FRAGMENT_XY_MULTIPLIER ");\n"
		"# = (temp.xy - temp.zw) * " UPARAM(UNIVERSAL_PARAM_FRAME_BUFFER_SCALE) ".zw + " FRAGMENT_POSITION ".xy;\n"
	};

	shaderCode[0] = code;
	return (1);
}


MotionBlurPostProcess::MotionBlurPostProcess(bool gradient) : Process(kProcessMotionBlurPost)
{
	gradientFlag = gradient;
}

MotionBlurPostProcess::MotionBlurPostProcess(const MotionBlurPostProcess& motionBlurPostProcess) : Process(motionBlurPostProcess)
{
	gradientFlag = motionBlurPostProcess.gradientFlag;
}

MotionBlurPostProcess::~MotionBlurPostProcess()
{
}

Process *MotionBlurPostProcess::Replicate(void) const
{
	return (new MotionBlurPostProcess(*this));
}

int32 MotionBlurPostProcess::GetPortCount(void) const
{
	return (1);
}

const char *MotionBlurPostProcess::GetPortName(int32 index) const
{
	return ("P");
}

int32 MotionBlurPostProcess::GenerateProcessSignature(const ShaderCompileData *compileData, unsigned_int32 *signature) const
{
	int32 count = Process::GenerateProcessSignature(compileData, signature);
	signature[count] = gradientFlag;
	return (count + 1);
}

void MotionBlurPostProcess::GenerateProcessData(const ShaderCompileData *compileData, ProcessData *data) const
{
	data->registerCount = 1;
	data->temporaryCount = 1;
	data->outputSize = 3;
	data->inputSize[0] = 2;
}

int32 MotionBlurPostProcess::GenerateShaderCode(const ShaderCompileData *compileData, const char **shaderCode) const
{
	static const char code[] =
	{
		"temp.xyz = " COLOR_TARGET "(" COLOR_TEXTURE ", %0" FRAGMENT_XY_MULTIPLIER ").xyz * 0.1111111;\n"
		FLOAT2 " velo = " VELOCITY_TARGET "(" VELOCITY_TEXTURE ", %0" FRAGMENT_XY_MULTIPLIER ").xy;\n"

		"tmp1 = velo.xyxy * " FLOAT4 "(1.75, 1.75, -1.75, -1.75) + %0.xyxy;\n"
		"temp.xyz += " COLOR_TARGET "(" COLOR_TEXTURE ", tmp1.xy" FRAGMENT_XY_MULTIPLIER ").xyz * 0.1111111;\n"
		"temp.xyz += " COLOR_TARGET "(" COLOR_TEXTURE ", tmp1.zw" FRAGMENT_XY_MULTIPLIER ").xyz * 0.1111111;\n"

		"tmp1 = velo.xyxy * " FLOAT4 "(3.5, 3.5, -3.5, -3.5) + %0.xyxy;\n"
		"temp.xyz += " COLOR_TARGET "(" COLOR_TEXTURE ", tmp1.xy" FRAGMENT_XY_MULTIPLIER ").xyz * 0.1111111;\n"
		"temp.xyz += " COLOR_TARGET "(" COLOR_TEXTURE ", tmp1.zw" FRAGMENT_XY_MULTIPLIER ").xyz * 0.1111111;\n"

		"tmp1 = velo.xyxy * " FLOAT4 "(5.5, 5.5, -5.5, -5.5) + %0.xyxy;\n"
		"temp.xyz += " COLOR_TARGET "(" COLOR_TEXTURE ", tmp1.xy" FRAGMENT_XY_MULTIPLIER ").xyz * 0.1111111;\n"
		"temp.xyz += " COLOR_TARGET "(" COLOR_TEXTURE ", tmp1.zw" FRAGMENT_XY_MULTIPLIER ").xyz * 0.1111111;\n"

		"tmp1 = velo.xyxy * " FLOAT4 "(7.5, 7.5, -7.5, -7.5) + %0.xyxy;\n"
		"temp.xyz += " COLOR_TARGET "(" COLOR_TEXTURE ", tmp1.xy" FRAGMENT_XY_MULTIPLIER ").xyz * 0.1111111;\n"
		"# = temp.xyz + " COLOR_TARGET "(" COLOR_TEXTURE ", tmp1.zw" FRAGMENT_XY_MULTIPLIER ").xyz * 0.1111111;\n"
	};

	static const char gradCode[] =
	{
		FLOAT4 " samp;\n"

		"samp.w = 1.0;\n"
		"temp.w = 1.0;\n"
		"temp.xyz = " COLOR_TARGET "(" COLOR_TEXTURE ", %0" FRAGMENT_XY_MULTIPLIER ").xyz;\n"

		FLOAT2 " velo = " VELOCITY_TARGET "(" VELOCITY_TEXTURE ", %0" FRAGMENT_XY_MULTIPLIER ").xy;\n"
		FLOAT4 " strc = " STRUCTURE_TARGET "(" STRUCTURE_TEXTURE ", %0" FRAGMENT_XY_MULTIPLIER ");\n"
		"float z0 = strc.z + strc.w * 0.25;\n"
		"float minDepth = z0 - max(max(abs(strc.x), abs(strc.y)), 0.00112) * 7.0;\n"	// 0.00112 = 2/255 / 7.0

		"tmp1 = velo.xyxy * " FLOAT4 "(1.75, 1.75, -1.75, -1.75) + %0.xyxy;\n"
		"samp.xyz = " COLOR_TARGET "(" COLOR_TEXTURE ", tmp1.xy" FRAGMENT_XY_MULTIPLIER ").xyz;\n"
		FLOAT2 " depth = " STRUCTURE_TARGET "(" STRUCTURE_TEXTURE ", tmp1.xy" FRAGMENT_XY_MULTIPLIER ").zw;\n"
		"if (depth.x + depth.y * 0.25 >= minDepth) temp += samp;\n"
		"samp.xyz = " COLOR_TARGET "(" COLOR_TEXTURE ", tmp1.zw" FRAGMENT_XY_MULTIPLIER ").xyz;\n"
		"depth = " STRUCTURE_TARGET "(" STRUCTURE_TEXTURE ", tmp1.zw" FRAGMENT_XY_MULTIPLIER ").zw;\n"
		"if (depth.x + depth.y * 0.25 >= minDepth) temp += samp;\n"

		"tmp1 = velo.xyxy * " FLOAT4 "(3.5, 3.5, -3.5, -3.5) + %0.xyxy;\n"
		"samp.xyz = " COLOR_TARGET "(" COLOR_TEXTURE ", tmp1.xy" FRAGMENT_XY_MULTIPLIER ").xyz;\n"
		"depth = " STRUCTURE_TARGET "(" STRUCTURE_TEXTURE ", tmp1.xy" FRAGMENT_XY_MULTIPLIER ").zw;\n"
		"if (depth.x + depth.y * 0.25 >= minDepth) temp += samp;\n"
		"samp.xyz = " COLOR_TARGET "(" COLOR_TEXTURE ", tmp1.zw" FRAGMENT_XY_MULTIPLIER ").xyz;\n"
		"depth = " STRUCTURE_TARGET "(" STRUCTURE_TEXTURE ", tmp1.zw" FRAGMENT_XY_MULTIPLIER ").zw;\n"
		"if (depth.x + depth.y * 0.25 >= minDepth) temp += samp;\n"

		"tmp1 = velo.xyxy * " FLOAT4 "(5.25, 5.25, -5.25, -5.25) + %0.xyxy;\n"
		"samp.xyz = " COLOR_TARGET "(" COLOR_TEXTURE ", tmp1.xy" FRAGMENT_XY_MULTIPLIER ").xyz;\n"
		"depth = " STRUCTURE_TARGET "(" STRUCTURE_TEXTURE ", tmp1.xy" FRAGMENT_XY_MULTIPLIER ").zw;\n"
		"if (depth.x + depth.y * 0.25 >= minDepth) temp += samp;\n"
		"samp.xyz = " COLOR_TARGET "(" COLOR_TEXTURE ", tmp1.zw" FRAGMENT_XY_MULTIPLIER ").xyz;\n"
		"depth = " STRUCTURE_TARGET "(" STRUCTURE_TEXTURE ", tmp1.zw" FRAGMENT_XY_MULTIPLIER ").zw;\n"
		"if (depth.x + depth.y * 0.25 >= minDepth) temp += samp;\n"

		"tmp1 = velo.xyxy * " FLOAT4 "(7.0, 7.0, -7.0, -7.0) + %0.xyxy;\n"
		"samp.xyz = " COLOR_TARGET "(" COLOR_TEXTURE ", tmp1.xy" FRAGMENT_XY_MULTIPLIER ").xyz;\n"
		"depth = " STRUCTURE_TARGET "(" STRUCTURE_TEXTURE ", tmp1.xy" FRAGMENT_XY_MULTIPLIER ").zw;\n"
		"if (depth.x + depth.y * 0.25 >= minDepth) temp += samp;\n"
		"samp.xyz = " COLOR_TARGET "(" COLOR_TEXTURE ", tmp1.zw" FRAGMENT_XY_MULTIPLIER ").xyz;\n"
		"depth = " STRUCTURE_TARGET "(" STRUCTURE_TEXTURE ", tmp1.zw" FRAGMENT_XY_MULTIPLIER ").zw;\n"
		"if (depth.x + depth.y * 0.25 >= minDepth) temp += samp;\n"

		"# = temp.xyz / temp.w;\n"
	};

	if (gradientFlag)
	{
		shaderCode[0] = gradCode;
	}
	else
	{
		shaderCode[0] = code;
	}

	return (1);
}


OcclusionPostProcess::OcclusionPostProcess() : Process(kProcessOcclusionPost)
{
}

OcclusionPostProcess::OcclusionPostProcess(const OcclusionPostProcess& occlusionPostProcess) : Process(occlusionPostProcess)
{
}

OcclusionPostProcess::~OcclusionPostProcess()
{
}

Process *OcclusionPostProcess::Replicate(void) const
{
	return (new OcclusionPostProcess(*this));
}

void OcclusionPostProcess::Initialize(void)
{
	vectorNoiseTexture = Texture::Get(&vectorNoiseTextureHeader, vectorNoiseTextureImage);
}

void OcclusionPostProcess::Terminate(void)
{
	vectorNoiseTexture->Release();
}

int32 OcclusionPostProcess::GenerateShaderCode(const ShaderCompileData *compileData, const char **shaderCode) const
{
	static const char code[] =
	{
		FLOAT4 " strc = " STRUCTURE_TARGET "(" STRUCTURE_TEXTURE ", " FRAGMENT_POSITION ".xy" FRAGMENT_XY_MULTIPLIER ");\n"
		"float z0 = strc.z + strc.w * 0.25;\n"

		"float scale = " FPARAM(FRAGMENT_PARAM_CONSTANT0) ".x * z0;\n"
		FLOAT3 " normal = normalize(" FLOAT3 "(strc.x, strc.y, -scale));\n"

		"scale = 1.0 / scale;\n"

		FLOAT2 " flip = " NOISE_TARGET "(" NOISE_TEXTURE ", " FRAGMENT_POSITION ".xy * 0.25).xy;\n"
		"flip.x = flip.x * 2.0 - 1.0;\n"
		"flip *= 1.41421356;\n"

		FLOAT3 " dp;\n"

		"float t = flip.x * 0.1;\n"
		"dp.x = 0.1 - flip.x * t;\n"
		"dp.y = -flip.y * t;\n"
		FLOAT2 " depth = " STRUCTURE_TARGET "(" STRUCTURE_TEXTURE ", (" FRAGMENT_POSITION ".xy + dp.xy * scale)" FRAGMENT_XY_MULTIPLIER ").zw;\n"
		"dp.z = depth.x + depth.y * 0.25 - z0;\n"

		"float h = dot(normal, dp);\n"
		"float dp2 = dot(dp, dp);\n"
		"float d = " RSQRT "(dp2);\n"

		"float weight = clamp(1.0 - h * 0.5, 0.0, 1.0);\n"
		"float f = clamp((d * h - " FPARAM(FRAGMENT_PARAM_CONSTANT0) ".z) * weight, 0.0, 1.0);\n"
		"float total = weight;\n"

		"t = flip.x * 0.25;\n"
		"dp.x = flip.x * t - 0.25;\n"
		"dp.y = flip.y * t;\n"
		"depth = " STRUCTURE_TARGET "(" STRUCTURE_TEXTURE ", (" FRAGMENT_POSITION ".xy + dp.xy * scale)" FRAGMENT_XY_MULTIPLIER ").zw;\n"
		"dp.z = depth.x + depth.y * 0.25 - z0;\n"

		"h = dot(normal, dp);\n"
		"dp2 = dot(dp, dp);\n"
		"d = " RSQRT "(dp2);\n"

		"weight = clamp(1.0 - h * 0.5, 0.0, 1.0);\n"
		"f += clamp((d * h - " FPARAM(FRAGMENT_PARAM_CONSTANT0) ".z) * weight, 0.0, 1.0);\n"
		"total += weight;\n"

		"t = flip.y * 0.4;\n"
		"dp.x = -flip.x * t;\n"
		"dp.y = 0.4 - flip.y * t;\n"
		"depth = " STRUCTURE_TARGET "(" STRUCTURE_TEXTURE ", (" FRAGMENT_POSITION ".xy + dp.xy * scale)" FRAGMENT_XY_MULTIPLIER ").zw;\n"
		"dp.z = depth.x + depth.y * 0.25 - z0;\n"

		"h = dot(normal, dp);\n"
		"dp2 = dot(dp, dp);\n"
		"d = " RSQRT "(dp2);\n"

		"weight = clamp(1.0 - h * 0.5, 0.0, 1.0);\n"
		"f += clamp((d * h - " FPARAM(FRAGMENT_PARAM_CONSTANT0) ".z) * weight, 0.0, 1.0);\n"
		"total += weight;\n"

		"t = flip.y * 0.55;\n"
		"dp.x = flip.x * t;\n"
		"dp.y = flip.y * t - 0.55;\n"
		"depth = " STRUCTURE_TARGET "(" STRUCTURE_TEXTURE ", (" FRAGMENT_POSITION ".xy + dp.xy * scale)" FRAGMENT_XY_MULTIPLIER ").zw;\n"
		"dp.z = depth.x + depth.y * 0.25 - z0;\n"

		"h = dot(normal, dp);\n"
		"dp2 = dot(dp, dp);\n"
		"d = " RSQRT "(dp2);\n"

		"weight = clamp(1.0 - h * 0.5, 0.0, 1.0);\n"
		"f += clamp((d * h - " FPARAM(FRAGMENT_PARAM_CONSTANT0) ".z) * weight, 0.0, 1.0);\n"
		"total += weight;\n"

		RESULT_COLOR_R " = 1.0 - f * " FPARAM(FRAGMENT_PARAM_CONSTANT0) ".y / max(total, 0.001);\n"
	};

	shaderCode[0] = code;
	return (1);
}


OcclusionBlurPostProcess::OcclusionBlurPostProcess() : Process(kProcessOcclusionBlurPost)
{
}

OcclusionBlurPostProcess::OcclusionBlurPostProcess(const OcclusionBlurPostProcess& occlusionBlurPostProcess) : Process(occlusionBlurPostProcess)
{
}

OcclusionBlurPostProcess::~OcclusionBlurPostProcess()
{
}

Process *OcclusionBlurPostProcess::Replicate(void) const
{
	return (new OcclusionBlurPostProcess(*this));
}

void OcclusionBlurPostProcess::GenerateProcessData(const ShaderCompileData *compileData, ProcessData *data) const
{
	data->temporaryCount = 2;
}

int32 OcclusionBlurPostProcess::GenerateShaderCode(const ShaderCompileData *compileData, const char **shaderCode) const
{
	static const char code[] =
	{
		FLOAT3 " blur;\n"
		FLOAT2 " samp;\n"

		"blur.xy = " FLOAT2 "(0.0, 0.0);\n"
		"samp.y = 1.0;\n"

		FLOAT4 " strc = " STRUCTURE_TARGET "(" STRUCTURE_TEXTURE ", " FRAGMENT_POSITION ".xy" FRAGMENT_XY_MULTIPLIER ");\n"
		"float z0 = strc.z + strc.w * 0.25;\n"
		"float range = max(max(abs(strc.x), abs(strc.y)), 0.001) * 2.5;\n"

		"temp.xy = (" FRAGMENT_POSITION ".xy + " FLOAT2 "(-0.5, -0.5))" FRAGMENT_XY_MULTIPLIER ";\n"
		"samp.x = " OCCLUSION_TARGET "(" OCCLUSION_TEXTURE ", temp.xy).x;\n"
		FLOAT2 " depth = " STRUCTURE_TARGET "(" STRUCTURE_TEXTURE ", temp.xy).zw;\n"
		"float z = depth.x + depth.y * 0.25;\n"
		"if (abs(z - z0) < range) blur.xy += samp;\n"
		"blur.z = samp.x;\n"

		"temp.xy = (" FRAGMENT_POSITION ".xy + " FLOAT2 "(-0.5, 1.5))" FRAGMENT_XY_MULTIPLIER ";\n"
		"samp.x = " OCCLUSION_TARGET "(" OCCLUSION_TEXTURE ", temp.xy).x;\n"
		"depth = " STRUCTURE_TARGET "(" STRUCTURE_TEXTURE ", temp.xy).zw;\n"
		"z = depth.x + depth.y * 0.25;\n"
		"if (abs(z - z0) < range) blur.xy += samp;\n"
		"blur.z += samp.x;\n"

		"temp.xy = (" FRAGMENT_POSITION ".xy + " FLOAT2 "(1.5, -0.5))" FRAGMENT_XY_MULTIPLIER ";\n"
		"samp.x = " OCCLUSION_TARGET "(" OCCLUSION_TEXTURE ", temp.xy).x;\n"
		"depth = " STRUCTURE_TARGET "(" STRUCTURE_TEXTURE ", temp.xy).zw;\n"
		"z = depth.x + depth.y * 0.25;\n"
		"if (abs(z - z0) < range) blur.xy += samp;\n"
		"blur.z += samp.x;\n"

		"temp.xy = (" FRAGMENT_POSITION ".xy + " FLOAT2 "(1.5, 1.5))" FRAGMENT_XY_MULTIPLIER ";\n"
		"samp.x = " OCCLUSION_TARGET "(" OCCLUSION_TEXTURE ", temp.xy).x;\n"
		"depth = " STRUCTURE_TARGET "(" STRUCTURE_TEXTURE ", temp.xy).zw;\n"
		"z = depth.x + depth.y * 0.25;\n"
		"if (abs(z - z0) < range) blur.xy += samp;\n"
		"blur.z += samp.x;\n"

		RESULT_COLOR_R " = (blur.y > 0.0) ? blur.x / blur.y : blur.z * 0.25;\n"
	};

	shaderCode[0] = code;
	return (1);
}


GlowBloomExtractPostProcess::GlowBloomExtractPostProcess() : Process(kProcessGlowBloomExtractPost)
{
}

GlowBloomExtractPostProcess::GlowBloomExtractPostProcess(const GlowBloomExtractPostProcess& glowBloomExtractPostProcess) : Process(glowBloomExtractPostProcess)
{
}

GlowBloomExtractPostProcess::~GlowBloomExtractPostProcess()
{
}

Process *GlowBloomExtractPostProcess::Replicate(void) const
{
	return (new GlowBloomExtractPostProcess(*this));
}

void GlowBloomExtractPostProcess::GenerateProcessData(const ShaderCompileData *compileData, ProcessData *data) const
{
	data->temporaryCount = 2;
}

int32 GlowBloomExtractPostProcess::GenerateShaderCode(const ShaderCompileData *compileData, const char **shaderCode) const
{
	static const char code[] =
	{
		"tmp1 = " FRAGMENT_TEXCOORD ".xyxy + " FLOAT4 "(-2.0, 0.0, 2.0, 0.0);\n"
		"tmp2 = " FRAGMENT_TEXCOORD ".xyxy + " FLOAT4 "(-4.0, 0.0, 4.0, 0.0);\n"

		FLOAT4 " col1 = " COLOR_TARGET "(" COLOR_TEXTURE ", " FRAGMENT_TEXCOORD ".xy" FRAGMENT_XY_MULTIPLIER ");\n"
		"col1.xyz *= col1.w;\n"

		"temp = " COLOR_TARGET "(" COLOR_TEXTURE ", tmp1.xy" FRAGMENT_XY_MULTIPLIER ");\n"
		"col1.xyz += temp.xyz * temp.w;\n"
		"temp = " COLOR_TARGET "(" COLOR_TEXTURE ", tmp1.zw" FRAGMENT_XY_MULTIPLIER ");\n"
		"col1.xyz += temp.xyz * temp.w;\n"

		FLOAT4 " col2 = " COLOR_TARGET "(" COLOR_TEXTURE ", tmp2.xy" FRAGMENT_XY_MULTIPLIER ");\n"
		"col2.xyz *= col2.w;\n"
		"temp = " COLOR_TARGET "(" COLOR_TEXTURE ", tmp2.zw" FRAGMENT_XY_MULTIPLIER ");\n"
		"col2.xyz += temp.xyz * temp.w;\n"

		RESULT_COLOR ".xyz = (col2.xyz * 0.5 + col1.xyz) * 0.25;\n"
	};

	shaderCode[0] = code;
	return (1);
}


GlowBloomPostProcess::GlowBloomPostProcess() : Process(kProcessGlowBloomPost)
{
}

GlowBloomPostProcess::GlowBloomPostProcess(const GlowBloomPostProcess& glowBloomPostProcess) : Process(glowBloomPostProcess)
{
}

GlowBloomPostProcess::~GlowBloomPostProcess()
{
}

Process *GlowBloomPostProcess::Replicate(void) const
{
	return (new GlowBloomPostProcess(*this));
}

int32 GlowBloomPostProcess::GetPortCount(void) const
{
	return (1);
}

const char *GlowBloomPostProcess::GetPortName(int32 index) const
{
	return ("RGB");
}

void GlowBloomPostProcess::GenerateProcessData(const ShaderCompileData *compileData, ProcessData *data) const
{
	data->registerCount = 1;
	data->temporaryCount = 2;
	data->outputSize = 3;
	data->inputSize[0] = 3;
}

int32 GlowBloomPostProcess::GenerateShaderCode(const ShaderCompileData *compileData, const char **shaderCode) const
{
	static const char code[] =
	{
		"tmp1 = " FRAGMENT_TEXCOORD ".xyxy + " FLOAT4 "(0.0, -1.0, 0.0, 1.0);\n"
		"tmp2 = " FRAGMENT_TEXCOORD ".xyxy + " FLOAT4 "(0.0, -2.0, 0.0, 2.0);\n"

		#if C4RENDER_TEXTURE_RECTANGLE

			FLOAT3 " col1 = " GLOW_BLOOM_TARGET "(" GLOW_BLOOM_TEXTURE ", " FRAGMENT_TEXCOORD ".xy).xyz;\n"
			"col1 += " GLOW_BLOOM_TARGET "(" GLOW_BLOOM_TEXTURE ", tmp1.xy).xyz;\n"
			"col1 += " GLOW_BLOOM_TARGET "(" GLOW_BLOOM_TEXTURE ", tmp1.zw).xyz;\n"

			FLOAT3 " col2 = " GLOW_BLOOM_TARGET "(" GLOW_BLOOM_TEXTURE ", tmp2.xy).xyz;\n"
			"col2 += " GLOW_BLOOM_TARGET "(" GLOW_BLOOM_TEXTURE ", tmp2.zw).xyz;\n"

		#else

			"temp.xy = " UPARAM(UNIVERSAL_PARAM_FRAME_BUFFER_SCALE) ".xy * 2.0;\n"

			FLOAT3 " col1 = " GLOW_BLOOM_TARGET "(" GLOW_BLOOM_TEXTURE ", " FRAGMENT_TEXCOORD ".xy * temp.xy).xyz;\n"
			"col1 += " GLOW_BLOOM_TARGET "(" GLOW_BLOOM_TEXTURE ", tmp1.xy * temp.xy).xyz;\n"
			"col1 += " GLOW_BLOOM_TARGET "(" GLOW_BLOOM_TEXTURE ", tmp1.zw * temp.xy).xyz;\n"

			FLOAT3 " col2 = " GLOW_BLOOM_TARGET "(" GLOW_BLOOM_TEXTURE ", tmp2.xy * temp.xy).xyz;\n"
			"col2 += " GLOW_BLOOM_TARGET "(" GLOW_BLOOM_TEXTURE ", tmp2.zw * temp.xy).xyz;\n"

		#endif

		"# = (col2 * 0.5 + col1) * 0.25 + %0;\n"
	};

	shaderCode[0] = code;
	return (1);
}


TransformPostProcess::TransformPostProcess(bool matrixFlag) : Process(kProcessTransformPost)
{
	colorMatrixFlag = matrixFlag;
}

TransformPostProcess::TransformPostProcess(const TransformPostProcess& transformPostProcess) : Process(transformPostProcess)
{
	colorMatrixFlag = transformPostProcess.colorMatrixFlag;
}

TransformPostProcess::~TransformPostProcess()
{
}

Process *TransformPostProcess::Replicate(void) const
{
	return (new TransformPostProcess(*this));
}

int32 TransformPostProcess::GetPortCount(void) const
{
	return (1);
}

const char *TransformPostProcess::GetPortName(int32 index) const
{
	return ("RGB");
}

int32 TransformPostProcess::GenerateProcessSignature(const ShaderCompileData *compileData, unsigned_int32 *signature) const
{
	int32 count = Process::GenerateProcessSignature(compileData, signature);
	signature[count] = colorMatrixFlag;
	return (count + 1);
}

void TransformPostProcess::GenerateProcessData(const ShaderCompileData *compileData, ProcessData *data) const
{
	data->inputSize[0] = 4;
}

int32 TransformPostProcess::GenerateShaderCode(const ShaderCompileData *compileData, const char **shaderCode) const
{
	static const char vectorCode[] =
	{
		RESULT_COLOR ".xyz = %0.xyz * " FPARAM(FRAGMENT_PARAM_CONSTANT0) ".xyz + " FPARAM(FRAGMENT_PARAM_CONSTANT3) ".xyz;\n"
		RESULT_COLOR ".w = %0.w;\n"
	};

	static const char matrixCode[] =
	{
		"temp.x = dot(%0.xyz, " FPARAM(FRAGMENT_PARAM_CONSTANT0) ".xyz);\n"
		"temp.y = dot(%0.xyz, " FPARAM(FRAGMENT_PARAM_CONSTANT1) ".xyz);\n"
		"temp.z = dot(%0.xyz, " FPARAM(FRAGMENT_PARAM_CONSTANT2) ".xyz);\n"
		RESULT_COLOR ".xyz = temp.xyz + " FPARAM(FRAGMENT_PARAM_CONSTANT3) ".xyz;\n"
		RESULT_COLOR ".w = %0.w;\n"
	};

	shaderCode[0] = (colorMatrixFlag) ? matrixCode : vectorCode;
	return (1);
}


DisplayWarpProcess::DisplayWarpProcess(bool chromatic) : Process(kProcessDisplayWarp)
{
	chromaticWarpFlag = chromatic;
}

DisplayWarpProcess::DisplayWarpProcess(const DisplayWarpProcess& displayWarpProcess) : Process(displayWarpProcess)
{
	chromaticWarpFlag = displayWarpProcess.chromaticWarpFlag;
}

DisplayWarpProcess::~DisplayWarpProcess()
{
}

Process *DisplayWarpProcess::Replicate(void) const
{
	return (new DisplayWarpProcess(*this));
}

int32 DisplayWarpProcess::GenerateProcessSignature(const ShaderCompileData *compileData, unsigned_int32 *signature) const
{
	int32 count = Process::GenerateProcessSignature(compileData, signature);
	signature[count] = chromaticWarpFlag;
	return (count + 1);
}

int32 DisplayWarpProcess::GenerateShaderCode(const ShaderCompileData *compileData, const char **shaderCode) const
{
	static const char chromaticWarpCode[] =
	{
		FLOAT2 " p = (" FRAGMENT_POSITION ".xy - " FPARAM(FRAGMENT_PARAM_CONSTANT0) ".xy) * " FPARAM(FRAGMENT_PARAM_CONSTANT0) ".z;\n"
		"float r2 = p.x * p.x + p.y * p.y;\n"
		"float f = " FPARAM(FRAGMENT_PARAM_CONSTANT2) ".x + " FPARAM(FRAGMENT_PARAM_CONSTANT2) ".y * r2 + (" FPARAM(FRAGMENT_PARAM_CONSTANT2) ".z + " FPARAM(FRAGMENT_PARAM_CONSTANT2) ".w * r2) * (r2 * r2);\n"
		RESULT_COLOR ".y = " COLOR_TARGET "(" COLOR_TEXTURE ", p * f + " FPARAM(FRAGMENT_PARAM_CONSTANT1) ".xy).y;\n"
		RESULT_COLOR ".x = " COLOR_TARGET "(" COLOR_TEXTURE ", p * (f * (" FPARAM(FRAGMENT_PARAM_CONSTANT3) ".x + " FPARAM(FRAGMENT_PARAM_CONSTANT3) ".y * r2)) + " FPARAM(FRAGMENT_PARAM_CONSTANT1) ".xy).x;\n"
		RESULT_COLOR ".z = " COLOR_TARGET "(" COLOR_TEXTURE ", p * (f * (" FPARAM(FRAGMENT_PARAM_CONSTANT3) ".z + " FPARAM(FRAGMENT_PARAM_CONSTANT3) ".w * r2)) + " FPARAM(FRAGMENT_PARAM_CONSTANT1) ".xy).z;\n"
	};

	static const char warpCode[] =
	{
		FLOAT2 " p = (" FRAGMENT_POSITION ".xy - " FPARAM(FRAGMENT_PARAM_CONSTANT0) ".xy) * " FPARAM(FRAGMENT_PARAM_CONSTANT0) ".z;\n"
		"float r2 = p.x * p.x + p.y * p.y;\n"
		FLOAT2 " q = p * (" FPARAM(FRAGMENT_PARAM_CONSTANT2) ".x + " FPARAM(FRAGMENT_PARAM_CONSTANT2) ".y * r2 + (" FPARAM(FRAGMENT_PARAM_CONSTANT2) ".z + " FPARAM(FRAGMENT_PARAM_CONSTANT2) ".w * r2) * (r2 * r2)) + " FPARAM(FRAGMENT_PARAM_CONSTANT1) ".xy;\n"
		RESULT_COLOR " = " COLOR_TARGET "(" COLOR_TEXTURE ", q);\n"
	};

	if (chromaticWarpFlag)
	{
		shaderCode[0] = chromaticWarpCode;
	}
	else
	{
		shaderCode[0] = warpCode;
	}

	return (1);
}

// ZYUQURM
