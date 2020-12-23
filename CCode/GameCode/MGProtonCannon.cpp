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


#include "MGProtonCannon.h"
#include "MGFighter.h"
#include "MGGame.h"


using namespace C4;


namespace
{
	const float kProtonBeamRange			= 24.0F;
	const float kMaxProtonBarrelSpeed		= 0.015F;
	const float kProtonEmissionFactor		= 4.0F / kMaxProtonBarrelSpeed;

	enum
	{
		kMaxProtonFlameLife		= 8000,
		kMaxProtonSparkLife		= 900
	};

	Storage<ProtonCannon> protonCannonStorage;
}


ProtonCannon::ProtonCannon() :
		Weapon(kWeaponProtonCannon, 0, kModelProtonCannon, kActionProtonCannon, 100, 200),
		protonAmmoBeamRegistration(kParticleSystemProtonAmmoBeam, TheGame->GetStringTable()->GetString(StringID('PART', kParticleSystemProtonAmmoBeam))),
		protonCannonModelRegistration(kModelProtonCannon, TheGame->GetStringTable()->GetString(StringID('MODL', 'WEAP', kModelProtonCannon)), "proton/gun/ProtonCannon", kModelPrecache, kControllerCollectable),
		protonAmmoModelRegistration(kModelProtonAmmo, TheGame->GetStringTable()->GetString(StringID('MODL', 'AMMO', kModelProtonAmmo)), "proton/ammo/Ammo", kModelPrecache, kControllerCollectable),
		protonCannonAction(kWeaponProtonCannon)
{
	TheInputMgr->AddAction(&protonCannonAction);

	protonScorchTexture = Texture::Get("proton/gun/Scorch");
}

ProtonCannon::~ProtonCannon()
{
	protonScorchTexture->Release();
}

void ProtonCannon::Construct(void)
{
	new(protonCannonStorage) ProtonCannon;
}

void ProtonCannon::Destruct(void)
{
	protonCannonStorage->~ProtonCannon();
}

WeaponController *ProtonCannon::NewWeaponController(FighterController *fighter) const
{
	return (new ProtonCannonController(fighter));
}


ProtonFlames::ProtonFlames() :
		FireParticleSystem(kParticleSystemProtonFlames, &particlePool, "fire/Flame"),
		particlePool(kMaxParticleCount, particleArray)
{
	SetNodeFlags(kNodeNonpersistent);
}

ProtonFlames::~ProtonFlames()
{
}

void ProtonFlames::NewFlame(const Point3D& position)
{
	FireParticle *particle = particlePool.NewParticle();
	if (particle)
	{
		particle->emitTime = 1;
		particle->lifeTime = kMaxProtonFlameLife - Math::Random(4000);

		particle->color.Set(1.0F, 1.0F, 1.0F, 1.0F);
		particle->position = position;

		particle->intensity = 0.25F;
		particle->noiseOffset.Set(Math::RandomFloat(1.0F), Math::RandomFloat(1.0F));
		FireTextureAttribute::CalculateNoiseVelocities(16, particle->noiseVelocity);

		AddParticle(particle);
	}
}

void ProtonFlames::AnimateParticles(void)
{
	int32 dt = TheTimeMgr->GetDeltaTime();

	Particle *particle = GetFirstParticle();
	while (particle)
	{
		Particle *next = particle->nextParticle;

		int32 time = (particle->lifeTime -= dt);
		if (time > 0)
		{
			particle->emitTime = 0;
 
			if (time >= 4000)
			{
				particle->radius = 0.125F; 
				particle->height = 0.5F;
			} 
			else
			{
				float h = (float) time * 0.000125F; 
				particle->radius = h * 0.25F;
				particle->height = h; 
			} 
		}
		else
		{
			FreeParticle(particle); 
		}

		particle = next;
	}
}


