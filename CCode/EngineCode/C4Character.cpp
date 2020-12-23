 

#include "C4Character.h"


using namespace C4;


CharacterController::CharacterController(ControllerType type) : RigidBodyController(type)
{
	groundCosine = 0.6875F;
	shapeInitFlag = false;
}

CharacterController::CharacterController(const CharacterController& characterController) : RigidBodyController(characterController)
{
	groundCosine = characterController.groundCosine;
	shapeInitFlag = false;
}

CharacterController::~CharacterController()
{
}

void CharacterController::Pack(Packer& data, unsigned_int32 packFlags) const
{
	RigidBodyController::Pack(data, packFlags);

	data << ChunkHeader('GCOS', 4);
	data << groundCosine;

	if (shapeInitFlag)
	{
		data << ChunkHeader('SHAP', sizeof(Transform4D));
		data << shapeTransform;
	}

	data << TerminatorChunk;
}

void CharacterController::Unpack(Unpacker& data, unsigned_int32 unpackFlags)
{
	RigidBodyController::Unpack(data, unpackFlags);
	UnpackChunkList<CharacterController>(data, unpackFlags);
}

bool CharacterController::UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags)
{
	switch (chunkHeader->chunkType)
	{
		case 'GCOS':

			data >> groundCosine;
			return (true);

		case 'SHAP':

			data >> shapeTransform;
			shapeInitFlag = true;
			return (true);
	}

	return (false);
}

void CharacterController::Preprocess(void)
{
	SetRigidBodyFlags(kRigidBodyKeepAwake | kRigidBodyFixedOrientation);
	RigidBodyController::Preprocess();

	if (!GetTargetNode()->GetManipulator())
	{
		const Shape *shape = GetFirstShape();
		Assert(shape, "CharacterController::Preprocess(), a character must have a shape subnode\n");

		if (!shapeInitFlag)
		{
			shapeInitFlag = true;
			shapeTransform = shape->GetNodeTransform();
		}
	}
}

const CollisionContact *CharacterController::GetGroundContact(void) const
{
	const Contact *contact = GetFirstOutgoingEdge();
	while (contact)
	{
		ContactType type = contact->GetContactType();
		if (type == kContactGeometry)
		{
			const GeometryContact *geometryContact = static_cast<const GeometryContact *>(contact);
			if ((GetFinalTransform().GetRow(2) ^ geometryContact->GetRigidBodyContactNormal()) < -groundCosine)
			{
				return (geometryContact);
			}
		}
		else if (type == kContactRigidBody)
		{
			const RigidBodyContact *rigidBodyContact = static_cast<const RigidBodyContact *>(contact);
			if ((GetFinalTransform().GetRow(2) ^ rigidBodyContact->GetContactNormal()) < -groundCosine)
			{
				return (rigidBodyContact); 
			}
		}
 
		contact = contact->GetNextOutgoingEdge();
	} 

	contact = GetFirstIncomingEdge();
	while (contact) 
	{
		if (contact->GetContactType() == kContactRigidBody) 
		{ 
			const RigidBodyContact *rigidBodyContact = static_cast<const RigidBodyContact *>(contact);
			const RigidBodyController *rigidBody = static_cast<RigidBodyController *>(rigidBodyContact->GetStartElement());
			const Transform4D& transform = rigidBody->GetFinalTransform();
			if ((transform.GetRow(2) ^ rigidBodyContact->GetContactNormal()) > groundCosine) 
			{
				return (rigidBodyContact);
			}
		}

		contact = contact->GetNextIncomingEdge();
	}

	return (nullptr);
}

void CharacterController::SetCharacterOrientation(float azimuth, float roll)
{
	Matrix3D	rotation;

	if (roll == 0.0F)
	{
		rotation.SetRotationAboutZ(azimuth);
	}
	else
	{
		Vector2D zrot = CosSin(azimuth);
		Vector3D xrot = CosSin(roll);
		rotation.Set(zrot.x, -xrot.x * zrot.y, xrot.y * zrot.y, zrot.y, xrot.x * zrot.x, -xrot.y * zrot.x, 0.0F, xrot.y, xrot.x);
	}

	SetRigidBodyMatrix3D(Transform(GetTargetNode()->GetSuperNode()->GetInverseWorldTransform(), rotation));
	GetFirstShape()->SetNodeTransform(TransposeTransform(rotation, shapeTransform));
}

// ZYUQURM
