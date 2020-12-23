#include "C4Zones.h"
#include "C4Configuration.h"


using namespace C4;


namespace
{
	const float kZoneClipEpsilon = 0.001F;
}


const char C4::kConnectorKeyFog[]			= "%Fog";
const char C4::kConnectorKeyAcoustics[]		= "%Acoustics";
const char C4::kConnectorKeyRadiosity[]		= "%Radiosity";


unsigned_int32 (*const ZoneObject::simplexMinFunc[4])(const Point3D *, Point3D *) =
{
	&CalculateZeroSimplexMinimum, &CalculateOneSimplexMinimum, &CalculateTwoSimplexMinimum, &CalculateThreeSimplexMinimum
};


ZoneObject::ZoneObject(ZoneType type) : Object(kObjectZone)
{
	zoneType = type;
	zoneFlags = 0;

	ambientLight.Set(1.0F, 1.0F, 1.0F);

	environmentMap = nullptr;
	environmentName[0] = 0;
}

ZoneObject::~ZoneObject()
{
	if (environmentMap)
	{
		environmentMap->Release();
	}
}

ZoneObject *ZoneObject::Create(Unpacker& data, unsigned_int32 unpackFlags)
{
	switch (data.GetType())
	{
		case kZoneInfinite:

			return (new InfiniteZoneObject);

		case kZoneBox:

			return (new BoxZoneObject);

		case kZoneCylinder:

			return (new CylinderZoneObject);

		case kZonePolygon:

			return (new PolygonZoneObject);
	}

	return (nullptr);
}

void ZoneObject::PackType(Packer& data) const
{
	Object::PackType(data);
	data << zoneType;
}

void ZoneObject::Pack(Packer& data, unsigned_int32 packFlags) const
{
	data << ChunkHeader('FLAG', 4);
	data << zoneFlags;

	data << ChunkHeader('AMBT', sizeof(ColorRGBA));
	data << ambientLight;

	if (environmentName[0] != 0)
	{
		PackHandle handle = data.BeginChunk('ENVR');
		data << environmentName;
		data.EndChunk(handle);
	}

	data << TerminatorChunk;
}

void ZoneObject::Unpack(Unpacker& data, unsigned_int32 unpackFlags)
{
	UnpackChunkList<ZoneObject>(data, unpackFlags);
}

bool ZoneObject::UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags)
{
	switch (chunkHeader->chunkType)
	{
		case 'FLAG':

			data >> zoneFlags;
			return (true);

		case 'AMBT':

			data >> ambientLight;
			return (true);

		case 'ENVR':

			data >> environmentName;
			SetEnvironmentMap(environmentName);
			return (true);
	}

	return (false);
}

void *ZoneObject::BeginSettingsUnpack(void)
{
	if (environmentMap)
	{
		environmentMap->Release();
		environmentMap = nullptr;
	}

	return (nullptr);
}

int32 ZoneObject::GetCategoryCount(void) const
{
	return (1);
}

Type ZoneObject::GetCategoryType(int32 index, const char **title) const
{
	if (index == 0)
	{
		*title = TheInterfaceMgr->GetStringTable()->GetString(StringID(kObjectZone));
		return (kObjectZone);
	}

	return (0);
}

int32 ZoneObject::GetCategorySettingCount(Type category) const
{
	if (category == kObjectZone)
	{
		return (4);
	}

	return (0);
}

Setting *ZoneObject::GetCategorySetting(Type category, int32 index, unsigned_int32 flags) const
{
	if (category == kObjectZone)
	{
		const StringTable *table = TheInterfaceMgr->GetStringTable();

		if (index == 0)
		{
			const char *title = table->GetString(StringID(kObjectZone, 'ZONE'));
			return (new HeadingSetting(kObjectZone, title));
		}

		if (index == 1)
		{
			const char *title = table->GetString(StringID(kObjectZone, 'ZONE', 'SKYB'));
			return (new BooleanSetting('SKYB', ((zoneFlags & kZoneRenderSkybox) != 0), title));
		}

		if (index == 2)
		{
			const char *title = table->GetString(StringID(kObjectZone, 'ZONE', 'AMBT'));
			const char *picker = table->GetString(StringID(kObjectZone, 'ZONE', 'CPCK'));
			return (new ColorSetting('AMBT', ambientLight, title, picker));
		}

		if (index == 3)
		{
			const char *title = table->GetString(StringID(kObjectZone, 'ZONE', 'ENVR'));
			const char *picker = table->GetString(StringID(kObjectZone, 'ZONE', 'TPCK'));
			return (new ResourceSetting('ENVR', environmentName, title, picker, TextureResource::GetDescriptor()));
		}
	}

	return (nullptr);
}

void ZoneObject::SetCategorySetting(Type category, const Setting *setting)
{
	if (category == kObjectZone)
	{
		Type identifier = setting->GetSettingIdentifier();

		if (identifier == 'SKYB')
		{
			if (static_cast<const BooleanSetting *>(setting)->GetBooleanValue())
			{
				zoneFlags |= kZoneRenderSkybox;
			}
			else
			{
				zoneFlags &= ~kZoneRenderSkybox;
			}
		}
		else if (identifier == 'AMBT')
		{
			ambientLight = static_cast<const ColorSetting *>(setting)->GetColor();
		}
		else if (identifier == 'ENVR')
		{
			environmentName = static_cast<const ResourceSetting *>(setting)->GetResourceName();
		}
	}
}

void ZoneObject::SetEnvironmentMap(const char *name)
{
	Texture *texture = environmentMap;

	environmentName = name;
	if (name[0] == 0)
	{
		name = "C4/environment";
	}

	environmentMap = Texture::Get(name);
	if (!environmentMap)
	{
		environmentMap = Texture::Get("C4/environment");
	}

	if (texture)
	{
		texture->Release();
	}
}

unsigned_int32 ZoneObject::CalculateZeroSimplexMinimum(const Point3D *simplex, Point3D *p)
{
	*p = simplex[0];
	return (0x0001);
}

unsigned_int32 ZoneObject::CalculateOneSimplexMinimum(const Point3D *simplex, Point3D *p)
{
	const Point3D& q1 = simplex[0];
	const Point3D& q2 = simplex[1];
	DVector3D v = q2 - q1;

	if (!(v * q1 < 0.0))
	{
		// Origin is in Voronoi region for vertex q1.

		*p = q1;
		return (0x0001);
	}

	if (!(v * q2 > 0.0))
	{
		// Origin is in Voronoi region for vertex q2.

		*p = q2;
		return (0x0002);
	}

	// Origin is in Voronoi region for line segment interior.

	*p = q1 - ProjectOnto(q1, v) / SquaredMag(v);
	return (0x0003);
}

unsigned_int32 ZoneObject::CalculateTwoSimplexMinimum(const Point3D *simplex, Point3D *p)
{
	const Point3D& q1 = simplex[0];
	const Point3D& q2 = simplex[1];
	const Point3D& q3 = simplex[2];
	DVector3D v1 = q2 - q1;
	DVector3D v2 = q3 - q2;
	DVector3D v3 = q1 - q3;

	bool m1 = !(v1 * q1 < 0.0);
	bool p1 = !(v1 * q2 > 0.0);
	bool m2 = !(v2 * q2 < 0.0);
	bool p2 = !(v2 * q3 > 0.0);
	bool m3 = !(v3 * q3 < 0.0);
	bool p3 = !(v3 * q1 > 0.0);

	if (m1 & p3)
	{
		// Origin is in Voronoi region for vertex q1.

		*p = q1;
		return (0x0001);
	}

	if (m2 & p1)
	{
		// Origin is in Voronoi region for vertex q2.

		*p = q2;
		return (0x0002);
	}

	if (m3 & p2)
	{
		// Origin is in Voronoi region for vertex q3.

		*p = q3;
		return (0x0004);
	}

	DVector3D n = v3 % v1;
	bool d1 = !((n % v1) * q1 < 0.0);
	bool d2 = !((n % v2) * q2 < 0.0);
	bool d3 = !((n % v3) * q3 < 0.0);

	if ((d1) && (!m1) && (!p1))
	{
		// Origin is in Voronoi region for edge v1.

		*p = q1 - ProjectOnto(q1, v1) / SquaredMag(v1);
		return (0x0003);
	}

	if ((d2) && (!m2) && (!p2))
	{
		// Origin is in Voronoi region for edge v2.

		*p = q2 - ProjectOnto(q2, v2) / SquaredMag(v2);
		return (0x0006);
	}

	if ((d3) && (!m3) && (!p3))
	{
		// Origin is in Voronoi region for edge v3.

		*p = q3 - ProjectOnto(q3, v3) / SquaredMag(v3);
		return (0x0005);
	}

	// Origin is in Voronoi region for triangle interior.

	*p = ProjectOnto(q1, n) / SquaredMag(n);
	return (0x0007);
}