ProtonSparks::ProtonSparks() :
		LineParticleSystem(kParticleSystemProtonSparks, &particlePool, "particle/Spark1"),
		particlePool(kMaxParticleCount, particleArray)
{
	SetNodeFlags(kNodeNonpersistent);
	SetLengthMultiplier(2.0F);
}

ProtonSparks::~ProtonSparks()
{
}

void ProtonSparks::NewSparks(const Point3D& position, const Vector3D& normal, int32 count)
{
	Vector3D tangent1 = Math::CreateUnitPerpendicular(normal);
	Vector3D tangent2 = normal % tangent1;

	const ConstVector2D *trig = Math::GetTrigTable();

	for (machine a = 0; a < count; a++)
	{
		Particle *particle = particlePool.NewParticle();
		if (!particle)
		{
			break;
		}

		particle->emitTime = 0;
		particle->lifeTime = kMaxProtonSparkLife - Math::Random(600);
		particle->radius = 0.025F;
		float f = Math::RandomFloat(0.5F) + 0.5F;
		particle->color.Set(f, f, f * 0.1F, 1.0F);
		particle->orientation = 0;
		particle->position = position;

		float speed = Math::RandomFloat(0.007F) + 0.003F;
		Vector2D csp = trig[Math::Random(64)] * speed;
		const Vector2D& cst = trig[Math::Random(256)];
		particle->velocity = (tangent1 * cst.x + tangent2 * cst.y) * csp.y + normal * csp.x;

		AddParticle(particle);
	}
}

void ProtonSparks::AnimateParticles(void)
{
	int32 dt = TheTimeMgr->GetDeltaTime();
	float fdt = TheTimeMgr->GetFloatDeltaTime();

	Particle *particle = GetFirstParticle();
	while (particle)
	{
		Particle *next = particle->nextParticle;

		int32 life = (particle->lifeTime -= dt);
		if (life > 0)
		{
			particle->velocity.z += K::gravity * fdt;
			particle->position += particle->velocity * fdt;

			if (life < 100)
			{
				particle->color.alpha = (float) life * 0.01F;
			}
		}
		else
		{
			FreeParticle(particle);
		}

		particle = next;
	}
}


ProtonBeam::ProtonBeam(GameCharacterController *attacker) :
		PolyboardParticleSystem(kParticleSystemProtonBeam, &particlePool),
		protonLight(ColorRGB(4.0F, 4.0F, 4.0F), 10.0F),
		protonFlare(1.0F, 0.25F, 0.0F, "texture/LightFlare"),
		protonWarp(2.0F, K::white, "effects/Warp"),
		protonSource("proton/gun/Proton", 40.0F, true),
		shockSource("proton/gun/Shock", 80.0F, true),
		particlePool(kMaxParticleCount, particleArray)
{
	attackerController = attacker;

	markingTime = 0;
	flameTime = 0;
	crackleTime = 0;

	SetNodeFlags(kNodeNonpersistent);
}

ProtonBeam::~ProtonBeam()
{
	flamesSystem->SetParticleSystemFlags(kParticleSystemSelfDestruct);
	sparksSystem->SetParticleSystemFlags(kParticleSystemSelfDestruct);
}

bool ProtonBeam::CalculateBoundingSphere(BoundingSphere *sphere) const
{
	sphere->SetCenter(Point3D(kProtonBeamRange * 0.5F, 0.0F, 0.0F));
	sphere->SetRadius(kProtonBeamRange * 0.5F);
	return (true);
}

