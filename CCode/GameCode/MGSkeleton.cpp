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


#include "MGSkeleton.h"
#include "MGHauntedArm.h"
#include "MGGame.h"


using namespace C4;


namespace
{
	const float kSkeletonRunForce = 200.0F;
	const float kSkeletonResistForce = 20.0F;
	const float kSkeletonTurnRate = 0.01F;
	const float kSkeletonAttackDistance = 2.0F;

	Storage<Skeleton> skeletonStorage;

	const Type skeletonPoseType[kSkeletonPoseCount] =
	{
		kSkeletonPoseWakeup, kSkeletonPoseRest1, kSkeletonPoseRest2, kSkeletonPoseRest3, kSkeletonPoseRest4, kSkeletonPoseRest5
	};

	const char *const skeletonPoseAnimation[kSkeletonPoseCount] =
	{
		"skeleton/Wakeup", "skeleton/Rest1", "skeleton/Rest2", "skeleton/Rest3", "skeleton/Rest4", "skeleton/Rest5"
	};

	const Type skeletonHelmetType[kSkeletonHelmetCount] =
	{
		kSkeletonHelmetRoman, kSkeletonHelmetViking
	};

	const Type skeletonWeaponType[kSkeletonWeaponCount] =
	{
		kSkeletonWeaponSword, kSkeletonWeaponPipe, kSkeletonWeaponBone, kSkeletonWeaponBranch, kSkeletonWeaponArm
	};
}


int32 SkeletonController::globalSkeletonCount = 0;
unsigned_int32 SkeletonController::globalSkeletonFlags = 0;

bool (SkeletonController::*SkeletonController::skeletonStateHandler[kSkeletonStateCount])(void) =
{
	&SkeletonController::HandleSleepingState,
	&SkeletonController::HandleStandingState,
	&SkeletonController::HandleWakingState,
	&SkeletonController::HandleChasingState,
	&SkeletonController::HandleBreakingArmState,
	&SkeletonController::HandleAttackingState,
	&SkeletonController::HandleRecoveringState
};


Skeleton::Skeleton() :
		skeletonControllerRegistration(kControllerSkeleton, TheGame->GetStringTable()->GetString(StringID('CTRL', kControllerSkeleton))),
		skeletonModelRegistration(kModelSkeleton, TheGame->GetStringTable()->GetString(StringID('MODL', 'MNST', kModelSkeleton)), "skeleton/Skeleton", kModelPrecache, kControllerSkeleton),
		skullModelRegistration(kModelSkull, nullptr, "skeleton/parts/Skull", kModelPrecache | kModelPrivate),
		ribsModelRegistration(kModelRibs, nullptr, "skeleton/parts/Ribs", kModelPrecache | kModelPrivate),
		femurModelRegistration(kModelFemur, nullptr, "skeleton/parts/Femur", kModelPrecache | kModelPrivate),
		humerusModelRegistration(kModelHumerus, nullptr, "skeleton/parts/Humerus", kModelPrecache | kModelPrivate),
		shieldModelRegistration(kModelShield, nullptr, "skeleton/shield/Shield", kModelPrecache | kModelPrivate),
		swordModelRegistration(kModelSword, nullptr, "skeleton/sword/Sword", kModelPrecache | kModelPrivate),
		pipeModelRegistration(kModelPipe, nullptr, "skeleton/pipe/Pipe", kModelPrecache | kModelPrivate),
		boneModelRegistration(kModelBone, nullptr, "skeleton/bone/Bone", kModelPrecache | kModelPrivate),
		branchModelRegistration(kModelBranch, nullptr, "skeleton/branch/Branch", kModelPrecache | kModelPrivate),
		romanHelmetModelRegistration(kModelRomanHelmet, nullptr, "skeleton/extras/Roman", kModelPrecache | kModelPrivate),
		vikingHelmet1ModelRegistration(kModelVikingHelmet1, nullptr, "skeleton/extras/Viking1", kModelPrecache | kModelPrivate),
		vikingHelmet2ModelRegistration(kModelVikingHelmet2, nullptr, "skeleton/extras/Viking2", kModelPrecache | kModelPrivate),
		vikingHelmet3ModelRegistration(kModelVikingHelmet3, nullptr, "skeleton/extras/Viking3", kModelPrecache | kModelPrivate),

		generateSkeletonRegistration(kMethodGenerateSkeleton, TheGame->GetStringTable()->GetString(StringID('MTHD', kMethodGenerateSkeleton))),
		getSkeletonCountRegistration(kMethodGetSkeletonCount, TheGame->GetStringTable()->GetString(StringID('MTHD', kMethodGetSkeletonCount)), kMethodNoTarget | kMethodOutputValue)
{
}

Skeleton::~Skeleton()
{
}

void Skeleton::Construct(void)
{
	new(skeletonStorage) Skeleton;
}

void Skeleton::Destruct(void)
{
	skeletonStorage->~Skeleton();
}


inline SkeletonConfiguration::SkeletonConfiguration()
{
	skeletonFlags = 0;
	skeletonPose = kSkeletonPoseNone;
	skeletonHelmet = kSkeletonHelmetNone;
	skeletonWeapon = kSkeletonWeaponNone;
}

inline SkeletonConfiguration::SkeletonConfiguration(const SkeletonConfiguration& skeletonConfiguration)
{ 
	skeletonFlags = skeletonConfiguration.skeletonFlags;
	skeletonPose = skeletonConfiguration.skeletonPose;
	skeletonHelmet = skeletonConfiguration.skeletonHelmet; 
	skeletonWeapon = skeletonConfiguration.skeletonWeapon;
} 

