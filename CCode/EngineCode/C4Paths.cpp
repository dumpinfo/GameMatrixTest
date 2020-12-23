 

#include "C4Paths.h"
#include "C4Computation.h"


using namespace C4;


const char C4::kConnectorKeyPath[] = "%Path";


PathComponent::PathComponent(PathType type)
{
	pathType = type;
}

PathComponent::PathComponent(const PathComponent& pathComponent)
{
	pathType = pathComponent.pathType;
}

PathComponent::~PathComponent()
{
}

PathComponent *PathComponent::Create(Unpacker& data, unsigned_int32 unpackFlags)
{
	switch (data.GetType())
	{
		case kPathLinear:

			return (new LinearPathComponent);

		case kPathElliptical:

			return (new EllipticalPathComponent);

		case kPathBezier:

			return (new BezierPathComponent);
	}

	return (nullptr);
}

void PathComponent::PackType(Packer& data) const
{
	data << pathType;
}


LinearPathComponent::LinearPathComponent() : PathComponent(kPathLinear)
{
}

LinearPathComponent::LinearPathComponent(const Point3D& p1, const Point3D& p2) : PathComponent(kPathLinear)
{
	controlPoint[0] = p1;
	controlPoint[1] = p2;
}

LinearPathComponent::LinearPathComponent(const LinearPathComponent& linearPathComponent) : PathComponent(linearPathComponent)
{
	controlPoint[0] = linearPathComponent.controlPoint[0];
	controlPoint[1] = linearPathComponent.controlPoint[1];
}

LinearPathComponent::~LinearPathComponent()
{
}

PathComponent *LinearPathComponent::Replicate(void) const
{
	return (new LinearPathComponent(*this));
}

void LinearPathComponent::Pack(Packer& data, unsigned_int32 packFlags) const
{
	PathComponent::Pack(data, packFlags);

	data << ChunkHeader('CPNT', sizeof(Point3D) * 2);
	data << controlPoint[0];
	data << controlPoint[1];

	data << TerminatorChunk;
}

void LinearPathComponent::Unpack(Unpacker& data, unsigned_int32 unpackFlags)
{
	PathComponent::Unpack(data, unpackFlags);
	UnpackChunkList<LinearPathComponent>(data, unpackFlags);
}

bool LinearPathComponent::UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags)
{
	switch (chunkHeader->chunkType)
	{
		case 'CPNT':

			data >> controlPoint[0];
			data >> controlPoint[1];
			return (true); 
	}

	return (false); 
}
 
Point3D LinearPathComponent::GetPosition(float t) const
{
	return (controlPoint[0] * (1.0F - t) + controlPoint[1] * t); 
}
 
Vector3D LinearPathComponent::GetTangent(float t) const 
{
	return (controlPoint[1] - controlPoint[0]);
}
 
const Point3D& LinearPathComponent::GetBeginPosition(void) const
{
	return (controlPoint[0]);
}

Vector3D LinearPathComponent::GetBeginTangent(void) const
{
	return (controlPoint[1] - controlPoint[0]);
}

const Point3D& LinearPathComponent::GetEndPosition(void) const
{
	return (controlPoint[1]);
}

Vector3D LinearPathComponent::GetEndTangent(void) const
{
	return (controlPoint[1] - controlPoint[0]);
}

int32 LinearPathComponent::GetControlPointCount(void) const
{
	return (2);
}

void LinearPathComponent::GetBoundingBox(Box3D *box) const
{
	float xmin = Fmin(controlPoint[0].x, controlPoint[1].x);
	float ymin = Fmin(controlPoint[0].y, controlPoint[1].y);
	float zmin = Fmin(controlPoint[0].z, controlPoint[1].z);

	float xmax = Fmax(controlPoint[0].x, controlPoint[1].x);
	float ymax = Fmax(controlPoint[0].y, controlPoint[1].y);
	float zmax = Fmax(controlPoint[0].z, controlPoint[1].z);

	box->Set(Point3D(xmin, ymin, zmin), Point3D(xmax, ymax, zmax));
}


EllipticalPathComponent::EllipticalPathComponent() : PathComponent(kPathElliptical)
{
}

EllipticalPathComponent::EllipticalPathComponent(const Point3D& p1, const Point3D& p2, const Point3D& p3) : PathComponent(kPathElliptical)
{
	controlPoint[0] = p1;
	controlPoint[1] = p2;
	controlPoint[2] = p3;
}

