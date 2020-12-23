 

#include "C4Controller.h"
#include "C4World.h"
#include "C4Skybox.h"
#include "C4Water.h"
#include "C4Joints.h"
#include "C4Panels.h"
#include "C4Kinematic.h"
#include "C4Shaders.h"


using namespace C4;


namespace C4
{
	template class Creatable<Controller>;
	template class Registrable<Controller, ControllerRegistration>;
}


FunctionRegistration::FunctionRegistration(ControllerRegistration *reg, FunctionType type, const char *name, unsigned_int32 flags)
{
	functionType = type;
	functionFlags = flags;
	functionName = name;

	reg->functionMap.Insert(this);
}

FunctionRegistration::~FunctionRegistration()
{
}


Function::Function(FunctionType funcType, ControllerType contType)
{
	functionType = funcType;
	controllerType = contType;
}

Function::Function(const Function& function)
{
	functionType = function.functionType;
	controllerType = function.controllerType;
}

Function::~Function()
{
}

void Function::Pack(Packer& data, unsigned_int32 packFlags) const
{
}

void Function::Unpack(Unpacker& data, unsigned_int32 unpackFlags)
{
}

void Function::Compress(Compressor& data) const
{
}

bool Function::Decompress(Decompressor& data)
{
	return (true);
}

bool Function::OverridesFunction(const Function *function) const
{
	return (false);
}

void Function::Preprocess(Controller *controller, FunctionMethod *method, const ScriptState *state)
{
}

void Function::Execute(Controller *controller, FunctionMethod *method, const ScriptState *state)
{
	CallCompletionProc();
}

void Function::Resume(Controller *controller, FunctionMethod *method, const ScriptState *state)
{
	Execute(controller, method, state);
}


ControllerRegistration::ControllerRegistration(ControllerType type, const char *name) : Registration<Controller, ControllerRegistration>(type)
{
	controllerName = name;
}

ControllerRegistration::~ControllerRegistration()
{
	functionMap.RemoveAll();
}

Function *ControllerRegistration::CreateFunction(FunctionType type) const
{
	FunctionRegistration *reg = functionMap.Find(type);
	if (reg) 
	{
		return (reg->Create());
	} 

	return (nullptr); 
}

 
Controller::Controller(ControllerType type)
{ 
	controllerType = type; 
	baseControllerType = kControllerGeneric;

	controllerIndex = kControllerUnassigned;
	controllerFlags = (type == kControllerGeneric) ? kControllerAsleep : 0; 

	targetNode = nullptr;
}

Controller::Controller(const Controller& controller)
{
	controllerType = controller.controllerType;
	baseControllerType = controller.baseControllerType;

	controllerIndex = kControllerUnassigned;
	controllerFlags = controller.controllerFlags & ~kControllerUpdate;

	targetNode = nullptr;
}

Controller::~Controller()
{
	Node *node = targetNode;
	if (node)
	{
		node->SetController(nullptr);
	}
}

Controller *Controller::Replicate(void) const
{
	return (new Controller(*this));
}

Controller *Controller::New(ControllerType type)
{
	Type	data[2];

	data[0] = type;
	data[1] = 0;

	Unpacker unpacker(data);
	return (Create(unpacker));
}

Controller *Controller::CreateUnknown(Unpacker& data, unsigned_int32 unpackFlags)
{
	return (new UnknownController(data.GetType()));
}

bool Controller::ValidNode(const Node *node)
{
	return (true);
}

void Controller::RegisterStandardControllers(void)
{
	const StringTable *table = TheInterfaceMgr->GetStringTable();

	static ControllerReg<Controller> genericRegistration(kControllerGeneric, table->GetString(StringID('CTRL', kControllerGeneric)));
	static ControllerReg<UnknownController> unknownRegistration(kControllerUnknown, table->GetString(StringID('CTRL', kControllerUnknown)));
	static ControllerReg<DistributionController> distributionRegistration(kControllerDistribution, table->GetString(StringID('CTRL', kControllerDistribution)));
	static ControllerReg<ShaderParameterController> shaderParameterRegistration(kControllerShaderParameter, table->GetString(StringID('CTRL', kControllerShaderParameter)));
	static ControllerReg<StrobeController> strobeRegistration(kControllerStrobe, table->GetString(StringID('CTRL', kControllerStrobe)));
	static ControllerReg<FlickerController> flickerRegistration(kControllerFlicker, table->GetString(StringID('CTRL', kControllerFlicker)));
	static ControllerReg<FlashController> flashRegistration(kControllerFlash, table->GetString(StringID('CTRL', kControllerFlash)));
	static ControllerReg<AnimationController> animationRegistration(kControllerAnimation, table->GetString(StringID('CTRL', kControllerAnimation)));
	static ControllerReg<MorphController> morphRegistration(kControllerMorph, table->GetString(StringID('CTRL', kControllerMorph)));
	static ControllerReg<SkinController> skinRegistration(kControllerSkin, table->GetString(StringID('CTRL', kControllerSkin)));
	static ControllerReg<ScriptController> scriptRegistration(kControllerScript, table->GetString(StringID('CTRL', kControllerScript)));
	static ControllerReg<PanelController> panelRegistration(kControllerPanel, table->GetString(StringID('CTRL', kControllerPanel)));
	static ControllerReg<MovementController> movementRegistration(kControllerMovement, table->GetString(StringID('CTRL', kControllerMovement)));
	static ControllerReg<OscillationController> oscillationRegistration(kControllerOscillation, table->GetString(StringID('CTRL', kControllerOscillation)));
	static ControllerReg<RotationController> rotationRegistration(kControllerRotation, table->GetString(StringID('CTRL', kControllerRotation)));
	static ControllerReg<SpinController> spinRegistration(kControllerSpin, table->GetString(StringID('CTRL', kControllerSpin)));
	static ControllerReg<RigidBodyController> rigidBodyRegistration(kControllerRigidBody, table->GetString(StringID('CTRL', kControllerRigidBody)));
	static ControllerReg<RagdollController> ragdollRegistration(kControllerRagdoll, table->GetString(StringID('CTRL', kControllerRagdoll)));
	static ControllerReg<PhysicsController> physicsRegistration(kControllerPhysics, table->GetString(StringID('CTRL', kControllerPhysics)));
	static ControllerReg<RopeController> ropeRegistration(kControllerRope, table->GetString(StringID('CTRL', kControllerRope)));
	static ControllerReg<ClothController> clothRegistration(kControllerCloth, table->GetString(StringID('CTRL', kControllerCloth)));
	static ControllerReg<WaterController> waterRegistration(kControllerWater, table->GetString(StringID('CTRL', kControllerWater)));

	AnimationController::RegisterFunctions(&animationRegistration);
	PanelController::RegisterFunctions(&panelRegistration);
	MovementController::RegisterFunctions(&movementRegistration);
	OscillationController::RegisterFunctions(&oscillationRegistration);
	RotationController::RegisterFunctions(&rotationRegistration);
	SpinController::RegisterFunctions(&spinRegistration);
	PhysicsController::RegisterFunctions(&physicsRegistration);
	RopeController::RegisterFunctions(&ropeRegistration);
	WaterController::RegisterFunctions(&waterRegistration);

	SetUnknownCreator(&CreateUnknown);
}

void Controller::PackType(Packer& data) const
{
	data << controllerType;
}

