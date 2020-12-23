 

#include "C4Blockers.h"


using namespace C4;


BlockerObject::BlockerObject(BlockerType type, Volume *volume) :
		Object(kObjectBlocker),
		VolumeObject(volume)
{
	blockerType = type;
}

BlockerObject::~BlockerObject()
{
}

BlockerObject *BlockerObject::Create(Unpacker& data, unsigned_int32 unpackFlags)
{
	switch (data.GetType())
	{
		case kBlockerPlate:

			return (new PlateBlockerObject);

		case kBlockerBox:

			return (new BoxBlockerObject);

		case kBlockerCylinder:

			return (new CylinderBlockerObject);

		case kBlockerSphere:

			return (new SphereBlockerObject);

		case kBlockerCapsule:

			return (new CapsuleBlockerObject);
	}

	return (nullptr);
}

void BlockerObject::PackType(Packer& data) const
{
	Object::PackType(data);
	data << blockerType;
}

void BlockerObject::Pack(Packer& data, unsigned_int32 packFlags) const
{
	PackVolume(data, packFlags);
}

void BlockerObject::Unpack(Unpacker& data, unsigned_int32 unpackFlags)
{
	UnpackVolume(data, unpackFlags);
}

int32 BlockerObject::GetObjectSize(float *size) const
{
	return (GetVolumeObjectSize(size));
}

void BlockerObject::SetObjectSize(const float *size)
{
	SetVolumeObjectSize(size);
}


PlateBlockerObject::PlateBlockerObject() : BlockerObject(kBlockerPlate, this)
{
}

PlateBlockerObject::PlateBlockerObject(const Vector2D& size) :
		BlockerObject(kBlockerPlate, this),
		PlateVolume(size)
{
}

PlateBlockerObject::~PlateBlockerObject()
{
}

void PlateBlockerObject::ApplyBlocker(int32 count, float radius, Point3D *restrict position, const Point3D *previous, const Transform4D& transform, const Transform4D& inverseTransform) const
{
	const Vector2D& size = GetPlateSize();
	float u = radius * 0.99609375F;

	for (machine k = 0; k < count; k++)
	{
		Point3D p = inverseTransform * previous[k];
		Point3D q = inverseTransform * position[k];

		if ((q.z < radius) && (p.z > u))
		{
			float dz = q.z - p.z;
			if (Fabs(dz) < K::min_float)
			{ 
				float x = q.x;
				float y = q.y;
 
				if ((Fmin(x, y) > -radius) && (Fmax(x - size.x, y - size.y) < radius))
				{ 
					position[k] = transform * Point3D(x, y, radius);
				}
			} 
			else
			{ 
				float t = (radius - p.z) / dz; 
				float s = 1.0F - t;

				float x = p.x * s + q.x * t;
				float y = p.y * s + q.y * t; 

				if ((Fmin(x, y) > -radius) && (Fmax(x - size.x, y - size.y) < radius))
				{
					position[k] = transform * Point3D(q.x, q.y, radius);
				}
			}
		}
	}
}


BoxBlockerObject::BoxBlockerObject() : BlockerObject(kBlockerBox, this)
{
}

BoxBlockerObject::BoxBlockerObject(const Vector3D& size) :
		BlockerObject(kBlockerBox, this),
		BoxVolume(size)
{
}

BoxBlockerObject::~BoxBlockerObject()
{
}

