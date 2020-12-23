 

#ifndef C4Kinematic_h
#define C4Kinematic_h


//# \component	Physics Manager
//# \prefix		PhysicsMgr/


#include "C4Controller.h"
#include "C4Connector.h"


namespace C4
{
	enum : ControllerType
	{
		kControllerKinematic			= 'KINE',
		kControllerMovement				= 'MOVE',
		kControllerOscillation			= 'OSCL',
		kControllerRotation				= 'ROTA',
		kControllerSpin					= 'SPIN'
	};


	enum : FunctionType
	{
		kFunctionGetMovementSpeed		= 'GSPD',
		kFunctionSetMovementSpeed		= 'SSPD',
		kFunctionMoveToStart			= 'STAR',
		kFunctionMoveToFinish			= 'FINI'
	};


	enum : FunctionType
	{
		kFunctionGetOscillationSpeed	= 'GSPD',
		kFunctionSetOscillationSpeed	= 'SSPD'
	};


	enum : FunctionType
	{
		kFunctionGetRotationSpeed		= 'GSPD',
		kFunctionSetRotationSpeed		= 'SSPD',
		kFunctionRotateToStart			= 'STAR',
		kFunctionRotateToFinish			= 'FINI'
	};


	enum : FunctionType
	{
		kFunctionGetSpinSpeed			= 'GSPD',
		kFunctionSetSpinSpeed			= 'SSPD'
	};


	//# \enum	KinematicFlags

	enum
	{
		kKinematicWakeRigidBodies		= 1 << 0
	};


	class Site;
	class Zone;


	//# \class	KinematicController		The base class for kinematic controllers.
	//
	//# The $KinematicController$ class is the base class for kinematic controllers.
	//
	//# \def	class KinematicController : public Controller
	//
	//# \ctor	KinematicController(ControllerType type);
	//
	//# The constructor has protected access. The $KinematicController$ class can only be used as a base class for another class.
	//
	//# \param	type	The controller type.
	//
	//# \desc
	//# The $KinematicController$ class is the base class for kinematic controllers that move an object
	//# in some exact manner without regard for external forces.
	//
	//# \base	Controller/Controller		A $KinematicController$ is a specific type of controller.
	//
	//# \also	$@MovementController@$
	//# \also	$@OscillationController@$
	//# \also	$@RotationController@$
	//# \also	$@SpinController@$


	class KinematicController : public Controller
	{
		private:

			unsigned_int32		kinematicFlags;

			void WakeCellRigidBodies(const Site *cell, const Box3D& box);
			void WakeZoneRigidBodies(Zone *zone, const Box3D& box);
 
		protected:

			KinematicController(ControllerType type); 
			KinematicController(const KinematicController& kinematicController);
 
			void WakeProximateRigidBodies(void);

		public: 

			~KinematicController(); 
 
			unsigned_int32 GetKinematicFlags(void) const
			{
				return (kinematicFlags);
			} 

			void SetKinematicFlags(unsigned_int32 flags)
			{
				kinematicFlags = flags;
			}

			void Pack(Packer& data, unsigned_int32 packFlags) const override;
			void Unpack(Unpacker& data, unsigned_int32 unpackFlags) override;
			bool UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags);

