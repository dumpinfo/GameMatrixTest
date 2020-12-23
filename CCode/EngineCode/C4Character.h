 

#ifndef C4Character_h
#define C4Character_h


//# \component	Physics Manager
//# \prefix		PhysicsMgr/


#include "C4Physics.h"


namespace C4
{
	//# \class	CharacterController		Manages a basic character model.
	//
	//# The $CharacterController$ class manages a basic character model.
	//
	//# \def	class CharacterController : public RigidBodyController
	//
	//# \ctor	CharacterController(ControllerType type);
	//
	//# The constructor has protected access. The $CharacterController$ class can only be used as a base class for another class.
	//
	//# \param	type	The controller type.
	//
	//# \desc
	//# The $CharacterController$ class is an extension of the $@RigidBodyController@$ class,
	//# and it implements a small amount of functionality that is commonly needed for basic characters.
	//#
	//# The $CharacterController$ class expects its target node to have exact one $@Shape@$ subnode.
	//# To faciliate robust and fast character physics, the character controller always keeps this shape
	//# in a fixed orientation. The $@CharacterController::SetCharacterOrientation@$ function should always
	//# be used to change the direction in which the character faces.
	//
	//# \base	PhysicsMgr/RigidBodyController		A $CharacterController$ is a specific type of rigid body controller.


	//# \function	CharacterController::GetGroundCosine	Returns the minimum ground cosine value.
	//
	//# \proto	float GetGroundCosine(void) const;
	//
	//# \desc
	//# The $GetGroundCosine$ function returns the minimum dot product between a surface normal and the up direction for
	//# which the surface is considered to be the ground. This value is used by the $@CharacterController::GetGroundContact@$
	//# when determining whether any geometry contact represents contact with the ground.
	//#
	//# The default ground cosine is 0.6875, and the up direction is always (0,0,1) in world space.
	//
	//# \also	$@CharacterController::SetGroundCosine@$
	//# \also	$@CharacterController::GetGroundContact@$


	//# \function	CharacterController::SetGroundCosine	Sets the minimum ground cosine value.
	//
	//# \proto	void SetGroundCosine(float cosine);
	//
	//# \param	cosine		The new minimum ground cosine.
	//
	//# \desc
	//# The $SetGroundCosine$ function sets the minimum dot product between a surface normal and the up direction for
	//# which the surface is considered to be the ground to the value specified by the $cosine$ parameter. This value
	//# is used by the $@CharacterController::GetGroundContact@$ when determining whether any geometry contact represents
	//# contact with the ground.
	//#
	//# The default ground cosine is 0.6875, and the up direction is always (0,0,1) in world space.
	//
	//# \also	$@CharacterController::GetGroundCosine@$
	//# \also	$@CharacterController::GetGroundContact@$


	//# \function	CharacterController::GetGroundContact	Returns a contact with the ground.
	//
	//# \proto	const CollisionContact *GetGroundContact(void) const;
	//
	//# \desc
	//# The $GetGroundContact$ function returns a pointer to a collision contact with the ground, if such a contact exists.
	//# Whether a character is in contact with the ground is determined by searching for any contact for which the dot product
	//# between the surface normal and the up direction is greater than the minimum ground cosine. The first contact found that
	//# satisfies this condition is returned. If no ground contact is found, then the return value is $nullptr$.
	//#
	//# The minimum ground cosine can be changed using the $@CharacterController::SetGroundCosine@$ function. The default
	//# value is 0.6875. The up direction is always (0,0,1) in world space.
	//
	//# \also	$@CharacterController::GetGroundCosine@$
	//# \also	$@CharacterController::SetGroundCosine@$


	//# \function	CharacterController::SetCharacterOrientation	Sets the orientation of a character model.
	//
	//# \proto	void SetCharacterOrientation(float azimuth, float roll = 0.0F);
	//
	//# \param	azimuth		The angle in the <i>x</i>-<i>y</i> plane representing the direction the character is facing.
	//# \param	roll		The roll angle about the character's local <i>x</i> axis.
	//
	//# \desc
	//# The $SetCharacterOrientation$ function sets the orientation of a character model to the angles specified by the
	//# $azimuth$ and $roll$ parameters. The azimuth angle is measured counterclockwise in the <i>x</i>-<i>y</i> plane,
	//# and zero corresponds to the positive <i>x</i> axis. If the roll angle is not zero, then the character is rotated
	//# about the <i>x</i> axis by the angle specified by the $roll$ parameter before the azimuth rotation is applied.
	//#
	//# The $SetCharacterOrientation$ function should always be called to change the orientation of a character using 
	//# the $CharacterController$ class instead of setting the transform directly. When the orientation of a character is
	//# changed, the $@Shape@$ subnode is transformed in the inverse manner so that it always maintains a fixed orientation
	//# in the world. 

 
	class CharacterController : public RigidBodyController
	{
		private: 

			Transform4D		shapeTransform; 
			bool			shapeInitFlag; 

			float			groundCosine;

		protected: 

			C4API CharacterController(ControllerType type);
			C4API CharacterController(const CharacterController& characterController);

		public:

			C4API ~CharacterController();

			float GetGroundCosine(void) const
			{
				return (groundCosine);
			}

			void SetGroundCosine(float cosine)
			{
				groundCosine = cosine;
			}

			C4API void Pack(Packer& data, unsigned_int32 packFlags) const override;
			C4API void Unpack(Unpacker& data, unsigned_int32 unpackFlags) override;
			bool UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags);

			C4API void Preprocess(void) override;

			C4API const CollisionContact *GetGroundContact(void) const;
			C4API void SetCharacterOrientation(float azimuth, float roll = 0.0F);
	};
}


#endif

// ZYUQURM