void BoxBlockerObject::ApplyBlocker(int32 count, float radius, Point3D *restrict position, const Point3D *previous, const Transform4D& transform, const Transform4D& inverseTransform) const
{
	const Vector3D& size = GetBoxSize();
	float u = radius * 0.99609375F;

	for (machine k = 0; k < count; k++)
	{
		Point3D p = inverseTransform * previous[k];
		Point3D q = inverseTransform * position[k];

		float sx = size.x + radius;
		if ((q.x < sx) && (p.x > size.x + u))
		{
			float dx = q.x - p.x;
			if (Fabs(dx) < K::min_float)
			{
				float y = q.y;
				float z = q.z;

				if ((Fmin(y, z) > -radius) && (Fmax(y - size.y, z - size.z) < radius))
				{
					position[k] = transform * Point3D(sx, y, z);
					continue;
				}
			}
			else
			{
				float t = (sx - p.x) / dx;
				float s = 1.0F - t;

				float y = p.y * s + q.y * t;
				float z = p.z * s + q.z * t;

				if ((Fmin(y, z) > -radius) && (Fmax(y - size.y, z - size.z) < radius))
				{
					position[k] = transform * Point3D(sx, q.y, q.z);
					continue;
				}
			}
		}

		sx = -radius;
		if ((q.x > sx) && (p.x < -u))
		{
			float dx = q.x - p.x;
			if (Fabs(dx) < K::min_float)
			{
				float y = q.y;
				float z = q.z;

				if ((Fmin(y, z) > -radius) && (Fmax(y - size.y, z - size.z) < radius))
				{
					position[k] = transform * Point3D(sx, y, z);
					continue;
				}
			}
			else
			{
				float t = (sx - p.x) / dx;
				float s = 1.0F - t;

				float y = p.y * s + q.y * t;
				float z = p.z * s + q.z * t;

				if ((Fmin(y, z) > -radius) && (Fmax(y - size.y, z - size.z) < radius))
				{
					position[k] = transform * Point3D(sx, q.y, q.z);
					continue;
				}
			}
		}

		float sy = size.y + radius;
		if ((q.y < sy) && (p.y > size.y + u))
		{
			float dy = q.y - p.y;
			if (Fabs(dy) < K::min_float)
			{
				float x = q.x;
				float z = q.z;

				if ((Fmin(x, z) > -radius) && (Fmax(x - size.x, z - size.z) < radius))
				{
					position[k] = transform * Point3D(x, sy, z);
					continue;
				}
			}
			else
			{
				float t = (sy - p.y) / dy;
				float s = 1.0F - t;

				float x = p.x * s + q.x * t;
				float z = p.z * s + q.z * t;

				if ((Fmin(x, z) > -radius) && (Fmax(x - size.x, z - size.z) < radius))
				{
					position[k] = transform * Point3D(q.x, sy, q.z);
					continue;
				}
			}
		}

		sy = -radius;
		if ((q.y > sy) && (p.y < -u))
		{
			float dy = q.y - p.y;
			if (Fabs(dy) < K::min_float)
			{
				float x = q.x;
				float z = q.z;

				if ((Fmin(x, z) > -radius) && (Fmax(x - size.x, z - size.z) < radius))
				{
					position[k] = transform * Point3D(x, sy, z);
					continue;
				}
			}
			else
			{
				float t = (sy - p.y) / dy;
				float s = 1.0F - t;

				float x = p.x * s + q.x * t;
				float z = p.z * s + q.z * t;

				if ((Fmin(x, z) > -radius) && (Fmax(x - size.x, z - size.z) < radius))
				{
					position[k] = transform * Point3D(q.x, sy, q.z);
					continue;
				}
			}
		}

		float sz = size.z + radius;
		if ((q.z < sz) && (p.z > size.z + u))
		{
			float dz = q.z - p.z;
			if (Fabs(dz) < K::min_float)
			{
				float x = q.x;
				float y = q.y;

				if ((Fmin(x, y) > -radius) && (Fmax(x - size.x, y - size.y) < radius))
				{
					position[k] = transform * Point3D(x, y, sz);
					continue;
				}
			}
			else
			{
				float t = (sz - p.z) / dz;
				float s = 1.0F - t;

				float x = p.x * s + q.x * t;
				float y = p.y * s + q.y * t;

				if ((Fmin(x, y) > -radius) && (Fmax(x - size.x, y - size.y) < radius))
				{
					position[k] = transform * Point3D(q.x, q.y, sz);
					continue;
				}
			}
		}

		sz = -radius;
		if ((q.z > sz) && (p.z < -u))
		{
			float dz = q.z - p.z;
			if (Fabs(dz) < K::min_float)
			{
				float x = q.x;
				float y = q.y;

				if ((Fmin(x, y) > -radius) && (Fmax(x - size.x, y - size.y) < radius))
				{
					position[k] = transform * Point3D(x, y, sz);
					continue;
				}
			}
			else
			{
				float t = (sz - p.z) / dz;
				float s = 1.0F - t;

				float x = p.x * s + q.x * t;
				float y = p.y * s + q.y * t;

				if ((Fmin(x, y) > -radius) && (Fmax(x - size.x, y - size.y) < radius))
				{
					position[k] = transform * Point3D(q.x, q.y, sz);
					continue;
				}
			}
		}
	}
}


