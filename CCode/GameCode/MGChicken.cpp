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


#include "MGChicken.h"
#include "MGGame.h"


using namespace C4;


namespace
{
	const float kChickenWalkForce = 0.5F;
	const float kChickenRunForce = 6.0F;
	const float kChickenResistForce = 0.8F;
	const float kChickenTurnRate = 0.001F;

	Storage<Chicken> chickenStorage;

	const Type chickenVariantType[kChickenVariantCount] =
	{
		kChickenVariantWhite, kChickenVariantBrown, kChickenVariantBlack
	};

	const MaterialType chickenMaterialType[kChickenVariantCount] =
	{
		kMaterialChickenWhite, kMaterialChickenBrown, kMaterialChickenBlack
	};
}


bool (ChickenController::*ChickenController::chickenStateHandler[kChickenStateCount])(void) =
{
	&ChickenController::HandleSleepingState,
	&ChickenController::HandleStandingState,
	&ChickenController::HandleWalkingState,
	&ChickenController::HandleRunningState
};


Chicken::Chicken() :
		chickenControllerRegistration(kControllerChicken, TheGame->GetStringTable()->GetString(StringID('CTRL', kControllerChicken))),
		chickenModelRegistration(kModelChicken, TheGame->GetStringTable()->GetString(StringID('MODL', 'ANML', kModelChicken)), "chicken/chicken/Chicken", 0, kControllerChicken),

		whiteMaterialRegistration(kMaterialChickenWhite, "chicken/chicken/Chicken-A"),
		brownMaterialRegistration(kMaterialChickenBrown, "chicken/chicken/Chicken-B"),
		blackMaterialRegistration(kMaterialChickenBlack, "chicken/chicken/Chicken-C")
{
}

Chicken::~Chicken()
{
}

void Chicken::Construct(void)
{
	new(chickenStorage) Chicken;
}

void Chicken::Destruct(void)
{
	chickenStorage->~Chicken();
}


ChickenController::ChickenController() : GameCharacterController(kCharacterAnimal, kControllerChicken)
{
	chickenFlags = 0;
	chickenVariant = kChickenVariantWhite;

	chickenState = kChickenStateSleeping;
	chickenMotion = kChickenMotionNone;
}

ChickenController::ChickenController(const ChickenController& chickenController) : GameCharacterController(chickenController)
{
	chickenFlags = chickenController.chickenFlags & ~kChickenInitialized;
	chickenVariant = chickenController.chickenVariant;

	chickenState = kChickenStateSleeping;
	chickenMotion = kChickenMotionNone;
}

ChickenController::~ChickenController()
{
}

Controller *ChickenController::Replicate(void) const
{
	return (new ChickenController(*this));
}

bool ChickenController::ValidNode(const Node *node)
{
	return ((node->GetNodeType() == kNodeModel) && (static_cast<const Model *>(node)->GetModelType() == kModelChicken));
}

void ChickenController::Pack(Packer& data, unsigned_int32 packFlags) const
{
	GameCharacterController::Pack(data, packFlags);

	data << ChunkHeader('FLAG', 4);
	data << chickenFlags;
 
	data << ChunkHeader('VRNT', 4);
	data << chickenVariant;
 
	if (!(packFlags & kPackEditor))
	{ 
		data << ChunkHeader('STAT', 4);
		data << chickenState;
 
		data << ChunkHeader('MOTN', 4);
		data << chickenMotion; 
 
		data << ChunkHeader('SDTM', 4);
		data << soundTime;

		data << ChunkHeader('DTIM', 4); 
		data << decisionTime;

		if (turnTime > 0)
		{
			data << ChunkHeader('TURN', 8);
			data << turnTime;
			data << turnRate;
		}

		data << ChunkHeader('AZTH', 4);
		data << chickenAzimuth;

		PackHandle handle = data.BeginChunk('ANIM');
		animationBlender.Pack(data, packFlags);
		data.EndChunk(handle);
	}

	data << TerminatorChunk;
}

