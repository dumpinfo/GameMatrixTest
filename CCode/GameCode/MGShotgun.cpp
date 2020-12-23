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


#include "MGShotgun.h"
#include "MGFighter.h"
#include "MGGame.h"


using namespace C4;


namespace
{
	Storage<Shotgun> shotgunStorage;
}


Shotgun::Shotgun() :
		Weapon(kWeaponShotgun, 0, kModelShotgun, kActionShotgun, 24, 48),
		shotgunModelReg(kModelShotgun, TheGame->GetStringTable()->GetString(StringID('MODL', 'WEAP', kModelShotgun)), "shotgun/gun/Shotgun", kModelPrecache, kControllerCollectable),
		shellAmmoModelReg(kModelShellAmmo, TheGame->GetStringTable()->GetString(StringID('MODL', 'AMMO', kModelShellAmmo)), "shotgun/ammo/Ammo", kModelPrecache, kControllerCollectable),
		shellModelRegistration(kModelShell, nullptr, "shotgun/shell/Shell", kModelPrecache | kModelPrivate),
		shotgunAction(kWeaponShotgun)
{
	TheInputMgr->AddAction(&shotgunAction);
}

Shotgun::~Shotgun()
{
}

void Shotgun::Construct(void)
{
	new(shotgunStorage) Shotgun;
}

void Shotgun::Destruct(void)
{
	shotgunStorage->~Shotgun();
}

WeaponController *Shotgun::NewWeaponController(FighterController *fighter) const
{
	return (new ShotgunController(fighter));
}


ShotgunController::ShotgunController(FighterController *fighter) : WeaponController(kControllerShotgun, fighter)
{
}

ShotgunController::~ShotgunController()
{
}

void ShotgunController::Preprocess(void)
{
	WeaponController::Preprocess();

	int32 markerCount = 0;
	int32 beamCount = 0;

	Model *model = GetTargetNode();
	Node *node = model->GetFirstSubnode();
	while (node)
	{
		NodeType type = node->GetNodeType();
		if (type == kNodeMarker)
		{
			const Marker *marker = static_cast<Marker *>(node);
			if (marker->GetMarkerType() == kMarkerLocator)
			{
				const LocatorMarker *locator = static_cast<const LocatorMarker *>(marker);
				if (locator->GetLocatorType() == 'shel')
				{
					shellEjectMarker[markerCount++] = locator;
				}
			}
		}
		else if (type == kNodeLight)
		{
			muzzleLight = static_cast<Light *>(node);
		}
		else if (type == kNodeEffect)
		{
			Effect *effect = static_cast<Effect *>(node);
			EffectType effectType = effect->GetEffectType();
			if (effectType == kEffectBeam)
			{
				muzzleFlashBeam[beamCount++] = static_cast<BeamEffect *>(effect);
			}
			else if (effectType == kEffectQuad)
			{
				muzzleFlashQuad = static_cast<QuadEffect *>(effect);
			}
		}
		else if (node->GetNodeHash() == Text::StaticHash<'D', 'r', 'u', 'm'>::value)
		{
			drumNode = node;
		}

		node = model->GetNextNode(node);
	}
 
	drumShellCount = 0;
	currentDrumAngle = 0.0F;
	targetDrumAngle = 0.0F; 

	flashTime = 0; 
	pumpTime = 0;

	frameAnimator.SetTargetModel(model); 
	frameAnimator.SetAnimation("shotgun/gun/Fire");
	model->SetRootAnimator(&frameAnimator); 
} 

ControllerMessage *ShotgunController::CreateMessage(ControllerMessageType type) const
{
	switch (type) 
	{
		case kShotgunMessageFire:

			return (new ShotgunFireMessage(GetControllerIndex()));

		case kShotgunMessageImpact:

			return (new ShotgunImpactMessage(GetControllerIndex()));

		case kShotgunMessageAdvance:

			return (new ShotgunAdvanceMessage(GetControllerIndex()));
	}

	return (WeaponController::CreateMessage(type));
}

