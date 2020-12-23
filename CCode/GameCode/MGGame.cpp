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


#include "C4ToolWindows.h"
#include "C4AudioCapture.h"
#include "C4Water.h"
#include "MGGame.h"
#include "MGSoldier.h"
#include "MGGusGraves.h"
#include "MGAxe.h"
#include "MGPistol.h"
#include "MGShotgun.h"
#include "MGCrossbow.h"
#include "MGSpikeShooter.h"
#include "MGGrenadeLauncher.h"
#include "MGQuantumCharger.h"
#include "MGRocketLauncher.h"
#include "MGPlasmaGun.h"
#include "MGProtonCannon.h"
#include "MGCatFlinger.h"
#include "MGChickenBlaster.h"
#include "MGBeerCannon.h"
#include "MGHornetGun.h"
#include "MGPowers.h"
#include "MGTreasure.h"
#include "MGSpider.h"
#include "MGBat.h"
#include "MGSkeleton.h"
#include "MGHauntedArm.h"
#include "MGGoblin.h"
#include "MGZombie.h"
#include "MGWitch.h"
#include "MGGolem.h"
#include "MGPumpkinhead.h"
#include "MGGrimReaper.h"
#include "MGFish.h"
#include "MGChicken.h"
#include "MGFireball.h"
#include "MGBlackCat.h"


using namespace C4;


namespace
{
	const float kInteractionDistance = 2.0F;	// This is how close the camera needs to be to an interactive object in order to use it.
}


Game *C4::TheGame = nullptr;


GameWorld::GameWorld(const char *name) :
		World(name),
		spectatorCamera(TheGame->GetCameraFocalLength(), 1.0F, 0.3F),
		rumbleShaker(0.25F)
{
	rumbleShaker.SetCompletionProc(&ShakeComplete, this);
}

GameWorld::~GameWorld()
{
	FrustumCamera *camera = rumbleShaker.GetTargetCamera();
	if (camera)
	{
		camera->SetShaker(nullptr);
	}
}

WorldResult GameWorld::Preprocess(void)
{
	WorldResult result = World::Preprocess();
	if (result != kWorldOkay)
	{
		return (result);
	}

	SetCamera(&spectatorCamera);
	playerCamera = &firstPersonCamera;

	spawnLocatorCount = 0;
	CollectZoneMarkers(GetRootNode());

	const Marker *marker = GetFirstSpectatorLocator();
	if (marker)
	{
		const Vector3D direction = marker->GetWorldTransform()[0];
		float azimuth = Atan(direction.y, direction.x);
		float altitude = Atan(direction.z, Sqrt(direction.x * direction.x + direction.y * direction.y));

		spectatorCamera.SetCameraAzimuth(azimuth);
		spectatorCamera.SetCameraAltitude(altitude);
		spectatorCamera.SetNodePosition(marker->GetWorldPosition());
	}
	else
	{
		spectatorCamera.SetNodePosition(Point3D(0.0F, 0.0F, 1.0F));
	}

	bloodIntensity = 0.0F;
	return (kWorldOkay);
}
 
void GameWorld::CollectZoneMarkers(Zone *zone)
{
	Marker *marker = zone->GetFirstMarker(); 
	while (marker)
	{ 
		Marker *next = marker->Next();

		if ((marker->GetMarkerType() == kMarkerLocator) && (marker->Enabled())) 
		{
			LocatorMarker *locator = static_cast<LocatorMarker *>(marker); 
			switch (locator->GetLocatorType()) 
			{
				case kLocatorSpawn:

					spawnLocatorCount++; 
					spawnLocatorList.Append(locator);
					break;

				case kLocatorSpectator:

					spectatorLocatorList.Append(locator);
					break;

				case kLocatorTeleport:

					teleportLocatorList.Append(locator);
					break;

				case kLocatorBenchmark:

					benchmarkLocatorList.Append(locator);
					break;
			}
		}

		marker = next;
	}

	Zone *subzone = zone->GetFirstSubzone();
	while (subzone)
	{
		CollectZoneMarkers(subzone);
		subzone = subzone->Next();
	}
}

RigidBodyStatus GameWorld::HandleNewRigidBodyContact(RigidBodyController *rigidBody, const RigidBodyContact *contact, RigidBodyController *contactBody)
{
	return (kRigidBodyUnchanged);
}

RigidBodyStatus GameWorld::HandleNewGeometryContact(RigidBodyController *rigidBody, const GeometryContact *contact)
{
	if (SquaredMag(rigidBody->GetLinearVelocity()) > 10.0F)
	{
		const Node *node = rigidBody->GetTargetNode();

		const Property *property = node->GetSharedProperty(kPropertyImpactSound);
		if (property)
		{
			bool hardSound = true;
			const MaterialObject *material = contact->GetContactGeometry()->GetTriangleMaterial(contact->GetSubcontact(0)->triangleIndex);
			if (material)
			{
				SubstanceType substanceType = material->GetMaterialSubstance();
				if (substanceType != kSubstanceNone)
				{
					const GameSubstance *substance = static_cast<const GameSubstance *>(MaterialObject::FindRegisteredSubstance(substanceType));
					if (substance)
					{
						const SubstanceData *substanceData = substance->GetSubstanceData();
						if (substanceData->substanceFlags & kSubstanceSoft)
						{
							hardSound = false;
						}
					}
				}
			}

			const ImpactSoundProperty *impactSoundProperty = static_cast<const ImpactSoundProperty *>(property);
			const ResourceName& name = (hardSound) ? impactSoundProperty->GetHardImpactSoundName() : impactSoundProperty->GetSoftImpactSoundName();

			OmniSource *source = new OmniSource(name, 48.0F);
			source->SetNodePosition(rigidBody->GetFinalPosition());
			node->GetWorld()->AddNewNode(source);
		}

		property = node->GetSharedProperty(kPropertyPaintCan);
		if ((property) && (Math::Random(2) == 0))
		{
			const GameWorld *world = static_cast<GameWorld *>(node->GetWorld());

			int32 i = Math::Random(4);
			MarkingData markingData(Vector2D(0.5F, 0.5F), Vector2D((i & 1) ? 0.5F : 0.0F, (i & 2) ? 0.5F : 0.0F), kMarkingLight | kMarkingDepthWrite | kMarkingForceClip);
			AutoReleaseMaterial material(kMaterialSplatter);

			markingData.center = contact->GetContactGeometry()->GetWorldTransform() * contact->GetGeometryContactPosition();
			markingData.normal = -(rigidBody->GetFinalTransform() * contact->GetRigidBodyContactNormal());
			markingData.tangent = Math::CreatePerpendicular(markingData.normal);
			markingData.tangent.RotateAboutAxis(Math::RandomFloat(K::tau), markingData.normal);
			markingData.radius = Math::RandomFloat(0.25F) + 0.25F;
			markingData.materialObject = material;
			markingData.color = static_cast<const PaintCanProperty *>(property)->GetPaintColor();
			markingData.lifeTime = 30000;
			MarkingEffect::New(world, &markingData);
		}
	}

	return (kRigidBodyUnchanged);
}

