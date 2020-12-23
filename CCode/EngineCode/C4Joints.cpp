 

#include "C4Joints.h"
#include "C4Simulation.h"
#include "C4World.h"
#include "C4Configuration.h"


using namespace C4;


const char C4::kConnectorKeyBody1[] = "%Body1";
const char C4::kConnectorKeyBody2[] = "%Body2";


JointObject::JointObject(JointType type) : Object(kObjectJoint)
{
	jointType = type;
	jointFlags = 0;

	solverMultiplier = 1;
	breakingForce = 1.0F;
	distanceLimit = 1.0F;
	angleLimit = 45.0F;
}

JointObject::~JointObject()
{
}

JointObject *JointObject::Create(Unpacker& data, unsigned_int32 unpackFlags)
{
	switch (data.GetType())
	{
		case kJointSpherical:

			return (new SphericalJointObject);

		case kJointUniversal:

			return (new UniversalJointObject);

		case kJointDiscal:

			return (new DiscalJointObject);

		case kJointRevolute:

			return (new RevoluteJointObject);

		case kJointCylindrical:

			return (new CylindricalJointObject);

		case kJointPrismatic:

			return (new PrismaticJointObject);
	}

	return (nullptr);
}

void JointObject::PackType(Packer& data) const
{
	Object::PackType(data);
	data << jointType;
}

void JointObject::Pack(Packer& data, unsigned_int32 packFlags) const
{
	data << ChunkHeader('FLAG', 4);
	data << jointFlags;

	data << ChunkHeader('SLVM', 4);
	data << solverMultiplier;

	data << ChunkHeader('BFRC', 4);
	data << breakingForce;

	data << ChunkHeader('DLIM', 4);
	data << distanceLimit;

	data << ChunkHeader('ALIM', 4);
	data << angleLimit;

	data << TerminatorChunk;
}

void JointObject::Unpack(Unpacker& data, unsigned_int32 unpackFlags)
{
	UnpackChunkList<JointObject>(data, unpackFlags);
}

bool JointObject::UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags)
{
	switch (chunkHeader->chunkType)
	{
		case 'FLAG':

			data >> jointFlags;
			return (true);

		case 'SLVM': 

			data >> solverMultiplier;
			return (true); 

		case 'BFRC': 

			data >> breakingForce;
			return (true); 

		case 'DLIM': 
 
			data >> distanceLimit;
			return (true);

		case 'ALIM': 

			data >> angleLimit;
			return (true);
	}

	return (false);
}

int32 JointObject::GetCategoryCount(void) const
{
	return (1);
}

Type JointObject::GetCategoryType(int32 index, const char **title) const
{
	if (index == 0)
	{
		*title = TheInterfaceMgr->GetStringTable()->GetString(StringID(kObjectJoint));
		return (kObjectJoint);
	}

	return (0);
}

int32 JointObject::GetCategorySettingCount(Type category) const
{
	if (category == kObjectJoint)
	{
		return (4);
	}

	return (0);
}

Setting *JointObject::GetCategorySetting(Type category, int32 index, unsigned_int32 flags) const
{
	if (category == kObjectJoint)
	{
		const StringTable *table = TheInterfaceMgr->GetStringTable();

		if (index == 0)
		{
			const char *title = table->GetString(StringID(kObjectJoint, 'JONT'));
			return (new HeadingSetting(kObjectJoint, title));
		}

		if (index == 1)
		{
			const char *title = table->GetString(StringID(kObjectJoint, 'JONT', 'SLVM'));
			return (new IntegerSetting('SLVM', solverMultiplier, title, 1, 5, 1));
		}

		if (index == 2)
		{
			const char *title = table->GetString(StringID(kObjectJoint, 'JONT', 'BRAK'));
			return (new BooleanSetting('BRAK', ((jointFlags & kJointBreakable) != 0), title));
		}

		if (index == 3)
		{
			const char *title = table->GetString(StringID(kObjectJoint, 'JONT', 'BFRC'));
			return (new TextSetting('BFRC', breakingForce, title));
		}
	}

	return (nullptr);
}

void JointObject::SetCategorySetting(Type category, const Setting *setting)
{
	if (category == kObjectJoint)
	{
		Type identifier = setting->GetSettingIdentifier();

		if (identifier == 'SLVM')
		{
			solverMultiplier = static_cast<const IntegerSetting *>(setting)->GetIntegerValue();
		}
		else if (identifier == 'BRAK')
		{
			if (static_cast<const BooleanSetting *>(setting)->GetBooleanValue())
			{
				jointFlags |= kJointBreakable;
			}
			else
			{
				jointFlags &= ~kJointBreakable;
			}
		}
		else if (identifier == 'BFRC')
		{
			breakingForce = FmaxZero(Text::StringToFloat(static_cast<const TextSetting *>(setting)->GetText()));
		}
	}
}


SphericalJointObject::SphericalJointObject() : JointObject(kJointSpherical)
{
}

SphericalJointObject::SphericalJointObject(JointType type) : JointObject(type)
{
}

SphericalJointObject::~SphericalJointObject()
{
}

int32 SphericalJointObject::GetCategorySettingCount(Type category) const
{
	int32 count = JointObject::GetCategorySettingCount(category);
	if (category == kObjectJoint)
	{
		count += 3;
	}

	return (count);
}

Setting *SphericalJointObject::GetCategorySetting(Type category, int32 index, unsigned_int32 flags) const
{
	if (category == kObjectJoint)
	{
		int32 i = index - JointObject::GetCategorySettingCount(kObjectJoint);
		if (i >= 0)
		{
			const StringTable *table = TheInterfaceMgr->GetStringTable();

			if (i == 0)
			{
				const char *title = table->GetString(StringID(kObjectJoint, 'LIMT'));
				return (new HeadingSetting('LIMT', title));
			}

			if (i == 1)
			{
				const char *title = table->GetString(StringID(kObjectJoint, 'LIMT', 'ALIM'));
				return (new BooleanSetting('ALIM', ((GetJointFlags() & kJointLimitAngle) != 0), title));
			}

			if (i == 2)
			{
				const char *title = table->GetString(StringID(kObjectJoint, 'LIMT', 'AMAX'));
				return (new IntegerSetting('AMAX', (int32) GetAngleLimit(), title, 1, 90, 1));
			}

			return (nullptr);
		}
	}

	return (JointObject::GetCategorySetting(category, index, flags));
}

void SphericalJointObject::SetCategorySetting(Type category, const Setting *setting)
{
	if (category == kObjectJoint)
	{
		Type identifier = setting->GetSettingIdentifier();

		if (identifier == 'ALIM')
		{
			if (static_cast<const BooleanSetting *>(setting)->GetBooleanValue())
			{
				SetJointFlags(GetJointFlags() | kJointLimitAngle);
			}
			else
			{
				SetJointFlags(GetJointFlags() & ~kJointLimitAngle);
			}
		}
		else if (identifier == 'AMAX')
		{
			SetAngleLimit((float) static_cast<const IntegerSetting *>(setting)->GetIntegerValue());
		}
		else
		{
			JointObject::SetCategorySetting(kObjectJoint, setting);
		}
	}
	else
	{
		JointObject::SetCategorySetting(category, setting);
	}
}


UniversalJointObject::UniversalJointObject() : SphericalJointObject(kJointUniversal)
{
}

UniversalJointObject::~UniversalJointObject()
{
}

int32 UniversalJointObject::GetCategorySettingCount(Type category) const
{
	int32 count = SphericalJointObject::GetCategorySettingCount(category);
	if (category == kObjectJoint)
	{
		count += 2;
	}

	return (count);
}

Setting *UniversalJointObject::GetCategorySetting(Type category, int32 index, unsigned_int32 flags) const
{
	if (category == kObjectJoint)
	{
		int32 i = index - SphericalJointObject::GetCategorySettingCount(kObjectJoint);
		if (i >= 0)
		{
			const StringTable *table = TheInterfaceMgr->GetStringTable();

			if (i == 0)
			{
				const char *title = table->GetString(StringID(kObjectJoint, kJointUniversal));
				return (new HeadingSetting(kJointUniversal, title));
			}

			if (i == 1)
			{
				const char *title = table->GetString(StringID(kObjectJoint, kJointUniversal, 'TWST'));
				return (new BooleanSetting('TWST', ((GetJointFlags() & kJointConstrainTwistPosition) != 0), title));
			}

			return (nullptr);
		}
	}

	return (SphericalJointObject::GetCategorySetting(category, index, flags));
}

void UniversalJointObject::SetCategorySetting(Type category, const Setting *setting)
{
	if (category == kObjectJoint)
	{
		Type identifier = setting->GetSettingIdentifier();

		if (identifier == 'TWST')
		{
			if (static_cast<const BooleanSetting *>(setting)->GetBooleanValue())
			{
				SetJointFlags(GetJointFlags() | kJointConstrainTwistPosition);
			}
			else
			{
				SetJointFlags(GetJointFlags() & ~kJointConstrainTwistPosition);
			}
		}
		else
		{
			SphericalJointObject::SetCategorySetting(kObjectJoint, setting);
		}
	}
	else
	{
		SphericalJointObject::SetCategorySetting(category, setting);
	}
}


DiscalJointObject::DiscalJointObject() : JointObject(kJointDiscal)
{
}

DiscalJointObject::DiscalJointObject(JointType type) : JointObject(type)
{
}

DiscalJointObject::~DiscalJointObject()
{
}

int32 DiscalJointObject::GetCategorySettingCount(Type category) const
{
	int32 count = JointObject::GetCategorySettingCount(category);
	if (category == kObjectJoint)
	{
		count += 3;
	}

	return (count);
}

Setting *DiscalJointObject::GetCategorySetting(Type category, int32 index, unsigned_int32 flags) const
{
	if (category == kObjectJoint)
	{
		int32 i = index - JointObject::GetCategorySettingCount(kObjectJoint);
		if (i >= 0)
		{
			const StringTable *table = TheInterfaceMgr->GetStringTable();

			if (i == 0)
			{
				const char *title = table->GetString(StringID(kObjectJoint, 'LIMT'));
				return (new HeadingSetting('LIMT', title));
			}

			if (i == 1)
			{
				const char *title = table->GetString(StringID(kObjectJoint, 'LIMT', 'ALIM'));
				return (new BooleanSetting('ALIM', ((GetJointFlags() & kJointLimitAngle) != 0), title));
			}

			if (i == 2)
			{
				const char *title = table->GetString(StringID(kObjectJoint, 'LIMT', 'AMAX'));
				return (new IntegerSetting('AMAX', (int32) GetAngleLimit(), title, 1, 90, 1));
			}

			return (nullptr);
		}
	}

	return (JointObject::GetCategorySetting(category, index, flags));
}

void DiscalJointObject::SetCategorySetting(Type category, const Setting *setting)
{
	if (category == kObjectJoint)
	{
		Type identifier = setting->GetSettingIdentifier();

		if (identifier == 'ALIM')
		{
			if (static_cast<const BooleanSetting *>(setting)->GetBooleanValue())
			{
				SetJointFlags(GetJointFlags() | kJointLimitAngle);
			}
			else
			{
				SetJointFlags(GetJointFlags() & ~kJointLimitAngle);
			}
		}
		else if (identifier == 'AMAX')
		{
			SetAngleLimit((float) static_cast<const IntegerSetting *>(setting)->GetIntegerValue());
		}
		else
		{
			JointObject::SetCategorySetting(kObjectJoint, setting);
		}
	}
	else
	{
		JointObject::SetCategorySetting(category, setting);
	}
}


RevoluteJointObject::RevoluteJointObject() : DiscalJointObject(kJointRevolute)
{
}

RevoluteJointObject::~RevoluteJointObject()
{
}


CylindricalJointObject::CylindricalJointObject() : DiscalJointObject(kJointCylindrical)
{
}

CylindricalJointObject::CylindricalJointObject(JointType type) : DiscalJointObject(type)
{
}

CylindricalJointObject::~CylindricalJointObject()
{
}

int32 CylindricalJointObject::GetCategorySettingCount(Type category) const
{
	int32 count = JointObject::GetCategorySettingCount(category);
	if (category == kObjectJoint)
	{
		count += 5;
	}

	return (count);
}

Setting *CylindricalJointObject::GetCategorySetting(Type category, int32 index, unsigned_int32 flags) const
{
	if (category == kObjectJoint)
	{
		int32 i = index - JointObject::GetCategorySettingCount(kObjectJoint);
		if (i >= 0)
		{
			const StringTable *table = TheInterfaceMgr->GetStringTable();

			if (i == 0)
			{
				const char *title = table->GetString(StringID(kObjectJoint, 'LIMT'));
				return (new HeadingSetting('LIMT', title));
			}

			if (i == 1)
			{
				const char *title = table->GetString(StringID(kObjectJoint, 'LIMT', 'DLIM'));
				return (new BooleanSetting('DLIM', ((GetJointFlags() & kJointLimitDistance) != 0), title));
			}

			if (i == 2)
			{
				const char *title = table->GetString(StringID(kObjectJoint, 'LIMT', 'DMAX'));
				return (new TextSetting('DMAX', GetDistanceLimit(), title));
			}

			if (i == 3)
			{
				const char *title = table->GetString(StringID(kObjectJoint, 'LIMT', 'ALIM'));
				return (new BooleanSetting('ALIM', ((GetJointFlags() & kJointLimitAngle) != 0), title));
			}

			if (i == 4)
			{
				const char *title = table->GetString(StringID(kObjectJoint, 'LIMT', 'AMAX'));
				return (new IntegerSetting('AMAX', (int32) GetAngleLimit(), title, 1, 90, 1));
			}

			return (nullptr);
		}
	}

	return (JointObject::GetCategorySetting(category, index, flags));
}