unsigned_int32 ZoneObject::CalculateThreeSimplexMinimum(const Point3D *simplex, Point3D *p)
{
	const Point3D& q1 = simplex[0];
	const Point3D& q2 = simplex[1];
	const Point3D& q3 = simplex[2];
	const Point3D& q4 = simplex[3];
	DVector3D v12 = q2 - q1;
	DVector3D v23 = q3 - q2;
	DVector3D v31 = q1 - q3;
	DVector3D v14 = q4 - q1;
	DVector3D v24 = q4 - q2;
	DVector3D v34 = q4 - q3;

	bool m12 = !(v12 * q1 < 0.0);
	bool p12 = !(v12 * q2 > 0.0);
	bool m23 = !(v23 * q2 < 0.0);
	bool p23 = !(v23 * q3 > 0.0);
	bool m31 = !(v31 * q3 < 0.0);
	bool p31 = !(v31 * q1 > 0.0);
	bool m14 = !(v14 * q1 < 0.0);
	bool p14 = !(v14 * q4 > 0.0);
	bool m24 = !(v24 * q2 < 0.0);
	bool p24 = !(v24 * q4 > 0.0);
	bool m34 = !(v34 * q3 < 0.0);
	bool p34 = !(v34 * q4 > 0.0);

	if (m12 & p31 & m14)
	{
		// Origin is in Voronoi region for vertex q1.

		*p = q1;
		return (0x0001);
	}

	if (m23 & p12 & m24)
	{
		// Origin is in Voronoi region for vertex q2.

		*p = q2;
		return (0x0002);
	}

	if (m31 & p23 & m34)
	{
		// Origin is in Voronoi region for vertex q3.

		*p = q3;
		return (0x0004);
	}

	if (p14 & p24 & p34)
	{
		// Origin is in Voronoi region for vertex q4.

		*p = q4;
		return (0x0008);
	}

	DVector3D n1 = v12 % v31;
	DVector3D n2 = v12 % v14;
	DVector3D n3 = v23 % v24;
	DVector3D n4 = v31 % v34;

	bool d11 = !((v12 % n1) * q1 < 0.0);
	bool d12 = !((v23 % n1) * q2 < 0.0);
	bool d13 = !((v31 % n1) * q3 < 0.0);
	bool d21 = !((n2 % v12) * q1 < 0.0);
	bool d22 = !((n2 % v24) * q2 < 0.0);
	bool d24 = !((v14 % n2) * q4 < 0.0);
	bool d32 = !((n3 % v23) * q2 < 0.0);
	bool d33 = !((n3 % v34) * q3 < 0.0);
	bool d34 = !((v24 % n3) * q4 < 0.0);
	bool d43 = !((n4 % v31) * q3 < 0.0);
	bool d41 = !((n4 % v14) * q1 < 0.0);
	bool d44 = !((v34 % n4) * q4 < 0.0);

	if ((!m12) && (!p12) && (d11) && (d21))
	{
		// Origin is in Voronoi region for edge v12.

		*p = q1 - ProjectOnto(q1, v12) / SquaredMag(v12);
		return (0x0003);
	}

	if ((!m23) && (!p23) && (d12) && (d32))
	{
		// Origin is in Voronoi region for edge v23.

		*p = q2 - ProjectOnto(q2, v23) / SquaredMag(v23);
		return (0x0006);
	}

	if ((!m31) && (!p31) && (d13) && (d43))
	{
		// Origin is in Voronoi region for edge v31.

		*p = q3 - ProjectOnto(q3, v31) / SquaredMag(v31);
		return (0x0005);
	}

	if ((!m14) && (!p14) && (d24) && (d41))
	{
		// Origin is in Voronoi region for edge v14.

		*p = q1 - ProjectOnto(q1, v14) / SquaredMag(v14);
		return (0x0009);
	}

	if ((!m24) && (!p24) && (d22) && (d34))
	{
		// Origin is in Voronoi region for edge v24.

		*p = q2 - ProjectOnto(q2, v24) / SquaredMag(v24);
		return (0x000A);
	}

	if ((!m34) && (!p34) && (d33) && (d44))
	{
		// Origin is in Voronoi region for edge v34.

		*p = q3 - ProjectOnto(q3, v34) / SquaredMag(v34);
		return (0x000C);
	}

	bool f1 = !((n1 * q1) * (n1 * v14) < 0.0);
	bool f2 = !((n2 * q2) * (n2 * v31) > 0.0);
	bool f3 = !((n3 * q3) * (n3 * v12) > 0.0);
	bool f4 = !((n4 * q4) * (n4 * v23) > 0.0);

	if ((f1) && (!d11) && (!d12) && (!d13))
	{
		// Origin is in Voronoi region for face f1.

		*p = ProjectOnto(q1, n1) / SquaredMag(n1);
		return (0x0007);
	}

	if ((f2) && (!d21) && (!d22) && (!d24))
	{
		// Origin is in Voronoi region for face f2.

		*p = ProjectOnto(q2, n2) / SquaredMag(n2);
		return (0x000B);
	}

	if ((f3) && (!d32) && (!d33) && (!d34))
	{
		// Origin is in Voronoi region for face f3.

		*p = ProjectOnto(q3, n3) / SquaredMag(n3);
		return (0x000E);
	}

	if ((f4) && (!d43) && (!d41) && (!d44))
	{
		// Origin is in Voronoi region for face f4.

		*p = ProjectOnto(q4, n4) / SquaredMag(n4);
		return (0x000D);
	}

	// Origin is in tetrahedron interior.

	p->Set(0.0F, 0.0F, 0.0F);
	return (0x000F);
}

Point3D ZoneObject::GetInitialSupportPoint(void) const
{
	return (Zero3D);
}

bool ZoneObject::IntersectsBoundingBox(const BoundingBox *box) const
{
	Point3D		alphaVertex[2][4];
	Point3D		betaVertex[2][4];
	Point3D		deltaVertex[2][4];

	const Point3D& boxCenter = box->GetCenter();
	if (InteriorPoint(boxCenter))
	{
		return (true);
	}

	const Vector3D *boxAxis = box->GetAxes();

	int32 simplexVertexCount = 1;
	alphaVertex[0][0] = GetInitialSupportPoint();
	betaVertex[0][0] = boxCenter + boxAxis[0] + boxAxis[1] + boxAxis[2];
	deltaVertex[0][0] = alphaVertex[0][0] - betaVertex[0][0];
	Point3D prevMinPoint = deltaVertex[0][0];

	for (unsigned_int32 simplexParity = 0;;)
	{
		Point3D		minPoint;

		const Vector3D& direction = prevMinPoint;

		CalculateSupportPoint(-direction, &alphaVertex[simplexParity][simplexVertexCount]);
		betaVertex[simplexParity][simplexVertexCount] = boxCenter + boxAxis[0] * Fsgn(boxAxis[0] * direction) + boxAxis[1] * Fsgn(boxAxis[1] * direction) + boxAxis[2] * Fsgn(boxAxis[2] * direction);
		deltaVertex[simplexParity][simplexVertexCount] = alphaVertex[simplexParity][simplexVertexCount] - betaVertex[simplexParity][simplexVertexCount];

		for (machine a = 0; a < simplexVertexCount; a++)
		{
			if (SquaredMag(deltaVertex[simplexParity][a] - deltaVertex[simplexParity][simplexVertexCount]) < 1.0e-5F)
			{
				simplexVertexCount--;
				break;
			}
		}

		unsigned_int32 mask = CalculateSimplexMinimum(simplexVertexCount, deltaVertex[simplexParity], &minPoint);
		if (mask == 0x0F)
		{
			return (true);
		}

		if (!(SquaredMag(minPoint) < SquaredMag(prevMinPoint) - 1.0e-4F))
		{
			break;
		}

		simplexVertexCount = 0;
		for (machine index = 0;; index++)
		{
			if (mask & 1)
			{
				alphaVertex[simplexParity ^ 1][simplexVertexCount] = alphaVertex[simplexParity][index];
				betaVertex[simplexParity ^ 1][simplexVertexCount] = betaVertex[simplexParity][index];
				deltaVertex[simplexParity ^ 1][simplexVertexCount] = deltaVertex[simplexParity][index];
				simplexVertexCount++;
			}

			if ((mask >>= 1) == 0)
			{
				break;
			}
		}

		simplexParity ^= 1;
		prevMinPoint = minPoint;
	}

	return (false);
}


InfiniteZoneObject::InfiniteZoneObject() : ZoneObject(kZoneInfinite)
{
	zoneBox.min.Set(0.0F, 0.0F, 0.0F);
	zoneBox.max.Set(1.0F, 1.0F, 1.0F);
}

InfiniteZoneObject::InfiniteZoneObject(const Box3D& box) : ZoneObject(kZoneInfinite)
{
	zoneBox = box;
}

InfiniteZoneObject::~InfiniteZoneObject()
{
}

void InfiniteZoneObject::Pack(Packer& data, unsigned_int32 packFlags) const
{
	ZoneObject::Pack(data, packFlags);

	data << ChunkHeader('ZBOX', sizeof(Point3D) * 2);
	data << zoneBox.min;
	data << zoneBox.max;

	data << TerminatorChunk;
}

void InfiniteZoneObject::Unpack(Unpacker& data, unsigned_int32 unpackFlags)
{
	ZoneObject::Unpack(data, unpackFlags);
	UnpackChunkList<InfiniteZoneObject>(data, unpackFlags);
}

bool InfiniteZoneObject::UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags)
{
	switch (chunkHeader->chunkType)
	{
		case 'ZBOX':

			data >> zoneBox.min;
			data >> zoneBox.max;
			return (true);
	}

	return (false);
}

int32 InfiniteZoneObject::GetObjectSize(float *size) const
{
	size[0] = zoneBox.min.x;
	size[1] = zoneBox.min.y;
	size[2] = zoneBox.min.z;
	size[3] = zoneBox.max.x;
	size[4] = zoneBox.max.y;
	size[5] = zoneBox.max.z;
	return (6);
}

void InfiniteZoneObject::SetObjectSize(const float *size)
{
	zoneBox.min.Set(size[0], size[1], size[2]);
	zoneBox.max.Set(size[3], size[4], size[5]);
}

