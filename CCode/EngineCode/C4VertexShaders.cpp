 

#include "C4VertexShaders.h"
#include "C4Shaders.h"

#if C4LOG_FILE

	#include "C4Engine.h"

#endif


#define C4LOG_VERTEX_SHADERS		0


using namespace C4;


#if C4OPENGL

	#define RESULT_POSITION			"gl_Position"
	#define RESULT_POINTSIZE		"gl_PointSize"
	#define RESULT_COLOR0			"vcolor[0]"
	#define RESULT_COLOR1			"vcolor[1]"
	#define RESULT_TEXCOORD0		"texcoord[0]"

#else

	#define RESULT_POSITION			"result.position"
	#define RESULT_POINTSIZE		"result.pointsize"
	#define RESULT_COLOR0			"result.color[0]"
	#define RESULT_COLOR1			"result.color[1]"
	#define RESULT_TEXCOORD0		"result.texcoord[0]"

#endif


Storage<HashTable<VertexShader>> VertexShader::hashTable;


#if C4OPENGL

	const char VertexShader::prologText[] =
	{
		#if C4GLCORE

			"#version 330\n"

		#else //[ MOBILE

			// -- Mobile code hidden --

		#endif //]

		"invariant gl_Position;\n"

		"layout(location = 0) in vec4 attrib[" VERTEX_ATTRIB_COUNT "];\n"
		"uniform vec4 vparam[" VERTEX_PARAM_COUNT "];\n"

		"layout(std140) uniform universal\n"
		"{\n"
			"vec4 uparam[" UNIVERSAL_PARAM_COUNT "];\n"
		"};\n"

		"out vresult\n"
		"{\n"
			"vec4 vcolor[2];\n"
			"vec4 texcoord[" SHADER_TEXCOORD_COUNT "];\n"
		"};\n"

		"void main()\n"
		"{\n"
			"vec4 temp;\n"
	};

	const char VertexShader::epilogText[] =
	{
		"}\n"
	};

#elif C4PSSL //[ PS4

	// -- PS4 code hidden --

#elif C4CG //[ PS3

	// -- PS3 code hidden --

#endif //]


const VertexSnippet VertexShader::nullTransform =
{
	'NULL', 0,

	RESULT_POSITION " = " ATTRIB(VERTEX_ATTRIB_POSITION0) ";\n"
};

const VertexSnippet VertexShader::modelviewProjectTransform =
{
	'MODL', 0,

	RESULT_POSITION ".x = dot(" ATTRIB(VERTEX_ATTRIB_POSITION0) ", " VPARAM(VERTEX_PARAM_MATRIX_MVP0) ");\n" 
	RESULT_POSITION ".y = dot(" ATTRIB(VERTEX_ATTRIB_POSITION0) ", " VPARAM(VERTEX_PARAM_MATRIX_MVP1) ");\n"
	RESULT_POSITION ".z = dot(" ATTRIB(VERTEX_ATTRIB_POSITION0) ", " VPARAM(VERTEX_PARAM_MATRIX_MVP2) ");\n"
	RESULT_POSITION ".w = dot(" ATTRIB(VERTEX_ATTRIB_POSITION0) ", " VPARAM(VERTEX_PARAM_MATRIX_MVP3) ");\n" 
};
 
const VertexSnippet VertexShader::modelviewProjectTransformInfinite =
{
	'MDLI', 0, 

	RESULT_POSITION ".x = dot(%OPOS, " VPARAM(VERTEX_PARAM_MATRIX_MVP0) ".xyz);\n" 
	RESULT_POSITION ".y = dot(%OPOS, " VPARAM(VERTEX_PARAM_MATRIX_MVP1) ".xyz);\n" 
	RESULT_POSITION ".z = dot(%OPOS, " VPARAM(VERTEX_PARAM_MATRIX_MVP2) ".xyz);\n"
	RESULT_POSITION ".w = dot(%OPOS, " VPARAM(VERTEX_PARAM_MATRIX_MVP3) ".xyz);\n"
};
 
const VertexSnippet VertexShader::modelviewProjectTransformHomogeneous =
{
	'MDLH', 0,

	RESULT_POSITION ".x = dot(%OPOS, " VPARAM(VERTEX_PARAM_MATRIX_MVP0) ".xyz) + " VPARAM(VERTEX_PARAM_MATRIX_MVP0) ".w;\n"
	RESULT_POSITION ".y = dot(%OPOS, " VPARAM(VERTEX_PARAM_MATRIX_MVP1) ".xyz) + " VPARAM(VERTEX_PARAM_MATRIX_MVP1) ".w;\n"
	RESULT_POSITION ".z = dot(%OPOS, " VPARAM(VERTEX_PARAM_MATRIX_MVP2) ".xyz) + " VPARAM(VERTEX_PARAM_MATRIX_MVP2) ".w;\n"
	RESULT_POSITION ".w = dot(%OPOS, " VPARAM(VERTEX_PARAM_MATRIX_MVP3) ".xyz) + " VPARAM(VERTEX_PARAM_MATRIX_MVP3) ".w;\n"
};

const VertexSnippet VertexShader::calculateCameraDirection =
{
	'CDIR', 0,

	FLOAT3 " cdir = " ATTRIB(VERTEX_ATTRIB_POSITION0) ".xyz - " VPARAM(VERTEX_PARAM_CAMERA_POSITION) ".xyz;\n"
};

const VertexSnippet VertexShader::calculateCameraDirection4D =
{
	'CDR4', 0,

	FLOAT3 " cdir = " ATTRIB(VERTEX_ATTRIB_POSITION0) ".xyz * " VPARAM(VERTEX_PARAM_CAMERA_POSITION) ".w - " VPARAM(VERTEX_PARAM_CAMERA_POSITION) ".xyz;\n"
};

const VertexSnippet VertexShader::scaleVertexCalculateCameraDirection =
{
	'SCDR', 0,

	FLOAT4 " vrtx;\n"

	"vrtx.xyz = " ATTRIB(VERTEX_ATTRIB_POSITION0) ".xyz * " VPARAM(VERTEX_PARAM_VERTEX_SCALE_OFFSET) ".xyz;\n"
	"vrtx.w = " ATTRIB(VERTEX_ATTRIB_OFFSET) ".w * " VPARAM(VERTEX_PARAM_VERTEX_SCALE_OFFSET) ".w;\n"
	FLOAT3 " cdir = vrtx.xyz - " VPARAM(VERTEX_PARAM_CAMERA_POSITION) ".xyz;\n"

};

const VertexSnippet VertexShader::scaleVertexCalculateCameraDirection4D =
{
	'SCD4', 0,

	FLOAT4 " vrtx;\n"

	"vrtx.xyz = " ATTRIB(VERTEX_ATTRIB_POSITION0) ".xyz * " VPARAM(VERTEX_PARAM_VERTEX_SCALE_OFFSET) ".xyz;\n"
	"vrtx.w = " ATTRIB(VERTEX_ATTRIB_OFFSET) ".w * " VPARAM(VERTEX_PARAM_VERTEX_SCALE_OFFSET) ".w;\n"
	FLOAT3 " cdir = vrtx.xyz * " VPARAM(VERTEX_PARAM_CAMERA_POSITION) ".w - " VPARAM(VERTEX_PARAM_CAMERA_POSITION) ".xyz;\n"
};

const VertexSnippet VertexShader::calculateBillboardPosition =
{
	'BILL', kVertexSnippetPositionFlag,

	FLOAT3 " opos = " ATTRIB(VERTEX_ATTRIB_POSITION0) ".xyz + " VPARAM(VERTEX_PARAM_CAMERA_RIGHT) ".xyz * " ATTRIB(VERTEX_ATTRIB_OFFSET) ".x + " VPARAM(VERTEX_PARAM_CAMERA_DOWN) ".xyz * " ATTRIB(VERTEX_ATTRIB_OFFSET) ".y;\n"
};

const VertexSnippet VertexShader::calculateBillboardScalePosition =
{
	'SBLL', kVertexSnippetPositionFlag,

	"temp.xy = " ATTRIB(VERTEX_ATTRIB_OFFSET) ".xy * " VPARAM(VERTEX_PARAM_VERTEX_SCALE_OFFSET) ".xy;\n"
	FLOAT3 " opos = " ATTRIB(VERTEX_ATTRIB_POSITION0) ".xyz + " VPARAM(VERTEX_PARAM_CAMERA_RIGHT) ".xyz * temp.x + " VPARAM(VERTEX_PARAM_CAMERA_DOWN) ".xyz * temp.y;\n"
};

const VertexSnippet VertexShader::calculateVertexBillboardPosition =
{
	'VBLL', kVertexSnippetPositionFlag,

	FLOAT3 " opos = " VPARAM(VERTEX_PARAM_CAMERA_RIGHT) ".xyz * " ATTRIB(VERTEX_ATTRIB_POSITION0) ".x + " VPARAM(VERTEX_PARAM_CAMERA_DOWN) ".xyz * " ATTRIB(VERTEX_ATTRIB_POSITION0) ".y;\n"
};

const VertexSnippet VertexShader::calculateVertexBillboardScalePosition =
{
	'VSBL', kVertexSnippetPositionFlag,

	"temp.xy = " ATTRIB(VERTEX_ATTRIB_POSITION0) ".xy * " VPARAM(VERTEX_PARAM_VERTEX_SCALE_OFFSET) ".xy;\n"
	FLOAT3 " opos = " VPARAM(VERTEX_PARAM_CAMERA_RIGHT) ".xyz * temp.x + " VPARAM(VERTEX_PARAM_CAMERA_DOWN) ".xyz * temp.y;\n"
};