void SkeletonConfiguration::Pack(Packer& data) const
{ 
	data << ChunkHeader('FLAG', 4);
	data << skeletonFlags; 
 
	data << ChunkHeader('POSE', 4);
	data << skeletonPose;

	data << ChunkHeader('HELM', 4); 
	data << skeletonHelmet;

	data << ChunkHeader('WEAP', 4);
	data << skeletonWeapon;
}

bool SkeletonConfiguration::UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data)
{
	switch (chunkHeader->chunkType)
	{
		case 'FLAG':

			data >> skeletonFlags;
			return (true);

		case 'POSE':

			data >> skeletonPose;
			return (true);

		case 'HELM':

			data >> skeletonHelmet;
			return (true);

		case 'WEAP':

			data >> skeletonWeapon;
			return (true);
	}

	return (false);
}

int32 SkeletonConfiguration::GetSettingCount(void) const
{
	return (8);
}

Setting *SkeletonConfiguration::GetSetting(int32 index) const
{
	const StringTable *table = TheGame->GetStringTable();

	if (index == 0)
	{
		int32 selection = 0;
		for (machine a = 0; a < kSkeletonPoseCount; a++)
		{
			if (skeletonPose == skeletonPoseType[a])
			{
				selection = a + 1;
				break;
			}
		}

		const char *title = table->GetString(StringID('CTRL', kControllerSkeleton, 'POSE'));
		MenuSetting *menu = new MenuSetting('POSE', selection, title, kSkeletonPoseCount + 1);
		menu->SetMenuItemString(0, table->GetString(StringID('CTRL', kControllerSkeleton, 'POSE', 'NONE')));
		for (machine a = 0; a < kSkeletonPoseCount; a++)
		{
			menu->SetMenuItemString(a + 1, table->GetString(StringID('CTRL', kControllerSkeleton, 'POSE', skeletonPoseType[a])));
		}

		return (menu);
	}

	if (index == 1)
	{
		const char *title = table->GetString(StringID('CTRL', kControllerSkeleton, 'RISE'));
		return (new BooleanSetting('RISE', ((skeletonFlags & kSkeletonRiseFromGround) != 0), title));
	}

	if (index == 2)
	{
		const char *title = table->GetString(StringID('CTRL', kControllerSkeleton, 'SHLD'));
		return (new BooleanSetting('SHLD', ((skeletonFlags & kSkeletonCarryShield) != 0), title));
	}

	if (index == 3)
	{
		const char *title = table->GetString(StringID('CTRL', kControllerSkeleton, 'PTCH'));
		return (new BooleanSetting('PTCH', ((skeletonFlags & kSkeletonShowPatch) != 0), title));
	}

	if (index == 4)
	{
		const char *title = table->GetString(StringID('CTRL', kControllerSkeleton, 'HOOK'));
		return (new BooleanSetting('HOOK', ((skeletonFlags & kSkeletonShowHook) != 0), title));
	}

	if (index == 5)
	{
		const char *title = table->GetString(StringID('CTRL', kControllerSkeleton, 'MRPH'));
		return (new BooleanSetting('MRPH', ((skeletonFlags & kSkeletonMorphSkull) != 0), title));
	}

	if (index == 6)
	{
		int32 selection = 0;
		if (skeletonHelmet == kSkeletonHelmetRandom)
		{
			selection = 1;
		}
		else
		{
			for (machine a = 0; a < kSkeletonHelmetCount; a++)
			{
				if (skeletonHelmet == skeletonHelmetType[a])
				{
					selection = a + 2;
					break;
				}
			}
		}

		const char *title = table->GetString(StringID('CTRL', kControllerSkeleton, 'HELM'));
		MenuSetting *menu = new MenuSetting('HELM', selection, title, kSkeletonHelmetCount + 2);
		menu->SetMenuItemString(0, table->GetString(StringID('CTRL', kControllerSkeleton, 'HELM', 'NONE')));
		menu->SetMenuItemString(1, table->GetString(StringID('CTRL', kControllerSkeleton, 'HELM', 'RAND')));
		for (machine a = 0; a < kSkeletonHelmetCount; a++)
		{
			menu->SetMenuItemString(a + 2, table->GetString(StringID('CTRL', kControllerSkeleton, 'HELM', skeletonHelmetType[a])));
		}

		return (menu);
	}

	if (index == 7)
	{
		int32 selection = 0;
		if (skeletonWeapon == kSkeletonWeaponRandom)
		{
			selection = 1;
		}
		else
		{
			for (machine a = 0; a < kSkeletonWeaponCount; a++)
			{
				if (skeletonWeapon == skeletonWeaponType[a])
				{
					selection = a + 2;
					break;
				}
			}
		}

		const char *title = table->GetString(StringID('CTRL', kControllerSkeleton, 'WEAP'));
		MenuSetting *menu = new MenuSetting('WEAP', selection, title, kSkeletonWeaponCount + 2);
		menu->SetMenuItemString(0, table->GetString(StringID('CTRL', kControllerSkeleton, 'WEAP', 'NONE')));
		menu->SetMenuItemString(1, table->GetString(StringID('CTRL', kControllerSkeleton, 'WEAP', 'RAND')));
		for (machine a = 0; a < kSkeletonWeaponCount; a++)
		{
			menu->SetMenuItemString(a + 2, table->GetString(StringID('CTRL', kControllerSkeleton, 'WEAP', skeletonWeaponType[a])));
		}

		return (menu);
	}

	return (nullptr);
}

