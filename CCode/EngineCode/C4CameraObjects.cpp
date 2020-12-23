 

#include "C4CameraObjects.h"
#include "C4Graphics.h"
#include "C4Configuration.h"


using namespace C4;


CameraObject::CameraObject(CameraType type, void (GraphicsMgr::*proc)(void)) : Object(kObjectCamera)
{
	cameraType = type;
	activateProc = proc;

	projectionOffset = 0.0F;

	nearDepth = 0.1F;
	farDepth = 1000.0F;

	clearFlags = 0;
	clearColor.Set(0.0F, 0.0F, 0.0F, 0.0F);
}

CameraObject::~CameraObject()
{
}

CameraObject *CameraObject::Create(Unpacker& data, unsigned_int32 unpackFlags)
{
	switch (data.GetType())
	{
		case kCameraOrtho:

			return (new OrthoCameraObject);

		case kCameraFrustum:

			return (new FrustumCameraObject);

		case kCameraRemote:

			return (new RemoteCameraObject);
	}

	return (nullptr);
}

void CameraObject::PackType(Packer& data) const
{
	Object::PackType(data);
	data << cameraType;
}

void CameraObject::Pack(Packer& data, unsigned_int32 packFlags) const
{
	data << ChunkHeader('VIEW', sizeof(Rect) + 8);
	data << viewRect;
	data << nearDepth;
	data << farDepth;

	data << ChunkHeader('CLER', 4 + sizeof(ColorRGBA));
	data << clearFlags;
	data << clearColor;

	data << TerminatorChunk;
}

void CameraObject::Unpack(Unpacker& data, unsigned_int32 unpackFlags)
{
	UnpackChunkList<CameraObject>(data, unpackFlags);
}

bool CameraObject::UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags)
{
	switch (chunkHeader->chunkType)
	{
		case 'VIEW':
		{
			data >> viewRect;
			data >> nearDepth;
			data >> farDepth;
			return (true);
		}

		case 'CLER':

			data >> clearFlags;
			data >> clearColor;
			return (true);
	}

	return (false);
}


OrthoCameraObject::OrthoCameraObject() : CameraObject(kCameraOrtho, &GraphicsMgr::ActivateOrthoCamera)
{
}

OrthoCameraObject::~OrthoCameraObject()
{
} 

void OrthoCameraObject::Pack(Packer& data, unsigned_int32 packFlags) const
{ 
	CameraObject::Pack(data, packFlags);
 
	data << ChunkHeader('ORTH', 16);
	data << orthoLeft;
	data << orthoRight; 
	data << orthoTop;
	data << orthoBottom; 
 
	data << TerminatorChunk;
}

void OrthoCameraObject::Unpack(Unpacker& data, unsigned_int32 unpackFlags) 
{
	CameraObject::Unpack(data, unpackFlags);
	UnpackChunkList<OrthoCameraObject>(data, unpackFlags);
}

bool OrthoCameraObject::UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags)
{
	switch (chunkHeader->chunkType)
	{
		case 'ORTH':
		{
			data >> orthoLeft;
			data >> orthoRight;
			data >> orthoTop;
			data >> orthoBottom;
			return (true);
		}
	}

	return (false);
}


FrustumCameraObject::FrustumCameraObject() : CameraObject(kCameraFrustum, &GraphicsMgr::ActivateFrustumCamera)
{
}

FrustumCameraObject::FrustumCameraObject(CameraType type, void (GraphicsMgr::*proc)(void)) : CameraObject(type, proc)
{
}

FrustumCameraObject::FrustumCameraObject(CameraType type, void (GraphicsMgr::*proc)(void), float focal, float aspect) : CameraObject(type, proc)
{
	frustumFlags = 0;
	focalLength = focal;
	aspectRatio = aspect;
}

FrustumCameraObject::FrustumCameraObject(float focal, float aspect) : CameraObject(kCameraFrustum, &GraphicsMgr::ActivateFrustumCamera)
{
	frustumFlags = 0;
	focalLength = focal;
	aspectRatio = aspect;
}

FrustumCameraObject::~FrustumCameraObject()
{
}

void FrustumCameraObject::Pack(Packer& data, unsigned_int32 packFlags) const
{
	CameraObject::Pack(data, packFlags);

	data << ChunkHeader('FRUS', 12);
	data << frustumFlags;
	data << focalLength;
	data << aspectRatio;

	data << TerminatorChunk;
}

void FrustumCameraObject::Unpack(Unpacker& data, unsigned_int32 unpackFlags)
{
	CameraObject::Unpack(data, unpackFlags);
	UnpackChunkList<FrustumCameraObject>(data, unpackFlags);
}

bool FrustumCameraObject::UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags)
{
	switch (chunkHeader->chunkType)
	{
		case 'FRUS':
		{
			data >> frustumFlags;
			data >> focalLength;
			data >> aspectRatio;
			return (true);
		}
	}

	return (false);
}

int32 FrustumCameraObject::GetObjectSize(float *size) const
{
	size[0] = GetFarDepth();
	size[1] = focalLength;
	return (2);
}

void FrustumCameraObject::SetObjectSize(const float *size)
{
	SetFarDepth(size[0]);
	focalLength = size[1];
}

