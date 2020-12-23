 

#include "C4Bounding.h"
#include "C4Computation.h"


using namespace C4;


void Box2D::Union(const Box2D& box)
{
	float xmin = Fmin(min.x, box.min.x);
	float ymin = Fmin(min.y, box.min.y);

	float xmax = Fmax(max.x, box.max.x);
	float ymax = Fmax(max.y, box.max.y);

	min.Set(xmin, ymin);
	max.Set(xmax, ymax);
}

void Box2D::Union(const Point2D& p)
{
	min.x = Fmin(min.x, p.x);
	min.y = Fmin(min.y, p.y);
	max.x = Fmax(max.x, p.x);
	max.y = Fmax(max.y, p.y);
}

void Box2D::Intersect(const Box2D& box)
{
	float xmin = Fmax(min.x, box.min.x);
	float ymin = Fmax(min.y, box.min.y);

	float xmax = Fmin(max.x, box.max.x);
	float ymax = Fmin(max.y, box.max.y);

	min.Set(xmin, ymin);
	max.Set(xmax, ymax);
}

void Box2D::Transform(const Transform4D& transform)
{
	Point2D		p[3];

	p[0] = transform * Point2D(min.x, max.y);
	p[1] = transform * Point2D(max.x, max.y);
	p[2] = transform * Point2D(max.x, min.y);

	min = transform * Point2D(min.x, min.y);
	max = min;

	for (machine a = 0; a < 3; a++)
	{
		float x = p[a].x;
		min.x = Fmin(min.x, x);
		max.x = Fmax(max.x, x);

		float y = p[a].y;
		min.y = Fmin(min.y, y);
		max.y = Fmax(max.y, y);
	}
}

void Box2D::Calculate(int32 vertexCount, const Point2D *vertex)
{
	float xmin = vertex[0].x;
	float ymin = vertex[0].y;
	float xmax = xmin;
	float ymax = ymin;

	for (machine a = 1; a < vertexCount; a++)
	{
		const Point2D& v = vertex[a];

		float x = v.x;
		xmin = Fmin(xmin, x);
		xmax = Fmax(xmax, x);

		float y = v.y;
		ymin = Fmin(ymin, y);
		ymax = Fmax(ymax, y);
	}

	min.Set(xmin, ymin);
	max.Set(xmax, ymax);
}

void Box2D::Calculate(const Point2D& p, int32 vertexCount, const Point2D *vertex)
{
	float xmin = p.x;
	float ymin = p.y;
	float xmax = xmin;
	float ymax = ymin;

	for (machine a = 0; a < vertexCount; a++)
	{
		const Point2D& v = vertex[a];

		float x = v.x;
		xmin = Fmin(xmin, x);
		xmax = Fmax(xmax, x);
 
		float y = v.y;
		ymin = Fmin(ymin, y);
		ymax = Fmax(ymax, y); 
	}
 
	min.Set(xmin, ymin);
	max.Set(xmax, ymax);
} 

bool Box2D::Contains(const Point2D& p) const 
{ 
	return ((p.x >= min.x) && (p.x <= max.x) && (p.y >= min.y) && (p.y <= max.y));
}

bool Box2D::Intersection(const Box2D& box) const 
{
	if ((max.x < box.min.x) || (min.x > box.max.x))
	{
		return (false);
	}

	if ((max.y < box.min.y) || (min.y > box.max.y))
	{
		return (false);
	}

	return (true);
}

Box2D C4::Union(const Box2D& box1, const Box2D& box2)
{
	float xmin = Fmin(box1.min.x, box2.min.x);
	float ymin = Fmin(box1.min.y, box2.min.y);

	float xmax = Fmax(box1.max.x, box2.max.x);
	float ymax = Fmax(box1.max.y, box2.max.y);

	return (Box2D(Point2D(xmin, ymin), Point2D(xmax, ymax)));
}

