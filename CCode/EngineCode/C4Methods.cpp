 

#include "C4Methods.h"
#include "C4Expressions.h"
#include "C4Geometries.h"
#include "C4Sources.h"
#include "C4Triggers.h"
#include "C4Instances.h"
#include "C4Random.h"


using namespace C4;


const char C4::kConnectorKeyController[]	= "$CTRL";
const char C4::kConnectorKeyInitiator[]		= "$INTR";
const char C4::kConnectorKeyTrigger[]		= "$TRIG";


namespace C4
{
	template class Registrable<Method, MethodRegistration>;
}


const AttributeType SetMaterialColorMethod::materialAttributeType[kMaterialAttributeCount] =
{
	kAttributeDiffuse, kAttributeSpecular, kAttributeMicrofacet, kAttributeEmission, kAttributeEnvironment, kAttributeReflection, kAttributeRefraction
};


MethodRegistration::MethodRegistration(MethodType type, const char *name, unsigned_int32 flags, MethodGroup group) : Registration<Method, MethodRegistration>(type)
{
	methodFlags = flags;
	methodName = name;
	methodGroup = group;
}

MethodRegistration::~MethodRegistration()
{
}


Fiber::Fiber(Method *start, Method *finish) : GraphEdge<Method, Fiber>(start, finish)
{
	fiberFlags = 0;
	fiberState = 0;
}

Fiber::Fiber(const Fiber& fiber, Method *start, Method *finish) : GraphEdge<Method, Fiber>(start, finish)
{
	fiberFlags = fiber.fiberFlags;
	fiberState = 0;
}

Fiber::~Fiber()
{
}

void Fiber::Pack(Packer& data, unsigned_int32 packFlags) const
{
	data << ChunkHeader('FLAG', 4);
	data << fiberFlags;

	data << ChunkHeader('STAT', 4);
	data << fiberState;

	data << TerminatorChunk;
}

void Fiber::Unpack(Unpacker& data, unsigned_int32 unpackFlags)
{
	UnpackChunkList<Fiber>(data, unpackFlags);
}

bool Fiber::UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags)
{
	switch (chunkHeader->chunkType)
	{
		case 'FLAG':

			data >> fiberFlags;
			return (true);

		case 'STAT':

			data >> fiberState;
			return (true);
	}

	return (false);
}


Method::Method(MethodType type) : methodReference(this)
{
	methodType = type;
	methodState = 0;

	targetKey[0] = 0;
	outputValueName[0] = 0;

	methodPosition.Set(0.0F, 0.0F); 
}

Method::Method(const Method& method) : methodReference(this) 
{
	methodType = method.methodType; 
	methodState = 0;

	targetKey = method.targetKey; 
	outputValueName = method.outputValueName;
 
	const Setting *setting = method.inputValueList.First(); 
	while (setting)
	{
		Setting *clone = setting->Clone();
		if (clone) 
		{
			inputValueList.Append(clone);
		}

		setting = setting->Next();
	}

	methodPosition = method.methodPosition;
}

Method::~Method()
{
}

Method *Method::Replicate(void) const
{
	return (new Method(*this));
}

Method *Method::New(MethodType type)
{
	Type	data[2];

	data[0] = type;
	data[1] = 0;

	Unpacker unpacker(data);
	return (Create(unpacker));
}

Method *Method::CreateMethod(Unpacker& data, unsigned_int32 unpackFlags)
{
	if (data.GetType() == kMethodEvent)
	{
		return (new EventMethod);
	}

	return (nullptr);
}

Method *Method::CreateUnknown(Unpacker& data, unsigned_int32 unpackFlags)
{
	return (new UnknownMethod(data.GetType()));
}

void Method::RegisterStandardMethods(void)
{
	const StringTable *table = TheInterfaceMgr->GetStringTable();

	static MethodReg<Method> nullRegistration(kMethodNull, table->GetString(StringID('MTHD', 'NULL')), kMethodNoTarget, 'BASC');
	static MethodReg<SectionMethod> sectionRegistration(kMethodSection, "", kMethodNoTarget, 'BASC');
	static MethodReg<ExpressionMethod> expressionRegistration(kMethodExpression, table->GetString(StringID('MTHD', kMethodExpression)), kMethodNoTarget | kMethodOutputValue, 'BASC');
	static MethodReg<FunctionMethod> callFunctionRegistration(kMethodFunction, table->GetString(StringID('MTHD', kMethodFunction)), kMethodNoSelfTarget, 'BASC');
	static MethodReg<SettingMethod> changeSettingRegistration(kMethodSetting, table->GetString(StringID('MTHD', kMethodSetting)), 0, 'BASC');
	static MethodReg<ActivateMethod> activateRegistration(kMethodActivate, table->GetString(StringID('MTHD', kMethodActivate)), kMethodNoMessage, 'BASC');
	static MethodReg<DeactivateMethod> deactivateRegistration(kMethodDeactivate, table->GetString(StringID('MTHD', kMethodDeactivate)), kMethodNoMessage, 'BASC');
	static MethodReg<TerminateMethod> terminateRegistration(kMethodTerminate, table->GetString(StringID('MTHD', kMethodTerminate)), kMethodNoSelfTarget | kMethodNoMessage, 'BASC');
	static MethodReg<GetVariableMethod> getVariableRegistration(kMethodGetVariable, table->GetString(StringID('MTHD', kMethodGetVariable)), kMethodNoMessage | kMethodOutputValue, 'BASC');
	static MethodReg<SetVariableMethod> setVariableRegistration(kMethodSetVariable, table->GetString(StringID('MTHD', kMethodSetVariable)), kMethodNoMessage, 'BASC');
	static MethodReg<SetColorValueMethod> setColorValueRegistration(kMethodSetColorValue, table->GetString(StringID('MTHD', kMethodSetColorValue)), kMethodNoMessage | kMethodOutputValue, 'BASC');
	static MethodReg<SetVectorValueMethod> setVectorValueRegistration(kMethodSetVectorValue, table->GetString(StringID('MTHD', kMethodSetVectorValue)), kMethodNoMessage | kMethodOutputValue, 'BASC');
	static MethodReg<TimeMethod> timeRegistration(kMethodTime, table->GetString(StringID('MTHD', kMethodTime)), kMethodNoTarget | kMethodOutputValue, 'BASC');
	static MethodReg<DelayMethod> delayRegistration(kMethodDelay, table->GetString(StringID('MTHD', kMethodDelay)), kMethodNoTarget, 'BASC');

	static MethodReg<RandomIntegerMethod> randomIntegerRegistration(kMethodRandomInteger, table->GetString(StringID('MTHD', kMethodRandomInteger)), kMethodNoTarget | kMethodOutputValue, 'STND');
	static MethodReg<RandomFloatMethod> randomFloatRegistration(kMethodRandomFloat, table->GetString(StringID('MTHD', kMethodRandomFloat)), kMethodNoTarget | kMethodOutputValue, 'STND');
	static MethodReg<GetStringLengthMethod> getStringLengthRegistration(kMethodGetStringLength, table->GetString(StringID('MTHD', kMethodGetStringLength)), kMethodNoTarget | kMethodOutputValue, 'STND');
	static MethodReg<WakeControllerMethod> wakeControllerRegistration(kMethodWakeController, table->GetString(StringID('MTHD', kMethodWakeController)), 0, 'STND');
	static MethodReg<SleepControllerMethod> sleepControllerRegistration(kMethodSleepController, table->GetString(StringID('MTHD', kMethodSleepController)), 0, 'STND');
	static MethodReg<GetWakeStateMethod> getWakeStateRegistration(kMethodGetWakeState, table->GetString(StringID('MTHD', kMethodGetWakeState)), kMethodNoMessage | kMethodOutputValue, 'STND');
	static MethodReg<EnableNodeMethod> enableNodeRegistration(kMethodEnableNode, table->GetString(StringID('MTHD', kMethodEnableNode)), 0, 'STND');
	static MethodReg<DisableNodeMethod> disableNodeRegistration(kMethodDisableNode, table->GetString(StringID('MTHD', kMethodDisableNode)), 0, 'STND');
	static MethodReg<GetEnableStateMethod> getEnableStateRegistration(kMethodGetEnableState, table->GetString(StringID('MTHD', kMethodGetEnableState)), kMethodNoMessage | kMethodOutputValue, 'STND');
	static MethodReg<DeleteNodeMethod> deleteNodeRegistration(kMethodDeleteNode, table->GetString(StringID('MTHD', kMethodDeleteNode)), kMethodNoSelfTarget, 'STND');
	static MethodReg<EnableInteractivityMethod> enableInteractivityRegistration(kMethodEnableInteractivity, table->GetString(StringID('MTHD', kMethodEnableInteractivity)), 0, 'STND');
	static MethodReg<DisableInteractivityMethod> disableInteractivityRegistration(kMethodDisableInteractivity, table->GetString(StringID('MTHD', kMethodDisableInteractivity)), 0, 'STND');
	static MethodReg<GetInteractivityStateMethod> getInteractivityStateRegistration(kMethodGetInteractivityState, table->GetString(StringID('MTHD', kMethodGetInteractivityState)), kMethodNoMessage | kMethodOutputValue, 'STND');
	static MethodReg<ShowGeometryMethod> showGeometryRegistration(kMethodShowGeometry, table->GetString(StringID('MTHD', kMethodShowGeometry)), 0, 'STND');
	static MethodReg<HideGeometryMethod> hideGeometryRegistration(kMethodHideGeometry, table->GetString(StringID('MTHD', kMethodHideGeometry)), 0, 'STND');
	static MethodReg<GetVisibilityStateMethod> getVisibilityStateRegistration(kMethodGetVisibilityState, table->GetString(StringID('MTHD', kMethodGetVisibilityState)), kMethodNoMessage, 'STND');
	static MethodReg<ActivateTriggerMethod> activateTriggerRegistration(kMethodActivateTrigger, table->GetString(StringID('MTHD', kMethodActivateTrigger)), 0, 'STND');
	static MethodReg<DeactivateTriggerMethod> deactivateTriggerRegistration(kMethodDeactivateTrigger, table->GetString(StringID('MTHD', kMethodDeactivateTrigger)), 0, 'STND');
	static MethodReg<BreakJointMethod> breakJointRegistration(kMethodBreakJoint, table->GetString(StringID('MTHD', kMethodBreakJoint)), 0, 'STND');
	static MethodReg<RemoveModifiersMethod> removeModifiersRegistration(kMethodRemoveModifiers, table->GetString(StringID('MTHD', kMethodRemoveModifiers)), 0, 'STND');
	static MethodReg<GetNodePositionMethod> getNodePositionRegistration(kMethodGetNodePosition, table->GetString(StringID('MTHD', kMethodGetNodePosition)), kMethodNoMessage | kMethodOutputValue, 'STND');
	static MethodReg<GetWorldPositionMethod> getWorldPositionRegistration(kMethodGetWorldPosition, table->GetString(StringID('MTHD', kMethodGetWorldPosition)), kMethodNoMessage | kMethodOutputValue, 'STND');
	static MethodReg<PlaySoundMethod> playSoundRegistration(kMethodPlaySound, table->GetString(StringID('MTHD', kMethodPlaySound)), kMethodNoTarget, 'STND');
	static MethodReg<PlaySourceMethod> playSourceRegistration(kMethodPlaySource, table->GetString(StringID('MTHD', kMethodPlaySource)), 0, 'STND');
	static MethodReg<StopSourceMethod> stopSourceRegistration(kMethodStopSource, table->GetString(StringID('MTHD', kMethodStopSource)), 0, 'STND');
	static MethodReg<VarySourceVolumeMethod> varySourceVolumeRegistration(kMethodVarySourceVolume, table->GetString(StringID('MTHD', kMethodVarySourceVolume)), 0, 'STND');
	static MethodReg<VarySourceFrequencyMethod> varySourceFrequencyRegistration(kMethodVarySourceFrequency, table->GetString(StringID('MTHD', kMethodVarySourceFrequency)), 0, 'STND');
	static MethodReg<SetMaterialColorMethod> setMaterialColorRegistration(kMethodSetMaterialColor, table->GetString(StringID('MTHD', kMethodSetMaterialColor)), 0, 'STND');
	static MethodReg<SetShaderScalarParameterMethod> setShaderScalarParameterRegistration(kMethodSetShaderScalarParameter, table->GetString(StringID('MTHD', 'SHDR', kMethodSetShaderScalarParameter)), 0, 'STND');
	static MethodReg<SetShaderVectorParameterMethod> setShaderVectorParameterRegistration(kMethodSetShaderVectorParameter, table->GetString(StringID('MTHD', 'SHDR', kMethodSetShaderVectorParameter)), 0, 'STND');
	static MethodReg<SetShaderColorParameterMethod> setShaderColorParameterRegistration(kMethodSetShaderColorParameter, table->GetString(StringID('MTHD', 'SHDR', kMethodSetShaderColorParameter)), 0, 'STND');

	static Creator<Method> methodCreator(&CreateMethod);
	Method::InstallCreator(&methodCreator);

	SetUnknownCreator(&CreateUnknown);
}

