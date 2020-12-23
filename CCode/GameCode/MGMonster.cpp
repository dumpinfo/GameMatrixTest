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


#include "C4Simulation.h"
#include "MGMonster.h"
#include "MGFighter.h"


using namespace C4;


MonsterController::MonsterController(ControllerType type) : GameCharacterController(kCharacterMonster, type)
{
	monsterFlags = 0;
	monsterHealth = 0;

	monsterRepelForce = 70.0F;
	monsterAzimuth = 0.0F;
	monsterRoll = 0.0F;

	damageTime = 0;

	repulsionForce.Set(0.0F, 0.0F);
	repulsionTime = 0;

	sourcePosition.Set(0.0F, 0.0F, 0.0F);
}

MonsterController::MonsterController(const MonsterController& monsterController) : GameCharacterController(monsterController)
{
	monsterFlags = 0;
	monsterHealth = monsterController.monsterHealth;

	monsterRepelForce = monsterController.monsterRepelForce;
	monsterAzimuth = 0.0F;
	monsterRoll = 0.0F;

	damageTime = 0;

	repulsionForce.Set(0.0F, 0.0F);
	repulsionTime = 0;

	sourcePosition = monsterController.sourcePosition;
}

MonsterController::~MonsterController()
{
}

void MonsterController::Pack(Packer& data, unsigned_int32 packFlags) const
{
	GameCharacterController::Pack(data, packFlags);

	data << ChunkHeader('FLAG', 4);
	data << unsigned_int32(monsterFlags & ~kMonsterCounted);

	data << ChunkHeader('HLTH', 4);
	data << monsterHealth;

	data << ChunkHeader('RFRC', 4);
	data << monsterRepelForce;

	if (!(packFlags & kPackEditor))
	{
		data << ChunkHeader('STAT', 4);
		data << monsterState;

		data << ChunkHeader('MOTN', 4);
		data << monsterMotion;

		data << ChunkHeader('STTM', 4);
		data << stateTime;

		data << ChunkHeader('DGTM', 4);
		data << damageTime;

		data << ChunkHeader('AZTH', 4);
		data << monsterAzimuth;

		data << ChunkHeader('ROLL', 4);
		data << monsterRoll;

		data << ChunkHeader('RPEL', sizeof(Vector2D) + 4);
		data << repulsionForce;
		data << repulsionTime;

		const GameCharacterController *enemy = enemyController;
		if ((enemy) && (enemy->GetCharacterType() != kCharacterPlayer))
		{
			data << ChunkHeader('ENMY', 4);
			data << enemy->GetTargetNode()->GetNodeIndex();
		}

		PackHandle handle = data.BeginChunk('ANIM');
		animationBlender.Pack(data, packFlags);
		data.EndChunk(handle);
	}

	data << TerminatorChunk;
}

void MonsterController::Unpack(Unpacker& data, unsigned_int32 unpackFlags)
{
	GameCharacterController::Unpack(data, unpackFlags); 
	UnpackChunkList<MonsterController>(data, unpackFlags);
}
 
bool MonsterController::UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags)
{ 
	switch (chunkHeader->chunkType)
	{
		case 'FLAG': 

			data >> monsterFlags; 
			return (true); 

		case 'HLTH':

			data >> monsterHealth; 
			return (true);

		case 'RFRC':

			data >> monsterRepelForce;
			return (true);

		case 'STAT':

			data >> monsterState;
			return (true);

		case 'MOTN':

			data >> monsterMotion;
			return (true);

		case 'STTM':

			data >> stateTime;
			return (true);

		case 'DGTM':

			data >> damageTime;
			return (true);

		case 'AZTH':

			data >> monsterAzimuth;
			return (true);

		case 'ROLL':

			data >> monsterRoll;
			return (true);

		case 'RPEL':

			data >> repulsionForce;
			data >> repulsionTime;
			return (true);

		case 'ENMY':
		{
			int32	index;

			data >> index;
			data.AddNodeLink(index, &EnemyLinkProc, this);
			return (true);
		}

		case 'ANIM':

			animationBlender.Unpack(data, unpackFlags);
			return (true);
	}

	return (false);
}

void MonsterController::EnemyLinkProc(Node *node, void *cookie)
{
	MonsterController *monsterController = static_cast<MonsterController *>(cookie);
	monsterController->enemyController = static_cast<GameCharacterController *>(node->GetController());
}

void MonsterController::Preprocess(void)
{
	GameCharacterController::Preprocess();

	Model *model = GetTargetNode();
	if (!model->GetManipulator())
	{
		if (!model->Enabled())
		{
			SetCollisionKind(GetCollisionKind() | (kCollisionCorpse | kCollisionRemains));
		}

		SetFrictionCoefficient(0.0F);

		const Vector3D direction = model->GetWorldTransform()[0];
		monsterAzimuth = Atan(direction.y, direction.x);

		GameCharacterController *enemy = enemyController;
		if (!enemy)
		{
			const Player *player = TheMessageMgr->GetLocalPlayer();
			if (player)
			{
				enemy = static_cast<const GamePlayer *>(player)->GetPlayerController();
				if (enemy)
				{
					enemyController = enemy;
				}
			}
		}
	}

	animationBlender.Preprocess(model);
}