void ProtonBeam::Preprocess(void)
{
	static const ConstColorRGB beamColor[4] =
	{
		{1.0F, 0.0F, 0.0F},
		{1.0F, 1.0F, 0.0F},
		{1.0F, 0.375F, 0.0F},
		{0.125F, 1.0F, 0.0F}
	};

	static const float beamRadius[64] =
	{
		0.075F, 0.15F, 0.2F, 0.2F, 0.2F, 0.2F, 0.2F, 0.2F, 0.2F, 0.2F, 0.2F, 0.2F, 0.2F, 0.2F, 0.2F, 0.2F,
		0.2F, 0.2F, 0.2F, 0.2F, 0.2F, 0.2F, 0.2F, 0.2F, 0.2F, 0.2F, 0.2F, 0.2F, 0.2F, 0.2F, 0.2F, 0.2F,
		0.2F, 0.2F, 0.2F, 0.2F, 0.2F, 0.2F, 0.2F, 0.2F, 0.2F, 0.2F, 0.2F, 0.2F, 0.2F, 0.2F, 0.2F, 0.2F,
		0.2F, 0.2F, 0.2F, 0.2F, 0.2F, 0.2F, 0.2F, 0.2F, 0.2F, 0.2F, 0.2F, 0.2F, 0.2F, 0.2F, 0.2F, 0.2F
	};

	PolyboardParticleSystem::Preprocess();

	SetTransformable(this);
	SetParticleSystemFlags(kParticleSystemObjectSpace);

	flamesSystem = new ProtonFlames;
	sparksSystem = new ProtonSparks;
	flamesSystem->SetNodePosition(GetNodePosition());
	sparksSystem->SetNodePosition(GetNodePosition());

	World *world = attackerController->GetTargetNode()->GetWorld();
	world->AddNewNode(flamesSystem);
	world->AddNewNode(sparksSystem);

	LightObject *lightObject = protonLight.GetObject();
	lightObject->SetMinDetailLevel(1);

	if (TheWorldMgr->GetLightDetailLevel() < 3)
	{
		lightObject->SetLightFlags(kLightShadowInhibit);
	}

	AppendNewSubnode(&protonLight);
	protonLight.AppendNewSubnode(&protonFlare);

	QuadEffectObject *quadEffectObject = protonWarp.GetObject();
	quadEffectObject->SetQuadFlags(kQuadSoftDepth);
	quadEffectObject->SetQuadSoftDepthScale(2.0F);
	protonLight.AppendNewSubnode(&protonWarp);

	protonWarp.SetDistortionState();
	protonWarp.SetRenderState(kRenderDepthTest | kRenderDepthInhibit | kRenderDepthOffset);
	protonWarp.SetDepthOffset(1.0F, &protonWarp.GetWorldPosition());
	protonWarp.GetFirstRenderSegment()->SetMaterialState(kMaterialTwoSided);

	protonSource.GetObject()->SetSourceFlags(kSourceInitialPlay | kSourceLoop | kSourceReverb);
	protonSource.SetSourcePriority(kSoundPriorityWeapon);
	AppendNewSubnode(&protonSource);

	shockSource.GetObject()->SetSourceFlags(kSourceLoop | kSourceReverb);
	AppendNewSubnode(&shockSource);

	for (machine index = 0; index < 4; index++)
	{
		controlRadius[index][0] = 0.077F;
		controlRadius[index][1] = Math::RandomFloat(1.0F) + 0.25F;
		controlRadius[index][2] = Math::RandomFloat(1.0F) + 0.25F;
		controlRadius[index][3] = 0.0F;

		controlDistance[index][0] = 0.0F;

		controlVelocity[index][0] = 0.0F;
		controlVelocity[index][1] = Math::RandomFloat(0.01F) + 0.01F;
		controlVelocity[index][2] = Math::RandomFloat(0.01F) + 0.01F;
		controlVelocity[index][3] = 0.0F;

		for (machine a = 0; a < 4; a++)
		{
			controlAngle[index][a] = 0.0F;
		}

		for (machine a = 0; a < 64; a++)
		{
			PolyParticle *particle = particlePool.NewParticle();

			particle->emitTime = 0;
			particle->lifeTime = 1;
			particle->radius = beamRadius[a];
			particle->color.Set(beamColor[index], 1.0F);
			particle->texcoord = 0.53125F;
			particle->terminator = (a == 63);

			AddParticle(particle);
		}
	}
}