void Method::PackType(Packer& data) const
{
	data << methodType;
}

void Method::Pack(Packer& data, unsigned_int32 packFlags) const
{
	data << ChunkHeader('STAT', 4);
	data << methodState;

	if (targetKey[0] != 0)
	{
		PackHandle handle = data.BeginChunk('TKEY');
		data << targetKey;
		data.EndChunk(handle);
	}

	if (outputValueName[0] != 0)
	{
		PackHandle handle = data.BeginChunk('OVAL');
		data << outputValueName;
		data.EndChunk(handle);
	}

	const Setting *setting = inputValueList.First();
	while (setting)
	{
		PackHandle handle = data.BeginChunk('IVAL');
		setting->PackType(data);
		setting->Pack(data, packFlags);
		data.EndChunk(handle);

		setting = setting->Next();
	}

	data << ChunkHeader('POSI', sizeof(Point2D));
	data << methodPosition;

	data << TerminatorChunk;
}

void Method::Unpack(Unpacker& data, unsigned_int32 unpackFlags)
{
	UnpackChunkList<Method>(data, unpackFlags);
}

bool Method::UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags)
{
	switch (chunkHeader->chunkType)
	{
		case 'STAT':

			data >> methodState;
			return (true);

		case 'TKEY':

			data >> targetKey;
			return (true);

		case 'OVAL':

			data >> outputValueName;
			return (true);

		case 'IVAL':
		{
			Setting *setting = Setting::Create(data, unpackFlags);
			if (setting)
			{
				setting->Unpack(++data, unpackFlags);
				inputValueList.Append(setting);
				return (true);
			}

			break;
		}

		case 'POSI':

			data >> methodPosition;
			return (true);
	}

	return (false);
}

void *Method::BeginSettingsUnpack(void)
{
	targetKey[0] = 0;
	outputValueName[0] = 0;

	inputValueList.Purge();
	return (nullptr);
}

Node *Method::GetTargetNode(const ScriptState *state) const
{
	if (targetKey[0] == '$')
	{
		if (Text::CompareText(&targetKey[1], &kConnectorKeyController[1]))
		{
			return (state->GetScriptControllerTarget());
		}

		if (Text::CompareText(&targetKey[1], &kConnectorKeyInitiator[1]))
		{
			return (state->GetInitiatorNode());
		}

		if (Text::CompareText(&targetKey[1], &kConnectorKeyTrigger[1]))
		{
			return (state->GetTriggerNode());
		}
	}
	else
	{
		return (state->GetScriptControllerTarget()->GetConnectedNode(targetKey));
	}

	return (nullptr);
}

void Method::ProcessInputValues(const ScriptState *state)
{
	Setting *setting = inputValueList.First();
	while (setting)
	{
		const Value *value = state->GetValue(setting->GetSettingValueName());
		if ((value) && (setting->SetValue(value)))
		{
			SetSetting(setting);
		}

		setting = setting->Next();
	}
}

void Method::SetOutputValue(const ScriptState *state, bool v)
{
	SetMethodResult(v);

	Value *output = state->GetValue(GetOutputValueName());
	if (output)
	{
		output->SetValue(v);
	}
}

void Method::SetOutputValue(const ScriptState *state, int32 v)
{
	SetMethodResult(v != 0);

	Value *output = state->GetValue(GetOutputValueName());
	if (output)
	{
		output->SetValue(v);
	}
}

void Method::SetOutputValue(const ScriptState *state, float v)
{
	SetMethodResult(v != 0.0F);

	Value *output = state->GetValue(GetOutputValueName());
	if (output)
	{
		output->SetValue(v);
	}
}

void Method::SetOutputValue(const ScriptState *state, const char *v)
{
	SetMethodResult(v[0] != 0);

	Value *output = state->GetValue(GetOutputValueName());
	if (output)
	{
		output->SetValue(v);
	}
}

void Method::SetOutputValue(const ScriptState *state, const ColorRGBA& v)
{
	SetMethodResult((v.red != 0.0F) || (v.green != 0.0F) || (v.blue != 0.0F) || (v.alpha != 0.0F));

	Value *output = state->GetValue(GetOutputValueName());
	if (output)
	{
		output->SetValue(v);
	}
}

void Method::SetOutputValue(const ScriptState *state, const Vector3D& v)
{
	SetMethodResult((v.x != 0.0F) || (v.y != 0.0F) || (v.z != 0.0F));

	Value *output = state->GetValue(GetOutputValueName());
	if (output)
	{
		output->SetValue(v);
	}
}

void Method::SetOutputValue(const ScriptState *state, const Value *v)
{
	SetMethodResult(v->GetBooleanValue());

	Value *output = state->GetValue(GetOutputValueName());
	if (output)
	{
		output->SetValue(v);
	}
}

void Method::Preprocess(const ScriptState *state)
{
}

void Method::Execute(const ScriptState *state)
{
	CallCompletionProc();
}

void Method::Resume(const ScriptState *state)
{
	Execute(state);
}


UnknownMethod::UnknownMethod(MethodType type) : Method(kMethodUnknown)
{
	unknownType = type;
	unknownSize = 0;
	unknownData = nullptr;
}

UnknownMethod::~UnknownMethod()
{
	delete[] unknownData;
}

UnknownMethod::UnknownMethod(const UnknownMethod& unknownMethod) : Method(unknownMethod)
{
	unknownType = unknownMethod.unknownType;
	unknownSize = unknownMethod.unknownSize;
	unknownData = new char[unknownSize];
	MemoryMgr::CopyMemory(unknownMethod.unknownData, unknownData, unknownSize);
}

Method *UnknownMethod::Replicate(void) const
{
	return (new UnknownMethod(*this));
}

void UnknownMethod::PackType(Packer& data) const
{
	data << unknownType;
}

void UnknownMethod::Pack(Packer& data, unsigned_int32 packFlags) const
{
	data.WriteData(unknownData, unknownSize);
}

void UnknownMethod::Unpack(Unpacker& data, unsigned_int32 unpackFlags)
{
	unsigned_int32 size = data.GetUnknownSize() - sizeof(MethodType);
	unknownSize = size;
	unknownData = new char[size];
	data.ReadData(unknownData, size);
}

int32 UnknownMethod::GetSettingCount(void) const
{
	return (1);
}

Setting *UnknownMethod::GetSetting(int32 index) const
{
	if (index == 0)
	{
		const StringTable *table = TheInterfaceMgr->GetStringTable();
		const char *title = table->GetString(StringID('MTHD', kMethodUnknown, 'TYPE'));
		return (new InfoSetting('TYPE', Text::TypeToHexCharString(unknownType), title));
	}

	return (nullptr);
}

void UnknownMethod::SetSetting(const Setting *setting)
{
}


SectionMethod::SectionMethod() : Method(kMethodSection)
{
	sectionWidth = 0.0F;
	sectionHeight = 0.0F;

	sectionColor.Set(0.96875F, 0.96875F, 0.96875F);
}

SectionMethod::SectionMethod(const SectionMethod& sectionMethod) :
		Method(sectionMethod),
		sectionComment(sectionMethod.sectionComment)
{
	sectionWidth = sectionMethod.sectionWidth;
	sectionHeight = sectionMethod.sectionHeight;

	sectionColor = sectionMethod.sectionColor;
}

SectionMethod::~SectionMethod()
{
}

Method *SectionMethod::Replicate(void) const
{
	return (new SectionMethod(*this));
}

void SectionMethod::Pack(Packer& data, unsigned_int32 packFlags) const
{
	Method::Pack(data, packFlags);

	data << ChunkHeader('SIZE', 8);
	data << sectionWidth;
	data << sectionHeight;

	data << ChunkHeader('COLR', sizeof(ColorRGBA));
	data << sectionColor;

	if (sectionComment.Length() != 0)
	{
		PackHandle handle = data.BeginChunk('CMNT');
		data << sectionComment;
		data.EndChunk(handle);
	}

	data << TerminatorChunk;
}

void SectionMethod::Unpack(Unpacker& data, unsigned_int32 unpackFlags)
{
	Method::Unpack(data, unpackFlags);
	UnpackChunkList<SectionMethod>(data, unpackFlags);
}

bool SectionMethod::UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags)
{
	switch (chunkHeader->chunkType)
	{
		case 'SIZE':

			data >> sectionWidth;
			data >> sectionHeight;
			return (true);

		case 'COLR':

			data >> sectionColor;
			return (true);

		case 'CMNT':

			data >> sectionComment;
			return (true);
	}

	return (false);
}

void *SectionMethod::BeginSettingsUnpack(void)
{
	sectionComment.Purge();
	return (Method::BeginSettingsUnpack());
}

int32 SectionMethod::GetSettingCount(void) const
{
	return (2);
}

Setting *SectionMethod::GetSetting(int32 index) const
{
	const StringTable *table = TheInterfaceMgr->GetStringTable();

	if (index == 0)
	{
		const char *title = table->GetString(StringID('MTHD', kMethodSection, 'CMNT'));
		const char *string = sectionComment;
		return (new TextSetting('CMNT', (string) ? string : "", title, 255));
	}
	else if (index == 1)
	{
		const char *title = table->GetString(StringID('MTHD', kMethodSection, 'COLR'));
		const char *picker = table->GetString(StringID('MTHD', kMethodSection, 'PICK'));
		return (new ColorSetting('COLR', sectionColor, title, picker));
	}

	return (nullptr);
}

void SectionMethod::SetSetting(const Setting *setting)
{
	Type identifier = setting->GetSettingIdentifier();

	if (identifier == 'CMNT')
	{
		sectionComment = static_cast<const TextSetting *>(setting)->GetText();
	}
	else if (identifier == 'COLR')
	{
		sectionColor = static_cast<const ColorSetting *>(setting)->GetColor();
	}
}


FunctionMethod::FunctionMethod() : Method(kMethodFunction)
{
	methodFunction = nullptr;
}

FunctionMethod::FunctionMethod(const FunctionMethod& functionMethod) : Method(functionMethod)
{
	const Function *function = functionMethod.methodFunction;
	methodFunction = (function) ? function->Clone() : nullptr;
}

FunctionMethod::~FunctionMethod()
{
	delete methodFunction;
}

Method *FunctionMethod::Replicate(void) const
{
	return (new FunctionMethod(*this));
}

void FunctionMethod::Pack(Packer& data, unsigned_int32 packFlags) const
{
	Method::Pack(data, packFlags);

	if (methodFunction)
	{
		data << int32(1);

		data << methodFunction->GetFunctionType();
		data << methodFunction->GetControllerType();

		PackHandle section = data.BeginSection();
		methodFunction->Pack(data, packFlags);
		data.EndSection(section);
	}
	else
	{
		data << int32(0);
	}
}

void FunctionMethod::Unpack(Unpacker& data, unsigned_int32 unpackFlags)
{
	int32	flag;

	Method::Unpack(data, unpackFlags);

	data >> flag;
	if (flag != 0)
	{
		unsigned_int32		size;
		FunctionType		functionType;
		ControllerType		controllerType;

		data >> functionType;
		data >> controllerType;
		data >> size;

		const void *mark = data.GetPointer();

		const ControllerRegistration *registration = Controller::FindRegistration(controllerType);
		if (registration)
		{
			Function *function = registration->CreateFunction(functionType);
			if (function)
			{
				function->Unpack(data, unpackFlags);
				methodFunction = function;
			}
			else
			{
				data.Skip(mark, size);
			}
		}
		else
		{
			data.Skip(mark, size);
		}
	}
}

void *FunctionMethod::BeginSettingsUnpack(void)
{
	delete methodFunction;
	methodFunction = nullptr;

	return (Method::BeginSettingsUnpack());
}

void FunctionMethod::ProcessInputValues(const ScriptState *state)
{
	if (methodFunction)
	{
		Setting *setting = GetFirstInputValue();
		while (setting)
		{
			const Value *value = state->GetValue(setting->GetSettingValueName());
			if ((value) && (setting->SetValue(value)))
			{
				methodFunction->SetSetting(setting);
			}

			setting = setting->Next();
		}
	}
}

void FunctionMethod::FunctionComplete(Function *function, void *cookie)
{
	static_cast<FunctionMethod *>(cookie)->CallCompletionProc();
}

void FunctionMethod::Preprocess(const ScriptState *state)
{
	if (methodFunction)
	{
		Node *node = GetTargetNode(state);
		if (node)
		{
			Controller *controller = node->GetController();
			if (controller)
			{
				methodFunction->Preprocess(controller, this, state);
			}
		}
	}
}

