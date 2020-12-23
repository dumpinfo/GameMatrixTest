 

#include "C4Primitives.h"
#include "C4Deformable.h"
#include "C4Simulation.h"
#include "C4Configuration.h"


using namespace C4;


namespace C4
{
	template class Creatable<PrimitiveGeometry>;
}


PrimitiveGeometryObject::PrimitiveGeometryObject(PrimitiveType type) : GeometryObject(kGeometryPrimitive)
{
	primitiveType = type;
	primitiveFlags = kPrimitiveEndcap;

	buildLevelCount = 1;
	maxSubdivX = 1;
	maxSubdivY = 1;
}

PrimitiveGeometryObject::~PrimitiveGeometryObject()
{
}

PrimitiveGeometryObject *PrimitiveGeometryObject::Create(Unpacker& data, unsigned_int32 unpackFlags)
{
	switch (data.GetType())
	{
		case kPrimitivePlate:

			return (new PlateGeometryObject);

		case kPrimitiveDisk:

			return (new DiskGeometryObject);

		case kPrimitiveHole:

			return (new HoleGeometryObject);

		case kPrimitiveAnnulus:

			return (new AnnulusGeometryObject);

		case kPrimitiveBox:

			return (new BoxGeometryObject);

		case kPrimitivePyramid:

			return (new PyramidGeometryObject);

		case kPrimitiveCylinder:

			return (new CylinderGeometryObject);

		case kPrimitiveCone:

			return (new ConeGeometryObject);

		case kPrimitiveTruncatedCone:

			return (new TruncatedConeGeometryObject);

		case kPrimitiveSphere:

			return (new SphereGeometryObject);

		case kPrimitiveDome:

			return (new DomeGeometryObject);

		case kPrimitiveTorus:

			return (new TorusGeometryObject);

		case kPrimitiveTube:

			return (new TubeGeometryObject);

		case kPrimitiveExtrusion:

			return (new ExtrusionGeometryObject);

		case kPrimitiveRevolution:

			return (new RevolutionGeometryObject);

		case kPrimitiveRope:

			return (new RopeGeometryObject);

		case kPrimitiveCloth:

			return (new ClothGeometryObject);
	} 

	return (nullptr);
} 

void PrimitiveGeometryObject::PackType(Packer& data) const 
{
	GeometryObject::PackType(data);
	data << primitiveType; 
}
 
void PrimitiveGeometryObject::Pack(Packer& data, unsigned_int32 packFlags) const 
{
	GeometryObject::Pack(data, packFlags);

	data << ChunkHeader('FLAG', 4); 
	data << primitiveFlags;

	data << ChunkHeader('LEVL', 4);
	data << buildLevelCount;

	data << ChunkHeader('SBDV', 8);
	data << maxSubdivX;
	data << maxSubdivY;

	data << TerminatorChunk;
}

void PrimitiveGeometryObject::Unpack(Unpacker& data, unsigned_int32 unpackFlags)
{
	GeometryObject::Unpack(data, unpackFlags);
	UnpackChunkList<PrimitiveGeometryObject>(data, unpackFlags);
}

bool PrimitiveGeometryObject::UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags)
{
	switch (chunkHeader->chunkType)
	{
		case 'FLAG':

			data >> primitiveFlags;
			return (true);

		case 'LEVL':

			data >> buildLevelCount;
			return (true);

		case 'SBDV':

			data >> maxSubdivX;
			data >> maxSubdivY;
			return (true);
	}

	return (false);
}

int32 PrimitiveGeometryObject::GetCategorySettingCount(Type category) const
{
	int32 count = GeometryObject::GetCategorySettingCount(category);
	if (category == kObjectGeometry)
	{
		count += 6;
	}

	return (count);
}

Setting *PrimitiveGeometryObject::GetCategorySetting(Type category, int32 index, unsigned_int32 flags) const
{
	if (category == kObjectGeometry)
	{
		int32 i = index - GeometryObject::GetCategorySettingCount(kObjectGeometry);
		if (i >= 0)
		{
			if (flags & kConfigurationScript)
			{
				return (nullptr);
			}

			const StringTable *table = TheInterfaceMgr->GetStringTable();

			if (i == 0)
			{
				const char *title = table->GetString(StringID(kObjectGeometry, kGeometryPrimitive));
				return (new HeadingSetting(kGeometryPrimitive, title));
			}

			if (i == 1)
			{
				const char *title = table->GetString(StringID(kObjectGeometry, kGeometryPrimitive, 'CAPS'));
				return (new BooleanSetting('CAPS', ((primitiveFlags & kPrimitiveEndcap) != 0), title));
			}

			if (i == 2)
			{
				const char *title = table->GetString(StringID(kObjectGeometry, kGeometryPrimitive, 'IVRT'));
				return (new BooleanSetting('IVRT', ((primitiveFlags & kPrimitiveInvert) != 0), title));
			}

			if (i == 3)
			{
				const char *title = table->GetString(StringID(kObjectGeometry, kGeometryPrimitive, 'LEVL'));
				return (new IntegerSetting('LEVL', buildLevelCount, title, 1, 4, 1));
			}

			if (i == 4)
			{
				const char *title = table->GetString(StringID(kObjectGeometry, kGeometryPrimitive, 'XSUB'));
				return (new TextSetting('XSUB', Text::IntegerToString(maxSubdivX), title, 2, &EditTextWidget::NumberFilter));
			}

			if (i == 5)
			{
				const char *title = table->GetString(StringID(kObjectGeometry, kGeometryPrimitive, 'YSUB'));
				return (new TextSetting('YSUB', Text::IntegerToString(maxSubdivY), title, 2, &EditTextWidget::NumberFilter));
			}

			return (nullptr);
		}
	}

	return (GeometryObject::GetCategorySetting(category, index, flags));
}

void PrimitiveGeometryObject::SetCategorySetting(Type category, const Setting *setting)
{
	if (category == kObjectGeometry)
	{
		Type identifier = setting->GetSettingIdentifier();

		if (identifier == 'CAPS')
		{
			if (static_cast<const BooleanSetting *>(setting)->GetBooleanValue())
			{
				primitiveFlags |= kPrimitiveEndcap;
			}
			else
			{
				primitiveFlags &= ~kPrimitiveEndcap;
			}
		}
		else if (identifier == 'IVRT')
		{
			if (static_cast<const BooleanSetting *>(setting)->GetBooleanValue())
			{
				primitiveFlags |= kPrimitiveInvert;
			}
			else
			{
				primitiveFlags &= ~kPrimitiveInvert;
			}
		}
		else if (identifier == 'LEVL')
		{
			buildLevelCount = static_cast<const IntegerSetting *>(setting)->GetIntegerValue();
		}
		else if (identifier == 'XSUB')
		{
			maxSubdivX = Max(Text::StringToInteger(static_cast<const TextSetting *>(setting)->GetText()), 1);
		}
		else if (identifier == 'YSUB')
		{
			maxSubdivY = Max(Text::StringToInteger(static_cast<const TextSetting *>(setting)->GetText()), 1);
		}
		else
		{
			GeometryObject::SetCategorySetting(kObjectGeometry, setting);
		}
	}
	else
	{
		GeometryObject::SetCategorySetting(category, setting);
	}
}

Point3D PrimitiveGeometryObject::GetInitialPrimitiveSupportPoint(void) const
{
	return (Point3D(0.0F, 0.0F, 0.0F));
}

Point3D PrimitiveGeometryObject::CalculatePrimitiveSupportPoint(const Vector3D& direction) const
{
	return (Point3D(0.0F, 0.0F, 0.0F));
}

void PrimitiveGeometryObject::CalculatePrimitiveSupportPointArray(int32 count, const Vector3D *direction, Point3D *support) const
{
}

int32 PrimitiveGeometryObject::GetMaxCollisionLevel(void) const
{
	return (buildLevelCount - 1);
}

void PrimitiveGeometryObject::TriangulateDisk(int32 vertexCount, unsigned_int32 baseIndex, Triangle *triangle)
{
	int32 count = vertexCount;
	for (machine k = 1;;)
	{
		machine k2 = k * 2;
		for (machine i = 2; i <= count; i += 2)
		{
			machine j = i * k;
			triangle->Set(baseIndex + j - k2, baseIndex + j - k, baseIndex + (j & ((j - vertexCount) >> 31)));
			triangle++;
		}

		count -= count >> 1;
		if (count < 3)
		{
			break;
		}

		k = k2;
	}
}

void PrimitiveGeometryObject::Preprocess(unsigned_int32 dynamicFlags)
{
	GeometryObject::Preprocess(dynamicFlags);

	if ((primitiveFlags & (kPrimitiveEndcap | kPrimitiveInvert)) != kPrimitiveEndcap)
	{
		ClearConvexPrimitiveFlag();
	}
}

void PrimitiveGeometryObject::BuildPrimitiveLevel(int32 level, const Transformable *transformable)
{
}

void PrimitiveGeometryObject::BuildStandardArrays(Mesh *inputMesh, Mesh *outputMesh, const Transformable *transformable) const
{
	Mesh	tempMesh;

	inputMesh->GenerateTexcoords(transformable, this);
	inputMesh->TransformTexcoords(transformable, this);
	tempMesh.BuildTangentArray(inputMesh);
	outputMesh->BuildSegmentArray(&tempMesh, GetSurfaceCount(), GetSurfaceData());
}

void PrimitiveGeometryObject::Build(Geometry *geometry)
{
	SetGeometryLevelCount(buildLevelCount);
	geometry->InvalidateShaderData();

	for (machine a = 0; a < kMaxAttributeArrayCount; a++)
	{
		geometry->SetVertexAttributeArray(a, 0, 0);
	}

	for (machine a = 0; a < buildLevelCount; a++)
	{
		BuildPrimitiveLevel(a, geometry);

		if (primitiveFlags & kPrimitiveInvert)
		{
			GetGeometryLevel(a)->InvertMesh();
		}
	}

	BuildCollisionData();
	ResetVertexBuffers();
}


PlateGeometryObject::PlateGeometryObject() : PrimitiveGeometryObject(kPrimitivePlate)
{
	SetConvexPrimitiveFlag();
	SetGeometryFlags(GetGeometryFlags() & ~kGeometryCastShadows);

	SetStaticSurfaceData(1, staticSurfaceData);
}

PlateGeometryObject::PlateGeometryObject(const Vector2D& size) : PrimitiveGeometryObject(kPrimitivePlate)
{
	plateSize = size;

	SetConvexPrimitiveFlag();
	SetGeometryFlags(GetGeometryFlags() & ~kGeometryCastShadows);

	SetStaticSurfaceData(1, staticSurfaceData, true);
	staticSurfaceData[0].textureAlignData[0].alignMode = kTextureAlignGlobalObjectPlane;
	staticSurfaceData[0].textureAlignData[0].alignPlane.Set(1.0F, 0.0F, 0.0F, 0.0F);
	staticSurfaceData[0].textureAlignData[1].alignMode = kTextureAlignGlobalObjectPlane;
	staticSurfaceData[0].textureAlignData[1].alignPlane.Set(0.0F, 1.0F, 0.0F, 0.0F);
}

PlateGeometryObject::~PlateGeometryObject()
{
}

void PlateGeometryObject::Pack(Packer& data, unsigned_int32 packFlags) const
{
	PrimitiveGeometryObject::Pack(data, packFlags);

	data << ChunkHeader('SIZE', sizeof(Vector2D));
	data << plateSize;

	data << TerminatorChunk;
}

void PlateGeometryObject::Unpack(Unpacker& data, unsigned_int32 unpackFlags)
{
	PrimitiveGeometryObject::Unpack(data, unpackFlags);
	UnpackChunkList<PlateGeometryObject>(data, unpackFlags);
}

bool PlateGeometryObject::UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags)
{
	switch (chunkHeader->chunkType)
	{
		case 'SIZE':

			data >> plateSize;
			return (true);
	}

	return (false);
}

int32 PlateGeometryObject::GetObjectSize(float *size) const
{
	size[0] = plateSize.x;
	size[1] = plateSize.y;
	return (2);
}

void PlateGeometryObject::SetObjectSize(const float *size)
{
	plateSize.x = size[0];
	plateSize.y = size[1];
}

bool PlateGeometryObject::ExteriorSphere(const Point3D& center, float radius) const
{
	if (Fabs(center.z) > radius)
	{
		return (true);
	}

	if ((center.x > plateSize.x + radius) || (center.x < -radius))
	{
		return (true);
	}

	if ((center.y > plateSize.y + radius) || (center.y < -radius))
	{
		return (true);
	}

	return (false);
}

bool PlateGeometryObject::ExteriorSweptSphere(const Point3D& p1, const Point3D& p2, float radius) const
{
	if ((p1.z * p2.z > 0.0F) && (Fmin(Fabs(p1.z), Fabs(p2.z)) > radius))
	{
		return (true);
	}

	float sx = plateSize.x + radius;

	if ((p1.x > sx) && (p2.x > sx))
	{
		return (true);
	}

	if ((p1.x < -radius) && (p2.x < -radius))
	{
		return (true);
	}

	float sy = plateSize.y + radius;

	if ((p1.y > sy) && (p2.y > sy))
	{
		return (true);
	}

	if ((p1.y < -radius) && (p2.y < -radius))
	{
		return (true);
	}

	return (false);
}

Point3D PlateGeometryObject::CalculatePrimitiveSupportPoint(const Vector3D& direction) const
{
	float x = (direction.x > 0.0F) ? plateSize.x : 0.0F;
	float y = (direction.y > 0.0F) ? plateSize.y : 0.0F;
	float z = (direction.z > 0.0F) ? 0.0F : kSemiInfiniteIntersectionDepth;
	return (Point3D(x, y, z));
}

void PlateGeometryObject::CalculatePrimitiveSupportPointArray(int32 count, const Vector3D *direction, Point3D *support) const
{
	for (machine a = 0; a < count; a++)
	{
		support->x = (direction->x > 0.0F) ? plateSize.x : 0.0F;
		support->y = (direction->y > 0.0F) ? plateSize.y : 0.0F;
		support->z = (direction->z > 0.0F) ? 0.0F : kSemiInfiniteIntersectionDepth;

		direction++;
		support++;
	}
}

void PlateGeometryObject::BuildPrimitiveLevel(int32 level, const Transformable *transformable)
{
	ArrayDescriptor		desc[4];
	Mesh				mesh;

	int32 xdiv = Max(GetMaxSubdivX() >> level, 1);
	int32 ydiv = Max(GetMaxSubdivY() >> level, 1);

	int32 vertexCount = (xdiv + 1) * (ydiv + 1);
	int32 triangleCount = xdiv * ydiv * 2;

	desc[0].identifier = kArrayPosition;
	desc[0].elementCount = vertexCount;
	desc[0].elementSize = sizeof(Point3D);
	desc[0].componentCount = 3;

	desc[1].identifier = kArrayNormal;
	desc[1].elementCount = vertexCount;
	desc[1].elementSize = sizeof(Vector3D);
	desc[1].componentCount = 3;

	desc[2].identifier = kArrayTexcoord;
	desc[2].elementCount = vertexCount;
	desc[2].elementSize = sizeof(Point2D);
	desc[2].componentCount = 2;

	desc[3].identifier = kArrayPrimitive;
	desc[3].elementCount = triangleCount;
	desc[3].elementSize = sizeof(Triangle);
	desc[3].componentCount = 1;

	mesh.AllocateStorage(vertexCount, 4, desc);

	Point3D *restrict vertex = mesh.GetArray<Point3D>(kArrayPosition) - 1;
	Point2D *restrict texcoord = mesh.GetArray<Point2D>(kArrayTexcoord) - 1;

	float sx = plateSize.x;
	float sy = plateSize.y;
	float dx = 1.0F / (float) xdiv;
	float dy = 1.0F / (float) ydiv;

	for (machine j = 0; j < ydiv; j++)
	{
		float t = (float) j * dy;
		float y = t * sy;

		for (machine i = 0; i < xdiv; i++)
		{
			float s = (float) i * dx;
			float x = s * sx;

			(++vertex)->Set(x, y, 0.0F);
			(++texcoord)->Set(s, t);
		}

		(++vertex)->Set(sx, y, 0.0F);
		(++texcoord)->Set(1.0F, t);
	}

	for (machine i = 0; i < xdiv; i++)
	{
		float s = (float) i * dx;
		float x = s * sx;

		(++vertex)->Set(x, sy, 0.0F);
		(++texcoord)->Set(s, 1.0F);
	}

	(++vertex)->Set(sx, sy, 0.0F);
	(++texcoord)->Set(1.0F, 1.0F);

	Vector3D *restrict normal = mesh.GetArray<Vector3D>(kArrayNormal);
	for (machine a = 0; a < vertexCount; a++)
	{
		normal[a].Set(0.0F, 0.0F, 1.0F);
	}

	Triangle *restrict triangle = mesh.GetArray<Triangle>(kArrayPrimitive);

	for (machine j = 0; j < ydiv; j++)
	{
		int32 k = j * (xdiv + 1);
		for (machine i = 0; i < xdiv; i++)
		{
			if (((i + j) & 1) == 0)
			{
				triangle[0].Set(k + i, k + i + 1, k + i + xdiv + 1);
				triangle[1].Set(k + i + 1, k + i + xdiv + 2, k + i + xdiv + 1);
			}
			else
			{
				triangle[0].Set(k + i, k + i + 1, k + i + xdiv + 2);
				triangle[1].Set(k + i, k + i + xdiv + 2, k + i + xdiv + 1);
			}

			triangle += 2;
		}
	}

	BuildStandardArrays(&mesh, GetGeometryLevel(level), transformable);
}


DiskGeometryObject::DiskGeometryObject() : PrimitiveGeometryObject(kPrimitiveDisk)
{
	SetConvexPrimitiveFlag();
	SetGeometryFlags(GetGeometryFlags() & ~kGeometryCastShadows);

	SetStaticSurfaceData(1, staticSurfaceData);
}

DiskGeometryObject::DiskGeometryObject(const Vector2D& size) : PrimitiveGeometryObject(kPrimitiveDisk)
{
	diskSize = size;
	ratioXY = size.x / size.y;

	SetConvexPrimitiveFlag();
	SetGeometryFlags(GetGeometryFlags() & ~kGeometryCastShadows);

	SetBuildLevelCount(2);
	SetMaxSubdivX(32);

	SetStaticSurfaceData(1, staticSurfaceData, true);
	staticSurfaceData[0].textureAlignData[0].alignMode = kTextureAlignGlobalObjectPlane;
	staticSurfaceData[0].textureAlignData[0].alignPlane.Set(1.0F, 0.0F, 0.0F, 0.0F);
	staticSurfaceData[0].textureAlignData[1].alignMode = kTextureAlignGlobalObjectPlane;
	staticSurfaceData[0].textureAlignData[1].alignPlane.Set(0.0F, 1.0F, 0.0F, 0.0F);
}

DiskGeometryObject::~DiskGeometryObject()
{
}

void DiskGeometryObject::Pack(Packer& data, unsigned_int32 packFlags) const
{
	PrimitiveGeometryObject::Pack(data, packFlags);

	data << ChunkHeader('SIZE', sizeof(Vector2D));
	data << diskSize;

	data << TerminatorChunk;
}

void DiskGeometryObject::Unpack(Unpacker& data, unsigned_int32 unpackFlags)
{
	PrimitiveGeometryObject::Unpack(data, unpackFlags);
	UnpackChunkList<DiskGeometryObject>(data, unpackFlags);
}

bool DiskGeometryObject::UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags)
{
	switch (chunkHeader->chunkType)
	{
		case 'SIZE':

			data >> diskSize;
			ratioXY = diskSize.x / diskSize.y;
			return (true);
	}

	return (false);
}

int32 DiskGeometryObject::GetObjectSize(float *size) const
{
	size[0] = diskSize.x;
	size[1] = diskSize.y;
	return (2);
}

void DiskGeometryObject::SetObjectSize(const float *size)
{
	diskSize.x = size[0];
	diskSize.y = size[1];
	ratioXY = diskSize.x / diskSize.y;
}

bool DiskGeometryObject::ExteriorSphere(const Point3D& center, float radius) const
{
	return (Fabs(center.z) > radius);
}

bool DiskGeometryObject::ExteriorSweptSphere(const Point3D& p1, const Point3D& p2, float radius) const
{
	return ((p1.z * p2.z > 0.0F) && (Fmin(Fabs(p1.z), Fabs(p2.z)) > radius));
}

Point3D DiskGeometryObject::GetInitialPrimitiveSupportPoint(void) const
{
	return (Point3D(diskSize.x, 0.0F, 0.0F));
}

Point3D DiskGeometryObject::CalculatePrimitiveSupportPoint(const Vector3D& direction) const
{
	float x = diskSize.x * diskSize.x * direction.x;
	float y = diskSize.y * diskSize.y * direction.y;
	float z = (direction.z > 0.0F) ? 0.0F : kSemiInfiniteIntersectionDepth;

	float r = x * direction.x + y * direction.y;
	if (r > K::min_float)
	{
		r = InverseSqrt(r);
		return (Point3D(x * r, y * r, z));
	}

	return (Point3D(0.0F, 0.0F, z));
}

void DiskGeometryObject::CalculatePrimitiveSupportPointArray(int32 count, const Vector3D *direction, Point3D *support) const
{
	for (machine a = 0; a < count; a++)
	{
		float x = diskSize.x * diskSize.x * direction->x;
		float y = diskSize.y * diskSize.y * direction->y;
		float z = (direction->z > 0.0F) ? 0.0F : kSemiInfiniteIntersectionDepth;

		float r = x * direction->x + y * direction->y;
		if (r > K::min_float)
		{
			r = InverseSqrt(r);
			support->Set(x * r, y * r, z);
		}
		else
		{
			support->Set(0.0F, 0.0F, z);
		}

		direction++;
		support++;
	}
}

void DiskGeometryObject::BuildPrimitiveLevel(int32 level, const Transformable *transformable)
{
	ArrayDescriptor		desc[4];
	Mesh				mesh;

	desc[0].identifier = kArrayPosition;
	desc[0].elementSize = sizeof(Point3D);
	desc[0].componentCount = 3;

	desc[1].identifier = kArrayNormal;
	desc[1].elementSize = sizeof(Vector3D);
	desc[1].componentCount = 3;

	desc[2].identifier = kArrayTexcoord;
	desc[2].elementSize = sizeof(Point2D);
	desc[2].componentCount = 2;

	desc[3].identifier = kArrayPrimitive;
	desc[3].elementSize = sizeof(Triangle);
	desc[3].componentCount = 1;

	int32 xdiv = Max(GetMaxSubdivX() >> level, 8) & ~1;
	int32 ydiv = Max(GetMaxSubdivY() >> level, 1);

	if (ydiv == 1)
	{
		int32 vertexCount = xdiv;
		int32 triangleCount = xdiv - 2;

		desc[0].elementCount = vertexCount;
		desc[1].elementCount = vertexCount;
		desc[2].elementCount = vertexCount;
		desc[3].elementCount = triangleCount;

		mesh.AllocateStorage(vertexCount, 4, desc);

		Point3D *restrict vertex = mesh.GetArray<Point3D>(kArrayPosition) - 1;
		Point2D *restrict texcoord = mesh.GetArray<Point2D>(kArrayTexcoord) - 1;

		float rx = diskSize.x;
		float tex = 0.5F / rx;
		float dt = K::tau / (float) xdiv;

		for (machine i = 0; i < xdiv; i++)
		{
			Vector2D v = CosSin((float) i * dt) & diskSize;

			(++vertex)->Set(v, 0.0F);
			(++texcoord)->Set((v.x + rx) * tex, (v.y * ratioXY + rx) * tex);
		}

		TriangulateDisk(xdiv, 0, mesh.GetArray<Triangle>(kArrayPrimitive));
	}
	else
	{
		int32 vertexCount = xdiv * ydiv + 1;
		int32 triangleCount = xdiv * ydiv * 2 - xdiv;

		desc[0].elementCount = vertexCount;
		desc[1].elementCount = vertexCount;
		desc[2].elementCount = vertexCount;
		desc[3].elementCount = triangleCount;

		mesh.AllocateStorage(vertexCount, 4, desc);

		Point3D *restrict vertex = mesh.GetArray<Point3D>(kArrayPosition);
		Point2D *restrict texcoord = mesh.GetArray<Point2D>(kArrayTexcoord);

		vertex->Set(0.0F, 0.0F, 0.0F);
		texcoord->Set(0.5F, 0.5F);

		float rx = diskSize.x;
		float ry = diskSize.y;
		float tex = 0.5F / rx;
		float dt = K::tau / (float) xdiv;
		float dr = 1.0F / (float) ydiv;

		for (machine i = 0; i < xdiv; i++)
		{
			Vector2D v = CosSin((float) i * dt);

			for (machine j = 1; j <= ydiv; j++)
			{
				float r = (float) j * dr;
				float x = rx * r * v.x;
				float y = ry * r * v.y;

				(++vertex)->Set(x, y, 0.0F);
				(++texcoord)->Set((x + rx) * tex, (y * ratioXY + rx) * tex);
			}
		}

		Triangle *restrict triangle = mesh.GetArray<Triangle>(kArrayPrimitive);

		for (machine i = 0; i < xdiv; i++)
		{
			int32 k = i * ydiv;
			int32 n = (i != xdiv - 1) ? ydiv : (1 - xdiv) * ydiv;

			triangle->Set(0, k + 1, k + n + 1);
			triangle++;

			for (machine j = 1; j < ydiv; j++)
			{
				if (((i + j) & 1) == 0)
				{
					triangle[0].Set(k + j, k + j + 1, k + n + j + 1);
					triangle[1].Set(k + j, k + n + j + 1, k + n + j);
				}
				else
				{
					triangle[0].Set(k + j, k + j + 1, k + n + j);
					triangle[1].Set(k + j + 1, k + n + j + 1, k + n + j);
				}

				triangle += 2;
			}
		}
	}

	int32 vertexCount = mesh.GetVertexCount();
	Vector3D *restrict normal = mesh.GetArray<Vector3D>(kArrayNormal);
	for (machine a = 0; a < vertexCount; a++)
	{
		normal[a].Set(0.0F, 0.0F, 1.0F);
	}

	BuildStandardArrays(&mesh, GetGeometryLevel(level), transformable);
}