void CylindricalJointObject::SetCategorySetting(Type category, const Setting *setting)
{
	if (category == kObjectJoint)
	{
		Type identifier = setting->GetSettingIdentifier();

		if (identifier == 'DLIM')
		{
			if (static_cast<const BooleanSetting *>(setting)->GetBooleanValue())
			{
				SetJointFlags(GetJointFlags() | kJointLimitDistance);
			}
			else
			{
				SetJointFlags(GetJointFlags() & ~kJointLimitDistance);
			}
		}
		else if (identifier == 'DMAX')
		{
			SetDistanceLimit(FmaxZero(Text::StringToFloat(static_cast<const TextSetting *>(setting)->GetText())));
		}
		else if (identifier == 'ALIM')
		{
			if (static_cast<const BooleanSetting *>(setting)->GetBooleanValue())
			{
				SetJointFlags(GetJointFlags() | kJointLimitAngle);
			}
			else
			{
				SetJointFlags(GetJointFlags() & ~kJointLimitAngle);
			}
		}
		else if (identifier == 'AMAX')
		{
			SetAngleLimit((float) static_cast<const IntegerSetting *>(setting)->GetIntegerValue());
		}
		else
		{
			JointObject::SetCategorySetting(kObjectJoint, setting);
		}
	}
	else
	{
		JointObject::SetCategorySetting(category, setting);
	}
}


PrismaticJointObject::PrismaticJointObject() : CylindricalJointObject(kJointPrismatic)
{
}

PrismaticJointObject::~PrismaticJointObject()
{
}

int32 PrismaticJointObject::GetCategorySettingCount(Type category) const
{
	int32 count = JointObject::GetCategorySettingCount(category);
	if (category == kObjectJoint)
	{
		count += 3;
	}

	return (count);
}

Setting *PrismaticJointObject::GetCategorySetting(Type category, int32 index, unsigned_int32 flags) const
{
	if (category == kObjectJoint)
	{
		int32 i = index - JointObject::GetCategorySettingCount(kObjectJoint);
		if (i >= 0)
		{
			const StringTable *table = TheInterfaceMgr->GetStringTable();

			if (i == 0)
			{
				const char *title = table->GetString(StringID(kObjectJoint, 'LIMT'));
				return (new HeadingSetting('LIMT', title));
			}

			if (i == 1)
			{
				const char *title = table->GetString(StringID(kObjectJoint, 'LIMT', 'DLIM'));
				return (new BooleanSetting('DLIM', ((GetJointFlags() & kJointLimitDistance) != 0), title));
			}

			if (i == 2)
			{
				const char *title = table->GetString(StringID(kObjectJoint, 'LIMT', 'DMAX'));
				return (new TextSetting('DMAX', GetDistanceLimit(), title));
			}

			return (nullptr);
		}
	}

	return (JointObject::GetCategorySetting(category, index, flags));
}

void PrismaticJointObject::SetCategorySetting(Type category, const Setting *setting)
{
	if (category == kObjectJoint)
	{
		Type identifier = setting->GetSettingIdentifier();

		if (identifier == 'DLIM')
		{
			if (static_cast<const BooleanSetting *>(setting)->GetBooleanValue())
			{
				SetJointFlags(GetJointFlags() | kJointLimitDistance);
			}
			else
			{
				SetJointFlags(GetJointFlags() & ~kJointLimitDistance);
			}
		}
		else if (identifier == 'DMAX')
		{
			SetDistanceLimit(FmaxZero(Text::StringToFloat(static_cast<const TextSetting *>(setting)->GetText())));
		}
		else
		{
			JointObject::SetCategorySetting(kObjectJoint, setting);
		}
	}
	else
	{
		JointObject::SetCategorySetting(category, setting);
	}
}


Joint::Joint(JointType type) : Node(kNodeJoint)
{
	jointType = type;
	initializedFlag = false;
}

Joint::Joint(const Joint& joint) : Node(joint)
{
	jointType = joint.jointType;
	initializedFlag = false;
}

Joint::~Joint()
{
	JointContact *contact = jointContact;
	delete contact;
}

Joint *Joint::Create(Unpacker& data, unsigned_int32 unpackFlags)
{
	switch (data.GetType())
	{
		case kJointSpherical:

			return (new SphericalJoint(nullptr));

		case kJointUniversal:

			return (new UniversalJoint(nullptr));

		case kJointDiscal:

			return (new DiscalJoint(nullptr));

		case kJointRevolute:

			return (new RevoluteJoint(nullptr));

		case kJointCylindrical:

			return (new CylindricalJoint(nullptr));

		case kJointPrismatic:

			return (new PrismaticJoint(nullptr));
	}

	return (nullptr);
}

void Joint::PackType(Packer& data) const
{
	Node::PackType(data);
	data << jointType;
}

void Joint::Pack(Packer& data, unsigned_int32 packFlags) const
{
	Node::Pack(data, packFlags);

	data << ChunkHeader('INIT', 4);
	data << initializedFlag;

	JointContact *contact = jointContact;
	if (contact)
	{
		data << ChunkHeader('JNT1', sizeof(Transform4D));
		data << bodyJointTransform[0];

		if (contact->GetFinishElement()->GetBodyType() != kBodyNull)
		{
			data << ChunkHeader('JNT2', sizeof(Transform4D));
			data << bodyJointTransform[1];
		}
	}

	data << TerminatorChunk;
}

void Joint::Unpack(Unpacker& data, unsigned_int32 unpackFlags)
{
	Node::Unpack(data, unpackFlags);
	UnpackChunkList<Joint>(data, unpackFlags);
}

bool Joint::UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags)
{
	switch (chunkHeader->chunkType)
	{
		case 'INIT':

			data >> initializedFlag;
			return (true);

		case 'JNT1':

			data >> bodyJointTransform[0];
			return (true);

		case 'JNT2':

			data >> bodyJointTransform[1];
			return (true);
	}

	return (false);
}

int32 Joint::GetInternalConnectorCount(void) const
{
	return (2);
}

const char *Joint::GetInternalConnectorKey(int32 index) const
{
	if (index == 0)
	{
		return (kConnectorKeyBody1);
	}
	else if (index == 1)
	{
		return (kConnectorKeyBody2);
	}

	return (nullptr);
}

bool Joint::ValidConnectedNode(const ConnectorKey& key, const Node *node) const
{
	if ((key == kConnectorKeyBody1) || (key == kConnectorKeyBody2))
	{
		Controller *controller = node->GetController();
		return ((controller) && (controller->GetBaseControllerType() == kControllerRigidBody));
	}

	return (Node::ValidConnectedNode(key, node));
}

void Joint::SetFirstConnectedRigidBody(Node *node)
{
	if (node)
	{
		Hub *hub = GetHub();
		if (hub)
		{
			Connector *connector = hub->FindOutgoingConnector(kConnectorKeyBody1);
			if (connector)
			{
				connector->SetConnectorTarget(node);
				return;
			}
		}

		AddConnector(kConnectorKeyBody1, node);
	}
}

void Joint::SetSecondConnectedRigidBody(Node *node)
{
	if (node)
	{
		Hub *hub = GetHub();
		if (hub)
		{
			Connector *connector = hub->FindOutgoingConnector(kConnectorKeyBody2);
			if (connector)
			{
				connector->SetConnectorTarget(node);
				return;
			}
		}

		AddConnector(kConnectorKeyBody2, node);
	}
}

void Joint::Preprocess(void)
{
	Node::Preprocess();

	if (!GetManipulator())
	{
		physicsController = GetWorld()->FindPhysicsController();
		if (physicsController)
		{
			const JointObject *object = GetObject();
			unsigned_int32 flags = object->GetJointFlags();

			if (flags & kJointBreakable)
			{
				breakingImpulse = object->GetBreakingForce() * kTimeStep;
			}
			else
			{
				breakingImpulse = K::infinity;
			}

			distanceLimit = object->GetDistanceLimit();
			angleLimitCosSin = CosSin(object->GetAngleLimit() * K::radians);

			constrainTwistFlag = ((flags & kJointConstrainTwistPosition) != 0);
			limitDistanceFlag = ((flags & kJointLimitDistance) != 0);
			limitAngleFlag = ((flags & kJointLimitAngle) != 0);

			if (!initializedFlag)
			{
				initializedFlag = true;

				JointContact *contact = jointContact;
				if (!contact)
				{
					RigidBodyController *rigidBody1 = GetRigidBody(GetFirstConnectedRigidBody(), &bodyJointTransform[0]);
					RigidBodyController *rigidBody2 = GetRigidBody(GetSecondConnectedRigidBody(), &bodyJointTransform[1]);

					if (rigidBody1)
					{
						if (rigidBody2)
						{
							jointContact = new JointContact(this, rigidBody1, rigidBody2);
						}
						else
						{
							jointContact = new StaticJointContact(this, rigidBody1, physicsController->GetNullBody());
						}
					}
					else if (rigidBody2)
					{
						bodyJointTransform[0] = bodyJointTransform[1];

						jointContact = new StaticJointContact(this, rigidBody2, physicsController->GetNullBody());
					}
				}
			}
		}
	}
}

void Joint::Neutralize(void)
{
	Node::Neutralize();

	JointContact *contact = jointContact;
	delete contact;

	initializedFlag = false;
}

RigidBodyController *Joint::GetRigidBody(const Node *node, Transform4D *jointTransform) const
{
	if (node)
	{
		Controller *controller = node->GetController();
		if ((controller) && (controller->GetBaseControllerType() == kControllerRigidBody))
		{
			*jointTransform = node->GetInverseWorldTransform() * GetWorldTransform();
			return (static_cast<RigidBodyController *>(controller));
		}
	}

	return (nullptr);
}

void Joint::HandleBrokenJoint(void)
{
	breakingImpulse = K::infinity;

	if (TheMessageMgr->Server())
	{
		physicsController->AddBrokenJoint(this);
	}
}

void Joint::BreakJoint(void)
{
	JointContact *contact = jointContact;
	delete contact;

	initializedFlag = false;

	Hub *hub = GetHub();
	if (hub)
	{
		Connector *connector = hub->FindOutgoingConnector(kConnectorKeyBody1);
		if (connector)
		{
			Hub *finish = connector->GetFinishElement();
			if (finish != hub)
			{
				Controller *controller = finish->GetNode()->GetController();
				if ((controller) && (controller->GetBaseControllerType() == kControllerRigidBody))
				{
					static_cast<RigidBodyController *>(controller)->RecursiveWake();
				}

				connector->SetConnectorTarget(nullptr);
			}
		}

		connector = hub->FindOutgoingConnector(kConnectorKeyBody2);
		if (connector)
		{
			Hub *finish = connector->GetFinishElement();
			if (finish != hub)
			{
				Controller *controller = finish->GetNode()->GetController();
				if ((controller) && (controller->GetBaseControllerType() == kControllerRigidBody))
				{
					static_cast<RigidBodyController *>(controller)->RecursiveWake();
				}

				connector->SetConnectorTarget(nullptr);
			}
		}
	}
}


SphericalJoint::SphericalJoint(void *) : Joint(kJointSpherical)
{
}

SphericalJoint::SphericalJoint() : Joint(kJointSpherical)
{
	SetNewObject(new SphericalJointObject);
}

SphericalJoint::SphericalJoint(JointType type) : Joint(type)
{
}

SphericalJoint::SphericalJoint(const SphericalJoint& sphericalJoint) : Joint(sphericalJoint)
{
}

SphericalJoint::~SphericalJoint()
{
}

Node *SphericalJoint::Replicate(void) const
{
	return (new SphericalJoint(*this));
}

void SphericalJoint::ApplyStaticVelocityConstraints(ConstraintContact *contact)
{
	/*	A = Dynamic joint position, relative to static joint position
		B = Center of mass of the rigid body, relative to static joint position
		V = Current linear velocity of the rigid body
		ω = Current angular velocity of the rigid body

		The position constraint C for a one-body spherical joint is given by:

			C = |A| = 0

		The derivative dA/dt is given by V + ω ⨯ (A - B), so the velocity constraint is:

			 dC      A
			──── = ───── ⋅ [V + ω ⨯ (A - B)] = 0
			 dt     |A|

		After applying a triple product identity to the angular term, this is written in Jacobian form as follows:

			 dC      A          (A - B) ⨯ A
			──── = ───── ⋅ V + ────────────── ⋅ ω = 0
			 dt     |A|             |A|
	*/

	StaticConstraint	constraint[2];

	RigidBodyController *rigidBody = static_cast<RigidBodyController *>(contact->GetStartElement());
	Vector3D jointPosition = rigidBody->GetFinalTransform() * GetBodyJointPosition(0);
	Vector3D jointDirection = jointPosition - rigidBody->GetWorldCenterOfMass();

	int32 count = 0;

	Vector3D deltaPosition = jointPosition - GetWorldPosition();
	float d = SquaredMag(deltaPosition);
	if (d > K::min_float)
	{
		deltaPosition *= InverseSqrt(d);

		constraint[0].row.Set(deltaPosition, jointDirection % deltaPosition);
		constraint[0].bias = 0.0F;
		constraint[0].index = kImpulseDelta;
		constraint[0].range.Set(K::minus_infinity, K::infinity);
		count = 1;
	}

	if (GetLimitAngleFlag())
	{
		count += GetAngleLimitStaticVelocityConstraint(contact, rigidBody, jointDirection, &constraint[count]);
	}

	if (count != 0)
	{
		contact->SolveVelocityConstraints(rigidBody, count, constraint);
	}

	if (Fabs(contact->GetAppliedImpulse(kImpulseDelta)) > GetBreakingImpulse())
	{
		HandleBrokenJoint();
	}

	rigidBody->Integrate();
}

