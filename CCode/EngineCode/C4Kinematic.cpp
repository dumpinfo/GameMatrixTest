 

#include "C4Kinematic.h"
#include "C4Methods.h"
#include "C4Geometries.h"
#include "C4Zones.h"


using namespace C4;


KinematicController::KinematicController(ControllerType type) : Controller(type)
{
	SetBaseControllerType(kControllerKinematic);

	kinematicFlags = 0;
}

KinematicController::KinematicController(const KinematicController& kinematicController) : Controller(kinematicController)
{
	kinematicFlags = kinematicController.kinematicFlags;
}

KinematicController::~KinematicController()
{
}

void KinematicController::Pack(Packer& data, unsigned_int32 packFlags) const
{
	Controller::Pack(data, packFlags);

	data << ChunkHeader('FLAG', 4);
	data << kinematicFlags;

	data << TerminatorChunk;
}

void KinematicController::Unpack(Unpacker& data, unsigned_int32 unpackFlags)
{
	Controller::Unpack(data, unpackFlags);

	#if C4LEGACY

		if (data.GetVersion() >= 73)
		{

	#endif

			UnpackChunkList<KinematicController>(data, unpackFlags);

	#if C4LEGACY

		}

	#endif
}

bool KinematicController::UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags)
{
	switch (chunkHeader->chunkType)
	{
		case 'FLAG':

			data >> kinematicFlags;
			return (true);
	}

	return (false);
}

int32 KinematicController::GetSettingCount(void) const
{
	return (2);
}

Setting *KinematicController::GetSetting(int32 index) const
{
	const StringTable *table = TheInterfaceMgr->GetStringTable();

	if (index == 0)
	{
		const char *title = table->GetString(StringID('CTRL', kControllerKinematic, 'KINE'));
		return (new HeadingSetting('KINE', title));
	}

	if (index == 1)
	{
		const char *title = table->GetString(StringID('CTRL', kControllerKinematic, 'WKBD'));
		return (new BooleanSetting('WKBD', ((kinematicFlags & kKinematicWakeRigidBodies) != 0), title));
	}

	return (nullptr);
}

void KinematicController::SetSetting(const Setting *setting)
{
	Type identifier = setting->GetSettingIdentifier();

	if (identifier == 'WKBD')
	{
		if (static_cast<const BooleanSetting *>(setting)->GetBooleanValue())
		{
			kinematicFlags |= kKinematicWakeRigidBodies; 
		}
		else
		{ 
			kinematicFlags &= ~kKinematicWakeRigidBodies;
		} 
	}
}
 
void KinematicController::WakeCellRigidBodies(const Site *cell, const Box3D& box)
{ 
	const Bond *bond = cell->GetFirstOutgoingEdge(); 
	while (bond)
	{
		const Site *site = bond->GetFinishElement();
		if (site->GetWorldBoundingBox().Intersection(box)) 
		{
			if (site->GetCellIndex() < 0)
			{
				const Node *node = static_cast<const Node *>(site);
				if (node->Enabled())
				{
					Controller *controller = node->GetController();
					if ((controller) && (controller->GetBaseControllerType() == kControllerRigidBody) && (controller->Asleep()))
					{
						controller->Wake();
					}
				}
			}
			else
			{
				WakeCellRigidBodies(site, box);
			}
		}

		bond = bond->GetNextOutgoingEdge();
	}
}

void KinematicController::WakeZoneRigidBodies(Zone *zone, const Box3D& box)
{
	WakeCellRigidBodies(zone->GetCellGraphSite(kCellGraphGeometry), box);

	Zone *subzone = zone->GetFirstSubzone();
	while (subzone)
	{
		WakeZoneRigidBodies(subzone, box);
		subzone = subzone->Next();
	}
}

inline void KinematicController::WakeProximateRigidBodies(void)
{
	if (GetKinematicFlags() & kKinematicWakeRigidBodies)
	{
		const Node *node = GetTargetNode();
		WakeZoneRigidBodies(node->GetOwningZone(), node->GetWorldBoundingBox());
	}
}


MovementController::MovementController() : KinematicController(kControllerMovement)
{
	movementState = 0;
	currentDistance = 0.0F;

	targetSpeed = 0.0F;
	currentSpeed = 0.0F;
	currentAcceleration = 0.0F;

	movementSpeed = 1.0F;
	accelerationTime = 0.0F;
	decelerationTime = 0.0F;

	startConnectorKey = "Start";
	finishConnectorKey = "Finish";
}

MovementController::MovementController(const MovementController& movementController) : KinematicController(movementController)
{
	movementState = 0;
	currentDistance = 0.0F;

	targetSpeed = 0.0F;
	currentSpeed = 0.0F;
	currentAcceleration = 0.0F;

	movementSpeed = movementController.movementSpeed;
	accelerationTime = movementController.accelerationTime;
	decelerationTime = movementController.decelerationTime;

	startConnectorKey = movementController.startConnectorKey;
	finishConnectorKey = movementController.finishConnectorKey;
}

MovementController::~MovementController()
{
}

Controller *MovementController::Replicate(void) const
{
	return (new MovementController(*this));
}

void MovementController::RegisterFunctions(ControllerRegistration *registration)
{
	const StringTable *table = TheInterfaceMgr->GetStringTable();

	static FunctionReg<GetMovementSpeedFunction> getMovementSpeedRegistration(registration, kFunctionGetMovementSpeed, table->GetString(StringID('CTRL', kControllerMovement, kFunctionGetMovementSpeed)), kFunctionOutputValue);
	static FunctionReg<SetMovementSpeedFunction> setMovementSpeedRegistration(registration, kFunctionSetMovementSpeed, table->GetString(StringID('CTRL', kControllerMovement, kFunctionSetMovementSpeed)));
	static FunctionReg<MoveToStartFunction> moveToStartRegistration(registration, kFunctionMoveToStart, table->GetString(StringID('CTRL', kControllerMovement, kFunctionMoveToStart)));
	static FunctionReg<MoveToFinishFunction> moveToFinishRegistration(registration, kFunctionMoveToFinish, table->GetString(StringID('CTRL', kControllerMovement, kFunctionMoveToFinish)));
}

void MovementController::Pack(Packer& data, unsigned_int32 packFlags) const
{
	KinematicController::Pack(data, packFlags);

	data << ChunkHeader('STAT', 4);
	data << movementState;

	data << ChunkHeader('MSPD', 4);
	data << movementSpeed;

	data << ChunkHeader('ACCT', 4);
	data << accelerationTime;

	data << ChunkHeader('DECT', 4);
	data << decelerationTime;

	PackHandle handle = data.BeginChunk('SKEY');
	data << startConnectorKey;
	data.EndChunk(handle);

	handle = data.BeginChunk('FKEY');
	data << finishConnectorKey;
	data.EndChunk(handle);

	if (movementState & kMovementInitialized)
	{
		data << ChunkHeader('DIST', 4);
		data << currentDistance;

		data << ChunkHeader('SPED', 8);
		data << targetSpeed;
		data << currentSpeed;

		data << ChunkHeader('ACCL', 4);
		data << currentAcceleration;

		data << ChunkHeader('MOVE', 4);
		data << movementDistance;

		data << ChunkHeader('OFST', sizeof(Vector3D));
		data << originalNodeOffset;
	}

	data << TerminatorChunk;
}

void MovementController::Unpack(Unpacker& data, unsigned_int32 unpackFlags)
{
	KinematicController::Unpack(data, unpackFlags);
	UnpackChunkList<MovementController>(data, unpackFlags);
}

bool MovementController::UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags)
{
	switch (chunkHeader->chunkType)
	{
		case 'STAT':

			data >> movementState;
			return (true);

		case 'MSPD':

			data >> movementSpeed;
			return (true);

		case 'ACCT':

			data >> accelerationTime;
			return (true);

		case 'DECT':

			data >> decelerationTime;
			return (true);

		case 'SKEY':

			data >> startConnectorKey;
			return (true);

		case 'FKEY':

			data >> finishConnectorKey;
			return (true);

		case 'DIST':

			data >> currentDistance;
			return (true);

		case 'SPED':

			data >> targetSpeed;
			data >> currentSpeed;
			return (true);

		case 'ACCL':

			data >> currentAcceleration;
			return (true);

		case 'MOVE':

			data >> movementDistance;
			return (true);

		case 'OFST':

			data >> originalNodeOffset;
			return (true);
	}

	return (false);
}

int32 MovementController::GetSettingCount(void) const
{
	return (KinematicController::GetSettingCount() + 6);
}

Setting *MovementController::GetSetting(int32 index) const
{
	int32 count = KinematicController::GetSettingCount();
	if (index < count)
	{
		return (KinematicController::GetSetting(index));
	}

	const StringTable *table = TheInterfaceMgr->GetStringTable();

	if (index == count)
	{
		const char *title = table->GetString(StringID('CTRL', kControllerMovement, 'MOVE'));
		return (new HeadingSetting('MOVE', title));
	}

	if (index == count + 1)
	{
		const char *title = table->GetString(StringID('CTRL', kControllerMovement, 'MSPD'));
		return (new TextSetting('MSPD', movementSpeed, title));
	}

	if (index == count + 2)
	{
		const char *title = table->GetString(StringID('CTRL', kControllerMovement, 'ACCT'));
		return (new TextSetting('ACCT', accelerationTime, title));
	}

	if (index == count + 3)
	{
		const char *title = table->GetString(StringID('CTRL', kControllerMovement, 'DECT'));
		return (new TextSetting('DECT', decelerationTime, title));
	}

	if (index == count + 4)
	{
		const char *title = table->GetString(StringID('CTRL', kControllerMovement, 'SKEY'));
		return (new TextSetting('SKEY', startConnectorKey, title, kMaxConnectorKeyLength, &Connector::ConnectorKeyFilter));
	}

	if (index == count + 5)
	{
		const char *title = table->GetString(StringID('CTRL', kControllerMovement, 'FKEY'));
		return (new TextSetting('FKEY', finishConnectorKey, title, kMaxConnectorKeyLength, &Connector::ConnectorKeyFilter));
	}

	return (nullptr);
}