CylinderBlockerObject::CylinderBlockerObject() : BlockerObject(kBlockerCylinder, this)
{
}

CylinderBlockerObject::CylinderBlockerObject(const Vector2D& size, float height) :
		BlockerObject(kBlockerCylinder, this),
		CylinderVolume(size, height)
{
}

CylinderBlockerObject::~CylinderBlockerObject()
{
}

void CylinderBlockerObject::ApplyBlocker(int32 count, float radius, Point3D *restrict position, const Point3D *previous, const Transform4D& transform, const Transform4D& inverseTransform) const
{
	const Vector2D& size = GetCylinderSize();
	float height = GetCylinderHeight();
	float u = radius * 0.99609375F;

	float rx = size.x + radius;
	float ry = size.y + radius;
	float r2 = rx * rx;
	float mx = 1.0F / r2;
	float my = 1.0F / (ry * ry);
	float r2my = r2 * my;

	for (machine k = 0; k < count; k++)
	{
		Point3D p = inverseTransform * previous[k];
		Point3D q = inverseTransform * position[k];
		Vector3D dp = q - p;

		float sz = height + radius;
		if ((q.z < sz) && (p.z > height + u))
		{
			if (Fabs(dp.z) < K::min_float)
			{
				float x = q.x;
				float y = q.y;

				if (x * x + y * y * r2my < r2)
				{
					position[k] = transform * Point3D(x, y, sz);
					continue;
				}
			}
			else
			{
				float t = (sz - p.z) / dp.z;
				float s = 1.0F - t;

				float x = p.x * s + q.x * t;
				float y = p.y * s + q.y * t;

				if (x * x + y * y * r2my < r2)
				{
					position[k] = transform * Point3D(q.x, q.y, sz);
					continue;
				}
			}
		}

		sz = -radius;
		if ((q.z > sz) && (p.z < -u))
		{
			if (Fabs(dp.z) < K::min_float)
			{
				float x = q.x;
				float y = q.y;

				if (x * x + y * y * r2my < r2)
				{
					position[k] = transform * Point3D(x, y, sz);
					continue;
				}
			}
			else
			{
				float t = (sz - p.z) / dp.z;
				float s = 1.0F - t;

				float x = p.x * s + q.x * t;
				float y = p.y * s + q.y * t;

				if (x * x + y * y * r2my < r2)
				{
					position[k] = transform * Point3D(q.x, q.y, sz);
					continue;
				}
			}
		}

		float a = dp.x * dp.x + dp.y * dp.y * r2my;
		float b = p.x * dp.x + p.y * dp.y * r2my;
		float c = p.x * p.x + p.y * p.y * r2my - r2;
		float d = b * b - a * c;

		if (d > K::min_float)
		{
			float t = -(b + Sqrt(d)) / a;
			if ((t < 1.0F) && (t > -radius * InverseMag(dp)))
			{
				float s = 1.0F - t;
				float z = p.z * s + q.z * t;
				if ((z > -radius) && (z < height + radius))
				{
					float x = p.x * s + q.x * t;
					float y = p.y * s + q.y * t;

					Vector3D normal(x * mx, y * my, 0.0F);
					position[k] = transform * (p + (dp - ProjectOnto(dp, Normalize(normal))));
					continue;
				}
			}
		}
	}
}


SphereBlockerObject::SphereBlockerObject() : BlockerObject(kBlockerSphere, this)
{
}

SphereBlockerObject::SphereBlockerObject(const Vector3D& size) :
		BlockerObject(kBlockerSphere, this),
		SphereVolume(size)
{
}

SphereBlockerObject::~SphereBlockerObject()
{
}

