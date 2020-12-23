 

#include "C4Emitters.h"
#include "C4Configuration.h"
#include "C4Random.h"


using namespace C4;


EmitterObject::EmitterObject(EmitterType type, Volume *volume) :
		Object(kObjectEmitter),
		VolumeObject(volume)
{
	emitterType = type;
	emitterFlags = 0;

	heightFieldSubdivX = 32;
	heightFieldSubdivY = 32;
	heightFieldScale.Set(32.0F, 32.0F);

	heightFieldStorage = nullptr;
}

EmitterObject::~EmitterObject()
{
	delete[] heightFieldStorage;
}

EmitterObject *EmitterObject::Create(Unpacker& data, unsigned_int32 unpackFlags)
{
	switch (data.GetType())
	{
		case kEmitterBox:

			return (new BoxEmitterObject);

		case kEmitterCylinder:

			return (new CylinderEmitterObject);

		case kEmitterSphere:

			return (new SphereEmitterObject);
	}

	return (nullptr);
}

void EmitterObject::PackType(Packer& data) const
{
	Object::PackType(data);
	data << emitterType;
}

void EmitterObject::Pack(Packer& data, unsigned_int32 packFlags) const
{
	data << ChunkHeader('FLAG', 4);
	data << emitterFlags;

	if (heightFieldStorage)
	{
		int32 heightCount = (heightFieldSubdivX + 1) * (heightFieldSubdivY + 1);

		data << ChunkHeader('HITE', 8 + heightCount * 4);
		data << heightFieldSubdivX;
		data << heightFieldSubdivY;
		data.WriteArray(heightCount, heightFieldStorage);
	}

	data << TerminatorChunk;

	PackVolume(data, packFlags);
}

void EmitterObject::Unpack(Unpacker& data, unsigned_int32 unpackFlags)
{
	UnpackChunkList<EmitterObject>(data, unpackFlags);
	UnpackVolume(data, unpackFlags);
}

bool EmitterObject::UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags)
{
	switch (chunkHeader->chunkType)
	{
		case 'FLAG':

			data >> emitterFlags;
			return (true);

		case 'HITE':
		{
			data >> heightFieldSubdivX;
			data >> heightFieldSubdivY;
			heightFieldScale.Set((float) heightFieldSubdivX, (float) heightFieldSubdivY);

			int32 heightCount = (heightFieldSubdivX + 1) * (heightFieldSubdivY + 1);

			heightFieldStorage = new float[heightCount];
			data.ReadArray(heightCount, heightFieldStorage);
			return (true);
		}
	} 

	return (false);
} 

void *EmitterObject::BeginSettingsUnpack(void) 
{
	delete[] heightFieldStorage;
	heightFieldStorage = nullptr; 

	return (Object::BeginSettingsUnpack()); 
} 

int32 EmitterObject::GetCategoryCount(void) const
{
	return (1); 
}

Type EmitterObject::GetCategoryType(int32 index, const char **title) const
{
	if (index == 0)
	{
		*title = TheInterfaceMgr->GetStringTable()->GetString(StringID(kObjectEmitter));
		return (kObjectEmitter);
	}

	return (0);
}

int32 EmitterObject::GetCategorySettingCount(Type category) const
{
	if (category == kObjectEmitter)
	{
		return (4);
	}

	return (0);
}

Setting *EmitterObject::GetCategorySetting(Type category, int32 index, unsigned_int32 flags) const
{
	if (category == kObjectEmitter)
	{
		if (flags & kConfigurationScript)
		{
			return (nullptr);
		}

		const StringTable *table = TheInterfaceMgr->GetStringTable();

		if (index == 0)
		{
			const char *title = table->GetString(StringID(kObjectEmitter, 'EMIT'));
			return (new HeadingSetting(kObjectEmitter, title));
		}

		if (index == 1)
		{
			const char *title = table->GetString(StringID(kObjectEmitter, 'EMIT', 'HITE'));
			return (new BooleanSetting('HITE', ((emitterFlags & kEmitterHeightField) != 0), title));
		}

		if (index == 2)
		{
			const char *title = table->GetString(StringID(kObjectEmitter, 'EMIT', 'XSUB'));
			return (new TextSetting('XSUB', Text::IntegerToString(heightFieldSubdivX), title, 3, &EditTextWidget::NumberFilter));
		}

		if (index == 3)
		{
			const char *title = table->GetString(StringID(kObjectEmitter, 'EMIT', 'YSUB'));
			return (new TextSetting('YSUB', Text::IntegerToString(heightFieldSubdivY), title, 3, &EditTextWidget::NumberFilter));
		}
	}

	return (nullptr);
}