const VertexSnippet VertexShader::calculateLightedBillboardPosition =
{
	'LBLL', kVertexSnippetPositionFlag,

	FLOAT3 " xdir = " VPARAM(VERTEX_PARAM_CAMERA_RIGHT) ".xyz * " ATTRIB(VERTEX_ATTRIB_OFFSET) ".x + " VPARAM(VERTEX_PARAM_CAMERA_DOWN) ".xyz * " ATTRIB(VERTEX_ATTRIB_OFFSET) ".y;\n"
	FLOAT3 " ydir = " VPARAM(VERTEX_PARAM_CAMERA_DOWN) ".xyz * " ATTRIB(VERTEX_ATTRIB_OFFSET) ".x - " VPARAM(VERTEX_PARAM_CAMERA_RIGHT) ".xyz * " ATTRIB(VERTEX_ATTRIB_OFFSET) ".y;\n"
	FLOAT3 " opos = " ATTRIB(VERTEX_ATTRIB_POSITION0) ".xyz + xdir * " ATTRIB(VERTEX_ATTRIB_OFFSET) ".z + ydir * " ATTRIB(VERTEX_ATTRIB_OFFSET) ".w;\n"
};

const VertexSnippet VertexShader::calculatePostboardPosition =
{
	'POST', kVertexSnippetPositionFlag,

	FLOAT3 " opos;\n"

	"temp.xy = " ATTRIB(VERTEX_ATTRIB_POSITION0) ".xy * " VPARAM(VERTEX_PARAM_CAMERA_POSITION) ".w - " VPARAM(VERTEX_PARAM_CAMERA_POSITION) ".xy;\n"
	"temp.w = " RSQRT "(temp.x * temp.x + temp.y * temp.y);\n"
	"opos.xy = " FLOAT2 "(temp.y * temp.w, -temp.x * temp.w) * " ATTRIB(VERTEX_ATTRIB_RADIUS) ".x + " ATTRIB(VERTEX_ATTRIB_POSITION0) ".xy;\n"
	"opos.z = " ATTRIB(VERTEX_ATTRIB_POSITION0) ".z;\n"
};

const VertexSnippet VertexShader::calculatePostboardScalePosition =
{
	'SPST', kVertexSnippetPositionFlag,

	FLOAT3 " opos;\n"

	"temp.xy = " ATTRIB(VERTEX_ATTRIB_POSITION0) ".xy * " VPARAM(VERTEX_PARAM_CAMERA_POSITION) ".w - " VPARAM(VERTEX_PARAM_CAMERA_POSITION) ".xy;\n"
	"temp.w = " RSQRT "(temp.x * temp.x + temp.y * temp.y);\n"
	"opos.xy = " FLOAT2 "(temp.y * temp.w, -temp.x * temp.w) * (" ATTRIB(VERTEX_ATTRIB_RADIUS) ".x * " VPARAM(VERTEX_PARAM_VERTEX_SCALE_OFFSET) ".x) + " ATTRIB(VERTEX_ATTRIB_POSITION0) ".xy;\n"
	"opos.z = " ATTRIB(VERTEX_ATTRIB_POSITION0) ".z * " VPARAM(VERTEX_PARAM_VERTEX_SCALE_OFFSET) ".z;\n"
};

const VertexSnippet VertexShader::calculatePolyboardNormal =
{
	'POLY', kVertexSnippetPositionFlag,

	FLOAT3 " opos = cross(normalize(cdir), " ATTRIB(VERTEX_ATTRIB_TANGENT) ".xyz);\n"
	"temp.w = dot(opos, opos);\n"
	"temp.y = " RSQRT "(temp.w);\n"
	"temp.x = temp.w * temp.y;\n"
	"temp.z = temp.x * (temp.x * (temp.x * 132.741 - 130.37) + 34.6667) + 1.0;\n"
	"opos *= min(temp.y, temp.z);\n"
};

const VertexSnippet VertexShader::calculateLinearPolyboardNormal =
{
	'LPOL', kVertexSnippetPositionFlag,

	FLOAT3 " opos = normalize(cross(cdir, " ATTRIB(VERTEX_ATTRIB_TANGENT) ".xyz));\n"
};

const VertexSnippet VertexShader::calculatePolyboardPosition =
{
	'CPBP', 0,

	"opos = opos * " ATTRIB(VERTEX_ATTRIB_TANGENT) ".w + " ATTRIB(VERTEX_ATTRIB_POSITION0) ".xyz;\n"
};

const VertexSnippet VertexShader::calculatePolyboardScalePosition =
{
	'CPSP', 0,

	"opos = opos * vrtx.w + vrtx.xyz;\n"
};

const VertexSnippet VertexShader::calculateScalePosition =
{
	'SPOS', kVertexSnippetPositionFlag,

	FLOAT3 " opos = " ATTRIB(VERTEX_ATTRIB_POSITION0) ".xyz * " VPARAM(VERTEX_PARAM_VERTEX_SCALE_OFFSET) ".xyz;\n"
};

const VertexSnippet VertexShader::calculateScaleOffsetPosition =
{
	'SOPS', kVertexSnippetPositionFlag,

	FLOAT3 " opos = " ATTRIB(VERTEX_ATTRIB_OFFSET) ".xyz * " VPARAM(VERTEX_PARAM_VERTEX_SCALE_OFFSET) ".w + " ATTRIB(VERTEX_ATTRIB_POSITION0) ".xyz * " VPARAM(VERTEX_PARAM_VERTEX_SCALE_OFFSET) ".xyz;\n"
};

const VertexSnippet VertexShader::calculateExpandNormalPosition =
{
	'NEPT', kVertexSnippetPositionFlag,

	FLOAT3 " opos = %NRML * " VPARAM(VERTEX_PARAM_VERTEX_SCALE_OFFSET) ".x + " ATTRIB(VERTEX_ATTRIB_POSITION0) ".xyz;\n"
};

const VertexSnippet VertexShader::calculateTerrainBorderPosition =
{
	'TRBP', kVertexSnippetPositionFlag,

	FLOAT3 " prim = " FLOAT3 "(" ATTRIB(VERTEX_ATTRIB_COLOR2) ".x >= 0.75, " ATTRIB(VERTEX_ATTRIB_COLOR2) ".y >= 0.75, " ATTRIB(VERTEX_ATTRIB_COLOR2) ".z >= 0.75);\n"
	"temp.xyz = " FLOAT3 "(" ATTRIB(VERTEX_ATTRIB_COLOR2) ".x < 0.25, " ATTRIB(VERTEX_ATTRIB_COLOR2) ".y < 0.25, " ATTRIB(VERTEX_ATTRIB_COLOR2) ".z < 0.25);\n"

	"prim = prim * " VPARAM(VERTEX_PARAM_TERRAIN_PARAMETER0) ".xyz + temp.xyz * " VPARAM(VERTEX_PARAM_TERRAIN_PARAMETER1) ".xyz;\n"
	"prim.x = max(max(prim.x, prim.y), prim.z);\n"
	FLOAT3 " opos = " ATTRIB(VERTEX_ATTRIB_POSITION0) ".xyz * prim.x + " ATTRIB(VERTEX_ATTRIB_POSITION1) ".xyz * (1.0 - prim.x);\n"
};

const VertexSnippet VertexShader::calculateWaterHeightPosition =
{
	'WHTP', kVertexSnippetPositionFlag,

	FLOAT3 " opos = " FLOAT3 "(" ATTRIB(VERTEX_ATTRIB_POSITION0) ".xy, " ATTRIB(VERTEX_ATTRIB_NORMAL) ".w);\n"
};

const VertexSnippet VertexShader::texcoordVertexTransform =
{
	'TXVT', 0,

	RESULT_POSITION ".x = dot(" ATTRIB(VERTEX_ATTRIB_TEXTURE0) ".xyz, " VPARAM(VERTEX_PARAM_MATRIX_MVP0) ".xyz) + " VPARAM(VERTEX_PARAM_MATRIX_MVP0) ".w;\n"
	RESULT_POSITION ".y = dot(" ATTRIB(VERTEX_ATTRIB_TEXTURE0) ".xyz, " VPARAM(VERTEX_PARAM_MATRIX_MVP1) ".xyz) + " VPARAM(VERTEX_PARAM_MATRIX_MVP1) ".w;\n"
	RESULT_POSITION ".z = dot(" ATTRIB(VERTEX_ATTRIB_TEXTURE0) ".xyz, " VPARAM(VERTEX_PARAM_MATRIX_MVP2) ".xyz) + " VPARAM(VERTEX_PARAM_MATRIX_MVP2) ".w;\n"
	RESULT_POSITION ".w = dot(" ATTRIB(VERTEX_ATTRIB_TEXTURE0) ".xyz, " VPARAM(VERTEX_PARAM_MATRIX_MVP3) ".xyz) + " VPARAM(VERTEX_PARAM_MATRIX_MVP3) ".w;\n"
};