void SkeletonConfiguration::SetSetting(const Setting *setting)
{
	Type identifier = setting->GetSettingIdentifier();

	if (identifier == 'POSE')
	{
		int32 selection = static_cast<const MenuSetting *>(setting)->GetMenuSelection();
		if (selection == 0)
		{
			skeletonPose = kSkeletonPoseNone;
		}
		else
		{
			skeletonPose = skeletonPoseType[selection - 1];
		}
	}
	else if (identifier == 'RISE')
	{
		if (static_cast<const BooleanSetting *>(setting)->GetBooleanValue())
		{
			skeletonFlags |= kSkeletonRiseFromGround;
		}
		else
		{
			skeletonFlags &= ~kSkeletonRiseFromGround;
		}
	}
	else if (identifier == 'SHLD')
	{
		if (static_cast<const BooleanSetting *>(setting)->GetBooleanValue())
		{
			skeletonFlags |= kSkeletonCarryShield;
		}
		else
		{
			skeletonFlags &= ~kSkeletonCarryShield;
		}
	}
	else if (identifier == 'PTCH')
	{
		if (static_cast<const BooleanSetting *>(setting)->GetBooleanValue())
		{
			skeletonFlags |= kSkeletonShowPatch;
		}
		else
		{
			skeletonFlags &= ~kSkeletonShowPatch;
		}
	}
	else if (identifier == 'HOOK')
	{
		if (static_cast<const BooleanSetting *>(setting)->GetBooleanValue())
		{
			skeletonFlags |= kSkeletonShowHook;
		}
		else
		{
			skeletonFlags &= ~kSkeletonShowHook;
		}
	}
	else if (identifier == 'MRPH')
	{
		if (static_cast<const BooleanSetting *>(setting)->GetBooleanValue())
		{
			skeletonFlags |= kSkeletonMorphSkull;
		}
		else
		{
			skeletonFlags &= ~kSkeletonMorphSkull;
		}
	}
	else if (identifier == 'HELM')
	{
		int32 selection = static_cast<const MenuSetting *>(setting)->GetMenuSelection();
		if (selection == 0)
		{
			skeletonHelmet = kSkeletonHelmetNone;
		}
		else if (selection == 1)
		{
			skeletonHelmet = kSkeletonHelmetRandom;
		}
		else
		{
			skeletonHelmet = skeletonHelmetType[selection - 2];
		}
	}
	else if (identifier == 'WEAP')
	{
		int32 selection = static_cast<const MenuSetting *>(setting)->GetMenuSelection();
		if (selection == 0)
		{
			skeletonWeapon = kSkeletonWeaponNone;
		}
		else if (selection == 1)
		{
			skeletonWeapon = kSkeletonWeaponRandom;
		}
		else
		{
			skeletonWeapon = skeletonWeaponType[selection - 2];
		}
	}
}


SkeletonController::SkeletonController() :
		MonsterController(kControllerSkeleton),
		frameAnimatorObserver(this, &SkeletonController::HandleAnimationEvent),
		poseEmissionAttribute(kAttributeMutable),
		pirateDiffuseAttribute(kAttributeMutable)
{
	SetControllerFlags(kControllerAsleep);

	SetMonsterState(kSkeletonStateSleeping);
	SetMonsterMotion(kSkeletonMotionNone);

	SetSourcePosition(Point3D(0.0F, 0.0F, 1.8F));

	skeletonState = 0;
	skeletonDamage = 0;
	skullMorphWeight = 0.0F;
}

SkeletonController::SkeletonController(const SkeletonConfiguration& configuration) :
		MonsterController(kControllerSkeleton),
		SkeletonConfiguration(configuration),
		frameAnimatorObserver(this, &SkeletonController::HandleAnimationEvent),
		poseEmissionAttribute(kAttributeMutable),
		pirateDiffuseAttribute(kAttributeMutable)
{
	SetControllerFlags(kControllerAsleep);

	SetMonsterState(kSkeletonStateSleeping);
	SetMonsterMotion(kSkeletonMotionNone);

	SetSourcePosition(Point3D(0.0F, 0.0F, 1.8F));

	skeletonState = 0;
	skeletonDamage = 0;
	skullMorphWeight = 0.0F;
}

SkeletonController::SkeletonController(const SkeletonController& skeletonController) :
		MonsterController(skeletonController),
		SkeletonConfiguration(skeletonController),
		frameAnimatorObserver(this, &SkeletonController::HandleAnimationEvent),
		poseEmissionAttribute(kAttributeMutable),
		pirateDiffuseAttribute(kAttributeMutable)
{
	SetControllerFlags(kControllerAsleep);

	skeletonState = 0;
	skeletonDamage = 0;
	skullMorphWeight = 0.0F;
}

SkeletonController::~SkeletonController()
{
	UncountMonster(globalSkeletonCount);
}

Controller *SkeletonController::Replicate(void) const
{
	return (new SkeletonController(*this));
}

bool SkeletonController::ValidNode(const Node *node)
{
	return ((node->GetNodeType() == kNodeModel) && (static_cast<const Model *>(node)->GetModelType() == kModelSkeleton));
}

void SkeletonController::Pack(Packer& data, unsigned_int32 packFlags) const
{
	MonsterController::Pack(data, packFlags);
	SkeletonConfiguration::Pack(data);

	if (!(packFlags & kPackEditor))
	{
		data << ChunkHeader('STAT', 4);
		data << skeletonState;

		data << ChunkHeader('SKMW', 4);
		data << skullMorphWeight;

		if (GetMonsterFlags() & kMonsterInitialized)
		{
			data << ChunkHeader('PCOL', sizeof(ColorRGBA));
			data << pirateDiffuseAttribute.GetDiffuseColor();
		}

		if (GetMonsterState() == kSkeletonStateStanding)
		{
			data << ChunkHeader('WTTM', 4);
			data << waitTime;
		}
	}

	data << TerminatorChunk;
}