HoleGeometryObject::HoleGeometryObject() : PrimitiveGeometryObject(kPrimitiveHole)
{
	SetGeometryFlags(GetGeometryFlags() & ~kGeometryCastShadows);

	SetStaticSurfaceData(1, staticSurfaceData);
}

HoleGeometryObject::HoleGeometryObject(const Vector2D& inner, const Vector2D& outer) : PrimitiveGeometryObject(kPrimitiveHole)
{
	innerSize = inner;
	outerSize = outer;
	innerRatioXY = inner.x / inner.y;

	SetGeometryFlags(GetGeometryFlags() & ~kGeometryCastShadows);

	SetBuildLevelCount(2);
	SetMaxSubdivX(32);

	SetStaticSurfaceData(1, staticSurfaceData, true);
	staticSurfaceData[0].textureAlignData[0].alignMode = kTextureAlignGlobalObjectPlane;
	staticSurfaceData[0].textureAlignData[0].alignPlane.Set(1.0F, 0.0F, 0.0F, 0.0F);
	staticSurfaceData[0].textureAlignData[1].alignMode = kTextureAlignGlobalObjectPlane;
	staticSurfaceData[0].textureAlignData[1].alignPlane.Set(0.0F, 1.0F, 0.0F, 0.0F);
}

HoleGeometryObject::~HoleGeometryObject()
{
}

void HoleGeometryObject::Pack(Packer& data, unsigned_int32 packFlags) const
{
	PrimitiveGeometryObject::Pack(data, packFlags);

	data << ChunkHeader('SIZE', sizeof(Vector2D) * 2);
	data << innerSize;
	data << outerSize;

	data << TerminatorChunk;
}

void HoleGeometryObject::Unpack(Unpacker& data, unsigned_int32 unpackFlags)
{
	PrimitiveGeometryObject::Unpack(data, unpackFlags);
	UnpackChunkList<HoleGeometryObject>(data, unpackFlags);
}

bool HoleGeometryObject::UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags)
{
	switch (chunkHeader->chunkType)
	{
		case 'SIZE':

			data >> innerSize;
			data >> outerSize;
			innerRatioXY = innerSize.x / innerSize.y;
			return (true);
	}

	return (false);
}

int32 HoleGeometryObject::GetObjectSize(float *size) const
{
	size[0] = innerSize.x;
	size[1] = innerSize.y;
	size[2] = outerSize.x;
	size[3] = outerSize.y;
	return (4);
}

void HoleGeometryObject::SetObjectSize(const float *size)
{
	innerSize.x = size[0];
	innerSize.y = size[1];
	outerSize.x = size[2];
	outerSize.y = size[3];
	innerRatioXY = innerSize.x / innerSize.y;
}

bool HoleGeometryObject::ExteriorSphere(const Point3D& center, float radius) const
{
	if (Fabs(center.z) > radius)
	{
		return (true);
	}

	float rx = outerSize.x + radius;
	if ((center.x > rx) || (center.x < -rx))
	{
		return (true);
	}

	float ry = outerSize.y + radius;
	if ((center.y > ry) || (center.y < -ry))
	{
		return (true);
	}

	return (false);
}

bool HoleGeometryObject::ExteriorSweptSphere(const Point3D& p1, const Point3D& p2, float radius) const
{
	if ((p1.z * p2.z > 0.0F) && (Fmin(Fabs(p1.z), Fabs(p2.z)) > radius))
	{
		return (true);
	}

	float rx = outerSize.x + radius;

	if ((p1.x > rx) && (p2.x > rx))
	{
		return (true);
	}

	if ((p1.x < -rx) && (p2.x < -rx))
	{
		return (true);
	}

	float ry = outerSize.y + radius;

	if ((p1.y > ry) && (p2.y > ry))
	{
		return (true);
	}

	if ((p1.y < -ry) && (p2.y < -ry))
	{
		return (true);
	}

	return (false);
}

void HoleGeometryObject::BuildPrimitiveLevel(int32 level, const Transformable *transformable)
{
	ArrayDescriptor		desc[4];
	Mesh				mesh;

	int32 xdiv = Max(GetMaxSubdivX() >> level, 8) & ~1;

	int32 vertexCount = xdiv + 4;
	int32 triangleCount = xdiv + 4;

	desc[0].identifier = kArrayPosition;
	desc[0].elementCount = vertexCount;
	desc[0].elementSize = sizeof(Point3D);
	desc[0].componentCount = 3;

	desc[1].identifier = kArrayNormal;
	desc[1].elementCount = vertexCount;
	desc[1].elementSize = sizeof(Vector3D);
	desc[1].componentCount = 3;

	desc[2].identifier = kArrayTexcoord;
	desc[2].elementCount = vertexCount;
	desc[2].elementSize = sizeof(Point2D);
	desc[2].componentCount = 2;

	desc[3].identifier = kArrayPrimitive;
	desc[3].elementCount = triangleCount;
	desc[3].elementSize = sizeof(Triangle);
	desc[3].componentCount = 1;

	mesh.AllocateStorage(vertexCount, 4, desc);

	Point3D *restrict vertex = mesh.GetArray<Point3D>(kArrayPosition);
	Point2D *restrict texcoord = mesh.GetArray<Point2D>(kArrayTexcoord);

	float rx = outerSize.x;
	float ry = outerSize.y;

	vertex[0].Set(rx, ry, 0.0F);
	texcoord[0].Set(1.0F, 1.0F);

	vertex[1].Set(-rx, ry, 0.0F);
	texcoord[1].Set(0.0F, 1.0F);

	vertex[2].Set(-rx, -ry, 0.0F);
	texcoord[2].Set(0.0F, 0.0F);

	vertex[3].Set(rx, -ry, 0.0F);
	texcoord[3].Set(1.0F, 0.0F);

	vertex += 3;
	texcoord += 3;

	float tx = 0.5F / rx;
	float ty = 0.5F / ry;
	float dt = K::tau / (float) xdiv;
	for (machine i = 0; i < xdiv; i++)
	{
		Vector2D v = CosSin((float) i * dt) & innerSize;

		(++vertex)->Set(v, 0.0F);
		(++texcoord)->Set((v.x + rx) * tx, (v.y + ry) * ty);
	}

	Vector3D *restrict normal = mesh.GetArray<Vector3D>(kArrayNormal);
	for (machine a = 0; a < vertexCount; a++)
	{
		normal[a].Set(0.0F, 0.0F, 1.0F);
	}

	Triangle *restrict triangle = mesh.GetArray<Triangle>(kArrayPrimitive);

	int32 d = xdiv >> 2;
	for (machine q = 0; q < 4; q++)
	{
		int32 k = q * d;

		triangle->Set(q, k + 4, (q - 1) & 3);
		triangle++;

		for (machine i = 0; i < d; i++)
		{
			triangle->Set(q, (k + i + 1) % xdiv + 4, k + i + 4);
			triangle++;
		}
	}

	BuildStandardArrays(&mesh, GetGeometryLevel(level), transformable);
}


AnnulusGeometryObject::AnnulusGeometryObject() : PrimitiveGeometryObject(kPrimitiveAnnulus)
{
	SetGeometryFlags(GetGeometryFlags() & ~kGeometryCastShadows);

	SetStaticSurfaceData(1, staticSurfaceData);
}

AnnulusGeometryObject::AnnulusGeometryObject(const Vector2D& inner, const Vector2D& outer) : PrimitiveGeometryObject(kPrimitiveAnnulus)
{
	innerSize = inner;
	outerSize = outer;
	innerRatioXY = inner.x / inner.y;
	outerRatioXY = outer.x / outer.y;

	SetGeometryFlags(GetGeometryFlags() & ~kGeometryCastShadows);

	SetBuildLevelCount(2);
	SetMaxSubdivX(32);

	SetStaticSurfaceData(1, staticSurfaceData, true);
	staticSurfaceData[0].textureAlignData[0].alignMode = kTextureAlignNatural;
	staticSurfaceData[0].textureAlignData[0].alignPlane.Set(1.0F, 0.0F, 0.0F, 0.0F);
	staticSurfaceData[0].textureAlignData[1].alignMode = kTextureAlignNatural;
	staticSurfaceData[0].textureAlignData[1].alignPlane.Set(0.0F, 1.0F, 0.0F, 0.0F);
}

AnnulusGeometryObject::~AnnulusGeometryObject()
{
}

void AnnulusGeometryObject::Pack(Packer& data, unsigned_int32 packFlags) const
{
	PrimitiveGeometryObject::Pack(data, packFlags);

	data << ChunkHeader('SIZE', sizeof(Vector2D) * 2);
	data << innerSize;
	data << outerSize;

	data << TerminatorChunk;
}

void AnnulusGeometryObject::Unpack(Unpacker& data, unsigned_int32 unpackFlags)
{
	PrimitiveGeometryObject::Unpack(data, unpackFlags);
	UnpackChunkList<AnnulusGeometryObject>(data, unpackFlags);
}

bool AnnulusGeometryObject::UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags)
{
	switch (chunkHeader->chunkType)
	{
		case 'SIZE':

			data >> innerSize;
			data >> outerSize;
			innerRatioXY = innerSize.x / innerSize.y;
			outerRatioXY = outerSize.x / outerSize.y;
			return (true);
	}

	return (false);
}

int32 AnnulusGeometryObject::GetObjectSize(float *size) const
{
	size[0] = innerSize.x;
	size[1] = innerSize.y;
	size[2] = outerSize.x;
	size[3] = outerSize.y;
	return (4);
}

void AnnulusGeometryObject::SetObjectSize(const float *size)
{
	innerSize.x = size[0];
	innerSize.y = size[1];
	outerSize.x = size[2];
	outerSize.y = size[3];
	innerRatioXY = innerSize.x / innerSize.y;
	outerRatioXY = outerSize.x / outerSize.y;
}

bool AnnulusGeometryObject::ExteriorSphere(const Point3D& center, float radius) const
{
	return (Fabs(center.z) > radius);
}

bool AnnulusGeometryObject::ExteriorSweptSphere(const Point3D& p1, const Point3D& p2, float radius) const
{
	return ((p1.z * p2.z > 0.0F) && (Fmin(Fabs(p1.z), Fabs(p2.z)) > radius));
}

void AnnulusGeometryObject::BuildPrimitiveLevel(int32 level, const Transformable *transformable)
{
	ArrayDescriptor		desc[4];
	Mesh				mesh;

	int32 xdiv = Max(GetMaxSubdivX() >> level, 8) & ~1;
	int32 ydiv = Max(GetMaxSubdivY() >> level, 1);

	int32 vertexCount = xdiv * (ydiv + 1);
	int32 triangleCount = xdiv * ydiv * 2;

	desc[0].identifier = kArrayPosition;
	desc[0].elementCount = vertexCount;
	desc[0].elementSize = sizeof(Point3D);
	desc[0].componentCount = 3;

	desc[1].identifier = kArrayNormal;
	desc[1].elementCount = vertexCount;
	desc[1].elementSize = sizeof(Vector3D);
	desc[1].componentCount = 3;

	desc[2].identifier = kArrayTexcoord;
	desc[2].elementCount = vertexCount;
	desc[2].elementSize = sizeof(Point2D);
	desc[2].componentCount = 2;

	desc[3].identifier = kArrayPrimitive;
	desc[3].elementCount = triangleCount;
	desc[3].elementSize = sizeof(Triangle);
	desc[3].componentCount = 1;

	mesh.AllocateStorage(vertexCount, 4, desc);

	Point3D *restrict vertex = mesh.GetArray<Point3D>(kArrayPosition) - 1;
	Point2D *restrict texcoord = mesh.GetArray<Point2D>(kArrayTexcoord) - 1;

	float rx1 = innerSize.x;
	float ry1 = innerSize.y;
	float rx2 = outerSize.x;
	float ry2 = outerSize.y;

	float tex = 0.5F / rx2;
	float dt = K::tau / (float) xdiv;
	float dr = 1.0F / (float) ydiv;

	for (machine i = 0; i < xdiv; i++)
	{
		Vector2D v = CosSin((float) i * dt);

		for (machine j = 0; j <= ydiv; j++)
		{
			float r = (float) j * dr;
			float x = (rx1 * (1.0F - r) + rx2 * r) * v.x;
			float y = (ry1 * (1.0F - r) + ry2 * r) * v.y;

			(++vertex)->Set(x, y, 0.0F);
			(++texcoord)->Set((x + rx2) * tex, (y * outerRatioXY + rx2) * tex);
		}
	}

	Vector3D *restrict normal = mesh.GetArray<Vector3D>(kArrayNormal);
	for (machine a = 0; a < vertexCount; a++)
	{
		normal[a].Set(0.0F, 0.0F, 1.0F);
	}

	Triangle *restrict triangle = mesh.GetArray<Triangle>(kArrayPrimitive);

	for (machine i = 0; i < xdiv; i++)
	{
		int32 k = i * (ydiv + 1);
		int32 n = (i != xdiv - 1) ? ydiv + 1 : (1 - xdiv) * (ydiv + 1);

		for (machine j = 0; j < ydiv; j++)
		{
			if (((i + j) & 1) == 0)
			{
				triangle[0].Set(k + j, k + j + 1, k + n + j + 1);
				triangle[1].Set(k + j, k + n + j + 1, k + n + j);
			}
			else
			{
				triangle[0].Set(k + j, k + j + 1, k + n + j);
				triangle[1].Set(k + j + 1, k + n + j + 1, k + n + j);
			}

			triangle += 2;
		}
	}

	BuildStandardArrays(&mesh, GetGeometryLevel(level), transformable);
}


BoxGeometryObject::BoxGeometryObject() : PrimitiveGeometryObject(kPrimitiveBox)
{
	SetConvexPrimitiveFlag();
	SetStaticSurfaceData(6, staticSurfaceData);
}

BoxGeometryObject::BoxGeometryObject(const Vector3D& size) : PrimitiveGeometryObject(kPrimitiveBox)
{
	boxSize = size;

	SetConvexPrimitiveFlag();
	SetStaticSurfaceData(6, staticSurfaceData, true);

	staticSurfaceData[0].textureAlignData[0].alignMode = kTextureAlignGlobalObjectPlane;
	staticSurfaceData[0].textureAlignData[0].alignPlane.Set(0.0F, 1.0F, 0.0F, 0.0F);
	staticSurfaceData[0].textureAlignData[1].alignMode = kTextureAlignGlobalObjectPlane;
	staticSurfaceData[0].textureAlignData[1].alignPlane.Set(0.0F, 0.0F, 1.0F, 0.0F);

	staticSurfaceData[1].textureAlignData[0].alignMode = kTextureAlignGlobalObjectPlane;
	staticSurfaceData[1].textureAlignData[0].alignPlane.Set(-1.0F, 0.0F, 0.0F, 0.0F);
	staticSurfaceData[1].textureAlignData[1].alignMode = kTextureAlignGlobalObjectPlane;
	staticSurfaceData[1].textureAlignData[1].alignPlane.Set(0.0F, 0.0F, 1.0F, 0.0F);

	staticSurfaceData[2].textureAlignData[0].alignMode = kTextureAlignGlobalObjectPlane;
	staticSurfaceData[2].textureAlignData[0].alignPlane.Set(0.0F, -1.0F, 0.0F, 0.0F);
	staticSurfaceData[2].textureAlignData[1].alignMode = kTextureAlignGlobalObjectPlane;
	staticSurfaceData[2].textureAlignData[1].alignPlane.Set(0.0F, 0.0F, 1.0F, 0.0F);

	staticSurfaceData[3].textureAlignData[0].alignMode = kTextureAlignGlobalObjectPlane;
	staticSurfaceData[3].textureAlignData[0].alignPlane.Set(1.0F, 0.0F, 0.0F, 0.0F);
	staticSurfaceData[3].textureAlignData[1].alignMode = kTextureAlignGlobalObjectPlane;
	staticSurfaceData[3].textureAlignData[1].alignPlane.Set(0.0F, 0.0F, 1.0F, 0.0F);

	staticSurfaceData[4].textureAlignData[0].alignMode = kTextureAlignGlobalObjectPlane;
	staticSurfaceData[4].textureAlignData[0].alignPlane.Set(-1.0F, 0.0F, 0.0F, 0.0F);
	staticSurfaceData[4].textureAlignData[1].alignMode = kTextureAlignGlobalObjectPlane;
	staticSurfaceData[4].textureAlignData[1].alignPlane.Set(0.0F, 1.0F, 0.0F, 0.0F);

	staticSurfaceData[5].textureAlignData[0].alignMode = kTextureAlignGlobalObjectPlane;
	staticSurfaceData[5].textureAlignData[0].alignPlane.Set(1.0F, 0.0F, 0.0F, 0.0F);
	staticSurfaceData[5].textureAlignData[1].alignMode = kTextureAlignGlobalObjectPlane;
	staticSurfaceData[5].textureAlignData[1].alignPlane.Set(0.0F, 1.0F, 0.0F, 0.0F);
}

BoxGeometryObject::~BoxGeometryObject()
{
}

void BoxGeometryObject::Pack(Packer& data, unsigned_int32 packFlags) const
{
	PrimitiveGeometryObject::Pack(data, packFlags);

	data << ChunkHeader('SIZE', sizeof(Vector3D));
	data << boxSize;

	data << TerminatorChunk;
}

void BoxGeometryObject::Unpack(Unpacker& data, unsigned_int32 unpackFlags)
{
	PrimitiveGeometryObject::Unpack(data, unpackFlags);
	UnpackChunkList<BoxGeometryObject>(data, unpackFlags);
}

bool BoxGeometryObject::UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags)
{
	switch (chunkHeader->chunkType)
	{
		case 'SIZE':

			data >> boxSize;
			return (true);
	}

	return (false);
}

int32 BoxGeometryObject::GetObjectSize(float *size) const
{
	size[0] = boxSize.x;
	size[1] = boxSize.y;
	size[2] = boxSize.z;
	return (3);
}

void BoxGeometryObject::SetObjectSize(const float *size)
{
	boxSize.x = size[0];
	boxSize.y = size[1];
	boxSize.z = size[2];
}

bool BoxGeometryObject::ExteriorSphere(const Point3D& center, float radius) const
{
	if ((center.z < -radius) || (center.z > boxSize.z + radius))
	{
		return (true);
	}

	if ((center.y < -radius) || (center.y > boxSize.y + radius))
	{
		return (true);
	}

	if ((center.x < -radius) || (center.x > boxSize.x + radius))
	{
		return (true);
	}

	return (false);
}

bool BoxGeometryObject::ExteriorSweptSphere(const Point3D& p1, const Point3D& p2, float radius) const
{
	float sz = boxSize.z + radius;

	if ((p1.z > sz) && (p2.z > sz))
	{
		return (true);
	}

	if ((p1.z < -radius) && (p2.z < -radius))
	{
		return (true);
	}

	float sy = boxSize.y + radius;

	if ((p1.y > sy) && (p2.y > sy))
	{
		return (true);
	}

	if ((p1.y < -radius) && (p2.y < -radius))
	{
		return (true);
	}

	float sx = boxSize.x + radius;

	if ((p1.x > sx) && (p2.x > sx))
	{
		return (true);
	}

	if ((p1.x < -radius) && (p2.x < -radius))
	{
		return (true);
	}

	return (false);
}

Point3D BoxGeometryObject::CalculatePrimitiveSupportPoint(const Vector3D& direction) const
{
	#if C4SIMD

		Point3D		result;

		vec_float size = VecLoadUnaligned(&boxSize.x);
		vec_float mask = VecMaskCmpgt(VecLoadUnaligned(&direction.x), VecFloatGetZero());
		VecStore3D(VecAnd(size, mask), &result.x);
		return (result);

	#else

		float x = (direction.x > 0.0F) ? boxSize.x : 0.0F;
		float y = (direction.y > 0.0F) ? boxSize.y : 0.0F;
		float z = (direction.z > 0.0F) ? boxSize.z : 0.0F;
		return (Point3D(x, y, z));

	#endif
}

void BoxGeometryObject::CalculatePrimitiveSupportPointArray(int32 count, const Vector3D *direction, Point3D *support) const
{
	#if C4SIMD

		vec_float zero = VecFloatGetZero();
		vec_float size = VecLoadUnaligned(&boxSize.x);

		for (machine a = 0; a < count; a++)
		{
			vec_float mask = VecMaskCmpgt(VecLoadUnaligned(&direction->x), zero);
			VecStore3D(VecAnd(size, mask), &support->x);

			direction++;
			support++;
		}

	#else

		for (machine a = 0; a < count; a++)
		{
			support->x = (direction->x > 0.0F) ? boxSize.x : 0.0F;
			support->y = (direction->y > 0.0F) ? boxSize.y : 0.0F;
			support->z = (direction->z > 0.0F) ? boxSize.z : 0.0F;

			direction++;
			support++;
		}

	#endif
}

void BoxGeometryObject::BuildPrimitiveLevel(int32 level, const Transformable *transformable)
{
	ArrayDescriptor		desc[5];
	Mesh				mesh;

	int32 xdiv = Max(GetMaxSubdivX() >> level, 1);
	int32 zdiv = Max(GetMaxSubdivY() >> level, 1);

	int32 vertexCount = (xdiv + 1) * (zdiv + 1) * 4;
	int32 triangleCount = xdiv * zdiv * 8;

	bool endcap = ((GetPrimitiveFlags() & kPrimitiveEndcap) != 0);
	if (endcap)
	{
		vertexCount += (xdiv + 1) * (xdiv + 1) * 2;
		triangleCount += xdiv * xdiv * 4;
	}

	desc[0].identifier = kArrayPosition;
	desc[0].elementCount = vertexCount;
	desc[0].elementSize = sizeof(Point3D);
	desc[0].componentCount = 3;

	desc[1].identifier = kArrayNormal;
	desc[1].elementCount = vertexCount;
	desc[1].elementSize = sizeof(Vector3D);
	desc[1].componentCount = 3;

	desc[2].identifier = kArrayTexcoord;
	desc[2].elementCount = vertexCount;
	desc[2].elementSize = sizeof(Point2D);
	desc[2].componentCount = 2;

	desc[3].identifier = kArraySurfaceIndex;
	desc[3].elementCount = vertexCount;
	desc[3].elementSize = 2;
	desc[3].componentCount = 1;

	desc[4].identifier = kArrayPrimitive;
	desc[4].elementCount = triangleCount;
	desc[4].elementSize = sizeof(Triangle);
	desc[4].componentCount = 1;

	mesh.AllocateStorage(vertexCount, 5, desc);

	Point3D *restrict vertex = mesh.GetArray<Point3D>(kArrayPosition) - 1;
	Point2D *restrict texcoord = mesh.GetArray<Point2D>(kArrayTexcoord) - 1;

	float sx = boxSize.x;
	float sy = boxSize.y;
	float sz = boxSize.z;
	float dx = 1.0F / (float) xdiv;
	float dz = 1.0F / (float) zdiv;

	for (machine j = 0; j < zdiv; j++)
	{
		float t = (float) j * dz;
		float z = t * sz;

		for (machine i = 0; i < xdiv; i++)
		{
			float s = (float) i * dx;
			float y = s * sy;

			(++vertex)->Set(sx, y, z);
			(++texcoord)->Set(s, t);
		}

		(++vertex)->Set(sx, sy, z);
		(++texcoord)->Set(1.0F, t);
	}

	for (machine i = 0; i < xdiv; i++)
	{
		float s = (float) i * dx;
		float y = s * sy;

		(++vertex)->Set(sx, y, sz);
		(++texcoord)->Set(s, 1.0F);
	}

	(++vertex)->Set(sx, sy, sz);
	(++texcoord)->Set(1.0F, 1.0F);

	for (machine j = 0; j < zdiv; j++)
	{
		float t = (float) j * dz;
		float z = t * sz;

		for (machine i = 0; i < xdiv; i++)
		{
			float s = (float) (xdiv - i) * dx;
			float x = s * sx;

			(++vertex)->Set(x, sy, z);
			(++texcoord)->Set(1.0F - s, t);
		}

		(++vertex)->Set(0.0F, sy, z);
		(++texcoord)->Set(1.0F, t);
	}

	for (machine i = 0; i < xdiv; i++)
	{
		float s = (float) (xdiv - i) * dx;
		float x = s * sx;

		(++vertex)->Set(x, sy, sz);
		(++texcoord)->Set(1.0F - s, 1.0F);
	}

	(++vertex)->Set(0.0F, sy, sz);
	(++texcoord)->Set(1.0F, 1.0F);

	for (machine j = 0; j < zdiv; j++)
	{
		float t = (float) j * dz;
		float z = t * sz;

		for (machine i = 0; i < xdiv; i++)
		{
			float s = (float) (xdiv - i) * dx;
			float y = s * sy;

			(++vertex)->Set(0.0F, y, z);
			(++texcoord)->Set(1.0F - s, t);
		}

		(++vertex)->Set(0.0F, 0.0F, z);
		(++texcoord)->Set(1.0F, t);
	}

	for (machine i = 0; i < xdiv; i++)
	{
		float s = (float) (xdiv - i) * dx;
		float y = s * sy;

		(++vertex)->Set(0.0F, y, sz);
		(++texcoord)->Set(1.0F - s, 1.0F);
	}

	(++vertex)->Set(0.0F, 0.0F, sz);
	(++texcoord)->Set(1.0F, 1.0F);

	for (machine j = 0; j < zdiv; j++)
	{
		float t = (float) j * dz;
		float z = t * sz;

		for (machine i = 0; i < xdiv; i++)
		{
			float s = (float) i * dx;
			float x = s * sx;

			(++vertex)->Set(x, 0.0F, z);
			(++texcoord)->Set(s, t);
		}

		(++vertex)->Set(sx, 0.0F, z);
		(++texcoord)->Set(1.0F, t);
	}

	for (machine i = 0; i < xdiv; i++)
	{
		float s = (float) i * dx;
		float x = s * sx;

		(++vertex)->Set(x, 0.0F, sz);
		(++texcoord)->Set(s, 1.0F);
	}

	(++vertex)->Set(sx, 0.0F, sz);
	(++texcoord)->Set(1.0F, 1.0F);

	int32 count = (xdiv + 1) * (zdiv + 1);
	Vector3D *restrict normal = mesh.GetArray<Vector3D>(kArrayNormal) - 1;
	unsigned_int16 *restrict surfaceIndex = mesh.GetArray<unsigned_int16>(kArraySurfaceIndex) - 1;

	for (machine a = 0; a < count; a++)
	{
		(++normal)->Set(1.0F, 0.0F, 0.0F);
		*++surfaceIndex = 0;
	}

	for (machine a = 0; a < count; a++)
	{
		(++normal)->Set(0.0F, 1.0F, 0.0F);
		*++surfaceIndex = 1;
	}

	for (machine a = 0; a < count; a++)
	{
		(++normal)->Set(-1.0F, 0.0F, 0.0F);
		*++surfaceIndex = 2;
	}

	for (machine a = 0; a < count; a++)
	{
		(++normal)->Set(0.0F, -1.0F, 0.0F);
		*++surfaceIndex = 3;
	}

	Triangle *restrict triangle = mesh.GetArray<Triangle>(kArrayPrimitive);

	int32 n = 0;
	for (machine a = 0; a < 4; a++)
	{
		for (machine j = 0; j < zdiv; j++)
		{
			int32 k = n + j * (xdiv + 1);
			for (machine i = 0; i < xdiv; i++)
			{
				if (((i + j) & 1) == 0)
				{
					triangle[0].Set(k + i, k + i + 1, k + i + xdiv + 1);
					triangle[1].Set(k + i + 1, k + i + xdiv + 2, k + i + xdiv + 1);
				}
				else
				{
					triangle[0].Set(k + i, k + i + 1, k + i + xdiv + 2);
					triangle[1].Set(k + i, k + i + xdiv + 2, k + i + xdiv + 1);
				}

				triangle += 2;
			}
		}

		n += (xdiv + 1) * (zdiv + 1);
	}

	if (endcap)
	{
		for (machine j = 0; j < xdiv; j++)
		{
			float t = (float) j * dx;
			float y = t * sy;

			for (machine i = 0; i < xdiv; i++)
			{
				float s = (float) (xdiv - i) * dx;
				float x = s * sx;

				(++vertex)->Set(x, y, 0.0F);
				(++texcoord)->Set(1.0F - s, t);
			}

			(++vertex)->Set(0.0F, y, 0.0F);
			(++texcoord)->Set(1.0F, t);
		}

		for (machine i = 0; i < xdiv; i++)
		{
			float s = (float) (xdiv - i) * dx;
			float x = s * sx;

			(++vertex)->Set(x, sy, 0.0F);
			(++texcoord)->Set(1.0F - s, 1.0F);
		}

		(++vertex)->Set(0.0F, sy, 0.0F);
		(++texcoord)->Set(1.0F, 1.0F);

		for (machine j = 0; j < xdiv; j++)
		{
			float t = (float) j * dx;
			float y = t * sy;

			for (machine i = 0; i < xdiv; i++)
			{
				float s = (float) i * dx;
				float x = s * sx;

				(++vertex)->Set(x, y, sz);
				(++texcoord)->Set(s, t);
			}

			(++vertex)->Set(sx, y, sz);
			(++texcoord)->Set(1.0F, t);
		}

		for (machine i = 0; i < xdiv; i++)
		{
			float s = (float) i * dx;
			float x = s * sx;

			(++vertex)->Set(x, sy, sz);
			(++texcoord)->Set(s, 1.0F);
		}

		(++vertex)->Set(sx, sy, sz);
		(++texcoord)->Set(1.0F, 1.0F);

		count = (xdiv + 1) * (xdiv + 1);

		for (machine a = 0; a < count; a++)
		{
			(++normal)->Set(0.0F, 0.0F, -1.0F);
			*++surfaceIndex = 4;
		}

		for (machine a = 0; a < count; a++)
		{
			(++normal)->Set(0.0F, 0.0F, 1.0F);
			*++surfaceIndex = 5;
		}

		for (machine a = 0; a < 2; a++)
		{
			for (machine j = 0; j < xdiv; j++)
			{
				int32 k = n + j * (xdiv + 1);
				for (machine i = 0; i < xdiv; i++)
				{
					if (((i + j) & 1) == 0)
					{
						triangle[0].Set(k + i, k + i + 1, k + i + xdiv + 1);
						triangle[1].Set(k + i + 1, k + i + xdiv + 2, k + i + xdiv + 1);
					}
					else
					{
						triangle[0].Set(k + i, k + i + 1, k + i + xdiv + 2);
						triangle[1].Set(k + i, k + i + xdiv + 2, k + i + xdiv + 1);
					}

					triangle += 2;
				}
			}

			n += (xdiv + 1) * (xdiv + 1);
		}
	}

	BuildStandardArrays(&mesh, GetGeometryLevel(level), transformable);
}