void ChickenController::Unpack(Unpacker& data, unsigned_int32 unpackFlags)
{
	GameCharacterController::Unpack(data, unpackFlags);
	UnpackChunkList<ChickenController>(data, unpackFlags);
}

bool ChickenController::UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags)
{
	switch (chunkHeader->chunkType)
	{
		case 'FLAG':

			data >> chickenFlags;
			return (true);

		case 'VRNT':

			data >> chickenVariant;
			return (true);

		case 'STAT':

			data >> chickenState;
			return (true);

		case 'MOTN':

			data >> chickenMotion;
			return (true);

		case 'SDTM':

			data >> soundTime;
			return (true);

		case 'DTIM':

			data >> decisionTime;
			return (true);

		case 'TURN':

			data >> turnTime;
			data >> turnRate;
			return (true);

		case 'AZTH':

			data >> chickenAzimuth;
			return (true);

		case 'ANIM':

			animationBlender.Unpack(data, unpackFlags);
			return (true);
	}

	return (false);
}

int32 ChickenController::GetSettingCount(void) const
{
	return (1);
}

Setting *ChickenController::GetSetting(int32 index) const
{
	const StringTable *table = TheGame->GetStringTable();

	if (index == 0)
	{
		int32 selection = 0;
		for (machine a = 1; a < kChickenVariantCount; a++)
		{
			if (chickenVariant == chickenVariantType[a])
			{
				selection = a;
				break;
			}
		}

		const char *title = table->GetString(StringID('CTRL', kControllerChicken, 'VRNT'));
		MenuSetting *menu = new MenuSetting('VRNT', selection, title, kChickenVariantCount);
		for (machine a = 0; a < kChickenVariantCount; a++)
		{
			menu->SetMenuItemString(a, table->GetString(StringID('CTRL', kControllerChicken, 'VRNT', chickenVariantType[a])));
		}

		return (menu);
	}

	return (nullptr);
}

void ChickenController::SetSetting(const Setting *setting)
{
	Type identifier = setting->GetSettingIdentifier();

	if (identifier == 'VRNT')
	{
		int32 selection = static_cast<const MenuSetting *>(setting)->GetMenuSelection();
		chickenVariant = chickenVariantType[selection];
	}
}

void ChickenController::Preprocess(void)
{
	Model *model = GetTargetNode();
	if (!model->GetManipulator())
	{
		unsigned_int32 flags = chickenFlags;
		if (!(flags & kChickenInitialized))
		{
			chickenFlags = flags | kChickenInitialized;

			soundTime = 0;
			decisionTime = 0;
			turnTime = 0;

			const Transform4D& transform = model->GetWorldTransform();
			const Vector3D direction = transform[0];
			chickenAzimuth = Atan(direction.y, direction.x);
		}
	}

	GameCharacterController::Preprocess();

	for (machine a = 0; a < kChickenVariantCount; a++)
	{
		if (chickenVariant == chickenVariantType[a])
		{
			Node *node = model->GetFirstSubnode();
			while (node)
			{
				if (node->GetNodeType() == kNodeGeometry)
				{
					Geometry *geometry = static_cast<Geometry *>(node);
					geometry->SetMaterialObject(0, AutoReleaseMaterial(chickenMaterialType[a]));
					break;
				}

				node = node->Next();
			}

			break;
		}
	}

	animationBlender.Preprocess(model);

	if (!model->GetManipulator())
	{
		if (chickenMotion == kChickenMotionStand)
		{
			Interpolator *interpolator = animationBlender.GetRecentAnimator()->GetFrameInterpolator();
			interpolator->SetCompletionProc(&StandCallback, this);
		}
	}
}

inline void ChickenController::NewSoundTime(void)
{
	soundTime = Math::Random(2000) + 500;
}

inline void ChickenController::NewStandingTime(void)
{
	decisionTime = Math::Random(5000) + 2000;
}