void MovementController::SetSetting(const Setting *setting)
{
	Type identifier = setting->GetSettingIdentifier();

	if (identifier == 'MSPD')
	{
		const char *text = static_cast<const TextSetting *>(setting)->GetText();
		movementSpeed = FmaxZero(Text::StringToFloat(text));
	}
	else if (identifier == 'ACCT')
	{
		const char *text = static_cast<const TextSetting *>(setting)->GetText();
		accelerationTime = FmaxZero(Text::StringToFloat(text));
	}
	else if (identifier == 'DECT')
	{
		const char *text = static_cast<const TextSetting *>(setting)->GetText();
		decelerationTime = FmaxZero(Text::StringToFloat(text));
	}
	else if (identifier == 'SKEY')
	{
		startConnectorKey = static_cast<const TextSetting *>(setting)->GetText();
	}
	else if (identifier == 'FKEY')
	{
		finishConnectorKey = static_cast<const TextSetting *>(setting)->GetText();
	}
	else
	{
		KinematicController::SetSetting(setting);
	}
}

void MovementController::Preprocess(void)
{
	Node *node = GetTargetNode();
	node->SetNodeFlags(node->GetNodeFlags() | kNodeVisibilitySite);

	if (!node->GetManipulator())
	{
		unsigned_int32 state = movementState;
		const Node *start = node->GetConnectedNode(startConnectorKey);
		const Node *finish = node->GetConnectedNode(finishConnectorKey);

		if (!(state & kMovementInitialized))
		{
			SetControllerFlags(GetControllerFlags() | kControllerAsleep);

			if ((start) && (finish))
			{
				float distance = Magnitude(finish->GetWorldPosition() - start->GetWorldPosition());
				if (distance > K::min_float)
				{
					movementState = state | kMovementInitialized;

					movementDistance = distance;
					originalNodeOffset = node->GetWorldPosition() - start->GetWorldPosition();
				}
			}
		}

		if (movementState & kMovementInitialized)
		{
			startPosition = &start->GetWorldPosition();
			finishPosition = &finish->GetWorldPosition();
			inverseMovementDistance = 1.0F / movementDistance;
			CalculateMovementParameters();
		}

		const Node *subnode = node;
		do
		{
			if (subnode->GetNodeType() == kNodeGeometry)
			{
				GeometryObject *object = static_cast<const Geometry *>(subnode)->GetObject();
				object->SetGeometryFlags(object->GetGeometryFlags() | kGeometryDynamic);
			}

			subnode = node->GetNextNode(subnode);
		} while (subnode);
	}

	KinematicController::Preprocess();
}

bool MovementController::InstanceExtractable(void) const
{
	return (true);
}

ControllerMessage *MovementController::CreateMessage(ControllerMessageType type) const
{
	if (type == kMovementMessageState)
	{
		return (new MovementStateMessage(GetControllerIndex()));
	}

	return (KinematicController::CreateMessage(type));
}

void MovementController::ReceiveMessage(const ControllerMessage *message)
{
	if (message->GetControllerMessageType() == kMovementMessageState)
	{
		const MovementStateMessage *stateMessage = static_cast<const MovementStateMessage *>(message);

		targetSpeed = stateMessage->targetSpeed;
		currentSpeed = stateMessage->currentSpeed;
		currentAcceleration = stateMessage->currentAcceleration;

		movementSpeed = stateMessage->movementSpeed;
		accelerationTime = stateMessage->accelerationTime;
		decelerationTime = stateMessage->decelerationTime;

		CalculateMovementParameters();

		if (movementState & kMovementInitialized)
		{
			UpdateNodeDistance(stateMessage->currentDistance);

			if (targetSpeed != 0.0F)
			{
				Wake();
			}
			else
			{
				Sleep();
			}
		}
	}
	else
	{
		KinematicController::ReceiveMessage(message);
	}
}

void MovementController::SendInitialStateMessages(Player *player) const
{
	player->SendMessage(MovementStateMessage(GetControllerIndex(), currentDistance, targetSpeed, currentSpeed, currentAcceleration, movementSpeed, accelerationTime, decelerationTime));
}

void MovementController::CalculateMovementParameters(void)
{
	minMovementSpeed = movementSpeed * 0.01F;

	if (decelerationTime > K::min_float)
	{
		decelerationRate = movementSpeed / decelerationTime;
		decelerationDistance = movementSpeed * decelerationTime * 0.5F;
	}
	else
	{
		decelerationRate = 0.0F;
		decelerationDistance = 0.0F;
	}
}

void MovementController::CalculateForwardParameters(void)
{
	float speed = movementSpeed;
	targetSpeed = speed;

	if (accelerationTime > K::min_float)
	{
		currentSpeed = 0.0F;
		currentAcceleration = speed / accelerationTime;
	}
	else
	{
		currentSpeed = speed;
		currentAcceleration = 0.0F;
	}
}

void MovementController::CalculateBackwardParameters(void)
{
	float speed = -movementSpeed;
	targetSpeed = speed;

	if (accelerationTime > K::min_float)
	{
		currentSpeed = 0.0F;
		currentAcceleration = speed / accelerationTime;
	}
	else
	{
		currentSpeed = speed;
		currentAcceleration = 0.0F;
	}
}

void MovementController::UpdateNodeDistance(float distance)
{
	Node *node = GetTargetNode();
	const Transform4D& inverseTransform = node->GetSuperNode()->GetInverseWorldTransform();

	currentDistance = distance;
	float t = distance * inverseMovementDistance;

	node->SetNodePosition(inverseTransform * (*startPosition * (1.0F - t) + *finishPosition * t + originalNodeOffset));
	node->Invalidate();

	SetGeometryVelocity((*finishPosition - *startPosition) * (inverseMovementDistance * currentSpeed));
}

void MovementController::SetGeometryVelocity(const Vector3D& velocity) const
{
	Node *node = GetTargetNode();
	Node *subnode = node;
	do
	{
		if (subnode->GetNodeType() == kNodeGeometry)
		{
			static_cast<Geometry *>(subnode)->SetGeometryVelocity(velocity);
		}

		subnode = node->GetNextNode(subnode);
	} while (subnode);
}

void MovementController::Move(void)
{
	WakeProximateRigidBodies();

	ObservableEventType eventType = 0;
	float dt = TheTimeMgr->GetDeltaSeconds();

	float distance = currentDistance;
	float target = targetSpeed;
	float speed = currentSpeed;

	if (!(target < 0.0F))
	{
		speed = Clamp(speed + currentAcceleration * dt, minMovementSpeed, target);
		distance += speed * dt;

		if (!(distance < movementDistance))
		{
			distance = movementDistance;
			eventType = kEventMovementReachedFinish;
		}
		else if (distance > movementDistance - decelerationDistance)
		{
			currentAcceleration = -decelerationRate;
		}
	}
	else
	{
		speed = Clamp(speed + currentAcceleration * dt, target, -minMovementSpeed);
		distance += speed * dt;

		if (!(distance > 0.0F))
		{
			distance = 0.0F;
			eventType = kEventMovementReachedStart;
		}
		else if (distance < decelerationDistance)
		{
			currentAcceleration = decelerationRate;
		}
	}

	currentSpeed = speed;
	UpdateNodeDistance(distance);

	if ((eventType != 0) && (TheMessageMgr->Server()))
	{
		targetSpeed = 0.0F;
		currentSpeed = 0.0F;
		currentAcceleration = 0.0F;

		Sleep();

		TheMessageMgr->SendMessageClients(MovementStateMessage(GetControllerIndex(), distance, 0.0F, 0.0F, 0.0F, movementSpeed, accelerationTime, decelerationTime));
		PostEvent(eventType);
	}
}

void MovementController::Wake(void)
{
	if (movementState & kMovementInitialized)
	{
		KinematicController::Wake();
	}
}

void MovementController::Sleep(void)
{
	SetGeometryVelocity(Zero3D);
	GetTargetNode()->StopMotion();
	KinematicController::Sleep();
}

void MovementController::Activate(Node *initiator, Node *trigger)
{
	MoveToFinish();
}

void MovementController::SetMovementSpeed(float speed)
{
	movementSpeed = speed;
	CalculateMovementParameters();

	if (!Asleep())
	{
		if (!(targetSpeed < 0.0F))
		{
			CalculateForwardParameters();
		}
		else
		{
			CalculateBackwardParameters();
		}
	}

	TheMessageMgr->SendMessageClients(MovementStateMessage(GetControllerIndex(), currentDistance, targetSpeed, currentSpeed, currentAcceleration, movementSpeed, accelerationTime, decelerationTime));
}

void MovementController::MoveToStart(void)
{
	CalculateBackwardParameters();
	Wake();

	TheMessageMgr->SendMessageClients(MovementStateMessage(GetControllerIndex(), currentDistance, targetSpeed, currentSpeed, currentAcceleration, movementSpeed, accelerationTime, decelerationTime));
}