PyramidGeometryObject::PyramidGeometryObject() : PrimitiveGeometryObject(kPrimitivePyramid)
{
	SetConvexPrimitiveFlag();
	SetStaticSurfaceData(5, staticSurfaceData);
}

PyramidGeometryObject::PyramidGeometryObject(const Vector2D& size, float height) : PrimitiveGeometryObject(kPrimitivePyramid)
{
	pyramidSize = size;
	pyramidHeight = height;

	SetConvexPrimitiveFlag();
	SetStaticSurfaceData(5, staticSurfaceData, true);

	staticSurfaceData[0].textureAlignData[0].alignMode = kTextureAlignGlobalObjectPlane;
	staticSurfaceData[0].textureAlignData[0].alignPlane.Set(0.0F, 1.0F, 0.0F, 0.0F);
	staticSurfaceData[0].textureAlignData[1].alignMode = kTextureAlignGlobalObjectPlane;
	staticSurfaceData[0].textureAlignData[1].alignPlane.Set(0.0F, 0.0F, 1.0F, 0.0F);

	staticSurfaceData[1].textureAlignData[0].alignMode = kTextureAlignGlobalObjectPlane;
	staticSurfaceData[1].textureAlignData[0].alignPlane.Set(-1.0F, 0.0F, 0.0F, 0.0F);
	staticSurfaceData[1].textureAlignData[1].alignMode = kTextureAlignGlobalObjectPlane;
	staticSurfaceData[1].textureAlignData[1].alignPlane.Set(0.0F, 0.0F, 1.0F, 0.0F);

	staticSurfaceData[2].textureAlignData[0].alignMode = kTextureAlignGlobalObjectPlane;
	staticSurfaceData[2].textureAlignData[0].alignPlane.Set(0.0F, -1.0F, 0.0F, 0.0F);
	staticSurfaceData[2].textureAlignData[1].alignMode = kTextureAlignGlobalObjectPlane;
	staticSurfaceData[2].textureAlignData[1].alignPlane.Set(0.0F, 0.0F, 1.0F, 0.0F);

	staticSurfaceData[3].textureAlignData[0].alignMode = kTextureAlignGlobalObjectPlane;
	staticSurfaceData[3].textureAlignData[0].alignPlane.Set(1.0F, 0.0F, 0.0F, 0.0F);
	staticSurfaceData[3].textureAlignData[1].alignMode = kTextureAlignGlobalObjectPlane;
	staticSurfaceData[3].textureAlignData[1].alignPlane.Set(0.0F, 0.0F, 1.0F, 0.0F);

	staticSurfaceData[4].textureAlignData[0].alignMode = kTextureAlignGlobalObjectPlane;
	staticSurfaceData[4].textureAlignData[0].alignPlane.Set(-1.0F, 0.0F, 0.0F, 0.0F);
	staticSurfaceData[4].textureAlignData[1].alignMode = kTextureAlignGlobalObjectPlane;
	staticSurfaceData[4].textureAlignData[1].alignPlane.Set(0.0F, 1.0F, 0.0F, 0.0F);
}

PyramidGeometryObject::~PyramidGeometryObject()
{
}

void PyramidGeometryObject::Pack(Packer& data, unsigned_int32 packFlags) const
{
	PrimitiveGeometryObject::Pack(data, packFlags);

	data << ChunkHeader('SIZE', sizeof(Vector2D) + 4);
	data << pyramidSize;
	data << pyramidHeight;

	data << TerminatorChunk;
}

void PyramidGeometryObject::Unpack(Unpacker& data, unsigned_int32 unpackFlags)
{
	PrimitiveGeometryObject::Unpack(data, unpackFlags);
	UnpackChunkList<PyramidGeometryObject>(data, unpackFlags);
}

bool PyramidGeometryObject::UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags)
{
	switch (chunkHeader->chunkType)
	{
		case 'SIZE':

			data >> pyramidSize;
			data >> pyramidHeight;
			return (true);
	}

	return (false);
}

int32 PyramidGeometryObject::GetObjectSize(float *size) const
{
	size[0] = pyramidSize.x;
	size[1] = pyramidSize.y;
	size[2] = pyramidHeight;
	return (3);
}

void PyramidGeometryObject::SetObjectSize(const float *size)
{
	pyramidSize.x = size[0];
	pyramidSize.y = size[1];
	pyramidHeight = size[2];
}

bool PyramidGeometryObject::ExteriorSphere(const Point3D& center, float radius) const
{
	if (center.z > pyramidHeight + radius)
	{
		return (true);
	}

	return (center.z < -radius);
}

bool PyramidGeometryObject::ExteriorSweptSphere(const Point3D& p1, const Point3D& p2, float radius) const
{
	float h = pyramidHeight + radius;
	if ((p1.z > h) && (p2.z > h))
	{
		return (true);
	}

	return ((p1.z < -radius) && (p2.z < -radius));
}

Point3D PyramidGeometryObject::CalculatePrimitiveSupportPoint(const Vector3D& direction) const
{
	float x = (direction.x > 0.0F) ? pyramidSize.x : 0.0F;
	float y = (direction.y > 0.0F) ? pyramidSize.y : 0.0F;
	Point3D apex(pyramidSize.x * 0.5F, pyramidSize.y * 0.5F, pyramidHeight);

	float t1 = x * direction.x + y * direction.y;
	float t2 = apex * direction;

	if (t2 > t1)
	{
		return (apex);
	}

	return (Point3D(x, y, 0.0F));
}

void PyramidGeometryObject::CalculatePrimitiveSupportPointArray(int32 count, const Vector3D *direction, Point3D *support) const
{
	Point3D apex(pyramidSize.x * 0.5F, pyramidSize.y * 0.5F, pyramidHeight);

	for (machine a = 0; a < count; a++)
	{
		float x = (direction->x > 0.0F) ? pyramidSize.x : 0.0F;
		float y = (direction->y > 0.0F) ? pyramidSize.y : 0.0F;

		float t1 = x * direction->x + y * direction->y;
		float t2 = apex * *direction;

		if (t2 > t1)
		{
			*support = apex;
		}
		else
		{
			support->Set(x, y, 0.0F);
		}

		direction++;
		support++;
	}
}

void PyramidGeometryObject::BuildPrimitiveLevel(int32 level, const Transformable *transformable)
{
	ArrayDescriptor		desc[5];
	Mesh				mesh;

	int32 xdiv = Max(GetMaxSubdivX() >> level, 1);
	int32 zdiv = Max(GetMaxSubdivY() >> level, 1);

	int32 vertexCount = ((xdiv + 1) * zdiv + 1) * 4;
	int32 triangleCount = (xdiv * zdiv * 2 - xdiv) * 4;

	bool endcap = ((GetPrimitiveFlags() & kPrimitiveEndcap) != 0);
	if (endcap)
	{
		vertexCount += (xdiv + 1) * (xdiv + 1);
		triangleCount += xdiv * xdiv * 2;
	}

	desc[0].identifier = kArrayPosition;
	desc[0].elementCount = vertexCount;
	desc[0].elementSize = sizeof(Point3D);
	desc[0].componentCount = 3;

	desc[1].identifier = kArrayNormal;
	desc[1].elementCount = vertexCount;
	desc[1].elementSize = sizeof(Vector3D);
	desc[1].componentCount = 3;

	desc[2].identifier = kArrayTexcoord;
	desc[2].elementCount = vertexCount;
	desc[2].elementSize = sizeof(Point2D);
	desc[2].componentCount = 2;

	desc[3].identifier = kArraySurfaceIndex;
	desc[3].elementCount = vertexCount;
	desc[3].elementSize = 2;
	desc[3].componentCount = 1;

	desc[4].identifier = kArrayPrimitive;
	desc[4].elementCount = triangleCount;
	desc[4].elementSize = sizeof(Triangle);
	desc[4].componentCount = 1;

	mesh.AllocateStorage(vertexCount, 5, desc);

	Point3D *restrict vertex = mesh.GetArray<Point3D>(kArrayPosition) - 1;
	Point2D *restrict texcoord = mesh.GetArray<Point2D>(kArrayTexcoord) - 1;

	float sx = pyramidSize.x;
	float sy = pyramidSize.y;
	float h = pyramidHeight;

	float dz = 1.0F / (float) zdiv;

	for (machine j = 0; j < zdiv; j++)
	{
		float t = (float) j * dz;
		float x = (1.0F - t * 0.5F) * sx;
		float z = t * h;

		float dx = (1.0F - t) / (float) xdiv;
		for (machine i = 0; i < xdiv; i++)
		{
			float s = t * 0.5F + (float) i * dx;
			float y = s * sy;

			(++vertex)->Set(x, y, z);
			(++texcoord)->Set(s, t);
		}

		(++vertex)->Set(x, (1.0F - t * 0.5F) * sy, z);
		(++texcoord)->Set(1.0F - t * 0.5F, t);
	}

	(++vertex)->Set(sx * 0.5F, sy * 0.5F, h);
	(++texcoord)->Set(0.5F, 1.0F);

	for (machine j = 0; j < zdiv; j++)
	{
		float t = (float) j * dz;
		float y = (1.0F - t * 0.5F) * sy;
		float z = t * h;

		float dx = (1.0F - t) / (float) xdiv;
		for (machine i = 0; i < xdiv; i++)
		{
			float s = t * 0.5F + ((float) (xdiv - i) + (float) i * t) * dx;
			float x = s * sx;

			(++vertex)->Set(x, y, z);
			(++texcoord)->Set(1.0F - s, t);
		}

		(++vertex)->Set(t * sx * 0.5F, y, z);
		(++texcoord)->Set(1.0F - t * 0.5F, t);
	}

	(++vertex)->Set(sx * 0.5F, sy * 0.5F, h);
	(++texcoord)->Set(0.5F, 1.0F);

	for (machine j = 0; j < zdiv; j++)
	{
		float t = (float) j * dz;
		float x = t * sx * 0.5F;
		float z = t * h;

		float dx = (1.0F - t) / (float) xdiv;
		for (machine i = 0; i < xdiv; i++)
		{
			float s = t * 0.5F + ((float) (xdiv - i) + (float) i * t) * dx;
			float y = s * sy;

			(++vertex)->Set(x, y, z);
			(++texcoord)->Set(1.0F - s, t);
		}

		(++vertex)->Set(x, t * sy * 0.5F, z);
		(++texcoord)->Set(1.0F - t * 0.5F, t);
	}

	(++vertex)->Set(sx * 0.5F, sy * 0.5F, h);
	(++texcoord)->Set(0.5F, 1.0F);

	for (machine j = 0; j < zdiv; j++)
	{
		float t = (float) j * dz;
		float y = t * sy * 0.5F;
		float z = t * h;

		float dx = (1.0F - t) / (float) xdiv;
		for (machine i = 0; i < xdiv; i++)
		{
			float s = t * 0.5F + (float) i * dx;
			float x = s * sx;

			(++vertex)->Set(x, y, z);
			(++texcoord)->Set(s, t);
		}

		(++vertex)->Set((1.0F - t * 0.5F) * sx, y, z);
		(++texcoord)->Set(1.0F - t * 0.5F, t);
	}

	(++vertex)->Set(sx * 0.5F, sy * 0.5F, h);
	(++texcoord)->Set(0.5F, 1.0F);

	int32 count = (xdiv + 1) * zdiv + 1;
	Vector3D *restrict normal = mesh.GetArray<Vector3D>(kArrayNormal) - 1;
	unsigned_int16 *restrict surfaceIndex = mesh.GetArray<unsigned_int16>(kArraySurfaceIndex) - 1;

	float r = InverseSqrt(h * h + sy * sy * 0.25F);
	Vector3D nrml(h * r, 0.0F, sx * r * 0.5F);
	for (machine a = 0; a < count; a++)
	{
		*++normal = nrml;
		*++surfaceIndex = 0;
	}

	nrml.Set(0.0F, h * r, sy * r * 0.5F);
	for (machine a = 0; a < count; a++)
	{
		*++normal = nrml;
		*++surfaceIndex = 1;
	}

	nrml.Set(-h * r, 0.0F, sx * r * 0.5F);
	for (machine a = 0; a < count; a++)
	{
		*++normal = nrml;
		*++surfaceIndex = 2;
	}

	nrml.Set(0.0F, -h * r, sy * r * 0.5F);
	for (machine a = 0; a < count; a++)
	{
		*++normal = nrml;
		*++surfaceIndex = 3;
	}

	Triangle *restrict triangle = mesh.GetArray<Triangle>(kArrayPrimitive);

	int32 n = 0;
	for (machine a = 0; a < 4; a++)
	{
		for (machine j = 0; j < zdiv - 1; j++)
		{
			int32 k = n + j * (xdiv + 1);
			for (machine i = 0; i < xdiv; i++)
			{
				if (((i + j) & 1) == 0)
				{
					triangle[0].Set(k + i, k + i + 1, k + i + xdiv + 1);
					triangle[1].Set(k + i + 1, k + i + xdiv + 2, k + i + xdiv + 1);
				}
				else
				{
					triangle[0].Set(k + i, k + i + 1, k + i + xdiv + 2);
					triangle[1].Set(k + i, k + i + xdiv + 2, k + i + xdiv + 1);
				}

				triangle += 2;
			}
		}

		int32 k = n + (zdiv - 1) * (xdiv + 1);
		int32 m = k + xdiv + 1;

		for (machine i = 0; i < xdiv; i++)
		{
			triangle->Set(k + i, k + i + 1, m);
			triangle++;
		}

		n = m + 1;
	}

	if (endcap)
	{
		float dx = 1.0F / (float) xdiv;

		for (machine j = 0; j < xdiv; j++)
		{
			float t = (float) j * dx;
			float y = t * sy;

			for (machine i = 0; i < xdiv; i++)
			{
				float s = (float) (xdiv - i) * dx;
				float x = s * sx;

				(++vertex)->Set(x, y, 0.0F);
				(++texcoord)->Set(1.0F - s, t);
			}

			(++vertex)->Set(0.0F, y, 0.0F);
			(++texcoord)->Set(1.0F, t);
		}

		for (machine i = 0; i < xdiv; i++)
		{
			float s = (float) (xdiv - i) * dx;
			float x = s * sx;

			(++vertex)->Set(x, sy, 0.0F);
			(++texcoord)->Set(1.0F - s, 1.0F);
		}

		(++vertex)->Set(0.0F, sy, 0.0F);
		(++texcoord)->Set(1.0F, 1.0F);

		count = (xdiv + 1) * (xdiv + 1);
		for (machine a = 0; a < count; a++)
		{
			(++normal)->Set(0.0F, 0.0F, -1.0F);
			*++surfaceIndex = 4;
		}

		for (machine j = 0; j < xdiv; j++)
		{
			int32 k = n + j * (xdiv + 1);
			for (machine i = 0; i < xdiv; i++)
			{
				if (((i + j) & 1) == 0)
				{
					triangle[0].Set(k + i, k + i + 1, k + i + xdiv + 1);
					triangle[1].Set(k + i + 1, k + i + xdiv + 2, k + i + xdiv + 1);
				}
				else
				{
					triangle[0].Set(k + i, k + i + 1, k + i + xdiv + 2);
					triangle[1].Set(k + i, k + i + xdiv + 2, k + i + xdiv + 1);
				}

				triangle += 2;
			}
		}
	}

	BuildStandardArrays(&mesh, GetGeometryLevel(level), transformable);
}


CylinderGeometryObject::CylinderGeometryObject() : PrimitiveGeometryObject(kPrimitiveCylinder)
{
	SetConvexPrimitiveFlag();
	SetStaticSurfaceData(3, staticSurfaceData);
}

CylinderGeometryObject::CylinderGeometryObject(const Vector2D& size, float height) : PrimitiveGeometryObject(kPrimitiveCylinder)
{
	cylinderSize = size;
	cylinderHeight = height;
	ratioXY = size.x / size.y;

	SetConvexPrimitiveFlag();
	SetBuildLevelCount(2);
	SetMaxSubdivX(32);

	SetStaticSurfaceData(3, staticSurfaceData, true);

	staticSurfaceData[0].textureAlignData[0].alignMode = kTextureAlignNatural;
	staticSurfaceData[0].textureAlignData[0].alignPlane.Set(1.0F, 0.0F, 0.0F, 0.0F);
	staticSurfaceData[0].textureAlignData[1].alignMode = kTextureAlignGlobalObjectPlane;
	staticSurfaceData[0].textureAlignData[1].alignPlane.Set(0.0F, 0.0F, 1.0F, 0.0F);

	staticSurfaceData[1].textureAlignData[0].alignMode = kTextureAlignGlobalObjectPlane;
	staticSurfaceData[1].textureAlignData[0].alignPlane.Set(1.0F, 0.0F, 0.0F, 0.0F);
	staticSurfaceData[1].textureAlignData[1].alignMode = kTextureAlignGlobalObjectPlane;
	staticSurfaceData[1].textureAlignData[1].alignPlane.Set(0.0F, 1.0F, 0.0F, 0.0F);

	staticSurfaceData[2].textureAlignData[0].alignMode = kTextureAlignGlobalObjectPlane;
	staticSurfaceData[2].textureAlignData[0].alignPlane.Set(-1.0F, 0.0F, 0.0F, 0.0F);
	staticSurfaceData[2].textureAlignData[1].alignMode = kTextureAlignGlobalObjectPlane;
	staticSurfaceData[2].textureAlignData[1].alignPlane.Set(0.0F, 1.0F, 0.0F, 0.0F);
}

CylinderGeometryObject::~CylinderGeometryObject()
{
}

void CylinderGeometryObject::Pack(Packer& data, unsigned_int32 packFlags) const
{
	PrimitiveGeometryObject::Pack(data, packFlags);

	data << ChunkHeader('SIZE', sizeof(Vector2D) + 4);
	data << cylinderSize;
	data << cylinderHeight;

	data << TerminatorChunk;
}

void CylinderGeometryObject::Unpack(Unpacker& data, unsigned_int32 unpackFlags)
{
	PrimitiveGeometryObject::Unpack(data, unpackFlags);
	UnpackChunkList<CylinderGeometryObject>(data, unpackFlags);
}

bool CylinderGeometryObject::UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags)
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

int32 CylinderGeometryObject::GetObjectSize(float *size) const
{
	size[0] = cylinderSize.x;
	size[1] = cylinderSize.y;
	size[2] = cylinderHeight;
	return (3);
}

void CylinderGeometryObject::SetObjectSize(const float *size)
{
	cylinderSize.x = size[0];
	cylinderSize.y = size[1];
	cylinderHeight = size[2];
	ratioXY = cylinderSize.x / cylinderSize.y;
}

bool CylinderGeometryObject::ExteriorSphere(const Point3D& center, float radius) const
{
	if ((center.z < -radius) || (center.z > cylinderHeight + radius))
	{
		return (true);
	}

	float x = center.x;
	float y = center.y;
	float n = SquaredMag(cylinderSize);
	return (x * x + y * y > n + 2.0F * radius * Sqrt(n) + radius * radius);
}

bool CylinderGeometryObject::ExteriorSweptSphere(const Point3D& p1, const Point3D& p2, float radius) const
{
	float h = cylinderHeight + radius;
	if ((p1.z > h) && (p2.z > h))
	{
		return (true);
	}

	return ((p1.z < -radius) && (p2.z < -radius));
}

Point3D CylinderGeometryObject::CalculatePrimitiveSupportPoint(const Vector3D& direction) const
{
	float x = cylinderSize.x * cylinderSize.x * direction.x;
	float y = cylinderSize.y * cylinderSize.y * direction.y;
	float z = (direction.z > 0.0F) ? cylinderHeight : 0.0F;

	float r = x * direction.x + y * direction.y;
	if (r > K::min_float)
	{
		r = InverseSqrt(r);
		return (Point3D(x * r, y * r, z));
	}

	return (Point3D(0.0F, 0.0F, z));
}

void CylinderGeometryObject::CalculatePrimitiveSupportPointArray(int32 count, const Vector3D *direction, Point3D *support) const
{
	for (machine a = 0; a < count; a++)
	{
		float x = cylinderSize.x * cylinderSize.x * direction->x;
		float y = cylinderSize.y * cylinderSize.y * direction->y;
		float z = (direction->z > 0.0F) ? cylinderHeight : 0.0F;

		float r = x * direction->x + y * direction->y;
		if (r > K::min_float)
		{
			r = InverseSqrt(r);
			support->Set(x * r, y * r, z);
		}
		else
		{
			support->Set(0.0F, 0.0F, z);
		}

		direction++;
		support++;
	}
}

