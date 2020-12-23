 

#include "C4Shafts.h"
#include "C4Cameras.h"
#include "C4Graphics.h"
#include "C4Configuration.h"


using namespace C4;


namespace
{
	enum
	{
		kCylinderShaftSubdivCount			= 16,
		kCylinderShaftVertexCount			= kCylinderShaftSubdivCount * 2,
		kCylinderShaftTriangleCount			= (kCylinderShaftSubdivCount * 2 - 2) * 2,

		kTruncatedConeShaftSubdivCount		= 16,
		kTruncatedConeShaftVertexCount		= kCylinderShaftSubdivCount * 2,
		kTruncatedConeShaftTriangleCount	= (kCylinderShaftSubdivCount * 2 - 2) * 2
	};

	static_assert((kCylinderShaftSubdivCount & (kCylinderShaftSubdivCount - 1)) == 0, "kCylinderShaftSubdivCount must be a power of two");
	static_assert((kTruncatedConeShaftSubdivCount & (kTruncatedConeShaftSubdivCount - 1)) == 0, "kTruncatedConeShaftSubdivCount must be a power of two");
}


SharedVertexBuffer BoxShaftEffect::indexBuffer(kVertexBufferIndexArray | kVertexBufferStatic);
SharedVertexBuffer CylinderShaftEffect::indexBuffer(kVertexBufferIndexArray | kVertexBufferStatic);
SharedVertexBuffer TruncatedPyramidShaftEffect::indexBuffer(kVertexBufferIndexArray | kVertexBufferStatic);
SharedVertexBuffer TruncatedConeShaftEffect::indexBuffer(kVertexBufferIndexArray | kVertexBufferStatic);


ShaftProcess::ShaftProcess(ProcessType type, unsigned_int32 flags, const Vector4D (*params)[4]) : Process(type)
{
	shaftProcessFlags = flags;
	shaftParams = params;

	if (flags & kShaftProcessNoise)
	{
		textureObject = Texture::Get("C4/screen");
	}
	else
	{
		textureObject = nullptr;
	}
}

ShaftProcess::ShaftProcess(const ShaftProcess& shaftProcess) : Process(shaftProcess)
{
	shaftProcessFlags = shaftProcess.shaftProcessFlags;
	shaftParams = shaftProcess.shaftParams;

	textureObject = shaftProcess.textureObject;
	if (textureObject)
	{
		textureObject->Retain();
	}
}

ShaftProcess::~ShaftProcess()
{
	if (textureObject)
	{
		textureObject->Release();
	}
}

int32 ShaftProcess::GenerateProcessSignature(const ShaderCompileData *compileData, unsigned_int32 *signature) const
{
	int32 count = Process::GenerateProcessSignature(compileData, signature);
	signature[count] = shaftProcessFlags;
	return (count + 1);
}

void ShaftProcess::GenerateProcessData(const ShaderCompileData *compileData, ProcessData *data) const
{
	data->registerCount = 1;
	data->outputSize = 3;

	data->interpolantCount = 2;
	data->interpolantType[0] = 'POSI';
	data->interpolantType[1] = 'OVDR';

	data->textureCount = (shaftProcessFlags & kShaftProcessNoise) ? 2 : 1;
	data->textureObject[0] = TheGraphicsMgr->GetStructureTexture();
	data->textureObject[1] = textureObject;

	compileData->shaderData->AddStateProc(StateProc_LoadShaftParams, shaftParams);
}

int32 ShaftProcess::GenerateShaderCode(const ShaderCompileData *compileData, const char **shaderCode) const
{
	/*	P = object-space rasterized point
		C = object-space camera position
		V = unnormalized object-space direction to camera C - P

		The final alpha value is given by 1 - exp(-Dm), where m is a user-specified intensity and D is the
		distance travelled through the light shaft, given by:

			D = (t₂ - t₁)|V| 

		In the case that the shaft has a linear density gradient given by ρ(z) = αz + β, D is calculated
		as the integral of |V|ρ(z(t))dt for t ranging over [t₁, t₂]. Inserting ρ(z(t) = α(Pz + tVz) + β, the 
		integral evaluates to:
 
			D = |V|[(αPz + β)(t₂ - t₁) + (αVz / 2)(t₂² - t₁²)]

		This simplifies to: 

			D = (t₂ - t₁)|V|[α(Pz + Vz(t₁ + t₂) / 2) + β] 
	*/ 

	static const char constantCode[] =
	{
		"float f = max(1.0 - exp2((t2 - t1) * length($OVDR) * " FPARAM(FRAGMENT_PARAM_CONSTANT1) ".y), 0.0);\n" 
	};

	static const char gradientCode[] =
	{
		"float limit = (" FPARAM(FRAGMENT_PARAM_CONSTANT1) ".z - $POSI.z) / $OVDR.z;\n"
		"if ($OVDR.z * " FPARAM(FRAGMENT_PARAM_CONSTANT1) ".x > 0.0)\n"
		"{\n"
			"t1 = min(t1, limit);\n"
			"t2 = min(t2, limit);\n"
		"}\n"
		"else\n"
		"{\n"
			"t1 = max(t1, limit);\n"
			"t2 = max(t2, limit);\n"
		"}\n"

		"float f = max(1.0 - exp2((t2 - t1) * length($OVDR) * (" FPARAM(FRAGMENT_PARAM_CONSTANT1) ".x * ($POSI.z + $OVDR.z * ((t1 + t2) * 0.5)) + " FPARAM(FRAGMENT_PARAM_CONSTANT1) ".y)), 0.0);\n"
	};

	static const char directCode[] =
	{
		"# = " FPARAM(FRAGMENT_PARAM_CONSTANT3) ".rgb * f;\n"
	};

	static const char noiseCode[] =
	{
		"# = " FPARAM(FRAGMENT_PARAM_CONSTANT3) ".rgb * f + %TRG1(%IMG1, " FRAGMENT_POSITION ".xy * 0.00390625 + " FLOAT2 "(" FPARAM(FRAGMENT_PARAM_CONSTANT2) ".w, " FPARAM(FRAGMENT_PARAM_CONSTANT3) ".w)).x * (pow(f, 0.125) * 0.0078125);\n"
	};

	unsigned_int32 flags = shaftProcessFlags;
	shaderCode[1] = (flags & kShaftProcessGradient) ? gradientCode : constantCode;
	shaderCode[2] = (flags & kShaftProcessNoise) ? noiseCode : directCode;
	return (3);
}

void ShaftProcess::StateProc_LoadShaftParams(const Renderable *renderable, const void *cookie)
{
	const Vector4D (& params)[4] = *static_cast<const Vector4D (*)[4]>(cookie);
	Render::SetFragmentShaderParameter(kFragmentParamConstant0, params[0]);
	Render::SetFragmentShaderParameter(kFragmentParamConstant1, params[1]);
	Render::SetFragmentShaderParameter(kFragmentParamConstant2, params[2]);
	Render::SetFragmentShaderParameter(kFragmentParamConstant3, params[3]);
}


BoxShaftProcess::BoxShaftProcess(unsigned_int32 flags, const Vector4D (*params)[4]) : ShaftProcess(kProcessBoxShaft, flags, params)
{
}

BoxShaftProcess::BoxShaftProcess(const BoxShaftProcess& boxShaftProcess) : ShaftProcess(boxShaftProcess)
{
}

BoxShaftProcess::~BoxShaftProcess()
{
}

Process *BoxShaftProcess::Replicate(void) const
{
	return (new BoxShaftProcess(*this));
}

int32 BoxShaftProcess::GenerateShaderCode(const ShaderCompileData *compileData, const char **shaderCode) const
{
	/*	P = object-space rasterized point
		C = object-space camera position
		V = unnormalized object-space direction to camera C - P

		The light shaft is an infinite box with four side planes given by:

			B₁ = < 1,  0,  0,  0>
			B₂ = <-1,  0,  0, sx>
			B₃ = < 0,  1,  0,  0>
			B₄ = < 0, -1,  0, sy>

		The point of intersection between the ray P + tV and any of these planes Bi is found by solving the following
		equation for t, where the point P is extended to 4D by giving it a w-coordinate of 1:

			Bi ∧ (P + tV) = 0

		The value of t is given by:

				   Bi ∧ P
			t = - ────────
				   Bi ∧ V

		A value of t = 0 corresponds to the rasterized point P, and a value of t = 1 corresponds to the
		camera position C. Values of at greater than 1 are behind the camera, so we clamp t to a maximum of 1.

		The difference between t = 0 and t = 1 corresponds to the distance |V|, so multiplying by |V| converts
		a value of t to an actual distance, and multiplying a distance by 1 / |V| produces a value in t-space.

		The depth z already stored in the framebuffer may be less than the depth of the point P. The depth z is the
		distance from the camera plane, so it needs to be converted to a length z′ along the direction V as follows:

			z′ = z / ((-V ∧ K) / |V|)

		Here, K is the antivector perpendicular to the camera's viewing direction. Multiplying by 1 / |V| and
		subtracting from the camera position t = 1 gives us the minimum allowable value of t:

			t₀ = 1 - z′ / |V| = 1 + z / (V ∧ K)

		The t values for the final endpoints of the line segment intersecting the light shaft are then:

			t₁ ← max(t₁, t₀)
			t₂ ← max(t₂, t₀)

		The total distance D inside the light shaft is:

			D = (t₂ - t₁)|V|

		The final alpha value is given by 1 - exp(-Dm), where m is a user-specified intensity.

		In the case that the shaft has a linear density gradient given by ρ(z) = αz + β, D is instead calculated
		as the integral of |V|ρ(z(t))dt for t ranging over [t₁, t₂]. Inserting ρ(z(t) = α(Pz + tVz) + β, the
		integral evaluates to:

			D = |V|[(αPz + β)(t₂ - t₁) + (αVz / 2)(t₂² - t₁²)]

		This simplifies to:

			D = (t₂ - t₁)|V|[α(Pz + Vz(t₁ + t₂) / 2) + β]
	*/

	static const char code[] =
	{
		FLOAT2 " d = " FLOAT2 "(1.0 / $OVDR.x, 1.0 / $OVDR.y);\n"

		FLOAT2 " depth = %TRG0(%IMG0, " FRAGMENT_POSITION ".xy" FRAGMENT_XY_MULTIPLIER ").zw;\n"
		"float t1 = 1.0 + (depth.x + depth.y * 0.25) / dot($OVDR, " FPARAM(FRAGMENT_PARAM_CONSTANT2) ".xyz);\n"
		"float t2 = 1.0;\n"

		"float t = -$POSI.x * d.x;\n"
		"if ($OVDR.x > 0.0) t1 = max(t1, t);\n"
		"else t2 = min(t2, t);\n"

		"t = (" FPARAM(FRAGMENT_PARAM_CONSTANT0) ".x - $POSI.x) * d.x;\n"
		"if ($OVDR.x < 0.0) t1 = max(t1, t);\n"
		"else t2 = min(t2, t);\n"

		"t = -$POSI.y * d.y;\n"
		"if ($OVDR.y > 0.0) t1 = max(t1, t);\n"
		"else t2 = min(t2, t);\n"

		"t = (" FPARAM(FRAGMENT_PARAM_CONSTANT0) ".y - $POSI.y) * d.y;\n"
		"if ($OVDR.y < 0.0) t1 = max(t1, t);\n"
		"else t2 = min(t2, t);\n"
	};

	shaderCode[0] = code;
	return (ShaftProcess::GenerateShaderCode(compileData, shaderCode));
}


