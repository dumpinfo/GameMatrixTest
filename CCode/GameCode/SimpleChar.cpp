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


#include "C4Panels.h"
#include "SimpleChar.h"


using namespace C4;


// This is the definition of the pointer to the Game class singleton.
// It should be initialized to nullptr, and its value will be set by
// the Game class constructor.

Game *C4::TheGame = nullptr;


C4::Application *CreateApplication(void)
{
	// This function should simply return a pointer to a new instance of
	// the Application class. Normally, the application/game module will
	// define a subclass of the Application class (in this case, the
	// Game class) and return a pointer to a new instance of that type.
	//
	// This function is called exactly one time right after the
	// application/game module DLL is loaded by the engine. The returned
	// class is destroyed via the virtual destructor of the Application
	// class right before the application/game module DLL is unloaded.

	return (new Game);
}


MovementAction::MovementAction(unsigned_int32 type, unsigned_int32 flag) : Action(type)
{
	// Each instance of the MovementAction class represents a movement
	// in a single direction, as indicated by the flag parameter.
	// All of the MovementAction instances are constructed in the
	// Game class constructor.

	movementFlag = flag;
}

MovementAction::~MovementAction()
{
}

void MovementAction::Begin(void)
{
	// This function is called when the input control associated with this
	// particular action is activated (e.g., a key was pressed). We respond to
	// such an event by setting a movement flag in the soldier controller.

	SoldierController *controller = TheGame->GetSoldierController();
	if (controller)
	{
		controller->SetMovementFlags(controller->GetMovementFlags() | movementFlag);
	}
}

void MovementAction::End(void)
{
	// This function is called when the input control associated with this
	// particular action is deactivated (e.g., a key was released). We respond to
	// such an event by clearing a movement flag in the soldier controller.

	SoldierController *controller = TheGame->GetSoldierController();
	if (controller)
	{
		controller->SetMovementFlags(controller->GetMovementFlags() & ~movementFlag);
	}
}


UseAction::UseAction() : Action(kActionUse)
{
}

UseAction::~UseAction()
{
}

void UseAction::Begin(void)
{
	// The player has pressed the fire/use button. If we are currently interacting with
	// a node in the scene and that node has a controller, then we send an activate event
	// to that controller to let it know that the player is doing something with it.

	SoldierController *controller = TheGame->GetSoldierController();
	if (controller)
	{
		const SoldierInteractor *interactor = controller->GetSoldierInteractor();
		const Node *interactionNode = interactor->GetInteractionNode();
		if (interactionNode)
		{
			Controller *interactionController = interactionNode->GetController();
			if (interactionController)
			{
				interactionController->HandleInteractionEvent(kInteractionEventActivate, &interactor->GetInteractionPosition(), controller->GetTargetNode());
			}
		}
	}
}
 
void UseAction::End(void)
{
	// The player has released the fire/use button. Let the node with which we are interacting 
	// know that we are done with it by sending its controller a deactivate event.
 
	SoldierController *controller = TheGame->GetSoldierController();
	if (controller)
	{ 
		const SoldierInteractor *interactor = controller->GetSoldierInteractor();
		const Node *interactionNode = interactor->GetInteractionNode(); 
		if (interactionNode) 
		{
			Controller *interactionController = interactionNode->GetController();
			if (interactionController)
			{ 
				interactionController->HandleInteractionEvent(kInteractionEventDeactivate, &interactor->GetInteractionPosition(), controller->GetTargetNode());
			}
		}
	}
}


SoldierInteractor::SoldierInteractor(SoldierController *controller)
{
	soldierController = controller;
}

SoldierInteractor::~SoldierInteractor()
{
}

void SoldierInteractor::HandleInteractionEvent(InteractionEventType type, Node *node, const Point3D *position)
{
	// Always call the base class counterpart.

	Interactor::HandleInteractionEvent(type, node, position);

	// If the node with which we are interacting has a controller,
	// then pass the event through to that controller.

	Controller *controller = node->GetController();
	if (controller)
	{
		controller->HandleInteractionEvent(type, position);
	}
}