void SphericalJoint::ApplyStaticPositionConstraints(ConstraintContact *contact)
{
	RigidBodyController *rigidBody = static_cast<RigidBodyController *>(contact->GetStartElement());
	Vector3D jointPosition = rigidBody->GetFinalTransform() * GetBodyJointPosition(0);
	Vector3D jointDirection = jointPosition - rigidBody->GetWorldCenterOfMass();

	Vector3D deltaPosition = jointPosition - GetWorldPosition();
	float d = SquaredMag(deltaPosition);
	if (d > K::min_float)
	{
		StaticConstraint	constraint;

		float f = InverseSqrt(d);
		deltaPosition *= f;

		constraint.row.Set(deltaPosition, jointDirection % deltaPosition);
		constraint.bias = d * f * kStaticJointStabilizeFactor;

		ConstraintContact::SolvePositionConstraints(rigidBody, 1, &constraint);
	}

	if (GetLimitAngleFlag())
	{
		ApplyAngleLimitStaticPositionConstraint(contact, rigidBody, jointDirection);
	}

	rigidBody->Integrate();
}

void SphericalJoint::ApplyVelocityConstraints(ConstraintContact *contact)
{
	/*	A₁ = Dynamic joint position for rigid body 1
		A₂ = Dynamic joint position for rigid body 2
		B₁ = Center of mass of rigid body 1
		B₂ = Center of mass of rigid body 2
		V₁ = Current linear velocity of rigid body 1
		V₂ = Current linear velocity of rigid body 2
		ω₁ = Current angular velocity of rigid body 1
		ω₂ = Current angular velocity of rigid body 2

		The position constraint C for a two-body spherical joint is given by:

			C = |A₂ - A₁| = 0

		Define D as:

			      A₂ - A₁
			D = ──────────
			     |A₂ - A₁|

		The derivative dA/dt is given by V + ω ⨯ (A - B), so the velocity constraint is:

			 dC
			──── = D ⋅ [V₂ + ω₂ ⨯ (A₂ - B₂) - V₁ - ω₁ ⨯ (A₁ - B₁)] = 0
			 dt

		After applying a triple product identity to the angular terms, this is written in Jacobian form as follows:

			 dC
			──── = -D ⋅ V₁ + [D ⨯ (A₁ - B₁)] ⋅ ω₁ + D ⋅ V₂ + [(A₂ - B₂) ⨯ D] ⋅ ω₂ = 0
			 dt
	*/

	Constraint		constraint[2];

	RigidBodyController *rigidBody1 = static_cast<RigidBodyController *>(contact->GetStartElement());
	RigidBodyController *rigidBody2 = static_cast<RigidBodyController *>(contact->GetFinishElement());
	Vector3D jointPosition1 = rigidBody1->GetFinalTransform() * GetBodyJointPosition(0);
	Vector3D jointPosition2 = rigidBody2->GetFinalTransform() * GetBodyJointPosition(1);
	Vector3D jointDirection1 = jointPosition1 - rigidBody1->GetWorldCenterOfMass();
	Vector3D jointDirection2 = jointPosition2 - rigidBody2->GetWorldCenterOfMass();

	int32 count = 0;

	Vector3D deltaPosition = jointPosition2 - jointPosition1;
	float d = SquaredMag(deltaPosition);
	if (d > K::min_float)
	{
		deltaPosition *= InverseSqrt(d);

		constraint[0].row[0].Set(-deltaPosition, deltaPosition % jointDirection1);
		constraint[0].row[1].Set(deltaPosition, jointDirection2 % deltaPosition);
		constraint[0].bias = 0.0F;
		constraint[0].index = kImpulseDelta;
		constraint[0].range.Set(K::minus_infinity, K::infinity);
		count = 1;
	}

	if (GetLimitAngleFlag())
	{
		count += GetAngleLimitVelocityConstraint(contact, rigidBody1, rigidBody2, jointDirection1, jointDirection2, &constraint[count]);
	}

	if (count != 0)
	{
		contact->SolveVelocityConstraints(rigidBody1, rigidBody2, count, constraint);
	}

	if (Fabs(contact->GetAppliedImpulse(kImpulseDelta)) > GetBreakingImpulse())
	{
		HandleBrokenJoint();
	}

	rigidBody1->Integrate();
	rigidBody2->Integrate();
}

void SphericalJoint::ApplyPositionConstraints(ConstraintContact *contact)
{
	RigidBodyController *rigidBody1 = static_cast<RigidBodyController *>(contact->GetStartElement());
	RigidBodyController *rigidBody2 = static_cast<RigidBodyController *>(contact->GetFinishElement());
	Vector3D jointPosition1 = rigidBody1->GetFinalTransform() * GetBodyJointPosition(0);
	Vector3D jointPosition2 = rigidBody2->GetFinalTransform() * GetBodyJointPosition(1);
	Vector3D jointDirection1 = jointPosition1 - rigidBody1->GetWorldCenterOfMass();
	Vector3D jointDirection2 = jointPosition2 - rigidBody2->GetWorldCenterOfMass();

	Vector3D deltaPosition = jointPosition2 - jointPosition1;
	float d = SquaredMag(deltaPosition);
	if (d > K::min_float)
	{
		Constraint		constraint;

		float f = InverseSqrt(d);
		deltaPosition *= f;

		constraint.row[0].Set(-deltaPosition, deltaPosition % jointDirection1);
		constraint.row[1].Set(deltaPosition, jointDirection2 % deltaPosition);
		constraint.bias = d * f * kJointStabilizeFactor;

		float deltaImpulse = ConstraintContact::CalculatePositionImpulse(rigidBody1, rigidBody2, &constraint);
		rigidBody1->ApplyPositionCorrection(constraint.row[0], deltaImpulse);
		rigidBody2->ApplyPositionCorrection(constraint.row[1], deltaImpulse);
	}

	if (GetLimitAngleFlag())
	{
		ApplyAngleLimitPositionConstraint(contact, rigidBody1, rigidBody2, jointDirection1, jointDirection2);
	}

	rigidBody1->Integrate();
	rigidBody2->Integrate();
}

bool SphericalJoint::GetAngleLimitStaticVelocityConstraint(ConstraintContact *contact, RigidBodyController *rigidBody, const Vector3D& jointDirection, StaticConstraint *constraint) const
{
	/*	A = Dynamic joint position
		B = Center of mass of the rigid body
		V = Current linear velocity of the rigid body
		ω = Current angular velocity of the rigid body
		Z = Static z axis of the joint

		To satisfy the angle limit constraint, the center of mass B of the rigid body must stay inside
		the cone with apex at the point A whose surface makes the angle ϕ with the axis Z.

		Define D as:

			      A - B
			D = ─────────
			     |A - B|

		When B lies on the surface of the cone, the outward normal direction N is given by:

				  D ⨯ Z ⨯ D
			N = ─────────────
				 |D ⨯ Z ⨯ D|

		The position constraint C for the angle limit is the following, where P is the nearest point
		on the surface of the cone:

			C = (B - P) ⋅ N ≤ 0

		Because the point A is fixed and the point B rotates about it, the derivative dB/dt is given
		by V + ω ⨯ (B - A), and thus the velocity constraint is the following (where dN/dt is neglected):

			 dC
			──── = [V + ω ⨯ (B - A)] ⋅ N = 0
			 dt

		After applying a triple product identity to the angular term, this is written in Jacobian form as follows:

			 dC
			──── = N ⋅ V + [N ⨯ (A - B)] ⋅ ω = 0
			 dt

		The constraint is enforced only when Z ⋅ D < cos ϕ, which is itself a valid position constraint
		but is much less stable than the above formulation.
	*/

	float d = SquaredMag(jointDirection);
	if (d > K::min_float)
	{
		Vector3D direction = jointDirection * InverseSqrt(d);
		const Vector3D& zdir = GetWorldTransform()[2];

		float angleCos = zdir * direction;
		if (angleCos < GetAngleLimitCosSin().x)
		{
			Vector3D axis = zdir % direction;
			Vector3D normal = Normalize(direction % axis);

			constraint->row.Set(normal, normal % jointDirection);
			constraint->bias = 0.0F;
			constraint->index = kImpulseAngleLimit;
			constraint->range.Set(K::minus_infinity, 0.0F);
			return (true);
		}
	}

	return (false);
}

void SphericalJoint::ApplyAngleLimitStaticPositionConstraint(ConstraintContact *contact, RigidBodyController *rigidBody, const Vector3D& jointDirection)
{
	/*	ϕ = Angle between cone surface and axis Z
		μ = Angle between direction B - A and axis Z
		α = Angular position correction, μ - ϕ

		The sine of the correction angle α is found with the following identity:

			sin α = sin(μ - ϕ) = sin μ cos ϕ - cos μ sin ϕ

		The sine and cosine of ϕ are precomputed, and the sine and cosine of μ are given by:

			cos μ = Z ⋅ D

			sin μ = |Z ⨯ D|

		The corresponding distance correction Δ at the center of mass B is given by:

			Δ = |A - B| sin α

		This is applied in the opposite direction of the normal N.
	*/

	float d = SquaredMag(jointDirection);
	if (d > K::min_float)
	{
		float f = InverseSqrt(d);
		Vector3D direction = jointDirection * f;
		const Vector3D& zdir = GetWorldTransform()[2];

		float angleCos = zdir * direction;
		const Vector2D& limitCosSin = GetAngleLimitCosSin();
		if (angleCos < limitCosSin.x)
		{
			StaticConstraint	constraint;

			Vector3D axis = zdir % direction;
			Vector3D normal = Normalize(direction % axis);

			constraint.row.Set(normal, normal % jointDirection);
			constraint.bias = (Magnitude(axis) * limitCosSin.x - angleCos * limitCosSin.y) * (d * f * kStaticJointStabilizeFactor);

			float limitImpulse = ConstraintContact::CalculatePositionImpulse(rigidBody, &constraint);
			rigidBody->ApplyPositionCorrection(constraint.row, limitImpulse);
		}
	}
}

bool SphericalJoint::GetAngleLimitVelocityConstraint(ConstraintContact *contact, RigidBodyController *rigidBody1, RigidBodyController *rigidBody2, const Vector3D& jointDirection1, const Vector3D& jointDirection2, Constraint *constraint) const
{
	/*	A₁ = Dynamic joint position for rigid body 1
		A₂ = Dynamic joint position for rigid body 2
		B₁ = Center of mass of rigid body 1
		B₂ = Center of mass of rigid body 2
		V₁ = Current linear velocity of rigid body 1
		V₂ = Current linear velocity of rigid body 2
		ω₁ = Current angular velocity of rigid body 1
		ω₂ = Current angular velocity of rigid body 2
		Z = Dynamic z axis of the joint for rigid body 1

		To satisfy the angle limit constraint, the center of mass B₂ of the second rigid body must stay
		inside the cone with apex at the point A₂ whose surface makes the angle ϕ with the axis Z.

		Define D₁ and D₂ as:

			       A₁ - B₁					   A₂ - B₂
			D₁ = ───────────				D₂ = ───────────
			      |A₁ - B₁|					  |A₂ - B₂|

		When B₂ lies on the surface of the cone, the outward normal directions N₁ and N₂ are given by:

				   D₁ ⨯ (Z ⨯ D₂)				   D₂ ⨯ Z ⨯ D₂
			N₁ = ─────────────────		N₂ = ───────────────
				  |D₁ ⨯ (Z ⨯ D₂)|			  |D₂ ⨯ Z ⨯ D₂|

		The position constraint C for the angle limit is the following, where P₁ and P₂ are the nearest
		points on the surface of the cone:

			C = (B₂ - P₂) ⋅ N₂ - (B₁ - P₁) ⋅ N₁ ≤ 0

		The velocity constraint is the following (where dN₁/dt and dN₂/dt are neglected):

			 dC
			──── = [V₂ + ω₂ ⨯ (B₂ - A₂)] ⋅ N₂ - [V₁ + ω₁ ⨯ (B₁ - A₁)] ⋅ N₁ = 0
			 dt

		After applying a triple product identity to the angular terms, this is written in Jacobian form as follows:

			 dC
			──── = -N₁ ⋅ V₁ - [N₁ ⨯ (A₁ - B₁)] ⋅ ω₁ + N₂ ⋅ V₂ + [N₂ ⨯ (A₂ - B₂)] ⋅ ω₂ = 0
			 dt

		The constraint is enforced only when Z ⋅ D₂ < cos ϕ.
	*/

	float d1 = SquaredMag(jointDirection1);
	float d2 = SquaredMag(jointDirection2);
	if (Fmin(d1, d2) > K::min_float)
	{
		float f2 = InverseSqrt(d2);
		Vector3D direction2 = jointDirection2 * f2;
		Vector3D zdir = rigidBody1->GetFinalTransform() * GetBodyJointAxis(0);

		float angleCos = zdir * direction2;
		if (angleCos < GetAngleLimitCosSin().x)
		{
			float f1 = InverseSqrt(d1);
			Vector3D direction1 = jointDirection1 * f1;

			Vector3D axis = zdir % direction2;
			Vector3D normal1 = Normalize(axis % direction1);
			Vector3D normal2 = Normalize(direction2 % axis);

			constraint->row[0].Set(normal1, jointDirection1 % normal1);
			constraint->row[1].Set(normal2, normal2 % jointDirection2);
			constraint->bias = 0.0F;
			constraint->index = kImpulseAngleLimit;
			constraint->range.Set(K::minus_infinity, 0.0F);
			return (true);
		}
	}

	return (false);
}