void Controller::Pack(Packer& data, unsigned_int32 packFlags) const
{
	data << ChunkHeader('FLAG', 4);
	data << unsigned_int32(controllerFlags & ~kControllerUpdate);

	data << ChunkHeader('INDX', 4);
	data << controllerIndex;

	data << TerminatorChunk;
}

void Controller::Unpack(Unpacker& data, unsigned_int32 unpackFlags)
{
	UnpackChunkList<Controller>(data, unpackFlags);

	if (unpackFlags & kUnpackNonpersistent)
	{
		controllerIndex = kControllerUnassigned;
	}
}

bool Controller::UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags)
{
	switch (chunkHeader->chunkType)
	{
		case 'FLAG':

			data >> controllerFlags;
			return (true);

		case 'INDX':

			data >> controllerIndex;
			return (true);
	}

	return (false);
}

void Controller::SetTargetNode(Node *node)
{
	if (targetNode != node)
	{
		if (!node)
		{
			World *world = targetNode->GetWorld();
			if (world)
			{
				world->RemoveController(this);
			}
		}

		targetNode = node;
	}
}

void Controller::Preprocess(void)
{
	World *world = targetNode->GetWorld();
	if ((world) && (!targetNode->GetManipulator()))
	{
		if (controllerType == kControllerGeneric)
		{
			controllerFlags |= kControllerMoveInhibit;
		}

		world->AddController(this);
	}
}

void Controller::Neutralize(void)
{
	World *world = targetNode->GetWorld();
	if (world)
	{
		world->RemoveController(this);
	}

	controllerIndex = kControllerUnassigned;
}

bool Controller::InstanceExtractable(void) const
{
	return (false);
}

ControllerMessage *Controller::CreateMessage(ControllerMessageType type) const
{
	switch (type)
	{
		case kControllerMessageSetting:

			return (new SettingMessage(GetControllerIndex()));

		case kControllerMessageFunction:

			return (new FunctionMessage(GetControllerIndex()));

		case kControllerMessageWake:
		case kControllerMessageSleep:

			return (new WakeSleepMessage(type, GetControllerIndex()));

		case kControllerMessageEnableNode:
		case kControllerMessageDisableNode:

			return (new NodeEnableDisableMessage(type, GetControllerIndex()));

		case kControllerMessageDeleteNode:

			return (new DeleteNodeMessage(GetControllerIndex()));

		case kControllerMessageEnableInteractivity:
		case kControllerMessageDisableInteractivity:

			return (new NodeInteractivityMessage(type, GetControllerIndex()));

		case kControllerMessageShowGeometry:
		case kControllerMessageHideGeometry:

			return (new GeometryVisibilityMessage(type, GetControllerIndex()));

		case kControllerMessagePlaySource:
		case kControllerMessageStopSource:

			return (new SourcePlayStopMessage(type, GetControllerIndex()));

		case kControllerMessageMaterialColor:

			return (new MaterialColorMessage(GetControllerIndex()));

		case kControllerMessageShaderParameter:

			return (new ShaderParameterMessage(GetControllerIndex()));
	}

	return (nullptr);
}

void Controller::ReceiveMessage(const ControllerMessage *message)
{
	message->HandleControllerMessage(this);
}

void Controller::SendInitialStateMessages(Player *player) const
{
}

void Controller::Wake(void)
{
	controllerFlags &= ~kControllerAsleep;

	World *world = targetNode->GetWorld();
	if (world)
	{
		world->WakeController(this);
	}
}

void Controller::Sleep(void)
{
	controllerFlags |= kControllerAsleep;

	World::SleepController(this);
}

void Controller::Move(void)
{
}

void Controller::StopMotion(void)
{
}

void Controller::Update(void)
{
	controllerFlags &= ~kControllerUpdate;
}

void Controller::SetDetailLevel(int32 level)
{
	Invalidate();
}

void Controller::Activate(Node *initiator, Node *trigger)
{
}

void Controller::Deactivate(Node *initiator, Node *trigger)
{
}

void Controller::HandleInteractionEvent(InteractionEventType type, const Point3D *position, Node *initiator)
{
	if (type == kInteractionEventActivate)
	{
		Activate(initiator, nullptr);
	}
	else if (type == kInteractionEventDeactivate)
	{
		Deactivate(initiator, nullptr);
	}
}

const void *Controller::GetControllerEventData(EventType event) const
{
	return (nullptr);
}


UnknownController::UnknownController() : Controller(kControllerUnknown)
{
	unknownType = 0;
	unknownSize = 0;
	unknownData = nullptr;

	SetControllerFlags(kControllerAsleep | kControllerLocal | kControllerMoveInhibit);
}

UnknownController::UnknownController(ControllerType type) : Controller(kControllerUnknown)
{
	unknownType = type;
	unknownSize = 0;
	unknownData = nullptr;

	SetControllerFlags(kControllerAsleep | kControllerLocal | kControllerMoveInhibit);
}

UnknownController::UnknownController(const UnknownController& unknownController) : Controller(unknownController)
{
	unknownType = unknownController.unknownType;
	unknownSize = unknownController.unknownSize;
	unknownData = new char[unknownSize];
	MemoryMgr::CopyMemory(unknownController.unknownData, unknownData, unknownSize);
}

UnknownController::~UnknownController()
{
	delete[] unknownData;
}

Controller *UnknownController::Replicate(void) const
{
	return (new UnknownController(*this));
}

bool UnknownController::ValidNode(const Node *node)
{
	const Controller *controller = node->GetController();
	return ((controller) && (controller->GetControllerType() == kControllerUnknown));
}

void UnknownController::PackType(Packer& data) const
{
	data << unknownType;
}

void UnknownController::Pack(Packer& data, unsigned_int32 packFlags) const
{
	data.WriteData(unknownData, unknownSize);
}

void UnknownController::Unpack(Unpacker& data, unsigned_int32 unpackFlags)
{
	unsigned_int32 size = data.GetUnknownSize() - sizeof(ControllerType);
	unknownSize = size;
	unknownData = new char[size];
	data.ReadData(unknownData, size);
}

int32 UnknownController::GetSettingCount(void) const
{
	return (1);
}

Setting *UnknownController::GetSetting(int32 index) const
{
	if (index == 0)
	{
		const StringTable *table = TheInterfaceMgr->GetStringTable();
		const char *title = table->GetString(StringID('CTRL', kControllerUnknown, 'TYPE'));
		return (new InfoSetting('TYPE', Text::TypeToHexCharString(unknownType), title));
	}

	return (nullptr);
}

void UnknownController::SetSetting(const Setting *setting)
{
}


SettingMessage::SettingMessage(int32 controllerIndex) : ControllerMessage(Controller::kControllerMessageSetting, controllerIndex)
{
	messageSetting = nullptr;
}

SettingMessage::SettingMessage(int32 controllerIndex, Type category, const Setting *setting) : ControllerMessage(Controller::kControllerMessageSetting, controllerIndex)
{
	settingCategory = category;
	messageSetting = setting->Clone();
}

SettingMessage::~SettingMessage()
{
	delete messageSetting;
}

void SettingMessage::Compress(Compressor& data) const
{
	ControllerMessage::Compress(data);

	data << messageSetting->GetSettingType();
	data << settingCategory;

	messageSetting->Compress(data);
}

bool SettingMessage::Decompress(Decompressor& data)
{
	if (ControllerMessage::Decompress(data))
	{
		Type	unpackData[2];

		data >> unpackData[0];
		unpackData[1] = 0;

		Unpacker unpacker(unpackData);
		Setting *setting = Setting::Create(unpacker);
		if (setting)
		{
			data >> settingCategory;

			setting->Decompress(data);
			messageSetting = setting;
			return (true);
		}
	}

	return (false);
}