inline void ChickenController::NewWalkingTime(void)
{
	decisionTime = Math::Random(7000) + 1000;
}

inline void ChickenController::NewRunningTime(void)
{
	decisionTime = Math::Random(1000) + 1000;
}

void ChickenController::Wake(void)
{
	GameCharacterController::Wake();

	if (chickenMotion == kChickenMotionNone)
	{
		chickenState = kChickenStateStanding;

		NewSoundTime();
		NewStandingTime();

		animationBlender.StartAnimation("chicken/chicken/Stand-A", kInterpolatorStop);
		SetChickenMotion(kChickenMotionStand);
	}
}

void ChickenController::Move(void)
{
	if ((this->*chickenStateHandler[chickenState])())
	{
		Model *model = GetTargetNode();
		int32 dt = TheTimeMgr->GetDeltaTime();

		int32 time = soundTime;
		if (time > 0)
		{
			soundTime = (time -= dt);
			if (time <= 0)
			{
				static const char *const name[9] =
				{
					"chicken/chicken/Chicken1", "chicken/chicken/Chicken2", "chicken/chicken/Chicken3", "chicken/chicken/Chicken4", "chicken/chicken/Chicken5",
					"chicken/chicken/Chicken6", "chicken/chicken/Chicken7", "chicken/chicken/Chicken8", "chicken/chicken/Chicken9"
				};

				OmniSource *source = new OmniSource(name[Math::Random(9)], 20.0F);
				source->SetNodeFlags(kNodeCloneInhibit | kNodeAnimateInhibit);
				source->SetSourceVelocity(GetLinearVelocity());
				source->SetCompletionProc(&SoundComplete, this);
				model->AppendNewSubnode(source);
			}
		}

		SetCharacterOrientation(chickenAzimuth);
		model->Animate();
	}
}

bool ChickenController::HandleSleepingState(void)
{
	return (false);
}

bool ChickenController::HandleStandingState(void)
{
	int32 time = (decisionTime -= TheTimeMgr->GetDeltaTime());
	if (time < 0)
	{
		chickenState = kChickenStateWalking;
		SetChickenMotion(kChickenMotionWalk);
		NewWalkingTime();

		turnTime = Math::Random((int32) (0.75F / kChickenTurnRate));
		turnRate = (Math::Random(2) == 0) ? kChickenTurnRate : -kChickenTurnRate;
	}

	SetExternalForce(Zero3D);
	SetExternalLinearResistance(Vector2D(kChickenResistForce, kChickenResistForce));
	return (true);
}

bool ChickenController::HandleWalkingState(void)
{
	int32 dt = TheTimeMgr->GetDeltaTime();

	int32 time = (decisionTime -= dt);
	if (time > 0)
	{
		time = turnTime;
		if (time > 0)
		{
			turnTime = time - dt;

			float azm = chickenAzimuth + turnRate * TheTimeMgr->GetFloatDeltaTime();
			if (azm > K::tau_over_2)
			{
				azm -= K::tau;
			}
			else if (azm < -K::tau_over_2)
			{
				azm += K::tau;
			}

			chickenAzimuth = azm;
		}

		SetExternalForce(CosSin(chickenAzimuth) * kChickenWalkForce);
	}
	else
	{
		chickenState = kChickenStateStanding;
		SetChickenMotion(kChickenMotionStand);
		NewStandingTime();
	}

	SetExternalLinearResistance(Vector2D(kChickenResistForce, kChickenResistForce));
	return (true);
}