void FunctionMethod::Execute(const ScriptState *state)
{
	if (methodFunction)
	{
		Node *node = GetTargetNode(state);
		if (node)
		{
			Controller *controller = node->GetController();
			if (controller)
			{
				ControllerType controllerType = controller->GetControllerType();
				if (controllerType == methodFunction->GetControllerType())
				{
					methodFunction->SetCompletionProc(&FunctionComplete, this);

					unsigned_int32 flags = Controller::FindRegistration(controllerType)->FindFunctionRegistration(methodFunction->GetFunctionType())->GetFunctionFlags();
					if ((flags & kFunctionRemote) && (!(controller->GetControllerFlags() & kControllerLocal)))
					{
						if (flags & kFunctionJournaled)
						{
							TheMessageMgr->SendMessageJournal(new FunctionMessage(controller->GetControllerIndex(), methodFunction));
						}
						else
						{
							TheMessageMgr->SendMessageAll(FunctionMessage(controller->GetControllerIndex(), methodFunction));
						}
					}
					else
					{
						methodFunction->Execute(controller, this, state);
					}

					return;
				}
			}
		}
	}

	CallCompletionProc();
}

void FunctionMethod::Resume(const ScriptState *state)
{
	if (methodFunction)
	{
		Node *node = GetTargetNode(state);
		if (node)
		{
			methodFunction->SetCompletionProc(&FunctionComplete, this);
			methodFunction->Resume(node->GetController(), this, state);
			return;
		}
	}

	CallCompletionProc();
}


SettingMethod::SettingMethod() : Method(kMethodSetting)
{
	categoryType = 0;
}

SettingMethod::SettingMethod(const SettingMethod& settingMethod) : Method(settingMethod)
{
	categoryType = settingMethod.categoryType;

	const Setting *setting = settingMethod.settingList.First();
	while (setting)
	{
		Setting *clone = setting->Clone();
		if (clone)
		{
			settingList.Append(clone);
		}

		setting = setting->Next();
	}
}

SettingMethod::~SettingMethod()
{
}

Method *SettingMethod::Replicate(void) const
{
	return (new SettingMethod(*this));
}

void SettingMethod::Pack(Packer& data, unsigned_int32 packFlags) const
{
	Method::Pack(data, packFlags);

	data << ChunkHeader('CTGY', 4);
	data << categoryType;

	const Setting *setting = settingList.First();
	while (setting)
	{
		PackHandle handle = data.BeginChunk('SETT');
		setting->PackType(data);
		setting->Pack(data, packFlags);
		data.EndChunk(handle);

		setting = setting->Next();
	}

	data << TerminatorChunk;
}

void SettingMethod::Unpack(Unpacker& data, unsigned_int32 unpackFlags)
{
	Method::Unpack(data, unpackFlags);
	UnpackChunkList<SettingMethod>(data, unpackFlags);
}

bool SettingMethod::UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags)
{
	switch (chunkHeader->chunkType)
	{
		case 'CTGY':

			data >> categoryType;
			return (true);

		case 'SETT':
		{
			Setting *setting = Setting::Create(data, unpackFlags);
			if (setting)
			{
				setting->Unpack(++data, unpackFlags);
				settingList.Append(setting);
				return (true);
			}

			break;
		}
	}

	return (false);
}

void *SettingMethod::BeginSettingsUnpack(void)
{
	settingList.Purge();
	return (Method::BeginSettingsUnpack());
}

void SettingMethod::Execute(const ScriptState *state)
{
	Node *node = GetTargetNode(state);
	if (node)
	{
		Controller *controller = node->GetController();
		if (controller)
		{
			Object *object = node->GetObject();
			if (object)
			{
				Type category = categoryType;
				int32 controllerIndex = controller->GetControllerIndex();

				const Setting *setting = settingList.First();
				while (setting)
				{
					TheMessageMgr->SendMessageJournal(new SettingMessage(controllerIndex, category, setting));
					setting = setting->Next();
				}

				Setting *input = GetFirstInputValue();
				while (input)
				{
					const Value *value = state->GetValue(input->GetSettingValueName());
					if ((value) && (input->SetValue(value)))
					{
						TheMessageMgr->SendMessageJournal(new SettingMessage(controllerIndex, category, input));
					}

					input = input->Next();
				}
			}
		}
	}

	CallCompletionProc();
}


ActivateMethod::ActivateMethod() : Method(kMethodActivate)
{
}

ActivateMethod::ActivateMethod(const ActivateMethod& activateMethod) : Method(activateMethod)
{
}

ActivateMethod::~ActivateMethod()
{
}

Method *ActivateMethod::Replicate(void) const
{
	return (new ActivateMethod(*this));
}

void ActivateMethod::Execute(const ScriptState *state)
{
	Node *node = GetTargetNode(state);
	if (node)
	{
		Controller *controller = node->GetController();
		if (controller)
		{
			controller->Activate(state->GetInitiatorNode(), state->GetTriggerNode());
		}
	}

	CallCompletionProc();
}


DeactivateMethod::DeactivateMethod() : Method(kMethodDeactivate)
{
}

DeactivateMethod::DeactivateMethod(const DeactivateMethod& deactivateMethod) : Method(deactivateMethod)
{
}

DeactivateMethod::~DeactivateMethod()
{
}

Method *DeactivateMethod::Replicate(void) const
{
	return (new DeactivateMethod(*this));
}

void DeactivateMethod::Execute(const ScriptState *state)
{
	Node *node = GetTargetNode(state);
	if (node)
	{
		Controller *controller = node->GetController();
		if (controller)
		{
			controller->Deactivate(state->GetInitiatorNode(), state->GetTriggerNode());
		}
	}

	CallCompletionProc();
}


TerminateMethod::TerminateMethod() : Method(kMethodTerminate)
{
}

TerminateMethod::TerminateMethod(const TerminateMethod& terminateMethod) : Method(terminateMethod)
{
}

TerminateMethod::~TerminateMethod()
{
}

Method *TerminateMethod::Replicate(void) const
{
	return (new TerminateMethod(*this));
}

void TerminateMethod::Execute(const ScriptState *state)
{
	Node *node = GetTargetNode(state);
	if (node)
	{
		Controller *controller = node->GetController();
		if ((controller) && (controller->GetBaseControllerType() == kControllerScript) && (controller != state->GetScriptController()))
		{
			static_cast<ScriptController *>(controller)->Terminate();
		}
	}

	CallCompletionProc();
}


GetVariableMethod::GetVariableMethod() : Method(kMethodGetVariable)
{
	valueName[0] = 0;
}

GetVariableMethod::GetVariableMethod(const char *name) : Method(kMethodGetVariable)
{
	valueName = name;
}

GetVariableMethod::GetVariableMethod(const GetVariableMethod& getVariableMethod) : Method(getVariableMethod)
{
	valueName = getVariableMethod.valueName;
}

GetVariableMethod::~GetVariableMethod()
{
}

Method *GetVariableMethod::Replicate(void) const
{
	return (new GetVariableMethod(*this));
}

void GetVariableMethod::Pack(Packer& data, unsigned_int32 packFlags) const
{
	Method::Pack(data, packFlags);

	data << valueName;
}

void GetVariableMethod::Unpack(Unpacker& data, unsigned_int32 unpackFlags)
{
	Method::Unpack(data, unpackFlags);

	data >> valueName;
}

int32 GetVariableMethod::GetSettingCount(void) const
{
	return (1);
}

Setting *GetVariableMethod::GetSetting(int32 index) const
{
	const StringTable *table = TheInterfaceMgr->GetStringTable();
	const char *title = table->GetString(StringID('MTHD', kMethodGetVariable, 'NAME'));
	return (new TextSetting('NAME', valueName, title, kMaxValueNameLength, &EditTextWidget::IdentifierFilter));
}

void GetVariableMethod::SetSetting(const Setting *setting)
{
	if (setting->GetSettingIdentifier() == 'NAME')
	{
		valueName = static_cast<const TextSetting *>(setting)->GetText();
	}
}

void GetVariableMethod::Execute(const ScriptState *state)
{
	const Node *node = GetTargetNode(state);
	if (node)
	{
		const Controller *controller = node->GetController();
		if ((controller) && (controller->GetBaseControllerType() == kControllerScript))
		{
			const ScriptController *scriptController = static_cast<const ScriptController *>(controller);

			const ScriptObject *object = scriptController->GetScriptObject();
			if (object)
			{
				const Value *value = object->GetValue(valueName);
				if (value)
				{
					SetOutputValue(state, value);
				}
				else
				{
					value = scriptController->GetValue(valueName);
					if (value)
					{
						SetOutputValue(state, value);
					}
				}
			}
		}
	}

	CallCompletionProc();
}


SetVariableMethod::SetVariableMethod() : Method(kMethodSetVariable)
{
	valueName[0] = 0;
	sourceName[0] = 0;
}

SetVariableMethod::SetVariableMethod(const char *name, const char *source) : Method(kMethodSetVariable)
{
	valueName = name;
	sourceName = source;
}

SetVariableMethod::SetVariableMethod(const SetVariableMethod& setVariableMethod) : Method(setVariableMethod)
{
	valueName = setVariableMethod.valueName;
	sourceName = setVariableMethod.sourceName;
}

SetVariableMethod::~SetVariableMethod()
{
}

Method *SetVariableMethod::Replicate(void) const
{
	return (new SetVariableMethod(*this));
}

void SetVariableMethod::Pack(Packer& data, unsigned_int32 packFlags) const
{
	Method::Pack(data, packFlags);

	data << valueName;
	data << sourceName;
}

void SetVariableMethod::Unpack(Unpacker& data, unsigned_int32 unpackFlags)
{
	Method::Unpack(data, unpackFlags);

	data >> valueName;
	data >> sourceName;
}

int32 SetVariableMethod::GetSettingCount(void) const
{
	return (2);
}

Setting *SetVariableMethod::GetSetting(int32 index) const
{
	const StringTable *table = TheInterfaceMgr->GetStringTable();

	if (index == 0)
	{
		const char *title = table->GetString(StringID('MTHD', kMethodSetVariable, 'NAME'));
		return (new TextSetting('NAME', valueName, title, kMaxValueNameLength, &EditTextWidget::IdentifierFilter));
	}
	else if (index == 1)
	{
		const char *title = table->GetString(StringID('MTHD', kMethodSetVariable, 'SORC'));
		return (new TextSetting('SORC', sourceName, title, kMaxValueNameLength, &EditTextWidget::IdentifierFilter));
	}

	return (nullptr);
}

void SetVariableMethod::SetSetting(const Setting *setting)
{
	Type identifier = setting->GetSettingIdentifier();

	if (identifier == 'NAME')
	{
		valueName = static_cast<const TextSetting *>(setting)->GetText();
	}
	else if (identifier == 'SORC')
	{
		sourceName = static_cast<const TextSetting *>(setting)->GetText();
	}
}

void SetVariableMethod::Execute(const ScriptState *state)
{
	const Value *source = state->GetValue(sourceName);
	if (source)
	{
		const Node *node = GetTargetNode(state);
		if (node)
		{
			const Controller *controller = node->GetController();
			if ((controller) && (controller->GetBaseControllerType() == kControllerScript))
			{
				const ScriptController *scriptController = static_cast<const ScriptController *>(controller);

				const ScriptObject *object = scriptController->GetScriptObject();
				if (object)
				{
					Value *value = object->GetValue(valueName);
					if (value)
					{
						value->SetValue(source);
					}
					else
					{
						value = scriptController->GetValue(valueName);
						if (value)
						{
							value->SetValue(source);
						}
					}
				}
			}
		}
	}

	CallCompletionProc();
}


SetColorValueMethod::SetColorValueMethod() : Method(kMethodSetColorValue)
{
	colorValue.Set(0.0F, 0.0F, 0.0F, 0.0F);
}

SetColorValueMethod::SetColorValueMethod(const ColorRGBA& value) : Method(kMethodSetColorValue)
{
	colorValue = value;
}

SetColorValueMethod::SetColorValueMethod(const SetColorValueMethod& setColorValueMethod) : Method(setColorValueMethod)
{
	colorValue = setColorValueMethod.colorValue;
}

SetColorValueMethod::~SetColorValueMethod()
{
}

Method *SetColorValueMethod::Replicate(void) const
{
	return (new SetColorValueMethod(*this));
}

void SetColorValueMethod::Pack(Packer& data, unsigned_int32 packFlags) const
{
	Method::Pack(data, packFlags);

	data << colorValue;
}

void SetColorValueMethod::Unpack(Unpacker& data, unsigned_int32 unpackFlags)
{
	Method::Unpack(data, unpackFlags);

	data >> colorValue;
}

