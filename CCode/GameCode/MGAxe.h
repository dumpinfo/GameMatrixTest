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


#ifndef MGAxe_h
#define MGAxe_h


#include "MGWeapons.h"
#include "MGMultiplayer.h"


namespace C4
{
	enum : ControllerType
	{
		kControllerAxe		= 'axe '
	};


	enum : ModelType
	{
		kModelAxe			= 'axe '
	};


	enum
	{
		kAxeEffectNone,
		kAxeEffectSparks
	};


	class Axe final : public Weapon
	{
		private:

			ModelRegistration		axeModelRegistration;

			Axe();
			~Axe();

		public:

			static void Construct(void);
			static void Destruct(void);

			WeaponController *NewWeaponController(FighterController *fighter) const;
	};


	class AxeController final : public WeaponController
	{
		private:

			FrameAnimator							frameAnimator;
			FrameAnimatorObserver<AxeController>	frameAnimatorObserver;

			void HandleAnimationEvent(FrameAnimator *animator, CueType cueType);

		public:

			enum
			{
				kAxeMessageSwing = kWeaponMessageBaseCount,
				kAxeMessageImpact
			};

			AxeController(FighterController *fighter);
			~AxeController();

			void Preprocess(void) override;

			ControllerMessage *CreateMessage(ControllerMessageType type) const override;

			void Move(void) override;

			void ShowSwingAnimation(void);
			WeaponResult UpdateWeapon(const Point3D& position, const Vector3D& direction, const Point3D& center) override;
	};


	class AxeSwingMessage : public ControllerMessage
	{
		public:

			AxeSwingMessage(int32 index);
			~AxeSwingMessage();

			void HandleControllerMessage(Controller *controller) const;
	};


	class AxeImpactMessage : public ControllerMessage
	{
		friend class AxeController;

		private:

			Point3D			impactPosition;
			Vector3D		impactNormal;
			SubstanceType	impactSubstance;

			AxeImpactMessage(int32 index);
 
		public:

			AxeImpactMessage(int32 index, const Point3D& position, const Vector3D& normal, SubstanceType substance); 
			~AxeImpactMessage();
 
			void Compress(Compressor& data) const override;
			bool Decompress(Decompressor& data) override;
 
			void HandleControllerMessage(Controller *controller) const;
	}; 
} 


#endif

// ZYUQURM