bool ChickenController::HandleRunningState(void)
{
	int32 dt = TheTimeMgr->GetDeltaTime();

	int32 time = (decisionTime -= dt);
	if (time > 0)
	{
		time = turnTime;
		if (time > 0)
		{
			turnTime = time - dt;

			float azm = chickenAzimuth + turnRate * TheTimeMgr->GetFloatDeltaTime();
			if (azm > K::tau_over_2)
			{
				azm -= K::tau;
			}
			else if (azm < -K::tau_over_2)
			{
				azm += K::tau;
			}

			chickenAzimuth = azm;
		}

		SetExternalForce(CosSin(chickenAzimuth) * kChickenRunForce);
	}
	else
	{
		if (Math::Random(3) != 0)
		{
			NewRunningTime();

			turnTime = Math::Random((int32) (0.75F / kChickenTurnRate));
			turnRate = (Math::Random(2) == 0) ? kChickenTurnRate * 2.0F : kChickenTurnRate * -2.0F;
		}
		else
		{
			chickenState = kChickenStateStanding;
			SetChickenMotion(kChickenMotionStand);
			NewStandingTime();
		}
	}

	SetExternalLinearResistance(Vector2D(kChickenResistForce, kChickenResistForce));
	return (true);
}

void ChickenController::SetChickenMotion(int32 motion)
{
	chickenMotion = motion;
	switch (motion)
	{
		case kChickenMotionStand:
		{
			static const char *const name[5] =
			{
				"chicken/chicken/Stand-A", "chicken/chicken/Stand-B", "chicken/chicken/Stand-C",
				"chicken/chicken/Peck-A", "chicken/chicken/Peck-B"
			};

			animationBlender.BlendAnimation(name[Math::Random(5)], kInterpolatorForward, 0.005F, &StandCallback, this);
			break;
		}

		case kChickenMotionWalk:
		{
			animationBlender.GetRecentAnimator()->GetFrameInterpolator()->SetCompletionProc(nullptr);
			FrameAnimator *animator = animationBlender.BlendAnimation("chicken/chicken/Walk", kInterpolatorForward | kInterpolatorLoop, 0.004F);
			animator->GetFrameInterpolator()->SetRate(2.0F);
			break;
		}

		case kChickenMotionRun:

			animationBlender.GetRecentAnimator()->GetFrameInterpolator()->SetCompletionProc(nullptr);
			animationBlender.BlendAnimation("chicken/chicken/Run", kInterpolatorForward | kInterpolatorLoop, 0.004F);
			break;
	}
}

void ChickenController::StandCallback(Interpolator *interpolator, void *cookie)
{
	ChickenController *chickenController = static_cast<ChickenController *>(cookie);
	chickenController->SetChickenMotion(kChickenMotionStand);
}

void ChickenController::SoundComplete(Source *source, void *cookie)
{
	static_cast<ChickenController *>(cookie)->NewSoundTime();
}

RigidBodyStatus ChickenController::HandleNewRigidBodyContact(const RigidBodyContact *contact, RigidBodyController *contactBody)
{
	if (contactBody->GetRigidBodyType() == kRigidBodyCharacter)
	{
		const GameCharacterController *character = static_cast<GameCharacterController *>(contactBody);
		if (character->GetCharacterType() == kCharacterAnimal)
		{
			return (kRigidBodyUnchanged);
		}
	}

	if (chickenState != kChickenStateRunning)
	{
		chickenState = kChickenStateRunning;
		SetChickenMotion(kChickenMotionRun);

		NewRunningTime();
	}

	return (kRigidBodyUnchanged);
}

RigidBodyStatus ChickenController::HandleNewGeometryContact(const GeometryContact *contact)
{
	const Transform4D& transform = GetFinalTransform();
	const Vector3D& normal = contact->GetRigidBodyContactNormal();

	if (Fabs(transform.GetRow(2) ^ normal) < GetGroundCosine())
	{
		turnRate = NonzeroFsgn(normal.y) * -kChickenTurnRate;
		if (chickenState == kChickenStateRunning)
		{
			turnRate *= 2.0F;
		}

		int32 time = (int32) Fabs(1.6F / turnRate);
		turnTime = time + Math::Random(time >> 2);
		decisionTime = Max(decisionTime, turnTime);
	}

	return (kRigidBodyUnchanged);
}

// ZYUQURM