void EmitterObject::SetCategorySetting(Type category, const Setting *setting)
{
	if (category == kObjectEmitter)
	{
		Type identifier = setting->GetSettingIdentifier();

		if (identifier == 'HITE')
		{
			if (static_cast<const BooleanSetting *>(setting)->GetBooleanValue())
			{
				emitterFlags |= kEmitterHeightField;
			}
			else
			{
				emitterFlags &= ~kEmitterHeightField;

				delete[] heightFieldStorage;
				heightFieldStorage = nullptr;
			}
		}
		else if (identifier == 'XSUB')
		{
			heightFieldSubdivX = Max(Text::StringToInteger(static_cast<const TextSetting *>(setting)->GetText()), 1);
			heightFieldScale.x = (float) heightFieldSubdivX;
		}
		else if (identifier == 'YSUB')
		{
			heightFieldSubdivY = Max(Text::StringToInteger(static_cast<const TextSetting *>(setting)->GetText()), 1);
			heightFieldScale.y = (float) heightFieldSubdivY;

			if (emitterFlags & kEmitterHeightField)
			{
				delete[] heightFieldStorage;

				int32 heightCount = (heightFieldSubdivX + 1) * (heightFieldSubdivY + 1);
				heightFieldStorage = new float[heightCount];
				MemoryMgr::ClearMemory(heightFieldStorage, heightCount * 4);
			}
		}
	}
}

int32 EmitterObject::GetObjectSize(float *size) const
{
	return (GetVolumeObjectSize(size));
}

void EmitterObject::SetObjectSize(const float *size)
{
	SetVolumeObjectSize(size);
}

void EmitterObject::SetHeightFieldSubdiv(int32 subdivX, int32 subdivY)
{
	heightFieldSubdivX = subdivX;
	heightFieldSubdivY = subdivY;
	heightFieldScale.Set((float) subdivX, (float) subdivY);
}

void EmitterObject::AllocateHeightField(void)
{
	delete[] heightFieldStorage;

	int32 heightCount = (heightFieldSubdivX + 1) * (heightFieldSubdivY + 1);
	heightFieldStorage = new float[heightCount];
	MemoryMgr::ClearMemory(heightFieldStorage, heightCount * 4);

	emitterFlags |= kEmitterHeightField;
}

void EmitterObject::DeallocateHeightField(void)
{
	delete[] heightFieldStorage;
	heightFieldStorage = nullptr;

	emitterFlags &= ~kEmitterHeightField;
}

float EmitterObject::SampleHeightField(float x, float y) const
{
	x *= heightFieldScale.x;
	y *= heightFieldScale.y;

	int32 i1 = MaxZero(Min((int32) x, heightFieldSubdivX));
	int32 j1 = MaxZero(Min((int32) y, heightFieldSubdivY));
	int32 i2 = Min(i1 + 1, heightFieldSubdivX);
	int32 j2 = Min(j1 + 1, heightFieldSubdivY);

	x -= PositiveFloor(x);
	y -= PositiveFloor(y);

	const float *h1 = heightFieldStorage + j1 * (heightFieldSubdivX + 1);
	const float *h2 = heightFieldStorage + j2 * (heightFieldSubdivX + 1);
	float z1 = h1[i1] * (1.0F - x) + h1[i2] * x;
	float z2 = h2[i1] * (1.0F - x) + h2[i2] * x;
	return (z1 * (1.0F - y) + z2 * y);
}


BoxEmitterObject::BoxEmitterObject() : EmitterObject(kEmitterBox, this)
{
}

BoxEmitterObject::BoxEmitterObject(const Vector3D& size) :
		EmitterObject(kEmitterBox, this),
		BoxVolume(size)
{
}

BoxEmitterObject::~BoxEmitterObject()
{
}

float BoxEmitterObject::GetEmitterRadius(void) const
{
	const Vector3D& boxSize = GetBoxSize();
	return (Fmax(boxSize.x, boxSize.y) * 0.5F);
}

