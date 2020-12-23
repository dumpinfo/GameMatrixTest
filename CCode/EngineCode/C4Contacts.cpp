 

#include "C4Contacts.h"
#include "C4Simulation.h"
#include "C4Joints.h"
#include "C4World.h"


using namespace C4;


namespace C4
{
	template <> Heap Memory<Contact>::heap("Contact", 65536);
	template class Memory<Contact>;
}


Contact::Contact(ContactType type, Body *body1, Body *body2, unsigned_int32 mask) : GraphEdge<Body, Contact>(body1, body2)
{
	contactType = type;

	enabledFlag = true;
	deadFlag = false;

	notificationMask = mask;
	contactParam = 1.0F;
	solverMultiplier = 1;
}

Contact::~Contact()
{
}

Contact *Contact::Create(Unpacker& data, unsigned_int32 unpackFlags, Body *nullBody)
{
	switch (data.GetType())
	{
		case kContactRigidBody:

			return (new RigidBodyContact(nullBody));

		case kContactGeometry:

			return (new GeometryContact(nullBody));

		case kContactJoint:

			return (new JointContact(nullBody));

		case kContactStaticJoint:

			return (new StaticJointContact(nullBody));
	}

	return (nullptr);
}

void Contact::PackType(Packer& data) const
{
	data << contactType;
}

void Contact::Pack(Packer& data, unsigned_int32 packFlags) const
{
	data << ChunkHeader('EFLG', 4);
	data << enabledFlag;

	data << ChunkHeader('SLVM', 4);
	data << solverMultiplier;

	const Body *body = GetStartElement();
	if (body->GetBodyType() != kBodyNull)
	{
		const Node *node = static_cast<const BodyController *>(body)->GetTargetNode();
		if (node->LinkedNodePackable(packFlags))
		{
			data << ChunkHeader('STRT', 4);
			data << node->GetNodeIndex();
		}
	}

	body = GetFinishElement();
	if (body->GetBodyType() != kBodyNull)
	{
		const Node *node = static_cast<const BodyController *>(body)->GetTargetNode();
		if (node->LinkedNodePackable(packFlags))
		{
			data << ChunkHeader('FNSH', 4);
			data << node->GetNodeIndex();
		}
	}

	data << TerminatorChunk;
}

void Contact::Unpack(Unpacker& data, unsigned_int32 unpackFlags)
{
	UnpackChunkList<Contact>(data, unpackFlags);
}

bool Contact::UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags)
{ 
	switch (chunkHeader->chunkType)
	{
		case 'EFLG': 

			data >> enabledFlag; 
			return (true);

		case 'SLVM': 

			data >> solverMultiplier; 
			return (true); 

		case 'STRT':
		{
			int32	nodeIndex; 

			data >> nodeIndex;
			data.AddNodeLink(nodeIndex, &StartBodyLinkProc, this);
			return (true);
		}

		case 'FNSH':
		{
			int32	nodeIndex;

			data >> nodeIndex;
			data.AddNodeLink(nodeIndex, &FinishBodyLinkProc, this);
			return (true);
		}
	}

	return (false);
}

void Contact::StartBodyLinkProc(Node *node, void *cookie)
{
	Contact *contact = static_cast<Contact *>(cookie);
	contact->SetStartElement(static_cast<BodyController *>(node->GetController()));
}

void Contact::FinishBodyLinkProc(Node *node, void *cookie)
{
	Contact *contact = static_cast<Contact *>(cookie);
	contact->SetFinishElement(static_cast<BodyController *>(node->GetController()));
}

bool Contact::NonpersistentFinishNode(void) const
{
	return (false);
}

void Contact::InitializeConstraints(void)
{
}

void Contact::InitializeRepeatConstraints(void)
{
}

void Contact::ApplyVelocityConstraints(void)
{
}

void Contact::ApplyFrictionConstraints(void)
{
}

void Contact::ApplyPositionConstraints(void)
{
}


ConstraintContact::ConstraintContact(ContactType type, Body *body1, Body *body2, unsigned_int32 mask) : Contact(type, body1, body2, mask)
{
}

ConstraintContact::~ConstraintContact()
{
}

void ConstraintContact::InitializeConstraints(void)
{
	for (machine a = 0; a < kMaxContactConstraintCount; a++)
	{
		cumulativeImpulse[a] = 0.0F;
		appliedImpulse[a] = 0.0F;
	}
}

void ConstraintContact::InitializeRepeatConstraints(void)
{
	for (machine a = 0; a < kMaxContactConstraintCount; a++)
	{
		cumulativeImpulse[a] = 0.0F;
		appliedImpulse[a] = 0.0F;
	}
}

float ConstraintContact::CalculateVelocityImpulse(const RigidBodyController *rigidBody, const StaticConstraint *constraint)
{
	const Vector3D& linearVelocity = rigidBody->GetLinearVelocity();
	const Antivector3D& angularVelocity = rigidBody->GetAngularVelocity();

	float inverseMass = rigidBody->GetInverseBodyMass();
	const InertiaTensor& inverseInertiaTensor = rigidBody->GetInverseWorldInertiaTensor();

	return (-(constraint->row.linear * linearVelocity + constraint->row.angular * angularVelocity + constraint->bias) / (constraint->row.linear * constraint->row.linear * inverseMass + constraint->row.angular * (inverseInertiaTensor * constraint->row.angular)));
}

Vector2D ConstraintContact::CalculateVelocityImpulse2D(const RigidBodyController *rigidBody, const StaticConstraint *constraint)
{
	const Vector3D& linearVelocity = rigidBody->GetLinearVelocity();
	const Antivector3D& angularVelocity = rigidBody->GetAngularVelocity();

	float inverseMass = rigidBody->GetInverseBodyMass();
	const InertiaTensor& inverseInertiaTensor = rigidBody->GetInverseWorldInertiaTensor();

	Jacobian column1(inverseMass, inverseInertiaTensor, constraint[0].row);
	Jacobian column2(inverseMass, inverseInertiaTensor, constraint[1].row);

	float m11 = constraint[0].row * column1;
	float m12 = constraint[0].row * column2;
	float m21 = constraint[1].row * column1;
	float m22 = constraint[1].row * column2;
	float d = -1.0F / (m11 * m22 - m12 * m21);

	float v1 = constraint[0].row.linear * linearVelocity + constraint[0].row.angular * angularVelocity + constraint[0].bias;
	float v2 = constraint[1].row.linear * linearVelocity + constraint[1].row.angular * angularVelocity + constraint[1].bias;

	return (Vector2D((m22 * v1 - m21 * v2) * d, (m11 * v2 - m12 * v1) * d));
}

Vector3D ConstraintContact::CalculateVelocityImpulse3D(const RigidBodyController *rigidBody, const StaticConstraint *constraint)
{
	Matrix3D	matrix;
	Vector3D	result;

	const Vector3D& linearVelocity = rigidBody->GetLinearVelocity();
	const Antivector3D& angularVelocity = rigidBody->GetAngularVelocity();

	float inverseMass = rigidBody->GetInverseBodyMass();
	const InertiaTensor& inverseInertiaTensor = rigidBody->GetInverseWorldInertiaTensor();

	for (machine j = 0; j < 3; j++)
	{
		Jacobian column(inverseMass, inverseInertiaTensor, constraint[j].row);

		for (machine i = 0; i < 3; i++)
		{
			matrix(i,j) = constraint[i].row * column;
		}

		result[j] = -(constraint[j].row.linear * linearVelocity + constraint[j].row.angular * angularVelocity + constraint[j].bias);
	}

	return (Inverse(matrix) * result);
}

Vector4D ConstraintContact::CalculateVelocityImpulse4D(const RigidBodyController *rigidBody, const StaticConstraint *constraint)
{
	Matrix4D	matrix;
	Vector4D	result;

	const Vector3D& linearVelocity = rigidBody->GetLinearVelocity();
	const Antivector3D& angularVelocity = rigidBody->GetAngularVelocity();

	float inverseMass = rigidBody->GetInverseBodyMass();
	const InertiaTensor& inverseInertiaTensor = rigidBody->GetInverseWorldInertiaTensor();

	for (machine j = 0; j < 4; j++)
	{
		Jacobian column(inverseMass, inverseInertiaTensor, constraint[j].row);

		for (machine i = 0; i < 4; i++)
		{
			matrix(i,j) = constraint[i].row * column;
		}

		result[j] = -(constraint[j].row.linear * linearVelocity + constraint[j].row.angular * angularVelocity + constraint[j].bias);
	}

	return (Inverse(matrix) * result);
}

float ConstraintContact::CalculateVelocityImpulse(const RigidBodyController *rigidBody1, const RigidBodyController *rigidBody2, const Constraint *constraint)
{
	const Vector3D& linearVelocity1 = rigidBody1->GetLinearVelocity();
	const Antivector3D& angularVelocity1 = rigidBody1->GetAngularVelocity();
	const Vector3D& linearVelocity2 = rigidBody2->GetLinearVelocity();
	const Antivector3D& angularVelocity2 = rigidBody2->GetAngularVelocity();

	float inverseMass1 = rigidBody1->GetInverseBodyMass();
	const InertiaTensor& inverseInertiaTensor1 = rigidBody1->GetInverseWorldInertiaTensor();
	float inverseMass2 = rigidBody2->GetInverseBodyMass();
	const InertiaTensor& inverseInertiaTensor2 = rigidBody2->GetInverseWorldInertiaTensor();

	return (-(constraint->row[0].linear * linearVelocity1 + constraint->row[0].angular * angularVelocity1 + constraint->row[1].linear * linearVelocity2 + constraint->row[1].angular * angularVelocity2 + constraint->bias) / (constraint->row[0].linear * constraint->row[0].linear * inverseMass1 + constraint->row[0].angular * (inverseInertiaTensor1 * constraint->row[0].angular) + constraint->row[1].linear * constraint->row[1].linear * inverseMass2 + constraint->row[1].angular * (inverseInertiaTensor2 * constraint->row[1].angular)));
}

