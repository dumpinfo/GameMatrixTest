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


#include "MGFighter.h"
#include "MGGame.h"


using namespace C4;


namespace
{
	const float kFighterRunForce = 70.0F;
	const float kFighterResistForce = 10.0F;
}


FighterInteractor::FighterInteractor(FighterController *controller)
{
	fighterController = controller;
}

FighterInteractor::~FighterInteractor()
{
}

void FighterInteractor::HandleInteractionEvent(InteractionEventType type, Node *node, const Point3D *position)
{
	// Only interact with things if not firing a weapon.
	if ((fighterController->GetFighterFlags() & kFighterFiring) == 0)
	{
		// Always call the base class counterpart.
		Interactor::HandleInteractionEvent(type, node, position);

		switch (type)
		{
			case kInteractionEventEngage:

				// This event is received when the player engages an interactive object.

				if (TheMessageMgr->Server())
				{
					Controller *controller = node->GetController();
					if (controller)
					{
						// Send a message to the player's machine indicating that interaction has begun.

						GamePlayer *player = fighterController->GetFighterPlayer();
						player->SendMessage(FighterBeginInteractionMessage(fighterController->GetControllerIndex(), controller->GetControllerIndex(), *position));
					}
				}

				break;

			case kInteractionEventDisengage:

				// This event is received when the player disengages an interactive object.

				if (TheMessageMgr->Server())
				{
					Controller *controller = node->GetController();
					if (controller)
					{
						// Send a message to the player's machine indicating that interaction has ended.

						GamePlayer *player = fighterController->GetFighterPlayer();
						player->SendMessage(FighterInteractionMessage(FighterController::kFighterMessageDisengageInteraction, fighterController->GetControllerIndex(), controller->GetControllerIndex()));
					}
				}

				break;

			case kInteractionEventTrack:

				// This event is received while the player is engaged with interactive object.

				if (fighterController->GetFighterPlayer() == TheMessageMgr->GetLocalPlayer())
				{
					// Only inform the interaction target on the client machine.

					Controller *controller = node->GetController();
					if (controller)
					{
						controller->HandleInteractionEvent(kInteractionEventTrack, position);
					}
				}

				break;
		}
	}
}


FighterController::FighterController(ControllerType type) :
		GameCharacterController(kCharacterPlayer, type),
		fighterInteractor(this),
		frameAnimatorObserver(this, &FighterController::HandleAnimationEvent),
		worldUpdateObserver(this, &FighterController::UpdateWeapon)
{
	weaponModel = nullptr;
	weaponMountMarker = nullptr;

	iconIndex = -1;
	iconEffect = nullptr; 
	shieldEffect = nullptr;
	mountNode = nullptr;
	flashlight = nullptr; 
	rootAnimator = nullptr;
 
	fighterFlags = 0;

	primaryAzimuth = 0.0F; 
	lookAzimuth = 0.0F;
	lookAltitude = 0.0F; 
 
	deltaLookAzimuth = 0.0F;
	deltaLookAltitude = 0.0F;
	lookInterpolateParam = 0.0F;
 
	movementFlags = 0;
	fighterMotion = kFighterMotionNone;
	motionComplete = false;
	targetDistance = 0.0F;
	damageTime = 0;

	SetCollisionExclusionMask(kCollisionCorpse);
}

FighterController::~FighterController()
{
	delete rootAnimator;
	delete mountNode;
}

void FighterController::Pack(Packer& data, unsigned_int32 packFlags) const
{
	GameCharacterController::Pack(data, packFlags);

	data << ChunkHeader('FLAG', 4);
	unsigned_int32 flags = fighterFlags & ~kFighterFiring;
	data << flags;

	data << ChunkHeader('ORNT', 12);
	data << primaryAzimuth;
	data << lookAzimuth;
	data << lookAltitude;

	data << ChunkHeader('DGTM', 4);
	data << damageTime;

	data << TerminatorChunk;
}

void FighterController::Unpack(Unpacker& data, unsigned_int32 unpackFlags)
{
	GameCharacterController::Unpack(data, unpackFlags);
	UnpackChunkList<FighterController>(data, unpackFlags);
}

bool FighterController::UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags)
{
	switch (chunkHeader->chunkType)
	{
		case 'FLAG':

			data >> fighterFlags;
			return (true);

		case 'ORNT':

			data >> primaryAzimuth;
			data >> lookAzimuth;
			data >> lookAltitude;
			return (true);

		case 'DGTM':

			data >> damageTime;
			return (true);
	}

	return (false);
}

void FighterController::Preprocess(void)
{
	GameCharacterController::Preprocess();

	SetFrictionCoefficient(0.001F);
	SetCollisionKind(GetCollisionKind() | kCollisionPlayer);

	modelAzimuth = primaryAzimuth;

	mountNode = new Node;
	mountNode->SetNodeFlags(kNodeNonpersistent | kNodeCloneInhibit | kNodeAnimateInhibit);

	Model *model = GetTargetNode();
	model->AppendSubnode(mountNode);

	rootAnimator = new MergeAnimator(model);
	mergeAnimator = new MergeAnimator(model);
	blendAnimator = new BlendAnimator(model);
	frameAnimator[0] = new FrameAnimator(model);
	frameAnimator[1] = new FrameAnimator(model);
	frameAnimator[1]->GetFrameInterpolator()->SetCompletionProc(&MotionComplete, this);
	weaponIKAnimator[0] = new WeaponIKAnimator(model, model->FindNode(Text::StaticHash<'B', 'i', 'p', '0', '0', '1', '_', 'L', '_', 'U', 'p', 'p', 'e', 'r', 'A', 'r', 'm'>::value), 'mntl');
	weaponIKAnimator[1] = new WeaponIKAnimator(model, model->FindNode(Text::StaticHash<'B', 'i', 'p', '0', '0', '1', '_', 'R', '_', 'U', 'p', 'p', 'e', 'r', 'A', 'r', 'm'>::value), 'mntr');

	rootAnimator->AppendSubnode(mergeAnimator);
	rootAnimator->AppendNewSubnode(weaponIKAnimator[0]);
	rootAnimator->AppendNewSubnode(weaponIKAnimator[1]);
	mergeAnimator->AppendSubnode(blendAnimator);
	blendAnimator->AppendSubnode(frameAnimator[0]);
	blendAnimator->AppendSubnode(frameAnimator[1]);
	model->SetRootAnimator(rootAnimator);

	SetFrameAnimatorObserver(&frameAnimatorObserver);

	World *world = model->GetWorld();
	world->AddUpdateObserver(&worldUpdateObserver);

	if (TheMessageMgr->Server())
	{
		world->AddInteractor(&fighterInteractor);
	}

	previousCenterOfMass = GetWorldCenterOfMass();
	weaponSwitchTime = TheTimeMgr->GetSystemAbsoluteTime();
}

ControllerMessage *FighterController::CreateMessage(ControllerMessageType type) const
{
	switch (type)
	{
		case kFighterMessageEngageInteraction:

			return (new FighterBeginInteractionMessage(GetControllerIndex()));

		case kFighterMessageDisengageInteraction:

			return (new FighterInteractionMessage(kFighterMessageDisengageInteraction, GetControllerIndex()));

		case kFighterMessageBeginMovement:
		case kFighterMessageEndMovement:
		case kFighterMessageChangeMovement:

			return (new FighterMovementMessage(type, GetControllerIndex()));

		case kFighterMessageBeginShield:
		case kFighterMessageEndShield:

			return (new ControllerMessage(type, GetControllerIndex()));

		case kFighterMessageBeginIcon:
		case kFighterMessageEndIcon:

			return (new FighterIconMessage(type, GetControllerIndex()));

		case kFighterMessageTeleport:

			return (new FighterTeleportMessage(GetControllerIndex()));

		case kFighterMessageLaunch:

			return (new CharacterStateMessage(kFighterMessageLaunch, GetControllerIndex()));

		case kFighterMessageLand:

			return (new ControllerMessage(kFighterMessageLand, GetControllerIndex()));

		case kFighterMessageUpdate:

			return (new FighterUpdateMessage(GetControllerIndex()));

		case kFighterMessageWeapon:

			return (new FighterWeaponMessage(GetControllerIndex()));

		case kFighterMessageEmptyAmmo:

			return (new ControllerMessage(kFighterMessageEmptyAmmo, GetControllerIndex()));

		case kFighterMessageDamage:

			return (new FighterDamageMessage(GetControllerIndex()));

		case kFighterMessageDeath:

			return (new ControllerMessage(kFighterMessageDeath, GetControllerIndex()));
	}

	return (GameCharacterController::CreateMessage(type));
}