void SettingMessage::HandleControllerMessage(Controller *controller) const
{
	Node *node = controller->GetTargetNode();

	Object *object = node->GetObject();
	object->SetCategorySetting(settingCategory, messageSetting);
	object->SetModifiedFlag();

	node->ProcessObjectSettings();
}

bool SettingMessage::OverridesMessage(const ControllerMessage *message) const
{
	if (message->GetControllerMessageType() == Controller::kControllerMessageSetting)
	{
		const Setting *setting = static_cast<const SettingMessage *>(message)->GetSetting();
		return (setting->GetSettingIdentifier() == messageSetting->GetSettingIdentifier());
	}

	return (false);
}


FunctionMessage::FunctionMessage(int32 controllerIndex) : ControllerMessage(Controller::kControllerMessageFunction, controllerIndex)
{
	messageFunction = nullptr;
}

FunctionMessage::FunctionMessage(int32 controllerIndex, const Function *function) : ControllerMessage(Controller::kControllerMessageFunction, controllerIndex)
{
	messageFunction = function->Clone();
	messageFunction->SetCompletionProc(function->GetCompletionProc(), function->GetCompletionCookie());
}

FunctionMessage::~FunctionMessage()
{
	delete messageFunction;
}

void FunctionMessage::Compress(Compressor& data) const
{
	ControllerMessage::Compress(data);

	data << messageFunction->GetFunctionType();
	messageFunction->Compress(data);
}

bool FunctionMessage::Decompress(Decompressor& data)
{
	if (ControllerMessage::Decompress(data))
	{
		const Controller *controller = TheWorldMgr->GetWorld()->GetController(GetControllerIndex());
		const ControllerRegistration *registration = Controller::FindRegistration(controller->GetControllerType());
		if (registration)
		{
			FunctionType	functionType;

			data >> functionType;

			Function *function = registration->CreateFunction(functionType);
			if (function)
			{
				function->Decompress(data);
				messageFunction = function;
				return (true);
			}
		}
	}

	return (false);
}

void FunctionMessage::HandleControllerMessage(Controller *controller) const
{
	messageFunction->Execute(controller, nullptr, nullptr);
}

bool FunctionMessage::OverridesMessage(const ControllerMessage *message) const
{
	if (message->GetControllerMessageType() == Controller::kControllerMessageFunction)
	{
		const Function *function = static_cast<const FunctionMessage *>(message)->GetFunction();
		return (messageFunction->OverridesFunction(function));
	}

	return (false);
}


WakeSleepMessage::WakeSleepMessage(ControllerMessageType type, int32 controllerIndex) : ControllerMessage(type, controllerIndex)
{
}

WakeSleepMessage::~WakeSleepMessage()
{
}

void WakeSleepMessage::HandleControllerMessage(Controller *controller) const
{
	if (GetControllerMessageType() == Controller::kControllerMessageWake)
	{
		controller->Wake();
	}
	else
	{
		controller->Sleep();
	}
}

bool WakeSleepMessage::OverridesMessage(const ControllerMessage *message) const
{
	ControllerMessageType type = message->GetControllerMessageType();
	return ((type == Controller::kControllerMessageWake) || (type == Controller::kControllerMessageSleep));
}


NodeEnableDisableMessage::NodeEnableDisableMessage(ControllerMessageType type, int32 controllerIndex) : ControllerMessage(type, controllerIndex)
{
}

NodeEnableDisableMessage::~NodeEnableDisableMessage()
{
}

void NodeEnableDisableMessage::HandleControllerMessage(Controller *controller) const
{
	Node *node = controller->GetTargetNode();

	if (GetControllerMessageType() == Controller::kControllerMessageEnableNode)
	{
		node->Enable();
	}
	else
	{
		node->Disable();
	}
}

bool NodeEnableDisableMessage::OverridesMessage(const ControllerMessage *message) const
{
	ControllerMessageType type = message->GetControllerMessageType();
	return ((type == Controller::kControllerMessageEnableNode) || (type == Controller::kControllerMessageDisableNode));
}


DeleteNodeMessage::DeleteNodeMessage(int32 controllerIndex) : ControllerMessage(Controller::kControllerMessageDeleteNode, controllerIndex, kMessageDestroyer)
{
}

DeleteNodeMessage::~DeleteNodeMessage()
{
}

void DeleteNodeMessage::HandleControllerMessage(Controller *controller) const
{
	delete controller->GetTargetNode();
}


NodeInteractivityMessage::NodeInteractivityMessage(ControllerMessageType type, int32 controllerIndex) : ControllerMessage(type, controllerIndex)
{
}

NodeInteractivityMessage::~NodeInteractivityMessage()
{
}

void NodeInteractivityMessage::HandleControllerMessage(Controller *controller) const
{
	Node *root = controller->GetTargetNode();
	Node *node = root;
	do
	{
		Property *property = node->GetProperty(kPropertyInteraction);
		if (property)
		{
			unsigned_int32 flags = property->GetPropertyFlags();

			if (GetControllerMessageType() == Controller::kControllerMessageEnableInteractivity)
			{
				flags &= ~kPropertyDisabled;
			}
			else
			{
				flags |= kPropertyDisabled;
			}

			property->SetPropertyFlags(flags);
		}

		node = root->GetNextNode(node);
	} while (node);
}

bool NodeInteractivityMessage::OverridesMessage(const ControllerMessage *message) const
{
	ControllerMessageType type = message->GetControllerMessageType();
	return ((type == Controller::kControllerMessageEnableInteractivity) || (type == Controller::kControllerMessageDisableInteractivity));
}


GeometryVisibilityMessage::GeometryVisibilityMessage(ControllerMessageType type, int32 controllerIndex) : ControllerMessage(type, controllerIndex)
{
}

GeometryVisibilityMessage::~GeometryVisibilityMessage()
{
}

void GeometryVisibilityMessage::HandleControllerMessage(Controller *controller) const
{
	ControllerMessageType type = GetControllerMessageType();

	Node *root = controller->GetTargetNode();
	Node *node = root;
	do
	{
		if (node->GetNodeType() == kNodeGeometry)
		{
			GeometryObject *object = static_cast<Geometry *>(node)->GetObject();
			unsigned_int32 flags = object->GetGeometryFlags();

			if (type == Controller::kControllerMessageShowGeometry)
			{
				flags &= ~kGeometryInvisible;
			}
			else
			{
				flags |= kGeometryInvisible;
			}

			object->SetGeometryFlags(flags);
			object->SetModifiedFlag();
		}

		node = root->GetNextNode(node);
	} while (node);
}

bool GeometryVisibilityMessage::OverridesMessage(const ControllerMessage *message) const
{
	ControllerMessageType type = message->GetControllerMessageType();
	return ((type == Controller::kControllerMessageShowGeometry) || (type == Controller::kControllerMessageHideGeometry));
}


BreakJointMessage::BreakJointMessage(int32 controllerIndex) : ControllerMessage(Controller::kControllerMessageBreakJoint, controllerIndex)
{
}

BreakJointMessage::~BreakJointMessage()
{
}

void BreakJointMessage::HandleControllerMessage(Controller *controller) const
{
	Node *node = controller->GetTargetNode();

	if (node->GetNodeType() == kNodeJoint)
	{
		static_cast<Joint *>(node)->BreakJoint();
	}
}