void MovementController::MoveToFinish(void)
{
	CalculateForwardParameters();
	Wake();

	TheMessageMgr->SendMessageClients(MovementStateMessage(GetControllerIndex(), currentDistance, targetSpeed, currentSpeed, currentAcceleration, movementSpeed, accelerationTime, decelerationTime));
}


GetMovementSpeedFunction::GetMovementSpeedFunction() : Function(kFunctionGetMovementSpeed, kControllerMovement)
{
}

GetMovementSpeedFunction::GetMovementSpeedFunction(const GetMovementSpeedFunction& getMovementSpeedFunction) : Function(getMovementSpeedFunction)
{
}

GetMovementSpeedFunction::~GetMovementSpeedFunction()
{
}

Function *GetMovementSpeedFunction::Replicate(void) const
{
	return (new GetMovementSpeedFunction(*this));
}

void GetMovementSpeedFunction::Execute(Controller *controller, FunctionMethod *method, const ScriptState *state)
{
	method->SetOutputValue(state, static_cast<const MovementController *>(controller)->GetMovementSpeed());
	CallCompletionProc();
}


SetMovementSpeedFunction::SetMovementSpeedFunction() : Function(kFunctionSetMovementSpeed, kControllerMovement)
{
	movementSpeed = 1.0F;
}

SetMovementSpeedFunction::SetMovementSpeedFunction(const SetMovementSpeedFunction& setMovementSpeedFunction) : Function(setMovementSpeedFunction)
{
	movementSpeed = setMovementSpeedFunction.movementSpeed;
}

SetMovementSpeedFunction::~SetMovementSpeedFunction()
{
}

Function *SetMovementSpeedFunction::Replicate(void) const
{
	return (new SetMovementSpeedFunction(*this));
}

void SetMovementSpeedFunction::Pack(Packer& data, unsigned_int32 packFlags) const
{
	Function::Pack(data, packFlags);

	data << movementSpeed;
}

void SetMovementSpeedFunction::Unpack(Unpacker& data, unsigned_int32 unpackFlags)
{
	Function::Unpack(data, unpackFlags);

	data >> movementSpeed;
}

int32 SetMovementSpeedFunction::GetSettingCount(void) const
{
	return (1);
}

Setting *SetMovementSpeedFunction::GetSetting(int32 index) const
{
	const StringTable *table = TheInterfaceMgr->GetStringTable();

	if (index == 0)
	{
		const char *title = table->GetString(StringID('CTRL', kControllerMovement, kFunctionSetMovementSpeed, 'MSPD'));
		return (new TextSetting('MSPD', movementSpeed, title));
	}

	return (nullptr);
}

void SetMovementSpeedFunction::SetSetting(const Setting *setting)
{
	Type identifier = setting->GetSettingIdentifier();

	if (identifier == 'MSPD')
	{
		const char *text = static_cast<const TextSetting *>(setting)->GetText();
		movementSpeed = FmaxZero(Text::StringToFloat(text));
	}
}

void SetMovementSpeedFunction::Execute(Controller *controller, FunctionMethod *method, const ScriptState *state)
{
	MovementController *movementController = static_cast<MovementController *>(controller);
	movementController->SetMovementSpeed(movementSpeed);
	CallCompletionProc();
}


MoveToStartFunction::MoveToStartFunction() :
		Function(kFunctionMoveToStart, kControllerMovement),
		Observer<MoveToStartFunction, Controller>(this, &MoveToStartFunction::HandleControllerEvent)
{
}

MoveToStartFunction::MoveToStartFunction(const MoveToStartFunction& moveToStartFunction) :
		Function(moveToStartFunction),
		Observer<MoveToStartFunction, Controller>(this, &MoveToStartFunction::HandleControllerEvent)
{
}

MoveToStartFunction::~MoveToStartFunction()
{
}

Function *MoveToStartFunction::Replicate(void) const
{
	return (new MoveToStartFunction(*this));
}

void MoveToStartFunction::Execute(Controller *controller, FunctionMethod *method, const ScriptState *state)
{
	MovementController *movementController = static_cast<MovementController *>(controller);
	movementController->AddObserver(this);
	movementController->MoveToStart();
}

void MoveToStartFunction::HandleControllerEvent(Controller *controller, Controller::ObservableEventType event)
{
	if (event == MovementController::kEventMovementReachedStart)
	{
		controller->RemoveObserver(this);
		CallCompletionProc();
	}
}


MoveToFinishFunction::MoveToFinishFunction() :
		Function(kFunctionMoveToFinish, kControllerMovement),
		Observer<MoveToFinishFunction, Controller>(this, &MoveToFinishFunction::HandleControllerEvent)
{
}

MoveToFinishFunction::MoveToFinishFunction(const MoveToFinishFunction& moveToFinishFunction) :
		Function(moveToFinishFunction),
		Observer<MoveToFinishFunction, Controller>(this, &MoveToFinishFunction::HandleControllerEvent)
{
}

MoveToFinishFunction::~MoveToFinishFunction()
{
}

Function *MoveToFinishFunction::Replicate(void) const
{
	return (new MoveToFinishFunction(*this));
}

void MoveToFinishFunction::Execute(Controller *controller, FunctionMethod *method, const ScriptState *state)
{
	MovementController *movementController = static_cast<MovementController *>(controller);
	movementController->AddObserver(this);
	movementController->MoveToFinish();
}

void MoveToFinishFunction::HandleControllerEvent(Controller *controller, Controller::ObservableEventType event)
{
	if (event == MovementController::kEventMovementReachedFinish)
	{
		controller->RemoveObserver(this);
		CallCompletionProc();
	}
}


MovementStateMessage::MovementStateMessage(int32 controllerIndex) : ControllerMessage(MovementController::kMovementMessageState, controllerIndex)
{
}

MovementStateMessage::MovementStateMessage(int32 controllerIndex, float distance, float targSpeed, float currSpeed, float currAccel, float moveSpeed, float accelTime, float decelTime) : ControllerMessage(MovementController::kMovementMessageState, controllerIndex)
{
	currentDistance = distance;

	targetSpeed = targSpeed;
	currentSpeed = currSpeed;
	currentAcceleration = currAccel;

	movementSpeed = moveSpeed;
	accelerationTime = accelTime;
	decelerationTime = decelTime;
}

MovementStateMessage::~MovementStateMessage()
{
}

void MovementStateMessage::Compress(Compressor& data) const
{
	ControllerMessage::Compress(data);

	data << currentDistance;

	data << targetSpeed;
	data << currentSpeed;
	data << currentAcceleration;

	data << movementSpeed;
	data << accelerationTime;
	data << decelerationTime;
}

bool MovementStateMessage::Decompress(Decompressor& data)
{
	if (ControllerMessage::Decompress(data))
	{
		data >> currentDistance;

		data >> targetSpeed;
		data >> currentSpeed;
		data >> currentAcceleration;

		data >> movementSpeed;
		data >> accelerationTime;
		data >> decelerationTime;

		return (true);
	}

	return (false);
}


OscillationController::OscillationController() : KinematicController(kControllerOscillation)
{
	oscillationState = 0;
	oscillationAngle = 0.0F;
	oscillationSpeed = 0.0F;

	currentSpeed = 0.0F;
	currentAcceleration = 0.0F;

	startConnectorKey = "Start";
	finishConnectorKey = "Finish";
}

OscillationController::OscillationController(const OscillationController& oscillationController) : KinematicController(oscillationController)
{
	oscillationState = 0;
	oscillationAngle = 0.0F;
	oscillationSpeed = oscillationController.oscillationSpeed;

	currentSpeed = 0.0F;
	currentAcceleration = 0.0F;

	startConnectorKey = oscillationController.startConnectorKey;
	finishConnectorKey = oscillationController.finishConnectorKey;
}

OscillationController::~OscillationController()
{
}

Controller *OscillationController::Replicate(void) const
{
	return (new OscillationController(*this));
}

void OscillationController::RegisterFunctions(ControllerRegistration *registration)
{
	const StringTable *table = TheInterfaceMgr->GetStringTable();

	static FunctionReg<GetOscillationSpeedFunction> getOscillationSpeedRegistration(registration, kFunctionGetOscillationSpeed, table->GetString(StringID('CTRL', kControllerOscillation, kFunctionGetOscillationSpeed)), kFunctionOutputValue);
	static FunctionReg<SetOscillationSpeedFunction> setOscillationSpeedRegistration(registration, kFunctionSetOscillationSpeed, table->GetString(StringID('CTRL', kControllerOscillation, kFunctionSetOscillationSpeed)));
}

void OscillationController::Pack(Packer& data, unsigned_int32 packFlags) const
{
	KinematicController::Pack(data, packFlags);

	data << ChunkHeader('STAT', 4);
	data << oscillationState;

	data << ChunkHeader('OSPD', 4);
	data << oscillationSpeed;

	PackHandle handle = data.BeginChunk('SKEY');
	data << startConnectorKey;
	data.EndChunk(handle);

	handle = data.BeginChunk('FKEY');
	data << finishConnectorKey;
	data.EndChunk(handle);

	if (oscillationState & kOscillationInitialized)
	{
		data << ChunkHeader('ANGL', 4);
		data << oscillationAngle;

		data << ChunkHeader('SPED', 4);
		data << currentSpeed;

		data << ChunkHeader('ACCL', 4);
		data << currentAcceleration;

		data << ChunkHeader('OFST', sizeof(Vector3D));
		data << originalNodeOffset;
	}

	data << TerminatorChunk;
}

void OscillationController::Unpack(Unpacker& data, unsigned_int32 unpackFlags)
{
	KinematicController::Unpack(data, unpackFlags);
	UnpackChunkList<OscillationController>(data, unpackFlags);
}