CylinderShaftProcess::CylinderShaftProcess(unsigned_int32 flags, const Vector4D (*params)[4]) : ShaftProcess(kProcessCylinderShaft, flags, params)
{
}

CylinderShaftProcess::CylinderShaftProcess(const CylinderShaftProcess& cylinderShaftProcess) : ShaftProcess(cylinderShaftProcess)
{
}

CylinderShaftProcess::~CylinderShaftProcess()
{
}

Process *CylinderShaftProcess::Replicate(void) const
{
	return (new CylinderShaftProcess(*this));
}

int32 CylinderShaftProcess::GenerateShaderCode(const ShaderCompileData *compileData, const char **shaderCode) const
{
	/*	P = object-space rasterized point
		C = object-space camera position
		V = unnormalized object-space direction to camera C - P

		The light shaft is an infinite cylinder of semiaxis lengths rx and ry:

			x² / rx² + y² / ry² = 1

		Multiply through by rx² ry²:

			ry²x² + rx²y² = rx²ry²

		Make the substitutions

			x ↦ Px + tVx
			y ↦ Py + tVy

		to get:

			ry²(Px + tVx)² + rx²(Py + tVy)² = rx²ry²

		Expand and collect terms:

			(ry²Vx² + rx²Vy²)t² + 2(ry²PxVx + rx²PyVy)t + (ry²Px² + rx²Py² - rx²ry²) = 0

		This has the form at² + 2bt + c = 0, where

			a = ry²Vx² + rx²Vy²
			b = ry²PxVx + rx²PyVy
			c = ry²Px² + rx²Py² - rx²ry²

		Solving for t with the quadratic formula gives:

			t = (-b ± √(b² - ac)) / a

		A value of t = 0 corresponds to the rasterized point P, and a value of t = 1 corresponds to the
		camera position C. Values of at greater than 1 are behind the camera, so we clamp t to a maximum of 1.

		The difference between t = 0 and t = 1 corresponds to the distance |V|, so multiplying by |V| converts
		a value of t to an actual distance, and multiplying a distance by 1 / |V| produces a value in t-space.

		The depth z already stored in the framebuffer may be less than the depth of the point P. The depth z is the
		distance from the camera plane, so it needs to be converted to a length z′ along the direction V as follows:

			z′ = z / ((-V ∧ K) / |V|)

		Here, K is the antivector perpendicular to the camera's viewing direction. Multiplying by 1 / |V| and
		subtracting from the camera position t = 1 gives us the minimum allowable value of t:

			t₀ = 1 - z′ / |V| = 1 + z / (V ∧ K)

		The u values for the final endpoints of the line segment intersecting the light shaft are then:

			t₁ = min(max((-b - √(b² - ac)) / a, t₀), 1)
			t₂ = min(max((-b + √(b² - ac)) / a, t₀), 1)

		The total distance D inside the light shaft is:

			D = (t₂ - t₁)|V|

		The final alpha value is given by 1 - exp(-Dm), where m is a user-specified intensity.

		In the case that the shaft has a linear density gradient given by ρ(z) = αz + β, D is instead calculated
		as the integral of |V|ρ(z(t))dt for t ranging over [t₁, t₂]. Inserting ρ(z(t) = α(Pz + tVz) + β, the
		integral evaluates to:

			D = |V|[(αPz + β)(t₂ - t₁) + (αVz / 2)(t₂² - t₁²)]

		This simplifies to:

			D = (t₂ - t₁)|V|[α(Pz + Vz(t₁ + t₂) / 2) + β]
	*/

	static const char code[] =
	{
		FLOAT2 " pos2 = $POSI.xy * $POSI.xy;\n"
		FLOAT2 " dir2 = $OVDR.xy * $OVDR.xy;\n"

		"float a = " FPARAM(FRAGMENT_PARAM_CONSTANT0) ".y * dir2.x + " FPARAM(FRAGMENT_PARAM_CONSTANT0) ".x * dir2.y;\n"
		"float b = " FPARAM(FRAGMENT_PARAM_CONSTANT0) ".y * $POSI.x * $OVDR.x + " FPARAM(FRAGMENT_PARAM_CONSTANT0) ".x * $POSI.y * $OVDR.y;\n"
		"float c = " FPARAM(FRAGMENT_PARAM_CONSTANT0) ".y * pos2.x + " FPARAM(FRAGMENT_PARAM_CONSTANT0) ".x * pos2.y - " FPARAM(FRAGMENT_PARAM_CONSTANT0) ".z;\n"
		"float d = sqrt(max(b * b - a * c, 0.0));\n"

		FLOAT2 " depth = %TRG0(%IMG0, " FRAGMENT_POSITION ".xy" FRAGMENT_XY_MULTIPLIER ").zw;\n"
		"float t0 = 1.0 + (depth.x + depth.y * 0.25) / dot($OVDR, " FPARAM(FRAGMENT_PARAM_CONSTANT2) ".xyz);\n"

		"float t1 = clamp((-d - b) / a, t0, 1.0);\n"
		"float t2 = clamp((d - b) / a, t0, 1.0);\n"
	};

	shaderCode[0] = code;
	return (ShaftProcess::GenerateShaderCode(compileData, shaderCode));
}


TruncatedPyramidShaftProcess::TruncatedPyramidShaftProcess(unsigned_int32 flags, const Vector4D (*params)[4]) : ShaftProcess(kProcessTruncatedPyramidShaft, flags, params)
{
}

TruncatedPyramidShaftProcess::TruncatedPyramidShaftProcess(const TruncatedPyramidShaftProcess& truncatedPyramidShaftProcess) : ShaftProcess(truncatedPyramidShaftProcess)
{
}

TruncatedPyramidShaftProcess::~TruncatedPyramidShaftProcess()
{
}

Process *TruncatedPyramidShaftProcess::Replicate(void) const
{
	return (new TruncatedPyramidShaftProcess(*this));
}

