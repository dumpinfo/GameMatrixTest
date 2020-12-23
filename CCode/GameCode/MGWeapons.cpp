//=============================================================
//
// C4 Engine version 4.5
// Copyright 1999-2015, by Terathon Software LLC
//
// This file is part of the C4 Engine and is provided under the
// terms of the license agreement entered by the registed user.
//
// Unauthorized redistribution of source code is strictly
// prohibited. Violators will be prosecuted.
//
//=============================================================


#include "MGWeapons.h"
#include "MGFighter.h"
#include "MGGame.h"


using namespace C4;


const Weapon *Weapon::weaponTable[kWeaponCount] = {nullptr};

Texture *Weapon::smokeTexture;
Texture *Weapon::sparkTexture;
Texture *Weapon::scorchTexture;
Texture *Weapon::trailTexture;
Texture *Weapon::flareTexture;
Texture *Weapon::shockTexture;
Texture *Weapon::warpTexture;
Texture *Weapon::distortTexture;
Texture *Weapon::flameTexture;


Weapon::Weapon(int32 index, unsigned_int32 flags, ModelType type, ActionType action, int32 ammo1, int32 maxAmmo1, int32 ammo2, int32 maxAmmo2)
{
	weaponIndex = index;
	weaponFlags = flags;

	modelType = type;
	actionType = action;

	initWeaponAmmo[0] = ammo1;
	maxWeaponAmmo[0] = maxAmmo1;
	initWeaponAmmo[1] = ammo2;
	maxWeaponAmmo[1] = maxAmmo2;

	weaponTable[index] = this;
}

ModelType Weapon::WeaponIndexToType(int32 weaponIndex)
{
	const Weapon *weapon = weaponTable[weaponIndex];
	return ((weapon) ? weapon->GetModelType() : 0);
}

int32 Weapon::WeaponTypeToIndex(ModelType weaponType)
{
	for (machine a = kWeaponNone + 1; a < kWeaponCount; a++)
	{
		const Weapon *weapon = weaponTable[a];
		if ((weapon) && (weapon->GetModelType() == weaponType))
		{
			return (a);
		}
	}

	return (kWeaponNone);
}

void Weapon::PreloadTextures(void)
{
	smokeTexture = Texture::Get("particle/Puff1");
	sparkTexture = Texture::Get("particle/Spark1");
	scorchTexture = Texture::Get("texture/Scorch");
	trailTexture = Texture::Get("texture/Trail");
	flareTexture = Texture::Get("texture/LightFlare");
	shockTexture = Texture::Get("effects/Shock");
	warpTexture = Texture::Get("effects/Warp");
	distortTexture = Texture::Get("effects/Heat");
	flameTexture = Texture::Get("fire/Flame");
}

void Weapon::ReleaseTextures(void)
{
	flameTexture->Release();
	distortTexture->Release();
	warpTexture->Release();
	shockTexture->Release();
	flareTexture->Release();
	trailTexture->Release();
	scorchTexture->Release();
	sparkTexture->Release();
	smokeTexture->Release();
}

WeaponController *Weapon::NewWeaponController(FighterController *fighter) const
{
	return (nullptr);
}


WeaponController::WeaponController(ControllerType type, FighterController *fighter) : Controller(type)
{
	fighterController = fighter;
	fireDelayTime = 0;
}

WeaponController::~WeaponController()
{
}

ControllerMessage *WeaponController::CreateMessage(ControllerMessageType type) const
{ 
	switch (type)
	{
		case kWeaponMessageReset: 

			return (new WeaponResetMessage(GetControllerIndex())); 
	}

	return (nullptr); 
}
 
void WeaponController::BeginFiring(bool primary) 
{
}

void WeaponController::EndFiring(void) 
{
}

WeaponResult WeaponController::UpdateWeapon(const Point3D& position, const Vector3D& direction, const Point3D& center)
{
	return (kWeaponIdle);
}

void WeaponController::ResetWeapon(void)
{
}


WeaponResetMessage::WeaponResetMessage(int32 index) : ControllerMessage(WeaponController::kWeaponMessageReset, index)
{
}

WeaponResetMessage::~WeaponResetMessage()
{
}

void WeaponResetMessage::HandleControllerMessage(Controller *controller) const
{
	static_cast<WeaponController *>(controller)->ResetWeapon();
}


ProjectileController::ProjectileController(ControllerType type) : RigidBodyController(type)
{
	velocityFlag = false;
	positionFlag = false;

	SetRigidBodyType(kRigidBodyProjectile);
	SetCollisionKind(kCollisionProjectile);
	SetCollisionExclusionMask(kCollisionSightPath | kCollisionSoundPath | kCollisionCorpse);
}

ProjectileController::ProjectileController(ControllerType type, const Vector3D& velocity, GameCharacterController *attacker) :
		RigidBodyController(type),
		attackerController(attacker)
{
	initialVelocity = velocity;

	velocityFlag = true;
	positionFlag = false;

	SetRigidBodyType(kRigidBodyProjectile);
	SetCollisionKind(kCollisionProjectile);
	SetCollisionExclusionMask(kCollisionSightPath | kCollisionSoundPath | kCollisionCorpse);
}