void SphereBlockerObject::ApplyBlocker(int32 count, float radius, Point3D *restrict position, const Point3D *previous, const Transform4D& transform, const Transform4D& inverseTransform) const
{
	const Vector3D& size = GetSphereSize();

	float rx = size.x + radius;
	float ry = size.y + radius;
	float rz = size.z + radius;
	float r2 = rx * rx;
	float mx = 1.0F / r2;
	float my = 1.0F / (ry * ry);
	float mz = 1.0F / (rz * rz);
	float r2my = r2 * my;
	float r2mz = r2 * mz;

	for (machine k = 0; k < count; k++)
	{
		Point3D p = inverseTransform * previous[k];
		Point3D q = inverseTransform * position[k];
		Vector3D dp = q - p;

		float a = dp.x * dp.x + dp.y * dp.y * r2my + dp.z * dp.z * r2mz;
		float b = p.x * dp.x + p.y * dp.y * r2my + p.z * dp.z * r2mz;
		float c = p.x * p.x + p.y * p.y * r2my + p.z * p.z * r2mz - r2;
		float d = b * b - a * c;

		if (d > K::min_float)
		{
			float t = -(b + Sqrt(d)) / a;
			if ((t < 1.0F) && (t > -radius * InverseMag(dp)))
			{
				float s = 1.0F - t;
				float x = p.x * s + q.x * t;
				float y = p.y * s + q.y * t;
				float z = p.z * s + q.z * t;

				Vector3D normal(x * mx, y * my, z * mz);
				position[k] = transform * (p + (dp - ProjectOnto(dp, Normalize(normal))));
			}
		}
	}
}


CapsuleBlockerObject::CapsuleBlockerObject() : BlockerObject(kBlockerCapsule, this)
{
}

CapsuleBlockerObject::CapsuleBlockerObject(const Vector3D& size, float height) :
		BlockerObject(kBlockerCapsule, this),
		CapsuleVolume(size, height)
{
}

CapsuleBlockerObject::~CapsuleBlockerObject()
{
}

void CapsuleBlockerObject::ApplyBlocker(int32 count, float radius, Point3D *restrict position, const Point3D *previous, const Transform4D& transform, const Transform4D& inverseTransform) const
{
	const Vector3D& size = GetCapsuleSize();
	float height = GetCapsuleHeight();

	float rx = size.x + radius;
	float ry = size.y + radius;
	float rz = size.z + radius;
	float r2 = rx * rx;
	float mx = 1.0F / r2;
	float my = 1.0F / (ry * ry);
	float mz = 1.0F / (rz * rz);
	float r2my = r2 * my;
	float r2mz = r2 * mz;

	for (machine k = 0; k < count; k++)
	{
		Point3D p = inverseTransform * previous[k];
		Point3D q = inverseTransform * position[k];
		Vector3D dp = q - p;

		float tmin = -radius * InverseMag(dp);

		float a0 = dp.x * dp.x + dp.y * dp.y * r2my;
		float b0 = p.x * dp.x + p.y * dp.y * r2my;
		float c0 = p.x * p.x + p.y * p.y * r2my - r2;
		float d = b0 * b0 - a0 * c0;

		if (d > K::min_float)
		{
			float t = -(b0 + Sqrt(d)) / a0;
			if ((t < 1.0F) && (t > tmin))
			{
				float s = 1.0F - t;
				float z = p.z * s + q.z * t;
				if ((z > -radius) && (z < height + radius))
				{
					float x = p.x * s + q.x * t;
					float y = p.y * s + q.y * t;

					Vector3D normal(x * mx, y * my, 0.0F);
					position[k] = transform * (p + (dp - ProjectOnto(dp, Normalize(normal))));
					continue;
				}
			}
		}

		float a1 = a0 + dp.z * dp.z * r2mz;
		float b1 = b0 + p.z * dp.z * r2mz;
		float c1 = c0 + p.z * p.z * r2mz;
		d = b1 * b1 - a1 * c1;

		if (d > K::min_float)
		{
			float t = -(b1 + Sqrt(d)) / a1;
			if ((t < 1.0F) && (t > tmin))
			{
				float s = 1.0F - t;
				float x = p.x * s + q.x * t;
				float y = p.y * s + q.y * t;
				float z = p.z * s + q.z * t;

				Vector3D normal(x * mx, y * my, z * mz);
				position[k] = transform * (p + (dp - ProjectOnto(dp, Normalize(normal))));
				continue;
			}
		}

		float sz = p.z - height;
		b1 = b0 + sz * dp.z * r2mz;
		c1 = c0 + sz * sz * r2mz;
		d = b1 * b1 - a1 * c1;

		if (d > K::min_float)
		{
			float t = -(b1 + Sqrt(d)) / a1;
			if ((t < 1.0F) && (t > tmin))
			{
				float s = 1.0F - t;
				float x = p.x * s + q.x * t;
				float y = p.y * s + q.y * t;
				float z = p.z * s + q.z * t;

				Vector3D normal(x * mx, y * my, (z - height) * mz);
				position[k] = transform * (p + (dp - ProjectOnto(dp, Normalize(normal))));
				continue;
			}
		}
	}
}