void SkeletonController::Unpack(Unpacker& data, unsigned_int32 unpackFlags)
{
	MonsterController::Unpack(data, unpackFlags);
	UnpackChunkList<SkeletonController>(data, unpackFlags);
}

bool SkeletonController::UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags)
{
	switch (chunkHeader->chunkType)
	{
		case 'STAT':

			data >> skeletonState;
			return (true);

		case 'SKMW':

			data >> skullMorphWeight;
			return (true);

		case 'PCOL':
		{
			ColorRGBA	color;

			data >> color;
			pirateDiffuseAttribute.SetDiffuseColor(color);
			return (true);
		}

		case 'WTTM':

			data >> waitTime;
			return (true);
	}

	return (SkeletonConfiguration::UnpackChunk(chunkHeader, data));
}

int32 SkeletonController::GetSettingCount(void) const
{
	return (SkeletonConfiguration::GetSettingCount());
}

Setting *SkeletonController::GetSetting(int32 index) const
{
	return (SkeletonConfiguration::GetSetting(index));
}

void SkeletonController::SetSetting(const Setting *setting)
{
	SkeletonConfiguration::SetSetting(setting);
}

void SkeletonController::Preprocess(void)
{
	const Model *model = GetTargetNode();
	if (!model->GetManipulator())
	{
		unsigned_int32 monsterFlags = GetMonsterFlags();
		if (!(monsterFlags & kMonsterInitialized))
		{
			SetMonsterFlags(monsterFlags | kMonsterInitialized);

			if (skeletonHelmet == kSkeletonHelmetRandom)
			{
				skeletonHelmet = skeletonHelmetType[Math::Random(kSkeletonHelmetCount)];
			}

			if (skeletonWeapon == kSkeletonWeaponRandom)
			{
				skeletonWeapon = skeletonWeaponType[Math::Random(kSkeletonWeaponCount - 1)];
			}

			int32 health = 30;

			if (skeletonFlags & kSkeletonCarryShield)
			{
				health += 20;
			}

			if (skeletonHelmet != kSkeletonHelmetNone)
			{
				health += 20;
			}

			SetMonsterHealth(health << 16);

			float f = Math::RandomFloat(1.0F);
			pirateDiffuseAttribute.SetDiffuseColor(ColorRGBA(f, f, f));
		}

		pirateAttributeList.Append(&pirateDiffuseAttribute);

		Node *skullNode = model->FindNode(Text::StaticHash<'S', 'k', 'u', 'l', 'l'>::value);
		Node *rightHandNode = model->FindNode(Text::StaticHash<'R', 'H', 'a', 'n', 'd'>::value);
		Node *leftLowerArmNode = model->FindNode(Text::StaticHash<'L', 'L', 'o', 'w', 'A', 'r', 'm'>::value);
		leftArmNode = model->FindNode(Text::StaticHash<'L', 'H', 'u', 'm', 'e', 'r', 'u', 's'>::value);

		patchNode = nullptr;
		hookNode = nullptr;

		breakArmDistance = Math::RandomFloat(10.0F) + 5.0F;

		if (skeletonState & kSkeletonBrokenArm)
		{
			leftArmNode->Disable();
		}

		skullMorphController = static_cast<MorphController *>(skullNode->GetController());
		skullMorphController->SetMorphWeight(0, skullMorphWeight);

		Node *node = skullNode->GetFirstSubnode();
		while (node)
		{
			const char *name = node->GetNodeName();
			if ((name) && (Text::CompareText(name, "Patch")))
			{
				patchNode = node;

				if (skeletonFlags & kSkeletonShowPatch)
				{
					node->Enable();
					static_cast<Geometry *>(node)->SetMaterialAttributeList(&pirateAttributeList);
				}
				else
				{
					node->Disable();
				}

				break;
			}

			node = node->Next();
		}

		node = leftLowerArmNode->GetFirstSubnode();
		while (node)
		{
			const char *name = node->GetNodeName();
			if ((name) && (Text::CompareText(name, "Hook")))
			{
				hookNode = node;

				if (skeletonFlags & kSkeletonShowHook)
				{
					node->Enable();
					static_cast<Geometry *>(node)->SetMaterialAttributeList(&pirateAttributeList);
				}
				else
				{
					node->Disable();
				}
			}
			else
			{
				if (skeletonFlags & kSkeletonShowHook)
				{
					node->Disable();
				}
				else
				{
					node->Enable();
				}
			}

			node = node->Next();
		}

		if (skeletonFlags & kSkeletonCarryShield)
		{
			Model *shield = Model::Get(kModelShield);
			shield->SetNodeFlags(kNodeNonpersistent | kNodeCloneInhibit | kNodeAnimateInhibit);
			leftLowerArmNode->AppendNewSubnode(shield);
		}

		if (skeletonHelmet != kSkeletonHelmetNone)
		{
			ModelType type = (skeletonHelmet == kSkeletonHelmetRoman) ? kModelRomanHelmet : kModelVikingHelmet1 + Math::Random(3);
			Model *helmet = Model::Get(type);

			helmet->SetNodeFlags(kNodeNonpersistent | kNodeCloneInhibit | kNodeAnimateInhibit);
			skullNode->AppendNewSubnode(helmet);
		}

		if (skeletonWeapon != kSkeletonWeaponNone)
		{
			static const ModelType modelType[kSkeletonWeaponCount] =
			{
				kModelSword, kModelPipe, kModelBone, kModelBranch, kModelHauntedArm
			};

			static const int32 weaponDamage[kSkeletonWeaponCount] =
			{
				24, 20, 16, 16, 20
			};

			for (machine a = 0; a < kSkeletonWeaponCount; a++)
			{
				if (skeletonWeapon == skeletonWeaponType[a])
				{
					skeletonDamage = weaponDamage[a] << 16;

					Model *weapon = Model::Get(modelType[a]);
					weaponModel = weapon;

					weapon->SetNodeFlags(kNodeNonpersistent | kNodeCloneInhibit | kNodeAnimateInhibit);

					if ((skeletonWeapon == kSkeletonWeaponArm) && (!(skeletonState & kSkeletonBrokenArm)))
					{
						weapon->Disable();
					}

					rightHandNode->AppendNewSubnode(weapon);
					break;
				}
			}
		}
	}

	MonsterController::Preprocess();

	if (!model->GetManipulator())
	{
		if (Asleep())
		{
			SetInitialPose();
		}

		SetAttackable(true);

		AnimationBlender *animationBlender = GetAnimationBlender();
		animationBlender->SetFrameAnimatorObserver(&frameAnimatorObserver);

		int32 motion = GetMonsterMotion();
		Interpolator *interpolator = animationBlender->GetRecentAnimator()->GetFrameInterpolator();

		if ((motion == kSkeletonMotionRise) || (motion == kSkeletonMotionWake))
		{
			interpolator->SetCompletionProc(&WakeCallback, this);
		}
		else if (motion == kSkeletonMotionBreakArm)
		{
			interpolator->SetCompletionProc(&BreakArmCallback, this);
		}
		else if ((motion == kSkeletonMotionAttack1) || (motion == kSkeletonMotionAttack2))
		{
			interpolator->SetCompletionProc(&AttackCallback, this);
		}
		else if (motion == kSkeletonMotionDamage)
		{
			interpolator->SetCompletionProc(&DamageCallback, this);
		}
	}
	else if (model->GetFirstSubnode())
	{
		SetInitialPose();
	}
}