bool OscillationController::UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags)
{
	switch (chunkHeader->chunkType)
	{
		case 'STAT':

			data >> oscillationState;
			return (true);

		case 'OSPD':

			data >> oscillationSpeed;
			return (true);

		case 'SKEY':

			data >> startConnectorKey;
			return (true);

		case 'FKEY':

			data >> finishConnectorKey;
			return (true);

		case 'ANGL':

			data >> oscillationAngle;
			return (true);

		case 'SPED':

			data >> currentSpeed;
			return (true);

		case 'ACCL':

			data >> currentAcceleration;
			return (true);

		case 'OFST':

			data >> originalNodeOffset;
			return (true);
	}

	return (false);
}

int32 OscillationController::GetSettingCount(void) const
{
	return (KinematicController::GetSettingCount() + 4);
}

Setting *OscillationController::GetSetting(int32 index) const
{
	int32 count = KinematicController::GetSettingCount();
	if (index < count)
	{
		return (KinematicController::GetSetting(index));
	}

	const StringTable *table = TheInterfaceMgr->GetStringTable();

	if (index == count)
	{
		const char *title = table->GetString(StringID('CTRL', kControllerOscillation, 'OSCL'));
		return (new HeadingSetting('OSCL', title));
	}

	if (index == count + 1)
	{
		const char *title = table->GetString(StringID('CTRL', kControllerOscillation, 'OSSP'));
		return (new TextSetting('OSSP', oscillationSpeed, title));
	}

	if (index == count + 2)
	{
		const char *title = table->GetString(StringID('CTRL', kControllerOscillation, 'SKEY'));
		return (new TextSetting('SKEY', startConnectorKey, title, kMaxConnectorKeyLength, &Connector::ConnectorKeyFilter));
	}

	if (index == count + 3)
	{
		const char *title = table->GetString(StringID('CTRL', kControllerOscillation, 'FKEY'));
		return (new TextSetting('FKEY', finishConnectorKey, title, kMaxConnectorKeyLength, &Connector::ConnectorKeyFilter));
	}

	return (nullptr);
}

void OscillationController::SetSetting(const Setting *setting)
{
	Type identifier = setting->GetSettingIdentifier();

	if (identifier == 'OSSP')
	{
		const char *text = static_cast<const TextSetting *>(setting)->GetText();
		oscillationSpeed = Text::StringToFloat(text);
	}
	else if (identifier == 'SKEY')
	{
		startConnectorKey = static_cast<const TextSetting *>(setting)->GetText();
	}
	else if (identifier == 'FKEY')
	{
		finishConnectorKey = static_cast<const TextSetting *>(setting)->GetText();
	}
	else
	{
		KinematicController::SetSetting(setting);
	}
}

void OscillationController::Preprocess(void)
{
	Node *node = GetTargetNode();
	node->SetNodeFlags(node->GetNodeFlags() | kNodeVisibilitySite);

	if (!node->GetManipulator())
	{
		unsigned_int32 state = oscillationState;
		const Node *start = node->GetConnectedNode(startConnectorKey);
		const Node *finish = node->GetConnectedNode(finishConnectorKey);

		if (!(state & kOscillationInitialized))
		{
			if (oscillationSpeed == 0.0F)
			{
				SetControllerFlags(GetControllerFlags() | kControllerAsleep);
			}

			if ((start) && (finish))
			{
				float distance = Magnitude(finish->GetWorldPosition() - start->GetWorldPosition());
				if (distance > K::min_float)
				{
					oscillationState = state | kOscillationInitialized;
					currentSpeed = oscillationSpeed;

					originalNodeOffset = node->GetWorldPosition() - start->GetWorldPosition();
				}
			}
		}

		if (oscillationState & kOscillationInitialized)
		{
			startPosition = &start->GetWorldPosition();
			finishPosition = &finish->GetWorldPosition();
		}

		const Node *subnode = node;
		do
		{
			if (subnode->GetNodeType() == kNodeGeometry)
			{
				GeometryObject *object = static_cast<const Geometry *>(subnode)->GetObject();
				object->SetGeometryFlags(object->GetGeometryFlags() | kGeometryDynamic);
			}

			subnode = node->GetNextNode(subnode);
		} while (subnode);
	}

	KinematicController::Preprocess();
}

bool OscillationController::InstanceExtractable(void) const
{
	return (true);
}

ControllerMessage *OscillationController::CreateMessage(ControllerMessageType type) const
{
	if (type == kOscillationMessageState)
	{
		return (new OscillationStateMessage(GetControllerIndex()));
	}

	return (KinematicController::CreateMessage(type));
}

void OscillationController::ReceiveMessage(const ControllerMessage *message)
{
	if (message->GetControllerMessageType() == kOscillationMessageState)
	{
		const OscillationStateMessage *stateMessage = static_cast<const OscillationStateMessage *>(message);

		oscillationSpeed = stateMessage->oscillationSpeed;
		currentSpeed = stateMessage->currentSpeed;
		currentAcceleration = stateMessage->currentAcceleration;

		if (oscillationState & kOscillationInitialized)
		{
			UpdateNodePosition(stateMessage->oscillationAngle);

			if (oscillationSpeed != 0.0F)
			{
				Wake();
			}
		}
	}
	else
	{
		KinematicController::ReceiveMessage(message);
	}
}

void OscillationController::SendInitialStateMessages(Player *player) const
{
	player->SendMessage(OscillationStateMessage(GetControllerIndex(), oscillationAngle, oscillationSpeed, currentSpeed, currentAcceleration));
}

void OscillationController::UpdateNodePosition(float angle)
{
	Node *node = GetTargetNode();
	const Transform4D& inverseTransform = node->GetSuperNode()->GetInverseWorldTransform();

	oscillationAngle = angle;
	Vector2D cs = CosSin(angle * K::tau_over_2);
	float t = cs.y * cs.y;

	node->SetNodePosition(inverseTransform * (*startPosition * (1.0F - t) + *finishPosition * t + originalNodeOffset));
	node->Invalidate();

	SetGeometryVelocity((*finishPosition - *startPosition) * ((cs.x * cs.y) * (currentSpeed * K::tau)));
}

void OscillationController::SetGeometryVelocity(const Vector3D& velocity) const
{
	Node *node = GetTargetNode();
	Node *subnode = node;
	do
	{
		if (subnode->GetNodeType() == kNodeGeometry)
		{
			static_cast<Geometry *>(subnode)->SetGeometryVelocity(velocity);
		}

		subnode = node->GetNextNode(subnode);
	} while (subnode);
}

void OscillationController::Move(void)
{
	WakeProximateRigidBodies();

	float dt = TheTimeMgr->GetDeltaSeconds();
	float target = oscillationSpeed;
	float speed = currentSpeed;

	if ((target > 0.0F) || (currentAcceleration > 0.0F))
	{
		speed = Fmin(speed + currentAcceleration * dt, target);
	}
	else
	{
		speed = Fmax(speed + currentAcceleration * dt, target);
	}

	currentSpeed = speed;
	float angle = Frac(oscillationAngle + speed * dt);
	UpdateNodePosition(angle);

	if ((speed == 0.0F) && (target == 0.0F))
	{
		if (TheMessageMgr->Server())
		{
			currentAcceleration = 0.0F;
			TheMessageMgr->SendMessageClients(OscillationStateMessage(GetControllerIndex(), angle, 0.0F, 0.0F, 0.0F));
		}

		Sleep();
	}
}

void OscillationController::Wake(void)
{
	if (oscillationState & kOscillationInitialized)
	{
		KinematicController::Wake();
	}
}

void OscillationController::Sleep(void)
{
	GetTargetNode()->StopMotion();
	KinematicController::Sleep();
}

void OscillationController::SetOscillationSpeed(float speed, float time)
{
	if (time > K::min_float)
	{
		currentAcceleration = (speed - oscillationSpeed) / time;
	}
	else
	{
		currentSpeed = speed;
		currentAcceleration = 0.0F;
	}

	oscillationSpeed = speed;
	if (speed != 0.0F)
	{
		Wake();
	}

	TheMessageMgr->SendMessageClients(OscillationStateMessage(GetControllerIndex(), oscillationAngle, oscillationSpeed, currentSpeed, currentAcceleration));
}


GetOscillationSpeedFunction::GetOscillationSpeedFunction() : Function(kFunctionGetOscillationSpeed, kControllerOscillation)
{
}

GetOscillationSpeedFunction::GetOscillationSpeedFunction(const GetOscillationSpeedFunction& getOscillationSpeedFunction) : Function(getOscillationSpeedFunction)
{
}

GetOscillationSpeedFunction::~GetOscillationSpeedFunction()
{
}

Function *GetOscillationSpeedFunction::Replicate(void) const
{
	return (new GetOscillationSpeedFunction(*this));
}

void GetOscillationSpeedFunction::Execute(Controller *controller, FunctionMethod *method, const ScriptState *state)
{
	method->SetOutputValue(state, static_cast<const OscillationController *>(controller)->GetOscillationSpeed());
	CallCompletionProc();
}


SetOscillationSpeedFunction::SetOscillationSpeedFunction() : Function(kFunctionSetOscillationSpeed, kControllerOscillation)
{
	oscillationSpeed = 1.0F;
	accelerationTime = 0.0F;
}

SetOscillationSpeedFunction::SetOscillationSpeedFunction(const SetOscillationSpeedFunction& setOscillationSpeedFunction) : Function(setOscillationSpeedFunction)
{
	oscillationSpeed = setOscillationSpeedFunction.oscillationSpeed;
	accelerationTime = setOscillationSpeedFunction.accelerationTime;
}