void ProtonBeam::AnimateParticles(void)
{
	int32 dt = TheTimeMgr->GetDeltaTime();

	if (markingTime > 0)
	{
		markingTime -= dt;
	}

	if (flameTime > 0)
	{
		flameTime -= dt;
	}

	if (crackleTime > 0)
	{
		crackleTime -= dt;
	}
}

void ProtonBeam::SetBarrelAngle(float angle)
{
	controlAngle[0][0] = angle;
	controlAngle[1][0] = angle + K::tau_over_4;
	controlAngle[2][0] = angle + K::tau_over_2;
	controlAngle[3][0] = angle - K::tau_over_4;
}

void ProtonBeam::SetBeamPosition(const Vector3D& firingDirection)
{
	CollisionData	collisionData;
	Point3D			lightPosition;

	const Point3D& p1 = GetWorldPosition();
	Point3D p2 = p1 + firingDirection * kProtonBeamRange;
	CollisionState state = GetWorld()->QueryCollision(p1, p2, 0.0F, kCollisionProjectile, &collisionData, attackerController);

	if (state != kCollisionStateNone)
	{
		p2 = GetInverseWorldTransform() * collisionData.position;

		float length = collisionData.param * kProtonBeamRange;
		float x1 = length * K::one_over_3;
		float x2 = x1 * 2.0F;

		for (machine index = 0; index < 4; index++)
		{
			controlDistance[index][1] = x1;
			controlDistance[index][2] = x2;
			controlDistance[index][3] = length;
		}

		bool sparksFlag = true;

		if (state == kCollisionStateGeometry)
		{
			if ((flameTime <= 0) && ((collisionData.geometry->GetObject()->GetGeometryFlags() & (kGeometryMarkingInhibit | kGeometryDynamic)) == 0))
			{
				float z = collisionData.normal.z;
				if (z > -0.25F)
				{
					flameTime = Math::Random(80);

					Vector3D tangent = Math::CreateUnitPerpendicular(collisionData.normal);
					tangent.RotateAboutAxis(Math::RandomFloat(K::tau), collisionData.normal);

					float offset = 0.125F - z * 0.125F;
					flamesSystem->NewFlame(collisionData.position + collisionData.normal * offset + tangent * Math::RandomFloat(0.25F));

					if (crackleTime <= 0)
					{
						crackleTime = 2000;

						OmniSource *source = new OmniSource("sound/Crackle", 64.0F);
						source->SetNodePosition(collisionData.position + collisionData.normal * 0.25F);
						GetWorld()->GetRootNode()->AppendSubnode(source);
						source->Preprocess();
					}
				}
			}
		}
		else if (state == kCollisionStateRigidBody)
		{
			p2 = GetInverseWorldTransform() * collisionData.position;

			RigidBodyController *rigidBody = collisionData.rigidBody;

			RigidBodyType type = rigidBody->GetRigidBodyType();
			if (type == kRigidBodyCharacter)
			{
				GameCharacterController *character = static_cast<GameCharacterController *>(rigidBody);
				character->Damage(TheTimeMgr->GetDeltaTime() * 16000, 0, attackerController, &collisionData.position);
				sparksFlag = false;
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
				sparksFlag = false;
			}
			else if (rigidBody->GetControllerType() == kControllerExplosive)
			{
				static_cast<ExplosiveController *>(rigidBody)->Damage(TheTimeMgr->GetDeltaTime() * 16000, 0.0F, attackerController);
			}
		}

		if (sparksFlag)
		{
			if (markingTime <= 0)
			{
				markingTime = 100;

				int32 i = Math::Random(4);
				MarkingData markingData(Vector2D(0.5F, 0.5F), Vector2D((i & 1) ? 0.5F : 0.0F, (i & 2) ? 0.5F : 0.0F), kMarkingForceClip);

				markingData.center = collisionData.position;
				markingData.normal = collisionData.normal;
				markingData.tangent = Math::CreatePerpendicular(collisionData.normal);
				markingData.tangent.RotateAboutAxis(Math::RandomFloat(K::tau), collisionData.normal);
				markingData.radius = 1.0F;
				markingData.textureName = "proton/gun/Scorch";
				markingData.color.Set(1.0F, 1.0F, 1.0F, 1.0F);
				markingData.lifeTime = 7000;
				MarkingEffect::New(GetWorld(), &markingData);
			}

			sparksSystem->NewSparks(collisionData.position, collisionData.normal, TheTimeMgr->GetDeltaTime());
		}

		lightPosition.Set(p2.x - 0.25F, p2.y, p2.z);

		shockSource.SetNodePosition(lightPosition);
		shockSource.Invalidate();

		if (!shockSource.Playing())
		{
			shockSource.Play();
		}
	}
	else
	{
		p2 = GetInverseWorldTransform() * p2;

		for (machine index = 0; index < 4; index++)
		{
			controlDistance[index][1] = kProtonBeamRange * 0.333333F;
			controlDistance[index][2] = kProtonBeamRange * 0.666667F;
			controlDistance[index][3] = kProtonBeamRange;
		}

		lightPosition = p2;

		if (shockSource.Playing())
		{
			shockSource.Stop();
		}
	}

	float fdt = TheTimeMgr->GetFloatDeltaTime();
	PolyParticle *particle = static_cast<PolyParticle *>(GetFirstParticle());

	for (machine index = 0; index < 4; index++)
	{
		Point3D		controlPoint[3];

		float angle = controlAngle[index][0];
		float radius = controlRadius[index][0];

		controlPoint[0].x = 0.0F;
		controlPoint[0].GetVectorYZ() = CosSin(angle) * radius;

		for (machine a = 1; a < 3; a++)
		{
			angle = controlAngle[index][a] + controlVelocity[index][a] * fdt;
			if (angle > K::tau_over_2)
			{
				angle -= K::tau;
			}

			controlAngle[index][a] = angle;

			radius = controlRadius[index][a];
			controlPoint[a].x = controlDistance[index][a];
			controlPoint[a].GetVectorYZ() = CosSin(angle) * radius;
		}

		for (machine a = 0; a < 64; a++)
		{
			float t = (float) a * 0.015873F;
			float u = 1.0F - t;
			float t2 = t * t;
			float u2 = u * u;

			particle->position = controlPoint[0] * (u2 * u) + controlPoint[1] * (3.0F * u2 * t) + controlPoint[2] * (3.0F * u * t2) + p2 * (t2 * t);
			particle->tangent = Normalize(controlPoint[0] * (-1.0F + 2.0F * t - t2) + controlPoint[1] * (1.0F - 4.0F * t + 3.0F * t2) + controlPoint[2] * (2.0F * t - 3.0F * t2) + p2 * t2);

			particle = particle->GetNextParticle();
		}
	}

	protonLight.SetNodePosition(lightPosition);
	protonLight.Invalidate();

	if (flickerInterpolator.GetMode() == kInterpolatorStop)
	{
		flickerIntensity = Math::RandomFloat(2.0F);
		float rate = Math::RandomFloat(0.05F) + 0.01F;
		flickerInterpolator.Set(0.0F, rate, kInterpolatorForward | kInterpolatorOscillate);
	}

	float f = flickerInterpolator.UpdateValue();
	float g = f * flickerIntensity + 2.0F;
	protonLight.GetObject()->SetLightColor(ColorRGB(g, g, g));
	protonWarp.GetObject()->SetQuadAlpha(f * 2.0F);
}


