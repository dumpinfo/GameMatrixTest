 

#ifndef C4Shaders_h
#define C4Shaders_h


//# \component	Graphics Manager
//# \prefix		GraphicsMgr/


#include "C4Horizon.h"


namespace C4
{
	typedef EngineResult			ShaderResult;
	typedef Graph<Process, Route>	ShaderGraph;


	enum : ShaderResult
	{
		kShaderOkay							= kEngineOkay,
		kShaderIncomplete					= 0x0001,
		kShaderRegisterOverflow				= 0x0002,
		kShaderLiteralOverflow				= 0x0003,
		kShaderTexcoordOverflow				= 0x0004,
		kShaderTextureUnitOverflow			= 0x0005,
		kShaderResultCount					= 6
	};


	enum
	{
		kShaderGraphTexcoord1,
		kShaderGraphTexcoord2,
		kShaderGraphParallax,
		kShaderGraphDiffuseColor,
		kShaderGraphSpecularColor,
		kShaderGraphSpecularExponent,
		kShaderGraphMicrofacet,
		kShaderGraphMicrofacetReflectivity,
		kShaderGraphEmissionColor,
		kShaderGraphReflectionColor,
		kShaderGraphRefractionColor,
		kShaderGraphEnvironmentColor,
		kShaderGraphTextureMap1,
		kShaderGraphTextureMap2,
		kShaderGraphNormalMap1,
		kShaderGraphNormalMap2,
		kShaderGraphSpecularMap,
		kShaderGraphEmissionMap,
		kShaderGraphOpacityMap,
		kShaderGraphDeltaDepth,
		kShaderGraphDepthLinearRamp,
		kShaderGraphFragmentDepth,
		kShaderGraphDepthMultiply,
		kShaderGraphVertexColor,
		kShaderGraphTextureCombiner,
		kShaderGraphNormalCombiner,
		kShaderGraphColorMultiply,
		kShaderGraphAlphaMultiply,
		kShaderGraphDiffuseMultiply1,
		kShaderGraphDiffuseMultiply2,
		kShaderGraphSpecularMultiply1,
		kShaderGraphSpecularMultiply2,
		kShaderGraphOcclusionMultiply,
		kShaderGraphEmissionMultiply,
		kShaderGraphOpacityMultiply,
		kShaderGraphOpacityInvert,
		kShaderGraphRefractionMultiply,
		kShaderGraphEnvironmentMultiply,
		kShaderGraphDiffuseReflection,
		kShaderGraphSpecularReflection,
		kShaderGraphLightSum,
		kShaderGraphHorizon,
		kShaderGraphBloom1,
		kShaderGraphBloom2,
		kShaderGraphAmbientOutput,
		kShaderGraphLightOutput,
		kShaderGraphAlphaOutput,
		kShaderGraphAlphaTestOutput,
		kShaderGraphEmissionOutput,
		kShaderGraphReflectionOutput,
		kShaderGraphRefractionOutput,
		kShaderGraphEnvironmentOutput,
		kShaderGraphTerrainEnvironmentOutput,
		kShaderGraphGlowOutput,
		kShaderGraphBloomOutput,
		kShaderGraphImpostorDepthOutput,
		kShaderGraphProcessCount
	};


	enum
	{
		kEffectGraphTexcoord,
		kEffectGraphEffectColor,
		kEffectGraphVertexColor,
		kEffectGraphTextureMap,
		kEffectGraphDeltaDepth,
		kEffectGraphDepthLinearRamp,
		kEffectGraphFragmentDepth,
		kEffectGraphDepthMultiply, 
		kEffectGraphFire,
		kEffectGraphDistortion,
		kEffectGraphColorMultiply, 
		kEffectGraphAlphaMultiply,
		kEffectGraphEffectMultiply, 
		kEffectGraphColorOutput,
		kEffectGraphAlphaOutput,
		kEffectGraphAlphaTestOutput, 
		kEffectGraphProcessCount
	}; 
 