void GameWorld::HandleWaterSubmergence(RigidBodyController *rigidBody)
{
	const WaterBlock *waterBlock = rigidBody->GetSubmergedWaterBlock();

	const Transform4D& transform = waterBlock->GetInverseWorldTransform();
	Point3D position = transform * rigidBody->GetWorldCenterOfMass();

	const Vector3D& velocity = rigidBody->GetLinearVelocity();
	float delta = (transform.GetRow(2) ^ velocity) * (rigidBody->GetBodyMass() * 0.015625F);

	float radius = waterBlock->GetObject()->GetWaterFieldScale() * 1.5F;

	PointWaveGenerator *generator = new PointWaveGenerator(position.GetPoint2D(), radius, delta, 2);
	generator->SetWaveGeneratorFlags(generator->GetWaveGeneratorFlags() | kWaveGeneratorNonpersistent);
	static_cast<WaterController *>(waterBlock->GetController())->AddWaveGenerator(generator);
}

void GameWorld::Interact(void)
{
	bool server = TheMessageMgr->Server();
	const Player *localPlayer = TheMessageMgr->GetLocalPlayer();

	const Player *player = TheMessageMgr->GetFirstPlayer();
	while (player)
	{
		FighterController *controller = static_cast<const GamePlayer *>(player)->GetPlayerController();
		if (controller)
		{
			if ((server) || (player == localPlayer) || (controller->GetFighterFlags() & kFighterTargetDistance))
			{
				CollisionData	data;

				const Point3D& p = controller->GetTargetNode()->GetWorldPosition();
				Point3D position(p.x, p.y, p.z + kCameraPositionHeight);

				const Vector3D& direction = controller->GetFiringDirection();
				controller->GetFighterInteractor()->SetInteractionProbe(position, position + direction * kInteractionDistance);

				Player *sightPlayer = nullptr;
				float targetDistance = 0.0F;

				// Fire a 100-meter ray into the world in the direction that the player is looking.

				CollisionState state = QueryCollision(position, position + direction * 100.0F, 0.0F, kCollisionSightPath, &data);
				if (state != kCollisionStateNone)
				{
					if (state == kCollisionStateGeometry)
					{
						// The ray hit a geometry node. Set the target distance to be the distance to the geometry.

						targetDistance = data.param * 100.0F;
					}
					else if (state == kCollisionStateRigidBody)
					{
						// The ray hit a rigid body. Set the target distance to be the distance to the rigid body.

						targetDistance = data.param * 100.0F;

						const RigidBodyController *rigidBody = data.rigidBody;
						if (rigidBody->GetControllerType() == kControllerGusGraves)
						{
							// If the node is another player, then remember who it is so we can display the name.

							sightPlayer = static_cast<const FighterController *>(rigidBody)->GetFighterPlayer();
						}
					}
				}

				controller->SetTargetDistance(targetDistance);

				if ((player == localPlayer) && (TheMessageMgr->Multiplayer()))
				{
					TheNamePopupBoard->SetPlayer(sightPlayer);
				}
			}
		}

		player = player->Next();
	}

	World::Interact();
}

void GameWorld::BeginRendering(void)
{
	// If you want to set a post-processing color matrix,
	// make the call to SetFinalColorTransform() here before
	// calling World::BeginRendering().

	float blood = FmaxZero(bloodIntensity - TheTimeMgr->GetFloatDeltaTime() * 0.002F);
	bloodIntensity = blood;

	SetFinalColorTransform(ColorRGBA(1.0F, 1.0F, 1.0F, 1.0F), ColorRGBA(blood, 0.0F, 0.0F, 0.0F));

	World::BeginRendering();
}

void GameWorld::EndRendering(void)
{
	World::EndRendering();

	const Player *player = TheMessageMgr->GetLocalPlayer();
	if (player)
	{
		const FighterController *controller = static_cast<const GamePlayer *>(player)->GetPlayerController();
		if (controller)
		{
			const Node *node = controller->GetFighterInteractor()->GetInteractionNode();
			if ((!node) || (node->GetNodeType() != kNodeEffect))
			{
				TheGame->GetCrosshairs()->Render();
			}
		}
	}
}

void GameWorld::SetCameraTargetModel(Model *model)
{
	firstPersonCamera.SetTargetModel(model);
	chaseCamera.SetTargetModel(model);
	SetCamera(playerCamera);

	FrustumCamera *camera = rumbleShaker.GetTargetCamera();
	if (camera)
	{
		camera->SetShaker(nullptr);
		rumbleShaker.Reset();
	}
}

void GameWorld::SetSpectatorCamera(const Point3D& position, float azm, float alt)
{
	firstPersonCamera.SetTargetModel(nullptr);
	chaseCamera.SetTargetModel(nullptr);

	FrustumCamera *camera = rumbleShaker.GetTargetCamera();
	if (camera)
	{
		camera->SetShaker(nullptr);
		rumbleShaker.Reset();
	}

	SetCamera(&spectatorCamera);
	spectatorCamera.SetNodePosition(position);
	spectatorCamera.SetCameraAzimuth(azm);
	spectatorCamera.SetCameraAltitude(alt);

	spectatorCamera.Update();
}

void GameWorld::SetLocalPlayerVisibility(void)
{
	const Player *player = TheMessageMgr->GetLocalPlayer();
	if (player)
	{
		const FighterController *controller = static_cast<const GamePlayer *>(player)->GetPlayerController();
		if (controller)
		{
			unsigned_int32 mask = (playerCamera == &firstPersonCamera) ? kPerspectivePrimary | kPerspectiveReflection | kPerspectiveRefraction | (kPerspectiveReflection << kPerspectiveDirectShadowShift) : 0;
			controller->SetPerspectiveExclusionMask(mask);
		}
	}
}

void GameWorld::ChangePlayerCamera(void)
{
	const Player *player = TheMessageMgr->GetLocalPlayer();
	if ((player) && (static_cast<const GamePlayer *>(player)->GetPlayerController()))
	{
		if (playerCamera == &firstPersonCamera)
		{
			playerCamera = &chaseCamera;
		}
		else
		{
			playerCamera = &firstPersonCamera;
		}

		FrustumCamera *camera = rumbleShaker.GetTargetCamera();
		if (camera)
		{
			camera->SetShaker(nullptr);
			playerCamera->SetShaker(&rumbleShaker);
		}

		SetCamera(playerCamera);
		SetLocalPlayerVisibility();
	}
}

void GameWorld::SetFocalLength(float focal)
{
	spectatorCamera.GetObject()->SetFocalLength(focal);
	firstPersonCamera.GetObject()->SetFocalLength(focal);
	chaseCamera.GetObject()->SetFocalLength(focal);
}

void GameWorld::ShakeCamera(float intensity, int32 duration)
{
	if (!playerCamera->GetShaker())
	{
		playerCamera->SetShaker(&rumbleShaker);
	}

	rumbleShaker.Rumble(intensity, duration);
}

void GameWorld::ShakeCamera(const Point3D& position, float intensity, int32 duration)
{
	float atten = Fmin(1.0F / Magnitude(position - playerCamera->GetNodePosition()) - 0.0625F, 1.0F);
	if (atten > 0.0F)
	{
		if (!playerCamera->GetShaker())
		{
			playerCamera->SetShaker(&rumbleShaker);
		}

		rumbleShaker.Rumble(intensity * atten, duration);
	}
}

void GameWorld::ShakeComplete(RumbleShaker *shaker, void *cookie)
{
	GameWorld *gameWorld = static_cast<GameWorld *>(cookie);
	gameWorld->rumbleShaker.GetTargetCamera()->SetShaker(nullptr);
}

