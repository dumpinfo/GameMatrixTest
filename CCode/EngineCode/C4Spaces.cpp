 

#include "C4Spaces.h"
#include "C4World.h"
#include "C4Cameras.h"
#include "C4Configuration.h"


using namespace C4;


namespace
{
	const C4::Line shadowSpaceLine[12] =
	{
		{{0, 1}},
		{{1, 2}},
		{{2, 3}},
		{{3, 0}},
		{{4, 5}},
		{{5, 6}},
		{{6, 7}},
		{{7, 4}},
		{{0, 4}},
		{{1, 5}},
		{{2, 6}},
		{{3, 7}}
	};


	const unsigned_int8 occlusionPolygonIndex[43] =
	{
		0x00, 0x80, 0x81, 0x00, 0x82, 0xC9, 0xC8, 0x00, 0x83, 0xC7, 0xC6, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x84, 0xCF, 0xCE, 0x00, 0xD1, 0xD9, 0xD8, 0x00, 0xD0, 0xD7, 0xD6, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x85, 0xCB, 0xCA, 0x00, 0xCD, 0xD5, 0xD4, 0x00, 0xCC, 0xD3, 0xD2
	};

	const unsigned_int8 occlusionVertexIndex[26][6] =
	{
		{1, 3, 7, 5},
		{2, 0, 4, 6},
		{3, 2, 6, 7},
		{0, 1, 5, 4},
		{4, 5, 7, 6},
		{1, 0, 2, 3},
		{2, 0, 1, 5, 4, 6},
		{0, 1, 3, 7, 5, 4},
		{3, 2, 0, 4, 6, 7},
		{1, 3, 2, 6, 7, 5},
		{1, 0, 4, 6, 2, 3},
		{5, 1, 0, 2, 3, 7},
		{4, 0, 2, 3, 1, 5},
		{0, 2, 6, 7, 3, 1},
		{0, 4, 5, 7, 6, 2},
		{4, 5, 1, 3, 7, 6},
		{1, 5, 7, 6, 4, 0},
		{5, 7, 3, 2, 6, 4},
		{3, 1, 5, 4, 6, 2},
		{2, 3, 7, 5, 4, 0},
		{1, 0, 4, 6, 7, 3},
		{0, 2, 6, 7, 5, 1},
		{7, 6, 2, 0, 1, 5},
		{6, 4, 0, 1, 3, 7},
		{5, 7, 3, 2, 0, 4},
		{4, 5, 1, 3, 2, 6}
	};

	const ConstPoint3D occlusionVertexPosition[8] =
	{
		{0.0F, 0.0F, 0.0F}, {1.0F, 0.0F, 0.0F}, {0.0F, 1.0F, 0.0F}, {1.0F, 1.0F, 0.0F},
		{0.0F, 0.0F, 1.0F}, {1.0F, 0.0F, 1.0F}, {0.0F, 1.0F, 1.0F}, {1.0F, 1.0F, 1.0F}
	};
}


Space::Space(SpaceType type) : Node(kNodeSpace)
{
	spaceType = type;
}

Space::Space(const Space& space) : Node(space)
{
	spaceType = space.spaceType;
}

Space::~Space()
{
}

Space *Space::Create(Unpacker& data, unsigned_int32 unpackFlags)
{
	switch (data.GetType())
	{
		case kSpaceFog:

			return (new FogSpace);

		case kSpaceShadow:

			return (new ShadowSpace);

		case kSpaceRadiosity:
 
			return (new RadiositySpace);

		case kSpaceAcoustics: 

			return (new AcousticsSpace); 

		case kSpaceOcclusion:
 
			return (new OcclusionSpace);
 
		case kSpacePaint: 

			return (new PaintSpace);

		case kSpacePhysics: 

			return (new PhysicsSpace);
	}

	return (nullptr);
}

void Space::PackType(Packer& data) const
{
	Node::PackType(data);
	data << spaceType;
}

bool Space::CalculateBoundingBox(Box3D *box) const
{
	GetObject()->CalculateBoundingBox(box);
	return (true);
}

bool Space::CalculateBoundingSphere(BoundingSphere *sphere) const
{
	GetObject()->CalculateBoundingSphere(sphere);
	return (true);
}


FogSpace::FogSpace() : Space(kSpaceFog)
{
}

FogSpace::FogSpace(const Vector2D& size) : Space(kSpaceFog)
{
	SetNewObject(new FogSpaceObject(size));
	SetPerspectiveExclusionMask(kPerspectiveRadiositySpace);
}

FogSpace::FogSpace(const FogSpace& fogSpace) : Space(fogSpace)
{
}

