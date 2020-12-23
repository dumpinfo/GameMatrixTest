 

#include "C4Regions.h"
#include "C4Cameras.h"
#include "C4Zones.h"


using namespace C4;


namespace C4
{
	template <> Heap EngineMemory<ShadowRegion>::heap("ShadowRegion", MemoryMgr::CalculatePoolSize(64, sizeof(ShadowRegion)), kHeapMutexless);
	template class EngineMemory<ShadowRegion>;

	template <> Heap EngineMemory<OcclusionRegion>::heap("OcclusionRegion", MemoryMgr::CalculatePoolSize(64, sizeof(OcclusionRegion)), kHeapMutexless);
	template class EngineMemory<OcclusionRegion>;

	template <> Heap EngineMemory<CameraRegion>::heap("CameraRegion", MemoryMgr::CalculatePoolSize(64, sizeof(CameraRegion)), kHeapMutexless);
	template class EngineMemory<CameraRegion>;

	template <> Heap EngineMemory<LightRegion>::heap("LightRegion", MemoryMgr::CalculatePoolSize(64, sizeof(LightRegion)), kHeapMutexless);
	template class EngineMemory<LightRegion>;
}


bool VisibilityRegion::BoxVisible(const Box3D& box) const
{
	#if C4SIMD

		const vec_float sign_bit = VecFloatGetMinusZero();
		vec_float min = VecLoadUnaligned(&box.min.x);
		vec_float max = VecLoadUnaligned(&box.max.x);

		const vec_float half = VecLoadVectorConstant<0x3F000000>();
		vec_float center = VecMul(VecAdd(min, max), half);
		vec_float size = VecMul(VecSub(max, min), half);

		int32 planeCount = GetPlaneCount();
		const Antivector4D *planeArray = GetPlaneArray();

		for (machine a = 0; a < planeCount; a++)
		{
			vec_float plane = VecLoadUnaligned(&planeArray[a].x);
			vec_float reff = VecHorizontalSum3D(VecOr(VecMul(plane, size), sign_bit));
			if (VecCmpltScalar(VecPlaneWedgePoint3D(plane, center), reff))
			{
				return (false);
			}
		}

	#else

		Point3D center = box.GetCenter();
		Vector3D size = box.GetSize() * 0.5F;

		int32 planeCount = GetPlaneCount();
		const Antivector4D *planeArray = GetPlaneArray();

		for (machine a = 0; a < planeCount; a++)
		{
			const Antivector4D& plane = planeArray[a];
			float reff = Fnabs(plane.x * size.x) + Fnabs(plane.y * size.y) + Fnabs(plane.z * size.z);
			if ((plane ^ center) < reff)
			{
				return (false);
			}
		}

	#endif

	return (true);
}

bool VisibilityRegion::BoxVisible(const Point3D& center, const Vector3D *axis) const
{
	#if C4SIMD

		const vec_float sign_bit = VecFloatGetMinusZero();
		vec_float c = VecLoadUnaligned(&center.x);

		int32 planeCount = GetPlaneCount();
		const Antivector4D *planeArray = GetPlaneArray();

		for (machine a = 0; a < planeCount; a++)
		{
			vec_float plane = VecLoadUnaligned(&planeArray[a].x);

			vec_float a1 = VecDot3D(plane, VecLoadUnaligned(&axis[0].x));
			vec_float a2 = VecDot3D(plane, VecLoadUnaligned(&axis[1].x));
			vec_float a3 = VecDot3D(plane, VecLoadUnaligned(&axis[2].x));

			a1 = VecOr(sign_bit, a1);
			a2 = VecOr(sign_bit, a2);
			a3 = VecOr(sign_bit, a3);

			a1 = VecAddScalar(a1, a2);
			a1 = VecAddScalar(a1, a3);

			if (VecCmpltScalar(VecPlaneWedgePoint3D(plane, c), a1))
			{
				return (false);
			} 
		}

	#else 

		int32 planeCount = GetPlaneCount(); 
		const Antivector4D *planeArray = GetPlaneArray();

		for (machine a = 0; a < planeCount; a++) 
		{
			const Antivector4D& plane = planeArray[a]; 
			float reff = Fnabs(plane ^ axis[0]) + Fnabs(plane ^ axis[1]) + Fnabs(plane ^ axis[2]); 
			if ((plane ^ center) < reff)
			{
				return (false);
			} 
		}

	#endif

	return (true);
}

bool VisibilityRegion::BoxVisible(const Point3D& center, const Vector3D& size) const
{
	#if C4SIMD

		const vec_float sign_bit = VecFloatGetMinusZero();
		vec_float c = VecLoadUnaligned(&center.x);
		vec_float s = VecLoadUnaligned(&size.x);

		int32 planeCount = GetPlaneCount();
		const Antivector4D *planeArray = GetPlaneArray();

		for (machine a = 0; a < planeCount; a++)
		{
			vec_float plane = VecLoadUnaligned(&planeArray[a].x);
			vec_float reff = VecHorizontalSum3D(VecOr(VecMul(plane, s), sign_bit));
			if (VecCmpltScalar(VecPlaneWedgePoint3D(plane, c), reff))
			{
				return (false);
			}
		}

	#else

		int32 planeCount = GetPlaneCount();
		const Antivector4D *planeArray = GetPlaneArray();

		for (machine a = 0; a < planeCount; a++)
		{
			const Antivector4D& plane = planeArray[a];
			float reff = Fnabs(plane.x * size.x) + Fnabs(plane.y * size.y) + Fnabs(plane.z * size.z);
			if ((plane ^ center) < reff)
			{
				return (false);
			}
		}

	#endif

	return (true);
}