void SkeletonController::SetInitialPose(void)
{
	Type pose = skeletonPose;
	if (pose != kSkeletonPoseNone)
	{
		Model *model = GetTargetNode();

		for (machine a = 0; a < kSkeletonPoseCount; a++)
		{
			if (pose == skeletonPoseType[a])
			{
				GetAnimationBlender()->StartAnimation(skeletonPoseAnimation[a], kInterpolatorStop);

				model->Animate();
				model->StopMotion();
				break;
			}
		}

		skeletonLight = static_cast<Light *>(model->FindNode(Text::StaticHash<'L', 'i', 'g', 'h', 't'>::value));
		ribcageQuadEffect = static_cast<QuadEffect *>(model->FindNode(Text::StaticHash<'G', 'l', 'o', 'w'>::value));
		eyeQuadEffect[0] = static_cast<QuadEffect *>(model->FindNode(Text::StaticHash<'L', 'G', 'l', 'o', 'w'>::value));
		eyeQuadEffect[1] = static_cast<QuadEffect *>(model->FindNode(Text::StaticHash<'R', 'G', 'l', 'o', 'w'>::value));

		skeletonLight->Disable();
		ribcageQuadEffect->Disable();
		eyeQuadEffect[0]->Disable();
		eyeQuadEffect[1]->Disable();

		poseEmissionAttribute.SetEmissionColor(ColorRGBA(0.0F, 0.0F, 0.0F, 0.0F));
		poseAttributeList.Append(&poseEmissionAttribute);

		Node *node = model->GetFirstSubnode();
		while (node)
		{
			NodeType type = node->GetNodeType();
			if (type == kNodeModel)
			{
				node = model->GetNextLevelNode(node);
				continue;
			}

			if ((type == kNodeGeometry) && (node != patchNode) && (node != hookNode))
			{
				Geometry *geometry = static_cast<Geometry *>(node);
				geometry->SetMaterialAttributeList(&poseAttributeList);
			}

			node = model->GetNextNode(node);
		}
	}
}

void SkeletonController::Wake(void)
{
	MonsterController::Wake();

	if (GetMonsterMotion() == kSkeletonMotionNone)
	{
		if (skeletonFlags & kSkeletonRiseFromGround)
		{
			SetMonsterState(kSkeletonStateWaking);
			SetSkeletonMotion(kSkeletonMotionRise);

			Model *model = GetTargetNode();
			model->Animate();

			OmniSource *source = new OmniSource("skeleton/Rise", 48.0F);
			source->SetNodePosition(model->GetWorldPosition());
			model->GetWorld()->AddNewNode(source);
		}
		else if (skeletonPose != kSkeletonPoseNone)
		{
			SetMonsterState(kSkeletonStateWaking);
			SetSkeletonMotion(kSkeletonMotionWake);

			skeletonLight->Enable();
			ribcageQuadEffect->Enable();
			eyeQuadEffect[0]->Enable();
			eyeQuadEffect[1]->Enable();

			const Geometry *geometry = static_cast<Geometry *>(leftArmNode);
			const Attribute *attribute = geometry->GetMaterialObject(0)->FindAttribute(kAttributeEmission);
			if (attribute)
			{
				poseEmissionAttribute.SetEmissionColor(static_cast<const EmissionAttribute *>(attribute)->GetEmissionColor());
			}
		}
		else
		{
			GetAnimationBlender()->StartAnimation("skeleton/Stand", kInterpolatorStop);
			SetMonsterState(kSkeletonStateChasing);
			SetSkeletonMotion(kSkeletonMotionChase);
		}
	}

	CountMonster(globalSkeletonCount);
}