bool BreakJointMessage::OverridesMessage(const ControllerMessage *message) const
{
	return (message->GetControllerMessageType() == Controller::kControllerMessageBreakJoint);
}


SourcePlayStopMessage::SourcePlayStopMessage(ControllerMessageType type, int32 controllerIndex) : ControllerMessage(type, controllerIndex)
{
}

SourcePlayStopMessage::~SourcePlayStopMessage()
{
}

void SourcePlayStopMessage::HandleControllerMessage(Controller *controller) const
{
	Node *node = controller->GetTargetNode();
	if (node->GetNodeType() == kNodeSource)
	{
		Source *source = static_cast<Source *>(node);

		if (GetControllerMessageType() == Controller::kControllerMessagePlaySource)
		{
			source->Play();
		}
		else
		{
			source->Stop();
		}
	}
}

bool SourcePlayStopMessage::OverridesMessage(const ControllerMessage *message) const
{
	ControllerMessageType type = message->GetControllerMessageType();
	return ((type == Controller::kControllerMessagePlaySource) || (type == Controller::kControllerMessageStopSource));
}


MaterialColorMessage::MaterialColorMessage(int32 controllerIndex) : ControllerMessage(Controller::kControllerMessageMaterialColor, controllerIndex)
{
}

MaterialColorMessage::MaterialColorMessage(int32 controllerIndex, AttributeType type, const ColorRGBA& color) : ControllerMessage(Controller::kControllerMessageMaterialColor, controllerIndex)
{
	attributeType = type;
	materialColor = color;
}

MaterialColorMessage::~MaterialColorMessage()
{
}

void MaterialColorMessage::Compress(Compressor& data) const
{
	ControllerMessage::Compress(data);

	data << attributeType;
	data << materialColor;
}

bool MaterialColorMessage::Decompress(Decompressor& data)
{
	if (ControllerMessage::Decompress(data))
	{
		data >> attributeType;
		data >> materialColor;
		return (true);
	}

	return (false);
}

void MaterialColorMessage::HandleControllerMessage(Controller *controller) const
{
	Node *node = controller->GetTargetNode();

	NodeType type = node->GetNodeType();
	if (type == kNodeGeometry)
	{
		const Geometry *geometry = static_cast<Geometry *>(node);

		int32 count = geometry->GetMaterialCount();
		for (machine a = 0; a < count; a++)
		{
			MaterialObject *object = geometry->GetMaterialObject(a);
			if (object)
			{
				Attribute *attribute = object->FindAttribute(attributeType);
				if (attribute)
				{
					attribute->SetAttributeColor(materialColor);
					object->SetModifiedFlag();
				}
			}
		}
	}
	else if (type == kNodeSkybox)
	{
		const Skybox *skybox = static_cast<Skybox *>(node);

		MaterialObject *object = skybox->GetMaterialObject();
		if (object)
		{
			Attribute *attribute = object->FindAttribute(attributeType);
			if (attribute)
			{
				attribute->SetAttributeColor(materialColor);
				object->SetModifiedFlag();
			}
		}
	}
}

bool MaterialColorMessage::OverridesMessage(const ControllerMessage *message) const
{
	return (message->GetControllerMessageType() == Controller::kControllerMessageMaterialColor);
}


ShaderParameterMessage::ShaderParameterMessage(int32 controllerIndex) : ControllerMessage(Controller::kControllerMessageShaderParameter, controllerIndex)
{
}

ShaderParameterMessage::ShaderParameterMessage(int32 controllerIndex, int32 slot, const Vector4D& param) : ControllerMessage(Controller::kControllerMessageShaderParameter, controllerIndex)
{
	parameterSlot = slot;
	parameterValue = param;
}

ShaderParameterMessage::~ShaderParameterMessage()
{
}

void ShaderParameterMessage::Compress(Compressor& data) const
{
	ControllerMessage::Compress(data);

	data << parameterSlot;
	data << parameterValue;
}

bool ShaderParameterMessage::Decompress(Decompressor& data)
{
	if (ControllerMessage::Decompress(data))
	{
		data >> parameterSlot;
		data >> parameterValue;
		return (true);
	}

	return (false);
}

void ShaderParameterMessage::HandleControllerMessage(Controller *controller) const
{
	Node *node = controller->GetTargetNode();

	if (node->GetNodeType() == kNodeGeometry)
	{
		const Geometry *geometry = static_cast<Geometry *>(node);

		int32 count = geometry->GetMaterialCount();
		for (machine a = 0; a < count; a++)
		{
			MaterialObject *object = geometry->GetMaterialObject(a);
			if (object)
			{
				Attribute *attribute = object->GetFirstAttribute();
				if ((attribute) && (attribute->GetAttributeType() == kAttributeShader))
				{
					static_cast<ShaderAttribute *>(attribute)->SetParameterValue(parameterSlot, parameterValue);
					object->SetModifiedFlag();
				}
			}
		}
	}
}

bool ShaderParameterMessage::OverridesMessage(const ControllerMessage *message) const
{
	if (message->GetControllerMessageType() != Controller::kControllerMessageShaderParameter)
	{
		return (false);
	}

	return (static_cast<const ShaderParameterMessage *>(message)->GetParameterSlot() == parameterSlot);
}


DistributionController::DistributionController() : Controller(kControllerDistribution)
{
	distributionFlags = 0;
}

DistributionController::DistributionController(unsigned_int32 flags) : Controller(kControllerDistribution)
{
	distributionFlags = flags;
}

DistributionController::DistributionController(const DistributionController& distributionController) : Controller(distributionController)
{
	distributionFlags = distributionController.distributionFlags;
}

DistributionController::~DistributionController()
{
}

Controller *DistributionController::Replicate(void) const
{
	return (new DistributionController(*this));
}

void DistributionController::Pack(Packer& data, unsigned_int32 packFlags) const
{
	Controller::Pack(data, packFlags);

	data << ChunkHeader('FLAG', 4);
	data << distributionFlags;

	data << TerminatorChunk;
}

void DistributionController::Unpack(Unpacker& data, unsigned_int32 unpackFlags)
{
	Controller::Unpack(data, unpackFlags);
	UnpackChunkList<DistributionController>(data, unpackFlags);
}

bool DistributionController::UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags)
{
	switch (chunkHeader->chunkType)
	{
		case 'FLAG':

			data >> distributionFlags;
			return (true);
	}

	return (false);
}

int32 DistributionController::GetSettingCount(void) const
{
	return (3);
}

Setting *DistributionController::GetSetting(int32 index) const
{
	const StringTable *table = TheInterfaceMgr->GetStringTable();

	if (index == 0)
	{
		const char *title = table->GetString(StringID('CTRL', kControllerDistribution, 'SELF'));
		return (new BooleanSetting('SELF', ((distributionFlags & kDistributionSelf) != 0), title));
	}

	if (index == 1)
	{
		const char *title = table->GetString(StringID('CTRL', kControllerDistribution, 'SUBS'));
		return (new BooleanSetting('SUBS', ((distributionFlags & kDistributionSubnodes) != 0), title));
	}

	if (index == 2)
	{
		const char *title = table->GetString(StringID('CTRL', kControllerDistribution, 'CONN'));
		return (new BooleanSetting('CONN', ((distributionFlags & kDistributionConnectors) != 0), title));
	}

	return (nullptr);
}