bool VisibilityRegion::DirectionVisible(const Vector3D& direction, float radius) const
{
	radius = -radius;

	int32 planeCount = GetPlaneCount() - auxiliaryPlaneCount;
	const Antivector4D *planeArray = GetPlaneArray();

	for (machine a = 0; a < planeCount; a++)
	{
		if ((planeArray[a] ^ direction) < radius)
		{
			return (false);
		}
	}

	return (true);
}

bool VisibilityRegion::QuadVisible(const Point3D *vertex) const
{
	#if C4SIMD

		const vec_float zero = VecFloatGetZero();

		int32 planeCount = GetPlaneCount();
		const Antivector4D *planeArray = GetPlaneArray();

		for (machine a = 0; a < planeCount; a++)
		{
			vec_float plane = VecLoadUnaligned(&planeArray[a].x);
			for (machine b = 0; b < 4; b++)
			{
				if (VecCmpgtScalar(VecPlaneWedgePoint3D(plane, VecLoadUnaligned(&vertex[b].x)), zero))
				{
					goto next;
				}
			}

			return (false);
			next:;
		}

	#else

		int32 planeCount = GetPlaneCount();
		const Antivector4D *planeArray = GetPlaneArray();

		for (machine a = 0; a < planeCount; a++)
		{
			const Antivector4D& plane = planeArray[a];
			for (machine b = 0; b < 4; b++)
			{
				if ((plane ^ vertex[b]) > 0.0F)
				{
					goto next;
				}
			}

			return (false);
			next:;
		}

	#endif

	return (true);
}

bool VisibilityRegion::PolygonVisible(int32 vertexCount, const Point3D *vertex) const
{
	#if C4SIMD

		const vec_float zero = VecFloatGetZero();

		int32 planeCount = GetPlaneCount();
		const Antivector4D *planeArray = GetPlaneArray();

		for (machine a = 0; a < planeCount; a++)
		{
			vec_float plane = VecLoadUnaligned(&planeArray[a].x);
			for (machine b = 0; b < vertexCount; b++)
			{
				if (VecCmpgtScalar(VecPlaneWedgePoint3D(plane, VecLoadUnaligned(&vertex[b].x)), zero))
				{
					goto next;
				}
			}

			return (false);
			next:;
		}

	#else

		int32 planeCount = GetPlaneCount();
		const Antivector4D *planeArray = GetPlaneArray();

		for (machine a = 0; a < planeCount; a++)
		{
			const Antivector4D& plane = planeArray[a];
			for (machine b = 0; b < vertexCount; b++)
			{
				if ((plane ^ vertex[b]) > 0.0F)
				{
					goto next;
				}
			}

			return (false);
			next:;
		}

	#endif

	return (true);
}

bool VisibilityRegion::PyramidVisible(const Point3D& apex, int32 vertexCount, const Point3D *vertex) const
{
	#if C4SIMD

		const vec_float zero = VecFloatGetZero();

		int32 planeCount = GetPlaneCount();
		const Antivector4D *planeArray = GetPlaneArray();

		for (machine a = 0; a < planeCount; a++)
		{
			vec_float plane = VecLoadUnaligned(&planeArray[a].x);
			vec_float d = VecPlaneWedgePoint3D(plane, VecLoadUnaligned(&apex.x));
			if (VecCmpltScalar(d, zero))
			{
				for (machine b = 0; b < vertexCount; b++)
				{
					if (VecCmpgtScalar(VecPlaneWedgePoint3D(plane, VecLoadUnaligned(&vertex[b].x)), zero))
					{
						goto next;
					}
				}

				return (false);
			}

			next:;
		}

	#else

		int32 planeCount = GetPlaneCount();
		const Antivector4D *planeArray = GetPlaneArray();

		for (machine a = 0; a < planeCount; a++)
		{
			const Antivector4D& plane = planeArray[a];
			if ((plane ^ apex) < 0.0F)
			{
				for (machine b = 0; b < vertexCount; b++)
				{
					if ((plane ^ vertex[b]) > 0.0F)
					{
						goto next;
					}
				}

				return (false);
			}

			next:;
		}

	#endif

	return (true);
}

bool VisibilityRegion::SphereVisible(const Point3D& center, float radius) const
{
	#if C4SIMD

		vec_float r = VecXor(VecLoadScalar(&radius), VecFloatGetMinusZero());
		vec_float c = VecLoadUnaligned(&center.x);

		int32 planeCount = GetPlaneCount();
		const Antivector4D *planeArray = GetPlaneArray();

		for (machine a = 0; a < planeCount; a++)
		{
			vec_float plane = VecLoadUnaligned(&planeArray[a].x);
			if (VecCmpltScalar(VecPlaneWedgePoint3D(plane, c), r))
			{
				return (false);
			}
		}

	#else

		radius = -radius;

		int32 planeCount = GetPlaneCount();
		const Antivector4D *planeArray = GetPlaneArray();

		for (machine a = 0; a < planeCount; a++)
		{
			if ((planeArray[a] ^ center) < radius)
			{
				return (false);
			}
		}

	#endif

	return (true);
}