EllipticalPathComponent::EllipticalPathComponent(const EllipticalPathComponent& ellipticalPathComponent) : PathComponent(ellipticalPathComponent)
{
	controlPoint[0] = ellipticalPathComponent.controlPoint[0];
	controlPoint[1] = ellipticalPathComponent.controlPoint[1];
	controlPoint[2] = ellipticalPathComponent.controlPoint[2];
}

EllipticalPathComponent::~EllipticalPathComponent()
{
}

PathComponent *EllipticalPathComponent::Replicate(void) const
{
	return (new EllipticalPathComponent(*this));
}

void EllipticalPathComponent::Pack(Packer& data, unsigned_int32 packFlags) const
{
	PathComponent::Pack(data, packFlags);

	data << ChunkHeader('CPNT', sizeof(Point3D) * 3);
	data << controlPoint[0];
	data << controlPoint[1];
	data << controlPoint[2];

	data << TerminatorChunk;
}

void EllipticalPathComponent::Unpack(Unpacker& data, unsigned_int32 unpackFlags)
{
	PathComponent::Unpack(data, unpackFlags);
	UnpackChunkList<EllipticalPathComponent>(data, unpackFlags);
}

bool EllipticalPathComponent::UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags)
{
	switch (chunkHeader->chunkType)
	{
		case 'CPNT':

			data >> controlPoint[0];
			data >> controlPoint[1];
			data >> controlPoint[2];
			return (true);
	}

	return (false);
}

Point3D EllipticalPathComponent::GetPosition(float t) const
{
	Vector3D dp = controlPoint[1] - controlPoint[0];
	Vector3D tangent = Normalize(controlPoint[2] - controlPoint[0]);
	Vector3D tx = ProjectOnto(dp, tangent);
	Vector3D ty = dp - tx;

	Vector2D v = CosSin(t * K::tau_over_4);
	return (controlPoint[0] + tx * v.y + ty * (1.0F - v.x));
}

Vector3D EllipticalPathComponent::GetTangent(float t) const
{
	Vector3D dp = controlPoint[1] - controlPoint[0];
	Vector3D tangent = Normalize(controlPoint[2] - controlPoint[0]);
	Vector3D tx = ProjectOnto(dp, tangent);
	Vector3D ty = dp - tx;

	Vector2D v = CosSin(t * K::tau_over_4);
	return (tx * v.x + ty * v.y);
}

const Point3D& EllipticalPathComponent::GetBeginPosition(void) const
{
	return (controlPoint[0]);
}

Vector3D EllipticalPathComponent::GetBeginTangent(void) const
{
	return (controlPoint[2] - controlPoint[0]);
}

const Point3D& EllipticalPathComponent::GetEndPosition(void) const
{
	return (controlPoint[1]);
}

Vector3D EllipticalPathComponent::GetEndTangent(void) const
{
	Vector3D dp = controlPoint[1] - controlPoint[0];
	Vector3D tangent = Normalize(controlPoint[2] - controlPoint[0]);
	return (dp - ProjectOnto(dp, tangent));
}

int32 EllipticalPathComponent::GetControlPointCount(void) const
{
	return (3);
}

void EllipticalPathComponent::GetBoundingBox(Box3D *box) const
{
	float xmin = Fmin(controlPoint[0].x, controlPoint[1].x);
	float ymin = Fmin(controlPoint[0].y, controlPoint[1].y);
	float zmin = Fmin(controlPoint[0].z, controlPoint[1].z);

	float xmax = Fmax(controlPoint[0].x, controlPoint[1].x);
	float ymax = Fmax(controlPoint[0].y, controlPoint[1].y);
	float zmax = Fmax(controlPoint[0].z, controlPoint[1].z);

	Vector3D dp = controlPoint[1] - controlPoint[0];
	Vector3D tangent = Normalize(controlPoint[2] - controlPoint[0]);
	Vector3D tx = tangent * (tangent * dp);
	Vector3D ty = dp - tx;

	Point3D p1 = controlPoint[0] + tx;
	Point3D p2 = controlPoint[0] + ty;

	xmin = Fmin(xmin, controlPoint[2].x, p1.x, p2.x);
	ymin = Fmin(ymin, controlPoint[2].y, p1.y, p2.y);
	zmin = Fmin(zmin, controlPoint[2].z, p1.z, p2.z);

	xmax = Fmax(xmax, controlPoint[2].x, p1.x, p2.x);
	ymax = Fmax(ymax, controlPoint[2].y, p1.y, p2.y);
	zmax = Fmax(zmax, controlPoint[2].z, p1.z, p2.z);

	box->Set(Point3D(xmin, ymin, zmin), Point3D(xmax, ymax, zmax));
}


