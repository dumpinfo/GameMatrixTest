 

#include "C4Math.h"
#include "C4Vector3D.h"


using namespace C4;


namespace C4
{
	namespace K
	{
		C4API extern const float infinity;
		C4API extern const float min_float;
	}
}


alignas(128) const unsigned_int32 C4::Math::trigTable[256][2] =
{
	{0x3F800000, 0x00000000}, {0x3F7FEC43, 0x3CC90AB0}, {0x3F7FB10F, 0x3D48FB30}, {0x3F7F4E6D, 0x3D96A905}, {0x3F7EC46D, 0x3DC8BD36}, {0x3F7E1324, 0x3DFAB273}, {0x3F7D3AAC, 0x3E164083}, {0x3F7C3B28, 0x3E2F10A3},
	{0x3F7B14BE, 0x3E47C5C2}, {0x3F79C79D, 0x3E605C13}, {0x3F7853F8, 0x3E78CFCD}, {0x3F76BA07, 0x3E888E94}, {0x3F74FA0B, 0x3E94A031}, {0x3F731447, 0x3EA09AE5}, {0x3F710908, 0x3EAC7CD4}, {0x3F6ED89E, 0x3EB8442A},
	{0x3F6C835E, 0x3EC3EF16}, {0x3F6A09A6, 0x3ECF7BCB}, {0x3F676BD8, 0x3EDAE880}, {0x3F64AA59, 0x3EE63375}, {0x3F61C597, 0x3EF15AEA}, {0x3F5EBE05, 0x3EFC5D28}, {0x3F5B941A, 0x3F039C3D}, {0x3F584853, 0x3F08F59B},
	{0x3F54DB31, 0x3F0E39DA}, {0x3F514D3D, 0x3F13682B}, {0x3F4D9F02, 0x3F187FC0}, {0x3F49D112, 0x3F1D7FD2}, {0x3F45E403, 0x3F22679A}, {0x3F41D870, 0x3F273656}, {0x3F3DAEF9, 0x3F2BEB4A}, {0x3F396842, 0x3F3085BB},
	{0x3F3504F3, 0x3F3504F3}, {0x3F3085BA, 0x3F396842}, {0x3F2BEB49, 0x3F3DAEFA}, {0x3F273655, 0x3F41D871}, {0x3F226799, 0x3F45E403}, {0x3F1D7FD1, 0x3F49D112}, {0x3F187FC0, 0x3F4D9F02}, {0x3F13682A, 0x3F514D3D},
	{0x3F0E39D9, 0x3F54DB32}, {0x3F08F59B, 0x3F584853}, {0x3F039C3C, 0x3F5B941B}, {0x3EFC5D27, 0x3F5EBE05}, {0x3EF15AE7, 0x3F61C598}, {0x3EE63374, 0x3F64AA59}, {0x3EDAE881, 0x3F676BD8}, {0x3ECF7BC9, 0x3F6A09A7},
	{0x3EC3EF15, 0x3F6C835E}, {0x3EB84427, 0x3F6ED89E}, {0x3EAC7CD3, 0x3F710908}, {0x3EA09AE2, 0x3F731448}, {0x3E94A030, 0x3F74FA0B}, {0x3E888E93, 0x3F76BA07}, {0x3E78CFC8, 0x3F7853F8}, {0x3E605C12, 0x3F79C79D},
	{0x3E47C5BC, 0x3F7B14BF}, {0x3E2F10A0, 0x3F7C3B28}, {0x3E164085, 0x3F7D3AAC}, {0x3DFAB26C, 0x3F7E1324}, {0x3DC8BD35, 0x3F7EC46D}, {0x3D96A8FB, 0x3F7F4E6D}, {0x3D48FB29, 0x3F7FB10F}, {0x3CC90A7E, 0x3F7FEC43},
	{0x00000000, 0x3F800000}, {0xBCC90A7E, 0x3F7FEC43}, {0xBD48FB29, 0x3F7FB10F}, {0xBD96A8FB, 0x3F7F4E6D}, {0xBDC8BD35, 0x3F7EC46D}, {0xBDFAB26C, 0x3F7E1324}, {0xBE164085, 0x3F7D3AAC}, {0xBE2F10A0, 0x3F7C3B28},
	{0xBE47C5BC, 0x3F7B14BF}, {0xBE605C12, 0x3F79C79D}, {0xBE78CFC8, 0x3F7853F8}, {0xBE888E93, 0x3F76BA07}, {0xBE94A030, 0x3F74FA0B}, {0xBEA09AE2, 0x3F731448}, {0xBEAC7CD3, 0x3F710908}, {0xBEB84427, 0x3F6ED89E},
	{0xBEC3EF15, 0x3F6C835E}, {0xBECF7BC9, 0x3F6A09A7}, {0xBEDAE881, 0x3F676BD8}, {0xBEE63374, 0x3F64AA59}, {0xBEF15AE7, 0x3F61C598}, {0xBEFC5D27, 0x3F5EBE05}, {0xBF039C3C, 0x3F5B941B}, {0xBF08F59B, 0x3F584853},
	{0xBF0E39D9, 0x3F54DB32}, {0xBF13682A, 0x3F514D3D}, {0xBF187FC0, 0x3F4D9F02}, {0xBF1D7FD1, 0x3F49D112}, {0xBF226799, 0x3F45E403}, {0xBF273655, 0x3F41D871}, {0xBF2BEB49, 0x3F3DAEFA}, {0xBF3085BA, 0x3F396842},
	{0xBF3504F3, 0x3F3504F3}, {0xBF396842, 0x3F3085BB}, {0xBF3DAEF9, 0x3F2BEB4A}, {0xBF41D870, 0x3F273656}, {0xBF45E403, 0x3F22679A}, {0xBF49D112, 0x3F1D7FD2}, {0xBF4D9F02, 0x3F187FC0}, {0xBF514D3D, 0x3F13682B},
	{0xBF54DB31, 0x3F0E39DA}, {0xBF584853, 0x3F08F59B}, {0xBF5B941A, 0x3F039C3D}, {0xBF5EBE05, 0x3EFC5D28}, {0xBF61C597, 0x3EF15AEA}, {0xBF64AA59, 0x3EE63375}, {0xBF676BD8, 0x3EDAE880}, {0xBF6A09A6, 0x3ECF7BCB},
	{0xBF6C835E, 0x3EC3EF16}, {0xBF6ED89E, 0x3EB8442A}, {0xBF710908, 0x3EAC7CD4}, {0xBF731447, 0x3EA09AE5}, {0xBF74FA0B, 0x3E94A031}, {0xBF76BA07, 0x3E888E94}, {0xBF7853F8, 0x3E78CFCD}, {0xBF79C79D, 0x3E605C13},
	{0xBF7B14BE, 0x3E47C5C2}, {0xBF7C3B28, 0x3E2F10A3}, {0xBF7D3AAC, 0x3E164083}, {0xBF7E1324, 0x3DFAB273}, {0xBF7EC46D, 0x3DC8BD36}, {0xBF7F4E6D, 0x3D96A905}, {0xBF7FB10F, 0x3D48FB30}, {0xBF7FEC43, 0x3CC90AB0},
	{0xBF800000, 0x00000000}, {0xBF7FEC43, 0xBCC90AB0}, {0xBF7FB10F, 0xBD48FB30}, {0xBF7F4E6D, 0xBD96A905}, {0xBF7EC46D, 0xBDC8BD36}, {0xBF7E1324, 0xBDFAB273}, {0xBF7D3AAC, 0xBE164083}, {0xBF7C3B28, 0xBE2F10A3},
	{0xBF7B14BE, 0xBE47C5C2}, {0xBF79C79D, 0xBE605C13}, {0xBF7853F8, 0xBE78CFCD}, {0xBF76BA07, 0xBE888E94}, {0xBF74FA0B, 0xBE94A031}, {0xBF731447, 0xBEA09AE5}, {0xBF710908, 0xBEAC7CD4}, {0xBF6ED89E, 0xBEB8442A},
	{0xBF6C835E, 0xBEC3EF16}, {0xBF6A09A6, 0xBECF7BCB}, {0xBF676BD8, 0xBEDAE880}, {0xBF64AA59, 0xBEE63375}, {0xBF61C597, 0xBEF15AEA}, {0xBF5EBE05, 0xBEFC5D28}, {0xBF5B941A, 0xBF039C3D}, {0xBF584853, 0xBF08F59B},
	{0xBF54DB31, 0xBF0E39DA}, {0xBF514D3D, 0xBF13682B}, {0xBF4D9F02, 0xBF187FC0}, {0xBF49D112, 0xBF1D7FD2}, {0xBF45E403, 0xBF22679A}, {0xBF41D870, 0xBF273656}, {0xBF3DAEF9, 0xBF2BEB4A}, {0xBF396842, 0xBF3085BB},
	{0xBF3504F3, 0xBF3504F3}, {0xBF3085BA, 0xBF396842}, {0xBF2BEB49, 0xBF3DAEFA}, {0xBF273655, 0xBF41D871}, {0xBF226799, 0xBF45E403}, {0xBF1D7FD1, 0xBF49D112}, {0xBF187FC0, 0xBF4D9F02}, {0xBF13682A, 0xBF514D3D},
	{0xBF0E39D9, 0xBF54DB32}, {0xBF08F59B, 0xBF584853}, {0xBF039C3C, 0xBF5B941B}, {0xBEFC5D27, 0xBF5EBE05}, {0xBEF15AE7, 0xBF61C598}, {0xBEE63374, 0xBF64AA59}, {0xBEDAE881, 0xBF676BD8}, {0xBECF7BC9, 0xBF6A09A7},
	{0xBEC3EF15, 0xBF6C835E}, {0xBEB84427, 0xBF6ED89E}, {0xBEAC7CD3, 0xBF710908}, {0xBEA09AE2, 0xBF731448}, {0xBE94A030, 0xBF74FA0B}, {0xBE888E93, 0xBF76BA07}, {0xBE78CFC8, 0xBF7853F8}, {0xBE605C12, 0xBF79C79D},
	{0xBE47C5BC, 0xBF7B14BF}, {0xBE2F10A0, 0xBF7C3B28}, {0xBE164085, 0xBF7D3AAC}, {0xBDFAB26C, 0xBF7E1324}, {0xBDC8BD35, 0xBF7EC46D}, {0xBD96A8FB, 0xBF7F4E6D}, {0xBD48FB29, 0xBF7FB10F}, {0xBCC90A7E, 0xBF7FEC43},
	{0x00000000, 0xBF800000}, {0x3CC90A7E, 0xBF7FEC43}, {0x3D48FB29, 0xBF7FB10F}, {0x3D96A8FB, 0xBF7F4E6D}, {0x3DC8BD35, 0xBF7EC46D}, {0x3DFAB26C, 0xBF7E1324}, {0x3E164085, 0xBF7D3AAC}, {0x3E2F10A0, 0xBF7C3B28},
	{0x3E47C5BC, 0xBF7B14BF}, {0x3E605C12, 0xBF79C79D}, {0x3E78CFC8, 0xBF7853F8}, {0x3E888E93, 0xBF76BA07}, {0x3E94A030, 0xBF74FA0B}, {0x3EA09AE2, 0xBF731448}, {0x3EAC7CD3, 0xBF710908}, {0x3EB84427, 0xBF6ED89E},
	{0x3EC3EF15, 0xBF6C835E}, {0x3ECF7BC9, 0xBF6A09A7}, {0x3EDAE881, 0xBF676BD8}, {0x3EE63374, 0xBF64AA59}, {0x3EF15AE7, 0xBF61C598}, {0x3EFC5D27, 0xBF5EBE05}, {0x3F039C3C, 0xBF5B941B}, {0x3F08F59B, 0xBF584853},
	{0x3F0E39D9, 0xBF54DB32}, {0x3F13682A, 0xBF514D3D}, {0x3F187FC0, 0xBF4D9F02}, {0x3F1D7FD1, 0xBF49D112}, {0x3F226799, 0xBF45E403}, {0x3F273655, 0xBF41D871}, {0x3F2BEB49, 0xBF3DAEFA}, {0x3F3085BA, 0xBF396842},
	{0x3F3504F3, 0xBF3504F3}, {0x3F396842, 0xBF3085BB}, {0x3F3DAEF9, 0xBF2BEB4A}, {0x3F41D870, 0xBF273656}, {0x3F45E403, 0xBF22679A}, {0x3F49D112, 0xBF1D7FD2}, {0x3F4D9F02, 0xBF187FC0}, {0x3F514D3D, 0xBF13682B},
	{0x3F54DB31, 0xBF0E39DA}, {0x3F584853, 0xBF08F59B}, {0x3F5B941A, 0xBF039C3D}, {0x3F5EBE05, 0xBEFC5D28}, {0x3F61C597, 0xBEF15AEA}, {0x3F64AA59, 0xBEE63375}, {0x3F676BD8, 0xBEDAE880}, {0x3F6A09A6, 0xBECF7BCB},
	{0x3F6C835E, 0xBEC3EF16}, {0x3F6ED89E, 0xBEB8442A}, {0x3F710908, 0xBEAC7CD4}, {0x3F731447, 0xBEA09AE5}, {0x3F74FA0B, 0xBE94A031}, {0x3F76BA07, 0xBE888E94}, {0x3F7853F8, 0xBE78CFCD}, {0x3F79C79D, 0xBE605C13},
	{0x3F7B14BE, 0xBE47C5C2}, {0x3F7C3B28, 0xBE2F10A3}, {0x3F7D3AAC, 0xBE164083}, {0x3F7E1324, 0xBDFAB273}, {0x3F7EC46D, 0xBDC8BD36}, {0x3F7F4E6D, 0xBD96A905}, {0x3F7FB10F, 0xBD48FB30}, {0x3F7FEC43, 0xBCC90AB0}
};