void SkeletonController::Move(void)
{
	if ((this->*skeletonStateHandler[GetMonsterState()])())
	{
		MonsterController::Move();
	}
}

bool SkeletonController::HandleSleepingState(void)
{
	return (true);
}

bool SkeletonController::HandleStandingState(void)
{
	if (GetStateTime() >= waitTime)
	{
		Vector3D	direction;

		if (GetEnemyDirection(&direction))
		{
			if (Magnitude(direction) > kSkeletonAttackDistance)
			{
				SetMonsterState(kSkeletonStateChasing);
				SetSkeletonMotion(kSkeletonMotionChase);
			}
			else if (Math::Random(3) != 0)
			{
				SetMonsterState(kSkeletonStateAttacking);
				SetSkeletonMotion(kSkeletonMotionAttack1);
			}
			else
			{
				SetMonsterState(kSkeletonStateAttacking);
				SetSkeletonMotion(kSkeletonMotionAttack2);
			}
		}
	}

	SetExternalForce(GetRepulsionForce());
	SetExternalLinearResistance(Vector2D(kSkeletonResistForce, kSkeletonResistForce));
	return (true);
}

bool SkeletonController::HandleWakingState(void)
{
	SetExternalLinearResistance(Vector2D(kSkeletonResistForce, kSkeletonResistForce));
	return (true);
}

bool SkeletonController::HandleChasingState(void)
{
	Vector3D	direction;

	Vector2D force = GetRepulsionForce();

	if (GetEnemyDirection(&direction))
	{
		float targetDistance = Magnitude(direction);
		float azm0 = Atan(direction.y, direction.x);
		float azm = SetMonsterAzimuth(azm0, kSkeletonTurnRate);

		if ((targetDistance > kSkeletonAttackDistance) || (GetStateTime() < 100))
		{
			force += CosSin(azm) * kSkeletonRunForce;

			if ((skeletonWeapon == kSkeletonWeaponArm) && (!(skeletonState & kSkeletonBrokenArm)) && (targetDistance < breakArmDistance))
			{
				SetMonsterState(kSkeletonStateBreakingArm);
				SetSkeletonMotion(kSkeletonMotionBreakArm);
			}
		}
		else
		{
			if ((skeletonWeapon != kSkeletonWeaponArm) || (skeletonState & kSkeletonBrokenArm))
			{
				if (Math::Random(3) != 0)
				{
					SetMonsterState(kSkeletonStateAttacking);
					SetSkeletonMotion(kSkeletonMotionAttack1);
				}
				else
				{
					SetMonsterState(kSkeletonStateAttacking);
					SetSkeletonMotion(kSkeletonMotionAttack2);
				}
			}
			else
			{
				SetMonsterState(kSkeletonStateBreakingArm);
				SetSkeletonMotion(kSkeletonMotionBreakArm);
			}
		}
	}

	if (GetCharacterState() & kCharacterGround)
	{
		SetExternalLinearResistance(Vector2D(kSkeletonResistForce, kSkeletonResistForce));
	}
	else
	{
		force *= 0.02F;
		SetExternalLinearResistance(Zero2D);
	}

	SetExternalForce(force);
	return (true);
}

bool SkeletonController::HandleBreakingArmState(void)
{
	SetExternalForce(GetRepulsionForce());
	SetExternalLinearResistance(Vector2D(kSkeletonResistForce, kSkeletonResistForce));
	return (true);
}

bool SkeletonController::HandleAttackingState(void)
{
	Vector3D	direction;

	if (GetEnemyDirection(&direction))
	{
		SetMonsterAzimuth(Atan(direction.y, direction.x), kSkeletonTurnRate);
	}

	SetExternalForce(GetRepulsionForce());
	SetExternalLinearResistance(Vector2D(kSkeletonResistForce, kSkeletonResistForce));

	unsigned_int32 state = skeletonState;
	if (state & kSkeletonMorphingSkull)
	{
		float morphWeight = skullMorphWeight + TheTimeMgr->GetFloatDeltaTime() * 0.01F;
		if (morphWeight > 1.0F)
		{
			morphWeight = 1.0F;
			skeletonState = state & ~kSkeletonMorphingSkull;
		}

		skullMorphWeight = morphWeight;
		skullMorphController->SetMorphWeight(0, morphWeight);
		skullMorphController->InvalidateMorph();
		skullMorphController->Invalidate();
	}

	return (true);
}

bool SkeletonController::HandleRecoveringState(void)
{
	SetExternalForce(GetRepulsionForce());
	SetExternalLinearResistance(Vector2D(kSkeletonResistForce, kSkeletonResistForce));
	return (true);
}

void SkeletonController::HandleAnimationEvent(FrameAnimator *animator, CueType cueType)
{
	switch (cueType)
	{
		case 'DMG1':

			DamageEnemy(skeletonDamage, kSkeletonAttackDistance);
			break;

		case 'DMG2':

			DamageEnemy((skeletonDamage * 3) >> 2, kSkeletonAttackDistance);
			break;

		case 'WSH1':
		{
			OmniSource *source = PlaySource("skeleton/Whoosh1", 32.0F);
			source->SetSourceFrequency(Math::RandomFloat(0.2F) + 1.0F);
			break;
		}

		case 'WSH2':
		{
			OmniSource *source = PlaySource("skeleton/Whoosh2", 32.0F);
			source->SetSourceFrequency(Math::RandomFloat(0.2F) + 1.0F);
			break;
		}

		case 'BRAK':
		{
			static const char breakName[3][16] =
			{
				"skeleton/Break1", "skeleton/Break2", "skeleton/Break3"
			};

			skeletonState |= kSkeletonBrokenArm;
			PlaySource(breakName[Math::Random(3)], 48.0F);

			leftArmNode->Disable();
			weaponModel->Enable();
			break;
		}
	}
}