FogSpace::~FogSpace()
{
}

Node *FogSpace::Replicate(void) const
{
	return (new FogSpace(*this));
}

void FogSpace::Pack(Packer& data, unsigned_int32 packFlags) const
{
	Space::Pack(data, packFlags);

	data << TerminatorChunk;
}

void FogSpace::Unpack(Unpacker& data, unsigned_int32 unpackFlags)
{
	Space::Unpack(data, unpackFlags);
	UnpackChunkList<FogSpace>(data, unpackFlags);
}

bool FogSpace::UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags)
{
	return (false);
}

int32 FogSpace::GetCategorySettingCount(Type category) const
{
	int32 count = Space::GetCategorySettingCount(category);
	if (category == 'NODE')
	{
		count += kPerspectiveMaskSettingCount;
	}

	return (count);
}

Setting *FogSpace::GetCategorySetting(Type category, int32 index, unsigned_int32 flags) const
{
	if (category == 'NODE')
	{
		int32 i = index - Space::GetCategorySettingCount('NODE');
		if (i >= 0)
		{
			return (GetPerspectiveMaskSetting(i));
		}
	}

	return (Space::GetCategorySetting(category, index, flags));
}

void FogSpace::SetCategorySetting(Type category, const Setting *setting)
{
	if ((category != 'NODE') || (!SetPerspectiveMaskSetting(setting)))
	{
		Space::SetCategorySetting(category, setting);
	}
}

bool FogSpace::FogVisible(const Node *node, const VisibilityRegion *region)
{
	const FogSpace *fog = static_cast<const FogSpace *>(node);
	return (region->QuadVisible(fog->worldVertex));
}

bool FogSpace::FogVisible(const Node *node, const VisibilityRegion *region, const List<OcclusionRegion> *occlusionList)
{
	const FogSpace *fog = static_cast<const FogSpace *>(node);
	const Point3D *vertex = fog->worldVertex;

	if (region->QuadVisible(vertex))
	{
		const OcclusionRegion *occluder = occlusionList->First();
		while (occluder)
		{
			if ((occluder->GetOcclusionMask() & kOcclusionFog) && (occluder->QuadOccluded(vertex)))
			{
				return (false);
			}

			occluder = occluder->Next();
		}

		return (true);
	}

	return (false);
}

void FogSpace::HandleTransformUpdate(void)
{
	Space::HandleTransformUpdate();

	const Transform4D& transform = GetWorldTransform();

	const Vector2D& plateSize = GetObject()->GetPlateSize();
	Vector3D dx = transform[0] * plateSize.x;
	Vector3D dy = transform[1] * plateSize.y;

	worldVertex[0] = transform.GetTranslation();
	worldVertex[1] = worldVertex[0] + dx;
	worldVertex[2] = worldVertex[1] + dy;
	worldVertex[3] = worldVertex[0] + dy;
}

void FogSpace::Preprocess(void)
{
	Space::Preprocess();

	Zone *zone = GetOwningZone();
	if (zone)
	{
		zone->AddFogSpace(this);
	}

	SetVisibilityProc(&FogVisible);
	SetOcclusionProc(&FogVisible);
}

void FogSpace::Neutralize(void)
{
	ListElement<FogSpace>::Detach();
	Space::Neutralize();
}


ShadowSpace::ShadowSpace() : Space(kSpaceShadow)
{
}

ShadowSpace::ShadowSpace(const Vector3D& size) : Space(kSpaceShadow)
{
	SetNewObject(new ShadowSpaceObject(size));
}

ShadowSpace::ShadowSpace(const ShadowSpace& shadowSpace) : Space(shadowSpace)
{
}

ShadowSpace::~ShadowSpace()
{
}

Node *ShadowSpace::Replicate(void) const
{
	return (new ShadowSpace(*this));
}

bool ShadowSpace::ClipSegmentToPlanes(Point3D& p1, Point3D& p2, int32 planeCount, const Antivector4D *plane)
{
	for (machine a = 0; a < planeCount; a++)
	{
		float d1 = plane[a] ^ p1;
		float d2 = plane[a] ^ p2;

		if (d1 < 0.0F)
		{
			if (d2 < 0.0F)
			{
				return (false);
			}

			Vector3D dp = p2 - p1;
			p1 -= dp * (d1 / (d2 - d1));
		}
		else if (d2 < 0.0F)
		{
			Vector3D dp = p1 - p2;
			p2 -= dp * (d2 / (d1 - d2));
		}
	}

	return (true);
}