float BoxEmitterObject::GetEmitterSurfaceArea(void) const
{
	const Vector3D& boxSize = GetBoxSize();
	return (boxSize.x * boxSize.y);
}

Point3D BoxEmitterObject::GetVolumeEmissionPoint(void) const
{
	const Vector3D& boxSize = GetBoxSize();
	return (Point3D(Math::RandomFloat(boxSize.x), Math::RandomFloat(boxSize.y), Math::RandomFloat(boxSize.z)));
}

Point3D BoxEmitterObject::GetTopSurfaceEmissionPoint(void) const
{
	const Vector3D& boxSize = GetBoxSize();
	return (Point3D(Math::RandomFloat(boxSize.x), Math::RandomFloat(boxSize.y), boxSize.z));
}

Point3D BoxEmitterObject::GetBottomSurfaceEmissionPoint(void) const
{
	const Vector3D& boxSize = GetBoxSize();

	const float *heightField = GetHeightFieldStorage();
	if (heightField)
	{
		float x = Math::RandomFloat(1.0F);
		float y = Math::RandomFloat(1.0F);
		float z = SampleHeightField(x, y);
		return (Point3D(x * boxSize.x, y * boxSize.y, z));
	}

	float x = Math::RandomFloat(boxSize.x);
	float y = Math::RandomFloat(boxSize.y);
	return (Point3D(x, y, 0.0F));
}


CylinderEmitterObject::CylinderEmitterObject() : EmitterObject(kEmitterCylinder, this)
{
}

CylinderEmitterObject::CylinderEmitterObject(const Vector2D& size, float height) :
		EmitterObject(kEmitterCylinder, this),
		CylinderVolume(size, height)
{
}

CylinderEmitterObject::~CylinderEmitterObject()
{
}

float CylinderEmitterObject::GetEmitterRadius(void) const
{
	const Vector2D& cylinderSize = GetCylinderSize();
	return (Fmax(cylinderSize.x, cylinderSize.y));
}

float CylinderEmitterObject::GetEmitterSurfaceArea(void) const
{
	const Vector2D& cylinderSize = GetCylinderSize();
	return (K::tau_over_2 * cylinderSize.x * cylinderSize.y);
}

Point3D CylinderEmitterObject::GetVolumeEmissionPoint(void) const
{
	Vector2D t = CosSin(Math::RandomFloat(K::tau));
	float r = Sqrt(Math::RandomFloat(1.0F));

	const Vector2D& cylinderSize = GetCylinderSize();
	return (Point3D(t.x * cylinderSize.x * r, t.y * cylinderSize.y * r, Math::RandomFloat(GetCylinderHeight())));
}

Point3D CylinderEmitterObject::GetTopSurfaceEmissionPoint(void) const
{
	Vector2D t = CosSin(Math::RandomFloat(K::tau));
	float r = Sqrt(Math::RandomFloat(1.0F));

	const Vector2D& cylinderSize = GetCylinderSize();
	return (Point3D(t.x * cylinderSize.x * r, t.y * cylinderSize.y * r, GetCylinderHeight()));
}

Point3D CylinderEmitterObject::GetBottomSurfaceEmissionPoint(void) const
{
	Vector2D p = CosSin(Math::RandomFloat(K::tau)) * Sqrt(Math::RandomFloat(1.0F));
	const Vector2D& cylinderSize = GetCylinderSize();

	const float *heightField = GetHeightFieldStorage();
	if (heightField)
	{
		float z = SampleHeightField(p.x * 0.5F + 0.5F, p.y * 0.5F + 0.5F);
		return (Point3D(p.x * cylinderSize.x, p.y * cylinderSize.y, z));
	}

	return (Point3D(p.x * cylinderSize.x, p.y * cylinderSize.y, 0.0F));
}


SphereEmitterObject::SphereEmitterObject() : EmitterObject(kEmitterSphere, this)
{
}

SphereEmitterObject::SphereEmitterObject(const Vector3D& size) :
		EmitterObject(kEmitterSphere, this),
		SphereVolume(size)
{
}

SphereEmitterObject::~SphereEmitterObject()
{
}

