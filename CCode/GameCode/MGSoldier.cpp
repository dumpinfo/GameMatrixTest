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


#include "MGSoldier.h"
#include "MGGame.h"


using namespace C4;


namespace
{
	Storage<Soldier> soldierStorage;
}


Soldier::Soldier() :
		soldierControllerRegistration(kControllerSoldier, nullptr),
		soldierModelRegistration(kModelSoldier, nullptr, "soldier/Soldier", kModelPrecache | kModelPrivate)
{
}

Soldier::~Soldier()
{
}

void Soldier::Construct(void)
{
	new(soldierStorage) Soldier;
}

void Soldier::Destruct(void)
{
	soldierStorage->~Soldier();
}


SoldierController::SoldierController() :
		FighterController(kControllerSoldier),
		armorEmissionAttribute(kAttributeMutable),
		helmetEmissionAttribute(kAttributeMutable)
{
}

SoldierController::~SoldierController()
{
}

void SoldierController::Preprocess(void)
{
	FighterController::Preprocess();

	Model *model = GetTargetNode();
	Animator *animator = GetMergeAnimator();

	spineTwistAnimator = new SpineTwistAnimator(model, model->FindNode(Text::StaticHash<'B', 'i', 'p', '0', '1', '_', 'S', 'p', 'i', 'n', 'e'>::value));
	animator->AppendNewSubnode(spineTwistAnimator);

	FrameAnimator *frameAnimator = GetFrameAnimator(0);
	frameAnimator->SetAnimation("soldier/Stand");

	frameAnimator = GetFrameAnimator(1);
	frameAnimator->SetAnimation("soldier/Stand");

	// Locate the head bone so we know where to attach the helmet.
	// Locate the hair node because we'll hide it when there's a helmet visible.

	headBone = model->FindNode(Text::StaticHash<'B', 'i', 'p', '0', '1', '_', 'H', 'e', 'a', 'd'>::value);
	hairNode = model->FindNode(Text::StaticHash<'H', 'a', 'i', 'r'>::value);
	helmetModel = nullptr;

	// Locate the segment of the body that has the armor material.
	// We identify this segment by looking for an emission map in the material.

	Geometry *body = static_cast<Geometry *>(model->FindNode(Text::StaticHash<'S', 'o', 'l', 'd', 'i', 'e', 'r'>::value));
	RenderSegment *segment = body->GetFirstRenderSegment();
	while (segment)
	{
		if ((*segment->GetMaterialObjectPointer())->FindAttribute(kAttributeEmissionTexture))
		{
			armorSegment = segment;
			segment->SetMaterialAttributeList(&armorAttributeList);
			break;
		}

		segment = segment->GetNextRenderSegment();
	}

	armorAttributeList.Append(&armorEmissionAttribute);
	helmetAttributeList.Append(&helmetEmissionAttribute);

	// Set the appearance based on the player's style settings.

	SetFighterStyle(GetFighterPlayer()->GetPlayerStyle(), false);
}

void SoldierController::SendInitialStateMessages(Player *player) const
{
	if (!(GetFighterFlags() & kFighterDead))
	{
		const GamePlayer *fighterPlayer = GetFighterPlayer();
		int32 weaponIndex = GetWeaponController()->GetControllerIndex();
		player->SendMessage(CreateSoldierMessage(GetControllerIndex(), GetTargetNode()->GetWorldPosition(), GetLookAzimuth(), GetLookAltitude(), GetMovementFlags(), fighterPlayer->GetCurrentWeapon(), weaponIndex, fighterPlayer->GetPlayerKey()));
		FighterController::SendInitialStateMessages(player); 
	}
}
 