int32 ShadowSpace::ClipToShadowBounds(int32 planeCount, const Antivector4D *plane, Point3D *vertex, Line *line, int32 baseIndex) const
{
	Point3D		p[8];

	const Point3D& p0 = GetWorldPosition();
	const Vector3D& size = GetObject()->GetBoxSize();
	Vector3D dx = GetWorldTransform()[0] * size.x;
	Vector3D dy = GetWorldTransform()[1] * size.y;
	Vector3D dz = GetWorldTransform()[2] * size.z;

	p[0] = p0;
	p[1] = p0 + dx;
	p[2] = p[1] + dy;
	p[3] = p0 + dy;
	p[4] = p0 + dz;
	p[5] = p[4] + dx;
	p[6] = p[5] + dy;
	p[7] = p[4] + dy;

	int32 lineCount = 0;
	const Line *spaceLine = shadowSpaceLine;
	for (machine a = 0; a < 12; a++)
	{
		Point3D p1 = p[spaceLine->index[0]];
		Point3D p2 = p[spaceLine->index[1]];
		spaceLine++;

		if (ClipSegmentToPlanes(p1, p2, planeCount, plane))
		{
			vertex[baseIndex] = p1;
			vertex[baseIndex + 1] = p2;
			line->Set(baseIndex, baseIndex + 1);

			line++;
			baseIndex += 2;
			lineCount++;
		}
	}

	return (lineCount);
}


RadiositySpace::RadiositySpace() : Space(kSpaceRadiosity)
{
}

RadiositySpace::RadiositySpace(const Vector3D& size, const Integer3D& resolution) : Space(kSpaceRadiosity)
{
	SetNewObject(new RadiositySpaceObject(size, resolution));
}

RadiositySpace::RadiositySpace(const RadiositySpace& radiositySpace) : Space(radiositySpace)
{
}

RadiositySpace::~RadiositySpace()
{
}

Node *RadiositySpace::Replicate(void) const
{
	return (new RadiositySpace(*this));
}


AcousticsSpace::AcousticsSpace() : Space(kSpaceAcoustics)
{
	soundRoom = nullptr;
}

AcousticsSpace::AcousticsSpace(const Vector3D& size) : Space(kSpaceAcoustics)
{
	SetNewObject(new AcousticsSpaceObject(size));

	soundRoom = nullptr;
}

AcousticsSpace::AcousticsSpace(const AcousticsSpace& acousticsSpace) : Space(acousticsSpace)
{
	soundRoom = nullptr;
}

AcousticsSpace::~AcousticsSpace()
{
	if (soundRoom)
	{
		soundRoom->Release();
	}
}

Node *AcousticsSpace::Replicate(void) const
{
	return (new AcousticsSpace(*this));
}

void AcousticsSpace::Preprocess(void)
{
	Space::Preprocess();

	if (!GetManipulator())
	{
		const AcousticsSpaceObject *object = GetObject();

		if (!soundRoom)
		{
			soundRoom = new SoundRoom(object->GetBoxSize());
		}
		else
		{
			soundRoom->SetRoomSize(object->GetBoxSize());
		}

		soundRoom->SetReflectionVolume(object->GetReflectionVolume());
		soundRoom->SetReflectionHFVolume(object->GetReflectionHFVolume());
		soundRoom->SetReverbDecayTime(object->GetReverbDecayTime());
		soundRoom->SetMediumHFAbsorption(object->GetMediumHFAbsorption());
	}
}

void AcousticsSpace::Neutralize(void)
{
	if (soundRoom)
	{
		soundRoom->Release();
		soundRoom = nullptr;
	}

	Space::Neutralize();
}


OcclusionSpace::OcclusionSpace() : Space(kSpaceOcclusion)
{
}

OcclusionSpace::OcclusionSpace(const Vector3D& size) : Space(kSpaceOcclusion)
{
	SetNewObject(new OcclusionSpaceObject(size));
}

OcclusionSpace::OcclusionSpace(const OcclusionSpace& occlusionSpace) : Space(occlusionSpace)
{
}

OcclusionSpace::~OcclusionSpace()
{
}

Node *OcclusionSpace::Replicate(void) const
{
	return (new OcclusionSpace(*this));
}

void OcclusionSpace::HandleTransformUpdate(void)
{
	Space::HandleTransformUpdate();

	const OcclusionSpaceObject *object = GetObject();
	const Vector3D& boxSize = object->GetBoxSize() * 0.5F;

	const Transform4D& transform = GetWorldTransform();
	worldCenter = transform.GetTranslation() + transform * boxSize;
	worldAxis[0] = transform[0] * boxSize.x;
	worldAxis[1] = transform[1] * boxSize.y;
	worldAxis[2] = transform[2] * boxSize.z;
}