void SkeletonController::SetSkeletonMotion(int32 motion)
{
	SetMonsterMotion(motion);
	switch (motion)
	{
		case kSkeletonMotionStand:

			GetAnimationBlender()->BlendAnimation("skeleton/Stand", kInterpolatorStop, 0.004F);
			break;

		case kSkeletonMotionRise:

			GetAnimationBlender()->StartAnimation("skeleton/Rise", kInterpolatorForward, &WakeCallback, this);
			break;

		case kSkeletonMotionWake:
		{
			static const char wakeName[3][16] =
			{
				"skeleton/Wake1", "skeleton/Wake2", "skeleton/Wake3"
			};

			Type pose = skeletonPose;
			if (pose == kSkeletonPoseWakeup)
			{
				Interpolator *interpolator = GetAnimationBlender()->GetRecentAnimator()->GetFrameInterpolator();
				interpolator->SetMode(kInterpolatorForward);
				interpolator->SetCompletionProc(&WakeCallback, this);
			}
			else
			{
				FrameAnimator *animator = GetAnimationBlender()->BlendAnimation("skeleton/Wakeup", kInterpolatorForward, 0.002F, &WakeCallback, this);
				if (pose - kSkeletonPoseRest1 >= 2)
				{
					animator->GetFrameInterpolator()->SetValue(561.0F);
				}
			}

			PlaySource(wakeName[Math::Random(3)], 48.0F);
			break;
		}

		case kSkeletonMotionChase:
		{
			FrameAnimator *animator = GetAnimationBlender()->BlendAnimation("skeleton/Run", kInterpolatorForward | kInterpolatorLoop, 0.004F);
			animator->GetFrameInterpolator()->SetRate(1.5F);
			break;
		}

		case kSkeletonMotionBreakArm:
		{
			FrameAnimator *animator = GetAnimationBlender()->BlendAnimation("skeleton/Break", kInterpolatorForward, 0.004F, &BreakArmCallback, this);
			animator->GetFrameInterpolator()->SetRate(1.5F);
			break;
		}

		case kSkeletonMotionAttack1:
		{
			static const char attackName[2][32] =
			{
				"skeleton/Attack1", "skeleton/Attack2"
			};

			GetAnimationBlender()->BlendAnimation("skeleton/Attack1", kInterpolatorForward, 0.004F, &AttackCallback, this);
			PlaySource(attackName[Math::Random(2)], 32.0F);

			if (skeletonFlags & kSkeletonMorphSkull)
			{
				skeletonState |= kSkeletonMorphingSkull;
			}

			break;
		}

		case kSkeletonMotionAttack2:

			GetAnimationBlender()->BlendAnimation("skeleton/Attack2", kInterpolatorForward, 0.004F, &AttackCallback, this);
			PlaySource("skeleton/Attack3", 32.0F);

			if (skeletonFlags & kSkeletonMorphSkull)
			{
				skeletonState |= kSkeletonMorphingSkull;
			}

			break;

		case kSkeletonMotionDamage:

			GetAnimationBlender()->BlendAnimation("skeleton/Damage", kInterpolatorForward, 0.004F, &DamageCallback, this);
			break;
	}
}

void SkeletonController::WakeCallback(Interpolator *interpolator, void *cookie)
{
	SkeletonController *skeletonController = static_cast<SkeletonController *>(cookie);
	skeletonController->SetMonsterState(kSkeletonStateChasing);
	skeletonController->SetSkeletonMotion(kSkeletonMotionChase);
}

void SkeletonController::BreakArmCallback(Interpolator *interpolator, void *cookie)
{
	SkeletonController *skeletonController = static_cast<SkeletonController *>(cookie);
	skeletonController->SetMonsterState(kSkeletonStateChasing);
	skeletonController->SetSkeletonMotion(kSkeletonMotionChase);
}

void SkeletonController::AttackCallback(Interpolator *interpolator, void *cookie)
{
	SkeletonController *skeletonController = static_cast<SkeletonController *>(cookie);
	skeletonController->SetMonsterState(kSkeletonStateStanding);
	skeletonController->SetSkeletonMotion(kSkeletonMotionStand);
	skeletonController->waitTime = Math::Random(300) + 200;
}

void SkeletonController::DamageCallback(Interpolator *interpolator, void *cookie)
{
	SkeletonController *skeletonController = static_cast<SkeletonController *>(cookie);
	skeletonController->SetMonsterState(kSkeletonStateStanding);
	skeletonController->SetSkeletonMotion(kSkeletonMotionStand);
	skeletonController->waitTime = Math::Random(200);
}