Box2D C4::Transform(const Box2D& box, const Transform4D& transform)
{
	Point2D		p[3];

	p[0] = transform * Point2D(box.min.x, box.max.y);
	p[1] = transform * Point2D(box.max.x, box.max.y);
	p[2] = transform * Point2D(box.max.x, box.min.y);

	Point2D pmin = transform * Point2D(box.min.x, box.min.y);
	Point2D pmax = pmin;

	for (machine a = 0; a < 3; a++)
	{
		float x = p[a].x;
		pmin.x = Fmin(pmin.x, x);
		pmax.x = Fmax(pmax.x, x);

		float y = p[a].y;
		pmin.y = Fmin(pmin.y, y);
		pmax.y = Fmax(pmax.y, y);
	}

	return (Box2D(pmin, pmax));
}


#if !C4SIMD

	void Box3D::Union(const Box3D& box)
	{
		float xmin = Fmin(min.x, box.min.x);
		float ymin = Fmin(min.y, box.min.y);
		float zmin = Fmin(min.z, box.min.z);

		float xmax = Fmax(max.x, box.max.x);
		float ymax = Fmax(max.y, box.max.y);
		float zmax = Fmax(max.z, box.max.z);

		min.Set(xmin, ymin, zmin);
		max.Set(xmax, ymax, zmax);
	}

	void Box3D::Union(const Point3D& p)
	{
		min.x = Fmin(min.x, p.x);
		min.y = Fmin(min.y, p.y);
		min.z = Fmin(min.z, p.z);
		max.x = Fmax(max.x, p.x);
		max.y = Fmax(max.y, p.y);
		max.z = Fmax(max.z, p.z);
	}

	void Box3D::Intersect(const Box3D& box)
	{
		float xmin = Fmax(min.x, box.min.x);
		float ymin = Fmax(min.y, box.min.y);
		float zmin = Fmax(min.z, box.min.z);

		float xmax = Fmin(max.x, box.max.x);
		float ymax = Fmin(max.y, box.max.y);
		float zmax = Fmin(max.z, box.max.z);

		min.Set(xmin, ymin, zmin);
		max.Set(xmax, ymax, zmax);
	}

#endif

void Box3D::Transform(const Transform4D& transform)
{
	Point3D		p[7];

	p[0] = transform * Point3D(max.x, min.y, min.z);
	p[1] = transform * Point3D(min.x, max.y, min.z);
	p[2] = transform * Point3D(max.x, max.y, min.z);
	p[3] = transform * Point3D(min.x, min.y, max.z);
	p[4] = transform * Point3D(max.x, min.y, max.z);
	p[5] = transform * Point3D(min.x, max.y, max.z);
	p[6] = transform * max;

	#if C4SIMD

		vec_float pmin = TransformPoint3D(transform, VecLoadUnaligned(&min.x));
		vec_float pmax = pmin;

		for (machine a = 0; a < 7; a++)
		{
			vec_float v = VecLoadUnaligned(&p[a].x);
			pmin = VecMin(pmin, v);
			pmax = VecMax(pmax, v);
		}

		VecStore3D(pmin, &min.x);
		VecStore3D(pmax, &max.x);

	#else

		min = transform * min;
		max = min;

		for (machine a = 0; a < 7; a++)
		{
			float x = p[a].x;
			min.x = Fmin(min.x, x);
			max.x = Fmax(max.x, x);

			float y = p[a].y;
			min.y = Fmin(min.y, y);
			max.y = Fmax(max.y, y);

			float z = p[a].z;
			min.z = Fmin(min.z, z);
			max.z = Fmax(max.z, z);
		}

	#endif
}

void Box3D::Calculate(int32 vertexCount, const Point3D *vertex)
{
	#if C4SIMD

		vec_float pmin = VecLoadUnaligned(&vertex[0].x);
		vec_float pmax = pmin;

		for (machine a = 1; a < vertexCount; a++)
		{
			vec_float v = VecLoadUnaligned(&vertex[a].x);
			pmin = VecMin(pmin, v);
			pmax = VecMax(pmax, v);
		}

		VecStore3D(pmin, &min.x);
		VecStore3D(pmax, &max.x);

	#else

		float xmin = vertex[0].x;
		float ymin = vertex[0].y;
		float zmin = vertex[0].z;
		float xmax = xmin;
		float ymax = ymin;
		float zmax = zmin;

		for (machine a = 1; a < vertexCount; a++)
		{
			const Point3D& v = vertex[a];

			float x = v.x;
			xmin = Fmin(xmin, x);
			xmax = Fmax(xmax, x);

			float y = v.y;
			ymin = Fmin(ymin, y);
			ymax = Fmax(ymax, y);

			float z = v.z;
			zmin = Fmin(zmin, z);
			zmax = Fmax(zmax, z);
		}

		min.Set(xmin, ymin, zmin);
		max.Set(xmax, ymax, zmax);

	#endif
}

