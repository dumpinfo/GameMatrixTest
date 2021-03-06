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


#ifndef MGMonster_h
#define MGMonster_h


#include "C4Physics.h"
#include "C4Models.h"
#include "C4Sources.h"
#include "C4Scripts.h"
#include "MGCharacter.h"



namespace C4
{
	//# \enum	MonsterFlags

	enum
	{
		kMonsterInitialized		= 1 << 0,		//## The monster has been initialized in the game.
		kMonsterGenerated		= 1 << 1,		//## The monster was generated by a script.
		kMonsterCounted			= 1 << 2		//## The monster has been included in the global count.
	};


	class MonsterController : public GameCharacterController
	{
		private:

			unsigned_int32					monsterFlags;
			Fixed							monsterHealth;

			int32							monsterState;
			int32							monsterMotion;

			int32							stateTime;
			int32							damageTime;

			float							monsterRepelForce;
			Point3D							sourcePosition;

			float							monsterAzimuth;
			float							monsterRoll;

			Vector2D						repulsionForce;
			int32							repulsionTime;

			Link<GameCharacterController>	enemyController;

			AnimationBlender				animationBlender;

			static void EnemyLinkProc(Node *node, void *cookie);

		protected:

			MonsterController(ControllerType type);
			MonsterController(const MonsterController& monsterController);

			int32 GetMonsterState(void) const
			{
				return (monsterState);
			}

			int32 GetMonsterMotion(void) const
			{
				return (monsterMotion);
			}

			void SetMonsterMotion(int32 motion)
			{
				monsterMotion = motion;
			}

			int32 GetStateTime(void) const
			{
				return (stateTime);
			}

			bool DamageTimeExpired(int32 reset)
			{
				if (damageTime > 0)
				{
					return (false);
				}

				damageTime = reset;
				return (true);
			}

			void SetMonsterRepelForce(float force)
			{
				monsterRepelForce = force;
			}

			void SetMonsterRoll(float roll)
			{
				monsterRoll = roll;
			}

			void SetSourcePosition(const Point3D& position)
			{
				sourcePosition = position; 
			}

			void SetMonsterState(int32 state) 
			{
				monsterState = state; 
				stateTime = 0;
			}
 
			const Vector2D& GetRepulsionForce(void) const
			{ 
				return (repulsionForce); 
			}

			AnimationBlender *GetAnimationBlender(void)
			{ 
				return (&animationBlender);
			}

			void CountMonster(int32& count);
			void UncountMonster(int32& count);

			float SetMonsterAzimuth(float azimuth, float maxRotationRate);
			float SetMonsterAzimuth(float azimuth, float maxRotationRate, float *deriv);

			bool GetEnemyDirection(Vector3D *direction);
			void DamageEnemy(Fixed damage, float range);

			OmniSource *PlaySource(const char *name, float range);

			void AttachRagdoll(ModelType type, const Point3D *position, const Vector3D *force);

		public:

			~MonsterController();

			unsigned_int32 GetMonsterFlags(void) const
			{
				return (monsterFlags);
			}

			void SetMonsterFlags(unsigned_int32 flags)
			{
				monsterFlags = flags;
			}

			Fixed GetMonsterHealth(void) const
			{
				return (monsterHealth);
			}

			void SetMonsterHealth(Fixed health)
			{
				monsterHealth = health;
			}

			float GetMonsterAzimuth(void) const
			{
				return (monsterAzimuth);
			}

			float GetMonsterRoll(void) const
			{
				return (monsterRoll);
			}

			void Pack(Packer& data, unsigned_int32 packFlags) const override;
			void Unpack(Unpacker& data, unsigned_int32 unpackFlags) override;
			bool UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags);

			void Preprocess(void) override;
			void Wake(void) override;
			void Move(void) override;

			RigidBodyStatus HandleNewRigidBodyContact(const RigidBodyContact *contact, RigidBodyController *contactBody) override;

			CharacterStatus Damage(Fixed damage, unsigned_int32 flags, GameCharacterController *attacker, const Point3D *position = nullptr, const Vector3D *impulse = nullptr) override;
			void Kill(GameCharacterController *attacker, const Point3D *position = nullptr, const Vector3D *impulse = nullptr) override;
	};


	class GenerateMonsterMethod : public Method
	{
		protected:

			GenerateMonsterMethod(MethodType type);
			GenerateMonsterMethod(const GenerateMonsterMethod& generateMonsterMethod);

			void GenerateMonster(Node *targetNode, Model *model, MonsterController *controller);

		public:

			~GenerateMonsterMethod();
	};
}


#endif

// ZYUQURM