void FighterController::ReceiveMessage(const ControllerMessage *message)
{
	switch (message->GetControllerMessageType())
	{
		case kFighterMessageEngageInteraction:
		{
			const FighterBeginInteractionMessage *m = static_cast<const FighterBeginInteractionMessage *>(message);

			World *world = GetTargetNode()->GetWorld();
			Controller *controller = world->GetController(m->GetInteractionControllerIndex());
			if (controller)
			{
				controller->HandleInteractionEvent(kInteractionEventEngage, &m->GetInteractionPosition());

				Node *node = controller->GetTargetNode();
				if (node->GetNodeType() == kNodeEffect)
				{
					const Effect *effect = static_cast<Effect *>(node);
					if (effect->GetEffectType() == kEffectPanel)
					{
						// If the player is looking at an interface panel, then play a sound.

						OmniSource *source = new OmniSource("sound/Engage", 10.0F);
						source->SetSourcePriority(kSoundPriorityPlayer);
						source->SetNodePosition(effect->GetBoundingSphere()->GetCenter());
						world->AddNewNode(source);
					}
				}
				else
				{
					// If the player is looking at a geometry, show the "Use" message.

					TheUsePopupBoard->Engage();
				}

				if (!TheMessageMgr->Server())
				{
					fighterInteractor.SetInteractionNode(node);
					world->AddInteractor(&fighterInteractor);
				}
			}

			break;
		}

		case kFighterMessageDisengageInteraction:
		{
			const FighterInteractionMessage *m = static_cast<const FighterInteractionMessage *>(message);

			World *world = GetTargetNode()->GetWorld();
			Controller *controller = world->GetController(m->GetInteractionControllerIndex());
			if (controller)
			{
				controller->HandleInteractionEvent(kInteractionEventDisengage, nullptr);

				Node *node = controller->GetTargetNode();
				if (node->GetNodeType() == kNodeEffect)
				{
					const Effect *effect = static_cast<Effect *>(node);
					if (effect->GetEffectType() == kEffectPanel)
					{
						// If the player was looking at an interface panel, then play a sound.

						OmniSource *source = new OmniSource("sound/Disengage", 10.0F);
						source->SetSourcePriority(kSoundPriorityPlayer);
						source->SetNodePosition(effect->GetBoundingSphere()->GetCenter());
						world->AddNewNode(source);
					}
				}
				else
				{
					// If the player was looking at a geometry, hide the "Use" message.

					TheUsePopupBoard->Disengage();
				}

				if (!TheMessageMgr->Server())
				{
					fighterInteractor.SetInteractionNode(nullptr);
					world->RemoveInteractor(&fighterInteractor);
				}
			}

			break;
		}

		case kFighterMessageBeginMovement:
		{
			const FighterMovementMessage *m = static_cast<const FighterMovementMessage *>(message);

			unsigned_int32 flag = m->GetMovementFlag();
			if (flag == kMovementUp)
			{
				if ((!(GetCharacterState() & kCharacterJumping)) && (GetOffGroundTime() < 250.0F))
				{
					// Reduce the jump impulse based on the vertical velocity that the character already has.

					float impulse = FmaxZero(2.5F * GetInverseBodyMass() - GetLinearVelocity().z) * GetBodyMass();
					if (impulse > 0.0F)
					{
						ApplyImpulse(Vector3D(0.0F, 0.0F, impulse));
						SetCharacterState(kCharacterJumping);
						PurgeContacts();
					}
				}
			}

			if (fighterPlayer != TheMessageMgr->GetLocalPlayer())
			{
				SetOrientation(m->GetMovementAzimuth(), m->GetMovementAltitude());
			}

			movementFlags |= flag;
			break;
		}

		case kFighterMessageEndMovement:
		{
			const FighterMovementMessage *m = static_cast<const FighterMovementMessage *>(message);

			if (fighterPlayer != TheMessageMgr->GetLocalPlayer())
			{
				SetOrientation(m->GetMovementAzimuth(), m->GetMovementAltitude());
			}

			movementFlags &= ~m->GetMovementFlag();
			break;
		}

		case kFighterMessageChangeMovement:
		{
			const FighterMovementMessage *m = static_cast<const FighterMovementMessage *>(message);

			if (fighterPlayer != TheMessageMgr->GetLocalPlayer())
			{
				SetOrientation(m->GetMovementAzimuth(), m->GetMovementAltitude());
			}

			movementFlags = (movementFlags & ~kMovementPlanarMask) | m->GetMovementFlag();
			break;
		}

		case kFighterMessageBeginShield:
		{
			if (fighterPlayer == TheMessageMgr->GetLocalPlayer())
			{
				TheDisplayBoard->ShowShield();
			}
			else if (!shieldEffect)
			{
				Node *node = GetTargetNode()->GetFirstSubnode();
				while (node)
				{
					if (node->GetNodeType() == kNodeGeometry)
					{
						shieldEffect = new ShellEffect(static_cast<Geometry *>(node), 0.03125F, ColorRGBA(0.25F, 0.25F, 0.0F, 0.0F));
						shieldEffect->SetNodeFlags(kNodeNonpersistent | kNodeCloneInhibit | kNodeAnimateInhibit);
						node->AppendNewSubnode(shieldEffect);
						break;
					}

					node = node->Next();
				}
			}

			break;
		}

		case kFighterMessageEndShield:
		{
			if (fighterPlayer == TheMessageMgr->GetLocalPlayer())
			{
				TheDisplayBoard->HideShield();
			}
			else
			{
				delete shieldEffect;
				shieldEffect = nullptr;
			}

			break;
		}

		case kFighterMessageBeginIcon:
		{
			static const char iconTextureName[kFighterIconCount][16] =
			{
				"game/Chat"
			};

			const FighterIconMessage *m = static_cast<const FighterIconMessage *>(message);

			int32 index = m->GetIconIndex();
			if (iconIndex != index)
			{
				iconIndex = index;
				delete iconEffect;

				iconEffect = new QuadEffect(0.25F, K::white, iconTextureName[index]);
				iconEffect->SetNodePosition(Point3D(0.0F, 0.0F, 2.0F));
				iconEffect->SetNodeFlags(kNodeNonpersistent | kNodeCloneInhibit | kNodeAnimateInhibit);
				iconEffect->GetObject()->SetQuadBlendState(kBlendInterpolate);
				GetTargetNode()->AppendNewSubnode(iconEffect);
			}

			break;
		}

		case kFighterMessageEndIcon:
		{
			delete iconEffect;
			iconEffect = nullptr;
			iconIndex = -1;
			break;
		}

		case kFighterMessageTeleport:
		{
			static const char *const soundName[2] =
			{
				"sound/Teleport-A", "sound/Teleport-B"
			};

			const FighterTeleportMessage *m = static_cast<const FighterTeleportMessage *>(message);

			float azm = m->GetTeleportAzimuth();
			primaryAzimuth = azm;
			modelAzimuth = azm;
			SetOrientation(azm, m->GetTeleportAltitude());

			const Point3D& position = m->GetInitialPosition();
			SetRigidBodyTransform(Transform4D(Matrix3D().SetRotationAboutZ(azm), position));
			previousCenterOfMass = GetWorldCenterOfMass();
			SetLinearVelocity(m->GetInitialVelocity());
			PurgeContacts();

			Node *node = GetTargetNode();
			node->StopMotion();
			node->Invalidate();

			World *world = node->GetWorld();
			const Point3D& center = m->GetEffectCenter();

			ColorRGB color(2.0F, 2.0F, 2.0F);
			PointLight *light = new PointLight(color, 4.0F);
			light->GetObject()->SetLightFlags(kLightShadowInhibit);
			light->SetController(new FlashController(color, 0.75F, 300));
			light->SetNodePosition(center);
			world->AddNewNode(light);

			const char *name = soundName[Math::Random(2)];

			OmniSource *source = new OmniSource(name, 32.0F);
			source->SetSourcePriority(kSoundPriorityPlayer);
			source->SetNodePosition(center);
			world->AddNewNode(source);

			source = new OmniSource(name, 32.0F);
			source->SetSourcePriority(kSoundPriorityPlayer);
			source->SetNodePosition(position);
			world->AddNewNode(source);
			break;
		}

		case kFighterMessageLaunch:
		{
			const CharacterStateMessage *m = static_cast<const CharacterStateMessage *>(message);

			ApplyImpulse(Inverse(GetFinalTransform()) * (m->GetInitialVelocity() - GetLinearVelocity()) * GetBodyMass());
			SetCharacterState(kCharacterJumping);
			PurgeContacts();

			OmniSource *source = new OmniSource("sound/Launch", 20.0F);
			source->SetSourcePriority(kSoundPriorityPlayer);
			source->SetNodePosition(m->GetInitialPosition());
			GetTargetNode()->GetWorld()->AddNewNode(source);
			break;
		}

		case kFighterMessageLand:
		{
			Node *node = GetTargetNode();
			OmniSource *source = new OmniSource("gus/Land", 20.0F);
			source->SetSourcePriority(kSoundPriorityPlayer);
			source->SetNodePosition(node->GetWorldPosition());
			node->GetWorld()->AddNewNode(source);
			break;
		}

		case kFighterMessageUpdate:
		{
			const FighterUpdateMessage *m = static_cast<const FighterUpdateMessage *>(message);

			if (fighterPlayer != TheMessageMgr->GetLocalPlayer())
			{
				UpdateOrientation(m->GetUpdateAzimuth(), m->GetUpdateAltitude());
			}

			break;
		}

		case kFighterMessageWeapon:
		{
			const FighterWeaponMessage *m = static_cast<const FighterWeaponMessage *>(message);
			SetWeapon(m->GetWeaponIndex(), m->GetWeaponControllerIndex());

			if (fighterPlayer == TheMessageMgr->GetLocalPlayer())
			{
				TheDisplayBoard->UpdatePlayerWeapons();
			}

			break;
		}

		case kFighterMessageEmptyAmmo:
		{
			if (weaponModel)
			{
				OmniSource *source = new OmniSource("sound/Click", 20.0F);
				source->SetSourcePriority(kSoundPriorityPlayer);
				source->SetSourceVelocity(GetLinearVelocity());
				source->SetNodePosition(weaponModel->GetWorldPosition());
				GetTargetNode()->GetWorld()->AddNewNode(source);
			}

			if ((fighterPlayer == TheMessageMgr->GetLocalPlayer()) && (TheEngine->GetVariable("weaponAmmoSwitch")->GetIntegerValue() != 0))
			{
				const GamePlayer *player = static_cast<GamePlayer *>(fighterPlayer.GetTarget());
				for (machine a = kWeaponCount - 1; a > kWeaponNone; a--)
				{
					if ((player->GetWeaponFlag(a)) && ((player->GetWeaponAmmo(a, 0) != 0) || (a == kWeaponAxe)))
					{
						TheMessageMgr->SendMessage(kPlayerServer, ClientWeaponMessage(kMessageClientSwitchWeapon, a));
						break;
					}
				}
			}

			break;
		}

		case kFighterMessageDeath:
		{
			Node *node = fighterInteractor.GetInteractionNode();
			if (node)
			{
				fighterInteractor.HandleInteractionEvent(kInteractionEventDisengage, node);
				GetTargetNode()->GetWorld()->RemoveInteractor(&fighterInteractor);
			}

			EndFiring(lookAzimuth, lookAltitude);
			SetWeapon(kWeaponNone, kControllerUnassigned);
			DeactivateFlashlight();

			const Model *model = GetTargetNode();
			OmniSource *source = new OmniSource("gus/Death", 32.0F);
			source->SetSourcePriority(kSoundPriorityDeath);
			source->SetNodePosition(model->GetWorldPosition());
			model->GetWorld()->AddNewNode(source);

			if (fighterPlayer == TheMessageMgr->GetLocalPlayer())
			{
				static_cast<GameWorld *>(model->GetWorld())->SetBloodIntensity(1.0F);
			}

			SetExternalForce(Zero3D);
			SetCollisionKind(GetCollisionKind() | kCollisionCorpse);
			SetCollisionExclusionMask(~kCollisionRigidBody);

			deathTime = 10000;
			fighterFlags |= kFighterDead;
			SetFighterMotion(kFighterMotionDeath);

			SetPerspectiveExclusionMask(0);
			break;
		}

		case kFighterMessageDamage:
		{
			static const char damageName[3][32] =
			{
				"gus/Damage1", "gus/Damage2", "gus/Damage3"
			};

			const FighterDamageMessage *m = static_cast<const FighterDamageMessage *>(message);

			const Node *model = GetTargetNode();
			GameWorld *world = static_cast<GameWorld *>(model->GetWorld());

			int32 intensity = m->GetDamageIntensity();
			if (intensity > 0)
			{
				if (GetFighterPlayer() == TheMessageMgr->GetLocalPlayer())
				{
					world->SetBloodIntensity((float) intensity * 0.01F);
				}

				if (!(m->GetDamageFlags() & kDamageBloodInhibit))
				{
					BloodParticleSystem *blood = new BloodParticleSystem(ColorRGB(0.5F, 0.0F, 0.0F), intensity);
					blood->SetNodePosition(m->GetDamageCenter());
					world->AddNewNode(blood);
				}
			}

			if (DamageTimeExpired(250))
			{
				OmniSource *source = new OmniSource(damageName[Math::Random(3)], 32.0F);
				source->SetSourcePriority(kSoundPriorityPlayer);
				source->SetNodePosition(model->GetWorldPosition());
				world->AddNewNode(source);
			}

			break;
		}

		default:

			GameCharacterController::ReceiveMessage(message);
			break;
	}
}