void Box3D::Calculate(const Point3D& p, int32 vertexCount, const Point3D *vertex)
{
	#if C4SIMD

		vec_float pmin = VecLoadUnaligned(&p.x);
		vec_float pmax = pmin;

		for (machine a = 0; a < vertexCount; a++)
		{
			vec_float v = VecLoadUnaligned(&vertex[a].x);
			pmin = VecMin(pmin, v);
			pmax = VecMax(pmax, v);
		}

		VecStore3D(pmin, &min.x);
		VecStore3D(pmax, &max.x);

	#else

		float xmin = p.x;
		float ymin = p.y;
		float zmin = p.z;
		float xmax = xmin;
		float ymax = ymin;
		float zmax = zmin;

		for (machine a = 0; a < vertexCount; a++)
		{
			const Point3D& v = vertex[a];

			float x = v.x;
			xmin = Fmin(xmin, x);
			xmax = Fmax(xmax, x);

			float y = v.y;
			ymin = Fmin(ymin, y);
			ymax = Fmax(ymax, y);

			float z = v.z;
			zmin = Fmin(zmin, z);
			zmax = Fmax(zmax, z);
		}

		min.Set(xmin, ymin, zmin);
		max.Set(xmax, ymax, zmax);

	#endif
}

bool Box3D::ExteriorSphere(const Point3D& center, float radius) const
{
	if (center.x < min.x - radius)
	{
		return (true);
	}

	if (center.x > max.x + radius)
	{
		return (true);
	}

	if (center.y < min.y - radius)
	{
		return (true);
	}

	if (center.y > max.y + radius)
	{
		return (true);
	}

	if (center.z < min.z - radius)
	{
		return (true);
	}

	if (center.z > max.z + radius)
	{
		return (true);
	}

	return (false);
}