const VertexSnippet VertexShader::extractGlowBloomTransform =
{
	'EXGB', 0,

	RESULT_POSITION " = " ATTRIB(VERTEX_ATTRIB_POSITION0) ";\n"
	RESULT_TEXCOORD0 ".xy = (" ATTRIB(VERTEX_ATTRIB_POSITION0) ".xy * 0.5F + 0.5F) * " UPARAM(UNIVERSAL_PARAM_VIEWPORT_TRANSFORM) ".xy + " UPARAM(UNIVERSAL_PARAM_VIEWPORT_TRANSFORM) ".zw;\n"
};

const VertexSnippet VertexShader::postProcessTransform =
{
	'PSTP', 0,

	RESULT_POSITION " = " ATTRIB(VERTEX_ATTRIB_POSITION0) ";\n"
	RESULT_TEXCOORD0 ".xy = (" ATTRIB(VERTEX_ATTRIB_POSITION0) ".xy + 1.0) * 0.25 * " UPARAM(UNIVERSAL_PARAM_VIEWPORT_TRANSFORM) ".xy;\n"
};

const VertexSnippet VertexShader::outputNormalTexcoord =
{
	'NMTX', 0,

	RESULT_TEXCOORD0 ".xyz = " ATTRIB(VERTEX_ATTRIB_NORMAL) ".xyz;\n"
};

const VertexSnippet VertexShader::outputPrimaryColor =
{
	'PCOL', 0,

	RESULT_COLOR0 " = " ATTRIB(VERTEX_ATTRIB_COLOR0) ";\n"
};

const VertexSnippet VertexShader::outputSecondaryColor =
{
	'SCOL', 0,

	RESULT_COLOR1 " = " ATTRIB(VERTEX_ATTRIB_COLOR1) ";\n"
};

const VertexSnippet VertexShader::outputPointSize =
{
	'PSIZ', 0,

	#if !C4PS3

		RESULT_POINTSIZE " = " ATTRIB(VERTEX_ATTRIB_RADIUS) ".x / (dot(%OPOS, " UPARAM(UNIVERSAL_PARAM_POINT_CAMERA_PLANE) ".xyz) + " UPARAM(UNIVERSAL_PARAM_POINT_CAMERA_PLANE) ".w);\n"

	#else //[ PS3

		// -- PS3 code hidden --

	#endif //]
};

const VertexSnippet VertexShader::outputInfinitePointSize =
{
	'IPSZ', 0,

	RESULT_POINTSIZE " = " ATTRIB(VERTEX_ATTRIB_RADIUS) ".x * " UPARAM(UNIVERSAL_PARAM_RADIUS_POINT_FACTOR) ".x;\n"
};

const VertexSnippet VertexShader::copyPrimaryTexcoord0 =
{
	'CPT0', 0,

	"$TEX0 = " ATTRIB(VERTEX_ATTRIB_TEXTURE0) ".xy;\n"
};

const VertexSnippet VertexShader::copyPrimaryTexcoord1 =
{
	'CPT1', 0,

	"$TEX1 = " ATTRIB(VERTEX_ATTRIB_TEXTURE0) ".xy;\n"
};

const VertexSnippet VertexShader::copySecondaryTexcoord1 =
{
	'CST1', 0,

	"$TEX1 = " ATTRIB(VERTEX_ATTRIB_TEXTURE1) ".xy;\n"
};

const VertexSnippet VertexShader::transformPrimaryTexcoord0 =
{
	'TPT0', 0,

	"$TEX0 = " ATTRIB(VERTEX_ATTRIB_TEXTURE0) ".xy * " VPARAM(VERTEX_PARAM_TEXCOORD_TRANSFORM0) ".xy + " VPARAM(VERTEX_PARAM_TEXCOORD_TRANSFORM0) ".zw;\n"
};

const VertexSnippet VertexShader::transformPrimaryTexcoord1 =
{
	'TPT1', 0,

	"$TEX1 = " ATTRIB(VERTEX_ATTRIB_TEXTURE0) ".xy * " VPARAM(VERTEX_PARAM_TEXCOORD_TRANSFORM1) ".xy + " VPARAM(VERTEX_PARAM_TEXCOORD_TRANSFORM1) ".zw;\n"
};

const VertexSnippet VertexShader::transformSecondaryTexcoord1 =
{
	'TST1', 0,

	"$TEX1 = " ATTRIB(VERTEX_ATTRIB_TEXTURE1) ".xy * " VPARAM(VERTEX_PARAM_TEXCOORD_TRANSFORM1) ".xy + " VPARAM(VERTEX_PARAM_TEXCOORD_TRANSFORM1) ".zw;\n"
};

const VertexSnippet VertexShader::animatePrimaryTexcoord0 =
{
	'APT0', 0,

	"$TEX0 = " VPARAM(VERTEX_PARAM_TEXCOORD_VELOCITY0) ".xy * " UPARAM(UNIVERSAL_PARAM_SHADER_TIME) ".y + " ATTRIB(VERTEX_ATTRIB_TEXTURE0) ".xy;\n"
};

const VertexSnippet VertexShader::animatePrimaryTexcoord1 =
{
	'APT1', 0,

	"$TEX1 = " VPARAM(VERTEX_PARAM_TEXCOORD_VELOCITY1) ".xy * " UPARAM(UNIVERSAL_PARAM_SHADER_TIME) ".y + " ATTRIB(VERTEX_ATTRIB_TEXTURE0) ".xy;\n"
};

const VertexSnippet VertexShader::animateSecondaryTexcoord1 =
{
	'AST1', 0,

	"$TEX1 = " VPARAM(VERTEX_PARAM_TEXCOORD_VELOCITY1) ".xy * " UPARAM(UNIVERSAL_PARAM_SHADER_TIME) ".y + " ATTRIB(VERTEX_ATTRIB_TEXTURE1) ".xy;\n"
};

const VertexSnippet VertexShader::transformAnimatePrimaryTexcoord0 =
{
	'XPT0', 0,

	"temp.xy = " ATTRIB(VERTEX_ATTRIB_TEXTURE0) ".xy * " VPARAM(VERTEX_PARAM_TEXCOORD_TRANSFORM0) ".xy + " VPARAM(VERTEX_PARAM_TEXCOORD_TRANSFORM0) ".zw;\n"
	"$TEX0 = " VPARAM(VERTEX_PARAM_TEXCOORD_VELOCITY0) ".xy * " UPARAM(UNIVERSAL_PARAM_SHADER_TIME) ".y + temp.xy;\n"
};

const VertexSnippet VertexShader::transformAnimatePrimaryTexcoord1 =
{
	'XPT1', 0,

	"temp.xy = " ATTRIB(VERTEX_ATTRIB_TEXTURE0) ".xy * " VPARAM(VERTEX_PARAM_TEXCOORD_TRANSFORM1) ".xy + " VPARAM(VERTEX_PARAM_TEXCOORD_TRANSFORM1) ".zw;\n"
	"$TEX1 = " VPARAM(VERTEX_PARAM_TEXCOORD_VELOCITY1) ".xy * " UPARAM(UNIVERSAL_PARAM_SHADER_TIME) ".y + temp.xy;\n"
};

const VertexSnippet VertexShader::transformAnimateSecondaryTexcoord1 =
{
	'XST1', 0,

	"temp.xy = " ATTRIB(VERTEX_ATTRIB_TEXTURE1) ".xy * " VPARAM(VERTEX_PARAM_TEXCOORD_TRANSFORM1) ".xy + " VPARAM(VERTEX_PARAM_TEXCOORD_TRANSFORM1) ".zw;\n"
	"$TEX1 = " VPARAM(VERTEX_PARAM_TEXCOORD_VELOCITY1) ".xy * " UPARAM(UNIVERSAL_PARAM_SHADER_TIME) ".y + temp.xy;\n"
};

const VertexSnippet VertexShader::generateTexcoord0 =
{
	'GTX0', 0,

	"$TEX0 = " ATTRIB(VERTEX_ATTRIB_POSITION0) ".xy * " VPARAM(VERTEX_PARAM_TEXCOORD_GENERATE) ".xy;\n"
};

const VertexSnippet VertexShader::generateTexcoord1 =
{
	'GTX1', 0,

	"$TEX1 = " ATTRIB(VERTEX_ATTRIB_POSITION0) ".xy * " VPARAM(VERTEX_PARAM_TEXCOORD_GENERATE) ".xy;\n"
};

const VertexSnippet VertexShader::generateTransformTexcoord0 =
{
	'GTT0', 0,

	"$TEX0 = " ATTRIB(VERTEX_ATTRIB_POSITION0) ".xy * " VPARAM(VERTEX_PARAM_TEXCOORD_TRANSFORM0) ".xy + " VPARAM(VERTEX_PARAM_TEXCOORD_TRANSFORM0) ".zw;\n"
};

const VertexSnippet VertexShader::generateTransformTexcoord1 =
{
	'GTT1', 0,

	"$TEX1 = " ATTRIB(VERTEX_ATTRIB_POSITION0) ".xy * " VPARAM(VERTEX_PARAM_TEXCOORD_TRANSFORM1) ".xy + " VPARAM(VERTEX_PARAM_TEXCOORD_TRANSFORM1) ".zw;\n"
};

const VertexSnippet VertexShader::generateBaseTexcoord =
{
	'GBTC', 0,

	FLOAT2 " btex = " ATTRIB(VERTEX_ATTRIB_POSITION0) ".xy * " VPARAM(VERTEX_PARAM_TEXCOORD_GENERATE) ".xy;\n"
};