float C4::Sqrt(float x)
{
	#if C4SIMD && C4SSE

		float	result;

		vec_float v = _mm_load_ss(&x);
		vec_float mask = _mm_cmplt_ss(v, VecLoadScalarConstant<0x00800000>());

		vec_float r = _mm_rsqrt_ss(v);
		r = _mm_mul_ss(_mm_mul_ss(_mm_sub_ss(VecLoadScalarConstant<0x40400000>(), _mm_mul_ss(v, _mm_mul_ss(r, r))), r), VecLoadScalarConstant<0x3F000001>());

		_mm_store_ss(&result, _mm_mul_ss(_mm_andnot_ps(mask, r), v));
		return (result);

	#else

		if (x < K::min_float)
		{
			return (0.0F);
		}

		unsigned_int32 i = 0x5F375A86 - (*reinterpret_cast<unsigned_int32 *>(&x) >> 1);
		float r = *reinterpret_cast<float *>(&i);
		r = 0.5F * r * (3.0F - x * r * r);
		r = 0.5F * r * (3.0F - x * r * r);
		return (r * x);

	#endif
}

float C4::InverseSqrt(float x)
{
	#if C4SIMD && C4SSE

		float	result;

		vec_float v = _mm_load_ss(&x);
		vec_float mask = _mm_cmplt_ss(v, VecLoadScalarConstant<0x00800000>());

		vec_float r = _mm_rsqrt_ss(v);
		r = _mm_mul_ss(_mm_mul_ss(_mm_sub_ss(VecLoadScalarConstant<0x40400000>(), _mm_mul_ss(v, _mm_mul_ss(r, r))), r), VecLoadScalarConstant<0x3F000001>());

		_mm_store_ss(&result, _mm_or_ps(_mm_andnot_ps(mask, r), _mm_and_ps(mask, VecLoadScalarConstant<0x7F800000>())));
		return (result);

	#else 

		if (x < K::min_float)
		{ 
			return (K::infinity);
		} 

		unsigned_int32 i = 0x5F375A86 - (*reinterpret_cast<unsigned_int32 *>(&x) >> 1);
		float r = *reinterpret_cast<float *>(&i); 
		r = 0.5F * r * (3.0F - x * r * r);
		r = 0.5F * r * (3.0F - x * r * r); 
		return (r); 

	#endif
}
 