SoldierController::SoldierController(float azimuth) :
		CharacterController(kControllerSoldier),
		soldierInteractor(this)
{
	soldierMotion = kMotionNone;
	movementFlags = 0;

	modelAzimuth = azimuth;
	modelAltitude = 0.0F;
}

SoldierController::SoldierController(const SoldierController& soldierController) :
		CharacterController(soldierController),
		soldierInteractor(this)
{
	soldierMotion = kMotionNone;
	movementFlags = 0;

	modelAzimuth = 0.0F;
	modelAltitude = 0.0F;
}

SoldierController::~SoldierController()
{
}

Controller *SoldierController::Replicate(void) const
{
	return (new SoldierController(*this));
}

void SoldierController::Preprocess(void)
{
	// This function is called once before the target node is ever
	// rendered or moved. The base class Preprocess() function should
	// always be called first, and then the subclass can do whatever
	// preprocessing it needs to do.

	CharacterController::Preprocess();

	SetRigidBodyFlags(kRigidBodyKeepAwake | kRigidBodyFixedOrientation);
	SetFrictionCoefficient(0.0F);

	// We use a frame animator to play animation resources
	// for the soldier model.

	Model *soldier = GetTargetNode();
	frameAnimator.SetTargetModel(soldier);
	soldier->SetRootAnimator(&frameAnimator);

	// Initialize the previous center of mass to the current center of mass
	// so that this doesn't contain garbage the first time be call ActivateTriggers().

	previousCenterOfMass = GetWorldCenterOfMass();

	// Register our interactor with the world.

	soldier->GetWorld()->AddInteractor(&soldierInteractor);
}

void SoldierController::Move(void)
{
	// This function is called once per frame to allow the controller to
	// move its target node.

	// The movementIndexTable is a 16-entry table that maps all combinations of
	// the forward, backward, left, and right movement flags to one of eight directions.
	// The direction codes are as follows:
	//
	// 0 - forward
	// 1 - backward
	// 2 - left
	// 3 - right
	// 4 - forward and left
	// 5 - forward and right
	// 6 - backward and left
	// 7 - backward and right
	//
	// The number 8 in the table means no movement, and it appears where either no
	// movement buttons are being pressed or two opposing buttons are the only ones pressed
	// (e.g., left and right pressed simultaneously cancel each other out).

	static const unsigned_int8 movementIndexTable[16] =
	{
		8, 0, 1, 8,
		2, 4, 6, 2,
		3, 5, 7, 3,
		8, 0, 1, 8
	};

	// First, we grab the mouse deltas from the Input Manager.
	// We use these to change the angles representing the direction in
	// which the player is looking/moving.

	float azm = modelAzimuth + TheInputMgr->GetMouseDeltaX();
	if (azm < -K::tau_over_2)
	{
		azm += K::tau;
	}
	else if (azm > K::tau_over_2)
	{
		azm -= K::tau;
	}

	float alt = Clamp(modelAltitude + TheInputMgr->GetMouseDeltaY(), -1.45F, 1.45F);

	modelAzimuth = azm;
	modelAltitude = alt;

	// Now, we determine whether the player is attempting to move, and
	// we play the appropriate animation on the soldier model.

	int32 motion = kMotionStand;
	Vector2D propel(0.0F, 0.0F);

	int32 index = movementIndexTable[movementFlags & kMovementPlanarMask];
	if (index < 8)
	{
		// The movementDirectionTable maps each direction code looked up in the
		// movementIndexTable to an angle measured counterclockwise from the straight
		// ahead direction in units of tau/8.

		static const float movementDirectionTable[8] =
		{
			0.0F, 4.0F, 2.0F, -2.0F, 1.0F, -1.0F, 3.0F, -3.0F
		};

		float direction = movementDirectionTable[index] * K::tau_over_8 + modelAzimuth;

		// Set the propulsive force based on the direction of movement.

		propel = CosSin(direction) * 100.0F;

		// Determine whether we should play the forward or backward running animation.

		motion = ((index == 1) || (index >= 6)) ? kMotionBackward : kMotionForward;
	}

	// Update the external force for the rigid body representing the character.
	// The GetGroundContact() function is a member of the CharacterController base class.

	if (GetGroundContact())
	{
		SetExternalLinearResistance(Vector2D(10.0F, 10.0F));
		SetExternalForce(propel);
	}
	else
	{
		// If the soldier is not on the ground, reduce the propulsive force down to 2%.
		// This controls how well the player is able to control his movement while
		// falling through the air.

		SetExternalLinearResistance(Zero2D);
		SetExternalForce(propel * 0.02F);
	}

	// Change the soldier's orientation based on horizontal mouse movement.
	// The SetCharacterOrientation() function is a member of the CharacterController base class.

	SetCharacterOrientation(modelAzimuth);

	// If the animation needs to be changed, do it.

	if (motion != soldierMotion)
	{
		SetSoldierMotion(motion);
	}

	// Activate triggers along the line connecting to the current center of mass
	// from the center of mass in the previous frame.

	Model *model = GetTargetNode();
	model->GetWorld()->ActivateTriggers(previousCenterOfMass, GetWorldCenterOfMass(), 0.25F, model);
	previousCenterOfMass = GetWorldCenterOfMass();

	// Call the Model::Animate() function to update the animation playing for the model.

	GetTargetNode()->Animate();
}