void ShotgunController::Move(void)
{
	int32 dt = TheTimeMgr->GetDeltaTime();

	if (muzzleLight->Enabled())
	{
		int32 time = flashTime;
		if (time <= 0)
		{
			muzzleLight->Disable();
			muzzleFlashBeam[0]->Disable();
			muzzleFlashBeam[1]->Disable();
			muzzleFlashQuad->Disable();
		}
		else
		{
			flashTime = time - dt;
		}
	}

	int32 time = pumpTime;
	if (time > 0)
	{
		pumpTime = (time -= dt);
		if (time <= 0)
		{
			OmniSource *source = new OmniSource("shotgun/gun/Pump", 32.0F);
			source->SetSourcePriority(kSoundPriorityWeapon);
			source->SetNodeFlags(kNodeCloneInhibit | kNodeAnimateInhibit);
			source->SetNodePosition(Point3D(0.25F, -0.125F, 1.0F));

			const FighterController *fighter = GetFighterController();
			source->SetSourceVelocity(fighter->GetLinearVelocity());
			fighter->GetTargetNode()->AppendNewSubnode(source);
		}
	}

	float angle = currentDrumAngle;
	float target = targetDrumAngle;
	if (angle < target)
	{
		angle += TheTimeMgr->GetFloatDeltaTime() * 0.005F;
		if (!(angle < target))
		{
			if (target >= 8.0F)
			{
				target = 0.0F;
			}

			targetDrumAngle = target;
			angle = target;
		}

		currentDrumAngle = angle;

		drumNode->SetNodeMatrix3D(Matrix3D().SetRotationAboutX(angle * K::tau_over_8));
		drumNode->Invalidate();
	}

	GetTargetNode()->Animate();
}

void ShotgunController::ShowFireAnimation(bool secondary)
{
	muzzleLight->Enable();

	muzzleFlashBeam[0]->Enable();
	if (secondary)
	{
		muzzleFlashBeam[1]->Enable();
	}

	muzzleFlashQuad->Enable();
	muzzleFlashQuad->SetQuadOrientation(Math::Random(256));

	flashTime = 20;
	pumpTime = 300;

	Interpolator *interpolator = frameAnimator.GetFrameInterpolator();
	interpolator->Set(0.0F, 1.0F, kInterpolatorForward);

	int32 shellCount = 1 + secondary;
	for (machine a = 0; a < shellCount; a++)
	{
		Model *shellModel = Model::Get(kModelShell);
		ShellController *controller = new ShellController(5000);
		shellModel->SetController(controller);

		const Transform4D& transform = shellEjectMarker[a]->GetWorldTransform();
		shellModel->SetNodeTransform(transform);
		GetTargetNode()->GetWorld()->AddNewNode(shellModel);

		controller->SetLinearVelocity(GetFighterController()->GetLinearVelocity() * 0.5F + (transform[2] - transform[1]) * 1.25F);
		controller->SetAngularVelocity(Math::RandomUnitVector3D() * Math::RandomFloat(10.0F));
	}
}

