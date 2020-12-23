#pragma once

#include "Math3D.h"

namespace Leadwerks
{
	class Vec3;
	class Mat4;
	
	class FastMath
	{
	public:
	
		/*void mmul_sse(const float * a, const float * b, float * r)
		{
		  __m128 a_line, b_line, r_line;
		  for (int i=0; i<16; i+=4) {
			// unroll the first step of the loop to avoid having to initialize r_line to zero
			a_line = _mm_load_ps(a);         // a_line = vec4(column(a, 0))
			b_line = _mm_set1_ps(b[i]);      // b_line = vec4(b[i][0])
			r_line = _mm_mul_ps(a_line, b_line); // r_line = a_line * b_line
			for (int j=1; j<4; j++) {
			  a_line = _mm_load_ps(&a[j*4]); // a_line = vec4(column(a, j))
			  b_line = _mm_set1_ps(b[i+j]);  // b_line = vec4(b[i][j])
											 // r_line += a_line * b_line
			  r_line = _mm_add_ps(_mm_mul_ps(a_line, b_line), r_line);
			}
			_mm_store_ps(&r[i], r_line);     // r[i] = r_line
		  }
		}*/

		static void Mat4MultiplyMat4(const Mat4& mat0, const Mat4& mat1, Mat4& result)
		{
			result.i.x = mat0.i.x*mat1.i.x + mat0.j.x*mat1.i.y + mat0.k.x*mat1.i.z + mat0.t.x*mat1.i.w;
			result.i.y = mat0.i.y*mat1.i.x + mat0.j.y*mat1.i.y + mat0.k.y*mat1.i.z + mat0.t.y*mat1.i.w;
			result.i.z = mat0.i.z*mat1.i.x + mat0.j.z*mat1.i.y + mat0.k.z*mat1.i.z + mat0.t.z*mat1.i.w;
			result.i.w = mat0.i.w*mat1.i.x + mat0.j.w*mat1.i.y + mat0.k.w*mat1.i.z + mat0.t.w*mat1.i.w;
			result.j.x = mat0.i.x*mat1.j.x + mat0.j.x*mat1.j.y + mat0.k.x*mat1.j.z + mat0.t.x*mat1.j.w;
			result.j.y = mat0.i.y*mat1.j.x + mat0.j.y*mat1.j.y + mat0.k.y*mat1.j.z + mat0.t.y*mat1.j.w;
			result.j.z = mat0.i.z*mat1.j.x + mat0.j.z*mat1.j.y + mat0.k.z*mat1.j.z + mat0.t.z*mat1.j.w;
			result.j.w = mat0.i.w*mat1.j.x + mat0.j.w*mat1.j.y + mat0.k.w*mat1.j.z + mat0.t.w*mat1.j.w;
			result.k.x = mat0.i.x*mat1.k.x + mat0.j.x*mat1.k.y + mat0.k.x*mat1.k.z + mat0.t.x*mat1.k.w;
			result.k.y = mat0.i.y*mat1.k.x + mat0.j.y*mat1.k.y + mat0.k.y*mat1.k.z + mat0.t.y*mat1.k.w;
			result.k.z = mat0.i.z*mat1.k.x + mat0.j.z*mat1.k.y + mat0.k.z*mat1.k.z + mat0.t.z*mat1.k.w;
			result.k.w = mat0.i.w*mat1.k.x + mat0.j.w*mat1.k.y + mat0.k.w*mat1.k.z + mat0.t.w*mat1.k.w;
			result.t.x = mat0.i.x*mat1.t.x + mat0.j.x*mat1.t.y + mat0.k.x*mat1.t.z + mat0.t.x*mat1.t.w;
			result.t.y = mat0.i.y*mat1.t.x + mat0.j.y*mat1.t.y + mat0.k.y*mat1.t.z + mat0.t.y*mat1.t.w;
			result.t.z = mat0.i.z*mat1.t.x + mat0.j.z*mat1.t.y + mat0.k.z*mat1.t.z + mat0.t.z*mat1.t.w;
			result.t.w = mat0.i.w*mat1.t.x + mat0.j.w*mat1.t.y + mat0.k.w*mat1.t.z + mat0.t.w*mat1.t.w;
		}
		
