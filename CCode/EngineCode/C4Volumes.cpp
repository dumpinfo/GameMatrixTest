 

#include "C4Volumes.h"
#include "C4Bounding.h"


using namespace C4;


void Volume::PackVolume(Packer& data, unsigned_int32 packFlags) const
{
}

void Volume::UnpackVolume(Unpacker& data, unsigned_int32 unpackFlags)
{
}

int32 Volume::GetVolumeObjectSize(float *size) const
{
	return (0);
}

void Volume::SetVolumeObjectSize(const float *size)
{
}


void PlateVolume::PackVolume(Packer& data, unsigned_int32 packFlags) const
{
	data << plateSize;
}

void PlateVolume::UnpackVolume(Unpacker& data, unsigned_int32 unpackFlags)
{
	data >> plateSize;
}

int32 PlateVolume::GetVolumeObjectSize(float *size) const
{
	size[0] = plateSize.x;
	size[1] = plateSize.y;
	return (2);
}

void PlateVolume::SetVolumeObjectSize(const float *size)
{
	plateSize.Set(size[0], size[1]);
}

float PlateVolume::GetVolumeLength(void) const
{
	return (0.0F);
}

void PlateVolume::CalculateBoundingBox(Box3D *box) const
{
	box->min.Set(0.0F, 0.0F, 0.0F);
	box->max.Set(plateSize, 0.0F);
}

void PlateVolume::CalculateBoundingSphere(BoundingSphere *sphere) const
{
	float sx = plateSize.x * 0.5F;
	float sy = plateSize.y * 0.5F;

	sphere->SetCenter(sx, sy, 0.0F);
	sphere->SetRadius(Sqrt(sx * sx + sy * sy));
}

bool PlateVolume::ExteriorSphere(const Point3D& center, float radius) const
{
	if (center.z > radius)
	{
		return (true);
	}

	if (center.x > plateSize.x + radius)
	{
		return (true);
	}

	if (center.x < -radius)
	{
		return (true);
	}

	if (center.y > plateSize.y + radius)
	{
		return (true);
	}

	if (center.y < -radius)
	{
		return (true);
	}

	return (false);
}

bool PlateVolume::InteriorPoint(const Point3D& p) const
{
	if (p.z > 0.0F)
	{ 
		return (false);
	}
 
	if ((p.x < 0.0F) || (p.x > plateSize.x))
	{ 
		return (false);
	}
 
	if ((p.y < 0.0F) || (p.y > plateSize.y))
	{ 
		return (false); 
	}

	return (true);
} 


void BoxVolume::PackVolume(Packer& data, unsigned_int32 packFlags) const
{
	data << boxSize;
}

void BoxVolume::UnpackVolume(Unpacker& data, unsigned_int32 unpackFlags)
{
	data >> boxSize;
}

int32 BoxVolume::GetVolumeObjectSize(float *size) const
{
	size[0] = boxSize.x;
	size[1] = boxSize.y;
	size[2] = boxSize.z;
	return (3);
}

void BoxVolume::SetVolumeObjectSize(const float *size)
{
	boxSize.Set(size[0], size[1], size[2]);
}

float BoxVolume::GetVolumeLength(void) const
{
	return (boxSize.z);
}

void BoxVolume::CalculateBoundingBox(Box3D *box) const
{
	box->min.Set(0.0F, 0.0F, 0.0F);
	box->max = boxSize;
}

void BoxVolume::CalculateBoundingSphere(BoundingSphere *sphere) const
{
	float sx = boxSize.x * 0.5F;
	float sy = boxSize.y * 0.5F;
	float sz = boxSize.z * 0.5F;

	sphere->SetCenter(sx, sy, sz);
	sphere->SetRadius(Sqrt(sx * sx + sy * sy + sz * sz));
}

bool BoxVolume::ExteriorSphere(const Point3D& center, float radius) const
{
	if (center.z > boxSize.z + radius)
	{
		return (true);
	}

	if (center.z < -radius)
	{
		return (true);
	}

	if (center.y > boxSize.y + radius)
	{
		return (true);
	}

	if (center.y < -radius)
	{
		return (true);
	}

	if (center.x > boxSize.x + radius)
	{
		return (true);
	}

	if (center.x < -radius)
	{
		return (true);
	}

	return (false);
}