SetOscillationSpeedFunction::~SetOscillationSpeedFunction()
{
}

Function *SetOscillationSpeedFunction::Replicate(void) const
{
	return (new SetOscillationSpeedFunction(*this));
}

void SetOscillationSpeedFunction::Pack(Packer& data, unsigned_int32 packFlags) const
{
	Function::Pack(data, packFlags);

	data << oscillationSpeed;
	data << accelerationTime;
}

void SetOscillationSpeedFunction::Unpack(Unpacker& data, unsigned_int32 unpackFlags)
{
	Function::Unpack(data, unpackFlags);

	data >> oscillationSpeed;
	data >> accelerationTime;
}

int32 SetOscillationSpeedFunction::GetSettingCount(void) const
{
	return (2);
}

Setting *SetOscillationSpeedFunction::GetSetting(int32 index) const
{
	const StringTable *table = TheInterfaceMgr->GetStringTable();

	if (index == 0)
	{
		const char *title = table->GetString(StringID('CTRL', kControllerOscillation, kFunctionSetOscillationSpeed, 'OSSP'));
		return (new TextSetting('OSSP', oscillationSpeed, title));
	}

	if (index == 1)
	{
		const char *title = table->GetString(StringID('CTRL', kControllerOscillation, kFunctionSetOscillationSpeed, 'ACCT'));
		return (new TextSetting('ACCT', accelerationTime, title));
	}

	return (nullptr);
}

void SetOscillationSpeedFunction::SetSetting(const Setting *setting)
{
	Type identifier = setting->GetSettingIdentifier();

	if (identifier == 'OSSP')
	{
		const char *text = static_cast<const TextSetting *>(setting)->GetText();
		oscillationSpeed = Text::StringToFloat(text);
	}
	else if (identifier == 'ACCT')
	{
		const char *text = static_cast<const TextSetting *>(setting)->GetText();
		accelerationTime = FmaxZero(Text::StringToFloat(text));
	}
}

void SetOscillationSpeedFunction::Execute(Controller *controller, FunctionMethod *method, const ScriptState *state)
{
	OscillationController *oscillationController = static_cast<OscillationController *>(controller);
	oscillationController->SetOscillationSpeed(oscillationSpeed, accelerationTime);
	CallCompletionProc();
}


OscillationStateMessage::OscillationStateMessage(int32 controllerIndex) : ControllerMessage(OscillationController::kOscillationMessageState, controllerIndex)
{
}

OscillationStateMessage::OscillationStateMessage(int32 controllerIndex, float angle, float speed, float currSpeed, float currAccel) : ControllerMessage(OscillationController::kOscillationMessageState, controllerIndex)
{
	oscillationAngle = angle;
	oscillationSpeed = speed;

	currentSpeed = currSpeed;
	currentAcceleration = currAccel;
}

OscillationStateMessage::~OscillationStateMessage()
{
}

void OscillationStateMessage::Compress(Compressor& data) const
{
	ControllerMessage::Compress(data);

	data << oscillationAngle;
	data << oscillationSpeed;

	data << currentSpeed;
	data << currentAcceleration;
}

bool OscillationStateMessage::Decompress(Decompressor& data)
{
	if (ControllerMessage::Decompress(data))
	{
		data >> oscillationAngle;
		data >> oscillationSpeed;

		data >> currentSpeed;
		data >> currentAcceleration;

		return (true);
	}

	return (false);
}


RotationController::RotationController() : KinematicController(kControllerRotation)
{
	rotationState = 0;
	currentAngle = 0.0F;

	targetSpeed = 0.0F;
	currentSpeed = 0.0F;
	currentAcceleration = 0.0F;

	rotationAngle = 90.0F;
	rotationSpeed = 90.0F;
	accelerationTime = 0.0F;
	decelerationTime = 0.0F;

	centerConnectorKey = "Center";
}

RotationController::RotationController(const RotationController& rotationController) : KinematicController(rotationController)
{
	rotationState = 0;
	currentAngle = 0.0F;

	targetSpeed = 0.0F;
	currentSpeed = 0.0F;
	currentAcceleration = 0.0F;

	rotationAngle = rotationController.rotationAngle;
	rotationSpeed = rotationController.rotationSpeed;
	accelerationTime = rotationController.accelerationTime;
	decelerationTime = rotationController.decelerationTime;

	centerConnectorKey = rotationController.centerConnectorKey;
}

RotationController::~RotationController()
{
}

Controller *RotationController::Replicate(void) const
{
	return (new RotationController(*this));
}

void RotationController::RegisterFunctions(ControllerRegistration *registration)
{
	const StringTable *table = TheInterfaceMgr->GetStringTable();

	static FunctionReg<GetRotationSpeedFunction> getRotationSpeedRegistration(registration, kFunctionGetRotationSpeed, table->GetString(StringID('CTRL', kControllerRotation, kFunctionGetRotationSpeed)), kFunctionOutputValue);
	static FunctionReg<SetRotationSpeedFunction> setRotationSpeedRegistration(registration, kFunctionSetRotationSpeed, table->GetString(StringID('CTRL', kControllerRotation, kFunctionSetRotationSpeed)));
	static FunctionReg<RotateToStartFunction> rotateToStartRegistration(registration, kFunctionRotateToStart, table->GetString(StringID('CTRL', kControllerRotation, kFunctionRotateToStart)));
	static FunctionReg<RotateToFinishFunction> rotateToFinishRegistration(registration, kFunctionRotateToFinish, table->GetString(StringID('CTRL', kControllerRotation, kFunctionRotateToFinish)));
}

void RotationController::Pack(Packer& data, unsigned_int32 packFlags) const
{
	KinematicController::Pack(data, packFlags);

	data << ChunkHeader('STAT', 4);
	data << rotationState;

	data << ChunkHeader('RANG', 4);
	data << rotationAngle;

	data << ChunkHeader('RSPD', 4);
	data << rotationSpeed;

	data << ChunkHeader('ACCT', 4);
	data << accelerationTime;

	data << ChunkHeader('DECT', 4);
	data << decelerationTime;

	PackHandle handle = data.BeginChunk('CKEY');
	data << centerConnectorKey;
	data.EndChunk(handle);

	if (rotationState & kRotationInitialized)
	{
		data << ChunkHeader('ANGL', 4);
		data << currentAngle;

		data << ChunkHeader('SPED', 8);
		data << targetSpeed;
		data << currentSpeed;

		data << ChunkHeader('ACCL', 4);
		data << currentAcceleration;

		data << ChunkHeader('XFRM', sizeof(Transform4D));
		data << originalNodeTransform;
	}

	data << TerminatorChunk;
}

void RotationController::Unpack(Unpacker& data, unsigned_int32 unpackFlags)
{
	KinematicController::Unpack(data, unpackFlags);
	UnpackChunkList<RotationController>(data, unpackFlags);
}

bool RotationController::UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags)
{
	switch (chunkHeader->chunkType)
	{
		case 'STAT':

			data >> rotationState;
			return (true);

		case 'RANG':

			data >> rotationAngle;
			return (true);

		case 'RSPD':

			data >> rotationSpeed;
			return (true);

		case 'ACCT':

			data >> accelerationTime;
			return (true);

		case 'DECT':

			data >> decelerationTime;
			return (true);

		case 'CKEY':

			data >> centerConnectorKey;
			return (true);

		case 'ANGL':

			data >> currentAngle;
			return (true);

		case 'SPED':

			data >> targetSpeed;
			data >> currentSpeed;
			return (true);

		case 'ACCL':

			data >> currentAcceleration;
			return (true);

		case 'XFRM':

			data >> originalNodeTransform;
			return (true);
	}

	return (false);
}

int32 RotationController::GetSettingCount(void) const
{
	return (KinematicController::GetSettingCount() + 6);
}

Setting *RotationController::GetSetting(int32 index) const
{
	int32 count = KinematicController::GetSettingCount();
	if (index < count)
	{
		return (KinematicController::GetSetting(index));
	}

	const StringTable *table = TheInterfaceMgr->GetStringTable();

	if (index == count)
	{
		const char *title = table->GetString(StringID('CTRL', kControllerRotation, 'ROTA'));
		return (new HeadingSetting('ROTA', title));
	}

	if (index == count + 1)
	{
		const char *title = table->GetString(StringID('CTRL', kControllerRotation, 'RANG'));
		return (new TextSetting('RANG', rotationAngle, title));
	}

	if (index == count + 2)
	{
		const char *title = table->GetString(StringID('CTRL', kControllerRotation, 'RSPD'));
		return (new TextSetting('RSPD', rotationSpeed, title));
	}

	if (index == count + 3)
	{
		const char *title = table->GetString(StringID('CTRL', kControllerRotation, 'ACCT'));
		return (new TextSetting('ACCT', accelerationTime, title));
	}

	if (index == count + 4)
	{
		const char *title = table->GetString(StringID('CTRL', kControllerRotation, 'DECT'));
		return (new TextSetting('DECT', decelerationTime, title));
	}

	if (index == count + 5)
	{
		const char *title = table->GetString(StringID('CTRL', kControllerRotation, 'CKEY'));
		return (new TextSetting('CKEY', centerConnectorKey, title, kMaxConnectorKeyLength, &Connector::ConnectorKeyFilter));
	}

	return (nullptr);
}