const VertexSnippet VertexShader::generateAnimateTexcoord0 =
{
	'GAT0', 0,

	"$TEX0 = " VPARAM(VERTEX_PARAM_TEXCOORD_VELOCITY0) ".xy * " UPARAM(UNIVERSAL_PARAM_SHADER_TIME) ".y + btex;\n"
};

const VertexSnippet VertexShader::generateAnimateTexcoord1 =
{
	'GAT1', 0,

	"$TEX1 = " VPARAM(VERTEX_PARAM_TEXCOORD_VELOCITY1) ".xy * " UPARAM(UNIVERSAL_PARAM_SHADER_TIME) ".y + btex;\n"
};

const VertexSnippet VertexShader::generateTransformAnimateTexcoord0 =
{
	'GXT0', 0,

	"temp.xy = " ATTRIB(VERTEX_ATTRIB_POSITION0) ".xy * " VPARAM(VERTEX_PARAM_TEXCOORD_TRANSFORM0) ".xy + " VPARAM(VERTEX_PARAM_TEXCOORD_TRANSFORM0) ".zw;\n"
	"$TEX0 = " VPARAM(VERTEX_PARAM_TEXCOORD_VELOCITY0) ".xy * " UPARAM(UNIVERSAL_PARAM_SHADER_TIME) ".y + temp.xy;\n"
};

const VertexSnippet VertexShader::generateTransformAnimateTexcoord1 =
{
	'GXT1', 0,

	"temp.xy = " ATTRIB(VERTEX_ATTRIB_POSITION0) ".xy * " VPARAM(VERTEX_PARAM_TEXCOORD_TRANSFORM1) ".xy + " VPARAM(VERTEX_PARAM_TEXCOORD_TRANSFORM1) ".zw;\n"
	"$TEX1 = " VPARAM(VERTEX_PARAM_TEXCOORD_VELOCITY1) ".xy * " UPARAM(UNIVERSAL_PARAM_SHADER_TIME) ".y + temp.xy;\n"
};

const VertexSnippet VertexShader::normalizeNormal =
{
	'NMZN', kVertexSnippetNormalFlag,

	FLOAT3 " nrml = normalize(" ATTRIB(VERTEX_ATTRIB_NORMAL) ".xyz);\n"
};

const VertexSnippet VertexShader::normalizeTangent =
{
	'NMZT', kVertexSnippetTangentFlag,

	FLOAT3 " tang = normalize(" ATTRIB(VERTEX_ATTRIB_TANGENT) ".xyz);\n"
};

const VertexSnippet VertexShader::orthonormalizeTangent =
{
	'ONMT', kVertexSnippetTangentFlag,

	FLOAT3 " tang = normalize(" ATTRIB(VERTEX_ATTRIB_TANGENT) ".xyz - %NRML * dot(%NRML, " ATTRIB(VERTEX_ATTRIB_TANGENT) ".xyz));\n"
};

const VertexSnippet VertexShader::generateTangent =
{
	'NMTG', kVertexSnippetTangentFlag,

	"temp.w = " RSQRT "(%NRML.x * %NRML.x + %NRML.z * %NRML.z);\n"
	FLOAT3 " tang = " FLOAT3 "(%NRML.z * temp.w, 0.0, %NRML.x * -temp.w);\n"
};

const VertexSnippet VertexShader::generateImpostorFrame =
{
	'IFRM', kVertexSnippetNormalFlag | kVertexSnippetTangentFlag,

	FLOAT3 " nrml = " FLOAT3 "(normalize(" VPARAM(VERTEX_PARAM_CAMERA_POSITION) ".xy - " ATTRIB(VERTEX_ATTRIB_POSITION0) ".xy), 0.0);\n"
	FLOAT3 " tang = " FLOAT3 "(-nrml.y, nrml.x, 0.0);\n"
	FLOAT3 " btng = " FLOAT3 "(0.0, 0.0, 1.0);\n"
};

const VertexSnippet VertexShader::calculateBitangent =
{
	'CBTN', 0,

	FLOAT3 " btng = cross(%NRML, %TANG);\n"
};

const VertexSnippet VertexShader::adjustBitangent =
{
	'ABTN', 0,

	"btng *= " ATTRIB(VERTEX_ATTRIB_TANGENT) ".w;\n"
};