bool BoxVolume::InteriorPoint(const Point3D& p) const
{
	if ((p.x < 0.0F) || (p.x > boxSize.x))
	{
		return (false);
	}

	if ((p.y < 0.0F) || (p.y > boxSize.y))
	{
		return (false);
	}

	if ((p.z < 0.0F) || (p.z > boxSize.z))
	{
		return (false);
	}

	return (true);
}


void PyramidVolume::PackVolume(Packer& data, unsigned_int32 packFlags) const
{
	data << pyramidSize;
	data << pyramidHeight;
}

void PyramidVolume::UnpackVolume(Unpacker& data, unsigned_int32 unpackFlags)
{
	data >> pyramidSize;
	data >> pyramidHeight;
}

int32 PyramidVolume::GetVolumeObjectSize(float *size) const
{
	size[0] = pyramidSize.x;
	size[1] = pyramidSize.y;
	size[2] = pyramidHeight;
	return (3);
}

void PyramidVolume::SetVolumeObjectSize(const float *size)
{
	pyramidSize.Set(size[0], size[1]);
	pyramidHeight = size[2];
}

float PyramidVolume::GetVolumeLength(void) const
{
	return (pyramidHeight);
}

void PyramidVolume::CalculateBoundingBox(Box3D *box) const
{
	box->min.Set(0.0F, 0.0F, 0.0F);
	box->max.Set(pyramidSize, pyramidHeight);
}

void PyramidVolume::CalculateBoundingSphere(BoundingSphere *sphere) const
{
	float sx = pyramidSize.x * 0.5F;
	float sy = pyramidSize.y * 0.5F;
	float r = Sqrt(sx * sx + sy * sy);
	float h = pyramidHeight;

	if (h > r)
	{
		float d = (r * r + h * h) / (h * 2.0F);
		sphere->SetCenter(sx, sy, h - d);
		sphere->SetRadius(d);
	}
	else
	{
		sphere->SetCenter(sx, sy, 0.0F);
		sphere->SetRadius(r);
	}
}

bool PyramidVolume::ExteriorSphere(const Point3D& center, float radius) const
{
	if (center.z > pyramidHeight + radius)
	{
		return (true);
	}

	return (center.z < -radius);
}

bool PyramidVolume::InteriorPoint(const Point3D& p) const
{
	float h = pyramidHeight;
	if ((p.z < 0.0F) || (p.z > h))
	{
		return (false);
	}

	float m1 = (h - p.z) / h;
	float m2 = m1 + 1.0F;
	m1 = 1.0F - m1;

	float sx = pyramidSize.x * 0.5F;
	float sy = pyramidSize.y * 0.5F;

	if ((p.x < sx * m1) || (p.x > sx * m2))
	{
		return (false);
	}

	if ((p.y < sy * m1) || (p.y > sy * m2))
	{
		return (false);
	}

	return (true);
}


void CylinderVolume::PackVolume(Packer& data, unsigned_int32 packFlags) const
{
	data << cylinderSize;
	data << cylinderHeight;
}

void CylinderVolume::UnpackVolume(Unpacker& data, unsigned_int32 unpackFlags)
{
	data >> cylinderSize;
	data >> cylinderHeight;
}

int32 CylinderVolume::GetVolumeObjectSize(float *size) const
{
	size[0] = cylinderSize.x;
	size[1] = cylinderSize.y;
	size[2] = cylinderHeight;
	return (3);
}

void CylinderVolume::SetVolumeObjectSize(const float *size)
{
	cylinderSize.Set(size[0], size[1]);
	cylinderHeight = size[2];
}

float CylinderVolume::GetVolumeLength(void) const
{
	return (cylinderHeight);
}

void CylinderVolume::CalculateBoundingBox(Box3D *box) const
{
	box->min.Set(-cylinderSize, 0.0F);
	box->max.Set(cylinderSize, cylinderHeight);
}

void CylinderVolume::CalculateBoundingSphere(BoundingSphere *sphere) const
{
	float r = Fmax(cylinderSize.x, cylinderSize.y);
	float h = cylinderHeight * 0.5F;
	sphere->SetCenter(Point3D(0.0F, 0.0F, h));
	sphere->SetRadius(Sqrt(h * h + r * r));
}