		static void Mat4MultiplyVec4(const Mat4& mat0, const Vec4& v1, Vec4& result)
		{
			result.x = mat0.i.x*v1.x + mat0.j.x*v1.y + mat0.k.x*v1.z + mat0.t.x*v1.w;
			result.y = mat0.i.y*v1.x + mat0.j.y*v1.y + mat0.k.y*v1.z + mat0.t.y*v1.w;
			result.z = mat0.i.z*v1.x + mat0.j.z*v1.y + mat0.k.z*v1.z + mat0.t.z*v1.w;
			result.w = mat0.i.w*v1.x + mat0.j.w*v1.y + mat0.k.w*v1.z + mat0.t.w*v1.w;
		}

		static void Mat4MultiplyMat4Ortho(const Mat4& mat0, const Mat4& mat1, Mat4& result)
		{
			result.i.x = mat0.i.x*mat1.i.x + mat0.j.x*mat1.i.y + mat0.k.x*mat1.i.z;
			result.i.y = mat0.i.y*mat1.i.x + mat0.j.y*mat1.i.y + mat0.k.y*mat1.i.z;
			result.i.z = mat0.i.z*mat1.i.x + mat0.j.z*mat1.i.y + mat0.k.z*mat1.i.z;
			result.j.x = mat0.i.x*mat1.j.x + mat0.j.x*mat1.j.y + mat0.k.x*mat1.j.z;
			result.j.y = mat0.i.y*mat1.j.x + mat0.j.y*mat1.j.y + mat0.k.y*mat1.j.z;
			result.j.z = mat0.i.z*mat1.j.x + mat0.j.z*mat1.j.y + mat0.k.z*mat1.j.z;
			result.k.x = mat0.i.x*mat1.k.x + mat0.j.x*mat1.k.y + mat0.k.x*mat1.k.z;
			result.k.y = mat0.i.y*mat1.k.x + mat0.j.y*mat1.k.y + mat0.k.y*mat1.k.z;
			result.k.z = mat0.i.z*mat1.k.x + mat0.j.z*mat1.k.y + mat0.k.z*mat1.k.z;
			result.t.x = mat0.i.x*mat1.t.x + mat0.j.x*mat1.t.y + mat0.k.x*mat1.t.z + mat0.t.x;
			result.t.y = mat0.i.y*mat1.t.x + mat0.j.y*mat1.t.y + mat0.k.y*mat1.t.z + mat0.t.y;
			result.t.z = mat0.i.z*mat1.t.x + mat0.j.z*mat1.t.y + mat0.k.z*mat1.t.z + mat0.t.z;
		}
		
		static void Mat3MultiplyMat3(const Mat3& mat0, const Mat3& mat1, Mat3& result)
		{
			result.i.x = mat0.i.x*mat1.i.x + mat0.j.x*mat1.i.y + mat0.k.x*mat1.i.z;
			result.i.y = mat0.i.y*mat1.i.x + mat0.j.y*mat1.i.y + mat0.k.y*mat1.i.z;
			result.i.z = mat0.i.z*mat1.i.x + mat0.j.z*mat1.i.y + mat0.k.z*mat1.i.z;
			result.j.x = mat0.i.x*mat1.j.x + mat0.j.x*mat1.j.y + mat0.k.x*mat1.j.z;
			result.j.y = mat0.i.y*mat1.j.x + mat0.j.y*mat1.j.y + mat0.k.y*mat1.j.z;
			result.j.z = mat0.i.z*mat1.j.x + mat0.j.z*mat1.j.y + mat0.k.z*mat1.j.z;
			result.k.x = mat0.i.x*mat1.k.x + mat0.j.x*mat1.k.y + mat0.k.x*mat1.k.z;
			result.k.y = mat0.i.y*mat1.k.x + mat0.j.y*mat1.k.y + mat0.k.y*mat1.k.z;
			result.k.z = mat0.i.z*mat1.k.x + mat0.j.z*mat1.k.y + mat0.k.z*mat1.k.z;
		}
		
