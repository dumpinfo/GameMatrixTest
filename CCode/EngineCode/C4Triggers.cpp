 

#include "C4Triggers.h"
#include "C4Zones.h"
#include "C4Configuration.h"


using namespace C4;


TriggerObject::TriggerObject(TriggerType type, Volume *volume) :
		Object(kObjectTrigger),
		VolumeObject(volume)
{
	triggerType = type;
	triggerFlags = kTriggerActivateDisable;

	activationConnectorKey[0] = 0;
	deactivationConnectorKey[0] = 0;
}

TriggerObject::~TriggerObject()
{
}

TriggerObject *TriggerObject::Create(Unpacker& data, unsigned_int32 unpackFlags)
{
	switch (data.GetType())
	{
		case kTriggerBox:

			return (new BoxTriggerObject);

		case kTriggerCylinder:

			return (new CylinderTriggerObject);

		case kTriggerSphere:

			return (new SphereTriggerObject);
	}

	return (nullptr);
}

void TriggerObject::PackType(Packer& data) const
{
	Object::PackType(data);
	data << triggerType;
}

void TriggerObject::Pack(Packer& data, unsigned_int32 packFlags) const
{
	data << ChunkHeader('FLAG', 4);
	data << triggerFlags;

	if (activationConnectorKey[0] != 0)
	{
		PackHandle handle = data.BeginChunk('ACON');
		data << activationConnectorKey;
		data.EndChunk(handle);
	}

	if (deactivationConnectorKey[0] != 0)
	{
		PackHandle handle = data.BeginChunk('DCON');
		data << deactivationConnectorKey;
		data.EndChunk(handle);
	}

	data << TerminatorChunk;

	PackVolume(data, packFlags);
}

void TriggerObject::Unpack(Unpacker& data, unsigned_int32 unpackFlags)
{
	UnpackChunkList<TriggerObject>(data, unpackFlags);
	UnpackVolume(data, unpackFlags);
}

bool TriggerObject::UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags)
{
	switch (chunkHeader->chunkType)
	{
		case 'FLAG':

			data >> triggerFlags;
			return (true);

		case 'ACON':

			data >> activationConnectorKey;
			return (true);

		case 'DCON':

			data >> deactivationConnectorKey;
			return (true);
	}

	return (false);
} 

void *TriggerObject::BeginSettingsUnpack(void)
{ 
	activationConnectorKey[0] = 0;
	deactivationConnectorKey[0] = 0; 
	return (nullptr);
}
 
int32 TriggerObject::GetCategoryCount(void) const
{ 
	return (1); 
}

Type TriggerObject::GetCategoryType(int32 index, const char **title) const
{ 
	if (index == 0)
	{
		*title = TheInterfaceMgr->GetStringTable()->GetString(StringID(kObjectTrigger));
		return (kObjectTrigger);
	}

	return (0);
}

int32 TriggerObject::GetCategorySettingCount(Type category) const
{
	if (category == kObjectTrigger)
	{
		return (5);
	}

	return (0);
}

Setting *TriggerObject::GetCategorySetting(Type category, int32 index, unsigned_int32 flags) const
{
	if (category == kObjectTrigger)
	{
		const StringTable *table = TheInterfaceMgr->GetStringTable();

		if (index == 0)
		{
			const char *title = table->GetString(StringID(kObjectTrigger, 'TRIG'));
			return (new HeadingSetting(kObjectTrigger, title));
		}

		if (index == 1)
		{
			const char *title = table->GetString(StringID(kObjectTrigger, 'TRIG', 'ONCE'));
			return (new BooleanSetting('ONCE', ((triggerFlags & kTriggerActivateDisable) != 0), title));
		}

		if (index == 2)
		{
			const char *title = table->GetString(StringID(kObjectTrigger, 'TRIG', 'CONT'));
			return (new BooleanSetting('CONT', ((triggerFlags & kTriggerContinuouslyActivated) != 0), title));
		}

		if (index == 3)
		{
			const char *title = table->GetString(StringID(kObjectTrigger, 'TRIG', 'ACON'));
			return (new TextSetting('ACON', activationConnectorKey, title, kMaxConnectorKeyLength, &Connector::ConnectorKeyFilter));
		}

		if (index == 4)
		{
			const char *title = table->GetString(StringID(kObjectTrigger, 'TRIG', 'DCON'));
			return (new TextSetting('DCON', deactivationConnectorKey, title, kMaxConnectorKeyLength, &Connector::ConnectorKeyFilter));
		}
	}

	return (nullptr);
}

