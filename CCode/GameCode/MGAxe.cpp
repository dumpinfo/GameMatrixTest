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


#include "MGAxe.h"
#include "MGFighter.h"
#include "MGGame.h"


using namespace C4;


namespace
{
	Storage<Axe> axeStorage;
}


Axe::Axe() :
		Weapon(kWeaponAxe, 0, kModelAxe, 0, 0, 0),
		axeModelRegistration(kModelAxe, nullptr, "axe/Axe", kModelPrecache | kModelPrivate)
{
}

Axe::~Axe()
{
}

void Axe::Construct(void)
{
	new(axeStorage) Axe;
}

void Axe::Destruct(void)
{
	axeStorage->~Axe();
}

WeaponController *Axe::NewWeaponController(FighterController *fighter) const
{
	return (new AxeController(fighter));
}


AxeController::AxeController(FighterController *fighter) :
		WeaponController(kControllerAxe, fighter),
		frameAnimatorObserver(this, &AxeController::HandleAnimationEvent)
{
}

AxeController::~AxeController()
{
}

void AxeController::Preprocess(void)
{
	WeaponController::Preprocess();

	Model *model = GetTargetNode();

	frameAnimator.SetTargetModel(model);
	frameAnimator.SetAnimation("axe/Attack1");
	model->SetRootAnimator(&frameAnimator);

	if (TheMessageMgr->Server())
	{
		frameAnimator.SetObserver(&frameAnimatorObserver);
	}
}

ControllerMessage *AxeController::CreateMessage(ControllerMessageType type) const
{
	switch (type)
	{
		case kAxeMessageSwing:

			return (new AxeSwingMessage(GetControllerIndex()));

		case kAxeMessageImpact:

			return (new AxeImpactMessage(GetControllerIndex()));
	}

	return (WeaponController::CreateMessage(type));
}

void AxeController::Move(void)
{
	GetTargetNode()->Animate();
}

void AxeController::ShowSwingAnimation(void)
{
	OmniSource *source = new OmniSource("axe/Swing", 16.0F);
	source->GetObject()->SetInitialSourceFrequency(Math::RandomFloat(0.2F) + 0.9F);
	source->SetSourcePriority(kSoundPriorityWeapon);
	GetTargetNode()->AppendNewSubnode(source);

	frameAnimator.GetFrameInterpolator()->Set(0.0F, 1.25F, kInterpolatorForward);
}

WeaponResult AxeController::UpdateWeapon(const Point3D& position, const Vector3D& direction, const Point3D& center)
{
	if (TheMessageMgr->Server())
	{ 
		int32 dt = TheTimeMgr->GetDeltaTime();
		int32 time = GetFireDelayTime() - dt;
 
		const FighterController *fighter = GetFighterController();
		unsigned_int32 flags = fighter->GetFighterFlags(); 

		if (((flags & kFighterFiring) != 0) && (time <= 0))
		{ 
			SetFireDelayTime(time + 500);
 
			TheMessageMgr->SendMessageAll(AxeSwingMessage(GetControllerIndex())); 
			return (kWeaponFired);
		}
		else
		{ 
			SetFireDelayTime(time);
		}
	}

	return (kWeaponIdle);
}

