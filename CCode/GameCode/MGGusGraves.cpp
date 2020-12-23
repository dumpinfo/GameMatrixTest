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


#include "MGGusGraves.h"
#include "MGGame.h"


using namespace C4;


namespace
{
	Storage<GusGraves> gusGravesStorage;
}


GusGraves::GusGraves() :
		gusGravesControllerRegistration(kControllerGusGraves, nullptr),
		gusGravesModelRegistration(kModelGusGraves, nullptr, "gus/Gus", kModelPrecache | kModelPrivate)
{
	flashlightTexture = Texture::Get("game/FlashLight");
}

GusGraves::~GusGraves()
{
	flashlightTexture->Release();
}

void GusGraves::Construct(void)
{
	new(gusGravesStorage) GusGraves;
}

void GusGraves::Destruct(void)
{
	gusGravesStorage->~GusGraves();
}


GusGravesController::GusGravesController() : FighterController(kControllerGusGraves)
{
}

GusGravesController::~GusGravesController()
{
}

void GusGravesController::Preprocess(void)
{
	FighterController::Preprocess();

	Model *model = GetTargetNode();
	Animator *animator = GetMergeAnimator();

	spineTwistAnimator = new SpineTwistAnimator(model, model->FindNode(Text::StaticHash<'B', 'i', 'p', '0', '0', '1', '_', 'S', 'p', 'i', 'n', 'e'>::value));
	animator->AppendNewSubnode(spineTwistAnimator);

	FrameAnimator *frameAnimator = GetFrameAnimator(0);
	frameAnimator->SetAnimation("gus/Stand");

	frameAnimator = GetFrameAnimator(1);
	frameAnimator->SetAnimation("gus/Stand");
}

void GusGravesController::SendInitialStateMessages(Player *player) const
{
	if (!(GetFighterFlags() & kFighterDead))
	{
		const GamePlayer *fighterPlayer = GetFighterPlayer();
		int32 weaponIndex = GetWeaponController()->GetControllerIndex();
		player->SendMessage(CreateGusGravesMessage(GetControllerIndex(), GetTargetNode()->GetWorldPosition(), GetLookAzimuth(), GetLookAltitude(), GetMovementFlags(), fighterPlayer->GetCurrentWeapon(), weaponIndex, fighterPlayer->GetPlayerKey()));
		FighterController::SendInitialStateMessages(player);
	}
}

void GusGravesController::SetFighterMotion(int32 motion)
{
	FighterController::SetFighterMotion(motion);

	FrameAnimator *animator1 = GetFrameAnimator(0);
	FrameAnimator *animator2 = GetFrameAnimator(1);
	Interpolator *interpolator1 = animator1->GetFrameInterpolator();
	Interpolator *interpolator2 = animator2->GetFrameInterpolator();
	Interpolator *weight1 = animator1->GetWeightInterpolator();
	Interpolator *weight2 = animator2->GetWeightInterpolator();

	switch (motion)
	{
		case kFighterMotionStop:
		{
			interpolator1->SetMode(kInterpolatorForward | kInterpolatorLoop);

			float w = weight2->GetValue();
			weight2->Set(w, 0.004F, kInterpolatorBackward);
			weight1->Set(1.0F - w, 0.004F, kInterpolatorForward);
			break;
		}

		case kFighterMotionStand:

			interpolator1->SetValue(0.0F);
			interpolator1->SetMode(kInterpolatorForward | kInterpolatorLoop);
			weight1->Set(1.0F, 0.0F, kInterpolatorStop);
			weight2->Set(0.0F, 0.0F, kInterpolatorStop); 
			break;

		case kFighterMotionForward: 
		{
			animator2->SetAnimation("gus/Run"); 
			interpolator2->SetMode(kInterpolatorForward | kInterpolatorLoop);

			float w = weight2->GetValue(); 
			weight2->Set(w, 0.004F, kInterpolatorForward);
			weight1->Set(1.0F - w, 0.004F, kInterpolatorBackward); 
			break; 
		}

		case kFighterMotionBackward:
		{ 
			animator2->SetAnimation("gus/Backward");
			interpolator2->SetMode(kInterpolatorForward | kInterpolatorLoop);
			interpolator2->SetValue(1.0F);

			float w = weight2->GetValue();
			weight2->Set(w, 0.004F, kInterpolatorForward);
			weight1->Set(1.0F - w, 0.004F, kInterpolatorBackward);
			break;
		}

		case kFighterMotionTurnLeft:

			animator2->SetAnimation("gus/TurnLeft");
			interpolator2->SetMode(kInterpolatorForward);

			interpolator1->SetMode(kInterpolatorStop);
			weight1->Set(0.0F, 0.0F, kInterpolatorStop);
			weight2->Set(1.0F, 0.0F, kInterpolatorStop);
			break;

		case kFighterMotionTurnRight:

			animator2->SetAnimation("gus/TurnRight");
			interpolator2->SetMode(kInterpolatorForward);

			interpolator1->SetMode(kInterpolatorStop);
			weight1->Set(0.0F, 0.0F, kInterpolatorStop);
			weight2->Set(1.0F, 0.0F, kInterpolatorStop);
			break;

		case kFighterMotionJump:

			animator2->SetAnimation("gus/Jump");
			interpolator2->SetMode(kInterpolatorForward);
			break;

		case kFighterMotionDeath:

			animator2->SetAnimation("gus/Death");
			interpolator2->SetMode(kInterpolatorForward);

			interpolator1->SetMode(kInterpolatorStop);
			weight1->Set(0.0F, 0.0F, kInterpolatorStop);
			weight2->Set(1.0F, 0.0F, kInterpolatorStop);
			break;
	}
}