void TriggerObject::SetCategorySetting(Type category, const Setting *setting)
{
	if (category == kObjectTrigger)
	{
		Type identifier = setting->GetSettingIdentifier();

		if (identifier == 'ONCE')
		{
			if (static_cast<const BooleanSetting *>(setting)->GetBooleanValue())
			{
				triggerFlags |= kTriggerActivateDisable;
			}
			else
			{
				triggerFlags &= ~kTriggerActivateDisable;
			}
		}
		else if (identifier == 'CONT')
		{
			if (static_cast<const BooleanSetting *>(setting)->GetBooleanValue())
			{
				triggerFlags |= kTriggerContinuouslyActivated;
			}
			else
			{
				triggerFlags &= ~kTriggerContinuouslyActivated;
			}
		}
		else if (identifier == 'ACON')
		{
			activationConnectorKey = static_cast<const TextSetting *>(setting)->GetText();
		}
		else if (identifier == 'DCON')
		{
			deactivationConnectorKey = static_cast<const TextSetting *>(setting)->GetText();
		}
	}
}

int32 TriggerObject::GetObjectSize(float *size) const
{
	return (GetVolumeObjectSize(size));
}

void TriggerObject::SetObjectSize(const float *size)
{
	SetVolumeObjectSize(size);
}

bool TriggerObject::IntersectSegment(const Point3D& p1, const Point3D& p2, float radius) const
{
	return (false);
}


BoxTriggerObject::BoxTriggerObject() : TriggerObject(kTriggerBox, this)
{
}

BoxTriggerObject::BoxTriggerObject(const Vector3D& size) :
		TriggerObject(kTriggerBox, this),
		BoxVolume(size)
{
}

BoxTriggerObject::~BoxTriggerObject()
{
}