void CylinderGeometryObject::BuildPrimitiveLevel(int32 level, const Transformable *transformable)
{
	ArrayDescriptor		desc[5];
	Mesh				mesh;

	int32 xdiv = Max(GetMaxSubdivX() >> level, 8) & ~1;
	int32 zdiv = Max(GetMaxSubdivY() >> level, 1);

	int32 vertexCount = (xdiv + 1) * (zdiv + 1);
	int32 triangleCount = xdiv * zdiv * 2;

	bool endcap = ((GetPrimitiveFlags() & kPrimitiveEndcap) != 0);
	if (endcap)
	{
		vertexCount += xdiv * 2;
		triangleCount += xdiv * 2 - 4;
	}

	desc[0].identifier = kArrayPosition;
	desc[0].elementCount = vertexCount;
	desc[0].elementSize = sizeof(Point3D);
	desc[0].componentCount = 3;

	desc[1].identifier = kArrayNormal;
	desc[1].elementCount = vertexCount;
	desc[1].elementSize = sizeof(Vector3D);
	desc[1].componentCount = 3;

	desc[2].identifier = kArrayTexcoord;
	desc[2].elementCount = vertexCount;
	desc[2].elementSize = sizeof(Point2D);
	desc[2].componentCount = 2;

	desc[3].identifier = kArraySurfaceIndex;
	desc[3].elementCount = vertexCount;
	desc[3].elementSize = 2;
	desc[3].componentCount = 1;

	desc[4].identifier = kArrayPrimitive;
	desc[4].elementCount = triangleCount;
	desc[4].elementSize = sizeof(Triangle);
	desc[4].componentCount = 1;

	mesh.AllocateStorage(vertexCount, 5, desc);

	Point3D *restrict vertex = mesh.GetArray<Point3D>(kArrayPosition) - 1;
	Vector3D *restrict normal = mesh.GetArray<Vector3D>(kArrayNormal) - 1;
	Point2D *restrict texcoord = mesh.GetArray<Point2D>(kArrayTexcoord) - 1;

	float rx = cylinderSize.x;
	float ry = cylinderSize.y;
	float h = cylinderHeight;

	float dx = 1.0F / (float) xdiv;
	float dz = 1.0F / (float) zdiv;

	float mx = 1.0F / rx;
	float my = 1.0F / ry;

	for (machine i = 0; i < xdiv; i++)
	{
		float s = (float) i * dx;
		Vector2D v = CosSin(s * K::tau);
		float x = rx * v.x;
		float y = ry * v.y;

		float nx = v.x * mx;
		float ny = v.y * my;
		float m = InverseSqrt(nx * nx + ny * ny);
		nx *= m;
		ny *= m;

		for (machine j = 0; j < zdiv; j++)
		{
			float t = (float) j * dz;

			(++vertex)->Set(x, y, t * h);
			(++normal)->Set(nx, ny, 0.0F);
			(++texcoord)->Set(s, t);
		}

		(++vertex)->Set(x, y, h);
		(++normal)->Set(nx, ny, 0.0F);
		(++texcoord)->Set(s, 1.0F);
	}

	const Point3D *vtx = mesh.GetArray<Point3D>(kArrayPosition);
	for (machine j = 0; j < zdiv; j++)
	{
		*++vertex = vtx[j];
		(++normal)->Set(1.0F, 0.0F, 0.0F);
		(++texcoord)->Set(1.0F, (float) j * dz);
	}

	*++vertex = vtx[zdiv];
	(++normal)->Set(1.0F, 0.0F, 0.0F);
	(++texcoord)->Set(1.0F, 1.0F);

	int32 count = (xdiv + 1) * (zdiv + 1);
	unsigned_int16 *restrict surfaceIndex = mesh.GetArray<unsigned_int16>(kArraySurfaceIndex) - 1;
	for (machine a = 0; a < count; a++)
	{
		*++surfaceIndex = 0;
	}

	Triangle *restrict triangle = mesh.GetArray<Triangle>(kArrayPrimitive);

	for (machine i = 0; i < xdiv; i++)
	{
		int32 k = i * (zdiv + 1);
		for (machine j = 0; j < zdiv; j++)
		{
			if (((i + j) & 1) == 0)
			{
				triangle[0].Set(k + j, k + j + zdiv + 1, k + j + 1);
				triangle[1].Set(k + j + 1, k + j + zdiv + 1, k + j + zdiv + 2);
			}
			else
			{
				triangle[0].Set(k + j, k + j + zdiv + 1, k + j + zdiv + 2);
				triangle[1].Set(k + j, k + j + zdiv + 2, k + j + 1);
			}

			triangle += 2;
		}
	}

	if (endcap)
	{
		float dt = K::tau / (float) xdiv;
		float tex = 0.5F / rx;

		for (machine i = xdiv; i > 0; i--)
		{
			machine k = OverflowZero(i, xdiv);
			Vector2D v = CosSin((float) k * dt) & cylinderSize;

			*++vertex = vtx[k * (zdiv + 1)];
			(++normal)->Set(0.0F, 0.0F, -1.0F);
			(++texcoord)->Set(1.0F - (v.x + rx) * tex, (v.y * ratioXY + rx) * tex);
			*++surfaceIndex = 1;
		}

		vtx += zdiv;

		for (machine i = 0; i < xdiv; i++)
		{
			Vector2D v = CosSin((float) i * dt) & cylinderSize;

			*++vertex = vtx[i * (zdiv + 1)];
			(++normal)->Set(0.0F, 0.0F, 1.0F);
			(++texcoord)->Set((v.x + rx) * tex, (v.y * ratioXY + rx) * tex);
			*++surfaceIndex = 2;
		}

		int32 n = (xdiv + 1) * (zdiv + 1);

		TriangulateDisk(xdiv, n, triangle);
		TriangulateDisk(xdiv, n + xdiv, triangle + (xdiv - 2));
	}

	BuildStandardArrays(&mesh, GetGeometryLevel(level), transformable);
}


ConeGeometryObject::ConeGeometryObject() : PrimitiveGeometryObject(kPrimitiveCone)
{
	SetConvexPrimitiveFlag();
	SetStaticSurfaceData(2, staticSurfaceData);
}

ConeGeometryObject::ConeGeometryObject(const Vector2D& size, float height) : PrimitiveGeometryObject(kPrimitiveCone)
{
	coneSize = size;
	coneHeight = height;
	ratioXY = size.x / size.y;

	SetConvexPrimitiveFlag();
	SetBuildLevelCount(2);
	SetMaxSubdivX(32);

	SetStaticSurfaceData(2, staticSurfaceData, true);

	staticSurfaceData[0].textureAlignData[0].alignMode = kTextureAlignNatural;
	staticSurfaceData[0].textureAlignData[0].alignPlane.Set(1.0F, 0.0F, 0.0F, 0.0F);
	staticSurfaceData[0].textureAlignData[1].alignMode = kTextureAlignGlobalObjectPlane;
	staticSurfaceData[0].textureAlignData[1].alignPlane.Set(0.0F, 0.0F, 1.0F, 0.0F);

	staticSurfaceData[1].textureAlignData[0].alignMode = kTextureAlignGlobalObjectPlane;
	staticSurfaceData[1].textureAlignData[0].alignPlane.Set(-1.0F, 0.0F, 0.0F, 0.0F);
	staticSurfaceData[1].textureAlignData[1].alignMode = kTextureAlignGlobalObjectPlane;
	staticSurfaceData[1].textureAlignData[1].alignPlane.Set(0.0F, 1.0F, 0.0F, 0.0F);
}

ConeGeometryObject::~ConeGeometryObject()
{
}

void ConeGeometryObject::Pack(Packer& data, unsigned_int32 packFlags) const
{
	PrimitiveGeometryObject::Pack(data, packFlags);

	data << ChunkHeader('SIZE', sizeof(Vector2D) + 4);
	data << coneSize;
	data << coneHeight;

	data << TerminatorChunk;
}

void ConeGeometryObject::Unpack(Unpacker& data, unsigned_int32 unpackFlags)
{
	PrimitiveGeometryObject::Unpack(data, unpackFlags);
	UnpackChunkList<ConeGeometryObject>(data, unpackFlags);
}

bool ConeGeometryObject::UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags)
{
	switch (chunkHeader->chunkType)
	{
		case 'SIZE':

			data >> coneSize;
			data >> coneHeight;
			ratioXY = coneSize.x / coneSize.y;
			return (true);
	}

	return (false);
}

int32 ConeGeometryObject::GetObjectSize(float *size) const
{
	size[0] = coneSize.x;
	size[1] = coneSize.y;
	size[2] = coneHeight;
	return (3);
}

void ConeGeometryObject::SetObjectSize(const float *size)
{
	coneSize.x = size[0];
	coneSize.y = size[1];
	coneHeight = size[2];
	ratioXY = coneSize.x / coneSize.y;
}

bool ConeGeometryObject::ExteriorSphere(const Point3D& center, float radius) const
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

bool ConeGeometryObject::ExteriorSweptSphere(const Point3D& p1, const Point3D& p2, float radius) const
{
	return ((p1.z < -radius) && (p2.z < -radius));
}

Point3D ConeGeometryObject::CalculatePrimitiveSupportPoint(const Vector3D& direction) const
{
	float x = coneSize.x * coneSize.x * direction.x;
	float y = coneSize.y * coneSize.y * direction.y;
	float r = x * direction.x + y * direction.y;

	if (r > K::min_float)
	{
		r = InverseSqrt(r);
		x *= r;
		y *= r;

		float t1 = x * direction.x + y * direction.y;
		float t2 = coneHeight * direction.z;

		if (t2 > t1)
		{
			return (Point3D(0.0F, 0.0F, coneHeight));
		}

		return (Point3D(x, y, 0.0F));
	}

	if (direction.z > 0.0F)
	{
		return (Point3D(0.0F, 0.0F, coneHeight));
	}

	return (Point3D(0.0F, 0.0F, 0.0F));
}

void ConeGeometryObject::CalculatePrimitiveSupportPointArray(int32 count, const Vector3D *direction, Point3D *support) const
{
	for (machine a = 0; a < count; a++)
	{
		float x = coneSize.x * coneSize.x * direction->x;
		float y = coneSize.y * coneSize.y * direction->y;
		float r = x * direction->x + y * direction->y;

		if (r > K::min_float)
		{
			r = InverseSqrt(r);
			x *= r;
			y *= r;

			float t1 = x * direction->x + y * direction->y;
			float t2 = coneHeight * direction->z;

			if (t2 > t1)
			{
				support->Set(0.0F, 0.0F, coneHeight);
			}
			else
			{
				support->Set(x, y, 0.0F);
			}
		}
		else
		{
			if (direction->z > 0.0F)
			{
				support->Set(0.0F, 0.0F, coneHeight);
			}
			else
			{
				support->Set(0.0F, 0.0F, 0.0F);
			}
		}

		direction++;
		support++;
	}
}

void ConeGeometryObject::BuildPrimitiveLevel(int32 level, const Transformable *transformable)
{
	ArrayDescriptor		desc[5];
	Mesh				mesh;

	int32 xdiv = Max(GetMaxSubdivX() >> level, 8) & ~1;
	int32 zdiv = Max(GetMaxSubdivY() >> level, 2);

	int32 vertexCount = (xdiv + 1) * (zdiv + 1) - 1;
	int32 triangleCount = xdiv * zdiv * 2 - xdiv;

	bool endcap = ((GetPrimitiveFlags() & kPrimitiveEndcap) != 0);
	if (endcap)
	{
		vertexCount += xdiv;
		triangleCount += xdiv - 2;
	}

	desc[0].identifier = kArrayPosition;
	desc[0].elementCount = vertexCount;
	desc[0].elementSize = sizeof(Point3D);
	desc[0].componentCount = 3;

	desc[1].identifier = kArrayNormal;
	desc[1].elementCount = vertexCount;
	desc[1].elementSize = sizeof(Vector3D);
	desc[1].componentCount = 3;

	desc[2].identifier = kArrayTexcoord;
	desc[2].elementCount = vertexCount;
	desc[2].elementSize = sizeof(Point2D);
	desc[2].componentCount = 2;

	desc[3].identifier = kArraySurfaceIndex;
	desc[3].elementCount = vertexCount;
	desc[3].elementSize = 2;
	desc[3].componentCount = 1;

	desc[4].identifier = kArrayPrimitive;
	desc[4].elementCount = triangleCount;
	desc[4].elementSize = sizeof(Triangle);
	desc[4].componentCount = 1;

	mesh.AllocateStorage(vertexCount, 5, desc);

	Point3D *restrict vertex = mesh.GetArray<Point3D>(kArrayPosition) - 1;
	Vector3D *restrict normal = mesh.GetArray<Vector3D>(kArrayNormal) - 1;
	Point2D *restrict texcoord = mesh.GetArray<Point2D>(kArrayTexcoord) - 1;

	float rx = coneSize.x;
	float ry = coneSize.y;
	float h = coneHeight;

	float dx = 1.0F / (float) xdiv;
	float dz = 1.0F / (float) zdiv;

	float mx = h / rx;
	float my = h / ry;

	for (machine i = 0; i < xdiv; i++)
	{
		float s = (float) i * dx;
		Vector2D v = CosSin(s * K::tau);
		float x = rx * v.x;
		float y = ry * v.y;

		float nx = v.x * mx;
		float ny = v.y * my;
		float m = InverseSqrt(nx * nx + ny * ny + 1.0F);
		nx *= m;
		ny *= m;

		for (machine j = 0; j < zdiv; j++)
		{
			float t = (float) j * dz;
			float u = 1.0F - t;

			(++vertex)->Set(x * u, y * u, h * t);
			(++normal)->Set(nx, ny, m);
			(++texcoord)->Set(s, t);
		}

		(++vertex)->Set(0.0F, 0.0F, h);
		(++normal)->Set(nx, ny, m);
		(++texcoord)->Set(s, 1.0F);
	}

	const Point3D *vtx = mesh.GetArray<Point3D>(kArrayPosition);
	const Vector3D *nrm = mesh.GetArray<Vector3D>(kArrayNormal);
	for (machine j = 0; j < zdiv; j++)
	{
		*++vertex = vtx[j];
		*++normal = nrm[j];
		(++texcoord)->Set(1.0F, (float) j * dz);
	}

	int32 count = xdiv * (zdiv + 1) + zdiv;
	unsigned_int16 *restrict surfaceIndex = mesh.GetArray<unsigned_int16>(kArraySurfaceIndex) - 1;
	for (machine a = 0; a < count; a++)
	{
		*++surfaceIndex = 0;
	}

	Triangle *restrict triangle = mesh.GetArray<Triangle>(kArrayPrimitive);

	for (machine i = 0; i < xdiv; i++)
	{
		int32 k = i * (zdiv + 1);
		for (machine j = 0; j < zdiv - 1; j++)
		{
			if (((i + j) & 1) == 0)
			{
				triangle[0].Set(k + j, k + j + zdiv + 1, k + j + 1);
				triangle[1].Set(k + j + 1, k + j + zdiv + 1, k + j + zdiv + 2);
			}
			else
			{
				triangle[0].Set(k + j, k + j + zdiv + 1, k + j + zdiv + 2);
				triangle[1].Set(k + j, k + j + zdiv + 2, k + j + 1);
			}

			triangle += 2;
		}

		triangle->Set(k + zdiv - 1, k + zdiv * 2, k + zdiv);
		triangle++;
	}

	if (endcap)
	{
		float dt = K::tau / (float) xdiv;
		float tex = 0.5F / rx;

		for (machine i = xdiv; i > 0; i--)
		{
			machine k = OverflowZero(i, xdiv);
			Vector2D v = CosSin((float) k * dt) & coneSize;

			*++vertex = vtx[k * (zdiv + 1)];
			(++normal)->Set(0.0F, 0.0F, -1.0F);
			(++texcoord)->Set(1.0F - (v.x + rx) * tex, (v.y * ratioXY + rx) * tex);
			*++surfaceIndex = 1;
		}

		int32 n = (xdiv + 1) * (zdiv + 1) - 1;
		TriangulateDisk(xdiv, n, triangle);
	}

	BuildStandardArrays(&mesh, GetGeometryLevel(level), transformable);
}


TruncatedConeGeometryObject::TruncatedConeGeometryObject() : PrimitiveGeometryObject(kPrimitiveTruncatedCone)
{
	SetConvexPrimitiveFlag();
	SetStaticSurfaceData(3, staticSurfaceData);
}

TruncatedConeGeometryObject::TruncatedConeGeometryObject(const Vector2D& size, float height, float ratio) : PrimitiveGeometryObject(kPrimitiveTruncatedCone)
{
	coneSize = size;
	coneHeight = height;
	coneRatio = ratio;
	ratioXY = size.x / size.y;

	SetConvexPrimitiveFlag();
	SetBuildLevelCount(2);
	SetMaxSubdivX(32);

	SetStaticSurfaceData(3, staticSurfaceData, true);

	staticSurfaceData[0].textureAlignData[0].alignMode = kTextureAlignNatural;
	staticSurfaceData[0].textureAlignData[0].alignPlane.Set(1.0F, 0.0F, 0.0F, 0.0F);
	staticSurfaceData[0].textureAlignData[1].alignMode = kTextureAlignGlobalObjectPlane;
	staticSurfaceData[0].textureAlignData[1].alignPlane.Set(0.0F, 0.0F, 1.0F, 0.0F);

	staticSurfaceData[1].textureAlignData[0].alignMode = kTextureAlignGlobalObjectPlane;
	staticSurfaceData[1].textureAlignData[0].alignPlane.Set(1.0F, 0.0F, 0.0F, 0.0F);
	staticSurfaceData[1].textureAlignData[1].alignMode = kTextureAlignGlobalObjectPlane;
	staticSurfaceData[1].textureAlignData[1].alignPlane.Set(0.0F, 1.0F, 0.0F, 0.0F);

	staticSurfaceData[2].textureAlignData[0].alignMode = kTextureAlignGlobalObjectPlane;
	staticSurfaceData[2].textureAlignData[0].alignPlane.Set(-1.0F, 0.0F, 0.0F, 0.0F);
	staticSurfaceData[2].textureAlignData[1].alignMode = kTextureAlignGlobalObjectPlane;
	staticSurfaceData[2].textureAlignData[1].alignPlane.Set(0.0F, 1.0F, 0.0F, 0.0F);
}

TruncatedConeGeometryObject::~TruncatedConeGeometryObject()
{
}

void TruncatedConeGeometryObject::Pack(Packer& data, unsigned_int32 packFlags) const
{
	PrimitiveGeometryObject::Pack(data, packFlags);

	data << ChunkHeader('SIZE', sizeof(Vector2D) + 8);
	data << coneSize;
	data << coneHeight;
	data << coneRatio;

	data << TerminatorChunk;
}

void TruncatedConeGeometryObject::Unpack(Unpacker& data, unsigned_int32 unpackFlags)
{
	PrimitiveGeometryObject::Unpack(data, unpackFlags);
	UnpackChunkList<TruncatedConeGeometryObject>(data, unpackFlags);
}

bool TruncatedConeGeometryObject::UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags)
{
	switch (chunkHeader->chunkType)
	{
		case 'SIZE':

			data >> coneSize;
			data >> coneHeight;
			data >> coneRatio;
			ratioXY = coneSize.x / coneSize.y;
			return (true);
	}

	return (false);
}

int32 TruncatedConeGeometryObject::GetObjectSize(float *size) const
{
	size[0] = coneSize.x;
	size[1] = coneSize.y;
	size[2] = coneHeight;
	size[3] = coneRatio;
	return (4);
}

void TruncatedConeGeometryObject::SetObjectSize(const float *size)
{
	coneSize.x = size[0];
	coneSize.y = size[1];
	coneHeight = size[2];
	coneRatio = size[3];
	ratioXY = coneSize.x / coneSize.y;
}

bool TruncatedConeGeometryObject::ExteriorSphere(const Point3D& center, float radius) const
{
	if ((center.z < -radius) || (center.z > coneHeight + radius))
	{
		return (true);
	}

	float x = center.x;
	float y = center.y;
	float n = SquaredMag(coneSize * Fmax(coneRatio, 1.0F));
	return (x * x + y * y > n + 2.0F * radius * Sqrt(n) + radius * radius);
}

bool TruncatedConeGeometryObject::ExteriorSweptSphere(const Point3D& p1, const Point3D& p2, float radius) const
{
	float h = coneHeight + radius;
	if ((p1.z > h) && (p2.z > h))
	{
		return (true);
	}

	return ((p1.z < -radius) && (p2.z < -radius));
}

Point3D TruncatedConeGeometryObject::CalculatePrimitiveSupportPoint(const Vector3D& direction) const
{
	float x1 = coneSize.x * coneSize.x * direction.x;
	float y1 = coneSize.y * coneSize.y * direction.y;
	float r = x1 * direction.x + y1 * direction.y;

	if (r > K::min_float)
	{
		r = InverseSqrt(r);
		x1 *= r;
		y1 *= r;

		float x2 = x1 * coneRatio;
		float y2 = y1 * coneRatio;

		float t1 = x1 * direction.x + y1 * direction.y;
		float t2 = x2 * direction.x + y2 * direction.y + coneHeight * direction.z;

		if (t1 > t2)
		{
			return (Point3D(x1, y1, 0.0F));
		}

		return (Point3D(x2, y2, coneHeight));
	}

	if (direction.z > 0.0F)
	{
		return (Point3D(0.0F, 0.0F, coneHeight));
	}

	return (Point3D(0.0F, 0.0F, 0.0F));
}

void TruncatedConeGeometryObject::CalculatePrimitiveSupportPointArray(int32 count, const Vector3D *direction, Point3D *support) const
{
	for (machine a = 0; a < count; a++)
	{
		float x1 = coneSize.x * coneSize.x * direction->x;
		float y1 = coneSize.y * coneSize.y * direction->y;
		float r = x1 * direction->x + y1 * direction->y;

		if (r > K::min_float)
		{
			r = InverseSqrt(r);
			x1 *= r;
			y1 *= r;

			float x2 = x1 * coneRatio;
			float y2 = y1 * coneRatio;

			float t1 = x1 * direction->x + y1 * direction->y;
			float t2 = x2 * direction->x + y2 * direction->y + coneHeight * direction->z;

			if (t1 > t2)
			{
				support->Set(x1, y1, 0.0F);
			}
			else
			{
				support->Set(x2, y2, coneHeight);
			}
		}
		else
		{
			if (direction->z > 0.0F)
			{
				support->Set(0.0F, 0.0F, coneHeight);
			}
			else
			{
				support->Set(0.0F, 0.0F, 0.0F);
			}
		}

		direction++;
		support++;
	}
}

void TruncatedConeGeometryObject::BuildPrimitiveLevel(int32 level, const Transformable *transformable)
{
	ArrayDescriptor		desc[5];
	Mesh				mesh;

	int32 xdiv = Max(GetMaxSubdivX() >> level, 8) & ~1;
	int32 zdiv = Max(GetMaxSubdivY() >> level, 1);

	int32 vertexCount = (xdiv + 1) * (zdiv + 1);
	int32 triangleCount = xdiv * zdiv * 2;

	bool endcap = ((GetPrimitiveFlags() & kPrimitiveEndcap) != 0);
	if (endcap)
	{
		vertexCount += xdiv * 2;
		triangleCount += xdiv * 2 - 4;
	}

	desc[0].identifier = kArrayPosition;
	desc[0].elementCount = vertexCount;
	desc[0].elementSize = sizeof(Point3D);
	desc[0].componentCount = 3;

	desc[1].identifier = kArrayNormal;
	desc[1].elementCount = vertexCount;
	desc[1].elementSize = sizeof(Vector3D);
	desc[1].componentCount = 3;

	desc[2].identifier = kArrayTexcoord;
	desc[2].elementCount = vertexCount;
	desc[2].elementSize = sizeof(Point2D);
	desc[2].componentCount = 2;

	desc[3].identifier = kArraySurfaceIndex;
	desc[3].elementCount = vertexCount;
	desc[3].elementSize = 2;
	desc[3].componentCount = 1;

	desc[4].identifier = kArrayPrimitive;
	desc[4].elementCount = triangleCount;
	desc[4].elementSize = sizeof(Triangle);
	desc[4].componentCount = 1;

	mesh.AllocateStorage(vertexCount, 5, desc);

	Point3D *restrict vertex = mesh.GetArray<Point3D>(kArrayPosition) - 1;
	Vector3D *restrict normal = mesh.GetArray<Vector3D>(kArrayNormal) - 1;
	Point2D *restrict texcoord = mesh.GetArray<Point2D>(kArrayTexcoord) - 1;

	float rx1 = coneSize.x;
	float ry1 = coneSize.y;
	float rx2 = rx1 * coneRatio;
	float ry2 = ry1 * coneRatio;
	float h = coneHeight;

	float ds = 1.0F / (float) xdiv;
	float dt = 1.0F / (float) zdiv;

	float mx = h / (rx1 - rx2);
	float my = h / (ry1 - ry2);

	for (machine i = 0; i < xdiv; i++)
	{
		float s = (float) i * ds;
		Vector2D v = CosSin(s * K::tau);
		float x1 = rx1 * v.x;
		float y1 = ry1 * v.y;
		float x2 = rx2 * v.x;
		float y2 = ry2 * v.y;
		float dx = (x2 - x1) * dt;
		float dy = (y2 - y1) * dt;

		float nx = v.x * mx;
		float ny = v.y * my;
		float m = InverseSqrt(nx * nx + ny * ny + 1.0F);
		nx *= m;
		ny *= m;

		for (machine j = 0; j < zdiv; j++)
		{
			float t = (float) j * dt;

			(++vertex)->Set(x1, y1, t * h);
			(++normal)->Set(nx, ny, m);
			(++texcoord)->Set(s, t);

			x1 += dx;
			y1 += dy;
		}

		(++vertex)->Set(x2, y2, h);
		(++normal)->Set(nx, ny, m);
		(++texcoord)->Set(s, 1.0F);
	}

	const Point3D *vtx = mesh.GetArray<Point3D>(kArrayPosition);
	const Vector3D *nrm = mesh.GetArray<Vector3D>(kArrayNormal);
	for (machine j = 0; j < zdiv; j++)
	{
		*++vertex = vtx[j];
		*++normal = nrm[j];
		(++texcoord)->Set(1.0F, (float) j * dt);
	}

	*++vertex = vtx[zdiv];
	*++normal = nrm[zdiv];
	(++texcoord)->Set(1.0F, 1.0F);

	int32 count = (xdiv + 1) * (zdiv + 1);
	unsigned_int16 *restrict surfaceIndex = mesh.GetArray<unsigned_int16>(kArraySurfaceIndex) - 1;
	for (machine a = 0; a < count; a++)
	{
		*++surfaceIndex = 0;
	}

	Triangle *restrict triangle = mesh.GetArray<Triangle>(kArrayPrimitive);

	for (machine i = 0; i < xdiv; i++)
	{
		int32 k = i * (zdiv + 1);
		for (machine j = 0; j < zdiv; j++)
		{
			if (((i + j) & 1) == 0)
			{
				triangle[0].Set(k + j, k + j + zdiv + 1, k + j + 1);
				triangle[1].Set(k + j + 1, k + j + zdiv + 1, k + j + zdiv + 2);
			}
			else
			{
				triangle[0].Set(k + j, k + j + zdiv + 1, k + j + zdiv + 2);
				triangle[1].Set(k + j, k + j + zdiv + 2, k + j + 1);
			}

			triangle += 2;
		}
	}

	if (endcap)
	{
		dt = K::tau / (float) xdiv;
		float tex = 0.5F / rx1;

		for (machine i = xdiv; i > 0; i--)
		{
			machine k = OverflowZero(i, xdiv);
			Vector2D v = CosSin((float) k * dt) & coneSize;

			*++vertex = vtx[k * (zdiv + 1)];
			(++normal)->Set(0.0F, 0.0F, -1.0F);
			(++texcoord)->Set(1.0F - (v.x + rx1) * tex, (v.y * ratioXY + rx1) * tex);
			*++surfaceIndex = 1;
		}

		vtx += zdiv;
		tex = 0.5F / rx2;

		for (machine i = 0; i < xdiv; i++)
		{
			Vector2D v = CosSin((float) i * dt);
			float x = rx2 * v.x;
			float y = ry2 * v.y;

			*++vertex = vtx[i * (zdiv + 1)];
			(++normal)->Set(0.0F, 0.0F, 1.0F);
			(++texcoord)->Set((x + rx2) * tex, (y * ratioXY + rx2) * tex);
			*++surfaceIndex = 2;
		}

		int32 n = (xdiv + 1) * (zdiv + 1);
		TriangulateDisk(xdiv, n, triangle);
		TriangulateDisk(xdiv, n + xdiv, triangle + (xdiv - 2));
	}

	BuildStandardArrays(&mesh, GetGeometryLevel(level), transformable);
}


