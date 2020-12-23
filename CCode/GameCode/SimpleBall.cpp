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


#include "SimpleBall.h"


using namespace C4;


// This is the definition of the pointer to the Game class singleton.
// It should be initialized to nullptr, and its value will be set by
// the Game class constructor.

Game *C4::TheGame = nullptr;

// This is the definition of the pointer to the StartWindow class singleton.
// It should be initialized to nullptr, and its value will be set by
// the StartWindow class constructor.

StartWindow *C4::TheStartWindow = nullptr;


C4::Application *CreateApplication(void)
{
	// This function should simply return a pointer to a new instance of
	// the Application class. Normally, the application/game module will
	// define a subclass of the Application class (in this case, the
	// Game class) and return a pointer to a new instance of that type.

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
	// particular action is activated (e.g., a key was pressed).

	GameWorld *world = static_cast<GameWorld *>(TheWorldMgr->GetWorld());
	if (world)
	{
		// If there's currently a world loaded, apply the movement to
		// the spectator camera.

		SpectatorCamera *camera = world->GetSpectatorCamera();
		camera->SetSpectatorFlags(camera->GetSpectatorFlags() | movementFlag);
	}
}

void MovementAction::End(void)
{
	// This function is called when the input control associated with this
	// particular action is deactivated (e.g., a key was released).

	GameWorld *world = static_cast<GameWorld *>(TheWorldMgr->GetWorld());
	if (world)
	{
		// If there's currently a world loaded, remove the movement from
		// the spectator camera.

		SpectatorCamera *camera = world->GetSpectatorCamera();
		camera->SetSpectatorFlags(camera->GetSpectatorFlags() & ~movementFlag);
	}
}


BallController::BallController() : RigidBodyController(kControllerBall)
{
	// This constructor is only called when a new ball model is created.
}

BallController::BallController(const BallController& ballController) : RigidBodyController(ballController)
{
	// This constructor is called when a ball controller is cloned.
}

BallController::~BallController()
{
}

Controller *BallController::Replicate(void) const
{
	return (new BallController(*this));
}
 
bool BallController::ValidNode(const Node *node)
{
	// This function is called by the engine to determine whether 
	// this particular type of controller can control the particular
	// node passed in through the node parameter. This function should 
	// return true if it can control the node, and otherwise it should
	// return false. In this case, the controller can only be applied
	// to model nodes. 

	return (node->GetNodeType() == kNodeModel); 
} 

void BallController::Preprocess(void)
{
	// This function is called once before the target node is ever 
	// rendered or moved. The base class Preprocess() function should
	// always be called first, and then the subclass can do whatever
	// preprocessing it needs to do. In this case, we set a few of the
	// ball's physical parameters and give it a random initial velocity.

	RigidBodyController::Preprocess();

	SetRestitutionCoefficient(0.99F);
	SetSpinFrictionMultiplier(0.1F);
	SetRollingResistance(0.01F);

	SetLinearVelocity(Math::RandomUnitVector3D() * 2.0F);
}

RigidBodyStatus BallController::HandleNewRigidBodyContact(const RigidBodyContact *contact, RigidBodyController *contactBody)
{
	// This function is called when the ball makes contact with another rigid body.

	if (contactBody->GetControllerType() == kControllerBall)
	{
		// Add a sound effect and some sparks to the world.

		Node *node = GetTargetNode();
		World *world = node->GetWorld();
		Point3D position = node->GetWorldTransform() * contact->GetContactPosition();

		OmniSource *source = new OmniSource("model/Ball", 40.0F);
		source->SetNodePosition(position);
		world->AddNewNode(source);

		SparkParticleSystem *sparks = new SparkParticleSystem(20);
		sparks->SetNodePosition(position);
		world->AddNewNode(sparks);
	}

	return (kRigidBodyUnchanged);
}


SparkParticleSystem::SparkParticleSystem(int32 count) :

		// Initialize the base class for line particles
		// and tell it where the particle pool is.
		LineParticleSystem(kParticleSystemSpark, &particlePool, "particle/Spark1"),

		// Initialize the particle pool by telling it where the array
		// of Particle structs is and how big it is.
		particlePool(kMaxParticleCount, particleArray)
{
	// The SparkEffect node creates a small burst of sparks and
	// then self-destructs when all of them have burned out.

	sparkCount = count;
	SetLengthMultiplier(4.0F);
	SetParticleSystemFlags(kParticleSystemSelfDestruct);
}

