 

#include "C4Fields.h"
#include "C4Forces.h"
#include "C4Zones.h"
#include "C4Configuration.h"


using namespace C4;


FieldObject::FieldObject(FieldType type, Volume *volume) :
		Object(kObjectField),
		VolumeObject(volume)
{
	fieldType = type;
	fieldFlags = 0;
}

FieldObject::~FieldObject()
{
}

FieldObject *FieldObject::Create(Unpacker& data, unsigned_int32 unpackFlags)
{
	switch (data.GetType())
	{
		case kFieldBox:

			return (new BoxFieldObject);

		case kFieldCylinder:

			return (new CylinderFieldObject);

		case kFieldSphere:

			return (new SphereFieldObject);
	}

	return (nullptr);
}

void FieldObject::PackType(Packer& data) const
{
	Object::PackType(data);
	data << fieldType;
}

void FieldObject::Pack(Packer& data, unsigned_int32 packFlags) const
{
	data << ChunkHeader('DATA', 4);
	data << fieldFlags;

	data << TerminatorChunk;

	PackVolume(data, packFlags);
}

void FieldObject::Unpack(Unpacker& data, unsigned_int32 unpackFlags)
{
	UnpackChunkList<FieldObject>(data, unpackFlags);
	UnpackVolume(data, unpackFlags);
}

bool FieldObject::UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags)
{
	switch (chunkHeader->chunkType)
	{
		case 'DATA':

			data >> fieldFlags;
			return (true);
	}

	return (false);
}

int32 FieldObject::GetCategoryCount(void) const
{
	return (1);
}

Type FieldObject::GetCategoryType(int32 index, const char **title) const
{
	if (index == 0)
	{
		*title = TheInterfaceMgr->GetStringTable()->GetString(StringID(kObjectField));
		return (kObjectField);
	}

	return (0);
}

int32 FieldObject::GetCategorySettingCount(Type category) const
{
	if (category == kObjectField)
	{
		return (2);
	}

	return (0);
} 

Setting *FieldObject::GetCategorySetting(Type category, int32 index, unsigned_int32 flags) const
{ 
	if (category == kObjectField)
	{ 
		const StringTable *table = TheInterfaceMgr->GetStringTable();

		if (index == 0) 
		{
			const char *title = table->GetString(StringID(kObjectField, 'FELD')); 
			return (new HeadingSetting(kObjectField, title)); 
		}

		if (index == 1)
		{ 
			const char *title = table->GetString(StringID(kObjectField, 'FELD', 'EXCL'));
			return (new BooleanSetting('EXCL', ((fieldFlags & kFieldExclusive) != 0), title));
		}
	}

	return (nullptr);
}

void FieldObject::SetCategorySetting(Type category, const Setting *setting)
{
	if (category == kObjectField)
	{
		Type identifier = setting->GetSettingIdentifier();

		if (identifier == 'EXCL')
		{
			if (static_cast<const BooleanSetting *>(setting)->GetBooleanValue())
			{
				fieldFlags |= kFieldExclusive;
			}
			else
			{
				fieldFlags &= ~kFieldExclusive;
			}
		}
	}
}

int32 FieldObject::GetObjectSize(float *size) const
{
	return (GetVolumeObjectSize(size));
}

void FieldObject::SetObjectSize(const float *size)
{
	SetVolumeObjectSize(size);
}

Antivector4D FieldObject::CalculateSurfacePlane(void) const
{
	return (Antivector4D(0.0F, 0.0F, -1.0F, 0.0F));
}


BoxFieldObject::BoxFieldObject() : FieldObject(kFieldBox, this)
{
}

BoxFieldObject::BoxFieldObject(const Vector3D& size) :
		FieldObject(kFieldBox, this),
		BoxVolume(size)
{
}

BoxFieldObject::~BoxFieldObject()
{
}

Antivector4D BoxFieldObject::CalculateSurfacePlane(void) const
{
	return (Antivector4D(0.0F, 0.0F, -1.0F, GetBoxSize().z));
}


CylinderFieldObject::CylinderFieldObject() : FieldObject(kFieldCylinder, this)
{
}

CylinderFieldObject::CylinderFieldObject(const Vector2D& size, float height) :
		FieldObject(kFieldCylinder, this),
		CylinderVolume(size, height)
{
}

CylinderFieldObject::~CylinderFieldObject()
{
}

Antivector4D CylinderFieldObject::CalculateSurfacePlane(void) const
{
	return (Antivector4D(0.0F, 0.0F, -1.0F, GetCylinderHeight()));
}


SphereFieldObject::SphereFieldObject() : FieldObject(kFieldSphere, this)
{
}