ProtonAmmoBeam::ProtonAmmoBeam() :
		PolyboardParticleSystem(kParticleSystemProtonAmmoBeam, &particlePool),
		particlePool(kMaxParticleCount, particleArray)
{
	beamLength = 0.6F;
}

ProtonAmmoBeam::ProtonAmmoBeam(const ProtonAmmoBeam& protonAmmoBeam) :
		PolyboardParticleSystem(protonAmmoBeam, &particlePool),
		particlePool(kMaxParticleCount, particleArray)
{
	beamLength = 0.6F;
}

ProtonAmmoBeam::~ProtonAmmoBeam()
{
}

Node *ProtonAmmoBeam::Replicate(void) const
{
	return (new ProtonAmmoBeam(*this));
}

bool ProtonAmmoBeam::CalculateBoundingSphere(BoundingSphere *sphere) const
{
	float f = beamLength * 0.5F;
	sphere->SetCenter(Point3D(f, 0.0F, 0.0F));
	sphere->SetRadius(f);
	return (true);
}

void ProtonAmmoBeam::Preprocess(void)
{
	static const ConstColorRGB beamColor[4] =
	{
		{1.0F, 0.0F, 0.0F},
		{1.0F, 1.0F, 0.0F},
		{1.0F, 0.375F, 0.0F},
		{0.125F, 1.0F, 0.0F}
	};

	PolyboardParticleSystem::Preprocess();

	SetTransformable(this);
	SetParticleSystemFlags(kParticleSystemObjectSpace);

	if (!GetManipulator())
	{
		float f = beamLength * K::one_over_3;

		for (machine index = 0; index < 4; index++)
		{
			controlRadius[index][0] = Math::RandomFloat(0.1F) + 0.1F;
			controlRadius[index][1] = Math::RandomFloat(0.1F) + 0.1F;

			controlDistance[index][0] = f;
			controlDistance[index][1] = f * 2.0F;

			controlVelocity[index][0] = Math::RandomFloat(0.01F) + 0.01F;
			controlVelocity[index][1] = Math::RandomFloat(0.01F) + 0.01F;

			controlAngle[index][0] = 0.0F;
			controlAngle[index][1] = 0.0F;

			for (machine a = 0; a < 16; a++)
			{
				PolyParticle *particle = particlePool.NewParticle();
				if (!particle)
				{
					break;
				}

				particle->emitTime = 0;
				particle->lifeTime = 1;
				particle->radius = 0.05F;
				particle->color.Set(beamColor[index], 1.0F);
				particle->texcoord = 0.53125F;
				particle->terminator = (a == 15);

				AddParticle(particle);
			}
		}
	}
}