			int32 GetSettingCount(void) const override;
			Setting *GetSetting(int32 index) const override;
			void SetSetting(const Setting *setting) override;
	};


	//# \class	MovementController		Manages a node that moves between two points.
	//
	//# The $MovementController$ class manages a node that moves between two points.
	//
	//# \def	class MovementController final : public KinematicController
	//
	//# \ctor	MovementController();
	//
	//# \desc
	//# The $MovementController$ class can be assigned to a node in order to make it move along a line between
	//# two locations, a start position and a finish position.
	//
	//# \base	KinematicController		A $MovementController$ is a specific type of kinematic controller.
	//
	//# \also	$@OscillationController@$
	//# \also	$@RotationController@$
	//# \also	$@SpinController@$
	//
	//# \wiki	Movement_Controller		Movement Controller


	class MovementController final : public KinematicController
	{
		private:

			enum
			{
				kMovementInitialized	= 1 << 0
			};

			unsigned_int32		movementState;
			float				currentDistance;

			float				targetSpeed;
			float				currentSpeed;
			float				currentAcceleration;

			float				minMovementSpeed;
			float				decelerationDistance;
			float				decelerationRate;

			float				movementSpeed;
			float				accelerationTime;
			float				decelerationTime;

			const Point3D		*startPosition;
			const Point3D		*finishPosition;
			float				movementDistance;
			float				inverseMovementDistance;
			Vector3D			originalNodeOffset;

			ConnectorKey		startConnectorKey;
			ConnectorKey		finishConnectorKey;

			MovementController(const MovementController& movementController);

			Controller *Replicate(void) const override;

			void CalculateMovementParameters(void);
			void CalculateForwardParameters(void);
			void CalculateBackwardParameters(void);

			void UpdateNodeDistance(float distance);
			void SetGeometryVelocity(const Vector3D& velocity) const;

		public:

			enum
			{
				kEventMovementReachedStart		= 'CMST',
				kEventMovementReachedFinish		= 'CMFN'
			};

			enum
			{
				kMovementMessageState
			};

			C4API MovementController();
			C4API ~MovementController();

			float GetMovementSpeed(void) const
			{
				return (movementSpeed);
			}

			float GetAccelerationTime(void) const
			{
				return (accelerationTime);
			}

			void SetAccelerationTime(float time)
			{
				accelerationTime = time;
			}

			float GetDecelerationTime(void) const
			{
				return (decelerationTime);
			}

			void SetDecelerationTime(float time)
			{
				decelerationTime = time;
			}

			const ConnectorKey& GetStartConnectorKey(void) const
			{
				return (startConnectorKey);
			}

			void SetStartConnectorKey(const ConnectorKey& key)
			{
				startConnectorKey = key;
			}

			const ConnectorKey& GetFinishConnectorKey(void) const
			{
				return (finishConnectorKey);
			}

			void SetFinishConnectorKey(const ConnectorKey& key)
			{
				finishConnectorKey = key;
			}

			static void RegisterFunctions(ControllerRegistration *registration);

			void Pack(Packer& data, unsigned_int32 packFlags) const override;
			void Unpack(Unpacker& data, unsigned_int32 unpackFlags) override;
			bool UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags);

			int32 GetSettingCount(void) const override;
			Setting *GetSetting(int32 index) const override;
			void SetSetting(const Setting *setting) override;

			void Preprocess(void) override;
			bool InstanceExtractable(void) const override;

			ControllerMessage *CreateMessage(ControllerMessageType type) const override;
			void ReceiveMessage(const ControllerMessage *message) override;
			void SendInitialStateMessages(Player *player) const override;

			void Wake(void) override;
			void Sleep(void) override;
			void Move(void) override;
			void Activate(Node *initiator, Node *trigger) override;

			C4API void SetMovementSpeed(float speed);
			C4API void MoveToStart(void);
			C4API void MoveToFinish(void);
	};


	class GetMovementSpeedFunction final : public Function
	{
		private:

			GetMovementSpeedFunction(const GetMovementSpeedFunction& getMovementSpeedFunction);

			Function *Replicate(void) const override;

		public:

			GetMovementSpeedFunction();
			~GetMovementSpeedFunction();

			void Execute(Controller *controller, FunctionMethod *method, const ScriptState *state) override;
	};


	class SetMovementSpeedFunction final : public Function
	{
		private:

			float		movementSpeed;

			SetMovementSpeedFunction(const SetMovementSpeedFunction& setMovementSpeedFunction);

			Function *Replicate(void) const override;

		public:

			SetMovementSpeedFunction();
			~SetMovementSpeedFunction();

			void Pack(Packer& data, unsigned_int32 packFlags) const override;
			void Unpack(Unpacker& data, unsigned_int32 unpackFlags) override;

			int32 GetSettingCount(void) const override;
			Setting *GetSetting(int32 index) const override;
			void SetSetting(const Setting *setting) override;

			void Execute(Controller *controller, FunctionMethod *method, const ScriptState *state) override;
	};


	class MoveToStartFunction final : public Function, public Observer<MoveToStartFunction, Controller>
	{
		private:

			MoveToStartFunction(const MoveToStartFunction& moveToStartFunction);

			Function *Replicate(void) const override;

			void HandleControllerEvent(Controller *controller, Controller::ObservableEventType event);

		public:

			MoveToStartFunction();
			~MoveToStartFunction();

			void Execute(Controller *controller, FunctionMethod *method, const ScriptState *state) override;
	};


	class MoveToFinishFunction final : public Function, public Observer<MoveToFinishFunction, Controller>
	{
		private:

			MoveToFinishFunction(const MoveToFinishFunction& moveToFinishFunction);

			Function *Replicate(void) const override;

			void HandleControllerEvent(Controller *controller, Controller::ObservableEventType event);

		public:

			MoveToFinishFunction();
			~MoveToFinishFunction();

			void Execute(Controller *controller, FunctionMethod *method, const ScriptState *state) override;
	};


	class MovementStateMessage : public ControllerMessage
	{
		friend class MovementController;

		private:

			float		currentDistance;

			float		targetSpeed;
			float		currentSpeed;
			float		currentAcceleration;

			float		movementSpeed;
			float		accelerationTime;
			float		decelerationTime;

			MovementStateMessage(int32 controllerIndex);

		public:

			MovementStateMessage(int32 controllerIndex, float distance, float targSpeed, float currSpeed, float currAccel, float moveSpeed, float accelTime, float decelTime);
			~MovementStateMessage();

			void Compress(Compressor& data) const override;
			bool Decompress(Decompressor& data) override;
	};


	//# \class	OscillationController		Manages a node that oscillates continuously.
	//
	//# The $OscillationController$ class manages a node that oscillates continuously.
	//
	//# \def	class OscillationController final : public KinematicController
	//
	//# \ctor	OscillationController();
	//
	//# \desc
	//# The $OscillationController$ class can be assigned to a node in order to make it continuously oscillate
	//# between two locations, a start position and a finish position.
	//
	//# \base	KinematicController		A $OscillationController$ is a specific type of kinematic controller.
	//
	//# \also	$@MovementController@$
	//# \also	$@RotationController@$
	//# \also	$@SpinController@$
	//
	//# \wiki	Oscillation_Controller		Oscillation Controller


	class OscillationController final : public KinematicController
	{
		private:

			enum
			{
				kOscillationInitialized		= 1 << 0
			};

			unsigned_int32		oscillationState;
			float				oscillationAngle;
			float				oscillationSpeed;

			float				currentSpeed;
			float				currentAcceleration;

			const Point3D		*startPosition;
			const Point3D		*finishPosition;
			Vector3D			originalNodeOffset;

			ConnectorKey		startConnectorKey;
			ConnectorKey		finishConnectorKey;

			OscillationController(const OscillationController& oscillationController);

			Controller *Replicate(void) const override;

			void UpdateNodePosition(float angle);
			void SetGeometryVelocity(const Vector3D& velocity) const;

		public:

			enum
			{
				kOscillationMessageState
			};

			C4API OscillationController();
			C4API ~OscillationController();

			float GetOscillationAngle(void) const
			{
				return (oscillationAngle);
			}

			float GetOscillationSpeed(void) const
			{
				return (oscillationSpeed);
			}

			const ConnectorKey& GetStartConnectorKey(void) const
			{
				return (startConnectorKey);
			}

			void SetStartConnectorKey(const ConnectorKey& key)
			{
				startConnectorKey = key;
			}

			const ConnectorKey& GetFinishConnectorKey(void) const
			{
				return (finishConnectorKey);
			}

			void SetFinishConnectorKey(const ConnectorKey& key)
			{
				finishConnectorKey = key;
			}

			static void RegisterFunctions(ControllerRegistration *registration);

			void Pack(Packer& data, unsigned_int32 packFlags) const override;
			void Unpack(Unpacker& data, unsigned_int32 unpackFlags) override;
			bool UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags);

			int32 GetSettingCount(void) const override;
			Setting *GetSetting(int32 index) const override;
			void SetSetting(const Setting *setting) override;

			void Preprocess(void) override;
			bool InstanceExtractable(void) const override;

			ControllerMessage *CreateMessage(ControllerMessageType type) const override;
			void ReceiveMessage(const ControllerMessage *message) override;
			void SendInitialStateMessages(Player *player) const override;

			void Wake(void) override;
			void Sleep(void) override;
			void Move(void) override;

			C4API void SetOscillationSpeed(float speed, float time);
	};


	class GetOscillationSpeedFunction final : public Function
	{
		private:

			GetOscillationSpeedFunction(const GetOscillationSpeedFunction& getOscillationSpeedFunction);

			Function *Replicate(void) const override;

		public:

			GetOscillationSpeedFunction();
			~GetOscillationSpeedFunction();

			void Execute(Controller *controller, FunctionMethod *method, const ScriptState *state) override;
	};


	class SetOscillationSpeedFunction final : public Function
	{
		private:

			float		oscillationSpeed;
			float		accelerationTime;

			SetOscillationSpeedFunction(const SetOscillationSpeedFunction& setOscillationSpeedFunction);

			Function *Replicate(void) const override;

		public:

			SetOscillationSpeedFunction();
			~SetOscillationSpeedFunction();

			void Pack(Packer& data, unsigned_int32 packFlags) const override;
			void Unpack(Unpacker& data, unsigned_int32 unpackFlags) override;

			int32 GetSettingCount(void) const override;
			Setting *GetSetting(int32 index) const override;
			void SetSetting(const Setting *setting) override;

			void Execute(Controller *controller, FunctionMethod *method, const ScriptState *state) override;
	};


	class OscillationStateMessage : public ControllerMessage
	{
		friend class OscillationController;

		private:

			float		oscillationAngle;
			float		oscillationSpeed;

			float		currentSpeed;
			float		currentAcceleration;

			OscillationStateMessage(int32 controllerIndex);

		public:

			OscillationStateMessage(int32 controllerIndex, float angle, float speed, float currSpeed, float currAccel);
			~OscillationStateMessage();

			void Compress(Compressor& data) const override;
			bool Decompress(Decompressor& data) override;
	};


	//# \class	RotationController		Manages a node that rotates between two angles.
	//
	//# The $RotationController$ class manages a node that rotates between two angles.
	//
	//# \def	class RotationController final : public KinematicController
	//
	//# \ctor	RotationController();
	//
	//# \desc
	//# The $RotationController$ class can be assigned to a node in order to make it rotate between its
	//# initial position and a given rotation angle.
	//
	//# \base	KinematicController		A $RotationController$ is a specific type of kinematic controller.
	//
	//# \also	$@SpinController@$
	//# \also	$@MovementController@$
	//# \also	$@OscillationController@$
	//
	//# \wiki	Rotation_Controller		Rotation Controller


	class RotationController final : public KinematicController
	{
		private:

			enum
			{
				kRotationInitialized	= 1 << 0
			};

			unsigned_int32		rotationState;
			float				currentAngle;

			float				targetSpeed;
			float				currentSpeed;
			float				currentAcceleration;

			float				minRotationSpeed;
			float				decelerationAngle;
			float				decelerationRate;

			float				rotationAngle;
			float				rotationSpeed;
			float				accelerationTime;
			float				decelerationTime;

			const Transform4D	*centerTransform;
			Transform4D			originalNodeTransform;

			ConnectorKey		centerConnectorKey;

			RotationController(const RotationController& rotationController);

			Controller *Replicate(void) const override;

			void CalculateRotationParameters(void);
			void CalculateForwardParameters(void);
			void CalculateBackwardParameters(void);

			void UpdateNodeAngle(float angle);

		public:

			enum
			{
				kEventRotationReachedStart		= 'CRST',
				kEventRotationReachedFinish		= 'CRFN'
			};

			enum
			{
				kRotationMessageState
			};

			C4API RotationController();
			C4API ~RotationController();

			float GetRotationAngle(void) const
			{
				return (rotationAngle);
			}

			float GetRotationSpeed(void) const
			{
				return (rotationSpeed);
			}

			float GetAccelerationTime(void) const
			{
				return (accelerationTime);
			}

			void SetAccelerationTime(float time)
			{
				accelerationTime = time;
			}

			float GetDecelerationTime(void) const
			{
				return (decelerationTime);
			}

			void SetDecelerationTime(float time)
			{
				decelerationTime = time;
			}

			const ConnectorKey& GetCenterConnectorKey(void) const
			{
				return (centerConnectorKey);
			}

			void SetCenterConnectorKey(const ConnectorKey& key)
			{
				centerConnectorKey = key;
			}

			static void RegisterFunctions(ControllerRegistration *registration);

			void Pack(Packer& data, unsigned_int32 packFlags) const override;
			void Unpack(Unpacker& data, unsigned_int32 unpackFlags) override;
			bool UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags);

			int32 GetSettingCount(void) const override;
			Setting *GetSetting(int32 index) const override;
			void SetSetting(const Setting *setting) override;

			void Preprocess(void) override;
			bool InstanceExtractable(void) const override;

			ControllerMessage *CreateMessage(ControllerMessageType type) const override;
			void ReceiveMessage(const ControllerMessage *message) override;
			void SendInitialStateMessages(Player *player) const override;

			void Wake(void) override;
			void Sleep(void) override;
			void Move(void) override;
			void Activate(Node *initiator, Node *trigger) override;

			C4API void SetRotationSpeed(float speed);
			C4API void RotateToStart(void);
			C4API void RotateToFinish(void);
	};


	class GetRotationSpeedFunction final : public Function
	{
		private:

			GetRotationSpeedFunction(const GetRotationSpeedFunction& getRotationSpeedFunction);

			Function *Replicate(void) const override;

		public:

			GetRotationSpeedFunction();
			~GetRotationSpeedFunction();

			void Execute(Controller *controller, FunctionMethod *method, const ScriptState *state) override;
	};


	class SetRotationSpeedFunction final : public Function
	{
		private:

			float		rotationSpeed;

			SetRotationSpeedFunction(const SetRotationSpeedFunction& setRotationSpeedFunction);

			Function *Replicate(void) const override;

		public:

			SetRotationSpeedFunction();
			~SetRotationSpeedFunction();

			void Pack(Packer& data, unsigned_int32 packFlags) const override;
			void Unpack(Unpacker& data, unsigned_int32 unpackFlags) override;

			int32 GetSettingCount(void) const override;
			Setting *GetSetting(int32 index) const override;
			void SetSetting(const Setting *setting) override;

			void Execute(Controller *controller, FunctionMethod *method, const ScriptState *state) override;
	};


	class RotateToStartFunction final : public Function, public Observer<RotateToStartFunction, Controller>
	{
		private:

			RotateToStartFunction(const RotateToStartFunction& rotateToStartFunction);

			Function *Replicate(void) const override;

			void HandleControllerEvent(Controller *controller, Controller::ObservableEventType event);

		public:

			RotateToStartFunction();
			~RotateToStartFunction();

			void Execute(Controller *controller, FunctionMethod *method, const ScriptState *state) override;
	};


	class RotateToFinishFunction final : public Function, public Observer<RotateToFinishFunction, Controller>
	{
		private:

			RotateToFinishFunction(const RotateToFinishFunction& rotateToFinishFunction);

			Function *Replicate(void) const override;

			void HandleControllerEvent(Controller *controller, Controller::ObservableEventType event);

		public:

			RotateToFinishFunction();
			~RotateToFinishFunction();

			void Execute(Controller *controller, FunctionMethod *method, const ScriptState *state) override;
	};


	class RotationStateMessage : public ControllerMessage
	{
		friend class RotationController;

		private:

			float		currentAngle;

			float		targetSpeed;
			float		currentSpeed;
			float		currentAcceleration;

			float		rotationSpeed;
			float		accelerationTime;
			float		decelerationTime;

			RotationStateMessage(int32 controllerIndex);

		public:

			RotationStateMessage(int32 controllerIndex, float angle, float targSpeed, float currSpeed, float currAccel, float rotateSpeed, float accelTime, float decelTime);
			~RotationStateMessage();

			void Compress(Compressor& data) const override;
			bool Decompress(Decompressor& data) override;
	};


	//# \class	SpinController		Manages a node that spins continuously.
	//
	//# The $SpinController$ class manages a node that spins continuously.
	//
	//# \def	class SpinController final : public KinematicController
	//
	//# \ctor	SpinController();
	//
	//# \desc
	//# The $SpinController$ class can be assigned to a node in order to make it continuously rotate
	//# about a given center position and axis.
	//
	//# \base	KinematicController		A $SpinController$ is a specific type of kinematic controller.
	//
	//# \also	$@RotationController@$
	//# \also	$@MovementController@$
	//# \also	$@OscillationController@$
	//
	//# \wiki	Spin_Controller		Spin Controller


	class SpinController final : public KinematicController
	{
		private:

			enum
			{
				kSpinInitialized	= 1 << 0
			};

			unsigned_int32		spinState;
			float				spinAngle;
			float				spinSpeed;

			float				currentSpeed;
			float				currentAcceleration;

			const Transform4D	*centerTransform;
			Transform4D			originalNodeTransform;

			ConnectorKey		centerConnectorKey;

			SpinController(const SpinController& spinController);

			Controller *Replicate(void) const override;

			void UpdateNodeAngle(float angle);

		public:

			enum
			{
				kSpinMessageState
			};

			C4API SpinController();
			C4API ~SpinController();

			float GetSpinAngle(void) const
			{
				return (spinAngle);
			}

			float GetSpinSpeed(void) const
			{
				return (spinSpeed);
			}

			const ConnectorKey& GetCenterConnectorKey(void) const
			{
				return (centerConnectorKey);
			}

			void SetCenterConnectorKey(const ConnectorKey& key)
			{
				centerConnectorKey = key;
			}

			static void RegisterFunctions(ControllerRegistration *registration);

			void Pack(Packer& data, unsigned_int32 packFlags) const override;
			void Unpack(Unpacker& data, unsigned_int32 unpackFlags) override;
			bool UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags);

			int32 GetSettingCount(void) const override;
			Setting *GetSetting(int32 index) const override;
			void SetSetting(const Setting *setting) override;

			void Preprocess(void) override;
			bool InstanceExtractable(void) const override;

			ControllerMessage *CreateMessage(ControllerMessageType type) const override;
			void ReceiveMessage(const ControllerMessage *message) override;
			void SendInitialStateMessages(Player *player) const override;

			void Wake(void) override;
			void Sleep(void) override;
			void Move(void) override;

			C4API void SetSpinSpeed(float speed, float time);
	};


	class GetSpinSpeedFunction final : public Function
	{
		private:

			GetSpinSpeedFunction(const GetSpinSpeedFunction& getSpinSpeedFunction);

			Function *Replicate(void) const override;

		public:

			GetSpinSpeedFunction();
			~GetSpinSpeedFunction();

			void Execute(Controller *controller, FunctionMethod *method, const ScriptState *state) override;
	};


	class SetSpinSpeedFunction final : public Function
	{
		private:

			float		spinSpeed;
			float		accelerationTime;

			SetSpinSpeedFunction(const SetSpinSpeedFunction& setSpinSpeedFunction);

			Function *Replicate(void) const override;

		public:

			SetSpinSpeedFunction();
			~SetSpinSpeedFunction();

			void Pack(Packer& data, unsigned_int32 packFlags) const override;
			void Unpack(Unpacker& data, unsigned_int32 unpackFlags) override;

			int32 GetSettingCount(void) const override;
			Setting *GetSetting(int32 index) const override;
			void SetSetting(const Setting *setting) override;

			void Execute(Controller *controller, FunctionMethod *method, const ScriptState *state) override;
	};


	class SpinStateMessage : public ControllerMessage
	{
		friend class SpinController;

		private:

			float		spinAngle;
			float		spinSpeed;

			float		currentSpeed;
			float		currentAcceleration;

			SpinStateMessage(int32 controllerIndex);

		public:

			SpinStateMessage(int32 controllerIndex, float angle, float speed, float currSpeed, float currAccel);
			~SpinStateMessage();

			void Compress(Compressor& data) const override;
			bool Decompress(Decompressor& data) override;
	};
}


#endif

// ZYUQURM