void SoldierController::SetFighterStyle(const int32 *style, bool prep)
{ 
	static const ConstColorRGBA emissionColor[kPlayerColorCount] =
	{
		{0.5F, 0.5F, 0.5F, 1.0F}, 
		{0.5F, 0.0F, 0.0F, 1.0F},
		{1.0F, 0.5F, 0.0F, 1.0F}, 
		{0.5F, 0.5F, 0.0F, 1.0F}, 
		{0.0F, 0.5F, 0.0F, 1.0F},
		{0.0F, 1.0F, 0.5F, 1.0F},
		{0.0F, 0.5F, 0.5F, 1.0F},
		{0.0F, 0.5F, 1.0F, 1.0F}, 
		{0.5F, 0.0F, 1.0F, 1.0F},
		{0.5F, 0.0F, 0.5F, 1.0F},
		{1.0F, 0.0F, 0.5F, 1.0F}
	};

	static const ConstColorRGBA armorColor[kPlayerColorCount] =
	{
		{1.0F, 1.0F, 1.0F, 1.0F},
		{1.0F, 0.5F, 0.5F, 1.0F},
		{1.0F, 0.75F, 0.25F, 1.0F},
		{1.0F, 1.0F, 0.5F, 1.0F},
		{0.0F, 1.0F, 0.0F, 1.0F},
		{0.5F, 1.0F, 0.75F, 1.0F},
		{0.5F, 1.0F, 1.0F, 1.0F},
		{0.5F, 0.75F, 1.0F, 1.0F},
		{0.75F, 0.5F, 1.0F, 1.0F},
		{1.0F, 0.0F, 1.0F, 1.0F},
		{1.0F, 0.5F, 0.75F, 1.0F}
	};

	delete helmetModel;

	int32 index = MaxZero(Min(style[kPlayerStyleHelmetType], kSoldierHelmetTypeCount - 1));
	if (index != 0)
	{
		static const char *const helmetModelName[kSoldierHelmetTypeCount - 1] =
		{
			"soldier/helmet/1/Helmet", "soldier/helmet/2/Helmet", "soldier/helmet/3/Helmet", "soldier/helmet/4/Helmet", "soldier/helmet/5/Helmet",
			"soldier/helmet/6/Helmet", "soldier/helmet/7/Helmet", "soldier/helmet/8/Helmet", "soldier/helmet/9/Helmet"
		};

		helmetModel = Model::New(helmetModelName[index - 1]);
		helmetModel->SetNodeFlags(kNodeNonpersistent | kNodeCloneInhibit | kNodeAnimateInhibit);
		headBone->AppendSubnode(helmetModel);
		if (prep)
		{
			helmetModel->Preprocess();
		}

		Node *geometry = helmetModel->GetFirstSubnode();
		static_cast<Geometry *>(geometry)->SetMaterialAttributeList(&helmetAttributeList);

		hairNode->Disable();
	}
	else
	{
		helmetModel = nullptr;
		hairNode->Enable();
	}

	index = MaxZero(Min(style[kPlayerStyleEmissionColor], kPlayerColorCount - 1));
	const ConstColorRGBA& emission = emissionColor[index];
	armorEmissionAttribute.SetEmissionColor(emission);
	helmetEmissionAttribute.SetEmissionColor(emission);

	index = MaxZero(Min(style[kPlayerStyleArmorColor], kPlayerColorCount - 1));
	if (index != 0)
	{
		const ConstColorRGBA& armor = armorColor[index];

		armorDiffuseAttribute.SetDiffuseColor(armor);
		armorAttributeList.Append(&armorDiffuseAttribute);

		if (helmetModel)
		{
			helmetDiffuseAttribute.SetDiffuseColor(armor);
			helmetAttributeList.Append(&helmetDiffuseAttribute);
		}
	}
	else
	{
		armorDiffuseAttribute.Detach();
		helmetDiffuseAttribute.Detach();
	}

	index = MaxZero(Min(style[kPlayerStyleArmorTexture], kSoldierArmorTextureCount - 1));
	if (index != 0)
	{
		static const char *const armorTextureName[kSoldierArmorTextureCount - 1] =
		{
			"soldier/armor/01", "soldier/armor/02", "soldier/armor/03", "soldier/armor/04",
			"soldier/armor/05", "soldier/armor/06", "soldier/armor/07", "soldier/armor/08",
			"soldier/armor/Atom1", "soldier/armor/Atom2", "soldier/armor/Biohaz1", "soldier/armor/Biohaz2",
			"soldier/armor/CamoBrn", "soldier/armor/CamoGrn", "soldier/armor/CamoUrb", "soldier/armor/CrashTest",
			"soldier/armor/EnergyBlu", "soldier/armor/EnergyGrn", "soldier/armor/EnergyRed", "soldier/armor/EnergyYel",
			"soldier/armor/Flames", "soldier/armor/Radio1", "soldier/armor/Radio2", "soldier/armor/Veins"
		};

		armorTextureAttribute.SetTexture(armorTextureName[index - 1]);
		armorAttributeList.Append(&armorTextureAttribute);
	}
	else
	{
		armorTextureAttribute.Detach();
	}

	armorSegment->InvalidateShaderData();

	const GamePlayer *player = GetFighterPlayer();
	if (player == TheMessageMgr->GetLocalPlayer())
	{
		GameWorld *world = static_cast<GameWorld *>(GetTargetNode()->GetWorld());
		if (world)
		{
			world->SetLocalPlayerVisibility();
		}
	}
}