ProximityResult GameWorld::RecordSplashDamage(Node *node, const Point3D& center, float radius, void *cookie)
{
	Controller *controller = node->GetController();
	if ((controller) && (controller->GetBaseControllerType() == kControllerRigidBody))
	{
		SplashDamageData *splashDamageData = static_cast<SplashDamageData *>(cookie);

		RigidBodyController *rigidBody = static_cast<RigidBodyController *>(controller);
		RigidBodyType type = rigidBody->GetRigidBodyType();

		if (type == kRigidBodyCharacter)
		{
			if (!(rigidBody->GetCollisionKind() & kCollisionCorpse))
			{
				splashDamageData->characterArray.AddElement(static_cast<GameCharacterController *>(rigidBody));
			}
		}
		else if (type == kRigidBodyGame)
		{
			splashDamageData->rigidBodyArray.AddElement(static_cast<GameRigidBodyController *>(rigidBody));
		}

		return (kProximitySkipSuccessors);
	}

	return (kProximityContinue);
}

void GameWorld::ProduceSplashDamage(const Point3D& center, float radius, int32 damage, GameCharacterController *attacker, const GameRigidBodyController *excludedBody) const
{
	SplashDamageData	splashDamageData;

	QueryProximity(center, radius, &RecordSplashDamage, &splashDamageData);

	float floatDamage = (float) (damage << 16);
	for (const Link<GameCharacterController>& characterLink : splashDamageData.characterArray)
	{
		GameCharacterController *character = characterLink;
		if (character)
		{
			const Point3D& position = character->GetTargetNode()->GetWorldPosition();
			Vector3D direction = Point3D(position.x, position.y, position.z + 1.0F) - center;

			float d2 = SquaredMag(direction);
			float r2 = radius * radius;
			if (d2 < r2)
			{
				Fixed fixedDamage = (int32) (floatDamage * (1.0F - d2 / r2));
				if (fixedDamage > 0)
				{
					Vector3D force = direction * ((float) fixedDamage * InverseMag(direction) * 2.0e-5F);

					character->ResetStandingTime();
					character->Damage(fixedDamage, 0, attacker, nullptr, &force);
				}
			}
		}
	}

	for (const Link<GameRigidBodyController>& rigidBodyLink : splashDamageData.rigidBodyArray)
	{
		GameRigidBodyController *rigidBody = rigidBodyLink;
		if ((rigidBody) && (rigidBody != excludedBody))
		{
			rigidBody->HandleSplashDamage(center, radius, floatDamage, attacker);
		}
	}
}