bool Box3D::ExteriorSweptSphere(const Point3D& p1, const Point3D& p2, float radius) const
{
	if ((!ExteriorSphere(p1, radius)) || (!ExteriorSphere(p2, radius)))
	{
		return (false);
	}

	Point3D q1 = p1;
	Point3D q2 = p2;

	float f = max.z + radius;
	float d1 = f - q1.z;
	float d2 = f - q2.z;

	if (d1 < 0.0F)
	{
		if (d2 < 0.0F)
		{
			return (true);
		}

		float dz = q2.z - q1.z;
		if (Fabs(dz) > K::min_float)
		{
			float t = d1 / dz;
			q1 = q1 + (q2 - q1) * t;
		}
	}
	else if (d2 < 0.0F)
	{
		float dz = q2.z - q1.z;
		if (Fabs(dz) > K::min_float)
		{
			float t = d1 / dz;
			q2 = q1 + (q2 - q1) * t;
		}
	}

	f = min.z - radius;
	d1 = q1.z - f;
	d2 = q2.z - f;

	if (d1 < 0.0F)
	{
		if (d2 < 0.0F)
		{
			return (true);
		}

		float dz = q1.z - q2.z;
		if (Fabs(dz) > K::min_float)
		{
			float t = d1 / dz;
			q1 = q1 + (q2 - q1) * t;
		}
	}
	else if (d2 < 0.0F)
	{
		float dz = q1.z - q2.z;
		if (Fabs(dz) > K::min_float)
		{
			float t = d1 / dz;
			q2 = q1 + (q2 - q1) * t;
		}
	}

	f = max.y + radius;
	d1 = f - q1.y;
	d2 = f - q2.y;

	if (d1 < 0.0F)
	{
		if (d2 < 0.0F)
		{
			return (true);
		}

		float dy = q2.y - q1.y;
		if (Fabs(dy) > K::min_float)
		{
			float t = d1 / dy;
			q1 = q1 + (q2 - q1) * t;
		}
	}
	else if (d2 < 0.0F)
	{
		float dy = q2.y - q1.y;
		if (Fabs(dy) > K::min_float)
		{
			float t = d1 / dy;
			q2 = q1 + (q2 - q1) * t;
		}
	}

	f = min.y - radius;
	d1 = q1.y - f;
	d2 = q2.y - f;

	if (d1 < 0.0F)
	{
		if (d2 < 0.0F)
		{
			return (true);
		}

		float dy = q1.y - q2.y;
		if (Fabs(dy) > K::min_float)
		{
			float t = d1 / dy;
			q1 = q1 + (q2 - q1) * t;
		}
	}
	else if (d2 < 0.0F)
	{
		float dy = q1.y - q2.y;
		if (Fabs(dy) > K::min_float)
		{
			float t = d1 / dy;
			q2 = q1 + (q2 - q1) * t;
		}
	}

	f = max.x + radius;
	d1 = f - q1.x;
	d2 = f - q2.x;

	if (d1 < 0.0F)
	{
		if (d2 < 0.0F)
		{
			return (true);
		}

		float dx = q2.x - q1.x;
		if (Fabs(dx) > K::min_float)
		{
			float t = d1 / dx;
			q1 = q1 + (q2 - q1) * t;
		}
	}
	else if (d2 < 0.0F)
	{
		float dx = q2.x - q1.x;
		if (Fabs(dx) > K::min_float)
		{
			float t = d1 / dx;
			q2 = q1 + (q2 - q1) * t;
		}
	}

	f = min.x - radius;
	d1 = q1.x - f;
	d2 = q2.x - f;

	if (d1 < 0.0F)
	{
		if (d2 < 0.0F)
		{
			return (true);
		}

		float dx = q1.x - q2.x;
		if (Fabs(dx) > K::min_float)
		{
			float t = d1 / dx;
			q1 = q1 + (q2 - q1) * t;
		}
	}
	else if (d2 < 0.0F)
	{
		float dx = q1.x - q2.x;
		if (Fabs(dx) > K::min_float)
		{
			float t = d1 / dx;
			q2 = q1 + (q2 - q1) * t;
		}
	}

	return (false);
}

bool Box3D::Contains(const Point3D& p) const
{
	return ((p.x >= min.x) && (p.x <= max.x) && (p.y >= min.y) && (p.y <= max.y) && (p.z >= min.z) && (p.z <= max.z));
}

bool Box3D::Intersection(const Box3D& box) const
{
	#if C4SIMD

		if (VecCmpltAny3D(VecLoadUnaligned(&max.x), VecLoadUnaligned(&box.min.x)))
		{
			return (false);
		}

		if (VecCmpgtAny3D(VecLoadUnaligned(&min.x), VecLoadUnaligned(&box.max.x)))
		{
			return (false);
		}

	#else

		if ((max.x < box.min.x) || (min.x > box.max.x))
		{
			return (false);
		}

		if ((max.y < box.min.y) || (min.y > box.max.y))
		{
			return (false);
		}

		if ((max.z < box.min.z) || (min.z > box.max.z))
		{
			return (false);
		}

	#endif

	return (true);
}

Box3D C4::Union(const Box3D& box1, const Box3D& box2)
{
	#if C4SIMD

		vec_float pmin = VecMin(VecLoadUnaligned(&box1.min.x), VecLoadUnaligned(&box2.min.x));
		vec_float pmax = VecMax(VecLoadUnaligned(&box1.max.x), VecLoadUnaligned(&box2.max.x));
		return (Box3D(pmin, pmax));

	#else

		float xmin = Fmin(box1.min.x, box2.min.x);
		float ymin = Fmin(box1.min.y, box2.min.y);
		float zmin = Fmin(box1.min.z, box2.min.z);

		float xmax = Fmax(box1.max.x, box2.max.x);
		float ymax = Fmax(box1.max.y, box2.max.y);
		float zmax = Fmax(box1.max.z, box2.max.z);

		return (Box3D(Point3D(xmin, ymin, zmin), Point3D(xmax, ymax, zmax)));

	#endif
}