void FighterController::SendInitialStateMessages(Player *player) const
{
	if (!(fighterFlags & kFighterDead))
	{
		if (GetFighterPlayer()->GetShieldTime() >= 0)
		{
			player->SendMessage(ControllerMessage(kFighterMessageBeginShield, GetControllerIndex()));
		}

		if (iconEffect)
		{
			player->SendMessage(FighterIconMessage(kFighterMessageBeginIcon, GetControllerIndex(), iconIndex));
		}
	}
}

void FighterController::SendSnapshot(void)
{
	GameCharacterController::SendSnapshot();
	TheMessageMgr->SendMessageClients(FighterUpdateMessage(GetControllerIndex(), lookAzimuth, lookAltitude), GetFighterPlayer());
}

void FighterController::SetMountNodeTransform(void)
{
	Point3D		position;

	Vector2D t = CosSin(lookAzimuth);
	Vector2D u = CosSin(lookAltitude);
	firingDirection.Set(t.x * u.x, t.y * u.x, u.y);

	t = CosSin(lookAzimuth - modelAzimuth);
	Vector3D view(t.x * u.x, t.y * u.x, u.y);
	Vector3D right(t.y, -t.x, 0.0F);
	Vector3D down = view % right;

	const GamePlayer *player = GetFighterPlayer();
	if ((player == TheMessageMgr->GetLocalPlayer()) && (static_cast<GameWorld *>(GetTargetNode()->GetWorld())->UsingFirstPersonCamera()))
	{
		position = right * 0.25F + down * 0.45F + view * 0.375F;
		position.z += 1.6F;
	}
	else
	{
		float d = 0.375F + lookAltitude * 0.1F;
		position = right * 0.25F + view * d;
		position.z += 1.15F;
	}

	if (weaponMountMarker)
	{
		const Point3D& markerPosition = weaponMountMarker->GetNodePosition();
		position -= view * markerPosition.x - right * markerPosition.y - down * markerPosition.z;
	}

	mountNode->SetNodeTransform(view, -right, -down, position);
}