void ProtonAmmoBeam::AnimateParticles(void)
{
	float fdt = TheTimeMgr->GetFloatDeltaTime();
	PolyParticle *particle = static_cast<PolyParticle *>(GetFirstParticle());

	for (machine index = 0; index < 4; index++)
	{
		Point3D		controlPoint[2];

		for (machine a = 0; a < 2; a++)
		{
			float angle = controlAngle[index][a] + controlVelocity[index][a] * fdt;
			if (angle > K::tau_over_2)
			{
				angle -= K::tau;
			}

			controlAngle[index][a] = angle;

			float radius = controlRadius[index][a];
			controlPoint[a].x = controlDistance[index][a];
			controlPoint[a].GetVectorYZ() = CosSin(angle) * radius;
		}

		for (machine a = 0; a < 16; a++)
		{
			float t = (float) a * 0.06666F;
			float u = 1.0F - t;
			float t2 = t * t;
			float u2 = u * u;

			particle->position = controlPoint[0] * (3.0F * u2 * t) + controlPoint[1] * (3.0F * u * t2);
			particle->position.x += beamLength * (t2 * t);

			particle->tangent = controlPoint[0] * (1.0F - 4.0F * t + 3.0F * t2) + controlPoint[1] * (2.0F * t - 3.0F * t2);
			particle->tangent.x += beamLength * t2;
			particle->tangent.Normalize();

			particle = particle->GetNextParticle();
		}
	}
}


ProtonCannonController::ProtonCannonController(FighterController *fighter) :
		WeaponController(kControllerProtonCannon, fighter),
		emissionAttribute(K::black, kAttributeMutable)
{
}

ProtonCannonController::~ProtonCannonController()
{
}