void SoldierController::SetSoldierMotion(int32 motion)
{
	// This function sets the animation resource corresponding to
	// the current type of motion assigned to the soldier.

	Interpolator *interpolator = frameAnimator.GetFrameInterpolator();

	if (motion == kMotionStand)
	{
		frameAnimator.SetAnimation("soldier/Stand");
		interpolator->SetMode(kInterpolatorForward | kInterpolatorLoop);
	}
	else if (motion == kMotionForward)
	{
		frameAnimator.SetAnimation("soldier/Run");
		interpolator->SetMode(kInterpolatorForward | kInterpolatorLoop);
	}
	else if (motion == kMotionBackward)
	{
		frameAnimator.SetAnimation("soldier/Backward");
		interpolator->SetMode(kInterpolatorForward | kInterpolatorLoop);
	}

	soldierMotion = motion;
}


ChaseCamera::ChaseCamera() : FrustumCamera(2.0F, 1.0F)
{
	targetModel = nullptr;
}

ChaseCamera::~ChaseCamera()
{
}

void ChaseCamera::Move(void)
{
	Model *model = GetTargetModel();
	if (model)
	{
		CollisionData	data;

		SoldierController *controller = static_cast<SoldierController *>(model->GetController());

		// Here, we calculate the local coordinate frame for the chase camera
		// based on the direction that the player is looking.

		Vector2D t = CosSin(controller->GetModelAzimuth());
		Vector2D u = CosSin(controller->GetModelAltitude());

		Vector3D view(t.x * u.x, t.y * u.x, u.y);
		Vector3D right(t.y, -t.x, 0.0F);
		Vector3D down = view % right;

		// We are going to place the camera behind the player, but we don't
		// want the camera to go through any geometry, so we'll do a quick
		// check for a collision.

		const Point3D& position = model->GetWorldPosition();
		Point3D p1(position.x, position.y, position.z + 1.5F);
		Point3D p2 = p1 - view * 4.0F;

		if (GetWorld()->DetectCollision(p1, p2, 0.3F, kCollisionCamera, &data))
		{
			// There's something in the way, so move the camera in closer
			// to the player.

			float s = data.param;
			p2 = p1 * (1.0F - s) + p2 * s;
		}

		// Set the camera's position and orientation.

		SetNodeTransform(right, down, view, p2);
	}
}


GameWorld::GameWorld(const char *name) : World(name)
{
	// This constructor is called when the Game::CreateWorld() function is
	// called to create a new world class. The world hasn't actually been loaded
	// from disk yet when we get here.

	spawnLocator = nullptr;
}

GameWorld::~GameWorld()
{
}