void FighterController::UpdateWeapon(WorldObservable *observable)
{
	if (weaponModel)
	{
		const Point3D& position = weaponFireMarker->GetWorldPosition();
		Vector3D direction = GetFiringDirection();

		Point3D center = GetTargetNode()->GetWorldPosition();
		center.z += kCameraPositionHeight;

		float minDistance = (position - center) * direction;
		float distance = GetTargetDistance();
		if (distance > minDistance)
		{
			// Compute a new firing direction that will hit
			// whatever the crosshairs are aimed at.

			Vector3D newDirection = Normalize(center + direction * distance - position);

			// Only allow a deflection of 15 degrees or less.

			if (newDirection * direction > 0.966F)
			{
				direction = newDirection;
			}
		}

		WeaponResult result = GetWeaponController()->UpdateWeapon(position, direction, center);
		if (result == kWeaponFired)
		{
			fighterFlags &= ~kFighterEmptyAmmo;
		}
		else if (result == kWeaponEmpty)
		{
			// We can only get here on the server.

			unsigned_int32 flags = fighterFlags;
			if (!(flags & kFighterEmptyAmmo))
			{
				// The player ran out of ammo on this frame.

				fighterFlags = flags | kFighterEmptyAmmo;

				// Setting a deferred weapon time prevents a weapon switch from happening immediately.
				// When a client machine received the kFighterMessageEmptyAmmo message, it may decide to
				// automatically switch to the next best weapon. When the server receives a weapon switch
				// request during the deferred weapon time, it waits until that time has passed until it
				// actually switches weapons.

				GamePlayer *player = GetFighterPlayer();
				player->SetDeferredWeaponState(500, 0);
				player->SetDeferredWeapon(kWeaponNone);

				TheMessageMgr->SendMessageAll(ControllerMessage(kFighterMessageEmptyAmmo, GetControllerIndex()));
			}
		}
	}
}

void FighterController::Move(void)
{
	GameCharacterController::Move();

	bool server = TheMessageMgr->Server();
	if (!(fighterFlags & kFighterDead))
	{
		static const unsigned_int8 movementIndexTable[16] =
		{
			8, 0, 1, 8,
			2, 4, 6, 2,
			3, 5, 7, 3,
			8, 0, 1, 8
		};

		damageTime = MaxZero(damageTime - TheTimeMgr->GetDeltaTime());

		if (fighterPlayer == TheMessageMgr->GetLocalPlayer())
		{
			float dt = TheTimeMgr->GetFloatDeltaTime();

			float azm = lookAzimuth + TheInputMgr->GetMouseDeltaX() + TheGame->GetLookSpeedX() * dt;
			if (azm < -K::tau_over_2)
			{
				azm += K::tau;
			}
			else if (azm > K::tau_over_2)
			{
				azm -= K::tau;
			}

			float alt = lookAltitude + TheInputMgr->GetMouseDeltaY() + TheGame->GetLookSpeedY() * dt;
			alt = Clamp(alt, -1.5F, 1.5F);

			if ((azm != lookAzimuth) || (alt != lookAltitude))
			{
				lookAzimuth = azm;
				lookAltitude = alt;

				if (!server)
				{
					TheMessageMgr->SendMessage(kPlayerServer, ClientOrientationMessage(azm, alt));
				}
			}

			TheSoundMgr->SetListenerVelocity(GetLinearVelocity());
		}

		int32 motion = fighterMotion;
		Vector2D force(0.0F, 0.0F);
		float azimuthOffset = 0.0F;

		int32 index = movementIndexTable[movementFlags & kMovementPlanarMask];
		if (index < 8)
		{
			static const float movementDirectionTable[8] =
			{
				0.0F, 4.0F, 2.0F, -2.0F, 1.0F, -1.0F, 3.0F, -3.0F
			};

			static const float movementAzimuthTable[8] =
			{
				0.0F, 0.0F, 2.0F, -2.0F, 1.0F, -1.0F, -1.0F, 1.0F
			};

			float direction = movementDirectionTable[index] * K::tau_over_8 + lookAzimuth;
			force += CosSin(direction) * kFighterRunForce;

			primaryAzimuth = lookAzimuth;
			azimuthOffset = movementAzimuthTable[index];
			motion = ((index == 1) || (index >= 6)) ? kFighterMotionBackward : kFighterMotionForward;
		}
		else if (motion <= kFighterMotionBackward)
		{
			motion = kFighterMotionStop;
		}

		if (GetCharacterState() & kCharacterGround)
		{
			SetExternalLinearResistance(Vector2D(kFighterResistForce, kFighterResistForce));
			SetExternalForce(force);
		}
		else
		{
			SetExternalLinearResistance(Zero2D);
			SetExternalForce(force * 0.02F);
		}

		lookInterpolateParam = FmaxZero(lookInterpolateParam - TheTimeMgr->GetSystemFloatDeltaTime() * TheMessageMgr->GetSnapshotFrequency());

		float azm = primaryAzimuth;
		if ((motion <= kFighterMotionStand) || (motion == kFighterMotionTurnLeft) || (motion == kFighterMotionTurnRight))
		{
			float interpolatedAzimuth = GetInterpolatedLookAzimuth();

			float da = interpolatedAzimuth - azm;
			if (da > K::tau_over_2)
			{
				da -= K::tau;
			}
			else if (da < -K::tau_over_2)
			{
				da += K::tau;
			}

			if (da > K::tau_over_8)
			{
				if (da > K::tau_over_4)
				{
					azm = interpolatedAzimuth - K::tau_over_4;
				}

				motion = kFighterMotionTurnLeft;
			}
			else if (da < -K::tau_over_8)
			{
				if (da < -K::tau_over_4)
				{
					azm = interpolatedAzimuth + K::tau_over_4;
				}

				motion = kFighterMotionTurnRight;
			}
		}

		if (motionComplete)
		{
			if (fighterMotion == kFighterMotionTurnLeft)
			{
				azm += K::tau_over_4;
				if (azm > K::tau_over_2)
				{
					azm -= K::tau;
				}
			}
			else if (fighterMotion == kFighterMotionTurnRight)
			{
				azm -= K::tau_over_4;
				if (azm < -K::tau_over_2)
				{
					azm += K::tau;
				}
			}

			motion = kFighterMotionStand;
		}

		if (motion != fighterMotion)
		{
			SetFighterMotion(motion);
		}

		primaryAzimuth = azm;
		modelAzimuth = azimuthOffset * K::tau_over_16 + azm;

		SetCharacterOrientation(modelAzimuth);
		SetMountNodeTransform();

		if (server)
		{
			Model *model = GetTargetNode();
			model->GetWorld()->ActivateTriggers(previousCenterOfMass, GetWorldCenterOfMass(), 0.33F, model);
		}

		previousCenterOfMass = GetWorldCenterOfMass();
	}
	else
	{
		if ((server) && ((deathTime -= TheTimeMgr->GetDeltaTime()) < 0))
		{
			TheMessageMgr->SendMessageAll(DeleteNodeMessage(GetControllerIndex()));
			return;
		}

		SetExternalLinearResistance(Vector2D(kFighterResistForce, kFighterResistForce));
	}

	Animate();
}