void SphericalJoint::ApplyAngleLimitPositionConstraint(ConstraintContact *contact, RigidBodyController *rigidBody1, RigidBodyController *rigidBody2, const Vector3D& jointDirection1, const Vector3D& jointDirection2)
{
	/*	ϕ = Angle between cone surface and axis Z
		μ = Angle between direction B₂ - A₂ and axis Z
		α = Angular position correction, μ - ϕ

		The sine of the correction angle α is found with the following identity:

			sin α = sin(μ - ϕ) = sin μ cos ϕ - cos μ sin ϕ

		The sine and cosine of ϕ are precomputed, and the sine and cosine of μ are given by:

			cos μ = Z ⋅ D₂

			sin μ = |Z ⨯ D₂|

		The corresponding distance correction Δ at the center of mass B₁ or B₂ is given by:

			Δ = min(|A₁ - B₁|, |A₂ - B₂|) sin α

		This is applied in the opposite direction of the normals N₁ and N₂.
	*/

	float d1 = SquaredMag(jointDirection1);
	float d2 = SquaredMag(jointDirection2);
	if (Fmin(d1, d2) > K::min_float)
	{
		float f2 = InverseSqrt(d2);
		Vector3D direction2 = jointDirection2 * f2;
		Vector3D zdir = rigidBody1->GetFinalTransform() * GetBodyJointAxis(0);

		float angleCos = zdir * direction2;
		const Vector2D& limitCosSin = GetAngleLimitCosSin();
		if (angleCos < limitCosSin.x)
		{
			Constraint		constraint;

			float f1 = InverseSqrt(d1);
			Vector3D direction1 = jointDirection1 * f1;

			Vector3D axis = zdir % direction2;
			Vector3D normal1 = Normalize(axis % direction1);
			Vector3D normal2 = Normalize(direction2 % axis);

			constraint.row[0].Set(normal1, jointDirection1 % normal1);
			constraint.row[1].Set(normal2, normal2 % jointDirection2);
			constraint.bias = (Magnitude(axis) * limitCosSin.x - angleCos * limitCosSin.y) * (Fmin(d1 * f1, d2 * f2) * kJointStabilizeFactor);

			float limitImpulse = ConstraintContact::CalculatePositionImpulse(rigidBody1, rigidBody2, &constraint);
			rigidBody1->ApplyPositionCorrection(constraint.row[0], limitImpulse);
			rigidBody2->ApplyPositionCorrection(constraint.row[1], limitImpulse);
		}
	}
}


UniversalJoint::UniversalJoint(void *) : SphericalJoint(kJointUniversal)
{
}

UniversalJoint::UniversalJoint() : SphericalJoint(kJointUniversal)
{
	SetNewObject(new UniversalJointObject);
}

UniversalJoint::UniversalJoint(const UniversalJoint& universalJoint) : SphericalJoint(universalJoint)
{
}

UniversalJoint::~UniversalJoint()
{
}

Node *UniversalJoint::Replicate(void) const
{
	return (new UniversalJoint(*this));
}

void UniversalJoint::ApplyStaticVelocityConstraints(ConstraintContact *contact)
{
	/*	X = Dynamic x axis of the joint
		Y = Static y axis of the joint

		The primary constraint for the universal joint is the same as the spherical joint.

		There is the additional constraint that the rigid body does not twist about the radial axis.
		The corresponding velocity constraint is simply the following in Jacobian form:

			 dC      X ⨯ Y
			──── = ───────── ⋅ ω = 0
			 dt     |X ⨯ Y|
	*/

	StaticConstraint	constraint[3];

	RigidBodyController *rigidBody = static_cast<RigidBodyController *>(contact->GetStartElement());
	Vector3D jointPosition = rigidBody->GetFinalTransform() * GetBodyJointPosition(0);
	Vector3D jointDirection = jointPosition - rigidBody->GetWorldCenterOfMass();

	int32 count = 0;

	Vector3D deltaPosition = jointPosition - GetWorldPosition();
	float d = SquaredMag(deltaPosition);
	if (d > K::min_float)
	{
		deltaPosition *= InverseSqrt(d);

		constraint[0].row.Set(deltaPosition, jointDirection % deltaPosition);
		constraint[0].bias = 0.0F;
		constraint[0].index = kImpulseDelta;
		constraint[0].range.Set(K::minus_infinity, K::infinity);
		count = 1;
	}

	if (GetConstrainTwistFlag())
	{
		Vector3D jointTangent = rigidBody->GetFinalTransform() * GetBodyJointTangent(0);
		const Vector3D& ydir = GetWorldTransform()[1];

		Vector3D twistAxis = jointTangent % ydir;
		d = SquaredMag(twistAxis);
		if (d > K::min_float)
		{
			twistAxis *= InverseSqrt(d);

			constraint[count].row.Set(Zero3D, twistAxis);
			constraint[count].bias = 0.0F;
			constraint[count].index = kImpulseTwist;
			constraint[count].range.Set(K::minus_infinity, K::infinity);
			count++;
		}
	}
	else
	{
		d = SquaredMag(jointDirection);
		if (d > K::min_float)
		{
			jointDirection *= InverseSqrt(d);

			constraint[count].row.Set(Zero3D, jointDirection);
			constraint[count].bias = 0.0F;
			constraint[count].index = kImpulseTwist;
			constraint[count].range.Set(K::minus_infinity, K::infinity);
			count++;
		}
	}

	if (GetLimitAngleFlag())
	{
		count += GetAngleLimitStaticVelocityConstraint(contact, rigidBody, jointDirection, &constraint[count]);
	}

	if (count != 0)
	{
		contact->SolveVelocityConstraints(rigidBody, count, constraint);
	}

	if (Fabs(contact->GetAppliedImpulse(kImpulseDelta)) > GetBreakingImpulse())
	{
		HandleBrokenJoint();
	}

	rigidBody->Integrate();
}

void UniversalJoint::ApplyStaticPositionConstraints(ConstraintContact *contact)
{
	StaticConstraint	constraint[2];

	RigidBodyController *rigidBody = static_cast<RigidBodyController *>(contact->GetStartElement());
	Vector3D jointPosition = rigidBody->GetFinalTransform() * GetBodyJointPosition(0);
	Vector3D jointDirection = jointPosition - rigidBody->GetWorldCenterOfMass();

	int32 count = 0;

	Vector3D deltaPosition = jointPosition - GetWorldPosition();
	float d = SquaredMag(deltaPosition);
	if (d > K::min_float)
	{
		float f = InverseSqrt(d);
		deltaPosition *= f;

		constraint[0].row.Set(deltaPosition, jointDirection % deltaPosition);
		constraint[0].bias = d * f * kStaticJointStabilizeFactor;
		count = 1;
	}

	if (GetConstrainTwistFlag())
	{
		Vector3D jointTangent = rigidBody->GetFinalTransform() * GetBodyJointTangent(0);
		Vector3D ydir = GetWorldTransform()[1];

		Vector3D twistAxis = jointTangent % ydir;
		d = SquaredMag(twistAxis);
		if (d > K::min_float)
		{
			twistAxis *= InverseSqrt(d);

			float twistCorrection = jointTangent * ydir;
			if (twistAxis * GetWorldTransform()[2] < 0.0F)
			{
				twistCorrection = Acos(twistCorrection) + K::tau_over_4;
			}

			constraint[count].row.Set(Zero3D, twistAxis);
			constraint[count].bias = twistCorrection * kStaticJointStabilizeFactor;
			count++;
		}
	}

	if (count != 0)
	{
		ConstraintContact::SolvePositionConstraints(rigidBody, count, constraint);
	}

	if (GetLimitAngleFlag())
	{
		ApplyAngleLimitStaticPositionConstraint(contact, rigidBody, jointDirection);
	}

	rigidBody->Integrate();
}

void UniversalJoint::ApplyVelocityConstraints(ConstraintContact *contact)
{
	Constraint		constraint[3];

	RigidBodyController *rigidBody1 = static_cast<RigidBodyController *>(contact->GetStartElement());
	RigidBodyController *rigidBody2 = static_cast<RigidBodyController *>(contact->GetFinishElement());
	Vector3D jointPosition1 = rigidBody1->GetFinalTransform() * GetBodyJointPosition(0);
	Vector3D jointPosition2 = rigidBody2->GetFinalTransform() * GetBodyJointPosition(1);
	Vector3D jointDirection1 = jointPosition1 - rigidBody1->GetWorldCenterOfMass();
	Vector3D jointDirection2 = jointPosition2 - rigidBody2->GetWorldCenterOfMass();

	int32 count = 0;

	Vector3D deltaPosition = jointPosition2 - jointPosition1;
	float d = SquaredMag(deltaPosition);
	if (d > K::min_float)
	{
		deltaPosition *= InverseSqrt(d);

		constraint[0].row[0].Set(-deltaPosition, deltaPosition % jointDirection1);
		constraint[0].row[1].Set(deltaPosition, jointDirection2 % deltaPosition);
		constraint[0].bias = 0.0F;
		constraint[0].index = kImpulseDelta;
		constraint[0].range.Set(K::minus_infinity, K::infinity);
		count = 1;
	}

	Vector3D jointTangent1 = rigidBody1->GetFinalTransform() * GetBodyJointTangent(0);
	Vector3D jointBitangent2 = rigidBody2->GetFinalTransform() * GetBodyJointBitangent(1);
	Vector3D twistAxis = jointTangent1 % jointBitangent2;

	d = SquaredMag(twistAxis);
	if (d > K::min_float)
	{
		twistAxis *= InverseSqrt(d);

		constraint[count].row[0].Set(Zero3D, -twistAxis);
		constraint[count].row[1].Set(Zero3D, twistAxis);
		constraint[count].bias = 0.0F;
		constraint[count].index = kImpulseTwist;
		constraint[count].range.Set(K::minus_infinity, K::infinity);
		count++;
	}

	if (GetLimitAngleFlag())
	{
		count += GetAngleLimitVelocityConstraint(contact, rigidBody1, rigidBody2, jointDirection1, jointDirection2, &constraint[count]);
	}

	if (count != 0)
	{
		contact->SolveVelocityConstraints(rigidBody1, rigidBody2, count, constraint);
	}

	if (Fabs(contact->GetAppliedImpulse(kImpulseDelta)) > GetBreakingImpulse())
	{
		HandleBrokenJoint();
	}

	rigidBody1->Integrate();
	rigidBody2->Integrate();
}

void UniversalJoint::ApplyPositionConstraints(ConstraintContact *contact)
{
	Constraint		constraint[2];

	RigidBodyController *rigidBody1 = static_cast<RigidBodyController *>(contact->GetStartElement());
	RigidBodyController *rigidBody2 = static_cast<RigidBodyController *>(contact->GetFinishElement());
	Vector3D jointPosition1 = rigidBody1->GetFinalTransform() * GetBodyJointPosition(0);
	Vector3D jointPosition2 = rigidBody2->GetFinalTransform() * GetBodyJointPosition(1);
	Vector3D jointDirection1 = jointPosition1 - rigidBody1->GetWorldCenterOfMass();
	Vector3D jointDirection2 = jointPosition2 - rigidBody2->GetWorldCenterOfMass();

	int32 count = 0;

	Vector3D deltaPosition = jointPosition2 - jointPosition1;
	float d = SquaredMag(deltaPosition);
	if (d > K::min_float)
	{
		float f = InverseSqrt(d);
		deltaPosition *= f;

		constraint[0].row[0].Set(-deltaPosition, deltaPosition % jointDirection1);
		constraint[0].row[1].Set(deltaPosition, jointDirection2 % deltaPosition);
		constraint[0].bias = d * f * kJointStabilizeFactor;
		count = 1;
	}

	if (GetConstrainTwistFlag())
	{
		Vector3D jointTangent1 = rigidBody1->GetFinalTransform() * GetBodyJointTangent(0);
		Vector3D jointBitangent2 = rigidBody2->GetFinalTransform() * GetBodyJointBitangent(1);
		Vector3D twistAxis = jointTangent1 % jointBitangent2;

		d = SquaredMag(twistAxis);
		if (d > K::min_float)
		{
			twistAxis *= InverseSqrt(d);

			float twistCorrection = jointTangent1 * jointBitangent2;
			if (twistAxis * GetWorldTransform()[2] < 0.0F)
			{
				twistCorrection = Acos(twistCorrection) + K::tau_over_4;
			}

			constraint[count].row[0].Set(Zero3D, -twistAxis);
			constraint[count].row[1].Set(Zero3D, twistAxis);
			constraint[count].bias = -twistCorrection * kJointStabilizeFactor;
			count++;
		}
	}

	if (count != 0)
	{
		ConstraintContact::SolvePositionConstraints(rigidBody1, rigidBody2, count, constraint);
	}

	if (GetLimitAngleFlag())
	{
		ApplyAngleLimitPositionConstraint(contact, rigidBody1, rigidBody2, jointDirection1, jointDirection2);
	}

	rigidBody1->Integrate();
	rigidBody2->Integrate();
}


DiscalJoint::DiscalJoint(void *) : Joint(kJointDiscal)
{
}

DiscalJoint::DiscalJoint() : Joint(kJointDiscal)
{
	SetNewObject(new DiscalJointObject);
}

DiscalJoint::DiscalJoint(JointType type) : Joint(type)
{
}

DiscalJoint::DiscalJoint(const DiscalJoint& discalJoint) : Joint(discalJoint)
{
}

DiscalJoint::~DiscalJoint()
{
}

Node *DiscalJoint::Replicate(void) const
{
	return (new DiscalJoint(*this));
}