bool OcclusionSpace::BoxVisible(const Node *node, const VisibilityRegion *region)
{
	const OcclusionSpace *space = static_cast<const OcclusionSpace *>(node);
	return (region->BoxVisible(space->worldCenter, space->worldAxis));
}

bool OcclusionSpace::BoxVisible(const Node *node, const VisibilityRegion *region, const List<OcclusionRegion> *occlusionList)
{
	const OcclusionSpace *space = static_cast<const OcclusionSpace *>(node);
	const Point3D& center = space->worldCenter;
	const Vector3D *axis = space->worldAxis;

	if (region->BoxVisible(center, axis))
	{
		const OcclusionRegion *occluder = occlusionList->First();
		while (occluder)
		{
			if (occluder->BoxOccluded(center, axis))
			{
				return (false);
			}

			occluder = occluder->Next();
		}

		return (true);
	}

	return (false);
}

void OcclusionSpace::Preprocess(void)
{
	Space::Preprocess();

	SetVisibilityProc(&BoxVisible);
	SetOcclusionProc(&BoxVisible);

	Zone *zone = GetOwningZone();
	if (zone)
	{
		zone->AddOcclusionSpace(this);
	}
}

void OcclusionSpace::Neutralize(void)
{
	ListElement<OcclusionSpace>::Detach();
	Space::Neutralize();
}

OcclusionRegion *OcclusionSpace::NewFrustumOcclusionRegion(const FrustumCamera *camera) const
{
	Point3D			tempVertex[2][10];
	Antivector4D	facePlane[6];

	const Transform4D& inverseTransform = GetInverseWorldTransform();
	Point3D viewPosition = inverseTransform * camera->GetWorldPosition();
	Vector3D viewDirection = inverseTransform * camera->GetWorldTransform()[2];
	float sineHalfField = camera->GetSineHalfField();

	const Vector3D& size = GetObject()->GetBoxSize();

	unsigned_int32 front = 0;
	int32 faceCount = 0;

	if ((viewPosition.x > size.x) && (viewDirection.x < sineHalfField))
	{
		front = 0x01;
		facePlane[0].Set(-inverseTransform(0,0), -inverseTransform(0,1), -inverseTransform(0,2), size.x - inverseTransform(0,3));
		faceCount = 1;
	}
	else if ((viewPosition.x < 0.0F) && (viewDirection.x > -sineHalfField))
	{
		front = 0x02;
		facePlane[0].Set(inverseTransform(0,0), inverseTransform(0,1), inverseTransform(0,2), inverseTransform(0,3));
		faceCount = 1;
	}

	if ((viewPosition.y > size.y) && (viewDirection.y < sineHalfField))
	{
		front |= 0x04;
		facePlane[faceCount].Set(-inverseTransform(1,0), -inverseTransform(1,1), -inverseTransform(1,2), size.y - inverseTransform(1,3));
		faceCount++;
	}
	else if ((viewPosition.y < 0.0F) && (viewDirection.y > -sineHalfField))
	{
		front |= 0x08;
		facePlane[faceCount].Set(inverseTransform(1,0), inverseTransform(1,1), inverseTransform(1,2), inverseTransform(1,3));
		faceCount++;
	}

	if ((viewPosition.z > size.z) && (viewDirection.z < sineHalfField))
	{
		front |= 0x10;
		facePlane[faceCount].Set(-inverseTransform(2,0), -inverseTransform(2,1), -inverseTransform(2,2), size.z - inverseTransform(2,3));
		faceCount++;
	}
	else if ((viewPosition.z < 0.0F) && (viewDirection.z > -sineHalfField))
	{
		front |= 0x20;
		facePlane[faceCount].Set(inverseTransform(2,0), inverseTransform(2,1), inverseTransform(2,2), inverseTransform(2,3));
		faceCount++;
	}

	if (faceCount == 0)
	{
		return (nullptr);
	}

	unsigned_int32 polygonIndex = occlusionPolygonIndex[front];
	const unsigned_int8 *vertexIndex = occlusionVertexIndex[polygonIndex & 0x1F];
	int32 vertexCount = polygonIndex >> 5;

	const Transform4D& transform = GetWorldTransform();
	for (machine a = 0; a < vertexCount; a++)
	{
		tempVertex[1][a] = transform * (occlusionVertexPosition[vertexIndex[a]] & size);
	}

	const Point3D *vertex = tempVertex[1];
	const Point3D& cameraPosition = camera->GetWorldPosition();

	for (machine a = 0; a < 4; a++)
	{
		int8	location[10];

		Point3D *result = tempVertex[a & 1];
		Antivector4D plane(camera->GetFrustumPlaneNormal(a), cameraPosition);
		vertexCount = Math::ClipPolygon(vertexCount, vertex, plane, location, result);
		if (vertexCount < 3)
		{
			return (nullptr);
		}

		vertex = result;
	}

	return (new OcclusionRegion(camera, vertexCount, vertex, faceCount, facePlane));
}