void DistributionController::SetSetting(const Setting *setting)
{
	Type identifier = setting->GetSettingIdentifier();

	if (identifier == 'SELF')
	{
		if (static_cast<const BooleanSetting *>(setting)->GetBooleanValue())
		{
			distributionFlags |= kDistributionSelf;
		}
		else
		{
			distributionFlags &= ~kDistributionSelf;
		}
	}
	else if (identifier == 'SUBS')
	{
		if (static_cast<const BooleanSetting *>(setting)->GetBooleanValue())
		{
			distributionFlags |= kDistributionSubnodes;
		}
		else
		{
			distributionFlags &= ~kDistributionSubnodes;
		}
	}
	else if (identifier == 'CONN')
	{
		if (static_cast<const BooleanSetting *>(setting)->GetBooleanValue())
		{
			distributionFlags |= kDistributionConnectors;
		}
		else
		{
			distributionFlags &= ~kDistributionConnectors;
		}
	}
}

void DistributionController::ReceiveMessage(const ControllerMessage *message)
{
	unsigned_int32 flags = distributionFlags;

	if (flags & kDistributionConnectors)
	{
		const Hub *hub = GetTargetNode()->GetHub();
		if (hub)
		{
			const Connector *connector = hub->GetFirstOutgoingEdge();
			while (connector)
			{
				const Node *node = connector->GetConnectorTarget();
				if (node)
				{
					Controller *controller = node->GetController();
					if (controller)
					{
						controller->ReceiveMessage(message);
					}
				}

				connector = connector->GetNextOutgoingEdge();
			}
		}
	}

	if (flags & kDistributionSubnodes)
	{
		const Node *node = GetTargetNode()->GetFirstSubnode();
		while (node)
		{
			Controller *controller = node->GetController();
			if (controller)
			{
				controller->ReceiveMessage(message);
			}

			node = node->Next();
		}
	}

	if (flags & kDistributionSelf)
	{
		Controller::ReceiveMessage(message);
	}
}


ShaderParameterController::ShaderParameterController() :
		Controller(kControllerShaderParameter),
		Observer<ShaderParameterController, Controller>(this, &ShaderParameterController::HandleControllerEvent)
{
	SetControllerFlags(kControllerLocal | kControllerMoveInhibit);

	parameterValue.Set(0.0F, 0.0F, 0.0F, 0.0F);

	parameterSlot = 0;
	driverConnectorKey[0] = 0;
}

ShaderParameterController::ShaderParameterController(int32 slot, const char *connectorKey) :
		Controller(kControllerShaderParameter),
		Observer<ShaderParameterController, Controller>(this, &ShaderParameterController::HandleControllerEvent)
{
	SetControllerFlags(kControllerLocal | kControllerMoveInhibit);

	parameterValue.Set(0.0F, 0.0F, 0.0F, 0.0F);

	parameterSlot = slot;
	driverConnectorKey = connectorKey;
}

ShaderParameterController::ShaderParameterController(const ShaderParameterController& shaderParameterController) :
		Controller(shaderParameterController),
		Observer<ShaderParameterController, Controller>(this, &ShaderParameterController::HandleControllerEvent)
{
	parameterValue.Set(0.0F, 0.0F, 0.0F, 0.0F);

	parameterSlot = shaderParameterController.parameterSlot;
	driverConnectorKey = shaderParameterController.driverConnectorKey;
}

ShaderParameterController::~ShaderParameterController()
{
}

Controller *ShaderParameterController::Replicate(void) const
{
	return (new ShaderParameterController(*this));
}

bool ShaderParameterController::ValidNode(const Node *node)
{
	NodeType type = node->GetNodeType();
	return ((type == kNodeGeometry) || (type == kNodeEffect));
}

void ShaderParameterController::Pack(Packer& data, unsigned_int32 packFlags) const
{
	Controller::Pack(data, packFlags);

	if (!GetTargetNode()->GetManipulator())
	{
		data << ChunkHeader('PARM', sizeof(Vector4D));
		data << parameterValue;
	}

	data << ChunkHeader('SLOT', 4);
	data << parameterSlot;

	PackHandle handle = data.BeginChunk('DKEY');
	data << driverConnectorKey;
	data.EndChunk(handle);

	data << TerminatorChunk;
}

void ShaderParameterController::Unpack(Unpacker& data, unsigned_int32 unpackFlags)
{
	Controller::Unpack(data, unpackFlags);
	UnpackChunkList<ShaderParameterController>(data, unpackFlags);
}

bool ShaderParameterController::UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags)
{
	switch (chunkHeader->chunkType)
	{
		case 'PARM':

			data >> parameterValue;
			return (true);

		case 'SLOT':

			data >> parameterSlot;
			return (true);

		case 'DKEY':

			data >> driverConnectorKey;
			return (true);
	}

	return (false);
}

int32 ShaderParameterController::GetSettingCount(void) const
{
	return (2);
}

Setting *ShaderParameterController::GetSetting(int32 index) const
{
	const StringTable *table = TheInterfaceMgr->GetStringTable();

	if (index == 0)
	{
		const char *title = table->GetString(StringID('CTRL', kControllerShaderParameter, 'SLOT'));
		MenuSetting *menu = new MenuSetting('SLOT', parameterSlot, title, 8);

		for (machine a = 0; a < 8; a++)
		{
			menu->SetMenuItemString(a, table->GetString(StringID('CTRL', kControllerShaderParameter, 'SLOT', 'PRM0' + a)));
		}

		return (menu);
	}

	if (index == 1)
	{
		const char *title = table->GetString(StringID('CTRL', kControllerShaderParameter, 'DKEY'));
		return (new TextSetting('DKEY', driverConnectorKey, title, kMaxConnectorKeyLength, &Connector::ConnectorKeyFilter));
	}

	return (nullptr);
}

void ShaderParameterController::SetSetting(const Setting *setting)
{
	Type identifier = setting->GetSettingIdentifier();

	if (identifier == 'SLOT')
	{
		parameterSlot = static_cast<const MenuSetting *>(setting)->GetMenuSelection();
	}
	else if (identifier == 'DKEY')
	{
		driverConnectorKey = static_cast<const TextSetting *>(setting)->GetText();
	}
}

void ShaderParameterController::Preprocess(void)
{
	Controller::Preprocess();

	RenderableNode *targetNode = static_cast<RenderableNode *>(GetTargetNode());
	if (!targetNode->GetManipulator())
	{
		targetNode->SetShaderParameterProc(&GetShaderParameterData, this);
	}

	if (driverConnectorKey[0] != 0)
	{
		const Node *node = targetNode->GetConnectedNode(driverConnectorKey);
		if (node)
		{
			Controller *controller = node->GetController();
			if (controller)
			{
				controller->AddObserver(this);
			}
		}
	}
}

void ShaderParameterController::HandleControllerEvent(Controller *controller, ObservableEventType event)
{
	if (event == kEventControllerColorUpdate)
	{
		const float *value = static_cast<const float *>(controller->GetControllerEventData(kEventControllerColorUpdate));
		if (value)
		{
			parameterValue.Set(value[0], value[1], value[2], value[3]);
		}
	}
}

const float *ShaderParameterController::GetShaderParameterData(int32 slot, void *cookie)
{
	const ShaderParameterController *controller = static_cast<ShaderParameterController *>(cookie);

	if (slot == controller->parameterSlot)
	{
		return (&controller->parameterValue.x);
	}

	return (nullptr);
}