Vector2D ConstraintContact::CalculateVelocityImpulse2D(const RigidBodyController *rigidBody1, const RigidBodyController *rigidBody2, const Constraint *constraint)
{
	const Vector3D& linearVelocity1 = rigidBody1->GetLinearVelocity();
	const Antivector3D& angularVelocity1 = rigidBody1->GetAngularVelocity();
	const Vector3D& linearVelocity2 = rigidBody2->GetLinearVelocity();
	const Antivector3D& angularVelocity2 = rigidBody2->GetAngularVelocity();

	float inverseMass1 = rigidBody1->GetInverseBodyMass();
	const InertiaTensor& inverseInertiaTensor1 = rigidBody1->GetInverseWorldInertiaTensor();
	float inverseMass2 = rigidBody2->GetInverseBodyMass();
	const InertiaTensor& inverseInertiaTensor2 = rigidBody2->GetInverseWorldInertiaTensor();

	Jacobian column11(inverseMass1, inverseInertiaTensor1, constraint[0].row[0]);
	Jacobian column12(inverseMass2, inverseInertiaTensor2, constraint[0].row[1]);
	Jacobian column21(inverseMass1, inverseInertiaTensor1, constraint[1].row[0]);
	Jacobian column22(inverseMass2, inverseInertiaTensor2, constraint[1].row[1]);

	float m11 = constraint[0].row[0] * column11 + constraint[0].row[1] * column12;
	float m12 = constraint[0].row[0] * column21 + constraint[0].row[1] * column22;
	float m21 = constraint[1].row[0] * column11 + constraint[1].row[1] * column12;
	float m22 = constraint[1].row[0] * column21 + constraint[1].row[1] * column22;
	float d = -1.0F / (m11 * m22 - m12 * m21);

	float v1 = constraint[0].row[0].linear * linearVelocity1 + constraint[0].row[0].angular * angularVelocity1 + constraint[0].row[1].linear * linearVelocity2 + constraint[0].row[1].angular * angularVelocity2 + constraint[0].bias;
	float v2 = constraint[1].row[0].linear * linearVelocity1 + constraint[1].row[0].angular * angularVelocity1 + constraint[1].row[1].linear * linearVelocity2 + constraint[1].row[1].angular * angularVelocity2 + constraint[1].bias;

	return (Vector2D((m22 * v1 - m21 * v2) * d, (m11 * v2 - m12 * v1) * d));
}

Vector3D ConstraintContact::CalculateVelocityImpulse3D(const RigidBodyController *rigidBody1, const RigidBodyController *rigidBody2, const Constraint *constraint)
{
	Matrix3D	matrix;
	Vector3D	result;

	const Vector3D& linearVelocity1 = rigidBody1->GetLinearVelocity();
	const Antivector3D& angularVelocity1 = rigidBody1->GetAngularVelocity();
	const Vector3D& linearVelocity2 = rigidBody2->GetLinearVelocity();
	const Antivector3D& angularVelocity2 = rigidBody2->GetAngularVelocity();

	float inverseMass1 = rigidBody1->GetInverseBodyMass();
	const InertiaTensor& inverseInertiaTensor1 = rigidBody1->GetInverseWorldInertiaTensor();
	float inverseMass2 = rigidBody2->GetInverseBodyMass();
	const InertiaTensor& inverseInertiaTensor2 = rigidBody2->GetInverseWorldInertiaTensor();

	for (machine j = 0; j < 3; j++)
	{
		Jacobian column1(inverseMass1, inverseInertiaTensor1, constraint[j].row[0]);
		Jacobian column2(inverseMass2, inverseInertiaTensor2, constraint[j].row[1]);

		for (machine i = 0; i < 3; i++)
		{
			matrix(i,j) = constraint[i].row[0] * column1 + constraint[i].row[1] * column2;
		}

		result[j] = -(constraint[j].row[0].linear * linearVelocity1 + constraint[j].row[0].angular * angularVelocity1 + constraint[j].row[1].linear * linearVelocity2 + constraint[j].row[1].angular * angularVelocity2 + constraint[j].bias);
	}

	return (Inverse(matrix) * result);
}

Vector4D ConstraintContact::CalculateVelocityImpulse4D(const RigidBodyController *rigidBody1, const RigidBodyController *rigidBody2, const Constraint *constraint)
{
	Matrix4D	matrix;
	Vector4D	result;

	const Vector3D& linearVelocity1 = rigidBody1->GetLinearVelocity();
	const Antivector3D& angularVelocity1 = rigidBody1->GetAngularVelocity();
	const Vector3D& linearVelocity2 = rigidBody2->GetLinearVelocity();
	const Antivector3D& angularVelocity2 = rigidBody2->GetAngularVelocity();

	float inverseMass1 = rigidBody1->GetInverseBodyMass();
	const InertiaTensor& inverseInertiaTensor1 = rigidBody1->GetInverseWorldInertiaTensor();
	float inverseMass2 = rigidBody2->GetInverseBodyMass();
	const InertiaTensor& inverseInertiaTensor2 = rigidBody2->GetInverseWorldInertiaTensor();

	for (machine j = 0; j < 4; j++)
	{
		Jacobian column1(inverseMass1, inverseInertiaTensor1, constraint[j].row[0]);
		Jacobian column2(inverseMass2, inverseInertiaTensor2, constraint[j].row[1]);

		for (machine i = 0; i < 4; i++)
		{
			matrix(i,j) = constraint[i].row[0] * column1 + constraint[i].row[1] * column2;
		}

		result[j] = -(constraint[j].row[0].linear * linearVelocity1 + constraint[j].row[0].angular * angularVelocity1 + constraint[j].row[1].linear * linearVelocity2 + constraint[j].row[1].angular * angularVelocity2 + constraint[j].bias);
	}

	return (Inverse(matrix) * result);
}

float ConstraintContact::CalculatePositionImpulse(const RigidBodyController *rigidBody, const StaticConstraint *constraint)
{
	float inverseMass = rigidBody->GetInverseBodyMass();
	const InertiaTensor& inverseInertiaTensor = rigidBody->GetInverseWorldInertiaTensor();
	return (-constraint->bias / (constraint->row.linear * constraint->row.linear * inverseMass + constraint->row.angular * (inverseInertiaTensor * constraint->row.angular)));
}

Vector2D ConstraintContact::CalculatePositionImpulse2D(const RigidBodyController *rigidBody, const StaticConstraint *constraint)
{
	float inverseMass = rigidBody->GetInverseBodyMass();
	const InertiaTensor& inverseInertiaTensor = rigidBody->GetInverseWorldInertiaTensor();

	Jacobian column1(inverseMass, inverseInertiaTensor, constraint[0].row);
	Jacobian column2(inverseMass, inverseInertiaTensor, constraint[1].row);

	float m11 = constraint[0].row * column1;
	float m12 = constraint[0].row * column2;
	float m21 = constraint[1].row * column1;
	float m22 = constraint[1].row * column2;
	float d = -1.0F / (m11 * m22 - m12 * m21);

	float v1 = constraint[0].bias;
	float v2 = constraint[1].bias;

	return (Vector2D((m22 * v1 - m21 * v2) * d, (m11 * v2 - m12 * v1) * d));
}

Vector3D ConstraintContact::CalculatePositionImpulse3D(const RigidBodyController *rigidBody, const StaticConstraint *constraint)
{
	Matrix3D	matrix;
	Vector3D	result;

	float inverseMass = rigidBody->GetInverseBodyMass();
	const InertiaTensor& inverseInertiaTensor = rigidBody->GetInverseWorldInertiaTensor();

	for (machine j = 0; j < 3; j++)
	{
		Jacobian column(inverseMass, inverseInertiaTensor, constraint[j].row);

		for (machine i = 0; i < 3; i++)
		{
			matrix(i,j) = constraint[i].row * column;
		}

		result[j] = -constraint[j].bias;
	}

	return (Inverse(matrix) * result);
}

Vector4D ConstraintContact::CalculatePositionImpulse4D(const RigidBodyController *rigidBody, const StaticConstraint *constraint)
{
	Matrix4D	matrix;
	Vector4D	result;

	float inverseMass = rigidBody->GetInverseBodyMass();
	const InertiaTensor& inverseInertiaTensor = rigidBody->GetInverseWorldInertiaTensor();

	for (machine j = 0; j < 4; j++)
	{
		Jacobian column(inverseMass, inverseInertiaTensor, constraint[j].row);

		for (machine i = 0; i < 4; i++)
		{
			matrix(i,j) = constraint[i].row * column;
		}

		result[j] = -constraint[j].bias;
	}

	return (Inverse(matrix) * result);
}

float ConstraintContact::CalculatePositionImpulse(const RigidBodyController *rigidBody1, const RigidBodyController *rigidBody2, const Constraint *constraint)
{
	float inverseMass1 = rigidBody1->GetInverseBodyMass();
	const InertiaTensor& inverseInertiaTensor1 = rigidBody1->GetInverseWorldInertiaTensor();
	float inverseMass2 = rigidBody2->GetInverseBodyMass();
	const InertiaTensor& inverseInertiaTensor2 = rigidBody2->GetInverseWorldInertiaTensor();

	return (-constraint->bias / (constraint->row[0].linear * constraint->row[0].linear * inverseMass1 + constraint->row[0].angular * (inverseInertiaTensor1 * constraint->row[0].angular) + constraint->row[1].linear * constraint->row[1].linear * inverseMass2 + constraint->row[1].angular * (inverseInertiaTensor2 * constraint->row[1].angular)));
}

Vector2D ConstraintContact::CalculatePositionImpulse2D(const RigidBodyController *rigidBody1, const RigidBodyController *rigidBody2, const Constraint *constraint)
{
	float inverseMass1 = rigidBody1->GetInverseBodyMass();
	const InertiaTensor& inverseInertiaTensor1 = rigidBody1->GetInverseWorldInertiaTensor();
	float inverseMass2 = rigidBody2->GetInverseBodyMass();
	const InertiaTensor& inverseInertiaTensor2 = rigidBody2->GetInverseWorldInertiaTensor();

	Jacobian column11(inverseMass1, inverseInertiaTensor1, constraint[0].row[0]);
	Jacobian column12(inverseMass2, inverseInertiaTensor2, constraint[0].row[1]);
	Jacobian column21(inverseMass1, inverseInertiaTensor1, constraint[1].row[0]);
	Jacobian column22(inverseMass2, inverseInertiaTensor2, constraint[1].row[1]);

	float m11 = constraint[0].row[0] * column11 + constraint[0].row[1] * column12;
	float m12 = constraint[0].row[0] * column21 + constraint[0].row[1] * column22;
	float m21 = constraint[1].row[0] * column11 + constraint[1].row[1] * column12;
	float m22 = constraint[1].row[0] * column21 + constraint[1].row[1] * column22;
	float d = -1.0F / (m11 * m22 - m12 * m21);

	float v1 = constraint[0].bias;
	float v2 = constraint[1].bias;

	return (Vector2D((m22 * v1 - m21 * v2) * d, (m11 * v2 - m12 * v1) * d));
}