Game::Game() :
		Singleton<Game>(TheGame),
		stringTable("game/Game"),

		controllerCreator(&CreateController),
		forceCreator(&CreateForce),
		effectCreator(&CreateEffect),
		particleSystemCreator(&CreateParticleSystem),

		displayEventHandler(&HandleDisplayEvent),

		gameRigidBodyControllerReg(kControllerGameRigidBody, stringTable.GetString(StringID('CTRL', kControllerGameRigidBody))),
		explosiveControllerReg(kControllerExplosive, stringTable.GetString(StringID('CTRL', kControllerExplosive))),
		crumbleControllerReg(kControllerCrumble, stringTable.GetString(StringID('CTRL', kControllerCrumble))),
		secretScriptControllerReg(kControllerSecretScript, stringTable.GetString(StringID('CTRL', kControllerSecretScript))),
		collectableControllerReg(kControllerCollectable, stringTable.GetString(StringID('CTRL', kControllerCollectable))),
		windControllerReg(kControllerWind, stringTable.GetString(StringID('CTRL', kControllerWind))),
		lightningControllerReg(kControllerLightning, stringTable.GetString(StringID('CTRL', kControllerLightning))),

		playerStatePropertyReg(kPropertyPlayerState, nullptr),
		collectablePropertyReg(kPropertyCollectable, stringTable.GetString(StringID('PROP', kPropertyCollectable))),
		containerPropertyReg(kPropertyContainer, stringTable.GetString(StringID('PROP', kPropertyContainer))),
		shootablePropertyReg(kPropertyShootable, stringTable.GetString(StringID('PROP', kPropertyShootable))),
		breakablePropertyReg(kPropertyBreakable, stringTable.GetString(StringID('PROP', kPropertyBreakable))),
		leakingGooPropertyReg(kPropertyLeakingGoo, stringTable.GetString(StringID('PROP', kPropertyLeakingGoo))),
		impactSoundPropertyReg(kPropertyImpactSound, stringTable.GetString(StringID('PROP', kPropertyImpactSound))),
		paintCanPropertyReg(kPropertyPaintCan, stringTable.GetString(StringID('PROP', kPropertyPaintCan))),
		teleporterPropertyReg(kPropertyTeleporter, stringTable.GetString(StringID('PROP', kPropertyTeleporter))),
		jumpPropertyReg(kPropertyJump, stringTable.GetString(StringID('PROP', kPropertyJump))),

		displayMessageReg(kMethodDisplayMessage, stringTable.GetString(StringID('MTHD', kMethodDisplayMessage)), kMethodNoTarget),
		displayActionReg(kMethodDisplayAction, stringTable.GetString(StringID('MTHD', kMethodDisplayAction)), kMethodNoTarget),
		activateFlashlightReg(kMethodActivateFlashlight, stringTable.GetString(StringID('MTHD', kMethodActivateFlashlight))),
		deactivateFlashlightReg(kMethodDeactivateFlashlight, stringTable.GetString(StringID('MTHD', kMethodDeactivateFlashlight))),
		getFlashlightStateReg(kMethodGetFlashlightState, stringTable.GetString(StringID('MTHD', kMethodGetFlashlightState)), kMethodNoMessage | kMethodOutputValue),
		giveWeaponReg(kMethodGiveWeapon, stringTable.GetString(StringID('MTHD', kMethodGiveWeapon))),
		giveAmmoReg(kMethodGiveAmmo, stringTable.GetString(StringID('MTHD', kMethodGiveAmmo))),
		givePowerReg(kMethodGivePower, stringTable.GetString(StringID('MTHD', kMethodGivePower))),
		giveTreasureReg(kMethodGiveTreasure, stringTable.GetString(StringID('MTHD', kMethodGiveTreasure))),
		giveHealthReg(kMethodGiveHealth, stringTable.GetString(StringID('MTHD', kMethodGiveHealth)), kMethodOutputValue),
		giveScoreReg(kMethodGiveScore, stringTable.GetString(StringID('MTHD', kMethodGiveScore))),
		damageCharacterReg(kMethodDamageCharacter, stringTable.GetString(StringID('MTHD', kMethodDamageCharacter))),
		killCharacterReg(kMethodKillCharacter, stringTable.GetString(StringID('MTHD', kMethodKillCharacter))),
		killAllMonstersReg(kMethodKillAllMonsters, stringTable.GetString(StringID('MTHD', kMethodKillAllMonsters)), kMethodNoTarget),
		teleportPlayerReg(kMethodTeleportPlayer, stringTable.GetString(StringID('MTHD', kMethodTeleportPlayer))),
		shakeCameraReg(kMethodShakeCamera, stringTable.GetString(StringID('MTHD', kMethodShakeCamera)), kMethodNoTarget),
		endWorldReg(kMethodEndWorld, stringTable.GetString(StringID('MTHD', kMethodEndWorld)), kMethodNoTarget),

		rainReg(kParticleSystemRain, stringTable.GetString(StringID('PART', kParticleSystemRain))),
		burningRainReg(kParticleSystemBurningRain, stringTable.GetString(StringID('PART', kParticleSystemBurningRain))),
		groundFogReg(kParticleSystemGroundFog, stringTable.GetString(StringID('PART', kParticleSystemGroundFog))),
		torchSmokeReg(kParticleSystemTorchSmoke, stringTable.GetString(StringID('PART', kParticleSystemTorchSmoke))),
		chimneySmokeReg(kParticleSystemChimneySmoke, stringTable.GetString(StringID('PART', kParticleSystemChimneySmoke))),
		heatWavesReg(kParticleSystemHeatWaves, stringTable.GetString(StringID('PART', kParticleSystemHeatWaves))),
		bubblingSteamReg(kParticleSystemBubblingSteam, stringTable.GetString(StringID('PART', kParticleSystemBubblingSteam))),
		flowingGooReg(kParticleSystemFlowingGoo, stringTable.GetString(StringID('PART', kParticleSystemFlowingGoo))),

		appleModelReg(kModelAppleHealth, stringTable.GetString(StringID('MODL', 'HLTH', kModelAppleHealth)), "health/Apple", 0, kControllerCollectable),
		bananaModelReg(kModelBananaHealth, stringTable.GetString(StringID('MODL', 'HLTH', kModelBananaHealth)), "health/Banana", 0, kControllerCollectable),
		grapesModelReg(kModelGrapesHealth, stringTable.GetString(StringID('MODL', 'HLTH', kModelGrapesHealth)), "health/Grapes", 0, kControllerCollectable),
		orangeModelReg(kModelOrangeHealth, stringTable.GetString(StringID('MODL', 'HLTH', kModelOrangeHealth)), "health/Orange", 0, kControllerCollectable),
		burgerModelReg(kModelBurgerHealth, stringTable.GetString(StringID('MODL', 'HLTH', kModelBurgerHealth)), "health/Burger", 0, kControllerCollectable),
		pizzaModelReg(kModelPizzaHealth, stringTable.GetString(StringID('MODL', 'HLTH', kModelPizzaHealth)), "health/Pizza", 0, kControllerCollectable),
		subModelReg(kModelSubHealth, stringTable.GetString(StringID('MODL', 'HLTH', kModelSubHealth)), "health/Sub", 0, kControllerCollectable),
		tacoModelReg(kModelTacoHealth, stringTable.GetString(StringID('MODL', 'HLTH', kModelTacoHealth)), "health/Taco", 0, kControllerCollectable),
		hamModelReg(kModelHamHealth, stringTable.GetString(StringID('MODL', 'HLTH', kModelHamHealth)), "health/Ham", 0, kControllerCollectable),
		roastModelReg(kModelRoastHealth, stringTable.GetString(StringID('MODL', 'HLTH', kModelRoastHealth)), "health/Roast", 0, kControllerCollectable),
		spaghettiModelReg(kModelSpaghettiHealth, stringTable.GetString(StringID('MODL', 'HLTH', kModelSpaghettiHealth)), "health/Spaghetti", 0, kControllerCollectable),
		turkeyModelReg(kModelTurkeyHealth, stringTable.GetString(StringID('MODL', 'HLTH', kModelTurkeyHealth)), "health/Turkey", 0, kControllerCollectable),
		gobletModelReg(kModelGobletHealth, stringTable.GetString(StringID('MODL', 'HLTH', kModelGobletHealth)), "health/Goblet", 0, kControllerCollectable),

		fireballModelReg(kModelFireball, nullptr, "fireball/Fireball", kModelPrecache | kModelPrivate),
		blackCatModelReg(kModelBlackCat, nullptr, "cat/cat/Cat", kModelPrecache | kModelPrivate),

		spawnLocatorReg(kLocatorSpawn, stringTable.GetString(StringID('LOCA', kLocatorSpawn))),
		spectatorLocatorReg(kLocatorSpectator, stringTable.GetString(StringID('LOCA', kLocatorSpectator))),
		teleportLocatorReg(kLocatorTeleport, stringTable.GetString(StringID('LOCA', kLocatorTeleport))),
		benchmarkLocatorReg(kLocatorBenchmark, stringTable.GetString(StringID('LOCA', kLocatorBenchmark))),
		centerLocatorReg(kLocatorCenter, stringTable.GetString(StringID('LOCA', kLocatorCenter))),
		fireLocatorReg(kLocatorFire, stringTable.GetString(StringID('LOCA', kLocatorFire))),

		looksensObserver(this, &Game::HandleLooksensEvent),
		invertXObserver(this, &Game::HandleInvertXEvent),
		invertYObserver(this, &Game::HandleInvertYEvent),
		fovObserver(this, &Game::HandleFovEvent),
		effectsVolumeObserver(this, &Game::HandleEffectsVolumeEvent),
		musicVolumeObserver(this, &Game::HandleMusicVolumeEvent),
		voiceVolumeObserver(this, &Game::HandleVoiceVolumeEvent),
		voiceReceiveObserver(this, &Game::HandleVoiceReceiveEvent),
		voiceSendObserver(this, &Game::HandleVoiceSendEvent),

		crossTypeObserver(&crosshairs, &Crosshairs::HandleCrossTypeEvent),
		crossColorObserver(&crosshairs, &Crosshairs::HandleCrossColorEvent),
		crossSizeObserver(&crosshairs, &Crosshairs::HandleCrossSizeEvent),

		hostCommandObserver(this, &Game::HandleHostCommand),
		joinCommandObserver(this, &Game::HandleJoinCommand),
		saveCommandObserver(this, &Game::HandleSaveCommand),
		restoreCommandObserver(this, &Game::HandleRestoreCommand),

		hostCommand("host", &hostCommandObserver),
		joinCommand("join", &joinCommandObserver),
		saveCommand("save", &saveCommandObserver),
		restoreCommand("restore", &restoreCommandObserver),

		#if MGCHEATS

			teleportCommandObserver(this, &Game::HandleTeleportCommand),
			benchCommandObserver(this, &Game::HandleBenchCommand),
			giveCommandObserver(this, &Game::HandleGiveCommand),
			godCommandObserver(this, &Game::HandleGodCommand),

			teleportCommand("tp", &teleportCommandObserver),
			benchCommand("bench", &benchCommandObserver),
			giveCommand("give", &giveCommandObserver),
			godCommand("god", &godCommandObserver),

		#endif

		effectsSoundGroup(kSoundGroupEffects, "Effects"),
		musicSoundGroup(kSoundGroupMusic, "Music"),
		voiceSoundGroup(kSoundGroupVoice, "Voice"),

		forwardAction(kActionForward, kMovementForward, kSpectatorMoveForward),
		backwardAction(kActionBackward, kMovementBackward, kSpectatorMoveBackward),
		leftAction(kActionLeft, kMovementLeft, kSpectatorMoveLeft),
		rightAction(kActionRight, kMovementRight, kSpectatorMoveRight),
		upAction(kActionUp, kMovementUp, kSpectatorMoveUp),
		downAction(kActionDown, kMovementDown, kSpectatorMoveDown),
		movementAction(kActionMovement, 0, 0),
		horizontalAction(kActionHorizontal),
		verticalAction(kActionVertical),
		primaryFireAction(kActionFirePrimary),
		secondaryFireAction(kActionFireSecondary),
		specialWeaponAction(kActionSpecialWeapon),
		nextWeaponAction(kActionNextWeapon),
		prevWeaponAction(kActionPrevWeapon),
		flashlightAction(kActionFlashlight),
		cameraViewAction(kActionCameraView),
		scoreboardAction(kActionScoreboard),
		loadAction(kActionLoad),
		saveAction(kActionSave)
{
	TheDisplayMgr->InstallDisplayEventHandler(&displayEventHandler);

	TheEngine->AddCommand(&hostCommand);
	TheEngine->AddCommand(&joinCommand);
	TheEngine->AddCommand(&saveCommand);
	TheEngine->AddCommand(&restoreCommand);

	#if MGCHEATS

		TheEngine->AddCommand(&teleportCommand);
		TheEngine->AddCommand(&benchCommand);
		TheEngine->AddCommand(&giveCommand);
		TheEngine->AddCommand(&godCommand);

	#endif

	TheSoundMgr->RegisterSoundGroup(&effectsSoundGroup);
	TheSoundMgr->RegisterSoundGroup(&musicSoundGroup);
	TheSoundMgr->SetDefaultSoundGroup(&effectsSoundGroup);

	TheInputMgr->AddAction(&forwardAction);
	TheInputMgr->AddAction(&backwardAction);
	TheInputMgr->AddAction(&leftAction);
	TheInputMgr->AddAction(&rightAction);
	TheInputMgr->AddAction(&upAction);
	TheInputMgr->AddAction(&downAction);
	TheInputMgr->AddAction(&movementAction);
	TheInputMgr->AddAction(&horizontalAction);
	TheInputMgr->AddAction(&verticalAction);
	TheInputMgr->AddAction(&useObjectAction);
	TheInputMgr->AddAction(&primaryFireAction);
	TheInputMgr->AddAction(&secondaryFireAction);
	TheInputMgr->AddAction(&specialWeaponAction);
	TheInputMgr->AddAction(&nextWeaponAction);
	TheInputMgr->AddAction(&prevWeaponAction);
	TheInputMgr->AddAction(&flashlightAction);
	TheInputMgr->AddAction(&cameraViewAction);
	TheInputMgr->AddAction(&scoreboardAction);
	TheInputMgr->AddAction(&loadAction);
	TheInputMgr->AddAction(&saveAction);
	TheInputMgr->AddAction(&chatAction);

	prevEscapeProc = TheInputMgr->GetEscapeProc();
	prevEscapeCookie = TheInputMgr->GetEscapeCookie();
	TheInputMgr->SetEscapeProc(&EscapeProc, this);

	TheWorldMgr->SetWorldCreator(&CreateWorld);
	TheMessageMgr->SetPlayerCreator(&CreatePlayer);

	Controller::InstallCreator(&controllerCreator);
	Force::InstallCreator(&forceCreator);
	Effect::InstallCreator(&effectCreator);
	ParticleSystem::InstallCreator(&particleSystemCreator);

	GameSubstance::RegisterSubstances();
	GameSubstance::RegisterMaterials();
	Weapon::PreloadTextures();

	GameRigidBodyController::RegisterFunctions(&gameRigidBodyControllerReg);
	ExplosiveController::RegisterFunctions(&explosiveControllerReg);
	CrumbleController::RegisterFunctions(&crumbleControllerReg);

	GusGraves::Construct();
	Soldier::Construct();

	Axe::Construct();
	Pistol::Construct();
	Shotgun::Construct();
	Crossbow::Construct();
	SpikeShooter::Construct();
	GrenadeLauncher::Construct();
	QuantumCharger::Construct();
	RocketLauncher::Construct();
	PlasmaGun::Construct();
	ProtonCannon::Construct();
	CatFlinger::Construct();
	ChickenBlaster::Construct();
	BeerCannon::Construct();
	HornetGun::Construct();

	CandyPower::Construct();
	ClockPower::Construct();
	FrogPower::Construct();
	DoubleAxePower::Construct();
	SkullSwordPower::Construct();
	PotionPower::Construct();

	GoldCoinTreasure::Construct();
	SilverCoinTreasure::Construct();
	BronzeCoinTreasure::Construct();
	CandyCornTreasure::Construct();
	GoldSkullTreasure::Construct();
	RedJewelTreasure::Construct();
	PurpleJewelTreasure::Construct();
	GreenJewelTreasure::Construct();
	YellowJewelTreasure::Construct();
	CrystalTreasure::Construct();

	Spider::Construct();
	Bat::Construct();
	Skeleton::Construct();
	HauntedArm::Construct();
	Goblin::Construct();
	Zombie::Construct();
	Witch::Construct();
	Golem::Construct();
	Pumpkinhead::Construct();
	GrimReaper::Construct();

	Fish::Construct();
	Chicken::Construct();

	gameFlags = 0;
	inputFlags = 0;
	multiplayerFlags = 0;
	gameDetailLevel = 0;

	titleMusic = nullptr;
	currentWorldName[0] = 0;

	TheEngine->InitVariable("looksens", "25", kVariablePermanent, &looksensObserver);
	TheEngine->InitVariable("invertX", "0", kVariablePermanent, &invertXObserver);
	TheEngine->InitVariable("invertY", "0", kVariablePermanent, &invertYObserver);

	fovVariable = TheEngine->InitVariable("fov", "65", kVariablePermanent);
	fovVariable->AddObserver(&fovObserver);

	TheEngine->InitVariable("playerEmissionColor", "2", kVariablePermanent);
	TheEngine->InitVariable("playerArmorColor", "0", kVariablePermanent);
	TheEngine->InitVariable("playerArmorTexture", "0", kVariablePermanent);
	TheEngine->InitVariable("playerHelmetType", "0", kVariablePermanent);

	TheEngine->InitVariable("weaponSwitch", "1", kVariablePermanent);
	TheEngine->InitVariable("weaponBetterSwitch", "1", kVariablePermanent);
	TheEngine->InitVariable("weaponAmmoSwitch", "1", kVariablePermanent);
	TheEngine->InitVariable("chatOpen", "1", kVariablePermanent);

	TheEngine->InitVariable("crossType", "2", kVariablePermanent, &crossTypeObserver);
	TheEngine->InitVariable("crossColor", "FFFFFF", kVariablePermanent, &crossColorObserver);
	crossSizeVariable = TheEngine->InitVariable("crossSize", "1", kVariablePermanent, &crossSizeObserver);

	effectsVolumeVariable = TheEngine->InitVariable("volumeEffects", "100", kVariablePermanent, &effectsVolumeObserver);
	musicVolumeVariable = TheEngine->InitVariable("volumeMusic", "100", kVariablePermanent, &musicVolumeObserver);
	voiceVolumeVariable = TheEngine->InitVariable("volumeVoice", "100", kVariablePermanent, &voiceVolumeObserver);

	voiceReceiveVariable = TheEngine->InitVariable("voiceReceive", "0", kVariablePermanent);
	voiceReceiveVariable->AddObserver(&voiceReceiveObserver);

	voiceSendVariable = TheEngine->InitVariable("voiceSend", "0", kVariablePermanent);
	voiceSendVariable->AddObserver(&voiceSendObserver);

	ChatWindow::New();
	MessageBoard::New();
	UsePopupBoard::New();
	NamePopupBoard::New();
	SecretPopupBoard::New();
	ActionBoard::New();

	LogoWindow *logoWindow = new LogoWindow;
	TheInterfaceMgr->AddWidget(logoWindow);
	logoWindow->SetCompletionProc(&LogoCompleteProc);

	TheInterfaceMgr->SetInputManagementMode(kInputManagementAutomatic);
}