int32 SetColorValueMethod::GetSettingCount(void) const
{
	return (4);
}

Setting *SetColorValueMethod::GetSetting(int32 index) const
{
	if ((unsigned_int32) index < 4U)
	{
		const StringTable *table = TheInterfaceMgr->GetStringTable();

		Type identifier = 'CHN0' + index;
		const char *title = table->GetString(StringID('MTHD', kMethodSetColorValue, identifier));
		return (new TextSetting(identifier, colorValue[index], title));
	}

	return (nullptr);
}

void SetColorValueMethod::SetSetting(const Setting *setting)
{
	unsigned_int32 index = setting->GetSettingIdentifier() - 'CHN0';
	if (index < 4U)
	{
		const char *text = static_cast<const TextSetting *>(setting)->GetText();
		colorValue[index] = Text::StringToFloat(text);
	}
}

void SetColorValueMethod::Execute(const ScriptState *state)
{
	SetOutputValue(state, colorValue);
	CallCompletionProc();
}


SetVectorValueMethod::SetVectorValueMethod() : Method(kMethodSetVectorValue)
{
	vectorValue.Set(0.0F, 0.0F, 0.0F);
}

SetVectorValueMethod::SetVectorValueMethod(const Vector3D& value) : Method(kMethodSetVectorValue)
{
	vectorValue = value;
}

SetVectorValueMethod::SetVectorValueMethod(const SetVectorValueMethod& setVectorValueMethod) : Method(setVectorValueMethod)
{
	vectorValue = setVectorValueMethod.vectorValue;
}

SetVectorValueMethod::~SetVectorValueMethod()
{
}

Method *SetVectorValueMethod::Replicate(void) const
{
	return (new SetVectorValueMethod(*this));
}

void SetVectorValueMethod::Pack(Packer& data, unsigned_int32 packFlags) const
{
	Method::Pack(data, packFlags);

	data << vectorValue;
}

void SetVectorValueMethod::Unpack(Unpacker& data, unsigned_int32 unpackFlags)
{
	Method::Unpack(data, unpackFlags);

	data >> vectorValue;
}

int32 SetVectorValueMethod::GetSettingCount(void) const
{
	return (3);
}

Setting *SetVectorValueMethod::GetSetting(int32 index) const
{
	if ((unsigned_int32) index < 3U)
	{
		const StringTable *table = TheInterfaceMgr->GetStringTable();

		Type identifier = 'COM0' + index;
		const char *title = table->GetString(StringID('MTHD', kMethodSetVectorValue, identifier));
		return (new TextSetting(identifier, vectorValue[index], title));
	}

	return (nullptr);
}

void SetVectorValueMethod::SetSetting(const Setting *setting)
{
	unsigned_int32 index = setting->GetSettingIdentifier() - 'COM0';
	if (index < 3U)
	{
		const char *text = static_cast<const TextSetting *>(setting)->GetText();
		vectorValue[index] = Text::StringToFloat(text);
	}
}

void SetVectorValueMethod::Execute(const ScriptState *state)
{
	SetOutputValue(state, vectorValue);
	CallCompletionProc();
}


EventMethod::EventMethod() : Method(kMethodEvent)
{
}

EventMethod::EventMethod(EventType eventType) : Method(kMethodEvent)
{
	scriptEvent = eventType;
}

EventMethod::EventMethod(const EventMethod& eventMethod) : Method(eventMethod)
{
	scriptEvent = eventMethod.scriptEvent;
}

EventMethod::~EventMethod()
{
}

Method *EventMethod::Replicate(void) const
{
	return (new EventMethod(*this));
}

void EventMethod::Pack(Packer& data, unsigned_int32 packFlags) const
{
	Method::Pack(data, packFlags);

	data << scriptEvent;
}

void EventMethod::Unpack(Unpacker& data, unsigned_int32 unpackFlags)
{
	Method::Unpack(data, unpackFlags);

	data >> scriptEvent;
}

void EventMethod::Execute(const ScriptState *state)
{
	SetMethodResult(state->GetScriptEvent() == scriptEvent);
	CallCompletionProc();
}


TimeMethod::TimeMethod() : Method(kMethodTime)
{
}

TimeMethod::TimeMethod(const TimeMethod& timeMethod) : Method(timeMethod)
{
}

TimeMethod::~TimeMethod()
{
}

Method *TimeMethod::Replicate(void) const
{
	return (new TimeMethod(*this));
}

void TimeMethod::Execute(const ScriptState *state)
{
	SetOutputValue(state, state->GetScriptTime() * 0.001F);
	CallCompletionProc();
}


DelayMethod::DelayMethod() :
		Method(kMethodDelay),
		timer(&DelayComplete, this)
{
	delayTime = 0;
}

DelayMethod::DelayMethod(int32 time) :
		Method(kMethodDelay),
		timer(&DelayComplete, this)
{
	delayTime = time;
}

DelayMethod::DelayMethod(const DelayMethod& delayMethod) :
		Method(delayMethod),
		timer(&DelayComplete, this)
{
	delayTime = delayMethod.delayTime;
}

DelayMethod::~DelayMethod()
{
}

Method *DelayMethod::Replicate(void) const
{
	return (new DelayMethod(*this));
}

void DelayMethod::Pack(Packer& data, unsigned_int32 packFlags) const
{
	Method::Pack(data, packFlags);

	data << ChunkHeader('TIME', 4);
	data << delayTime;

	if (timer.GetOwningList())
	{
		data << ChunkHeader('RMNG', 4);
		data << timer.GetTime();
	}

	data << TerminatorChunk;
}

void DelayMethod::Unpack(Unpacker& data, unsigned_int32 unpackFlags)
{
	Method::Unpack(data, unpackFlags);
	UnpackChunkList<DelayMethod>(data, unpackFlags);
}

bool DelayMethod::UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags)
{
	switch (chunkHeader->chunkType)
	{
		case 'TIME':

			data >> delayTime;
			return (true);

		case 'RMNG':
		{
			int32	time;

			data >> time;
			timer.SetTime(time);
			return (true);
		}
	}

	return (false);
}

int32 DelayMethod::GetSettingCount(void) const
{
	return (1);
}

Setting *DelayMethod::GetSetting(int32 index) const
{
	const StringTable *table = TheInterfaceMgr->GetStringTable();
	const char *title = table->GetString(StringID('MTHD', kMethodDelay, 'TIME'));
	return (new TextSetting('TIME', (float) delayTime * 0.001F, title));
}

void DelayMethod::SetSetting(const Setting *setting)
{
	if (setting->GetSettingIdentifier() == 'TIME')
	{
		const char *text = static_cast<const TextSetting *>(setting)->GetText();
		delayTime = MaxZero((int32) (Text::StringToFloat(text) * 1000.0F));
	}
}

void DelayMethod::DelayComplete(DeferredTask *timer, void *cookie)
{
	static_cast<DelayMethod *>(cookie)->CallCompletionProc();
}

void DelayMethod::Execute(const ScriptState *state)
{
	timer.SetTime(delayTime);
	TheTimeMgr->AddTask(&timer);
}

void DelayMethod::Resume(const ScriptState *state)
{
	TheTimeMgr->AddTask(&timer);
}


RandomIntegerMethod::RandomIntegerMethod() : Method(kMethodRandomInteger)
{
	minInteger = 0;
	maxInteger = 1;
}

RandomIntegerMethod::RandomIntegerMethod(const RandomIntegerMethod& randomIntegerMethod) : Method(randomIntegerMethod)
{
	minInteger = randomIntegerMethod.minInteger;
	maxInteger = randomIntegerMethod.maxInteger;
}

RandomIntegerMethod::~RandomIntegerMethod()
{
}

Method *RandomIntegerMethod::Replicate(void) const
{
	return (new RandomIntegerMethod(*this));
}

void RandomIntegerMethod::Pack(Packer& data, unsigned_int32 packFlags) const
{
	Method::Pack(data, packFlags);

	data << minInteger;
	data << maxInteger;
}

void RandomIntegerMethod::Unpack(Unpacker& data, unsigned_int32 unpackFlags)
{
	Method::Unpack(data, unpackFlags);

	data >> minInteger;
	data >> maxInteger;
}

int32 RandomIntegerMethod::GetSettingCount(void) const
{
	return (2);
}

Setting *RandomIntegerMethod::GetSetting(int32 index) const
{
	const StringTable *table = TheInterfaceMgr->GetStringTable();

	if (index == 0)
	{
		const char *title = table->GetString(StringID('MTHD', kMethodRandomInteger, 'MINV'));
		return (new TextSetting('MINV', Text::IntegerToString(minInteger), title, 11, &EditTextWidget::SignedNumberFilter));
	}

	if (index == 1)
	{
		const char *title = table->GetString(StringID('MTHD', kMethodRandomInteger, 'MAXV'));
		return (new TextSetting('MAXV', Text::IntegerToString(maxInteger), title, 11, &EditTextWidget::SignedNumberFilter));
	}

	return (nullptr);
}

void RandomIntegerMethod::SetSetting(const Setting *setting)
{
	Type identifier = setting->GetSettingIdentifier();

	if (identifier == 'MINV')
	{
		const char *text = static_cast<const TextSetting *>(setting)->GetText();
		minInteger = Text::StringToInteger(text);
	}
	else if (identifier == 'MAXV')
	{
		const char *text = static_cast<const TextSetting *>(setting)->GetText();
		maxInteger = Max(Text::StringToInteger(text), minInteger);
	}
}

void RandomIntegerMethod::Execute(const ScriptState *state)
{
	SetOutputValue(state, (int32) Math::Random(maxInteger - minInteger + 1) + minInteger);
	CallCompletionProc();
}


RandomFloatMethod::RandomFloatMethod() : Method(kMethodRandomFloat)
{
	minFloat = 0.0F;
	maxFloat = 1.0F;
}

RandomFloatMethod::RandomFloatMethod(const RandomFloatMethod& randomFloatMethod) : Method(randomFloatMethod)
{
	minFloat = randomFloatMethod.minFloat;
	maxFloat = randomFloatMethod.maxFloat;
}

RandomFloatMethod::~RandomFloatMethod()
{
}

Method *RandomFloatMethod::Replicate(void) const
{
	return (new RandomFloatMethod(*this));
}

void RandomFloatMethod::Pack(Packer& data, unsigned_int32 packFlags) const
{
	Method::Pack(data, packFlags);

	data << minFloat;
	data << maxFloat;
}

void RandomFloatMethod::Unpack(Unpacker& data, unsigned_int32 unpackFlags)
{
	Method::Unpack(data, unpackFlags);

	data >> minFloat;
	data >> maxFloat;
}

int32 RandomFloatMethod::GetSettingCount(void) const
{
	return (2);
}

Setting *RandomFloatMethod::GetSetting(int32 index) const
{
	const StringTable *table = TheInterfaceMgr->GetStringTable();

	if (index == 0)
	{
		const char *title = table->GetString(StringID('MTHD', kMethodRandomFloat, 'MINV'));
		return (new TextSetting('MINV', minFloat, title));
	}

	if (index == 1)
	{
		const char *title = table->GetString(StringID('MTHD', kMethodRandomFloat, 'MAXV'));
		return (new TextSetting('MAXV', maxFloat, title));
	}

	return (nullptr);
}

void RandomFloatMethod::SetSetting(const Setting *setting)
{
	Type identifier = setting->GetSettingIdentifier();

	if (identifier == 'MINV')
	{
		const char *text = static_cast<const TextSetting *>(setting)->GetText();
		minFloat = Text::StringToFloat(text);
	}
	else if (identifier == 'MAXV')
	{
		const char *text = static_cast<const TextSetting *>(setting)->GetText();
		maxFloat = Fmax(Text::StringToFloat(text), minFloat);
	}
}

void RandomFloatMethod::Execute(const ScriptState *state)
{
	SetOutputValue(state, Math::RandomFloat(maxFloat - minFloat) + minFloat);
	CallCompletionProc();
}


GetStringLengthMethod::GetStringLengthMethod() : Method(kMethodGetStringLength)
{
	methodString[0] = 0;
}

GetStringLengthMethod::GetStringLengthMethod(const GetStringLengthMethod& getStringLengthMethod) : Method(getStringLengthMethod)
{
	methodString = getStringLengthMethod.methodString;
}

GetStringLengthMethod::~GetStringLengthMethod()
{
}

Method *GetStringLengthMethod::Replicate(void) const
{
	return (new GetStringLengthMethod(*this));
}

void GetStringLengthMethod::Pack(Packer& data, unsigned_int32 packFlags) const
{
	Method::Pack(data, packFlags);

	data << methodString;
}

void GetStringLengthMethod::Unpack(Unpacker& data, unsigned_int32 unpackFlags)
{
	Method::Unpack(data, unpackFlags);

	data >> methodString;
}

