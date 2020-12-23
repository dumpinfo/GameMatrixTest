 

#ifndef C4Processes_h
#define C4Processes_h


//# \component	Graphics Manager
//# \prefix		GraphicsMgr/


#include "C4Renderable.h"


namespace C4
{
	#if C4OPENGL

		#define RESULT_COLOR			"fcolor"
		#define RESULT_COLOR_R			"fcolor.x"
		#define RESULT_VELOCITY			"velocity"
		#define RESULT_DEPTH			"gl_FragDepth"

		#define FRAGMENT_POSITION		"gl_FragCoord"
		#define FRAGMENT_COLOR0			"vcolor[0]"
		#define FRAGMENT_COLOR1			"vcolor[1]"
		#define FRAGMENT_TEXCOORD		"texcoord[0]"

		#if C4RENDER_TEXTURE_RECTANGLE

			#define FRAGMENT_XY_MULTIPLIER

		#else

			#define FRAGMENT_XY_MULTIPLIER	" * " UPARAM(UNIVERSAL_PARAM_FRAME_BUFFER_SCALE) ".xy"

		#endif

		#define FLOAT2					"vec2"
		#define FLOAT3					"vec3"
		#define FLOAT4					"vec4"
		#define HALF3					"vec3"

		#define DDX						"dFdx"
		#define DDY						"dFdy"
		#define LERP					"mix"
		#define FRAC					"fract"
		#define RSQRT					"inversesqrt"

	#elif C4PSSL //[ PS4

		// -- PS4 code hidden --

	#elif C4CG //[ PS3

		// -- PS3 code hidden --

	#endif //]


	typedef Type	ProcessType;
	typedef Type	ProcessGroup;


	enum : ProcessType
	{
		kProcessSection						= 'SECT'
	};


	enum : ProcessType
	{
		kProcessConstant					= 'CNST',
		kProcessScalar						= 'SCLR',
		kProcessVector						= 'VCTR',
		kProcessColor						= 'COLR'
	};


	enum : ProcessType
	{
		kProcessParameter					= 'PARM',
		kProcessTime						= 'TIME',
		kProcessDetailLevel					= 'DETL',
		kProcessVertexColor					= 'VCOL',
		kProcessFragmentPosition			= 'FPOS',
		kProcessFragmentDepth				= 'FDEP',
		kProcessPrimitiveFacing				= 'PFAC'
	};


	enum : ProcessType
	{
		kProcessTextureMap					= 'TEXM',
		kProcessProjectiveTextureMap		= 'PJTX',
		kProcessNormalMap					= 'NRMM',
		kProcessImpostorTexture				= 'ITEX',
		kProcessImpostorNormal				= 'INRM',
		kProcessTerrainTexture				= 'TTEX',
		kProcessTerrainNormal				= 'TNRM',
		kProcessTerrainNormal2				= 'TNM2',
		kProcessTerrainNormal3				= 'TNM3',
		kProcessMerge2						= 'MRG2',
		kProcessMerge3						= 'MRG3', 
		kProcessMerge4						= 'MRG4'
	};
 

	enum : ProcessType 
	{
		kProcessInterpolant					= 'TERP',
		kProcessDerived						= 'DERV', 
		kProcessTexcoord0					= 'TEX0',
		kProcessTexcoord1					= 'TEX1', 
		kProcessRawTexcoord					= 'RTXC', 
		kProcessImpostorTexcoord			= 'IMPT',
		kProcessImpostorBlend				= 'IBLD',
		kProcessImpostorDepth				= 'IDEP',
		kProcessTerrainTexcoord				= 'TERA', 
		kProcessTriplanarBlend				= 'TBLD',
		kProcessVertexGeometry				= 'GEOM',
		kProcessObjectPosition				= 'POSI',
		kProcessWorldPosition				= 'WPOS',
		kProcessObjectNormal				= 'NRML',
		kProcessObjectTangent				= 'TANG',
		kProcessObjectBitangent				= 'BTNG',
		kProcessWorldNormal					= 'WNRM',
		kProcessWorldTangent				= 'WTAN',
		kProcessWorldBitangent				= 'WBTN',
		kProcessTangentLightDirection		= 'LDIR',
		kProcessTangentViewDirection		= 'VDIR',
		kProcessTangentHalfwayDirection		= 'HDIR',
		kProcessObjectLightDirection		= 'OLDR',
		kProcessObjectViewDirection			= 'OVDR',
		kProcessObjectHalfwayDirection		= 'OHDR',
		kProcessTerrainLightDirection		= 'TLDR',
		kProcessTerrainViewDirection		= 'TVDR',
		kProcessTerrainHalfwayDirection		= 'THDR',
		kProcessTangentLightVector			= 'LVEC',
		kProcessTangentViewVector			= 'VVEC',
		kProcessObjectLightVector			= 'OLVC',
		kProcessObjectViewVector			= 'OVVC'
	};


	enum : ProcessType
	{
		kProcessAbsolute					= 'ABS ',
		kProcessAdd							= 'ADD ',
		kProcessSubtract					= 'SUB ',
		kProcessAverage						= 'AVG ',
		kProcessInvert						= 'INV ',
		kProcessExpand						= 'VEX ',
		kProcessMultiply					= 'MUL ',
		kProcessMultiplyAdd					= 'MAD ',
		kProcessLerp						= 'LRP ',
		kProcessDivide						= 'DIV ',
		kProcessDot3						= 'DP3 ',
		kProcessDot4						= 'DP4 ',
		kProcessCross						= 'XPD ',
		kProcessReciprocal					= 'RCP ',
		kProcessReciprocalSquareRoot		= 'RSQ ',
		kProcessSquareRoot					= 'SQR ',
		kProcessMagnitude					= 'MAG ',
		kProcessNormalize					= 'NRM ',
		kProcessFloor						= 'FLR ',
		kProcessRound						= 'RND ',
		kProcessFraction					= 'FRC ',
		kProcessSaturate					= 'SAT ',
		kProcessMinimum						= 'MIN ',
		kProcessMaximum						= 'MAX ',
		kProcessSetLessThan					= 'SLT ',
		kProcessSetGreaterThan				= 'SGT ',
		kProcessSetLessEqual				= 'SLE ',
		kProcessSetGreaterEqual				= 'SGE ',
		kProcessSetEqual					= 'SEQ ',
		kProcessSetNotEqual					= 'SNE ',
		kProcessSine						= 'SIN ',
		kProcessCosine						= 'COS ',
		kProcessExp2						= 'EX2 ',
		kProcessLog2						= 'LG2 ',
		kProcessPower						= 'POW '
	};


	enum : ProcessType
	{
		kProcessDiffuse						= 'DIFF',
		kProcessSpecular					= 'SPEC',
		kProcessMicrofacet					= 'MCFT',
		kProcessTerrainDiffuse				= 'TDIF',
		kProcessTerrainSpecular				= 'TSPC',
		kProcessGenerateImpostorNormal		= 'GINM',
		kProcessCombineNormals				= 'CNRM',
		kProcessFrontNormal					= 'FNRM',
		kProcessReflectVector				= 'RVEC',
		kProcessLinearRamp					= 'RAMP',
		kProcessSmoothParameter				= 'SMTH',
		kProcessSteepParameter				= 'STEP',
		kProcessWorldTransform				= 'WXFM',
		kProcessDeltaDepth					= 'DLTA',
		kProcessParallax					= 'PLAX',
		kProcessFire						= 'FIRE',
		kProcessDistortion					= 'DSTN'
	};


	enum : ProcessType
	{
		kProcessTerminal					= 'TERM',
		kProcessKill						= 'KILL',
		kProcessImpostorTransition			= 'IMPX',
		kProcessGeometryTransition			= 'GEOX'
	};


	enum : ProcessType
	{
		kProcessFrameBuffer					= 'FBUF'
	};


	enum : ProcessType
	{
		kProcessInput						= 'INPT',
		kProcessOutput						= 'OUTP',
		kProcessNullOutput					= 'NULO',
		kProcessAddOutput					= 'ADDO',
		kProcessAlphaOutput					= 'ALFA',
		kProcessAlphaTestOutput				= 'ATST',
		kProcessAmbientOutput				= 'AMBT',
		kProcessVertexOcclusionOutput		= 'VOCC',
		kProcessEmissionOutput				= 'EMIS',
		kProcessReflectionOutput			= 'REFL',
		kProcessRefractionOutput			= 'REFR',
		kProcessEnvironmentOutput			= 'ENVR',
		kProcessTerrainEnvironmentOutput	= 'TENV',
		kProcessImpostorDepthOutput			= 'IMPZ',
		kProcessLightOutput					= 'LITE',
		kProcessGlowOutput					= 'GLOW',
		kProcessBloomOutput					= 'BLOM',
		kProcessGlowBloomOutput				= 'GLBL',
		kProcessStructureOutput				= 'STRC'
	};


	enum : ProcessType
	{
		kProcessInfiniteShadow				= 'ISHD',
		kProcessPointShadow					= 'PSHD',
		kProcessSpotShadow					= 'SSHD',
		kProcessPointIllumination			= 'PILM',
		kProcessCubeIllumination			= 'CILM',
		kProcessSpotIllumination			= 'SILM'
	};


	enum : ProcessType
	{
		kProcessConstantFog					= 'CFOG',
		kProcessLinearFog					= 'LFOG',
		kProcessAmbientFog					= 'AMBF',
		kProcessLightFog					= 'LITF',
		kProcessAlphaFog					= 'ALPF'
	};


	enum : ProcessType
	{
		kProcessColorPost					= 'PCOL',
		kProcessDistortPost					= 'PDST',
		kProcessMotionBlurPost				= 'PMBL',
		kProcessOcclusionPost				= 'POCC',
		kProcessOcclusionBlurPost			= 'POBL',
		kProcessGlowBloomExtractPost		= 'PGBE',
		kProcessGlowBloomPost				= 'PGLB',
		kProcessTransformPost				= 'PXFM',
		kProcessDisplayWarp					= 'DWRP'
	};


	//# \enum	RouteFlags

	enum
	{
		kRouteHighDetail					= 1 << 0		//## The route only exists at the highest detail level.
	};


	//# \enum	ProcessFlags

	enum
	{
		kProcessHighDetail					= 1 << 0,		//## The process only functions at the highest detail level. This only affects certain processes.
		kProcessLowDetail					= 1 << 1		//## The process only functions at the lowest detail level. This only affects certain processes.
	};


	//# \enum	ProcessPortFlags

	enum
	{
		kProcessPortOptional				= 1 << 0,		//## The input to the port is optional for all levels of detail.
		kProcessPortOmissible				= 1 << 1		//## The input to the port must exist at the highest detail, but can be omitted at lower detail levels.
	};


	enum
	{
		kShaderSourcePrimaryColor			= 1 << 0,
		kShaderSourceSecondaryColor			= 1 << 1,
		kShaderSourceTangentViewDirection	= 1 << 2
	};


	enum
	{
		kShaderOutputAmbient				= 1 << 0,
		kShaderOutputLight					= 1 << 1,
		kShaderOutputPlain					= 1 << 2
	};


	#if C4PS3 //[ PS3

		// -- PS3 code hidden --

	#endif //]


	enum
	{
		kMaxProcessPortCount				= 4,
		kProcessPortHiddenDependency		= 255,

		kMaxProcessLiteralCount				= 2,
		kMaxShaderLiteralCount				= 16,

		kMaxProcessInterpolantCount			= 6,
		kMaxProcessTextureCount				= 4,
		kMaxProcessCodeCount				= 5
	};


	class Route;
	class Process;


	struct SwizzleData
	{
		char				size;
		bool				negate;
		unsigned_int8		component[4];
	};


	struct LiteralData
	{
		Type				literalType;
		float				literalValue;
	};


	struct InterpolantData
	{
		Type				interpolantType;
		int32				texcoordIndex;
		SwizzleData			swizzleData;
	};


	struct ProcessData
	{
		int32							registerCount;
		int32							preregisterCount;
		int32							temporaryCount;
		int32							literalCount;
		int32							interpolantCount;
		int32							textureCount;
		int32							passthruPort;

		int32							outputSize;
		int32							inputSize[kMaxProcessPortCount];

		LiteralData						literalData[kMaxProcessLiteralCount];
		Type							interpolantType[kMaxProcessInterpolantCount];
		const Render::TextureObject		*textureObject[kMaxProcessTextureCount];

		int32							outputRegister;
		mutable int32					outputCount;

		unsigned_int8					textureUnit[kMaxProcessTextureCount];
	};


	struct ShaderAllocationData
	{
		int32							maxRegister;
		int32							temporaryCount;

		#if C4PS3 //[ PS3

			// -- PS3 code hidden --

		#endif //]

		int32							literalCount;
		LiteralData						literalData[kMaxShaderLiteralCount];

		int32							interpolantCount;
		InterpolantData					interpolantData[kMaxShaderInterpolantCount];
	};


	struct ShaderCompileData
	{
		const Renderable				*renderable;
		const RenderSegment				*renderSegment;
		ShaderData						*shaderData;

		ShaderType						shaderType;
		ShaderVariant					shaderVariant;
		int32							detailLevel;
		bool							shadowFlag;

		mutable unsigned_int32			shaderSourceFlags;
		mutable const Process			*terrainViewDirection;
	};


	class ProcessRegistration : public Registration<Process, ProcessRegistration>
	{
		private:

			const char		*processName;
			ProcessGroup	processGroup;

		protected:

			ProcessRegistration(ProcessType type, const char *name, ProcessGroup group = 0);

		public:

			~ProcessRegistration();

			ProcessType GetProcessType(void) const
			{
				return (GetRegistrableType());
			}

			const char *GetProcessName(void) const
			{
				return (processName);
			}

			ProcessGroup GetProcessGroup(void) const
			{
				return (processGroup);
			}
	};


	template <class classType> class ProcessReg : public ProcessRegistration
	{
		public:

			ProcessReg(ProcessType type, const char *name, ProcessGroup group = 0) : ProcessRegistration(type, name, group)
			{
			}

			Process *Create(void) const
			{
				return (new classType);
			}
	};


	//# \class	Route	Represents the data flow from one process to another in a shader graph.
	//
	//# The $Route$ class represents the data flow from one process to another in a shader graph.
	//
	//# \def	class Route : public GraphEdge<Process, Route>, public Packable, public Configurable, public Memory<Process>
	//
	//# \ctor	Route(Process *start, Process *finish, int32 port);
	//
	//# \param	start		A pointer to the process for which the route is an output.
	//# \param	finish		A pointer to the process for which the route is an input.
	//# \param	port		The index of the input port to which the route is connected.
	//
	//# \desc
	//# A $Route$ object is an edge in a shader graph that connects two processes. It represents the data flow from
	//# the output of one process to a specific input port of another process.
	//#
	//# When a route is initially constructed, the output process from which the route originates must be specified by
	//# the $start$ parameter, and the input process to which it is connected must be specified by the $finish$ parameter.
	//# The $port$ parameter specifies which port of the input process the route is connected to, and it must be in the
	//# range [0,&nbsp;<i>n</i>&nbsp;&minus;&nbsp;1], where <i>n</i> is the number of ports belonging to the input process.
	//#
	//# A route carries information about how data is modified before it is sent to the destination process.
	//# The data can be negated, and it can have a swizzle applied to it.
	//
	//# \base	Utilities/GraphEdge<Process, Route>		A route is an edge in a graph.
	//# \base	ResourceMgr/Packable					Routes can be packed for storage in resources.
	//# \base	InterfaceMgr/Configurable				Routes can be configured by the user in the Shader Editor.
	//# \base	MemoryMgr/Memory<Process>				Components of a shader graph are stored in a dedicated heap.
	//
	//# \also	$@Process@$