void MonsterController::Wake(void)
{
	GameCharacterController::Wake();

	Model *model = GetTargetNode();
	if (!model->Enabled())
	{
		model->Enable();
		SetCollisionKind(GetCollisionKind() & ~(kCollisionCorpse | kCollisionRemains));
	}
}

void MonsterController::Move(void)
{
	GameCharacterController::Move();

	SetCharacterOrientation(monsterAzimuth, monsterRoll);

	int32 dt = TheTimeMgr->GetDeltaTime();
	stateTime += dt;
	damageTime = MaxZero(damageTime - dt);

	int32 time = repulsionTime;
	if (time > 0)
	{
		time -= dt;
		repulsionTime = time;

		if (time <= 0)
		{
			repulsionForce.Set(0.0F, 0.0F);
		}
	}

	GetTargetNode()->Animate();
}

RigidBodyStatus MonsterController::HandleNewRigidBodyContact(const RigidBodyContact *contact, RigidBodyController *contactBody)
{
	if ((contactBody->GetRigidBodyType() == kRigidBodyCharacter) && (static_cast<GameCharacterController *>(contactBody)->GetCharacterType() == kCharacterMonster))
	{
		float m = GetBodyMass() * 5.0F;
		Antivector3D normal = GetFinalTransform() * contact->GetContactNormal();
		if (contact->GetStartElement() == this)
		{
			m = -m;
		}

		repulsionForce.Set(normal.x * m, normal.y * m);
		repulsionTime = 250;

		return (kRigidBodyUnchanged);
	}

	return (GameCharacterController::HandleNewRigidBodyContact(contact, contactBody));
}

CharacterStatus MonsterController::Damage(Fixed damage, unsigned_int32 flags, GameCharacterController *attacker, const Point3D *position, const Vector3D *force)
{
	if (Asleep())
	{
		Wake();
	}

	Fixed health = monsterHealth;
	damage = Min(damage, health);

	if (attacker)
	{
		if (attacker->GetCharacterType() == kCharacterPlayer)
		{
			static_cast<FighterController *>(attacker)->GetFighterPlayer()->AddPlayerScore(damage * 100);
		}

		GameCharacterController *enemy = enemyController;
		if ((attacker != enemy) && (attacker->GetCharacterState() & kCharacterAttackable) && (attacker->GetControllerType() != GetControllerType()))
		{
			enemyController = attacker;
		}
	}

	monsterHealth = (health -= damage);
	if (health == 0)
	{
		Kill(attacker, position, force);
		return (kCharacterKilled);
	}

	return (kCharacterDamaged);
}

void MonsterController::Kill(GameCharacterController *attacker, const Point3D *position, const Vector3D *force)
{
	GameCharacterController::Kill(attacker);

	if ((attacker) && (attacker->GetCharacterType() == kCharacterPlayer))
	{
		static_cast<FighterController *>(attacker)->GetFighterPlayer()->AddPlayerKill();
	}

	// Allow the rigid body controller to go to sleep, but only for the physics simulation.
	// The Move() function will still be called because the kRigidBodyPartialSleep flag is specified.
	// Also set the kRigidBodyForceFieldInhibit flag so that explosions don't wake up the corpse.

	SetRigidBodyFlags((GetRigidBodyFlags() & ~kRigidBodyKeepAwake) | (kRigidBodyPartialSleep | kRigidBodyForceFieldInhibit));

	// Increase the sleep box size so that corpses tend to sleep faster.

	SetSleepBoxMultiplier(2.0F);

	// Add the kCollisionCorpse flag to the collision kind for this monster.

	SetCollisionKind(GetCollisionKind() | kCollisionCorpse);

	// No longer allow collisions with characters or projectiles, and don't let line-of-sight tests hit this monster.

	SetCollisionExclusionMask(GetCollisionExclusionMask() | (kCollisionCharacter | kCollisionProjectile | kCollisionSightPath));
}

void MonsterController::CountMonster(int32& count)
{
	unsigned_int32 flags = monsterFlags;
	if ((flags & (kMonsterGenerated | kMonsterCounted)) == kMonsterGenerated)
	{
		monsterFlags = flags | kMonsterCounted;
		count++;
	}
}

void MonsterController::UncountMonster(int32& count)
{
	unsigned_int32 flags = monsterFlags;
	if (flags & kMonsterCounted)
	{
		monsterFlags = flags & ~kMonsterCounted;
		count--;
	}
}