int32 GetStringLengthMethod::GetSettingCount(void) const
{
	return (1);
}

Setting *GetStringLengthMethod::GetSetting(int32 index) const
{
	if (index == 0)
	{
		const StringTable *table = TheInterfaceMgr->GetStringTable();
		const char *title = table->GetString(StringID('MTHD', kMethodGetStringLength, 'ISTR'));
		return (new TextSetting('ISTR', methodString, title, kMaxMethodStringLength));
	}

	return (nullptr);
}

void GetStringLengthMethod::SetSetting(const Setting *setting)
{
	Type identifier = setting->GetSettingIdentifier();

	if (identifier == 'ISTR')
	{
		methodString = static_cast<const TextSetting *>(setting)->GetText();
	}
}

void GetStringLengthMethod::Execute(const ScriptState *state)
{
	SetOutputValue(state, methodString.Length());
	CallCompletionProc();
}


WakeControllerMethod::WakeControllerMethod() : Method(kMethodWakeController)
{
}

WakeControllerMethod::WakeControllerMethod(const WakeControllerMethod& wakeControllerMethod) : Method(wakeControllerMethod)
{
}

WakeControllerMethod::~WakeControllerMethod()
{
}

Method *WakeControllerMethod::Replicate(void) const
{
	return (new WakeControllerMethod(*this));
}

void WakeControllerMethod::Execute(const ScriptState *state)
{
	Node *node = GetTargetNode(state);
	if (node)
	{
		Controller *controller = node->GetController();
		if (controller)
		{
			TheMessageMgr->SendMessageJournal(new WakeSleepMessage(Controller::kControllerMessageWake, controller->GetControllerIndex()));
		}
		else if (node->GetNodeType() == kNodeGeneric)
		{
			node = node->GetFirstSubnode();
			while (node)
			{
				controller = node->GetController();
				if (controller)
				{
					TheMessageMgr->SendMessageJournal(new WakeSleepMessage(Controller::kControllerMessageWake, controller->GetControllerIndex()));
				}

				node = node->Next();
			}
		}
	}

	CallCompletionProc();
}


SleepControllerMethod::SleepControllerMethod() : Method(kMethodSleepController)
{
}

SleepControllerMethod::SleepControllerMethod(const SleepControllerMethod& sleepControllerMethod) : Method(sleepControllerMethod)
{
}

SleepControllerMethod::~SleepControllerMethod()
{
}

Method *SleepControllerMethod::Replicate(void) const
{
	return (new SleepControllerMethod(*this));
}

void SleepControllerMethod::Execute(const ScriptState *state)
{
	Node *node = GetTargetNode(state);
	if (node)
	{
		Controller *controller = node->GetController();
		if (controller)
		{
			TheMessageMgr->SendMessageJournal(new WakeSleepMessage(Controller::kControllerMessageSleep, controller->GetControllerIndex()));
		}
		else if (node->GetNodeType() == kNodeGeneric)
		{
			node = node->GetFirstSubnode();
			while (node)
			{
				controller = node->GetController();
				if (controller)
				{
					TheMessageMgr->SendMessageJournal(new WakeSleepMessage(Controller::kControllerMessageSleep, controller->GetControllerIndex()));
				}

				node = node->Next();
			}
		}
	}

	CallCompletionProc();
}


GetWakeStateMethod::GetWakeStateMethod() : Method(kMethodGetWakeState)
{
}

GetWakeStateMethod::GetWakeStateMethod(const GetWakeStateMethod& getWakeStateMethod) : Method(getWakeStateMethod)
{
}

GetWakeStateMethod::~GetWakeStateMethod()
{
}

Method *GetWakeStateMethod::Replicate(void) const
{
	return (new GetWakeStateMethod(*this));
}

void GetWakeStateMethod::Execute(const ScriptState *state)
{
	const Node *node = GetTargetNode(state);
	if (node)
	{
		const Controller *controller = node->GetController();
		SetOutputValue(state, ((controller) && (!controller->Asleep())));
	}
	else
	{
		SetOutputValue(state, false);
	}

	CallCompletionProc();
}


EnableNodeMethod::EnableNodeMethod() : Method(kMethodEnableNode)
{
}

EnableNodeMethod::EnableNodeMethod(const EnableNodeMethod& enableNodeMethod) : Method(enableNodeMethod)
{
}

EnableNodeMethod::~EnableNodeMethod()
{
}

Method *EnableNodeMethod::Replicate(void) const
{
	return (new EnableNodeMethod(*this));
}

void EnableNodeMethod::Execute(const ScriptState *state)
{
	Node *node = GetTargetNode(state);
	if (node)
	{
		Controller *controller = node->GetController();
		if (controller)
		{
			TheMessageMgr->SendMessageJournal(new NodeEnableDisableMessage(Controller::kControllerMessageEnableNode, controller->GetControllerIndex()));
		}
	}

	CallCompletionProc();
}


DisableNodeMethod::DisableNodeMethod() : Method(kMethodDisableNode)
{
}

DisableNodeMethod::DisableNodeMethod(const DisableNodeMethod& disableNodeMethod) : Method(disableNodeMethod)
{
}

DisableNodeMethod::~DisableNodeMethod()
{
}

Method *DisableNodeMethod::Replicate(void) const
{
	return (new DisableNodeMethod(*this));
}

void DisableNodeMethod::Execute(const ScriptState *state)
{
	Node *node = GetTargetNode(state);
	if (node)
	{
		Controller *controller = node->GetController();
		if (controller)
		{
			TheMessageMgr->SendMessageJournal(new NodeEnableDisableMessage(Controller::kControllerMessageDisableNode, controller->GetControllerIndex()));
		}
	}

	CallCompletionProc();
}


GetEnableStateMethod::GetEnableStateMethod() : Method(kMethodGetEnableState)
{
}

GetEnableStateMethod::GetEnableStateMethod(const GetEnableStateMethod& getEnableStateMethod) : Method(getEnableStateMethod)
{
}

GetEnableStateMethod::~GetEnableStateMethod()
{
}

Method *GetEnableStateMethod::Replicate(void) const
{
	return (new GetEnableStateMethod(*this));
}

void GetEnableStateMethod::Execute(const ScriptState *state)
{
	const Node *node = GetTargetNode(state);
	SetOutputValue(state, ((node) && (node->Enabled())));
	CallCompletionProc();
}


DeleteNodeMethod::DeleteNodeMethod() : Method(kMethodDeleteNode)
{
}

DeleteNodeMethod::DeleteNodeMethod(const DeleteNodeMethod& deleteNodeMethod) : Method(deleteNodeMethod)
{
}

DeleteNodeMethod::~DeleteNodeMethod()
{
}

Method *DeleteNodeMethod::Replicate(void) const
{
	return (new DeleteNodeMethod(*this));
}

void DeleteNodeMethod::Execute(const ScriptState *state)
{
	Node *methodTarget = GetTargetNode(state);
	if (methodTarget)
	{
		Node *controllerTarget = state->GetScriptControllerTarget();

		if ((methodTarget != controllerTarget) && (!methodTarget->Successor(controllerTarget)))
		{
			Controller *controller = methodTarget->GetController();
			if (controller)
			{
				TheMessageMgr->SendMessageJournal(new DeleteNodeMessage(controller->GetControllerIndex()));
			}
		}
	}

	CallCompletionProc();
}


EnableInteractivityMethod::EnableInteractivityMethod() : Method(kMethodEnableInteractivity)
{
}

EnableInteractivityMethod::EnableInteractivityMethod(const EnableInteractivityMethod& enableInteractivityMethod) : Method(enableInteractivityMethod)
{
}

EnableInteractivityMethod::~EnableInteractivityMethod()
{
}

Method *EnableInteractivityMethod::Replicate(void) const
{
	return (new EnableInteractivityMethod(*this));
}

void EnableInteractivityMethod::Execute(const ScriptState *state)
{
	Node *node = GetTargetNode(state);
	if (node)
	{
		Controller *controller = node->GetController();
		if (controller)
		{
			TheMessageMgr->SendMessageJournal(new NodeInteractivityMessage(Controller::kControllerMessageEnableInteractivity, controller->GetControllerIndex()));
		}
	}

	CallCompletionProc();
}


DisableInteractivityMethod::DisableInteractivityMethod() : Method(kMethodDisableInteractivity)
{
}

DisableInteractivityMethod::DisableInteractivityMethod(const DisableInteractivityMethod& disableInteractivityMethod) : Method(disableInteractivityMethod)
{
}

DisableInteractivityMethod::~DisableInteractivityMethod()
{
}

Method *DisableInteractivityMethod::Replicate(void) const
{
	return (new DisableInteractivityMethod(*this));
}

void DisableInteractivityMethod::Execute(const ScriptState *state)
{
	Node *node = GetTargetNode(state);
	if (node)
	{
		Controller *controller = node->GetController();
		if (controller)
		{
			TheMessageMgr->SendMessageJournal(new NodeInteractivityMessage(Controller::kControllerMessageDisableInteractivity, controller->GetControllerIndex()));
		}
	}

	CallCompletionProc();
}


GetInteractivityStateMethod::GetInteractivityStateMethod() : Method(kMethodGetInteractivityState)
{
}

GetInteractivityStateMethod::GetInteractivityStateMethod(const GetInteractivityStateMethod& getInteractivityStateMethod) : Method(getInteractivityStateMethod)
{
}

GetInteractivityStateMethod::~GetInteractivityStateMethod()
{
}

Method *GetInteractivityStateMethod::Replicate(void) const
{
	return (new GetInteractivityStateMethod(*this));
}

void GetInteractivityStateMethod::Execute(const ScriptState *state)
{
	const Node *node = GetTargetNode(state);
	if (node)
	{
		const Property *property = node->GetProperty(kPropertyInteraction);
		SetOutputValue(state, ((property) && ((property->GetPropertyFlags() & kPropertyDisabled) == 0)));
	}
	else
	{
		SetOutputValue(state, false);
	}

	CallCompletionProc();
}


ShowGeometryMethod::ShowGeometryMethod() : Method(kMethodShowGeometry)
{
}

ShowGeometryMethod::ShowGeometryMethod(const ShowGeometryMethod& showGeometryMethod) : Method(showGeometryMethod)
{
}

ShowGeometryMethod::~ShowGeometryMethod()
{
}

Method *ShowGeometryMethod::Replicate(void) const
{
	return (new ShowGeometryMethod(*this));
}

void ShowGeometryMethod::Execute(const ScriptState *state)
{
	Node *node = GetTargetNode(state);
	if (node)
	{
		Controller *controller = node->GetController();
		if (controller)
		{
			TheMessageMgr->SendMessageJournal(new GeometryVisibilityMessage(Controller::kControllerMessageShowGeometry, controller->GetControllerIndex()));
		}
	}

	CallCompletionProc();
}


HideGeometryMethod::HideGeometryMethod() : Method(kMethodHideGeometry)
{
}

HideGeometryMethod::HideGeometryMethod(const HideGeometryMethod& hideGeometryMethod) : Method(hideGeometryMethod)
{
}

HideGeometryMethod::~HideGeometryMethod()
{
}

Method *HideGeometryMethod::Replicate(void) const
{
	return (new HideGeometryMethod(*this));
}

void HideGeometryMethod::Execute(const ScriptState *state)
{
	Node *node = GetTargetNode(state);
	if (node)
	{
		Controller *controller = node->GetController();
		if (controller)
		{
			TheMessageMgr->SendMessageJournal(new GeometryVisibilityMessage(Controller::kControllerMessageHideGeometry, controller->GetControllerIndex()));
		}
	}

	CallCompletionProc();
}


GetVisibilityStateMethod::GetVisibilityStateMethod() : Method(kMethodGetVisibilityState)
{
}

GetVisibilityStateMethod::GetVisibilityStateMethod(const GetVisibilityStateMethod& getVisibilityStateMethod) : Method(getVisibilityStateMethod)
{
}

GetVisibilityStateMethod::~GetVisibilityStateMethod()
{
}

Method *GetVisibilityStateMethod::Replicate(void) const
{
	return (new GetVisibilityStateMethod(*this));
}

void GetVisibilityStateMethod::Execute(const ScriptState *state)
{
	const Node *node = GetTargetNode(state);
	if ((node) && (node->GetNodeType() == kNodeGeometry))
	{
		const GeometryObject *object = static_cast<const Geometry *>(node)->GetObject();
		SetOutputValue(state, (object->GetGeometryFlags() & kGeometryInvisible) != 0);
	}
	else
	{
		SetOutputValue(state, false);
	}

	CallCompletionProc();
}


ActivateTriggerMethod::ActivateTriggerMethod() : Method(kMethodActivateTrigger)
{
}