float C4::Sin(float x)
{
	#if C4SIMD

		float	result;

		vec_float b = VecMulScalar(VecAndc(VecLoadScalar(&x), VecFloatGetMinusZero()), VecLoadScalarConstant<0x4222F983>());
		vec_float i = VecPositiveFloorScalar(b);
		b = VecMulScalar(VecSubScalar(b, i), VecLoadScalarConstant<0x3CC90FDB>());

		const Vector2D& cossin = Math::GetTrigTable()[VecTruncateConvert(i) & 255];
		vec_float cosine_alpha = VecLoadScalar(&cossin.x);
		vec_float sine_alpha = VecLoadScalar(&cossin.y);

		vec_float b2 = VecMulScalar(b, b);
		vec_float sine_beta = VecNmsubScalar(VecMulScalar(b, b2), VecNmsubScalar(b2, VecLoadScalarConstant<0x3E2AAAAB>(), VecLoadScalarConstant<0x3C088889>()), b);
		vec_float cosine_beta = VecNmsubScalar(b2, VecNmsub(b2, VecLoadScalarConstant<0x3D2AAAAB>(), VecLoadScalarConstant<0x3F000000>()), VecLoadScalarConstant<0x3F800000>());

		vec_float sine = VecMaddScalar(sine_alpha, cosine_beta, VecMulScalar(cosine_alpha, sine_beta));

		VecStoreX((x < 0.0F) ? VecNegate(sine) : sine, &result);
		return (result);

	#else

		float b = Fabs(x) * 40.74366543F;	// 256 / 2pi
		float i = PositiveFloor(b);
		b = (b - i) * 0.0245436926F;		// 2pi / 256

		const Vector2D& cossin_alpha = Math::GetTrigTable()[(int32) i & 255];

		float b2 = b * b;
		float sine_beta = b - b * b2 * (0.1666666667F - b2 * 0.008333333333F);
		float cosine_beta = 1.0F - b2 * (0.5F - b2 * 0.04166666667F);

		float sine = cossin_alpha.y * cosine_beta + cossin_alpha.x * sine_beta;
		return ((x < 0.0F) ? -sine : sine);

	#endif
}