Vector3D ConstraintContact::CalculatePositionImpulse3D(const RigidBodyController *rigidBody1, const RigidBodyController *rigidBody2, const Constraint *constraint)
{
	Matrix3D	matrix;
	Vector3D	result;

	float inverseMass1 = rigidBody1->GetInverseBodyMass();
	const InertiaTensor& inverseInertiaTensor1 = rigidBody1->GetInverseWorldInertiaTensor();
	float inverseMass2 = rigidBody2->GetInverseBodyMass();
	const InertiaTensor& inverseInertiaTensor2 = rigidBody2->GetInverseWorldInertiaTensor();

	for (machine j = 0; j < 3; j++)
	{
		Jacobian column1(inverseMass1, inverseInertiaTensor1, constraint[j].row[0]);
		Jacobian column2(inverseMass2, inverseInertiaTensor2, constraint[j].row[1]);

		for (machine i = 0; i < 3; i++)
		{
			matrix(i,j) = constraint[i].row[0] * column1 + constraint[i].row[1] * column2;
		}

		result[j] = -constraint[j].bias;
	}

	return (Inverse(matrix) * result);
}

Vector4D ConstraintContact::CalculatePositionImpulse4D(const RigidBodyController *rigidBody1, const RigidBodyController *rigidBody2, const Constraint *constraint)
{
	Matrix4D	matrix;
	Vector4D	result;

	float inverseMass1 = rigidBody1->GetInverseBodyMass();
	const InertiaTensor& inverseInertiaTensor1 = rigidBody1->GetInverseWorldInertiaTensor();
	float inverseMass2 = rigidBody2->GetInverseBodyMass();
	const InertiaTensor& inverseInertiaTensor2 = rigidBody2->GetInverseWorldInertiaTensor();

	for (machine j = 0; j < 4; j++)
	{
		Jacobian column1(inverseMass1, inverseInertiaTensor1, constraint[j].row[0]);
		Jacobian column2(inverseMass2, inverseInertiaTensor2, constraint[j].row[1]);

		for (machine i = 0; i < 4; i++)
		{
			matrix(i,j) = constraint[i].row[0] * column1 + constraint[i].row[1] * column2;
		}

		result[j] = -constraint[j].bias;
	}

	return (Inverse(matrix) * result);
}

float ConstraintContact::AccumulateConstraintImpulse(int32 index, float multiplier, const Range<float>& range)
{
	float m = (cumulativeImpulse[index] += multiplier);
	m = Clamp(m, range.min, range.max);
	multiplier = m - appliedImpulse[index];
	appliedImpulse[index] = m;
	return (multiplier);
}

void ConstraintContact::SolveVelocityConstraints(RigidBodyController *rigidBody, int32 count, const StaticConstraint *constraint)
{
	if (count == 4)
	{
		Vector4D impulse = CalculateVelocityImpulse4D(rigidBody, constraint);

		impulse[0] = AccumulateConstraintImpulse(constraint[0].index, impulse[0], constraint[0].range);
		impulse[1] = AccumulateConstraintImpulse(constraint[1].index, impulse[1], constraint[1].range);
		impulse[2] = AccumulateConstraintImpulse(constraint[2].index, impulse[2], constraint[2].range);
		impulse[3] = AccumulateConstraintImpulse(constraint[3].index, impulse[3], constraint[3].range);

		rigidBody->ApplyVelocityCorrection(constraint[0].row, impulse[0]);
		rigidBody->ApplyVelocityCorrection(constraint[1].row, impulse[1]);
		rigidBody->ApplyVelocityCorrection(constraint[2].row, impulse[2]);
		rigidBody->ApplyVelocityCorrection(constraint[3].row, impulse[3]);
	}
	else if (count == 3)
	{
		Vector3D impulse = CalculateVelocityImpulse3D(rigidBody, constraint);

		impulse[0] = AccumulateConstraintImpulse(constraint[0].index, impulse[0], constraint[0].range);
		impulse[1] = AccumulateConstraintImpulse(constraint[1].index, impulse[1], constraint[1].range);
		impulse[2] = AccumulateConstraintImpulse(constraint[2].index, impulse[2], constraint[2].range);

		rigidBody->ApplyVelocityCorrection(constraint[0].row, impulse[0]);
		rigidBody->ApplyVelocityCorrection(constraint[1].row, impulse[1]);
		rigidBody->ApplyVelocityCorrection(constraint[2].row, impulse[2]);
	}
	else if (count == 2)
	{
		Vector2D impulse = CalculateVelocityImpulse2D(rigidBody, constraint);

		impulse[0] = AccumulateConstraintImpulse(constraint[0].index, impulse[0], constraint[0].range);
		impulse[1] = AccumulateConstraintImpulse(constraint[1].index, impulse[1], constraint[1].range);

		rigidBody->ApplyVelocityCorrection(constraint[0].row, impulse[0]);
		rigidBody->ApplyVelocityCorrection(constraint[1].row, impulse[1]);
	}
	else
	{
		float impulse = CalculateVelocityImpulse(rigidBody, constraint);
		impulse = AccumulateConstraintImpulse(constraint[0].index, impulse, constraint[0].range);
		rigidBody->ApplyVelocityCorrection(constraint[0].row, impulse);
	}
}

void ConstraintContact::SolveVelocityConstraints(RigidBodyController *rigidBody1, RigidBodyController *rigidBody2, int32 count, const Constraint *constraint)
{
	if (count == 4)
	{
		Vector4D impulse = CalculateVelocityImpulse4D(rigidBody1, rigidBody2, constraint);

		impulse[0] = AccumulateConstraintImpulse(constraint[0].index, impulse[0], constraint[0].range);
		impulse[1] = AccumulateConstraintImpulse(constraint[1].index, impulse[1], constraint[1].range);
		impulse[2] = AccumulateConstraintImpulse(constraint[2].index, impulse[2], constraint[2].range);
		impulse[3] = AccumulateConstraintImpulse(constraint[3].index, impulse[3], constraint[3].range);

		rigidBody1->ApplyVelocityCorrection(constraint[0].row[0], impulse[0]);
		rigidBody1->ApplyVelocityCorrection(constraint[1].row[0], impulse[1]);
		rigidBody1->ApplyVelocityCorrection(constraint[2].row[0], impulse[2]);
		rigidBody1->ApplyVelocityCorrection(constraint[3].row[0], impulse[3]);

		rigidBody2->ApplyVelocityCorrection(constraint[0].row[1], impulse[0]);
		rigidBody2->ApplyVelocityCorrection(constraint[1].row[1], impulse[1]);
		rigidBody2->ApplyVelocityCorrection(constraint[2].row[1], impulse[2]);
		rigidBody2->ApplyVelocityCorrection(constraint[3].row[1], impulse[3]);
	}
	else if (count == 3)
	{
		Vector3D impulse = CalculateVelocityImpulse3D(rigidBody1, rigidBody2, constraint);

		impulse[0] = AccumulateConstraintImpulse(constraint[0].index, impulse[0], constraint[0].range);
		impulse[1] = AccumulateConstraintImpulse(constraint[1].index, impulse[1], constraint[1].range);
		impulse[2] = AccumulateConstraintImpulse(constraint[2].index, impulse[2], constraint[2].range);

		rigidBody1->ApplyVelocityCorrection(constraint[0].row[0], impulse[0]);
		rigidBody1->ApplyVelocityCorrection(constraint[1].row[0], impulse[1]);
		rigidBody1->ApplyVelocityCorrection(constraint[2].row[0], impulse[2]);

		rigidBody2->ApplyVelocityCorrection(constraint[0].row[1], impulse[0]);
		rigidBody2->ApplyVelocityCorrection(constraint[1].row[1], impulse[1]);
		rigidBody2->ApplyVelocityCorrection(constraint[2].row[1], impulse[2]);
	}
	else if (count == 2)
	{
		Vector2D impulse = CalculateVelocityImpulse2D(rigidBody1, rigidBody2, constraint);

		impulse[0] = AccumulateConstraintImpulse(constraint[0].index, impulse[0], constraint[0].range);
		impulse[1] = AccumulateConstraintImpulse(constraint[1].index, impulse[1], constraint[1].range);

		rigidBody1->ApplyVelocityCorrection(constraint[0].row[0], impulse[0]);
		rigidBody1->ApplyVelocityCorrection(constraint[1].row[0], impulse[1]);

		rigidBody2->ApplyVelocityCorrection(constraint[0].row[1], impulse[0]);
		rigidBody2->ApplyVelocityCorrection(constraint[1].row[1], impulse[1]);
	}
	else
	{
		float impulse = CalculateVelocityImpulse(rigidBody1, rigidBody2, constraint);
		impulse = AccumulateConstraintImpulse(constraint[0].index, impulse, constraint[0].range);
		rigidBody1->ApplyVelocityCorrection(constraint[0].row[0], impulse);
		rigidBody2->ApplyVelocityCorrection(constraint[0].row[1], impulse);
	}
}

void ConstraintContact::SolvePositionConstraints(RigidBodyController *rigidBody, int32 count, const StaticConstraint *constraint)
{
	if (count == 4)
	{
		Vector4D impulse = CalculatePositionImpulse4D(rigidBody, constraint);

		rigidBody->ApplyPositionCorrection(constraint[0].row, impulse[0]);
		rigidBody->ApplyPositionCorrection(constraint[1].row, impulse[1]);
		rigidBody->ApplyPositionCorrection(constraint[2].row, impulse[2]);
		rigidBody->ApplyPositionCorrection(constraint[3].row, impulse[3]);
	}
	else if (count == 3)
	{
		Vector3D impulse = CalculatePositionImpulse3D(rigidBody, constraint);

		rigidBody->ApplyPositionCorrection(constraint[0].row, impulse[0]);
		rigidBody->ApplyPositionCorrection(constraint[1].row, impulse[1]);
		rigidBody->ApplyPositionCorrection(constraint[2].row, impulse[2]);
	}
	else if (count == 2)
	{
		Vector2D impulse = CalculatePositionImpulse2D(rigidBody, constraint);

		rigidBody->ApplyPositionCorrection(constraint[0].row, impulse[0]);
		rigidBody->ApplyPositionCorrection(constraint[1].row, impulse[1]);
	}
	else
	{
		float impulse = CalculatePositionImpulse(rigidBody, constraint);
		rigidBody->ApplyPositionCorrection(constraint[0].row, impulse);
	}
}