void SoldierController::SetFighterMotion(int32 motion)
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
			animator2->SetAnimation("soldier/Run");
			interpolator2->SetMode(kInterpolatorForward | kInterpolatorLoop);

			float w = weight2->GetValue();
			weight2->Set(w, 0.004F, kInterpolatorForward);
			weight1->Set(1.0F - w, 0.004F, kInterpolatorBackward);
			break;
		}

		case kFighterMotionBackward:
		{
			animator2->SetAnimation("soldier/Backward");
			interpolator2->SetMode(kInterpolatorBackward | kInterpolatorLoop);
			interpolator2->SetValue(1.0F);

			float w = weight2->GetValue();
			weight2->Set(w, 0.004F, kInterpolatorForward);
			weight1->Set(1.0F - w, 0.004F, kInterpolatorBackward);
			break;
		}

		case kFighterMotionTurnLeft:

			animator2->SetAnimation("soldier/TurnLeft");
			interpolator2->SetMode(kInterpolatorForward);

			interpolator1->SetMode(kInterpolatorStop);
			weight1->Set(0.0F, 0.0F, kInterpolatorStop);
			weight2->Set(1.0F, 0.0F, kInterpolatorStop);
			break;

		case kFighterMotionTurnRight:

			animator2->SetAnimation("soldier/TurnRight");
			interpolator2->SetMode(kInterpolatorForward);

			interpolator1->SetMode(kInterpolatorStop);
			weight1->Set(0.0F, 0.0F, kInterpolatorStop);
			weight2->Set(1.0F, 0.0F, kInterpolatorStop);
			break;

		case kFighterMotionJump:

			animator2->SetAnimation("soldier/Jump");
			interpolator2->SetMode(kInterpolatorForward);
			break;

		case kFighterMotionDeath:

			animator2->SetAnimation("soldier/Death");
			interpolator2->SetMode(kInterpolatorForward);

			interpolator1->SetMode(kInterpolatorStop);
			weight1->Set(0.0F, 0.0F, kInterpolatorStop);
			weight2->Set(1.0F, 0.0F, kInterpolatorStop);
			break;
	}
}

void SoldierController::Animate(void)
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


CreateSoldierMessage::CreateSoldierMessage() : CreateFighterMessage(kModelMessageSoldier)
{
}

CreateSoldierMessage::CreateSoldierMessage(int32 fighterIndex, const Point3D& position, float azm, float alt, unsigned_int32 movement, int32 weapon, int32 weaponIndex, int32 key) : CreateFighterMessage(kModelMessageSoldier, fighterIndex, position, azm, alt, movement, weapon, weaponIndex, key)
{
}

CreateSoldierMessage::~CreateSoldierMessage()
{
}

bool CreateSoldierMessage::HandleMessage(Player *sender) const
{
	GameWorld *world = static_cast<GameWorld *>(TheWorldMgr->GetWorld());
	if (world)
	{
		SoldierController *controller = new SoldierController;

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

		Model *model = Model::Get(kModelSoldier);
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