bool InfiniteZoneObject::ExteriorSphere(const Point3D& center, float radius) const
{
	return (false);
}

bool InfiniteZoneObject::InteriorSphere(const Point3D& center, float radius) const
{
	return (true);
}

bool InfiniteZoneObject::ExteriorSweptSphere(const Point3D& p1, const Point3D& p2, float radius) const
{
	return (false);
}

bool InfiniteZoneObject::InteriorSweptSphere(const Point3D& p1, const Point3D& p2, float radius) const
{
	return (true);
}

bool InfiniteZoneObject::InteriorPoint(const Point3D& position) const
{
	return (true);
}

bool InfiniteZoneObject::CalculateSupportPoint(const Vector3D& direction, Point3D *point) const
{
	return (false);
}

int32 InfiniteZoneObject::ClipInteriorEdges(int32 edgeCount, const Point3D (*edge)[2], Point3D (*restrict clippedEdge)[2]) const
{
	return (0);
}


BoxZoneObject::BoxZoneObject() : ZoneObject(kZoneBox)
{
}

BoxZoneObject::BoxZoneObject(const Vector3D& size) : ZoneObject(kZoneBox)
{
	boxSize = size;
}

BoxZoneObject::~BoxZoneObject()
{
}

void BoxZoneObject::Pack(Packer& data, unsigned_int32 packFlags) const
{
	ZoneObject::Pack(data, packFlags);

	data << ChunkHeader('SIZE', sizeof(Vector3D));
	data << boxSize;

	data << TerminatorChunk;
}

void BoxZoneObject::Unpack(Unpacker& data, unsigned_int32 unpackFlags)
{
	ZoneObject::Unpack(data, unpackFlags);
	UnpackChunkList<BoxZoneObject>(data, unpackFlags);
}

bool BoxZoneObject::UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags)
{
	switch (chunkHeader->chunkType)
	{
		case 'SIZE':

			data >> boxSize;
			return (true);
	}

	return (false);
}

int32 BoxZoneObject::GetObjectSize(float *size) const
{
	size[0] = boxSize.x;
	size[1] = boxSize.y;
	size[2] = boxSize.z;
	return (3);
}

void BoxZoneObject::SetObjectSize(const float *size)
{
	boxSize.Set(size[0], size[1], size[2]);
}

bool BoxZoneObject::ExteriorSphere(const Point3D& center, float radius) const
{
	if ((center.x < -radius) || (center.x > boxSize.x + radius))
	{
		return (true);
	}

	if ((center.y < -radius) || (center.y > boxSize.y + radius))
	{
		return (true);
	}

	if ((center.z < -radius) || (center.z > boxSize.z + radius))
	{
		return (true);
	}

	return (false);
}

bool BoxZoneObject::InteriorSphere(const Point3D& center, float radius) const
{
	if ((center.x < radius) || (center.x > boxSize.x - radius))
	{
		return (false);
	}

	if ((center.y < radius) || (center.y > boxSize.y - radius))
	{
		return (false);
	}

	if ((center.z < radius) || (center.z > boxSize.z - radius))
	{
		return (false);
	}

	return (true);
}

bool BoxZoneObject::ExteriorSweptSphere(const Point3D& p1, const Point3D& p2, float radius) const
{
	if ((!BoxZoneObject::ExteriorSphere(p1, radius)) || (!BoxZoneObject::ExteriorSphere(p2, radius)))
	{
		return (false);
	}

	Point3D q1 = p1;
	Point3D q2 = p2;

	float d1 = q1.z + radius;
	float d2 = q2.z + radius;

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

	float sz = boxSize.z + radius;
	d1 = sz - q1.z;
	d2 = sz - q2.z;

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

	d1 = q1.y + radius;
	d2 = q2.y + radius;

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

	float sy = boxSize.y + radius;
	d1 = sy - q1.y;
	d2 = sy - q2.y;

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

	d1 = q1.x + radius;
	d2 = q2.x + radius;

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

	float sx = boxSize.x + radius;
	d1 = sx - q1.x;
	d2 = sx - q2.x;

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

	return (false);
}

bool BoxZoneObject::InteriorSweptSphere(const Point3D& p1, const Point3D& p2, float radius) const
{
	return ((BoxZoneObject::InteriorSphere(p1, radius)) && (BoxZoneObject::InteriorSphere(p2, radius)));
}

bool BoxZoneObject::InteriorPoint(const Point3D& position) const
{
	if ((position.x < 0.0F) || (position.x > boxSize.x))
	{
		return (false);
	}

	if ((position.y < 0.0F) || (position.y > boxSize.y))
	{
		return (false);
	}

	if ((position.z < 0.0F) || (position.z > boxSize.z))
	{
		return (false);
	}

	return (true);
}

bool BoxZoneObject::CalculateSupportPoint(const Vector3D& direction, Point3D *support) const
{
	support->x = (direction.x > 0.0F) ? boxSize.x : 0.0F;
	support->y = (direction.y > 0.0F) ? boxSize.y : 0.0F;
	support->z = (direction.z > 0.0F) ? boxSize.z : 0.0F;

	return (true);
}

int32 BoxZoneObject::ClipInteriorEdges(int32 edgeCount, const Point3D (*edge)[2], Point3D (*restrict clippedEdge)[2]) const
{
	Point3D (*clippedStart)[2] = clippedEdge;

	for (machine i = 0; i < edgeCount; i++)
	{
		Point3D p1 = edge[i][0];
		Point3D p2 = edge[i][1];
		bool clip1 = false;
		bool clip2 = false;

		// Clip against x = 0.

		float x1 = p1.x;
		float x2 = p2.x;
		int8 loc1 = (x1 > kZoneClipEpsilon) ? kPolygonInterior : ((x1 < -kZoneClipEpsilon) ? kPolygonExterior : kPolygonBoundary);
		int8 loc2 = (x2 > kZoneClipEpsilon) ? kPolygonInterior : ((x2 < -kZoneClipEpsilon) ? kPolygonExterior : kPolygonBoundary);
		int32 minloc = Min(loc1, loc2);

		if (Max(loc1, loc2) != kPolygonInterior)
		{
			if (minloc == kPolygonExterior)
			{
				(*clippedEdge)[0] = p1;
				(*clippedEdge)[1] = p2;
				clippedEdge++;
			}

			continue;
		}

		if (minloc == kPolygonExterior)
		{
			float t = x1 / (x1 - x2);
			float y = p1.y + t * (p2.y - p1.y);
			float z = p1.z + t * (p2.z - p1.z);

			if (loc1 == kPolygonExterior)
			{
				p1.Set(0.0F, y, z);
				clip1 = true;
			}
			else
			{
				p2.Set(0.0F, y, z);
				clip2 = true;
			}
		}

		// Clip against x = boxSize.x.

		x1 = boxSize.x - p1.x;
		x2 = boxSize.x - p2.x;
		loc1 = (x1 > kZoneClipEpsilon) ? kPolygonInterior : ((x1 < -kZoneClipEpsilon) ? kPolygonExterior : kPolygonBoundary);
		loc2 = (x2 > kZoneClipEpsilon) ? kPolygonInterior : ((x2 < -kZoneClipEpsilon) ? kPolygonExterior : kPolygonBoundary);
		minloc = Min(loc1, loc2);

		if (Max(loc1, loc2) != kPolygonInterior)
		{
			if (minloc == kPolygonExterior)
			{
				(*clippedEdge)[0] = p1;
				(*clippedEdge)[1] = p2;
				clippedEdge++;
			}

			continue;
		}

		if (minloc == kPolygonExterior)
		{
			float t = x1 / (x1 - x2);
			float y = p1.y + t * (p2.y - p1.y);
			float z = p1.z + t * (p2.z - p1.z);

			if (loc1 == kPolygonExterior)
			{
				p1.Set(boxSize.x, y, z);
				clip1 = true;
			}
			else
			{
				p2.Set(boxSize.x, y, z);
				clip2 = true;
			}
		}

		// Clip against y = 0.

		float y1 = p1.y;
		float y2 = p2.y;
		loc1 = (y1 > kZoneClipEpsilon) ? kPolygonInterior : ((y1 < -kZoneClipEpsilon) ? kPolygonExterior : kPolygonBoundary);
		loc2 = (y2 > kZoneClipEpsilon) ? kPolygonInterior : ((y2 < -kZoneClipEpsilon) ? kPolygonExterior : kPolygonBoundary);
		minloc = Min(loc1, loc2);

		if (Max(loc1, loc2) != kPolygonInterior)
		{
			if (minloc == kPolygonExterior)
			{
				(*clippedEdge)[0] = p1;
				(*clippedEdge)[1] = p2;
				clippedEdge++;
			}

			continue;
		}

		if (minloc == kPolygonExterior)
		{
			float t = y1 / (y1 - y2);
			float x = p1.x + t * (p2.x - p1.x);
			float z = p1.z + t * (p2.z - p1.z);

			if (loc1 == kPolygonExterior)
			{
				p1.Set(x, 0.0F, z);
				clip1 = true;
			}
			else
			{
				p2.Set(x, 0.0F, z);
				clip2 = true;
			}
		}

		// Clip against y = boxSize.y.

		y1 = boxSize.y - p1.y;
		y2 = boxSize.y - p2.y;
		loc1 = (y1 > kZoneClipEpsilon) ? kPolygonInterior : ((y1 < -kZoneClipEpsilon) ? kPolygonExterior : kPolygonBoundary);
		loc2 = (y2 > kZoneClipEpsilon) ? kPolygonInterior : ((y2 < -kZoneClipEpsilon) ? kPolygonExterior : kPolygonBoundary);
		minloc = Min(loc1, loc2);

		if (Max(loc1, loc2) != kPolygonInterior)
		{
			if (minloc == kPolygonExterior)
			{
				(*clippedEdge)[0] = p1;
				(*clippedEdge)[1] = p2;
				clippedEdge++;
			}

			continue;
		}

		if (minloc == kPolygonExterior)
		{
			float t = y1 / (y1 - y2);
			float x = p1.x + t * (p2.x - p1.x);
			float z = p1.z + t * (p2.z - p1.z);

			if (loc1 == kPolygonExterior)
			{
				p1.Set(x, boxSize.y, z);
				clip1 = true;
			}
			else
			{
				p2.Set(x, boxSize.y, z);
				clip2 = true;
			}
		}

		// Clip against z = 0.

		float z1 = p1.z;
		float z2 = p2.z;
		loc1 = (z1 > kZoneClipEpsilon) ? kPolygonInterior : ((z1 < -kZoneClipEpsilon) ? kPolygonExterior : kPolygonBoundary);
		loc2 = (z2 > kZoneClipEpsilon) ? kPolygonInterior : ((z2 < -kZoneClipEpsilon) ? kPolygonExterior : kPolygonBoundary);
		minloc = Min(loc1, loc2);

		if (Max(loc1, loc2) != kPolygonInterior)
		{
			if (minloc == kPolygonExterior)
			{
				(*clippedEdge)[0] = p1;
				(*clippedEdge)[1] = p2;
				clippedEdge++;
			}

			continue;
		}

		if (minloc == kPolygonExterior)
		{
			float t = z1 / (z1 - z2);
			float x = p1.x + t * (p2.x - p1.x);
			float y = p1.y + t * (p2.y - p1.y);

			if (loc1 == kPolygonExterior)
			{
				p1.Set(x, y, 0.0F);
				clip1 = true;
			}
			else
			{
				p2.Set(x, y, 0.0F);
				clip2 = true;
			}
		}

		// Clip against z = boxSize.z.

		z1 = boxSize.z - p1.z;
		z2 = boxSize.z - p2.z;
		loc1 = (z1 > kZoneClipEpsilon) ? kPolygonInterior : ((z1 < -kZoneClipEpsilon) ? kPolygonExterior : kPolygonBoundary);
		loc2 = (z2 > kZoneClipEpsilon) ? kPolygonInterior : ((z2 < -kZoneClipEpsilon) ? kPolygonExterior : kPolygonBoundary);
		minloc = Min(loc1, loc2);

		if (Max(loc1, loc2) != kPolygonInterior)
		{
			if (minloc == kPolygonExterior)
			{
				(*clippedEdge)[0] = p1;
				(*clippedEdge)[1] = p2;
				clippedEdge++;
			}

			continue;
		}

		if (minloc == kPolygonExterior)
		{
			float t = z1 / (z1 - z2);
			float x = p1.x + t * (p2.x - p1.x);
			float y = p1.y + t * (p2.y - p1.y);

			if (loc1 == kPolygonExterior)
			{
				p1.Set(x, y, boxSize.z);
				clip1 = true;
			}
			else
			{
				p2.Set(x, y, boxSize.z);
				clip2 = true;
			}
		}

		if (clip1)
		{
			(*clippedEdge)[0] = p1;
			(*clippedEdge)[1] = edge[i][0];
			clippedEdge++;
		}

		if (clip2)
		{
			(*clippedEdge)[0] = p2;
			(*clippedEdge)[1] = edge[i][1];
			clippedEdge++;
		}
	}

	return ((int32) (clippedEdge - clippedStart));
}