Game::~Game()
{
	ExitCurrentGame();

	if (titleMusic)
	{
		titleMusic->Release();
	}

	boardList.Purge();
	windowList.Purge();

	Chicken::Destruct();
	Fish::Destruct();

	GrimReaper::Destruct();
	Pumpkinhead::Destruct();
	Golem::Destruct();
	Witch::Destruct();
	Zombie::Destruct();
	Goblin::Destruct();
	HauntedArm::Destruct();
	Skeleton::Destruct();
	Bat::Destruct();
	Spider::Destruct();

	CrystalTreasure::Destruct();
	YellowJewelTreasure::Destruct();
	GreenJewelTreasure::Destruct();
	PurpleJewelTreasure::Destruct();
	RedJewelTreasure::Destruct();
	GoldSkullTreasure::Destruct();
	CandyCornTreasure::Destruct();
	BronzeCoinTreasure::Destruct();
	SilverCoinTreasure::Destruct();
	GoldCoinTreasure::Destruct();

	PotionPower::Destruct();
	SkullSwordPower::Destruct();
	DoubleAxePower::Destruct();
	FrogPower::Destruct();
	ClockPower::Destruct();
	CandyPower::Destruct();

	HornetGun::Destruct();
	BeerCannon::Destruct();
	ChickenBlaster::Destruct();
	CatFlinger::Destruct();
	ProtonCannon::Destruct();
	PlasmaGun::Destruct();
	RocketLauncher::Destruct();
	QuantumCharger::Destruct();
	GrenadeLauncher::Destruct();
	SpikeShooter::Destruct();
	Crossbow::Destruct();
	Shotgun::Destruct();
	Pistol::Destruct();
	Axe::Destruct();

	Soldier::Destruct();
	GusGraves::Destruct();

	Weapon::ReleaseTextures();
	GameSubstance::UnregisterMaterials();
	GameSubstance::UnregisterSubstances();

	TheWorldMgr->SetWorldCreator(nullptr);
	TheMessageMgr->SetPlayerCreator(nullptr);

	TheInputMgr->SetEscapeProc(prevEscapeProc, prevEscapeCookie);
}