bool VisibilityRegion::EllipsoidVisible(const Point3D& center, const Vector3D *axis) const
{
	#if C4SIMD

		vec_float c = VecLoadUnaligned(&center.x);

		int32 planeCount = GetPlaneCount();
		const Antivector4D *planeArray = GetPlaneArray();

		for (machine a = 0; a < planeCount; a++)
		{
			vec_float plane = VecLoadUnaligned(&planeArray[a].x);
			vec_float R_dot_N = VecDot3D(plane, VecLoadUnaligned(&axis[0].x));
			vec_float S_dot_N = VecDot3D(plane, VecLoadUnaligned(&axis[1].x));
			vec_float T_dot_N = VecDot3D(plane, VecLoadUnaligned(&axis[2].x));
			vec_float d2 = VecMulScalar(R_dot_N, R_dot_N);
			d2 = VecMaddScalar(S_dot_N, S_dot_N, d2);
			d2 = VecMaddScalar(T_dot_N, T_dot_N, d2);

			vec_float reff = VecNegate(VecSqrtScalar(d2));
			if (VecCmpltScalar(VecPlaneWedgePoint3D(plane, c), reff))
			{
				return (false);
			}
		}

	#else

		int32 planeCount = GetPlaneCount();
		const Antivector4D *planeArray = GetPlaneArray();

		for (machine a = 0; a < planeCount; a++)
		{
			const Antivector4D& plane = planeArray[a];
			float R_dot_N = plane ^ axis[0];
			float S_dot_N = plane ^ axis[1];
			float T_dot_N = plane ^ axis[2];
			float reff = -Sqrt(R_dot_N * R_dot_N + S_dot_N * S_dot_N + T_dot_N * T_dot_N);
			if ((plane ^ center) < reff)
			{
				return (false);
			}
		}

	#endif

	return (true);
}

bool VisibilityRegion::CylinderVisible(const Point3D& p1, const Point3D& p2, float radius) const
{
	#if C4SIMD

		vec_float q1 = VecLoadUnaligned(&p1.x);
		vec_float q2 = VecLoadUnaligned(&p2.x);

		vec_float dp = VecSub(q2, q1);
		vec_float m = VecInverseSqrtScalar(VecDot3D(dp, dp));
		dp = VecMul(dp, VecSmearX(m));

		const vec_float zero = VecFloatGetZero();
		const vec_float one = VecLoadVectorConstant<0x3F800000>();

		vec_float r = VecLoadScalar(&radius);

		int32 planeCount = GetPlaneCount();
		const Antivector4D *planeArray = GetPlaneArray();

		for (machine a = 0; a < planeCount; a++)
		{
			vec_float plane = VecLoadUnaligned(&planeArray[a].x);

			vec_float d1 = VecDot3D(plane, q1);
			vec_float d2 = VecDot3D(plane, q2);

			vec_float s = VecDot3D(plane, dp);
			vec_float reff = VecMulScalar(r, VecSqrtScalar(VecMaxScalar(VecNmsubScalar(s, s, one), zero)));

			vec_float f = VecSubScalar(VecNegate(VecSmearW(plane)), reff);
			if (VecCmpltScalar(d1, f))
			{
				if (VecCmpltScalar(d2, f))
				{
					return (false);
				}

				vec_float t = VecDivScalar(VecSubScalar(f, d1), VecSubScalar(d2, d1));
				q1 = VecMadd(VecSmearX(t), VecSub(q2, q1), q1);
			}
			else if (VecCmpltScalar(d2, f))
			{
				vec_float t = VecDivScalar(VecSubScalar(f, d1), VecSubScalar(d2, d1));
				q2 = VecMadd(VecSmearX(t), VecSub(q2, q1), q1);
			}
		}

	#else

		Point3D q1 = p1;
		Point3D q2 = p2;

		Vector3D dp = q2 - q1;
		dp.Normalize();

		int32 planeCount = GetPlaneCount();
		const Antivector4D *planeArray = GetPlaneArray();

		for (machine a = 0; a < planeCount; a++)
		{
			const Antivector4D& plane = planeArray[a];

			float d1 = plane ^ q1.GetVector3D();
			float d2 = plane ^ q2.GetVector3D();

			float s = plane ^ dp;
			float reff = radius * Sqrt(FmaxZero(1.0F - s * s));

			float f = -plane.w - reff;
			if (d1 < f)
			{
				if (d2 < f)
				{
					return (false);
				}

				float t = (f - d1) / (d2 - d1);
				q1 += t * (q2 - q1);
			}
			else if (d2 < f)
			{
				float t = (f - d1) / (d2 - d1);
				q2 = q1 + t * (q2 - q1);
			}
		}

	#endif

	return (true);
}


OcclusionRegion::OcclusionRegion(const FrustumCamera *camera, int32 vertexCount, const Point3D *vertex, int32 frontPlaneCount, const Antivector4D *frontPlane, unsigned_int32 mask)
{
	float	distance[2][4];

	occlusionMask = mask;

	const float kCullEpsilon = kBoundaryEpsilon * 2.0F;

	const Point3D& cameraPosition = camera->GetWorldPosition();
	const Vector3D *frustumNormal = &camera->GetFrustumPlaneNormal(0);

	Vector3D v1 = vertex[vertexCount - 1] - cameraPosition;
	for (machine k = 0; k < 4; k++)
	{
		distance[0][k] = frustumNormal[k] * v1;
	}

	int32 count = 0;
	Antivector4D *plane = planeArray;

	unsigned_int32 p1 = 0;
	for (machine a = 0; a < vertexCount; a++)
	{
		Vector3D v2 = vertex[a] - cameraPosition;
		Antivector3D occluderNormal = Normalize(v2 ^ v1);

		bool cull = false;
		unsigned_int32 p2 = p1 ^ 1;
		for (machine k = 0; k < 4; k++)
		{
			const Vector3D& n = frustumNormal[k];
			float d = n * v2;
			distance[p2][k] = d;

			if (Fmax(d, distance[p1][k]) < kCullEpsilon)
			{
				cull |= (occluderNormal * n > 0.0F);
			}
		}

		if (!cull)
		{
			plane->Set(occluderNormal, cameraPosition);
			count++;
			plane++;
		}

		v1 = v2;
		p1 ^= 1;
	}

	for (machine a = 0; a < frontPlaneCount; a++)
	{
		plane[a] = frontPlane[a];
	}

	planeCount = count + frontPlaneCount;
}