BezierPathComponent::BezierPathComponent() : PathComponent(kPathBezier)
{
}

BezierPathComponent::BezierPathComponent(const Point3D& p1, const Point3D& p2, const Point3D& p3, const Point3D& p4) : PathComponent(kPathBezier)
{
	controlPoint[0] = p1;
	controlPoint[1] = p2;
	controlPoint[2] = p3;
	controlPoint[3] = p4;
}

BezierPathComponent::BezierPathComponent(const BezierPathComponent& bezierPathComponent) : PathComponent(bezierPathComponent)
{
	controlPoint[0] = bezierPathComponent.controlPoint[0];
	controlPoint[1] = bezierPathComponent.controlPoint[1];
	controlPoint[2] = bezierPathComponent.controlPoint[2];
	controlPoint[3] = bezierPathComponent.controlPoint[3];
}

BezierPathComponent::~BezierPathComponent()
{
}

PathComponent *BezierPathComponent::Replicate(void) const
{
	return (new BezierPathComponent(*this));
}

void BezierPathComponent::Pack(Packer& data, unsigned_int32 packFlags) const
{
	PathComponent::Pack(data, packFlags);

	data << ChunkHeader('CPNT', sizeof(Point3D) * 4);
	data << controlPoint[0];
	data << controlPoint[1];
	data << controlPoint[2];
	data << controlPoint[3];

	data << TerminatorChunk;
}

void BezierPathComponent::Unpack(Unpacker& data, unsigned_int32 unpackFlags)
{
	PathComponent::Unpack(data, unpackFlags);
	UnpackChunkList<BezierPathComponent>(data, unpackFlags);
}

bool BezierPathComponent::UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags)
{
	switch (chunkHeader->chunkType)
	{
		case 'CPNT':

			data >> controlPoint[0];
			data >> controlPoint[1];
			data >> controlPoint[2];
			data >> controlPoint[3];
			return (true);
	}

	return (false);
}

Point3D BezierPathComponent::GetPosition(float t) const
{
	float u = 1.0F - t;
	float t2 = t * t;
	float u2 = u * u;

	return (controlPoint[0] * (u2 * u) + controlPoint[1] * (3.0F * u2 * t) + controlPoint[2] * (3.0F * u * t2) + controlPoint[3] * (t2 * t));
}

Vector3D BezierPathComponent::GetTangent(float t) const
{
	float t2 = t * t;

	return (controlPoint[0] * (-1.0F + 2.0F * t - t2) + controlPoint[1] * (1.0F - 4.0F * t + 3.0F * t2) + controlPoint[2] * (2.0F * t - 3.0F * t2) + controlPoint[3] * t2);
}

const Point3D& BezierPathComponent::GetBeginPosition(void) const
{
	return (controlPoint[0]);
}

Vector3D BezierPathComponent::GetBeginTangent(void) const
{
	return (controlPoint[1] - controlPoint[0]);
}

const Point3D& BezierPathComponent::GetEndPosition(void) const
{
	return (controlPoint[3]);
}

Vector3D BezierPathComponent::GetEndTangent(void) const
{
	return (controlPoint[3] - controlPoint[2]);
}

int32 BezierPathComponent::GetControlPointCount(void) const
{
	return (4);
}

void BezierPathComponent::GetBoundingBox(Box3D *box) const
{
	float xmin = controlPoint[0].x;
	float ymin = controlPoint[0].y;
	float zmin = controlPoint[0].z;

	float xmax = xmin;
	float ymax = ymin;
	float zmax = zmin;

	for (machine a = 1; a < 4; a++)
	{
		xmin = Fmin(xmin, controlPoint[a].x);
		ymin = Fmin(ymin, controlPoint[a].y);
		zmin = Fmin(zmin, controlPoint[a].z);

		xmax = Fmax(xmax, controlPoint[a].x);
		ymax = Fmax(ymax, controlPoint[a].y);
		zmax = Fmax(zmax, controlPoint[a].z);
	}

	box->Set(Point3D(xmin, ymin, zmin), Point3D(xmax, ymax, zmax));
}


Path::Path()
{
}

Path::Path(const Vector3D& normal)
{
	pathNormal = normal;
}