WeaponResult ShotgunController::UpdateWeapon(const Point3D& position, const Vector3D& direction, const Point3D& center)
{
	if (TheMessageMgr->Server())
	{
		int32 dt = TheTimeMgr->GetDeltaTime();
		int32 time = GetFireDelayTime() - dt;

		FighterController *fighter = GetFighterController();
		unsigned_int32 flags = fighter->GetFighterFlags();

		if (((flags & kFighterFiring) != 0) && (time <= 0))
		{
			static const ConstVector2D shotVector[5] =
			{
				{0.0F, 0.0F}, {2.0F, 0.0F}, {0.0F, 2.0F}, {-2.0F, 0.0F}, {0.0F, -2.0F}
			};

			CollisionData							collisionData;
			Array<GameRigidBodyController *, 5>		breakableArray;

			SetFireDelayTime(time + 800);

			GamePlayer *player = fighter->GetFighterPlayer();
			int32 ammo = player->GetWeaponAmmo(kWeaponShotgun, 0);
			if (ammo <= 0)
			{
				return (kWeaponEmpty);
			}

			bool secondary = (((flags & kFighterFiringSecondary) != 0) && (ammo >= 2));
			int32 shellCount = 1 + secondary;
			player->SetWeaponAmmo(kWeaponShotgun, 0, ammo - shellCount);

			TheMessageMgr->SendMessageAll(ShotgunFireMessage(GetControllerIndex(), position, secondary));

			if ((drumShellCount += shellCount) >= 6)
			{
				drumShellCount -= 6;
				TheMessageMgr->SendMessageAll(ShotgunAdvanceMessage(GetControllerIndex()));
			}

			float m = Magnitude(direction.GetVector2D());
			Vector3D right(direction.y * m, -direction.x * m, 0.0F);
			Vector3D down = direction % right;
			Vector3D forward = direction * 25.0F;

			const World *world = GetTargetNode()->GetWorld();
			for (machine a = 0; a < 5; a++)
			{
				const Vector2D& v = shotVector[a];
				Vector3D shotDirection = forward + right * v.x + down * v.y;

				CollisionState state = world->QueryCollision(position, position + shotDirection, 0.0F, kCollisionProjectile, &collisionData);
				if (state == kCollisionStateGeometry)
				{
					if (a == 0)
					{
						Game::ProcessGeometryProperties(collisionData.geometry, collisionData.position, fighter->GetTargetNode());

						SubstanceType substance = kSubstanceNone;
						const MaterialObject *material = collisionData.geometry->GetTriangleMaterial(collisionData.triangleIndex);
						if (material)
						{
							substance = material->GetMaterialSubstance();
						}

						TheMessageMgr->SendMessageAll(ShotgunImpactMessage(GetControllerIndex(), collisionData.position, collisionData.normal, substance));
					}
				}
				else if (state == kCollisionStateRigidBody)
				{
					Fixed damage = (secondary) ? 20 << 16 : 10 << 16;
					RigidBodyController *rigidBody = collisionData.rigidBody;

					RigidBodyType type = rigidBody->GetRigidBodyType();
					if (type == kRigidBodyCharacter)
					{
						if (rigidBody != fighter)
						{
							Vector3D force = shotDirection * ((secondary) ? 6.0F : 3.0F);
							GameCharacterController *character = static_cast<GameCharacterController *>(rigidBody);
							character->Damage(damage, 0, fighter, &collisionData.position, &force);
						}
					}
					else if (type == kRigidBodyProjectile)
					{
						Vector3D normal = rigidBody->GetLinearVelocity();

						m = SquaredMag(normal);
						if (m > K::min_float)
						{
							normal *= -InverseSqrt(m);
						}
						else
						{
							normal.Set(0.0F, 0.0F, 1.0F);
						}

						static_cast<ProjectileController *>(rigidBody)->Destroy(collisionData.position, normal);
					}
					else
					{
						const Node *node = rigidBody->GetTargetNode();
						const Transform4D& inverseTransform = node->GetInverseWorldTransform();

						Vector3D impulse = inverseTransform * shotDirection;
						impulse *= (secondary) ? 0.02F : 0.015F;
						ProjectileController::ApplyRigidBodyImpulse(rigidBody, impulse, inverseTransform * collisionData.position);

						SubstanceType substance = collisionData.shape->GetObject()->GetShapeSubstance();
						TheMessageMgr->SendMessageAll(ShotgunImpactMessage(GetControllerIndex(), collisionData.position, collisionData.normal, substance));

						ControllerType controllerType = rigidBody->GetControllerType();
						if (controllerType == kControllerGameRigidBody)
						{
							if (SquaredMag(collisionData.position - position) < 25.0F)
							{
								const Property *property = node->GetSharedProperty(kPropertyBreakable);
								if (property)
								{
									GameRigidBodyController *gameRigidBody = static_cast<GameRigidBodyController *>(rigidBody);
									if (breakableArray.FindElement(gameRigidBody) < 0)
									{
										breakableArray.AddElement(gameRigidBody);
									}
								}
							}
						}
						else if (controllerType == kControllerExplosive)
						{
							static_cast<ExplosiveController *>(rigidBody)->Damage(damage, 0.0F, fighter);
						}
					}
				}
			}

			for (GameRigidBodyController *gameRigidBody : breakableArray)
			{
				gameRigidBody->BreakApart(0, fighter->GetTargetNode());
			}

			return (kWeaponFired);
		}
		else
		{
			SetFireDelayTime(time);
		}
	}

	return (kWeaponIdle);
}


ShotgunFireMessage::ShotgunFireMessage(int32 index) : ControllerMessage(ShotgunController::kShotgunMessageFire, index)
{
}

ShotgunFireMessage::ShotgunFireMessage(int32 index, const Point3D& position, bool secondary) : ControllerMessage(ShotgunController::kShotgunMessageFire, index)
{
	firePosition = position;
	fireSecondary = secondary;
}

ShotgunFireMessage::~ShotgunFireMessage()
{
}

void ShotgunFireMessage::Compress(Compressor& data) const
{
	ControllerMessage::Compress(data);

	data << firePosition;
	data << fireSecondary;
}

bool ShotgunFireMessage::Decompress(Decompressor& data)
{
	if (ControllerMessage::Decompress(data))
	{
		data >> firePosition;
		data >> fireSecondary;
		return (true);
	}

	return (false);
}

void ShotgunFireMessage::HandleControllerMessage(Controller *controller) const
{
	ShotgunController *shotgunController = static_cast<ShotgunController *>(controller);
	shotgunController->ShowFireAnimation(fireSecondary);

	OmniSource *source = new OmniSource("shotgun/gun/Shotgun", 64.0F);
	source->SetSourcePriority(kSoundPriorityWeapon);
	source->SetSourceVelocity(shotgunController->GetFighterController()->GetLinearVelocity());
	source->SetNodePosition(firePosition);
	shotgunController->GetTargetNode()->GetWorld()->AddNewNode(source);

	if (fireSecondary)
	{
		source->SetSourceVolume(1.5F);
		source->SetSourceFrequency(0.875F);
	}
}