void AxeController::HandleAnimationEvent(FrameAnimator *animator, CueType cueType)
{
	if (cueType == 'DAMG')
	{
		CollisionData	collisionData;

		FighterController *fighter = GetFighterController();
		const GameWorld *world = static_cast<GameWorld *>(GetTargetNode()->GetWorld());

		Point3D position = fighter->GetTargetNode()->GetWorldPosition();
		position.z += kCameraPositionHeight;

		CollisionState state = world->QueryCollision(position, position + fighter->GetFiringDirection() * 2.0F, 0.25F, kCollisionProjectile, &collisionData, fighter);
		if (state == kCollisionStateGeometry)
		{
			Game::ProcessGeometryProperties(collisionData.geometry, collisionData.position, fighter->GetTargetNode());

			SubstanceType substance = kSubstanceNone;
			const MaterialObject *material = collisionData.geometry->GetTriangleMaterial(collisionData.triangleIndex);
			if (material)
			{
				substance = material->GetMaterialSubstance();
			}

			TheMessageMgr->SendMessageAll(AxeImpactMessage(GetControllerIndex(), collisionData.position, collisionData.normal, substance));
		}
		else if (state == kCollisionStateRigidBody)
		{
			Fixed damage = 40 << 16;
			RigidBodyController *rigidBody = collisionData.rigidBody;

			RigidBodyType type = rigidBody->GetRigidBodyType();
			if (type == kRigidBodyCharacter)
			{
				GameCharacterController *character = static_cast<GameCharacterController *>(rigidBody);
				character->Damage(damage, 0, fighter, &collisionData.position);

				TheMessageMgr->SendMessageAll(AxeImpactMessage(GetControllerIndex(), collisionData.position, collisionData.normal, kSubstanceFlesh));
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
				const Node *node = rigidBody->GetTargetNode();
				const Transform4D& inverseTransform = node->GetInverseWorldTransform();

				Vector3D impulse = inverseTransform * fighter->GetFiringDirection() * 0.125F;
				ProjectileController::ApplyRigidBodyImpulse(rigidBody, impulse, inverseTransform * collisionData.position);

				SubstanceType substance = collisionData.shape->GetObject()->GetShapeSubstance();
				TheMessageMgr->SendMessageAll(AxeImpactMessage(GetControllerIndex(), collisionData.position, collisionData.normal, substance));

				ControllerType controllerType = rigidBody->GetControllerType();
				if (controllerType == kControllerGameRigidBody)
				{
					const Property *property = node->GetSharedProperty(kPropertyBreakable);
					if (property)
					{
						static_cast<GameRigidBodyController *>(rigidBody)->BreakApart(0, fighter->GetTargetNode());
					}
				}
				else if (controllerType == kControllerExplosive)
				{
					static_cast<ExplosiveController *>(rigidBody)->Damage(damage, 0.0F, fighter);
				}
			}
		}
	}
}


AxeSwingMessage::AxeSwingMessage(int32 index) : ControllerMessage(AxeController::kAxeMessageSwing, index)
{
}

AxeSwingMessage::~AxeSwingMessage()
{
}

void AxeSwingMessage::HandleControllerMessage(Controller *controller) const
{
	AxeController *axeController = static_cast<AxeController *>(controller);
	axeController->ShowSwingAnimation();
}


AxeImpactMessage::AxeImpactMessage(int32 index) : ControllerMessage(AxeController::kAxeMessageImpact, index)
{
}

AxeImpactMessage::AxeImpactMessage(int32 index, const Point3D& position, const Vector3D& normal, SubstanceType substance) : ControllerMessage(AxeController::kAxeMessageImpact, index)
{
	impactPosition = position;
	impactNormal = normal;
	impactSubstance = substance;
}

AxeImpactMessage::~AxeImpactMessage()
{
}

void AxeImpactMessage::Compress(Compressor& data) const
{
	ControllerMessage::Compress(data);

	data << impactPosition;
	data << impactNormal;
	data << impactSubstance;
}

bool AxeImpactMessage::Decompress(Decompressor& data)
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

void AxeImpactMessage::HandleControllerMessage(Controller *controller) const
{
	if (impactSubstance != kSubstanceNone)
	{
		Node *node = controller->GetTargetNode();
		World *world = node->GetWorld();

		if (impactSubstance == kSubstanceFlesh)
		{
			static const char soundName[2][32] =
			{
				"spike/spike/Flesh1", "spike/spike/Flesh2"
			};

			OmniSource *source = new OmniSource(soundName[Math::Random(2)], 32.0F);
			source->SetSourcePriority(kSoundPriorityImpact);
			source->SetNodePosition(impactPosition);
			world->AddNewNode(source);
		}
		else
		{
			const GameSubstance *substance = static_cast<const GameSubstance *>(MaterialObject::FindRegisteredSubstance(impactSubstance));
			if (substance)
			{
				const SubstanceData *substanceData = substance->GetSubstanceData();
				const char *soundName = substanceData->axeImpactSoundName;
				if (soundName[0] != 0)
				{
					OmniSource *source = new OmniSource(soundName, 32.0F);
					source->GetObject()->SetInitialSourceFrequency(Math::RandomFloat(0.1F) + 0.95F);
					source->SetSourcePriority(kSoundPriorityImpact);
					source->SetNodePosition(impactPosition);
					world->AddNewNode(source);

					int32 effectType = substanceData->axeImpactEffectType;
					if (effectType == kAxeEffectSparks)
					{
						SparksParticleSystem *sparks = new SparksParticleSystem(5);
						sparks->SetNodePosition(impactPosition);
						world->AddNewNode(sparks);
					}
				}
			}
		}
	}
}

// ZYUQURM