void ProtonCannonController::Preprocess(void)
{
	WeaponController::Preprocess();

	firingState = 0;
	protonBeam = nullptr;

	Model *model = GetTargetNode();
	barrelNode = model->FindNode(Text::StaticHash<'B', 'a', 'r', 'r', 'e', 'l'>::value);
	spinSource = static_cast<OmniSource *>(model->FindNode(Text::StaticHash<'S', 'p', 'i', 'n'>::value));
	spinSource->SetSourcePriority(kSoundPriorityWeapon);

	barrelTransform = barrelNode->GetNodeTransform();
	barrelAngle = 0.0F;
	barrelSpeed = 0.0F;

	attributeList.Append(&emissionAttribute);
	static_cast<Geometry *>(barrelNode)->SetMaterialAttributeList(&attributeList);
	static_cast<Geometry *>(model->FindNode(Text::StaticHash<'G', 'u', 'n'>::value))->SetMaterialAttributeList(&attributeList);
}

ControllerMessage *ProtonCannonController::CreateMessage(ControllerMessageType type) const
{
	switch (type)
	{
		case kProtonCannonMessageState:

			return (new ProtonCannonStateMessage(GetControllerIndex()));
	}

	return (WeaponController::CreateMessage(type));
}

void ProtonCannonController::SendInitialStateMessages(Player *player) const
{
	unsigned_int32 state = firingState;
	if (state != 0)
	{
		player->SendMessage(ProtonCannonStateMessage(GetControllerIndex(), state, barrelSpeed));
	}
}

void ProtonCannonController::Move(void)
{
	Matrix3D	m;

	FighterController *fighter = GetFighterController();
	const Point3D& firePosition = fighter->GetWeaponFireMarker()->GetNodePosition();

	float dt = TheTimeMgr->GetFloatDeltaTime();
	float speed = barrelSpeed;

	unsigned_int32 state = firingState;
	if (state & kProtonCannonSpin)
	{
		if (speed < kMaxProtonBarrelSpeed)
		{
			speed = Fmin(speed + 3.0e-5F * dt, kMaxProtonBarrelSpeed);
		}
		else
		{
			bool server = TheMessageMgr->Server();
			GamePlayer *player = fighter->GetFighterPlayer();

			for (;;)
			{
				if (state & kProtonCannonBeam)
				{
					if (!protonBeam)
					{
						ProtonBeam *beam = new ProtonBeam(fighter);
						protonBeam = beam;

						beam->SetNodePosition(firePosition);
						GetTargetNode()->AppendNewSubnode(beam);
					}

					if (server)
					{
						int32 time = firingTime + TheTimeMgr->GetDeltaTime();
						int32 change = time >> 6;
						if (change != 0)
						{
							time -= change << 6;
							int32 ammo = MaxZero(player->GetWeaponAmmo(kWeaponProtonCannon, 0) - change);
							player->SetWeaponAmmo(kWeaponProtonCannon, 0, ammo);

							if (ammo == 0)
							{
								TheMessageMgr->SendMessageAll(ProtonCannonStateMessage(GetControllerIndex(), state & ~kProtonCannonBeam, barrelSpeed));
							}
						}

						firingTime = time;
					}
				}
				else
				{
					if ((server) && (player->GetWeaponAmmo(kWeaponProtonCannon, 0) > 0))
					{
						TheMessageMgr->SendMessageAll(ProtonCannonStateMessage(GetControllerIndex(), state |= kProtonCannonBeam, barrelSpeed));
						continue;
					}
				}

				break;
			}
		}
	}
	else
	{
		if (speed == 0.0F)
		{
			if (spinSource->Playing())
			{
				spinSource->Stop();
			}

			firingState = 0;
			return;
		}

		speed = FmaxZero(speed - 3.0e-5F * dt);
	}

	barrelSpeed = speed;
	spinSource->SetSourceFrequency(Fmin(speed * 41.67F + 0.375F, 1.0F));

	float emit = FmaxZero(speed * kProtonEmissionFactor - 3.0F);
	emissionAttribute.SetEmissionColor(ColorRGBA(emit, emit, emit, 1.0F));

	const Vector3D& velocity = fighter->GetLinearVelocity();
	spinSource->SetSourceVelocity(velocity);

	float angle = barrelAngle + speed * dt;
	if (angle > K::tau)
	{
		angle -= K::tau;
	}

	barrelAngle = angle;

	if (protonBeam)
	{
		protonBeam->SetBarrelAngle(angle);
		protonBeam->SetSourceVelocity(velocity);
	}

	m.SetRotationAboutX(angle);
	barrelNode->SetNodeTransform(Transform4D(m, firePosition - m * firePosition) * barrelTransform);
	barrelNode->Invalidate();
}