void DiscalJoint::ApplyStaticVelocityConstraints(ConstraintContact *contact)
{
	/*	The primary constraint for the discal joint is the same as the spherical joint.

		There is the additional constraint that the rigid body does not rotate in the plane formed
		by the static joint axis and the dynamic radial axis. The corresponding velocity constraint
		is the following in Jacobian form:

			 dC      (A - B) ⨯ Z
			──── = ─────────────── ⋅ ω = 0
			 dt     |(A - B) ⨯ Z|
	*/

	StaticConstraint	constraint[3];

	RigidBodyController *rigidBody = static_cast<RigidBodyController *>(contact->GetStartElement());
	Vector3D jointPosition = rigidBody->GetFinalTransform() * GetBodyJointPosition(0);
	Vector3D jointDirection = jointPosition - rigidBody->GetWorldCenterOfMass();

	int32 count = 0;

	Vector3D deltaPosition = jointPosition - GetWorldPosition();
	float d = SquaredMag(deltaPosition);
	if (d > K::min_float)
	{
		deltaPosition *= InverseSqrt(d);

		constraint[0].row.Set(deltaPosition, jointDirection % deltaPosition);
		constraint[0].bias = 0.0F;
		constraint[0].index = kImpulseDelta;
		constraint[0].range.Set(K::minus_infinity, K::infinity);
		count = 1;
	}

	const Vector3D& zdir = GetWorldTransform()[2];
	Vector3D xdir = jointDirection % zdir;
	d = SquaredMag(xdir);
	if (d > K::min_float)
	{
		constraint[count].row.Set(Zero3D, xdir * InverseSqrt(d));
		constraint[count].bias = 0.0F;
		constraint[count].index = kImpulseRotationX;
		constraint[count].range.Set(K::minus_infinity, K::infinity);
		count++;
	}

	if (GetLimitAngleFlag())
	{
		count += GetAngleLimitStaticVelocityConstraint(contact, rigidBody, jointDirection, &constraint[count]);
	}

	if (count != 0)
	{
		contact->SolveVelocityConstraints(rigidBody, count, constraint);
	}

	if (Fabs(contact->GetAppliedImpulse(kImpulseDelta)) > GetBreakingImpulse())
	{
		HandleBrokenJoint();
	}

	rigidBody->Integrate();
}

void DiscalJoint::ApplyStaticPositionConstraints(ConstraintContact *contact)
{
	/*	The fixed orientation of the dynamic joint axis is enforced by an angular position correction
		α corresponding to the angle between the plane perpendicular to the static joint axis and the
		dynamic radial axis (A - B). The rotation occurs about the axis (A - B) ⨯ Z, and the sine
		of the angle α is given by:

			sin α = cos (τ/4 - α) = (A - B) ⋅ Z

		Because α should be close to zero, we make the approximation α ≈ sin α and use the dot product
		directly as the angle of rotation.
	*/

	StaticConstraint	constraint[2];

	RigidBodyController *rigidBody = static_cast<RigidBodyController *>(contact->GetStartElement());
	Vector3D jointPosition = rigidBody->GetFinalTransform() * GetBodyJointPosition(0);
	Vector3D jointDirection = jointPosition - rigidBody->GetWorldCenterOfMass();
	const Vector3D& zdir = GetWorldTransform()[2];

	int32 count = 0;

	Vector3D deltaPosition = jointPosition - GetWorldPosition();
	float d = SquaredMag(deltaPosition);
	if (d > K::min_float)
	{
		float f = InverseSqrt(d);
		deltaPosition *= f;

		constraint[0].row.Set(deltaPosition, ProjectOnto(jointDirection % deltaPosition, zdir));
		constraint[0].bias = d * f * kStaticJointStabilizeFactor;
		count = 1;
	}

	Vector3D rotateAxis = jointDirection % zdir;
	d = SquaredMag(rotateAxis);
	if (d > K::min_float)
	{
		rotateAxis *= InverseSqrt(d);

		constraint[count].row.Set(Zero3D, rotateAxis);
		constraint[count].bias = (jointDirection * zdir) * (InverseMag(jointDirection) * kStaticJointStabilizeFactor);
		count++;
	}

	if (count != 0)
	{
		ConstraintContact::SolvePositionConstraints(rigidBody, count, constraint);
	}

	if (GetLimitAngleFlag())
	{
		ApplyAngleLimitStaticPositionConstraint(contact, rigidBody, jointDirection);
	}

	rigidBody->Integrate();
}

void DiscalJoint::ApplyVelocityConstraints(ConstraintContact *contact)
{
	Constraint		constraint[2];

	RigidBodyController *rigidBody1 = static_cast<RigidBodyController *>(contact->GetStartElement());
	RigidBodyController *rigidBody2 = static_cast<RigidBodyController *>(contact->GetFinishElement());
	Vector3D jointPosition1 = rigidBody1->GetFinalTransform() * GetBodyJointPosition(0);
	Vector3D jointPosition2 = rigidBody2->GetFinalTransform() * GetBodyJointPosition(1);
	Vector3D jointDirection1 = jointPosition1 - rigidBody1->GetWorldCenterOfMass();
	Vector3D jointDirection2 = jointPosition2 - rigidBody2->GetWorldCenterOfMass();

	int32 count = 0;

	Vector3D deltaPosition = jointPosition2 - jointPosition1;
	float d = SquaredMag(deltaPosition);
	if (d > K::min_float)
	{
		deltaPosition *= InverseSqrt(d);

		constraint[0].row[0].Set(-deltaPosition, deltaPosition % jointDirection1);
		constraint[0].row[1].Set(deltaPosition, jointDirection2 % deltaPosition);
		constraint[0].bias = 0.0F;
		constraint[0].index = kImpulseDelta;
		constraint[0].range.Set(K::minus_infinity, K::infinity);
		count = 1;
	}

	if (GetLimitAngleFlag())
	{
		count += GetAngleLimitVelocityConstraint(contact, rigidBody1, rigidBody2, jointDirection1, jointDirection2, &constraint[count]);
	}

	if (count != 0)
	{
		contact->SolveVelocityConstraints(rigidBody1, rigidBody2, count, constraint);
	}

	if (Fabs(contact->GetAppliedImpulse(kImpulseDelta)) > GetBreakingImpulse())
	{
		HandleBrokenJoint();
	}

	rigidBody1->Integrate();
	rigidBody2->Integrate();
}

void DiscalJoint::ApplyPositionConstraints(ConstraintContact *contact)
{
	RigidBodyController *rigidBody1 = static_cast<RigidBodyController *>(contact->GetStartElement());
	RigidBodyController *rigidBody2 = static_cast<RigidBodyController *>(contact->GetFinishElement());
	Vector3D jointPosition1 = rigidBody1->GetFinalTransform() * GetBodyJointPosition(0);
	Vector3D jointPosition2 = rigidBody2->GetFinalTransform() * GetBodyJointPosition(1);
	Vector3D jointDirection1 = jointPosition1 - rigidBody1->GetWorldCenterOfMass();
	Vector3D jointDirection2 = jointPosition2 - rigidBody2->GetWorldCenterOfMass();

	Vector3D deltaPosition = jointPosition2 - jointPosition1;
	float d = SquaredMag(deltaPosition);
	if (d > K::min_float)
	{
		Constraint		constraint;

		float f = InverseSqrt(d);
		deltaPosition *= f;

		constraint.row[0].Set(-deltaPosition, deltaPosition % jointDirection1);
		constraint.row[1].Set(deltaPosition, jointDirection2 % deltaPosition);
		constraint.bias = d * f * kJointStabilizeFactor;

		float deltaImpulse = ConstraintContact::CalculatePositionImpulse(rigidBody1, rigidBody2, &constraint);
		rigidBody1->ApplyPositionCorrection(constraint.row[0], deltaImpulse);
		rigidBody2->ApplyPositionCorrection(constraint.row[1], deltaImpulse);
	}

	if (GetLimitAngleFlag())
	{
		ApplyAngleLimitPositionConstraint(contact, rigidBody1, rigidBody2, jointDirection1, jointDirection2);
	}

	rigidBody1->Integrate();
	rigidBody2->Integrate();
}

bool DiscalJoint::GetAngleLimitStaticVelocityConstraint(ConstraintContact *contact, RigidBodyController *rigidBody, const Vector3D& jointDirection, StaticConstraint *constraint) const
{
	float d = SquaredMag(jointDirection);
	if (d > K::min_float)
	{
		Vector3D direction = jointDirection * InverseSqrt(d);
		const Vector3D& xdir = GetWorldTransform()[0];

		float angleCos = xdir * direction;
		if (angleCos < GetAngleLimitCosSin().x)
		{
			Vector3D axis = xdir % direction;
			Vector3D normal = Normalize(direction % axis);

			constraint->row.Set(normal, normal % jointDirection);
			constraint->bias = 0.0F;
			constraint->index = kImpulseAngleLimit;
			constraint->range.Set(K::minus_infinity, 0.0F);
			return (true);
		}
	}

	return (false);
}

void DiscalJoint::ApplyAngleLimitStaticPositionConstraint(ConstraintContact *contact, RigidBodyController *rigidBody, const Vector3D& jointDirection)
{
	float d = SquaredMag(jointDirection);
	if (d > K::min_float)
	{
		float f = InverseSqrt(d);
		Vector3D direction = jointDirection * f;
		const Vector3D& xdir = GetWorldTransform()[0];

		float angleCos = xdir * direction;
		const Vector2D& limitCosSin = GetAngleLimitCosSin();
		if (angleCos < limitCosSin.x)
		{
			StaticConstraint	constraint;

			Vector3D axis = xdir % direction;
			Vector3D normal = Normalize(direction % axis);

			constraint.row.Set(normal, normal % jointDirection);
			constraint.bias = (Magnitude(axis) * limitCosSin.x - angleCos * limitCosSin.y) * (d * f * kStaticJointStabilizeFactor);

			float limitImpulse = ConstraintContact::CalculatePositionImpulse(rigidBody, &constraint);
			rigidBody->ApplyPositionCorrection(constraint.row, limitImpulse);
		}
	}
}

bool DiscalJoint::GetAngleLimitVelocityConstraint(ConstraintContact *contact, RigidBodyController *rigidBody1, RigidBodyController *rigidBody2, const Vector3D& jointDirection1, const Vector3D& jointDirection2, Constraint *constraint) const
{
	float d1 = SquaredMag(jointDirection1);
	float d2 = SquaredMag(jointDirection2);
	if (Fmin(d1, d2) > K::min_float)
	{
		float f2 = InverseSqrt(d2);
		Vector3D direction2 = jointDirection2 * f2;
		Vector3D xdir = rigidBody1->GetFinalTransform() * GetBodyJointTangent(0);

		float angleCos = xdir * direction2;
		if (angleCos < GetAngleLimitCosSin().x)
		{
			float f1 = InverseSqrt(d1);
			Vector3D direction1 = jointDirection1 * f1;

			Vector3D axis = xdir % direction2;
			Vector3D normal1 = Normalize(axis % direction1);
			Vector3D normal2 = Normalize(direction2 % axis);

			constraint->row[0].Set(normal1, jointDirection1 % normal1);
			constraint->row[1].Set(normal2, normal2 % jointDirection2);
			constraint->bias = 0.0F;
			constraint->index = kImpulseAngleLimit;
			constraint->range.Set(K::minus_infinity, 0.0F);
			return (true);
		}
	}

	return (false);
}

void DiscalJoint::ApplyAngleLimitPositionConstraint(ConstraintContact *contact, RigidBodyController *rigidBody1, RigidBodyController *rigidBody2, const Vector3D& jointDirection1, const Vector3D& jointDirection2)
{
	float d1 = SquaredMag(jointDirection1);
	float d2 = SquaredMag(jointDirection2);
	if (Fmin(d1, d2) > K::min_float)
	{
		float f2 = InverseSqrt(d2);
		Vector3D direction2 = jointDirection2 * f2;
		Vector3D xdir = rigidBody1->GetFinalTransform() * GetBodyJointTangent(0);

		float angleCos = xdir * direction2;
		const Vector2D& limitCosSin = GetAngleLimitCosSin();
		if (angleCos < limitCosSin.x)
		{
			Constraint		constraint;

			float f1 = InverseSqrt(d1);
			Vector3D direction1 = jointDirection1 * f1;

			Vector3D axis = xdir % direction2;
			Vector3D normal1 = Normalize(axis % direction1);
			Vector3D normal2 = Normalize(direction2 % axis);

			constraint.row[0].Set(normal1, jointDirection1 % normal1);
			constraint.row[1].Set(normal2, normal2 % jointDirection2);
			constraint.bias = (Magnitude(axis) * limitCosSin.x - angleCos * limitCosSin.y) * (Fmin(d1 * f1, d2 * f2) * kJointStabilizeFactor);

			float limitImpulse = ConstraintContact::CalculatePositionImpulse(rigidBody1, rigidBody2, &constraint);
			rigidBody1->ApplyPositionCorrection(constraint.row[0], limitImpulse);
			rigidBody2->ApplyPositionCorrection(constraint.row[1], limitImpulse);
		}
	}
}


RevoluteJoint::RevoluteJoint(void *) : DiscalJoint(kJointRevolute)
{
}

RevoluteJoint::RevoluteJoint() : DiscalJoint(kJointRevolute)
{
	SetNewObject(new RevoluteJointObject);
}

RevoluteJoint::RevoluteJoint(const RevoluteJoint& revoluteJoint) : DiscalJoint(revoluteJoint)
{
}

RevoluteJoint::~RevoluteJoint()
{
}

Node *RevoluteJoint::Replicate(void) const
{
	return (new RevoluteJoint(*this));
}