RigidBodyStatus FighterController::HandleNewGeometryContact(const GeometryContact *contact)
{
	if (TheMessageMgr->Server())
	{
		const Geometry *geometry = contact->GetContactGeometry();

		const RemotePortal *portal = Game::DetectTeleporter(geometry);
		if (portal)
		{
			Transform4D		portalTransform;

			portal->CalculateRemoteTransform(&portalTransform);

			Vector2D t = CosSin(lookAzimuth);
			Vector2D p = CosSin(lookAltitude);
			Vector3D direction(t.x * p.x, t.y * p.x, p.y);

			direction = portalTransform * direction;
			float azm = Atan(direction.y, direction.x);
			float alt = Atan(direction.z);

			Point3D position = portalTransform * GetFinalPosition();
			Vector3D velocity = portalTransform * GetOriginalLinearVelocity();
			Point3D center = GetFinalTransform() * (contact->GetRigidBodyContactPosition() - contact->GetRigidBodyContactNormal() * 0.5F);

			TheMessageMgr->SendMessageAll(FighterTeleportMessage(GetControllerIndex(), position, velocity, azm, alt, center));
			return (kRigidBodyContactsBroken);
		}

		if (!(GetCharacterState() & kCharacterJumping))
		{
			const Property *property = geometry->GetProperty(kPropertyJump);
			if (property)
			{
				const JumpProperty *jumpProperty = static_cast<const JumpProperty *>(property);

				const Node *destination = geometry->GetConnectedNode(jumpProperty->GetDestinationConnectorKey());
				if (destination)
				{
					float g = GetPhysicsController()->GetGravityAcceleration().z;
					float vz = Sqrt(-2.0F * g * jumpProperty->GetJumpHeight());

					const Point3D& position = GetFinalPosition();
					Vector3D velocity = (destination->GetWorldPosition() - position) * (g * -0.5F / vz);
					velocity.z += vz;

					TheMessageMgr->SendMessageAll(CharacterStateMessage(kFighterMessageLaunch, GetControllerIndex(), position, velocity));
					return (kRigidBodyContactsBroken);
				}
			}
		}

		const Subcontact *subcontact = contact->GetSubcontact(0);
		if (subcontact->alphaNormal.z < -GetGroundCosine())
		{
			float m = subcontact->impactSpeed - 12.0F;
			if (TheMessageMgr->Multiplayer())
			{
				m -= 6.0F;
			}

			if (m > 0.0F)
			{
				TheMessageMgr->SendMessageAll(ControllerMessage(kFighterMessageLand, GetControllerIndex()));
				Damage((Fixed) (m * 20.0F) << 16, 0, nullptr);
			}
		}
	}

	return (kRigidBodyUnchanged);
}

void FighterController::EnterWorld(World *world, const Point3D& worldPosition)
{
	if (TheMessageMgr->Multiplayer())
	{
		Point3D position(worldPosition.x, worldPosition.y, worldPosition.z + 1.0F);

		OmniSource *source = new OmniSource("sound/Spawn", 32.0F);
		source->SetSourcePriority(kSoundPriorityPlayer);
		source->SetNodePosition(position);
		world->AddNewNode(source);

		MaterializeParticleSystem *system = new MaterializeParticleSystem(ColorRGB(0.5F, 1.0F, 0.25F), 0.5F);
		system->SetNodePosition(position);
		world->AddNewNode(system);
	}
}

CharacterStatus FighterController::Damage(Fixed damage, unsigned_int32 flags, GameCharacterController *attacker, const Point3D *position, const Vector3D *force)
{
	if (GetFighterPlayer()->Damage(damage, 0, attacker) != kCharacterUnaffected)
	{
		Point3D		defaultPosition;

		if (!position)
		{
			const Point3D& p = GetTargetNode()->GetWorldPosition();
			defaultPosition.Set(p.x, p.y, p.z + 1.5F);
			position = &defaultPosition;
		}

		TheMessageMgr->SendMessageAll(FighterDamageMessage(GetControllerIndex(), damage >> 15, flags, *position));
		return (kCharacterDamaged);
	}

	return (kCharacterUnaffected);
}

void FighterController::Kill(GameCharacterController *attacker, const Point3D *position, const Vector3D *force)
{
	GetFighterPlayer()->Kill(attacker);
}

void FighterController::UpdateOrientation(float azm, float alt)
{
	deltaLookAzimuth = GetInterpolatedLookAzimuth() - azm;
	if (deltaLookAzimuth > K::tau_over_2)
	{
		deltaLookAzimuth -= K::tau;
	}
	else if (deltaLookAzimuth < -K::tau_over_2)
	{
		deltaLookAzimuth += K::tau;
	}

	deltaLookAltitude = GetInterpolatedLookAltitude() - alt;

	lookAzimuth = azm;
	lookAltitude = alt;
	lookInterpolateParam = 1.0F;
}

void FighterController::BeginMovement(unsigned_int32 flag, float azm, float alt)
{
	const Point3D& position = GetTargetNode()->GetWorldPosition();
	Vector3D velocity = GetLinearVelocity();

	FighterMovementMessage message(kFighterMessageBeginMovement, GetControllerIndex(), position, velocity, azm, alt, flag);
	TheMessageMgr->SendMessageAll(message);
}

void FighterController::EndMovement(unsigned_int32 flag, float azm, float alt)
{
	const Point3D& position = GetTargetNode()->GetWorldPosition();
	Vector3D velocity = GetLinearVelocity();

	FighterMovementMessage message(kFighterMessageEndMovement, GetControllerIndex(), position, velocity, azm, alt, flag);
	TheMessageMgr->SendMessageAll(message);
}

void FighterController::ChangeMovement(unsigned_int32 flags, float azm, float alt)
{
	const Point3D& position = GetTargetNode()->GetWorldPosition();
	Vector3D velocity = GetLinearVelocity();

	FighterMovementMessage message(kFighterMessageChangeMovement, GetControllerIndex(), position, velocity, azm, alt, flags);
	TheMessageMgr->SendMessageAll(message);
}

void FighterController::BeginFiring(bool primary, float azm, float alt)
{
	SetOrientation(azm, alt);

	unsigned_int32 flags = fighterFlags & ~kFighterFiring;
	fighterFlags = flags | ((primary) ? kFighterFiringPrimary : kFighterFiringSecondary);

	GetWeaponController()->BeginFiring(primary);
}

void FighterController::EndFiring(float azm, float alt)
{
	SetOrientation(azm, alt);
	fighterFlags &= ~kFighterFiring;

	GetWeaponController()->EndFiring();
}

void FighterController::SetWeapon(int32 weaponIndex, int32 weaponControllerIndex)
{
	GamePlayer *player = GetFighterPlayer();

	if ((!weaponModel) || (player->GetCurrentWeapon() != weaponIndex))
	{
		bool soundFlag = false;
		int32 fireDelay = 0;

		if (weaponModel)
		{
			soundFlag = true;
			fireDelay = static_cast<WeaponController *>(weaponModel->GetController())->GetFireDelayTime();

			delete weaponModel;
			weaponModel = nullptr;
		}

		weaponMountMarker = nullptr;
		weaponIKAnimator[0]->SetIKTarget(nullptr);
		weaponIKAnimator[1]->SetIKTarget(nullptr);

		const Weapon *weapon = Weapon::Get(weaponIndex);
		if (weapon)
		{
			weaponModel = Model::Get(weapon->GetModelType());
			WeaponController *weaponController = weapon->NewWeaponController(this);
			weaponController->SetControllerIndex(weaponControllerIndex);
			weaponController->SetFireDelayTime(fireDelay);
			weaponModel->SetController(weaponController);
			mountNode->AppendSubnode(weaponModel);

			if (soundFlag)
			{
				// Play the weaponing switching sound, but only if there was a previous weapon
				// and enough time has passed since the last switch.

				unsigned_int32 time = TheTimeMgr->GetSystemAbsoluteTime();
				if ((int32) (time - weaponSwitchTime) >= 250)
				{
					weaponSwitchTime = time;

					OmniSource *source = new OmniSource("sound/Weapon", 20.0F);
					source->SetSourcePriority(kSoundPriorityPlayer);
					mountNode->AppendNewSubnode(source);
				}
			}

			Node *node = weaponModel->GetFirstSubnode();
			while (node)
			{
				NodeType type = node->GetNodeType();
				if (type == kNodeGeometry)
				{
					Geometry *geometry = static_cast<Geometry *>(node);
					if (geometry->GetGeometryType() == kGeometryGeneric)
					{
						GenericGeometry *genericGeometry = static_cast<GenericGeometry *>(geometry);
						genericGeometry->SetRenderableFlags(genericGeometry->GetRenderableFlags() | kRenderableMotionBlurGradient);
					}
				}
				else if (type == kNodeMarker)
				{
					Marker *marker = static_cast<Marker *>(node);
					if (marker->GetMarkerType() == kMarkerLocator)
					{
						LocatorType locatorType = static_cast<LocatorMarker *>(node)->GetLocatorType();
						if (locatorType == 'fire')
						{
							weaponFireMarker = marker;
						}
						else if (locatorType == 'mont')
						{
							weaponMountMarker = marker;
						}
						else if (locatorType == 'left')
						{
							weaponIKAnimator[0]->SetIKTarget(marker);
						}
						else if (locatorType == 'rght')
						{
							weaponIKAnimator[1]->SetIKTarget(marker);
						}
					}
				}

				// If there are any controllers inside the weapon model, we don't want them to be assigned controller
				// indexes on the client that could conflict with server-assigned indexes for other controllers.

				Controller *controller = node->GetController();
				if (controller)
				{
					controller->SetControllerFlags(controller->GetControllerFlags() | kControllerLocal);
				}

				node = weaponModel->GetNextNode(node);
			}

			weaponModel->Preprocess();
			player->SetCurrentWeapon(weaponIndex);
			SetMountNodeTransform();

			unsigned_int32 flags = fighterFlags & ~kFighterEmptyAmmo;
			fighterFlags = flags;

			if (flags & kFighterFiring)
			{
				weaponController->BeginFiring((flags & kFighterFiringPrimary) != 0);
			}
		}
	}
}