float MonsterController::SetMonsterAzimuth(float azimuth, float maxRotationRate)
{
	float delta = azimuth - monsterAzimuth;
	if (delta > K::tau_over_2)
	{
		delta -= K::tau;
	}
	else if (delta < -K::tau_over_2)
	{
		delta += K::tau;
	}

	float f = TheTimeMgr->GetFloatDeltaTime() * maxRotationRate;
	delta = Clamp(delta, -f, f);

	azimuth = monsterAzimuth + delta;
	if (azimuth > K::tau_over_2)
	{
		azimuth -= K::tau;
	}
	else if (azimuth < -K::tau_over_2)
	{
		azimuth += K::tau;
	}

	monsterAzimuth = azimuth;
	return (azimuth);
}

float MonsterController::SetMonsterAzimuth(float azimuth, float maxRotationRate, float *deriv)
{
	float delta = azimuth - monsterAzimuth;
	if (delta > K::tau_over_2)
	{
		delta -= K::tau;
	}
	else if (delta < -K::tau_over_2)
	{
		delta += K::tau;
	}

	float dt = TheTimeMgr->GetFloatDeltaTime();
	float f = dt * maxRotationRate;
	delta = Clamp(delta, -f, f);
	*deriv = delta / Fmax(dt, 1.0F);

	azimuth = monsterAzimuth + delta;
	if (azimuth > K::tau_over_2)
	{
		azimuth -= K::tau;
	}
	else if (azimuth < -K::tau_over_2)
	{
		azimuth += K::tau;
	}

	monsterAzimuth = azimuth;
	return (azimuth);
}

bool MonsterController::GetEnemyDirection(Vector3D *direction)
{
	GameCharacterController *enemy = enemyController;
	if ((enemy) && (!(enemy->GetCharacterState() & kCharacterDead)))
	{
		*direction = enemy->GetTargetNode()->GetWorldPosition() - GetTargetNode()->GetWorldPosition();
		return (true);
	}

	const Player *player = TheMessageMgr->GetLocalPlayer();
	if (player)
	{
		enemy = static_cast<const GamePlayer *>(player)->GetPlayerController();
		enemyController = enemy;

		if ((enemy) && (!(enemy->GetCharacterState() & kCharacterDead)))
		{
			*direction = enemy->GetTargetNode()->GetWorldPosition() - GetTargetNode()->GetWorldPosition();
			return (true);
		}
	}

	return (false);
}

void MonsterController::DamageEnemy(Fixed damage, float range)
{
	GameCharacterController *enemy = enemyController;
	if (enemy)
	{
		range += 0.25F;

		const Model *model = GetTargetNode();
		Vector2D direction = enemy->GetTargetNode()->GetWorldPosition().GetVector2D() - model->GetWorldPosition().GetVector2D();
		if ((SquaredMag(direction) < range * range) && (direction * model->GetWorldTransform()[0].GetVector2D() > 0.0F))
		{
			enemy->Damage(damage, 0, this);
		}
	}
}

OmniSource *MonsterController::PlaySource(const char *name, float range)
{
	OmniSource *source = new OmniSource(name, range);
	source->SetNodeFlags(kNodeCloneInhibit | kNodeAnimateInhibit);
	source->SetNodePosition(sourcePosition);
	source->SetSourceVelocity(GetLinearVelocity());
	GetTargetNode()->AppendNewSubnode(source);
	return (source);
}

void MonsterController::AttachRagdoll(ModelType type, const Point3D *position, const Vector3D *force)
{
	RagdollController *ragdoll = GetTargetNode()->AttachRagdoll(type);
	if ((ragdoll) && (force))
	{
		Transform4D transform = Inverse(ragdoll->GetFinalTransform());
		Vector3D impulse = transform * *force * kTimeStep;

		if (position)
		{
			ragdoll->ApplyImpulse(impulse, transform * *position);
		}
		else
		{
			ragdoll->ApplyImpulse(impulse);
		}

		ragdoll->ApplyImpulse(transform[2] * (Magnitude(*force) * kTimeStep * 0.5F));
	}
}


GenerateMonsterMethod::GenerateMonsterMethod(MethodType type) : Method(type)
{
}

GenerateMonsterMethod::GenerateMonsterMethod(const GenerateMonsterMethod& generateMonsterMethod) : Method(generateMonsterMethod)
{
}

GenerateMonsterMethod::~GenerateMonsterMethod()
{
}

void GenerateMonsterMethod::GenerateMonster(Node *targetNode, Model *model, MonsterController *controller)
{
	controller->SetMonsterFlags(kMonsterGenerated);

	model->SetController(controller);
	model->SetNodeTransform(targetNode->GetWorldTransform());

	targetNode->GetWorld()->GetRootNode()->AppendSubnode(model);
	model->Preprocess();

	controller->Wake();
}

// ZYUQURM