CylinderZoneObject::CylinderZoneObject() : ZoneObject(kZoneCylinder)
{
}

CylinderZoneObject::CylinderZoneObject(const Vector2D& size, float height) : ZoneObject(kZoneCylinder)
{
	cylinderSize = size;
	cylinderHeight = height;
	ratioXY = size.x / size.y;
}

CylinderZoneObject::~CylinderZoneObject()
{
}

void CylinderZoneObject::Pack(Packer& data, unsigned_int32 packFlags) const
{
	ZoneObject::Pack(data, packFlags);

	data << ChunkHeader('SIZE', sizeof(Vector2D) + 4);
	data << cylinderSize;
	data << cylinderHeight;

	data << TerminatorChunk;
}

void CylinderZoneObject::Unpack(Unpacker& data, unsigned_int32 unpackFlags)
{
	ZoneObject::Unpack(data, unpackFlags);
	UnpackChunkList<CylinderZoneObject>(data, unpackFlags);
}

bool CylinderZoneObject::UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags)
{
	switch (chunkHeader->chunkType)
	{
		case 'SIZE':

			data >> cylinderSize;
			data >> cylinderHeight;
			ratioXY = cylinderSize.x / cylinderSize.y;
			return (true);
	}

	return (false);
}

int32 CylinderZoneObject::GetObjectSize(float *size) const
{
	size[0] = cylinderSize.x;
	size[1] = cylinderSize.y;
	size[2] = cylinderHeight;
	return (3);
}

void CylinderZoneObject::SetObjectSize(const float *size)
{
	cylinderSize.Set(size[0], size[1]);
	cylinderHeight = size[2];
	ratioXY = cylinderSize.x / cylinderSize.y;
}

bool CylinderZoneObject::ExteriorSphere(const Point3D& center, float radius) const
{
	if ((center.z < -radius) || (center.z > cylinderHeight + radius))
	{
		return (true);
	}

	float rx = cylinderSize.x + radius;
	float ry = cylinderSize.y + radius;
	float m = rx / ry;

	return (center.x * center.x + m * m * center.y * center.y > rx * rx);
}

bool CylinderZoneObject::InteriorSphere(const Point3D& center, float radius) const
{
	if ((center.z < radius) || (center.z > cylinderHeight - radius))
	{
		return (false);
	}

	float rx = cylinderSize.x - radius;
	if (rx <= 0.0F)
	{
		return (false);
	}

	float ry = cylinderSize.y - radius;
	if (ry <= 0.0F)
	{
		return (false);
	}

	float m = rx / ry;
	if (center.x * center.x + m * m * center.y * center.y > rx * rx)
	{
		return (false);
	}

	return (true);
}

bool CylinderZoneObject::ExteriorSweptSphere(const Point3D& p1, const Point3D& p2, float radius) const
{
	if ((!CylinderZoneObject::ExteriorSphere(p1, radius)) || (!CylinderZoneObject::ExteriorSphere(p2, radius)))
	{
		return (false);
	}

	float sx = p1.x;
	float sy = p1.y;
	float dx = p2.x - sx;
	float dy = p2.y - sy;

	float rx = cylinderSize.x + radius;
	float ry = cylinderSize.y + radius;
	float m2 = rx * rx / (ry * ry);

	float a = dx * dx + m2 * dy * dy;
	if (Fabs(a) > K::min_float)
	{
		float b = -(sx * dx + m2 * sy * dy);
		float d = b * b - a * (sx * sx + m2 * sy * sy - rx * rx);
		if (d < K::min_float)
		{
			return (true);
		}

		a = 1.0F / a;
		d = Sqrt(d);
		float t1 = (b - d) * a;
		float t2 = (b + d) * a;

		if ((t1 < 0.0F) && (t2 < 0.0F))
		{
			return (true);
		}

		if ((t1 > 1.0F) && (t2 > 1.0F))
		{
			return (true);
		}

		float sz = p1.z;
		float dz = p2.z - sz;

		float z1 = sz + dz * t1;
		float z2 = sz + dz * t2;
		if ((z1 < -radius) && (z2 < -radius))
		{
			return (true);
		}

		float height = cylinderHeight + radius;
		if ((z1 > height) && (z2 > height))
		{
			return (true);
		}

		return (false);
	}

	float z1 = p1.z;
	float z2 = p2.z;
	if ((z1 < -radius) && (z2 < -radius))
	{
		return (true);
	}

	float height = cylinderHeight + radius;
	if ((z1 > height) && (z2 > height))
	{
		return (true);
	}

	return (sx * sx + m2 * sy * sy > rx * rx);
}

bool CylinderZoneObject::InteriorSweptSphere(const Point3D& p1, const Point3D& p2, float radius) const
{
	return ((CylinderZoneObject::InteriorSphere(p1, radius)) && (CylinderZoneObject::InteriorSphere(p2, radius)));
}

bool CylinderZoneObject::InteriorPoint(const Point3D& position) const
{
	if ((position.z < 0.0F) || (position.z > cylinderHeight))
	{
		return (false);
	}

	float m = ratioXY;
	float rx = cylinderSize.x;
	if (position.x * position.x + m * m * position.y * position.y > rx * rx)
	{
		return (false);
	}

	return (true);
}

bool CylinderZoneObject::CalculateSupportPoint(const Vector3D& direction, Point3D *support) const
{
	const Vector2D& size = cylinderSize;

	float x = size.x * size.x * direction.x;
	float y = size.y * size.y * direction.y;
	float z = (direction.z > 0.0F) ? cylinderHeight : 0.0F;

	float r = x * direction.x + y * direction.y;
	if (r > K::min_float)
	{
		r = InverseSqrt(r);
		support->Set(x * r, y * r, z);
	}
	else
	{
		support->Set(0.0F, 0.0F, z);
	}

	return (true);
}