const LocatorMarker *GameWorld::FindSpawnLocator(const Zone *zone)
{
	// Iterate through all of the markers in the zone.

	const Marker *marker = zone->GetFirstMarker();
	while (marker)
	{
		if (marker->Enabled())
		{
			MarkerType type = marker->GetMarkerType();
			if (type == kMarkerLocator)
			{
				const LocatorMarker *locator = static_cast<const LocatorMarker *>(marker);
				if (locator->GetLocatorType() == kLocatorSpawn)
				{
					return (locator);
				}
			}
		}

		// Get the next marker in the list.

		marker = marker->Next();
	}

	// Look in all of the subzones.

	const Zone *subzone = zone->GetFirstSubzone();
	while (subzone)
	{
		const LocatorMarker *locator = FindSpawnLocator(subzone);
		if (locator)
		{
			return (locator);
		}

		subzone = subzone->Next();
	}

	return (nullptr);
}

WorldResult GameWorld::Preprocess(void)
{
	// The Preprocess() function is called after the world has been constructed.
	// We must always call the base class Preprocess() function first. If it returns
	// an error, then we just return the same result code.

	WorldResult result = World::Preprocess();
	if (result != kWorldOkay)
	{
		return (result);
	}

	// The world is now completely loaded. We search for a locator node that represents the
	// player's spawn position. It has a locator type of kLocatorSpawn.

	spawnLocator = FindSpawnLocator(GetRootNode());

	return (kWorldOkay);
}

void GameWorld::Interact(void)
{
	// The Interact() function is called once per frame. Before calling the base
	// class's Interact() function, we set up the interaction probe to be a line
	// segment extending two meters from the players head in the direction that
	// the camera is looking.

	SoldierController *controller = TheGame->GetSoldierController();
	if (controller)
	{
		const Point3D& p = controller->GetTargetNode()->GetWorldPosition();
		Point3D position(p.x, p.y, p.z + 1.5F);

		const Vector3D& direction = chaseCamera.GetWorldTransform()[2];
		controller->GetSoldierInteractor()->SetInteractionProbe(position, position + direction * 2.0F);
	}

	// Always call the base class counterpart.

	World::Interact();
}

void GameWorld::Render(void)
{
	// This function is called once per frame to render the world.
	// The subclass may do whatever it needs to before or after rendering,
	// but at some point must call World::Render().

	World::Render();
}


Game::Game() :

		// This is the constructor for the main application/game module class.
		// This class is create by the CreateApplication() function, which is
		// called right after the application/game DLL is loaded by the engine.
		// There is only one instance of this class, so it inherits from the
		// Singleton template, which we initialize first.

		Singleton<Game>(TheGame),

		// The display event handler encapsulates a function that gets called
		// when the Display Manager changes something like the screen resolution.

		displayEventHandler(&HandleDisplayEvent),

		// A model registration represents a model that can be instanced.
		// This particular declaration associates the kModelSoldier type with the
		// model named "Data/*/soldier/Soldier.mdl". The fourth parameter tells the
		// engine to pre-cache the model resource and not to display the model in
		// the World Editor. The last parameter specifies the default controller
		// type to assign to models of type kModelSoldier.

		soldierModelReg(kModelSoldier, nullptr, "soldier/Soldier", kModelPrecache | kModelPrivate, kControllerSoldier),

		// Locator markers are registered so that the World Editor
		// can display their names in the Get Info dialog box.

		locatorReg(kLocatorSpawn, "Spawn Location")
{
	// This installs an event handler for display events. This is only
	// necessary if we need to perform some action in response to
	// display events for some reason.

	TheDisplayMgr->InstallDisplayEventHandler(&displayEventHandler);

	// This sets the function that is called when the user hits the
	// escape key during gameplay. We save the old function so that
	// it can be restored when the game DLL is unloaded.

	prevEscapeProc = TheInputMgr->GetEscapeProc();
	prevEscapeCookie = TheInputMgr->GetEscapeCookie();
	TheInputMgr->SetEscapeProc(&EscapeProc, this);

	// This registers our world class constructor with the World Manager.
	// We only need to do this if we have defined a subclass of the World
	// class that holds extra information.

	TheWorldMgr->SetWorldCreator(&CreateWorld);

	// These create the movement actions that are used to
	// move the player around and interact with objects.

	forwardAction = new MovementAction(kActionForward, kMovementForward);
	backwardAction = new MovementAction(kActionBackward, kMovementBackward);
	leftAction = new MovementAction(kActionLeft, kMovementLeft);
	rightAction = new MovementAction(kActionRight, kMovementRight);
	upAction = new MovementAction(kActionUp, kMovementUp);
	downAction = new MovementAction(kActionDown, kMovementDown);
	useAction = new UseAction;

	// These register our new actions with the Input Manager.

	TheInputMgr->AddAction(forwardAction);
	TheInputMgr->AddAction(backwardAction);
	TheInputMgr->AddAction(leftAction);
	TheInputMgr->AddAction(rightAction);
	TheInputMgr->AddAction(upAction);
	TheInputMgr->AddAction(downAction);
	TheInputMgr->AddAction(useAction);

	// Let the Interface Manager determine when to change input devices to gameplay mode.

	TheInterfaceMgr->SetInputManagementMode(kInputManagementAutomatic);

	soldierController = nullptr;
}