int32 TruncatedPyramidShaftProcess::GenerateShaderCode(const ShaderCompileData *compileData, const char **shaderCode) const
{
	/*	P = object-space rasterized point
		C = object-space camera position
		V = unnormalized object-space direction to camera C - P

		The light shaft is a truncated pyramid with base dimensions 2sx and 2sy, truncated height h′, and ratio of
		top side to base dimensions g. The untruncated height h is given by

			h = h′ / (1 - g)

		and we place the apex of the untruncated pyramid at the origin with the pyramid opening in the positive z
		direction. The four side planes are then given by:

			B₁ = < h,  0, sx,  0>
			B₂ = <-h,  0, sx,  0>
			B₃ = < h, sy,  0,  0>
			B₄ = <-h, sy,  0,  0>

		The point of intersection between the ray P + tV and any of these planes Bi is found by solving the following
		equation for t, where the point P is extended to 4D by giving it a w-coordinate of 1:

			Bi ∧ (P + tV) = 0

		The value of t is given by:

				   Bi ∧ P
			t = - ────────
				   Bi ∧ V

		A value of t = 0 corresponds to the rasterized point P, and a value of t = 1 corresponds to the
		camera position C. Values of at greater than 1 are behind the camera, so we clamp t to a maximum of 1.

		The difference between t = 0 and t = 1 corresponds to the distance |V|, so multiplying by |V| converts
		a value of t to an actual distance, and multiplying a distance by 1 / |V| produces a value in t-space.

		The depth z already stored in the framebuffer may be less than the depth of the point P. The depth z is the
		distance from the camera plane, so it needs to be converted to a length z′ along the direction V as follows:

			z′ = z / ((-V ∧ K) / |V|)

		Here, K is the antivector perpendicular to the camera's viewing direction. Multiplying by 1 / |V| and
		subtracting from the camera position t = 1 gives us the minimum allowable value of t:

			t₀ = 1 - z′ / |V| = 1 + z / (V ∧ K)

		The t values for the final endpoints of the line segment intersecting the light shaft are then:

			t₁ ← max(t₁, t₀)
			t₂ ← max(t₂, t₀)

		The total distance D inside the light shaft is:

			D = (t₂ - t₁)|V|

		The final alpha value is given by 1 - exp(-Dm), where m is a user-specified intensity.

		In the case that the shaft has a linear density gradient given by ρ(z) = αz + β, D is instead calculated
		as the integral of |V|ρ(z(t))dt for t ranging over [t₁, t₂]. Inserting ρ(z(t) = α(Pz + tVz) + β, the
		integral evaluates to:

			D = |V|[(αPz + β)(t₂ - t₁) + (αVz / 2)(t₂² - t₁²)]

		This simplifies to:

			D = (t₂ - t₁)|V|[α(Pz + Vz(t₁ + t₂) / 2) + β]
	*/

	static const char code[] =
	{
		FLOAT3 " posi = " FLOAT3 "($POSI.x - " FPARAM(FRAGMENT_PARAM_CONSTANT0) ".x, $POSI.y - " FPARAM(FRAGMENT_PARAM_CONSTANT0) ".y, " FPARAM(FRAGMENT_PARAM_CONSTANT0) ".z - $POSI.z);\n"

		FLOAT2 " depth = %TRG0(%IMG0, " FRAGMENT_POSITION ".xy" FRAGMENT_XY_MULTIPLIER ").zw;\n"
		"float t1 = 1.0 + (depth.x + depth.y * 0.25) / dot($OVDR, " FPARAM(FRAGMENT_PARAM_CONSTANT2) ".xyz);\n"
		"float t2 = 1.0;\n"

		"float s = " FPARAM(FRAGMENT_PARAM_CONSTANT0) ".z * $OVDR.x - " FPARAM(FRAGMENT_PARAM_CONSTANT0) ".x * $OVDR.z;\n"
		"float t = (-" FPARAM(FRAGMENT_PARAM_CONSTANT0) ".z * posi.x - " FPARAM(FRAGMENT_PARAM_CONSTANT0) ".x * posi.z) / s;\n"
		"float z = posi.z - $OVDR.z * t;\n"
		"if (z > 0.0)\n"
		"{\n"
			"if (s > 0.0) t1 = max(t1, t);\n"
			"else t2 = min(t2, t);\n"
		"}\n"

		"s = -" FPARAM(FRAGMENT_PARAM_CONSTANT0) ".z * $OVDR.x - " FPARAM(FRAGMENT_PARAM_CONSTANT0) ".x * $OVDR.z;\n"
		"t = (" FPARAM(FRAGMENT_PARAM_CONSTANT0) ".z * posi.x - " FPARAM(FRAGMENT_PARAM_CONSTANT0) ".x * posi.z) / s;\n"
		"z = posi.z - $OVDR.z * t;\n"
		"if (z > 0.0)\n"
		"{\n"
			"if (s > 0.0) t1 = max(t1, t);\n"
			"else t2 = min(t2, t);\n"
		"}\n"

		"s = " FPARAM(FRAGMENT_PARAM_CONSTANT0) ".z * $OVDR.y - " FPARAM(FRAGMENT_PARAM_CONSTANT0) ".y * $OVDR.z;\n"
		"t = (-" FPARAM(FRAGMENT_PARAM_CONSTANT0) ".z * posi.y - " FPARAM(FRAGMENT_PARAM_CONSTANT0) ".y * posi.z) / s;\n"
		"z = posi.z - $OVDR.z * t;\n"
		"if (z > 0.0)\n"
		"{\n"
			"if (s > 0.0) t1 = max(t1, t);\n"
			"else t2 = min(t2, t);\n"
		"}\n"

		"s = -" FPARAM(FRAGMENT_PARAM_CONSTANT0) ".z * $OVDR.y - " FPARAM(FRAGMENT_PARAM_CONSTANT0) ".y * $OVDR.z;\n"
		"t = (" FPARAM(FRAGMENT_PARAM_CONSTANT0) ".z * posi.y - " FPARAM(FRAGMENT_PARAM_CONSTANT0) ".y * posi.z) / s;\n"
		"z = posi.z - $OVDR.z * t;\n"
		"if (z > 0.0)\n"
		"{\n"
			"if (s > 0.0) t1 = max(t1, t);\n"
			"else t2 = min(t2, t);\n"
		"}\n"
	};

	shaderCode[0] = code;
	return (ShaftProcess::GenerateShaderCode(compileData, shaderCode));
}


TruncatedConeShaftProcess::TruncatedConeShaftProcess(unsigned_int32 flags, const Vector4D (*params)[4]) : ShaftProcess(kProcessTruncatedConeShaft, flags, params)
{
}

TruncatedConeShaftProcess::TruncatedConeShaftProcess(const TruncatedConeShaftProcess& truncatedConeShaftProcess) : ShaftProcess(truncatedConeShaftProcess)
{
}

TruncatedConeShaftProcess::~TruncatedConeShaftProcess()
{
}

Process *TruncatedConeShaftProcess::Replicate(void) const
{
	return (new TruncatedConeShaftProcess(*this));
}

int32 TruncatedConeShaftProcess::GenerateShaderCode(const ShaderCompileData *compileData, const char **shaderCode) const
{
	/*	P = object-space rasterized point
		C = object-space camera position
		V = unnormalized object-space direction to camera C - P

		The light shaft is a truncated cone of base semiaxis lengths rx and ry, truncated height h′, and ratio of
		top side to base dimensions g. The untruncated height h is given by

			h = h′ / (1 - g)

		and we place the apex of the untruncated cone at the origin with the cone opening in the positive z
		direction. This gives us the quadratic equation:

			ry²x² + rx²y² = rx²ry²z² / h²

		Make the substitutions

			x ↦ Px + tVx
			y ↦ Py + tVy
			z ↦ Pz + tVz

		to get:

			ry²(Px + tVx)² + rx²(Py + tVy)² = rx²ry²(Pz + tVz)² / h²

		Expand and collect terms:

			(ry²Vx² + rx²Vy² - rx²ry²Vz² / h²)t² + 2(ry²PxVx + rx²PyVy - rx²ry²PzVz / h²)t + (ry²Px² + rx²Py² - rx²ry²Pz² / h²) = 0

		This has the form at² + 2bt + c = 0, where

			a = ry²Vx² + rx²Vy² - rx²ry²Vz² / h²
			b = ry²PxVx + rx²PyVy - rx²ry²PzVz / h²
			c = ry²Px² + rx²Py² - rx²ry²Pz² / h²

		Solving for t with the quadratic formula gives:

			t = (-b ± √(b² - ac)) / a

		A value of t = 0 corresponds to the rasterized point P, and a value of t = 1 corresponds to the
		camera position C. Values of at greater than 1 are behind the camera, so we clamp t to a maximum of 1.

		The difference between t = 0 and t = 1 corresponds to the distance |V|, so multiplying by |V| converts
		a value of t to an actual distance, and multiplying a distance by 1 / |V| produces a value in t-space.

		The depth z already stored in the framebuffer may be less than the depth of the point P. The depth z is the
		distance from the camera plane, so it needs to be converted to a length z′ along the direction V as follows:

			z′ = z / ((-V ∧ K) / |V|)

		Here, K is the antivector perpendicular to the camera's viewing direction. Multiplying by 1 / |V| and
		subtracting from the camera position t = 1 gives us the minimum allowable value of t:

			t₀ = 1 - z′ / |V| = 1 + z / (V ∧ K)

		The u values for the final endpoints of the line segment intersecting the light shaft are then:

			t₁ = min(max((-b - √(b² - ac)) / a, t₀), 1)
			t₂ = min(max((-b + √(b² - ac)) / a, t₀), 1)

		The total distance D inside the light shaft is:

			D = (t₂ - t₁)|V|
	*/

	static const char code[] =
	{
		"float pz = " FPARAM(FRAGMENT_PARAM_CONSTANT0) ".w - $POSI.z;\n"
		FLOAT2 " pos2 = $POSI.xy * $POSI.xy;\n"
		FLOAT3 " dir2 = $OVDR.xyz * $OVDR.xyz;\n"

		"float a = " FPARAM(FRAGMENT_PARAM_CONSTANT0) ".y * dir2.x + " FPARAM(FRAGMENT_PARAM_CONSTANT0) ".x * dir2.y - " FPARAM(FRAGMENT_PARAM_CONSTANT0) ".z * dir2.z;\n"
		"float b = " FPARAM(FRAGMENT_PARAM_CONSTANT0) ".y * $POSI.x * $OVDR.x + " FPARAM(FRAGMENT_PARAM_CONSTANT0) ".x * $POSI.y * $OVDR.y + " FPARAM(FRAGMENT_PARAM_CONSTANT0) ".z * pz * $OVDR.z;\n"
		"float c = " FPARAM(FRAGMENT_PARAM_CONSTANT0) ".y * pos2.x + " FPARAM(FRAGMENT_PARAM_CONSTANT0) ".x * pos2.y - " FPARAM(FRAGMENT_PARAM_CONSTANT0) ".z * pz * pz;\n"
		"float d = sqrt(max(b * b - a * c, 0.0));\n"

		"float u1 = (-d - b) / a;\n"
		"float u2 = (d - b) / a;\n"

		"bool lin = (abs(a) < 0.0001220703125);\n"
		"if (lin)\n"
		"{\n"
			"u1 = c / b * -0.5;\n"
			"u2 = u1;\n"
		"}\n"

		"float z1 = pz - $OVDR.z * u1;\n"
		"float z2 = pz - $OVDR.z * u2;\n"
		"if (max(z1, z2) < 0.0) discard;\n"

		FLOAT2 " depth = %TRG0(%IMG0, " FRAGMENT_POSITION ".xy" FRAGMENT_XY_MULTIPLIER ").zw;\n"
		"float t1 = 1.0 + (depth.x + depth.y * 0.25) / dot($OVDR, " FPARAM(FRAGMENT_PARAM_CONSTANT2) ".xyz);\n"
		"float t2 = 1.0;\n"

		"if (lin)\n"
		"{\n"
			"z1 = -$OVDR.z;\n"
			"z2 = $OVDR.z;\n"
		"}\n"

		"if (z1 > 0.0) t1 = max(t1, u1);\n"
		"if (z2 > 0.0) t2 = min(t2, u2);\n"
	};

	shaderCode[0] = code;
	return (ShaftProcess::GenerateShaderCode(compileData, shaderCode));
}


