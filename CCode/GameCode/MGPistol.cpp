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


#include "MGPistol.h"
#include "MGFighter.h"
#include "MGGame.h"


using namespace C4;


namespace
{
	Storage<Pistol> pistolStorage;
}


Pistol::Pistol() :
		Weapon(kWeaponPistol, 0, kModelPistol, kActionPistol, 144, 144),
		pistolModelRegistration(kModelPistol, TheGame->GetStringTable()->GetString(StringID('MODL', 'WEAP', kModelPistol)), "pistol/gun/Pistol", kModelPrecache, kControllerCollectable),
		bulletAmmoModelRegistration(kModelBulletAmmo, TheGame->GetStringTable()->GetString(StringID('MODL', 'AMMO', kModelBulletAmmo)), "pistol/ammo/Ammo", kModelPrecache, kControllerCollectable),

		stoneBulletHoleMaterialRegistration(kMaterialBulletHoleStone, "pistol/hole/Stone"),
		woodBulletHoleMaterialRegistration(kMaterialBulletHoleWood, "pistol/hole/Wood"),
		metalBulletHoleMaterialRegistration(kMaterialBulletHoleMetal, "pistol/hole/Metal"),
		glassBulletHoleMaterialRegistration(kMaterialBulletHoleGlass, "pistol/hole/Glass"),

		pistolAction(kWeaponPistol)
{
	TheInputMgr->AddAction(&pistolAction);
}

Pistol::~Pistol()
{
}

void Pistol::Construct(void)
{
	new(pistolStorage) Pistol;
}

void Pistol::Destruct(void)
{
	pistolStorage->~Pistol();
}

WeaponController *Pistol::NewWeaponController(FighterController *fighter) const
{
	return (new PistolController(fighter));
}


PistolController::PistolController(FighterController *fighter) : WeaponController(kControllerPistol, fighter)
{
}

PistolController::~PistolController()
{
}

void PistolController::Preprocess(void)
{
	WeaponController::Preprocess();

	Model *model = GetTargetNode();
	Node *node = model->GetFirstSubnode();
	while (node)
	{
		NodeType type = node->GetNodeType();
		if (type == kNodeLight)
		{
			muzzleLight = static_cast<Light *>(node);
		}
		else if (type == kNodeEffect)
		{
			Effect *effect = static_cast<Effect *>(node);
			EffectType effectType = effect->GetEffectType();
			if (effectType == kEffectBeam)
			{
				muzzleFlashBeam = static_cast<BeamEffect *>(effect);
			}
			else if (effectType == kEffectQuad)
			{
				muzzleFlashQuad = static_cast<QuadEffect *>(effect);
			}
		}

		node = model->GetNextNode(node);
	}

	flashTime = 0;

	frameAnimator.SetTargetModel(model);
	frameAnimator.SetAnimation("pistol/gun/Fire");
	model->SetRootAnimator(&frameAnimator);
}

ControllerMessage *PistolController::CreateMessage(ControllerMessageType type) const
{
	switch (type)
	{
		case kPistolMessageFire:

			return (new PistolFireMessage(GetControllerIndex())); 

		case kPistolMessageImpact:
 
			return (new PistolImpactMessage(GetControllerIndex()));
	} 

	return (WeaponController::CreateMessage(type));
} 

void PistolController::Move(void) 
{ 
	if (muzzleLight->Enabled())
	{
		int32 time = flashTime;
		if (time <= 0) 
		{
			muzzleLight->Disable();
			muzzleFlashBeam->Disable();
			muzzleFlashQuad->Disable();
		}
		else
		{
			flashTime = time - TheTimeMgr->GetDeltaTime();
		}
	}

	GetTargetNode()->Animate();
}

void PistolController::ShowFireAnimation(void)
{
	flashTime = 20;

	muzzleLight->Enable();
	muzzleFlashBeam->Enable();
	muzzleFlashQuad->Enable();
	muzzleFlashQuad->SetQuadOrientation(Math::Random(256));

	frameAnimator.GetFrameInterpolator()->Set(0.0F, 1.0F, kInterpolatorForward);
}