void GusGravesController::Animate(void)
{
	if (!(GetFighterFlags() & kFighterDead))
	{
		Quaternion	rotation;

		float azm = GetInterpolatedLookAzimuth();
		float alt = GetInterpolatedLookAltitude();
		float modelAzimuth = GetModelAzimuth();

		Model *fighter = GetTargetNode();

		int32 motion = GetFighterMotion();
		if ((motion <= kFighterMotionStand) || (motion == kFighterMotionForward) || (motion == kFighterMotionBackward))
		{
			rotation.SetRotationAboutY(-alt * K::one_over_3);
		}
		else
		{
			const Matrix3D& p = GetFrameAnimator(1)->GetTransformTrackHeader()->GetTransformFrameData()->transform;
			Transform4D m = fighter->GetFirstSubnode()->GetNodeTransform() * Inverse(p);
			float x = m(0,0);
			float y = m(1,0);
			modelAzimuth += Atan(y, x);

			float d = InverseSqrt(x * x + y * y);
			rotation.SetRotationAboutAxis(alt * K::one_over_3, Vector3D(y * d, -x * d, 0.0F));
		}

		float f = azm - modelAzimuth;
		if (f < -K::tau_over_2)
		{
			f += K::tau;
		}
		else if (f > K::tau_over_2)
		{
			f -= K::tau;
		}

		rotation = Quaternion().SetRotationAboutZ(f * K::one_over_3) * rotation;
		spineTwistAnimator->SetSpineRotation(rotation);
	}

	FighterController::Animate();
}


CreateGusGravesMessage::CreateGusGravesMessage() : CreateFighterMessage(kModelMessageGusGraves)
{
}

CreateGusGravesMessage::CreateGusGravesMessage(int32 fighterIndex, const Point3D& position, float azm, float alt, unsigned_int32 movement, int32 weapon, int32 weaponIndex, int32 key) : CreateFighterMessage(kModelMessageGusGraves, fighterIndex, position, azm, alt, movement, weapon, weaponIndex, key)
{
}

CreateGusGravesMessage::~CreateGusGravesMessage()
{
}

bool CreateGusGravesMessage::HandleMessage(Player *sender) const
{
	GameWorld *world = static_cast<GameWorld *>(TheWorldMgr->GetWorld());
	if (world)
	{
		GusGravesController *controller = new GusGravesController;

		float azm = GetInitialAzimuth();
		controller->SetPrimaryAzimuth(azm);
		controller->SetLookAzimuth(azm);
		controller->SetLookAltitude(GetInitialAltitude());
		controller->SetMovementFlags(GetMovementFlags());

		GamePlayer *player = static_cast<GamePlayer *>(TheMessageMgr->GetPlayer(GetPlayerKey()));
		if (player)
		{
			player->SetPlayerController(controller);
		}

		Model *model = Model::Get(kModelGusGraves);
		InitializeModel(world, model, controller);

		if (player)
		{
			controller->SetWeapon(GetWeaponIndex(), GetWeaponControllerIndex());

			if (player == TheMessageMgr->GetLocalPlayer())
			{
				world->SetLocalPlayerVisibility();
				world->SetCameraTargetModel(model);

				// We don't want the local player to be able to see himself for the name popup.

				controller->SetCollisionExclusionMask(kCollisionSightPath);

				delete TheScoreBoard;

				DisplayBoard::Open();
				TheDisplayBoard->UpdatePlayerScore();
				TheDisplayBoard->UpdatePlayerHealth();
				TheDisplayBoard->UpdatePlayerWeapons();
			}
		}
	}

	return (true);
}

// ZYUQURM