bool CylinderVolume::ExteriorSphere(const Point3D& center, float radius) const
{
	if (center.z > cylinderHeight + radius)
	{
		return (true);
	}

	if (center.z < -radius)
	{
		return (true);
	}

	float x = center.x;
	float y = center.y;
	float n = SquaredMag(cylinderSize);
	return (x * x + y * y > n + 2.0F * radius * Sqrt(n) + radius * radius);
}

bool CylinderVolume::InteriorPoint(const Point3D& p) const
{
	if ((p.z < 0.0F) || (p.z > cylinderHeight))
	{
		return (false);
	}

	float sx = cylinderSize.x;
	float sy = cylinderSize.y;
	sx *= sx;
	sy *= sy;

	return (!(p.x * p.x * sy + p.y * p.y * sx > sx * sy));
}


void ConeVolume::PackVolume(Packer& data, unsigned_int32 packFlags) const
{
	data << coneSize;
	data << coneHeight;
}

void ConeVolume::UnpackVolume(Unpacker& data, unsigned_int32 unpackFlags)
{
	data >> coneSize;
	data >> coneHeight;
}

int32 ConeVolume::GetVolumeObjectSize(float *size) const
{
	size[0] = coneSize.x;
	size[1] = coneSize.y;
	size[2] = coneHeight;
	return (3);
}

void ConeVolume::SetVolumeObjectSize(const float *size)
{
	coneSize.Set(size[0], size[1]);
	coneHeight = size[2];
}

float ConeVolume::GetVolumeLength(void) const
{
	return (coneHeight);
}

void ConeVolume::CalculateBoundingBox(Box3D *box) const
{
	box->min.Set(-coneSize, 0.0F);
	box->max.Set(coneSize, coneHeight);
}

void ConeVolume::CalculateBoundingSphere(BoundingSphere *sphere) const
{
	float r = Fmax(coneSize.x, coneSize.y);
	float h = coneHeight;

	if (h > r)
	{
		float d = (r * r + h * h) / (h * 2.0F);
		sphere->SetCenter(0.0F, 0.0F, h - d);
		sphere->SetRadius(d);
	}
	else
	{
		sphere->SetCenter(0.0F, 0.0F, 0.0F);
		sphere->SetRadius(r);
	}
}

bool ConeVolume::ExteriorSphere(const Point3D& center, float radius) const
{
	if (center.z < -radius)
	{
		return (true);
	}

	float x = center.x;
	float y = center.y;
	float n = SquaredMag(coneSize);
	return (x * x + y * y > n + 2.0F * radius * Sqrt(n) + radius * radius);
}

bool ConeVolume::InteriorPoint(const Point3D& p) const
{
	float h = coneHeight;
	if ((p.z < 0.0F) || (p.z > h))
	{
		return (false);
	}

	float m = (h - p.z) / h;
	float sx = coneSize.x * m;
	float sy = coneSize.y * m;
	sx *= sx;
	sy *= sy;

	return (!(p.x * p.x * sy + p.y * p.y * sx > sx * sy));
}


void SphereVolume::PackVolume(Packer& data, unsigned_int32 packFlags) const
{
	data << sphereSize;
}

void SphereVolume::UnpackVolume(Unpacker& data, unsigned_int32 unpackFlags)
{
	data >> sphereSize;
}

int32 SphereVolume::GetVolumeObjectSize(float *size) const
{
	size[0] = sphereSize.x;
	size[1] = sphereSize.y;
	size[2] = sphereSize.z;
	return (3);
}

void SphereVolume::SetVolumeObjectSize(const float *size)
{
	sphereSize.Set(size[0], size[1], size[2]);
}

float SphereVolume::GetVolumeLength(void) const
{
	return (sphereSize.z * 2.0F);
}

void SphereVolume::CalculateBoundingBox(Box3D *box) const
{
	box->min = -sphereSize;
	box->max = sphereSize;
}

void SphereVolume::CalculateBoundingSphere(BoundingSphere *sphere) const
{
	sphere->SetCenter(Point3D(0.0F, 0.0F, 0.0F));
	sphere->SetRadius(Fmax(sphereSize.x, sphereSize.y, sphereSize.z));
}