OcclusionRegion *OcclusionSpace::NewOrthoOcclusionRegion(const OrthoCamera *camera) const
{
	Antivector4D	facePlane[3];

	const Transform4D& inverseTransform = GetInverseWorldTransform();
	Vector3D viewDirection = inverseTransform * -camera->GetWorldTransform()[2];
	const Vector3D& size = GetObject()->GetBoxSize();

	unsigned_int32 front = 0;

	if (viewDirection.x > 0.0F)
	{
		front = 0x01;
		facePlane[0].Set(-1.0F, 0.0F, 0.0F, size.x);
	}
	else
	{
		front = 0x02;
		facePlane[0].Set(1.0F, 0.0F, 0.0F, 0.0F);
	}

	if (viewDirection.y > 0.0F)
	{
		front |= 0x04;
		facePlane[1].Set(0.0F, -1.0F, 0.0F, size.y);
	}
	else
	{
		front |= 0x08;
		facePlane[1].Set(0.0F, 1.0F, 0.0F, 0.0F);
	}

	if (viewDirection.z > 0.0F)
	{
		front |= 0x10;
		facePlane[2].Set(0.0F, 0.0F, -1.0F, size.z);
	}
	else
	{
		front |= 0x20;
		facePlane[2].Set(0.0F, 0.0F, 1.0F, 0.0F);
	}

	unsigned_int32 polygonIndex = occlusionPolygonIndex[front];
	const unsigned_int8 *vertexIndex = occlusionVertexIndex[polygonIndex & 0x1F];
	int32 vertexCount = polygonIndex >> 5;

	OcclusionRegion *occluder = new OcclusionRegion();
	Antivector4D *plane = occluder->GetPlaneArray();

	Point3D v1 = (occlusionVertexPosition[vertexIndex[vertexCount - 1]] & size);
	for (machine a = 0; a < vertexCount; a++)
	{
		Point3D v2 = (occlusionVertexPosition[vertexIndex[a]] & size);
		Vector3D normal = Normalize(viewDirection % (v2 - v1));
		*plane++ = Antivector4D(normal, v1) * inverseTransform;

		v1 = v2;
	}

	for (machine a = 0; a < 3; a++)
	{
		plane[a] = facePlane[a] * inverseTransform;
	}

	occluder->SetPlaneCount(vertexCount + 3);
	return (occluder);
}


PaintSpace::PaintSpace() : Space(kSpacePaint)
{
}

PaintSpace::PaintSpace(const Vector3D& size, const Integer2D& resolution, int32 count) : Space(kSpacePaint)
{
	SetNewObject(new PaintSpaceObject(size, resolution, count));
}

PaintSpace::PaintSpace(const PaintSpace& paintSpace) : Space(paintSpace)
{
}

PaintSpace::~PaintSpace()
{
}

Node *PaintSpace::Replicate(void) const
{
	return (new PaintSpace(*this));
}

void PaintSpace::HandleTransformUpdate(void)
{
	Space::HandleTransformUpdate();

	paintEnvironment.paintTransform = GetInverseWorldTransform();

	const Vector3D& size = GetObject()->GetBoxSize();
	paintEnvironment.paintTransform.GetRow(0) /= size.x;
	paintEnvironment.paintTransform.GetRow(1) /= size.y;
}

void PaintSpace::Preprocess(void)
{
	Space::Preprocess();

	PaintSpaceObject *object = GetObject();
	object->Preprocess();

	paintEnvironment.paintTexture = object->GetPaintTexturePointer();
}

void PaintSpace::Neutralize(void)
{
	GetObject()->Neutralize();
	Space::Neutralize();
}


PhysicsSpace::PhysicsSpace() : Space(kSpacePhysics)
{
}

PhysicsSpace::PhysicsSpace(const Vector3D& size) : Space(kSpacePhysics)
{
	SetNewObject(new PhysicsSpaceObject(size));
}

PhysicsSpace::PhysicsSpace(const PhysicsSpace& physicsSpace) : Space(physicsSpace)
{
}

PhysicsSpace::~PhysicsSpace()
{
}

Node *PhysicsSpace::Replicate(void) const
{
	return (new PhysicsSpace(*this));
}

// ZYUQURM