float SphereEmitterObject::GetEmitterRadius(void) const
{
	const Vector3D& sphereSize = GetSphereSize();
	return (Fmax(sphereSize.x, sphereSize.y, sphereSize.z));
}

float SphereEmitterObject::GetEmitterSurfaceArea(void) const
{
	const Vector3D& sphereSize = GetSphereSize();
	return (K::tau_over_2 * sphereSize.x * sphereSize.y);
}

Point3D SphereEmitterObject::GetVolumeEmissionPoint(void) const
{
	Vector3D v = Math::RandomUnitVector3D();
	float r = Sqrt(Math::RandomFloat(1.0F));

	const Vector3D& sphereSize = GetSphereSize();
	return (Point3D(sphereSize.x * v.x * r, sphereSize.y * v.y * r, sphereSize.z * v.z * r));
}

Point3D SphereEmitterObject::GetTopSurfaceEmissionPoint(void) const
{
	Vector3D v = Math::RandomUnitVector3D();
	const Vector3D& sphereSize = GetSphereSize();
	return (Point3D(sphereSize.x * v.x, sphereSize.y * v.y, sphereSize.z * Fabs(v.z)));
}

Point3D SphereEmitterObject::GetBottomSurfaceEmissionPoint(void) const
{
	Vector3D p = Math::RandomUnitVector3D();
	const Vector3D& sphereSize = GetSphereSize();
	const float *heightField = GetHeightFieldStorage();

	if (heightField)
	{
		float z = SampleHeightField(p.x * 0.5F + 0.5F, p.y * 0.5F + 0.5F);
		return (Point3D(p.x * sphereSize.x, p.y * sphereSize.y, Fmax(z - sphereSize.z, sphereSize.z * Fnabs(p.z))));
	}

	return (Point3D(sphereSize.x * p.x, sphereSize.y * p.y, sphereSize.z * Fnabs(p.z)));
}


Emitter::Emitter(EmitterType type) : Node(kNodeEmitter)
{
	emitterType = type;
}

Emitter::Emitter(const Emitter& emitter) : Node(emitter)
{
	emitterType = emitter.emitterType;
}

Emitter::~Emitter()
{
}

Emitter *Emitter::Create(Unpacker& data, unsigned_int32 unpackFlags)
{
	switch (data.GetType())
	{
		case kEmitterBox:

			return (new BoxEmitter);

		case kEmitterCylinder:

			return (new CylinderEmitter);

		case kEmitterSphere:

			return (new SphereEmitter);
	}

	return (nullptr);
}

void Emitter::PackType(Packer& data) const
{
	Node::PackType(data);
	data << emitterType;
}


BoxEmitter::BoxEmitter() : Emitter(kEmitterBox)
{
}

BoxEmitter::BoxEmitter(const Vector3D& size) : Emitter(kEmitterBox)
{
	SetNewObject(new BoxEmitterObject(size));
}

BoxEmitter::BoxEmitter(const BoxEmitter& boxEmitter) : Emitter(boxEmitter)
{
}

BoxEmitter::~BoxEmitter()
{
}

Node *BoxEmitter::Replicate(void) const
{
	return (new BoxEmitter(*this));
}


CylinderEmitter::CylinderEmitter() : Emitter(kEmitterCylinder)
{
}

CylinderEmitter::CylinderEmitter(const Vector2D& size, float height) : Emitter(kEmitterCylinder)
{
	SetNewObject(new CylinderEmitterObject(size, height));
}

CylinderEmitter::CylinderEmitter(const CylinderEmitter& cylinderEmitter) : Emitter(cylinderEmitter)
{
}

CylinderEmitter::~CylinderEmitter()
{
}

Node *CylinderEmitter::Replicate(void) const
{
	return (new CylinderEmitter(*this));
}


SphereEmitter::SphereEmitter() : Emitter(kEmitterSphere)
{
}

SphereEmitter::SphereEmitter(const Vector3D& size) : Emitter(kEmitterSphere)
{
	SetNewObject(new SphereEmitterObject(size));
}

SphereEmitter::SphereEmitter(const SphereEmitter& sphereEmitter) : Emitter(sphereEmitter)
{
}

SphereEmitter::~SphereEmitter()
{
}

Node *SphereEmitter::Replicate(void) const
{
	return (new SphereEmitter(*this));
}

// ZYUQURM