const VertexSnippet VertexShader::vertexSnippet[kVertexSnippetCount] =
{
	// kVertexSnippetOutputObjectPosition
	{
		'POSI', 0,

		"$POSI.xyz = %OPOS;\n"
	},

	// kVertexSnippetOutputObjectNormal
	{
		'NRML', 0,

		"$NRML.xyz = %NRML;\n"
	},

	// kVertexSnippetOutputObjectTangent
	{
		'TANG', 0,

		"$TANG.xyz = %TANG;\n"
	},

	// kVertexSnippetOutputObjectBitangent
	{
		'BTNG', 0,

		"$BTNG.xyz = btng;\n"
	},

	// kVertexSnippetOutputWorldPosition
	{
		'WPOS', 0,

		"$WPOS.x = dot(" VPARAM(VERTEX_PARAM_MATRIX_WORLD0) ".xyz, %OPOS) + " VPARAM(VERTEX_PARAM_MATRIX_WORLD0) ".w;\n"
		"$WPOS.y = dot(" VPARAM(VERTEX_PARAM_MATRIX_WORLD1) ".xyz, %OPOS) + " VPARAM(VERTEX_PARAM_MATRIX_WORLD1) ".w;\n"
		"$WPOS.z = dot(" VPARAM(VERTEX_PARAM_MATRIX_WORLD2) ".xyz, %OPOS) + " VPARAM(VERTEX_PARAM_MATRIX_WORLD2) ".w;\n"
	},

	// kVertexSnippetOutputWorldNormal
	{
		'WNRM', 0,

		"$WNRM.x = dot(" VPARAM(VERTEX_PARAM_MATRIX_WORLD0) ".xyz, %NRML);\n"
		"$WNRM.y = dot(" VPARAM(VERTEX_PARAM_MATRIX_WORLD1) ".xyz, %NRML);\n"
		"$WNRM.z = dot(" VPARAM(VERTEX_PARAM_MATRIX_WORLD2) ".xyz, %NRML);\n"
	},

	// kVertexSnippetOutputWorldTangent
	{
		'WTAN', 0,

		"$WTAN.x = dot(" VPARAM(VERTEX_PARAM_MATRIX_WORLD0) ".xyz, %TANG);\n"
		"$WTAN.y = dot(" VPARAM(VERTEX_PARAM_MATRIX_WORLD1) ".xyz, %TANG);\n"
		"$WTAN.z = dot(" VPARAM(VERTEX_PARAM_MATRIX_WORLD2) ".xyz, %TANG);\n"
	},

	// kVertexSnippetOutputWorldBitangent
	{
		'WBTN', 0,

		"$WBTN.x = dot(" VPARAM(VERTEX_PARAM_MATRIX_WORLD0) ".xyz, btng);\n"
		"$WBTN.y = dot(" VPARAM(VERTEX_PARAM_MATRIX_WORLD1) ".xyz, btng);\n"
		"$WBTN.z = dot(" VPARAM(VERTEX_PARAM_MATRIX_WORLD2) ".xyz, btng);\n"
	},

	// kVertexSnippetOutputCameraNormal
	{
		'NRMC', 0,

		"$NRMC.x = dot(" VPARAM(VERTEX_PARAM_MATRIX_CAMERA0) ".xyz, %NRML);\n"
		"$NRMC.y = dot(" VPARAM(VERTEX_PARAM_MATRIX_CAMERA1) ".xyz, %NRML);\n"
		"$NRMC.z = dot(" VPARAM(VERTEX_PARAM_MATRIX_CAMERA2) ".xyz, %NRML);\n"
	},

	// kVertexSnippetOutputVertexGeometry
	{
		'GEOM', 0,

		"$GEOM.xy = " ATTRIB(VERTEX_ATTRIB_POSITION1) ".xy;\n"
		"$GEOM.z = " ATTRIB(VERTEX_ATTRIB_POSITION1) ".z - %OPOS.z;\n"
	},

	// kVertexSnippetOutputObjectInfiniteLightDirection
	{
		'OOIL', 0,

		"$OLDR.xyz = " VPARAM(VERTEX_PARAM_LIGHT_POSITION) ".xyz;\n"
	},

	// kVertexSnippetCalculateObjectPointLightDirection
	{
		'COPL', 0,

		FLOAT3 " ldir = " VPARAM(VERTEX_PARAM_LIGHT_POSITION) ".xyz - %OPOS;\n"
	},

	// kVertexSnippetOutputObjectPointLightDirection
	{
		'OOPL', 0,

		"$OLDR.xyz = ldir;\n"
	},

	// kVertexSnippetOutputTangentInfiniteLightDirection
	{
		'OTIL', 0,

		"$LDIR.x = dot(%TANG, " VPARAM(VERTEX_PARAM_LIGHT_POSITION) ".xyz);\n"
		"$LDIR.y = dot(btng, " VPARAM(VERTEX_PARAM_LIGHT_POSITION) ".xyz);\n"
		"$LDIR.z = dot(%NRML, " VPARAM(VERTEX_PARAM_LIGHT_POSITION) ".xyz);\n"
	},

	// kVertexSnippetOutputTangentPointLightDirection
	{
		'OTPL', 0,

		"$LDIR.x = dot(%TANG, ldir);\n"
		"$LDIR.y = dot(btng, ldir);\n"
		"$LDIR.z = dot(%NRML, ldir);\n"
	},

	// kVertexSnippetCalculateObjectViewDirection
	{
		'COVD', 0,

		FLOAT3 " vdir = " VPARAM(VERTEX_PARAM_CAMERA_POSITION) ".xyz - %OPOS;\n"
	},

	// kVertexSnippetOutputObjectViewDirection
	{
		'OOVD', 0,

		"$OVDR.xyz = vdir;\n"
	},

	// kVertexSnippetOutputTangentViewDirection
	{
		'OTVD', 0,

		"$VDIR.x = dot(%TANG, vdir);\n"
		"$VDIR.y = dot(btng, vdir);\n"
		"$VDIR.z = dot(%NRML, vdir);\n"
	},

	// kVertexSnippetOutputBillboardInfiniteLightDirection
	{
		'OBIL', 0,

		"temp.xyz = normalize(vdir);\n"
		"$LDIR.x = dot(tang, " VPARAM(VERTEX_PARAM_LIGHT_POSITION) ".xyz);\n"
		"$LDIR.y = dot(btng, " VPARAM(VERTEX_PARAM_LIGHT_POSITION) ".xyz);\n"
		"$LDIR.z = dot(temp, " VPARAM(VERTEX_PARAM_LIGHT_POSITION) ".xyz);\n"
	},

	// kVertexSnippetOutputBillboardPointLightDirection
	{
		'OBPL', 0,

		"temp.xyz = normalize(vdir);\n"
		"$LDIR.x = dot(tang, ldir);\n"
		"$LDIR.y = dot(btng, ldir);\n"
		"$LDIR.z = dot(temp, ldir);\n"
	},

	// kVertexSnippetCalculateTerrainTangentData
	{
		'CTTD', 0,

		FLOAT4 " ttnd;\n"

		"ttnd.xyz = %NRML * %NRML;\n"
		"ttnd = ttnd.x + ttnd.yzyz;\n"
		"ttnd.xy = max(ttnd.xy, 0.00390625);\n"
		"ttnd.x = " RSQRT "(ttnd.x);\n"
		"ttnd.y = " RSQRT "(ttnd.y);\n"
	},

	// kVertexSnippetOutputTerrainInfiniteLightDirection
	{
		'TLDI', 0,

		FLOAT3 " ltan = cross(" VPARAM(VERTEX_PARAM_LIGHT_POSITION) ".xyz, %NRML);\n"
		"temp.xy = %NRML.xz * " VPARAM(VERTEX_PARAM_LIGHT_POSITION) ".yx - %NRML.yx * " VPARAM(VERTEX_PARAM_LIGHT_POSITION) ".xz;\n"
		"temp.zw = %NRML.xz * ltan.yx - %NRML.yx * ltan.xz;\n"
		"$TLDR.xy = temp.xz * ttnd.x;\n"
		"$TL2X = temp.y * ttnd.y;\n"
		"$TL2Y = temp.w * ttnd.y;\n"
		"$TLDR.z = dot(%NRML, " VPARAM(VERTEX_PARAM_LIGHT_POSITION) ".xyz);\n"
	},

	// kVertexSnippetOutputTerrainPointLightDirection
	{
		'TLDP', 0,

		FLOAT3 " tldp = " VPARAM(VERTEX_PARAM_LIGHT_POSITION) ".xyz - %OPOS;\n"
		FLOAT3 " ltan = cross(tldp, %NRML);\n"
		"temp.xy = %NRML.xz * tldp.yx - %NRML.yx * tldp.xz;\n"
		"temp.zw = %NRML.xz * ltan.yx - %NRML.yx * ltan.xz;\n"
		"$TLDR.xy = temp.xz * ttnd.x;\n"
		"$TL2X = temp.y * ttnd.y;\n"
		"$TL2Y = temp.w * ttnd.y;\n"
		"$TLDR.z = dot(%NRML, tldp);\n"
	},

	// kVertexSnippetOutputTerrainViewDirection
	{
		'TVDR', 0,

		FLOAT3 " tvdp = " VPARAM(VERTEX_PARAM_CAMERA_POSITION) ".xyz - %OPOS;\n"
		FLOAT3 " vtan = cross(tvdp, %NRML);\n"
		"temp.xy = %NRML.xz * tvdp.yx - %NRML.yx * tvdp.xz;\n"
		"temp.zw = %NRML.xz * vtan.yx - %NRML.yx * vtan.xz;\n"
		"$TVDR.xy = temp.xz * ttnd.x;\n"
		"$TV2X = temp.y * ttnd.y;\n"
		"$TV2Y = temp.w * ttnd.y;\n"
		"$TVDR.z = dot(%NRML, tvdp);\n"
	},

	// kVertexSnippetOutputTerrainWorldTangentFrame
	{
		'TWTF', 0,

		FLOAT4 " wtan;\n"

		"$TWNM.x = dot(" VPARAM(VERTEX_PARAM_MATRIX_WORLD0) ".xyz, %NRML);\n"
		"$TWNM.y = dot(" VPARAM(VERTEX_PARAM_MATRIX_WORLD1) ".xyz, %NRML);\n"
		"$TWNM.z = dot(" VPARAM(VERTEX_PARAM_MATRIX_WORLD2) ".xyz, %NRML);\n"

		"wtan.xw = -%NRML.yx * ttnd.xy;\n"
		"wtan.yz = %NRML.xz * ttnd.xy;\n"

		"$TWT1.x = " VPARAM(VERTEX_PARAM_MATRIX_WORLD0) ".x * wtan.x + " VPARAM(VERTEX_PARAM_MATRIX_WORLD0) ".y * wtan.y;\n"
		"$TWT1.y = " VPARAM(VERTEX_PARAM_MATRIX_WORLD1) ".x * wtan.x + " VPARAM(VERTEX_PARAM_MATRIX_WORLD1) ".y * wtan.y;\n"
		"$TWT1.z = " VPARAM(VERTEX_PARAM_MATRIX_WORLD2) ".x * wtan.x + " VPARAM(VERTEX_PARAM_MATRIX_WORLD2) ".y * wtan.y;\n"

		"$TWT2.x = " VPARAM(VERTEX_PARAM_MATRIX_WORLD0) ".x * wtan.z + " VPARAM(VERTEX_PARAM_MATRIX_WORLD0) ".z * wtan.w;\n"
		"$TWT2.y = " VPARAM(VERTEX_PARAM_MATRIX_WORLD1) ".x * wtan.z + " VPARAM(VERTEX_PARAM_MATRIX_WORLD1) ".z * wtan.w;\n"
		"$TWT2.z = " VPARAM(VERTEX_PARAM_MATRIX_WORLD2) ".x * wtan.z + " VPARAM(VERTEX_PARAM_MATRIX_WORLD2) ".z * wtan.w;\n"
	},

	// kVertexSnippetOutputRawTexcoords
	{
		'RTXC', 0,

		"$RTXC.xyz = " ATTRIB(VERTEX_ATTRIB_TEXTURE0) ".xyz;\n"
	},

	// kVertexSnippetOutputTerrainTexcoords
	{
		'TERA', 0,

		"$TERA.xyz = %OPOS * " VPARAM(VERTEX_PARAM_TERRAIN_TEXCOORD_SCALE) ".x;\n"
	},

	// kVertexSnippetOutputImpostorTexcoords
	{
		'CITX', 0,

		FLOAT2 " itmp;\n"

		FLOAT2 " idir = " VPARAM(VERTEX_PARAM_CAMERA_POSITION) ".xy - " ATTRIB(VERTEX_ATTRIB_POSITION0) ".xy * " VPARAM(VERTEX_PARAM_CAMERA_POSITION) ".w;\n"

		"temp.x = (" ATTRIB(VERTEX_ATTRIB_RADIUS) ".y * idir.x + " ATTRIB(VERTEX_ATTRIB_RADIUS) ".z * idir.y) * " RSQRT "(dot(idir, idir));\n"
		"temp.x = 2.0 - temp.x - temp.x * temp.x * temp.x;\n"

		"itmp.x = " ATTRIB(VERTEX_ATTRIB_RADIUS) ".y * idir.y - " ATTRIB(VERTEX_ATTRIB_RADIUS) ".z * idir.x;\n"

		#if C4OPENGL

			"itmp.y = float(itmp.x < 0.0);\n"

		#else

			"itmp.y = (itmp.x < 0.0);\n"

		#endif

		"temp.x += (8.0 - temp.x * 2.0) * itmp.y;\n"

		"temp.w = " FRAC "(temp.x);\n"
		"$IBLD = temp.w;\n"
		"temp.x = temp.x - temp.w + " ATTRIB(VERTEX_ATTRIB_TEXTURE0) ".x;\n"
		"$IMPT.xy = " ATTRIB(VERTEX_ATTRIB_TEXTURE0) ".xy;\n"
		"$IMPT.z = temp.x * 0.125;\n"
		"$IMPT.w = temp.x * 0.125 + 0.125;\n"
	},

	// kVertexSnippetOutputImpostorTransitionBlend
	{
		'IXBL', 0,

		"temp.xy = " ATTRIB(VERTEX_ATTRIB_POSITION0) ".xy - " UPARAM(UNIVERSAL_PARAM_IMPOSTOR_CAMERA_POSITION) ".xy;\n"
		"$IXBL = length(temp.xy) * " VPARAM(VERTEX_PARAM_IMPOSTOR_TRANSITION) ".x + " VPARAM(VERTEX_PARAM_IMPOSTOR_TRANSITION) ".y;\n"
	},

	// kVertexSnippetOutputGeometryImpostorTexcoords
	{
		'GITX', 0,

		"$GITX:x = dot(" ATTRIB(VERTEX_ATTRIB_POSITION0) ".xyz, " VPARAM(VERTEX_PARAM_IMPOSTOR_PLANE_S) ".xyz) + " VPARAM(VERTEX_PARAM_IMPOSTOR_PLANE_S) ".w;\n"
		"$GITX:y = dot(" ATTRIB(VERTEX_ATTRIB_POSITION0) ".xyz, " VPARAM(VERTEX_PARAM_IMPOSTOR_PLANE_T) ".xyz) + " VPARAM(VERTEX_PARAM_IMPOSTOR_PLANE_T) ".w;\n"
	},

	// kVertexSnippetOutputPaintTexcoords
	{
		'PTXC', 0,

		"$PTXC:x = dot(" ATTRIB(VERTEX_ATTRIB_POSITION0) ".xyz, " VPARAM(VERTEX_PARAM_PAINT_PLANE_S) ".xyz) + " VPARAM(VERTEX_PARAM_PAINT_PLANE_S) ".w;\n"
		"$PTXC:y = dot(" ATTRIB(VERTEX_ATTRIB_POSITION0) ".xyz, " VPARAM(VERTEX_PARAM_PAINT_PLANE_T) ".xyz) + " VPARAM(VERTEX_PARAM_PAINT_PLANE_T) ".w;\n"
	},

	// kVertexSnippetOutputFireTexcoords
	{
		'FIRE', 0,

		"$FIRE.xy = " ATTRIB(VERTEX_ATTRIB_TEXTURE0) ".xy;\n"
		"$FIRE.z = " ATTRIB(VERTEX_ATTRIB_TEXTURE0) ".y * " VPARAM(VERTEX_PARAM_FIRE_PARAMS) ".x;\n"

		"temp.xy = " ATTRIB(VERTEX_ATTRIB_TEXTURE0) ".xy + " ATTRIB(VERTEX_ATTRIB_TEXTURE0) ".zw;\n"
		"$FIR1 = " VPARAM(VERTEX_PARAM_TEXCOORD_VELOCITY0) " * " UPARAM(UNIVERSAL_PARAM_SHADER_TIME) ".y + temp.xyxy;\n"
		"$FIR2 = " VPARAM(VERTEX_PARAM_TEXCOORD_VELOCITY1) ".xy * " UPARAM(UNIVERSAL_PARAM_SHADER_TIME) ".y + temp.xy;\n"
	},

	// kVertexSnippetOutputFireArrayTexcoords
	{
		'FIRA', 0,

		"$FIRE.xy = " ATTRIB(VERTEX_ATTRIB_TEXTURE0) ".xy;\n"
		"$FIRE.z = " ATTRIB(VERTEX_ATTRIB_TEXTURE0) ".y * " ATTRIB(VERTEX_ATTRIB_RADIUS) ".y;\n"

		"temp.xy = " ATTRIB(VERTEX_ATTRIB_TEXTURE0) ".xy + " ATTRIB(VERTEX_ATTRIB_TEXTURE0) ".zw;\n"
		"$FIR1 = " ATTRIB(VERTEX_ATTRIB_TEXTURE1) " * " UPARAM(UNIVERSAL_PARAM_SHADER_TIME) ".y + temp.xyxy;\n"
		"$FIR2 = " ATTRIB(VERTEX_ATTRIB_RADIUS) ".zw * " UPARAM(UNIVERSAL_PARAM_SHADER_TIME) ".y + temp.xy;\n"
	},

	// kVertexSnippetCalculateCameraDistance
	{
		'CAMD', 0,

		"temp.w = dot(vdir, vdir);\n"
		"float dist = " RSQRT "(temp.w) * temp.w;\n"
	},

	// kVertexSnippetOutputCameraWarpFunction
	{
		'WARP', 0,

		"$WARP.x = dot(" VPARAM(VERTEX_PARAM_CAMERA_RIGHT) ".xyz, %NRML);\n"
		"$WARP.y = dot(" VPARAM(VERTEX_PARAM_CAMERA_DOWN) ".xyz, %NRML);\n"

		"temp.x = dist * 8.0;\n"
		"temp.w = temp.x / (temp.x * dist + 4.0);\n"
		"$WARP.z = temp.w * " VPARAM(VERTEX_PARAM_REFLECTION_SCALE) ".x;\n"
		"$WARP.w = temp.w * " VPARAM(VERTEX_PARAM_REFRACTION_SCALE) ".x;\n"
	},

	// kVertexSnippetOutputCameraBumpWarpFunction
	{
		'BWRP', 0,

		"$RGHT.x = dot(" VPARAM(VERTEX_PARAM_CAMERA_RIGHT) ".xyz, %TANG);\n"
		"$RGHT.y = dot(" VPARAM(VERTEX_PARAM_CAMERA_RIGHT) ".xyz, btng);\n"
		"$RGHT.z = dot(" VPARAM(VERTEX_PARAM_CAMERA_RIGHT) ".xyz, %NRML);\n"
		"$DOWN.x = dot(" VPARAM(VERTEX_PARAM_CAMERA_DOWN) ".xyz, %TANG);\n"
		"$DOWN.y = dot(" VPARAM(VERTEX_PARAM_CAMERA_DOWN) ".xyz, btng);\n"
		"$DOWN.z = dot(" VPARAM(VERTEX_PARAM_CAMERA_DOWN) ".xyz, %NRML);\n"

		"temp.x = dist * 8.0;\n"
		"temp.w = temp.x / (temp.x * dist + 4.0);\n"
		"$RGHT.w = temp.w * " VPARAM(VERTEX_PARAM_REFLECTION_SCALE) ".x;\n"
		"$DOWN.w = temp.w * " VPARAM(VERTEX_PARAM_REFRACTION_SCALE) ".x;\n"
	},

	// kVertexSnippetOutputDistortionDepth
	{
		'DDEP', 0,

		"$DDEP = dot(%OPOS, " VPARAM(VERTEX_PARAM_DISTORT_CAMERA_PLANE) ".xyz) + " VPARAM(VERTEX_PARAM_DISTORT_CAMERA_PLANE) ".w;\n"
	},

	// kVertexSnippetOutputImpostorDepth
	{
		'IDEP', 0,

		"temp.y = dot(" VPARAM(VERTEX_PARAM_MATRIX_CAMERA1) ".xyz, %OPOS) + " VPARAM(VERTEX_PARAM_MATRIX_CAMERA1) ".w;\n"
		"temp.z = dot(" VPARAM(VERTEX_PARAM_MATRIX_CAMERA2) ".xyz, %OPOS) + " VPARAM(VERTEX_PARAM_MATRIX_CAMERA2) ".w - temp.y * " VPARAM(VERTEX_PARAM_IMPOSTOR_DEPTH) ".z;\n"
		"$IDEP = temp.z * " VPARAM(VERTEX_PARAM_IMPOSTOR_DEPTH) ".x + " VPARAM(VERTEX_PARAM_IMPOSTOR_DEPTH) ".y;\n"
	},

	// kVertexSnippetOutputImpostorRadius
	{
		'IRAD', 0,

		"$IRAD = abs(" ATTRIB(VERTEX_ATTRIB_RADIUS) ".x) * " FLOAT2 "(2.0, -1.0);\n"
	},

	// kVertexSnippetOutputImpostorShadowRadius
	{
		'ISRD', 0,

		"$ISRD = abs(" ATTRIB(VERTEX_ATTRIB_RADIUS) ".x) * " FLOAT2 "(4.0, -1.0);\n"
	},

	// kVertexSnippetOutputPointLightAttenuation
	{
		'CPLA', 0,

		"$ATTN.xyz = ldir * " UPARAM(UNIVERSAL_PARAM_LIGHT_RANGE) ".w;\n"
	},

	// kVertexSnippetOutputSpotLightAttenuation
	{
		'CSLA', 0,

		"temp.x = dot(%OPOS, " VPARAM(VERTEX_PARAM_MATRIX_LIGHT0) ".xyz) + " VPARAM(VERTEX_PARAM_MATRIX_LIGHT0) ".w;\n"
		"temp.y = dot(%OPOS, " VPARAM(VERTEX_PARAM_MATRIX_LIGHT1) ".xyz) + " VPARAM(VERTEX_PARAM_MATRIX_LIGHT1) ".w;\n"
		"temp.z = dot(%OPOS, " VPARAM(VERTEX_PARAM_MATRIX_LIGHT2) ".xyz) + " VPARAM(VERTEX_PARAM_MATRIX_LIGHT2) ".w;\n"
		"$ATTN.xyz = temp.xyz * " UPARAM(UNIVERSAL_PARAM_LIGHT_RANGE) ".w;\n"
	},

	// kVertexSnippetOutputCascadeTexcoord
	{
		'CSCT', 0,

		"$SHAD.x = dot(%OPOS, " VPARAM(VERTEX_PARAM_MATRIX_SHADOW0) ".xyz) + " VPARAM(VERTEX_PARAM_MATRIX_SHADOW0) ".w;\n"
		"$SHAD.y = dot(%OPOS, " VPARAM(VERTEX_PARAM_MATRIX_SHADOW1) ".xyz) + " VPARAM(VERTEX_PARAM_MATRIX_SHADOW1) ".w;\n"
		"$SHAD.z = dot(%OPOS, " VPARAM(VERTEX_PARAM_MATRIX_SHADOW2) ".xyz) + " VPARAM(VERTEX_PARAM_MATRIX_SHADOW2) ".w;\n"

		"$CSCD.x = dot(%OPOS, " VPARAM(VERTEX_PARAM_SHADOW_CASCADE_PLANE1) ".xyz) + " VPARAM(VERTEX_PARAM_SHADOW_CASCADE_PLANE1) ".w;\n"
		"$CSCD.y = dot(%OPOS, " VPARAM(VERTEX_PARAM_SHADOW_CASCADE_PLANE2) ".xyz) + " VPARAM(VERTEX_PARAM_SHADOW_CASCADE_PLANE2) ".w;\n"
		"$CSCD.z = dot(%OPOS, " VPARAM(VERTEX_PARAM_SHADOW_CASCADE_PLANE3) ".xyz) + " VPARAM(VERTEX_PARAM_SHADOW_CASCADE_PLANE3) ".w;\n"
	},

	// kVertexSnippetOutputPointProjectTexcoord
	{
		'PPPT', 0,

		"$PROJ.x = dot(%OPOS, " VPARAM(VERTEX_PARAM_MATRIX_LIGHT0) ".xyz) + " VPARAM(VERTEX_PARAM_MATRIX_LIGHT0) ".w;\n"
		"$PROJ.y = dot(%OPOS, " VPARAM(VERTEX_PARAM_MATRIX_LIGHT1) ".xyz) + " VPARAM(VERTEX_PARAM_MATRIX_LIGHT1) ".w;\n"
		"$PROJ.z = dot(%OPOS, " VPARAM(VERTEX_PARAM_MATRIX_LIGHT2) ".xyz) + " VPARAM(VERTEX_PARAM_MATRIX_LIGHT2) ".w;\n"
	},

	// kVertexSnippetOutputSpotProjectTexcoord
	{
		'SPPT', 0,

		"$PROJ.x = dot(%OPOS, " VPARAM(VERTEX_PARAM_MATRIX_LIGHT_PROJECTOR0) ".xyz) + " VPARAM(VERTEX_PARAM_MATRIX_LIGHT_PROJECTOR0) ".w;\n"
		"$PROJ.y = dot(%OPOS, " VPARAM(VERTEX_PARAM_MATRIX_LIGHT_PROJECTOR1) ".xyz) + " VPARAM(VERTEX_PARAM_MATRIX_LIGHT_PROJECTOR1) ".w;\n"
		"$PROJ.z = dot(%OPOS, " VPARAM(VERTEX_PARAM_MATRIX_LIGHT2) ".xyz) + " VPARAM(VERTEX_PARAM_MATRIX_LIGHT2) ".w;\n"
	},

	// kVertexSnippetOutputSpotShadowTexcoord
	{
		'SPSH', 0,

		"$SPOT.x = dot(%OPOS, " VPARAM(VERTEX_PARAM_MATRIX_SHADOW0) ".xyz) + " VPARAM(VERTEX_PARAM_MATRIX_SHADOW0) ".w;\n"
		"$SPOT.y = dot(%OPOS, " VPARAM(VERTEX_PARAM_MATRIX_SHADOW1) ".xyz) + " VPARAM(VERTEX_PARAM_MATRIX_SHADOW1) ".w;\n"
		"$SPOT.z = dot(%OPOS, " VPARAM(VERTEX_PARAM_MATRIX_SHADOW2) ".xyz) + " VPARAM(VERTEX_PARAM_MATRIX_SHADOW2) ".w;\n"
	},

	// kVertexSnippetOutputRadiosityPosition
	{
		'RDPS', 0,

		"temp.x = dot(%OPOS, " VPARAM(VERTEX_PARAM_MATRIX_SPACE0) ".xyz) + " VPARAM(VERTEX_PARAM_MATRIX_SPACE0) ".w;\n"
		"temp.y = dot(%OPOS, " VPARAM(VERTEX_PARAM_MATRIX_SPACE1) ".xyz) + " VPARAM(VERTEX_PARAM_MATRIX_SPACE1) ".w;\n"
		"temp.z = dot(%OPOS, " VPARAM(VERTEX_PARAM_MATRIX_SPACE2) ".xyz) + " VPARAM(VERTEX_PARAM_MATRIX_SPACE2) ".w;\n"
		"$RPOS.xyz = temp.xyz * " VPARAM(VERTEX_PARAM_SPACE_SCALE) ".xyz;\n"
	},

	// kVertexSnippetOutputRadiosityNormal
	{
		'RDNM', 0,

		"$RNRM.x = dot(" VPARAM(VERTEX_PARAM_MATRIX_SPACE0) ".xyz, %NRML);\n"
		"$RNRM.y = dot(" VPARAM(VERTEX_PARAM_MATRIX_SPACE1) ".xyz, %NRML);\n"
		"$RNRM.z = dot(" VPARAM(VERTEX_PARAM_MATRIX_SPACE2) ".xyz, %NRML);\n"
	},

	// kVertexSnippetOutputRadiosityViewNormal
	{
		'RDVN', 0,

		"$RNRM.x = dot(" VPARAM(VERTEX_PARAM_MATRIX_SPACE0) ".xyz, vdir);\n"
		"$RNRM.y = dot(" VPARAM(VERTEX_PARAM_MATRIX_SPACE1) ".xyz, vdir);\n"
		"$RNRM.z = dot(" VPARAM(VERTEX_PARAM_MATRIX_SPACE2) ".xyz, vdir);\n"
	},

	// kVertexSnippetOutputRadiosityTangent
	{
		'RDTG', 0,

		"$RTNG.x = dot(" VPARAM(VERTEX_PARAM_MATRIX_SPACE0) ".xyz, %TANG);\n"
		"$RTNG.y = dot(" VPARAM(VERTEX_PARAM_MATRIX_SPACE1) ".xyz, %TANG);\n"
		"$RTNG.z = dot(" VPARAM(VERTEX_PARAM_MATRIX_SPACE2) ".xyz, %TANG);\n"
	},

	// kVertexSnippetOutputRadiosityTangentHandedness
	{
		'RDTH', 0,

		"$RTGH.x = dot(" VPARAM(VERTEX_PARAM_MATRIX_SPACE0) ".xyz, " ATTRIB(VERTEX_ATTRIB_TANGENT) ".xyz);\n"
		"$RTGH.y = dot(" VPARAM(VERTEX_PARAM_MATRIX_SPACE1) ".xyz, " ATTRIB(VERTEX_ATTRIB_TANGENT) ".xyz);\n"
		"$RTGH.z = dot(" VPARAM(VERTEX_PARAM_MATRIX_SPACE2) ".xyz, " ATTRIB(VERTEX_ATTRIB_TANGENT) ".xyz);\n"
		"$RTGH.w = " ATTRIB(VERTEX_ATTRIB_TANGENT) ".w;\n"
	},

	// kVertexSnippetOutputFiniteConstantFogFactors
	{
		'FCFF', 0,

		"$FDTP = dot(%OPOS, " VPARAM(VERTEX_PARAM_FOG_PLANE) ".xyz) + " VPARAM(VERTEX_PARAM_FOG_PLANE) ".w;\n"
		"$FDTV = dot(vdir, " VPARAM(VERTEX_PARAM_FOG_PLANE) ".xyz);\n"
	},

	// kVertexSnippetOutputInfiniteConstantFogFactors
	{
		'ICFF', 0,

		"$FDTV = dot(%OPOS, " VPARAM(VERTEX_PARAM_FOG_PLANE) ".xyz);\n"
	},

	// kVertexSnippetOutputFiniteLinearFogFactors
	{
		'FLFF', 0,

		"temp.z = dot(%OPOS, " VPARAM(VERTEX_PARAM_FOG_PLANE) ".xyz) + " VPARAM(VERTEX_PARAM_FOG_PLANE) ".w;\n"
		"$FOGK = (temp.z + " UPARAM(UNIVERSAL_PARAM_FOG_PARAMS1) ".x) * " UPARAM(UNIVERSAL_PARAM_FOG_PARAMS1) ".y;\n"
		"$FDTV = dot(vdir, " VPARAM(VERTEX_PARAM_FOG_PLANE) ".xyz);\n"
		"$FDTP = temp.z * " UPARAM(UNIVERSAL_PARAM_FOG_PARAMS1) ".w;\n"
	},

	// kVertexSnippetOutputInfiniteLinearFogFactors
	{
		'ILFF', 0,

		"temp.x = dot(%OPOS, " VPARAM(VERTEX_PARAM_FOG_PLANE) ".xyz);\n"
		"$FOGK = temp.x * " UPARAM(UNIVERSAL_PARAM_FOG_PARAMS1) ".z;\n"
		"$FDTV = temp.x;\n"
	},

	// kVertexSnippetMotionBlurTransform
	{
		'BLUR', 0,

		"$VELA.x = dot(%OPOS, " VPARAM(VERTEX_PARAM_MATRIX_VELOCITY_A0) ".xyz) + " VPARAM(VERTEX_PARAM_MATRIX_VELOCITY_A0) ".w;\n"
		"$VELA.y = dot(%OPOS, " VPARAM(VERTEX_PARAM_MATRIX_VELOCITY_A1) ".xyz) + " VPARAM(VERTEX_PARAM_MATRIX_VELOCITY_A1) ".w;\n"
		"$VELA.w = dot(%OPOS, " VPARAM(VERTEX_PARAM_MATRIX_VELOCITY_A3) ".xyz) + " VPARAM(VERTEX_PARAM_MATRIX_VELOCITY_A3) ".w;\n"

		"$VELB.x = dot(%OPOS, " VPARAM(VERTEX_PARAM_MATRIX_VELOCITY_B0) ".xyz) + " VPARAM(VERTEX_PARAM_MATRIX_VELOCITY_B0) ".w;\n"
		"$VELB.y = dot(%OPOS, " VPARAM(VERTEX_PARAM_MATRIX_VELOCITY_B1) ".xyz) + " VPARAM(VERTEX_PARAM_MATRIX_VELOCITY_B1) ".w;\n"
		"$VELB.w = dot(%OPOS, " VPARAM(VERTEX_PARAM_MATRIX_VELOCITY_B3) ".xyz) + " VPARAM(VERTEX_PARAM_MATRIX_VELOCITY_B3) ".w;\n"
	},

	// kVertexSnippetDeformMotionBlurTransform
	{
		'DBLR', 0,

		"$VELA.x = dot(" ATTRIB(VERTEX_ATTRIB_VELOCITY) ".xyz, " VPARAM(VERTEX_PARAM_MATRIX_VELOCITY_A0) ".xyz) + " VPARAM(VERTEX_PARAM_MATRIX_VELOCITY_A0) ".w;\n"
		"$VELA.y = dot(" ATTRIB(VERTEX_ATTRIB_VELOCITY) ".xyz, " VPARAM(VERTEX_PARAM_MATRIX_VELOCITY_A1) ".xyz) + " VPARAM(VERTEX_PARAM_MATRIX_VELOCITY_A1) ".w;\n"
		"$VELA.w = dot(" ATTRIB(VERTEX_ATTRIB_VELOCITY) ".xyz, " VPARAM(VERTEX_PARAM_MATRIX_VELOCITY_A3) ".xyz) + " VPARAM(VERTEX_PARAM_MATRIX_VELOCITY_A3) ".w;\n"

		"$VELB.x = dot(" ATTRIB(VERTEX_ATTRIB_POSITION0) ".xyz, " VPARAM(VERTEX_PARAM_MATRIX_VELOCITY_B0) ".xyz) + " VPARAM(VERTEX_PARAM_MATRIX_VELOCITY_B0) ".w;\n"
		"$VELB.y = dot(" ATTRIB(VERTEX_ATTRIB_POSITION0) ".xyz, " VPARAM(VERTEX_PARAM_MATRIX_VELOCITY_B1) ".xyz) + " VPARAM(VERTEX_PARAM_MATRIX_VELOCITY_B1) ".w;\n"
		"$VELB.w = dot(" ATTRIB(VERTEX_ATTRIB_POSITION0) ".xyz, " VPARAM(VERTEX_PARAM_MATRIX_VELOCITY_B3) ".xyz) + " VPARAM(VERTEX_PARAM_MATRIX_VELOCITY_B3) ".w;\n"
	},

	// kVertexSnippetVelocityMotionBlurTransform
	{
		'VBLR', 0,

		"temp.xyz = " ATTRIB(VERTEX_ATTRIB_POSITION0) ".xyz - " ATTRIB(VERTEX_ATTRIB_VELOCITY) ".xyz * " UPARAM(UNIVERSAL_PARAM_SHADER_TIME) ".z;\n"

		"$VELA.x = dot(temp.xyz, " VPARAM(VERTEX_PARAM_MATRIX_VELOCITY_A0) ".xyz) + " VPARAM(VERTEX_PARAM_MATRIX_VELOCITY_A0) ".w;\n"
		"$VELA.y = dot(temp.xyz, " VPARAM(VERTEX_PARAM_MATRIX_VELOCITY_A1) ".xyz) + " VPARAM(VERTEX_PARAM_MATRIX_VELOCITY_A1) ".w;\n"
		"$VELA.w = dot(temp.xyz, " VPARAM(VERTEX_PARAM_MATRIX_VELOCITY_A3) ".xyz) + " VPARAM(VERTEX_PARAM_MATRIX_VELOCITY_A3) ".w;\n"

		"$VELB.x = dot(" ATTRIB(VERTEX_ATTRIB_POSITION0) ".xyz, " VPARAM(VERTEX_PARAM_MATRIX_VELOCITY_B0) ".xyz) + " VPARAM(VERTEX_PARAM_MATRIX_VELOCITY_B0) ".w;\n"
		"$VELB.y = dot(" ATTRIB(VERTEX_ATTRIB_POSITION0) ".xyz, " VPARAM(VERTEX_PARAM_MATRIX_VELOCITY_B1) ".xyz) + " VPARAM(VERTEX_PARAM_MATRIX_VELOCITY_B1) ".w;\n"
		"$VELB.w = dot(" ATTRIB(VERTEX_ATTRIB_POSITION0) ".xyz, " VPARAM(VERTEX_PARAM_MATRIX_VELOCITY_B3) ".xyz) + " VPARAM(VERTEX_PARAM_MATRIX_VELOCITY_B3) ".w;\n"
	},

	// kVertexSnippetInfiniteMotionBlurTransform
	{
		'IBLR', 0,

		"$VELA.x = dot(%OPOS, " VPARAM(VERTEX_PARAM_MATRIX_VELOCITY_A0) ".xyz);\n"
		"$VELA.y = dot(%OPOS, " VPARAM(VERTEX_PARAM_MATRIX_VELOCITY_A1) ".xyz);\n"
		"$VELA.w = dot(%OPOS, " VPARAM(VERTEX_PARAM_MATRIX_VELOCITY_A3) ".xyz);\n"

		"$VELB.x = dot(%OPOS, " VPARAM(VERTEX_PARAM_MATRIX_VELOCITY_B0) ".xyz);\n"
		"$VELB.y = dot(%OPOS, " VPARAM(VERTEX_PARAM_MATRIX_VELOCITY_B1) ".xyz);\n"
		"$VELB.w = dot(%OPOS, " VPARAM(VERTEX_PARAM_MATRIX_VELOCITY_B3) ".xyz);\n"
	}
};