bool BoxTriggerObject::IntersectSegment(const Point3D& p1, const Point3D& p2, float radius) const
{
	const Vector3D& boxSize = GetBoxSize();
	float sx = boxSize.x + radius;
	float sy = boxSize.y + radius;
	float sz = boxSize.z + radius;

	if ((p1.x > -radius) && (p1.x < sx) && (p1.y > -radius) && (p1.y < sy) && (p1.z > -radius) && (p1.z < sz))
	{
		return (true);
	}

	if ((p2.x > -radius) && (p2.x < sx) && (p2.y > -radius) && (p2.y < sy) && (p2.z > -radius) && (p2.z < sz))
	{
		return (true);
	}

	Point3D q1 = p1;
	Point3D q2 = p2;

	float d1 = q1.z + radius;
	float d2 = q2.z + radius;

	if (d1 < 0.0F)
	{
		if (d2 < 0.0F)
		{
			return (false);
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

	d1 = sz - q1.z;
	d2 = sz - q2.z;

	if (d1 < 0.0F)
	{
		if (d2 < 0.0F)
		{
			return (false);
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
			return (false);
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

	d1 = sy - q1.y;
	d2 = sy - q2.y;

	if (d1 < 0.0F)
	{
		if (d2 < 0.0F)
		{
			return (false);
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
			return (false);
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

	d1 = sx - q1.x;
	d2 = sx - q2.x;

	if (d1 < 0.0F)
	{
		if (d2 < 0.0F)
		{
			return (false);
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

	return (true);
}


CylinderTriggerObject::CylinderTriggerObject() : TriggerObject(kTriggerCylinder, this)
{
}

CylinderTriggerObject::CylinderTriggerObject(const Vector2D& size, float height) :
		TriggerObject(kTriggerCylinder, this),
		CylinderVolume(size, height)
{
}

CylinderTriggerObject::~CylinderTriggerObject()
{
}

bool CylinderTriggerObject::IntersectSegment(const Point3D& p1, const Point3D& p2, float radius) const
{
	const Vector2D& cylinderSize = GetCylinderSize();
	float rx = cylinderSize.x + radius;
	float ry = cylinderSize.y + radius;
	float h = GetCylinderHeight() + radius;
	float m2 = rx / ry;
	m2 *= m2;

	if ((p1.z > -radius) && (p1.z < h) && (p1.x * p1.x + m2 * p1.y * p1.y < rx * rx))
	{
		return (true);
	}

	if ((p2.z > -radius) && (p2.z < h) && (p2.x * p2.x + m2 * p2.y * p2.y < rx * rx))
	{
		return (true);
	}

	float sx = p1.x;
	float sy = p1.y;
	float dx = p2.x - sx;
	float dy = p2.y - sy;

	float a = dx * dx + m2 * dy * dy;
	if (a > K::min_float)
	{
		float b = -(sx * dx + m2 * sy * dy);
		float d = b * b - a * (sx * sx + m2 * sy * sy - rx * rx);
		if (d < K::min_float)
		{
			return (false);
		}

		a = 1.0F / a;
		d = Sqrt(d);
		float t1 = (b - d) * a;
		float t2 = (b + d) * a;
		if ((Fmax(t1, t2) < 0.0F) || (Fmin(t1, t2) > 1.0F))
		{
			return (false);
		}

		float sz = p1.z;
		float dz = p2.z - sz;

		float z1 = sz + dz * Saturate(t1);
		float z2 = sz + dz * Saturate(t2);
		if ((Fmax(z1, z2) < -radius) || (Fmin(z1, z2) > h))
		{
			return (false);
		}

		return (true);
	}

	float z1 = p1.z;
	float z2 = p2.z;
	if ((Fmax(z1, z2) < -radius) || (Fmin(z1, z2) > h))
	{
		return (false);
	}

	return (sx * sx + m2 * sy * sy < rx * rx);
}


SphereTriggerObject::SphereTriggerObject() : TriggerObject(kTriggerSphere, this)
{
}

SphereTriggerObject::SphereTriggerObject(const Vector3D& size) :
		TriggerObject(kTriggerSphere, this),
		SphereVolume(size)
{
}

SphereTriggerObject::~SphereTriggerObject()
{
}

bool SphereTriggerObject::IntersectSegment(const Point3D& p1, const Point3D& p2, float radius) const
{
	const Vector3D& sphereSize = GetSphereSize();
	float rx = sphereSize.x + radius;
	float ry = sphereSize.y + radius;
	float rz = sphereSize.z + radius;
	float m2 = rx / ry;
	float n2 = rx / rz;
	m2 *= m2;
	n2 *= n2;

	if (p1.x * p1.x + m2 * p1.y * p1.y + n2 * p1.z * p1.z < rx * rx)
	{
		return (true);
	}

	if (p2.x * p2.x + m2 * p2.y * p2.y + n2 * p2.z * p2.z < rx * rx)
	{
		return (true);
	}

	float sx = p1.x;
	float sy = p1.y;
	float sz = p1.z;

	float dx = p2.x - sx;
	float dy = p2.y - sy;
	float dz = p2.z - sz;

	float m = dx * dx + dy * dy + dz * dz;
	if (m > K::min_float)
	{
		m = InverseSqrt(m);
		dx *= m;
		dy *= m;
		dz *= m;

		float a = dx * dx + m2 * dy * dy + n2 * dz * dz;
		float b = sx * dx + m2 * sy * dy + n2 * sz * dz;
		float c = sx * sx + m2 * sy * sy + n2 * sz * sz - rx * rx;

		float d = b * b - a * c;
		if (d > K::min_float)
		{
			d = Sqrt(d);
			a = m / a;
			b = -b;

			float t1 = (b + d) * a;
			float t2 = (b - d) * a;

			return ((Fmax(t1, t2) > 0.0F) && (Fmin(t1, t2) < 1.0F));
		}
	}

	return (false);
}


Trigger::Trigger(TriggerType type) : Node(kNodeTrigger)
{
	triggerType = type;
}

Trigger::Trigger(const Trigger& trigger) : Node(trigger)
{
	triggerType = trigger.triggerType;
}

Trigger::~Trigger()
{
}

Trigger *Trigger::Create(Unpacker& data, unsigned_int32 unpackFlags)
{
	switch (data.GetType())
	{
		case kTriggerBox:

			return (new BoxTrigger);

		case kTriggerCylinder:

			return (new CylinderTrigger);

		case kTriggerSphere:

			return (new SphereTrigger);
	}

	return (nullptr);
}

void Trigger::PackType(Packer& data) const
{
	Node::PackType(data);
	data << triggerType;
}

void Trigger::Pack(Packer& data, unsigned_int32 packFlags) const
{
	Node::Pack(data, packFlags);

	data << TerminatorChunk;
}

void Trigger::Unpack(Unpacker& data, unsigned_int32 unpackFlags)
{
	Node::Unpack(data, unpackFlags);
	UnpackChunkList<Trigger>(data, unpackFlags);
}

bool Trigger::UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags)
{
	return (false);
}

bool Trigger::CalculateBoundingBox(Box3D *box) const
{
	GetObject()->CalculateBoundingBox(box);
	return (true);
}

void Trigger::HandleVisibilityUpdate(void)
{
	PurgeVisibility();
	GetOwningZone()->InsertZoneTreeSite(kCellGraphTrigger, this, GetMaxSubzoneDepth(), GetForcedSubzoneDepth());
}

void Trigger::Preprocess(void)
{
	SetActiveUpdateFlags(GetActiveUpdateFlags() | kUpdateVisibility);
	Node::Preprocess();
}

void Trigger::Neutralize(void)
{
	ListElement<Trigger>::Detach();
	Node::Neutralize();
}

void Trigger::Activate(Node *initiator)
{
	Controller *controller = GetController();
	if (controller)
	{
		controller->Activate(initiator, this);
	}
	else
	{
		Node *node = GetConnectedNode(GetObject()->GetActivationConnectorKey());
		if (node)
		{
			controller = node->GetController();
			if (controller)
			{
				controller->Activate(initiator, this);
			}
			else
			{
				node = node->GetFirstSubnode();
				while (node)
				{
					controller = node->GetController();
					if (controller)
					{
						controller->Activate(initiator, this);
					}

					node = node->Next();
				}
			}
		}
	}
}

void Trigger::Deactivate(void)
{
	Controller *controller = GetController();
	if (controller)
	{
		controller->Deactivate(nullptr, this);
	}
	else
	{
		Node *node = GetConnectedNode(GetObject()->GetDeactivationConnectorKey());
		if (node)
		{
			controller = node->GetController();
			if (controller)
			{
				controller->Deactivate(nullptr, this);
			}
			else
			{
				node = node->GetFirstSubnode();
				while (node)
				{
					controller = node->GetController();
					if (controller)
					{
						controller->Deactivate(nullptr, this);
					}

					node = node->Next();
				}
			}
		}
	}
}


BoxTrigger::BoxTrigger() : Trigger(kTriggerBox)
{
}

BoxTrigger::BoxTrigger(const Vector3D& size) : Trigger(kTriggerBox)
{
	SetNewObject(new BoxTriggerObject(size));
}

BoxTrigger::BoxTrigger(const BoxTrigger& boxTrigger) : Trigger(boxTrigger)
{
}

BoxTrigger::~BoxTrigger()
{
}

Node *BoxTrigger::Replicate(void) const
{
	return (new BoxTrigger(*this));
}


CylinderTrigger::CylinderTrigger() : Trigger(kTriggerCylinder)
{
}

CylinderTrigger::CylinderTrigger(const Vector2D& size, float height) : Trigger(kTriggerCylinder)
{
	SetNewObject(new CylinderTriggerObject(size, height));
}

CylinderTrigger::CylinderTrigger(const CylinderTrigger& cylinderTrigger) : Trigger(cylinderTrigger)
{
}

CylinderTrigger::~CylinderTrigger()
{
}

Node *CylinderTrigger::Replicate(void) const
{
	return (new CylinderTrigger(*this));
}


SphereTrigger::SphereTrigger() : Trigger(kTriggerSphere)
{
}

SphereTrigger::SphereTrigger(const Vector3D& size) : Trigger(kTriggerSphere)
{
	SetNewObject(new SphereTriggerObject(size));
}

SphereTrigger::SphereTrigger(const SphereTrigger& sphereTrigger) : Trigger(sphereTrigger)
{
}

SphereTrigger::~SphereTrigger()
{
}

Node *SphereTrigger::Replicate(void) const
{
	return (new SphereTrigger(*this));
}

// ZYUQURM