bool OcclusionRegion::BoxOccluded(const Box3D& box) const
{
	#if C4SIMD

		const vec_float sign_bit = VecFloatGetMinusZero();
		vec_float min = VecLoadUnaligned(&box.min.x);
		vec_float max = VecLoadUnaligned(&box.max.x);

		const vec_float half = VecLoadVectorConstant<0x3F000000>();
		vec_float center = VecMul(VecAdd(min, max), half);
		vec_float size = VecMul(VecSub(max, min), half);

		int32 count = planeCount;
		for (machine a = 0; a < count; a++)
		{
			vec_float plane = VecLoadUnaligned(&planeArray[a].x);
			vec_float reff = VecHorizontalSum3D(VecAndc(VecMul(plane, size), sign_bit));
			if (VecCmpltScalar(VecPlaneWedgePoint3D(plane, center), reff))
			{
				return (false);
			}
		}

	#else

		Point3D center = box.GetCenter();
		Vector3D size = box.GetSize() * 0.5F;

		int32 count = planeCount;
		for (machine a = 0; a < count; a++)
		{
			const Antivector4D& plane = planeArray[a];
			float reff = Fabs(plane.x * size.x) + Fabs(plane.y * size.y) + Fabs(plane.z * size.z);
			if ((plane ^ center) < reff)
			{
				return (false);
			}
		}

	#endif

	return (true);
}

bool OcclusionRegion::BoxOccluded(const Point3D& center, const Vector3D *axis) const
{
	#if C4SIMD

		const vec_float sign_bit = VecFloatGetMinusZero();
		vec_float c = VecLoadUnaligned(&center.x);

		int32 count = planeCount;
		for (machine a = 0; a < count; a++)
		{
			vec_float plane = VecLoadUnaligned(&planeArray[a].x);

			vec_float a1 = VecDot3D(plane, VecLoadUnaligned(&axis[0].x));
			vec_float a2 = VecDot3D(plane, VecLoadUnaligned(&axis[1].x));
			vec_float a3 = VecDot3D(plane, VecLoadUnaligned(&axis[2].x));

			a1 = VecAndc(a1, sign_bit);
			a2 = VecAndc(a2, sign_bit);
			a3 = VecAndc(a3, sign_bit);

			a1 = VecAddScalar(a1, a2);
			a1 = VecAddScalar(a1, a3);

			if (VecCmpltScalar(VecPlaneWedgePoint3D(plane, c), a1))
			{
				return (false);
			}
		}

	#else

		int32 count = planeCount;
		for (machine a = 0; a < count; a++)
		{
			const Antivector4D& plane = planeArray[a];
			float reff = Fabs(plane ^ axis[0]) + Fabs(plane ^ axis[1]) + Fabs(plane ^ axis[2]);
			if ((plane ^ center) < reff)
			{
				return (false);
			}
		}

	#endif

	return (true);
}

bool OcclusionRegion::BoxOccluded(const Point3D& center, const Vector3D& size) const
{
	#if C4SIMD

		const vec_float sign_bit = VecFloatGetMinusZero();
		vec_float c = VecLoadUnaligned(&center.x);
		vec_float s = VecLoadUnaligned(&size.x);

		int32 count = planeCount;
		for (machine a = 0; a < count; a++)
		{
			vec_float plane = VecLoadUnaligned(&planeArray[a].x);
			vec_float reff = VecHorizontalSum3D(VecAndc(VecMul(plane, s), sign_bit));
			if (VecCmpltScalar(VecPlaneWedgePoint3D(plane, c), reff))
			{
				return (false);
			}
		}

	#else

		int32 count = planeCount;
		for (machine a = 0; a < count; a++)
		{
			const Antivector4D& plane = planeArray[a];
			float reff = Fabs(plane.x * size.x) + Fabs(plane.y * size.y) + Fabs(plane.z * size.z);
			if ((plane ^ center) < reff)
			{
				return (false);
			}
		}

	#endif

	return (true);
}

bool OcclusionRegion::DirectionOccluded(const Vector3D& direction, float radius) const
{
	int32 count = planeCount;
	for (machine a = 0; a < count; a++)
	{
		if ((planeArray[a] ^ direction) < radius)
		{
			return (false);
		}
	}

	return (true);
}

bool OcclusionRegion::QuadOccluded(const Point3D *vertex) const
{
	#if C4SIMD

		const vec_float zero = VecFloatGetZero();

		int32 count = planeCount;
		for (machine a = 0; a < count; a++)
		{
			vec_float plane = VecLoadUnaligned(&planeArray[a].x);
			for (machine b = 0; b < 4; b++)
			{
				if (VecCmpltScalar(VecPlaneWedgePoint3D(plane, VecLoadUnaligned(&vertex[b].x)), zero))
				{
					return (false);
				}
			}
		}

	#else

		int32 count = planeCount;
		for (machine a = 0; a < count; a++)
		{
			const Antivector4D& plane = planeArray[a];
			for (machine b = 0; b < 4; b++)
			{
				if ((plane ^ vertex[b]) < 0.0F)
				{
					return (false);
				}
			}
		}

	#endif

	return (true);
}

