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


#ifndef MGInput_h
#define MGInput_h


#include "C4Input.h"
#include "MGBase.h"


namespace C4
{
	enum : ActionType
	{
		kActionForward				= 'frwd',
		kActionBackward				= 'bkwd',
		kActionLeft					= 'left',
		kActionRight				= 'rght',
		kActionUp					= 'jump',
		kActionDown					= 'down',
		kActionMovement				= 'move',
		kActionHorizontal			= 'horz',
		kActionVertical				= 'vert',
		kActionFirePrimary			= 'fire',
		kActionFireSecondary		= 'trig',
		kActionUse					= 'uobj',
		kActionPistol				= 'pist',
		kActionShotgun				= 'shgn',
		kActionCrossbow				= 'cbow',
		kActionSpikeShooter			= 'spsh',
		kActionGrenadeLauncher		= 'gren',
		kActionQuantumCharger		= 'qchg',
		kActionRocketLauncher		= 'rock',
		kActionPlasmaGun			= 'plas',
		kActionProtonCannon			= 'pcan',
		kActionSpecialWeapon		= 'spec',
		kActionNextWeapon			= 'next',
		kActionPrevWeapon			= 'prev',
		kActionFlashlight			= 'lite',
		kActionCameraView			= 'camr',
		kActionScoreboard			= 'scor',
		kActionChat					= 'mess',
		kActionLoad					= 'load',
		kActionSave					= 'save'
	};


	enum
	{
		kMovementForward			= 1 << 0,
		kMovementBackward			= 1 << 1,
		kMovementLeft				= 1 << 2,
		kMovementRight				= 1 << 3,
		kMovementUp					= 1 << 4,
		kMovementDown				= 1 << 5,
		kMovementPlanarMask			= 15
	};


	class MovementAction : public Action
	{
		private:

			unsigned_int32		movementFlag;
			unsigned_int32		spectatorFlag;

		public:

			MovementAction(ActionType type, unsigned_int32 moveFlag, unsigned_int32 specFlag);
			~MovementAction();

			void Begin(void);
			void End(void);
			void Move(int32 value);
	};


	class LookAction : public Action
	{
		public:

			LookAction(ActionType type);
			~LookAction();

			void Update(float value);
	};


	class FireAction : public Action
	{
		public:

			FireAction(ActionType type);
			~FireAction();

			void Begin(void);
			void End(void);
	};


	class UseAction : public Action
	{
		public: 

			UseAction();
			~UseAction(); 

			void Begin(void); 
			void End(void);
	};
 

	class WeaponAction : public Action 
	{ 
		private:

			int32	weaponIndex;
 
		public:

			WeaponAction(int32 weapon);
			~WeaponAction();

			void Begin(void);
	};


	class SwitchAction : public Action
	{
		public:

			SwitchAction(ActionType type);
			~SwitchAction();

			void Begin(void);
			void Update(float value);
	};


	class ChatAction : public Action
	{
		public:

			ChatAction();
			~ChatAction();

			void Begin(void);
	};
}


#endif

// ZYUQURM