SparkParticleSystem::SparkParticleSystem() :

		// Initialize the base class for line particles
		// and tell it where the particle pool is.
		LineParticleSystem(kParticleSystemSpark, &particlePool, "particle/Spark1"),

		// Initialize the particle pool by telling it where the array
		// of Particle structs is and how big it is.
		particlePool(kMaxParticleCount, particleArray)
{
	// This constructor gets used when the particle effect is being loaded from
	// a saved game. In this case, we don't need to initialize anything.
}

SparkParticleSystem::~SparkParticleSystem()
{
}

bool SparkParticleSystem::CalculateBoundingSphere(BoundingSphere *sphere) const
{
	// Just return a sphere that's big enough to always enclose
	// all of the particles. This is in local coordinates.

	sphere->SetCenter(0.0F, 0.0F, 0.0F);
	sphere->SetRadius(20.0F);
	return (true);
}

void SparkParticleSystem::Preprocess(void)
{
	// This function creates the spark particles.

	// Always call the base class Preprocess() function.
	LineParticleSystem::Preprocess();

	// If there's already a particle in the system, then the effect was loaded
	// from a saved game. We only create new particles if the system is empty.

	if (!GetFirstParticle())
	{
		// Calculate the world-space center.
		Point3D center = GetSuperNode()->GetWorldTransform() * GetNodePosition();
		const ConstVector2D *trig = Math::GetTrigTable();

		int32 count = sparkCount;
		for (machine a = 0; a < count; a++)
		{
			// Grab a new unused particle from the pool.
			Particle *particle = particlePool.NewParticle();
			if (!particle)
			{
				break;
			}

			particle->emitTime = 0;							// Particle appears right away.
			particle->lifeTime = 500 + Math::Random(750);	// Particle lives 500-1250 milliseconds.
			particle->radius = 0.02F;						// The radius is 20 mm.
			particle->color.Set(1.0F, 1.0F, 0.1F, 1.0F);	// It's yellow.
			particle->orientation = 0;						// This doesn't matter for line particles.
			particle->position = center;					// It starts at the effect's center.

			// Calculate a random velocity in a random direction.
			float speed = Math::RandomFloat(0.004F);
			Vector2D csp = trig[Math::Random(128)] * speed;
			const Vector2D& cst = trig[Math::Random(256)];
			particle->velocity.Set(cst.x * csp.y, cst.y * csp.y, csp.x);

			// Add the particle to the particle system.
			AddParticle(particle);
		}
	}
}

void SparkParticleSystem::AnimateParticles(void)
{
	// This function is called once per frame to move the particles.

	// Times are in milliseconds.
	int32 dt = TheTimeMgr->GetDeltaTime();
	float fdt = TheTimeMgr->GetFloatDeltaTime();

	Particle *particle = GetFirstParticle();
	while (particle)
	{
		// Get the next particle now in case the current one is removed from the system.
		Particle *next = particle->nextParticle;

		int32 life = (particle->lifeTime -= dt);
		if (life > 0)
		{
			// Update velocity with gravity.
			particle->velocity.z += K::gravity * fdt;

			// Move the particle and see if it hit the floor plane at z=0.
			float z1 = particle->position.z - particle->radius;
			particle->position += particle->velocity * fdt;
			float z2 = particle->position.z - particle->radius;
			if (z1 * z2 <= 0.0F)
			{
				// The particle hit the floor, so reflect its velocity and remove some energy.
				particle->position.z = 0.05F - z2;
				particle->velocity.z *= -0.5F;
			}

			// If the particle is nearing the end of its life, fade it out.
			if (life < 100)
			{
				particle->color.alpha = (float) life * 0.01F;
			}
		}
		else
		{
			// Particle burned out.
			FreeParticle(particle);
		}

		particle = next;
	}
}


// The StartWindow constructor initializes the Window base class with the name of the panel
// resource to load. The startButtonObserver member is initialized with the functions that
// is called when the "Start" push button in the window posts an activate event.

StartWindow::StartWindow() :
		Window("panel/SimpleBall"),
		Singleton<StartWindow>(TheStartWindow),
		startButtonObserver(this, &StartWindow::HandleStartButtonEvent)
{
}

StartWindow::~StartWindow()
{
}

void StartWindow::Preprocess(void)
{
	// We must call the Window base class Preprocess() function first to initialize the
	// internal structures that are used to search for widgets.

	Window::Preprocess();

	// Find the push button widget named "Start" and assign our observer to it.

	Widget *button = FindWidget("Start");
	button->SetObserver(&startButtonObserver);
}

void StartWindow::HandleStartButtonEvent(Widget *widget, const WidgetEventData *eventData)
{
	// This function is called whenever the "Start" push button posts an event.

	if (eventData->eventType == kEventWidgetActivate)
	{
		// If the widget was activated, then the user clicked the push button.
		// Delete the start window and load a world to play.

		delete this;

		TheWorldMgr->LoadWorld("world/SimpleBall");
	}
}