VertexShader::VertexShader(const char *source, unsigned_int32 size, const unsigned_int32 *signature) : VertexShaderObject(source, size)
{
	MemoryMgr::CopyMemory(signature, shaderSignature, signature[0] * 4 + 4);

	#if C4LOG_FILE && C4LOG_VERTEX_SHADERS

		Engine::LogSource(source);

	#endif

	#if C4CONSOLE //[ CONSOLE

		// -- Console code hidden --

	#endif //]
}

#if C4CONSOLE //[ CONSOLE

	// -- Console code hidden --

#endif //]

VertexShader::~VertexShader()
{
	#if C4CONSOLE //[ CONSOLE

		// -- Console code hidden --

	#endif //]
}

void VertexShader::Initialize(void)
{
	new(hashTable) HashTable<VertexShader>(16, 16);

	#if C4CONSOLE //[ CONSOLE

		// -- Console code hidden --

	#endif //]
}

void VertexShader::Terminate(void)
{
	hashTable->~HashTable();
}

unsigned_int32 VertexShader::Hash(const KeyType& key)
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

VertexShader *VertexShader::Find(const unsigned_int32 *signature)
{
	VertexShader *shader = hashTable->Find(ShaderSignature(signature));
	if (shader)
	{
		shader->Retain();
	}

	return (shader);
}