ProjectileController::ProjectileController(const ProjectileController& projectileController) :
		RigidBodyController(projectileController),
		attackerController(projectileController.attackerController)
{
	initialVelocity = projectileController.initialVelocity;

	velocityFlag = true;
	positionFlag = false;
}

ProjectileController::~ProjectileController()
{
}

void ProjectileController::Pack(Packer& data, unsigned_int32 packFlags) const
{
	RigidBodyController::Pack(data, packFlags);

	if (velocityFlag)
	{
		data << ChunkHeader('VELO', sizeof(Vector3D));
		data << initialVelocity;
	}

	if (positionFlag)
	{
		data << ChunkHeader('FPOS', sizeof(Point3D));
		data << firstPosition;
	}

	const GameCharacterController *attacker = attackerController;
	if (attacker)
	{
		data << ChunkHeader('ATCK', 4);
		data << attacker->GetTargetNode()->GetNodeIndex();
	}

	data << TerminatorChunk;
}

void ProjectileController::Unpack(Unpacker& data, unsigned_int32 unpackFlags)
{
	RigidBodyController::Unpack(data, unpackFlags);
	UnpackChunkList<ProjectileController>(data, unpackFlags);
}

bool ProjectileController::UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags)
{
	switch (chunkHeader->chunkType)
	{
		case 'VELO':

			data >> initialVelocity;
			velocityFlag = true;
			return (true);

		case 'FPOS':

			data >> firstPosition;
			positionFlag = true;
			return (true);

		case 'ATCK':
		{
			int32	nodeIndex;

			data >> nodeIndex;
			data.AddNodeLink(nodeIndex, &AttackerLinkProc, this);
			return (true);
		}
	}

	return (false);
}

void ProjectileController::AttackerLinkProc(Node *node, void *cookie)
{
	ProjectileController *controller = static_cast<ProjectileController *>(cookie);
	controller->attackerController = static_cast<GameCharacterController *>(node->GetController());
}

void ProjectileController::Preprocess(void)
{
	RigidBodyController::Preprocess();

	if (velocityFlag)
	{
		velocityFlag = false;
		SetLinearVelocity(initialVelocity);
	}
}

bool ProjectileController::ValidRigidBodyCollision(const RigidBodyController *body) const
{
	if (RigidBodyController::ValidRigidBodyCollision(body))
	{
		return (body != attackerController);
	}

	return (false);
}

void ProjectileController::HandlePhysicsSpaceExit(void)
{
	if (TheMessageMgr->Server())
	{
		Destroy(GetTargetNode()->GetWorldPosition(), Normalize(-GetLinearVelocity()));
	}
}

void ProjectileController::EnterWorld(World *world, const Point3D& worldPosition)
{
}

void ProjectileController::SetSourceVelocity(OmniSource *source)
{
	const GameCharacterController *attacker = attackerController;
	if (attacker)
	{
		source->SetSourceVelocity(attacker->GetLinearVelocity());
	}
}

int32 ProjectileController::GetAttackerIndex(void) const
{
	const GameCharacterController *controller = attackerController;
	if (controller)
	{
		return (controller->GetControllerIndex());
	}

	return (kControllerUnassigned);
}

Transform4D ProjectileController::CalculateProjectileTransform(const Point3D& position, const Vector3D& velocity)
{
	Vector3D zdir = velocity * InverseMag(velocity);
	float m = InverseSqrt(zdir.x * zdir.x + zdir.y * zdir.y);
	Vector3D xdir(zdir.y * m, -zdir.x * m, 0.0F);
	return (Transform4D(xdir, zdir % xdir, zdir, position));
}

void ProjectileController::Teleport(const Point3D& startPosition, const Transform4D& teleportTransform, const Vector3D& teleportVelocity)
{
	static const char *const soundName[2] =
	{
		"sound/Teleport-A", "sound/Teleport-B"
	};

	SetRigidBodyTransform(teleportTransform);
	SetLinearVelocity(teleportVelocity);
	PurgeContacts();

	World *world = GetTargetNode()->GetWorld();

	ColorRGB color(2.0F, 2.0F, 2.0F);
	PointLight *light = new PointLight(color, 4.0F);
	light->GetObject()->SetLightFlags(kLightShadowInhibit);
	light->SetController(new FlashController(color, 0.75F, 300));
	light->SetNodePosition(startPosition);
	world->AddNewNode(light);

	const char *name = soundName[Math::Random(2)];

	OmniSource *source = new OmniSource(name, 64.0F);
	source->SetSourcePriority(kSoundPriorityImpact);
	source->SetNodePosition(startPosition);
	world->AddNewNode(source);

	source = new OmniSource(name, 64.0F);
	source->SetNodePosition(teleportTransform.GetTranslation());
	world->AddNewNode(source);
}

void ProjectileController::ApplyRigidBodyImpulse(RigidBodyController *rigidBody, const Vector3D& impulse, const Point3D& position)
{
	float d = SquaredMag(impulse);
	if (d > K::min_float)
	{
		float f = InverseSqrt(d);
		float m = Fmin(d * f * rigidBody->GetInverseBodyMass(), 4.0F) * rigidBody->GetBodyMass();
		rigidBody->ApplyImpulse(impulse * (m * f), position);
	}
}

// ZYUQURM