SphereGeometryObject::SphereGeometryObject() : PrimitiveGeometryObject(kPrimitiveSphere)
{
	SetConvexPrimitiveFlag();
	SetStaticSurfaceData(1, staticSurfaceData);
}

SphereGeometryObject::SphereGeometryObject(const Vector3D& size) : PrimitiveGeometryObject(kPrimitiveSphere)
{
	sphereSize = size;
	ratioXY = size.x / size.y;
	ratioXZ = size.x / size.z;

	SetConvexPrimitiveFlag();
	SetBuildLevelCount(2);
	SetMaxSubdivX(32);
	SetMaxSubdivY(16);

	SetStaticSurfaceData(1, staticSurfaceData, true);
	staticSurfaceData[0].textureAlignData[0].alignMode = kTextureAlignNatural;
	staticSurfaceData[0].textureAlignData[0].alignPlane.Set(1.0F, 0.0F, 0.0F, 0.0F);
	staticSurfaceData[0].textureAlignData[1].alignMode = kTextureAlignNatural;
	staticSurfaceData[0].textureAlignData[1].alignPlane.Set(0.0F, 1.0F, 0.0F, 0.0F);
}

SphereGeometryObject::~SphereGeometryObject()
{
}

void SphereGeometryObject::Pack(Packer& data, unsigned_int32 packFlags) const
{
	PrimitiveGeometryObject::Pack(data, packFlags);

	data << ChunkHeader('SIZE', sizeof(Vector3D));
	data << sphereSize;

	data << TerminatorChunk;
}

void SphereGeometryObject::Unpack(Unpacker& data, unsigned_int32 unpackFlags)
{
	PrimitiveGeometryObject::Unpack(data, unpackFlags);
	UnpackChunkList<SphereGeometryObject>(data, unpackFlags);
}

bool SphereGeometryObject::UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags)
{
	switch (chunkHeader->chunkType)
	{
		case 'SIZE':

			data >> sphereSize;
			ratioXY = sphereSize.x / sphereSize.y;
			ratioXZ = sphereSize.x / sphereSize.z;
			return (true);
	}

	return (false);
}

int32 SphereGeometryObject::GetObjectSize(float *size) const
{
	size[0] = sphereSize.x;
	size[1] = sphereSize.y;
	size[2] = sphereSize.z;
	return (3);
}

void SphereGeometryObject::SetObjectSize(const float *size)
{
	sphereSize.x = size[0];
	sphereSize.y = size[1];
	sphereSize.z = size[2];
	ratioXY = sphereSize.x / sphereSize.y;
	ratioXZ = sphereSize.x / sphereSize.z;
}

Point3D SphereGeometryObject::GetInitialPrimitiveSupportPoint(void) const
{
	return (Point3D(sphereSize.x, 0.0F, 0.0F));
}

Point3D SphereGeometryObject::CalculatePrimitiveSupportPoint(const Vector3D& direction) const
{
	#if C4SIMD

		Point3D		result;

		vec_float size = VecLoadUnaligned(&GetSphereSize().x);
		vec_float dir = VecLoadUnaligned(&direction.x);
		vec_float p = VecMul(VecMul(size, size), dir);
		p = VecMul(p, VecSmearX(VecInverseSqrtScalar(VecDot3D(p, dir))));

		VecStore3D(p, &result.x);
		return (result);

	#else

		float x = sphereSize.x * sphereSize.x * direction.x;
		float y = sphereSize.y * sphereSize.y * direction.y;
		float z = sphereSize.z * sphereSize.z * direction.z;
		float r = InverseSqrt(x * direction.x + y * direction.y + z * direction.z);

		return (Point3D(x * r, y * r, z * r));

	#endif
}

void SphereGeometryObject::CalculatePrimitiveSupportPointArray(int32 count, const Vector3D *direction, Point3D *support) const
{
	#if C4SIMD

		vec_float size = VecLoadUnaligned(&GetSphereSize().x);
		vec_float s2 = VecMul(size, size);

		for (machine a = 0; a < count; a++)
		{
			vec_float dir = VecLoadUnaligned(&direction->x);
			vec_float p = VecMul(s2, dir);
			p = VecMul(p, VecSmearX(VecInverseSqrtScalar(VecDot3D(p, dir))));

			VecStore3D(p, &support->x);

			direction++;
			support++;
		}

	#else

		Vector3D s2 = sphereSize & sphereSize;

		for (machine a = 0; a < count; a++)
		{
			float x = s2.x * direction->x;
			float y = s2.y * direction->y;
			float z = s2.z * direction->z;
			float r = InverseSqrt(x * direction->x + y * direction->y + z * direction->z);

			support->Set(x * r, y * r, z * r);

			direction++;
			support++;
		}

	#endif
}

void SphereGeometryObject::BuildPrimitiveLevel(int32 level, const Transformable *transformable)
{
	ArrayDescriptor		desc[4];
	Mesh				mesh;

	int32 xdiv = Max(GetMaxSubdivX() >> level, 8) & ~3;
	int32 zdiv = Max(GetMaxSubdivY() >> level, 4) & ~1;

	int32 vertexCount = (xdiv + 1) * (zdiv + 1) - 2;
	int32 triangleCount = xdiv * (zdiv - 1) * 2;

	desc[0].identifier = kArrayPosition;
	desc[0].elementCount = vertexCount;
	desc[0].elementSize = sizeof(Point3D);
	desc[0].componentCount = 3;

	desc[1].identifier = kArrayNormal;
	desc[1].elementCount = vertexCount;
	desc[1].elementSize = sizeof(Vector3D);
	desc[1].componentCount = 3;

	desc[2].identifier = kArrayTexcoord;
	desc[2].elementCount = vertexCount;
	desc[2].elementSize = sizeof(Point2D);
	desc[2].componentCount = 2;

	desc[3].identifier = kArrayPrimitive;
	desc[3].elementCount = triangleCount;
	desc[3].elementSize = sizeof(Triangle);
	desc[3].componentCount = 1;

	mesh.AllocateStorage(vertexCount, 4, desc);

	Point3D *restrict vertex = mesh.GetArray<Point3D>(kArrayPosition) - 1;
	Vector3D *restrict normal = mesh.GetArray<Vector3D>(kArrayNormal) - 1;
	Point2D *restrict texcoord = mesh.GetArray<Point2D>(kArrayTexcoord) - 1;

	float rx = sphereSize.x;
	float ry = sphereSize.y;
	float rz = sphereSize.z;

	float dx = 1.0F / (float) xdiv;
	float dz = 1.0F / (float) zdiv;

	float mx = 1.0F / rx;
	float my = 1.0F / ry;
	float mz = 1.0F / rz;

	for (machine i = 0; i < xdiv; i++)
	{
		float s = (float) i * dx;
		Vector2D v = CosSin(s * K::tau);

		for (machine j = 1; j < zdiv; j++)
		{
			float t = (float) j * dz;
			Vector2D u = CosSin((1.0F - t) * K::tau_over_2);

			float x = rx * v.x * u.y;
			float y = ry * v.y * u.y;
			float z = rz * u.x;

			float nx = v.x * u.y * mx;
			float ny = v.y * u.y * my;
			float nz = u.x * mz;
			float m = InverseSqrt(nx * nx + ny * ny + nz * nz);
			nx *= m;
			ny *= m;
			nz *= m;

			(++vertex)->Set(x, y, z);
			(++normal)->Set(nx, ny, nz);
			(++texcoord)->Set(s, t);
		}

		(++vertex)->Set(0.0F, 0.0F, -rz);
		(++normal)->Set(0.0F, 0.0F, -1.0F);
		(++texcoord)->Set(s, 0.0F);

		(++vertex)->Set(0.0F, 0.0F, rz);
		(++normal)->Set(0.0F, 0.0F, 1.0F);
		(++texcoord)->Set(s, 1.0F);
	}

	const Point3D *vtx = mesh.GetArray<Point3D>(kArrayPosition);
	const Vector3D *nml = mesh.GetArray<Vector3D>(kArrayNormal);
	for (machine j = 0; j < zdiv - 1; j++)
	{
		*++vertex = vtx[j];
		*++normal = nml[j];
		(++texcoord)->Set(1.0F, (float) (j + 1) * dz);
	}

	Triangle *restrict triangle = mesh.GetArray<Triangle>(kArrayPrimitive);

	for (machine i = 0; i < xdiv; i++)
	{
		int32 k = i * (zdiv + 1);

		triangle->Set(k + zdiv - 1, k + zdiv + 1, k);
		triangle++;

		for (machine j = 0; j < zdiv - 2; j++)
		{
			if (((i + j) & 1) == 0)
			{
				triangle[0].Set(k + j, k + j + zdiv + 1, k + j + 1);
				triangle[1].Set(k + j + 1, k + j + zdiv + 1, k + j + zdiv + 2);
			}
			else
			{
				triangle[0].Set(k + j, k + j + zdiv + 1, k + j + zdiv + 2);
				triangle[1].Set(k + j, k + j + zdiv + 2, k + j + 1);
			}

			triangle += 2;
		}

		triangle->Set(k + zdiv - 2, k + zdiv * 2 - 1, k + zdiv);
		triangle++;
	}

	BuildStandardArrays(&mesh, GetGeometryLevel(level), transformable);
}


DomeGeometryObject::DomeGeometryObject() : PrimitiveGeometryObject(kPrimitiveDome)
{
	SetConvexPrimitiveFlag();
	SetStaticSurfaceData(2, staticSurfaceData);
}

DomeGeometryObject::DomeGeometryObject(const Vector3D& size) : PrimitiveGeometryObject(kPrimitiveDome)
{
	domeSize = size;
	ratioXY = size.x / size.y;
	ratioXZ = size.x / size.z;

	SetConvexPrimitiveFlag();
	SetBuildLevelCount(2);
	SetMaxSubdivX(32);
	SetMaxSubdivY(8);

	SetStaticSurfaceData(2, staticSurfaceData, true);

	staticSurfaceData[0].textureAlignData[0].alignMode = kTextureAlignNatural;
	staticSurfaceData[0].textureAlignData[0].alignPlane.Set(1.0F, 0.0F, 0.0F, 0.0F);
	staticSurfaceData[0].textureAlignData[1].alignMode = kTextureAlignNatural;
	staticSurfaceData[0].textureAlignData[1].alignPlane.Set(0.0F, 1.0F, 0.0F, 0.0F);

	staticSurfaceData[1].textureAlignData[0].alignMode = kTextureAlignGlobalObjectPlane;
	staticSurfaceData[1].textureAlignData[0].alignPlane.Set(-1.0F, 0.0F, 0.0F, 0.0F);
	staticSurfaceData[1].textureAlignData[1].alignMode = kTextureAlignGlobalObjectPlane;
	staticSurfaceData[1].textureAlignData[1].alignPlane.Set(0.0F, 1.0F, 0.0F, 0.0F);
}

DomeGeometryObject::~DomeGeometryObject()
{
}

void DomeGeometryObject::Pack(Packer& data, unsigned_int32 packFlags) const
{
	PrimitiveGeometryObject::Pack(data, packFlags);

	data << ChunkHeader('SIZE', sizeof(Vector3D));
	data << domeSize;

	data << TerminatorChunk;
}

void DomeGeometryObject::Unpack(Unpacker& data, unsigned_int32 unpackFlags)
{
	PrimitiveGeometryObject::Unpack(data, unpackFlags);
	UnpackChunkList<DomeGeometryObject>(data, unpackFlags);
}

bool DomeGeometryObject::UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags)
{
	switch (chunkHeader->chunkType)
	{
		case 'SIZE':

			data >> domeSize;
			ratioXY = domeSize.x / domeSize.y;
			ratioXZ = domeSize.x / domeSize.z;
			return (true);
	}

	return (false);
}

int32 DomeGeometryObject::GetObjectSize(float *size) const
{
	size[0] = domeSize.x;
	size[1] = domeSize.y;
	size[2] = domeSize.z;
	return (3);
}

void DomeGeometryObject::SetObjectSize(const float *size)
{
	domeSize.x = size[0];
	domeSize.y = size[1];
	domeSize.z = size[2];
	ratioXY = domeSize.x / domeSize.y;
	ratioXZ = domeSize.x / domeSize.z;
}

bool DomeGeometryObject::ExteriorSphere(const Point3D& center, float radius) const
{
	return (center.z < -radius);
}

bool DomeGeometryObject::ExteriorSweptSphere(const Point3D& p1, const Point3D& p2, float radius) const
{
	return ((p1.z < -radius) && (p2.z < -radius));
}

Point3D DomeGeometryObject::CalculatePrimitiveSupportPoint(const Vector3D& direction) const
{
	float x = domeSize.x * domeSize.x * direction.x;
	float y = domeSize.y * domeSize.y * direction.y;

	if (direction.z > 0.0F)
	{
		float z = domeSize.z * domeSize.z * direction.z;
		float r = InverseSqrt(x * direction.x + y * direction.y + z * direction.z);

		return (Point3D(x * r, y * r, z * r));
	}

	float r = x * direction.x + y * direction.y;
	if (r > K::min_float)
	{
		r = InverseSqrt(r);
		return (Point3D(x * r, y * r, 0.0F));
	}

	return (Point3D(0.0F, 0.0F, 0.0F));
}

void DomeGeometryObject::CalculatePrimitiveSupportPointArray(int32 count, const Vector3D *direction, Point3D *support) const
{
	for (machine a = 0; a < count; a++)
	{
		float x = domeSize.x * domeSize.x * direction->x;
		float y = domeSize.y * domeSize.y * direction->y;

		if (direction->z > 0.0F)
		{
			float z = domeSize.z * domeSize.z * direction->z;
			float r = InverseSqrt(x * direction->x + y * direction->y + z * direction->z);

			support->Set(x * r, y * r, z * r);
		}
		else
		{
			float r = x * direction->x + y * direction->y;
			if (r > K::min_float)
			{
				r = InverseSqrt(r);
				support->Set(x * r, y * r, 0.0F);
			}
			else
			{
				support->Set(0.0F, 0.0F, 0.0F);
			}
		}

		direction++;
		support++;
	}
}

void DomeGeometryObject::BuildPrimitiveLevel(int32 level, const Transformable *transformable)
{
	ArrayDescriptor		desc[5];
	Mesh				mesh;

	int32 xdiv = Max(GetMaxSubdivX() >> level, 8) & ~3;
	int32 zdiv = Max(GetMaxSubdivY() >> level, 4);

	int32 vertexCount = xdiv * zdiv + 1;
	int32 triangleCount = xdiv * (zdiv * 2 - 1);

	bool endcap = ((GetPrimitiveFlags() & kPrimitiveEndcap) != 0);
	if (endcap)
	{
		vertexCount += xdiv;
		triangleCount += xdiv - 2;
	}

	desc[0].identifier = kArrayPosition;
	desc[0].elementCount = vertexCount;
	desc[0].elementSize = sizeof(Point3D);
	desc[0].componentCount = 3;

	desc[1].identifier = kArrayNormal;
	desc[1].elementCount = vertexCount;
	desc[1].elementSize = sizeof(Vector3D);
	desc[1].componentCount = 3;

	desc[2].identifier = kArrayTexcoord;
	desc[2].elementCount = vertexCount;
	desc[2].elementSize = sizeof(Point2D);
	desc[2].componentCount = 2;

	desc[3].identifier = kArraySurfaceIndex;
	desc[3].elementCount = vertexCount;
	desc[3].elementSize = 2;
	desc[3].componentCount = 1;

	desc[4].identifier = kArrayPrimitive;
	desc[4].elementCount = triangleCount;
	desc[4].elementSize = sizeof(Triangle);
	desc[4].componentCount = 1;

	mesh.AllocateStorage(vertexCount, 5, desc);

	Point3D *restrict vertex = mesh.GetArray<Point3D>(kArrayPosition);
	Vector3D *restrict normal = mesh.GetArray<Vector3D>(kArrayNormal);
	Point2D *restrict texcoord = mesh.GetArray<Point2D>(kArrayTexcoord);

	float rx = domeSize.x;
	float ry = domeSize.y;
	float rz = domeSize.z;

	vertex->Set(0.0F, 0.0F, rz);
	normal->Set(0.0F, 0.0F, 1.0F);
	texcoord->Set(0.5F, 0.5F);

	float dx = 1.0F / (float) xdiv;
	float dz = 1.0F / (float) zdiv;

	float mx = 1.0F / rx;
	float my = 1.0F / ry;
	float mz = 1.0F / rz;

	for (machine i = 0; i < xdiv; i++)
	{
		float s = (float) i * dx;
		Vector2D v = CosSin(s * K::tau);

		for (machine j = 0; j < zdiv; j++)
		{
			float t = 1.0F - (float) j * dz;
			Vector2D u = CosSin(t * K::tau_over_4);

			float x = rx * v.x * u.y;
			float y = ry * v.y * u.y;
			float z = rz * u.x;

			float nx = v.x * u.y * mx;
			float ny = v.y * u.y * my;
			float nz = u.x * mz;
			float m = InverseSqrt(nx * nx + ny * ny + nz * nz);
			nx *= m;
			ny *= m;
			nz *= m;

			(++vertex)->Set(x, y, z);
			(++normal)->Set(nx, ny, nz);

			float w = InverseSqrt(x * x + y * y) * t * 0.5F;
			(++texcoord)->Set(x * w + 0.5F, y * w + 0.5F);
		}
	}

	int32 count = xdiv * zdiv + 1;
	unsigned_int16 *restrict surfaceIndex = mesh.GetArray<unsigned_int16>(kArraySurfaceIndex) - 1;
	for (machine a = 0; a < count; a++)
	{
		*++surfaceIndex = 0;
	}

	Triangle *restrict triangle = mesh.GetArray<Triangle>(kArrayPrimitive);

	for (machine i = 0; i < xdiv; i++)
	{
		int32 k = i * zdiv;
		int32 n = (i != xdiv - 1) ? zdiv : (1 - xdiv) * zdiv;

		for (machine j = 1; j < zdiv; j++)
		{
			if (((i + j) & 1) == 0)
			{
				triangle[0].Set(k + j, k + j + n, k + j + 1);
				triangle[1].Set(k + j + 1, k + j + n, k + j + n + 1);
			}
			else
			{
				triangle[0].Set(k + j, k + j + n, k + j + n + 1);
				triangle[1].Set(k + j, k + j + n + 1, k + j + 1);
			}

			triangle += 2;
		}

		triangle->Set(k + zdiv, k + n + zdiv, 0);
		triangle++;
	}

	if (endcap)
	{
		const Point3D *vtx = mesh.GetArray<Point3D>(kArrayPosition);

		float dt = K::tau / (float) xdiv;
		float tex = 0.5F / rx;

		for (machine i = xdiv; i > 0; i--)
		{
			machine k = OverflowZero(i, xdiv);
			Vector2D v = CosSin((float) k * dt) & domeSize.GetVector2D();

			*++vertex = vtx[k * zdiv + 1];
			(++normal)->Set(0.0F, 0.0F, -1.0F);
			(++texcoord)->Set(1.0F - (v.x + rx) * tex, (v.y * ratioXY + rx) * tex);
			*++surfaceIndex = 1;
		}

		int32 n = xdiv * zdiv + 1;
		TriangulateDisk(xdiv, n, triangle);
	}

	BuildStandardArrays(&mesh, GetGeometryLevel(level), transformable);
}


TorusGeometryObject::TorusGeometryObject() : PrimitiveGeometryObject(kPrimitiveTorus)
{
	SetStaticSurfaceData(1, staticSurfaceData);
}

TorusGeometryObject::TorusGeometryObject(const Vector2D& primary, float secondary) : PrimitiveGeometryObject(kPrimitiveTorus)
{
	primarySize = primary;
	secondaryRadius = secondary;
	primaryRatioXY = primary.x / primary.y;

	SetBuildLevelCount(2);
	SetMaxSubdivX(32);
	SetMaxSubdivY(16);

	SetStaticSurfaceData(1, staticSurfaceData, true);
	staticSurfaceData[0].textureAlignData[0].alignMode = kTextureAlignNatural;
	staticSurfaceData[0].textureAlignData[0].alignPlane.Set(1.0F, 0.0F, 0.0F, 0.0F);
	staticSurfaceData[0].textureAlignData[1].alignMode = kTextureAlignNatural;
	staticSurfaceData[0].textureAlignData[1].alignPlane.Set(0.0F, 1.0F, 0.0F, 0.0F);
}

TorusGeometryObject::~TorusGeometryObject()
{
}

void TorusGeometryObject::Pack(Packer& data, unsigned_int32 packFlags) const
{
	PrimitiveGeometryObject::Pack(data, packFlags);

	data << ChunkHeader('SIZE', sizeof(Vector2D) + 4);
	data << primarySize;
	data << secondaryRadius;

	data << TerminatorChunk;
}

void TorusGeometryObject::Unpack(Unpacker& data, unsigned_int32 unpackFlags)
{
	PrimitiveGeometryObject::Unpack(data, unpackFlags);
	UnpackChunkList<TorusGeometryObject>(data, unpackFlags);
}

bool TorusGeometryObject::UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags)
{
	switch (chunkHeader->chunkType)
	{
		case 'SIZE':

			data >> primarySize;
			data >> secondaryRadius;
			primaryRatioXY = primarySize.x / primarySize.y;
			return (true);
	}

	return (false);
}

int32 TorusGeometryObject::GetObjectSize(float *size) const
{
	size[0] = primarySize.x;
	size[1] = primarySize.y;
	size[2] = secondaryRadius;
	return (3);
}

void TorusGeometryObject::SetObjectSize(const float *size)
{
	primarySize.x = size[0];
	primarySize.y = size[1];
	secondaryRadius = size[2];
	primaryRatioXY = primarySize.x / primarySize.y;
}

bool TorusGeometryObject::ExteriorSphere(const Point3D& center, float radius) const
{
	return (Fabs(center.z) > secondaryRadius + radius);
}

bool TorusGeometryObject::ExteriorSweptSphere(const Point3D& p1, const Point3D& p2, float radius) const
{
	return ((p1.z * p2.z > 0.0F) && (Fmin(Fabs(p1.z), Fabs(p2.z)) > secondaryRadius + radius));
}

void TorusGeometryObject::BuildPrimitiveLevel(int32 level, const Transformable *transformable)
{
	ArrayDescriptor		desc[4];
	Mesh				mesh;

	int32 xdiv = Max(GetMaxSubdivX() >> level, 8) & ~1;
	int32 zdiv = Max(GetMaxSubdivY() >> level, 8) & ~1;

	int32 vertexCount = (xdiv + 1) * (zdiv + 1);
	int32 triangleCount = xdiv * zdiv * 2;

	desc[0].identifier = kArrayPosition;
	desc[0].elementCount = vertexCount;
	desc[0].elementSize = sizeof(Point3D);
	desc[0].componentCount = 3;

	desc[1].identifier = kArrayNormal;
	desc[1].elementCount = vertexCount;
	desc[1].elementSize = sizeof(Vector3D);
	desc[1].componentCount = 3;

	desc[2].identifier = kArrayTexcoord;
	desc[2].elementCount = vertexCount;
	desc[2].elementSize = sizeof(Point2D);
	desc[2].componentCount = 2;

	desc[3].identifier = kArrayPrimitive;
	desc[3].elementCount = triangleCount;
	desc[3].elementSize = sizeof(Triangle);
	desc[3].componentCount = 1;

	mesh.AllocateStorage(vertexCount, 4, desc);

	Point3D *restrict vertex = mesh.GetArray<Point3D>(kArrayPosition) - 1;
	Vector3D *restrict normal = mesh.GetArray<Vector3D>(kArrayNormal) - 1;
	Point2D *restrict texcoord = mesh.GetArray<Point2D>(kArrayTexcoord) - 1;

	float rx = primarySize.x;
	float ry = primarySize.y;
	float r = secondaryRadius;

	float dx = 1.0F / (float) xdiv;
	float dz = 1.0F / (float) zdiv;

	for (machine i = 0; i < xdiv; i++)
	{
		float s = (float) i * dx;
		Vector2D v = CosSin(s * K::tau);

		float px = rx * v.x;
		float py = ry * v.y;

		float tx = -rx * v.y;
		float ty = ry * v.x;
		float qt = InverseSqrt(tx * tx + ty * ty);
		tx *= qt;
		ty *= qt;

		for (machine j = 0; j < zdiv; j++)
		{
			float t = (float) j * dz;
			Vector2D u = CosSin(t * K::tau);

			float nx = ty * u.x;
			float ny = -tx * u.x;
			float nz = u.y;
			float m = InverseSqrt(nx * nx + ny * ny + nz * nz);

			(++vertex)->Set(px + r * nx, py + r * ny, r * nz);
			(++normal)->Set(nx * m, ny * m, nz * m);
			(++texcoord)->Set(s, t);
		}

		++vertex;
		++normal;
		++texcoord;

		vertex[0] = vertex[-zdiv];
		normal[0] = normal[-zdiv];
		texcoord[0].Set(texcoord[-zdiv].x, 1.0F);
	}

	const Point3D *vtx = mesh.GetArray<Point3D>(kArrayPosition);
	const Vector3D *nml = mesh.GetArray<Vector3D>(kArrayNormal);
	const Point2D *tex = mesh.GetArray<Point2D>(kArrayTexcoord);

	for (machine j = 0; j <= zdiv; j++)
	{
		*++vertex = vtx[j];
		*++normal = nml[j];
		(++texcoord)->Set(1.0F, tex[j].y);
	}

	Triangle *restrict triangle = mesh.GetArray<Triangle>(kArrayPrimitive);

	for (machine i = 0; i < xdiv; i++)
	{
		int32 k = i * (zdiv + 1);
		for (machine j = 0; j < zdiv; j++)
		{
			if (((i + j) & 1) == 0)
			{
				triangle[0].Set(k + j, k + j + zdiv + 1, k + j + 1);
				triangle[1].Set(k + j + 1, k + j + zdiv + 1, k + j + zdiv + 2);
			}
			else
			{
				triangle[0].Set(k + j, k + j + zdiv + 2, k + j + 1);
				triangle[1].Set(k + j, k + j + zdiv + 1, k + j + zdiv + 2);
			}

			triangle += 2;
		}
	}

	BuildStandardArrays(&mesh, GetGeometryLevel(level), transformable);
}