void RevoluteJoint::ApplyStaticVelocityConstraints(ConstraintContact *contact)
{
	/*	The primary constraint for the revolute joint is the same as the spherical joint.

		There is the additional constraint that the rigid body rotates only in the plane perpendicular to
		the static joint axis. The corresponding velocity constraints are the following in Jacobian form:

			 dC      (A - B) ⨯ Z
			──── = ─────────────── ⋅ ω = 0
			 dt     |(A - B) ⨯ Z|

			 dC      Z ⨯ (A - B) ⨯ Z
			──── = ─────────────────── ⋅ ω = 0
			 dt       |(A - B) ⨯ Z|
	*/

	StaticConstraint	constraint[4];

	RigidBodyController *rigidBody = static_cast<RigidBodyController *>(contact->GetStartElement());
	Vector3D jointPosition = rigidBody->GetFinalTransform() * GetBodyJointPosition(0);
	Vector3D jointDirection = jointPosition - rigidBody->GetWorldCenterOfMass();

	int32 count = 0;

	Vector3D deltaPosition = jointPosition - GetWorldPosition();
	float d = SquaredMag(deltaPosition);
	if (d > K::min_float)
	{
		deltaPosition *= InverseSqrt(d);

		constraint[0].row.Set(deltaPosition, jointDirection % deltaPosition);
		constraint[0].bias = 0.0F;
		constraint[0].index = kImpulseDelta;
		constraint[0].range.Set(K::minus_infinity, K::infinity);
		count = 1;
	}

	const Vector3D& zdir = GetWorldTransform()[2];
	Vector3D xdir = jointDirection % zdir;
	d = SquaredMag(xdir);
	if (d > K::min_float)
	{
		xdir *= InverseSqrt(d);
		Vector3D ydir = zdir % xdir;

		constraint[count].row.Set(Zero3D, xdir);
		constraint[count].bias = 0.0F;
		constraint[count].index = kImpulseRotationX;
		constraint[count].range.Set(K::minus_infinity, K::infinity);
		count++;

		constraint[count].row.Set(Zero3D, ydir);
		constraint[count].bias = 0.0F;
		constraint[count].index = kImpulseRotationY;
		constraint[count].range.Set(K::minus_infinity, K::infinity);
		count++;
	}

	if (GetLimitAngleFlag())
	{
		count += GetAngleLimitStaticVelocityConstraint(contact, rigidBody, jointDirection, &constraint[count]);
	}

	if (count != 0)
	{
		contact->SolveVelocityConstraints(rigidBody, count, constraint);
	}

	if (Fabs(contact->GetAppliedImpulse(kImpulseDelta)) > GetBreakingImpulse())
	{
		HandleBrokenJoint();
	}

	rigidBody->Integrate();
}

void RevoluteJoint::ApplyStaticPositionConstraints(ConstraintContact *contact)
{
	StaticConstraint	constraint[2];

	RigidBodyController *rigidBody = static_cast<RigidBodyController *>(contact->GetStartElement());
	Vector3D jointPosition = rigidBody->GetFinalTransform() * GetBodyJointPosition(0);
	Vector3D jointDirection = jointPosition - rigidBody->GetWorldCenterOfMass();
	const Vector3D& zdir = GetWorldTransform()[2];

	int32 count = 0;

	Vector3D deltaPosition = jointPosition - GetWorldPosition();
	float d = SquaredMag(deltaPosition);
	if (d > K::min_float)
	{
		float f = InverseSqrt(d);
		deltaPosition *= f;

		constraint[0].row.Set(deltaPosition, ProjectOnto(jointDirection % deltaPosition, zdir));
		constraint[0].bias = d * f * kStaticJointStabilizeFactor;
		count = 1;
	}

	Vector3D jointAxis = rigidBody->GetFinalTransform() * GetBodyJointAxis(0);

	Vector3D rotateAxis = zdir % jointAxis;
	d = SquaredMag(rotateAxis);
	if (d > K::min_float)
	{
		float f = InverseSqrt(d);
		rotateAxis *= f;

		constraint[count].row.Set(Zero3D, rotateAxis);
		constraint[count].bias = d * f * kStaticJointStabilizeFactor;
		count++;
	}

	if (count != 0)
	{
		ConstraintContact::SolvePositionConstraints(rigidBody, count, constraint);
	}

	if (GetLimitAngleFlag())
	{
		ApplyAngleLimitStaticPositionConstraint(contact, rigidBody, jointDirection);
	}

	rigidBody->Integrate();
}

void RevoluteJoint::ApplyVelocityConstraints(ConstraintContact *contact)
{
	Constraint		constraint[4];

	RigidBodyController *rigidBody1 = static_cast<RigidBodyController *>(contact->GetStartElement());
	RigidBodyController *rigidBody2 = static_cast<RigidBodyController *>(contact->GetFinishElement());
	Vector3D jointPosition1 = rigidBody1->GetFinalTransform() * GetBodyJointPosition(0);
	Vector3D jointPosition2 = rigidBody2->GetFinalTransform() * GetBodyJointPosition(1);
	Vector3D jointDirection1 = jointPosition1 - rigidBody1->GetWorldCenterOfMass();
	Vector3D jointDirection2 = jointPosition2 - rigidBody2->GetWorldCenterOfMass();

	int32 count = 0;

	Vector3D deltaPosition = jointPosition2 - jointPosition1;
	float d = SquaredMag(deltaPosition);
	if (d > K::min_float)
	{
		deltaPosition *= InverseSqrt(d);

		constraint[0].row[0].Set(-deltaPosition, deltaPosition % jointDirection1);
		constraint[0].row[1].Set(deltaPosition, jointDirection2 % deltaPosition);
		constraint[0].bias = 0.0F;
		constraint[0].index = kImpulseDelta;
		constraint[0].range.Set(K::minus_infinity, K::infinity);
		count = 1;
	}

	Vector3D jointAxis1 = rigidBody1->GetFinalTransform() * GetBodyJointAxis(0);
	Vector3D jointAxis2 = rigidBody2->GetFinalTransform() * GetBodyJointAxis(1);
	float weight1 = rigidBody2->GetInverseBodyMass();
	float weight2 = rigidBody1->GetInverseBodyMass();

	Vector3D zdir = Normalize(jointAxis1 * weight1 + jointAxis2 * weight2);
	Vector3D xdir = (zdir % jointDirection1) * weight1 + (jointDirection2 % zdir) * weight2;
	d = SquaredMag(xdir);
	if (d > K::min_float)
	{
		xdir *= InverseSqrt(d);
		Vector3D ydir = zdir % xdir;

		constraint[count].row[0].Set(Zero3D, -xdir);
		constraint[count].row[1].Set(Zero3D, xdir);
		constraint[count].bias = 0.0F;
		constraint[count].index = kImpulseRotationX;
		constraint[count].range.Set(K::minus_infinity, K::infinity);
		count++;

		constraint[count].row[0].Set(Zero3D, -ydir);
		constraint[count].row[1].Set(Zero3D, ydir);
		constraint[count].bias = 0.0F;
		constraint[count].index = kImpulseRotationY;
		constraint[count].range.Set(K::minus_infinity, K::infinity);
		count++;
	}

	if (GetLimitAngleFlag())
	{
		count += GetAngleLimitVelocityConstraint(contact, rigidBody1, rigidBody2, jointDirection1, jointDirection2, &constraint[count]);
	}

	if (count != 0)
	{
		contact->SolveVelocityConstraints(rigidBody1, rigidBody2, count, constraint);
	}

	if (Fabs(contact->GetAppliedImpulse(kImpulseDelta)) > GetBreakingImpulse())
	{
		HandleBrokenJoint();
	}

	rigidBody1->Integrate();
	rigidBody2->Integrate();
}

void RevoluteJoint::ApplyPositionConstraints(ConstraintContact *contact)
{
	Constraint		constraint[2];

	RigidBodyController *rigidBody1 = static_cast<RigidBodyController *>(contact->GetStartElement());
	RigidBodyController *rigidBody2 = static_cast<RigidBodyController *>(contact->GetFinishElement());
	Vector3D jointPosition1 = rigidBody1->GetFinalTransform() * GetBodyJointPosition(0);
	Vector3D jointPosition2 = rigidBody2->GetFinalTransform() * GetBodyJointPosition(1);
	Vector3D jointDirection1 = jointPosition1 - rigidBody1->GetWorldCenterOfMass();
	Vector3D jointDirection2 = jointPosition2 - rigidBody2->GetWorldCenterOfMass();

	int32 count = 0;

	Vector3D deltaPosition = jointPosition2 - jointPosition1;
	float d = SquaredMag(deltaPosition);
	if (d > K::min_float)
	{
		float f = InverseSqrt(d);
		deltaPosition *= f;

		Vector3D jointAxis1 = rigidBody1->GetFinalTransform() * GetBodyJointAxis(0);
		Vector3D jointAxis2 = rigidBody2->GetFinalTransform() * GetBodyJointAxis(1);
		float weight1 = rigidBody2->GetInverseBodyMass();
		float weight2 = rigidBody1->GetInverseBodyMass();
		Vector3D zdir = Normalize(jointAxis1 * weight1 + jointAxis2 * weight2);

		constraint[0].row[0].Set(-deltaPosition, ProjectOnto(deltaPosition % jointDirection1, zdir));
		constraint[0].row[1].Set(deltaPosition, ProjectOnto(jointDirection2 % deltaPosition, zdir));
		constraint[0].bias = d * f * kJointStabilizeFactor;
		count = 1;
	}

	Vector3D jointAxis1 = rigidBody1->GetFinalTransform() * GetBodyJointAxis(0);
	Vector3D jointAxis2 = rigidBody2->GetFinalTransform() * GetBodyJointAxis(1);

	Vector3D rotateAxis = jointAxis1 % jointAxis2;
	d = SquaredMag(rotateAxis);
	if (d > K::min_float)
	{
		float f = InverseSqrt(d);
		rotateAxis *= f;

		constraint[count].row[0].Set(Zero3D, -rotateAxis);
		constraint[count].row[1].Set(Zero3D, rotateAxis);
		constraint[count].bias = d * f * kJointStabilizeFactor;
		count++;
	}

	if (count != 0)
	{
		ConstraintContact::SolvePositionConstraints(rigidBody1, rigidBody2, count, constraint);
	}

	if (GetLimitAngleFlag())
	{
		ApplyAngleLimitPositionConstraint(contact, rigidBody1, rigidBody2, jointDirection1, jointDirection2);
	}

	rigidBody1->Integrate();
	rigidBody2->Integrate();
}


CylindricalJoint::CylindricalJoint(void *) : DiscalJoint(kJointCylindrical)
{
}

CylindricalJoint::CylindricalJoint() : DiscalJoint(kJointCylindrical)
{
	SetNewObject(new CylindricalJointObject);
}

CylindricalJoint::CylindricalJoint(JointType type) : DiscalJoint(type)
{
}

CylindricalJoint::CylindricalJoint(const CylindricalJoint& cylindricalJoint) : DiscalJoint(cylindricalJoint)
{
}

CylindricalJoint::~CylindricalJoint()
{
}

Node *CylindricalJoint::Replicate(void) const
{
	return (new CylindricalJoint(*this));
}

void CylindricalJoint::ApplyStaticVelocityConstraints(ConstraintContact *contact)
{
	/*	A = Dynamic joint position, relative to static joint position
		B = Center of mass of the rigid body, relative to static joint position
		V = Current linear velocity of the rigid body
		ω = Current angular velocity of the rigid body
		Z = Static z axis of the joint

		The position constraint C for a one-body cylindrical joint is given by:

			C = |A - (A ⋅ Z)Z| = 0

		This means that the point A lies on the line passing through the static joint position with the direction Z.

		Define D as:

			      A - (A ⋅ Z)Z
			D = ────────────────
			     |A - (A ⋅ Z)Z|

		The derivative dA/dt is given by V + ω ⨯ (A - B), so the velocity constraint is:

			 dC
			──── = D ⋅ [V + ω ⨯ (A - B) - (V ⋅ Z)Z - ((ω ⨯ (A - B)) ⋅ Z)Z] = 0
			 dt

		The last two terms are zero when multiplied by A - (A ⋅ Z)Z, so this simplifies to:

			 dC
			──── = D ⋅ [V + ω ⨯ (A - B)] = 0
			 dt

		After applying a triple product identity to the angular term, this is written in Jacobian form as follows:

			 dC
			──── = D ⋅ V + [(A - B) ⨯ D] ⋅ ω = 0
			 dt

		There is the additional constraint that the rigid body rotates only in the plane perpendicular to
		the static joint axis. The corresponding velocity constraints are the following in Jacobian form:

			 dC      (A - B) ⨯ Z
			──── = ─────────────── ⋅ ω = 0
			 dt     |(A - B) ⨯ Z|

			 dC      Z ⨯ [(A - B) ⨯ Z]
			──── = ──────────────────── ⋅ ω = 0
			 dt        |(A - B) ⨯ Z|
	*/

	StaticConstraint	constraint[4];

	RigidBodyController *rigidBody = static_cast<RigidBodyController *>(contact->GetStartElement());
	Vector3D jointPosition = rigidBody->GetFinalTransform() * GetBodyJointPosition(0);
	Vector3D jointDirection = jointPosition - rigidBody->GetWorldCenterOfMass();
	jointPosition -= GetWorldPosition();

	int32 count = 0;

	const Vector3D& zdir = GetWorldTransform()[2];
	float p = jointPosition * zdir;
	float d = SquaredMag(jointPosition) - p * p;
	if (d > K::min_float)
	{
		Vector3D deltaPosition = (jointPosition - zdir * p) * InverseSqrt(d);

		constraint[0].row.Set(deltaPosition, jointDirection % deltaPosition);
		constraint[0].bias = 0.0F;
		constraint[0].index = kImpulseDelta;
		constraint[0].range.Set(K::minus_infinity, K::infinity);
		count = 1;
	}

	Vector3D xdir = jointDirection % zdir;
	d = SquaredMag(xdir);
	if (d > K::min_float)
	{
		xdir *= InverseSqrt(d);
		Vector3D ydir = zdir % xdir;

		constraint[count].row.Set(Zero3D, xdir);
		constraint[count].bias = 0.0F;
		constraint[count].index = kImpulseRotationX;
		constraint[count].range.Set(K::minus_infinity, K::infinity);
		count++;

		constraint[count].row.Set(Zero3D, ydir);
		constraint[count].bias = 0.0F;
		constraint[count].index = kImpulseRotationY;
		constraint[count].range.Set(K::minus_infinity, K::infinity);
		count++;
	}

	if (GetLimitAngleFlag())
	{
		count += GetAngleLimitStaticVelocityConstraint(contact, rigidBody, jointDirection, &constraint[count]);
	}

	if (count != 0)
	{
		contact->SolveVelocityConstraints(rigidBody, count, constraint);
	}

	if (GetLimitDistanceFlag())
	{
		ApplyDistanceLimitStaticVelocityConstraint(contact, rigidBody, jointPosition);
	}

	if (Fabs(contact->GetAppliedImpulse(kImpulseDelta)) > GetBreakingImpulse())
	{
		HandleBrokenJoint();
	}

	rigidBody->Integrate();
}