bool OcclusionRegion::PolygonOccluded(int32 vertexCount, const Point3D *vertex) const
{
	#if C4SIMD

		const vec_float zero = VecFloatGetZero();

		int32 count = planeCount;
		for (machine a = 0; a < count; a++)
		{
			vec_float plane = VecLoadUnaligned(&planeArray[a].x);
			for (machine b = 0; b < vertexCount; b++)
			{
				if (VecCmpltScalar(VecPlaneWedgePoint3D(plane, VecLoadUnaligned(&vertex[b].x)), zero))
				{
					return (false);
				}
			}
		}

	#else

		int32 count = planeCount;
		for (machine a = 0; a < count; a++)
		{
			const Antivector4D& plane = planeArray[a];
			for (machine b = 0; b < vertexCount; b++)
			{
				if ((plane ^ vertex[b]) < 0.0F)
				{
					return (false);
				}
			}
		}

	#endif

	return (true);
}

bool OcclusionRegion::SphereOccluded(const Point3D& center, float radius) const
{
	#if C4SIMD

		vec_float r = VecLoadScalar(&radius);
		vec_float c = VecLoadUnaligned(&center.x);

		int32 count = planeCount;
		for (machine a = 0; a < count; a++)
		{
			vec_float plane = VecLoadUnaligned(&planeArray[a].x);
			if (VecCmpltScalar(VecPlaneWedgePoint3D(plane, c), r))
			{
				return (false);
			}
		}

	#else

		int32 count = planeCount;
		for (machine a = 0; a < count; a++)
		{
			if ((planeArray[a] ^ center) < radius)
			{
				return (false);
			}
		}

	#endif

	return (true);
}

bool OcclusionRegion::EllipsoidOccluded(const Point3D& center, const Vector3D *axis) const
{
	#if C4SIMD

		vec_float c = VecLoadUnaligned(&center.x);

		int32 count = planeCount;
		for (machine a = 0; a < count; a++)
		{
			vec_float plane = VecLoadUnaligned(&planeArray[a].x);
			vec_float R_dot_N = VecDot3D(plane, VecLoadUnaligned(&axis[0].x));
			vec_float S_dot_N = VecDot3D(plane, VecLoadUnaligned(&axis[1].x));
			vec_float T_dot_N = VecDot3D(plane, VecLoadUnaligned(&axis[2].x));
			vec_float d2 = VecMul(R_dot_N, R_dot_N);
			d2 = VecMadd(S_dot_N, S_dot_N, d2);
			d2 = VecMadd(T_dot_N, T_dot_N, d2);

			vec_float reff = VecSqrt(d2);
			if (VecCmpltScalar(VecPlaneWedgePoint3D(plane, c), reff))
			{
				return (false);
			}
		}

	#else

		int32 count = planeCount;
		for (machine a = 0; a < count; a++)
		{
			const Antivector4D& plane = planeArray[a];
			float R_dot_N = plane ^ axis[0];
			float S_dot_N = plane ^ axis[1];
			float T_dot_N = plane ^ axis[2];
			float reff = Sqrt(R_dot_N * R_dot_N + S_dot_N * S_dot_N + T_dot_N * T_dot_N);
			if ((plane ^ center) < reff)
			{
				return (false);
			}
		}

	#endif

	return (true);
}

bool OcclusionRegion::CylinderOccluded(const Point3D& p1, const Point3D& p2, float radius) const
{
	#if C4SIMD

		vec_float q1 = VecLoadUnaligned(&p1.x);
		vec_float q2 = VecLoadUnaligned(&p2.x);

		vec_float dp = VecSub(q2, q1);
		vec_float m = VecInverseSqrtScalar(VecDot3D(dp, dp));
		dp = VecMul(dp, VecSmearX(m));

		const vec_float zero = VecFloatGetZero();
		const vec_float one = VecLoadVectorConstant<0x3F800000>();

		vec_float r = VecLoadScalar(&radius);

		int32 count = planeCount;
		for (machine a = 0; a < count; a++)
		{
			vec_float plane = VecLoadUnaligned(&planeArray[a].x);

			vec_float s = VecDot3D(plane, dp);
			vec_float reff = VecMulScalar(r, VecSqrtScalar(VecMaxScalar(VecNmsub(s, s, one), zero)));

			if ((VecCmpltScalar(VecPlaneWedgePoint3D(plane, q1), reff)) || (VecCmpltScalar(VecPlaneWedgePoint3D(plane, q2), reff)))
			{
				return (false);
			}
		}

	#else

		Vector3D dp = p2 - p1;
		dp.Normalize();

		int32 count = planeCount;
		for (machine a = 0; a < count; a++)
		{
			const Antivector4D& plane = planeArray[a];

			float s = plane ^ dp;
			float reff = radius * Sqrt(FmaxZero(1.0F - s * s));
			if (((plane ^ p1) < reff) || ((plane ^ p2) < reff))
			{
				return (false);
			}
		}

	#endif

	return (true);
}


ShadowRegion::ShadowRegion(Zone *zone, const ShadowRegion *shadowRegion) : VisibilityRegion(&regionConvexity)
{
	regionZone = zone;

	int32 count = shadowRegion->regionConvexity.planeCount;
	regionConvexity.planeCount = count;

	const Antivector4D *plane = shadowRegion->regionConvexity.plane;
	for (machine a = 0; a < count; a++)
	{
		regionConvexity.plane[a] = plane[a];
	}
}


CameraRegion::CameraRegion(const Camera *camera, Zone *zone) : VisibilityRegion(&regionPolyhedron)
{
	regionCamera = camera;
	regionZone = zone;
}

CameraRegion::CameraRegion(const CameraRegion *region, const Polyhedron *polyhedron) :
		VisibilityRegion(&regionPolyhedron),
		regionPolyhedron(*polyhedron)
{
	regionCamera = region->regionCamera;
	regionZone = region->regionZone;
}