float C4::Cos(float x)
{
	#if C4SIMD

		float	result;

		vec_float b = VecMulScalar(VecAndc(VecLoadScalar(&x), VecFloatGetMinusZero()), VecLoadScalarConstant<0x4222F983>());
		vec_float i = VecPositiveFloorScalar(b);
		b = VecMulScalar(VecSubScalar(b, i), VecLoadScalarConstant<0x3CC90FDB>());

		const Vector2D& cossin = Math::GetTrigTable()[VecTruncateConvert(i) & 255];
		vec_float cosine_alpha = VecLoadScalar(&cossin.x);
		vec_float sine_alpha = VecLoadScalar(&cossin.y);

		vec_float b2 = VecMulScalar(b, b);
		vec_float sine_beta = VecNmsubScalar(VecMulScalar(b, b2), VecNmsubScalar(b2, VecLoadScalarConstant<0x3E2AAAAB>(), VecLoadScalarConstant<0x3C088889>()), b);
		vec_float cosine_beta = VecNmsubScalar(b2, VecNmsub(b2, VecLoadScalarConstant<0x3D2AAAAB>(), VecLoadScalarConstant<0x3F000000>()), VecLoadScalarConstant<0x3F800000>());

		VecStoreX(VecSubScalar(VecMulScalar(cosine_alpha, cosine_beta), VecMulScalar(sine_alpha, sine_beta)), &result);
		return (result);

	#else

		float b = Fabs(x) * 40.74366543F;	// 256 / 2pi
		float i = PositiveFloor(b);
		b = (b - i) * 0.0245436926F;		// 2pi / 256

		const Vector2D& cossin_alpha = Math::GetTrigTable()[(int32) i & 255];

		float b2 = b * b;
		float sine_beta = b - b * b2 * (0.1666666667F - b2 * 0.008333333333F);
		float cosine_beta = 1.0F - b2 * (0.5F - b2 * 0.04166666667F);

		return (cossin_alpha.x * cosine_beta - cossin_alpha.y * sine_beta);

	#endif
}