ShaftEffectObject::ShaftEffectObject(ShaftType type, Volume *volume) :
		EffectObject(kEffectShaft),
		VolumeObject(volume)
{
	shaftType = type;

	shaftDensity[0] = 1.0F;
	shaftDensity[1] = 1.0F;

	topPlaneCullMargin = 0.0F;
}

ShaftEffectObject::ShaftEffectObject(ShaftType type, Volume *volume, const ColorRGBA& color) :
		EffectObject(kEffectShaft),
		VolumeObject(volume)
{
	shaftType = type;

	shaftFlags = 0;
	shaftColor = color;
	shaftDensity[0] = 1.0F;
	shaftDensity[1] = 1.0F;

	intensityScale.Set(0.0625F, 0.0625F);
	intensityHoldTime[0].Set(0.0F, 0.0F);
	intensityHoldTime[1].Set(0.0F, 0.0F);
	intensityTransitionTime.Set(5000.0F, 5000.0F);

	topPlaneCullMargin = 0.0F;
}

ShaftEffectObject::~ShaftEffectObject()
{
}

ShaftEffectObject *ShaftEffectObject::Create(Unpacker& data, unsigned_int32 unpackFlags)
{
	switch (data.GetType())
	{
		case kShaftBox:

			return (new BoxShaftEffectObject);

		case kShaftCylinder:

			return (new CylinderShaftEffectObject);

		case kShaftTruncatedPyramid:

			return (new TruncatedPyramidShaftEffectObject);

		case kShaftTruncatedCone:

			return (new TruncatedConeShaftEffectObject);
	}

	return (nullptr);
}

void ShaftEffectObject::PackType(Packer& data) const
{
	EffectObject::PackType(data);
	data << shaftType;
}

void ShaftEffectObject::Pack(Packer& data, unsigned_int32 packFlags) const
{
	EffectObject::Pack(data, packFlags);

	data << ChunkHeader('FLAG', 4);
	data << shaftFlags;

	data << ChunkHeader('COLR', sizeof(ColorRGBA));
	data << shaftColor;

	data << ChunkHeader('DENS', 8);
	data << shaftDensity[0];
	data << shaftDensity[1];

	data << ChunkHeader('SCAL', sizeof(Range<float>));
	data << intensityScale;

	data << ChunkHeader('HOLD', sizeof(Range<float>) * 2);
	data << intensityHoldTime[0];
	data << intensityHoldTime[1];

	data << ChunkHeader('TRAN', sizeof(Range<float>));
	data << intensityTransitionTime;

	data << ChunkHeader('TPCM', 4);
	data << topPlaneCullMargin;

	data << TerminatorChunk;

	PackVolume(data, packFlags);
}

void ShaftEffectObject::Unpack(Unpacker& data, unsigned_int32 unpackFlags)
{
	EffectObject::Unpack(data, unpackFlags);
	UnpackChunkList<ShaftEffectObject>(data, unpackFlags);
	UnpackVolume(data, unpackFlags);
}

bool ShaftEffectObject::UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags)
{
	switch (chunkHeader->chunkType)
	{
		case 'FLAG':

			data >> shaftFlags;
			return (true);

		case 'COLR':

			data >> shaftColor;
			return (true);

		case 'DENS':

			data >> shaftDensity[0];
			data >> shaftDensity[1];
			return (true);

		case 'SCAL':

			data >> intensityScale;
			return (true);

		case 'HOLD':

			data >> intensityHoldTime[0];
			data >> intensityHoldTime[1];
			return (true);

		case 'TRAN':

			data >> intensityTransitionTime;
			return (true);

		case 'TPCM':

			data >> topPlaneCullMargin;
			return (true);
	}

	return (false);
}

int32 ShaftEffectObject::GetCategoryCount(void) const
{
	return (1);
}

Type ShaftEffectObject::GetCategoryType(int32 index, const char **title) const
{
	if (index == 0)
	{
		*title = TheInterfaceMgr->GetStringTable()->GetString(StringID(kEffectShaft));
		return (kEffectShaft);
	}

	return (0);
}

int32 ShaftEffectObject::GetCategorySettingCount(Type category) const
{
	if (category == kEffectShaft)
	{
		return (17);
	}

	return (0);
}

Setting *ShaftEffectObject::GetCategorySetting(Type category, int32 index, unsigned_int32 flags) const
{
	if (category == kEffectShaft)
	{
		const StringTable *table = TheInterfaceMgr->GetStringTable();

		if (index == 0)
		{
			const char *title = table->GetString(StringID(kEffectShaft, 'SHFT'));
			return (new HeadingSetting(kEffectShaft, title));
		}

		if (index == 1)
		{
			const char *title = table->GetString(StringID(kEffectShaft, 'SHFT', 'COLR'));
			const char *picker = table->GetString(StringID(kEffectShaft, 'SHFT', 'PICK'));
			return (new ColorSetting('COLR', shaftColor, title, picker));
		}

		if (index == 2)
		{
			if (flags & kConfigurationScript)
			{
				return (nullptr);
			}

			const char *title = table->GetString(StringID(kEffectShaft, 'SHFT', 'BDNS'));
			return (new FloatSetting('BDNS', shaftDensity[0], title, 0.0F, 1.0F, 0.01F));
		}

		if (index == 3)
		{
			if (flags & kConfigurationScript)
			{
				return (nullptr);
			}

			const char *title = table->GetString(StringID(kEffectShaft, 'SHFT', 'TDNS'));
			return (new FloatSetting('TDNS', shaftDensity[1], title, 0.0F, 1.0F, 0.01F));
		}

		if (index == 4)
		{
			const char *title = table->GetString(StringID(kEffectShaft, 'SHFT', 'SCL1'));
			return (new TextSetting('SCL1', intensityScale.min, title));
		}

		if (index == 5)
		{
			if (flags & kConfigurationScript)
			{
				return (nullptr);
			}

			const char *title = table->GetString(StringID(kEffectShaft, 'SHFT', 'NOIS'));
			return (new BooleanSetting('NOIS', ((shaftFlags & kShaftRandomNoise) != 0), title));
		}

		if (index == 6)
		{
			if (flags & kConfigurationScript)
			{
				return (nullptr);
			}

			const char *title = table->GetString(StringID(kEffectShaft, 'SHFT', 'CULL'));
			return (new BooleanSetting('CULL', ((shaftFlags & kShaftTopPlaneCull) != 0), title));
		}

		if (index == 7)
		{
			const char *title = table->GetString(StringID(kEffectShaft, 'SHFT', 'TPCM'));
			return (new TextSetting('TPCM', topPlaneCullMargin, title));
		}

		if (index == 8)
		{
			const char *title = table->GetString(StringID(kEffectShaft, 'SHFT', 'SANM'));
			return (new HeadingSetting('SANM', title));
		}

		if (index == 9)
		{
			if (flags & kConfigurationScript)
			{
				return (nullptr);
			}

			const char *title = table->GetString(StringID(kEffectShaft, 'SHFT', 'ANIM'));
			return (new BooleanSetting('ANIM', ((shaftFlags & kShaftAnimateIntensity) != 0), title));
		}

		if (index == 10)
		{
			const char *title = table->GetString(StringID(kEffectShaft, 'SHFT', 'SCL2'));
			return (new TextSetting('SCL2', intensityScale.max, title));
		}

		if (index == 11)
		{
			const char *title = table->GetString(StringID(kEffectShaft, 'SHFT', 'HMN1'));
			return (new TextSetting('HMN1', intensityHoldTime[0].min * 0.001F, title));
		}

		if (index == 12)
		{
			const char *title = table->GetString(StringID(kEffectShaft, 'SHFT', 'HMX1'));
			return (new TextSetting('HMX1', intensityHoldTime[0].max * 0.001F, title));
		}

		if (index == 13)
		{
			const char *title = table->GetString(StringID(kEffectShaft, 'SHFT', 'HMN2'));
			return (new TextSetting('HMN2', intensityHoldTime[1].min * 0.001F, title));
		}

		if (index == 14)
		{
			const char *title = table->GetString(StringID(kEffectShaft, 'SHFT', 'HMX2'));
			return (new TextSetting('HMX2', intensityHoldTime[1].max * 0.001F, title));
		}

		if (index == 15)
		{
			const char *title = table->GetString(StringID(kEffectShaft, 'SHFT', 'TMIN'));
			return (new TextSetting('TMIN', intensityTransitionTime.min * 0.001F, title));
		}

		if (index == 16)
		{
			const char *title = table->GetString(StringID(kEffectShaft, 'SHFT', 'TMAX'));
			return (new TextSetting('TMAX', intensityTransitionTime.max * 0.001F, title));
		}
	}

	return (nullptr);
}