ActivateTriggerMethod::ActivateTriggerMethod(const ActivateTriggerMethod& activateTriggerMethod) : Method(activateTriggerMethod)
{
}

ActivateTriggerMethod::~ActivateTriggerMethod()
{
}

Method *ActivateTriggerMethod::Replicate(void) const
{
	return (new ActivateTriggerMethod(*this));
}

void ActivateTriggerMethod::Execute(const ScriptState *state)
{
	Node *node = GetTargetNode(state);
	if ((node) && (node->Enabled()) && (node->GetNodeType() == kNodeTrigger))
	{
		Trigger *trigger = static_cast<Trigger *>(node);

		unsigned_int32 triggerFlags = trigger->GetObject()->GetTriggerFlags();
		if (triggerFlags & kTriggerActivateDisable)
		{
			trigger->SetNodeFlags(trigger->GetNodeFlags() | kNodeDisabled);
		}

		trigger->Activate();
	}

	CallCompletionProc();
}


DeactivateTriggerMethod::DeactivateTriggerMethod() : Method(kMethodDeactivateTrigger)
{
}

DeactivateTriggerMethod::DeactivateTriggerMethod(const DeactivateTriggerMethod& deactivateTriggerMethod) : Method(deactivateTriggerMethod)
{
}

DeactivateTriggerMethod::~DeactivateTriggerMethod()
{
}

Method *DeactivateTriggerMethod::Replicate(void) const
{
	return (new DeactivateTriggerMethod(*this));
}

void DeactivateTriggerMethod::Execute(const ScriptState *state)
{
	Node *node = GetTargetNode(state);
	if ((node) && (node->Enabled()) && (node->GetNodeType() == kNodeTrigger))
	{
		static_cast<Trigger *>(node)->Deactivate();
	}

	CallCompletionProc();
}


BreakJointMethod::BreakJointMethod() : Method(kMethodBreakJoint)
{
}

BreakJointMethod::BreakJointMethod(const BreakJointMethod& breakJointMethod) : Method(breakJointMethod)
{
}

BreakJointMethod::~BreakJointMethod()
{
}

Method *BreakJointMethod::Replicate(void) const
{
	return (new BreakJointMethod(*this));
}

void BreakJointMethod::Execute(const ScriptState *state)
{
	Node *node = GetTargetNode(state);
	if (node)
	{
		Controller *controller = node->GetController();
		if (controller)
		{
			TheMessageMgr->SendMessageJournal(new BreakJointMessage(controller->GetControllerIndex()));
		}
	}

	CallCompletionProc();
}


RemoveModifiersMethod::RemoveModifiersMethod() : Method(kMethodRemoveModifiers)
{
	modifierType = 0;
}

RemoveModifiersMethod::RemoveModifiersMethod(const RemoveModifiersMethod& removeModifiersMethod) : Method(removeModifiersMethod)
{
	modifierType = removeModifiersMethod.modifierType;
}

RemoveModifiersMethod::~RemoveModifiersMethod()
{
}

Method *RemoveModifiersMethod::Replicate(void) const
{
	return (new RemoveModifiersMethod(*this));
}

void RemoveModifiersMethod::Pack(Packer& data, unsigned_int32 packFlags) const
{
	Method::Pack(data, packFlags);

	data << modifierType;
}

void RemoveModifiersMethod::Unpack(Unpacker& data, unsigned_int32 unpackFlags)
{
	Method::Unpack(data, unpackFlags);

	data >> modifierType;
}

int32 RemoveModifiersMethod::GetSettingCount(void) const
{
	return (1);
}

Setting *RemoveModifiersMethod::GetSetting(int32 index) const
{
	int32 count = 1;
	int32 selection = 0;

	const ModifierRegistration *registration = Modifier::GetFirstRegistration();
	while (registration)
	{
		if (registration->GetModifierType() == modifierType)
		{
			selection = count;
		}

		count++;
		registration = registration->Next();
	}

	const StringTable *table = TheInterfaceMgr->GetStringTable();
	const char *title = table->GetString(StringID('MTHD', kMethodRemoveModifiers, 'TYPE'));
	MenuSetting *menu = new MenuSetting('TYPE', selection, title, count);

	menu->SetMenuItemString(0, table->GetString(StringID('MTHD', kMethodRemoveModifiers, 'TYPE', 'ALL ')));

	registration = Modifier::GetFirstRegistration();
	for (machine a = 1; a < count; a++)
	{
		menu->SetMenuItemString(a, registration->GetModifierName());
		registration = registration->Next();
	}

	return (menu);
}

void RemoveModifiersMethod::SetSetting(const Setting *setting)
{
	if (setting->GetSettingIdentifier() == 'TYPE')
	{
		int32 selection = static_cast<const MenuSetting *>(setting)->GetMenuSelection();
		if (selection == 0)
		{
			modifierType = 0;
		}
		else
		{
			int32 count = 1;

			const ModifierRegistration *registration = Modifier::GetFirstRegistration();
			while (registration)
			{
				if (count == selection)
				{
					modifierType = registration->GetModifierType();
					break;
				}

				count++;
				registration = registration->Next();
			}
		}
	}
}

void RemoveModifiersMethod::Execute(const ScriptState *state)
{
	const Node *node = GetTargetNode(state);
	if ((node) && (node->GetNodeType() == kNodeInstance))
	{
		ModifierType type = modifierType;

		const Modifier *modifier = static_cast<const Instance *>(node)->GetFirstModifier();
		while (modifier)
		{
			const Modifier *next = modifier->Next();

			if ((type == 0) || (modifier->GetModifierType() == type))
			{
				delete modifier;
			}

			modifier = next;
		}
	}

	CallCompletionProc();
}


GetNodePositionMethod::GetNodePositionMethod() : Method(kMethodGetNodePosition)
{
}

GetNodePositionMethod::GetNodePositionMethod(const GetNodePositionMethod& getNodePositionMethod) : Method(getNodePositionMethod)
{
}

GetNodePositionMethod::~GetNodePositionMethod()
{
}

Method *GetNodePositionMethod::Replicate(void) const
{
	return (new GetNodePositionMethod(*this));
}

void GetNodePositionMethod::Execute(const ScriptState *state)
{
	const Node *node = GetTargetNode(state);
	if (node)
	{
		SetOutputValue(state, node->GetNodePosition());
		SetMethodResult(true);
	}
	else
	{
		SetOutputValue(state, Zero3D);
	}

	CallCompletionProc();
}


GetWorldPositionMethod::GetWorldPositionMethod() : Method(kMethodGetWorldPosition)
{
}

GetWorldPositionMethod::GetWorldPositionMethod(const GetWorldPositionMethod& getWorldPositionMethod) : Method(getWorldPositionMethod)
{
}

GetWorldPositionMethod::~GetWorldPositionMethod()
{
}

Method *GetWorldPositionMethod::Replicate(void) const
{
	return (new GetWorldPositionMethod(*this));
}

void GetWorldPositionMethod::Execute(const ScriptState *state)
{
	const Node *node = GetTargetNode(state);
	if (node)
	{
		SetOutputValue(state, node->GetWorldPosition());
		SetMethodResult(true);
	}
	else
	{
		SetOutputValue(state, Zero3D);
	}

	CallCompletionProc();
}


PlaySoundMethod::PlaySoundMethod() : Method(kMethodPlaySound)
{
	soundName[0] = 0;
	streamFlag = false;

	loopCount = 1;

	soundVolume = 1.0F;
	fadeTime = 0;

	methodSound = nullptr;
}

PlaySoundMethod::PlaySoundMethod(const char *name, bool stream) : Method(kMethodPlaySound)
{
	soundName = name;
	streamFlag = stream;

	loopCount = 1;

	soundVolume = 1.0F;
	fadeTime = 0;

	methodSound = nullptr;
}

PlaySoundMethod::PlaySoundMethod(const PlaySoundMethod& playSoundMethod) : Method(playSoundMethod)
{
	soundName = playSoundMethod.soundName;
	streamFlag = playSoundMethod.streamFlag;

	loopCount = playSoundMethod.loopCount;

	soundVolume = playSoundMethod.soundVolume;
	fadeTime = playSoundMethod.fadeTime;

	methodSound = nullptr;
}

PlaySoundMethod::~PlaySoundMethod()
{
	if (methodSound)
	{
		methodSound->Release();
	}
}

Method *PlaySoundMethod::Replicate(void) const
{
	return (new PlaySoundMethod(*this));
}

void PlaySoundMethod::Pack(Packer& data, unsigned_int32 packFlags) const
{
	Method::Pack(data, packFlags);

	PackHandle handle = data.BeginChunk('NAME');
	data << soundName;
	data.EndChunk(handle);

	data << ChunkHeader('FLAG', 4);
	data << streamFlag;

	data << ChunkHeader('LOOP', 4);
	data << loopCount;

	data << ChunkHeader('VOLU', 8);
	data << soundVolume;
	data << fadeTime;

	data << TerminatorChunk;
}

void PlaySoundMethod::Unpack(Unpacker& data, unsigned_int32 unpackFlags)
{
	Method::Unpack(data, unpackFlags);
	UnpackChunkList<PlaySoundMethod>(data, unpackFlags);
}

bool PlaySoundMethod::UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags)
{
	switch (chunkHeader->chunkType)
	{
		case 'NAME':

			data >> soundName;
			return (true);

		case 'FLAG':

			data >> streamFlag;
			return (true);

		case 'LOOP':

			data >> loopCount;
			return (true);

		case 'VOLU':

			data >> soundVolume;
			data >> fadeTime;
			return (true);
	}

	return (false);
}

int32 PlaySoundMethod::GetSettingCount(void) const
{
	return (5);
}

Setting *PlaySoundMethod::GetSetting(int32 index) const
{
	const StringTable *table = TheInterfaceMgr->GetStringTable();

	if (index == 0)
	{
		const char *title = table->GetString(StringID('MTHD', kMethodPlaySound, 'NAME'));
		const char *picker = table->GetString(StringID('MTHD', kMethodPlaySound));
		return (new ResourceSetting('NAME', soundName, title, picker, SoundResource::GetDescriptor()));
	}

	if (index == 1)
	{
		const char *title = table->GetString(StringID('MTHD', kMethodPlaySound, 'STRM'));
		return (new BooleanSetting('STRM', streamFlag, title));
	}

	if (index == 2)
	{
		const char *title = table->GetString(StringID('MTHD', kMethodPlaySound, 'LOOP'));
		return (new TextSetting('LOOP', Text::IntegerToString(loopCount), title, 2, &EditTextWidget::NumberFilter));
	}

	if (index == 3)
	{
		const char *title = table->GetString(StringID('MTHD', kMethodPlaySound, 'VOLU'));
		return (new IntegerSetting('VOLU', (int32) (soundVolume * 100.0 + 0.5F), title, 1, 100, 1));
	}

	if (index == 4)
	{
		const char *title = table->GetString(StringID('MTHD', kMethodPlaySound, 'FADE'));
		return (new TextSetting('FADE', (float) fadeTime * 0.001F, title));
	}

	return (nullptr);
}

void PlaySoundMethod::SetSetting(const Setting *setting)
{
	Type identifier = setting->GetSettingIdentifier();

	if (identifier == 'NAME')
	{
		soundName = static_cast<const ResourceSetting *>(setting)->GetResourceName();
	}
	else if (identifier == 'STRM')
	{
		streamFlag = static_cast<const BooleanSetting *>(setting)->GetBooleanValue();
	}
	else if (identifier == 'LOOP')
	{
		const char *text = static_cast<const TextSetting *>(setting)->GetText();
		loopCount = Max(Text::StringToInteger(text), 1);
	}
	else if (identifier == 'VOLU')
	{
		int32 value = static_cast<const IntegerSetting *>(setting)->GetIntegerValue();
		soundVolume = (float) value * 0.01F;
	}
	else if (identifier == 'FADE')
	{
		const char *text = static_cast<const TextSetting *>(setting)->GetText();
		fadeTime = MaxZero((int32) (Text::StringToFloat(text) * 1000.0F));
	}
}

void PlaySoundMethod::SoundComplete(Sound *sound, void *cookie)
{
	PlaySoundMethod *method = static_cast<PlaySoundMethod *>(cookie);

	method->methodSound->Release();
	method->methodSound = nullptr;

	method->CallCompletionProc();
}

void PlaySoundMethod::Execute(const ScriptState *state)
{
	methodSound = new Sound;
	if (streamFlag)
	{
		WaveStreamer *streamer = new WaveStreamer;
		methodSound->Stream(streamer);

		streamer->AddComponent(soundName);
	}
	else
	{
		methodSound->Load(soundName);
	}

	methodSound->SetSoundFlags(kSoundPersistent);
	methodSound->SetLoopCount(loopCount);
	methodSound->SetCompletionProc(&SoundComplete, this);

	if (fadeTime == 0)
	{
		methodSound->SetSoundProperty(kSoundVolume, soundVolume);
		methodSound->Play();
	}
	else
	{
		methodSound->SetSoundProperty(kSoundVolume, 0.0F);
		methodSound->Play();
		methodSound->VaryVolume(soundVolume, fadeTime);
	}
}