bool SphereVolume::ExteriorSphere(const Point3D& center, float radius) const
{
	if (center.z > sphereSize.z + radius)
	{
		return (true);
	}

	if (center.z < -sphereSize.z - radius)
	{
		return (true);
	}

	if (center.y > sphereSize.y + radius)
	{
		return (true);
	}

	if (center.y < -sphereSize.y - radius)
	{
		return (true);
	}

	if (center.x > sphereSize.x + radius)
	{
		return (true);
	}

	if (center.x < -sphereSize.x - radius)
	{
		return (true);
	}

	return (false);
}

bool SphereVolume::InteriorPoint(const Point3D& p) const
{
	float sx = sphereSize.x;
	float sy = sphereSize.y;
	float sz = sphereSize.z;
	sx *= sx;
	sy *= sy;
	sz *= sz;

	float f = sx * sy;
	return (!(p.x * p.x * (sy * sz) + p.y * p.y * (sx * sz) + p.z * p.z * f > f * sz));
}


void DomeVolume::PackVolume(Packer& data, unsigned_int32 packFlags) const
{
	data << domeSize;
}

void DomeVolume::UnpackVolume(Unpacker& data, unsigned_int32 unpackFlags)
{
	data >> domeSize;
}

int32 DomeVolume::GetVolumeObjectSize(float *size) const
{
	size[0] = domeSize.x;
	size[1] = domeSize.y;
	size[2] = domeSize.z;
	return (3);
}

void DomeVolume::SetVolumeObjectSize(const float *size)
{
	domeSize.Set(size[0], size[1], size[2]);
}

float DomeVolume::GetVolumeLength(void) const
{
	return (domeSize.z);
}

void DomeVolume::CalculateBoundingBox(Box3D *box) const
{
	box->min.Set(-domeSize.x, -domeSize.y, 0.0F);
	box->max = domeSize;
}

void DomeVolume::CalculateBoundingSphere(BoundingSphere *sphere) const
{
	sphere->SetCenter(Point3D(0.0F, 0.0F, 0.0F));
	sphere->SetRadius(Fmax(domeSize.x, domeSize.y, domeSize.z));
}

bool DomeVolume::ExteriorSphere(const Point3D& center, float radius) const
{
	if (center.z > domeSize.z + radius)
	{
		return (true);
	}

	if (center.z < -radius)
	{
		return (true);
	}

	if (center.y > domeSize.y + radius)
	{
		return (true);
	}

	if (center.y < -domeSize.y - radius)
	{
		return (true);
	}

	if (center.x > domeSize.x + radius)
	{
		return (true);
	}

	if (center.x < -domeSize.x - radius)
	{
		return (true);
	}

	return (false);
}

bool DomeVolume::InteriorPoint(const Point3D& p) const
{
	if (p.z < 0.0F)
	{
		return (false);
	}

	float sx = domeSize.x;
	float sy = domeSize.y;
	float sz = domeSize.z;
	sx *= sx;
	sy *= sy;
	sz *= sz;

	float f = sx * sy;
	return (!(p.x * p.x * (sy * sz) + p.y * p.y * (sx * sz) + p.z * p.z * f > f * sz));
}


void CapsuleVolume::PackVolume(Packer& data, unsigned_int32 packFlags) const
{
	data << capsuleSize;
	data << capsuleHeight;
}

void CapsuleVolume::UnpackVolume(Unpacker& data, unsigned_int32 unpackFlags)
{
	data >> capsuleSize;
	data >> capsuleHeight;
}

int32 CapsuleVolume::GetVolumeObjectSize(float *size) const
{
	size[0] = capsuleSize.x;
	size[1] = capsuleSize.y;
	size[2] = capsuleSize.z;
	size[3] = capsuleHeight;
	return (4);
}

void CapsuleVolume::SetVolumeObjectSize(const float *size)
{
	capsuleSize.Set(size[0], size[1], size[2]);
	capsuleHeight = size[3];
}

float CapsuleVolume::GetVolumeLength(void) const
{
	return (capsuleSize.z * 2.0F + capsuleHeight);
}

void CapsuleVolume::CalculateBoundingBox(Box3D *box) const
{
	box->min = -capsuleSize;
	box->max.Set(capsuleSize.x, capsuleSize.y, capsuleSize.z + capsuleHeight);
}

void CapsuleVolume::CalculateBoundingSphere(BoundingSphere *sphere) const
{
	float h = capsuleHeight * 0.5F;
	sphere->SetCenter(Point3D(0.0F, 0.0F, h));
	sphere->SetRadius(Fmax(capsuleSize.x, capsuleSize.y, capsuleSize.z + h));
}