bool CameraRegion::ContainsPoint(const Point3D& position) const
{
	int32 count = regionPolyhedron.planeCount;
	const Antivector4D *plane = regionPolyhedron.plane;

	for (machine a = 0; a < count; a++)
	{
		if ((plane[a] ^ position) < 0.0F)
		{
			return (false);
		}
	}

	return (true);
}

void CameraRegion::SetPolygonExtrusion(int32 vertexCount, const Point3D *vertex, const Point3D& cameraPosition, const Vector3D& viewDirection, float farDepth, const Antivector4D& frontPlane)
{
	Point3D			supportPoint;

	Antivector4D backPlane(-viewDirection, farDepth + cameraPosition * viewDirection);

	const Zone *zone = regionZone;
	if (zone->GetObject()->CalculateSupportPoint(zone->GetInverseWorldTransform() * viewDirection, &supportPoint))
	{
		backPlane.w = Fmin(backPlane.w, viewDirection * (zone->GetWorldTransform() * supportPoint));
	}

	float f = backPlane ^ cameraPosition;

	regionPolyhedron.planeCount = (unsigned_int8) (vertexCount + 2);
	regionPolyhedron.vertexCount = (unsigned_int8) (vertexCount * 2);
	regionPolyhedron.edgeCount = (unsigned_int8) (vertexCount * 3);
	regionPolyhedron.faceCount = (unsigned_int8) (vertexCount + 2);

	Point3D *v1 = regionPolyhedron.vertex;
	Point3D *v2 = v1 + vertexCount;
	Antivector4D *plane = regionPolyhedron.plane;

	Vector3D p1 = vertex[vertexCount - 1] - cameraPosition;
	for (machine a = 0; a < vertexCount; a++)
	{
		const Point3D& q = vertex[a];
		Vector3D p2 = q - cameraPosition;
		Antivector3D normal = Normalize(p2 ^ p1);

		plane[0].Set(normal, cameraPosition);
		plane++;

		v1[a] = q;
		v2[a] = cameraPosition - (f / (backPlane ^ p2)) * p2;

		p1 = p2;
	}

	plane[0] = frontPlane;
	plane[1] = backPlane;

	Edge *e1 = regionPolyhedron.edge;
	Edge *e2 = e1 + vertexCount;
	Edge *e3 = e2 + vertexCount;

	machine i = vertexCount - 1;
	for (machine j = 0; j < vertexCount; j++)
	{
		machine k = j + 1;
		k &= (k - vertexCount) >> 8;

		e1[j].vertexIndex[0] = (unsigned_int8) j;
		e1[j].vertexIndex[1] = (unsigned_int8) (j + vertexCount);
		e1[j].faceIndex[0] = (unsigned_int8) j;
		e1[j].faceIndex[1] = (unsigned_int8) k;

		e2[j].vertexIndex[0] = (unsigned_int8) j;
		e2[j].vertexIndex[1] = (unsigned_int8) k;
		e2[j].faceIndex[0] = (unsigned_int8) k;
		e2[j].faceIndex[1] = (unsigned_int8) vertexCount;

		e3[j].vertexIndex[0] = (unsigned_int8) (k + vertexCount);
		e3[j].vertexIndex[1] = (unsigned_int8) (j + vertexCount);
		e3[j].faceIndex[0] = (unsigned_int8) k;
		e3[j].faceIndex[1] = (unsigned_int8) (vertexCount + 1);

		regionPolyhedron.face[j].edgeCount = 4;
		regionPolyhedron.face[j].edgeIndex[0] = (unsigned_int8) j;
		regionPolyhedron.face[j].edgeIndex[1] = (unsigned_int8) (i + vertexCount * 2);
		regionPolyhedron.face[j].edgeIndex[2] = (unsigned_int8) i;
		regionPolyhedron.face[j].edgeIndex[3] = (unsigned_int8) (i + vertexCount);

		i = j;
	}

	Face *face = &regionPolyhedron.face[vertexCount];
	face[0].edgeCount = vertexCount;
	face[1].edgeCount = vertexCount;

	unsigned_int8 i1 = vertexCount;
	unsigned_int8 i2 = vertexCount * 3;
	for (machine a = 0; a < vertexCount; a++)
	{
		face[0].edgeIndex[a] = i1++;
		face[1].edgeIndex[a] = --i2;
	}

	SetAuxiliaryPlaneCount(1);
}


LightRegion::LightRegion(Light *light, Zone *zone, const Portal *portal) : VisibilityRegion(&regionPolyhedron)
{
	regionLight = light;
	regionZone = zone;
	illuminatedPortal = portal;
	regionStamp = 0xFFFFFFFF;
}

LightRegion::LightRegion(Light *light, Zone *zone, const Portal *portal, const LightRegion *region) : VisibilityRegion(&regionPolyhedron)
{
	regionLight = light;
	regionZone = zone;
	illuminatedPortal = portal;
	regionStamp = 0xFFFFFFFF;

	int32 count = region->GetPlaneCount();
	SetPlaneCount(count);

	const Antivector4D *p = region->GetPlaneArray();
	Antivector4D *plane = GetPlaneArray();
	for (machine a = 0; a < count; a++)
	{
		plane[a] = p[a];
	}
}

LightRegion::LightRegion(const LightRegion *region, const Polyhedron *polyhedron) :
		VisibilityRegion(&regionPolyhedron),
		regionPolyhedron(*polyhedron)
{
	regionLight = region->regionLight;
	regionZone = region->regionZone;
	illuminatedPortal = region->illuminatedPortal;
	regionStamp = 0xFFFFFFFF;
}