ShotgunImpactMessage::ShotgunImpactMessage(int32 index) : ControllerMessage(ShotgunController::kShotgunMessageImpact, index)
{
}

ShotgunImpactMessage::ShotgunImpactMessage(int32 index, const Point3D& position, const Vector3D& normal, SubstanceType substance) : ControllerMessage(ShotgunController::kShotgunMessageImpact, index)
{
	impactPosition = position;
	impactNormal = normal;
	impactSubstance = substance;
}

ShotgunImpactMessage::~ShotgunImpactMessage()
{
}

void ShotgunImpactMessage::Compress(Compressor& data) const
{
	ControllerMessage::Compress(data);

	data << impactPosition;
	data << impactNormal;
	data << impactSubstance;
}

bool ShotgunImpactMessage::Decompress(Decompressor& data)
{
	if (ControllerMessage::Decompress(data))
	{
		data >> impactPosition;
		data >> impactNormal;
		data >> impactSubstance;
		return (true);
	}

	return (false);
}

void ShotgunImpactMessage::HandleControllerMessage(Controller *controller) const
{
	MaterialType materialType = kMaterialGeneric;
	const SubstanceData *substanceData = nullptr;

	if (impactSubstance != kSubstanceNone)
	{
		const GameSubstance *substance = static_cast<const GameSubstance *>(MaterialObject::FindRegisteredSubstance(impactSubstance));
		if (substance)
		{
			substanceData = substance->GetSubstanceData();
			materialType = substanceData->bulletHoleMaterialType;
		}
	}

	World *world = controller->GetTargetNode()->GetWorld();

	if (materialType == kMaterialGeneric)
	{
		if (substanceData)
		{
			Point3D		position[5];

			Vector3D tangent = Math::CreateUnitPerpendicular(impactNormal);
			Vector3D bitangent = impactNormal % tangent;

			const ConstVector2D *trig = Math::GetTrigTable();
			for (machine a = 0; a < 5; a++)
			{
				Vector2D cs = trig[Math::Random(256)] * Math::RandomFloat(0.5F);
				position[a] = impactPosition + tangent * cs.x + bitangent * cs.y;
			}

			DustParticleSystem *dust = new DustParticleSystem(5, position, ColorRGBA(0.5F, 0.25F, 0.125F, 1.0F));
			dust->SetNodePosition(impactPosition);
			world->AddNewNode(dust);
		}
	}
	else
	{
		SparksParticleSystem *sparks = new SparksParticleSystem(25);
		sparks->SetNodePosition(impactPosition);
		world->AddNewNode(sparks);
	}
}


ShotgunAdvanceMessage::ShotgunAdvanceMessage(int32 index) : ControllerMessage(ShotgunController::kShotgunMessageAdvance, index)
{
}

ShotgunAdvanceMessage::~ShotgunAdvanceMessage()
{
}

void ShotgunAdvanceMessage::HandleControllerMessage(Controller *controller) const
{
	static_cast<ShotgunController *>(controller)->AdvanceDrum();
}


ShellController::ShellController() : RemainsController(kControllerShell, 0)
{
}

ShellController::ShellController(int32 life) : RemainsController(kControllerShell, life)
{
	bounceCount = 0;
}

ShellController::~ShellController()
{
}

void ShellController::Pack(Packer& data, unsigned_int32 packFlags) const
{
	RemainsController::Pack(data, packFlags);

	data << ChunkHeader('BONC', 4);
	data << bounceCount;

	data << TerminatorChunk;
}

void ShellController::Unpack(Unpacker& data, unsigned_int32 unpackFlags)
{
	RemainsController::Unpack(data, unpackFlags);
	UnpackChunkList<ShellController>(data, unpackFlags);
}

bool ShellController::UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags)
{
	switch (chunkHeader->chunkType)
	{
		case 'BONC':

			data >> bounceCount;
			return (true);
	}

	return (false);
}

RigidBodyStatus ShellController::HandleNewGeometryContact(const GeometryContact *contact)
{
	if ((contact->GetSubcontact(0)->alphaNormal.z < 0.5F) && (++bounceCount == 1))
	{
		static const char soundName[2][32] =
		{
			"shotgun/shell/Shell-hard1", "shotgun/shell/Shell-hard2"
		};

		OmniSource *source = new OmniSource(soundName[Math::Random(2)], 16.0F);
		source->SetNodePosition(GetFinalTransform().GetTranslation());
		GetTargetNode()->GetWorld()->AddNewNode(source);
	}

	return (kRigidBodyUnchanged);
}

// ZYUQURM
