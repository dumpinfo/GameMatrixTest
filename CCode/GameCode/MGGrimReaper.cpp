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


#include "MGGrimReaper.h"
#include "MGGame.h"


using namespace C4;


namespace
{
	//const float kGrimReaperRunForce = 500.0F;
	const float kGrimReaperResistForce = 50.0F;

	Storage<GrimReaper> grimReaperStorage;
}


int32 GrimReaperController::globalGrimReaperCount = 0;

bool (GrimReaperController::*GrimReaperController::grimReaperStateHandler[kGrimReaperStateCount])(void) =
{
	&GrimReaperController::HandleSleepingState,
	&GrimReaperController::HandleStandingState,
	&GrimReaperController::HandleDeadState
};


GrimReaper::GrimReaper() :
		grimReaperControllerRegistration(kControllerGrimReaper, TheGame->GetStringTable()->GetString(StringID('CTRL', kControllerGrimReaper))),
		grimReaperModelRegistration(kModelGrimReaper, TheGame->GetStringTable()->GetString(StringID('MODL', 'MNST', kModelGrimReaper)), "reaper/GrimReaper", 0, kControllerGrimReaper),

		generateGrimReaperRegistration(kMethodGenerateGrimReaper, TheGame->GetStringTable()->GetString(StringID('MTHD', kMethodGenerateGrimReaper))),
		getGrimReaperCountRegistration(kMethodGetGrimReaperCount, TheGame->GetStringTable()->GetString(StringID('MTHD', kMethodGetGrimReaperCount)), kMethodNoTarget | kMethodOutputValue)
{
}

GrimReaper::~GrimReaper()
{
}

void GrimReaper::Construct(void)
{
	new(grimReaperStorage) GrimReaper;
}

void GrimReaper::Destruct(void)
{
	grimReaperStorage->~GrimReaper();
}


inline GrimReaperConfiguration::GrimReaperConfiguration()
{
	grimReaperFlags = 0;
}

inline GrimReaperConfiguration::GrimReaperConfiguration(const GrimReaperConfiguration& grimReaperConfiguration)
{
	grimReaperFlags = grimReaperConfiguration.grimReaperFlags;
}

void GrimReaperConfiguration::Pack(Packer& data) const
{
	data << ChunkHeader('FLAG', 4);
	data << grimReaperFlags;
}

bool GrimReaperConfiguration::UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data)
{
	switch (chunkHeader->chunkType)
	{
		case 'FLAG':

			data >> grimReaperFlags;
			return (true);
	}

	return (false);
}

int32 GrimReaperConfiguration::GetSettingCount(void) const
{
	return (0);
}

Setting *GrimReaperConfiguration::GetSetting(int32 index) const
{
	return (nullptr);
}

void GrimReaperConfiguration::SetSetting(const Setting *setting)
{
}


GrimReaperController::GrimReaperController() :
		MonsterController(kControllerGrimReaper),
		frameAnimatorObserver(this, &GrimReaperController::HandleAnimationEvent)
{
	SetControllerFlags(kControllerAsleep);

	SetMonsterHealth(1000 << 16); 
	SetMonsterState(kGrimReaperStateSleeping);
	SetMonsterMotion(kGrimReaperMotionNone);
 
	SetSourcePosition(Point3D(0.0F, 0.0F, 1.5F));
} 

GrimReaperController::GrimReaperController(const GrimReaperConfiguration& configuration) :
		MonsterController(kControllerGrimReaper), 
		GrimReaperConfiguration(configuration),
		frameAnimatorObserver(this, &GrimReaperController::HandleAnimationEvent) 
{ 
	SetControllerFlags(kControllerAsleep);

	SetMonsterHealth(1000 << 16);
	SetMonsterState(kGrimReaperStateSleeping); 
	SetMonsterMotion(kGrimReaperMotionNone);

	SetSourcePosition(Point3D(0.0F, 0.0F, 1.5F));
}

GrimReaperController::GrimReaperController(const GrimReaperController& grimReaperController) :
		MonsterController(grimReaperController),
		GrimReaperConfiguration(grimReaperController),
		frameAnimatorObserver(this, &GrimReaperController::HandleAnimationEvent)
{
	SetControllerFlags(kControllerAsleep);
}

GrimReaperController::~GrimReaperController()
{
	UncountMonster(globalGrimReaperCount);
}

Controller *GrimReaperController::Replicate(void) const
{
	return (new GrimReaperController(*this));
}

bool GrimReaperController::ValidNode(const Node *node)
{
	return ((node->GetNodeType() == kNodeModel) && (static_cast<const Model *>(node)->GetModelType() == kModelGrimReaper));
}

void GrimReaperController::Pack(Packer& data, unsigned_int32 packFlags) const
{
	MonsterController::Pack(data, packFlags);
	GrimReaperConfiguration::Pack(data);

	data << TerminatorChunk;
}

void GrimReaperController::Unpack(Unpacker& data, unsigned_int32 unpackFlags)
{
	MonsterController::Unpack(data, unpackFlags);
	UnpackChunkList<GrimReaperController>(data, unpackFlags);
}

bool GrimReaperController::UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags)
{
	return (GrimReaperConfiguration::UnpackChunk(chunkHeader, data));
}

int32 GrimReaperController::GetSettingCount(void) const
{
	return (GrimReaperConfiguration::GetSettingCount());
}

Setting *GrimReaperController::GetSetting(int32 index) const
{
	return (GrimReaperConfiguration::GetSetting(index));
}

void GrimReaperController::SetSetting(const Setting *setting)
{
	GrimReaperConfiguration::SetSetting(setting);
}