World *Game::CreateWorld(const char *name, void *cookie)
{
	return (new GameWorld(name));
}

Player *Game::CreatePlayer(PlayerKey key, void *cookie)
{
	return (new GamePlayer(key));
}

Controller *Game::CreateController(Unpacker& data, unsigned_int32 unpackFlags)
{
	switch (data.GetType())
	{
		case kControllerFireball:

			return (new FireballController);

		case kControllerTarball:

			return (new TarballController);

		case kControllerLavaball:

			return (new LavaballController);

		case kControllerVenom:

			return (new VenomController);

		case kControllerBlackCat:

			return (new BlackCatController);

		case kControllerExplosion:

			return (new ExplosionController);

		case kControllerShell:

			return (new ShellController);

		case kControllerRemains:

			return (new RemainsController);
	}

	return (nullptr);
}

Force *Game::CreateForce(Unpacker& data, unsigned_int32 unpackFlags)
{
	ForceType type = data.GetType();

	if (type == kForceExplosion)
	{
		return (new ExplosionForce);
	}

	return (nullptr);
}

Effect *Game::CreateEffect(Unpacker& data, unsigned_int32 unpackFlags)
{
	EffectType type = data.GetType();

	if (type == kEffectShell)
	{
		return (new ShellEffect);
	}

	return (nullptr);
}

ParticleSystem *Game::CreateParticleSystem(Unpacker& data, unsigned_int32 unpackFlags)
{
	switch (data.GetType())
	{
		case kParticleSystemSparks:

			return (new SparksParticleSystem);

		case kParticleSystemDust:

			return (new DustParticleSystem);

		case kParticleSystemBlood:

			return (new BloodParticleSystem);

		case kParticleSystemSmokeTrail:

			return (new SmokeTrailParticleSystem);

		case kParticleSystemBroomSmoke:

			return (new BroomSmokeParticleSystem);

		case kParticleSystemLeakingGoo:

			return (new LeakingGooParticleSystem);

		case kParticleSystemGooSplash:

			return (new GooSplashParticleSystem);

		case kParticleSystemBurstingGoo:

			return (new BurstingGooParticleSystem);

		case kParticleSystemSpiderGuts:

			return (new SpiderGutsParticleSystem);

		case kParticleSystemBurningFlames:

			return (new BurningFlamesParticleSystem);

		case kParticleSystemArrowExplosion:

			return (new ArrowExplosionParticleSystem);

		case kParticleSystemArrowSparks:

			return (new ArrowSparksParticleSystem);

		case kParticleSystemQuantumLoop:

			return (new QuantumLoopParticleSystem);

		case kParticleSystemQuantumBolt:

			return (new QuantumBoltParticleSystem);

		case kParticleSystemQuantumSpider:

			return (new QuantumSpiderParticleSystem);

		case kParticleSystemQuantumWaves:

			return (new QuantumWavesParticleSystem);

		case kParticleSystemChargeExplosion:

			return (new ChargeExplosionParticleSystem);

		case kParticleSystemGrenadeExplosion:

			return (new GrenadeExplosionParticleSystem);

		case kParticleSystemGrenadeSparks:

			return (new GrenadeSparksParticleSystem);

		case kParticleSystemRocketExplosion:

			return (new RocketExplosionParticleSystem);

		case kParticleSystemRocketSparks:

			return (new RocketSparksParticleSystem);

		case kParticleSystemPlasmaExplosion:

			return (new PlasmaExplosionParticleSystem);

		case kParticleSystemFireballExplosion:

			return (new FireballExplosionParticleSystem);

		case kParticleSystemFireballSparks:

			return (new FireballSparksParticleSystem);

		case kParticleSystemTarballTrail:

			return (new TarballTrailParticleSystem);

		case kParticleSystemTarballExplosion:

			return (new TarballExplosionParticleSystem);
	}

	return (nullptr);
}

void Game::HandleDisplayEvent(const DisplayEventData *eventData, void *cookie)
{
	if (eventData->eventType == kEventDisplayChange)
	{
		// The display resolution has changed, so we need to reposition
		// the various interface items on the screen.

		TheChatWindow->UpdateDisplayPosition();
		TheMessageBoard->UpdateDisplayPosition();
		TheUsePopupBoard->UpdateDisplayPosition();
		TheNamePopupBoard->UpdateDisplayPosition();
		TheSecretPopupBoard->UpdateDisplayPosition();
		TheActionBoard->UpdateDisplayPosition();

		if (TheDisplayBoard)
		{
			TheDisplayBoard->UpdateDisplayPosition();
		}
	}
}

void Game::LogoCompleteProc(LogoWindow *window, void *cookie)
{
	MainWindow::Open();
}

void Game::EscapeProc(void *cookie)
{
	MainWindow::Open();
}

void Game::HandleLooksensEvent(Variable *variable)
{
	int32 sensitivity = Min(Max(variable->GetIntegerValue(), 1), 100);
	lookSensitivity = sensitivity;

	unsigned_int32 flags = inputFlags;
	float m = (float) sensitivity * -2.0e-4F;
	lookMultiplierX = (flags & kInputXInverted) ? -m : m;
	lookMultiplierY = (flags & kInputYInverted) ? -m : m;
}

