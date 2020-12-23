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


#ifndef SimpleChar_h
#define SimpleChar_h


#include "C4World.h"
#include "C4Input.h"
#include "C4Cameras.h"
#include "C4Application.h"
#include "C4Interface.h"
#include "C4Character.h"
#include "C4Zones.h"
#include "C4Engine.h"


// Every application/game module needs to declare a function called CreateApplication()
// exactly as follows. (It must be declared extern "C", and it must include the tag C4MODULEEXPORT.)
// The engine looks for this function in the DLL and calls it to create an instance of
// the subclass of the Application class that the application/game module defines.

extern "C"
{
	C4MODULEEXPORT C4::Application *CreateApplication(void);
}


namespace C4
{
	// These are action types used to define action bindings in the
	// Input Manager. If the four-character code for an action is
	// 'abcd', then any input control (there can be more than one)
	// bound to %abcd triggers the associated action.

	enum : ActionType
	{
		kActionForward			= 'frwd',
		kActionBackward			= 'bkwd',
		kActionLeft				= 'left',
		kActionRight			= 'rght',
		kActionUp				= 'jump',
		kActionDown				= 'down',
		kActionUse				= 'fire'
	};


	// These are movement flags used by the soldier controller. They are set or cleared
	// by the Begin() and End() functions in the MovementAction class.

	enum
	{
		kMovementForward		= 1 << 0,
		kMovementBackward		= 1 << 1,
		kMovementLeft			= 1 << 2,
		kMovementRight			= 1 << 3,
		kMovementUp				= 1 << 4,
		kMovementDown			= 1 << 5,
		kMovementPlanarMask		= 15
	};


	// Model types are associated with a model resource using the ModelRegistration
	// class. Models are registered with the engine in the Game constructor.

	enum : ModelType
	{
		kModelSoldier			= 'sold'
	};


	// This is the type for the controller that we use to move the soldier.

	enum : ControllerType
	{
		kControllerSoldier		= 'sold'
	};


	// New locator types are registered with the engine in the Game constructor.
	// The 'spwn' locator is used to specify where the player should be positioned
	// when a world is loaded.

	enum : LocatorType
	{
		kLocatorSpawn			= 'spwn'
	};


	class SoldierController;


	// An Action object represents an input action that can be triggered by
	// some input control, such as a key on the keyboard or a button on a joystick.
	// The Begin() and End() methods are called when the button is pressed and
	// released, respectively. Actions are registered with the Input Manager when
	// the Game class is constructed.

	class MovementAction : public Action
	{
		private:

			unsigned_int32		movementFlag;
 
		public:

			MovementAction(unsigned_int32 type, unsigned_int32 flag); 
			~MovementAction();
 
			void Begin(void);
			void End(void);
	}; 

 
	class UseAction : public Action 
	{
		public:

			UseAction(); 
			~UseAction();

			void Begin(void);
			void End(void);
	};


	// The Interactor class is used to track player interactions with objects in the scene.

	class SoldierInteractor : public Interactor
	{
		private:

			SoldierController	*soldierController;

		public:

			SoldierInteractor(SoldierController *controller);
			~SoldierInteractor();

			void HandleInteractionEvent(InteractionEventType type, Node *node, const Point3D *position) override;
	};


	// Controllers are used to control anything that moves in the world.
	// New types of controllers defined by the application/game module can be
	// registered with the engine when the Game class is constructed.
	//
	// This particular controller is used to animate the soldier. It uses the
	// built-in character controller as a base class so that the engine's native
	// physics can be used to move the character.

	class SoldierController final : public CharacterController
	{
		private:

			// These are motion states that are used to keep track
			// of which animation should be played.

			enum
			{
				kMotionNone,
				kMotionStand,
				kMotionForward,
				kMotionBackward
			};

			// The movement flags tell how the user is trying to move the player.

			unsigned_int32		movementFlags;

			// The soldier motion keeps track of what animation is currently playing.

			int32				soldierMotion;