GameWorld::GameWorld(const char *name) :
		World(name),
		spectatorCamera(2.0F, 1.0F, 0.3F)
{
	// This constructor is called when the Game::CreateWorld() function is
	// called to create a new world class. The world hasn't actually been loaded
	// from disk yet when we get here.
}

GameWorld::~GameWorld()
{
}

const LocatorMarker *GameWorld::FindSpectatorLocator(const Zone *zone)
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
				if (locator->GetLocatorType() == kLocatorSpectator)
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
		const LocatorMarker *locator = FindSpectatorLocator(subzone);
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
	// We must always call the base class Preprocess() function first.

	WorldResult result = World::Preprocess();
	if (result != kWorldOkay)
	{
		return (result);
	}

	// The world is now completely loaded. We search for a locator node that represents the
	// spectator camera's starting position. It has a locator type of kLocatorSpectator.

	const LocatorMarker *marker = FindSpectatorLocator(GetRootNode());
	if (marker)
	{
		// A spectator marker was found.
		// Set the spectator camera's initial position and orientation.

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

	// Set the world's current camera to be our spectator camera.
	// The world will not render without a camera being set.

	SetCamera(&spectatorCamera);

	return (kWorldOkay);
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
		// This class is created by the CreateApplication() function, which is
		// called right after the application/game DLL is loaded by the engine.
		// There is only one instance of this class, so it inherits from the
		// Singleton template, which we initialize first.

		Singleton<Game>(TheGame),

		// The display event handler encapsulates a function that gets called
		// when the Display Manager changes something like the screen resolution.

		displayEventHandler(&HandleDisplayEvent),

		// A model registration represents a model that can be instanced.
		// This particular declaration associates the kModelBall type with the
		// model named "model/Ball.mdl". The fourth parameter tells the engine
		// to pre-cache the model resource, and the last parameter specifies
		// the default controller type to assign to models of type kModelBall.

		ballModelReg(kModelBall, "Bouncing Ball", "model/Ball", kModelPrecache, kControllerBall),

		// A controller registration tells the engine about an application-defined
		// type of controller and registers its name and validation function.
		// In this case, the name "Bouncing Ball" will appear in the list of
		// available controllers in the World Editor for any node that is determined
		// to be a valid target node by the BallController::ValidNode() function.

		controllerReg(kControllerBall, "Bouncing Ball"),

		// A particle system registration tells the engine about an application-defined
		// type of particle system and registers its name.

		sparkParticleSystemReg(kParticleSystemSpark, "Sparks"),

		// Locator markers are registered so that the World Editor
		// can display their names in the Get Info dialog box.

		locatorReg(kLocatorSpectator, "Spectator Camera")
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

	// These create the movement actions that are used to fly the
	// spectator camera around.

	forwardAction = new MovementAction(kActionForward, kSpectatorMoveForward);
	backwardAction = new MovementAction(kActionBackward, kSpectatorMoveBackward);
	leftAction = new MovementAction(kActionLeft, kSpectatorMoveLeft);
	rightAction = new MovementAction(kActionRight, kSpectatorMoveRight);
	upAction = new MovementAction(kActionUp, kSpectatorMoveUp);
	downAction = new MovementAction(kActionDown, kSpectatorMoveDown);

	// These register our new actions with the Input Manager.

	TheInputMgr->AddAction(forwardAction);
	TheInputMgr->AddAction(backwardAction);
	TheInputMgr->AddAction(leftAction);
	TheInputMgr->AddAction(rightAction);
	TheInputMgr->AddAction(upAction);
	TheInputMgr->AddAction(downAction);

	// Let the Interface Manager determine when to change input devices to gameplay mode.

	TheInterfaceMgr->SetInputManagementMode(kInputManagementAutomatic);

	// Put the Message Manager in single-player mode.

	TheMessageMgr->BeginSinglePlayerGame();

	// Create the start window and tell the Interface Manager to display it.

	TheInterfaceMgr->AddWidget(new StartWindow);
}

Game::~Game()
{
	// When the game DLL is about to be unloaded, this destructor is called.

	TheWorldMgr->UnloadWorld();
	TheWorldMgr->SetWorldCreator(nullptr);

	// If the start window exists, delete it. We need to do this here, instead of letting the
	// Interface Manager clean it up, because the destructor code for the window is in this DLL.

	delete TheStartWindow;

	// Tell the Message Manager to clean up.

	TheMessageMgr->EndGame();

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
	// return a pointer to a newly constructed subclass of the World class.

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

// ZYUQURM