Game::~Game()
{
	// When the game DLL is about to be unloaded, this destructor is called.

	TheWorldMgr->UnloadWorld();
	TheWorldMgr->SetWorldCreator(nullptr);

	delete useAction;
	delete downAction;
	delete upAction;
	delete rightAction;
	delete leftAction;
	delete backwardAction;
	delete forwardAction;

	// Restore the previous escape key handling function.

	TheInputMgr->SetEscapeProc(prevEscapeProc, prevEscapeCookie);
}

World *Game::CreateWorld(const char *name, void *cookie)
{
	// This function is called when a new world is being loaded. It should
	// return a pointer to a newly created subclass instance of the World class.

	return (new GameWorld(name));
}

void Game::HandleDisplayEvent(const DisplayEventData *eventData, void *cookie)
{
	// This function is called when a display event occurs (because we
	// registered it in the Game constructor).

	if (eventData->eventType == kEventDisplayChange)
	{
		// The screen resolution has changed. Handle accordingly.
	}
}

void Game::EscapeProc(void *cookie)
{
	// This function is called when the user hits the escape key in gameplay
	// mode because we registered it using the InputMgr::SetEscapeProc() function.
}

EngineResult Game::LoadWorld(const char *name)
{
	// Attempt to load the world.

	WorldResult result = TheWorldMgr->LoadWorld(name);
	if (result == kWorldOkay)
	{
		GameWorld *world = static_cast<GameWorld *>(TheWorldMgr->GetWorld());
		const LocatorMarker *locator = world->GetSpawnLocator();
		if (locator)
		{
			// If a spawn locator was found in the world, put a soldier character there.

			// The BeginSinglePlayerGame() function puts the Message Manager in single player mode.

			TheMessageMgr->BeginSinglePlayerGame();

			// Calculate the angle corresponding to the direction the character is initially facing.

			const Vector3D direction = locator->GetWorldTransform()[0];
			float azimuth = Atan(direction.y, direction.x);

			// Load a soldier model and attach a controller to it.

			Model *model = Model::Get(kModelSoldier);
			SoldierController *controller = new SoldierController(azimuth);
			model->SetController(controller);
			TheGame->soldierController = controller;

			// Put the model in the world at the locator's position.

			model->SetNodePosition(locator->GetWorldPosition());
			locator->GetWorld()->AddNewNode(model);

			// Set the world's current camera to be our chase camera.
			// The world will not render without a camera being set.

			ChaseCamera *camera = world->GetChaseCamera();
			camera->SetTargetModel(model);
			world->SetCamera(camera);
		}
	}

	return (result);
}

void Game::UnloadWorld(void)
{
	TheWorldMgr->UnloadWorld();

	TheMessageMgr->EndGame();

	TheGame->soldierController = nullptr;
}

// ZYUQURM