PathPrimitiveGeometryObject::PathPrimitiveGeometryObject(PrimitiveType type) : PrimitiveGeometryObject(type)
{
	texcoordScale = 1.0F;
}

PathPrimitiveGeometryObject::PathPrimitiveGeometryObject(PrimitiveType type, const Path *path) :
		PrimitiveGeometryObject(type),
		primitivePath(*path)
{
	primitivePath.GetBoundingBox(&pathBoundingBox);
	texcoordScale = 1.0F;
}

PathPrimitiveGeometryObject::~PathPrimitiveGeometryObject()
{
}

void PathPrimitiveGeometryObject::Pack(Packer& data, unsigned_int32 packFlags) const
{
	PrimitiveGeometryObject::Pack(data, packFlags);

	PackHandle handle = data.BeginChunk('PATH');
	primitivePath.Pack(data, packFlags);
	data.EndChunk(handle);

	data << ChunkHeader('BBOX', sizeof(Box3D));
	data << primitiveBoundingBox;

	data << ChunkHeader('TSCL', 4);
	data << texcoordScale;

	data << TerminatorChunk;
}

void PathPrimitiveGeometryObject::Unpack(Unpacker& data, unsigned_int32 unpackFlags)
{
	PrimitiveGeometryObject::Unpack(data, unpackFlags);
	UnpackChunkList<PathPrimitiveGeometryObject>(data, unpackFlags);
}

bool PathPrimitiveGeometryObject::UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags)
{
	switch (chunkHeader->chunkType)
	{
		case 'PATH':

			primitivePath.Unpack(data, unpackFlags);
			primitivePath.GetBoundingBox(&pathBoundingBox);
			return (true);

		case 'BBOX':

			data >> primitiveBoundingBox;
			return (true);

		case 'TSCL':

			data >> texcoordScale;
			return (true);
	}

	return (false);
}

void *PathPrimitiveGeometryObject::BeginSettingsUnpack(void)
{
	primitivePath.BeginSettingsUnpack();
	return (PrimitiveGeometryObject::BeginSettingsUnpack());
}

void PathPrimitiveGeometryObject::EndSettingsUnpack(void *cookie)
{
	PrimitiveGeometryObject::EndSettingsUnpack(cookie);
	primitivePath.EndSettingsUnpack(nullptr);
}

int32 PathPrimitiveGeometryObject::GetCategorySettingCount(Type category) const
{
	int32 count = PrimitiveGeometryObject::GetCategorySettingCount(category);
	if (category == kObjectGeometry)
	{
		count += 2;
	}

	return (count);
}

Setting *PathPrimitiveGeometryObject::GetCategorySetting(Type category, int32 index, unsigned_int32 flags) const
{
	if (category == kObjectGeometry)
	{
		int32 i = index - PrimitiveGeometryObject::GetCategorySettingCount(kObjectGeometry);
		if (i >= 0)
		{
			if (flags & kConfigurationScript)
			{
				return (nullptr);
			}

			const StringTable *table = TheInterfaceMgr->GetStringTable();

			if (i == 0)
			{
				const char *title = table->GetString(StringID(kObjectGeometry, 'PATH'));
				return (new HeadingSetting('PATH', title));
			}

			if (i == 1)
			{
				const char *title = table->GetString(StringID(kObjectGeometry, 'PATH', 'PTSC'));
				return (new TextSetting('PTSC', texcoordScale, title));
			}

			return (nullptr);
		}
	}

	return (PrimitiveGeometryObject::GetCategorySetting(category, index, flags));
}

void PathPrimitiveGeometryObject::SetCategorySetting(Type category, const Setting *setting)
{
	if (category == kObjectGeometry)
	{
		Type identifier = setting->GetSettingIdentifier();

		if (identifier == 'PTSC')
		{
			texcoordScale = Text::StringToFloat(static_cast<const TextSetting *>(setting)->GetText());
		}
		else
		{
			PrimitiveGeometryObject::SetCategorySetting(kObjectGeometry, setting);
		}
	}
	else
	{
		PrimitiveGeometryObject::SetCategorySetting(category, setting);
	}
}

bool PathPrimitiveGeometryObject::ExteriorSphere(const Point3D& center, float radius) const
{
	return (primitiveBoundingBox.ExteriorSphere(center, radius));
}

bool PathPrimitiveGeometryObject::ExteriorSweptSphere(const Point3D& p1, const Point3D& p2, float radius) const
{
	return (primitiveBoundingBox.ExteriorSweptSphere(p1, p2, radius));
}

void PathPrimitiveGeometryObject::SetPrimitivePath(const Path *path)
{
	primitivePath = *path;
	primitivePath.GetBoundingBox(&pathBoundingBox);

	InitPathSurfaces();
}


TubeGeometryObject::TubeGeometryObject() : PathPrimitiveGeometryObject(kPrimitiveTube)
{
}

TubeGeometryObject::TubeGeometryObject(const Path *path, const Vector2D& size) : PathPrimitiveGeometryObject(kPrimitiveTube, path)
{
	tubeSize = size;
	ratioXY = size.x / size.y;

	SetBuildLevelCount(2);
	SetMaxSubdivX(8);
	SetMaxSubdivY(16);

	InitPathSurfaces();
}

TubeGeometryObject::~TubeGeometryObject()
{
}

void TubeGeometryObject::Pack(Packer& data, unsigned_int32 packFlags) const
{
	PathPrimitiveGeometryObject::Pack(data, packFlags);

	data << ChunkHeader('SIZE', sizeof(Vector2D));
	data << tubeSize;

	data << TerminatorChunk;
}

void TubeGeometryObject::Unpack(Unpacker& data, unsigned_int32 unpackFlags)
{
	PathPrimitiveGeometryObject::Unpack(data, unpackFlags);
	UnpackChunkList<TubeGeometryObject>(data, unpackFlags);
}

bool TubeGeometryObject::UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags)
{
	switch (chunkHeader->chunkType)
	{
		case 'SIZE':

			data >> tubeSize;
			ratioXY = tubeSize.x / tubeSize.y;
			return (true);
	}

	return (false);
}

int32 TubeGeometryObject::GetObjectSize(float *size) const
{
	size[0] = tubeSize.x;
	size[1] = tubeSize.y;
	return (2);
}

void TubeGeometryObject::SetObjectSize(const float *size)
{
	tubeSize.x = size[0];
	tubeSize.y = size[1];
	ratioXY = tubeSize.x / tubeSize.y;
}

void TubeGeometryObject::InitPathSurfaces(void)
{
	const Path *tubePath = GetPrimitivePath();
	int32 count = tubePath->GetPathComponentCount();
	SetSurfaceCount(count + 2);

	const PathComponent *component = tubePath->GetFirstPathComponent();
	for (machine a = 0; a < count; a++)
	{
		SurfaceData *data = GetSurfaceData(a);
		data->surfaceFlags = 0;
		data->materialIndex = 0;

		data->textureAlignData[0].alignMode = kTextureAlignNatural;
		data->textureAlignData[0].alignPlane.Set(1.0F, 0.0F, 0.0F, 0.0F);
		data->textureAlignData[1].alignMode = kTextureAlignNatural;
		data->textureAlignData[1].alignPlane.Set(0.0F, 1.0F, 0.0F, 0.0F);

		component = component->Next();
	}

	SurfaceData *data = GetSurfaceData(count);

	data[0].surfaceFlags = 0;
	data[0].materialIndex = 0;
	data[0].textureAlignData[0].alignMode = kTextureAlignGlobalObjectPlane;
	data[0].textureAlignData[0].alignPlane.Set(0.0F, 1.0F, 0.0F, 0.0F);
	data[0].textureAlignData[1].alignMode = kTextureAlignGlobalObjectPlane;
	data[0].textureAlignData[1].alignPlane.Set(0.0F, 0.0F, 1.0F, 0.0F);

	data[1].surfaceFlags = 0;
	data[1].materialIndex = 0;
	data[1].textureAlignData[0].alignMode = kTextureAlignGlobalObjectPlane;
	data[1].textureAlignData[0].alignPlane.Set(0.0F, -1.0F, 0.0F, 0.0F);
	data[1].textureAlignData[1].alignMode = kTextureAlignGlobalObjectPlane;
	data[1].textureAlignData[1].alignPlane.Set(0.0F, 0.0F, 1.0F, 0.0F);
}

void TubeGeometryObject::BuildPrimitiveLevel(int32 level, const Transformable *transformable)
{
	ArrayDescriptor		desc[5];
	Mesh				mesh;

	int32 xdiv = Max(GetMaxSubdivX() >> level, 4) & ~1;
	int32 zdiv = Max(GetMaxSubdivY() >> level, 8);

	float dx = 1.0F / (float) xdiv;
	float dz = 1.0F / (float) zdiv;

	int32 vertexCount = 0;
	int32 triangleCount = 0;
	int32 componentCount = 0;
	float totalLength = 0.0F;

	const Path *tubePath = GetPrimitivePath();
	const PathComponent *component = tubePath->GetFirstPathComponent();
	const Point3D& pathBeginPosition = component->GetBeginPosition();

	do
	{
		if (component->GetPathType() == kPathLinear)
		{
			vertexCount += (xdiv + 1) * 2;
			triangleCount += xdiv * 2;

			const LinearPathComponent *linearComponent = static_cast<const LinearPathComponent *>(component);
			totalLength += Magnitude(linearComponent->GetControlPoint(1) - linearComponent->GetControlPoint(0));
		}
		else
		{
			vertexCount += (xdiv + 1) * (zdiv + 1);
			triangleCount += xdiv * zdiv * 2;

			Point3D p1 = component->GetBeginPosition();
			for (machine j = 1; j <= zdiv; j++)
			{
				Point3D p2 = component->GetPosition((float) j * dz);
				totalLength += Magnitude(p2 - p1);
				p1 = p2;
			}
		}

		componentCount++;
		component = component->Next();
	} while (component);

	bool closed = (tubePath->GetLastPathComponent()->GetEndPosition() == pathBeginPosition);
	bool endcap = ((!closed) && ((GetPrimitiveFlags() & kPrimitiveEndcap) != 0));

	if (endcap)
	{
		vertexCount += xdiv * 2;
		triangleCount += xdiv * 2 - 4;
	}

	desc[0].identifier = kArrayPosition;
	desc[0].elementCount = vertexCount;
	desc[0].elementSize = sizeof(Point3D);
	desc[0].componentCount = 3;

	desc[1].identifier = kArrayNormal;
	desc[1].elementCount = vertexCount;
	desc[1].elementSize = sizeof(Vector3D);
	desc[1].componentCount = 3;

	desc[2].identifier = kArrayTexcoord;
	desc[2].elementCount = vertexCount;
	desc[2].elementSize = sizeof(Point2D);
	desc[2].componentCount = 2;

	desc[3].identifier = kArraySurfaceIndex;
	desc[3].elementCount = vertexCount;
	desc[3].elementSize = 2;
	desc[3].componentCount = 1;

	desc[4].identifier = kArrayPrimitive;
	desc[4].elementCount = triangleCount;
	desc[4].elementSize = sizeof(Triangle);
	desc[4].componentCount = 1;

	mesh.AllocateStorage(vertexCount, 5, desc);

	Point3D *restrict vertex = mesh.GetArray<Point3D>(kArrayPosition) - 1;
	Vector3D *restrict normal = mesh.GetArray<Vector3D>(kArrayNormal) - 1;
	Point2D *restrict texcoord = mesh.GetArray<Point2D>(kArrayTexcoord) - 1;
	unsigned_int16 *restrict surfaceIndex = mesh.GetArray<unsigned_int16>(kArraySurfaceIndex) - 1;
	Triangle *restrict triangle = mesh.GetArray<Triangle>(kArrayPrimitive);

	float rx = tubeSize.x;
	float ry = tubeSize.y;

	float mx = 1.0F / rx;
	float my = 1.0F / ry;

	int32 componentIndex = 0;
	int32 baseVertexIndex = 0;

	float invLength = GetTexcoordScale() / totalLength;
	float tex1 = 0.0F;

	component = tubePath->GetFirstPathComponent();
	while (component)
	{
		const Point3D	*endPosition;

		const PathComponent *nextComponent = component->Next();
		if (nextComponent)
		{
			endPosition = &nextComponent->GetBeginPosition();
		}
		else
		{
			endPosition = (closed) ? &pathBeginPosition : &component->GetEndPosition();
		}

		Vector3D endTangent = Normalize(component->GetEndTangent());

		if (component->GetPathType() == kPathLinear)
		{
			const Point3D& beginPosition = component->GetBeginPosition();
			Vector3D beginTangent = Normalize(component->GetBeginTangent());

			Vector3D beginRight = Normalize(beginTangent % tubePath->GetPathNormal());
			Vector3D beginDown = beginTangent % beginRight;

			Vector3D endRight = Normalize(endTangent % tubePath->GetPathNormal());
			Vector3D endDown = endTangent % endRight;

			float tex2 = tex1 + Magnitude(*endPosition - beginPosition) * invLength;

			for (machine i = 0; i < xdiv; i++)
			{
				float s = (float) i * dx;
				Vector2D v = CosSin(s * K::tau);
				float x = rx * v.x;
				float y = ry * v.y;

				float nx = v.x * mx;
				float ny = v.y * my;
				float m = InverseSqrt(nx * nx + ny * ny);
				nx *= m;
				ny *= m;

				*++vertex = beginPosition + beginRight * x + beginDown * y;
				*++normal = beginRight * nx + beginDown * ny;
				(++texcoord)->Set(s, tex1);
				*++surfaceIndex = (unsigned_int16) componentIndex;

				*++vertex = *endPosition + endRight * x + endDown * y;
				*++normal = endRight * nx + endDown * ny;
				(++texcoord)->Set(s, tex2);
				*++surfaceIndex = (unsigned_int16) componentIndex;
			}

			const Point3D *vtx = mesh.GetArray<Point3D>(kArrayPosition) + baseVertexIndex;
			const Vector3D *nrm = mesh.GetArray<Vector3D>(kArrayNormal) + baseVertexIndex;

			*++vertex = vtx[0];
			*++normal = nrm[0];
			(++texcoord)->Set(1.0F, tex1);
			*++surfaceIndex = (unsigned_int16) componentIndex;

			*++vertex = vtx[1];
			*++normal = nrm[1];
			(++texcoord)->Set(1.0F, tex2);
			*++surfaceIndex = (unsigned_int16) componentIndex;

			for (machine i = 0; i < xdiv; i++)
			{
				int32 k = baseVertexIndex + i * 2;
				if ((i & 1) == 0)
				{
					triangle[0].Set(k, k + 2, k + 1);
					triangle[1].Set(k + 1, k + 2, k + 3);
				}
				else
				{
					triangle[0].Set(k, k + 2, k + 3);
					triangle[1].Set(k, k + 3, k + 1);
				}

				triangle += 2;
			}

			tex1 = tex2;
			baseVertexIndex += (xdiv + 1) * 2;
		}
		else
		{
			float	tex2;

			for (machine i = 0; i < xdiv; i++)
			{
				Point3D		p1;

				float s = (float) i * dx;
				Vector2D v = CosSin(s * K::tau);
				float x = rx * v.x;
				float y = ry * v.y;

				float nx = v.x * mx;
				float ny = v.y * my;
				float m = InverseSqrt(nx * nx + ny * ny);
				nx *= m;
				ny *= m;

				tex2 = tex1;
				for (machine j = 0; j < zdiv; j++)
				{
					float t = (float) j * dz;

					Point3D p2 = component->GetPosition(t);
					if (j != 0)
					{
						tex2 += Magnitude(p2 - p1) * invLength;
					}

					p1 = p2;

					Vector3D forward = Normalize(component->GetTangent(t));
					Vector3D right = Normalize(forward % tubePath->GetPathNormal());
					Vector3D down = forward % right;

					*++vertex = p2 + right * x + down * y;
					*++normal = right * nx + down * ny;
					(++texcoord)->Set(s, tex2);
					*++surfaceIndex = (unsigned_int16) componentIndex;
				}

				tex2 += Magnitude(*endPosition - p1) * invLength;

				Vector3D right = Normalize(endTangent % tubePath->GetPathNormal());
				Vector3D down = endTangent % right;

				*++vertex = *endPosition + right * x + down * y;
				*++normal = right * nx + down * ny;
				(++texcoord)->Set(s, tex2);
				*++surfaceIndex = (unsigned_int16) componentIndex;
			}

			const Point3D *vtx = mesh.GetArray<Point3D>(kArrayPosition) + baseVertexIndex;
			const Vector3D *nrm = mesh.GetArray<Vector3D>(kArrayNormal) + baseVertexIndex;
			const Point2D *tex = mesh.GetArray<Point2D>(kArrayTexcoord) + baseVertexIndex;

			for (machine j = 0; j < zdiv; j++)
			{
				*++vertex = vtx[j];
				*++normal = nrm[j];
				(++texcoord)->Set(1.0F, tex[j].y);
				*++surfaceIndex = (unsigned_int16) componentIndex;
			}

			*++vertex = vtx[zdiv];
			*++normal = nrm[zdiv];
			(++texcoord)->Set(1.0F, tex[zdiv].y);
			*++surfaceIndex = (unsigned_int16) componentIndex;

			for (machine i = 0; i < xdiv; i++)
			{
				int32 k = baseVertexIndex + i * (zdiv + 1);
				for (machine j = 0; j < zdiv; j++)
				{
					if (((i + j) & 1) == 0)
					{
						triangle[0].Set(k + j, k + j + zdiv + 1, k + j + 1);
						triangle[1].Set(k + j + 1, k + j + zdiv + 1, k + j + zdiv + 2);
					}
					else
					{
						triangle[0].Set(k + j, k + j + zdiv + 1, k + j + zdiv + 2);
						triangle[1].Set(k + j, k + j + zdiv + 2, k + j + 1);
					}

					triangle += 2;
				}
			}

			tex1 = tex2;
			baseVertexIndex += (xdiv + 1) * (zdiv + 1);
		}

		componentIndex++;
		component = nextComponent;
	}

	if (endcap)
	{
		component = tubePath->GetFirstPathComponent();
		int32 delta = (component->GetPathType() == kPathLinear) ? 2 : zdiv + 1;
		Vector3D nrml = Normalize(-component->GetBeginTangent());

		const Point3D *vtx = mesh.GetArray<Point3D>(kArrayPosition);
		float dt = K::tau / (float) xdiv;
		float tex = 0.5F / rx;

		for (machine i = xdiv; i > 0; i--)
		{
			machine k = OverflowZero(i, xdiv);
			Vector2D v = CosSin((float) k * dt) & tubeSize;

			*++vertex = vtx[k * delta];
			*++normal = nrml;
			(++texcoord)->Set(1.0F - (v.x + rx) * tex, (v.y * ratioXY + rx) * tex);
			*++surfaceIndex = (unsigned_int16) componentCount;
		}

		component = tubePath->GetLastPathComponent();
		delta = (component->GetPathType() == kPathLinear) ? 2 : zdiv + 1;
		nrml = Normalize(component->GetEndTangent());

		vtx += baseVertexIndex - xdiv * delta - 1;

		for (machine i = 0; i < xdiv; i++)
		{
			Vector2D v = CosSin((float) i * dt) & tubeSize;

			*++vertex = vtx[i * delta];
			*++normal = nrml;
			(++texcoord)->Set((v.x + rx) * tex, (v.y * ratioXY + rx) * tex);
			*++surfaceIndex = (unsigned_int16) (componentCount + 1);
		}

		TriangulateDisk(xdiv, baseVertexIndex, triangle);
		TriangulateDisk(xdiv, baseVertexIndex + xdiv, triangle + (xdiv - 2));
	}

	BuildStandardArrays(&mesh, GetGeometryLevel(level), transformable);

	if (level == 0)
	{
		primitiveBoundingBox.Calculate(vertexCount, mesh.GetArray<Point3D>(kArrayPosition));
	}
}


ExtrusionGeometryObject::ExtrusionGeometryObject() : PathPrimitiveGeometryObject(kPrimitiveExtrusion)
{
}

ExtrusionGeometryObject::ExtrusionGeometryObject(const Path *path, const Vector2D& size, float height) : PathPrimitiveGeometryObject(kPrimitiveExtrusion, path)
{
	extrusionSize = size;
	extrusionHeight = height;

	SetBuildLevelCount(1 + !path->LinearPath());
	SetMaxSubdivX(16);

	InitPathSurfaces();
}

ExtrusionGeometryObject::~ExtrusionGeometryObject()
{
}

void ExtrusionGeometryObject::Pack(Packer& data, unsigned_int32 packFlags) const
{
	PathPrimitiveGeometryObject::Pack(data, packFlags);

	data << ChunkHeader('SIZE', sizeof(Vector2D));
	data << extrusionSize;

	data << ChunkHeader('HITE', 4);
	data << extrusionHeight;

	data << TerminatorChunk;
}

void ExtrusionGeometryObject::Unpack(Unpacker& data, unsigned_int32 unpackFlags)
{
	PathPrimitiveGeometryObject::Unpack(data, unpackFlags);
	UnpackChunkList<ExtrusionGeometryObject>(data, unpackFlags);
}

bool ExtrusionGeometryObject::UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags)
{
	switch (chunkHeader->chunkType)
	{
		case 'SIZE':

			data >> extrusionSize;
			return (true);

		case 'HITE':

			data >> extrusionHeight;
			return (true);
	}

	return (false);
}

int32 ExtrusionGeometryObject::GetObjectSize(float *size) const
{
	size[0] = extrusionSize.x;
	size[1] = extrusionSize.y;
	size[2] = extrusionHeight;
	return (3);
}

void ExtrusionGeometryObject::SetObjectSize(const float *size)
{
	extrusionSize.x = size[0];
	extrusionSize.y = size[1];
	extrusionHeight = size[2];
}

void ExtrusionGeometryObject::InitPathSurfaces(void)
{
	const Path *extrusionPath = GetPrimitivePath();
	int32 count = extrusionPath->GetPathComponentCount();
	SetSurfaceCount(count + 2);

	const PathComponent *component = extrusionPath->GetFirstPathComponent();
	for (machine a = 0; a < count; a++)
	{
		SurfaceData *data = GetSurfaceData(a);
		data->surfaceFlags = 0;
		data->materialIndex = 0;

		data->textureAlignData[0].alignMode = kTextureAlignNatural;
		data->textureAlignData[0].alignPlane.Set(1.0F, 0.0F, 0.0F, 0.0F);
		data->textureAlignData[1].alignMode = kTextureAlignGlobalObjectPlane;
		data->textureAlignData[1].alignPlane.Set(0.0F, 0.0F, 1.0F, 0.0F);

		component = component->Next();
	}

	SurfaceData *data = GetSurfaceData(count);

	data[0].surfaceFlags = 0;
	data[0].materialIndex = 0;
	data[0].textureAlignData[0].alignMode = kTextureAlignGlobalObjectPlane;
	data[0].textureAlignData[0].alignPlane.Set(1.0F, 0.0F, 0.0F, 0.0F);
	data[0].textureAlignData[1].alignMode = kTextureAlignGlobalObjectPlane;
	data[0].textureAlignData[1].alignPlane.Set(0.0F, 1.0F, 0.0F, 0.0F);

	data[1].surfaceFlags = 0;
	data[1].materialIndex = 0;
	data[1].textureAlignData[0].alignMode = kTextureAlignGlobalObjectPlane;
	data[1].textureAlignData[0].alignPlane.Set(-1.0F, 0.0F, 0.0F, 0.0F);
	data[1].textureAlignData[1].alignMode = kTextureAlignGlobalObjectPlane;
	data[1].textureAlignData[1].alignPlane.Set(0.0F, 1.0F, 0.0F, 0.0F);
}