StrobeController::StrobeController() : Controller(kControllerStrobe)
{
	strobeState = kStrobeStatic;

	strobeColor[0].Set(0.0F, 0.0F, 0.0F, 0.0F);
	strobeColor[1].Set(1.0F, 1.0F, 1.0F, 1.0F);
	strobeHoldTime[0].Set(50.0F, 250.0F);
	strobeHoldTime[1].Set(50.0F, 250.0F);
	strobeTransitionTime.Set(0.0F, 0.0F);
}

StrobeController::StrobeController(const ColorRGB& color1, const ColorRGB& color2, const Range<float>& holdTime1, const Range<float>& holdTime2, const Range<float>& transitionTime) : Controller(kControllerStrobe)
{
	strobeState = kStrobeStatic;

	strobeColor[0].Set(color1, 0.0F);
	strobeColor[1].Set(color2, 1.0F);
	strobeHoldTime[0] = holdTime1;
	strobeHoldTime[1] = holdTime2;
	strobeTransitionTime = transitionTime;
}

StrobeController::StrobeController(const StrobeController& strobeController) : Controller(strobeController)
{
	strobeState = kStrobeStatic;

	strobeColor[0] = strobeController.strobeColor[0];
	strobeColor[1] = strobeController.strobeColor[1];
	strobeHoldTime[0] = strobeController.strobeHoldTime[0];
	strobeHoldTime[1] = strobeController.strobeHoldTime[1];
	strobeTransitionTime = strobeController.strobeTransitionTime;
}

StrobeController::~StrobeController()
{
}

Controller *StrobeController::Replicate(void) const
{
	return (new StrobeController(*this));
}

bool StrobeController::ValidNode(const Node *node)
{
	return (node->GetNodeType() == kNodeLight);
}

void StrobeController::Pack(Packer& data, unsigned_int32 packFlags) const
{
	Controller::Pack(data, packFlags);

	if (strobeState != kStrobeStatic)
	{
		data << ChunkHeader('STAT', 4);
		data << strobeState;

		data << ChunkHeader('COLR', sizeof(ColorRGBA));
		data << colorValue;

		data << ChunkHeader('HOLD', 4);
		data << colorHoldTime;

		data << ChunkHeader('ANGL', 4);
		data << transitionAngle;

		data << ChunkHeader('SPED', 4);
		data << transitionSpeed;
	}

	data << ChunkHeader('SCLR', sizeof(ColorRGBA) * 2);
	data << strobeColor[0];
	data << strobeColor[1];

	data << ChunkHeader('SHLD', sizeof(Range<float>) * 2);
	data << strobeHoldTime[0];
	data << strobeHoldTime[1];

	data << ChunkHeader('STRN', sizeof(Range<float>));
	data << strobeTransitionTime;

	data << TerminatorChunk;
}

void StrobeController::Unpack(Unpacker& data, unsigned_int32 unpackFlags)
{
	Controller::Unpack(data, unpackFlags);
	UnpackChunkList<StrobeController>(data, unpackFlags);
}

bool StrobeController::UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags)
{
	switch (chunkHeader->chunkType)
	{
		case 'STAT':

			data >> strobeState;
			return (true);

		case 'COLR':

			data >> colorValue;
			return (true);

		case 'HOLD':

			data >> colorHoldTime;
			return (true);

		case 'ANGL':

			data >> transitionAngle;
			return (true);

		case 'SPED':

			data >> transitionSpeed;
			return (true);

		case 'SCLR':

			data >> strobeColor[0];
			data >> strobeColor[1];
			return (true);

		case 'SHLD':

			data >> strobeHoldTime[0];
			data >> strobeHoldTime[1];
			return (true);

		case 'STRN':

			data >> strobeTransitionTime;
			return (true);
	}

	return (false);
}

int32 StrobeController::GetSettingCount(void) const
{
	return (8);
}

Setting *StrobeController::GetSetting(int32 index) const
{
	const StringTable *table = TheInterfaceMgr->GetStringTable();

	if (index == 0)
	{
		const char *title = table->GetString(StringID('CTRL', kControllerStrobe, 'CLR1'));
		const char *picker = table->GetString(StringID('CTRL', kControllerStrobe, 'PCK1'));
		return (new ColorSetting('CLR1', strobeColor[0], title, picker));
	}

	if (index == 1)
	{
		const char *title = table->GetString(StringID('CTRL', kControllerStrobe, 'CLR2'));
		const char *picker = table->GetString(StringID('CTRL', kControllerStrobe, 'PCK2'));
		return (new ColorSetting('CLR2', strobeColor[1], title, picker));
	}

	if (index == 2)
	{
		const char *title = table->GetString(StringID('CTRL', kControllerStrobe, 'HMN1'));
		return (new TextSetting('HMN1', strobeHoldTime[0].min * 0.001F, title));
	}

	if (index == 3)
	{
		const char *title = table->GetString(StringID('CTRL', kControllerStrobe, 'HMX1'));
		return (new TextSetting('HMX1', strobeHoldTime[0].max * 0.001F, title));
	}

	if (index == 4)
	{
		const char *title = table->GetString(StringID('CTRL', kControllerStrobe, 'HMN2'));
		return (new TextSetting('HMN2', strobeHoldTime[1].min * 0.001F, title));
	}

	if (index == 5)
	{
		const char *title = table->GetString(StringID('CTRL', kControllerStrobe, 'HMX2'));
		return (new TextSetting('HMX2', strobeHoldTime[1].max * 0.001F, title));
	}

	if (index == 6)
	{
		const char *title = table->GetString(StringID('CTRL', kControllerStrobe, 'TMIN'));
		return (new TextSetting('TMIN', strobeTransitionTime.min * 0.001F, title));
	}

	if (index == 7)
	{
		const char *title = table->GetString(StringID('CTRL', kControllerStrobe, 'TMAX'));
		return (new TextSetting('TMAX', strobeTransitionTime.max * 0.001F, title));
	}

	return (nullptr);
}

void StrobeController::SetSetting(const Setting *setting)
{
	Type identifier = setting->GetSettingIdentifier();

	if (identifier == 'CLR1')
	{
		strobeColor[0] = static_cast<const ColorSetting *>(setting)->GetColor();
	}
	else if (identifier == 'CLR2')
	{
		strobeColor[1] = static_cast<const ColorSetting *>(setting)->GetColor();
	}
	else if (identifier == 'HMN1')
	{
		const char *text = static_cast<const TextSetting *>(setting)->GetText();
		strobeHoldTime[0].min = FmaxZero(Text::StringToFloat(text)) * 1000.0F;
	}
	else if (identifier == 'HMX1')
	{
		const char *text = static_cast<const TextSetting *>(setting)->GetText();
		strobeHoldTime[0].max = FmaxZero(Text::StringToFloat(text)) * 1000.0F;
	}
	else if (identifier == 'HMN2')
	{
		const char *text = static_cast<const TextSetting *>(setting)->GetText();
		strobeHoldTime[1].min = FmaxZero(Text::StringToFloat(text)) * 1000.0F;
	}
	else if (identifier == 'HMX2')
	{
		const char *text = static_cast<const TextSetting *>(setting)->GetText();
		strobeHoldTime[1].max = FmaxZero(Text::StringToFloat(text)) * 1000.0F;
	}
	else if (identifier == 'TMIN')
	{
		const char *text = static_cast<const TextSetting *>(setting)->GetText();
		strobeTransitionTime.min = FmaxZero(Text::StringToFloat(text)) * 1000.0F;
	}
	else if (identifier == 'TMAX')
	{
		const char *text = static_cast<const TextSetting *>(setting)->GetText();
		strobeTransitionTime.max = FmaxZero(Text::StringToFloat(text)) * 1000.0F;
	}
}