void RotationController::SetSetting(const Setting *setting)
{
	Type identifier = setting->GetSettingIdentifier();

	if (identifier == 'RANG')
	{
		const char *text = static_cast<const TextSetting *>(setting)->GetText();
		rotationAngle = FmaxZero(Text::StringToFloat(text));
	}
	else if (identifier == 'RSPD')
	{
		const char *text = static_cast<const TextSetting *>(setting)->GetText();
		rotationSpeed = FmaxZero(Text::StringToFloat(text));
	}
	else if (identifier == 'ACCT')
	{
		const char *text = static_cast<const TextSetting *>(setting)->GetText();
		accelerationTime = FmaxZero(Text::StringToFloat(text));
	}
	else if (identifier == 'DECT')
	{
		const char *text = static_cast<const TextSetting *>(setting)->GetText();
		decelerationTime = FmaxZero(Text::StringToFloat(text));
	}
	else if (identifier == 'CKEY')
	{
		centerConnectorKey = static_cast<const TextSetting *>(setting)->GetText();
	}
	else
	{
		KinematicController::SetSetting(setting);
	}
}

void RotationController::Preprocess(void)
{
	Node *node = GetTargetNode();
	node->SetNodeFlags(node->GetNodeFlags() | kNodeVisibilitySite);

	if (!node->GetManipulator())
	{
		unsigned_int32 state = rotationState;
		Node *center = node->GetConnectedNode(centerConnectorKey);

		if (!(state & kRotationInitialized))
		{
			SetControllerFlags(GetControllerFlags() | kControllerAsleep);

			if (center)
			{
				rotationState = state | kRotationInitialized;
				originalNodeTransform = node->GetNodeTransform();
			}
		}

		if (rotationState & kRotationInitialized)
		{
			centerTransform = &center->GetWorldTransform();
			CalculateRotationParameters();
		}

		const Node *subnode = node;
		do
		{
			if (subnode->GetNodeType() == kNodeGeometry)
			{
				GeometryObject *object = static_cast<const Geometry *>(subnode)->GetObject();
				object->SetGeometryFlags(object->GetGeometryFlags() | kGeometryDynamic);
			}

			subnode = node->GetNextNode(subnode);
		} while (subnode);
	}

	KinematicController::Preprocess();
}

bool RotationController::InstanceExtractable(void) const
{
	return (true);
}

ControllerMessage *RotationController::CreateMessage(ControllerMessageType type) const
{
	if (type == kRotationMessageState)
	{
		return (new RotationStateMessage(GetControllerIndex()));
	}

	return (KinematicController::CreateMessage(type));
}

void RotationController::ReceiveMessage(const ControllerMessage *message)
{
	if (message->GetControllerMessageType() == kRotationMessageState)
	{
		const RotationStateMessage *stateMessage = static_cast<const RotationStateMessage *>(message);

		targetSpeed = stateMessage->targetSpeed;
		currentSpeed = stateMessage->currentSpeed;
		currentAcceleration = stateMessage->currentAcceleration;

		rotationSpeed = stateMessage->rotationSpeed;
		accelerationTime = stateMessage->accelerationTime;
		decelerationTime = stateMessage->decelerationTime;

		CalculateRotationParameters();

		if (rotationState & kRotationInitialized)
		{
			UpdateNodeAngle(stateMessage->currentAngle);

			if (targetSpeed != 0.0F)
			{
				Wake();
			}
			else
			{
				Sleep();
			}
		}
	}
	else
	{
		KinematicController::ReceiveMessage(message);
	}
}

void RotationController::SendInitialStateMessages(Player *player) const
{
	player->SendMessage(RotationStateMessage(GetControllerIndex(), currentAngle, targetSpeed, currentSpeed, currentAcceleration, rotationSpeed, accelerationTime, decelerationTime));
}

void RotationController::CalculateRotationParameters(void)
{
	minRotationSpeed = rotationSpeed * 0.01F;

	if (decelerationTime > K::min_float)
	{
		decelerationRate = rotationSpeed / decelerationTime;
		decelerationAngle = decelerationRate * decelerationTime * decelerationTime * 0.5F;
	}
	else
	{
		decelerationRate = 0.0F;
		decelerationAngle = 0.0F;
	}
}

void RotationController::CalculateForwardParameters(void)
{
	float speed = rotationSpeed;
	targetSpeed = speed;

	if (accelerationTime > K::min_float)
	{
		currentSpeed = 0.0F;
		currentAcceleration = speed / accelerationTime;
	}
	else
	{
		currentSpeed = speed;
		currentAcceleration = 0.0F;
	}
}

void RotationController::CalculateBackwardParameters(void)
{
	float speed = -rotationSpeed;
	targetSpeed = speed;

	if (accelerationTime > K::min_float)
	{
		currentSpeed = 0.0F;
		currentAcceleration = speed / accelerationTime;
	}
	else
	{
		currentSpeed = speed;
		currentAcceleration = 0.0F;
	}
}

void RotationController::UpdateNodeAngle(float angle)
{
	Matrix3D	rotator;

	Node *node = GetTargetNode();
	const Transform4D& inverseTransform = node->GetSuperNode()->GetInverseWorldTransform();

	currentAngle = angle;
	rotator.SetRotationAboutAxis(angle * K::radians, inverseTransform * (*centerTransform)[2]);

	Point3D center = inverseTransform * centerTransform->GetTranslation();
	Transform4D transform(rotator, center - rotator * center);

	node->SetNodeTransform(transform * originalNodeTransform);
	node->Invalidate();
}

void RotationController::Move(void)
{
	WakeProximateRigidBodies();

	ObservableEventType eventType = 0;
	float dt = TheTimeMgr->GetDeltaSeconds();

	float angle = currentAngle;
	float target = targetSpeed;
	float speed = currentSpeed;

	if (!(target < 0.0F))
	{
		speed = Clamp(speed + currentAcceleration * dt, minRotationSpeed, target);
		angle += speed * dt;

		if (!(angle < rotationAngle))
		{
			angle = rotationAngle;
			eventType = kEventRotationReachedFinish;
		}
		else if (angle > rotationAngle - decelerationAngle)
		{
			currentAcceleration = -decelerationRate;
		}
	}
	else
	{
		speed = Clamp(speed + currentAcceleration * dt, target, -minRotationSpeed);
		angle += speed * dt;

		if (!(angle > 0.0F))
		{
			angle = 0.0F;
			eventType = kEventRotationReachedStart;
		}
		else if (angle < decelerationAngle)
		{
			currentAcceleration = decelerationRate;
		}
	}

	currentSpeed = speed;
	UpdateNodeAngle(angle);

	if ((eventType != 0) && (TheMessageMgr->Server()))
	{
		targetSpeed = 0.0F;
		currentSpeed = 0.0F;
		currentAcceleration = 0.0F;

		TheMessageMgr->SendMessageClients(RotationStateMessage(GetControllerIndex(), angle, 0.0F, 0.0F, 0.0F, rotationSpeed, accelerationTime, decelerationTime));
		Sleep();

		PostEvent(eventType);
	}
}

void RotationController::Wake(void)
{
	if (rotationState & kRotationInitialized)
	{
		KinematicController::Wake();
	}
}

void RotationController::Sleep(void)
{
	GetTargetNode()->StopMotion();
	KinematicController::Sleep();
}

void RotationController::Activate(Node *initiator, Node *trigger)
{
	RotateToFinish();
}

void RotationController::SetRotationSpeed(float speed)
{
	rotationSpeed = speed;
	CalculateRotationParameters();

	if (!Asleep())
	{
		if (!(targetSpeed < 0.0F))
		{
			CalculateForwardParameters();
		}
		else
		{
			CalculateBackwardParameters();
		}
	}

	TheMessageMgr->SendMessageClients(RotationStateMessage(GetControllerIndex(), currentAngle, targetSpeed, currentSpeed, currentAcceleration, rotationSpeed, accelerationTime, decelerationTime));
}

void RotationController::RotateToStart(void)
{
	CalculateBackwardParameters();
	Wake();

	TheMessageMgr->SendMessageClients(RotationStateMessage(GetControllerIndex(), currentAngle, targetSpeed, currentSpeed, currentAcceleration, rotationSpeed, accelerationTime, decelerationTime));
}

void RotationController::RotateToFinish(void)
{
	CalculateForwardParameters();
	Wake();

	TheMessageMgr->SendMessageClients(RotationStateMessage(GetControllerIndex(), currentAngle, targetSpeed, currentSpeed, currentAcceleration, rotationSpeed, accelerationTime, decelerationTime));
}


GetRotationSpeedFunction::GetRotationSpeedFunction() : Function(kFunctionGetRotationSpeed, kControllerRotation)
{
}

GetRotationSpeedFunction::GetRotationSpeedFunction(const GetRotationSpeedFunction& getRotationSpeedFunction) : Function(getRotationSpeedFunction)
{
}

GetRotationSpeedFunction::~GetRotationSpeedFunction()
{
}

Function *GetRotationSpeedFunction::Replicate(void) const
{
	return (new GetRotationSpeedFunction(*this));
}

void GetRotationSpeedFunction::Execute(Controller *controller, FunctionMethod *method, const ScriptState *state)
{
	method->SetOutputValue(state, static_cast<const RotationController *>(controller)->GetRotationSpeed());
	CallCompletionProc();
}


SetRotationSpeedFunction::SetRotationSpeedFunction() : Function(kFunctionSetRotationSpeed, kControllerRotation)
{
	rotationSpeed = 90.0F;
}

SetRotationSpeedFunction::SetRotationSpeedFunction(const SetRotationSpeedFunction& setRotationSpeedFunction) : Function(setRotationSpeedFunction)
{
	rotationSpeed = setRotationSpeedFunction.rotationSpeed;
}

SetRotationSpeedFunction::~SetRotationSpeedFunction()
{
}