int32 CylinderZoneObject::ClipInteriorEdges(int32 edgeCount, const Point3D (*edge)[2], Point3D (*restrict clippedEdge)[2]) const
{
	Point3D (*clippedStart)[2] = clippedEdge;

	for (machine i = 0; i < edgeCount; i++)
	{
		Point3D p1 = edge[i][0];
		Point3D p2 = edge[i][1];
		bool clip1 = false;
		bool clip2 = false;

		// Clip against z = 0.

		float z1 = p1.z;
		float z2 = p2.z;
		int8 loc1 = (z1 > kZoneClipEpsilon) ? kPolygonInterior : ((z1 < -kZoneClipEpsilon) ? kPolygonExterior : kPolygonBoundary);
		int8 loc2 = (z2 > kZoneClipEpsilon) ? kPolygonInterior : ((z2 < -kZoneClipEpsilon) ? kPolygonExterior : kPolygonBoundary);
		int32 minloc = Min(loc1, loc2);

		if (Max(loc1, loc2) != kPolygonInterior)
		{
			if (minloc == kPolygonExterior)
			{
				(*clippedEdge)[0] = p1;
				(*clippedEdge)[1] = p2;
				clippedEdge++;
			}

			continue;
		}

		if (minloc == kPolygonExterior)
		{
			float t = z1 / (z1 - z2);
			float x = p1.x + t * (p2.x - p1.x);
			float y = p1.y + t * (p2.y - p1.y);

			if (loc1 == kPolygonExterior)
			{
				p1.Set(x, y, 0.0F);
				clip1 = true;
			}
			else
			{
				p2.Set(x, y, 0.0F);
				clip2 = true;
			}
		}

		// Clip against z = cylinderHeight.

		z1 = cylinderHeight - p1.z;
		z2 = cylinderHeight - p2.z;
		loc1 = (z1 > kZoneClipEpsilon) ? kPolygonInterior : ((z1 < -kZoneClipEpsilon) ? kPolygonExterior : kPolygonBoundary);
		loc2 = (z2 > kZoneClipEpsilon) ? kPolygonInterior : ((z2 < -kZoneClipEpsilon) ? kPolygonExterior : kPolygonBoundary);
		minloc = Min(loc1, loc2);

		if (Max(loc1, loc2) != kPolygonInterior)
		{
			if (minloc == kPolygonExterior)
			{
				(*clippedEdge)[0] = p1;
				(*clippedEdge)[1] = p2;
				clippedEdge++;
			}

			continue;
		}

		if (minloc == kPolygonExterior)
		{
			float t = z1 / (z1 - z2);
			float x = p1.x + t * (p2.x - p1.x);
			float y = p1.y + t * (p2.y - p1.y);

			if (loc1 == kPolygonExterior)
			{
				p1.Set(x, y, cylinderHeight);
				clip1 = true;
			}
			else
			{
				p2.Set(x, y, cylinderHeight);
				clip2 = true;
			}
		}

		// Clip against lateral surface.

		float m2 = ratioXY * ratioXY;
		float rx2 = cylinderSize.x * cylinderSize.x;

		loc1 = (p1.x * p1.x + m2 * p1.y * p1.y < rx2) ? kPolygonInterior : kPolygonExterior;
		loc2 = (p2.x * p2.x + m2 * p2.y * p2.y < rx2) ? kPolygonInterior : kPolygonExterior;

		if (Max(loc1, loc2) == kPolygonExterior)
		{
			Vector3D dp = p2 - p1;

			float a = dp.x * dp.x + dp.y * dp.y * m2;
			float b = p1.x * dp.x + p1.y * dp.y * m2;
			float c = p1.x * p1.x + p1.y * p1.y * m2 - rx2;
			float d = b * b - a * c;

			if (d > K::min_float)
			{
				a = -1.0F / a;

				if (loc2 == kPolygonExterior)
				{
					float t = (b - Sqrt(d)) * a;
					p2 = p1 + dp * t;
					clip2 = true;
				}

				if (loc1 == kPolygonExterior)
				{
					float t = (b + Sqrt(d)) * a;
					p1 = p1 + dp * t;
					clip1 = true;
				}
			}
		}

		if (clip1)
		{
			(*clippedEdge)[0] = p1;
			(*clippedEdge)[1] = edge[i][0];
			clippedEdge++;
		}

		if (clip2)
		{
			(*clippedEdge)[0] = p2;
			(*clippedEdge)[1] = edge[i][1];
			clippedEdge++;
		}
	}

	return ((int32) (clippedEdge - clippedStart));
}


PolygonZoneObject::PolygonZoneObject() : ZoneObject(kZonePolygon)
{
}

PolygonZoneObject::PolygonZoneObject(const Vector2D& size, float height) : ZoneObject(kZonePolygon)
{
	SetPolygonSize(size, height);
}

PolygonZoneObject::~PolygonZoneObject()
{
}

void PolygonZoneObject::Pack(Packer& data, unsigned_int32 packFlags) const
{
	ZoneObject::Pack(data, packFlags);

	data << ChunkHeader('HITE', 4);
	data << polygonHeight;

	data << ChunkHeader('VERT', 4 + vertexCount * sizeof(Point3D));
	data << vertexCount;
	data.WriteArray(vertexCount, polygonVertex);

	data << TerminatorChunk;
}

void PolygonZoneObject::Unpack(Unpacker& data, unsigned_int32 unpackFlags)
{
	ZoneObject::Unpack(data, unpackFlags);
	UnpackChunkList<PolygonZoneObject>(data, unpackFlags);
}

bool PolygonZoneObject::UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags)
{
	switch (chunkHeader->chunkType)
	{
		case 'HITE':

			data >> polygonHeight;
			return (true);

		case 'VERT':

			data >> vertexCount;
			data.ReadArray(vertexCount, polygonVertex);
			return (true);
	}

	return (false);
}

void PolygonZoneObject::SetPolygonSize(const Vector2D& size, float height)
{
	polygonHeight = height;
	vertexCount = 4;

	float x = size.x;
	float y = size.y;

	polygonVertex[0].Set(0.0F, 0.0F, 0.0F);
	polygonVertex[1].Set(x, 0.0F, 0.0F);
	polygonVertex[2].Set(x, y, 0.0F);
	polygonVertex[3].Set(0.0F, y, 0.0F);
}

int32 PolygonZoneObject::GetObjectSize(float *size) const
{
	size[0] = polygonHeight;
	return (1);
}

void PolygonZoneObject::SetObjectSize(const float *size)
{
	polygonHeight = size[0];
}

bool PolygonZoneObject::ExteriorSphere(const Point3D& center, float radius) const
{
	if ((center.z < -radius) || (center.z > polygonHeight + radius))
	{
		return (true);
	}

	int32 count = vertexCount;
	const Point3D *v1 = &polygonVertex[count - 1];
	for (machine a = 0; a < count; a++)
	{
		const Point3D *v2 = &polygonVertex[a];

		float nx = v2->y - v1->y;
		float ny = v1->x - v2->x;
		float m = InverseSqrt(nx * nx + ny * ny);
		nx *= m;
		ny *= m;

		if ((center.x - v1->x) * nx + (center.y - v1->y) * ny > radius)
		{
			return (true);
		}

		v1 = v2;
	}

	return (false);
}

bool PolygonZoneObject::InteriorSphere(const Point3D& center, float radius) const
{
	if ((center.z < radius) || (center.z > polygonHeight - radius))
	{
		return (false);
	}

	int32 count = vertexCount;
	const Point3D *v1 = &polygonVertex[count - 1];
	for (machine a = 0; a < count; a++)
	{
		const Point3D *v2 = &polygonVertex[a];

		float nx = v1->y - v2->y;
		float ny = v2->x - v1->x;
		float m = InverseSqrt(nx * nx + ny * ny);
		nx *= m;
		ny *= m;

		if ((center.x - v1->x) * nx + (center.y - v1->y) * ny < radius)
		{
			return (false);
		}

		v1 = v2;
	}

	return (true);
}

bool PolygonZoneObject::ExteriorSweptSphere(const Point3D& p1, const Point3D& p2, float radius) const
{
	if ((!PolygonZoneObject::ExteriorSphere(p1, radius)) || (!PolygonZoneObject::ExteriorSphere(p2, radius)))
	{
		return (false);
	}

	Point3D q1 = p1;
	Point3D q2 = p2;

	float d1 = q1.z + radius;
	float d2 = q2.z + radius;

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

	float sz = polygonHeight + radius;
	d1 = sz - q1.z;
	d2 = sz - q2.z;

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

	int32 count = vertexCount;
	const Point3D *v1 = &polygonVertex[count - 1];
	for (machine a = 0; a < count; a++)
	{
		const Point3D *v2 = &polygonVertex[a];

		float nx = v1->y - v2->y;
		float ny = v2->x - v1->x;
		float m = InverseSqrt(nx * nx + ny * ny);
		nx *= m;
		ny *= m;

		float w = radius - nx * v1->x - ny * v1->y;
		d1 = q1.x * nx + q1.y * ny + w;
		d2 = q2.x * nx + q2.y * ny + w;

		if (d1 < 0.0F)
		{
			if (d2 < 0.0F)
			{
				return (true);
			}

			float dq = d1 - d2;
			if (Fabs(dq) > K::min_float)
			{
				float t = d1 / dq;
				q1 = q1 + (q2 - q1) * t;
			}
		}
		else if (d2 < 0.0F)
		{
			float dq = d1 - d2;
			if (Fabs(dq) > K::min_float)
			{
				float t = d1 / dq;
				q2 = q1 + (q2 - q1) * t;
			}
		}

		v1 = v2;
	}

	return (false);
}