void GrimReaperController::Preprocess(void)
{
	MonsterController::Preprocess();

	if (!GetTargetNode()->GetManipulator())
	{
		GetAnimationBlender()->SetFrameAnimatorObserver(&frameAnimatorObserver);
	}
}

void GrimReaperController::Wake(void)
{
	MonsterController::Wake();

	SetMonsterState(kGrimReaperStateStanding);
	SetGrimReaperMotion(kGrimReaperMotionStand);

	CountMonster(globalGrimReaperCount);
}

void GrimReaperController::Move(void)
{
	if ((this->*grimReaperStateHandler[GetMonsterState()])())
	{
		MonsterController::Move();
	}
}

bool GrimReaperController::HandleSleepingState(void)
{
	return (true);
}

bool GrimReaperController::HandleStandingState(void)
{
	SetExternalForce(GetRepulsionForce());
	SetExternalLinearResistance(Vector2D(kGrimReaperResistForce, kGrimReaperResistForce));
	return (true);
}

bool GrimReaperController::HandleDeadState(void)
{
	if (GetStateTime() >= 10000)
	{
		delete GetTargetNode();
		return (false);
	}

	SetExternalForce(Zero3D);
	SetExternalLinearResistance(Vector2D(kGrimReaperResistForce, kGrimReaperResistForce));
	return (true);
}

void GrimReaperController::HandleAnimationEvent(FrameAnimator *animator, CueType cueType)
{
}

void GrimReaperController::SetGrimReaperMotion(int32 motion)
{
	SetMonsterMotion(motion);
	switch (motion)
	{
		case kGrimReaperMotionStand:

			GetAnimationBlender()->BlendAnimation("reaper/Stand", kInterpolatorForward, 0.004F);
			break;

		case kGrimReaperMotionDeath:

			GetAnimationBlender()->BlendAnimation("reaper/Death", kInterpolatorForward, 0.004F);
			break;
	}
}

CharacterStatus GrimReaperController::Damage(Fixed damage, unsigned_int32 flags, GameCharacterController *attacker, const Point3D *position, const Vector3D *force)
{
	CharacterStatus status = MonsterController::Damage(damage, flags, attacker, position, force);
	return (status);
}

void GrimReaperController::Kill(GameCharacterController *attacker, const Point3D *position, const Vector3D *force)
{
	MonsterController::Kill(attacker, position, force);

	UncountMonster(globalGrimReaperCount);
	SetMonsterState(kGrimReaperStateDead);

	SetGrimReaperMotion(kGrimReaperMotionDeath);
	SetExternalForce(Zero3D);
}


GenerateGrimReaperMethod::GenerateGrimReaperMethod() : GenerateMonsterMethod(kMethodGenerateGrimReaper)
{
}

GenerateGrimReaperMethod::GenerateGrimReaperMethod(const GenerateGrimReaperMethod& generateGrimReaperMethod) :
		GenerateMonsterMethod(generateGrimReaperMethod),
		GrimReaperConfiguration(generateGrimReaperMethod)
{
}

GenerateGrimReaperMethod::~GenerateGrimReaperMethod()
{
}

Method *GenerateGrimReaperMethod::Replicate(void) const
{
	return (new GenerateGrimReaperMethod(*this));
}

void GenerateGrimReaperMethod::Pack(Packer& data, unsigned_int32 packFlags) const
{
	GenerateMonsterMethod::Pack(data, packFlags);
	GrimReaperConfiguration::Pack(data);
	data << TerminatorChunk;
}

void GenerateGrimReaperMethod::Unpack(Unpacker& data, unsigned_int32 unpackFlags)
{
	GenerateMonsterMethod::Unpack(data, unpackFlags);
	UnpackChunkList<GenerateGrimReaperMethod>(data, unpackFlags);
}

bool GenerateGrimReaperMethod::UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags)
{
	return (GrimReaperConfiguration::UnpackChunk(chunkHeader, data));
}

int32 GenerateGrimReaperMethod::GetSettingCount(void) const
{
	return (GrimReaperConfiguration::GetSettingCount());
}

Setting *GenerateGrimReaperMethod::GetSetting(int32 index) const
{
	return (GrimReaperConfiguration::GetSetting(index));
}

void GenerateGrimReaperMethod::SetSetting(const Setting *setting)
{
	GrimReaperConfiguration::SetSetting(setting);
}

void GenerateGrimReaperMethod::Execute(const ScriptState *state)
{
	Node *node = GetTargetNode(state);
	if (node)
	{
		GamePlayer *player = static_cast<GamePlayer *>(TheMessageMgr->GetLocalPlayer());
		if (player->GetPlayerController())
		{
			Model *model = Model::Get(kModelGrimReaper);
			GrimReaperController *controller = new GrimReaperController(*this);
			GenerateMonster(node, model, controller);
		}
	}

	CallCompletionProc();
}


GetGrimReaperCountMethod::GetGrimReaperCountMethod() : Method(kMethodGetGrimReaperCount)
{
}

GetGrimReaperCountMethod::GetGrimReaperCountMethod(const GetGrimReaperCountMethod& getGrimReaperCountMethod) : Method(getGrimReaperCountMethod)
{
}

GetGrimReaperCountMethod::~GetGrimReaperCountMethod()
{
}

Method *GetGrimReaperCountMethod::Replicate(void) const
{
	return (new GetGrimReaperCountMethod(*this));
}

void GetGrimReaperCountMethod::Execute(const ScriptState *state)
{
	SetOutputValue(state, GrimReaperController::GetGlobalGrimReaperCount());
	CallCompletionProc();
}

// ZYUQURM