PlaySourceMethod::PlaySourceMethod() : Method(kMethodPlaySource)
{
}

PlaySourceMethod::PlaySourceMethod(const PlaySourceMethod& playSourceMethod) : Method(playSourceMethod)
{
}

PlaySourceMethod::~PlaySourceMethod()
{
	Node *node = sourceLink;
	if (node)
	{
		static_cast<Source *>(node)->SetCompletionProc(nullptr);
	}
}

Method *PlaySourceMethod::Replicate(void) const
{
	return (new PlaySourceMethod(*this));
}

void PlaySourceMethod::SourceComplete(Source *source, void *cookie)
{
	source->SetCompletionProc(nullptr);
	PlaySourceMethod *playSourceMethod = static_cast<PlaySourceMethod *>(cookie);
	playSourceMethod->sourceLink = nullptr;
	playSourceMethod->CallCompletionProc();
}

void PlaySourceMethod::Execute(const ScriptState *state)
{
	Node *node = GetTargetNode(state);
	if ((node) && (node->GetNodeType() == kNodeSource))
	{
		Source *source = static_cast<Source *>(node);

		if ((source->Enabled()) && (!source->Playing()))
		{
			Controller *controller = source->GetController();
			if (controller)
			{
				if (source->GetObject()->GetSourceFlags() & kSourceLoop)
				{
					TheMessageMgr->SendMessageJournal(new SourcePlayStopMessage(Controller::kControllerMessagePlaySource, controller->GetControllerIndex()));
				}
				else
				{
					sourceLink = source;
					source->SetCompletionProc(&SourceComplete, this);

					TheMessageMgr->SendMessageAll(SourcePlayStopMessage(Controller::kControllerMessagePlaySource, controller->GetControllerIndex()));
					return;
				}
			}
		}
	}

	CallCompletionProc();
}

void PlaySourceMethod::Resume(const ScriptState *state)
{
	Node *node = GetTargetNode(state);
	if (node)
	{
		Source *source = static_cast<Source *>(node);

		sourceLink = source;
		source->SetCompletionProc(&SourceComplete, this);
		return;
	}

	CallCompletionProc();
}


StopSourceMethod::StopSourceMethod() : Method(kMethodStopSource)
{
}

StopSourceMethod::StopSourceMethod(const StopSourceMethod& stopSourceMethod) : Method(stopSourceMethod)
{
}

StopSourceMethod::~StopSourceMethod()
{
}

Method *StopSourceMethod::Replicate(void) const
{
	return (new StopSourceMethod(*this));
}

void StopSourceMethod::Execute(const ScriptState *state)
{
	Node *node = GetTargetNode(state);
	if ((node) && (node->GetNodeType() == kNodeSource))
	{
		Source *source = static_cast<Source *>(node);
		Controller *controller = source->GetController();
		if (controller)
		{
			TheMessageMgr->SendMessageJournal(new SourcePlayStopMessage(Controller::kControllerMessageStopSource, controller->GetControllerIndex()));
			source->CallCompletionProc();
		}
	}

	CallCompletionProc();
}


VarySourceVolumeMethod::VarySourceVolumeMethod() : Method(kMethodVarySourceVolume)
{
	volumeTarget = 0.0F;
	variationTime = 1000;
	stopFlag = true;
}

VarySourceVolumeMethod::VarySourceVolumeMethod(float volume, int32 time, bool stop) : Method(kMethodVarySourceVolume)
{
	volumeTarget = volume;
	variationTime = time;
	stopFlag = stop;
}

VarySourceVolumeMethod::VarySourceVolumeMethod(const VarySourceVolumeMethod& varySourceVolumeMethod) : Method(varySourceVolumeMethod)
{
	volumeTarget = varySourceVolumeMethod.volumeTarget;
	variationTime = varySourceVolumeMethod.variationTime;
	stopFlag = varySourceVolumeMethod.stopFlag;
}

VarySourceVolumeMethod::~VarySourceVolumeMethod()
{
}

Method *VarySourceVolumeMethod::Replicate(void) const
{
	return (new VarySourceVolumeMethod(*this));
}

void VarySourceVolumeMethod::Pack(Packer& data, unsigned_int32 packFlags) const
{
	Method::Pack(data, packFlags);

	data << ChunkHeader('VOLU', 4);
	data << volumeTarget;

	data << ChunkHeader('TIME', 4);
	data << variationTime;

	data << ChunkHeader('STOP', 4);
	data << stopFlag;

	data << TerminatorChunk;
}

void VarySourceVolumeMethod::Unpack(Unpacker& data, unsigned_int32 unpackFlags)
{
	Method::Unpack(data, unpackFlags);
	UnpackChunkList<VarySourceVolumeMethod>(data, unpackFlags);
}

bool VarySourceVolumeMethod::UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags)
{
	switch (chunkHeader->chunkType)
	{
		case 'VOLU':

			data >> volumeTarget;
			return (true);

		case 'TIME':

			data >> variationTime;
			return (true);

		case 'STOP':

			data >> stopFlag;
			return (true);
	}

	return (false);
}

int32 VarySourceVolumeMethod::GetSettingCount(void) const
{
	return (3);
}

Setting *VarySourceVolumeMethod::GetSetting(int32 index) const
{
	const StringTable *table = TheInterfaceMgr->GetStringTable();

	if (index == 0)
	{
		const char *title = table->GetString(StringID('MTHD', kMethodVarySourceVolume, 'VOLU'));
		return (new IntegerSetting('VOLU', (int32) (volumeTarget * 100.0 + 0.5F), title, 0, 100, 1));
	}

	if (index == 1)
	{
		const char *title = table->GetString(StringID('MTHD', kMethodVarySourceVolume, 'TIME'));
		return (new TextSetting('TIME', (float) variationTime * 0.001F, title));
	}

	if (index == 2)
	{
		const char *title = table->GetString(StringID('MTHD', kMethodVarySourceVolume, 'STOP'));
		return (new BooleanSetting('STOP', stopFlag, title));
	}

	return (nullptr);
}

void VarySourceVolumeMethod::SetSetting(const Setting *setting)
{
	Type identifier = setting->GetSettingIdentifier();

	if (identifier == 'VOLU')
	{
		int32 value = static_cast<const IntegerSetting *>(setting)->GetIntegerValue();
		volumeTarget = (float) value * 0.01F;
	}
	else if (identifier == 'TIME')
	{
		const char *text = static_cast<const TextSetting *>(setting)->GetText();
		variationTime = MaxZero((int32) (Text::StringToFloat(text) * 1000.0F + 0.5F));
	}
	else if (identifier == 'STOP')
	{
		stopFlag = static_cast<const BooleanSetting *>(setting)->GetBooleanValue();
	}
}

void VarySourceVolumeMethod::Execute(const ScriptState *state)
{
	Node *node = GetTargetNode(state);
	if ((node) && (node->GetNodeType() == kNodeSource))
	{
		static_cast<Source *>(node)->VaryVolume(volumeTarget, variationTime, stopFlag);
	}

	CallCompletionProc();
}


VarySourceFrequencyMethod::VarySourceFrequencyMethod() : Method(kMethodVarySourceFrequency)
{
	frequencyTarget = 0.0F;
	variationTime = 1000;
	stopFlag = true;
}

VarySourceFrequencyMethod::VarySourceFrequencyMethod(float frequency, int32 time, bool stop) : Method(kMethodVarySourceFrequency)
{
	frequencyTarget = frequency;
	variationTime = time;
	stopFlag = stop;
}

VarySourceFrequencyMethod::VarySourceFrequencyMethod(const VarySourceFrequencyMethod& varySourceFrequencyMethod) : Method(varySourceFrequencyMethod)
{
	frequencyTarget = varySourceFrequencyMethod.frequencyTarget;
	variationTime = varySourceFrequencyMethod.variationTime;
	stopFlag = varySourceFrequencyMethod.stopFlag;
}

VarySourceFrequencyMethod::~VarySourceFrequencyMethod()
{
}

Method *VarySourceFrequencyMethod::Replicate(void) const
{
	return (new VarySourceFrequencyMethod(*this));
}

void VarySourceFrequencyMethod::Pack(Packer& data, unsigned_int32 packFlags) const
{
	Method::Pack(data, packFlags);

	data << ChunkHeader('FREQ', 4);
	data << frequencyTarget;

	data << ChunkHeader('TIME', 4);
	data << variationTime;

	data << ChunkHeader('STOP', 4);
	data << stopFlag;

	data << TerminatorChunk;
}

void VarySourceFrequencyMethod::Unpack(Unpacker& data, unsigned_int32 unpackFlags)
{
	Method::Unpack(data, unpackFlags);
	UnpackChunkList<VarySourceFrequencyMethod>(data, unpackFlags);
}

bool VarySourceFrequencyMethod::UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags)
{
	switch (chunkHeader->chunkType)
	{
		case 'FREQ':

			data >> frequencyTarget;
			return (true);

		case 'TIME':

			data >> variationTime;
			return (true);

		case 'STOP':

			data >> stopFlag;
			return (true);
	}

	return (false);
}

int32 VarySourceFrequencyMethod::GetSettingCount(void) const
{
	return (3);
}

Setting *VarySourceFrequencyMethod::GetSetting(int32 index) const
{
	const StringTable *table = TheInterfaceMgr->GetStringTable();

	if (index == 0)
	{
		const char *title = table->GetString(StringID('MTHD', kMethodVarySourceFrequency, 'FREQ'));
		return (new FloatSetting('FREQ', frequencyTarget, title, 0.25F, 4.0F, 0.05F));
	}

	if (index == 1)
	{
		const char *title = table->GetString(StringID('MTHD', kMethodVarySourceFrequency, 'TIME'));
		return (new TextSetting('TIME', (float) variationTime * 0.001F, title));
	}

	if (index == 2)
	{
		const char *title = table->GetString(StringID('MTHD', kMethodVarySourceFrequency, 'STOP'));
		return (new BooleanSetting('STOP', stopFlag, title));
	}

	return (nullptr);
}

void VarySourceFrequencyMethod::SetSetting(const Setting *setting)
{
	Type identifier = setting->GetSettingIdentifier();

	if (identifier == 'FREQ')
	{
		frequencyTarget = static_cast<const FloatSetting *>(setting)->GetFloatValue();
	}
	else if (identifier == 'TIME')
	{
		const char *text = static_cast<const TextSetting *>(setting)->GetText();
		variationTime = MaxZero((int32) (Text::StringToFloat(text) * 1000.0F + 0.5F));
	}
	else if (identifier == 'STOP')
	{
		stopFlag = static_cast<const BooleanSetting *>(setting)->GetBooleanValue();
	}
}

void VarySourceFrequencyMethod::Execute(const ScriptState *state)
{
	Node *node = GetTargetNode(state);
	if ((node) && (node->GetNodeType() == kNodeSource))
	{
		static_cast<Source *>(node)->VaryFrequency(frequencyTarget, variationTime, stopFlag);
	}

	CallCompletionProc();
}


SetMaterialColorMethod::SetMaterialColorMethod() : Method(kMethodSetMaterialColor)
{
	attributeType = kAttributeDiffuse;
	materialColor.Set(1.0F, 1.0F, 1.0F, 1.0F);
}

SetMaterialColorMethod::SetMaterialColorMethod(AttributeType type, const ColorRGBA& color) : Method(kMethodSetMaterialColor)
{
	attributeType = type;
	materialColor = color;
}

SetMaterialColorMethod::SetMaterialColorMethod(const SetMaterialColorMethod& setMaterialColorMethod) : Method(setMaterialColorMethod)
{
	attributeType = setMaterialColorMethod.attributeType;
	materialColor = setMaterialColorMethod.materialColor;
}

SetMaterialColorMethod::~SetMaterialColorMethod()
{
}

Method *SetMaterialColorMethod::Replicate(void) const
{
	return (new SetMaterialColorMethod(*this));
}

void SetMaterialColorMethod::Pack(Packer& data, unsigned_int32 packFlags) const
{
	Method::Pack(data, packFlags);

	data << ChunkHeader('ATTR', 4);
	data << attributeType;

	data << ChunkHeader('COLR', sizeof(ColorRGBA));
	data << materialColor;

	data << TerminatorChunk;
}

void SetMaterialColorMethod::Unpack(Unpacker& data, unsigned_int32 unpackFlags)
{
	Method::Unpack(data, unpackFlags);
	UnpackChunkList<SetMaterialColorMethod>(data, unpackFlags);
}