Blocker::Blocker(BlockerType type) : Node(kNodeBlocker)
{
	blockerType = type;
}

Blocker::Blocker(const Blocker& blocker) : Node(blocker)
{
	blockerType = blocker.blockerType;
}

Blocker::~Blocker()
{
}

Blocker *Blocker::Create(Unpacker& data, unsigned_int32 unpackFlags)
{
	switch (data.GetType())
	{
		case kBlockerPlate:

			return (new PlateBlocker);

		case kBlockerBox:

			return (new BoxBlocker);

		case kBlockerCylinder:

			return (new CylinderBlocker);

		case kBlockerSphere:

			return (new SphereBlocker);

		case kBlockerCapsule:

			return (new CapsuleBlocker);
	}

	return (nullptr);
}

void Blocker::PackType(Packer& data) const
{
	Node::PackType(data);
	data << blockerType;
}


PlateBlocker::PlateBlocker() : Blocker(kBlockerPlate)
{
}

PlateBlocker::PlateBlocker(const Vector2D& size) : Blocker(kBlockerPlate)
{
	SetNewObject(new PlateBlockerObject(size));
}

PlateBlocker::PlateBlocker(const PlateBlocker& plateBlocker) : Blocker(plateBlocker)
{
}

PlateBlocker::~PlateBlocker()
{
}

Node *PlateBlocker::Replicate(void) const
{
	return (new PlateBlocker(*this));
}


BoxBlocker::BoxBlocker() : Blocker(kBlockerBox)
{
}

BoxBlocker::BoxBlocker(const Vector3D& size) : Blocker(kBlockerBox)
{
	SetNewObject(new BoxBlockerObject(size));
}

BoxBlocker::BoxBlocker(const BoxBlocker& boxBlocker) : Blocker(boxBlocker)
{
}

BoxBlocker::~BoxBlocker()
{
}

Node *BoxBlocker::Replicate(void) const
{
	return (new BoxBlocker(*this));
}


CylinderBlocker::CylinderBlocker() : Blocker(kBlockerCylinder)
{
}

CylinderBlocker::CylinderBlocker(const Vector2D& size, float height) : Blocker(kBlockerCylinder)
{
	SetNewObject(new CylinderBlockerObject(size, height));
}

CylinderBlocker::CylinderBlocker(const CylinderBlocker& cylinderBlocker) : Blocker(cylinderBlocker)
{
}

CylinderBlocker::~CylinderBlocker()
{
}

Node *CylinderBlocker::Replicate(void) const
{
	return (new CylinderBlocker(*this));
}


SphereBlocker::SphereBlocker() : Blocker(kBlockerSphere)
{
}

SphereBlocker::SphereBlocker(const Vector3D& size) : Blocker(kBlockerSphere)
{
	SetNewObject(new SphereBlockerObject(size));
}

SphereBlocker::SphereBlocker(const SphereBlocker& sphereBlocker) : Blocker(sphereBlocker)
{
}

SphereBlocker::~SphereBlocker()
{
}

Node *SphereBlocker::Replicate(void) const
{
	return (new SphereBlocker(*this));
}


CapsuleBlocker::CapsuleBlocker() : Blocker(kBlockerCapsule)
{
}

CapsuleBlocker::CapsuleBlocker(const Vector3D& size, float height) : Blocker(kBlockerCapsule)
{
	SetNewObject(new CapsuleBlockerObject(size, height));
}

CapsuleBlocker::CapsuleBlocker(const CapsuleBlocker& capsuleBlocker) : Blocker(capsuleBlocker)
{
}

CapsuleBlocker::~CapsuleBlocker()
{
}

Node *CapsuleBlocker::Replicate(void) const
{
	return (new CapsuleBlocker(*this));
}

// ZYUQURM