WeaponResult PistolController::UpdateWeapon(const Point3D& position, const Vector3D& direction, const Point3D& center)
{
	if (TheMessageMgr->Server())
	{
		int32 dt = TheTimeMgr->GetDeltaTime();
		int32 time = GetFireDelayTime() - dt;

		FighterController *fighter = GetFighterController();
		unsigned_int32 flags = fighter->GetFighterFlags();

		if (((flags & kFighterFiring) != 0) && (time <= 0))
		{
			CollisionData	collisionData;

			SetFireDelayTime(time + 300);

			GamePlayer *player = fighter->GetFighterPlayer();
			int32 ammo = player->GetWeaponAmmo(kWeaponPistol, 0);
			if (ammo <= 0)
			{
				return (kWeaponEmpty);
			}

			player->SetWeaponAmmo(kWeaponPistol, 0, ammo - 1);

			TheMessageMgr->SendMessageAll(PistolFireMessage(GetControllerIndex(), position));

			const GameWorld *world = static_cast<GameWorld *>(GetTargetNode()->GetWorld());
			CollisionState state = world->QueryCollision(position, position + direction * 100.0F, 0.0F, kCollisionProjectile, &collisionData);
			if (state == kCollisionStateGeometry)
			{
				Game::ProcessGeometryProperties(collisionData.geometry, collisionData.position, fighter->GetTargetNode());

				SubstanceType substance = kSubstanceNone;
				const MaterialObject *material = collisionData.geometry->GetTriangleMaterial(collisionData.triangleIndex);
				if (material)
				{
					substance = material->GetMaterialSubstance();
				}

				TheMessageMgr->SendMessageAll(PistolImpactMessage(GetControllerIndex(), collisionData.position, collisionData.normal, substance));
			}
			else if (state == kCollisionStateRigidBody)
			{
				RigidBodyController *rigidBody = collisionData.rigidBody;

				RigidBodyType type = rigidBody->GetRigidBodyType();
				if (type == kRigidBodyCharacter)
				{
					if (rigidBody != fighter)
					{
						GameCharacterController *character = static_cast<GameCharacterController *>(rigidBody);
						character->Damage(8 << 16, 0, fighter, &collisionData.position);
					}
				}
				else if (type == kRigidBodyProjectile)
				{
					Vector3D normal = rigidBody->GetLinearVelocity();

					float m = SquaredMag(normal);
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
					Node *node = rigidBody->GetTargetNode();
					const Transform4D& inverseTransform = node->GetInverseWorldTransform();
					Point3D nodePosition = inverseTransform * collisionData.position;

					Vector3D impulse = inverseTransform * direction * 0.125F;
					ProjectileController::ApplyRigidBodyImpulse(rigidBody, impulse, nodePosition);

					SubstanceType substance = collisionData.shape->GetObject()->GetShapeSubstance();
					TheMessageMgr->SendMessageAll(PistolImpactMessage(GetControllerIndex(), collisionData.position, collisionData.normal, substance));

					ControllerType controllerType = rigidBody->GetControllerType();
					if (controllerType == kControllerGameRigidBody)
					{
						Property *property = node->GetProperty(kPropertyLeakingGoo);
						if (property)
						{
							LeakingGooProperty *leakingGooProperty = static_cast<LeakingGooProperty *>(property);
							int32 count = leakingGooProperty->GetMaxLeakCount();
							if (count > 0)
							{
								leakingGooProperty->SetMaxLeakCount(count - (TheGame->GetGameDetailLevel() + 1));
								TheMessageMgr->SendMessageAll(LeakGooMessage(rigidBody->GetControllerIndex(), nodePosition, collisionData.normal * node->GetWorldTransform()));
							}
						}
					}
					else if (controllerType == kControllerExplosive)
					{
						static_cast<ExplosiveController *>(rigidBody)->Damage(8 << 16, 0.0F, fighter);
					}
				}
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


PistolFireMessage::PistolFireMessage(int32 index) : ControllerMessage(PistolController::kPistolMessageFire, index)
{
}

PistolFireMessage::PistolFireMessage(int32 index, const Point3D& position) : ControllerMessage(PistolController::kPistolMessageFire, index)
{
	firePosition = position;
}

PistolFireMessage::~PistolFireMessage()
{
}

void PistolFireMessage::Compress(Compressor& data) const
{
	ControllerMessage::Compress(data);

	data << firePosition;
}

bool PistolFireMessage::Decompress(Decompressor& data)
{
	if (ControllerMessage::Decompress(data))
	{
		data >> firePosition;
		return (true);
	}

	return (false);
}

void PistolFireMessage::HandleControllerMessage(Controller *controller) const
{
	PistolController *pistolController = static_cast<PistolController *>(controller);
	pistolController->ShowFireAnimation();

	OmniSource *source = new OmniSource("pistol/gun/Pistol", 32.0F);
	source->SetSourcePriority(kSoundPriorityWeapon);
	source->SetSourceVelocity(pistolController->GetFighterController()->GetLinearVelocity());
	source->SetNodePosition(firePosition);
	pistolController->GetTargetNode()->GetWorld()->AddNewNode(source);
}


PistolImpactMessage::PistolImpactMessage(int32 index) : ControllerMessage(PistolController::kPistolMessageImpact, index)
{
}

PistolImpactMessage::PistolImpactMessage(int32 index, const Point3D& position, const Vector3D& normal, SubstanceType substance) : ControllerMessage(PistolController::kPistolMessageImpact, index)
{
	impactPosition = position;
	impactNormal = normal;
	impactSubstance = substance;
}

PistolImpactMessage::~PistolImpactMessage()
{
}

void PistolImpactMessage::Compress(Compressor& data) const
{
	ControllerMessage::Compress(data);

	data << impactPosition;
	data << impactNormal;
	data << impactSubstance;
}

bool PistolImpactMessage::Decompress(Decompressor& data)
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

void PistolImpactMessage::HandleControllerMessage(Controller *controller) const
{
	static const char impactSoundName[2][32] =
	{
		"pistol/gun/Impact1", "pistol/gun/Impact2"
	};

	MarkingData		markingData;

	World *world = controller->GetTargetNode()->GetWorld();

	markingData.center = impactPosition;
	markingData.normal = impactNormal;
	markingData.tangent = Math::CreatePerpendicular(impactNormal);
	markingData.tangent.RotateAboutAxis(Math::RandomFloat(K::tau), impactNormal);
	markingData.radius = 0.028F;
	markingData.clip.Set(-0.1F, 0.028F);
	markingData.color.Set(1.0F, 1.0F, 1.0F, 1.0F);
	markingData.lifeTime = 30000;

	MaterialType materialType = kMaterialGeneric;
	const SubstanceData *substanceData = nullptr;
	const char *soundName = "";

	if (impactSubstance != kSubstanceNone)
	{
		const GameSubstance *substance = static_cast<const GameSubstance *>(MaterialObject::FindRegisteredSubstance(impactSubstance));
		if (substance)
		{
			substanceData = substance->GetSubstanceData();
			materialType = substanceData->bulletHoleMaterialType;
			soundName = substanceData->bulletImpactSoundName;
		}
	}

	if (materialType == kMaterialGeneric)
	{
		if (substanceData)
		{
			DustParticleSystem *dust = new DustParticleSystem(1, &impactPosition, ColorRGBA(0.5F, 0.25F, 0.125F, 1.0F));
			dust->SetNodePosition(impactPosition);
			world->AddNewNode(dust);
		}
		else
		{
			markingData.textureName = "pistol/hole/Hole-diff";
			MarkingEffect::New(world, &markingData);
		}
	}
	else
	{
		AutoReleaseMaterial material(materialType);
		markingData.materialObject = material;
		markingData.markingFlags = substanceData->bulletHoleMarkingFlags;
		markingData.color.GetColorRGB() = substanceData->bulletHoleColor;
		MarkingEffect::New(world, &markingData);

		SparksParticleSystem *sparks = new SparksParticleSystem(5);
		sparks->SetNodePosition(impactPosition);
		world->AddNewNode(sparks);
	}

	OmniSource *source = new OmniSource((soundName[0] != 0) ? soundName : impactSoundName[Math::Random(2)], 32.0F);
	source->GetObject()->SetReflectionVolume(0.25F);
	source->SetSourcePriority(kSoundPriorityImpact);
	source->SetNodePosition(impactPosition);
	world->AddNewNode(source);
}

// ZYUQURM