bool SetMaterialColorMethod::UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags)
{
	switch (chunkHeader->chunkType)
	{
		case 'ATTR':

			data >> attributeType;
			return (true);

		case 'COLR':

			data >> materialColor;
			return (true);
	}

	return (false);
}

int32 SetMaterialColorMethod::GetSettingCount(void) const
{
	return (2);
}

Setting *SetMaterialColorMethod::GetSetting(int32 index) const
{
	const StringTable *table = TheInterfaceMgr->GetStringTable();

	if (index == 0)
	{
		int32 selection = 0;
		for (machine a = 1; a < kMaterialAttributeCount; a++)
		{
			if (attributeType == materialAttributeType[a])
			{
				selection = a;
				break;
			}
		}

		const char *title = table->GetString(StringID('MTHD', kMethodSetMaterialColor, 'ATTR'));
		MenuSetting *menu = new MenuSetting('ATTR', selection, title, kMaterialAttributeCount);
		for (machine a = 0; a < kMaterialAttributeCount; a++)
		{
			menu->SetMenuItemString(a, table->GetString(StringID('MTHD', kMethodSetMaterialColor, 'ATTR', materialAttributeType[a])));
		}

		return (menu);
	}

	if (index == 1)
	{
		const char *title = table->GetString(StringID('MTHD', kMethodSetMaterialColor, 'COLR'));
		const char *picker = table->GetString(StringID('MTHD', kMethodSetMaterialColor));
		return (new ColorSetting('COLR', materialColor, title, picker, kColorPickerAlpha));
	}

	return (nullptr);
}

void SetMaterialColorMethod::SetSetting(const Setting *setting)
{
	Type identifier = setting->GetSettingIdentifier();

	if (identifier == 'ATTR')
	{
		attributeType = materialAttributeType[static_cast<const MenuSetting *>(setting)->GetMenuSelection()];
	}
	else if (identifier == 'COLR')
	{
		materialColor = static_cast<const ColorSetting *>(setting)->GetColor();
	}
}

void SetMaterialColorMethod::Execute(const ScriptState *state)
{
	Node *node = GetTargetNode(state);
	if (node)
	{
		Controller *controller = node->GetController();
		if (controller)
		{
			TheMessageMgr->SendMessageJournal(new MaterialColorMessage(controller->GetControllerIndex(), attributeType, materialColor));
		}
	}

	CallCompletionProc();
}


SetShaderParameterMethod::SetShaderParameterMethod(MethodType type) : Method(type)
{
	parameterSlot = 0;
	parameterValue.Set(0.0F, 0.0F, 0.0F, 0.0F);
}

SetShaderParameterMethod::SetShaderParameterMethod(MethodType type, int32 slot, const Vector4D& param) : Method(type)
{
	parameterSlot = slot;
	parameterValue = param;
}

SetShaderParameterMethod::SetShaderParameterMethod(const SetShaderParameterMethod& setShaderParameterMethod) : Method(setShaderParameterMethod)
{
	parameterSlot = setShaderParameterMethod.parameterSlot;
	parameterValue = setShaderParameterMethod.parameterValue;
}

SetShaderParameterMethod::~SetShaderParameterMethod()
{
}

void SetShaderParameterMethod::Pack(Packer& data, unsigned_int32 packFlags) const
{
	Method::Pack(data, packFlags);

	data << ChunkHeader('SLOT', 4);
	data << parameterSlot;

	data << ChunkHeader('PARM', sizeof(Vector4D));
	data << parameterValue;

	data << TerminatorChunk;
}

void SetShaderParameterMethod::Unpack(Unpacker& data, unsigned_int32 unpackFlags)
{
	Method::Unpack(data, unpackFlags);
	UnpackChunkList<SetShaderParameterMethod>(data, unpackFlags);
}

bool SetShaderParameterMethod::UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags)
{
	switch (chunkHeader->chunkType)
	{
		case 'SLOT':

			data >> parameterSlot;
			return (true);

		case 'PARM':

			data >> parameterValue;
			return (true);
	}

	return (false);
}

int32 SetShaderParameterMethod::GetSettingCount(void) const
{
	return (1);
}

Setting *SetShaderParameterMethod::GetSetting(int32 index) const
{
	if (index == 0)
	{
		const StringTable *table = TheInterfaceMgr->GetStringTable();

		const char *title = table->GetString(StringID('MTHD', 'SHDR', 'SLOT'));
		MenuSetting *menu = new MenuSetting('SLOT', parameterSlot, title, 8);
		for (machine a = 0; a < 8; a++)
		{
			menu->SetMenuItemString(a, table->GetString(StringID('MTHD', 'SHDR', 'SLOT', 'PRM0' + a)));
		}

		return (menu);
	}

	return (nullptr);
}

void SetShaderParameterMethod::SetSetting(const Setting *setting)
{
	Type identifier = setting->GetSettingIdentifier();

	if (identifier == 'SLOT')
	{
		parameterSlot = static_cast<const MenuSetting *>(setting)->GetMenuSelection();
	}
}

void SetShaderParameterMethod::Execute(const ScriptState *state)
{
	Node *node = GetTargetNode(state);
	if (node)
	{
		Controller *controller = node->GetController();
		if (controller)
		{
			TheMessageMgr->SendMessageJournal(new ShaderParameterMessage(controller->GetControllerIndex(), parameterSlot, parameterValue));
		}
	}

	CallCompletionProc();
}


SetShaderScalarParameterMethod::SetShaderScalarParameterMethod() : SetShaderParameterMethod(kMethodSetShaderScalarParameter)
{
}

SetShaderScalarParameterMethod::SetShaderScalarParameterMethod(int32 slot, float param) : SetShaderParameterMethod(kMethodSetShaderScalarParameter, slot, Vector4D(param, param, param, param))
{
}

SetShaderScalarParameterMethod::SetShaderScalarParameterMethod(const SetShaderScalarParameterMethod& setShaderScalarParameterMethod) : SetShaderParameterMethod(setShaderScalarParameterMethod)
{
}

SetShaderScalarParameterMethod::~SetShaderScalarParameterMethod()
{
}

Method *SetShaderScalarParameterMethod::Replicate(void) const
{
	return (new SetShaderScalarParameterMethod(*this));
}

int32 SetShaderScalarParameterMethod::GetSettingCount(void) const
{
	return (SetShaderParameterMethod::GetSettingCount() + 1);
}

Setting *SetShaderScalarParameterMethod::GetSetting(int32 index) const
{
	int32 count = SetShaderParameterMethod::GetSettingCount();
	if (index < count)
	{
		return (SetShaderParameterMethod::GetSetting(index));
	}

	if (index == count)
	{
		const StringTable *table = TheInterfaceMgr->GetStringTable();
		const char *title = table->GetString(StringID('MTHD', 'SHDR', kMethodSetShaderScalarParameter, 'VALU'));
		return (new TextSetting('VALU', parameterValue.x, title));
	}

	return (nullptr);
}

void SetShaderScalarParameterMethod::SetSetting(const Setting *setting)
{
	Type identifier = setting->GetSettingIdentifier();

	if (identifier == 'VALU')
	{
		float f = Text::StringToFloat(static_cast<const TextSetting *>(setting)->GetText());
		SetParameterValue(Vector4D(f, f, f, f));
	}
	else
	{
		SetShaderParameterMethod::SetSetting(setting);
	}
}


SetShaderVectorParameterMethod::SetShaderVectorParameterMethod() : SetShaderParameterMethod(kMethodSetShaderVectorParameter)
{
}

SetShaderVectorParameterMethod::SetShaderVectorParameterMethod(int32 slot, const Vector4D& param) : SetShaderParameterMethod(kMethodSetShaderVectorParameter, slot, param)
{
}

SetShaderVectorParameterMethod::SetShaderVectorParameterMethod(const SetShaderVectorParameterMethod& setShaderVectorParameterMethod) : SetShaderParameterMethod(setShaderVectorParameterMethod)
{
}

SetShaderVectorParameterMethod::~SetShaderVectorParameterMethod()
{
}

Method *SetShaderVectorParameterMethod::Replicate(void) const
{
	return (new SetShaderVectorParameterMethod(*this));
}

int32 SetShaderVectorParameterMethod::GetSettingCount(void) const
{
	return (SetShaderParameterMethod::GetSettingCount() + 4);
}

Setting *SetShaderVectorParameterMethod::GetSetting(int32 index) const
{
	int32 count = SetShaderParameterMethod::GetSettingCount();
	if (index < count)
	{
		return (SetShaderParameterMethod::GetSetting(index));
	}

	const StringTable *table = TheInterfaceMgr->GetStringTable();

	if (index == count)
	{
		const char *title = table->GetString(StringID('MTHD', 'SHDR', kMethodSetShaderVectorParameter, 'XXXX'));
		return (new TextSetting('XXXX', parameterValue.x, title));
	}

	if (index == count + 1)
	{
		const char *title = table->GetString(StringID('MTHD', 'SHDR', kMethodSetShaderVectorParameter, 'YYYY'));
		return (new TextSetting('YYYY', parameterValue.y, title));
	}

	if (index == count + 2)
	{
		const char *title = table->GetString(StringID('MTHD', 'SHDR', kMethodSetShaderVectorParameter, 'ZZZZ'));
		return (new TextSetting('ZZZZ', parameterValue.z, title));
	}

	if (index == count + 3)
	{
		const char *title = table->GetString(StringID('MTHD', 'SHDR', kMethodSetShaderVectorParameter, 'WWWW'));
		return (new TextSetting('WWWW', parameterValue.w, title));
	}

	return (nullptr);
}

void SetShaderVectorParameterMethod::SetSetting(const Setting *setting)
{
	Type identifier = setting->GetSettingIdentifier();

	if (identifier == 'XXXX')
	{
		parameterValue.x = Text::StringToFloat(static_cast<const TextSetting *>(setting)->GetText());
	}
	else if (identifier == 'YYYY')
	{
		parameterValue.y = Text::StringToFloat(static_cast<const TextSetting *>(setting)->GetText());
	}
	else if (identifier == 'ZZZZ')
	{
		parameterValue.z = Text::StringToFloat(static_cast<const TextSetting *>(setting)->GetText());
	}
	else if (identifier == 'WWWW')
	{
		parameterValue.w = Text::StringToFloat(static_cast<const TextSetting *>(setting)->GetText());
	}
	else
	{
		SetShaderParameterMethod::SetSetting(setting);
	}
}


SetShaderColorParameterMethod::SetShaderColorParameterMethod() : SetShaderParameterMethod(kMethodSetShaderColorParameter)
{
}

SetShaderColorParameterMethod::SetShaderColorParameterMethod(int32 slot, const ColorRGBA& param) : SetShaderParameterMethod(kMethodSetShaderColorParameter, slot, Vector4D(param.red, param.green, param.blue, param.alpha))
{
}

SetShaderColorParameterMethod::SetShaderColorParameterMethod(const SetShaderColorParameterMethod& setShaderColorParameterMethod) : SetShaderParameterMethod(setShaderColorParameterMethod)
{
}

SetShaderColorParameterMethod::~SetShaderColorParameterMethod()
{
}

Method *SetShaderColorParameterMethod::Replicate(void) const
{
	return (new SetShaderColorParameterMethod(*this));
}

int32 SetShaderColorParameterMethod::GetSettingCount(void) const
{
	return (SetShaderParameterMethod::GetSettingCount() + 1);
}

Setting *SetShaderColorParameterMethod::GetSetting(int32 index) const
{
	int32 count = SetShaderParameterMethod::GetSettingCount();
	if (index < count)
	{
		return (SetShaderParameterMethod::GetSetting(index));
	}

	if (index == count)
	{
		const StringTable *table = TheInterfaceMgr->GetStringTable();
		const char *title = table->GetString(StringID('MTHD', 'SHDR', kMethodSetShaderColorParameter, 'COLR'));
		const char *picker = table->GetString(StringID('MTHD', 'SHDR', kMethodSetShaderColorParameter, 'PICK'));
		return (new ColorSetting('COLR', ColorRGBA(parameterValue.x, parameterValue.y, parameterValue.z, parameterValue.w), title, picker, kColorPickerAlpha));
	}

	return (nullptr);
}

void SetShaderColorParameterMethod::SetSetting(const Setting *setting)
{
	Type identifier = setting->GetSettingIdentifier();

	if (identifier == 'COLR')
	{
		const ColorRGBA& color = static_cast<const ColorSetting *>(setting)->GetColor();
		parameterValue.Set(color.red, color.green, color.blue, color.alpha);
	}
	else
	{
		SetShaderParameterMethod::SetSetting(setting);
	}
}

// ZYUQURM