Path::Path(const Path& path)
{
	pathNormal = path.pathNormal;

	const PathComponent *component = path.pathComponentList.First();
	while (component)
	{
		pathComponentList.Append(component->Clone());
		component = component->Next();
	}
}

Path::~Path()
{
}

Path& Path::operator =(const Path& path)
{
	pathComponentList.Purge();

	pathNormal = path.pathNormal;

	const PathComponent *component = path.pathComponentList.First();
	while (component)
	{
		pathComponentList.Append(component->Clone());
		component = component->Next();
	}

	return (*this);
}

void Path::Pack(Packer& data, unsigned_int32 packFlags) const
{
	data << ChunkHeader('NRML', sizeof(Vector3D));
	data << pathNormal;

	const PathComponent *component = pathComponentList.First();
	while (component)
	{
		PackHandle handle = data.BeginChunk('COMP');
		component->PackType(data);
		component->Pack(data, packFlags);
		data.EndChunk(handle);

		component = component->Next();
	}

	data << TerminatorChunk;
}

void Path::Unpack(Unpacker& data, unsigned_int32 unpackFlags)
{
	UnpackChunkList<Path>(data, unpackFlags);
}

bool Path::UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags)
{
	switch (chunkHeader->chunkType)
	{
		case 'NRML':

			data >> pathNormal;
			return (true);

		case 'COMP':
		{
			PathComponent *component = PathComponent::Create(data, unpackFlags);
			if (component)
			{
				component->Unpack(++data, unpackFlags);
				pathComponentList.Append(component);
				return (true);
			}

			break;
		}
	}

	return (false);
}

void *Path::BeginSettingsUnpack(void)
{
	pathComponentList.Purge();
	return (nullptr);
}

Point3D Path::GetPathState(float t, Vector3D *tangent, Vector3D *binormal) const
{
	int32 count = pathComponentList.GetElementCount();
	t = (float) count * t;

	int32 index = Min((int32) t, count - 1);
	t -= (float) index;

	const PathComponent *component = pathComponentList[index];
	if (tangent)
	{
		*tangent = component->GetTangent(t);
		if (binormal)
		{
			*binormal = *tangent % pathNormal;
		}
	}

	return (component->GetPosition(t));
}

void Path::GetBoundingBox(Box3D *box) const
{
	const PathComponent *component = pathComponentList.First();
	component->GetBoundingBox(box);

	for (;;)
	{
		Box3D	temp;

		component = component->Next();
		if (!component)
		{
			break;
		}

		component->GetBoundingBox(&temp);
		box->Union(temp);
	}
}

bool Path::LinearPath(void) const
{
	const PathComponent *component = pathComponentList.First();
	while (component)
	{
		if (component->GetPathType() != kPathLinear)
		{
			return (false);
		}

		component = component->Next();
	}

	return (true);
}


PathMarker::PathMarker() : Marker(kMarkerPath)
{
}

PathMarker::PathMarker(const Vector3D& normal) :
		Marker(kMarkerPath),
		markerPath(normal)
{
}

PathMarker::PathMarker(const PathMarker& pathMarker) :
		Marker(pathMarker),
		markerPath(pathMarker.markerPath)
{
}

PathMarker::~PathMarker()
{
}

Node *PathMarker::Replicate(void) const
{
	return (new PathMarker(*this));
}

void PathMarker::Pack(Packer& data, unsigned_int32 packFlags) const
{
	Marker::Pack(data, packFlags);

	PackHandle handle = data.BeginChunk('PATH');
	markerPath.Pack(data, packFlags);
	data.EndChunk(handle);

	data << TerminatorChunk;
}

void PathMarker::Unpack(Unpacker& data, unsigned_int32 unpackFlags)
{
	Marker::Unpack(data, unpackFlags);
	UnpackChunkList<PathMarker>(data, unpackFlags);
}

bool PathMarker::UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags)
{
	switch (chunkHeader->chunkType)
	{
		case 'PATH':

			markerPath.Unpack(data, unpackFlags);
			return (true);
	}

	return (false);
}

void *PathMarker::BeginSettingsUnpack(void)
{
	markerPath.BeginSettingsUnpack();
	return (Marker::BeginSettingsUnpack());
}

void PathMarker::EndSettingsUnpack(void *cookie)
{
	Marker::EndSettingsUnpack(cookie);
	markerPath.EndSettingsUnpack(nullptr);
}

// ZYUQURM