void ShaftEffectObject::SetCategorySetting(Type category, const Setting *setting)
{
	if (category == kEffectShaft)
	{
		Type identifier = setting->GetSettingIdentifier();

		if (identifier == 'COLR')
		{
			shaftColor = static_cast<const ColorSetting *>(setting)->GetColor();
		}
		else if (identifier == 'BDNS')
		{
			shaftDensity[0] = static_cast<const FloatSetting *>(setting)->GetFloatValue();
		}
		else if (identifier == 'TDNS')
		{
			shaftDensity[1] = static_cast<const FloatSetting *>(setting)->GetFloatValue();
		}
		else if (identifier == 'SCL1')
		{
			const char *text = static_cast<const TextSetting *>(setting)->GetText();
			intensityScale.min = FmaxZero(Text::StringToFloat(text));
		}
		else if (identifier == 'NOIS')
		{
			bool b = static_cast<const BooleanSetting *>(setting)->GetBooleanValue();
			if (b) shaftFlags |= kShaftRandomNoise;
			else shaftFlags &= ~kShaftRandomNoise;
		}
		else if (identifier == 'CULL')
		{
			bool b = static_cast<const BooleanSetting *>(setting)->GetBooleanValue();
			if (b) shaftFlags |= kShaftTopPlaneCull;
			else shaftFlags &= ~kShaftTopPlaneCull;
		}
		else if (identifier == 'TPCM')
		{
			const char *text = static_cast<const TextSetting *>(setting)->GetText();
			topPlaneCullMargin = FmaxZero(Text::StringToFloat(text));
		}
		else if (identifier == 'ANIM')
		{
			bool b = static_cast<const BooleanSetting *>(setting)->GetBooleanValue();
			if (b) shaftFlags |= kShaftAnimateIntensity;
			else shaftFlags &= ~kShaftAnimateIntensity;
		}
		else if (identifier == 'SCL2')
		{
			const char *text = static_cast<const TextSetting *>(setting)->GetText();
			intensityScale.max = FmaxZero(Text::StringToFloat(text));
		}
		else if (identifier == 'HMN1')
		{
			const char *text = static_cast<const TextSetting *>(setting)->GetText();
			intensityHoldTime[0].min = FmaxZero(Text::StringToFloat(text)) * 1000.0F;
		}
		else if (identifier == 'HMX1')
		{
			const char *text = static_cast<const TextSetting *>(setting)->GetText();
			intensityHoldTime[0].max = FmaxZero(Text::StringToFloat(text)) * 1000.0F;
		}
		else if (identifier == 'HMN2')
		{
			const char *text = static_cast<const TextSetting *>(setting)->GetText();
			intensityHoldTime[1].min = FmaxZero(Text::StringToFloat(text)) * 1000.0F;
		}
		else if (identifier == 'HMX2')
		{
			const char *text = static_cast<const TextSetting *>(setting)->GetText();
			intensityHoldTime[1].max = FmaxZero(Text::StringToFloat(text)) * 1000.0F;
		}
		else if (identifier == 'TMIN')
		{
			const char *text = static_cast<const TextSetting *>(setting)->GetText();
			intensityTransitionTime.min = FmaxZero(Text::StringToFloat(text)) * 1000.0F;
		}
		else if (identifier == 'TMAX')
		{
			const char *text = static_cast<const TextSetting *>(setting)->GetText();
			intensityTransitionTime.max = FmaxZero(Text::StringToFloat(text)) * 1000.0F;
		}
	}
}

int32 ShaftEffectObject::GetObjectSize(float *size) const
{
	return (GetVolumeObjectSize(size));
}

void ShaftEffectObject::SetObjectSize(const float *size)
{
	SetVolumeObjectSize(size);
}


BoxShaftEffectObject::BoxShaftEffectObject() : ShaftEffectObject(kShaftBox, this)
{
}

BoxShaftEffectObject::BoxShaftEffectObject(const Vector3D& size, const ColorRGBA& color) :
		ShaftEffectObject(kShaftBox, this, color),
		BoxVolume(size)
{
}

BoxShaftEffectObject::~BoxShaftEffectObject()
{
}


CylinderShaftEffectObject::CylinderShaftEffectObject() : ShaftEffectObject(kShaftCylinder, this)
{
}

CylinderShaftEffectObject::CylinderShaftEffectObject(const Vector2D& size, float height, const ColorRGBA& color) :
		ShaftEffectObject(kShaftCylinder, this, color),
		CylinderVolume(size, height)
{
}

CylinderShaftEffectObject::~CylinderShaftEffectObject()
{
}


TruncatedPyramidShaftEffectObject::TruncatedPyramidShaftEffectObject() : ShaftEffectObject(kShaftTruncatedPyramid, this)
{
}

TruncatedPyramidShaftEffectObject::TruncatedPyramidShaftEffectObject(const Vector2D& size, float height, float ratio, const ColorRGBA& color) :
		ShaftEffectObject(kShaftTruncatedPyramid, this, color),
		TruncatedPyramidVolume(size, height, ratio)
{
}

TruncatedPyramidShaftEffectObject::~TruncatedPyramidShaftEffectObject()
{
}


TruncatedConeShaftEffectObject::TruncatedConeShaftEffectObject() : ShaftEffectObject(kShaftTruncatedCone, this)
{
}

TruncatedConeShaftEffectObject::TruncatedConeShaftEffectObject(const Vector2D& size, float height, float ratio, const ColorRGBA& color) :
		ShaftEffectObject(kShaftTruncatedCone, this, color),
		TruncatedConeVolume(size, height, ratio)
{
}

TruncatedConeShaftEffectObject::~TruncatedConeShaftEffectObject()
{
}


ShaftEffect::ShaftEffect(ShaftType type) :
		Effect(kEffectShaft, kRenderIndexedTriangles),
		vertexBuffer(kVertexBufferAttribute | kVertexBufferDynamic)
{
	shaftType = type;
	shaftState = kShaftStatic;
}

ShaftEffect::ShaftEffect(const ShaftEffect& shaftEffect) :
		Effect(shaftEffect),
		vertexBuffer(kVertexBufferAttribute | kVertexBufferDynamic)
{
	shaftType = shaftEffect.shaftType;
	shaftState = kShaftStatic;
}

ShaftEffect::~ShaftEffect()
{
}

ShaftEffect *ShaftEffect::Create(Unpacker& data, unsigned_int32 unpackFlags)
{
	switch (data.GetType())
	{
		case kShaftBox:

			return (new BoxShaftEffect);

		case kShaftCylinder:

			return (new CylinderShaftEffect);

		case kShaftTruncatedPyramid:

			return (new TruncatedPyramidShaftEffect);

		case kShaftTruncatedCone:

			return (new TruncatedConeShaftEffect);
	}

	return (nullptr);
}

void ShaftEffect::PackType(Packer& data) const
{
	Effect::PackType(data);
	data << shaftType;
}

void ShaftEffect::Pack(Packer& data, unsigned_int32 packFlags) const
{
	Effect::Pack(data, packFlags);

	if (shaftState != kShaftStatic)
	{
		data << ChunkHeader('STAT', 4);
		data << shaftState;

		data << ChunkHeader('SCAL', 4);
		data << intensityScale;

		data << ChunkHeader('HOLD', 4);
		data << intensityHoldTime;

		data << ChunkHeader('ANGL', 4);
		data << transitionAngle;

		data << ChunkHeader('SPED', 4);
		data << transitionSpeed;
	}

	data << TerminatorChunk;
}

void ShaftEffect::Unpack(Unpacker& data, unsigned_int32 unpackFlags)
{
	Effect::Unpack(data, unpackFlags);
	UnpackChunkList<ShaftEffect>(data, unpackFlags);
}

bool ShaftEffect::UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags)
{
	switch (chunkHeader->chunkType)
	{
		case 'STAT':

			data >> shaftState;
			return (true);

		case 'SCAL':

			data >> intensityScale;
			return (true);

		case 'HOLD':

			data >> intensityHoldTime;
			return (true);

		case 'ANGL':

			data >> transitionAngle;
			return (true);

		case 'SPED':

			data >> transitionSpeed;
			return (true);
	}

	return (false);
}

bool ShaftEffect::CalculateBoundingBox(Box3D *box) const
{
	GetObject()->CalculateBoundingBox(box);
	return (true);
}

bool ShaftEffect::CalculateBoundingSphere(BoundingSphere *sphere) const
{
	GetObject()->CalculateBoundingSphere(sphere);
	return (true);
}

void ShaftEffect::Preprocess(void)
{
	Effect::Preprocess();

	SetTransformable(this);
	SetTransparentPosition(&GetWorldPosition());
	SetShaderFlags(kShaderAmbientEffect);
	SetAmbientBlendState(kBlendAccumulate | kBlendAlphaPreserve);

	attributeList.Append(&shaderAttribute);
	SetMaterialAttributeList(&attributeList);

	SetVertexBuffer(kVertexBufferAttributeArray, &vertexBuffer, sizeof(Point3D));
	SetVertexAttributeArray(kArrayPosition, 0, 3);

	shaftParams[0].z = 0.0F;
	shaftParams[0].w = 0.0F;
	shaftParams[1].w = 0.0F;
	shaftParams[2].w = 0.0F;
	shaftParams[3].w = 0.0F;

	ProcessObjectSettings();
	UpdateEffectGeometry();

	const ShaftEffectObject *object = GetObject();
	if (shaftState == kShaftStatic)
	{
		if (object->GetShaftFlags() & kShaftAnimateIntensity)
		{
			switch (Math::Random(4))
			{
				case 0:

					shaftState = kShaftHold1;
					intensityScale = object->GetIntensityScale().min;
					intensityHoldTime = Math::RandomFloat(object->GetIntensityHoldTime(0).max);
					transitionAngle = 0.0F;
					transitionSpeed = 0.0F;
					break;

				case 1:

					shaftState = kShaftHold2;
					intensityScale = object->GetIntensityScale().max;
					intensityHoldTime = Math::RandomFloat(object->GetIntensityHoldTime(1).max);
					transitionAngle = 0.0F;
					transitionSpeed = 0.0F;
					break;

				case 2:
				{
					shaftState = kShaftTransition1;

					float angle = Math::RandomFloat(K::tau_over_2);
					transitionAngle = angle;

					const Range<float>& time = object->GetIntensityTransitionTime();
					transitionSpeed = K::tau_over_2 / (Math::RandomFloat(time.max - time.min) + time.min);

					const Range<float>& scale = object->GetIntensityScale();
					intensityScale = scale.min - (Cos(angle) * 0.5F - 0.5F) * (scale.max - scale.min);

					intensityHoldTime = 0.0F;
					break;
				}

				case 3:
				{
					shaftState = kShaftTransition2;

					float angle = Math::RandomFloat(K::tau_over_2);
					transitionAngle = angle;

					const Range<float>& time = object->GetIntensityTransitionTime();
					transitionSpeed = K::tau_over_2 / (Math::RandomFloat(time.max - time.min) + time.min);

					const Range<float>& scale = object->GetIntensityScale();
					intensityScale = scale.max + (Cos(angle) * 0.5F - 0.5F) * (scale.max - scale.min);

					intensityHoldTime = 0.0F;
					break;
				}
			}
		}
		else
		{
			intensityScale = object->GetIntensityScale().min;
		}
	}
}