bool CapsuleVolume::ExteriorSphere(const Point3D& center, float radius) const
{
	if (center.z > capsuleHeight + capsuleSize.z + radius)
	{
		return (true);
	}

	if (center.z < capsuleSize.z - radius)
	{
		return (true);
	}

	float x = center.x;
	float y = center.y;
	float n = SquaredMag(capsuleSize.GetVector2D());
	return (x * x + y * y > n + 2.0F * radius * Sqrt(n) + radius * radius);
}

bool CapsuleVolume::InteriorPoint(const Point3D& p) const
{
	float sx = capsuleSize.x;
	float sy = capsuleSize.y;
	sx *= sx;
	sy *= sy;

	float h = capsuleHeight;
	if ((p.z > 0.0F) && (p.z < h))
	{
		return (!(p.x * p.x * sy + p.y * p.y * sx > sx * sy));
	}

	float f = sx * sy;
	float sz = capsuleSize.z;
	sz *= sz;

	if (p.z < h)
	{
		return (!(p.x * p.x * (sy * sz) + p.y * p.y * (sx * sz) + p.z * p.z * f > f * sz));
	}

	float z = p.z - h;
	return (!(p.x * p.x * (sy * sz) + p.y * p.y * (sx * sz) + z * z * f > f * sz));
}


void TruncatedPyramidVolume::PackVolume(Packer& data, unsigned_int32 packFlags) const
{
	PyramidVolume::PackVolume(data, packFlags);

	data << pyramidRatio;
}

void TruncatedPyramidVolume::UnpackVolume(Unpacker& data, unsigned_int32 unpackFlags)
{
	PyramidVolume::UnpackVolume(data, unpackFlags);

	data >> pyramidRatio;
}

int32 TruncatedPyramidVolume::GetVolumeObjectSize(float *size) const
{
	PyramidVolume::GetVolumeObjectSize(size);

	size[3] = pyramidRatio;
	return (4);
}

void TruncatedPyramidVolume::SetVolumeObjectSize(const float *size)
{
	PyramidVolume::SetVolumeObjectSize(size);

	pyramidRatio = size[3];
}

void TruncatedPyramidVolume::CalculateBoundingSphere(BoundingSphere *sphere) const
{
	float sx = GetPyramidSize().x * 0.5F;
	float sy = GetPyramidSize().y * 0.5F;
	float h = GetPyramidHeight();
	float r = pyramidRatio;

	float s2 = sx * sx + sy * sy;
	float z = FmaxZero((s2 * (r * r - 1.0F) + h * h) / (h * 2.0F));

	sphere->SetCenter(sx, sy, z);
	sphere->SetRadius(Sqrt(s2 + z * z));
}

bool TruncatedPyramidVolume::ExteriorSphere(const Point3D& center, float radius) const
{
	if (center.z > GetPyramidHeight() + radius)
	{
		return (true);
	}

	return (center.z < -radius);
}

bool TruncatedPyramidVolume::InteriorPoint(const Point3D& p) const
{
	float h = GetPyramidHeight();
	if ((p.z < 0.0F) || (p.z > h))
	{
		return (false);
	}

	float f = p.z / h * (pyramidRatio - 1.0F);
	float m1 = -f;
	float m2 = f + 2.0F;

	float sx = GetPyramidSize().x * 0.5F;
	float sy = GetPyramidSize().y * 0.5F;

	if ((p.x < sx * m1) || (p.x > sx * m2))
	{
		return (false);
	}

	if ((p.y < sy * m1) || (p.y > sy * m2))
	{
		return (false);
	}

	return (true);
}


void TruncatedConeVolume::PackVolume(Packer& data, unsigned_int32 packFlags) const
{
	ConeVolume::PackVolume(data, packFlags);

	data << coneRatio;
}

void TruncatedConeVolume::UnpackVolume(Unpacker& data, unsigned_int32 unpackFlags)
{
	ConeVolume::UnpackVolume(data, unpackFlags);

	data >> coneRatio;
}

int32 TruncatedConeVolume::GetVolumeObjectSize(float *size) const
{
	ConeVolume::GetVolumeObjectSize(size);

	size[3] = coneRatio;
	return (4);
}