void StrobeController::Preprocess(void)
{
	Controller::Preprocess();

	if (strobeState == kStrobeStatic)
	{
		strobeState = kStrobeTransition2;
		colorValue = strobeColor[0];
		transitionAngle = K::tau_over_2;
		transitionSpeed = 1.0F;
	}
}

void StrobeController::Sleep(void)
{
	Controller::Update();
	Controller::Sleep();
}

void StrobeController::Move(void)
{
	Invalidate();
	Controller::Sleep();
}

void StrobeController::Update(void)
{
	Controller::Update();
	Controller::Wake();

	float dt = TheTimeMgr->GetFloatDeltaTime();

	unsigned_int32 state = strobeState;
	if (state <= kStrobeHold2)
	{
		float time = colorHoldTime - dt;
		if (time > 0.0F)
		{
			colorHoldTime = time;
		}
		else
		{
			strobeState = state + 2;
			transitionAngle = 0.0F;

			const Range<float>& transition = strobeTransitionTime;
			transitionSpeed = K::tau_over_2 / Fmax((Math::RandomFloat(transition.max - transition.min) + transition.min), 1.0F);
		}
	}
	else
	{
		float angle = transitionAngle + transitionSpeed * dt;
		if (angle < K::tau_over_2)
		{
			transitionAngle = angle;

			float t = Cos(angle) * 0.5F;
			t = (state == kStrobeTransition1) ? 0.5F - t : 0.5F + t;

			colorValue.GetColorRGB() = strobeColor[0].GetColorRGB() * (1.0F - t) + strobeColor[1].GetColorRGB() * t;
			colorValue.alpha = t;
		}
		else
		{
			if (state == kStrobeTransition1)
			{
				strobeState = kStrobeHold2;
				colorValue = strobeColor[1];

				const Range<float>& hold = strobeHoldTime[1];
				colorHoldTime = Math::RandomFloat(hold.max - hold.min) + hold.min;
			}
			else
			{
				strobeState = kStrobeHold1;
				colorValue = strobeColor[0];

				const Range<float>& hold = strobeHoldTime[0];
				colorHoldTime = Math::RandomFloat(hold.max - hold.min) + hold.min;
			}
		}

		const Light *light = static_cast<Light *>(GetTargetNode());
		light->GetObject()->SetLightColor(colorValue.GetColorRGB());

		PostEvent(kEventControllerColorUpdate);
	}
}

const void *StrobeController::GetControllerEventData(EventType event) const
{
	if (event == kEventControllerColorUpdate)
	{
		return (&colorValue.red);
	}

	return (nullptr);
}


FlickerController::FlickerController() : Controller(kControllerFlicker)
{
	flickerFlags = 0;

	maxFlickerIntensity = 0.5F;
	maxMotionAmplitude = 0.01F;
}

FlickerController::FlickerController(float intensity, float amplitude) : Controller(kControllerFlicker)
{
	flickerFlags = 0;

	maxFlickerIntensity = intensity;
	maxMotionAmplitude = amplitude;
}

FlickerController::FlickerController(const FlickerController& flickerController) : Controller(flickerController)
{
	flickerFlags = 0;

	maxFlickerIntensity = flickerController.maxFlickerIntensity;
	maxMotionAmplitude = flickerController.maxMotionAmplitude;
}

FlickerController::~FlickerController()
{
}

Controller *FlickerController::Replicate(void) const
{
	return (new FlickerController(*this));
}

bool FlickerController::ValidNode(const Node *node)
{
	return ((node->GetNodeType() == kNodeLight) && (static_cast<const Light *>(node)->GetBaseLightType() == kLightPoint));
}

void FlickerController::Pack(Packer& data, unsigned_int32 packFlags) const
{
	Controller::Pack(data, packFlags);

	data << ChunkHeader('FLAG', 4);
	data << flickerFlags;

	data << ChunkHeader('INTS', 4);
	data << maxFlickerIntensity;

	data << ChunkHeader('AMPL', 4);
	data << maxMotionAmplitude;

	if (flickerFlags & kFlickerInitialized)
	{
		data << ChunkHeader('VALU', sizeof(ColorRGBA));
		data << colorValue;

		data << ChunkHeader('ORIG', sizeof(Point3D) + sizeof(ColorRGB));
		data << originalPosition;
		data << originalColor;
	}

	data << TerminatorChunk;
}

void FlickerController::Unpack(Unpacker& data, unsigned_int32 unpackFlags)
{
	Controller::Unpack(data, unpackFlags);
	UnpackChunkList<FlickerController>(data, unpackFlags);
}

bool FlickerController::UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags)
{
	switch (chunkHeader->chunkType)
	{
		case 'FLAG':

			data >> flickerFlags;
			return (true);

		case 'INTS':

			data >> maxFlickerIntensity;
			return (true);

		case 'AMPL':

			data >> maxMotionAmplitude;
			return (true);

		case 'VALU':

			data >> colorValue;
			return (true);

		case 'ORIG':

			data >> originalPosition;
			data >> originalColor;
			return (true);
	}

	return (false);
}

int32 FlickerController::GetSettingCount(void) const
{
	return (2);
}

Setting *FlickerController::GetSetting(int32 index) const
{
	const StringTable *table = TheInterfaceMgr->GetStringTable();

	if (index == 0)
	{
		const char *title = table->GetString(StringID('CTRL', kControllerFlicker, 'INTS'));
		return (new FloatSetting('INTS', maxFlickerIntensity, title, 0.05F, 1.0F, 0.05F));
	}

	if (index == 1)
	{
		const char *title = table->GetString(StringID('CTRL', kControllerFlicker, 'AMPL'));
		return (new FloatSetting('AMPL', maxMotionAmplitude, title, 0.001F, 0.05F, 0.001F));
	}

	return (nullptr);
}

void FlickerController::SetSetting(const Setting *setting)
{
	Type identifier = setting->GetSettingIdentifier();

	if (identifier == 'INTS')
	{
		maxFlickerIntensity = static_cast<const FloatSetting *>(setting)->GetFloatValue();
	}
	else if (identifier == 'AMPL')
	{
		maxMotionAmplitude = static_cast<const FloatSetting *>(setting)->GetFloatValue();
	}
}

void FlickerController::Preprocess(void)
{
	Controller::Preprocess();

	PointLight *light = static_cast<PointLight *>(GetTargetNode());
	if (!light->GetManipulator())
	{
		PointLightObject *object = light->GetObject();

		unsigned_int32 flags = object->GetLightFlags();
		if (flags & kLightStatic)
		{
			object->SetLightFlags((flags & ~kLightStatic) | kLightConfined);
			object->SetConfinementRadius(maxMotionAmplitude);
		}

		flags = flickerFlags;
		if (!(flags & kFlickerInitialized))
		{
			flickerFlags = flags | kFlickerInitialized;

			originalPosition = light->GetNodePosition();
			originalColor = object->GetLightColor();
			colorValue = originalColor;
		}
	}
}

void FlickerController::Neutralize(void)
{
	PointLight *light = static_cast<PointLight *>(GetTargetNode());
	if (!light->GetManipulator())
	{
		PointLightObject *object = light->GetObject();

		unsigned_int32 flags = object->GetLightFlags();
		if (flags & kLightConfined)
		{
			object->SetLightFlags((flags & ~kLightConfined) | kLightStatic);
			object->SetConfinementRadius(0.0F);
		}

		flags = flickerFlags;
		if (flags & kFlickerInitialized)
		{
			flickerFlags = flags & ~kFlickerInitialized;

			light->SetNodePosition(originalPosition);
			object->SetLightColor(originalColor);
		}
	}

	Controller::Neutralize();
}