void ShaftEffect::Neutralize(void)
{
	shaderAttribute.GetShaderGraph()->Purge();
	vertexBuffer.Establish(0);

	Effect::Neutralize();
}

void ShaftEffect::ProcessObjectSettings(void)
{
	const ShaftEffectObject *object = GetObject();

	if (object->GetShaftFlags() & kShaftTopPlaneCull)
	{
		maxCameraHeight = object->GetVolumeLength() + object->GetTopPlaneCullMargin();
	}
	else
	{
		maxCameraHeight = K::infinity;
	}

	float d1 = object->GetShaftDensity(0);
	float d2 = object->GetShaftDensity(1);

	densityAlpha = (d2 - d1) / object->GetVolumeLength();
	densityBeta = d1;

	shaftParams[1].z = -d1 / densityAlpha;

	if (shaftState == kShaftStatic)
	{
		intensityScale = object->GetIntensityScale().min;
	}

	const ColorRGBA& color = object->GetShaftColor();
	shaftParams[3].GetVector3D().Set(color.red, color.green, color.blue);
}

void ShaftEffect::Render(const FrustumCamera *camera, List<Renderable> *effectList)
{
	if ((TheGraphicsMgr->GetGraphicsActiveFlags() & kGraphicsActiveStructureEffects) != 0)
	{
		const Transform4D& inverseTransform = GetInverseWorldTransform();
		if ((inverseTransform.GetRow(2) ^ camera->GetWorldPosition()) < maxCameraHeight)
		{
			unsigned_int32 state = shaftState;
			if (state != kShaftStatic)
			{
				float dt = TheTimeMgr->GetFloatDeltaTime();

				if (state <= kShaftHold2)
				{
					float time = intensityHoldTime - dt;
					if (time > 0.0F)
					{
						intensityHoldTime = time;
					}
					else
					{
						shaftState = state + 2;
						transitionAngle = 0.0F;

						const Range<float>& transition = GetObject()->GetIntensityTransitionTime();
						transitionSpeed = K::tau_over_2 / Fmax((Math::RandomFloat(transition.max - transition.min) + transition.min), 1.0F);
					}
				}
				else
				{
					const ShaftEffectObject *object = GetObject();
					const Range<float>& scale = object->GetIntensityScale();

					float angle = transitionAngle + transitionSpeed * dt;
					if (angle < K::tau_over_2)
					{
						transitionAngle = angle;

						if (state == kShaftTransition1)
						{
							intensityScale = scale.min - (Cos(angle) * 0.5F - 0.5F) * (scale.max - scale.min);
						}
						else
						{
							intensityScale = scale.max + (Cos(angle) * 0.5F - 0.5F) * (scale.max - scale.min);
						}
					}
					else
					{
						if (state == kShaftTransition1)
						{
							shaftState = kShaftHold2;
							intensityScale = scale.max;

							const Range<float>& hold = object->GetIntensityHoldTime(1);
							intensityHoldTime = Math::RandomFloat(hold.max - hold.min) + hold.min;
						}
						else
						{
							shaftState = kShaftHold1;
							intensityScale = scale.min;

							const Range<float>& hold = object->GetIntensityHoldTime(0);
							intensityHoldTime = Math::RandomFloat(hold.max - hold.min) + hold.min;
						}
					}
				}

				if (intensityScale == 0.0F)
				{
					return;
				}
			}

			float scale = -intensityScale;
			shaftParams[1].x = scale * densityAlpha;
			shaftParams[1].y = scale * densityBeta;

			shaftParams[2].GetVector3D() = inverseTransform * camera->GetWorldTransform()[2];

			shaftParams[2].w = Math::RandomFloat(1.0F);
			shaftParams[3].w = Math::RandomFloat(1.0F);

			effectList[GetEffectListIndex()].Append(this);
		}
	}
}


BoxShaftEffect::BoxShaftEffect() : ShaftEffect(kShaftBox)
{
	Initialize();
}

BoxShaftEffect::BoxShaftEffect(const Vector3D& size, const ColorRGBA& color) : ShaftEffect(kShaftBox)
{
	SetNewObject(new BoxShaftEffectObject(size, color));
	Initialize();
}

BoxShaftEffect::BoxShaftEffect(const BoxShaftEffect& boxShaftEffect) : ShaftEffect(boxShaftEffect)
{
	Initialize();
}

BoxShaftEffect::~BoxShaftEffect()
{
	indexBuffer.Release();
}

void BoxShaftEffect::Initialize(void)
{
	static const Triangle shaftTriangle[12] =
	{
		{{0, 1, 3}}, {{0, 3, 2}}, {{4, 6, 7}}, {{4, 7, 5}},
		{{0, 5, 1}}, {{0, 4, 5}}, {{1, 5, 7}}, {{1, 7, 3}},
		{{3, 7, 6}}, {{3, 6, 2}}, {{2, 6, 4}}, {{2, 4, 0}}
	};

	if (indexBuffer.Retain() == 1)
	{
		indexBuffer.Establish(sizeof(Triangle) * 12, shaftTriangle);
	}
}

Node *BoxShaftEffect::Replicate(void) const
{
	return (new BoxShaftEffect(*this));
}

void BoxShaftEffect::HandleTransformUpdate(void)
{
	ShaftEffect::HandleTransformUpdate();

	const BoxShaftEffectObject *object = GetObject();
	const Vector3D& boxSize = object->GetBoxSize() * 0.5F;

	const Transform4D& transform = GetWorldTransform();
	worldCenter = transform.GetTranslation() + transform * boxSize;
	worldAxis[0] = transform[0] * boxSize.x;
	worldAxis[1] = transform[1] * boxSize.y;
	worldAxis[2] = transform[2] * boxSize.z;
}

bool BoxShaftEffect::ShaftVisible(const Node *node, const VisibilityRegion *region)
{
	const BoxShaftEffect *shaft = static_cast<const BoxShaftEffect *>(node);
	return (region->BoxVisible(shaft->worldCenter, shaft->worldAxis));
}

bool BoxShaftEffect::ShaftVisible(const Node *node, const VisibilityRegion *region, const List<OcclusionRegion> *occlusionList)
{
	const BoxShaftEffect *shaft = static_cast<const BoxShaftEffect *>(node);

	if (region->BoxVisible(shaft->worldCenter, shaft->worldAxis))
	{
		const OcclusionRegion *occluder = occlusionList->First();
		while (occluder)
		{
			if (occluder->BoxOccluded(shaft->worldCenter, shaft->worldAxis))
			{
				return (false);
			}

			occluder = occluder->Next();
		}

		return (true);
	}

	return (false);
}

void BoxShaftEffect::Preprocess(void)
{
	SetPrimitiveCount(12);
	SetVertexBuffer(kVertexBufferIndexArray, &indexBuffer);

	SetVertexCount(8);
	vertexBuffer.Establish(sizeof(Point3D) * 8);

	ShaftEffect::Preprocess();

	ShaderGraph *graph = shaderAttribute.GetShaderGraph();

	const BoxShaftEffectObject *object = GetObject();
	unsigned_int32 flags = (object->GetShaftDensity(0) != object->GetShaftDensity(1)) ? kShaftProcessGradient : 0;
	if (object->GetShaftFlags() & kShaftRandomNoise)
	{
		flags |= kShaftProcessNoise;
	}

	BoxShaftProcess *boxShaftProcess = new BoxShaftProcess(flags, &shaftParams);
	graph->AddElement(boxShaftProcess);

	AmbientOutputProcess *ambientOutputProcess = new AmbientOutputProcess;
	graph->AddElement(ambientOutputProcess);

	new Route(boxShaftProcess, ambientOutputProcess, 0);

	SetVisibilityProc(&ShaftVisible);
	SetOcclusionProc(&ShaftVisible);
}

void BoxShaftEffect::UpdateEffectGeometry(void)
{
	volatile Point3D *restrict vertex = vertexBuffer.BeginUpdateSync<Point3D>();

	const Vector3D& size = GetObject()->GetBoxSize();
	shaftParams[0].GetVector2D().Set(size.x, size.y);

	vertex[0].Set(0.0F, 0.0F, 0.0F);
	vertex[1].Set(size.x, 0.0F, 0.0F);
	vertex[2].Set(0.0F, size.y, 0.0F);
	vertex[3].Set(size.x, size.y, 0.0F);

	vertex[4].Set(0.0F, 0.0F, size.z);
	vertex[5].Set(size.x, 0.0F, size.z);
	vertex[6].Set(0.0F, size.y, size.z);
	vertex[7].Set(size.x, size.y, size.z);

	vertexBuffer.EndUpdateSync();
}