ProjectionResult FrustumCameraObject::ProjectSphere(const Point3D& center, float radius, float offset, ProjectionRect *rect) const
{
	float cx = center.x;
	float cy = center.y;
	float cz = center.z;
	float r2 = radius * radius;

	float cx2 = cx * cx;
	float cy2 = cy * cy;
	float cz2 = cz * cz;
	float cxz2 = cx2 + cz2;
	if (cxz2 + cy2 > r2)
	{
		float left = -1.0F;
		float right = 1.0F;
		float bottom = -1.0F;
		float top = 1.0F;

		float rcz = 1.0F / cz;

		float dx = (cxz2 - r2) * cz2;
		if (dx > K::min_float)
		{
			dx = Sqrt(dx);
			float ax = 1.0F / cxz2;
			float bx = radius * cx;

			float nx1 = (bx + dx) * ax;
			float nx2 = (bx - dx) * ax;

			float nz1 = (radius - nx1 * cx) * rcz;
			float nz2 = (radius - nx2 * cx) * rcz;

			float pz1 = cz - radius * nz1;
			float pz2 = cz - radius * nz2;

			if (pz1 < 0.0F)
			{
				float x = nz1 * focalLength / nx1 - offset;
				if (nx1 > 0.0F)
				{
					left = Fmax(left, x);
				}
				else
				{
					right = Fmin(right, x);
				}
			}

			if (pz2 < 0.0F)
			{
				float x = nz2 * focalLength / nx2 - offset;
				if (nx2 < 0.0F)
				{
					right = Fmin(right, x);
				}
				else
				{
					left = Fmax(left, x);
				}
			}
		}

		float cyz2 = cy2 + cz2;
		float dy = (cyz2 - r2) * cz2;
		if (dy > K::min_float)
		{
			dy = Sqrt(dy);
			float ay = 1.0F / cyz2;
			float by = radius * cy;

			float ny1 = (by + dy) * ay;
			float ny2 = (by - dy) * ay;

			float nz1 = (radius - ny1 * cy) * rcz;
			float nz2 = (radius - ny2 * cy) * rcz;

			float pz1 = cz - radius * nz1;
			float pz2 = cz - radius * nz2;

			if (pz1 < 0.0F)
			{
				float y = nz1 * focalLength / (ny1 * aspectRatio);
				if (ny1 > 0.0F)
				{
					bottom = Fmax(bottom, y);
				}
				else
				{
					top = Fmin(top, y);
				}
			}

			if (pz2 < 0.0F)
			{
				float y = nz2 * focalLength / (ny2 * aspectRatio);
				if (ny2 < 0.0F)
				{
					top = Fmin(top, y);
				}
				else
				{
					bottom = Fmax(bottom, y);
				}
			}
		}

		if ((!(left < right)) || (!(bottom < top)))
		{
			return (kProjectionEmpty);
		}

		rect->left = left;
		rect->right = right;
		rect->bottom = bottom;
		rect->top = top;

		return (kProjectionPartial);
	}

	return (kProjectionFull);
}


RemoteCameraObject::RemoteCameraObject() : FrustumCameraObject(kCameraRemote, &GraphicsMgr::ActivateRemoteCamera)
{
}

RemoteCameraObject::RemoteCameraObject(float focal, float aspect, const Transform4D& transform, const Antivector4D& clipPlane) : FrustumCameraObject(kCameraRemote, &GraphicsMgr::ActivateRemoteCamera, focal, aspect)
{
	SetRemoteTransform(transform);
	SetRemoteClipPlane(clipPlane);
}

RemoteCameraObject::~RemoteCameraObject()
{
}

void RemoteCameraObject::Pack(Packer& data, unsigned_int32 packFlags) const
{
	FrustumCameraObject::Pack(data, packFlags);

	data << ChunkHeader('RMOT', sizeof(Transform4D));
	data << remoteTransform;

	data << TerminatorChunk;
}

void RemoteCameraObject::Unpack(Unpacker& data, unsigned_int32 unpackFlags)
{
	FrustumCameraObject::Unpack(data, unpackFlags);
	UnpackChunkList<RemoteCameraObject>(data, unpackFlags);
}

bool RemoteCameraObject::UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags)
{
	switch (chunkHeader->chunkType)
	{
		case 'RMOT':
		{
			Transform4D		transform;

			data >> transform;
			SetRemoteTransform(transform);
			return (true);
		}
	}

	return (false);
}

void RemoteCameraObject::SetRemoteTransform(const Transform4D& transform)
{
	remoteTransform = transform;
	inverseRemoteTransform = Inverse(transform);
	remoteDeterminant = (Determinant(transform) < 0.0F) ? -1.0F : 1.0F;
}

void RemoteCameraObject::SetFrustumBoundary(float left, float right, float top, float bottom)
{
	frustumBoundary.left = Fmax(left, -1.0F);
	frustumBoundary.right = Fmin(right, 1.0F);

	float a = 1.0F / GetAspectRatio();
	frustumBoundary.bottom = Fmax(bottom * -a, -1.0F);
	frustumBoundary.top = Fmin(top * -a, 1.0F);
}

// ZYUQURM