void ProtonCannonController::BeginFiring(bool primary)
{
	FighterController *fighter = GetFighterController();
	fighter->SetFighterFlags(fighter->GetFighterFlags() | kFighterTargetDistance);

	firingTime = 0;

	const GamePlayer *player = fighter->GetFighterPlayer();
	unsigned_int32 state = (player->GetWeaponAmmo(kWeaponProtonCannon, 0) > 0) ? kProtonCannonSpin | kProtonCannonBeam : kProtonCannonSpin;
	TheMessageMgr->SendMessageAll(ProtonCannonStateMessage(GetControllerIndex(), state, barrelSpeed));
}

void ProtonCannonController::EndFiring(void)
{
	FighterController *fighter = GetFighterController();
	fighter->SetFighterFlags(fighter->GetFighterFlags() & ~kFighterTargetDistance);

	TheMessageMgr->SendMessageAll(ProtonCannonStateMessage(GetControllerIndex(), 0, barrelSpeed));
}

void ProtonCannonController::SetFiringState(unsigned_int32 state, float speed)
{
	firingState = state;

	if (state & kProtonCannonSpin)
	{
		if (!spinSource->Playing())
		{
			spinSource->Play();
		}

		barrelSpeed = speed;
	}

	if (!(state & kProtonCannonBeam))
	{
		delete protonBeam;
		protonBeam = nullptr;
	}
}

WeaponResult ProtonCannonController::UpdateWeapon(const Point3D& position, const Vector3D& direction, const Point3D& center)
{
	if (protonBeam)
	{
		protonBeam->SetBeamPosition(direction);
	}

	if (TheMessageMgr->Server())
	{
		const FighterController *fighter = GetFighterController();
		if (fighter->GetFighterFlags() & kFighterFiring)
		{
			const GamePlayer *player = fighter->GetFighterPlayer();
			return ((player->GetWeaponAmmo(kWeaponProtonCannon, 0) > 0) ? kWeaponFired : kWeaponEmpty);
		}
	}

	return (kWeaponIdle);
}


ProtonCannonStateMessage::ProtonCannonStateMessage(int32 index) : ControllerMessage(ProtonCannonController::kProtonCannonMessageState, index)
{
}

ProtonCannonStateMessage::ProtonCannonStateMessage(int32 index, unsigned_int32 state, float speed) : ControllerMessage(ProtonCannonController::kProtonCannonMessageState, index)
{
	firingState = state;
	barrelSpeed = speed;
}

ProtonCannonStateMessage::~ProtonCannonStateMessage()
{
}

void ProtonCannonStateMessage::Compress(Compressor& data) const
{
	ControllerMessage::Compress(data);

	data << (unsigned_int8) firingState;
	data << barrelSpeed;
}

bool ProtonCannonStateMessage::Decompress(Decompressor& data)
{
	if (ControllerMessage::Decompress(data))
	{
		unsigned_int8	state;

		data >> state;
		firingState = state;

		data >> barrelSpeed;
		return (true);
	}

	return (false);
}

void ProtonCannonStateMessage::HandleControllerMessage(Controller *controller) const
{
	static_cast<ProtonCannonController *>(controller)->SetFiringState(firingState, barrelSpeed);
}

// ZYUQURM