void ConstraintContact::SolvePositionConstraints(RigidBodyController *rigidBody1, RigidBodyController *rigidBody2, int32 count, const Constraint *constraint)
{
	if (count == 4)
	{
		Vector4D impulse = CalculatePositionImpulse4D(rigidBody1, rigidBody2, constraint);

		rigidBody1->ApplyPositionCorrection(constraint[0].row[0], impulse[0]);
		rigidBody1->ApplyPositionCorrection(constraint[1].row[0], impulse[1]);
		rigidBody1->ApplyPositionCorrection(constraint[2].row[0], impulse[2]);
		rigidBody1->ApplyPositionCorrection(constraint[3].row[0], impulse[3]);

		rigidBody2->ApplyPositionCorrection(constraint[0].row[1], impulse[0]);
		rigidBody2->ApplyPositionCorrection(constraint[1].row[1], impulse[1]);
		rigidBody2->ApplyPositionCorrection(constraint[2].row[1], impulse[2]);
		rigidBody2->ApplyPositionCorrection(constraint[3].row[1], impulse[3]);
	}
	else if (count == 3)
	{
		Vector3D impulse = CalculatePositionImpulse3D(rigidBody1, rigidBody2, constraint);

		rigidBody1->ApplyPositionCorrection(constraint[0].row[0], impulse[0]);
		rigidBody1->ApplyPositionCorrection(constraint[1].row[0], impulse[1]);
		rigidBody1->ApplyPositionCorrection(constraint[2].row[0], impulse[2]);

		rigidBody2->ApplyPositionCorrection(constraint[0].row[1], impulse[0]);
		rigidBody2->ApplyPositionCorrection(constraint[1].row[1], impulse[1]);
		rigidBody2->ApplyPositionCorrection(constraint[2].row[1], impulse[2]);
	}
	else if (count == 2)
	{
		Vector2D impulse = CalculatePositionImpulse2D(rigidBody1, rigidBody2, constraint);

		rigidBody1->ApplyPositionCorrection(constraint[0].row[0], impulse[0]);
		rigidBody1->ApplyPositionCorrection(constraint[1].row[0], impulse[1]);

		rigidBody2->ApplyPositionCorrection(constraint[0].row[1], impulse[0]);
		rigidBody2->ApplyPositionCorrection(constraint[1].row[1], impulse[1]);
	}
	else
	{
		float impulse = CalculatePositionImpulse(rigidBody1, rigidBody2, constraint);
		rigidBody1->ApplyPositionCorrection(constraint[0].row[0], impulse);
		rigidBody2->ApplyPositionCorrection(constraint[0].row[1], impulse);
	}
}


CollisionContact::CollisionContact(ContactType type, Body *body1, Body *body2, unsigned_int32 mask) : ConstraintContact(type, body1, body2, mask)
{
	cachedSimplexVertexCount = 0;
}

CollisionContact::~CollisionContact()
{
	#if C4DIAGS

		delete contactPointRenderable.GetTarget();
		delete contactVectorRenderable.GetTarget();

	#endif
}

void CollisionContact::Pack(Packer& data, unsigned_int32 packFlags) const
{
	ConstraintContact::Pack(data, packFlags);

	int32 count = cachedSimplexVertexCount;
	data << ChunkHeader('SPLX', 4 + count * (sizeof(Point3D) * 2));
	data << count;

	for (machine a = 0; a < count; a++)
	{
		data << cachedAlphaVertex[a];
		data << cachedBetaVertex[a];
	}

	count = subcontactCount;
	data << ChunkHeader('SBCT', 4 + count * (sizeof(Point3D) * 2 + sizeof(Vector3D) * 5 + 16));
	data << count;

	for (machine a = 0; a < count; a++)
	{
		data << subcontact[a].inactiveStepCount;
		data << subcontact[a].lastUpdateStep;
		data << subcontact[a].triangleIndex;
		data << subcontact[a].alphaPosition;
		data << subcontact[a].alphaNormal;
		data << subcontact[a].alphaTangent[0];
		data << subcontact[a].alphaTangent[1];
		data << subcontact[a].betaPosition;
		data << subcontact[a].betaNormal;
		data << subcontact[a].betaTangent[0];
		data << subcontact[a].betaTangent[1];
		data << subcontact[a].impactSpeed;
		data << subcontact[a].bounceSpeed;
	}

	data << TerminatorChunk;
}

void CollisionContact::Unpack(Unpacker& data, unsigned_int32 unpackFlags)
{
	ConstraintContact::Unpack(data, unpackFlags);
	UnpackChunkList<CollisionContact>(data, unpackFlags);
}

bool CollisionContact::UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags)
{
	switch (chunkHeader->chunkType)
	{
		case 'SPLX':
		{
			int32	count;

			data >> count;
			cachedSimplexVertexCount = count;

			for (machine a = 0; a < count; a++)
			{
				data >> cachedAlphaVertex[a];
				data >> cachedBetaVertex[a];
			}

			return (true);
		}

		case 'SBCT':
		{
			int32	count;

			data >> count;
			subcontactCount = count;

			for (machine a = 0; a < count; a++)
			{
				data >> subcontact[a].inactiveStepCount;
				data >> subcontact[a].lastUpdateStep;
				data >> subcontact[a].triangleIndex;
				data >> subcontact[a].alphaPosition;
				data >> subcontact[a].alphaNormal;
				data >> subcontact[a].alphaTangent[0];
				data >> subcontact[a].alphaTangent[1];
				data >> subcontact[a].betaPosition;
				data >> subcontact[a].betaNormal;
				data >> subcontact[a].betaTangent[0];
				data >> subcontact[a].betaTangent[1];
				data >> subcontact[a].impactSpeed;
				data >> subcontact[a].bounceSpeed;
			}

			return (true);
		}
	}

	return (false);
}

float CollisionContact::CalculateContactArea(const Point3D& p0, const Point3D& p1, const Point3D& p2, const Point3D& p3)
{
	Vector3D v1 = p1 - p0;
	Vector3D v2 = p2 - p0;
	Vector3D v3 = p3 - p0;

	Antivector3D w1 = v1 ^ v2;
	Antivector3D w2 = v2 ^ v3;

	if (w1 * w2 > 0.0F)
	{
		return (Magnitude(w1 + w2));
	}

	Antivector3D w3 = v3 ^ v1;

	if (w1 * w3 > 0.0F)
	{
		return (Magnitude(w1 + w3));
	}

	return (Magnitude(w2) + Magnitude(w3));
}

#if C4DIAGS

	void CollisionContact::BuildContactRenderables(const Node *node1, const Node *node2)
	{
		World *world = node1->GetWorld();
		if (world->GetDiagnosticFlags() & kDiagnosticContacts)
		{
			ColorRGBA color(1.0F, 1.0F, 1.0F);

			unsigned_int32 index = Math::Random(3);
			color[index] = Math::RandomFloat(0.5F) + 0.5F;
			color[IncMod<3>(index)] = Math::RandomFloat(0.5F) + 0.5F;

			ContactRenderable *renderable = new ContactVectorRenderable(subcontact, color);
			renderable->SetTransformable(node1);
			contactVectorRenderable = renderable;
			world->AddContactRenderable(renderable);

			renderable = new ContactPointRenderable(subcontact, color);
			renderable->SetTransformable(node2);
			contactPointRenderable = renderable;
			world->AddContactRenderable(renderable);
		}
	}

#endif


GeometryContact::GeometryContact(Geometry *geometry, RigidBodyController *rigidBody, const IntersectionData *intersectionData, unsigned_int32 signature) : CollisionContact(kContactGeometry, rigidBody, rigidBody->GetPhysicsController()->GetNullBody(), kContactNotificationOutgoing)
{
	contactGeometry = geometry;
	contactSignature = signature;
	SetContactParam(intersectionData->contactParam);

	subcontactCount = 1;
	InitializeSubcontact(&subcontact[0], rigidBody, AdjugateTransform(rigidBody->GetFinalTransform(), intersectionData->alphaContact), intersectionData);

	if (rigidBody->GetRigidBodyFlags() & kRigidBodyDisabledContact)
	{
		Disable();
	}

	#if C4DIAGS

		BuildContactRenderables(rigidBody->GetTargetNode(), geometry);

	#endif
}

GeometryContact::GeometryContact(Body *nullBody) : CollisionContact(kContactGeometry, nullBody, nullBody)
{
}

GeometryContact::~GeometryContact()
{
}

void GeometryContact::Pack(Packer& data, unsigned_int32 packFlags) const
{
	CollisionContact::Pack(data, packFlags);

	if (contactGeometry->LinkedNodePackable(packFlags))
	{
		data << ChunkHeader('GEOM', 4);
		data << contactGeometry->GetNodeIndex();
	}

	data << ChunkHeader('SIGN', 4);
	data << contactSignature;

	data << TerminatorChunk;
}

void GeometryContact::Unpack(Unpacker& data, unsigned_int32 unpackFlags)
{
	CollisionContact::Unpack(data, unpackFlags);
	UnpackChunkList<GeometryContact>(data, unpackFlags);
}

bool GeometryContact::UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags)
{
	switch (chunkHeader->chunkType)
	{
		case 'GEOM':
		{
			int32	nodeIndex;

			data >> nodeIndex;
			data.AddNodeLink(nodeIndex, &GeometryLinkProc, this);
			return (true);
		}

		case 'SIGN':

			data >> contactSignature;
			return (true);
	}

	return (false);
}

void GeometryContact::GeometryLinkProc(Node *node, void *cookie)
{
	GeometryContact *geometryContact = static_cast<GeometryContact *>(cookie);
	geometryContact->contactGeometry = static_cast<Geometry *>(node);
}

bool GeometryContact::NonpersistentFinishNode(void) const
{
	return ((contactGeometry->GetNodeFlags() & kNodeNonpersistent) != 0);
}

void GeometryContact::InitializeSubcontact(Subcontact *subcontact, const RigidBodyController *rigidBody, const Point3D& alphaPosition, const IntersectionData *intersectionData) const
{
	subcontact->inactiveStepCount = 0;
	subcontact->lastUpdateStep = rigidBody->GetPhysicsController()->GetSimulationStep();
	subcontact->triangleIndex = intersectionData->triangleIndex;

	subcontact->alphaPosition = alphaPosition;
	subcontact->alphaNormal = intersectionData->contactNormal * rigidBody->GetFinalTransform();
	subcontact->alphaTangent[0] = Math::CreateUnitPerpendicular(subcontact->alphaNormal);
	subcontact->alphaTangent[1] = subcontact->alphaNormal % subcontact->alphaTangent[0];

	subcontact->betaPosition = contactGeometry->GetInverseWorldTransform() * intersectionData->betaContact;
	subcontact->betaNormal = intersectionData->contactNormal * contactGeometry->GetWorldTransform();
	subcontact->betaTangent[0] = Math::CreateUnitPerpendicular(subcontact->betaNormal);
	subcontact->betaTangent[1] = subcontact->betaNormal % subcontact->betaTangent[0];

	Vector3D contactVelocity = rigidBody->GetOriginalLinearVelocity() + rigidBody->GetOriginalAngularVelocity() % (intersectionData->alphaContact - rigidBody->GetWorldCenterOfMass());
	float impactSpeed = FmaxZero((contactVelocity - contactGeometry->GetGeometryVelocity()) * intersectionData->contactNormal);
	float bounceSpeed = impactSpeed * rigidBody->GetRestitutionCoefficient();

	// Dampen restitution at low speeds to account for noise created by the finite time step.

	float m = bounceSpeed * bounceSpeed * bounceSpeed * 8.0F;
	bounceSpeed *= m / (m + 1.0F);

	// Eliminate restitution if the bounce speed would be unable to overcome the applied forces
	// during the next simulation step.

	if (bounceSpeed < rigidBody->GetAppliedForce() * intersectionData->contactNormal * (rigidBody->GetInverseBodyMass() * kTimeStep * 2.0F))
	{
		bounceSpeed = 0.0F;
	}

	subcontact->impactSpeed = impactSpeed;
	subcontact->bounceSpeed = bounceSpeed;
}