void FighterController::ActivateFlashlight(void)
{
	if (!flashlight)
	{
		flashlight = new SpotLight(ColorRGB(2.0F, 2.0F, 2.0F), 20.0F, 2.0F, "game/FlashLight");
		flashlight->SetNodeMatrix3D(K::minus_y_unit, K::minus_z_unit, K::x_unit);
		flashlight->SetNodePosition(Point3D(0.25F, 0.0F, 0.0F));
		flashlight->SetExcludedNode(GetTargetNode());
		mountNode->AppendNewSubnode(flashlight);

		OmniSource *source = new OmniSource("sound/Flashlight", 16.0F);
		source->SetSourcePriority(kSoundPriorityPlayer);
		source->SetSourceVolume(0.5F);
		mountNode->AppendNewSubnode(source);
	}
}

void FighterController::DeactivateFlashlight(void)
{
	if (flashlight)
	{
		delete flashlight;
		flashlight = nullptr;

		OmniSource *source = new OmniSource("sound/Flashlight", 16.0F);
		source->SetSourcePriority(kSoundPriorityPlayer);
		source->SetSourceVolume(0.5F);
		mountNode->AppendNewSubnode(source);
	}
}

void FighterController::ToggleFlashlight(void)
{
	if (!flashlight)
	{
		ActivateFlashlight();
	}
	else
	{
		DeactivateFlashlight();
	}
}

void FighterController::SetPerspectiveExclusionMask(unsigned_int32 mask) const
{
	Node *node = GetTargetNode()->GetFirstSubnode();
	while (node)
	{
		if (node != mountNode)
		{
			Node *subnode = node;
			do
			{
				if (subnode->GetNodeType() == kNodeGeometry)
				{
					Geometry *geometry = static_cast<Geometry *>(subnode);
					geometry->SetPerspectiveExclusionMask(mask);

					if (mask == 0)
					{
						geometry->SetMinDetailLevel(0);
					}
					else
					{
						geometry->SetMinDetailLevel(geometry->GetObject()->GetGeometryLevelCount() - 1);
					}
				}

				subnode = node->GetNextNode(subnode);
			} while (subnode);
		}

		node = node->Next();
	}
}

void FighterController::SetFighterStyle(const int32 *style, bool prep)
{
}

void FighterController::SetFighterMotion(int32 motion)
{
	fighterMotion = motion;
	motionComplete = false;
}

void FighterController::HandleAnimationEvent(FrameAnimator *animator, CueType cueType)
{
	switch (cueType)
	{
		case 'STEP':
		{
			const char		*name;

			if (!GetSubmergedWaterBlock())
			{
				const SubstanceData *substanceData = GetGroundSubstanceData();
				if (!substanceData)
				{
					break;
				}

				name = substanceData->footstepSoundName[Math::Random(4)];
				if (name[0] == 0)
				{
					break;
				}
			}
			else
			{
				if (!(GetCharacterState() & kCharacterGround))
				{
					break;
				}

				static const char waterFootstepSoundName[4][SubstanceData::kMaxStringSize] =
				{
					"sound/step/Water1", "sound/step/Water2", "sound/step/Water3", "sound/step/Water4"
				};

				name = waterFootstepSoundName[Math::Random(4)];
			}

			OmniSource *source = new OmniSource(name, 20.0F);
			source->SetSourcePriority(kSoundPriorityPlayer);

			Model *model = GetTargetNode();
			source->SetNodePosition(model->GetNodePosition());
			model->GetSuperNode()->AppendNewSubnode(source);

			source->SetSourceVolume(0.125F);
			source->SetSourceFrequency(Math::RandomFloat(0.5F) + 0.75F);
			break;
		}
	}
}

void FighterController::MotionComplete(Interpolator *interpolator, void *cookie)
{
	static_cast<FighterController *>(cookie)->motionComplete = true;
}

void FighterController::Animate(void)
{
	GetTargetNode()->Animate();
}


CreateFighterMessage::CreateFighterMessage(ModelMessageType type) : CreateModelMessage(type)
{
}

CreateFighterMessage::CreateFighterMessage(ModelMessageType type, int32 fighterIndex, const Point3D& position, float azm, float alt, unsigned_int32 movement, int32 weapon, int32 weaponController, int32 key) : CreateModelMessage(type, fighterIndex, position)
{
	initialAzimuth = azm;
	initialAltitude = alt;

	movementFlags = movement;

	weaponIndex = weapon;
	weaponControllerIndex = weaponController;

	playerKey = key;
}

CreateFighterMessage::~CreateFighterMessage()
{
}

void CreateFighterMessage::Compress(Compressor& data) const
{
	CreateModelMessage::Compress(data);

	data << initialAzimuth;
	data << initialAltitude;

	data << (unsigned_int8) movementFlags;

	data << (unsigned_int8) weaponIndex;
	data << (unsigned_int16) weaponControllerIndex;

	data << (int16) playerKey;
}

bool CreateFighterMessage::Decompress(Decompressor& data)
{
	if (CreateModelMessage::Decompress(data))
	{
		int16			key;
		unsigned_int8	movement;
		unsigned_int8	weapon;
		unsigned_int16	weaponController;

		data >> initialAzimuth;
		data >> initialAltitude;

		data >> movement;
		movementFlags = movement;

		data >> weapon;
		weaponIndex = weapon;

		data >> weaponController;
		weaponControllerIndex = weaponController;

		data >> key;
		playerKey = key;

		return (true);
	}

	return (false);
}


FighterInteractionMessage::FighterInteractionMessage(ControllerMessageType type, int32 controllerIndex) : ControllerMessage(type, controllerIndex)
{
}

FighterInteractionMessage::FighterInteractionMessage(ControllerMessageType type, int32 controllerIndex, int32 interactionIndex) : ControllerMessage(type, controllerIndex)
{
	interactionControllerIndex = interactionIndex;
}

FighterInteractionMessage::~FighterInteractionMessage()
{
}

void FighterInteractionMessage::Compress(Compressor& data) const
{
	ControllerMessage::Compress(data);

	data << (unsigned_int16) interactionControllerIndex;
}

bool FighterInteractionMessage::Decompress(Decompressor& data)
{
	if (ControllerMessage::Decompress(data))
	{
		unsigned_int16	index;

		data >> index;
		interactionControllerIndex = index;

		return (true);
	}

	return (false);
}


FighterBeginInteractionMessage::FighterBeginInteractionMessage(int32 controllerIndex) : FighterInteractionMessage(FighterController::kFighterMessageEngageInteraction, controllerIndex)
{
}

FighterBeginInteractionMessage::FighterBeginInteractionMessage(int32 controllerIndex, int32 interactionIndex, const Point3D& position) : FighterInteractionMessage(FighterController::kFighterMessageEngageInteraction, controllerIndex, interactionIndex)
{
	interactionPosition = position;
}

FighterBeginInteractionMessage::~FighterBeginInteractionMessage()
{
}

void FighterBeginInteractionMessage::Compress(Compressor& data) const
{
	FighterInteractionMessage::Compress(data);

	data << interactionPosition;
}

bool FighterBeginInteractionMessage::Decompress(Decompressor& data)
{
	if (FighterInteractionMessage::Decompress(data))
	{
		data >> interactionPosition;
		return (true);
	}

	return (false);
}


FighterMovementMessage::FighterMovementMessage(ControllerMessageType type, int32 controllerIndex) : CharacterStateMessage(type, controllerIndex)
{
}