CharacterStatus SkeletonController::Damage(Fixed damage, unsigned_int32 flags, GameCharacterController *attacker, const Point3D *position, const Vector3D *force)
{
	Model *model = GetTargetNode();

	if (!position)
	{
		position = &GetWorldCenterOfMass();
	}

	if (!(flags & kDamageBloodInhibit))
	{
		int32 particleCount = damage >> 15;
		if (particleCount > 0)
		{
			BloodParticleSystem *blood = new BloodParticleSystem(ColorRGB(0.5F, 0.375F, 0.125F), particleCount);
			blood->SetNodePosition(*position);
			model->GetWorld()->AddNewNode(blood);
		}
	}

	if (DamageTimeExpired(200))
	{
		OmniSource *source = PlaySource("skeleton/Damage", 32.0F);
		source->SetSourceFrequency(Math::RandomFloat(0.2F) + 0.8F);
	}

	unsigned_int32 state = GetMonsterState();
	if (state == kSkeletonStateWaking)
	{
		damage >>= 1;
	}

	CharacterStatus status = MonsterController::Damage(damage, flags, attacker, position, force);
	if ((status == kCharacterDamaged) && (state == kSkeletonStateChasing))
	{
		if (Math::Random(3) == 0)
		{
			SetMonsterState(kSkeletonStateRecovering);
			SetSkeletonMotion(kSkeletonMotionDamage);
		}
	}

	return (status);
}

void SkeletonController::Kill(GameCharacterController *attacker, const Point3D *position, const Vector3D *force)
{
	MonsterController::Kill(attacker, position, force);

	Model *model = GetTargetNode();
	World *world = model->GetWorld();
	const Point3D& modelPosition = model->GetWorldPosition();

	unsigned_int32 flags = globalSkeletonFlags;
	if (!(flags & kSkeletonDeathSound))
	{
		// Set a flag that prevents the death sound from being played more than once per frame.

		globalSkeletonFlags = flags | kSkeletonDeathSound;

		OmniSource *source = new OmniSource("skeleton/Death", 64.0F);
		source->SetNodePosition(modelPosition);
		world->AddNewNode(source);
	}

	int32 count = 6 - TheGame->GetGameDetailLevel() * 2;
	for (machine a = 0; a < count; a++)
	{
		static const ModelType modelType[6] =
		{
			kModelSkull, kModelRibs, kModelFemur, kModelFemur, kModelHumerus, kModelHumerus
		};

		static const float modelHeight[6] =
		{
			1.5F, 1.0F, 0.5F, 0.5F, 1.0F, 1.0F
		};

		Model *bone = Model::Get(modelType[a]);

		RemainsController *controller = new RemainsController(Math::Random(4000, 6000));
		bone->SetController(controller);

		bone->SetNodePosition(Point3D(modelPosition.x, modelPosition.y, modelPosition.z + modelHeight[a]));
		world->AddNewNode(bone);

		Vector3D linearVelocity = Math::RandomUnitVector3D() * 7.0F;
		linearVelocity.z = Fabs(linearVelocity.z);
		controller->SetLinearVelocity(linearVelocity);

		controller->SetAngularVelocity(Math::RandomUnitVector3D() * (Math::RandomFloat(4.0F) + 4.0F));
	}

	delete model;
}


GenerateSkeletonMethod::GenerateSkeletonMethod() : GenerateMonsterMethod(kMethodGenerateSkeleton)
{
}

GenerateSkeletonMethod::GenerateSkeletonMethod(const GenerateSkeletonMethod& generateSkeletonMethod) :
		GenerateMonsterMethod(generateSkeletonMethod),
		SkeletonConfiguration(generateSkeletonMethod)
{
}

GenerateSkeletonMethod::~GenerateSkeletonMethod()
{
}

Method *GenerateSkeletonMethod::Replicate(void) const
{
	return (new GenerateSkeletonMethod(*this));
}

void GenerateSkeletonMethod::Pack(Packer& data, unsigned_int32 packFlags) const
{
	GenerateMonsterMethod::Pack(data, packFlags);
	SkeletonConfiguration::Pack(data);
	data << TerminatorChunk;
}

void GenerateSkeletonMethod::Unpack(Unpacker& data, unsigned_int32 unpackFlags)
{
	GenerateMonsterMethod::Unpack(data, unpackFlags);
	UnpackChunkList<GenerateSkeletonMethod>(data, unpackFlags);
}

bool GenerateSkeletonMethod::UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags)
{
	return (SkeletonConfiguration::UnpackChunk(chunkHeader, data));
}

int32 GenerateSkeletonMethod::GetSettingCount(void) const
{
	return (SkeletonConfiguration::GetSettingCount());
}

Setting *GenerateSkeletonMethod::GetSetting(int32 index) const
{
	return (SkeletonConfiguration::GetSetting(index));
}

void GenerateSkeletonMethod::SetSetting(const Setting *setting)
{
	SkeletonConfiguration::SetSetting(setting);
}

void GenerateSkeletonMethod::Execute(const ScriptState *state)
{
	Node *node = GetTargetNode(state);
	if (node)
	{
		GamePlayer *player = static_cast<GamePlayer *>(TheMessageMgr->GetLocalPlayer());
		if (player->GetPlayerController())
		{
			Model *model = Model::Get(kModelSkeleton);
			SkeletonController *controller = new SkeletonController(*this);
			GenerateMonster(node, model, controller);
		}
	}

	CallCompletionProc();
}


GetSkeletonCountMethod::GetSkeletonCountMethod() : Method(kMethodGetSkeletonCount)
{
}

GetSkeletonCountMethod::GetSkeletonCountMethod(const GetSkeletonCountMethod& getSkeletonCountMethod) : Method(getSkeletonCountMethod)
{
}

GetSkeletonCountMethod::~GetSkeletonCountMethod()
{
}

Method *GetSkeletonCountMethod::Replicate(void) const
{
	return (new GetSkeletonCountMethod(*this));
}

void GetSkeletonCountMethod::Execute(const ScriptState *state)
{
	SetOutputValue(state, SkeletonController::GetGlobalSkeletonCount());
	CallCompletionProc();
}

// ZYUQURM