void GeometryContact::UpdateContact(const IntersectionData *intersectionData)
{
	float		squaredDelta[kMaxSubcontactCount];

	RigidBodyController *rigidBody = static_cast<RigidBodyController *>(GetStartElement());
	Point3D alphaPosition = AdjugateTransform(rigidBody->GetFinalTransform(), intersectionData->alphaContact);

	for (machine a = 0; a < subcontactCount; a++)
	{
		squaredDelta[a] = SquaredMag(subcontact[a].alphaPosition - alphaPosition);
	}

	machine subcontactIndex = 0;
	float minSquaredDelta = 0.0F;
	if (subcontactCount > 0)
	{
		minSquaredDelta = squaredDelta[0];
		for (machine a = 1; a < subcontactCount; a++)
		{
			if (squaredDelta[a] < minSquaredDelta)
			{
				minSquaredDelta = squaredDelta[a];
				subcontactIndex = a;
			}
		}
	}

	if (minSquaredDelta > kMaxSubcontactSquaredDelta)
	{
		subcontactIndex = subcontactCount;
		if (subcontactIndex == kMaxSubcontactCount)
		{
			float currentArea = CalculateContactArea(subcontact[0].alphaPosition, subcontact[1].alphaPosition, subcontact[2].alphaPosition, subcontact[3].alphaPosition);
			float area1 = CalculateContactArea(alphaPosition, subcontact[1].alphaPosition, subcontact[2].alphaPosition, subcontact[3].alphaPosition);
			float area2 = CalculateContactArea(subcontact[0].alphaPosition, alphaPosition, subcontact[2].alphaPosition, subcontact[3].alphaPosition);
			float area3 = CalculateContactArea(subcontact[0].alphaPosition, subcontact[1].alphaPosition, alphaPosition, subcontact[3].alphaPosition);
			float area4 = CalculateContactArea(subcontact[0].alphaPosition, subcontact[1].alphaPosition, subcontact[2].alphaPosition, alphaPosition);

			if (area1 > currentArea)
			{
				if (area1 > Fmax(area2, area3, area4))
				{
					subcontactIndex = 0;
					currentArea = area1;
				}
			}

			if (area2 > currentArea)
			{
				if (area2 > Fmax(area1, area3, area4))
				{
					subcontactIndex = 1;
					currentArea = area2;
				}
			}

			if (area3 > currentArea)
			{
				if (area3 > Fmax(area1, area2, area4))
				{
					subcontactIndex = 2;
					currentArea = area3;
				}
			}

			if (area4 > currentArea)
			{
				if (area4 > Fmax(area1, area2, area3))
				{
					subcontactIndex = 3;
					currentArea = area4;
				}
			}

			if (subcontactIndex == kMaxSubcontactCount)
			{
				return;
			}
		}
		else
		{
			subcontactCount = subcontactIndex + 1;
		}
	}

	InitializeSubcontact(&subcontact[subcontactIndex], rigidBody, alphaPosition, intersectionData);

	#if C4DIAGS

		ContactRenderable *renderable = contactVectorRenderable;
		if (renderable)
		{
			renderable->UpdateContact(subcontactCount, subcontact);
		}

		renderable = contactPointRenderable;
		if (renderable)
		{
			renderable->UpdateContact(subcontactCount, subcontact);
		}

	#endif
}

void GeometryContact::InitializeConstraints(void)
{
	CollisionContact::InitializeConstraints();

	RigidBodyController *rigidBody = static_cast<RigidBodyController *>(GetStartElement());
	rigidBody->SetMovementVelocity(contactGeometry->GetGeometryVelocity());

	int32 simulationStep = rigidBody->GetPhysicsController()->GetSimulationStep();
	const Transform4D& transform = rigidBody->GetFinalTransform();

	for (machine a = 0; a < subcontactCount; a++)
	{
		Point3D rigidBodyWorldPosition = transform * subcontact[a].alphaPosition;
		Point3D geometryWorldPosition = contactGeometry->GetWorldTransform() * subcontact[a].betaPosition;
		Vector3D deltaWorldPosition = geometryWorldPosition - rigidBodyWorldPosition;

		Antivector3D rigidBodyWorldNormal = transform * subcontact[a].alphaNormal;
		Antivector3D geometryWorldNormal = subcontact[a].betaNormal * contactGeometry->GetInverseWorldTransform();

		float delta2 = SquaredMag(deltaWorldPosition);
		float separation = deltaWorldPosition * geometryWorldNormal;
		if ((delta2 - separation * separation > kMaxSubcontactSquaredTangentialSeparation) || ((rigidBodyWorldNormal * geometryWorldNormal < 0.5F) && (delta2 > -kContactEpsilon)))
		{
			subcontact[a].inactiveStepCount = kMaxInactiveSubcontactStepCount;
			subcontact[a].lastUpdateStep = simulationStep - 1;
		}

		subcontact[a].activeFlag = false;
	}

	for (machine a = 0; a < subcontactCount;)
	{
		if (subcontact[a].lastUpdateStep != simulationStep)
		{
			subcontact[a].lastUpdateStep = simulationStep;

			int32 inactiveStepCount = ++subcontact[a].inactiveStepCount;
			if (inactiveStepCount >= kMaxInactiveSubcontactStepCount)
			{
				int32 count = subcontactCount - 1;
				if (count > 0)
				{
					subcontactCount = count;
					for (machine b = a; b < subcontactCount; b++)
					{
						subcontact[b] = subcontact[b + 1];
					}

					#if C4DIAGS

						ContactRenderable *renderable = contactVectorRenderable;
						if (renderable)
						{
							renderable->UpdateContact(count, subcontact);
						}

						renderable = contactPointRenderable;
						if (renderable)
						{
							renderable->UpdateContact(count, subcontact);
						}

					#endif

					continue;
				}
				else
				{
					rigidBody->SetLinearVelocity(rigidBody->GetLinearVelocity() + contactGeometry->GetGeometryVelocity());

					delete this;
					break;
				}
			}
		}

		a++;
	}
}

void GeometryContact::ApplyVelocityConstraints(void)
{
	/*	A = Point of contact between the rigid body and geometry
		B = Center of mass of the rigid body
		N = Contact normal pointing away from the rigid body
		V = Current linear velocity of the rigid body
		ω = Current angular velocity of the rigid body

		The position constraint for a geometry contact is given by:

			C = A ⋅ -N ≥ 0

		The derivative dA/dt is given by V + ω ⨯ (A - B), so the velocity constraint is:

			 dC
			──── = V ⋅ -N + [ω ⨯ (A - B)] ⋅ -N = 0
			 dt

		After applying a triple product identity to the angular term, this is written in Jacobian form as follows:

			 dC
			──── = -N ⋅ V + [N ⨯ (A - B)] ⋅ ω = 0
			 dt
	*/

	RigidBodyController *rigidBody = static_cast<RigidBodyController *>(GetStartElement());
	const Transform4D& transform = rigidBody->GetFinalTransform();
	Point3D centerMass = transform * rigidBody->GetBodyCenterOfMass();

	for (machine a = 0; a < subcontactCount; a++)
	{
		Point3D rigidBodyWorldPosition = transform * subcontact[a].alphaPosition;
		Point3D geometryWorldPosition = contactGeometry->GetWorldTransform() * subcontact[a].betaPosition;
		Vector3D deltaWorldPosition = geometryWorldPosition - rigidBodyWorldPosition;
		Antivector3D geometryWorldNormal = subcontact[a].betaNormal * contactGeometry->GetInverseWorldTransform();

		float separation = deltaWorldPosition * geometryWorldNormal;
		if (separation < -kContactEpsilon)
		{
			subcontact[a].inactiveStepCount = 0;

			if (separation < 0.0F)
			{
				subcontact[a].activeFlag = true;
			}
		}

		if (subcontact[a].activeFlag)
		{
			StaticConstraint	constraint;

			Vector3D contactDirection = rigidBodyWorldPosition - centerMass;
			Vector3D rotationalVelocity = rigidBody->GetAngularVelocity() % contactDirection;
			Vector3D contactVelocity = rigidBody->GetLinearVelocity() + rotationalVelocity - contactGeometry->GetGeometryVelocity();

			constraint.row.Set(-geometryWorldNormal, geometryWorldNormal % contactDirection);
			constraint.bias = -subcontact[a].bounceSpeed;
			constraint.index = a * 4;
			constraint.range.Set(0.0F, K::infinity);

			float rollingResistance = rigidBody->GetRollingResistance();
			if (rollingResistance > 0.0F)
			{
				// Resist rolling motion by moving the point at which impulses are applied slightly
				// ahead of the contact point so that an opposing torque is generated.

				Vector3D offset = ProjectOnto(rotationalVelocity, geometryWorldNormal) - rotationalVelocity;
				float d = SquaredMag(offset);
				if (d > K::min_float)
				{
					// Dampen the rolling resistance when the contact velocity is not close to zero.

					float m = FmaxZero(1.0F - Magnitude(contactVelocity) * 2.0F);
					m = m * m * (3.0F - 2.0F * m);

					offset *= rollingResistance * InverseSqrt(d) * m;
					constraint.row.angular += geometryWorldNormal % offset;
				}
			}

			float constraintImpulse = CalculateVelocityImpulse(rigidBody, &constraint);
			constraintImpulse = AccumulateConstraintImpulse(constraint.index, constraintImpulse, constraint.range);
			rigidBody->ApplyVelocityCorrection(constraint.row, constraintImpulse);
		}
	}

	rigidBody->Integrate();
}