bool PolygonZoneObject::InteriorSweptSphere(const Point3D& p1, const Point3D& p2, float radius) const
{
	return ((PolygonZoneObject::InteriorSphere(p1, radius)) && (PolygonZoneObject::InteriorSphere(p2, radius)));
}

bool PolygonZoneObject::InteriorPoint(const Point3D& position) const
{
	if ((position.z < 0.0F) || (position.z > polygonHeight))
	{
		return (false);
	}

	int32 count = vertexCount;
	const Point3D *v1 = &polygonVertex[count - 1];
	for (machine a = 0; a < count; a++)
	{
		const Point3D *v2 = &polygonVertex[a];

		float nx = v2->y - v1->y;
		float ny = v1->x - v2->x;
		if ((position.x - v1->x) * nx + (position.y - v1->y) * ny > 0.0F)
		{
			return (false);
		}

		v1 = v2;
	}

	return (true);
}

Point3D PolygonZoneObject::GetInitialSupportPoint(void) const
{
	return (polygonVertex[0]);
}

bool PolygonZoneObject::CalculateSupportPoint(const Vector3D& direction, Point3D *support) const
{
	const Point3D *q = &polygonVertex[0];
	float f = q->x * direction.x + q->y * direction.y;

	for (machine a = 1; a < vertexCount; a++)
	{
		const Point3D& p = polygonVertex[a];
		float d = p.x * direction.x + p.y * direction.y;
		if (d > f)
		{
			f = d;
			q = &p;
		}
	}

	support->GetPoint2D() = q->GetPoint2D();
	support->z = (direction.z > 0.0F) ? polygonHeight : 0.0F;
	return (true);
}

int32 PolygonZoneObject::ClipInteriorEdges(int32 edgeCount, const Point3D (*edge)[2], Point3D (*restrict clippedEdge)[2]) const
{
	Point3D (*clippedStart)[2] = clippedEdge;

	for (machine i = 0; i < edgeCount; i++)
	{
		Point3D p1 = edge[i][0];
		Point3D p2 = edge[i][1];
		bool clip1 = false;
		bool clip2 = false;

		// Clip against z = 0.

		float z1 = p1.z;
		float z2 = p2.z;
		int8 loc1 = (z1 > kZoneClipEpsilon) ? kPolygonInterior : ((z1 < -kZoneClipEpsilon) ? kPolygonExterior : kPolygonBoundary);
		int8 loc2 = (z2 > kZoneClipEpsilon) ? kPolygonInterior : ((z2 < -kZoneClipEpsilon) ? kPolygonExterior : kPolygonBoundary);
		int32 minloc = Min(loc1, loc2);

		if (Max(loc1, loc2) != kPolygonInterior)
		{
			if (minloc == kPolygonExterior)
			{
				(*clippedEdge)[0] = p1;
				(*clippedEdge)[1] = p2;
				clippedEdge++;
			}

			continue;
		}

		if (minloc == kPolygonExterior)
		{
			float t = z1 / (z1 - z2);
			float x = p1.x + t * (p2.x - p1.x);
			float y = p1.y + t * (p2.y - p1.y);

			if (loc1 == kPolygonExterior)
			{
				p1.Set(x, y, 0.0F);
				clip1 = true;
			}
			else
			{
				p2.Set(x, y, 0.0F);
				clip2 = true;
			}
		}

		// Clip against z = polygonHeight.

		z1 = polygonHeight - p1.z;
		z2 = polygonHeight - p2.z;
		loc1 = (z1 > kZoneClipEpsilon) ? kPolygonInterior : ((z1 < -kZoneClipEpsilon) ? kPolygonExterior : kPolygonBoundary);
		loc2 = (z2 > kZoneClipEpsilon) ? kPolygonInterior : ((z2 < -kZoneClipEpsilon) ? kPolygonExterior : kPolygonBoundary);
		minloc = Min(loc1, loc2);

		if (Max(loc1, loc2) != kPolygonInterior)
		{
			if (minloc == kPolygonExterior)
			{
				(*clippedEdge)[0] = p1;
				(*clippedEdge)[1] = p2;
				clippedEdge++;
			}

			continue;
		}

		if (minloc == kPolygonExterior)
		{
			float t = z1 / (z1 - z2);
			float x = p1.x + t * (p2.x - p1.x);
			float y = p1.y + t * (p2.y - p1.y);

			if (loc1 == kPolygonExterior)
			{
				p1.Set(x, y, polygonHeight);
				clip1 = true;
			}
			else
			{
				p2.Set(x, y, polygonHeight);
				clip2 = true;
			}
		}

		int32 count = vertexCount;
		const Point3D *v1 = &polygonVertex[count - 1];
		for (machine j = 0; j < count; j++)
		{
			const Point3D *v2 = &polygonVertex[j];

			float nx = v1->y - v2->y;
			float ny = v2->x - v1->x;
			float m = InverseSqrt(nx * nx + ny * ny);
			nx *= m;
			ny *= m;

			float d1 = (p1.x - v1->x) * nx + (p1.y - v1->y) * ny;
			float d2 = (p2.x - v1->x) * nx + (p2.y - v1->y) * ny;

			loc1 = (d1 > kZoneClipEpsilon) ? kPolygonInterior : ((d1 < -kZoneClipEpsilon) ? kPolygonExterior : kPolygonBoundary);
			loc2 = (d2 > kZoneClipEpsilon) ? kPolygonInterior : ((d2 < -kZoneClipEpsilon) ? kPolygonExterior : kPolygonBoundary);
			minloc = Min(loc1, loc2);

			if (Max(loc1, loc2) != kPolygonInterior)
			{
				if (minloc == kPolygonExterior)
				{
					(*clippedEdge)[0] = p1;
					(*clippedEdge)[1] = p2;
					clippedEdge++;
				}

				goto next;
			}

			if (minloc == kPolygonExterior)
			{
				float t = d1 / (d1 - d2);

				if (loc1 == kPolygonExterior)
				{
					p1 = p1 + (p2 - p1) * t;
					clip1 = true;
				}
				else
				{
					p2 = p1 + (p2 - p1) * t;
					clip2 = true;
				}
			}

			v1 = v2;
		}

		if (clip1)
		{
			(*clippedEdge)[0] = p1;
			(*clippedEdge)[1] = edge[i][0];
			clippedEdge++;
		}

		if (clip2)
		{
			(*clippedEdge)[0] = p2;
			(*clippedEdge)[1] = edge[i][1];
			clippedEdge++;
		}

		next:;
	}

	return ((int32) (clippedEdge - clippedStart));
}


C4::Zone::Zone(ZoneType type) : Node(kNodeZone)
{
	zoneType = type;

	traversalExclusionMask = 0;

	connectedFogSpace = nullptr;
	connectedShadowSpace = nullptr;
	connectedAcousticsSpace = nullptr;
	connectedRadiositySpace = nullptr;
}

C4::Zone::Zone(const Zone& zone) : Node(zone)
{
	zoneType = zone.zoneType;

	traversalExclusionMask = 0;

	connectedFogSpace = nullptr;
	connectedShadowSpace = nullptr;
	connectedAcousticsSpace = nullptr;
	connectedRadiositySpace = nullptr;
}

C4::Zone::~Zone()
{
	subzoneList.RemoveAll();
	portalList.RemoveAll();
	occlusionPortalList.RemoveAll();
	occlusionSpaceList.RemoveAll();
	fogSpaceList.RemoveAll();
	markerList.RemoveAll();

	lightRegionList.RemoveAll();
	sourceRegionList.RemoveAll();
}

C4::Zone *C4::Zone::Create(Unpacker& data, unsigned_int32 unpackFlags)
{
	switch (data.GetType())
	{
		case kZoneInfinite:

			return (new InfiniteZone);

		case kZoneBox:

			return (new BoxZone);

		case kZoneCylinder:

			return (new CylinderZone);

		case kZonePolygon:

			return (new PolygonZone);
	}

	return (nullptr);
}

void C4::Zone::Detach(void)
{
	ListElement<Zone>::Detach();
	Node::Detach();
}

void C4::Zone::PackType(Packer& data) const
{
	Node::PackType(data);
	data << zoneType;
}

void C4::Zone::Pack(Packer& data, unsigned_int32 packFlags) const
{
	Node::Pack(data, packFlags);

	const Node *physicsNode = physicsNodeLink;
	if ((physicsNode) && (physicsNode->LinkedNodePackable(packFlags)))
	{
		data << ChunkHeader('PLNK', 4);
		data << physicsNode->GetNodeIndex();
	}

	data << TerminatorChunk;
}

void C4::Zone::Unpack(Unpacker& data, unsigned_int32 unpackFlags)
{
	Node::Unpack(data, unpackFlags);
	UnpackChunkList<Zone>(data, unpackFlags);
}

bool C4::Zone::UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags)
{
	switch (chunkHeader->chunkType)
	{
		case 'PLNK':
		{
			int32	physicsNodeIndex;

			data >> physicsNodeIndex;
			data.AddNodeLink(physicsNodeIndex, &PhysicsNodeLinkProc, this);
			return (true);
		}
	}

	return (false);
}

void C4::Zone::PhysicsNodeLinkProc(Node *node, void *cookie)
{
	Zone *zone = static_cast<Zone *>(cookie);
	zone->physicsNodeLink = node;
}

int32 C4::Zone::GetInternalConnectorCount(void) const
{
	return (4);
}