void FlickerController::Move(void)
{
	if (flickerInterpolator.GetMode() == kInterpolatorStop)
	{
		flickerIntensity = Math::RandomFloat(maxFlickerIntensity);
		float rate = Math::RandomFloat(0.015F) + 0.002F;
		flickerInterpolator.Set(0.0F, rate, kInterpolatorForward | kInterpolatorOscillate);
	}

	float t = flickerInterpolator.UpdateValue() * flickerIntensity + 0.5F;
	colorValue.Set(originalColor * t, t);

	PointLight *light = static_cast<PointLight *>(GetTargetNode());
	light->GetObject()->SetLightColor(colorValue.GetColorRGB());

	PostEvent(kEventControllerColorUpdate);

	if (motionInterpolator.GetMode() == kInterpolatorStop)
	{
		const ConstVector2D *trig = Math::GetTrigTable();
		const Vector2D& cst = trig[Math::Random(256)];
		const Vector2D& csp = trig[Math::Random(128)];

		motionDirection.Set(cst.x * csp.y, cst.y * csp.y, csp.x);
		motionAmplitude = Math::RandomFloat(maxMotionAmplitude);
		float rate = Math::RandomFloat(0.005F) + 0.001F;
		motionInterpolator.Set(0.0F, rate, kInterpolatorForward);
	}

	float f = Sin(motionInterpolator.UpdateValue() * K::tau) * motionAmplitude;
	light->SetNodePosition(originalPosition + motionDirection * f);
	light->Invalidate();
}

const void *FlickerController::GetControllerEventData(EventType event) const
{
	if (event == kEventControllerColorUpdate)
	{
		return (&colorValue.red);
	}

	return (nullptr);
}


FlashController::FlashController() : Controller(kControllerFlash)
{
	flashFlags = 0;
	flashColor.Set(1.0F, 1.0F, 1.0F);
	flashStart = 0.0F;
	flashDuration = 1000;

	SetControllerFlags(GetControllerFlags() | kControllerAsleep);
}

FlashController::FlashController(const ColorRGB& color, float start, int32 duration) : Controller(kControllerFlash)
{
	flashFlags = kFlashNonpersistent;
	flashColor = color;
	flashStart = start;
	flashDuration = duration;
}

FlashController::FlashController(const FlashController& flashController) : Controller(flashController)
{
	flashFlags = flashController.flashFlags;
	flashColor = flashController.flashColor;
	flashStart = flashController.flashStart;
	flashDuration = flashController.flashDuration;
}

FlashController::~FlashController()
{
}

Controller *FlashController::Replicate(void) const
{
	return (new FlashController(*this));
}

bool FlashController::ValidNode(const Node *node)
{
	return (node->GetNodeType() == kNodeLight);
}

void FlashController::Pack(Packer& data, unsigned_int32 packFlags) const
{
	Controller::Pack(data, packFlags);

	data << ChunkHeader('FLAG', 4);
	data << flashFlags;

	data << ChunkHeader('COLR', sizeof(ColorRGB));
	data << flashColor;

	data << ChunkHeader('STAR', 4);
	data << flashStart;

	data << ChunkHeader('DURA', 4);
	data << flashDuration;

	if (flashFlags & kFlashInitialized)
	{
		PackHandle handle = data.BeginChunk('FLSH');
		flashInterpolator.Pack(data, packFlags);
		data.EndChunk(handle);

		data << ChunkHeader('VALU', sizeof(ColorRGBA));
		data << colorValue;
	}

	data << TerminatorChunk;
}

void FlashController::Unpack(Unpacker& data, unsigned_int32 unpackFlags)
{
	Controller::Unpack(data, unpackFlags);
	UnpackChunkList<FlashController>(data, unpackFlags);
}

bool FlashController::UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags)
{
	switch (chunkHeader->chunkType)
	{
		case 'FLAG':

			data >> flashFlags;
			return (true);

		case 'COLR':

			data >> flashColor;
			return (true);

		case 'STAR':

			data >> flashStart;
			return (true);

		case 'DURA':

			data >> flashDuration;
			return (true);

		case 'FLSH':

			flashInterpolator.Unpack(data, unpackFlags);
			return (true);

		case 'VALU':

			data >> colorValue;
			return (true);
	}

	return (false);
}

int32 FlashController::GetSettingCount(void) const
{
	return (3);
}

Setting *FlashController::GetSetting(int32 index) const
{
	const StringTable *table = TheInterfaceMgr->GetStringTable();

	if (index == 0)
	{
		const char *title = table->GetString(StringID('CTRL', kControllerFlash, 'COLR'));
		const char *picker = table->GetString(StringID('CTRL', kControllerFlash, 'PICK'));
		return (new ColorSetting('COLR', flashColor, title, picker));
	}

	if (index == 1)
	{
		const char *title = table->GetString(StringID('CTRL', kControllerFlash, 'STAR'));
		return (new FloatSetting('STAR', flashStart, title, 0.0F, 1.0F, 0.01F));
	}

	if (index == 2)
	{
		const char *title = table->GetString(StringID('CTRL', kControllerFlash, 'DURA'));
		return (new TextSetting('DURA', (float) flashDuration * 0.001F, title));
	}

	return (nullptr);
}

void FlashController::SetSetting(const Setting *setting)
{
	Type identifier = setting->GetSettingIdentifier();

	if (identifier == 'COLR')
	{
		flashColor = static_cast<const ColorSetting *>(setting)->GetColor().GetColorRGB();
	}
	else if (identifier == 'STAR')
	{
		flashStart = static_cast<const FloatSetting *>(setting)->GetFloatValue();
	}
	else if (identifier == 'DURA')
	{
		const char *text = static_cast<const TextSetting *>(setting)->GetText();
		flashDuration = Max((int32) (Text::StringToFloat(text) * 1000.0F + 0.5F), 1);
	}
}

void FlashController::Initialize(void)
{
	unsigned_int32 flags = flashFlags;
	if ((!(flags & kFlashInitialized)) && (!GetTargetNode()->GetManipulator()))
	{
		flashFlags = flags | kFlashInitialized;
		flashInterpolator.Set(flashStart, (2.0F - flashStart) / (float) flashDuration, kInterpolatorForward | kInterpolatorOscillate);
		colorValue.Set(flashColor * flashStart, flashStart);
	}
}

void FlashController::Preprocess(void)
{
	Controller::Preprocess();

	if (!Asleep())
	{
		Initialize();
	}
}

void FlashController::Wake(void)
{
	Initialize();
	GetTargetNode()->Enable();
	Controller::Wake();
}

void FlashController::Sleep(void)
{
	GetTargetNode()->Disable();
	Controller::Sleep();
}

void FlashController::Move(void)
{
	float value = flashInterpolator.UpdateValue();
	colorValue.Set(flashColor * value, value);

	Light *light = static_cast<Light *>(GetTargetNode());
	light->GetObject()->SetLightColor(colorValue.GetColorRGB());

	PostEvent(kEventControllerColorUpdate);

	if (flashInterpolator.GetMode() == kInterpolatorStop)
	{
		if (flashFlags & kFlashNonpersistent)
		{
			delete light;
			return;
		}

		Sleep();
	}
}

// ZYUQURM