void ExtrusionGeometryObject::BuildPrimitiveLevel(int32 level, const Transformable *transformable)
{
	ArrayDescriptor		desc[5];
	Mesh				mesh;

	int32 xdiv = Max(GetMaxSubdivX() >> level, 8) & ~1;
	int32 zdiv = Max(GetMaxSubdivY() >> level, 1);

	float dx = 1.0F / (float) xdiv;
	float dz = 1.0F / (float) zdiv;

	int32 vertexCount = 0;
	int32 capVertexCount = 0;
	int32 triangleCount = 0;
	int32 componentCount = 0;
	float totalLength = 0.0F;

	const Box3D& bounds = GetPathBoundingBox();

	bool endcap = ((GetPrimitiveFlags() & kPrimitiveEndcap) != 0);
	float area = 0.0F;

	const Path *extrusionPath = GetPrimitivePath();
	const Point3D *endPosition = &extrusionPath->GetLastPathComponent()->GetEndPosition();

	const PathComponent *component = extrusionPath->GetFirstPathComponent();
	const Point3D& pathBeginPosition = component->GetBeginPosition();

	do
	{
		if (component->GetPathType() == kPathLinear)
		{
			vertexCount += 2 * (zdiv + 1);
			capVertexCount += 1;
			triangleCount += zdiv * 2;

			const LinearPathComponent *linearComponent = static_cast<const LinearPathComponent *>(component);
			totalLength += Magnitude(linearComponent->GetControlPoint(1) - linearComponent->GetControlPoint(0));
			area += ((linearComponent->GetBeginPosition() - pathBeginPosition) % (linearComponent->GetEndPosition() - pathBeginPosition)).z;
		}
		else
		{
			vertexCount += (xdiv + 1) * (zdiv + 1);
			capVertexCount += xdiv;
			triangleCount += xdiv * zdiv * 2;

			Point3D p1 = component->GetBeginPosition();
			for (machine i = 1; i <= xdiv; i++)
			{
				Point3D p2 = component->GetPosition((float) i * dx);

				totalLength += Magnitude(p2 - p1);
				area += ((p1 - pathBeginPosition) % (p2 - pathBeginPosition)).z;

				p1 = p2;
			}
		}

		const Point3D& beginPosition = component->GetBeginPosition();
		if (beginPosition != *endPosition)
		{
			endcap = false;
		}

		endPosition = &component->GetEndPosition();

		componentCount++;
		component = component->Next();
	} while (component);

	float winding = NonzeroFsgn(area);

	bool closed = (extrusionPath->GetLastPathComponent()->GetEndPosition() == pathBeginPosition);
	if (!closed)
	{
		endcap = false;
	}

	if (endcap)
	{
		vertexCount += capVertexCount * 2;
		triangleCount += (capVertexCount - 2) * 2;
	}

	desc[0].identifier = kArrayPosition;
	desc[0].elementCount = vertexCount;
	desc[0].elementSize = sizeof(Point3D);
	desc[0].componentCount = 3;

	desc[1].identifier = kArrayNormal;
	desc[1].elementCount = vertexCount;
	desc[1].elementSize = sizeof(Vector3D);
	desc[1].componentCount = 3;

	desc[2].identifier = kArrayTexcoord;
	desc[2].elementCount = vertexCount;
	desc[2].elementSize = sizeof(Point2D);
	desc[2].componentCount = 2;

	desc[3].identifier = kArraySurfaceIndex;
	desc[3].elementCount = vertexCount;
	desc[3].elementSize = 2;
	desc[3].componentCount = 1;

	desc[4].identifier = kArrayPrimitive;
	desc[4].elementCount = triangleCount;
	desc[4].elementSize = sizeof(Triangle);
	desc[4].componentCount = 1;

	mesh.AllocateStorage(vertexCount, 5, desc);

	Point3D *restrict vertex = mesh.GetArray<Point3D>(kArrayPosition) - 1;
	Vector3D *restrict normal = mesh.GetArray<Vector3D>(kArrayNormal) - 1;
	Point2D *restrict texcoord = mesh.GetArray<Point2D>(kArrayTexcoord) - 1;
	unsigned_int16 *restrict surfaceIndex = mesh.GetArray<unsigned_int16>(kArraySurfaceIndex) - 1;
	Triangle *restrict triangle = mesh.GetArray<Triangle>(kArrayPrimitive);

	Vector2D pathSize = bounds.max.GetVector2D() - bounds.min.GetVector2D();
	float sx = extrusionSize.x / pathSize.x;
	float sy = extrusionSize.y / pathSize.y;
	float h = extrusionHeight;

	int32 componentIndex = 0;
	int32 baseVertexIndex = 0;

	float invLength = GetTexcoordScale() / totalLength;
	float tex1 = 0.0F;

	component = extrusionPath->GetFirstPathComponent();
	do
	{
		const PathComponent *nextComponent = component->Next();
		if (nextComponent)
		{
			endPosition = &nextComponent->GetBeginPosition();
		}
		else
		{
			endPosition = (closed) ? &pathBeginPosition : &component->GetEndPosition();
		}

		if (component->GetPathType() == kPathLinear)
		{
			Point3D p1 = component->GetBeginPosition() - bounds.min.GetVector3D();
			Point3D p2 = *endPosition - bounds.min.GetVector3D();

			float tex2 = tex1 + Magnitude(p2 - p1) * invLength;

			p1.x *= sx;
			p1.y *= sy;
			p2.x *= sx;
			p2.y *= sy;

			float nx = p2.y - p1.y;
			float ny = p1.x - p2.x;
			float m = InverseSqrt(nx * nx + ny * ny);
			nx *= m;
			ny *= m;

			for (machine j = 0; j < zdiv; j++)
			{
				float t = (float) j * dz;

				(++vertex)->Set(p1.x, p1.y, p1.z + t * h);
				(++normal)->Set(nx, ny, 0.0F);
				(++texcoord)->Set(tex1, t);
				*++surfaceIndex = (unsigned_int16) componentIndex;
			}

			(++vertex)->Set(p1.x, p1.y, p1.z + h);
			(++normal)->Set(nx, ny, 0.0F);
			(++texcoord)->Set(tex1, 1.0F);
			*++surfaceIndex = (unsigned_int16) componentIndex;

			for (machine j = 0; j < zdiv; j++)
			{
				float t = (float) j * dz;

				(++vertex)->Set(p2.x, p2.y, p2.z + t * h);
				(++normal)->Set(nx, ny, 0.0F);
				(++texcoord)->Set(tex2, t);
				*++surfaceIndex = (unsigned_int16) componentIndex;
			}

			(++vertex)->Set(p2.x, p2.y, p2.z + h);
			(++normal)->Set(nx, ny, 0.0F);
			(++texcoord)->Set(tex2, 1.0F);
			*++surfaceIndex = (unsigned_int16) componentIndex;

			for (machine j = 0; j < zdiv; j++)
			{
				if ((j & 1) == 0)
				{
					triangle[0].Set(baseVertexIndex + j, baseVertexIndex + j + zdiv + 1, baseVertexIndex + j + 1);
					triangle[1].Set(baseVertexIndex + j + 1, baseVertexIndex + j + zdiv + 1, baseVertexIndex + j + zdiv + 2);
				}
				else
				{
					triangle[0].Set(baseVertexIndex + j, baseVertexIndex + j + zdiv + 1, baseVertexIndex + j + zdiv + 2);
					triangle[1].Set(baseVertexIndex + j, baseVertexIndex + j + zdiv + 2, baseVertexIndex + j + 1);
				}

				triangle += 2;
			}

			tex1 = tex2;
			baseVertexIndex += (zdiv + 1) * 2;
		}
		else
		{
			Point3D		p1;

			for (machine i = 0; i < xdiv; i++)
			{
				float s = (float) i * dx;

				Point3D p2 = component->GetPosition(s) - bounds.min.GetVector3D();
				if (i != 0)
				{
					tex1 += Magnitude(p2 - p1) * invLength;
				}

				p1 = p2;

				p2.x *= sx;
				p2.y *= sy;

				Vector3D tangent = component->GetTangent(s);
				float nx = tangent.y;
				float ny = -tangent.x;
				float m = InverseSqrt(nx * nx + ny * ny);
				nx *= m;
				ny *= m;

				for (machine j = 0; j < zdiv; j++)
				{
					float t = (float) j * dz;

					(++vertex)->Set(p2.x, p2.y, p2.z + t * h);
					(++normal)->Set(nx, ny, 0.0F);
					(++texcoord)->Set(tex1, t);
					*++surfaceIndex = (unsigned_int16) componentIndex;
				}

				(++vertex)->Set(p2.x, p2.y, p2.z + h);
				(++normal)->Set(nx, ny, 0.0F);
				(++texcoord)->Set(tex1, 1.0F);
				*++surfaceIndex = (unsigned_int16) componentIndex;
			}

			Point3D p2 = *endPosition - bounds.min.GetVector3D();
			tex1 += Magnitude(p2 - p1) * invLength;

			p2.x *= sx;
			p2.y *= sy;

			Vector3D endTangent = Normalize(component->GetEndTangent());
			float nx = endTangent.y;
			float ny = -endTangent.x;
			float m = InverseSqrt(nx * nx + ny * ny);
			nx *= m;
			ny *= m;

			for (machine j = 0; j < zdiv; j++)
			{
				float t = (float) j * dz;

				(++vertex)->Set(p2.x, p2.y, p2.z + t * h);
				(++normal)->Set(nx, ny, 0.0F);
				(++texcoord)->Set(tex1, t);
				*++surfaceIndex = (unsigned_int16) componentIndex;
			}

			(++vertex)->Set(p2.x, p2.y, p2.z + h);
			(++normal)->Set(nx, ny, 0.0F);
			(++texcoord)->Set(tex1, 1.0F);
			*++surfaceIndex = (unsigned_int16) componentIndex;

			for (machine i = 0; i < xdiv; i++)
			{
				int32 k = baseVertexIndex + i * (zdiv + 1);
				for (machine j = 0; j < zdiv; j++)
				{
					if (((i + j) & 1) == 0)
					{
						triangle[0].Set(k + j, k + j + zdiv + 1, k + j + 1);
						triangle[1].Set(k + j + 1, k + j + zdiv + 1, k + j + zdiv + 2);
					}
					else
					{
						triangle[0].Set(k + j, k + j + zdiv + 1, k + j + zdiv + 2);
						triangle[1].Set(k + j, k + j + zdiv + 2, k + j + 1);
					}

					triangle += 2;
				}
			}

			baseVertexIndex += (xdiv + 1) * (zdiv + 1);
		}

		componentIndex++;
		component = nextComponent;
	} while (component);

	if (endcap)
	{
		Point3D *topVertex = vertex + capVertexCount;
		Vector3D *topNormal = normal + capVertexCount;
		Point2D *topTexcoord = texcoord + capVertexCount;
		unsigned_int16 *topSurfaceIndex = surfaceIndex + capVertexCount;

		Point3D *bottomVertex = topVertex + 1;
		Vector3D *bottomNormal = topNormal + 1;
		Point2D *bottomTexcoord = topTexcoord + 1;
		unsigned_int16 *bottomSurfaceIndex = topSurfaceIndex + 1;

		float tx = 1.0F / pathSize.x;
		float ty = 1.0F / pathSize.y;

		const Point3D *vtx = mesh.GetArray<Point3D>(kArrayPosition);

		component = extrusionPath->GetFirstPathComponent();
		do
		{
			if (component->GetPathType() == kPathLinear)
			{
				const Point3D& p1 = vtx[0];
				const Point3D& p2 = vtx[zdiv];

				*--bottomVertex = p1;
				(--bottomNormal)->Set(0.0F, 0.0F, -winding);
				(--bottomTexcoord)->Set(1.0F - p1.x * tx, p1.y * ty);
				*--bottomSurfaceIndex = (unsigned_int16) componentCount;

				*++topVertex = p2;
				(++topNormal)->Set(0.0F, 0.0F, winding);
				(++topTexcoord)->Set(p2.x * tx, p2.y * ty);
				*++topSurfaceIndex = (unsigned_int16) (componentCount + 1);

				vtx += 2 * (zdiv + 1);
			}
			else
			{
				for (machine i = 0; i < xdiv; i++)
				{
					const Point3D& p1 = vtx[0];
					const Point3D& p2 = vtx[zdiv];

					*--bottomVertex = p1;
					(--bottomNormal)->Set(0.0F, 0.0F, -winding);
					(--bottomTexcoord)->Set(1.0F - p1.x * tx, p1.y * ty);
					*--bottomSurfaceIndex = (unsigned_int16) componentCount;

					*++topVertex = p2;
					(++topNormal)->Set(0.0F, 0.0F, winding);
					(++topTexcoord)->Set(p2.x * tx, p2.y * ty);
					*++topSurfaceIndex = (unsigned_int16) (componentCount + 1);

					vtx += zdiv + 1;
				}

				vtx += zdiv + 1;
			}

			component = component->Next();
		} while (component);

		int32 capTriangleCount = capVertexCount - 2;
		int32 capVertexIndex = vertexCount - capVertexCount * 2;

		int32 count = Math::TriangulatePolygon(capVertexCount, vertex + 1, Vector3D(0.0F, 0.0F, -winding), triangle, capVertexIndex);
		for (machine a = count; a < capTriangleCount; a++)
		{
			triangle[a].Set(capVertexIndex, capVertexIndex, capVertexIndex);
		}

		triangle += capTriangleCount;
		capVertexIndex = vertexCount - capVertexCount;

		count = Math::TriangulatePolygon(capVertexCount, vertex + (capVertexCount + 1), Vector3D(0.0F, 0.0F, winding), triangle, capVertexIndex);
		for (machine a = count; a < capTriangleCount; a++)
		{
			triangle[a].Set(capVertexIndex, capVertexIndex, capVertexIndex);
		}
	}

	BuildStandardArrays(&mesh, GetGeometryLevel(level), transformable);

	if (level == 0)
	{
		primitiveBoundingBox.Calculate(vertexCount, mesh.GetArray<Point3D>(kArrayPosition));
	}
}


RevolutionGeometryObject::RevolutionGeometryObject() : PathPrimitiveGeometryObject(kPrimitiveRevolution)
{
}

RevolutionGeometryObject::RevolutionGeometryObject(const Path *path, const Vector2D& size, float height) : PathPrimitiveGeometryObject(kPrimitiveRevolution, path)
{
	revolutionSize = size;
	revolutionHeight = height;

	SetBuildLevelCount(1 + !path->LinearPath());
	SetMaxSubdivX(32);
	SetMaxSubdivY(16);

	InitPathSurfaces();
}

RevolutionGeometryObject::~RevolutionGeometryObject()
{
}

void RevolutionGeometryObject::Pack(Packer& data, unsigned_int32 packFlags) const
{
	PathPrimitiveGeometryObject::Pack(data, packFlags);

	data << ChunkHeader('SIZE', sizeof(Vector2D));
	data << revolutionSize;

	data << ChunkHeader('HITE', 4);
	data << revolutionHeight;

	data << TerminatorChunk;
}

void RevolutionGeometryObject::Unpack(Unpacker& data, unsigned_int32 unpackFlags)
{
	PathPrimitiveGeometryObject::Unpack(data, unpackFlags);
	UnpackChunkList<RevolutionGeometryObject>(data, unpackFlags);
}

bool RevolutionGeometryObject::UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags)
{
	switch (chunkHeader->chunkType)
	{
		case 'SIZE':

			data >> revolutionSize;
			ratioXY = revolutionSize.x / revolutionSize.y;
			return (true);

		case 'HITE':

			data >> revolutionHeight;
			return (true);
	}

	return (false);
}

int32 RevolutionGeometryObject::GetObjectSize(float *size) const
{
	size[0] = revolutionSize.x;
	size[1] = revolutionSize.y;
	size[2] = revolutionHeight;
	return (3);
}

void RevolutionGeometryObject::SetObjectSize(const float *size)
{
	revolutionSize.x = size[0];
	revolutionSize.y = size[1];
	revolutionHeight = size[2];
	ratioXY = revolutionSize.x / revolutionSize.y;
}

void RevolutionGeometryObject::InitPathSurfaces(void)
{
	const Path *revolutionPath = GetPrimitivePath();
	int32 count = revolutionPath->GetPathComponentCount();
	SetSurfaceCount(count + 2);

	const PathComponent *component = revolutionPath->GetFirstPathComponent();
	for (machine a = 0; a < count; a++)
	{
		SurfaceData *data = GetSurfaceData(a);
		data->surfaceFlags = 0;
		data->materialIndex = 0;

		data->textureAlignData[0].alignMode = kTextureAlignNatural;
		data->textureAlignData[0].alignPlane.Set(1.0F, 0.0F, 0.0F, 0.0F);
		data->textureAlignData[1].alignMode = kTextureAlignGlobalObjectPlane;
		data->textureAlignData[1].alignPlane.Set(0.0F, 0.0F, 1.0F, 0.0F);

		component = component->Next();
	}

	SurfaceData *data = GetSurfaceData(count);

	data[0].surfaceFlags = 0;
	data[0].materialIndex = 0;
	data[0].textureAlignData[0].alignMode = kTextureAlignGlobalObjectPlane;
	data[0].textureAlignData[0].alignPlane.Set(1.0F, 0.0F, 0.0F, 0.0F);
	data[0].textureAlignData[1].alignMode = kTextureAlignGlobalObjectPlane;
	data[0].textureAlignData[1].alignPlane.Set(0.0F, 1.0F, 0.0F, 0.0F);

	data[1].surfaceFlags = 0;
	data[1].materialIndex = 0;
	data[1].textureAlignData[0].alignMode = kTextureAlignGlobalObjectPlane;
	data[1].textureAlignData[0].alignPlane.Set(-1.0F, 0.0F, 0.0F, 0.0F);
	data[1].textureAlignData[1].alignMode = kTextureAlignGlobalObjectPlane;
	data[1].textureAlignData[1].alignPlane.Set(0.0F, 1.0F, 0.0F, 0.0F);
}

void RevolutionGeometryObject::BuildPrimitiveLevel(int32 level, const Transformable *transformable)
{
	ArrayDescriptor		desc[5];
	Mesh				mesh;

	int32 xdiv = Max(GetMaxSubdivX() >> level, 8) & ~1;
	int32 zdiv = Max(GetMaxSubdivY() >> level, 8);

	float dx = 1.0F / (float) xdiv;
	float dz = 1.0F / (float) zdiv;

	int32 vertexCount = 0;
	int32 triangleCount = 0;
	int32 componentCount = 0;
	float totalLength = 0.0F;

	const Path *revolutionPath = GetPrimitivePath();
	const PathComponent *component = revolutionPath->GetFirstPathComponent();
	while (component)
	{
		if (component->GetPathType() == kPathLinear)
		{
			vertexCount += (xdiv + 1) * 2;
			triangleCount += xdiv * 2;

			const LinearPathComponent *linearComponent = static_cast<const LinearPathComponent *>(component);
			totalLength += Magnitude(linearComponent->GetControlPoint(1) - linearComponent->GetControlPoint(0));
		}
		else
		{
			vertexCount += (xdiv + 1) * (zdiv + 1);
			triangleCount += xdiv * zdiv * 2;

			Point3D p1 = component->GetBeginPosition();
			for (machine j = 1; j <= zdiv; j++)
			{
				Point3D p2 = component->GetPosition((float) j * dz);
				totalLength += Magnitude(p2 - p1);
				p1 = p2;
			}
		}

		componentCount++;
		component = component->Next();
	}

	bool endcap = ((GetPrimitiveFlags() & kPrimitiveEndcap) != 0);
	if (endcap)
	{
		vertexCount += xdiv * 2;
		triangleCount += xdiv * 2 - 4;
	}

	desc[0].identifier = kArrayPosition;
	desc[0].elementCount = vertexCount;
	desc[0].elementSize = sizeof(Point3D);
	desc[0].componentCount = 3;

	desc[1].identifier = kArrayNormal;
	desc[1].elementCount = vertexCount;
	desc[1].elementSize = sizeof(Vector3D);
	desc[1].componentCount = 3;

	desc[2].identifier = kArrayTexcoord;
	desc[2].elementCount = vertexCount;
	desc[2].elementSize = sizeof(Point2D);
	desc[2].componentCount = 2;

	desc[3].identifier = kArraySurfaceIndex;
	desc[3].elementCount = vertexCount;
	desc[3].elementSize = 2;
	desc[3].componentCount = 1;

	desc[4].identifier = kArrayPrimitive;
	desc[4].elementCount = triangleCount;
	desc[4].elementSize = sizeof(Triangle);
	desc[4].componentCount = 1;

	mesh.AllocateStorage(vertexCount, 5, desc);

	Point3D *restrict vertex = mesh.GetArray<Point3D>(kArrayPosition) - 1;
	Vector3D *restrict normal = mesh.GetArray<Vector3D>(kArrayNormal) - 1;
	Point2D *restrict texcoord = mesh.GetArray<Point2D>(kArrayTexcoord) - 1;
	unsigned_int16 *restrict surfaceIndex = mesh.GetArray<unsigned_int16>(kArraySurfaceIndex) - 1;
	Triangle *restrict triangle = mesh.GetArray<Triangle>(kArrayPrimitive);

	float rx = revolutionSize.x;
	float ry = revolutionSize.y;
	float sz = revolutionHeight / (pathBoundingBox.max.y - pathBoundingBox.min.y);

	float mx = 1.0F / rx;
	float my = 1.0F / ry;

	int32 componentIndex = 0;
	int32 baseVertexIndex = 0;

	float invLength = GetTexcoordScale() / totalLength;
	float tex1 = 0.0F;

	component = revolutionPath->GetFirstPathComponent();
	while (component)
	{
		const Point3D	*endPosition;

		const PathComponent *nextComponent = component->Next();
		if (nextComponent)
		{
			endPosition = &nextComponent->GetBeginPosition();
		}
		else
		{
			endPosition = &component->GetEndPosition();
		}

		Vector3D endTangent = Normalize(component->GetEndTangent());

		if (component->GetPathType() == kPathLinear)
		{
			const Point3D& beginPosition = component->GetBeginPosition();
			Vector3D beginTangent = Normalize(component->GetBeginTangent());

			float r1 = beginPosition.x;
			float r2 = endPosition->x;
			float z1 = beginPosition.y * sz;
			float z2 = endPosition->y * sz;

			float tex2 = tex1 + Magnitude(*endPosition - beginPosition) * invLength;

			for (machine i = 0; i < xdiv; i++)
			{
				float s = (float) i * dx;
				Vector2D v = CosSin(s * K::tau);
				float x = rx * v.x;
				float y = ry * v.y;

				float nx = v.x * mx;
				float ny = v.y * my;
				float m = InverseSqrt(nx * nx + ny * ny);
				nx *= m;
				ny *= m;

				float d2 = x * x + y * y;
				float invD = InverseSqrt(d2);
				float d = d2 * invD;
				float u1 = (d + r1) * invD;
				float u2 = (d + r2) * invD;

				(++vertex)->Set(x * u1, y * u1, z1);
				(++normal)->Set(nx * beginTangent.y, ny * beginTangent.y, -beginTangent.x).Normalize();
				(++texcoord)->Set(s, tex1);
				*++surfaceIndex = (unsigned_int16) componentIndex;

				(++vertex)->Set(x * u2, y * u2, z2);
				(++normal)->Set(nx * endTangent.y, ny * endTangent.y, -endTangent.x).Normalize();
				(++texcoord)->Set(s, tex2);
				*++surfaceIndex = (unsigned_int16) componentIndex;
			}

			const Point3D *vtx = mesh.GetArray<Point3D>(kArrayPosition) + baseVertexIndex;
			const Vector3D *nrm = mesh.GetArray<Vector3D>(kArrayNormal) + baseVertexIndex;

			*++vertex = vtx[0];
			*++normal = nrm[0];
			(++texcoord)->Set(1.0F, tex1);
			*++surfaceIndex = (unsigned_int16) componentIndex;

			*++vertex = vtx[1];
			*++normal = nrm[1];
			(++texcoord)->Set(1.0F, tex2);
			*++surfaceIndex = (unsigned_int16) componentIndex;

			for (machine i = 0; i < xdiv; i++)
			{
				int32 k = baseVertexIndex + i * 2;
				if ((i & 1) == 0)
				{
					triangle[0].Set(k, k + 2, k + 1);
					triangle[1].Set(k + 1, k + 2, k + 3);
				}
				else
				{
					triangle[0].Set(k, k + 2, k + 3);
					triangle[1].Set(k, k + 3, k + 1);
				}

				triangle += 2;
			}

			tex1 = tex2;
			baseVertexIndex += (xdiv + 1) * 2;
		}
		else
		{
			float	tex2;

			for (machine i = 0; i < xdiv; i++)
			{
				Point3D		p1;

				float s = (float) i * dx;
				Vector2D v = CosSin(s * K::tau);
				float x = rx * v.x;
				float y = ry * v.y;

				float nx = v.x * mx;
				float ny = v.y * my;
				float m = InverseSqrt(nx * nx + ny * ny);
				nx *= m;
				ny *= m;

				float d2 = x * x + y * y;
				float invD = InverseSqrt(d2);
				float d = d2 * invD;

				tex2 = tex1;
				for (machine j = 0; j < zdiv; j++)
				{
					float t = (float) j * dz;

					Point3D p2 = component->GetPosition(t);
					Vector3D tangent = component->GetTangent(t);

					if (j != 0)
					{
						tex2 += Magnitude(p2 - p1) * invLength;
					}

					p1 = p2;

					float u = (d + p2.x) * invD;

					(++vertex)->Set(x * u, y * u, p2.y * sz);
					(++normal)->Set(nx * tangent.y, ny * tangent.y, -tangent.x).Normalize();
					(++texcoord)->Set(s, tex2);
					*++surfaceIndex = (unsigned_int16) componentIndex;
				}

				tex2 += Magnitude(*endPosition - p1) * invLength;
				float u = (d + endPosition->x) * invD;

				(++vertex)->Set(x * u, y * u, endPosition->y * sz);
				(++normal)->Set(nx * endTangent.y, ny * endTangent.y, -endTangent.x).Normalize();
				(++texcoord)->Set(s, tex2);
				*++surfaceIndex = (unsigned_int16) componentIndex;
			}

			const Point3D *vtx = mesh.GetArray<Point3D>(kArrayPosition) + baseVertexIndex;
			const Vector3D *nrm = mesh.GetArray<Vector3D>(kArrayNormal) + baseVertexIndex;
			const Point2D *tex = mesh.GetArray<Point2D>(kArrayTexcoord) + baseVertexIndex;

			for (machine j = 0; j < zdiv; j++)
			{
				*++vertex = vtx[j];
				*++normal = nrm[j];
				(++texcoord)->Set(1.0F, tex[j].y);
				*++surfaceIndex = (unsigned_int16) componentIndex;
			}

			*++vertex = vtx[zdiv];
			*++normal = nrm[zdiv];
			(++texcoord)->Set(1.0F, tex[zdiv].y);
			*++surfaceIndex = (unsigned_int16) componentIndex;

			for (machine i = 0; i < xdiv; i++)
			{
				int32 k = baseVertexIndex + i * (zdiv + 1);
				for (machine j = 0; j < zdiv; j++)
				{
					if (((i + j) & 1) == 0)
					{
						triangle[0].Set(k + j, k + j + zdiv + 1, k + j + 1);
						triangle[1].Set(k + j + 1, k + j + zdiv + 1, k + j + zdiv + 2);
					}
					else
					{
						triangle[0].Set(k + j, k + j + zdiv + 1, k + j + zdiv + 2);
						triangle[1].Set(k + j, k + j + zdiv + 2, k + j + 1);
					}

					triangle += 2;
				}
			}

			tex1 = tex2;
			baseVertexIndex += (xdiv + 1) * (zdiv + 1);
		}

		componentIndex++;
		component = nextComponent;
	}

	if (endcap)
	{
		component = revolutionPath->GetFirstPathComponent();
		int32 delta = (component->GetPathType() == kPathLinear) ? 2 : zdiv + 1;

		const Point3D *vtx = mesh.GetArray<Point3D>(kArrayPosition);
		float dt = K::tau / (float) xdiv;
		float r = component->GetBeginPosition().x;
		float cx = rx + r;
		float cy = ry + r;
		float tex = 0.5F / r;

		for (machine i = xdiv; i > 0; i--)
		{
			machine k = OverflowZero(i, xdiv);
			Vector2D v = CosSin((float) k * dt);
			float x = cx * v.x;
			float y = cy * v.y;

			*++vertex = vtx[k * delta];
			(++normal)->Set(0.0F, 0.0F, -1.0F);
			(++texcoord)->Set(1.0F - (x + cx) * tex, (y * ratioXY + cx) * tex);
			*++surfaceIndex = (unsigned_int16) componentCount;
		}

		component = revolutionPath->GetLastPathComponent();
		delta = (component->GetPathType() == kPathLinear) ? 2 : zdiv + 1;

		vtx += baseVertexIndex - xdiv * delta - 1;

		r = component->GetEndPosition().x;
		cx = rx + r;
		cy = ry + r;
		tex = 0.5F / r;

		for (machine i = 0; i < xdiv; i++)
		{
			Vector2D v = CosSin((float) i * dt);
			float x = cx * v.x;
			float y = cy * v.y;

			*++vertex = vtx[i * delta];
			(++normal)->Set(0.0F, 0.0F, 1.0F);
			(++texcoord)->Set((x + cx) * tex, (y * ratioXY + cx) * tex);
			*++surfaceIndex = (unsigned_int16) (componentCount + 1);
		}

		TriangulateDisk(xdiv, baseVertexIndex, triangle);
		TriangulateDisk(xdiv, baseVertexIndex + xdiv, triangle + (xdiv - 2));
	}

	BuildStandardArrays(&mesh, GetGeometryLevel(level), transformable);

	if (level == 0)
	{
		primitiveBoundingBox.Calculate(vertexCount, mesh.GetArray<Point3D>(kArrayPosition));
	}
}