FighterMovementMessage::FighterMovementMessage(ControllerMessageType type, int32 controllerIndex, const Point3D& position, const Vector3D& velocity, float azimuth, float altitude, unsigned_int32 flag) : CharacterStateMessage(type, controllerIndex, position, velocity)
{
	movementAzimuth = azimuth;
	movementAltitude = altitude;
	movementFlag = flag;
}

FighterMovementMessage::~FighterMovementMessage()
{
}

void FighterMovementMessage::Compress(Compressor& data) const
{
	CharacterStateMessage::Compress(data);

	data << movementAzimuth;
	data << movementAltitude;

	data << (unsigned_int8) movementFlag;
}

bool FighterMovementMessage::Decompress(Decompressor& data)
{
	if (CharacterStateMessage::Decompress(data))
	{
		unsigned_int8	flag;

		data >> movementAzimuth;
		data >> movementAltitude;

		data >> flag;
		movementFlag = flag;

		return (true);
	}

	return (false);
}


FighterIconMessage::FighterIconMessage(ControllerMessageType type, int32 controllerIndex) : ControllerMessage(type, controllerIndex)
{
}

FighterIconMessage::FighterIconMessage(ControllerMessageType type, int32 controllerIndex, int32 icon) : ControllerMessage(type, controllerIndex)
{
	iconIndex = icon;
}

FighterIconMessage::~FighterIconMessage()
{
}

void FighterIconMessage::Compress(Compressor& data) const
{
	ControllerMessage::Compress(data);

	data << (unsigned_int8) iconIndex;
}

bool FighterIconMessage::Decompress(Decompressor& data)
{
	if (ControllerMessage::Decompress(data))
	{
		unsigned_int8	icon;

		data >> icon;
		iconIndex = icon;
		return (icon < kFighterIconCount);
	}

	return (false);
}


FighterTeleportMessage::FighterTeleportMessage(int32 controllerIndex) : CharacterStateMessage(FighterController::kFighterMessageTeleport, controllerIndex)
{
}

FighterTeleportMessage::FighterTeleportMessage(int32 controllerIndex, const Point3D& position, const Vector3D& velocity, float azimuth, float altitude, const Point3D& center) : CharacterStateMessage(FighterController::kFighterMessageTeleport, controllerIndex, position, velocity)
{
	teleportAzimuth = azimuth;
	teleportAltitude = altitude;

	effectCenter = center;
}

FighterTeleportMessage::~FighterTeleportMessage()
{
}

void FighterTeleportMessage::Compress(Compressor& data) const
{
	CharacterStateMessage::Compress(data);

	data << teleportAzimuth;
	data << teleportAltitude;
	data << effectCenter;
}

bool FighterTeleportMessage::Decompress(Decompressor& data)
{
	if (CharacterStateMessage::Decompress(data))
	{
		data >> teleportAzimuth;
		data >> teleportAltitude;
		data >> effectCenter;
		return (true);
	}

	return (false);
}


FighterUpdateMessage::FighterUpdateMessage(int32 controllerIndex) : ControllerMessage(FighterController::kFighterMessageUpdate, controllerIndex)
{
}

FighterUpdateMessage::FighterUpdateMessage(int32 controllerIndex, float azimuth, float altitude) : ControllerMessage(FighterController::kFighterMessageUpdate, controllerIndex)
{
	updateAzimuth = azimuth;
	updateAltitude = altitude;

	SetMessageFlags(kMessageUnreliable);
}

FighterUpdateMessage::~FighterUpdateMessage()
{
}

void FighterUpdateMessage::Compress(Compressor& data) const
{
	ControllerMessage::Compress(data);

	data << updateAzimuth;
	data << updateAltitude;
}

bool FighterUpdateMessage::Decompress(Decompressor& data)
{
	if (ControllerMessage::Decompress(data))
	{
		data >> updateAzimuth;
		data >> updateAltitude;
		return (true);
	}

	return (false);
}


FighterWeaponMessage::FighterWeaponMessage(int32 controllerIndex) : ControllerMessage(FighterController::kFighterMessageWeapon, controllerIndex)
{
}

FighterWeaponMessage::FighterWeaponMessage(int32 controllerIndex, int32 weapon, int32 weaponController) : ControllerMessage(FighterController::kFighterMessageWeapon, controllerIndex)
{
	weaponIndex = weapon;
	weaponControllerIndex = weaponController;
}

FighterWeaponMessage::~FighterWeaponMessage()
{
}

void FighterWeaponMessage::Compress(Compressor& data) const
{
	ControllerMessage::Compress(data);

	data << (unsigned_int8) weaponIndex;
	data << (unsigned_int16) weaponControllerIndex;
}

bool FighterWeaponMessage::Decompress(Decompressor& data)
{
	if (ControllerMessage::Decompress(data))
	{
		unsigned_int8	weapon;
		unsigned_int16	weaponController;

		data >> weapon;
		weaponIndex = weapon;

		data >> weaponController;
		weaponControllerIndex = weaponController;

		return (weapon < kWeaponCount);
	}

	return (false);
}


FighterDamageMessage::FighterDamageMessage(int32 controllerIndex) : ControllerMessage(FighterController::kFighterMessageDamage, controllerIndex)
{
}

FighterDamageMessage::FighterDamageMessage(int32 controllerIndex, int32 intensity, unsigned_int32 flags, const Point3D& center) : ControllerMessage(FighterController::kFighterMessageDamage, controllerIndex)
{
	damageIntensity = intensity;
	damageFlags = flags;
	damageCenter = center;
}

FighterDamageMessage::~FighterDamageMessage()
{
}

void FighterDamageMessage::Compress(Compressor& data) const
{
	ControllerMessage::Compress(data);

	data << (unsigned_int16) damageIntensity;
	data << (unsigned_int16) damageFlags;
	data << damageCenter;
}

bool FighterDamageMessage::Decompress(Decompressor& data)
{
	if (ControllerMessage::Decompress(data))
	{
		unsigned_int16	intensity, flags;

		data >> intensity;
		damageIntensity = intensity;

		data >> flags;
		damageFlags = flags;

		data >> damageCenter;
		return (true);
	}

	return (false);
}


SpineTwistAnimator::SpineTwistAnimator() : Animator(kAnimatorSpineTwist)
{
}

SpineTwistAnimator::SpineTwistAnimator(Model *model, Node *node) : Animator(kAnimatorSpineTwist, model, node)
{
}

SpineTwistAnimator::~SpineTwistAnimator()
{
}

void SpineTwistAnimator::Preprocess(void)
{
	Animator::Preprocess();

	const Animator *animator = GetSuperNode()->GetFirstSubnode();
	const Node *node = GetTargetNode()->GetSuperNode();
	superNodeTransformIndex[0] = animator->GetNodeTransformIndex(node);
	superNodeTransformIndex[1] = animator->GetNodeTransformIndex(node->GetSuperNode());

	spineRotation = 1.0F;
}

void SpineTwistAnimator::Configure(void)
{
	AllocateStorage(GetAnimatorTransformNodeStart(), 3, 3);

	AnimatorTransform **outputTable = GetOutputTransformTable();
	AnimatorTransform *transformTable = GetAnimatorTransformTable();
	for (machine a = 0; a < 3; a++)
	{
		outputTable[a] = &transformTable[a];
	}
}

void SpineTwistAnimator::Move(void)
{
	const Animator *animator = GetSuperNode()->GetFirstSubnode();
	int32 spineIndex = GetOutputTransformNodeStart() - animator->GetOutputTransformNodeStart();

	const AnimatorTransform *table = animator->GetAnimatorTransformTable();
	const AnimatorTransform *transform1 = &table[superNodeTransformIndex[0]];
	const AnimatorTransform *transform2 = &table[superNodeTransformIndex[1]];

	Quaternion superRotation = transform2->rotation * transform1->rotation;

	AnimatorTransform *transformTable = GetAnimatorTransformTable();
	for (machine a = 0; a < 3; a++)
	{
		Quaternion q = Conjugate(superRotation) * spineRotation * superRotation;

		const AnimatorTransform *transform = &table[spineIndex + a];
		superRotation = superRotation * transform->rotation;

		transformTable[a].rotation = q * transform->rotation;
		transformTable[a].position = transform->position;
	}
}