const char *C4::Zone::GetInternalConnectorKey(int32 index) const
{
	if (index == 0)
	{
		return (kConnectorKeyFog);
	}
	else if (index == 1)
	{
		return (kConnectorKeyShadow);
	}
	else if (index == 1)
	{
		return (kConnectorKeyAcoustics);
	}
	else if (index == 2)
	{
		return (kConnectorKeyRadiosity);
	}

	return (nullptr);
}

Node *C4::Zone::GetHierarchicalConnectedNode(const char *key) const
{
	const Zone *zone = this;
	do
	{
		const Connector *connector = zone->GetConnector(key);
		if (connector)
		{
			return (connector->GetConnectorTarget());
		}

		zone = zone->GetOwningZone();
	} while (zone);

	return (nullptr);
}

void C4::Zone::ProcessInternalConnectors(void)
{
	connectedFogSpace = static_cast<FogSpace *>(GetHierarchicalConnectedNode(kConnectorKeyFog));
	connectedShadowSpace = static_cast<ShadowSpace *>(GetHierarchicalConnectedNode(kConnectorKeyShadow));
	connectedAcousticsSpace = static_cast<AcousticsSpace *>(GetHierarchicalConnectedNode(kConnectorKeyAcoustics));
	connectedRadiositySpace = static_cast<RadiositySpace *>(GetHierarchicalConnectedNode(kConnectorKeyRadiosity));
}

bool C4::Zone::ValidConnectedNode(const ConnectorKey& key, const Node *node) const
{
	if (key == kConnectorKeyFog)
	{
		if (node->GetNodeType() == kNodeSpace)
		{
			return (static_cast<const Space *>(node)->GetSpaceType() == kSpaceFog);
		}

		return (false);
	}

	if (key == kConnectorKeyShadow)
	{
		if (node->GetNodeType() == kNodeSpace)
		{
			return (static_cast<const Space *>(node)->GetSpaceType() == kSpaceShadow);
		}

		return (false);
	}

	if (key == kConnectorKeyAcoustics)
	{
		if (node->GetNodeType() == kNodeSpace)
		{
			return (static_cast<const Space *>(node)->GetSpaceType() == kSpaceAcoustics);
		}

		return (false);
	}

	if (key == kConnectorKeyRadiosity)
	{
		if (node->GetNodeType() == kNodeSpace)
		{
			return (static_cast<const Space *>(node)->GetSpaceType() == kSpaceRadiosity);
		}

		return (false);
	}

	return (Node::ValidConnectedNode(key, node));
}

void C4::Zone::SetConnectedFogSpace(FogSpace *fogSpace)
{
	connectedFogSpace = fogSpace;
	SetConnectedNode(kConnectorKeyFog, fogSpace);
}

void C4::Zone::SetConnectedShadowSpace(ShadowSpace *shadowSpace)
{
	connectedShadowSpace = shadowSpace;
	SetConnectedNode(kConnectorKeyShadow, shadowSpace);
}

void C4::Zone::SetConnectedAcousticsSpace(AcousticsSpace *acousticsSpace)
{
	connectedAcousticsSpace = acousticsSpace;
	SetConnectedNode(kConnectorKeyAcoustics, acousticsSpace);
}

void C4::Zone::SetConnectedRadiositySpace(RadiositySpace *radiositySpace)
{
	connectedRadiositySpace = radiositySpace;

	if (radiositySpace)
	{
		ambientEnvironment.ambientShaderTypeDelta = kShaderAmbientRadiosity - kShaderAmbient;
		ambientEnvironment.radiositySpaceObject = radiositySpace->GetObject();
		ambientEnvironment.radiositySpaceTransformable = radiositySpace;
	}
	else
	{
		ambientEnvironment.ambientShaderTypeDelta = 0;
		ambientEnvironment.radiositySpaceObject = nullptr;
		ambientEnvironment.radiositySpaceTransformable = nullptr;
	}

	SetConnectedNode(kConnectorKeyRadiosity, radiositySpace);
}

void C4::Zone::Preprocess(void)
{
	Zone *zone = GetOwningZone();
	if (zone)
	{
		zone->AddSubzone(this);
	}

	if (!GetManipulator())
	{
		const Box3D& box = GetWorldBoundingBox();
		float size = Fmax(box.max.x - box.min.x, box.max.y - box.min.y);

		if (size >= 64.0F)
		{
			for (machine a = 0; a < kCellGraphCount; a++)
			{
				cellGraph[a].Activate(box, 16.0F);
			}
		}
		else if (size >= 32.0F)
		{
			cellGraph[kCellGraphGeometry].Activate(box, 8.0F);
			cellGraph[kCellGraphEffect].Activate(box, 8.0F);
		}
		else if (size >= 16.0F)
		{
			cellGraph[kCellGraphGeometry].Activate(box, 4.0F);
			cellGraph[kCellGraphEffect].Activate(box, 4.0F);
		}
	}

	Node::Preprocess();

	ZoneObject *object = GetObject();
	ambientEnvironment.ambientLightColor = &object->GetAmbientLight();

	Texture *const& texture = object->GetEnvironmentMap();
	ambientEnvironment.environmentMap = &texture;
	if (!texture)
	{
		object->SetEnvironmentMap("");
	}

	RadiositySpace *radiositySpace = connectedRadiositySpace;
	if (radiositySpace)
	{
		ambientEnvironment.ambientShaderTypeDelta = kShaderAmbientRadiosity - kShaderAmbient;
		ambientEnvironment.radiositySpaceObject = radiositySpace->GetObject();
		ambientEnvironment.radiositySpaceTransformable = radiositySpace;
	}
	else
	{
		ambientEnvironment.ambientShaderTypeDelta = 0;
		ambientEnvironment.radiositySpaceObject = nullptr;
		ambientEnvironment.radiositySpaceTransformable = nullptr;
	}
}

void C4::Zone::InvalidateLightRegions(void) const
{
	for (;;)
	{
		const LightRegion *lightRegion = GetFirstLightRegion();
		if (!lightRegion)
		{
			break;
		}

		lightRegion->GetLight()->InvalidateLightRegions();
	}
}

void C4::Zone::InvalidateSourceRegions(void) const
{
	const SourceRegion *sourceRegion = GetFirstSourceRegion();
	while (sourceRegion)
	{
		sourceRegion->GetSource()->InvalidateSourceRegions();
		sourceRegion = sourceRegion->Next();
	}
}

bool C4::Zone::InsertZoneTreeSite(int32 index, Node *node, int32 maxDepth, int32 forcedDepth)
{
	if (forcedDepth < 0)
	{
		bool covered = false;

		if (maxDepth > 0)
		{
			Zone *zone = subzoneList.First();
			if (zone)
			{
				Point3D		edge[2][96][2];

				const Box3D& box = node->GetWorldBoundingBox();

				edge[0][0][0] = box.min;
				edge[0][0][1].Set(box.max.x, box.min.y, box.min.z);
				edge[0][1][0].Set(box.min.x, box.max.y, box.min.z);
				edge[0][1][1].Set(box.max.x, box.max.y, box.min.z);
				edge[0][2][0].Set(box.min.x, box.min.y, box.max.z);
				edge[0][2][1].Set(box.max.x, box.min.y, box.max.z);
				edge[0][3][0].Set(box.min.x, box.max.y, box.max.z);
				edge[0][3][1].Set(box.max.x, box.max.y, box.max.z);

				edge[0][4][0] = box.min;
				edge[0][4][1].Set(box.min.x, box.max.y, box.min.z);
				edge[0][5][0].Set(box.max.x, box.min.y, box.min.z);
				edge[0][5][1].Set(box.max.x, box.max.y, box.min.z);
				edge[0][6][0].Set(box.min.x, box.min.y, box.max.z);
				edge[0][6][1].Set(box.min.x, box.max.y, box.max.z);
				edge[0][7][0].Set(box.max.x, box.min.y, box.max.z);
				edge[0][7][1].Set(box.max.x, box.max.y, box.max.z);

				edge[0][8][0] = box.min;
				edge[0][8][1].Set(box.min.x, box.min.y, box.max.z);
				edge[0][9][0].Set(box.max.x, box.min.y, box.min.z);
				edge[0][9][1].Set(box.max.x, box.min.y, box.max.z);
				edge[0][10][0].Set(box.min.x, box.max.y, box.min.z);
				edge[0][10][1].Set(box.min.x, box.max.y, box.max.z);
				edge[0][11][0].Set(box.max.x, box.max.y, box.min.z);
				edge[0][11][1].Set(box.max.x, box.max.y, box.max.z);

				int32 edgeCount = 12;
				unsigned_int32 edgeParity = 0;

				Point3D worldCenter = (box.min + box.max) * 0.5F;
				Vector3D worldSize = (box.max - box.min) * 0.5F;

				const Transform4D *prevWorldTransform = &Identity4D;

				do
				{
					const Transform4D& inverseTransform = zone->GetInverseWorldTransform();
					BoundingBox boundingBox(inverseTransform * worldCenter, inverseTransform[0] * worldSize.x, inverseTransform[1] * worldSize.y, inverseTransform[2] * worldSize.z);

					const ZoneObject *object = zone->GetObject();
					if (object->IntersectsBoundingBox(&boundingBox))
					{
						covered |= zone->InsertZoneTreeSite(index, node, maxDepth - 1, -1);

						if ((!covered) && (edgeCount <= 48))
						{
							Point3D (*oldEdge)[2] = edge[edgeParity];
							Point3D (*newEdge)[2] = edge[edgeParity ^ 1];

							Transform4D transform = inverseTransform * *prevWorldTransform;
							for (machine a = 0; a < edgeCount; a++)
							{
								oldEdge[a][0] = transform * oldEdge[a][0];
								oldEdge[a][1] = transform * oldEdge[a][1];
							}

							edgeCount = object->ClipInteriorEdges(edgeCount, oldEdge, newEdge);
							covered |= (edgeCount == 0);

							edgeParity ^= 1;
							prevWorldTransform = &zone->GetWorldTransform();
						}
					}

					zone = zone->Next();
				} while (zone);
			}
		}

		if (!covered)
		{
			cellGraph[index].InsertSite(node);
			node->GetZoneMembershipArray().AddElement(this);
		}

		return (covered);
	}

	if (maxDepth > 0)
	{
		// Add subzones to the membership array first to be consistent
		// with the order induced by the coverage algorithm above.

		Zone *subzone = subzoneList.First();
		while (subzone)
		{
			subzone->InsertZoneTreeSite(index, node, maxDepth - 1, forcedDepth - 1);
			subzone = subzone->Next();
		}
	}

	cellGraph[index].InsertSite(node);
	node->GetZoneMembershipArray().AddElement(this);
	return (true);
}