	//# \function	Route::GetRouteFlags		Returns the route flags.
	//
	//# \proto	unsigned_int32 GetRouteFlags(void) const;
	//
	//# \desc
	//# The $GetRouteFlags$ function returns the flags for a route, which can be a combination (through logical OR)
	//# of the following constants.
	//
	//# \table	RouteFlags
	//
	//# The route flags are initially set to 0.
	//
	//# \also	$@Route::SetRouteFlags@$


	//# \function	Route::SetRouteFlags		Sets the route flags.
	//
	//# \proto	void SetRouteFlags(unsigned_int32 flags);
	//
	//# \param	flags	The new route flags.
	//
	//# \desc
	//# The $SetRouteFlags$ function sets the flags for a route, which can be a combination (through logical OR)
	//# of the following constants.
	//
	//# \table	RouteFlags
	//
	//# The route flags are initially set to 0.
	//
	//# \also	$@Route::GetRouteFlags@$


	//# \function	Route::GetRouteNegation		Returns the route negation flag.
	//
	//# \proto	bool GetRouteNegation(void) const;
	//
	//# \desc
	//# The $GetRouteNegation$ function returns a boolean value indicating whether the route negates its data.
	//# If the return value is $true$, then the data is negated.
	//#
	//# Initially, a route does not negate its data.
	//
	//# \also	$@Route::SetRouteNegation@$
	//# \also	$@Route::GetRouteSwizzle@$
	//# \also	$@Route::SetRouteSwizzle@$


	//# \function	Route::SetRouteNegation		Sets the route negation flag.
	//
	//# \proto	void SetRouteNegation(bool negation);
	//
	//# \param	negation	The new negation flag.
	//
	//# \desc
	//# The $SetRouteNegation$ function specifies whether the route negates its data.
	//# If the value of the $negation$ parameter is $true$, then the data is negated.
	//#
	//# Initially, a route does not negate its data.
	//
	//# \also	$@Route::GetRouteNegation@$
	//# \also	$@Route::GetRouteSwizzle@$
	//# \also	$@Route::SetRouteSwizzle@$


	//# \function	Route::GetRouteSwizzle		Returns the route swizzle code.
	//
	//# \proto	unsigned_int32 GetRouteSwizzle(void) const;
	//
	//# \desc
	//# The $GetRouteSwizzle$ function returns a four-character code containing the route swizzle code.
	//# The swizzle code always consists of four lower-case letters that can be a combination of letters
	//# in the set {x, y, z, w, r, g, b, a}.
	//#
	//# Initially, the route swizzle is $'xyzw'$.
	//
	//# \also	$@Route::SetRouteSwizzle@$
	//# \also	$@Route::GetRouteNegation@$
	//# \also	$@Route::SetRouteNegation@$


	//# \function	Route::SetRouteSwizzle		Returns the route swizzle code.
	//
	//# \proto	void SetRouteSwizzle(unsigned_int32 swizzle);
	//
	//# \param	swizzle		The new swizzle code, consisting of four lower-case letters.
	//
	//# \desc
	//# The $SetRouteSwizzle$ function sets the route swizzle code to that specified by the $swizzle$ parameter.
	//# The swizzle code must consists of four lower-case letters that is a combination of letters
	//# in the set {x, y, z, w, r, g, b, a}.
	//#
	//# Initially, the route swizzle is $'xyzw'$.
	//
	//# \also	$@Route::SetRouteSwizzle@$
	//# \also	$@Route::GetRouteNegation@$
	//# \also	$@Route::SetRouteNegation@$


	class Route : public GraphEdge<Process, Route>, public Packable, public Configurable, public EngineMemory<Process>
	{
		friend class ShaderAttribute;

		private:

			unsigned_int32		routeFlags;
			int32				routePort;

			bool				routeNegation;
			unsigned_int32		routeSwizzle;

			static const unsigned_int8 swizzleTable[26];

		public:

			C4API Route(Process *start, Process *finish, int32 port);
			C4API Route(const Route& route, Process *start, Process *finish);
			C4API ~Route();

			static char GetSwizzleChar(int32 c)
			{
				return ((char) ('w' + ((c + 1) & 3)));
			}

			unsigned_int32 GetRouteFlags(void) const
			{
				return (routeFlags);
			}

			void SetRouteFlags(unsigned_int32 flags)
			{
				routeFlags = flags;
			}

			int32 GetRoutePort(void) const
			{
				return (routePort);
			}

			void SetRoutePort(int32 port)
			{
				routePort = port;
			}

			bool GetRouteNegation(void) const
			{
				return (routeNegation);
			}

			void SetRouteNegation(bool negation)
			{
				routeNegation = negation;
			}

			unsigned_int32 GetRouteSwizzle(void) const
			{
				return (routeSwizzle);
			}

			void SetRouteSwizzle(unsigned_int32 swizzle)
			{
				routeSwizzle = swizzle;
			}

			void Pack(Packer& data, unsigned_int32 packFlags) const override;
			void Unpack(Unpacker& data, unsigned_int32 unpackFlags) override;
			bool UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags);

			int32 GetSettingCount(void) const override;
			Setting *GetSetting(int32 index) const override;
			void SetSetting(const Setting *setting) override;

			bool operator ==(const Route& route) const;