ScaleAnimator::ScaleAnimator() : Animator(kAnimatorScale)
{
}

ScaleAnimator::ScaleAnimator(Model *model, Node *node) : Animator(kAnimatorScale, model, node)
{
	scale = 1.0F;
}

ScaleAnimator::~ScaleAnimator()
{
}

void ScaleAnimator::Configure(void)
{
	AllocateStorage(GetAnimatorTransformNodeStart(), 1, 1);

	AnimatorTransform **outputTable = GetOutputTransformTable();
	AnimatorTransform *transformTable = GetAnimatorTransformTable();
	outputTable[0] = &transformTable[0];
}

void ScaleAnimator::Move(void)
{
	const Animator *animator = GetSuperNode()->GetFirstSubnode();
	int32 targetIndex = GetOutputTransformNodeStart() - animator->GetOutputTransformNodeStart();

	const AnimatorTransform *previousTable = animator->GetAnimatorTransformTable();
	AnimatorTransform *transformTable = GetAnimatorTransformTable();
	transformTable[0].rotation = previousTable[targetIndex].rotation * scale;
	transformTable[0].position = previousTable[targetIndex].position * scale;
}


WeaponIKAnimator::WeaponIKAnimator() : Animator(kAnimatorWeaponIK)
{
}

WeaponIKAnimator::WeaponIKAnimator(Model *model, Node *node, LocatorType type) : Animator(kAnimatorWeaponIK, model, node)
{
	locatorType = type;
	effectorNode = nullptr;
	ikTargetNode = nullptr;
}

WeaponIKAnimator::~WeaponIKAnimator()
{
}

void WeaponIKAnimator::Preprocess(void)
{
	Animator::Preprocess();

	const Model *model = GetTargetModel();
	const Node *root = GetTargetNode();

	rootAnimationIndex = model->GetTransformAnimationIndex(root);
	middleAnimationIndex = model->GetTransformAnimationIndex(root->GetFirstSubnode());

	const Node *node = root;
	for (machine a = 0; a < kMaxSuperNodeCount; a++)
	{
		node = node->GetSuperNode();
		if (node == model)
		{
			break;
		}

		superAnimationIndex[a] = model->GetTransformAnimationIndex(node);
	}

	const Node *effector = root->GetFirstSubnode();
	if (effector)
	{
		effector = effector->GetFirstSubnode();
		while (effector)
		{
			if (effector->GetNodeType() == kNodeMarker)
			{
				effectorNode = effector;
				effectorDistance = Magnitude(effector->GetNodePosition());
				break;
			}

			effector = effector->Next();
		}
	}
}

void WeaponIKAnimator::Configure(void)
{
	AllocateStorage(GetAnimatorTransformNodeStart(), 2, 2);

	AnimatorTransform **outputTable = GetOutputTransformTable();
	AnimatorTransform *transformTable = GetAnimatorTransformTable();
	for (machine a = 0; a < 2; a++)
	{
		outputTable[a] = &transformTable[a];
	}
}

void WeaponIKAnimator::Move(void)
{
	const Animator *animator = GetSuperNode()->GetFirstSubnode();
	const AnimatorTransform *const *table = animator->GetOutputTransformTable();

	const Node *effector = effectorNode;
	const Node *target = ikTargetNode;
	if ((effector) && (target))
	{
		Quaternion	q1, q2;

		const Node *model = GetTargetModel();

		// Calculate the transform of the IK target node in model-space coordinates.

		Transform4D targetTransform = target->GetNodeTransform();
		const Node *super = target->GetSuperNode();
		while (super != model)
		{
			targetTransform = super->GetNodeTransform() * targetTransform;
			super = super->GetSuperNode();
		}

		// Calculate the transform of the animator root node in model-space coordinates.

		const Node *root = GetTargetNode();
		const AnimatorTransform *transform1 = table[rootAnimationIndex];
		Transform4D rootTransform(transform1->rotation.GetRotationMatrix(), transform1->position);

		int32 superCount = 0;
		super = root->GetSuperNode();
		while (super != model)
		{
			const AnimatorTransform *transform = table[superAnimationIndex[superCount++]];
			rootTransform = Transform4D(transform->rotation.GetRotationMatrix(), transform->position) * rootTransform;
			super = super->GetSuperNode();
		}

		// Move the target transform into animator space (the coordinate space of the root node).

		targetTransform = Inverse(rootTransform) * targetTransform;

		// Get the secondary bone and its transform in animator-space coordinates.

		const AnimatorTransform *transform2 = table[middleAnimationIndex];
		Transform4D middleTransform(transform2->rotation.GetRotationMatrix(), transform2->position);

		// Transform the effector position into animator space.

		Point3D effectorPosition = middleTransform * effector->GetNodePosition();

		// Get the distances for the IK problem.

		const Point3D& m = middleTransform.GetTranslation();
		float x = Magnitude(m);
		float y = effectorDistance;

		Point3D targetPosition = targetTransform.GetTranslation();
		float distance = Magnitude(targetPosition);

		if ((x > distance - y) && (x < distance + y))
		{
			// The IK problem can be solved, so do it.

			float inverseDistance = 1.0F / distance;
			Vector3D targetDirection = targetPosition * inverseDistance;

			float u = (x * x - y * y + distance * distance) * (inverseDistance * 0.5F);
			float z = Sqrt(x * x - u * u);

			Point3D c = targetPosition * (u * inverseDistance);
			Vector3D v = m - c;
			v -= ProjectOnto(v, targetDirection);
			Point3D r = c + v * (z * InverseMag(v));

			Vector3D axis1 = Normalize(m % r);
			float angle1 = m * r * InverseMag(m) * InverseMag(r);
			float ct = Sqrt(0.5F + angle1 * 0.5F);
			float st = Sqrt(0.5F - angle1 * 0.5F);
			q1.Set(axis1 * st, ct);

			Vector3D p = (q1 * effectorPosition * Conjugate(q1)).GetVector3D() - r;
			Vector3D t = targetPosition - r;

			Vector3D axis2 = Normalize(p % t);
			float angle2 = p * t * (InverseMag(p) * InverseMag(t));
			ct = Sqrt(0.5F + angle2 * 0.5F);
			st = Sqrt(0.5F - angle2 * 0.5F);
			q2.Set(axis2 * st, ct);
		}
		else
		{
			// The IK problem is impossible, either because the target is out of reach
			// or the target is too close to the root position. In this case, make the
			// bones point straight at the target and get the effector as close as possible.

			Point3D c = targetPosition * (x / distance);

			Vector3D axis1 = Normalize(m % c);
			float angle1 = m * c * InverseMag(m) * InverseMag(c);
			float ct = Sqrt(0.5F + angle1 * 0.5F);
			float st = Sqrt(0.5F - angle1 * 0.5F);
			q1.Set(axis1 * st, ct);

			Vector3D p = (q1 * effectorPosition * Conjugate(q1)).GetVector3D() - c;
			Vector3D t = targetPosition - c;

			Vector3D axis2 = Normalize(p % t);
			float angle2 = p * t * (InverseMag(p) * InverseMag(t));
			ct = Sqrt(0.5F + angle2 * 0.5F);
			st = Sqrt(0.5F - angle2 * 0.5F);
			q2.Set(axis2 * st, ct);
		}

		AnimatorTransform *transformTable = GetAnimatorTransformTable();
		transformTable[0].rotation = transform1->rotation * q1;
		transformTable[0].position = transform1->position;
		transformTable[1].rotation = q2 * transform2->rotation;
		transformTable[1].position = transform2->position;
	}
	else
	{
		// Pass through transforms from previous animator.

		const AnimatorTransform *transform1 = table[rootAnimationIndex];
		const AnimatorTransform *transform2 = table[middleAnimationIndex];

		AnimatorTransform *transformTable = GetAnimatorTransformTable();
		transformTable[0].rotation = transform1->rotation;
		transformTable[0].position = transform1->position;
		transformTable[1].rotation = transform2->rotation;
		transformTable[1].position = transform2->position;
	}
}

// ZYUQURM