PrimitiveGeometry::PrimitiveGeometry(PrimitiveType type) : Geometry(kGeometryPrimitive)
{
	primitiveType = type;
}

PrimitiveGeometry::PrimitiveGeometry(const PrimitiveGeometry& primitive) : Geometry(primitive)
{
	primitiveType = primitive.primitiveType;
}

PrimitiveGeometry::~PrimitiveGeometry()
{
}

PrimitiveGeometry *PrimitiveGeometry::Create(Unpacker& data, unsigned_int32 unpackFlags)
{
	switch (data.GetType())
	{
		case kPrimitivePlate:

			return (new PlateGeometry);

		case kPrimitiveDisk:

			return (new DiskGeometry);

		case kPrimitiveHole:

			return (new HoleGeometry);

		case kPrimitiveAnnulus:

			return (new AnnulusGeometry);

		case kPrimitiveBox:

			return (new BoxGeometry);

		case kPrimitivePyramid:

			return (new PyramidGeometry);

		case kPrimitiveCylinder:

			return (new CylinderGeometry);

		case kPrimitiveCone:

			return (new ConeGeometry);

		case kPrimitiveTruncatedCone:

			return (new TruncatedConeGeometry);

		case kPrimitiveSphere:

			return (new SphereGeometry);

		case kPrimitiveDome:

			return (new DomeGeometry);

		case kPrimitiveTorus:

			return (new TorusGeometry);

		case kPrimitiveTube:

			return (new TubeGeometry);

		case kPrimitiveExtrusion:

			return (new ExtrusionGeometry);

		case kPrimitiveRevolution:

			return (new RevolutionGeometry);

		case kPrimitiveRope:

			return (new RopeGeometry);

		case kPrimitiveCloth:

			return (new ClothGeometry);
	}

	return (nullptr);
}

void PrimitiveGeometry::PackType(Packer& data) const
{
	Geometry::PackType(data);
	data << primitiveType;
}

bool PrimitiveGeometry::PathPrimitive(void) const
{
	return (false);
}


PlateGeometry::PlateGeometry() : PrimitiveGeometry(kPrimitivePlate)
{
}

PlateGeometry::PlateGeometry(const Vector2D& size) : PrimitiveGeometry(kPrimitivePlate)
{
	SetNewObject(new PlateGeometryObject(size));
}

PlateGeometry::PlateGeometry(const PlateGeometry& plateGeometry) : PrimitiveGeometry(plateGeometry)
{
}

PlateGeometry::~PlateGeometry()
{
}

Node *PlateGeometry::Replicate(void) const
{
	return (new PlateGeometry(*this));
}

bool PlateGeometry::CalculateBoundingBox(Box3D *box) const
{
	box->min.Set(0.0F, 0.0F, 0.0F);
	box->max.Set(GetObject()->GetPlateSize(), 0.0F);
	return (true);
}

bool PlateGeometry::CalculateBoundingSphere(BoundingSphere *sphere) const
{
	const Vector2D& plateSize = GetObject()->GetPlateSize();

	float x = plateSize.x * 0.5F;
	float y = plateSize.y * 0.5F;

	sphere->SetCenter(x, y, 0.0F);
	sphere->SetRadius(Sqrt(x * x + y * y));
	return (true);
}


DiskGeometry::DiskGeometry() : PrimitiveGeometry(kPrimitiveDisk)
{
}

DiskGeometry::DiskGeometry(const Vector2D& size) : PrimitiveGeometry(kPrimitiveDisk)
{
	SetNewObject(new DiskGeometryObject(size));
}

DiskGeometry::DiskGeometry(const DiskGeometry& diskGeometry) : PrimitiveGeometry(diskGeometry)
{
}

DiskGeometry::~DiskGeometry()
{
}

Node *DiskGeometry::Replicate(void) const
{
	return (new DiskGeometry(*this));
}

bool DiskGeometry::CalculateBoundingBox(Box3D *box) const
{
	const Vector2D& diskSize = GetObject()->GetDiskSize();
	box->min.Set(-diskSize, 0.0F);
	box->max.Set(diskSize, 0.0F);
	return (true);
}

bool DiskGeometry::CalculateBoundingSphere(BoundingSphere *sphere) const
{
	const Vector2D& diskSize = GetObject()->GetDiskSize();
	sphere->SetCenter(0.0F, 0.0F, 0.0F);
	sphere->SetRadius(Fmax(diskSize.x, diskSize.y));
	return (true);
}


HoleGeometry::HoleGeometry() : PrimitiveGeometry(kPrimitiveHole)
{
}

HoleGeometry::HoleGeometry(const Vector2D& inner, const Vector2D& outer) : PrimitiveGeometry(kPrimitiveHole)
{
	SetNewObject(new HoleGeometryObject(inner, outer));
}

HoleGeometry::HoleGeometry(const HoleGeometry& holeGeometry) : PrimitiveGeometry(holeGeometry)
{
}

HoleGeometry::~HoleGeometry()
{
}

Node *HoleGeometry::Replicate(void) const
{
	return (new HoleGeometry(*this));
}

bool HoleGeometry::CalculateBoundingBox(Box3D *box) const
{
	const Vector2D& outerSize = GetObject()->GetOuterSize();
	box->min.Set(-outerSize, 0.0F);
	box->max.Set(outerSize, 0.0F);
	return (true);
}

bool HoleGeometry::CalculateBoundingSphere(BoundingSphere *sphere) const
{
	sphere->SetCenter(0.0F, 0.0F, 0.0F);
	sphere->SetRadius(Magnitude(GetObject()->GetOuterSize()));
	return (true);
}


AnnulusGeometry::AnnulusGeometry() : PrimitiveGeometry(kPrimitiveAnnulus)
{
}

AnnulusGeometry::AnnulusGeometry(const Vector2D& inner, const Vector2D& outer) : PrimitiveGeometry(kPrimitiveAnnulus)
{
	SetNewObject(new AnnulusGeometryObject(inner, outer));
}

AnnulusGeometry::AnnulusGeometry(const AnnulusGeometry& annulusGeometry) : PrimitiveGeometry(annulusGeometry)
{
}

AnnulusGeometry::~AnnulusGeometry()
{
}

Node *AnnulusGeometry::Replicate(void) const
{
	return (new AnnulusGeometry(*this));
}

bool AnnulusGeometry::CalculateBoundingBox(Box3D *box) const
{
	const Vector2D& outerSize = GetObject()->GetOuterSize();
	box->min.Set(-outerSize, 0.0F);
	box->max.Set(outerSize, 0.0F);
	return (true);
}

bool AnnulusGeometry::CalculateBoundingSphere(BoundingSphere *sphere) const
{
	const Vector2D& outerSize = GetObject()->GetOuterSize();

	sphere->SetCenter(0.0F, 0.0F, 0.0F);
	sphere->SetRadius(Fmax(outerSize.x, outerSize.y));
	return (true);
}


BoxGeometry::BoxGeometry() : PrimitiveGeometry(kPrimitiveBox)
{
}

BoxGeometry::BoxGeometry(const Vector3D& size) : PrimitiveGeometry(kPrimitiveBox)
{
	SetNewObject(new BoxGeometryObject(size));
}

BoxGeometry::BoxGeometry(const BoxGeometry& boxGeometry) : PrimitiveGeometry(boxGeometry)
{
}

BoxGeometry::~BoxGeometry()
{
}

Node *BoxGeometry::Replicate(void) const
{
	return (new BoxGeometry(*this));
}

bool BoxGeometry::CalculateBoundingBox(Box3D *box) const
{
	box->min.Set(0.0F, 0.0F, 0.0F);
	box->max = GetObject()->GetBoxSize();
	return (true);
}

bool BoxGeometry::CalculateBoundingSphere(BoundingSphere *sphere) const
{
	const Vector3D& boxSize = GetObject()->GetBoxSize();

	float sx = boxSize.x * 0.5F;
	float sy = boxSize.y * 0.5F;
	float sz = boxSize.z * 0.5F;

	sphere->SetCenter(sx, sy, sz);
	sphere->SetRadius(Sqrt(sx * sx + sy * sy + sz * sz));
	return (true);
}


PyramidGeometry::PyramidGeometry() : PrimitiveGeometry(kPrimitivePyramid)
{
}

PyramidGeometry::PyramidGeometry(const Vector2D& size, float height) : PrimitiveGeometry(kPrimitivePyramid)
{
	SetNewObject(new PyramidGeometryObject(size, height));
}

PyramidGeometry::PyramidGeometry(const PyramidGeometry& pyramidGeometry) : PrimitiveGeometry(pyramidGeometry)
{
}

PyramidGeometry::~PyramidGeometry()
{
}

Node *PyramidGeometry::Replicate(void) const
{
	return (new PyramidGeometry(*this));
}

bool PyramidGeometry::CalculateBoundingBox(Box3D *box) const
{
	const PyramidGeometryObject *object = GetObject();
	box->min.Set(0.0F, 0.0F, 0.0F);
	box->max.Set(object->GetPyramidSize(), object->GetPyramidHeight());
	return (true);
}

bool PyramidGeometry::CalculateBoundingSphere(BoundingSphere *sphere) const
{
	const PyramidGeometryObject *object = GetObject();
	const Vector2D& pyramidSize = object->GetPyramidSize();
	float rx = pyramidSize.x * 0.5F;
	float ry = pyramidSize.y * 0.5F;
	float r = Sqrt(rx * rx + ry * ry);
	float h = object->GetPyramidHeight();

	if (h > r)
	{
		float d = (r * r + h * h) / (h * 2.0F);
		sphere->SetCenter(rx, ry, h - d);
		sphere->SetRadius(d);
	}
	else
	{
		sphere->SetCenter(rx, ry, 0.0F);
		sphere->SetRadius(r);
	}

	return (true);
}


CylinderGeometry::CylinderGeometry() : PrimitiveGeometry(kPrimitiveCylinder)
{
}

CylinderGeometry::CylinderGeometry(const Vector2D& size, float height) : PrimitiveGeometry(kPrimitiveCylinder)
{
	SetNewObject(new CylinderGeometryObject(size, height));
}

CylinderGeometry::CylinderGeometry(const CylinderGeometry& cylinderGeometry) : PrimitiveGeometry(cylinderGeometry)
{
}

CylinderGeometry::~CylinderGeometry()
{
}

Node *CylinderGeometry::Replicate(void) const
{
	return (new CylinderGeometry(*this));
}

bool CylinderGeometry::CalculateBoundingBox(Box3D *box) const
{
	const CylinderGeometryObject *object = GetObject();
	const Vector2D& cylinderSize = object->GetCylinderSize();

	box->min.Set(-cylinderSize, 0.0F);
	box->max.Set(cylinderSize, object->GetCylinderHeight());
	return (true);
}

bool CylinderGeometry::CalculateBoundingSphere(BoundingSphere *sphere) const
{
	const CylinderGeometryObject *object = GetObject();
	const Vector2D& cylinderSize = object->GetCylinderSize();

	float r = Fmax(cylinderSize.x, cylinderSize.y);
	float h = object->GetCylinderHeight() * 0.5F;

	sphere->SetCenter(0.0F, 0.0F, h);
	sphere->SetRadius(Sqrt(h * h + r * r));
	return (true);
}


ConeGeometry::ConeGeometry() : PrimitiveGeometry(kPrimitiveCone)
{
}

ConeGeometry::ConeGeometry(const Vector2D& size, float height) : PrimitiveGeometry(kPrimitiveCone)
{
	SetNewObject(new ConeGeometryObject(size, height));
}

ConeGeometry::ConeGeometry(const ConeGeometry& coneGeometry) : PrimitiveGeometry(coneGeometry)
{
}

ConeGeometry::~ConeGeometry()
{
}

Node *ConeGeometry::Replicate(void) const
{
	return (new ConeGeometry(*this));
}

bool ConeGeometry::CalculateBoundingBox(Box3D *box) const
{
	const ConeGeometryObject *object = GetObject();
	const Vector2D& coneSize = object->GetConeSize();

	box->min.Set(-coneSize, 0.0F);
	box->max.Set(coneSize, object->GetConeHeight());
	return (true);
}

bool ConeGeometry::CalculateBoundingSphere(BoundingSphere *sphere) const
{
	const ConeGeometryObject *object = GetObject();
	const Vector2D& coneSize = object->GetConeSize();

	float r = Fmax(coneSize.x, coneSize.y);
	float h = object->GetConeHeight();

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

	return (true);
}


TruncatedConeGeometry::TruncatedConeGeometry() : PrimitiveGeometry(kPrimitiveTruncatedCone)
{
}

TruncatedConeGeometry::TruncatedConeGeometry(const Vector2D& size, float height, float ratio) : PrimitiveGeometry(kPrimitiveTruncatedCone)
{
	SetNewObject(new TruncatedConeGeometryObject(size, height, ratio));
}

TruncatedConeGeometry::TruncatedConeGeometry(const TruncatedConeGeometry& truncatedConeGeometry) : PrimitiveGeometry(truncatedConeGeometry)
{
}

TruncatedConeGeometry::~TruncatedConeGeometry()
{
}

Node *TruncatedConeGeometry::Replicate(void) const
{
	return (new TruncatedConeGeometry(*this));
}

bool TruncatedConeGeometry::CalculateBoundingBox(Box3D *box) const
{
	const TruncatedConeGeometryObject *object = GetObject();
	const Vector2D& coneSize = object->GetConeSize();

	box->min.Set(-coneSize, 0.0F);
	box->max.Set(coneSize, object->GetConeHeight());
	return (true);
}

bool TruncatedConeGeometry::CalculateBoundingSphere(BoundingSphere *sphere) const
{
	const TruncatedConeGeometryObject *object = GetObject();
	const Vector2D& coneSize = object->GetConeSize();

	float s = Fmax(coneSize.x, coneSize.y);
	float h = object->GetConeHeight();
	float r = object->GetConeRatio();

	float z = FmaxZero((s * s * (r * r - 1.0F) + h * h) / (h * 2.0F));

	sphere->SetCenter(0.0F, 0.0F, z);
	sphere->SetRadius(Sqrt(s * s + z * z));
	return (true);
}


SphereGeometry::SphereGeometry() : PrimitiveGeometry(kPrimitiveSphere)
{
}

SphereGeometry::SphereGeometry(const Vector3D& size) : PrimitiveGeometry(kPrimitiveSphere)
{
	SetNewObject(new SphereGeometryObject(size));
}

SphereGeometry::SphereGeometry(const SphereGeometry& sphereGeometry) : PrimitiveGeometry(sphereGeometry)
{
}

SphereGeometry::~SphereGeometry()
{
}

Node *SphereGeometry::Replicate(void) const
{
	return (new SphereGeometry(*this));
}

bool SphereGeometry::CalculateBoundingBox(Box3D *box) const
{
	const Vector3D& sphereSize = GetObject()->GetSphereSize();
	box->min = -sphereSize;
	box->max = sphereSize;
	return (true);
}

bool SphereGeometry::CalculateBoundingSphere(BoundingSphere *sphere) const
{
	const Vector3D& sphereSize = GetObject()->GetSphereSize();
	sphere->SetCenter(0.0F, 0.0F, 0.0F);
	sphere->SetRadius(Fmax(sphereSize.x, sphereSize.y, sphereSize.z));
	return (true);
}


DomeGeometry::DomeGeometry() : PrimitiveGeometry(kPrimitiveDome)
{
}

DomeGeometry::DomeGeometry(const Vector3D& size) : PrimitiveGeometry(kPrimitiveDome)
{
	SetNewObject(new DomeGeometryObject(size));
}

DomeGeometry::DomeGeometry(const DomeGeometry& domeGeometry) : PrimitiveGeometry(domeGeometry)
{
}

DomeGeometry::~DomeGeometry()
{
}

Node *DomeGeometry::Replicate(void) const
{
	return (new DomeGeometry(*this));
}

bool DomeGeometry::CalculateBoundingBox(Box3D *box) const
{
	const Vector3D& domeSize = GetObject()->GetDomeSize();
	box->min.Set(-domeSize.x, -domeSize.y, 0.0F);
	box->max = domeSize;
	return (true);
}

bool DomeGeometry::CalculateBoundingSphere(BoundingSphere *sphere) const
{
	const Vector3D& domeSize = GetObject()->GetDomeSize();
	sphere->SetCenter(0.0F, 0.0F, 0.0F);
	sphere->SetRadius(Fmax(domeSize.x, domeSize.y, domeSize.z));
	return (true);
}


TorusGeometry::TorusGeometry() : PrimitiveGeometry(kPrimitiveTorus)
{
}

TorusGeometry::TorusGeometry(const Vector2D& primary, float secondary) : PrimitiveGeometry(kPrimitiveTorus)
{
	SetNewObject(new TorusGeometryObject(primary, secondary));
}

TorusGeometry::TorusGeometry(const TorusGeometry& torusGeometry) : PrimitiveGeometry(torusGeometry)
{
}

TorusGeometry::~TorusGeometry()
{
}

Node *TorusGeometry::Replicate(void) const
{
	return (new TorusGeometry(*this));
}

bool TorusGeometry::CalculateBoundingBox(Box3D *box) const
{
	const TorusGeometryObject *object = GetObject();
	const Vector2D& primarySize = object->GetPrimarySize();
	float r = object->GetSecondaryRadius();

	box->min.Set(-primarySize.x - r, -primarySize.y - r, -r);
	box->max.Set(primarySize.x + r, primarySize.y + r, r);
	return (true);
}

bool TorusGeometry::CalculateBoundingSphere(BoundingSphere *sphere) const
{
	const TorusGeometryObject *object = GetObject();
	const Vector2D& primarySize = object->GetPrimarySize();

	sphere->SetCenter(0.0F, 0.0F, 0.0F);
	sphere->SetRadius(Fmax(primarySize.x, primarySize.y) + object->GetSecondaryRadius());
	return (true);
}


PathPrimitiveGeometry::PathPrimitiveGeometry(PrimitiveType type) : PrimitiveGeometry(type)
{
}

PathPrimitiveGeometry::PathPrimitiveGeometry(const PathPrimitiveGeometry& pathPrimitiveGeometry) : PrimitiveGeometry(pathPrimitiveGeometry)
{
}

PathPrimitiveGeometry::~PathPrimitiveGeometry()
{
}

void PathPrimitiveGeometry::Pack(Packer& data, unsigned_int32 packFlags) const
{
	PrimitiveGeometry::Pack(data, packFlags);

	data << TerminatorChunk;
}

void PathPrimitiveGeometry::Unpack(Unpacker& data, unsigned_int32 unpackFlags)
{
	PrimitiveGeometry::Unpack(data, unpackFlags);
	UnpackChunkList<PathPrimitiveGeometry>(data, unpackFlags);
}

bool PathPrimitiveGeometry::UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags)
{
	return (false);
}

bool PathPrimitiveGeometry::CalculateBoundingBox(Box3D *box) const
{
	*box = GetObject()->GetPrimitiveBoundingBox();
	return (true);
}

bool PathPrimitiveGeometry::PathPrimitive(void) const
{
	return (true);
}

int32 PathPrimitiveGeometry::GetInternalConnectorCount(void) const
{
	return (PrimitiveGeometry::GetInternalConnectorCount() + 1);
}

const char *PathPrimitiveGeometry::GetInternalConnectorKey(int32 index) const
{
	int32 count = PrimitiveGeometry::GetInternalConnectorCount();
	if (index < count)
	{
		return (PrimitiveGeometry::GetInternalConnectorKey(index));
	}

	if (index == count)
	{
		return (kConnectorKeyPath);
	}

	return (nullptr);
}

bool PathPrimitiveGeometry::ValidConnectedNode(const ConnectorKey& key, const Node *node) const
{
	if (key == kConnectorKeyPath)
	{
		if (node->GetNodeType() == kNodeMarker)
		{
			return (static_cast<const Marker *>(node)->GetMarkerType() == kMarkerPath);
		}

		return (false);
	}

	return (PrimitiveGeometry::ValidConnectedNode(key, node));
}

PathMarker *PathPrimitiveGeometry::GetConnectedPathMarker(void) const
{
	Node *node = GetConnectedNode(kConnectorKeyPath);
	if (node)
	{
		return (static_cast<PathMarker *>(node));
	}

	return (nullptr);
}

void PathPrimitiveGeometry::SetConnectedPathMarker(PathMarker *marker)
{
	SetConnectedNode(kConnectorKeyPath, marker);
}


TubeGeometry::TubeGeometry() : PathPrimitiveGeometry(kPrimitiveTube)
{
}

TubeGeometry::TubeGeometry(const Path *path, const Vector2D& size) : PathPrimitiveGeometry(kPrimitiveTube)
{
	SetNewObject(new TubeGeometryObject(path, size));
}

TubeGeometry::TubeGeometry(const TubeGeometry& tubeGeometry) : PathPrimitiveGeometry(tubeGeometry)
{
}

TubeGeometry::~TubeGeometry()
{
}

Node *TubeGeometry::Replicate(void) const
{
	return (new TubeGeometry(*this));
}

bool TubeGeometry::CalculateBoundingSphere(BoundingSphere *sphere) const
{
	const TubeGeometryObject *object = GetObject();
	const Box3D& bounds = object->GetPathBoundingBox();
	const Vector2D& size = object->GetTubeSize();

	sphere->SetCenter((bounds.min + bounds.max) * 0.5F);
	sphere->SetRadius(Magnitude(bounds.max - bounds.min) * 0.5F + Fmax(size.x, size.y));
	return (true);
}


ExtrusionGeometry::ExtrusionGeometry() : PathPrimitiveGeometry(kPrimitiveExtrusion)
{
}

ExtrusionGeometry::ExtrusionGeometry(const Path *path, const Vector2D& size, float height) : PathPrimitiveGeometry(kPrimitiveExtrusion)
{
	SetNewObject(new ExtrusionGeometryObject(path, size, height));
}

ExtrusionGeometry::ExtrusionGeometry(const ExtrusionGeometry& extrusionGeometry) : PathPrimitiveGeometry(extrusionGeometry)
{
}

ExtrusionGeometry::~ExtrusionGeometry()
{
}

Node *ExtrusionGeometry::Replicate(void) const
{
	return (new ExtrusionGeometry(*this));
}

bool ExtrusionGeometry::CalculateBoundingSphere(BoundingSphere *sphere) const
{
	const ExtrusionGeometryObject *object = GetObject();
	const Vector2D& size = object->GetExtrusionSize();
	float sx = size.x * 0.5F;
	float sy = size.y * 0.5F;
	float sz = object->GetExtrusionHeight() * 0.5F;

	sphere->SetCenter(sx, sy, sz);
	sphere->SetRadius(Sqrt(sx * sx + sy * sy + sz * sz));
	return (true);
}


RevolutionGeometry::RevolutionGeometry() : PathPrimitiveGeometry(kPrimitiveRevolution)
{
}

RevolutionGeometry::RevolutionGeometry(const Path *path, const Vector2D& size, float height) : PathPrimitiveGeometry(kPrimitiveRevolution)
{
	SetNewObject(new RevolutionGeometryObject(path, size, height));
}

RevolutionGeometry::RevolutionGeometry(const RevolutionGeometry& revolutionGeometry) : PathPrimitiveGeometry(revolutionGeometry)
{
}

RevolutionGeometry::~RevolutionGeometry()
{
}

Node *RevolutionGeometry::Replicate(void) const
{
	return (new RevolutionGeometry(*this));
}

bool RevolutionGeometry::CalculateBoundingSphere(BoundingSphere *sphere) const
{
	const RevolutionGeometryObject *object = GetObject();
	const Box3D& bounds = object->GetPathBoundingBox();
	const Vector2D& size = object->GetRevolutionSize();

	float r = Fmax(size.x, size.y) + bounds.max.x;
	float h = object->GetRevolutionHeight() * 0.5F;

	sphere->SetCenter(0.0F, 0.0F, h);
	sphere->SetRadius(Sqrt(h * h + r * r));
	return (true);
}

// ZYUQURM