void LightRegion::SetInfinitePolygonExtrusion(int32 vertexCount, const Point3D *vertex, const Vector3D& lightDirection, const Antivector4D& frontPlane)
{
	Point3D		supportPoint;

	const Zone *zone = regionZone;
	if (zone->GetObject()->CalculateSupportPoint(zone->GetInverseWorldTransform() * lightDirection, &supportPoint))
	{
		Antivector4D backPlane(-lightDirection, zone->GetWorldTransform() * supportPoint);
		float f = 1.0F / (backPlane ^ lightDirection);

		regionPolyhedron.planeCount = (unsigned_int8) (vertexCount + 2);
		regionPolyhedron.vertexCount = (unsigned_int8) (vertexCount * 2);
		regionPolyhedron.edgeCount = (unsigned_int8) (vertexCount * 3);
		regionPolyhedron.faceCount = (unsigned_int8) (vertexCount + 2);

		Point3D *v1 = regionPolyhedron.vertex;
		Point3D *v2 = v1 + vertexCount;
		Antivector4D *plane = regionPolyhedron.plane;

		const Point3D *p1 = &vertex[vertexCount - 1];
		for (machine a = 0; a < vertexCount; a++)
		{
			const Point3D *p2 = &vertex[a];
			Antivector3D normal = Normalize((*p2 - *p1) ^ lightDirection);

			plane[0].Set(normal, *p1);
			plane++;

			v1[a] = *p2;
			v2[a] = *p2 - lightDirection * ((backPlane ^ *p2) * f);

			p1 = p2;
		}

		plane[0] = frontPlane;
		plane[1] = backPlane;

		Edge *e1 = regionPolyhedron.edge;
		Edge *e2 = e1 + vertexCount;
		Edge *e3 = e2 + vertexCount;

		machine i = vertexCount - 1;
		for (machine j = 0; j < vertexCount; j++)
		{
			machine k = j + 1;
			k &= (k - vertexCount) >> 8;

			e1[j].vertexIndex[0] = (unsigned_int8) j;
			e1[j].vertexIndex[1] = (unsigned_int8) (j + vertexCount);
			e1[j].faceIndex[0] = (unsigned_int8) j;
			e1[j].faceIndex[1] = (unsigned_int8) k;

			e2[j].vertexIndex[0] = (unsigned_int8) j;
			e2[j].vertexIndex[1] = (unsigned_int8) k;
			e2[j].faceIndex[0] = (unsigned_int8) k;
			e2[j].faceIndex[1] = (unsigned_int8) vertexCount;

			e3[j].vertexIndex[0] = (unsigned_int8) (k + vertexCount);
			e3[j].vertexIndex[1] = (unsigned_int8) (j + vertexCount);
			e3[j].faceIndex[0] = (unsigned_int8) k;
			e3[j].faceIndex[1] = (unsigned_int8) (vertexCount + 1);

			regionPolyhedron.face[j].edgeCount = 4;
			regionPolyhedron.face[j].edgeIndex[0] = (unsigned_int8) j;
			regionPolyhedron.face[j].edgeIndex[1] = (unsigned_int8) (i + vertexCount * 2);
			regionPolyhedron.face[j].edgeIndex[2] = (unsigned_int8) i;
			regionPolyhedron.face[j].edgeIndex[3] = (unsigned_int8) (i + vertexCount);

			i = j;
		}

		Face *face = &regionPolyhedron.face[vertexCount];
		face[0].edgeCount = vertexCount;
		face[1].edgeCount = vertexCount;

		unsigned_int8 i1 = vertexCount;
		unsigned_int8 i2 = vertexCount * 3;
		for (machine a = 0; a < vertexCount; a++)
		{
			face[0].edgeIndex[a] = i1++;
			face[1].edgeIndex[a] = --i2;
		}
	}
	else
	{
		regionPolyhedron.planeCount = (unsigned_int8) (vertexCount + 1);
		regionPolyhedron.vertexCount = 0;
		regionPolyhedron.edgeCount = 0;
		regionPolyhedron.faceCount = 0;

		Antivector4D *plane = regionPolyhedron.plane;
		const Point3D *p1 = &vertex[vertexCount - 1];

		for (machine a = 0; a < vertexCount; a++)
		{
			const Point3D *p2 = &vertex[a];
			Antivector3D normal = Normalize((*p2 - *p1) ^ lightDirection);

			plane[0].Set(normal, *p1);
			plane++;
			p1 = p2;
		}

		plane[0] = frontPlane;
	}
}