CylinderShaftEffect::CylinderShaftEffect() : ShaftEffect(kShaftCylinder)
{
	Initialize();
}

CylinderShaftEffect::CylinderShaftEffect(const Vector2D& size, float height, const ColorRGBA& color) : ShaftEffect(kShaftCylinder)
{
	SetNewObject(new CylinderShaftEffectObject(size, height, color));
	Initialize();
}

CylinderShaftEffect::CylinderShaftEffect(const CylinderShaftEffect& cylinderShaftEffect) : ShaftEffect(cylinderShaftEffect)
{
	Initialize();
}

CylinderShaftEffect::~CylinderShaftEffect()
{
	indexBuffer.Release();
}

void CylinderShaftEffect::Initialize(void)
{
	if (indexBuffer.Retain() == 1)
	{
		indexBuffer.Establish(sizeof(Triangle) * kCylinderShaftTriangleCount);

		volatile Triangle *restrict triangle = indexBuffer.BeginUpdateSync<Triangle>();

		for (machine a = 0; a < kCylinderShaftSubdivCount; a++)
		{
			machine b = (a + 1) & (kCylinderShaftSubdivCount - 1);
			triangle[0].Set(a, a + kCylinderShaftSubdivCount, b);
			triangle[1].Set(b, a + kCylinderShaftSubdivCount, b + kCylinderShaftSubdivCount);
			triangle += 2;
		}

		machine d1 = 1;
		machine d2 = 2;
		do
		{
			for (machine a = 0; a < kCylinderShaftSubdivCount; a += d2)
			{
				triangle->Set(a, a + d1, (a + d2) & (kCylinderShaftSubdivCount - 1));
				triangle++;
			}

			d1 *= 2;
			d2 *= 2;
		} while (d2 < kCylinderShaftSubdivCount);

		d1 = 1;
		d2 = 2;
		do
		{
			for (machine a = 0; a < kCylinderShaftSubdivCount; a += d2)
			{
				triangle->Set(a + kCylinderShaftSubdivCount, ((a + d2) & (kCylinderShaftSubdivCount - 1)) + kCylinderShaftSubdivCount, a + d1 + kCylinderShaftSubdivCount);
				triangle++;
			}

			d1 *= 2;
			d2 *= 2;
		} while (d2 < kCylinderShaftSubdivCount);

		indexBuffer.EndUpdateSync();
	}
}

Node *CylinderShaftEffect::Replicate(void) const
{
	return (new CylinderShaftEffect(*this));
}

void CylinderShaftEffect::HandleTransformUpdate(void)
{
	ShaftEffect::HandleTransformUpdate();

	const CylinderShaftEffectObject *object = GetObject();
	worldEndpoint = GetWorldPosition() + GetWorldTransform()[2] * object->GetCylinderHeight();

	const Vector2D& size = object->GetCylinderSize();
	cylinderRadius = Fmax(size.x, size.y);
}

bool CylinderShaftEffect::ShaftVisible(const Node *node, const VisibilityRegion *region)
{
	const CylinderShaftEffect *shaft = static_cast<const CylinderShaftEffect *>(node);

	const Point3D& p1 = shaft->GetWorldPosition();
	const Point3D& p2 = shaft->worldEndpoint;

	return (region->CylinderVisible(p1, p2, shaft->cylinderRadius));
}

bool CylinderShaftEffect::ShaftVisible(const Node *node, const VisibilityRegion *region, const List<OcclusionRegion> *occlusionList)
{
	const CylinderShaftEffect *shaft = static_cast<const CylinderShaftEffect *>(node);

	const Point3D& p1 = shaft->GetWorldPosition();
	const Point3D& p2 = shaft->worldEndpoint;
	float radius = shaft->cylinderRadius;

	if (region->CylinderVisible(p1, p2, radius))
	{
		const OcclusionRegion *occluder = occlusionList->First();
		while (occluder)
		{
			if (occluder->CylinderOccluded(p1, p2, radius))
			{
				return (false);
			}

			occluder = occluder->Next();
		}

		return (true);
	}

	return (false);
}

void CylinderShaftEffect::Preprocess(void)
{
	SetPrimitiveCount(kCylinderShaftTriangleCount);
	SetVertexBuffer(kVertexBufferIndexArray, &indexBuffer);

	SetVertexCount(kCylinderShaftVertexCount);
	vertexBuffer.Establish(sizeof(Point3D) * kCylinderShaftVertexCount);

	ShaftEffect::Preprocess();

	ShaderGraph *graph = shaderAttribute.GetShaderGraph();

	const CylinderShaftEffectObject *object = GetObject();
	unsigned_int32 flags = (object->GetShaftDensity(0) != object->GetShaftDensity(1)) ? kShaftProcessGradient : 0;
	if (object->GetShaftFlags() & kShaftRandomNoise)
	{
		flags |= kShaftProcessNoise;
	}

	CylinderShaftProcess *cylinderShaftProcess = new CylinderShaftProcess(flags, &shaftParams);
	graph->AddElement(cylinderShaftProcess);

	AmbientOutputProcess *ambientOutputProcess = new AmbientOutputProcess;
	graph->AddElement(ambientOutputProcess);

	new Route(cylinderShaftProcess, ambientOutputProcess, 0);

	SetVisibilityProc(&ShaftVisible);
	SetOcclusionProc(&ShaftVisible);
}

void CylinderShaftEffect::UpdateEffectGeometry(void)
{
	volatile Point3D *restrict vertex = vertexBuffer.BeginUpdateSync<Point3D>();

	const CylinderShaftEffectObject *object = GetObject();
	const Vector2D& size = object->GetCylinderSize();
	float height = object->GetCylinderHeight();

	float rx2 = size.x * size.x;
	float ry2 = size.y * size.y;
	shaftParams[0].GetVector3D().Set(rx2, ry2, rx2 * ry2);

	float t = 0.0F;
	float dt = K::tau / (float) kCylinderShaftSubdivCount;

	float m = 1.0F / Cos(dt * 0.5F);
	float sx = size.x * m;
	float sy = size.y * m;

	for (machine a = 0; a < kCylinderShaftSubdivCount; a++)
	{
		float	x, y;

		CosSin(t, &x, &y);
		x *= sx;
		y *= sy;

		vertex[0].Set(x, y, 0.0F);
		vertex[kCylinderShaftSubdivCount].Set(x, y, height);
		vertex++;

		t += dt;
	}

	vertexBuffer.EndUpdateSync();
}


TruncatedPyramidShaftEffect::TruncatedPyramidShaftEffect() : ShaftEffect(kShaftTruncatedPyramid)
{
	Initialize();
}

TruncatedPyramidShaftEffect::TruncatedPyramidShaftEffect(const Vector2D& size, float height, float ratio, const ColorRGBA& color) : ShaftEffect(kShaftTruncatedPyramid)
{
	SetNewObject(new TruncatedPyramidShaftEffectObject(size, height, ratio, color));
	Initialize();
}

TruncatedPyramidShaftEffect::TruncatedPyramidShaftEffect(const TruncatedPyramidShaftEffect& truncatedPyramidShaftEffect) : ShaftEffect(truncatedPyramidShaftEffect)
{
	Initialize();
}

TruncatedPyramidShaftEffect::~TruncatedPyramidShaftEffect()
{
	indexBuffer.Release();
}

void TruncatedPyramidShaftEffect::Initialize(void)
{
	static const Triangle shaftTriangle[12] =
	{
		{{0, 1, 3}}, {{0, 3, 2}}, {{4, 6, 7}}, {{4, 7, 5}},
		{{0, 5, 1}}, {{0, 4, 5}}, {{1, 5, 7}}, {{1, 7, 3}},
		{{3, 7, 6}}, {{3, 6, 2}}, {{2, 6, 4}}, {{2, 4, 0}}
	};

	if (indexBuffer.Retain() == 1)
	{
		indexBuffer.Establish(sizeof(Triangle) * 12, shaftTriangle);
	}
}

Node *TruncatedPyramidShaftEffect::Replicate(void) const
{
	return (new TruncatedPyramidShaftEffect(*this));
}

void TruncatedPyramidShaftEffect::HandleTransformUpdate(void)
{
	ShaftEffect::HandleTransformUpdate();

	const TruncatedPyramidShaftEffectObject *object = GetObject();
	Vector3D pyramidSize(object->GetPyramidSize() * 0.5F, object->GetPyramidHeight() * 0.5F);

	const Transform4D& transform = GetWorldTransform();
	worldCenter = transform.GetTranslation() + transform * pyramidSize;
	worldAxis[0] = transform[0] * pyramidSize.x;
	worldAxis[1] = transform[1] * pyramidSize.y;
	worldAxis[2] = transform[2] * pyramidSize.z;
}

bool TruncatedPyramidShaftEffect::ShaftVisible(const Node *node, const VisibilityRegion *region)
{
	const TruncatedPyramidShaftEffect *shaft = static_cast<const TruncatedPyramidShaftEffect *>(node);
	return (region->BoxVisible(shaft->worldCenter, shaft->worldAxis));
}

bool TruncatedPyramidShaftEffect::ShaftVisible(const Node *node, const VisibilityRegion *region, const List<OcclusionRegion> *occlusionList)
{
	const TruncatedPyramidShaftEffect *shaft = static_cast<const TruncatedPyramidShaftEffect *>(node);

	if (region->BoxVisible(shaft->worldCenter, shaft->worldAxis))
	{
		const OcclusionRegion *occluder = occlusionList->First();
		while (occluder)
		{
			if (occluder->BoxOccluded(shaft->worldCenter, shaft->worldAxis))
			{
				return (false);
			}

			occluder = occluder->Next();
		}

		return (true);
	}

	return (false);
}