void GeometryContact::ApplyFrictionConstraints(void)
{
	RigidBodyController *rigidBody = static_cast<RigidBodyController *>(GetStartElement());
	const Transform4D& transform = rigidBody->GetFinalTransform();
	Point3D centerMass = transform * rigidBody->GetBodyCenterOfMass();

	for (machine a = 0; a < subcontactCount; a++)
	{
		if (subcontact[a].activeFlag)
		{
			float frictionCoefficient = rigidBody->GetFrictionCoefficient();
			if (frictionCoefficient > 0.0F)
			{
				Point3D rigidBodyWorldPosition = transform * subcontact[a].alphaPosition;
				Antivector3D geometryWorldNormal = subcontact[a].betaNormal * contactGeometry->GetInverseWorldTransform();

				Vector3D contactDirection = rigidBodyWorldPosition - centerMass;
				Vector3D rotationalVelocity = rigidBody->GetAngularVelocity() % contactDirection;
				Vector3D contactVelocity = rigidBody->GetLinearVelocity() + rotationalVelocity - contactGeometry->GetGeometryVelocity();
				contactVelocity -= ProjectOnto(contactVelocity, geometryWorldNormal);

				float d = SquaredMag(contactVelocity);
				if (d > K::min_float)
				{
					StaticConstraint	constraint[2];

					contactVelocity *= InverseSqrt(d);

					Vector3D tangentX = contactGeometry->GetWorldTransform() * subcontact[a].betaTangent[0];
					Vector3D tangentY = contactGeometry->GetWorldTransform() * subcontact[a].betaTangent[1];

					machine constraintIndex = a * 4;
					float limit = appliedImpulse[constraintIndex] * frictionCoefficient;
					float limitX = Fabs(contactVelocity * tangentX) * limit;
					float limitY = Fabs(contactVelocity * tangentY) * limit;

					constraint[0].row.Set(tangentX, contactDirection % tangentX);
					constraint[0].bias = 0.0F;
					constraint[0].index = constraintIndex + 1;
					constraint[0].range.Set(-limitX, limitX);

					constraint[1].row.Set(tangentY, contactDirection % tangentY);
					constraint[1].bias = 0.0F;
					constraint[1].index = constraintIndex + 2;
					constraint[1].range.Set(-limitY, limitY);

					SolveVelocityConstraints(rigidBody, 2, constraint);

					float spinFrictionMultiplier = rigidBody->GetSpinFrictionMultiplier();
					if (spinFrictionMultiplier > 0.0F)
					{
						// The maximum spin friction is applied when the contact point is directly below the center of mass.

						d = geometryWorldNormal * contactDirection;
						if (d > K::min_float)
						{
							limit *= d * InverseMag(contactDirection) * spinFrictionMultiplier;

							constraint[0].row.Set(Zero3D, geometryWorldNormal);
							constraint[0].bias = 0.0F;
							constraint[0].index = constraintIndex + 3;
							constraint[0].range.Set(-limit, limit);

							float impulse = CalculateVelocityImpulse(rigidBody, constraint);
							impulse = AccumulateConstraintImpulse(constraint[0].index, impulse, constraint[0].range);
							rigidBody->ApplyVelocityCorrection(constraint[0].row, impulse);
						}
					}
				}
			}
		}
	}

	rigidBody->Integrate();
}

void GeometryContact::ApplyPositionConstraints(void)
{
	RigidBodyController *rigidBody = static_cast<RigidBodyController *>(GetStartElement());
	const Transform4D& transform = rigidBody->GetFinalTransform();

	for (machine a = 0; a < subcontactCount; a++)
	{
		Point3D rigidBodyWorldPosition = transform * subcontact[a].alphaPosition;
		Point3D geometryWorldPosition = contactGeometry->GetWorldTransform() * subcontact[a].betaPosition;
		Vector3D deltaWorldPosition = geometryWorldPosition - rigidBodyWorldPosition;
		Antivector3D geometryWorldNormal = subcontact[a].betaNormal * contactGeometry->GetInverseWorldTransform();

		float separation = deltaWorldPosition * geometryWorldNormal;
		if (separation < kContactEpsilon)
		{
			StaticConstraint	constraint;

			Point3D centerMass = transform * rigidBody->GetBodyCenterOfMass();

			constraint.row.Set(-geometryWorldNormal, geometryWorldNormal % (rigidBodyWorldPosition - centerMass));
			constraint.bias = separation * kContactStabilizeFactor;

			float positionImpulse = CalculatePositionImpulse(rigidBody, &constraint);
			rigidBody->ApplyPositionCorrection(constraint.row, positionImpulse);
		}
	}

	rigidBody->Integrate();
}


RigidBodyContact::RigidBodyContact(RigidBodyController *rigidBody1, RigidBodyController *rigidBody2, const Shape *shape1, const Shape *shape2, unsigned_int32 signature1, unsigned_int32 signature2, const IntersectionData *intersectionData) : CollisionContact(kContactRigidBody, rigidBody1, rigidBody2, kContactNotificationOutgoing | kContactNotificationIncoming)
{
	startShape = shape1;
	finishShape = shape2;
	startSignature = signature1;
	finishSignature = signature2;
	SetContactParam(intersectionData->contactParam);

	subcontactCount = 1;
	InitializeSubcontact(&subcontact[0], rigidBody1, rigidBody2, AdjugateTransform(rigidBody1->GetFinalTransform(), intersectionData->alphaContact), intersectionData);

	if ((rigidBody1->GetRigidBodyFlags() | rigidBody2->GetRigidBodyFlags()) & kRigidBodyDisabledContact)
	{
		Disable();
	}

	#if C4DIAGS

		BuildContactRenderables(rigidBody1->GetTargetNode(), rigidBody2->GetTargetNode());

	#endif
}

RigidBodyContact::RigidBodyContact(Body *nullBody) : CollisionContact(kContactRigidBody, nullBody, nullBody)
{
}

RigidBodyContact::~RigidBodyContact()
{
}

void RigidBodyContact::Pack(Packer& data, unsigned_int32 packFlags) const
{
	CollisionContact::Pack(data, packFlags);

	data << ChunkHeader('SIGN', 8);
	data << startSignature;
	data << finishSignature;

	data << TerminatorChunk;
}

void RigidBodyContact::Unpack(Unpacker& data, unsigned_int32 unpackFlags)
{
	CollisionContact::Unpack(data, unpackFlags);
	UnpackChunkList<RigidBodyContact>(data, unpackFlags);
}

bool RigidBodyContact::UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags)
{
	switch (chunkHeader->chunkType)
	{
		case 'SIGN':

			data >> startSignature;
			data >> finishSignature;
			return (true);
	}

	return (false);
}

bool RigidBodyContact::NonpersistentFinishNode(void) const
{
	return ((static_cast<const RigidBodyController *>(GetFinishElement())->GetTargetNode()->GetNodeFlags() & kNodeNonpersistent) != 0);
}

void RigidBodyContact::InitializeSubcontact(Subcontact *subcontact, RigidBodyController *rigidBody1, RigidBodyController *rigidBody2, const Point3D& alphaPosition, const IntersectionData *intersectionData) const
{
	subcontact->inactiveStepCount = 0;
	subcontact->lastUpdateStep = rigidBody1->GetPhysicsController()->GetSimulationStep();
	subcontact->triangleIndex = 0;

	subcontact->alphaPosition = alphaPosition;
	subcontact->alphaNormal = intersectionData->contactNormal * rigidBody1->GetFinalTransform();
	subcontact->alphaTangent[0] = Math::CreateUnitPerpendicular(subcontact->alphaNormal);
	subcontact->alphaTangent[1] = subcontact->alphaNormal % subcontact->alphaTangent[0];

	subcontact->betaPosition = AdjugateTransform(rigidBody2->GetFinalTransform(), intersectionData->betaContact);
	subcontact->betaNormal = intersectionData->contactNormal * rigidBody2->GetFinalTransform();
	subcontact->betaTangent[0] = Math::CreateUnitPerpendicular(subcontact->betaNormal);
	subcontact->betaTangent[1] = subcontact->betaNormal % subcontact->betaTangent[0];

	Vector3D contactVelocity = rigidBody1->GetOriginalLinearVelocity() + rigidBody1->GetOriginalAngularVelocity() % (intersectionData->alphaContact - rigidBody1->GetWorldCenterOfMass()) - rigidBody2->GetOriginalLinearVelocity() - rigidBody2->GetOriginalAngularVelocity() % (intersectionData->betaContact - rigidBody2->GetWorldCenterOfMass());
	float impactSpeed = FmaxZero(contactVelocity * intersectionData->contactNormal);
	float bounceSpeed = impactSpeed * Fmax(rigidBody1->GetRestitutionCoefficient(), rigidBody2->GetRestitutionCoefficient());

	// Dampen restitution at low speeds to account for noise created by the finite time step.

	float m = bounceSpeed * bounceSpeed * bounceSpeed * 8.0F;
	bounceSpeed *= m / (m + 1.0F);

	// Eliminate restitution if the bounce speed would be unable to overcome the applied forces
	// during the next simulation step.

	if (bounceSpeed < (rigidBody2->GetAppliedForce() * rigidBody2->GetInverseBodyMass() - rigidBody1->GetAppliedForce() * rigidBody1->GetInverseBodyMass()) * intersectionData->contactNormal * (kTimeStep * 2.0F))
	{
		bounceSpeed = 0.0F;
	}

	subcontact->impactSpeed = impactSpeed;
	subcontact->bounceSpeed = bounceSpeed;
}

void RigidBodyContact::UpdateContact(const IntersectionData *intersectionData)
{
	float		squaredDelta[kMaxSubcontactCount];

	RigidBodyController *rigidBody1 = static_cast<RigidBodyController *>(GetStartElement());
	Point3D alphaPosition = AdjugateTransform(rigidBody1->GetFinalTransform(), intersectionData->alphaContact);

	for (machine a = 0; a < subcontactCount; a++)
	{
		squaredDelta[a] = SquaredMag(subcontact[a].alphaPosition - alphaPosition);
	}

	machine subcontactIndex = 0;
	float minSquaredDelta = 0.0F;
	if (subcontactCount > 0)
	{
		minSquaredDelta = squaredDelta[0];
		for (machine a = 1; a < subcontactCount; a++)
		{
			if (squaredDelta[a] < minSquaredDelta)
			{
				minSquaredDelta = squaredDelta[a];
				subcontactIndex = a;
			}
		}
	}

	if (minSquaredDelta > kMaxSubcontactSquaredDelta)
	{
		subcontactIndex = subcontactCount;
		if (subcontactIndex == kMaxSubcontactCount)
		{
			float currentArea = CalculateContactArea(subcontact[0].alphaPosition, subcontact[1].alphaPosition, subcontact[2].alphaPosition, subcontact[3].alphaPosition);
			float area1 = CalculateContactArea(alphaPosition, subcontact[1].alphaPosition, subcontact[2].alphaPosition, subcontact[3].alphaPosition);
			float area2 = CalculateContactArea(subcontact[0].alphaPosition, alphaPosition, subcontact[2].alphaPosition, subcontact[3].alphaPosition);
			float area3 = CalculateContactArea(subcontact[0].alphaPosition, subcontact[1].alphaPosition, alphaPosition, subcontact[3].alphaPosition);
			float area4 = CalculateContactArea(subcontact[0].alphaPosition, subcontact[1].alphaPosition, subcontact[2].alphaPosition, alphaPosition);

			if (area1 > currentArea)
			{
				if (area1 > Fmax(area2, area3, area4))
				{
					subcontactIndex = 0;
					currentArea = area1;
				}
			}

			if (area2 > currentArea)
			{
				if (area2 > Fmax(area1, area3, area4))
				{
					subcontactIndex = 1;
					currentArea = area2;
				}
			}

			if (area3 > currentArea)
			{
				if (area3 > Fmax(area1, area2, area4))
				{
					subcontactIndex = 2;
					currentArea = area3;
				}
			}

			if (area4 > currentArea)
			{
				if (area4 > Fmax(area1, area2, area3))
				{
					subcontactIndex = 3;
					currentArea = area4;
				}
			}

			if (subcontactIndex == kMaxSubcontactCount)
			{
				return;
			}
		}
		else
		{
			subcontactCount = subcontactIndex + 1;
		}
	}

	RigidBodyController *rigidBody2 = static_cast<RigidBodyController *>(GetFinishElement());
	InitializeSubcontact(&subcontact[subcontactIndex], rigidBody1, rigidBody2, alphaPosition, intersectionData);

	#if C4DIAGS

		ContactRenderable *renderable = contactVectorRenderable;
		if (renderable)
		{
			renderable->UpdateContact(subcontactCount, subcontact);
		}

		renderable = contactPointRenderable;
		if (renderable)
		{
			renderable->UpdateContact(subcontactCount, subcontact);
		}

	#endif
}