void LightRegion::SetPointPolygonExtrusion(int32 vertexCount, const Point3D *vertex, const Point3D& lightPosition, float lightRange, float confinementRadius, const Portal *portal)
{
	Point3D		supportPoint;

	Antivector4D frontPlane = -portal->GetWorldPlane();

	Point3D boundaryPoint = portal->CalculateClosestBoundaryPoint(lightPosition);
	Vector3D extrusionDirection = Normalize(boundaryPoint - lightPosition);
	Antivector4D backPlane(-extrusionDirection, lightRange + lightPosition * extrusionDirection);

	const Zone *zone = regionZone;
	if (zone->GetObject()->CalculateSupportPoint(zone->GetInverseWorldTransform() * extrusionDirection, &supportPoint))
	{
		Antivector4D oppositePlane(-extrusionDirection, zone->GetWorldTransform() * supportPoint);
		if ((oppositePlane ^ lightPosition) < lightRange)
		{
			backPlane = oppositePlane;
		}
	}

	regionPolyhedron.planeCount = (unsigned_int8) (vertexCount + 2);
	regionPolyhedron.vertexCount = (unsigned_int8) (vertexCount * 2);
	regionPolyhedron.edgeCount = (unsigned_int8) (vertexCount * 3);
	regionPolyhedron.faceCount = (unsigned_int8) (vertexCount + 2);

	Point3D *v1 = regionPolyhedron.vertex;
	Point3D *v2 = v1 + vertexCount;
	Antivector4D *plane = regionPolyhedron.plane;

	Vector3D p1 = vertex[vertexCount - 1] - lightPosition;

	if (confinementRadius < K::min_float)
	{
		float f = backPlane ^ lightPosition;

		for (machine a = 0; a < vertexCount; a++)
		{
			const Point3D& q = vertex[a];
			Vector3D p2 = q - lightPosition;
			Antivector3D normal = Normalize(p2 ^ p1);

			plane[0].Set(normal, lightPosition);
			plane++;

			v1[a] = q;
			v2[a] = lightPosition - (f / (backPlane ^ p2)) * p2;

			p1 = p2;
		}
	}
	else
	{
		float r2 = confinementRadius * confinementRadius;
		for (machine a = 0; a < vertexCount; a++)
		{
			const Point3D& q = vertex[a];
			Vector3D p2 = q - lightPosition;
			Antivector3D normal = Normalize(p2 ^ p1);

			float d2 = Math::SquaredDistancePointToLine(lightPosition, q, p2 - p1);
			Vector3D offset = normal * InverseSqrt((d2 - r2) / (r2 * d2));
			normal = Normalize((p2 - offset) ^ (p1 - offset));

			Point3D position = lightPosition + offset;
			plane[0].Set(normal, position);
			plane++;

			Vector3D p3 = q - position;

			v1[a] = q;
			v2[a] = position - ((backPlane ^ position) / (backPlane ^ p3)) * p3;

			p1 = p2;
		}
	}

	plane[0] = frontPlane;
	plane[1] = backPlane;

	Edge *e1 = regionPolyhedron.edge;
	Edge *e2 = e1 + vertexCount;
	Edge *e3 = e2 + vertexCount;

	machine i = vertexCount - 1;
	for (machine j = 0; j < vertexCount; j++)
	{
		machine k = j + 1;
		k &= (k - vertexCount) >> 8;

		e1[j].vertexIndex[0] = (unsigned_int8) j;
		e1[j].vertexIndex[1] = (unsigned_int8) (j + vertexCount);
		e1[j].faceIndex[0] = (unsigned_int8) j;
		e1[j].faceIndex[1] = (unsigned_int8) k;

		e2[j].vertexIndex[0] = (unsigned_int8) j;
		e2[j].vertexIndex[1] = (unsigned_int8) k;
		e2[j].faceIndex[0] = (unsigned_int8) k;
		e2[j].faceIndex[1] = (unsigned_int8) vertexCount;

		e3[j].vertexIndex[0] = (unsigned_int8) (k + vertexCount);
		e3[j].vertexIndex[1] = (unsigned_int8) (j + vertexCount);
		e3[j].faceIndex[0] = (unsigned_int8) k;
		e3[j].faceIndex[1] = (unsigned_int8) (vertexCount + 1);

		regionPolyhedron.face[j].edgeCount = 4;
		regionPolyhedron.face[j].edgeIndex[0] = (unsigned_int8) j;
		regionPolyhedron.face[j].edgeIndex[1] = (unsigned_int8) (i + vertexCount * 2);
		regionPolyhedron.face[j].edgeIndex[2] = (unsigned_int8) i;
		regionPolyhedron.face[j].edgeIndex[3] = (unsigned_int8) (i + vertexCount);

		i = j;
	}

	Face *face = &regionPolyhedron.face[vertexCount];
	face[0].edgeCount = vertexCount;
	face[1].edgeCount = vertexCount;

	unsigned_int8 i1 = vertexCount;
	unsigned_int8 i2 = vertexCount * 3;
	for (machine a = 0; a < vertexCount; a++)
	{
		face[0].edgeIndex[a] = i1++;
		face[1].edgeIndex[a] = --i2;
	}
}


#if C4DIAGS

	RegionRenderable::RegionRenderable(const Polyhedron *polyhedron) :
			Renderable(kRenderIndexedTriangles),
			vertexBuffer(kVertexBufferAttribute | kVertexBufferDynamic),
			indexBuffer(kVertexBufferIndex | kVertexBufferDynamic),
			diffuseColor(ColorRGBA(0.0F, 0.0625F, 0.0F, 0.0F), kAttributeMutable)
	{
		static Triangle triangleArray[kMaxPolyhedronFaceCount * (kMaxFaceEdgeCount - 2)];

		SetShaderFlags(kShaderAmbientEffect);
		SetRenderableFlags(kRenderableFogInhibit);
		SetAmbientBlendState(kBlendAccumulate);

		attributeList.Append(&diffuseColor);
		SetMaterialAttributeList(&attributeList);

		SetVertexBuffer(kVertexBufferAttributeArray, &vertexBuffer, sizeof(Point3D));
		SetVertexBuffer(kVertexBufferIndexArray, &indexBuffer);
		SetVertexAttributeArray(kArrayPosition, 0, 3);

		int32 triangleCount = polyhedron->GetTriangleArray(triangleArray);

		SetVertexCount(polyhedron->vertexCount);
		SetPrimitiveCount(triangleCount);

		vertexBuffer.Establish(sizeof(Point3D) * polyhedron->vertexCount, polyhedron->vertex);
		indexBuffer.Establish(sizeof(Triangle) * triangleCount, triangleArray);
	}

	RegionRenderable::~RegionRenderable()
	{
	}

#endif

// ZYUQURM