float C4::Tan(float x)
{
	#if C4SIMD

		float	cosine, sine;

		vec_float b = VecMulScalar(VecAndc(VecLoadScalar(&x), VecFloatGetMinusZero()), VecLoadScalarConstant<0x4222F983>());
		vec_float i = VecPositiveFloorScalar(b);
		b = VecMulScalar(VecSubScalar(b, i), VecLoadScalarConstant<0x3CC90FDB>());

		const Vector2D& cossin = Math::GetTrigTable()[VecTruncateConvert(i) & 255];
		vec_float cosine_alpha = VecLoadScalar(&cossin.x);
		vec_float sine_alpha = VecLoadScalar(&cossin.y);

		vec_float b2 = VecMulScalar(b, b);
		vec_float sine_beta = VecNmsubScalar(VecMulScalar(b, b2), VecNmsubScalar(b2, VecLoadScalarConstant<0x3E2AAAAB>(), VecLoadScalarConstant<0x3C088889>()), b);
		vec_float cosine_beta = VecNmsubScalar(b2, VecNmsub(b2, VecLoadScalarConstant<0x3D2AAAAB>(), VecLoadScalarConstant<0x3F000000>()), VecLoadScalarConstant<0x3F800000>());

		VecStoreX(VecMaddScalar(sine_alpha, cosine_beta, VecMulScalar(cosine_alpha, sine_beta)), &sine);
		VecStoreX(VecSubScalar(VecMulScalar(cosine_alpha, cosine_beta), VecMulScalar(sine_alpha, sine_beta)), &cosine);

		float result = sine / cosine;
		return ((x < 0.0F) ? -result : result);

	#else

		float b = Fabs(x) * 40.74366543F;	// 256 / 2pi
		float i = PositiveFloor(b);
		b = (b - i) * 0.0245436926F;		// 2pi / 256

		const Vector2D& cossin_alpha = Math::GetTrigTable()[(int32) i & 255];

		float b2 = b * b;
		float sine_beta = b - b * b2 * (0.1666666667F - b2 * 0.008333333333F);
		float cosine_beta = 1.0F - b2 * (0.5F - b2 * 0.04166666667F);

		float sine = cossin_alpha.y * cosine_beta + cossin_alpha.x * sine_beta;
		float cosine = cossin_alpha.x * cosine_beta - cossin_alpha.y * sine_beta;

		float result = sine / cosine;
		return ((x < 0.0F) ? -result : result);

	#endif
}