			// The azimuth and altitude represent the direction the player is looking
			// by using the mouse.

			float				modelAzimuth;
			float				modelAltitude;

			// The frame animator controls playback of an animation resource.

			FrameAnimator		frameAnimator;

			// The previous center of mass stores the center point of the character on the
			// previous frame. This is used with the new center point to activate triggers.

			Point3D				previousCenterOfMass;

			// We keep an interactor object here in the controller.

			SoldierInteractor	soldierInteractor;

			SoldierController(const SoldierController& soldierController);

			Controller *Replicate(void) const override;

			void SetSoldierMotion(int32 motion);

		public:

			SoldierController(float azimuth);
			~SoldierController();

			Model *GetTargetNode(void) const
			{
				return (static_cast<Model *>(Controller::GetTargetNode()));
			}

			unsigned_int32 GetMovementFlags(void) const
			{
				return (movementFlags);
			}

			void SetMovementFlags(unsigned_int32 flags)
			{
				movementFlags = flags;
			}

			float GetModelAzimuth(void) const
			{
				return (modelAzimuth);
			}

			float GetModelAltitude(void) const
			{
				return (modelAltitude);
			}

			SoldierInteractor *GetSoldierInteractor(void)
			{
				return (&soldierInteractor);
			}

			void Preprocess(void) override;
			void Move(void) override;
	};


	// The ChaseCamera class represents a camera that will track the player's movement.

	class ChaseCamera : public FrustumCamera
	{
		private:

			Model		*targetModel;

		public:

			ChaseCamera();
			~ChaseCamera();

			Model *GetTargetModel(void) const
			{
				return (targetModel);
			}

			void SetTargetModel(Model *model)
			{
				targetModel = model;
			}

			void Move(void) override;
	};


	// The application/game module will usually define a subclass of the World
	// class so that extra information can be associated with the current world.
	// In this case, a pointer to a spawn locator and an instance of the ChaseCamera
	// class is included with the world. A new instance of this World subclass should
	// be returned when the Game::CreateWorld() function is called (see below).

	class GameWorld : public World
	{
		private:

			const LocatorMarker		*spawnLocator;

			ChaseCamera				chaseCamera;

			static const LocatorMarker *FindSpawnLocator(const Zone *zone);

		public:

			GameWorld(const char *name);
			~GameWorld();

			const LocatorMarker *GetSpawnLocator(void) const
			{
				return (spawnLocator);
			}

			ChaseCamera *GetChaseCamera(void)
			{
				return (&chaseCamera);
			}

			ResourceResult Preprocess(void);

			void Interact(void);
			void Render(void);
	};


	// Every application/game module needs to define a subclass of the Application
	// class to serve as the primary interface with the engine. This subclass is
	// created and returned to the engine in the CreateApplication() function.
	// There should be only one instance of this class, so it inherits from the
	// Singleton template. A pointer to the Game instance is declared below.

	class Game : public Application, public Singleton<Game>
	{
		private:

			DisplayEventHandler				displayEventHandler;

			ModelRegistration				soldierModelReg;
			LocatorRegistration				locatorReg;

			InputMgr::KeyProc				*prevEscapeProc;
			void							*prevEscapeCookie;

			MovementAction					*forwardAction;
			MovementAction					*backwardAction;
			MovementAction					*leftAction;
			MovementAction					*rightAction;
			MovementAction					*upAction;
			MovementAction					*downAction;
			UseAction						*useAction;

			SoldierController				*soldierController;

			static World *CreateWorld(const char *name, void *cookie);

			static void HandleDisplayEvent(const DisplayEventData *eventData, void *cookie);

			static void EscapeProc(void *cookie);

		public:

			Game();
			~Game();

			SoldierController *GetSoldierController(void) const
			{
				return (soldierController);
			}

			EngineResult LoadWorld(const char *name) override;
			void UnloadWorld(void) override;
	};


	// This is a pointer to the one instance of the Game class through which
	// any other part of the application/game module can access it.

	extern Game *TheGame;
}


#endif

// ZYUQURM