void C4::Zone::InsertInfiniteSite(int32 index, Node *node, int32 depth)
{
	if (depth > 0)
	{
		Zone *subzone = subzoneList.First();
		while (subzone)
		{
			subzone->InsertInfiniteSite(index, node, depth - 1);
			subzone = subzone->Next();
		}
	}

	new Bond(&cellGraph[index], node);
	node->GetZoneMembershipArray().AddElement(this);
}


InfiniteZone::InfiniteZone() : Zone(kZoneInfinite)
{
	auxiliaryObject = nullptr;
}

InfiniteZone::InfiniteZone(const InfiniteZone& infiniteZone) : Zone(infiniteZone)
{
	auxiliaryObject = infiniteZone.auxiliaryObject;
	if (auxiliaryObject)
	{
		auxiliaryObject->Retain();
	}
}

InfiniteZone::~InfiniteZone()
{
	if (auxiliaryObject)
	{
		auxiliaryObject->Release();
	}
}

Node *InfiniteZone::Replicate(void) const
{
	return (new InfiniteZone(*this));
}

void InfiniteZone::Prepack(List<Object> *linkList) const
{
	Zone::Prepack(linkList);
	if (auxiliaryObject)
	{
		linkList->Append(auxiliaryObject);
	}
}

void InfiniteZone::Pack(Packer& data, unsigned_int32 packFlags) const
{
	Zone::Pack(data, packFlags);

	if ((auxiliaryObject) && (!(packFlags & kPackSettings)))
	{
		data << ChunkHeader('AUXO', 4);
		data << auxiliaryObject->GetObjectIndex();
	}

	data << TerminatorChunk;
}

void InfiniteZone::Unpack(Unpacker& data, unsigned_int32 unpackFlags)
{
	Zone::Unpack(data, unpackFlags);
	UnpackChunkList<InfiniteZone>(data, unpackFlags);
}

bool InfiniteZone::UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags)
{
	switch (chunkHeader->chunkType)
	{
		case 'AUXO':
		{
			int32	objectIndex;

			data >> objectIndex;
			data.AddObjectLink(objectIndex, &AuxiliaryObjectLinkProc, this);
			return (true);
		}
	}

	return (false);
}

void InfiniteZone::AuxiliaryObjectLinkProc(Object *object, void *cookie)
{
	InfiniteZone *infiniteZone = static_cast<InfiniteZone *>(cookie);
	infiniteZone->SetAuxiliaryObject(object);
}

void InfiniteZone::SetAuxiliaryObject(Object *object)
{
	if (auxiliaryObject != object)
	{
		if (auxiliaryObject)
		{
			auxiliaryObject->Release();
		}

		if (object)
		{
			object->Retain();
		}

		auxiliaryObject = object;
	}
}

bool InfiniteZone::CalculateBoundingBox(Box3D *box) const
{
	*box = GetObject()->GetZoneBox();
	return (true);
}

bool InfiniteZone::CalculateBoundingSphere(BoundingSphere *sphere) const
{
	const Box3D& box = GetObject()->GetZoneBox();
	sphere->SetCenter(box.GetCenter());
	sphere->SetRadius(Magnitude(box.GetSize()) * 0.5F);
	return (true);
}

void InfiniteZone::Preprocess(void)
{
	SetWorldBoundingBox(GetObject()->GetZoneBox());
	Zone::Preprocess();
}


BoxZone::BoxZone() : Zone(kZoneBox)
{
}

BoxZone::BoxZone(const Vector3D& size) : Zone(kZoneBox)
{
	SetNewObject(new BoxZoneObject(size));
}

BoxZone::BoxZone(const BoxZone& boxZone) : Zone(boxZone)
{
}

BoxZone::~BoxZone()
{
}

Node *BoxZone::Replicate(void) const
{
	return (new BoxZone(*this));
}

bool BoxZone::CalculateBoundingBox(Box3D *box) const
{
	box->min.Set(0.0F, 0.0F, 0.0F);
	box->max = GetObject()->GetBoxSize();
	return (true);
}

bool BoxZone::CalculateBoundingSphere(BoundingSphere *sphere) const
{
	Vector3D size = GetObject()->GetBoxSize() * 0.5F;
	sphere->SetCenter(Zero3D + size);
	sphere->SetRadius(Magnitude(size));
	return (true);
}

void BoxZone::Preprocess(void)
{
	Zone::Preprocess();

	SetVisibilityProc(&BoxVisible);
	SetOcclusionProc(&BoxVisible);
}


CylinderZone::CylinderZone() : Zone(kZoneCylinder)
{
}

CylinderZone::CylinderZone(const Vector2D& size, float height) : Zone(kZoneCylinder)
{
	SetNewObject(new CylinderZoneObject(size, height));
}

CylinderZone::CylinderZone(const CylinderZone& cylinderZone) : Zone(cylinderZone)
{
}

CylinderZone::~CylinderZone()
{
}

Node *CylinderZone::Replicate(void) const
{
	return (new CylinderZone(*this));
}

bool CylinderZone::CalculateBoundingBox(Box3D *box) const
{
	const CylinderZoneObject *object = GetObject();
	const Vector2D& cylinderSize = object->GetCylinderSize();

	box->min.Set(-cylinderSize.x, -cylinderSize.y, 0.0F);
	box->max.Set(cylinderSize.x, cylinderSize.y, object->GetCylinderHeight());
	return (true);
}

bool CylinderZone::CalculateBoundingSphere(BoundingSphere *sphere) const
{
	const CylinderZoneObject *object = GetObject();
	Point3D size(object->GetCylinderSize() * 0.5F, object->GetCylinderHeight() * 0.5F);

	sphere->SetCenter(size);
	sphere->SetRadius(Magnitude(size));
	return (true);
}

void CylinderZone::Preprocess(void)
{
	Zone::Preprocess();

	SetVisibilityProc(&BoxVisible);
	SetOcclusionProc(&BoxVisible);
}


PolygonZone::PolygonZone() : Zone(kZonePolygon)
{
}

PolygonZone::PolygonZone(const Vector2D& size, float height) : Zone(kZonePolygon)
{
	SetNewObject(new PolygonZoneObject(size, height));
}

PolygonZone::PolygonZone(const PolygonZone& polygonZone) : Zone(polygonZone)
{
}

PolygonZone::~PolygonZone()
{
}

Node *PolygonZone::Replicate(void) const
{
	return (new PolygonZone(*this));
}

bool PolygonZone::CalculateBoundingBox(Box3D *box) const
{
	PolygonZoneObject *object = GetObject();

	const Point3D *vertex = object->GetVertexArray();
	float xmin = vertex->x;
	float ymin = vertex->y;
	float xmax = xmin;
	float ymax = ymin;

	int32 count = object->GetVertexCount();
	for (machine a = 1; a < count; a++)
	{
		const Point3D& p = vertex[a];
		float x = p.x;
		float y = p.y;

		xmin = Fmin(xmin, x);
		xmax = Fmax(xmax, x);
		ymin = Fmin(ymin, y);
		ymax = Fmax(ymax, y);
	}

	box->min.Set(xmin, ymin, 0.0F);
	box->max.Set(xmax, ymax, object->GetPolygonHeight());
	return (true);
}

bool PolygonZone::CalculateBoundingSphere(BoundingSphere *sphere) const
{
	PolygonZoneObject *object = GetObject();

	const Point3D *vertex = object->GetVertexArray();
	float xmin = vertex->x;
	float ymin = vertex->y;
	float xmax = xmin;
	float ymax = ymin;

	int32 count = object->GetVertexCount();
	for (machine a = 1; a < count; a++)
	{
		const Point3D& p = vertex[a];
		float x = p.x;
		float y = p.y;

		xmin = Fmin(xmin, x);
		xmax = Fmax(xmax, x);
		ymin = Fmin(ymin, y);
		ymax = Fmax(ymax, y);
	}

	float h = object->GetPolygonHeight() * 0.5F;
	sphere->SetCenter(Point3D((xmin + xmax) * 0.5F, (ymin + ymax) * 0.5F, h));
	sphere->SetRadius(Magnitude(Vector3D((xmax - xmin) * 0.5F, (ymax - ymin) * 0.5F, h)));
	return (true);
}

void PolygonZone::Preprocess(void)
{
	Zone::Preprocess();

	SetVisibilityProc(&BoxVisible);
	SetOcclusionProc(&BoxVisible);
}

// ZYUQURM