			int32 GenerateOutputSize(void) const;
			int32 GenerateOutputIdentifier(const ShaderCompileData *compileData, const ShaderAllocationData *allocData, SwizzleData *swizzleData, char *name) const;
			unsigned_int32 GenerateRouteSignature(void) const;
	};


	//# \class	Process		Represents an operation performed on data in a shader graph.
	//
	//# The $Process$ class represents an operation performed on data in a shader graph.
	//
	//# \def	class Process : public GraphElement<Process, Route>, public ListElement<Process>, public Packable,
	//# \def2	public Configurable, public Registrable<Process, ProcessRegistration>, public Memory<Process>
	//
	//# \ctor	Process(ProcessType type);
	//
	//# \param	type	The type of the process.
	//
	//# \desc
	//# A $Process$ object is a node in a shader graph that represents some kind of operation or data source.
	//# Each type of process has between 0 and 4 input ports through which data can be received from other processes.
	//# Most types of processes generate an output value that can be sent to the input ports of other processes.
	//
	//# \base	Utilities/GraphElement<Process, Route>				A process is an element in a graph.
	//# \base	Utilities/ListElement<Process>						Used internally by the Graphics Manager.
	//# \base	ResourceMgr/Packable								Processes can be packed for storage in resources.
	//# \base	InterfaceMgr/Configurable							Processes can be configured by the user in the Shader Editor.
	//# \base	System/Registrable<Process, ProcessRegistration>	Process types are registered for display in the Shader Editor.
	//# \base	MemoryMgr/Memory<Process>							Components of a shader graph are stored in a dedicated heap.
	//
	//# \also	$@Route@$
	//
	//# \wiki	Shader_Editor					Shader Editor
	//# \wiki	Basic_Shader_Processes			List of Basic Shader Processes
	//# \wiki	Mathematical_Shader_Processes	List of Mathematical Shader Processes
	//# \wiki	Complex_Shader_Processes		List of Complex Shader Processes
	//# \wiki	Interpolant_Shader_Processes	List of Interpolant Shader Processes


	//# \function	Process::GetProcessType		Returns the type of a process.
	//
	//# \proto	ProcessType GetProcessType(void) const;
	//
	//# \desc
	//# The $GetProcessType$ function returns the type of a process.


	//# \function	Process::GetProcessFlags		Returns the process flags.
	//
	//# \proto	unsigned_int32 GetProcessFlags(void) const;
	//
	//# \desc
	//# The $GetProcessFlags$ function returns the flags for a process, which can be a combination (through logical OR)
	//# of the following constants.
	//
	//# \table	ProcessFlags
	//
	//# The process flags are initially set to 0.
	//
	//# \also	$@Process::SetProcessFlags@$


	//# \function	Process::SetProcessFlags		Sets the process flags.
	//
	//# \proto	void SetProcessFlags(unsigned_int32 flags);
	//
	//# \param	flags	The new process flags.
	//
	//# \desc
	//# The $SetProcessFlags$ function sets the flags for a process, which can be a combination (through logical OR)
	//# of the following constants.
	//
	//# \table	ProcessFlags
	//
	//# The process flags are initially set to 0.
	//
	//# \also	$@Process::GetProcessFlags@$


	//# \function	Process::GetPortRoute		Returns the route connected to an input port.
	//
	//# \proto	Route *GetPortRoute(int32 port) const;
	//
	//# \param	port	The index of the input port.
	//
	//# \desc
	//# The $GetPortRoute$ function returns a pointer to the route connected to the input port having the
	//# index specified by the $port$ parameter. If the input port has no route connected to it, or the
	//# process does not have an input port with the given index, then the return value is $nullptr$.
	//
	//# \also	$@Process::GetPortCount@$
	//# \also	$@Process::GetPortFlags@$


	//# \function	Process::GetPortCount		Returns the number of input ports for a process.
	//
	//# \proto	virtual int32 GetPortCount(void) const;
	//
	//# \desc
	//# The $GetPortCount$ function returns the number of input ports possessed by a process.
	//# The return value can be between 0 and 4, inclusive.
	//
	//# \also	$@Process::GetPortRoute@$
	//# \also	$@Process::GetPortFlags@$


	//# \function	Process::GetPortFlags		Returns the port flags for a specific input port.
	//
	//# \proto	virtual unsigned_int32 GetPortFlags(int32 index) const;
	//
	//# \param	index	The port index.
	//
	//# \desc
	//# The $GetPortFlags$ function returns the flags for the input port having the index specified
	//# by the $index$ parameter. The flags can be a combination (through logical OR) of the
	//# following constants.
	//
	//# \table	ProcessPortFlags
	//
	//# \also	$@Process::GetPortRoute@$
	//# \also	$@Process::GetPortCount@$


	class Process : public GraphElement<Process, Route>, public ListElement<Process>, public Packable, public Configurable, public Registrable<Process, ProcessRegistration>, public EngineMemory<Process>
	{
		friend class ShaderAttribute;

		private:

			ProcessType				processType;
			ProcessType				baseProcessType;

			unsigned_int32			processFlags;

			union
			{
				mutable int32				processIndex;
				mutable Process				*cloneProcess;
				mutable const ProcessData	*processData;
			};

			Point2D					processPosition;
			String<>				processComment;

			int16					readyCount;
			int16					pathLength;

			#if C4PS3 //[ PS3

				// -- PS3 code hidden --

			#endif //]

			virtual Process *Replicate(void) const = 0;

		protected:

			Process(ProcessType type);
			Process(const Process& process);

			void SetBaseProcessType(ProcessType type)
			{
				baseProcessType = type;
			}

			static int32 PregenerateOutputIdentifier(const SwizzleData *swizzleData, char *name);
			static int32 PostgenerateOutputIdentifier(const SwizzleData *swizzleData, char *name);

		public:

			virtual ~Process();

			using ListElement<Process>::Previous;
			using ListElement<Process>::Next;

			ProcessType GetProcessType(void) const
			{
				return (processType);
			}

			ProcessType GetBaseProcessType(void) const
			{
				return (baseProcessType);
			}

			unsigned_int32 GetProcessFlags(void) const
			{
				return (processFlags);
			}

			void SetProcessFlags(unsigned_int32 flags)
			{
				processFlags = flags;
			}

			int32 GetProcessIndex(void) const
			{
				return (processIndex);
			}

			Process *GetCloneProcess(void) const
			{
				return (cloneProcess);
			}

			void SetCloneProcess(Process *process)
			{
				cloneProcess = process;
			}

			const ProcessData *GetProcessData(void) const
			{
				return (processData);
			}

			const Point2D& GetProcessPosition(void) const
			{
				return (processPosition);
			}

			void SetProcessPosition(const Point2D& position)
			{
				processPosition = position;
			}

			const char *GetProcessComment(void) const
			{
				return (processComment);
			}

			void SetProcessComment(const char *comment)
			{
				processComment = comment;
			}

			Process *Clone(void) const
			{
				return (Replicate());
			}

			C4API static Process *New(ProcessType type);

			static void RegisterStandardProcesses(void);

			void PackType(Packer& data) const override;
			void Pack(Packer& data, unsigned_int32 packFlags) const override;
			void Unpack(Unpacker& data, unsigned_int32 unpackFlags) override;
			bool UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags);
			void *BeginSettingsUnpack(void) override;

			int32 GetSettingCount(void) const override;
			Setting *GetSetting(int32 index) const override;
			void SetSetting(const Setting *setting) override;

			virtual bool operator ==(const Process& process) const;

			Route *GetPortRoute(int32 port) const;
			virtual int32 GetPortCount(void) const;
			virtual int32 GetInternalPortCount(void) const;
			virtual unsigned_int32 GetPortFlags(int32 index) const;
			virtual const char *GetPortName(int32 index) const;

			#if C4PS3 //[ PS3

				// -- PS3 code hidden --

			#endif //]

			virtual void ReferenceStateParams(const Process *process);
			virtual void Preschedule(const ShaderCompileData *compileData);

			virtual int32 GenerateProcessSignature(const ShaderCompileData *compileData, unsigned_int32 *signature) const;
			virtual int32 GenerateDerivedInterpolantTypes(const ShaderCompileData *compileData, ProcessType *type) const;
			virtual void GenerateProcessData(const ShaderCompileData *compileData, ProcessData *data) const;
			virtual int32 GenerateOutputIdentifier(const ShaderCompileData *compileData, const ShaderAllocationData *allocData, SwizzleData *swizzleData, char *name) const;
			virtual int32 GenerateShaderCode(const ShaderCompileData *compileData, const char **shaderCode) const;
	};


	class SectionProcess final : public Process
	{
		private:

			float		sectionWidth;
			float		sectionHeight;

			ColorRGBA	sectionColor;

			SectionProcess(const SectionProcess& sectionProcess);

			Process *Replicate(void) const override;

		public:

			C4API SectionProcess();
			C4API ~SectionProcess();

			float GetSectionWidth(void) const
			{
				return (sectionWidth);
			}

			float GetSectionHeight(void) const
			{
				return (sectionHeight);
			}

			void SetSectionSize(float width, float height)
			{
				sectionWidth = width;
				sectionHeight = height;
			}

			const ColorRGBA& GetSectionColor(void) const
			{
				return (sectionColor);
			}

			void SetSectionColor(const ColorRGBA& color)
			{
				sectionColor = color;
			}

			void Pack(Packer& data, unsigned_int32 packFlags) const override;
			void Unpack(Unpacker& data, unsigned_int32 unpackFlags) override;
			bool UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags);

			int32 GetSettingCount(void) const override;
			Setting *GetSetting(int32 index) const override;
			void SetSetting(const Setting *setting) override;
	};


	class ConstantProcess : public Process
	{
		private:

			int32			parameterSlot;

		protected:

			const float		*parameterData;

			static const char *const constantIdentifier[kMaxShaderConstantCount];

			static ShaderStateProc *const scalarStateProc[kMaxShaderConstantCount];
			static ShaderStateProc *const vectorStateProc[kMaxShaderConstantCount];

			ConstantProcess(ProcessType type);
			ConstantProcess(const ConstantProcess& constantProcess);

			static void StateProc_LoadScalar0(const Renderable *renderable, const void *cookie);
			static void StateProc_LoadScalar1(const Renderable *renderable, const void *cookie);
			static void StateProc_LoadScalar2(const Renderable *renderable, const void *cookie);
			static void StateProc_LoadScalar3(const Renderable *renderable, const void *cookie);
			static void StateProc_LoadScalar4(const Renderable *renderable, const void *cookie);
			static void StateProc_LoadScalar5(const Renderable *renderable, const void *cookie);
			static void StateProc_LoadScalar6(const Renderable *renderable, const void *cookie);
			static void StateProc_LoadScalar7(const Renderable *renderable, const void *cookie);

			static void StateProc_LoadVector0(const Renderable *renderable, const void *cookie);
			static void StateProc_LoadVector1(const Renderable *renderable, const void *cookie);
			static void StateProc_LoadVector2(const Renderable *renderable, const void *cookie);
			static void StateProc_LoadVector3(const Renderable *renderable, const void *cookie);
			static void StateProc_LoadVector4(const Renderable *renderable, const void *cookie);
			static void StateProc_LoadVector5(const Renderable *renderable, const void *cookie);
			static void StateProc_LoadVector6(const Renderable *renderable, const void *cookie);
			static void StateProc_LoadVector7(const Renderable *renderable, const void *cookie);

		public:

			~ConstantProcess();

			int32 GetParameterSlot(void) const
			{
				return (parameterSlot);
			}

			void SetParameterSlot(int32 slot)
			{
				parameterSlot = slot;
			}

			void SetParameterData(const float *data)
			{
				parameterData = data;
			}

			virtual void SetParameterValue(const Vector4D& param) = 0;

			void Pack(Packer& data, unsigned_int32 packFlags) const override;
			void Unpack(Unpacker& data, unsigned_int32 unpackFlags) override;

			int32 GetSettingCount(void) const override;
			Setting *GetSetting(int32 index) const override;
			void SetSetting(const Setting *setting) override;

			bool operator ==(const Process& process) const override;

			int32 GenerateProcessSignature(const ShaderCompileData *compileData, unsigned_int32 *signature) const override;
	};


	class ScalarProcess final : public ConstantProcess
	{
		private:

			float		scalarValue;

			ScalarProcess(const ScalarProcess& scalarProcess);

			Process *Replicate(void) const override;

		public:

			ScalarProcess();
			~ScalarProcess();

			float GetScalarValue(void) const
			{
				return (scalarValue);
			}

			void SetScalarValue(float value)
			{
				scalarValue = value;
			}

			void SetParameterValue(const Vector4D& param) override;

			void Pack(Packer& data, unsigned_int32 packFlags) const override;
			void Unpack(Unpacker& data, unsigned_int32 unpackFlags) override;

			int32 GetSettingCount(void) const override;
			Setting *GetSetting(int32 index) const override;
			void SetSetting(const Setting *setting) override;

			bool operator ==(const Process& process) const override;

			void ReferenceStateParams(const Process *process) override;

			int32 GenerateProcessSignature(const ShaderCompileData *compileData, unsigned_int32 *signature) const override;
			void GenerateProcessData(const ShaderCompileData *compileData, ProcessData *data) const override;
			int32 GenerateOutputIdentifier(const ShaderCompileData *compileData, const ShaderAllocationData *allocData, SwizzleData *swizzleData, char *name) const override;
	};


	class VectorProcess final : public ConstantProcess
	{
		private:

			Vector4D	vectorValue;

			VectorProcess(const VectorProcess& vectorProcess);

			Process *Replicate(void) const override;

		public:

			VectorProcess();
			~VectorProcess();

			const Vector4D& GetVectorValue(void) const
			{
				return (vectorValue);
			}

			void SetVectorValue(const Vector4D& value)
			{
				vectorValue = value;
			}

			void SetParameterValue(const Vector4D& param) override;

			void Pack(Packer& data, unsigned_int32 packFlags) const override;
			void Unpack(Unpacker& data, unsigned_int32 unpackFlags) override;

			int32 GetSettingCount(void) const override;
			Setting *GetSetting(int32 index) const override;
			void SetSetting(const Setting *setting) override;

			bool operator ==(const Process& process) const override;

			void ReferenceStateParams(const Process *process) override;

			int32 GenerateProcessSignature(const ShaderCompileData *compileData, unsigned_int32 *signature) const override;
			void GenerateProcessData(const ShaderCompileData *compileData, ProcessData *data) const override;
			int32 GenerateOutputIdentifier(const ShaderCompileData *compileData, const ShaderAllocationData *allocData, SwizzleData *swizzleData, char *name) const override;
	};


	class ColorProcess final : public ConstantProcess
	{
		private:

			ColorRGBA	colorValue;

			ColorProcess(const ColorProcess& colorProcess);

			Process *Replicate(void) const override;

		public:

			ColorProcess();
			~ColorProcess();

			const ColorRGBA& GetColorValue(void) const
			{
				return (colorValue);
			}

			void SetColorValue(const ColorRGBA& value)
			{
				colorValue = value;
			}

			void SetParameterValue(const Vector4D& param) override;

			void Pack(Packer& data, unsigned_int32 packFlags) const override;
			void Unpack(Unpacker& data, unsigned_int32 unpackFlags) override;

			int32 GetSettingCount(void) const override;
			Setting *GetSetting(int32 index) const override;
			void SetSetting(const Setting *setting) override;

			bool operator ==(const Process& process) const override;

			void ReferenceStateParams(const Process *process) override;

			int32 GenerateProcessSignature(const ShaderCompileData *compileData, unsigned_int32 *signature) const override;
			void GenerateProcessData(const ShaderCompileData *compileData, ProcessData *data) const override;
			int32 GenerateOutputIdentifier(const ShaderCompileData *compileData, const ShaderAllocationData *allocData, SwizzleData *swizzleData, char *name) const override;
	};


	class TimeProcess final : public Process
	{
		private:

			TimeProcess(const TimeProcess& timeProcess);

			Process *Replicate(void) const override;

		public:

			TimeProcess();
			~TimeProcess();

			void GenerateProcessData(const ShaderCompileData *compileData, ProcessData *data) const override;
			int32 GenerateOutputIdentifier(const ShaderCompileData *compileData, const ShaderAllocationData *allocData, SwizzleData *swizzleData, char *name) const override;
	};


	class DetailLevelProcess final : public Process
	{
		private:

			DetailLevelProcess(const DetailLevelProcess& detailLevelProcess);

			Process *Replicate(void) const override;

			static void StateProc_SetDetailLevelParam(const Renderable *renderable, const void *cookie);

		public:

			DetailLevelProcess();
			~DetailLevelProcess();

			void GenerateProcessData(const ShaderCompileData *compileData, ProcessData *data) const override;
			int32 GenerateOutputIdentifier(const ShaderCompileData *compileData, const ShaderAllocationData *allocData, SwizzleData *swizzleData, char *name) const override;
	};


	class TextureMapProcess : public Process, public ListElement<TextureMapProcess>
	{
		friend class ShaderAttribute;

		private:

			mutable Texture				*textureObject;
			ResourceName				textureName;

			mutable unsigned_int32		*signatureUnit;

			Process *Replicate(void) const override;

		protected:

			TextureMapProcess(ProcessType type);
			TextureMapProcess(const TextureMapProcess& textureMapProcess);

		public:

			TextureMapProcess();
			~TextureMapProcess();

			const ResourceName& GetTextureName(void) const
			{
				return (textureName);
			}

			void Pack(Packer& data, unsigned_int32 packFlags) const override;
			void Unpack(Unpacker& data, unsigned_int32 unpackFlags) override;
			void *BeginSettingsUnpack(void) override;

			int32 GetSettingCount(void) const override;
			Setting *GetSetting(int32 index) const override;
			void SetSetting(const Setting *setting) override;

			bool operator ==(const Process& process) const override;

			static int32 GetTexcoordSize(const Render::TextureObject *texture);

			C4API Texture *GetTexture(void) const;

			C4API void SetTexture(const char *name);
			C4API void SetTexture(Texture *texture);
			C4API void SetTexture(const TextureHeader *header, const void *image = nullptr);

			int32 GetPortCount(void) const override;
			unsigned_int32 GetPortFlags(int32 index) const override;
			const char *GetPortName(int32 index) const override;

			#if C4PS3 //[ PS3

				// -- PS3 code hidden --

			#endif //]

			int32 GenerateProcessSignature(const ShaderCompileData *compileData, unsigned_int32 *signature) const override;
			void GenerateProcessData(const ShaderCompileData *compileData, ProcessData *data) const override;
			int32 GenerateShaderCode(const ShaderCompileData *compileData, const char **shaderCode) const override;
	};


	class ProjectiveTextureMapProcess final : public TextureMapProcess
	{
		private:

			ProjectiveTextureMapProcess(const ProjectiveTextureMapProcess& projectiveTextureMapProcess);

			Process *Replicate(void) const override;

		public:

			ProjectiveTextureMapProcess();
			~ProjectiveTextureMapProcess();

			void GenerateProcessData(const ShaderCompileData *compileData, ProcessData *data) const override;
			int32 GenerateShaderCode(const ShaderCompileData *compileData, const char **shaderCode) const override;
	};


	class NormalMapProcess final : public TextureMapProcess
	{
		private:

			NormalMapProcess(const NormalMapProcess& normalMapProcess);

			Process *Replicate(void) const override;

		public:

			NormalMapProcess();
			~NormalMapProcess();

			int32 GenerateShaderCode(const ShaderCompileData *compileData, const char **shaderCode) const override;
	};


	class ImpostorTextureProcess : public TextureMapProcess
	{
		private:

			Texture		*screenTextureObject;

			Process *Replicate(void) const override;

		protected:

			ImpostorTextureProcess(ProcessType type);
			ImpostorTextureProcess(const ImpostorTextureProcess& impostorTextureProcess);

		public:

			ImpostorTextureProcess();
			~ImpostorTextureProcess();

			int32 GetPortCount(void) const override;

			int32 GenerateDerivedInterpolantTypes(const ShaderCompileData *compileData, ProcessType *type) const override;
			void GenerateProcessData(const ShaderCompileData *compileData, ProcessData *data) const override;
			int32 GenerateShaderCode(const ShaderCompileData *compileData, const char **shaderCode) const override;
	};


	class ImpostorNormalProcess final : public ImpostorTextureProcess
	{
		private:

			ImpostorNormalProcess(const ImpostorNormalProcess& impostorNormalProcess);

			Process *Replicate(void) const override;

		public:

			ImpostorNormalProcess();
			~ImpostorNormalProcess();

			int32 GenerateShaderCode(const ShaderCompileData *compileData, const char **shaderCode) const override;
	};


	class TerrainTextureProcess : public TextureMapProcess
	{
		private:

			Process *Replicate(void) const override;

		protected:

			enum
			{
				kTerrainBlendFull,
				kTerrainBlendPrimary,
				kTerrainBlendSecondary
			};

			int32		blendMode;

			TerrainTextureProcess(ProcessType type);
			TerrainTextureProcess(const TerrainTextureProcess& terrainTextureProcess);

		public:

			TerrainTextureProcess();
			~TerrainTextureProcess();

			void Pack(Packer& data, unsigned_int32 packFlags) const override;
			void Unpack(Unpacker& data, unsigned_int32 unpackFlags) override;

			int32 GetSettingCount(void) const override;
			Setting *GetSetting(int32 index) const override;
			void SetSetting(const Setting *setting) override;

			bool operator ==(const Process& process) const override;

			int32 GetPortCount(void) const override;

			int32 GenerateProcessSignature(const ShaderCompileData *compileData, unsigned_int32 *signature) const override;
			int32 GenerateDerivedInterpolantTypes(const ShaderCompileData *compileData, ProcessType *type) const override;
			void GenerateProcessData(const ShaderCompileData *compileData, ProcessData *data) const override;
			int32 GenerateShaderCode(const ShaderCompileData *compileData, const char **shaderCode) const override;
	};


	class TerrainNormalProcess final : public TerrainTextureProcess
	{
		private:

			TerrainNormalProcess(const TerrainNormalProcess& terrainNormalProcess);

			Process *Replicate(void) const override;

		public:

			TerrainNormalProcess();
			~TerrainNormalProcess();

			void GenerateProcessData(const ShaderCompileData *compileData, ProcessData *data) const override;
			int32 GenerateShaderCode(const ShaderCompileData *compileData, const char **shaderCode) const override;
	};


	class TerrainNormal2Process final : public TerrainTextureProcess
	{
		private:

			TerrainNormal2Process(const TerrainNormal2Process& terrainNormal2Process);

			Process *Replicate(void) const override;

		public:

			TerrainNormal2Process();
			~TerrainNormal2Process();

			void GenerateProcessData(const ShaderCompileData *compileData, ProcessData *data) const override;
			int32 GenerateShaderCode(const ShaderCompileData *compileData, const char **shaderCode) const override;
	};


	class TerrainNormal3Process final : public TerrainTextureProcess
	{
		private:

			TerrainNormal3Process(const TerrainNormal3Process& terrainNormal3Process);

			Process *Replicate(void) const override;

		public:

			TerrainNormal3Process();
			~TerrainNormal3Process();

			void GenerateProcessData(const ShaderCompileData *compileData, ProcessData *data) const override;
			int32 GenerateShaderCode(const ShaderCompileData *compileData, const char **shaderCode) const override;
	};


	class Merge2Process final : public Process
	{
		private:

			Merge2Process(const Merge2Process& merge2Process);

			Process *Replicate(void) const override;

		public:

			Merge2Process();
			~Merge2Process();

			int32 GetPortCount(void) const override;
			const char *GetPortName(int32 index) const override;

			void GenerateProcessData(const ShaderCompileData *compileData, ProcessData *data) const override;
			int32 GenerateShaderCode(const ShaderCompileData *compileData, const char **shaderCode) const override;
	};


	class Merge3Process final : public Process
	{
		private:

			Merge3Process(const Merge3Process& merge3Process);

			Process *Replicate(void) const override;

		public:

			Merge3Process();
			~Merge3Process();

			int32 GetPortCount(void) const override;
			const char *GetPortName(int32 index) const override;

			void GenerateProcessData(const ShaderCompileData *compileData, ProcessData *data) const override;
			int32 GenerateShaderCode(const ShaderCompileData *compileData, const char **shaderCode) const override;
	};


	class Merge4Process final : public Process
	{
		private:

			Merge4Process(const Merge4Process& merge4Process);

			Process *Replicate(void) const override;

		public:

			Merge4Process();
			~Merge4Process();

			int32 GetPortCount(void) const override;
			const char *GetPortName(int32 index) const override;

			void GenerateProcessData(const ShaderCompileData *compileData, ProcessData *data) const override;
			int32 GenerateShaderCode(const ShaderCompileData *compileData, const char **shaderCode) const override;
	};


	class VertexColorProcess final : public Process
	{
		private:

			VertexColorProcess(const VertexColorProcess& vertexColorProcess);

			Process *Replicate(void) const override;

		public:

			VertexColorProcess();
			~VertexColorProcess();

			void GenerateProcessData(const ShaderCompileData *compileData, ProcessData *data) const override;
			int32 GenerateOutputIdentifier(const ShaderCompileData *compileData, const ShaderAllocationData *allocData, SwizzleData *swizzleData, char *name) const override;
	};


	class FragmentPositionProcess final : public Process
	{
		private:

			FragmentPositionProcess(const FragmentPositionProcess& fragmentPositionProcess);

			Process *Replicate(void) const override;

		public:

			FragmentPositionProcess();
			~FragmentPositionProcess();

			void GenerateProcessData(const ShaderCompileData *compileData, ProcessData *data) const override;
			int32 GenerateOutputIdentifier(const ShaderCompileData *compileData, const ShaderAllocationData *allocData, SwizzleData *swizzleData, char *name) const override;
	};


	class FragmentDepthProcess final : public Process
	{
		private:

			FragmentDepthProcess(const FragmentDepthProcess& fragmentDepthProcess);

			Process *Replicate(void) const override;

		public:

			FragmentDepthProcess();
			~FragmentDepthProcess();

			void GenerateProcessData(const ShaderCompileData *compileData, ProcessData *data) const override;
			int32 GenerateShaderCode(const ShaderCompileData *compileData, const char **shaderCode) const override;
	};


	class PrimitiveFacingProcess final : public Process
	{
		private:

			PrimitiveFacingProcess(const PrimitiveFacingProcess& primitiveFacingProcess);

			Process *Replicate(void) const override;

		public:

			PrimitiveFacingProcess();
			~PrimitiveFacingProcess();

			void GenerateProcessData(const ShaderCompileData *compileData, ProcessData *data) const override;
			int32 GenerateShaderCode(const ShaderCompileData *compileData, const char **shaderCode) const override;
	};


	class InterpolantProcess : public Process
	{
		protected:

			InterpolantProcess(ProcessType type);
			InterpolantProcess(const InterpolantProcess& interpolantProcess);

		public:

			~InterpolantProcess();

			static int32 GetInterpolantSize(Type type);
			static int32 GetInterpolantName(Type type, const ShaderAllocationData *allocData, char *name, SwizzleData *swizzleData = nullptr);

			void GenerateProcessData(const ShaderCompileData *compileData, ProcessData *data) const override;
			int32 GenerateOutputIdentifier(const ShaderCompileData *compileData, const ShaderAllocationData *allocData, SwizzleData *swizzleData, char *name) const override;
	};


	class Texcoord0Process final : public InterpolantProcess
	{
		private:

			Texcoord0Process(const Texcoord0Process& texcoord0Process);

			Process *Replicate(void) const override;

		public:

			Texcoord0Process();
			~Texcoord0Process();

			#if C4OPENGL

				void GenerateProcessData(const ShaderCompileData *compileData, ProcessData *data) const override;
				int32 GenerateOutputIdentifier(const ShaderCompileData *compileData, const ShaderAllocationData *allocData, SwizzleData *swizzleData, char *name) const override;

			#endif
	};


	class Texcoord1Process final : public InterpolantProcess
	{
		private:

			Texcoord1Process(const Texcoord1Process& texcoord1Process);

			Process *Replicate(void) const override;

		public:

			Texcoord1Process();
			~Texcoord1Process();
	};


	class RawTexcoordProcess final : public InterpolantProcess
	{
		private:

			RawTexcoordProcess(const RawTexcoordProcess& rawTexcoordProcess);

			Process *Replicate(void) const override;

		public:

			RawTexcoordProcess();
			~RawTexcoordProcess();
	};


	class ImpostorTexcoordProcess final : public InterpolantProcess
	{
		private:

			ImpostorTexcoordProcess(const ImpostorTexcoordProcess& impostorTexcoordProcess);

			Process *Replicate(void) const override;

		public:

			ImpostorTexcoordProcess();
			~ImpostorTexcoordProcess();
	};


	class ImpostorBlendProcess final : public InterpolantProcess
	{
		private:

			Texture		*textureObject;

			ImpostorBlendProcess(const ImpostorBlendProcess& impostorBlendProcess);

			Process *Replicate(void) const override;

		public:

			ImpostorBlendProcess();
			~ImpostorBlendProcess();

			void GenerateProcessData(const ShaderCompileData *compileData, ProcessData *data) const override;
			int32 GenerateShaderCode(const ShaderCompileData *compileData, const char **shaderCode) const override;
	};


	class TerrainTexcoordProcess final : public InterpolantProcess
	{
		private:

			TerrainTexcoordProcess(const TerrainTexcoordProcess& terrainTexcoordProcess);

			Process *Replicate(void) const override;

		public:

			TerrainTexcoordProcess();
			~TerrainTexcoordProcess();

			void GenerateProcessData(const ShaderCompileData *compileData, ProcessData *data) const override;
			int32 GenerateShaderCode(const ShaderCompileData *compileData, const char **shaderCode) const override;
	};


	class TriplanarBlendProcess final : public InterpolantProcess
	{
		private:

			TriplanarBlendProcess(const TriplanarBlendProcess& triplanarBlendProcess);

			Process *Replicate(void) const override;

		public:

			TriplanarBlendProcess();
			~TriplanarBlendProcess();

			void GenerateProcessData(const ShaderCompileData *compileData, ProcessData *data) const override;
			int32 GenerateShaderCode(const ShaderCompileData *compileData, const char **shaderCode) const override;
	};


	class VertexGeometryProcess final : public InterpolantProcess
	{
		private:

			VertexGeometryProcess(const VertexGeometryProcess& vertexGeometryProcess);

			Process *Replicate(void) const override;

		public:

			VertexGeometryProcess();
			~VertexGeometryProcess();
	};


	class ObjectPositionProcess final : public InterpolantProcess
	{
		private:

			ObjectPositionProcess(const ObjectPositionProcess& objectPositionProcess);

			Process *Replicate(void) const override;

		public:

			ObjectPositionProcess();
			~ObjectPositionProcess();
	};


	class WorldPositionProcess final : public InterpolantProcess
	{
		private:

			WorldPositionProcess(const WorldPositionProcess& worldPositionProcess);

			Process *Replicate(void) const override;

		public:

			WorldPositionProcess();
			~WorldPositionProcess();
	};


	class ObjectNormalProcess final : public InterpolantProcess
	{
		private:

			ObjectNormalProcess(const ObjectNormalProcess& objectNormalProcess);

			Process *Replicate(void) const override;

		public:

			ObjectNormalProcess();
			~ObjectNormalProcess();

			int32 GenerateOutputIdentifier(const ShaderCompileData *compileData, const ShaderAllocationData *allocData, SwizzleData *swizzleData, char *name) const override;
			int32 GenerateShaderCode(const ShaderCompileData *compileData, const char **shaderCode) const override;
	};


	class ObjectTangentProcess final : public InterpolantProcess
	{
		private:

			ObjectTangentProcess(const ObjectTangentProcess& objectTangentProcess);

			Process *Replicate(void) const override;

		public:

			ObjectTangentProcess();
			~ObjectTangentProcess();

			int32 GenerateOutputIdentifier(const ShaderCompileData *compileData, const ShaderAllocationData *allocData, SwizzleData *swizzleData, char *name) const override;
			int32 GenerateShaderCode(const ShaderCompileData *compileData, const char **shaderCode) const override;
	};


	class ObjectBitangentProcess final : public InterpolantProcess
	{
		private:

			ObjectBitangentProcess(const ObjectBitangentProcess& objectBitangentProcess);

			Process *Replicate(void) const override;

		public:

			ObjectBitangentProcess();
			~ObjectBitangentProcess();

			int32 GenerateOutputIdentifier(const ShaderCompileData *compileData, const ShaderAllocationData *allocData, SwizzleData *swizzleData, char *name) const override;
			int32 GenerateShaderCode(const ShaderCompileData *compileData, const char **shaderCode) const override;
	};


	class WorldNormalProcess final : public InterpolantProcess
	{
		private:

			WorldNormalProcess(const WorldNormalProcess& worldNormalProcess);

			Process *Replicate(void) const override;

		public:

			WorldNormalProcess();
			~WorldNormalProcess();

			int32 GenerateOutputIdentifier(const ShaderCompileData *compileData, const ShaderAllocationData *allocData, SwizzleData *swizzleData, char *name) const override;
			int32 GenerateShaderCode(const ShaderCompileData *compileData, const char **shaderCode) const override;
	};


	class WorldTangentProcess final : public InterpolantProcess
	{
		private:

			WorldTangentProcess(const WorldTangentProcess& worldTangentProcess);

			Process *Replicate(void) const override;

		public:

			WorldTangentProcess();
			~WorldTangentProcess();

			int32 GenerateOutputIdentifier(const ShaderCompileData *compileData, const ShaderAllocationData *allocData, SwizzleData *swizzleData, char *name) const override;
			int32 GenerateShaderCode(const ShaderCompileData *compileData, const char **shaderCode) const override;
	};


	class WorldBitangentProcess final : public InterpolantProcess
	{
		private:

			WorldBitangentProcess(const WorldBitangentProcess& worldBitangentProcess);

			Process *Replicate(void) const override;

		public:

			WorldBitangentProcess();
			~WorldBitangentProcess();

			int32 GenerateOutputIdentifier(const ShaderCompileData *compileData, const ShaderAllocationData *allocData, SwizzleData *swizzleData, char *name) const override;
			int32 GenerateShaderCode(const ShaderCompileData *compileData, const char **shaderCode) const override;
	};


	class TangentLightDirectionProcess final : public InterpolantProcess
	{
		private:

			TangentLightDirectionProcess(const TangentLightDirectionProcess& tangentLightDirectionProcess);

			Process *Replicate(void) const override;

		public:

			TangentLightDirectionProcess();
			~TangentLightDirectionProcess();

			int32 GenerateOutputIdentifier(const ShaderCompileData *compileData, const ShaderAllocationData *allocData, SwizzleData *swizzleData, char *name) const override;
			int32 GenerateShaderCode(const ShaderCompileData *compileData, const char **shaderCode) const override;
	};


	class TangentViewDirectionProcess final : public InterpolantProcess
	{
		private:

			TangentViewDirectionProcess(const TangentViewDirectionProcess& tangentViewDirectionProcess);

			Process *Replicate(void) const override;

		public:

			TangentViewDirectionProcess();
			~TangentViewDirectionProcess();

			int32 GenerateDerivedInterpolantTypes(const ShaderCompileData *compileData, ProcessType *type) const override;
			int32 GenerateOutputIdentifier(const ShaderCompileData *compileData, const ShaderAllocationData *allocData, SwizzleData *swizzleData, char *name) const override;
			int32 GenerateShaderCode(const ShaderCompileData *compileData, const char **shaderCode) const override;
	};


	class TangentHalfwayDirectionProcess final : public InterpolantProcess
	{
		private:

			TangentHalfwayDirectionProcess(const TangentHalfwayDirectionProcess& tangentHalfwayDirectionProcess);

			Process *Replicate(void) const override;

		public:

			TangentHalfwayDirectionProcess();
			~TangentHalfwayDirectionProcess();

			int32 GenerateDerivedInterpolantTypes(const ShaderCompileData *compileData, ProcessType *type) const override;
			void GenerateProcessData(const ShaderCompileData *compileData, ProcessData *data) const override;
			int32 GenerateOutputIdentifier(const ShaderCompileData *compileData, const ShaderAllocationData *allocData, SwizzleData *swizzleData, char *name) const override;
			int32 GenerateShaderCode(const ShaderCompileData *compileData, const char **shaderCode) const override;
	};


	class ObjectLightDirectionProcess final : public InterpolantProcess
	{
		private:

			ObjectLightDirectionProcess(const ObjectLightDirectionProcess& objectLightDirectionProcess);

			Process *Replicate(void) const override;

		public:

			ObjectLightDirectionProcess();
			~ObjectLightDirectionProcess();

			int32 GenerateOutputIdentifier(const ShaderCompileData *compileData, const ShaderAllocationData *allocData, SwizzleData *swizzleData, char *name) const override;
			int32 GenerateShaderCode(const ShaderCompileData *compileData, const char **shaderCode) const override;
	};


	class ObjectViewDirectionProcess final : public InterpolantProcess
	{
		private:

			ObjectViewDirectionProcess(const ObjectViewDirectionProcess& objectViewDirectionProcess);

			Process *Replicate(void) const override;

		public:

			ObjectViewDirectionProcess();
			~ObjectViewDirectionProcess();

			int32 GenerateOutputIdentifier(const ShaderCompileData *compileData, const ShaderAllocationData *allocData, SwizzleData *swizzleData, char *name) const override;
			int32 GenerateShaderCode(const ShaderCompileData *compileData, const char **shaderCode) const override;
	};


	class ObjectHalfwayDirectionProcess final : public InterpolantProcess
	{
		private:

			ObjectHalfwayDirectionProcess(const ObjectHalfwayDirectionProcess& objectHalfwayDirectionProcess);

			Process *Replicate(void) const override;

		public:

			ObjectHalfwayDirectionProcess();
			~ObjectHalfwayDirectionProcess();

			int32 GenerateDerivedInterpolantTypes(const ShaderCompileData *compileData, ProcessType *type) const override;
			void GenerateProcessData(const ShaderCompileData *compileData, ProcessData *data) const override;
			int32 GenerateOutputIdentifier(const ShaderCompileData *compileData, const ShaderAllocationData *allocData, SwizzleData *swizzleData, char *name) const override;
			int32 GenerateShaderCode(const ShaderCompileData *compileData, const char **shaderCode) const override;
	};


	class TerrainLightDirectionProcess final : public InterpolantProcess
	{
		private:

			TerrainLightDirectionProcess(const TerrainLightDirectionProcess& terrainLightDirectionProcess);

			Process *Replicate(void) const override;

		public:

			TerrainLightDirectionProcess();
			~TerrainLightDirectionProcess();

			void GenerateProcessData(const ShaderCompileData *compileData, ProcessData *data) const override;
			int32 GenerateShaderCode(const ShaderCompileData *compileData, const char **shaderCode) const override;
	};


	class TerrainViewDirectionProcess final : public InterpolantProcess
	{
		private:

			TerrainViewDirectionProcess(const TerrainViewDirectionProcess& terrainViewDirectionProcess);

			Process *Replicate(void) const override;

		public:

			TerrainViewDirectionProcess();
			~TerrainViewDirectionProcess();

			int32 GenerateDerivedInterpolantTypes(const ShaderCompileData *compileData, ProcessType *type) const override;
			void GenerateProcessData(const ShaderCompileData *compileData, ProcessData *data) const override;
			int32 GenerateShaderCode(const ShaderCompileData *compileData, const char **shaderCode) const override;
	};


	class TerrainHalfwayDirectionProcess final : public InterpolantProcess
	{
		private:

			TerrainHalfwayDirectionProcess(const TerrainHalfwayDirectionProcess& terrainHalfwayDirectionProcess);

			Process *Replicate(void) const override;

		public:

			TerrainHalfwayDirectionProcess();
			~TerrainHalfwayDirectionProcess();

			int32 GenerateDerivedInterpolantTypes(const ShaderCompileData *compileData, ProcessType *type) const override;
			void GenerateProcessData(const ShaderCompileData *compileData, ProcessData *data) const override;
			int32 GenerateShaderCode(const ShaderCompileData *compileData, const char **shaderCode) const override;
	};


	class TangentLightVectorProcess final : public InterpolantProcess
	{
		private:

			TangentLightVectorProcess(const TangentLightVectorProcess& tangentLightVectorProcess);

			Process *Replicate(void) const override;

		public:

			TangentLightVectorProcess();
			~TangentLightVectorProcess();

			void GenerateProcessData(const ShaderCompileData *compileData, ProcessData *data) const override;
			int32 GenerateOutputIdentifier(const ShaderCompileData *compileData, const ShaderAllocationData *allocData, SwizzleData *swizzleData, char *name) const override;
	};


	class TangentViewVectorProcess final : public InterpolantProcess
	{
		private:

			TangentViewVectorProcess(const TangentViewVectorProcess& tangentViewVectorProcess);

			Process *Replicate(void) const override;

		public:

			TangentViewVectorProcess();
			~TangentViewVectorProcess();

			int32 GenerateDerivedInterpolantTypes(const ShaderCompileData *compileData, ProcessType *type) const override;
			void GenerateProcessData(const ShaderCompileData *compileData, ProcessData *data) const override;
			int32 GenerateOutputIdentifier(const ShaderCompileData *compileData, const ShaderAllocationData *allocData, SwizzleData *swizzleData, char *name) const override;
	};


	class ObjectLightVectorProcess final : public InterpolantProcess
	{
		private:

			ObjectLightVectorProcess(const ObjectLightVectorProcess& objectLightVectorProcess);

			Process *Replicate(void) const override;

		public:

			ObjectLightVectorProcess();
			~ObjectLightVectorProcess();

			void GenerateProcessData(const ShaderCompileData *compileData, ProcessData *data) const override;
			int32 GenerateOutputIdentifier(const ShaderCompileData *compileData, const ShaderAllocationData *allocData, SwizzleData *swizzleData, char *name) const override;
	};


	class ObjectViewVectorProcess final : public InterpolantProcess
	{
		private:

			ObjectViewVectorProcess(const ObjectViewVectorProcess& objectViewVectorProcess);

			Process *Replicate(void) const override;

		public:

			ObjectViewVectorProcess();
			~ObjectViewVectorProcess();

			void GenerateProcessData(const ShaderCompileData *compileData, ProcessData *data) const override;
			int32 GenerateOutputIdentifier(const ShaderCompileData *compileData, const ShaderAllocationData *allocData, SwizzleData *swizzleData, char *name) const override;
	};


	class UnaryProcess : public Process
	{
		protected:

			UnaryProcess(ProcessType type);
			UnaryProcess(const UnaryProcess& unaryProcess);

		public:

			~UnaryProcess();

			int32 GetPortCount(void) const override;
			const char *GetPortName(int32 index) const override;

			void GenerateProcessData(const ShaderCompileData *compileData, ProcessData *data) const override;
	};


	class BinaryProcess : public Process
	{
		protected:

			BinaryProcess(ProcessType type);
			BinaryProcess(const BinaryProcess& binaryProcess);

		public:

			~BinaryProcess();

			int32 GetPortCount(void) const override;
			const char *GetPortName(int32 index) const override;

			void GenerateProcessData(const ShaderCompileData *compileData, ProcessData *data) const override;
			int32 GenerateOutputIdentifier(const ShaderCompileData *compileData, const ShaderAllocationData *allocData, SwizzleData *swizzleData, char *name) const override;
	};


	class TrinaryProcess : public Process
	{
		protected:

			TrinaryProcess(ProcessType type);
			TrinaryProcess(const TrinaryProcess& trinaryProcess);

		public:

			~TrinaryProcess();

			int32 GetPortCount(void) const override;

			void GenerateProcessData(const ShaderCompileData *compileData, ProcessData *data) const override;
			int32 GenerateOutputIdentifier(const ShaderCompileData *compileData, const ShaderAllocationData *allocData, SwizzleData *swizzleData, char *name) const override;
	};


	class AbsoluteProcess final : public UnaryProcess
	{
		private:

			AbsoluteProcess(const AbsoluteProcess& absoluteProcess);

			Process *Replicate(void) const override;

		public:

			AbsoluteProcess();
			~AbsoluteProcess();

			int32 GenerateShaderCode(const ShaderCompileData *compileData, const char **shaderCode) const override;
	};


	class InvertProcess final : public UnaryProcess
	{
		private:

			InvertProcess(const InvertProcess& invertProcess);

			Process *Replicate(void) const override;

		public:

			InvertProcess();
			~InvertProcess();

			int32 GenerateShaderCode(const ShaderCompileData *compileData, const char **shaderCode) const override;
	};


	class ExpandProcess final : public UnaryProcess
	{
		private:

			ExpandProcess(const ExpandProcess& expandProcess);

			Process *Replicate(void) const override;

		public:

			ExpandProcess();
			~ExpandProcess();

			int32 GenerateShaderCode(const ShaderCompileData *compileData, const char **shaderCode) const override;
	};


	class ReciprocalProcess final : public UnaryProcess
	{
		private:

			ReciprocalProcess(const ReciprocalProcess& reciprocalProcess);

			Process *Replicate(void) const override;

		public:

			ReciprocalProcess();
			~ReciprocalProcess();

			void GenerateProcessData(const ShaderCompileData *compileData, ProcessData *data) const override;
			int32 GenerateShaderCode(const ShaderCompileData *compileData, const char **shaderCode) const override;
	};


	class ReciprocalSquareRootProcess final : public UnaryProcess
	{
		private:

			ReciprocalSquareRootProcess(const ReciprocalSquareRootProcess& reciprocalSquareRootProcess);

			Process *Replicate(void) const override;

		public:

			ReciprocalSquareRootProcess();
			~ReciprocalSquareRootProcess();

			void GenerateProcessData(const ShaderCompileData *compileData, ProcessData *data) const override;
			int32 GenerateShaderCode(const ShaderCompileData *compileData, const char **shaderCode) const override;
	};


	class SquareRootProcess final : public UnaryProcess
	{
		private:

			SquareRootProcess(const SquareRootProcess& squareRootProcess);

			Process *Replicate(void) const override;

		public:

			SquareRootProcess();
			~SquareRootProcess();

			void GenerateProcessData(const ShaderCompileData *compileData, ProcessData *data) const override;
			int32 GenerateShaderCode(const ShaderCompileData *compileData, const char **shaderCode) const override;
	};


	class MagnitudeProcess final : public UnaryProcess
	{
		private:

			MagnitudeProcess(const MagnitudeProcess& magnitudeProcess);

			Process *Replicate(void) const override;

		public:

			MagnitudeProcess();
			~MagnitudeProcess();

			void GenerateProcessData(const ShaderCompileData *compileData, ProcessData *data) const override;
			int32 GenerateShaderCode(const ShaderCompileData *compileData, const char **shaderCode) const override;
	};


	class NormalizeProcess final : public UnaryProcess
	{
		private:

			NormalizeProcess(const NormalizeProcess& normalizeProcess);

			Process *Replicate(void) const override;

		public:

			NormalizeProcess();
			~NormalizeProcess();

			void GenerateProcessData(const ShaderCompileData *compileData, ProcessData *data) const override;
			int32 GenerateShaderCode(const ShaderCompileData *compileData, const char **shaderCode) const override;
	};


	class FloorProcess final : public UnaryProcess
	{
		private:

			FloorProcess(const FloorProcess& floorProcess);

			Process *Replicate(void) const override;

		public:

			FloorProcess();
			~FloorProcess();

			int32 GenerateShaderCode(const ShaderCompileData *compileData, const char **shaderCode) const override;
	};


	class RoundProcess final : public UnaryProcess
	{
		private:

			RoundProcess(const RoundProcess& roundProcess);

			Process *Replicate(void) const override;

		public:

			RoundProcess();
			~RoundProcess();

			int32 GenerateShaderCode(const ShaderCompileData *compileData, const char **shaderCode) const override;
	};


	class FractionProcess final : public UnaryProcess
	{
		private:

			FractionProcess(const FractionProcess& fractionProcess);

			Process *Replicate(void) const override;

		public:

			FractionProcess();
			~FractionProcess();

			int32 GenerateShaderCode(const ShaderCompileData *compileData, const char **shaderCode) const override;
	};


	class SaturateProcess final : public UnaryProcess
	{
		private:

			SaturateProcess(const SaturateProcess& saturateProcess);

			Process *Replicate(void) const override;

		public:

			SaturateProcess();
			~SaturateProcess();

			int32 GenerateShaderCode(const ShaderCompileData *compileData, const char **shaderCode) const override;
	};


	class SineProcess final : public UnaryProcess
	{
		private:

			SineProcess(const SineProcess& sineProcess);

			Process *Replicate(void) const override;

		public:

			SineProcess();
			~SineProcess();

			void GenerateProcessData(const ShaderCompileData *compileData, ProcessData *data) const override;
			int32 GenerateShaderCode(const ShaderCompileData *compileData, const char **shaderCode) const override;
	};


	class CosineProcess final : public UnaryProcess
	{
		private:

			CosineProcess(const CosineProcess& cosineProcess);

			Process *Replicate(void) const override;

		public:

			CosineProcess();
			~CosineProcess();

			void GenerateProcessData(const ShaderCompileData *compileData, ProcessData *data) const override;
			int32 GenerateShaderCode(const ShaderCompileData *compileData, const char **shaderCode) const override;
	};


	class Exp2Process final : public UnaryProcess
	{
		private:

			Exp2Process(const Exp2Process& exp2Process);

			Process *Replicate(void) const override;

		public:

			Exp2Process();
			~Exp2Process();

			void GenerateProcessData(const ShaderCompileData *compileData, ProcessData *data) const override;
			int32 GenerateShaderCode(const ShaderCompileData *compileData, const char **shaderCode) const override;
	};


	class Log2Process final : public UnaryProcess
	{
		private:

			Log2Process(const Log2Process& log2Process);

			Process *Replicate(void) const override;

		public:

			Log2Process();
			~Log2Process();

			void GenerateProcessData(const ShaderCompileData *compileData, ProcessData *data) const override;
			int32 GenerateShaderCode(const ShaderCompileData *compileData, const char **shaderCode) const override;
	};


	class AddProcess final : public BinaryProcess
	{
		private:

			AddProcess(const AddProcess& addProcess);

			Process *Replicate(void) const override;

		public:

			AddProcess();
			~AddProcess();

			unsigned_int32 GetPortFlags(int32 index) const override;

			int32 GenerateShaderCode(const ShaderCompileData *compileData, const char **shaderCode) const override;
	};


	class SubtractProcess final : public BinaryProcess
	{
		private:

			SubtractProcess(const SubtractProcess& subtractProcess);

			Process *Replicate(void) const override;

		public:

			SubtractProcess();
			~SubtractProcess();

			unsigned_int32 GetPortFlags(int32 index) const override;

			int32 GenerateShaderCode(const ShaderCompileData *compileData, const char **shaderCode) const override;
	};


	class AverageProcess final : public BinaryProcess
	{
		private:

			AverageProcess(const AverageProcess& averageProcess);

			Process *Replicate(void) const override;

		public:

			AverageProcess();
			~AverageProcess();

			unsigned_int32 GetPortFlags(int32 index) const override;

			int32 GenerateShaderCode(const ShaderCompileData *compileData, const char **shaderCode) const override;
	};


	class MultiplyProcess final : public BinaryProcess
	{
		private:

			MultiplyProcess(const MultiplyProcess& multiplyProcess);

			Process *Replicate(void) const override;

		public:

			MultiplyProcess();
			~MultiplyProcess();

			unsigned_int32 GetPortFlags(int32 index) const override;

			int32 GenerateShaderCode(const ShaderCompileData *compileData, const char **shaderCode) const override;
	};


	class DivideProcess final : public BinaryProcess
	{
		private:

			DivideProcess(const DivideProcess& divideProcess);

			Process *Replicate(void) const override;

		public:

			DivideProcess();
			~DivideProcess();

			void GenerateProcessData(const ShaderCompileData *compileData, ProcessData *data) const override;
			int32 GenerateShaderCode(const ShaderCompileData *compileData, const char **shaderCode) const override;
	};


	class Dot3Process final : public BinaryProcess
	{
		private:

			Dot3Process(const Dot3Process& dot3Process);

			Process *Replicate(void) const override;

		public:

			Dot3Process();
			~Dot3Process();

			void GenerateProcessData(const ShaderCompileData *compileData, ProcessData *data) const override;
			int32 GenerateShaderCode(const ShaderCompileData *compileData, const char **shaderCode) const override;
	};


	class Dot4Process final : public BinaryProcess
	{
		private:

			Dot4Process(const Dot4Process& dot3Process);

			Process *Replicate(void) const override;

		public:

			Dot4Process();
			~Dot4Process();

			void GenerateProcessData(const ShaderCompileData *compileData, ProcessData *data) const override;
			int32 GenerateShaderCode(const ShaderCompileData *compileData, const char **shaderCode) const override;
	};


	class CrossProcess final : public BinaryProcess
	{
		private:

			CrossProcess(const CrossProcess& crossProcess);

			Process *Replicate(void) const override;

		public:

			CrossProcess();
			~CrossProcess();

			void GenerateProcessData(const ShaderCompileData *compileData, ProcessData *data) const override;
			int32 GenerateShaderCode(const ShaderCompileData *compileData, const char **shaderCode) const override;
	};


	class MinimumProcess final : public BinaryProcess
	{
		private:

			MinimumProcess(const MinimumProcess& minimumProcess);

			Process *Replicate(void) const override;

		public:

			MinimumProcess();
			~MinimumProcess();

			unsigned_int32 GetPortFlags(int32 index) const override;

			int32 GenerateShaderCode(const ShaderCompileData *compileData, const char **shaderCode) const override;
	};


	class MaximumProcess final : public BinaryProcess
	{
		private:

			MaximumProcess(const MaximumProcess& maximumProcess);

			Process *Replicate(void) const override;

		public:

			MaximumProcess();
			~MaximumProcess();

			unsigned_int32 GetPortFlags(int32 index) const override;

			int32 GenerateShaderCode(const ShaderCompileData *compileData, const char **shaderCode) const override;
	};


	class SetLessThanProcess final : public BinaryProcess
	{
		private:

			SetLessThanProcess(const SetLessThanProcess& setLessThanProcess);

			Process *Replicate(void) const override;

		public:

			SetLessThanProcess();
			~SetLessThanProcess();

			unsigned_int32 GetPortFlags(int32 index) const override;

			int32 GenerateShaderCode(const ShaderCompileData *compileData, const char **shaderCode) const override;
	};


	class SetGreaterThanProcess final : public BinaryProcess
	{
		private:

			SetGreaterThanProcess(const SetGreaterThanProcess& setGreaterThanProcess);

			Process *Replicate(void) const override;

		public:

			SetGreaterThanProcess();
			~SetGreaterThanProcess();

			unsigned_int32 GetPortFlags(int32 index) const override;

			int32 GenerateShaderCode(const ShaderCompileData *compileData, const char **shaderCode) const override;
	};


	class SetLessEqualProcess final : public BinaryProcess
	{
		private:

			SetLessEqualProcess(const SetLessEqualProcess& setLessEqualProcess);

			Process *Replicate(void) const override;

		public:

			SetLessEqualProcess();
			~SetLessEqualProcess();

			unsigned_int32 GetPortFlags(int32 index) const override;

			int32 GenerateShaderCode(const ShaderCompileData *compileData, const char **shaderCode) const override;
	};


	class SetGreaterEqualProcess final : public BinaryProcess
	{
		private:

			SetGreaterEqualProcess(const SetGreaterEqualProcess& setGreaterEqualProcess);

			Process *Replicate(void) const override;

		public:

			SetGreaterEqualProcess();
			~SetGreaterEqualProcess();

			unsigned_int32 GetPortFlags(int32 index) const override;

			int32 GenerateShaderCode(const ShaderCompileData *compileData, const char **shaderCode) const override;
	};


	class SetEqualProcess final : public BinaryProcess
	{
		private:

			SetEqualProcess(const SetEqualProcess& setEqualProcess);

			Process *Replicate(void) const override;

		public:

			SetEqualProcess();
			~SetEqualProcess();

			unsigned_int32 GetPortFlags(int32 index) const override;

			int32 GenerateShaderCode(const ShaderCompileData *compileData, const char **shaderCode) const override;
	};


	class SetNotEqualProcess final : public BinaryProcess
	{
		private:

			SetNotEqualProcess(const SetNotEqualProcess& setNotEqualProcess);

			Process *Replicate(void) const override;

		public:

			SetNotEqualProcess();
			~SetNotEqualProcess();

			unsigned_int32 GetPortFlags(int32 index) const override;

			int32 GenerateShaderCode(const ShaderCompileData *compileData, const char **shaderCode) const override;
	};


	class PowerProcess final : public BinaryProcess
	{
		private:

			PowerProcess(const PowerProcess& powerProcess);

			Process *Replicate(void) const override;

		public:

			PowerProcess();
			~PowerProcess();

			void GenerateProcessData(const ShaderCompileData *compileData, ProcessData *data) const override;
			int32 GenerateShaderCode(const ShaderCompileData *compileData, const char **shaderCode) const override;
	};


	class MultiplyAddProcess final : public TrinaryProcess
	{
		private:

			MultiplyAddProcess(const MultiplyAddProcess& multiplyAddProcess);

			Process *Replicate(void) const override;

		public:

			MultiplyAddProcess();
			~MultiplyAddProcess();

			unsigned_int32 GetPortFlags(int32 index) const override;
			const char *GetPortName(int32 index) const override;

			void GenerateProcessData(const ShaderCompileData *compileData, ProcessData *data) const override;
			int32 GenerateShaderCode(const ShaderCompileData *compileData, const char **shaderCode) const override;
	};


	class LerpProcess final : public TrinaryProcess
	{
		private:

			LerpProcess(const LerpProcess& lerpProcess);

			Process *Replicate(void) const override;

		public:

			LerpProcess();
			~LerpProcess();

			unsigned_int32 GetPortFlags(int32 index) const override;
			const char *GetPortName(int32 index) const override;

			void GenerateProcessData(const ShaderCompileData *compileData, ProcessData *data) const override;
			int32 GenerateShaderCode(const ShaderCompileData *compileData, const char **shaderCode) const override;
	};


	class DiffuseProcess final : public Process
	{
		private:

			DiffuseProcess(const DiffuseProcess& diffuseProcess);

			Process *Replicate(void) const override;

		public:

			DiffuseProcess();
			~DiffuseProcess();

			int32 GetPortCount(void) const override;
			unsigned_int32 GetPortFlags(int32 index) const override;
			const char *GetPortName(int32 index) const override;

			int32 GenerateProcessSignature(const ShaderCompileData *compileData, unsigned_int32 *signature) const override;
			int32 GenerateDerivedInterpolantTypes(const ShaderCompileData *compileData, ProcessType *type) const override;
			void GenerateProcessData(const ShaderCompileData *compileData, ProcessData *data) const override;
			int32 GenerateShaderCode(const ShaderCompileData *compileData, const char **shaderCode) const override;
	};


	class SpecularProcess final : public Process
	{
		private:

			SpecularProcess(const SpecularProcess& specularProcess);

			Process *Replicate(void) const override;

		public:

			SpecularProcess();
			~SpecularProcess();

			int32 GetPortCount(void) const override;
			unsigned_int32 GetPortFlags(int32 index) const override;
			const char *GetPortName(int32 index) const override;

			int32 GenerateProcessSignature(const ShaderCompileData *compileData, unsigned_int32 *signature) const override;
			int32 GenerateDerivedInterpolantTypes(const ShaderCompileData *compileData, ProcessType *type) const override;
			void GenerateProcessData(const ShaderCompileData *compileData, ProcessData *data) const override;
			int32 GenerateShaderCode(const ShaderCompileData *compileData, const char **shaderCode) const override;
	};


	class MicrofacetProcess final : public Process
	{
		private:

			MicrofacetAttribute::MicrofacetParams			microfacetParams;
			const MicrofacetAttribute::MicrofacetParams		*microfacetData;

			MicrofacetProcess(const MicrofacetProcess& microfacetProcess);

			Process *Replicate(void) const override;

		public:

			MicrofacetProcess();
			~MicrofacetProcess();

			void SetMicrofacetParams(const MicrofacetAttribute::MicrofacetParams *params)
			{
				microfacetParams.microfacetColor = params->microfacetColor;
				microfacetParams.microfacetSlope = params->microfacetSlope;
				microfacetParams.microfacetThreshold = params->microfacetThreshold;
			}

			void SetMicrofacetData(const MicrofacetAttribute::MicrofacetParams *data)
			{
				microfacetData = data;
			}

			void Pack(Packer& data, unsigned_int32 packFlags) const override;
			void Unpack(Unpacker& data, unsigned_int32 unpackFlags) override;

			int32 GetSettingCount(void) const override;
			Setting *GetSetting(int32 index) const override;
			void SetSetting(const Setting *setting) override;

			bool operator ==(const Process& process) const override;

			int32 GetPortCount(void) const override;
			unsigned_int32 GetPortFlags(int32 index) const override;
			const char *GetPortName(int32 index) const override;

			void ReferenceStateParams(const Process *process) override;

			int32 GenerateProcessSignature(const ShaderCompileData *compileData, unsigned_int32 *signature) const override;
			int32 GenerateDerivedInterpolantTypes(const ShaderCompileData *compileData, ProcessType *type) const override;
			void GenerateProcessData(const ShaderCompileData *compileData, ProcessData *data) const override;
			int32 GenerateShaderCode(const ShaderCompileData *compileData, const char **shaderCode) const override;
	};


	class TerrainDiffuseProcess final : public Process
	{
		private:

			TerrainDiffuseProcess(const TerrainDiffuseProcess& terrainDiffuseProcess);

			Process *Replicate(void) const override;

			bool BumpEnabled(void) const;

		public:

			TerrainDiffuseProcess();
			~TerrainDiffuseProcess();

			int32 GetPortCount(void) const override;
			unsigned_int32 GetPortFlags(int32 index) const override;
			const char *GetPortName(int32 index) const override;

			int32 GenerateProcessSignature(const ShaderCompileData *compileData, unsigned_int32 *signature) const override;
			int32 GenerateDerivedInterpolantTypes(const ShaderCompileData *compileData, ProcessType *type) const override;
			void GenerateProcessData(const ShaderCompileData *compileData, ProcessData *data) const override;
			int32 GenerateShaderCode(const ShaderCompileData *compileData, const char **shaderCode) const override;
	};


	class TerrainSpecularProcess final : public Process
	{
		private:

			TerrainSpecularProcess(const TerrainSpecularProcess& terrainSpecularProcess);

			Process *Replicate(void) const override;

			bool BumpEnabled(void) const;

		public:

			TerrainSpecularProcess();
			~TerrainSpecularProcess();

			int32 GetPortCount(void) const override;
			unsigned_int32 GetPortFlags(int32 index) const override;
			const char *GetPortName(int32 index) const override;

			int32 GenerateProcessSignature(const ShaderCompileData *compileData, unsigned_int32 *signature) const override;
			int32 GenerateDerivedInterpolantTypes(const ShaderCompileData *compileData, ProcessType *type) const override;
			void GenerateProcessData(const ShaderCompileData *compileData, ProcessData *data) const override;
			int32 GenerateShaderCode(const ShaderCompileData *compileData, const char **shaderCode) const override;
	};


	class GenerateImpostorNormalProcess final : public Process
	{
		private:

			GenerateImpostorNormalProcess(const GenerateImpostorNormalProcess& generateImpostorNormalProcess);

			Process *Replicate(void) const override;

		public:

			GenerateImpostorNormalProcess();
			~GenerateImpostorNormalProcess();

			void GenerateProcessData(const ShaderCompileData *compileData, ProcessData *data) const override;
			int32 GenerateShaderCode(const ShaderCompileData *compileData, const char **shaderCode) const override;
	};


	class ImpostorDepthProcess final : public InterpolantProcess
	{
		private:

			ImpostorDepthProcess(const ImpostorDepthProcess& impostorDepthProcess);

			Process *Replicate(void) const override;

		public:

			ImpostorDepthProcess();
			~ImpostorDepthProcess();
	};


	class CombineNormalsProcess final : public Process
	{
		private:

			CombineNormalsProcess(const CombineNormalsProcess& combineNormalsProcess);

			Process *Replicate(void) const override;

		public:

			CombineNormalsProcess();
			~CombineNormalsProcess();

			int32 GetPortCount(void) const override;
			unsigned_int32 GetPortFlags(int32 index) const override;
			const char *GetPortName(int32 index) const override;

			void GenerateProcessData(const ShaderCompileData *compileData, ProcessData *data) const override;
			int32 GenerateShaderCode(const ShaderCompileData *compileData, const char **shaderCode) const override;
	};


	class FrontNormalProcess final : public Process
	{
		private:

			FrontNormalProcess(const FrontNormalProcess& frontNormalProcess);

			Process *Replicate(void) const override;

		public:

			FrontNormalProcess();
			~FrontNormalProcess();

			int32 GetPortCount(void) const override;
			const char *GetPortName(int32 index) const override;

			int32 GenerateDerivedInterpolantTypes(const ShaderCompileData *compileData, ProcessType *type) const override;
			void GenerateProcessData(const ShaderCompileData *compileData, ProcessData *data) const override;
			int32 GenerateShaderCode(const ShaderCompileData *compileData, const char **shaderCode) const override;
	};


	class ReflectVectorProcess final : public Process
	{
		private:

			ReflectVectorProcess(const ReflectVectorProcess& reflectVectorProcess);

			Process *Replicate(void) const override;

		public:

			ReflectVectorProcess();
			~ReflectVectorProcess();

			int32 GetPortCount(void) const override;
			unsigned_int32 GetPortFlags(int32 index) const override;
			const char *GetPortName(int32 index) const override;

			void GenerateProcessData(const ShaderCompileData *compileData, ProcessData *data) const override;
			int32 GenerateShaderCode(const ShaderCompileData *compileData, const char **shaderCode) const override;
	};


	class LinearRampProcess final : public UnaryProcess
	{
		private:

			float		rampCenter;
			float		rampWidth;

			LinearRampProcess(const LinearRampProcess& linearRampProcess);

			Process *Replicate(void) const override;

		public:

			LinearRampProcess();
			~LinearRampProcess();

			float GetRampCenter(void) const
			{
				return (rampCenter);
			}

			void SetRampCenter(float center)
			{
				rampCenter = center;
			}

			float GetRampWidth(void) const
			{
				return (rampWidth);
			}

			void SetRampWidth(float width)
			{
				rampWidth = width;
			}

			void Pack(Packer& data, unsigned_int32 packFlags) const override;
			void Unpack(Unpacker& data, unsigned_int32 unpackFlags) override;

			int32 GetSettingCount(void) const override;
			Setting *GetSetting(int32 index) const override;
			void SetSetting(const Setting *setting) override;

			bool operator ==(const Process& process) const override;

			int32 GenerateProcessSignature(const ShaderCompileData *compileData, unsigned_int32 *signature) const override;
			void GenerateProcessData(const ShaderCompileData *compileData, ProcessData *data) const override;
			int32 GenerateShaderCode(const ShaderCompileData *compileData, const char **shaderCode) const override;
	};


	class SmoothParameterProcess final : public UnaryProcess
	{
		private:

			SmoothParameterProcess(const SmoothParameterProcess& smoothParameterProcess);

			Process *Replicate(void) const override;

		public:

			SmoothParameterProcess();
			~SmoothParameterProcess();

			const char *GetPortName(int32 index) const override;

			int32 GenerateShaderCode(const ShaderCompileData *compileData, const char **shaderCode) const override;
	};


	class SteepParameterProcess final : public UnaryProcess
	{
		private:

			SteepParameterProcess(const SteepParameterProcess& steepParameterProcess);

			Process *Replicate(void) const override;

		public:

			SteepParameterProcess();
			~SteepParameterProcess();

			const char *GetPortName(int32 index) const override;

			int32 GenerateShaderCode(const ShaderCompileData *compileData, const char **shaderCode) const override;
	};


	class WorldTransformProcess final : public Process
	{
		private:

			WorldTransformProcess(const WorldTransformProcess& worldTransformProcess);

			Process *Replicate(void) const override;

		public:

			WorldTransformProcess();
			~WorldTransformProcess();

			int32 GetPortCount(void) const override;
			const char *GetPortName(int32 index) const override;

			void GenerateProcessData(const ShaderCompileData *compileData, ProcessData *data) const override;
			int32 GenerateShaderCode(const ShaderCompileData *compileData, const char **shaderCode) const override;
	};


	class DeltaDepthProcess final : public Process
	{
		private:

			float		deltaScale;

			DeltaDepthProcess(const DeltaDepthProcess& deltaDepthProcess);

			Process *Replicate(void) const override;

			static bool StructureEffectsEnabled(void);

		public:

			DeltaDepthProcess();
			~DeltaDepthProcess();

			float GetDeltaScale(void) const
			{
				return (deltaScale);
			}

			void SetDeltaScale(float scale)
			{
				deltaScale = scale;
			}

			void Pack(Packer& data, unsigned_int32 packFlags) const override;
			void Unpack(Unpacker& data, unsigned_int32 unpackFlags) override;

			int32 GetSettingCount(void) const override;
			Setting *GetSetting(int32 index) const override;
			void SetSetting(const Setting *setting) override;

			bool operator ==(const Process& process) const override;

			int32 GenerateProcessSignature(const ShaderCompileData *compileData, unsigned_int32 *signature) const override;
			void GenerateProcessData(const ShaderCompileData *compileData, ProcessData *data) const override;
			int32 GenerateShaderCode(const ShaderCompileData *compileData, const char **shaderCode) const override;
	};


	class ParallaxProcess final : public TextureMapProcess
	{
		private:

			ParallaxProcess(const ParallaxProcess& parallaxProcess);

			Process *Replicate(void) const override;

			bool ProcessEnabled(const ShaderCompileData *compileData) const;

			static void StateProc_CalculateParallaxScale(const Renderable *renderable, const void *cookie);

		public:

			ParallaxProcess();
			~ParallaxProcess();

			int32 GetSettingCount(void) const override;
			Setting *GetSetting(int32 index) const override;
			void SetSetting(const Setting *setting) override;

			int32 GenerateProcessSignature(const ShaderCompileData *compileData, unsigned_int32 *signature) const override;
			int32 GenerateDerivedInterpolantTypes(const ShaderCompileData *compileData, ProcessType *type) const override;
			void GenerateProcessData(const ShaderCompileData *compileData, ProcessData *data) const override;
			int32 GenerateOutputIdentifier(const ShaderCompileData *compileData, const ShaderAllocationData *allocData, SwizzleData *swizzleData, char *name) const override;
			int32 GenerateShaderCode(const ShaderCompileData *compileData, const char **shaderCode) const override;
	};


	class KillProcess final : public Process
	{
		private:

			KillProcess(const KillProcess& killProcess);

			Process *Replicate(void) const override;

		public:

			KillProcess();
			~KillProcess();

			int32 GetPortCount(void) const override;
			unsigned_int32 GetPortFlags(int32 index) const override;
			const char *GetPortName(int32 index) const override;

			void GenerateProcessData(const ShaderCompileData *compileData, ProcessData *data) const override;
			int32 GenerateShaderCode(const ShaderCompileData *compileData, const char **shaderCode) const override;
	};


	class ImpostorTransitionProcess final : public Process
	{
		private:

			ImpostorTransitionProcess(const ImpostorTransitionProcess& impostorTransitionProcess);

			Process *Replicate(void) const override;

		public:

			ImpostorTransitionProcess();
			~ImpostorTransitionProcess();

			int32 GenerateDerivedInterpolantTypes(const ShaderCompileData *compileData, ProcessType *type) const override;
			void GenerateProcessData(const ShaderCompileData *compileData, ProcessData *data) const override;
			int32 GenerateShaderCode(const ShaderCompileData *compileData, const char **shaderCode) const override;
	};


	class GeometryTransitionProcess final : public Process
	{
		private:

			Texture		*textureObject;

			GeometryTransitionProcess(const GeometryTransitionProcess& geometryTransitionProcess);

			Process *Replicate(void) const override;

			bool ProcessEnabled(const ShaderCompileData *compileData) const;

		public:

			GeometryTransitionProcess();
			~GeometryTransitionProcess();

			int32 GetSettingCount(void) const override;
			Setting *GetSetting(int32 index) const override;
			void SetSetting(const Setting *setting) override;

			int32 GenerateProcessSignature(const ShaderCompileData *compileData, unsigned_int32 *signature) const override;
			void GenerateProcessData(const ShaderCompileData *compileData, ProcessData *data) const override;
			int32 GenerateShaderCode(const ShaderCompileData *compileData, const char **shaderCode) const override;
	};


	class FireProcess final : public TextureMapProcess
	{
		private:

			FireTextureAttribute::FireParams			fireParams;
			const FireTextureAttribute::FireParams		*fireData;

			FireProcess(const FireProcess& fireProcess);

			Process *Replicate(void) const override;

			static void StateProc_SetFireParams(const Renderable *renderable, const void *cookie);

		public:

			FireProcess();
			~FireProcess();

			void SetFireParams(const FireTextureAttribute::FireParams *params)
			{
				fireParams.fireIntensity = params->fireIntensity;
				fireParams.noiseVelocity[0] = params->noiseVelocity[0];
				fireParams.noiseVelocity[1] = params->noiseVelocity[1];
				fireParams.noiseVelocity[2] = params->noiseVelocity[2];
			}

			void SetFireData(const FireTextureAttribute::FireParams *data)
			{
				fireData = data;
			}

			void Pack(Packer& data, unsigned_int32 packFlags) const override;
			void Unpack(Unpacker& data, unsigned_int32 unpackFlags) override;

			int32 GetPortCount(void) const override;

			void GenerateProcessData(const ShaderCompileData *compileData, ProcessData *data) const override;
			int32 GenerateShaderCode(const ShaderCompileData *compileData, const char **shaderCode) const override;
	};


	class DistortionProcess final : public TextureMapProcess
	{
		private:

			DistortionProcess(const DistortionProcess& distortionProcess);

			Process *Replicate(void) const override;

			static void StateProc_CopyDistortionPlane(const Renderable *renderable, const void *cookie);
			static void StateProc_TransformDistortionPlane(const Renderable *renderable, const void *cookie);

		public:

			DistortionProcess();
			~DistortionProcess();

			void GenerateProcessData(const ShaderCompileData *compileData, ProcessData *data) const override;
			int32 GenerateShaderCode(const ShaderCompileData *compileData, const char **shaderCode) const override;
	};


	class FrameBufferProcess final : public Process
	{
		private:

			unsigned_int32		frameBufferIndex;

			FrameBufferProcess(const FrameBufferProcess& frameBufferProcess);

			Process *Replicate(void) const override;

		public:

			enum
			{
				kFrameBufferReflection,
				kFrameBufferRefraction,
				kFrameBufferStructure,
				kFrameBufferVelocity,
				kFrameBufferOcclusion,
				kFrameBufferGlowBloom,
				kFrameBufferInfiniteShadow,
				kFrameBufferPointShadow,
				kFrameBufferSpotShadow
			};

			FrameBufferProcess(unsigned_int32 index);
			~FrameBufferProcess();

			int32 GetPortCount(void) const override;

			int32 GenerateProcessSignature(const ShaderCompileData *compileData, unsigned_int32 *signature) const override;
			void GenerateProcessData(const ShaderCompileData *compileData, ProcessData *data) const override;
			int32 GenerateShaderCode(const ShaderCompileData *compileData, const char **shaderCode) const override;
	};


	class OutputProcess : public Process
	{
		private:

			Process *Replicate(void) const override;

		protected:

			OutputProcess(ProcessType type);
			OutputProcess(const OutputProcess& outputProcess);

		public:

			OutputProcess();
			~OutputProcess();
	};


	class InputProcess final : public Process
	{
		private:

			String<7>		inputPortName;

			InputProcess();
			InputProcess(const InputProcess& inputProcess);

			Process *Replicate(void) const override;

		public:

			InputProcess(const char *name);
			~InputProcess();

			const char *GetInputPortName(void) const
			{
				return (inputPortName);
			}

			void SetInputPortName(const char *name)
			{
				inputPortName = name;
			}

			void Pack(Packer& data, unsigned_int32 packFlags) const override;
			void Unpack(Unpacker& data, unsigned_int32 unpackFlags) override;

			int32 GetSettingCount(void) const override;
			Setting *GetSetting(int32 index) const override;
			void SetSetting(const Setting *setting) override;

			bool operator ==(const Process& process) const override;
	};


	class NullOutputProcess final : public OutputProcess
	{
		private:

			NullOutputProcess(const NullOutputProcess& nullOutputProcess);

			Process *Replicate(void) const override;

		public:

			NullOutputProcess();
			~NullOutputProcess();

			int32 GenerateShaderCode(const ShaderCompileData *compileData, const char **shaderCode) const override;
	};


	class AddOutputProcess final : public OutputProcess
	{
		private:

			AddOutputProcess(const AddOutputProcess& addOutputProcess);

			Process *Replicate(void) const override;

		public:

			AddOutputProcess();
			~AddOutputProcess();

			int32 GetPortCount(void) const override;
			const char *GetPortName(int32 index) const override;

			void GenerateProcessData(const ShaderCompileData *compileData, ProcessData *data) const override;
			int32 GenerateOutputIdentifier(const ShaderCompileData *compileData, const ShaderAllocationData *allocData, SwizzleData *swizzleData, char *name) const override;
			int32 GenerateShaderCode(const ShaderCompileData *compileData, const char **shaderCode) const override;
	};


	class AlphaOutputProcess final : public OutputProcess
	{
		private:

			AlphaOutputProcess(const AlphaOutputProcess& alphaOutputProcess);

			Process *Replicate(void) const override;

		public:

			C4API AlphaOutputProcess();
			C4API ~AlphaOutputProcess();

			int32 GetPortCount(void) const override;
			unsigned_int32 GetPortFlags(int32 index) const override;
			const char *GetPortName(int32 index) const override;

			void GenerateProcessData(const ShaderCompileData *compileData, ProcessData *data) const override;
			int32 GenerateShaderCode(const ShaderCompileData *compileData, const char **shaderCode) const override;
	};


	class AlphaTestOutputProcess final : public OutputProcess
	{
		private:

			AlphaTestOutputProcess(const AlphaTestOutputProcess& alphaTestOutputProcess);

			Process *Replicate(void) const override;

		public:

			C4API AlphaTestOutputProcess();
			C4API ~AlphaTestOutputProcess();

			int32 GetPortCount(void) const override;
			unsigned_int32 GetPortFlags(int32 index) const override;
			const char *GetPortName(int32 index) const override;

			int32 GenerateProcessSignature(const ShaderCompileData *compileData, unsigned_int32 *signature) const override;
			void GenerateProcessData(const ShaderCompileData *compileData, ProcessData *data) const override;
			int32 GenerateShaderCode(const ShaderCompileData *compileData, const char **shaderCode) const override;
	};


	class AmbientOutputProcess final : public OutputProcess
	{
		private:

			static Texture		*radiosityColorTexture;

			AmbientOutputProcess(const AmbientOutputProcess& ambientOutputProcess);

			Process *Replicate(void) const override;

			static ShaderType GetAmbientShaderType(const ShaderCompileData *compileData);
			static bool AmbientOcclusionEnabled(const ShaderCompileData *compileData);
			static bool TwoSidedMaterial(const ShaderCompileData *compileData);
			bool AmbientBumpsEnabled(void) const;

			static void StateProc_ConfigureAmbientLight(const Renderable *renderable, const void *cookie);
			static void StateProc_ConfigureDarkAmbientLight(const Renderable *renderable, const void *cookie);

			static void StateProc_ConfigureRadiositySpace(const Renderable *renderable, const void *cookie);
			static void StateProc_ConfigureTransformRadiositySpace(const Renderable *renderable, const void *cookie);

		public:

			C4API AmbientOutputProcess();
			C4API ~AmbientOutputProcess();

			static void Initialize(void);
			static void Terminate(void);

			int32 GetPortCount(void) const override;
			unsigned_int32 GetPortFlags(int32 index) const override;
			const char *GetPortName(int32 index) const override;

			int32 GenerateProcessSignature(const ShaderCompileData *compileData, unsigned_int32 *signature) const override;
			void GenerateProcessData(const ShaderCompileData *compileData, ProcessData *data) const override;
			int32 GenerateOutputIdentifier(const ShaderCompileData *compileData, const ShaderAllocationData *allocData, SwizzleData *swizzleData, char *name) const override;
			int32 GenerateShaderCode(const ShaderCompileData *compileData, const char **shaderCode) const override;
	};


	class VertexOcclusionOutputProcess final : public OutputProcess
	{
		private:

			VertexOcclusionOutputProcess(const VertexOcclusionOutputProcess& vertexOcclusionOutputProcess);

			Process *Replicate(void) const override;

		public:

			VertexOcclusionOutputProcess();
			~VertexOcclusionOutputProcess();

			int32 GetPortCount(void) const override;
			const char *GetPortName(int32 index) const override;

			void GenerateProcessData(const ShaderCompileData *compileData, ProcessData *data) const override;
			int32 GenerateOutputIdentifier(const ShaderCompileData *compileData, const ShaderAllocationData *allocData, SwizzleData *swizzleData, char *name) const override;
			int32 GenerateShaderCode(const ShaderCompileData *compileData, const char **shaderCode) const override;
	};


	class EmissionOutputProcess final : public OutputProcess
	{
		private:

			EmissionOutputProcess(const EmissionOutputProcess& emissionOutputProcess);

			Process *Replicate(void) const override;

		public:

			C4API EmissionOutputProcess();
			C4API ~EmissionOutputProcess();

			int32 GetPortCount(void) const override;
			unsigned_int32 GetPortFlags(int32 index) const override;
			const char *GetPortName(int32 index) const override;

			void GenerateProcessData(const ShaderCompileData *compileData, ProcessData *data) const override;
			int32 GenerateOutputIdentifier(const ShaderCompileData *compileData, const ShaderAllocationData *allocData, SwizzleData *swizzleData, char *name) const override;
	};


	class ReflectionOutputProcess final : public OutputProcess
	{
		private:

			ReflectionAttribute::ReflectionParams			reflectionParams;
			const ReflectionAttribute::ReflectionParams		*reflectionData;

			ReflectionOutputProcess(const ReflectionOutputProcess& reflectionOutputProcess);

			Process *Replicate(void) const override;

			static void StateProc_CalculateReflectionScale(const Renderable *renderable, const void *cookie);

		public:

			C4API ReflectionOutputProcess();
			C4API ~ReflectionOutputProcess();

			void SetReflectionParams(const ReflectionAttribute::ReflectionParams *params)
			{
				reflectionParams.normalIncidenceReflectivity = params->normalIncidenceReflectivity;
				reflectionParams.reflectionOffsetScale = params->reflectionOffsetScale;
			}

			void SetReflectionData(const ReflectionAttribute::ReflectionParams *data)
			{
				reflectionData = data;
			}

			void Pack(Packer& data, unsigned_int32 packFlags) const override;
			void Unpack(Unpacker& data, unsigned_int32 unpackFlags) override;

			int32 GetSettingCount(void) const override;
			Setting *GetSetting(int32 index) const override;
			void SetSetting(const Setting *setting) override;

			bool operator ==(const Process& process) const override;

			int32 GetPortCount(void) const override;
			unsigned_int32 GetPortFlags(int32 index) const override;
			const char *GetPortName(int32 index) const override;

			void ReferenceStateParams(const Process *process) override;

			int32 GenerateProcessSignature(const ShaderCompileData *compileData, unsigned_int32 *signature) const override;
			int32 GenerateDerivedInterpolantTypes(const ShaderCompileData *compileData, ProcessType *type) const override;
			void GenerateProcessData(const ShaderCompileData *compileData, ProcessData *data) const override;
			int32 GenerateShaderCode(const ShaderCompileData *compileData, const char **shaderCode) const override;
	};


	class RefractionOutputProcess final : public OutputProcess
	{
		private:

			RefractionAttribute::RefractionParams			refractionParams;
			const RefractionAttribute::RefractionParams		*refractionData;

			RefractionOutputProcess(const RefractionOutputProcess& refractionOutputProcess);

			Process *Replicate(void) const override;

			static void StateProc_CalculateRefractionParams(const Renderable *renderable, const void *cookie);

		public:

			C4API RefractionOutputProcess();
			C4API ~RefractionOutputProcess();

			void SetRefractionParams(const RefractionAttribute::RefractionParams *params)
			{
				refractionParams.refractionOffsetScale = params->refractionOffsetScale;
			}

			void SetRefractionData(const RefractionAttribute::RefractionParams *data)
			{
				refractionData = data;
			}

			void Pack(Packer& data, unsigned_int32 packFlags) const override;
			void Unpack(Unpacker& data, unsigned_int32 unpackFlags) override;

			int32 GetSettingCount(void) const override;
			Setting *GetSetting(int32 index) const override;
			void SetSetting(const Setting *setting) override;

			bool operator ==(const Process& process) const override;

			int32 GetPortCount(void) const override;
			unsigned_int32 GetPortFlags(int32 index) const override;
			const char *GetPortName(int32 index) const override;

			void ReferenceStateParams(const Process *process) override;

			void GenerateProcessData(const ShaderCompileData *compileData, ProcessData *data) const override;
			int32 GenerateShaderCode(const ShaderCompileData *compileData, const char **shaderCode) const override;
	};


	class EnvironmentOutputProcess final : public OutputProcess
	{
		private:

			Texture				*textureObject;
			ResourceName		textureName;

			EnvironmentOutputProcess(const EnvironmentOutputProcess& environmentOutputProcess);

			Process *Replicate(void) const override;

		public:

			C4API EnvironmentOutputProcess();
			C4API ~EnvironmentOutputProcess();

			const ResourceName& GetTextureName(void) const
			{
				return (textureName);
			}

			void Pack(Packer& data, unsigned_int32 packFlags) const override;
			void Unpack(Unpacker& data, unsigned_int32 unpackFlags) override;
			void *BeginSettingsUnpack(void) override;

			int32 GetSettingCount(void) const override;
			Setting *GetSetting(int32 index) const override;
			void SetSetting(const Setting *setting) override;

			bool operator ==(const Process& process) const override;

			void SetTexture(const char *name);

			int32 GetPortCount(void) const override;
			unsigned_int32 GetPortFlags(int32 index) const override;
			const char *GetPortName(int32 index) const override;

			int32 GenerateProcessSignature(const ShaderCompileData *compileData, unsigned_int32 *signature) const override;
			int32 GenerateDerivedInterpolantTypes(const ShaderCompileData *compileData, ProcessType *type) const override;
			void GenerateProcessData(const ShaderCompileData *compileData, ProcessData *data) const override;
			int32 GenerateShaderCode(const ShaderCompileData *compileData, const char **shaderCode) const override;
	};


	class TerrainEnvironmentOutputProcess final : public OutputProcess
	{
		private:

			Texture				*textureObject;
			ResourceName		textureName;

			TerrainEnvironmentOutputProcess(const TerrainEnvironmentOutputProcess& terrainEnvironmentOutputProcess);

			Process *Replicate(void) const override;

			bool BumpEnabled(void) const;

		public:

			C4API TerrainEnvironmentOutputProcess();
			C4API ~TerrainEnvironmentOutputProcess();

			const ResourceName& GetTextureName(void) const
			{
				return (textureName);
			}

			void Pack(Packer& data, unsigned_int32 packFlags) const override;
			void Unpack(Unpacker& data, unsigned_int32 unpackFlags) override;
			void *BeginSettingsUnpack(void) override;

			int32 GetSettingCount(void) const override;
			Setting *GetSetting(int32 index) const override;
			void SetSetting(const Setting *setting) override;

			bool operator ==(const Process& process) const override;

			void SetTextureName(const char *name);

			int32 GetPortCount(void) const override;
			unsigned_int32 GetPortFlags(int32 index) const override;
			const char *GetPortName(int32 index) const override;

			int32 GenerateProcessSignature(const ShaderCompileData *compileData, unsigned_int32 *signature) const override;
			int32 GenerateDerivedInterpolantTypes(const ShaderCompileData *compileData, ProcessType *type) const override;
			void GenerateProcessData(const ShaderCompileData *compileData, ProcessData *data) const override;
			int32 GenerateShaderCode(const ShaderCompileData *compileData, const char **shaderCode) const override;
	};


	class ImpostorDepthOutputProcess final : public OutputProcess
	{
		private:

			ImpostorDepthOutputProcess(const ImpostorDepthOutputProcess& impostorDepthOutputProcess);

			Process *Replicate(void) const override;

		public:

			C4API ImpostorDepthOutputProcess();
			C4API ~ImpostorDepthOutputProcess();

			int32 GetPortCount(void) const override;
			unsigned_int32 GetPortFlags(int32 index) const override;
			const char *GetPortName(int32 index) const override;

			void GenerateProcessData(const ShaderCompileData *compileData, ProcessData *data) const override;
			int32 GenerateShaderCode(const ShaderCompileData *compileData, const char **shaderCode) const override;
	};


	class LightOutputProcess final : public OutputProcess
	{
		private:

			LightOutputProcess(const LightOutputProcess& lightOutputProcess);

			Process *Replicate(void) const override;

		public:

			C4API LightOutputProcess();
			C4API ~LightOutputProcess();

			int32 GetPortCount(void) const override;
			int32 GetInternalPortCount(void) const override;
			unsigned_int32 GetPortFlags(int32 index) const override;
			const char *GetPortName(int32 index) const override;

			static ShaderType GetLightShaderType(const ShaderCompileData *compileData);

			int32 GenerateProcessSignature(const ShaderCompileData *compileData, unsigned_int32 *signature) const override;
			void GenerateProcessData(const ShaderCompileData *compileData, ProcessData *data) const override;
			int32 GenerateOutputIdentifier(const ShaderCompileData *compileData, const ShaderAllocationData *allocData, SwizzleData *swizzleData, char *name) const override;
			int32 GenerateShaderCode(const ShaderCompileData *compileData, const char **shaderCode) const override;
	};


	class GlowOutputProcess final : public OutputProcess
	{
		private:

			GlowOutputProcess(const GlowOutputProcess& glowOutputProcess);

			Process *Replicate(void) const override;

		public:

			C4API GlowOutputProcess();
			C4API ~GlowOutputProcess();

			int32 GetPortCount(void) const override;
			unsigned_int32 GetPortFlags(int32 index) const override;
			const char *GetPortName(int32 index) const override;

			void GenerateProcessData(const ShaderCompileData *compileData, ProcessData *data) const override;
			int32 GenerateShaderCode(const ShaderCompileData *compileData, const char **shaderCode) const override;
	};


	class BloomOutputProcess final : public OutputProcess
	{
		private:

			BloomOutputProcess(const BloomOutputProcess& bloomOutputProcess);

			Process *Replicate(void) const override;

		public:

			C4API BloomOutputProcess();
			C4API ~BloomOutputProcess();

			int32 GetPortCount(void) const override;
			int32 GetInternalPortCount(void) const override;
			unsigned_int32 GetPortFlags(int32 index) const override;
			const char *GetPortName(int32 index) const override;

			void GenerateProcessData(const ShaderCompileData *compileData, ProcessData *data) const override;
			int32 GenerateShaderCode(const ShaderCompileData *compileData, const char **shaderCode) const override;
	};


	class GlowBloomOutputProcess final : public OutputProcess
	{
		private:

			GlowBloomOutputProcess(const GlowBloomOutputProcess& glowBloomOutputProcess);

			Process *Replicate(void) const override;

		public:

			GlowBloomOutputProcess();
			~GlowBloomOutputProcess();

			int32 GetPortCount(void) const override;
			const char *GetPortName(int32 index) const override;

			void GenerateProcessData(const ShaderCompileData *compileData, ProcessData *data) const override;
			int32 GenerateShaderCode(const ShaderCompileData *compileData, const char **shaderCode) const override;
	};


	class StructureOutputProcess final : public OutputProcess
	{
		private:

			StructureOutputProcess(const StructureOutputProcess& structureOutputProcess);

			Process *Replicate(void) const override;

			static unsigned_int32 GetStructureRenderFlags(unsigned_int32 renderableFlags);

		public:

			StructureOutputProcess();
			~StructureOutputProcess();

			int32 GenerateProcessSignature(const ShaderCompileData *compileData, unsigned_int32 *signature) const override;
			void GenerateProcessData(const ShaderCompileData *compileData, ProcessData *data) const override;
			int32 GenerateShaderCode(const ShaderCompileData *compileData, const char **shaderCode) const override;
	};


	class InfiniteShadowProcess final : public Process
	{
		private:

			InfiniteShadowProcess(const InfiniteShadowProcess& infiniteShadowProcess);

			Process *Replicate(void) const override;

		public:

			InfiniteShadowProcess();
			~InfiniteShadowProcess();

			int32 GetPortCount(void) const override;
			unsigned_int32 GetPortFlags(int32 index) const override;
			const char *GetPortName(int32 index) const override;

			void GenerateProcessData(const ShaderCompileData *compileData, ProcessData *data) const override;
			int32 GenerateShaderCode(const ShaderCompileData *compileData, const char **shaderCode) const override;
	};


	class PointShadowProcess final : public Process
	{
		private:

			PointShadowProcess(const PointShadowProcess& pointShadowProcess);

			Process *Replicate(void) const override;

		public:

			PointShadowProcess();
			~PointShadowProcess();

			void GenerateProcessData(const ShaderCompileData *compileData, ProcessData *data) const override;
			int32 GenerateShaderCode(const ShaderCompileData *compileData, const char **shaderCode) const override;
	};


	class SpotShadowProcess final : public Process
	{
		private:

			SpotShadowProcess(const SpotShadowProcess& spotShadowProcess);

			Process *Replicate(void) const override;

		public:

			SpotShadowProcess();
			~SpotShadowProcess();

			void GenerateProcessData(const ShaderCompileData *compileData, ProcessData *data) const override;
			int32 GenerateShaderCode(const ShaderCompileData *compileData, const char **shaderCode) const override;
	};


	class PointIlluminationProcess final : public Process
	{
		private:

			PointIlluminationProcess(const PointIlluminationProcess& pointIlluminationProcess);

			Process *Replicate(void) const override;

		public:

			PointIlluminationProcess();
			~PointIlluminationProcess();

			void GenerateProcessData(const ShaderCompileData *compileData, ProcessData *data) const override;
			int32 GenerateShaderCode(const ShaderCompileData *compileData, const char **shaderCode) const override;
	};


	class CubeIlluminationProcess final : public Process
	{
		private:

			CubeIlluminationProcess(const CubeIlluminationProcess& cubeIlluminationProcess);

			Process *Replicate(void) const override;

		public:

			CubeIlluminationProcess();
			~CubeIlluminationProcess();

			void GenerateProcessData(const ShaderCompileData *compileData, ProcessData *data) const override;
			int32 GenerateShaderCode(const ShaderCompileData *compileData, const char **shaderCode) const override;
	};


	class SpotIlluminationProcess final : public Process
	{
		private:

			SpotIlluminationProcess(const SpotIlluminationProcess& spotIlluminationProcess);

			Process *Replicate(void) const override;

		public:

			SpotIlluminationProcess();
			~SpotIlluminationProcess();

			void GenerateProcessData(const ShaderCompileData *compileData, ProcessData *data) const override;
			int32 GenerateShaderCode(const ShaderCompileData *compileData, const char **shaderCode) const override;
	};


	class ConstantFogProcess final : public Process
	{
		private:

			ConstantFogProcess(const ConstantFogProcess& constantFogProcess);

			Process *Replicate(void) const override;

		public:

			ConstantFogProcess();
			~ConstantFogProcess();

			void Preschedule(const ShaderCompileData *compileData) override;

			int32 GenerateProcessSignature(const ShaderCompileData *compileData, unsigned_int32 *signature) const override;
			void GenerateProcessData(const ShaderCompileData *compileData, ProcessData *data) const override;
			int32 GenerateShaderCode(const ShaderCompileData *compileData, const char **shaderCode) const override;

			static void StateProc_CopyFogPlane(const Renderable *renderable, const void *cookie);
			static void StateProc_TransformFogPlane(const Renderable *renderable, const void *cookie);
	};


	class LinearFogProcess final : public Process
	{
		private:

			LinearFogProcess(const LinearFogProcess& linearFogProcess);

			Process *Replicate(void) const override;

		public:

			LinearFogProcess();
			~LinearFogProcess();

			void Preschedule(const ShaderCompileData *compileData) override;

			void GenerateProcessData(const ShaderCompileData *compileData, ProcessData *data) const override;
			int32 GenerateShaderCode(const ShaderCompileData *compileData, const char **shaderCode) const override;
	};


	class AmbientFogProcess final : public Process
	{
		private:

			AmbientFogProcess(const AmbientFogProcess& ambientFogProcess);

			Process *Replicate(void) const override;

		public:

			AmbientFogProcess();
			~AmbientFogProcess();

			void GenerateProcessData(const ShaderCompileData *compileData, ProcessData *data) const override;
			int32 GenerateShaderCode(const ShaderCompileData *compileData, const char **shaderCode) const override;
	};


	class LightFogProcess final : public Process
	{
		private:

			LightFogProcess(const LightFogProcess& lightFogProcess);

			Process *Replicate(void) const override;

		public:

			LightFogProcess();
			~LightFogProcess();

			void GenerateProcessData(const ShaderCompileData *compileData, ProcessData *data) const override;
			int32 GenerateShaderCode(const ShaderCompileData *compileData, const char **shaderCode) const override;
	};


	class AlphaFogProcess final : public Process
	{
		private:

			AlphaFogProcess(const AlphaFogProcess& alphaFogProcess);

			Process *Replicate(void) const override;

		public:

			AlphaFogProcess();
			~AlphaFogProcess();

			void GenerateProcessData(const ShaderCompileData *compileData, ProcessData *data) const override;
			int32 GenerateShaderCode(const ShaderCompileData *compileData, const char **shaderCode) const override;
	};


	class ColorPostProcess final : public Process
	{
		private:

			ColorPostProcess(const ColorPostProcess& colorPostProcess);

			Process *Replicate(void) const override;

		public:

			ColorPostProcess();
			~ColorPostProcess();

			int32 GetPortCount(void) const override;
			const char *GetPortName(int32 index) const override;

			void GenerateProcessData(const ShaderCompileData *compileData, ProcessData *data) const override;
			int32 GenerateShaderCode(const ShaderCompileData *compileData, const char **shaderCode) const override;
	};


	class DistortPostProcess final : public Process
	{
		private:

			DistortPostProcess(const DistortPostProcess& distortPostProcess);

			Process *Replicate(void) const override;

		public:

			DistortPostProcess();
			~DistortPostProcess();

			void GenerateProcessData(const ShaderCompileData *compileData, ProcessData *data) const override;
			int32 GenerateShaderCode(const ShaderCompileData *compileData, const char **shaderCode) const override;
	};


	class MotionBlurPostProcess final : public Process
	{
		private:

			bool	gradientFlag;

			MotionBlurPostProcess(const MotionBlurPostProcess& motionBlurPostProcess);

			Process *Replicate(void) const override;

		public:

			MotionBlurPostProcess(bool gradient);
			~MotionBlurPostProcess();

			int32 GetPortCount(void) const override;
			const char *GetPortName(int32 index) const override;

			int32 GenerateProcessSignature(const ShaderCompileData *compileData, unsigned_int32 *signature) const override;
			void GenerateProcessData(const ShaderCompileData *compileData, ProcessData *data) const override;
			int32 GenerateShaderCode(const ShaderCompileData *compileData, const char **shaderCode) const override;
	};


	class OcclusionPostProcess final : public Process
	{
		private:

			static Texture		*vectorNoiseTexture;

			OcclusionPostProcess(const OcclusionPostProcess& occlusionPostProcess);

			Process *Replicate(void) const override;

		public:

			OcclusionPostProcess();
			~OcclusionPostProcess();

			static void Initialize(void);
			static void Terminate(void);

			static Texture *GetVectorNoiseTexture(void)
			{
				return (vectorNoiseTexture);
			}

			int32 GenerateShaderCode(const ShaderCompileData *compileData, const char **shaderCode) const override;
	};


	class OcclusionBlurPostProcess final : public Process
	{
		private:

			OcclusionBlurPostProcess(const OcclusionBlurPostProcess& occlusionBlurPostProcess);

			Process *Replicate(void) const override;

		public:

			OcclusionBlurPostProcess();
			~OcclusionBlurPostProcess();

			void GenerateProcessData(const ShaderCompileData *compileData, ProcessData *data) const override;
			int32 GenerateShaderCode(const ShaderCompileData *compileData, const char **shaderCode) const override;
	};


	class GlowBloomExtractPostProcess final : public Process
	{
		private:

			GlowBloomExtractPostProcess(const GlowBloomExtractPostProcess& glowBloomExtractPostProcess);

			Process *Replicate(void) const override;

		public:

			GlowBloomExtractPostProcess();
			~GlowBloomExtractPostProcess();

			void GenerateProcessData(const ShaderCompileData *compileData, ProcessData *data) const override;
			int32 GenerateShaderCode(const ShaderCompileData *compileData, const char **shaderCode) const override;
	};


	class GlowBloomPostProcess final : public Process
	{
		private:

			GlowBloomPostProcess(const GlowBloomPostProcess& glowBloomPostProcess);

			Process *Replicate(void) const override;

		public:

			GlowBloomPostProcess();
			~GlowBloomPostProcess();

			int32 GetPortCount(void) const override;
			const char *GetPortName(int32 index) const override;

			void GenerateProcessData(const ShaderCompileData *compileData, ProcessData *data) const override;
			int32 GenerateShaderCode(const ShaderCompileData *compileData, const char **shaderCode) const override;
	};


	class TransformPostProcess final : public Process
	{
		private:

			bool		colorMatrixFlag;

			TransformPostProcess(const TransformPostProcess& transformPostProcess);

			Process *Replicate(void) const override;

		public:

			TransformPostProcess(bool matrixFlag);
			~TransformPostProcess();

			int32 GetPortCount(void) const override;
			const char *GetPortName(int32 index) const override;

			int32 GenerateProcessSignature(const ShaderCompileData *compileData, unsigned_int32 *signature) const override;
			void GenerateProcessData(const ShaderCompileData *compileData, ProcessData *data) const override;
			int32 GenerateShaderCode(const ShaderCompileData *compileData, const char **shaderCode) const override;
	};


	class DisplayWarpProcess final : public Process
	{
		private:

			bool		chromaticWarpFlag;

			DisplayWarpProcess(const DisplayWarpProcess& displayWarpProcess);

			Process *Replicate(void) const override;

		public:

			DisplayWarpProcess(bool chromatic);
			~DisplayWarpProcess();

			int32 GenerateProcessSignature(const ShaderCompileData *compileData, unsigned_int32 *signature) const override;
			int32 GenerateShaderCode(const ShaderCompileData *compileData, const char **shaderCode) const override;
	};
}


#endif

// ZYUQURM