void TruncatedConeVolume::SetVolumeObjectSize(const float *size)
{
	ConeVolume::SetVolumeObjectSize(size);

	coneRatio = size[3];
}

void TruncatedConeVolume::CalculateBoundingSphere(BoundingSphere *sphere) const
{
	float s = Fmax(GetConeSize().x, GetConeSize().y);
	float h = GetConeHeight();
	float r = coneRatio;

	float z = FmaxZero((s * s * (r * r - 1.0F) + h * h) / (h * 2.0F));

	sphere->SetCenter(0.0F, 0.0F, z);
	sphere->SetRadius(Sqrt(s * s + z * z));
}

bool TruncatedConeVolume::ExteriorSphere(const Point3D& center, float radius) const
{
	if (center.z > GetConeHeight() + radius)
	{
		return (true);
	}

	if (center.z < -radius)
	{
		return (true);
	}

	float x = center.x;
	float y = center.y;
	float n = SquaredMag(GetConeSize());
	return (x * x + y * y > n + 2.0F * radius * Sqrt(n) + radius * radius);
}

bool TruncatedConeVolume::InteriorPoint(const Point3D& p) const
{
	float h = GetConeHeight();
	if ((p.z < 0.0F) || (p.z > h))
	{
		return (false);
	}

	float f = p.z / h * (coneRatio - 1.0F) + 1.0F;
	float sx = GetConeSize().x * f;
	float sy = GetConeSize().y * f;
	sx *= sx;
	sy *= sy;

	return (!(p.x * p.x * sy + p.y * p.y * sx > sx * sy));
}


void TruncatedDomeVolume::PackVolume(Packer& data, unsigned_int32 packFlags) const
{
	DomeVolume::PackVolume(data, packFlags);

	data << domeHeight;
	data << domeRatio;
}

void TruncatedDomeVolume::UnpackVolume(Unpacker& data, unsigned_int32 unpackFlags)
{
	DomeVolume::UnpackVolume(data, unpackFlags);

	data >> domeHeight;
	data >> domeRatio;
}

int32 TruncatedDomeVolume::GetVolumeObjectSize(float *size) const
{
	DomeVolume::GetVolumeObjectSize(size);

	size[2] = domeHeight;
	size[3] = domeRatio;
	return (4);
}

void TruncatedDomeVolume::SetVolumeObjectSize(const float *size)
{
	DomeVolume::SetVolumeObjectSize(size);

	domeHeight = size[2];
	domeRatio = size[3];

	SetDomeSizeZ(domeHeight * InverseSqrt(1.0F - domeRatio * domeRatio));
}

void TruncatedDomeVolume::CalculateBoundingBox(Box3D *box) const
{
	const Vector2D& size = GetDomeSize().GetVector2D();
	box->min.Set(-size, 0.0F);
	box->max.Set(size, domeHeight);
}

void TruncatedDomeVolume::CalculateBoundingSphere(BoundingSphere *sphere) const
{
	float s = Fmax(GetDomeSize().x, GetDomeSize().y);
	float h = domeHeight;
	float r = domeRatio;

	float z = FmaxZero((s * s * (r * r - 1.0F) + h * h) / (h * 2.0F));

	sphere->SetCenter(0.0F, 0.0F, z);
	sphere->SetRadius(Sqrt(s * s + z * z));
}

bool TruncatedDomeVolume::ExteriorSphere(const Point3D& center, float radius) const
{
	if (center.z > domeHeight + radius)
	{
		return (true);
	}

	if (center.z < -radius)
	{
		return (true);
	}

	float x = center.x;
	float y = center.y;
	float n = SquaredMag(GetDomeSize().GetVector2D());
	return (x * x + y * y > n + 2.0F * radius * Sqrt(n) + radius * radius);
}

bool TruncatedDomeVolume::InteriorPoint(const Point3D& p) const
{
	float h = domeHeight;
	if ((p.z < 0.0F) || (p.z > h))
	{
		return (false);
	}

	float sx = GetDomeSize().x;
	float sy = GetDomeSize().y;
	float sz = GetDomeSize().z;
	sx *= sx;
	sy *= sy;
	sz *= sz;

	float f = sx * sy;
	return (!(p.x * p.x * (sy * sz) + p.y * p.y * (sx * sz) + p.z * p.z * f > f * sz));
}

// ZYUQURM