void TruncatedPyramidShaftEffect::Preprocess(void)
{
	SetPrimitiveCount(12);
	SetVertexBuffer(kVertexBufferIndexArray, &indexBuffer);

	SetVertexCount(8);
	vertexBuffer.Establish(sizeof(Point3D) * 8);

	ShaftEffect::Preprocess();

	ShaderGraph *graph = shaderAttribute.GetShaderGraph();

	const TruncatedPyramidShaftEffectObject *object = GetObject();
	unsigned_int32 flags = (object->GetShaftDensity(0) != object->GetShaftDensity(1)) ? kShaftProcessGradient : 0;
	if (object->GetShaftFlags() & kShaftRandomNoise)
	{
		flags |= kShaftProcessNoise;
	}

	TruncatedPyramidShaftProcess *truncatedPyramidShaftProcess = new TruncatedPyramidShaftProcess(flags, &shaftParams);
	graph->AddElement(truncatedPyramidShaftProcess);

	AmbientOutputProcess *ambientOutputProcess = new AmbientOutputProcess;
	graph->AddElement(ambientOutputProcess);

	new Route(truncatedPyramidShaftProcess, ambientOutputProcess, 0);

	SetVisibilityProc(&ShaftVisible);
	SetOcclusionProc(&ShaftVisible);
}

void TruncatedPyramidShaftEffect::UpdateEffectGeometry(void)
{
	volatile Point3D *restrict vertex = vertexBuffer.BeginUpdateSync<Point3D>();

	const TruncatedPyramidShaftEffectObject *object = GetObject();
	const Vector2D& size = object->GetPyramidSize();
	float height = object->GetPyramidHeight();
	float ratio = object->GetPyramidRatio();

	float f = 1.0F - ratio;
	float h = height / f;
	float sx = size.x * 0.5F;
	float sy = size.y * 0.5F;
	shaftParams[0].GetVector3D().Set(sx, sy, h);

	vertex[0].Set(0.0F, 0.0F, 0.0F);
	vertex[1].Set(size.x, 0.0F, 0.0F);
	vertex[2].Set(0.0F, size.y, 0.0F);
	vertex[3].Set(size.x, size.y, 0.0F);

	sx *= f;
	sy *= f;

	vertex[4].Set(sx, sy, height);
	vertex[5].Set(size.x - sx, sy, height);
	vertex[6].Set(sx, size.y - sy, height);
	vertex[7].Set(size.x - sx, size.y - sy, height);

	vertexBuffer.EndUpdateSync();
}


TruncatedConeShaftEffect::TruncatedConeShaftEffect() : ShaftEffect(kShaftTruncatedCone)
{
	Initialize();
}

TruncatedConeShaftEffect::TruncatedConeShaftEffect(const Vector2D& size, float height, float ratio, const ColorRGBA& color) : ShaftEffect(kShaftTruncatedCone)
{
	SetNewObject(new TruncatedConeShaftEffectObject(size, height, ratio, color));
	Initialize();
}

TruncatedConeShaftEffect::TruncatedConeShaftEffect(const TruncatedConeShaftEffect& truncatedConeShaftEffect) : ShaftEffect(truncatedConeShaftEffect)
{
	Initialize();
}

TruncatedConeShaftEffect::~TruncatedConeShaftEffect()
{
	indexBuffer.Release();
}

void TruncatedConeShaftEffect::Initialize(void)
{
	if (indexBuffer.Retain() == 1)
	{
		indexBuffer.Establish(sizeof(Triangle) * kTruncatedConeShaftTriangleCount);

		volatile Triangle *restrict triangle = indexBuffer.BeginUpdateSync<Triangle>();

		for (machine a = 0; a < kTruncatedConeShaftSubdivCount; a++)
		{
			machine b = (a + 1) & (kTruncatedConeShaftSubdivCount - 1);
			triangle[0].Set(a, a + kTruncatedConeShaftSubdivCount, b);
			triangle[1].Set(b, a + kTruncatedConeShaftSubdivCount, b + kTruncatedConeShaftSubdivCount);
			triangle += 2;
		}

		machine d1 = 1;
		machine d2 = 2;
		do
		{
			for (machine a = 0; a < kTruncatedConeShaftSubdivCount; a += d2)
			{
				triangle->Set(a, a + d1, (a + d2) & (kTruncatedConeShaftSubdivCount - 1));
				triangle++;
			}

			d1 *= 2;
			d2 *= 2;
		} while (d2 < kTruncatedConeShaftSubdivCount);

		d1 = 1;
		d2 = 2;
		do
		{
			for (machine a = 0; a < kTruncatedConeShaftSubdivCount; a += d2)
			{
				triangle->Set(a + kTruncatedConeShaftSubdivCount, ((a + d2) & (kTruncatedConeShaftSubdivCount - 1)) + kTruncatedConeShaftSubdivCount, a + d1 + kTruncatedConeShaftSubdivCount);
				triangle++;
			}

			d1 *= 2;
			d2 *= 2;
		} while (d2 < kTruncatedConeShaftSubdivCount);

		indexBuffer.EndUpdateSync();
	}
}

Node *TruncatedConeShaftEffect::Replicate(void) const
{
	return (new TruncatedConeShaftEffect(*this));
}

void TruncatedConeShaftEffect::HandleTransformUpdate(void)
{
	ShaftEffect::HandleTransformUpdate();

	const TruncatedConeShaftEffectObject *object = GetObject();
	worldEndpoint = GetWorldPosition() + GetWorldTransform()[2] * object->GetConeHeight();

	const Vector2D& size = object->GetConeSize();
	cylinderRadius = Fmax(size.x, size.y);
}

bool TruncatedConeShaftEffect::ShaftVisible(const Node *node, const VisibilityRegion *region)
{
	const TruncatedConeShaftEffect *shaft = static_cast<const TruncatedConeShaftEffect *>(node);

	const Point3D& p1 = shaft->GetWorldPosition();
	const Point3D& p2 = shaft->worldEndpoint;

	return (region->CylinderVisible(p1, p2, shaft->cylinderRadius));
}

bool TruncatedConeShaftEffect::ShaftVisible(const Node *node, const VisibilityRegion *region, const List<OcclusionRegion> *occlusionList)
{
	const TruncatedConeShaftEffect *shaft = static_cast<const TruncatedConeShaftEffect *>(node);

	const Point3D& p1 = shaft->GetWorldPosition();
	const Point3D& p2 = shaft->worldEndpoint;
	float radius = shaft->cylinderRadius;

	if (region->CylinderVisible(p1, p2, radius))
	{
		const OcclusionRegion *occluder = occlusionList->First();
		while (occluder)
		{
			if (occluder->CylinderOccluded(p1, p2, radius))
			{
				return (false);
			}

			occluder = occluder->Next();
		}

		return (true);
	}

	return (false);
}

void TruncatedConeShaftEffect::Preprocess(void)
{
	SetPrimitiveCount(kTruncatedConeShaftTriangleCount);
	SetVertexBuffer(kVertexBufferIndexArray, &indexBuffer);

	SetVertexCount(kTruncatedConeShaftVertexCount);
	vertexBuffer.Establish(sizeof(Point3D) * kTruncatedConeShaftVertexCount);

	ShaftEffect::Preprocess();

	ShaderGraph *graph = shaderAttribute.GetShaderGraph();

	const TruncatedConeShaftEffectObject *object = GetObject();
	unsigned_int32 flags = (object->GetShaftDensity(0) != object->GetShaftDensity(1)) ? kShaftProcessGradient : 0;
	if (object->GetShaftFlags() & kShaftRandomNoise)
	{
		flags |= kShaftProcessNoise;
	}

	TruncatedConeShaftProcess *truncatedConeShaftProcess = new TruncatedConeShaftProcess(flags, &shaftParams);
	graph->AddElement(truncatedConeShaftProcess);

	AmbientOutputProcess *ambientOutputProcess = new AmbientOutputProcess;
	graph->AddElement(ambientOutputProcess);

	new Route(truncatedConeShaftProcess, ambientOutputProcess, 0);

	SetVisibilityProc(&ShaftVisible);
	SetOcclusionProc(&ShaftVisible);
}

void TruncatedConeShaftEffect::UpdateEffectGeometry(void)
{
	volatile Point3D *restrict vertex = vertexBuffer.BeginUpdateSync<Point3D>();

	const TruncatedConeShaftEffectObject *object = GetObject();
	const Vector2D& size = object->GetConeSize();
	float height = object->GetConeHeight();
	float ratio = object->GetConeRatio();

	float h = height / (1.0F - ratio);
	float rx2 = size.x * size.x;
	float ry2 = size.y * size.y;
	shaftParams[0].Set(rx2, ry2, rx2 * ry2 / (h * h), h);

	float t = 0.0F;
	float dt = K::tau / (float) kTruncatedConeShaftSubdivCount;

	float m = 1.0F / Cos(dt * 0.5F);
	float sx = size.x * m;
	float sy = size.y * m;

	for (machine a = 0; a < kTruncatedConeShaftSubdivCount; a++)
	{
		float	x, y;

		CosSin(t, &x, &y);
		x *= sx;
		y *= sy;

		vertex[0].Set(x, y, 0.0F);
		vertex[kTruncatedConeShaftSubdivCount].Set(x * ratio, y * ratio, height);
		vertex++;

		t += dt;
	}

	vertexBuffer.EndUpdateSync();
}

// ZYUQURM