	enum
	{
		kPlainGraphTexcoord1, 
		kPlainGraphTexcoord2,
		kPlainGraphParallax,
		kPlainGraphDiffuseColor,
		kPlainGraphTextureMap1,
		kPlainGraphTextureMap2,
		kPlainGraphVertexColor,
		kPlainGraphTextureCombiner,
		kPlainGraphColorMultiply,
		kPlainGraphDiffuseMultiply1,
		kPlainGraphDiffuseMultiply2,
		kPlainGraphAlphaTestOutput,
		kPlainGraphProcessCount
	};


	//# \class	ShaderAttribute		Shader graph container.
	//
	//# The $ShaderAttribute$ class is a container for a set of shader graphs.
	//
	//# \def	class ShaderAttribute final : public Attribute
	//
	//# \ctor	ShaderAttribute();
	//
	//# \desc
	//# The $ShaderAttribute$ class is used to store a set of shader graphs in a material object.
	//
	//# \base	Attribute		A shader attribute is a special type of attribute.
	//
	//# \also	$@Process@$
	//# \also	$@Route@$


	//# \function	ShaderAttribute::GetShaderGraph		Returns a pointer to a shader graph.
	//
	//# \proto	ShaderGraph *GetShaderGraph(void);
	//# \proto	const ShaderGraph *GetShaderGraph(void) const;
	//
	//# \desc
	//# The $GetShaderGraph$ function returns a pointer to the shader graph stored inside a shader attribute.


	//# \function	ShaderAttribute::SetParameterValue		Sets a shader parameter value.
	//
	//# \proto	void SetParameterValue(int32 slot, const Vector4D& param);
	//
	//# \param	slot	The shader parameter slot. This can be between 0 and 7, inclusive.
	//# \param	param	The new shader parameter value.
	//
	//# \desc
	//# The $SetParameterValue$ function sets the shader parameter value indexed by the $slot$ parameter to
	//# the value specified by the $param$ parameter. This affects all constant processes in both shader graphs
	//# contained in the shader attribute. For vector and color constant processes, all four components of the
	//# parameter are used. For scalar constant processes, only the $x$ component of the parameter is used.


	class ShaderAttribute final : public Attribute
	{
		private:

			enum
			{
				kMaxShaderSourceSize		= 16384,
				kMaxShaderSignatureSize		= 1024
			};

			ShaderGraph			shaderGraph;

			Attribute *Replicate(void) const override;

			static void PackShader(const ShaderGraph *graph, Packer& data, unsigned_int32 packFlags);
			static void UnpackShader(ShaderGraph *graph, Unpacker& data, unsigned_int32 unpackFlags);

			static ShaderResult PrepareProcessPorts(const Process *process, const ShaderCompileData *compileData);
			static ShaderResult PrepareAmbientShader(const ShaderCompileData *compileData, ShaderGraph *graph, List<Process> *terminalList);
			static ShaderResult PrepareUnifiedShader(const ShaderCompileData *compileData, ShaderGraph *graph, List<Process> *terminalList);
			static ShaderResult PrepareLightShader(const ShaderCompileData *compileData, ShaderGraph *graph, List<Process> *terminalList);
			static ShaderResult PreparePlainShader(ShaderType shaderType, const ShaderCompileData *compileData, ShaderGraph *graph, List<Process> *terminalList);

			static Process *FindDerivedInterpolant(ProcessType type, int32 count, Process *const *interpolant);
			static void OrganizeDerivedInterpolants(const ShaderCompileData *compileData, ShaderGraph *graph);

			static void OptimizeTextureMaps(const ShaderGraph *graph);
			static void EliminateDeadCode(const ShaderGraph *graph, List<Process> *terminalList);
			static void CalculatePathLengths(const ShaderGraph *graph, List<Process> *processList, List<Process> *readyList);
			static int32 ScheduleShader(const ShaderCompileData *compileData, List<Process> *readyList, List<Process> *scheduleList, unsigned_int32 *shaderSignature);