void Game::HandleInvertXEvent(Variable *variable)
{
	unsigned_int32 flags = inputFlags;
	float m = Fabs(lookMultiplierX);

	if (variable->GetIntegerValue() != 0)
	{
		flags |= kInputXInverted;
	}
	else
	{
		flags &= ~kInputXInverted;
		m = -m;
	}

	inputFlags = flags;
	lookMultiplierX = m;
}

void Game::HandleInvertYEvent(Variable *variable)
{
	unsigned_int32 flags = inputFlags;
	float m = Fabs(lookMultiplierY);

	if (variable->GetIntegerValue() != 0)
	{
		flags |= kInputYInverted;
	}
	else
	{
		flags &= ~kInputYInverted;
		m = -m;
	}

	inputFlags = flags;
	lookMultiplierY = m;
}

void Game::HandleFovEvent(Variable *variable)
{
	GameWorld *world = static_cast<GameWorld *>(TheWorldMgr->GetWorld());
	if (world)
	{
		world->SetFocalLength(TheGame->GetCameraFocalLength());
	}

	crosshairs.HandleCrossSizeEvent(crossSizeVariable);
}

void Game::HandleEffectsVolumeEvent(Variable *variable)
{
	effectsSoundGroup.SetVolume((float) variable->GetIntegerValue() * 0.01F);
}

void Game::HandleMusicVolumeEvent(Variable *variable)
{
	musicSoundGroup.SetVolume((float) variable->GetIntegerValue() * 0.01F);
}

void Game::HandleVoiceVolumeEvent(Variable *variable)
{
	voiceSoundGroup.SetVolume((float) variable->GetIntegerValue() * 0.01F);
}

void Game::HandleVoiceReceiveEvent(Variable *variable)
{
	// The "Receive voice chat" variable has changed. If we're in a multiplayer game,
	// then tell the server either to start or stop sending chat data.

	if (TheMessageMgr->Multiplayer())
	{
		MessageType type = (variable->GetIntegerValue() != 0) ? kMessageClientVoiceReceiveStart : kMessageClientVoiceReceiveStop;
		TheMessageMgr->SendMessage(kPlayerServer, ClientMiscMessage(type));
	}
}

void Game::HandleVoiceSendEvent(Variable *variable)
{
	// The "Send voice chat" variable has changed. If we're in a multiplayer game,
	// then tell the Audio Capture Manager either to start or stop capturing voice.
	// (Redundant calls to these functions have no effect.)

	if (TheMessageMgr->Multiplayer())
	{
		if (variable->GetIntegerValue() != 0)
		{
			TheAudioCaptureMgr->StartAudioCapture();
		}
		else
		{
			TheAudioCaptureMgr->StopAudioCapture();
		}
	}
}

float Game::GetCameraFocalLength(void) const
{
	int32 angle = Min(Max(fovVariable->GetIntegerValue(), 45), 90);
	return (1.0F / Tan((float) angle * (K::radians * 0.5F)));
}

void Game::InitPlayerStyle(int32 *style)
{
	style[kPlayerStyleEmissionColor] = TheEngine->GetVariable("playerEmissionColor")->GetIntegerValue();
	style[kPlayerStyleArmorColor] = TheEngine->GetVariable("playerArmorColor")->GetIntegerValue();
	style[kPlayerStyleArmorTexture] = TheEngine->GetVariable("playerArmorTexture")->GetIntegerValue();
	style[kPlayerStyleHelmetType] = TheEngine->GetVariable("playerHelmetType")->GetIntegerValue();
}

void Game::HandleHostCommand(Command *command, const char *text)
{
	if (*text != 0)
	{
		ResourceName	name;

		Text::ReadString(text, name, kMaxResourceNameLength);
		HostMultiplayerGame(name, 0);
	}
	else
	{
		HostGameWindow::Open();
	}
}

void Game::HandleJoinCommand(Command *command, const char *text)
{
	JoinGameWindow::Open();
}

void Game::HandleSaveCommand(Command *command, const char *text)
{
	if (*text != 0)
	{
		ResourceName	name;

		Text::ReadString(text, name, kMaxResourceNameLength);
		SaveSinglePlayerGame(name);
	}
}

void Game::HandleRestoreCommand(Command *command, const char *text)
{
	if (*text != 0)
	{
		ResourceName	name;

		Text::ReadString(text, name, kMaxResourceNameLength);
		RestoreSinglePlayerGame(name);
	}
}

#if MGCHEATS

	void Game::HandleTeleportCommand(Command *command, const char *text)
	{
		GameWorld *world = static_cast<GameWorld *>(TheWorldMgr->GetWorld());
		if (world)
		{
			if (*text != 0)
			{
				TeleportWindow::Teleport(text);
			}
			else
			{
				TeleportWindow::Open();
			}
		}
	}

	void Game::HandleBenchCommand(Command *command, const char *text)
	{
		GameWorld *world = static_cast<GameWorld *>(TheWorldMgr->GetWorld());
		if (world)
		{
			BenchmarkCamera *camera = new BenchmarkCamera(TheGame->GetCameraFocalLength(), 1.0F);
			camera->BeginBenchmark(world, world->GetFirstBenchmarkLocator());
		}
	}

	void Game::HandleGiveCommand(Command *command, const char *text)
	{
		if (!TheMessageMgr->Multiplayer())
		{
			GamePlayer *player = static_cast<GamePlayer *>(TheMessageMgr->GetLocalPlayer());
			if (player)
			{
				for (machine a = 0; a < kWeaponStandardCount; a++)
				{
					const Weapon *weapon = Weapon::Get(a);
					if (weapon)
					{
						player->AcquireWeapon(a);
						player->SetWeaponAmmo(a, 0, MaxZero(weapon->GetMaxWeaponAmmo(0)));
						player->SetWeaponAmmo(a, 1, MaxZero(weapon->GetMaxWeaponAmmo(1)));
					}
				}
			}
		}
	}

	void Game::HandleGodCommand(Command *command, const char *text)
	{
		if (!TheMessageMgr->Multiplayer())
		{
			gameFlags ^= kGameGodMode;
		}
	}

#endif

void Game::ClearInterface(void)
{
	delete TheMainWindow;
	TheInterfaceMgr->GetStrip()->Hide();
	TheConsoleWindow->Close();

	lookSpeedX = 0.0F;
	lookSpeedY = 0.0F;
}

void Game::StartSinglePlayerGame(const char *name)
{
	ClearInterface();
	ExitCurrentGame();

	currentWorldName = name;

	World *world = new GameWorld(name);
	LoadWindow *window = new LoadWindow(world);
	window->SetCompletionProc(&SinglePlayerWorldLoaded, this);
	AddWindow(window);
}

void Game::SinglePlayerWorldLoaded(LoadWindow *window, void *cookie)
{
	Game *game = static_cast<Game *>(cookie);

	TheMessageMgr->BeginSinglePlayerGame();

	GamePlayer *player = static_cast<GamePlayer *>(TheMessageMgr->GetLocalPlayer());
	game->previousPlayerState = *player->GetPlayerState();

	game->SpawnPlayer(player);
}

void Game::SaveSinglePlayerGame(const char *name)
{
	PlayerStateProperty *property = nullptr;
	const GamePlayer *player = static_cast<GamePlayer *>(TheMessageMgr->GetLocalPlayer());
	if (player)
	{
		FighterController *controller = player->GetPlayerController();
		if (controller)
		{
			property = new PlayerStateProperty(controller->GetTargetNode(), player->GetPlayerState(), (controller->GetFlashlight() != nullptr));
			TheWorldMgr->GetWorld()->GetRootNode()->AddProperty(property);
		}
	}

	TheWorldMgr->SaveDeltaWorld(name);
	delete property;
}