Box3D C4::Transform(const Box3D& box, const Transform4D& transform)
{
	Point3D		p[7];

	p[0] = transform * Point3D(box.max.x, box.min.y, box.min.z);
	p[1] = transform * Point3D(box.min.x, box.max.y, box.min.z);
	p[2] = transform * Point3D(box.max.x, box.max.y, box.min.z);
	p[3] = transform * Point3D(box.min.x, box.min.y, box.max.z);
	p[4] = transform * Point3D(box.max.x, box.min.y, box.max.z);
	p[5] = transform * Point3D(box.min.x, box.max.y, box.max.z);
	p[6] = transform * box.max;

	#if C4SIMD

		vec_float pmin = TransformPoint3D(transform, VecLoadUnaligned(&box.min.x));
		vec_float pmax = pmin;

		for (machine a = 0; a < 7; a++)
		{
			vec_float v = VecLoadUnaligned(&p[a].x);
			pmin = VecMin(pmin, v);
			pmax = VecMax(pmax, v);
		}

	#else

		Point3D pmin = transform * box.min;
		Point3D pmax = pmin;

		for (machine a = 0; a < 7; a++)
		{
			float x = p[a].x;
			pmin.x = Fmin(pmin.x, x);
			pmax.x = Fmax(pmax.x, x);

			float y = p[a].y;
			pmin.y = Fmin(pmin.y, y);
			pmax.y = Fmax(pmax.y, y);

			float z = p[a].z;
			pmin.z = Fmin(pmin.z, z);
			pmax.z = Fmax(pmax.z, z);
		}

	#endif

	return (Box3D(pmin, pmax));
}


void BoundingBox::Calculate(int32 vertexCount, const Point3D *vertex)
{
	Math::CalculatePrincipalAxes(vertexCount, vertex, &boxCenter, boxAxis);

	float xmin = K::infinity;
	float ymin = K::infinity;
	float zmin = K::infinity;
	float xmax = K::minus_infinity;
	float ymax = K::minus_infinity;
	float zmax = K::minus_infinity;

	for (machine a = 0; a < vertexCount; a++)
	{
		const Point3D& p = vertex[a];

		float x = p * boxAxis[0];
		xmin = Fmin(xmin, x);
		xmax = Fmax(xmax, x);

		float y = p * boxAxis[1];
		ymin = Fmin(ymin, y);
		ymax = Fmax(ymax, y);

		float z = p * boxAxis[2];
		zmin = Fmin(zmin, z);
		zmax = Fmax(zmax, z);
	}

	float i = (xmin + xmax) * 0.5F;
	float j = (ymin + ymax) * 0.5F;
	float k = (zmin + zmax) * 0.5F;
	boxCenter = boxAxis[0] * i + boxAxis[1] * j + boxAxis[2] * k;

	boxAxis[0] *= (xmax - xmin) * 0.5F;
	boxAxis[1] *= (ymax - ymin) * 0.5F;
	boxAxis[2] *= (zmax - zmin) * 0.5F;
}


void BoundingSphere::Union(const BoundingSphere *sphere)
{
	float r = sphere->sphereRadius;
	Vector3D v = sphere->sphereCenter - sphereCenter;
	float n = SquaredMag(v);

	if (n > K::min_float)
	{
		float m = InverseSqrt(n);
		float s = m * n;

		if (sphereRadius < s + r)
		{
			float t = s + sphereRadius;
			if (r < t)
			{
				t = (t + r) * 0.5F;
				sphereCenter += v * (m * (t - sphereRadius));
				sphereRadius = t;
			}
			else
			{
				sphereCenter = sphere->sphereCenter;
				sphereRadius = r;
			}
		}
	}
	else
	{
		sphereRadius = Fmax(sphereRadius, r);
	}
}