Function *SetRotationSpeedFunction::Replicate(void) const
{
	return (new SetRotationSpeedFunction(*this));
}

void SetRotationSpeedFunction::Pack(Packer& data, unsigned_int32 packFlags) const
{
	Function::Pack(data, packFlags);

	data << rotationSpeed;
}

void SetRotationSpeedFunction::Unpack(Unpacker& data, unsigned_int32 unpackFlags)
{
	Function::Unpack(data, unpackFlags);

	data >> rotationSpeed;
}

int32 SetRotationSpeedFunction::GetSettingCount(void) const
{
	return (1);
}

Setting *SetRotationSpeedFunction::GetSetting(int32 index) const
{
	const StringTable *table = TheInterfaceMgr->GetStringTable();

	if (index == 0)
	{
		const char *title = table->GetString(StringID('CTRL', kControllerRotation, kFunctionSetRotationSpeed, 'RSPD'));
		return (new TextSetting('RSPD', rotationSpeed, title));
	}

	return (nullptr);
}

void SetRotationSpeedFunction::SetSetting(const Setting *setting)
{
	Type identifier = setting->GetSettingIdentifier();

	if (identifier == 'RSPD')
	{
		const char *text = static_cast<const TextSetting *>(setting)->GetText();
		rotationSpeed = FmaxZero(Text::StringToFloat(text));
	}
}

void SetRotationSpeedFunction::Execute(Controller *controller, FunctionMethod *method, const ScriptState *state)
{
	RotationController *rotationController = static_cast<RotationController *>(controller);
	rotationController->SetRotationSpeed(rotationSpeed);
	CallCompletionProc();
}


RotateToStartFunction::RotateToStartFunction() :
		Function(kFunctionRotateToStart, kControllerRotation),
		Observer<RotateToStartFunction, Controller>(this, &RotateToStartFunction::HandleControllerEvent)
{
}

RotateToStartFunction::RotateToStartFunction(const RotateToStartFunction& rotateToStartFunction) :
		Function(rotateToStartFunction),
		Observer<RotateToStartFunction, Controller>(this, &RotateToStartFunction::HandleControllerEvent)
{
}

RotateToStartFunction::~RotateToStartFunction()
{
}

Function *RotateToStartFunction::Replicate(void) const
{
	return (new RotateToStartFunction(*this));
}

void RotateToStartFunction::Execute(Controller *controller, FunctionMethod *method, const ScriptState *state)
{
	RotationController *rotationController = static_cast<RotationController *>(controller);
	rotationController->AddObserver(this);
	rotationController->RotateToStart();
}

void RotateToStartFunction::HandleControllerEvent(Controller *controller, Controller::ObservableEventType event)
{
	if (event == RotationController::kEventRotationReachedStart)
	{
		controller->RemoveObserver(this);
		CallCompletionProc();
	}
}


RotateToFinishFunction::RotateToFinishFunction() :
		Function(kFunctionRotateToFinish, kControllerRotation),
		Observer<RotateToFinishFunction, Controller>(this, &RotateToFinishFunction::HandleControllerEvent)
{
}

RotateToFinishFunction::RotateToFinishFunction(const RotateToFinishFunction& rotateToFinishFunction) :
		Function(rotateToFinishFunction),
		Observer<RotateToFinishFunction, Controller>(this, &RotateToFinishFunction::HandleControllerEvent)
{
}

RotateToFinishFunction::~RotateToFinishFunction()
{
}

Function *RotateToFinishFunction::Replicate(void) const
{
	return (new RotateToFinishFunction(*this));
}

void RotateToFinishFunction::Execute(Controller *controller, FunctionMethod *method, const ScriptState *state)
{
	RotationController *rotationController = static_cast<RotationController *>(controller);
	rotationController->AddObserver(this);
	rotationController->RotateToFinish();
}

void RotateToFinishFunction::HandleControllerEvent(Controller *controller, Controller::ObservableEventType event)
{
	if (event == RotationController::kEventRotationReachedFinish)
	{
		controller->RemoveObserver(this);
		CallCompletionProc();
	}
}


RotationStateMessage::RotationStateMessage(int32 controllerIndex) : ControllerMessage(RotationController::kRotationMessageState, controllerIndex)
{
}

RotationStateMessage::RotationStateMessage(int32 controllerIndex, float angle, float targSpeed, float currSpeed, float currAccel, float rotateSpeed, float accelTime, float decelTime) : ControllerMessage(RotationController::kRotationMessageState, controllerIndex)
{
	currentAngle = angle;

	targetSpeed = targSpeed;
	currentSpeed = currSpeed;
	currentAcceleration = currAccel;

	rotationSpeed = rotateSpeed;
	accelerationTime = accelTime;
	decelerationTime = decelTime;
}

RotationStateMessage::~RotationStateMessage()
{
}

void RotationStateMessage::Compress(Compressor& data) const
{
	ControllerMessage::Compress(data);

	data << currentAngle;

	data << targetSpeed;
	data << currentSpeed;
	data << currentAcceleration;

	data << rotationSpeed;
	data << accelerationTime;
	data << decelerationTime;
}

bool RotationStateMessage::Decompress(Decompressor& data)
{
	if (ControllerMessage::Decompress(data))
	{
		data >> currentAngle;

		data >> targetSpeed;
		data >> currentSpeed;
		data >> currentAcceleration;

		data >> rotationSpeed;
		data >> accelerationTime;
		data >> decelerationTime;

		return (true);
	}

	return (false);
}


SpinController::SpinController() : KinematicController(kControllerSpin)
{
	spinState = 0;
	spinAngle = 0.0F;
	spinSpeed = 0.0F;

	currentSpeed = 0.0F;
	currentAcceleration = 0.0F;

	centerConnectorKey = "Center";
}

SpinController::SpinController(const SpinController& spinController) : KinematicController(spinController)
{
	spinState = 0;
	spinAngle = 0.0F;
	spinSpeed = spinController.spinSpeed;

	currentSpeed = 0.0F;
	currentAcceleration = 0.0F;

	centerConnectorKey = spinController.centerConnectorKey;
}

SpinController::~SpinController()
{
}

Controller *SpinController::Replicate(void) const
{
	return (new SpinController(*this));
}

void SpinController::RegisterFunctions(ControllerRegistration *registration)
{
	const StringTable *table = TheInterfaceMgr->GetStringTable();

	static FunctionReg<GetSpinSpeedFunction> getSpinSpeedRegistration(registration, kFunctionGetSpinSpeed, table->GetString(StringID('CTRL', kControllerSpin, kFunctionGetSpinSpeed)), kFunctionOutputValue);
	static FunctionReg<SetSpinSpeedFunction> setSpinSpeedRegistration(registration, kFunctionSetSpinSpeed, table->GetString(StringID('CTRL', kControllerSpin, kFunctionSetSpinSpeed)));
}

void SpinController::Pack(Packer& data, unsigned_int32 packFlags) const
{
	KinematicController::Pack(data, packFlags);

	data << ChunkHeader('STAT', 4);
	data << spinState;

	data << ChunkHeader('SSPD', 4);
	data << spinSpeed;

	PackHandle handle = data.BeginChunk('CKEY');
	data << centerConnectorKey;
	data.EndChunk(handle);

	if (spinState & kSpinInitialized)
	{
		data << ChunkHeader('ANGL', 4);
		data << spinAngle;

		data << ChunkHeader('SPED', 4);
		data << currentSpeed;

		data << ChunkHeader('ACCL', 4);
		data << currentAcceleration;

		data << ChunkHeader('XFRM', sizeof(Transform4D));
		data << originalNodeTransform;
	}

	data << TerminatorChunk;
}

void SpinController::Unpack(Unpacker& data, unsigned_int32 unpackFlags)
{
	KinematicController::Unpack(data, unpackFlags);
	UnpackChunkList<SpinController>(data, unpackFlags);
}

bool SpinController::UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags)
{
	switch (chunkHeader->chunkType)
	{
		case 'STAT':

			data >> spinState;
			return (true);

		case 'SSPD':

			data >> spinSpeed;
			return (true);

		case 'CKEY':

			data >> centerConnectorKey;
			return (true);

		case 'ANGL':

			data >> spinAngle;
			return (true);

		case 'SPED':

			data >> currentSpeed;
			return (true);

		case 'ACCL':

			data >> currentAcceleration;
			return (true);

		case 'XFRM':

			data >> originalNodeTransform;
			return (true);
	}

	return (false);
}

int32 SpinController::GetSettingCount(void) const
{
	return (KinematicController::GetSettingCount() + 3);
}

Setting *SpinController::GetSetting(int32 index) const
{
	int32 count = KinematicController::GetSettingCount();
	if (index < count)
	{
		return (KinematicController::GetSetting(index));
	}

	const StringTable *table = TheInterfaceMgr->GetStringTable();

	if (index == count)
	{
		const char *title = table->GetString(StringID('CTRL', kControllerSpin, 'SPIN'));
		return (new HeadingSetting('SPIN', title));
	}

	if (index == count + 1)
	{
		const char *title = table->GetString(StringID('CTRL', kControllerSpin, 'SPSP'));
		return (new TextSetting('SPSP', spinSpeed, title));
	}

	if (index == count + 2)
	{
		const char *title = table->GetString(StringID('CTRL', kControllerSpin, 'CKEY'));
		return (new TextSetting('CKEY', centerConnectorKey, title, kMaxConnectorKeyLength, &Connector::ConnectorKeyFilter));
	}

	return (nullptr);
}