WorldResult Game::RestoreSinglePlayerGame(const char *name)
{
	ClearInterface();
	ExitCurrentGame();

	TheMessageMgr->BeginSinglePlayerGame();

	WorldResult result = TheWorldMgr->RestoreDeltaWorld(name);
	if (result == kWorldOkay)
	{
		World *world = TheWorldMgr->GetWorld();
		currentWorldName = world->GetWorldName();

		const PlayerStateProperty *property = static_cast<PlayerStateProperty *>(world->GetRootNode()->GetProperty(kPropertyPlayerState));
		if (property)
		{
			Model *model = property->GetModelNode();
			if (model)
			{
				FighterController *controller = static_cast<FighterController *>(model->GetController());
				GamePlayer *localPlayer = static_cast<GamePlayer *>(TheMessageMgr->GetLocalPlayer());
				localPlayer->SetPlayerController(controller, property->GetPlayerState());

				controller->SetWeapon(localPlayer->GetCurrentWeapon(), world->NewControllerIndex());

				GameWorld *gameWorld = static_cast<GameWorld *>(world);
				gameWorld->SetCameraTargetModel(model);
				gameWorld->SetLocalPlayerVisibility();

				DisplayBoard::Open();
				TheDisplayBoard->UpdatePlayerScore();
				TheDisplayBoard->UpdatePlayerHealth();
				TheDisplayBoard->UpdatePlayerWeapons();

				if (property->GetFlashlightFlag())
				{
					controller->ToggleFlashlight();
				}
			}

			delete property;
		}
	}

	return (result);
}

EngineResult Game::HostMultiplayerGame(const char *name, unsigned_int32 flags)
{
	ClearInterface();
	ExitCurrentGame();

	TheNetworkMgr->SetProtocol(kGameProtocol);
	TheNetworkMgr->SetPortNumber(kGamePort);
	TheNetworkMgr->SetBroadcastPortNumber(kGamePort);

	EngineResult result = TheMessageMgr->BeginMultiplayerGame(true);
	if (result == kNetworkOkay)
	{
		result = TheWorldMgr->LoadWorld(name);
		if (result == kWorldOkay)
		{
			multiplayerFlags = flags;

			GamePlayer *player = static_cast<GamePlayer *>(TheMessageMgr->GetLocalPlayer());
			unsigned_int32 playerFlags = (flags & kMultiplayerDedicated) ? kPlayerInactive : 0;

			if (TheEngine->GetVariable("voiceReceive")->GetIntegerValue() != 0)
			{
				playerFlags |= kPlayerReceiveVoiceChat;
			}

			player->SetPlayerFlags(playerFlags);

			if (TheEngine->GetVariable("voiceSend")->GetIntegerValue() != 0)
			{
				TheAudioCaptureMgr->StartAudioCapture();
			}

			ScoreBoard::Open();
		}
	}

	return (result);
}

EngineResult Game::JoinMultiplayerGame(const char *name, unsigned_int32 flags)
{
	ClearInterface();

	EngineResult result = TheWorldMgr->LoadWorld(name);
	if (result == kWorldOkay)
	{
		multiplayerFlags = 0;

		GamePlayer *player = static_cast<GamePlayer *>(TheMessageMgr->GetPlayer(kPlayerServer));
		if (flags & kMultiplayerDedicated)
		{
			player->SetPlayerFlags(player->GetPlayerFlags() | kPlayerInactive);
		}

		if (TheEngine->GetVariable("voiceSend")->GetIntegerValue() != 0)
		{
			TheAudioCaptureMgr->StartAudioCapture();
		}
	}

	return (result);
}

void Game::ExitCurrentGame(void)
{
	MainWindow::ReturnToGame();

	TheAudioCaptureMgr->StopAudioCapture();
	TheMessageMgr->EndGame();

	multiplayerFlags = 0;
	gameFlags = 0;

	TheWorldMgr->UnloadWorld();

	TheMessageBoard->InstantHide();
	TheUsePopupBoard->InstantHide();
	TheNamePopupBoard->InstantHide();
	TheSecretPopupBoard->InstantHide();
	TheActionBoard->Hide();

	delete TheDeadBoard;
	delete TheScoreBoard;
	delete TheDisplayBoard;
}

WorldResult Game::RestartWorld(void)
{
	GameWorld *world = static_cast<GameWorld *>(TheWorldMgr->GetWorld());
	if ((!world) || (world->GetSpawnLocatorCount() == 0))
	{
		return (kWorldOkay);
	}

	ClearInterface();
	ExitCurrentGame();

	WorldResult result = TheWorldMgr->LoadWorld(currentWorldName);
	if (result == kWorldOkay)
	{
		TheMessageMgr->BeginSinglePlayerGame();
		TheInputMgr->ResetAllActions();

		GamePlayer *player = static_cast<GamePlayer *>(TheMessageMgr->GetLocalPlayer());
		player->RestorePlayerState(&previousPlayerState);

		SpawnPlayer(player);
	}

	return (result);
}

void Game::StartTitleMusic(void)
{
	/*if (!titleMusic)
	{
		titleMusic = new Sound;
		WaveStreamer *streamer = new WaveStreamer;
		streamer->AddComponent("music/Mangler");

		titleMusic->Stream(streamer);
		titleMusic->SetLoopCount(kSoundLoopInfinite);
		titleMusic->SetSoundProperty(kSoundVolume, 0.0F);
		titleMusic->SetSoundGroup(&musicSoundGroup);
		titleMusic->Play();
		titleMusic->VaryVolume(1.0F, 1500);
	}*/
}

void Game::StopTitleMusic(void)
{
	/*if (titleMusic)
	{
		titleMusic->Release();
		titleMusic = nullptr;
	}*/
}

EngineResult Game::LoadWorld(const char *name)
{
	StartSinglePlayerGame(name);
	return (kEngineOkay);
}

void Game::UnloadWorld(void)
{
	ExitCurrentGame();
	delete TheMainWindow;
	delete TheTeleportWindow;
}

const RemotePortal *Game::DetectTeleporter(const Geometry *geometry)
{
	const Property *property = geometry->GetProperty(kPropertyTeleporter);
	if (property)
	{
		const TeleporterProperty *teleporterProperty = static_cast<const TeleporterProperty *>(property);

		const Node *node = geometry->GetConnectedNode(teleporterProperty->GetPortalConnectorKey());
		if ((node) && (node->GetNodeType() == kNodePortal))
		{
			const Portal *portal = static_cast<const Portal *>(node);
			if (portal->GetPortalType() == kPortalRemote)
			{
				return (static_cast<const RemotePortal *>(portal));
			}
		}
	}

	return (nullptr);
}

void Game::ProcessGeometryProperties(const Geometry *geometry, const Point3D& position, Node *initiator)
{
	const Property *property = geometry->GetFirstProperty();
	while (property)
	{
		switch (property->GetPropertyType())
		{
			case kPropertyShootable:
			{
				Controller *controller = geometry->GetController();
				if (controller)
				{
					controller->Activate(initiator, nullptr);
				}

				break;
			}
		}

		property = property->Next();
	}
}

// ZYUQURM