void C4::CosSin(float x, float *c, float *s)
{
	#if C4SIMD

		vec_float b = VecMulScalar(VecAndc(VecLoadScalar(&x), VecFloatGetMinusZero()), VecLoadScalarConstant<0x4222F983>());
		vec_float i = VecPositiveFloorScalar(b);
		b = VecMulScalar(VecSubScalar(b, i), VecLoadScalarConstant<0x3CC90FDB>());

		const Vector2D& cossin = Math::GetTrigTable()[VecTruncateConvert(i) & 255];
		vec_float cosine_alpha = VecLoadScalar(&cossin.x);
		vec_float sine_alpha = VecLoadScalar(&cossin.y);

		vec_float b2 = VecMulScalar(b, b);
		vec_float sine_beta = VecNmsubScalar(VecMulScalar(b, b2), VecNmsubScalar(b2, VecLoadScalarConstant<0x3E2AAAAB>(), VecLoadScalarConstant<0x3C088889>()), b);
		vec_float cosine_beta = VecNmsubScalar(b2, VecNmsub(b2, VecLoadScalarConstant<0x3D2AAAAB>(), VecLoadScalarConstant<0x3F000000>()), VecLoadScalarConstant<0x3F800000>());

		vec_float sine = VecMaddScalar(sine_alpha, cosine_beta, VecMulScalar(cosine_alpha, sine_beta));
		vec_float cosine = VecSubScalar(VecMulScalar(cosine_alpha, cosine_beta), VecMulScalar(sine_alpha, sine_beta));

		VecStoreX(cosine, c);
		VecStoreX((x < 0.0F) ? VecNegate(sine) : sine, s);

	#else

		float b = Fabs(x) * 40.74366543F;	// 256 / 2pi
		float i = PositiveFloor(b);
		b = (b - i) * 0.0245436926F;		// 2pi / 256

		const Vector2D& cossin_alpha = Math::GetTrigTable()[(int32) i & 255];

		float b2 = b * b;
		float sine_beta = b - b * b2 * (0.1666666667F - b2 * 0.008333333333F);
		float cosine_beta = 1.0F - b2 * (0.5F - b2 * 0.04166666667F);

		*c = cossin_alpha.x * cosine_beta - cossin_alpha.y * sine_beta;
		float sine = cossin_alpha.y * cosine_beta + cossin_alpha.x * sine_beta;
		*s = (x < 0.0F) ? -sine : sine;

	#endif
}