void BoundingSphere::Calculate(int32 vertexCount, const Point3D *vertex)
{
	Vector3D	axis[3];

	Math::CalculatePrincipalAxes(vertexCount, vertex, &sphereCenter, axis);

	float xmin = K::infinity;
	float ymin = K::infinity;
	float zmin = K::infinity;
	float xmax = K::minus_infinity;
	float ymax = K::minus_infinity;
	float zmax = K::minus_infinity;

	for (machine a = 0; a < vertexCount; a++)
	{
		const Point3D& p = vertex[a];

		float x = p * axis[0];
		xmin = Fmin(xmin, x);
		xmax = Fmax(xmax, x);

		float y = p * axis[1];
		ymin = Fmin(ymin, y);
		ymax = Fmax(ymax, y);

		float z = p * axis[2];
		zmin = Fmin(zmin, z);
		zmax = Fmax(zmax, z);
	}

	float i = (xmin + xmax) * 0.5F;
	float j = (ymin + ymax) * 0.5F;
	float k = (zmin + zmax) * 0.5F;
	sphereCenter = axis[0] * i + axis[1] * j + axis[2] * k;

	float r = (xmax - xmin) * 0.5F;
	for (machine a = 0; a < vertexCount; a++)
	{
		Vector3D dp = vertex[a] - sphereCenter;
		float n = SquaredMag(dp);
		if (n > r * r)
		{
			Point3D g = sphereCenter - (dp * (InverseSqrt(n) * r));
			sphereCenter = (g + vertex[a]) * 0.5F;
			r = Magnitude(g - sphereCenter);
		}
	}

	sphereRadius = r;
}

bool BoundingSphere::IntersectsSweptSphere(const Point3D& p1, const Point3D& p2, float radius) const
{
	float r = sphereRadius + radius;
	Vector3D dp = p2 - p1;

	Vector3D e1 = sphereCenter - p1;
	if (e1 * dp < 0.0F)
	{
		return (e1 * e1 < r * r);
	}

	Vector3D e2 = sphereCenter - p2;
	if (e2 * dp > 0.0F)
	{
		return (e2 * e2 < r * r);
	}

	float c = e1 * dp;
	float dp2 = dp * dp;
	return (e1 * e1 * dp2 - c * c < r * r * dp2);
}


void BoundingEllipsoid::Calculate(int32 vertexCount, const Point3D *vertex)
{
	Math::CalculatePrincipalAxes(vertexCount, vertex, &ellipsoidCenter, ellipsoidAxis);

	float sx = InverseMag(ellipsoidAxis[0]);
	float sy = InverseMag(ellipsoidAxis[1]);
	float sz = InverseMag(ellipsoidAxis[2]);
	sx *= sx;
	sy *= sy;
	sz *= sz;

	for (machine a = 0; a < vertexCount; a++)
	{
		Vector3D p = vertex[a] - ellipsoidCenter;

		float r2 = p.x * p.x * sx + p.y * p.y * sy + p.z * p.z * sz;
		if (r2 > 1.0F)
		{
			float r = Sqrt(r2);
			ellipsoidAxis[0] *= r;
			ellipsoidAxis[1] *= r;
			ellipsoidAxis[2] *= r;

			float s = 1.0F / r2;
			sx *= s;
			sy *= s;
			sz *= s;
		}
	}
}


void BoundingCylinder::Calculate(int32 vertexCount, const Point3D *vertex)
{
	Point3D		center;
	Vector3D	axis[3];

	Math::CalculatePrincipalAxes(vertexCount, vertex, &center, axis);

	float zmin = K::infinity;
	float zmax = K::minus_infinity;
	float rmax = 0.0F;

	for (machine a = 0; a < vertexCount; a++)
	{
		Vector3D p = vertex[a] - center;

		float z = p * axis[0];
		zmin = Fmin(zmin, z);
		zmax = Fmax(zmax, z);

		float r = Magnitude(p - axis[0] * z);
		rmax = Fmax(rmax, r);
	}

	cylinderEndpoint1 = center + axis[0] * zmin;
	cylinderEndpoint2 = center + axis[0] * zmax;
	cylinderRadius = rmax;
}

// ZYUQURM