void RigidBodyContact::InitializeConstraints(void)
{
	CollisionContact::InitializeConstraints();

	RigidBodyController *rigidBody1 = static_cast<RigidBodyController *>(GetStartElement());
	RigidBodyController *rigidBody2 = static_cast<RigidBodyController *>(GetFinishElement());

	int32 simulationStep = rigidBody1->GetPhysicsController()->GetSimulationStep();
	const Transform4D& transform1 = rigidBody1->GetFinalTransform();
	const Transform4D& transform2 = rigidBody2->GetFinalTransform();

	for (machine a = 0; a < subcontactCount; a++)
	{
		Point3D worldPosition1 = transform1 * subcontact[a].alphaPosition;
		Point3D worldPosition2 = transform2 * subcontact[a].betaPosition;
		Vector3D deltaWorldPosition = worldPosition2 - worldPosition1;

		Antivector3D worldNormal1 = transform1 * subcontact[a].alphaNormal;
		Antivector3D worldNormal2 = transform2 * subcontact[a].betaNormal;

		float delta2 = SquaredMag(deltaWorldPosition);
		float separation = deltaWorldPosition * worldNormal1;
		if ((delta2 - separation * separation > kMaxSubcontactSquaredTangentialSeparation) || ((worldNormal1 * worldNormal2 < 0.5F) && (delta2 > -kContactEpsilon)))
		{
			subcontact[a].inactiveStepCount = kMaxInactiveSubcontactStepCount;
			subcontact[a].lastUpdateStep = simulationStep - 1;
		}

		subcontact[a].activeFlag = false;
	}

	for (machine a = 0; a < subcontactCount;)
	{
		if (subcontact[a].lastUpdateStep != simulationStep)
		{
			subcontact[a].lastUpdateStep = simulationStep;

			int32 inactiveStepCount = ++subcontact[a].inactiveStepCount;
			if (inactiveStepCount >= kMaxInactiveSubcontactStepCount)
			{
				int32 count = subcontactCount - 1;
				if (count > 0)
				{
					subcontactCount = count;
					for (machine b = a; b < subcontactCount; b++)
					{
						subcontact[b] = subcontact[b + 1];
					}

					#if C4DIAGS

						ContactRenderable *renderable = contactVectorRenderable;
						if (renderable)
						{
							renderable->UpdateContact(count, subcontact);
						}

						renderable = contactPointRenderable;
						if (renderable)
						{
							renderable->UpdateContact(count, subcontact);
						}

					#endif

					continue;
				}
				else
				{
					delete this;
					break;
				}
			}
		}

		a++;
	}
}

void RigidBodyContact::ApplyVelocityConstraints(void)
{
	RigidBodyController *rigidBody1 = static_cast<RigidBodyController *>(GetStartElement());
	RigidBodyController *rigidBody2 = static_cast<RigidBodyController *>(GetFinishElement());
	const Transform4D& transform1 = rigidBody1->GetFinalTransform();
	const Transform4D& transform2 = rigidBody2->GetFinalTransform();
	Point3D centerMass1 = transform1 * rigidBody1->GetBodyCenterOfMass();
	Point3D centerMass2 = transform2 * rigidBody2->GetBodyCenterOfMass();

	for (machine a = 0; a < subcontactCount; a++)
	{
		Point3D worldPosition1 = transform1 * subcontact[a].alphaPosition;
		Point3D worldPosition2 = transform2 * subcontact[a].betaPosition;
		Vector3D deltaWorldPosition = worldPosition2 - worldPosition1;
		Antivector3D worldNormal1 = transform1 * subcontact[a].alphaNormal;

		float separation = deltaWorldPosition * worldNormal1;
		if (separation < -kContactEpsilon)
		{
			subcontact[a].inactiveStepCount = 0;

			if (separation < 0.0F)
			{
				subcontact[a].activeFlag = true;
			}
		}

		if (subcontact[a].activeFlag)
		{
			Constraint		constraint;

			Vector3D contactDirection1 = worldPosition1 - centerMass1;
			Vector3D contactDirection2 = worldPosition2 - centerMass2;
			Vector3D rotationalVelocity1 = rigidBody1->GetAngularVelocity() % contactDirection1;
			Vector3D rotationalVelocity2 = rigidBody2->GetAngularVelocity() % contactDirection2;
			Vector3D contactVelocity = rigidBody2->GetLinearVelocity() + rotationalVelocity2 - rigidBody1->GetLinearVelocity() - rotationalVelocity1;

			constraint.row[0].Set(-worldNormal1, worldNormal1 % contactDirection1);
			constraint.row[1].Set(worldNormal1, contactDirection2 % worldNormal1);
			constraint.bias = -subcontact[a].bounceSpeed;
			constraint.index = a * 4;
			constraint.range.Set(0.0F, K::infinity);

			float rollingResistance1 = rigidBody1->GetRollingResistance();
			float rollingResistance2 = rigidBody2->GetRollingResistance();
			if (Fmax(rollingResistance1, rollingResistance2) > 0.0F)
			{
				// Resist rolling motion by moving the point at which impulses are applied slightly
				// ahead of the contact point so that an opposing torque is generated.

				Vector3D offset = rotationalVelocity2 - rotationalVelocity1;
				offset = ProjectOnto(offset, worldNormal1) - offset;
				float d = SquaredMag(offset);
				if (d > K::min_float)
				{
					// Dampen the rolling resistance when the contact velocity is not close to zero.

					float m = FmaxZero(1.0F - Magnitude(contactVelocity) * 2.0F);
					m = m * m * (3.0F - 2.0F * m);

					offset *= InverseSqrt(d) * m;
					constraint.row[0].angular += (worldNormal1 % offset) * rollingResistance1;
					constraint.row[1].angular += (offset % worldNormal1) * rollingResistance2;
				}
			}

			float constraintImpulse = CalculateVelocityImpulse(rigidBody1, rigidBody2, &constraint);
			constraintImpulse = AccumulateConstraintImpulse(a * 4, constraintImpulse, Range<float>(0.0F, K::infinity));
			rigidBody1->ApplyVelocityCorrection(constraint.row[0], constraintImpulse);
			rigidBody2->ApplyVelocityCorrection(constraint.row[1], constraintImpulse);
		}
	}

	rigidBody1->Integrate();
	rigidBody2->Integrate();
}

void RigidBodyContact::ApplyFrictionConstraints(void)
{
	RigidBodyController *rigidBody1 = static_cast<RigidBodyController *>(GetStartElement());
	RigidBodyController *rigidBody2 = static_cast<RigidBodyController *>(GetFinishElement());
	const Transform4D& transform1 = rigidBody1->GetFinalTransform();
	const Transform4D& transform2 = rigidBody2->GetFinalTransform();
	Point3D centerMass1 = transform1 * rigidBody1->GetBodyCenterOfMass();
	Point3D centerMass2 = transform2 * rigidBody2->GetBodyCenterOfMass();

	for (machine a = 0; a < subcontactCount; a++)
	{
		if (subcontact[a].activeFlag)
		{
			float frictionCoefficient = Fmin(rigidBody1->GetFrictionCoefficient(), rigidBody2->GetFrictionCoefficient());
			if (frictionCoefficient > 0.0F)
			{
				Point3D worldPosition1 = transform1 * subcontact[a].alphaPosition;
				Point3D worldPosition2 = transform2 * subcontact[a].betaPosition;
				Antivector3D worldNormal1 = transform1 * subcontact[a].alphaNormal;

				Vector3D contactDirection1 = worldPosition1 - centerMass1;
				Vector3D contactDirection2 = worldPosition2 - centerMass2;
				Vector3D rotationalVelocity1 = rigidBody1->GetAngularVelocity() % contactDirection1;
				Vector3D rotationalVelocity2 = rigidBody2->GetAngularVelocity() % contactDirection2;
				Vector3D contactVelocity = rigidBody2->GetLinearVelocity() + rotationalVelocity2 - rigidBody1->GetLinearVelocity() - rotationalVelocity1;
				contactVelocity -= ProjectOnto(contactVelocity, worldNormal1);

				float d = SquaredMag(contactVelocity);
				if (d > K::min_float)
				{
					Constraint		constraint[2];

					contactVelocity *= InverseSqrt(d);

					Vector3D tangentX = transform1 * subcontact[a].alphaTangent[0];
					Vector3D tangentY = transform1 * subcontact[a].alphaTangent[1];

					machine constraintIndex = a * 4;
					float limit = appliedImpulse[constraintIndex] * frictionCoefficient;
					float limitX = Fabs(contactVelocity * tangentX) * limit;
					float limitY = Fabs(contactVelocity * tangentY) * limit;

					constraint[0].row[0].Set(tangentX, contactDirection1 % tangentX);
					constraint[0].row[1].Set(-tangentX, tangentX % contactDirection2);
					constraint[0].bias = 0.0F;
					constraint[0].index = constraintIndex + 1;
					constraint[0].range.Set(-limitX, limitX);

					constraint[1].row[0].Set(tangentY, contactDirection1 % tangentY);
					constraint[1].row[1].Set(-tangentY, tangentY % contactDirection2);
					constraint[1].bias = 0.0F;
					constraint[1].index = constraintIndex + 2;
					constraint[1].range.Set(-limitY, limitY);

					SolveVelocityConstraints(rigidBody1, rigidBody2, 2, constraint);
				}
			}
		}
	}

	rigidBody1->Integrate();
	rigidBody2->Integrate();
}