void CylindricalJoint::ApplyStaticPositionConstraints(ConstraintContact *contact)
{
	StaticConstraint	constraint[2];

	RigidBodyController *rigidBody = static_cast<RigidBodyController *>(contact->GetStartElement());
	Vector3D jointPosition = rigidBody->GetFinalTransform() * GetBodyJointPosition(0);
	Vector3D jointDirection = jointPosition - rigidBody->GetWorldCenterOfMass();
	jointPosition -= GetWorldPosition();

	int32 count = 0;

	const Vector3D& zdir = GetWorldTransform()[2];
	float p = jointPosition * zdir;
	float d = SquaredMag(jointPosition) - p * p;
	if (d > K::min_float)
	{
		float f = InverseSqrt(d);
		Vector3D deltaPosition = (jointPosition - zdir * p) * f;

		constraint[0].row.Set(deltaPosition, jointDirection % deltaPosition);
		constraint[0].bias = d * f * kStaticJointStabilizeFactor;
		count = 1;
	}

	Vector3D jointAxis = rigidBody->GetFinalTransform() * GetBodyJointAxis(0);

	Vector3D rotateAxis = zdir % jointAxis;
	d = SquaredMag(rotateAxis);
	if (d > K::min_float)
	{
		float f = InverseSqrt(d);
		rotateAxis *= f;

		constraint[count].row.Set(Zero3D, rotateAxis);
		constraint[count].bias = d * f * kStaticJointStabilizeFactor;
		count++;
	}

	if (count != 0)
	{
		ConstraintContact::SolvePositionConstraints(rigidBody, count, constraint);
	}

	if (GetLimitDistanceFlag())
	{
		ApplyDistanceLimitStaticPositionConstraint(contact, rigidBody, jointPosition);
	}

	if (GetLimitAngleFlag())
	{
		ApplyAngleLimitStaticPositionConstraint(contact, rigidBody, jointDirection);
	}

	rigidBody->Integrate();
}

void CylindricalJoint::ApplyVelocityConstraints(ConstraintContact *contact)
{
	/*	A₁ = Dynamic joint position for rigid body 1
		A₂ = Dynamic joint position for rigid body 2
		B₁ = Center of mass of rigid body 1
		B₂ = Center of mass of rigid body 2
		V₁ = Current linear velocity of rigid body 1
		V₂ = Current linear velocity of rigid body 2
		ω₁ = Current angular velocity of rigid body 1
		ω₂ = Current angular velocity of rigid body 2
		Z = Weighted average z axis of the joint

		The position constraint C for a two-body cylindrical joint is given by:

			C = |(A₂ - A₁) - ((A₂ - A₁) ⋅ Z)Z| = 0

		Define D as:

			      (A₂ - A₁) - ((A₂ - A₁) ⋅ Z)Z
			D = ───────────────────────────────
			     |(A₂ - A₁) - ((A₂ - A₁) ⋅ Z)Z|

		As in the one-body case, the velocity constraint simplifies and becomes the following in Jacobian form:

			 dC
			──── = D ⋅ V₂ + [(A₂ - B₂) ⨯ D] ⋅ ω₂ - D ⋅ V₁ + [D ⨯ (A₁ - B₁)] ⋅ ω₁ = 0
			 dt
	*/

	Constraint		constraint[4];

	RigidBodyController *rigidBody1 = static_cast<RigidBodyController *>(contact->GetStartElement());
	RigidBodyController *rigidBody2 = static_cast<RigidBodyController *>(contact->GetFinishElement());
	Point3D jointPosition1 = rigidBody1->GetFinalTransform() * GetBodyJointPosition(0);
	Point3D jointPosition2 = rigidBody2->GetFinalTransform() * GetBodyJointPosition(1);
	Vector3D jointDirection1 = jointPosition1 - rigidBody1->GetWorldCenterOfMass();
	Vector3D jointDirection2 = jointPosition2 - rigidBody2->GetWorldCenterOfMass();
	Vector3D deltaPosition = jointPosition2 - jointPosition1;

	int32 count = 0;

	Vector3D jointAxis1 = rigidBody1->GetFinalTransform() * GetBodyJointAxis(0);
	Vector3D jointAxis2 = rigidBody2->GetFinalTransform() * GetBodyJointAxis(1);
	float weight1 = rigidBody2->GetInverseBodyMass();
	float weight2 = rigidBody1->GetInverseBodyMass();
	Vector3D zdir = Normalize(jointAxis1 * weight1 + jointAxis2 * weight2);

	float p = deltaPosition * zdir;
	float d = SquaredMag(deltaPosition) - p * p;
	if (d > K::min_float)
	{
		float f = InverseSqrt(d);
		deltaPosition = (deltaPosition - zdir * p) * f;

		constraint[0].row[0].Set(-deltaPosition, deltaPosition % jointDirection1);
		constraint[0].row[1].Set(deltaPosition, jointDirection2 % deltaPosition);
		constraint[0].bias = 0.0F;
		constraint[0].index = kImpulseDelta;
		constraint[0].range.Set(K::minus_infinity, K::infinity);
		count = 1;
	}

	Vector3D xdir = (zdir % jointDirection1) * weight1 + (jointDirection2 % zdir) * weight2;
	d = SquaredMag(xdir);
	if (d > K::min_float)
	{
		xdir *= InverseSqrt(d);
		Vector3D ydir = zdir % xdir;

		constraint[count].row[0].Set(Zero3D, -xdir);
		constraint[count].row[1].Set(Zero3D, xdir);
		constraint[count].bias = 0.0F;
		constraint[count].index = kImpulseRotationX;
		constraint[count].range.Set(K::minus_infinity, K::infinity);
		count++;

		constraint[count].row[0].Set(Zero3D, -ydir);
		constraint[count].row[1].Set(Zero3D, ydir);
		constraint[count].bias = 0.0F;
		constraint[count].index = kImpulseRotationY;
		constraint[count].range.Set(K::minus_infinity, K::infinity);
		count++;
	}

	if (GetLimitAngleFlag())
	{
		count += GetAngleLimitVelocityConstraint(contact, rigidBody1, rigidBody2, jointDirection1, jointDirection2, &constraint[count]);
	}

	if (count != 0)
	{
		contact->SolveVelocityConstraints(rigidBody1, rigidBody2, count, constraint);
	}

	if (GetLimitDistanceFlag())
	{
		ApplyDistanceLimitVelocityConstraint(contact, rigidBody1, rigidBody2, zdir, deltaPosition);
	}

	if (Fabs(contact->GetAppliedImpulse(kImpulseDelta)) > GetBreakingImpulse())
	{
		HandleBrokenJoint();
	}

	rigidBody1->Integrate();
	rigidBody2->Integrate();
}

void CylindricalJoint::ApplyPositionConstraints(ConstraintContact *contact)
{
	Constraint		constraint[2];

	RigidBodyController *rigidBody1 = static_cast<RigidBodyController *>(contact->GetStartElement());
	RigidBodyController *rigidBody2 = static_cast<RigidBodyController *>(contact->GetFinishElement());
	Point3D jointPosition1 = rigidBody1->GetFinalTransform() * GetBodyJointPosition(0);
	Point3D jointPosition2 = rigidBody2->GetFinalTransform() * GetBodyJointPosition(1);
	Vector3D jointDirection1 = jointPosition1 - rigidBody1->GetWorldCenterOfMass();
	Vector3D jointDirection2 = jointPosition2 - rigidBody2->GetWorldCenterOfMass();

	int32 count = 0;

	Vector3D jointAxis1 = rigidBody1->GetFinalTransform() * GetBodyJointAxis(0);
	Vector3D jointAxis2 = rigidBody2->GetFinalTransform() * GetBodyJointAxis(1);
	float weight1 = rigidBody2->GetInverseBodyMass();
	float weight2 = rigidBody1->GetInverseBodyMass();
	Vector3D zdir = Normalize(jointAxis1 * weight1 + jointAxis2 * weight2);

	Vector3D deltaPosition = jointPosition2 - jointPosition1;
	float p = deltaPosition * zdir;
	float d = SquaredMag(deltaPosition) - p * p;
	if (d > K::min_float)
	{
		float f = InverseSqrt(d);
		deltaPosition = (deltaPosition - zdir * p) * f;

		constraint[0].row[0].Set(-deltaPosition, deltaPosition % jointDirection1);
		constraint[0].row[1].Set(deltaPosition, jointDirection2 % deltaPosition);
		constraint[0].bias = d * f * kJointStabilizeFactor;
		count = 1;
	}

	Vector3D rotateAxis = jointAxis1 % jointAxis2;
	d = SquaredMag(rotateAxis);
	if (d > K::min_float)
	{
		float f = InverseSqrt(d);
		rotateAxis *= f;

		constraint[count].row[0].Set(Zero3D, -rotateAxis);
		constraint[count].row[1].Set(Zero3D, rotateAxis);
		constraint[count].bias = d * f * kJointStabilizeFactor;
		count++;
	}

	if (count != 0)
	{
		ConstraintContact::SolvePositionConstraints(rigidBody1, rigidBody2, count, constraint);
	}

	if (GetLimitDistanceFlag())
	{
		ApplyDistanceLimitPositionConstraint(contact, rigidBody1, rigidBody2, zdir, deltaPosition);
	}

	if (GetLimitAngleFlag())
	{
		ApplyAngleLimitPositionConstraint(contact, rigidBody1, rigidBody2, jointDirection1, jointDirection2);
	}

	rigidBody1->Integrate();
	rigidBody2->Integrate();
}

void CylindricalJoint::ApplyDistanceLimitStaticVelocityConstraint(ConstraintContact *contact, RigidBodyController *rigidBody, const Vector3D& deltaPosition)
{
	const Vector3D& zdir = GetWorldTransform()[2];
	Vector3D axisDeltaPosition = ProjectOnto(deltaPosition, zdir);

	float limit = GetDistanceLimit();
	float d = SquaredMag(axisDeltaPosition);
	if (d > limit * limit)
	{
		StaticConstraint	constraint;

		constraint.row.Set(zdir, Zero3D);
		constraint.bias = 0.0F;

		float limitImpulse = ConstraintContact::CalculateVelocityImpulse(rigidBody, &constraint);
		limitImpulse = contact->AccumulateConstraintImpulse(kImpulseDistanceLimit, limitImpulse, Range<float>(K::minus_infinity, K::infinity));
		rigidBody->ApplyLinearVelocityCorrection(constraint.row.linear, limitImpulse);
	}
}

void CylindricalJoint::ApplyDistanceLimitStaticPositionConstraint(ConstraintContact *contact, RigidBodyController *rigidBody, const Vector3D& deltaPosition)
{
	const Vector3D& zdir = GetWorldTransform()[2];
	Vector3D axisDeltaPosition = ProjectOnto(deltaPosition, zdir);

	float limit = GetDistanceLimit();
	float d = SquaredMag(axisDeltaPosition);
	if (d > limit * limit)
	{
		StaticConstraint	constraint;

		constraint.row.Set(zdir, Zero3D);
		constraint.bias = (d * InverseSqrt(d) - limit) * NonzeroFsgn(axisDeltaPosition * zdir);

		float limitImpulse = ConstraintContact::CalculatePositionImpulse(rigidBody, &constraint);
		rigidBody->ApplyLinearPositionCorrection(constraint.row.linear, limitImpulse);
	}
}

void CylindricalJoint::ApplyDistanceLimitVelocityConstraint(ConstraintContact *contact, RigidBodyController *rigidBody1, RigidBodyController *rigidBody2, const Vector3D& zdir, const Vector3D& deltaPosition)
{
	Vector3D axisDeltaPosition = ProjectOnto(deltaPosition, zdir);

	float limit = GetDistanceLimit();
	float d = SquaredMag(axisDeltaPosition);
	if (d > limit * limit)
	{
		Constraint		constraint;

		constraint.row[0].Set(-zdir, Zero3D);
		constraint.row[1].Set(zdir, Zero3D);
		constraint.bias = 0.0F;

		float limitImpulse = ConstraintContact::CalculateVelocityImpulse(rigidBody1, rigidBody2, &constraint);
		limitImpulse = contact->AccumulateConstraintImpulse(kImpulseDistanceLimit, limitImpulse, Range<float>(K::minus_infinity, K::infinity));
		rigidBody1->ApplyLinearVelocityCorrection(constraint.row[0].linear, limitImpulse);
		rigidBody2->ApplyLinearVelocityCorrection(constraint.row[1].linear, limitImpulse);
	}
}