SphereFieldObject::SphereFieldObject(const Vector3D& size) :
		FieldObject(kFieldSphere, this),
		SphereVolume(size)
{
}

SphereFieldObject::~SphereFieldObject()
{
}


Field::Field(FieldType type) : Node(kNodeField)
{
	fieldType = type;
	fieldForce = nullptr;

	queryThreadFlags = 0;
}

Field::Field(const Field& field) : Node(field)
{
	fieldType = field.fieldType;
	fieldForce = nullptr;

	const Force *force = field.fieldForce;
	if (force)
	{
		SetForce(force->Clone());
	}

	queryThreadFlags = 0;
}

Field::~Field()
{
	delete fieldForce;
}

Field *Field::Create(Unpacker& data, unsigned_int32 unpackFlags)
{
	switch (data.GetType())
	{
		case kFieldBox:

			return (new BoxField);

		case kFieldCylinder:

			return (new CylinderField);

		case kFieldSphere:

			return (new SphereField);
	}

	return (nullptr);
}

void Field::PackType(Packer& data) const
{
	Node::PackType(data);
	data << fieldType;
}

void Field::Pack(Packer& data, unsigned_int32 packFlags) const
{
	Node::Pack(data, packFlags);

	if (fieldForce)
	{
		PackHandle handle = data.BeginChunk('FORC');
		fieldForce->PackType(data);
		fieldForce->Pack(data, packFlags);
		data.EndChunk(handle);
	}

	data << TerminatorChunk;
}

void Field::Unpack(Unpacker& data, unsigned_int32 unpackFlags)
{
	Node::Unpack(data, unpackFlags);
	UnpackChunkList<Field>(data, unpackFlags);
}

bool Field::UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags)
{
	switch (chunkHeader->chunkType)
	{
		case 'FORC':

			Force *force = Force::Create(data, unpackFlags);
			if (force)
			{
				force->Unpack(++data, unpackFlags);
				SetForce(force);
				return (true);
			}

			break;
	}

	return (false);
}

void *Field::BeginSettingsUnpack(void)
{
	delete fieldForce;
	fieldForce = nullptr;

	return (Node::BeginSettingsUnpack());
}

void Field::HandleTransformUpdate(void)
{
	Node::HandleTransformUpdate();

	surfacePlane = GetObject()->CalculateSurfacePlane() * GetInverseWorldTransform();
}

void Field::HandleVisibilityUpdate(void)
{
	PurgeVisibility();
	GetOwningZone()->InsertZoneTreeSite(kCellGraphField, this, GetMaxSubzoneDepth(), GetForcedSubzoneDepth());
}

bool Field::CalculateBoundingBox(Box3D *box) const
{
	GetObject()->CalculateBoundingBox(box);
	return (true);
}

void Field::Preprocess(void)
{
	if (fieldForce)
	{
		fieldForce->Preprocess();

		if (!(GetObject()->GetFieldFlags() & kFieldExclusive))
		{
			SetActiveUpdateFlags(GetActiveUpdateFlags() | kUpdateVisibility);
		}
	}

	Node::Preprocess();
}

void Field::Neutralize(void)
{
	DismantleVisibility();
	Node::Neutralize();
}

void Field::SetForce(Force *force)
{
	if (fieldForce != force)
	{
		if (fieldForce)
		{
			fieldForce->SetTargetField(nullptr);
		}

		if (force)
		{
			force->SetTargetField(this);
		}

		fieldForce = force;
	}
}


BoxField::BoxField() : Field(kFieldBox)
{
}

BoxField::BoxField(const Vector3D& size) : Field(kFieldBox)
{
	SetNewObject(new BoxFieldObject(size));
}

BoxField::BoxField(const BoxField& boxField) : Field(boxField)
{
}

BoxField::~BoxField()
{
}

Node *BoxField::Replicate(void) const
{
	return (new BoxField(*this));
}


CylinderField::CylinderField() : Field(kFieldCylinder)
{
}

CylinderField::CylinderField(const Vector2D& size, float height) : Field(kFieldCylinder)
{
	SetNewObject(new CylinderFieldObject(size, height));
}

CylinderField::CylinderField(const CylinderField& cylinderField) : Field(cylinderField)
{
}

CylinderField::~CylinderField()
{
}

Node *CylinderField::Replicate(void) const
{
	return (new CylinderField(*this));
}


SphereField::SphereField() : Field(kFieldSphere)
{
}

SphereField::SphereField(const Vector3D& size) : Field(kFieldSphere)
{
	SetNewObject(new SphereFieldObject(size));
}

SphereField::SphereField(const SphereField& sphereField) : Field(sphereField)
{
}

SphereField::~SphereField()
{
}

Node *SphereField::Replicate(void) const
{
	return (new SphereField(*this));
}

// ZYUQURM