VertexShader *VertexShader::Get(const VertexAssembly *assembly)
{
	const unsigned_int32 *signature = assembly->signatureStorage;
	VertexShader *shader = hashTable->Find(ShaderSignature(signature));
	if (!shader)
	{
		char *source = ShaderAttribute::sourceStorage;
		int32 size = Text::CopyText(prologText, source);

		int32 count = signature[0];
		for (machine a = 0; a < count; a++)
		{
			size += Text::CopyText(assembly->vertexSnippet[a]->shaderCode, &source[size]);
		}

		size += Text::CopyText(epilogText, &source[size]);

		shader = MemoryMgr::GetMainHeap()->New<VertexShader>(sizeof(VertexShader) + signature[0] * 4);
		new(shader) VertexShader(source, size, signature);
		hashTable->Insert(shader);
	}

	shader->Retain();
	return (shader);
}

VertexShader *VertexShader::New(const char *source, unsigned_int32 size, const unsigned_int32 *signature)
{
	VertexShader *shader = MemoryMgr::GetMainHeap()->New<VertexShader>(sizeof(VertexShader) + signature[0] * 4);
	new(shader) VertexShader(source, size, signature);
	hashTable->Insert(shader);

	shader->Retain();
	return (shader);
}

void VertexShader::ReleaseCache(void)
{
	int32 bucketCount = hashTable->GetBucketCount();
	for (machine a = 0; a < bucketCount; a++)
	{
		VertexShader *shader = hashTable->GetFirstBucketElement(a);
		while (shader)
		{
			VertexShader *next = shader->Next();

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