void RigidBodyContact::ApplyPositionConstraints(void)
{
	RigidBodyController *rigidBody1 = static_cast<RigidBodyController *>(GetStartElement());
	RigidBodyController *rigidBody2 = static_cast<RigidBodyController *>(GetFinishElement());
	const Transform4D& transform1 = rigidBody1->GetFinalTransform();
	const Transform4D& transform2 = rigidBody2->GetFinalTransform();

	for (machine a = 0; a < subcontactCount; a++)
	{
		Point3D worldPosition1 = transform1 * subcontact[a].alphaPosition;
		Point3D worldPosition2 = transform2 * subcontact[a].betaPosition;
		Vector3D deltaWorldPosition = worldPosition2 - worldPosition1;
		Antivector3D worldNormal1 = transform1 * subcontact[a].alphaNormal;

		float separation = deltaWorldPosition * worldNormal1;
		if (separation < kContactEpsilon)
		{
			Constraint		constraint;

			Point3D centerMass1 = transform1 * rigidBody1->GetBodyCenterOfMass();
			Point3D centerMass2 = transform2 * rigidBody2->GetBodyCenterOfMass();

			constraint.row[0].Set(-worldNormal1, worldNormal1 % (worldPosition1 - centerMass1));
			constraint.row[1].Set(worldNormal1, (worldPosition2 - centerMass2) % worldNormal1);
			constraint.bias = separation * kContactStabilizeFactor;

			float positionImpulse = CalculatePositionImpulse(rigidBody1, rigidBody2, &constraint);
			rigidBody1->ApplyPositionCorrection(constraint.row[0], positionImpulse);
			rigidBody2->ApplyPositionCorrection(constraint.row[1], positionImpulse);
		}
	}

	rigidBody1->Integrate();
	rigidBody2->Integrate();
}

void RigidBodyContact::GetWorldContactPosition(const RigidBodyController *rigidBody, Point3D *position, Antivector3D *normal) const
{
	if (GetStartElement() == rigidBody)
	{
		*position = static_cast<RigidBodyController *>(GetFinishElement())->GetFinalTransform() * subcontact[0].betaPosition;
		*normal = rigidBody->GetFinalTransform() * subcontact[0].alphaNormal;
	}
	else
	{
		const Transform4D& worldTransform = static_cast<RigidBodyController *>(GetStartElement())->GetFinalTransform();
		*position = worldTransform * subcontact[0].alphaPosition;
		*normal = worldTransform * -subcontact[0].alphaNormal;
	}
}


DeformableContact::DeformableContact(Body *body1, Body *body2) : Contact(kContactDeformable, body1, body2)
{
}

DeformableContact::~DeformableContact()
{
}


JointContact::JointContact(Joint *joint, RigidBodyController *body1, RigidBodyController *body2) : ConstraintContact(kContactJoint, body1, body2)
{
	SetJoint(joint);
}

JointContact::JointContact(ContactType type, Joint *joint, Body *body1, Body *body2) : ConstraintContact(type, body1, body2)
{
	SetJoint(joint);
}

JointContact::JointContact(Body *nullBody) : ConstraintContact(kContactJoint, nullBody, nullBody)
{
}

JointContact::JointContact(ContactType type, Body *nullBody) : ConstraintContact(type, nullBody, nullBody)
{
}

JointContact::~JointContact()
{
}

void JointContact::Pack(Packer& data, unsigned_int32 packFlags) const
{
	ConstraintContact::Pack(data, packFlags);

	if (contactJoint->LinkedNodePackable(packFlags))
	{
		data << ChunkHeader('JONT', 4);
		data << contactJoint->GetNodeIndex();
	}

	data << TerminatorChunk;
}

void JointContact::Unpack(Unpacker& data, unsigned_int32 unpackFlags)
{
	ConstraintContact::Unpack(data, unpackFlags);
	UnpackChunkList<JointContact>(data, unpackFlags);
}

bool JointContact::UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags)
{
	switch (chunkHeader->chunkType)
	{
		case 'JONT':
		{
			int32	nodeIndex;

			data >> nodeIndex;
			data.AddNodeLink(nodeIndex, &JointLinkProc, this);
			return (true);
		}
	}

	return (false);
}

void JointContact::JointLinkProc(Node *node, void *cookie)
{
	JointContact *jointContact = static_cast<JointContact *>(cookie);

	Joint *joint = static_cast<Joint *>(node);
	jointContact->SetJoint(joint);

	joint->jointContact = jointContact;
}

void JointContact::SetJoint(Joint *joint)
{
	contactJoint = joint;
	SetConstraintSolverMultiplier(joint->GetObject()->GetConstraintSolverMultiplier());
}

void JointContact::ApplyVelocityConstraints(void)
{
	contactJoint->ApplyVelocityConstraints(this);
}

void JointContact::ApplyPositionConstraints(void)
{
	contactJoint->ApplyPositionConstraints(this);
}


StaticJointContact::StaticJointContact(Joint *joint, RigidBodyController *rigidBody, Body *nullBody) : JointContact(kContactStaticJoint, joint, rigidBody, nullBody)
{
}

StaticJointContact::StaticJointContact(Body *nullBody) : JointContact(kContactStaticJoint, nullBody)
{
}

StaticJointContact::~StaticJointContact()
{
}

void StaticJointContact::ApplyVelocityConstraints(void)
{
	GetContactJoint()->ApplyStaticVelocityConstraints(this);
}

void StaticJointContact::ApplyPositionConstraints(void)
{
	GetContactJoint()->ApplyStaticPositionConstraints(this);
}


#if C4DIAGS

	ContactRenderable::ContactRenderable(const ColorRGBA& color, const char *texture) :
			Renderable(kRenderQuads),
			colorAttribute(color),
			textureAttribute(texture),
			vertexBuffer(kVertexBufferAttribute | kVertexBufferDynamic)
	{
		SetAmbientBlendState(kBlendInterpolate);

		attributeList.Append(&colorAttribute);
		attributeList.Append(&textureAttribute);
		SetMaterialAttributeList(&attributeList);
	}

	ContactRenderable::~ContactRenderable()
	{
	}


	ContactVectorRenderable::ContactVectorRenderable(const Subcontact *subcontact, const ColorRGBA& color) : ContactRenderable(color, "C4/vector")
	{
		SetShaderFlags(kShaderVertexPolyboard);

		SetVertexAttributeArray(kArrayPosition, 0, 3);
		SetVertexAttributeArray(kArrayTangent, sizeof(Point3D), 4);
		SetVertexAttributeArray(kArrayTexcoord, sizeof(Point3D) + sizeof(Vector4D), 2);

		SetVertexBuffer(kVertexBufferAttributeArray, &vertexBuffer, sizeof(ContactVertex));
		vertexBuffer.Establish(sizeof(ContactVertex) * ConstraintContact::kMaxSubcontactCount * 4);

		ContactVectorRenderable::UpdateContact(1, subcontact);
	}

	ContactVectorRenderable::~ContactVectorRenderable()
	{
	}

	void ContactVectorRenderable::UpdateContact(int32 count, const Subcontact *subcontact)
	{
		// Texture map is 32 x 256 pixels.
		// Quad is 0.02 x 0.16 meters.

		SetVertexCount(count * 4);

		volatile ContactVertex *restrict vertex = vertexBuffer.BeginUpdate<ContactVertex>();

		for (machine a = 0; a < count; a++)
		{
			const Vector3D& normal = subcontact->alphaNormal;
			Point3D p1 = subcontact->alphaPosition - normal * 0.01F;
			Point3D p2 = subcontact->alphaPosition + normal * 0.15F;

			vertex[0].position = p1;
			vertex[0].tangent.Set(normal, -0.01F);
			vertex[0].texcoord.Set(0.0F, 0.0F);

			vertex[1].position = p1;
			vertex[1].tangent.Set(normal, 0.01F);
			vertex[1].texcoord.Set(1.0F, 0.0F);

			vertex[2].position = p2;
			vertex[2].tangent.Set(normal, 0.01F);
			vertex[2].texcoord.Set(1.0F, 1.0F);

			vertex[3].position = p2;
			vertex[3].tangent.Set(normal, -0.01F);
			vertex[3].texcoord.Set(0.0F, 1.0F);

			subcontact++;
			vertex += 4;
		}

		vertexBuffer.EndUpdate();
	}


	ContactPointRenderable::ContactPointRenderable(const Subcontact *subcontact, const ColorRGBA& color) : ContactRenderable(color, "C4/contact")
	{
		SetShaderFlags(kShaderVertexBillboard);

		SetVertexAttributeArray(kArrayPosition, 0, 3);
		SetVertexAttributeArray(kArrayBillboard, sizeof(Point3D), 2);
		SetVertexAttributeArray(kArrayTexcoord, sizeof(Point3D) + sizeof(Vector2D), 2);

		SetVertexBuffer(kVertexBufferAttributeArray, &vertexBuffer, sizeof(ContactVertex));
		vertexBuffer.Establish(sizeof(ContactVertex) * ConstraintContact::kMaxSubcontactCount * 4);

		ContactPointRenderable::UpdateContact(1, subcontact);
	}

	ContactPointRenderable::~ContactPointRenderable()
	{
	}

	void ContactPointRenderable::UpdateContact(int32 count, const Subcontact *subcontact)
	{
		SetVertexCount(count * 4);

		volatile ContactVertex *restrict vertex = vertexBuffer.BeginUpdate<ContactVertex>();

		for (machine a = 0; a < count; a++)
		{
			vertex[0].position = subcontact->betaPosition;
			vertex[0].billboard.Set(-0.02F, 0.02F);
			vertex[0].texcoord.Set(0.0F, 0.0F);

			vertex[1].position = subcontact->betaPosition;
			vertex[1].billboard.Set(0.02F, 0.02F);
			vertex[1].texcoord.Set(1.0F, 0.0F);

			vertex[2].position = subcontact->betaPosition;
			vertex[2].billboard.Set(0.02F, -0.02F);
			vertex[2].texcoord.Set(1.0F, 1.0F);

			vertex[3].position = subcontact->betaPosition;
			vertex[3].billboard.Set(-0.02F, -0.02F);
			vertex[3].texcoord.Set(0.0F, 1.0F);

			subcontact++;
			vertex += 4;
		}

		vertexBuffer.EndUpdate();
	}

#endif

// ZYUQURM