void CylindricalJoint::ApplyDistanceLimitPositionConstraint(ConstraintContact *contact, RigidBodyController *rigidBody1, RigidBodyController *rigidBody2, const Vector3D& zdir, const Vector3D& deltaPosition)
{
	Vector3D axisDeltaPosition = ProjectOnto(deltaPosition, zdir);

	float limit = GetDistanceLimit();
	float d = SquaredMag(axisDeltaPosition);
	if (d > limit * limit)
	{
		Constraint		constraint;

		constraint.row[0].Set(-zdir, Zero3D);
		constraint.row[1].Set(zdir, Zero3D);
		constraint.bias = (d * InverseSqrt(d) - limit) * NonzeroFsgn(axisDeltaPosition * zdir);

		float limitImpulse = ConstraintContact::CalculatePositionImpulse(rigidBody1, rigidBody2, &constraint);
		rigidBody1->ApplyPositionCorrection(constraint.row[0], limitImpulse);
		rigidBody2->ApplyPositionCorrection(constraint.row[1], limitImpulse);
	}
}


PrismaticJoint::PrismaticJoint(void *) : CylindricalJoint(kJointPrismatic)
{
}

PrismaticJoint::PrismaticJoint() : CylindricalJoint(kJointPrismatic)
{
	SetNewObject(new PrismaticJointObject);
}

PrismaticJoint::PrismaticJoint(const PrismaticJoint& prismaticJoint) : CylindricalJoint(prismaticJoint)
{
}

PrismaticJoint::~PrismaticJoint()
{
}

Node *PrismaticJoint::Replicate(void) const
{
	return (new PrismaticJoint(*this));
}

void PrismaticJoint::ApplyStaticVelocityConstraints(ConstraintContact *contact)
{
	StaticConstraint	constraint[4];

	RigidBodyController *rigidBody = static_cast<RigidBodyController *>(contact->GetStartElement());
	Vector3D jointPosition = rigidBody->GetFinalTransform() * GetBodyJointPosition(0);
	Vector3D jointDirection = jointPosition - rigidBody->GetWorldCenterOfMass();
	jointPosition -= GetWorldPosition();

	int32 count = 0;

	const Vector3D& zdir = GetWorldTransform()[2];
	float p = jointPosition * zdir;
	float d = SquaredMag(jointPosition) - p * p;
	if (d > K::min_float)
	{
		Vector3D deltaPosition = (jointPosition - zdir * p) * InverseSqrt(d);

		constraint[0].row.Set(deltaPosition, jointDirection % deltaPosition);
		constraint[0].bias = 0.0F;
		constraint[0].index = kImpulseDelta;
		constraint[0].range.Set(K::minus_infinity, K::infinity);
		count = 1;
	}

	const Vector3D& xdir = GetWorldTransform()[0];
	const Vector3D& ydir = GetWorldTransform()[1];

	constraint[count].row.Set(Zero3D, xdir);
	constraint[count].bias = 0.0F;
	constraint[count].index = kImpulseRotationX;
	constraint[count].range.Set(K::minus_infinity, K::infinity);
	count++;

	constraint[count].row.Set(Zero3D, ydir);
	constraint[count].bias = 0.0F;
	constraint[count].index = kImpulseRotationY;
	constraint[count].range.Set(K::minus_infinity, K::infinity);
	count++;

	constraint[count].row.Set(Zero3D, zdir);
	constraint[count].bias = 0.0F;
	constraint[count].index = kImpulseRotationZ;
	constraint[count].range.Set(K::minus_infinity, K::infinity);
	count++;

	contact->SolveVelocityConstraints(rigidBody, count, constraint);

	if (GetLimitDistanceFlag())
	{
		ApplyDistanceLimitStaticVelocityConstraint(contact, rigidBody, jointPosition);
	}

	if (Fabs(contact->GetAppliedImpulse(kImpulseDelta)) > GetBreakingImpulse())
	{
		HandleBrokenJoint();
	}

	rigidBody->Integrate();
}

void PrismaticJoint::ApplyStaticPositionConstraints(ConstraintContact *contact)
{
	StaticConstraint	constraint[3];

	RigidBodyController *rigidBody = static_cast<RigidBodyController *>(contact->GetStartElement());
	Vector3D jointPosition = rigidBody->GetFinalTransform() * GetBodyJointPosition(0) - GetWorldPosition();

	int32 count = 0;

	const Vector3D& zdir = GetWorldTransform()[2];
	float p = jointPosition * zdir;
	float d = SquaredMag(jointPosition) - p * p;
	if (d > K::min_float)
	{
		float f = InverseSqrt(d);
		Vector3D deltaPosition = (jointPosition - zdir * p) * f;

		constraint[0].row.Set(deltaPosition, Zero3D);
		constraint[0].bias = d * f * kStaticJointStabilizeFactor;
		count = 1;
	}

	Vector3D jointAxis = rigidBody->GetFinalTransform() * GetBodyJointAxis(0);

	Vector3D rotateAxis = zdir % jointAxis;
	d = SquaredMag(rotateAxis);
	if (d > K::min_float)
	{
		float f = InverseSqrt(d);
		rotateAxis *= f;

		constraint[count].row.Set(Zero3D, rotateAxis);
		constraint[count].bias = d * f * kStaticJointStabilizeFactor;
		count++;
	}

	Vector3D jointTangent = rigidBody->GetFinalTransform() * GetBodyJointTangent(0);
	jointTangent -= ProjectOnto(jointTangent, zdir);

	const Vector3D& xdir = GetWorldTransform()[0];
	rotateAxis = xdir % jointTangent;
	d = SquaredMag(rotateAxis);
	if (d > K::min_float)
	{
		float f = InverseSqrt(d);
		rotateAxis *= f;

		constraint[count].row.Set(Zero3D, rotateAxis);
		constraint[count].bias = d * f * kStaticJointStabilizeFactor;
		count++;
	}

	if (count != 0)
	{
		ConstraintContact::SolvePositionConstraints(rigidBody, count, constraint);
	}

	if (GetLimitDistanceFlag())
	{
		ApplyDistanceLimitStaticPositionConstraint(contact, rigidBody, jointPosition);
	}

	rigidBody->Integrate();
}

void PrismaticJoint::ApplyVelocityConstraints(ConstraintContact *contact)
{
	Constraint		constraint[4];

	RigidBodyController *rigidBody1 = static_cast<RigidBodyController *>(contact->GetStartElement());
	RigidBodyController *rigidBody2 = static_cast<RigidBodyController *>(contact->GetFinishElement());
	Point3D jointPosition1 = rigidBody1->GetFinalTransform() * GetBodyJointPosition(0);
	Point3D jointPosition2 = rigidBody2->GetFinalTransform() * GetBodyJointPosition(1);
	Vector3D jointDirection1 = jointPosition1 - rigidBody1->GetWorldCenterOfMass();
	Vector3D jointDirection2 = jointPosition2 - rigidBody2->GetWorldCenterOfMass();
	Vector3D deltaPosition = jointPosition2 - jointPosition1;

	int32 count = 0;

	Vector3D jointAxis1 = rigidBody1->GetFinalTransform() * GetBodyJointAxis(0);
	Vector3D jointAxis2 = rigidBody2->GetFinalTransform() * GetBodyJointAxis(1);
	float weight1 = rigidBody2->GetInverseBodyMass();
	float weight2 = rigidBody1->GetInverseBodyMass();
	Vector3D zdir = Normalize(jointAxis1 * weight1 + jointAxis2 * weight2);

	float p = deltaPosition * zdir;
	float d = SquaredMag(deltaPosition) - p * p;
	if (d > K::min_float)
	{
		float f = InverseSqrt(d);
		deltaPosition = (deltaPosition - zdir * p) * f;

		constraint[0].row[0].Set(-deltaPosition, deltaPosition % jointDirection1);
		constraint[0].row[1].Set(deltaPosition, jointDirection2 % deltaPosition);
		constraint[0].bias = 0.0F;
		constraint[0].index = kImpulseDelta;
		constraint[0].range.Set(K::minus_infinity, K::infinity);
		count = 1;
	}

	Vector3D jointTangent1 = rigidBody1->GetFinalTransform() * GetBodyJointTangent(0);
	Vector3D jointTangent2 = rigidBody2->GetFinalTransform() * GetBodyJointTangent(1);

	Vector3D xdir = (zdir % jointTangent1) * weight1 + (zdir % jointTangent2) * weight2;
	d = SquaredMag(xdir);
	if (d > K::min_float)
	{
		Vector3D ydir = zdir % xdir * InverseSqrt(d);
		xdir = ydir % zdir;

		constraint[count].row[0].Set(Zero3D, -xdir);
		constraint[count].row[1].Set(Zero3D, xdir);
		constraint[count].bias = 0.0F;
		constraint[count].index = kImpulseRotationX;
		constraint[count].range.Set(K::minus_infinity, K::infinity);
		count++;

		constraint[count].row[0].Set(Zero3D, -ydir);
		constraint[count].row[1].Set(Zero3D, ydir);
		constraint[count].bias = 0.0F;
		constraint[count].index = kImpulseRotationY;
		constraint[count].range.Set(K::minus_infinity, K::infinity);
		count++;

		constraint[count].row[0].Set(Zero3D, -zdir);
		constraint[count].row[1].Set(Zero3D, zdir);
		constraint[count].bias = 0.0F;
		constraint[count].index = kImpulseRotationZ;
		constraint[count].range.Set(K::minus_infinity, K::infinity);
		count++;
	}

	if (count != 0)
	{
		contact->SolveVelocityConstraints(rigidBody1, rigidBody2, count, constraint);
	}

	if (GetLimitDistanceFlag())
	{
		ApplyDistanceLimitVelocityConstraint(contact, rigidBody1, rigidBody2, zdir, deltaPosition);
	}

	if (Fabs(contact->GetAppliedImpulse(kImpulseDelta)) > GetBreakingImpulse())
	{
		HandleBrokenJoint();
	}

	rigidBody1->Integrate();
	rigidBody2->Integrate();
}

void PrismaticJoint::ApplyPositionConstraints(ConstraintContact *contact)
{
	Constraint		constraint[3];

	RigidBodyController *rigidBody1 = static_cast<RigidBodyController *>(contact->GetStartElement());
	RigidBodyController *rigidBody2 = static_cast<RigidBodyController *>(contact->GetFinishElement());
	Point3D jointPosition1 = rigidBody1->GetFinalTransform() * GetBodyJointPosition(0);
	Point3D jointPosition2 = rigidBody2->GetFinalTransform() * GetBodyJointPosition(1);

	int32 count = 0;

	Vector3D jointAxis1 = rigidBody1->GetFinalTransform() * GetBodyJointAxis(0);
	Vector3D jointAxis2 = rigidBody2->GetFinalTransform() * GetBodyJointAxis(1);
	float weight1 = rigidBody2->GetInverseBodyMass();
	float weight2 = rigidBody1->GetInverseBodyMass();
	Vector3D zdir = Normalize(jointAxis1 * weight1 + jointAxis2 * weight2);

	Vector3D deltaPosition = jointPosition2 - jointPosition1;
	float p = deltaPosition * zdir;
	float d = SquaredMag(deltaPosition) - p * p;
	if (d > K::min_float)
	{
		float f = InverseSqrt(d);
		deltaPosition = (deltaPosition - zdir * p) * f;

		constraint[0].row[0].Set(-deltaPosition, Zero3D);
		constraint[0].row[1].Set(deltaPosition, Zero3D);
		constraint[0].bias = d * f * kJointStabilizeFactor;
		count = 1;
	}

	Vector3D rotateAxis = jointAxis1 % jointAxis2;
	d = SquaredMag(rotateAxis);
	if (d > K::min_float)
	{
		float f = InverseSqrt(d);
		rotateAxis *= f;

		constraint[count].row[0].Set(Zero3D, -rotateAxis);
		constraint[count].row[1].Set(Zero3D, rotateAxis);
		constraint[count].bias = d * f * kJointStabilizeFactor;
		count++;
	}

	Vector3D jointTangent1 = rigidBody1->GetFinalTransform() * GetBodyJointTangent(0);
	Vector3D jointTangent2 = rigidBody2->GetFinalTransform() * GetBodyJointTangent(1);
	jointTangent1 -= ProjectOnto(jointTangent1, zdir);
	jointTangent2 -= ProjectOnto(jointTangent2, zdir);

	rotateAxis = jointTangent1 % jointTangent2;
	d = SquaredMag(rotateAxis);
	if (d > K::min_float)
	{
		float f = InverseSqrt(d);
		rotateAxis *= f;

		constraint[count].row[0].Set(Zero3D, -rotateAxis);
		constraint[count].row[1].Set(Zero3D, rotateAxis);
		constraint[count].bias = d * f * kJointStabilizeFactor;
		count++;
	}

	if (count != 0)
	{
		ConstraintContact::SolvePositionConstraints(rigidBody1, rigidBody2, count, constraint);
	}

	if (GetLimitDistanceFlag())
	{
		ApplyDistanceLimitPositionConstraint(contact, rigidBody1, rigidBody2, zdir, deltaPosition);
	}

	rigidBody1->Integrate();
	rigidBody2->Integrate();
}

// ZYUQURM