		static void Mat4MultiplyMat4Ortho(const Mat4& mat0, const Mat4& mat1, float* result)
		{
			result[0] = mat0.i.x*mat1.i.x + mat0.j.x*mat1.i.y + mat0.k.x*mat1.i.z;
			result[1] = mat0.i.y*mat1.i.x + mat0.j.y*mat1.i.y + mat0.k.y*mat1.i.z;
			result[2] = mat0.i.z*mat1.i.x + mat0.j.z*mat1.i.y + mat0.k.z*mat1.i.z;
			result[4] = mat0.i.x*mat1.j.x + mat0.j.x*mat1.j.y + mat0.k.x*mat1.j.z;
			result[5] = mat0.i.y*mat1.j.x + mat0.j.y*mat1.j.y + mat0.k.y*mat1.j.z;
			result[6] = mat0.i.z*mat1.j.x + mat0.j.z*mat1.j.y + mat0.k.z*mat1.j.z;
			result[8] = mat0.i.x*mat1.k.x + mat0.j.x*mat1.k.y + mat0.k.x*mat1.k.z;
			result[9] = mat0.i.y*mat1.k.x + mat0.j.y*mat1.k.y + mat0.k.y*mat1.k.z;
			result[10] = mat0.i.z*mat1.k.x + mat0.j.z*mat1.k.y + mat0.k.z*mat1.k.z;
			result[12] = mat0.i.x*mat1.t.x + mat0.j.x*mat1.t.y + mat0.k.x*mat1.t.z + mat0.t.x;
			result[13] = mat0.i.y*mat1.t.x + mat0.j.y*mat1.t.y + mat0.k.y*mat1.t.z + mat0.t.y;
			result[14] = mat0.i.z*mat1.t.x + mat0.j.z*mat1.t.y + mat0.k.z*mat1.t.z + mat0.t.z;
		}
		
		static void Mat4MultiplyVec3(const Mat4& mat, const Vec3& v, Vec3& result)
		{
			result.x = mat.i.x*v.x + mat.j.x*v.y + mat.k.x*v.z + mat.t.x;
			result.y = mat.i.y*v.x + mat.j.y*v.y + mat.k.y*v.z + mat.t.y;
			result.z = mat.i.z*v.x + mat.j.z*v.y + mat.k.z*v.z + mat.t.z;
		}
		
		static void Mat3MultiplyVec3(const Mat3& mat, const Vec3& v, Vec3& result)
		{
			result.x = mat.i.x*v.x + mat.j.x*v.y + mat.k.x*v.z;
			result.y = mat.i.y*v.x + mat.j.y*v.y + mat.k.y*v.z;
			result.z = mat.i.z*v.x + mat.j.z*v.y + mat.k.z*v.z;
		}
		
		static void Mat3MultiplyVec3(const Mat4& mat, const Vec3& v, Vec3& result)
		{
			result.x = mat.i.x*v.x + mat.j.x*v.y + mat.k.x*v.z;
			result.y = mat.i.y*v.x + mat.j.y*v.y + mat.k.y*v.z;
			result.z = mat.i.z*v.x + mat.j.z*v.y + mat.k.z*v.z;
		}

		static void Mat4MultiplyNormal(const float* mat, const float* v, float* result)
		{
			result[0] = mat[0]*v[0] + mat[4]*v[1] + mat[8]*v[2];// + mat[12];
			result[1] = mat[1]*v[0] + mat[5]*v[1] + mat[9]*v[2];// + mat[13];
			result[2] = mat[2]*v[0] + mat[6]*v[1] + mat[10]*v[2];// + mat[14];
		}

		static void Mat4MultiplyVec3(const float* mat, const float* v, float* result)
		{
			result[0] = mat[0]*v[0] + mat[4]*v[1] + mat[8]*v[2] + mat[12];
			result[1] = mat[1]*v[0] + mat[5]*v[1] + mat[9]*v[2] + mat[13];
			result[2] = mat[2]*v[0] + mat[6]*v[1] + mat[10]*v[2] + mat[14];
		}
	};
}