			static bool AllocateOutputRegister(ProcessData *data, unsigned_int8 *registerLive);
			static bool AllocateInterpolant(Type type, int32 size, ShaderAllocationData *allocData, bool (*usage)[4]);
			static int32 AllocateTextureUnit(ShaderData *shaderData, const Render::TextureObject *textureObject);
			static ShaderResult AllocateShaderResources(const ShaderCompileData *compileData, ShaderAllocationData *allocData, int32 processCount, ProcessData *processData, const List<Process> *scheduleList);

			static int32 GenerateSwizzleData(const char *code, SwizzleData *swizzleData);
			static int32 GenerateLiteralConstantValue(Type type, const ShaderAllocationData *allocData, char *value);
			static void GenerateShaderCode(const ShaderCompileData *compileData, const ShaderAllocationData *allocData, const ProcessData *processData, const List<Process> *scheduleList, char *shaderCode, int32 *shaderLength);
			static int32 GenerateShaderProlog(const ShaderCompileData *compileData, const ShaderAllocationData *allocData, char *shaderCode);

			static int32 GenerateVertexOutputName(Type type, const ShaderAllocationData *allocData, int32 mask, char *name);
			static VertexShader *GenerateVertexShader(const ShaderCompileData *compileData, const ShaderAllocationData *allocData, unsigned_int32 *shaderStateFlags);
			static void BuildStateProcList(const ShaderCompileData *compileData, unsigned_int32 shaderStateFlags);

			#if C4OPENGL

				static void BindShaderUniforms(ShaderType type, ShaderProgram *shaderProgram);

			#endif

			static Process *BuildTextureCombiner(const MaterialObject *materialObject, ShaderGraph *graph, Process **textureMap1, Process **textureMap2, Process **textureCombiner, Process **vertexColor);

		public:

			static char				sourceStorage[kMaxShaderSourceSize];
			static unsigned_int32	signatureStorage[kMaxShaderSignatureSize];

			C4API ShaderAttribute();
			C4API ShaderAttribute(const ShaderAttribute& shaderAttribute);
			C4API ~ShaderAttribute();

			ShaderGraph *GetShaderGraph(void)
			{
				return (&shaderGraph);
			}

			const ShaderGraph *GetShaderGraph(void) const
			{
				return (&shaderGraph);
			}

			void Pack(Packer& data, unsigned_int32 packFlags) const override;
			void Unpack(Unpacker& data, unsigned_int32 unpackFlags) override;
			bool UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags);
			void *BeginSettingsUnpack(void) override;

			bool operator ==(const Attribute& attribute) const;

			C4API static void CloneShader(const ShaderGraph *sourceGraph, ShaderGraph *destinGraph, bool reference = false);

			ShaderResult CompileShader(ShaderType type, const ShaderKey& key, const Renderable *renderable, const RenderSegment *renderSegment, ShaderData *shaderData) const;
			static ShaderResult CompileShader(ShaderGraph *graph, ShaderType type, const ShaderKey& key, const Renderable *renderable, const RenderSegment *renderSegment, ShaderData *shaderData);
			C4API ShaderResult TestShader(ShaderType type, const ShaderKey& key, const Renderable *renderable, const RenderSegment *renderSegment) const;
			static ShaderProgram *CompilePostShader(const ShaderGraph *graph, VertexShader *vertexShader);

			C4API static void BuildRegularShaderGraph(const Renderable *renderable, const RenderSegment *renderSegment, const MaterialObject *materialObject, const List<Attribute> *attributeList, ShaderGraph *graph, Process **process);
			static void BuildEffectShaderGraph(const Renderable *renderable, const RenderSegment *renderSegment, const MaterialObject *materialObject, const List<Attribute> *attributeList, ShaderGraph *graph);
			static void BuildPlainShaderGraph(ShaderType shaderType, const Renderable *renderable, const RenderSegment *renderSegment, const MaterialObject *materialObject, const List<Attribute> *attributeList, ShaderGraph *graph);

			C4API void SetParameterValue(int32 slot, const Vector4D& param);
	};
}


#endif

// ZYUQURM