Vector2D C4::CosSin(float x)
{
	#if C4SIMD

		Vector2D	result;

		vec_float b = VecMulScalar(VecAndc(VecLoadScalar(&x), VecFloatGetMinusZero()), VecLoadScalarConstant<0x4222F983>());
		vec_float i = VecPositiveFloorScalar(b);
		b = VecMulScalar(VecSubScalar(b, i), VecLoadScalarConstant<0x3CC90FDB>());

		const Vector2D& cossin = Math::GetTrigTable()[VecTruncateConvert(i) & 255];
		vec_float cosine_alpha = VecLoadScalar(&cossin.x);
		vec_float sine_alpha = VecLoadScalar(&cossin.y);

		vec_float b2 = VecMulScalar(b, b);
		vec_float sine_beta = VecNmsubScalar(VecMulScalar(b, b2), VecNmsubScalar(b2, VecLoadScalarConstant<0x3E2AAAAB>(), VecLoadScalarConstant<0x3C088889>()), b);
		vec_float cosine_beta = VecNmsubScalar(b2, VecNmsub(b2, VecLoadScalarConstant<0x3D2AAAAB>(), VecLoadScalarConstant<0x3F000000>()), VecLoadScalarConstant<0x3F800000>());

		vec_float sine = VecMaddScalar(sine_alpha, cosine_beta, VecMulScalar(cosine_alpha, sine_beta));
		vec_float cosine = VecSubScalar(VecMulScalar(cosine_alpha, cosine_beta), VecMulScalar(sine_alpha, sine_beta));

		VecStoreX(cosine, &result.x);
		VecStoreX((x < 0.0F) ? VecNegate(sine) : sine, &result.y);
		return (result);

	#else

		float b = Fabs(x) * 40.74366543F;	// 256 / 2pi
		float i = PositiveFloor(b);
		b = (b - i) * 0.0245436926F;		// 2pi / 256

		const Vector2D& cossin_alpha = Math::GetTrigTable()[(int32) i & 255];

		float b2 = b * b;
		float sine_beta = b - b * b2 * (0.1666666667F - b2 * 0.008333333333F);
		float cosine_beta = 1.0F - b2 * (0.5F - b2 * 0.04166666667F);

		float sine = cossin_alpha.y * cosine_beta + cossin_alpha.x * sine_beta;
		float cosine = cossin_alpha.x * cosine_beta - cossin_alpha.y * sine_beta;
		return (Vector2D(cosine, (x < 0.0F) ? -sine : sine));

	#endif
}


Vector3D Math::CreatePerpendicular(const Vector3D& v)
{
	float x = v.x;
	float y = v.y;
	float z = v.z;

	float ax = Fabs(x);
	float ay = Fabs(y);
	float az = Fabs(z);

	if ((az > ax) && (az > ay))
	{
		return (Vector3D(0.0F, z, -y));
	}

	if (ay > ax)
	{
		return (Vector3D(y, -x, 0.0F));
	}

	return (Vector3D(-z, 0.0F, x));
}

Vector3D Math::CreateUnitPerpendicular(const Vector3D& v)
{
	float x = v.x;
	float y = v.y;
	float z = v.z;

	float ax = Fabs(x);
	float ay = Fabs(y);
	float az = Fabs(z);

	if ((az > ax) && (az > ay))
	{
		float r = InverseSqrt(y * y + z * z);
		return (Vector3D(0.0F, z * r, -y * r));
	}

	if (ay > ax)
	{
		float r = InverseSqrt(x * x + y * y);
		return (Vector3D(y * r, -x * r, 0.0F));
	}

	float r = InverseSqrt(x * x + z * z);
	return (Vector3D(-z * r, 0.0F, x * r));
}

unsigned_int32 Math::Hash(int32 count, const float *value)
{
	unsigned_int32 hash = 0;
	for (machine a = 0; a < count; a++)
	{
		unsigned_int32 i = *reinterpret_cast<const unsigned_int32 *>(value);

		hash ^= i;
		hash = hash * 0x6B84DF47 + 1;

		value++;
	}

	return (hash);
}

// ZYUQURM