void SpinController::SetSetting(const Setting *setting)
{
	Type identifier = setting->GetSettingIdentifier();

	if (identifier == 'SPSP')
	{
		const char *text = static_cast<const TextSetting *>(setting)->GetText();
		spinSpeed = Text::StringToFloat(text);
	}
	else if (identifier == 'CKEY')
	{
		centerConnectorKey = static_cast<const TextSetting *>(setting)->GetText();
	}
	else
	{
		KinematicController::SetSetting(setting);
	}
}

void SpinController::Preprocess(void)
{
	Node *node = GetTargetNode();
	node->SetNodeFlags(node->GetNodeFlags() | kNodeVisibilitySite);

	if (!node->GetManipulator())
	{
		unsigned_int32 state = spinState;
		Node *center = node->GetConnectedNode(centerConnectorKey);

		if (!(state & kSpinInitialized))
		{
			if (spinSpeed == 0.0F)
			{
				SetControllerFlags(GetControllerFlags() | kControllerAsleep);
			}

			if (center)
			{
				spinState = state | kSpinInitialized;
				currentSpeed = spinSpeed;

				originalNodeTransform = node->GetNodeTransform();
			}
		}

		if (spinState & kSpinInitialized)
		{
			centerTransform = &center->GetWorldTransform();
		}

		const Node *subnode = node;
		do
		{
			if (subnode->GetNodeType() == kNodeGeometry)
			{
				GeometryObject *object = static_cast<const Geometry *>(subnode)->GetObject();
				object->SetGeometryFlags(object->GetGeometryFlags() | kGeometryDynamic);
			}

			subnode = node->GetNextNode(subnode);
		} while (subnode);
	}

	KinematicController::Preprocess();
}

bool SpinController::InstanceExtractable(void) const
{
	return (true);
}

ControllerMessage *SpinController::CreateMessage(ControllerMessageType type) const
{
	if (type == kSpinMessageState)
	{
		return (new SpinStateMessage(GetControllerIndex()));
	}

	return (KinematicController::CreateMessage(type));
}

void SpinController::ReceiveMessage(const ControllerMessage *message)
{
	if (message->GetControllerMessageType() == kSpinMessageState)
	{
		const SpinStateMessage *stateMessage = static_cast<const SpinStateMessage *>(message);

		spinSpeed = stateMessage->spinSpeed;
		currentSpeed = stateMessage->currentSpeed;
		currentAcceleration = stateMessage->currentAcceleration;

		if (spinState & kSpinInitialized)
		{
			UpdateNodeAngle(stateMessage->spinAngle);

			if (spinSpeed != 0.0F)
			{
				Wake();
			}
		}
	}
	else
	{
		KinematicController::ReceiveMessage(message);
	}
}

void SpinController::SendInitialStateMessages(Player *player) const
{
	player->SendMessage(SpinStateMessage(GetControllerIndex(), spinAngle, spinSpeed, currentSpeed, currentAcceleration));
}

void SpinController::UpdateNodeAngle(float angle)
{
	Matrix3D	rotator;

	Node *node = GetTargetNode();
	const Transform4D& inverseTransform = node->GetSuperNode()->GetInverseWorldTransform();

	spinAngle = angle;
	rotator.SetRotationAboutAxis(angle * K::tau, inverseTransform * (*centerTransform)[2]);

	Point3D center = inverseTransform * centerTransform->GetTranslation();
	Transform4D transform(rotator, center - rotator * center);

	node->SetNodeTransform(transform * originalNodeTransform);
	node->Invalidate();
}

void SpinController::Move(void)
{
	WakeProximateRigidBodies();

	float dt = TheTimeMgr->GetDeltaSeconds();
	float target = spinSpeed;
	float speed = currentSpeed;

	if ((target > 0.0F) || (currentAcceleration > 0.0F))
	{
		speed = Fmin(speed + currentAcceleration * dt, target);
	}
	else
	{
		speed = Fmax(speed + currentAcceleration * dt, target);
	}

	currentSpeed = speed;
	float angle = Frac(spinAngle + speed * dt);
	UpdateNodeAngle(angle);

	if ((speed == 0.0F) && (target == 0.0F))
	{
		if (TheMessageMgr->Server())
		{
			currentAcceleration = 0.0F;
			TheMessageMgr->SendMessageClients(SpinStateMessage(GetControllerIndex(), angle, 0.0F, 0.0F, 0.0F));
		}

		Sleep();
	}
}

void SpinController::Wake(void)
{
	if (spinState & kSpinInitialized)
	{
		KinematicController::Wake();
	}
}

void SpinController::Sleep(void)
{
	GetTargetNode()->StopMotion();
	KinematicController::Sleep();
}

void SpinController::SetSpinSpeed(float speed, float time)
{
	if (time > K::min_float)
	{
		currentAcceleration = (speed - spinSpeed) / time;
	}
	else
	{
		currentSpeed = speed;
		currentAcceleration = 0.0F;
	}

	spinSpeed = speed;
	if (speed != 0.0F)
	{
		Wake();
	}

	TheMessageMgr->SendMessageClients(SpinStateMessage(GetControllerIndex(), spinAngle, spinSpeed, currentSpeed, currentAcceleration));
}


GetSpinSpeedFunction::GetSpinSpeedFunction() : Function(kFunctionGetSpinSpeed, kControllerSpin)
{
}

GetSpinSpeedFunction::GetSpinSpeedFunction(const GetSpinSpeedFunction& getSpinSpeedFunction) : Function(getSpinSpeedFunction)
{
}

GetSpinSpeedFunction::~GetSpinSpeedFunction()
{
}

Function *GetSpinSpeedFunction::Replicate(void) const
{
	return (new GetSpinSpeedFunction(*this));
}

void GetSpinSpeedFunction::Execute(Controller *controller, FunctionMethod *method, const ScriptState *state)
{
	method->SetOutputValue(state, static_cast<const SpinController *>(controller)->GetSpinSpeed());
	CallCompletionProc();
}


SetSpinSpeedFunction::SetSpinSpeedFunction() : Function(kFunctionSetSpinSpeed, kControllerSpin)
{
	spinSpeed = 1.0F;
	accelerationTime = 0.0F;
}

SetSpinSpeedFunction::SetSpinSpeedFunction(const SetSpinSpeedFunction& setSpinSpeedFunction) : Function(setSpinSpeedFunction)
{
	spinSpeed = setSpinSpeedFunction.spinSpeed;
	accelerationTime = setSpinSpeedFunction.accelerationTime;
}

SetSpinSpeedFunction::~SetSpinSpeedFunction()
{
}

Function *SetSpinSpeedFunction::Replicate(void) const
{
	return (new SetSpinSpeedFunction(*this));
}

void SetSpinSpeedFunction::Pack(Packer& data, unsigned_int32 packFlags) const
{
	Function::Pack(data, packFlags);

	data << spinSpeed;
	data << accelerationTime;
}

void SetSpinSpeedFunction::Unpack(Unpacker& data, unsigned_int32 unpackFlags)
{
	Function::Unpack(data, unpackFlags);

	data >> spinSpeed;
	data >> accelerationTime;
}

int32 SetSpinSpeedFunction::GetSettingCount(void) const
{
	return (2);
}

Setting *SetSpinSpeedFunction::GetSetting(int32 index) const
{
	const StringTable *table = TheInterfaceMgr->GetStringTable();

	if (index == 0)
	{
		const char *title = table->GetString(StringID('CTRL', kControllerSpin, kFunctionSetSpinSpeed, 'SPSP'));
		return (new TextSetting('SPSP', spinSpeed, title));
	}

	if (index == 1)
	{
		const char *title = table->GetString(StringID('CTRL', kControllerSpin, kFunctionSetSpinSpeed, 'ACCT'));
		return (new TextSetting('ACCT', accelerationTime, title));
	}

	return (nullptr);
}

void SetSpinSpeedFunction::SetSetting(const Setting *setting)
{
	Type identifier = setting->GetSettingIdentifier();

	if (identifier == 'SPSP')
	{
		const char *text = static_cast<const TextSetting *>(setting)->GetText();
		spinSpeed = Text::StringToFloat(text);
	}
	else if (identifier == 'ACCT')
	{
		const char *text = static_cast<const TextSetting *>(setting)->GetText();
		accelerationTime = FmaxZero(Text::StringToFloat(text));
	}
}

void SetSpinSpeedFunction::Execute(Controller *controller, FunctionMethod *method, const ScriptState *state)
{
	SpinController *spinController = static_cast<SpinController *>(controller);
	spinController->SetSpinSpeed(spinSpeed, accelerationTime);
	CallCompletionProc();
}


SpinStateMessage::SpinStateMessage(int32 controllerIndex) : ControllerMessage(SpinController::kSpinMessageState, controllerIndex)
{
}

SpinStateMessage::SpinStateMessage(int32 controllerIndex, float angle, float speed, float currSpeed, float currAccel) : ControllerMessage(SpinController::kSpinMessageState, controllerIndex)
{
	spinAngle = angle;
	spinSpeed = speed;

	currentSpeed = currSpeed;
	currentAcceleration = currAccel;
}

SpinStateMessage::~SpinStateMessage()
{
}

void SpinStateMessage::Compress(Compressor& data) const
{
	ControllerMessage::Compress(data);

	data << spinAngle;
	data << spinSpeed;

	data << currentSpeed;
	data << currentAcceleration;
}

bool SpinStateMessage::Decompress(Decompressor& data)
{
	if (ControllerMessage::Decompress(data))
	{
		data >> spinAngle;
		data >> spinSpeed;

		data >> currentSpeed;
		data >> currentAcceleration;

		return (true);
	}

	return (false);
}

// ZYUQURM
