 

#include "C4Expressions.h"


using namespace C4;


Token::Token(C4::TokenType type)
{
	tokenType = type;
}

Token::~Token()
{
}


IdentifierToken::IdentifierToken(const char *identifier) : Token(kTokenIdentifier)
{
	identifierString = identifier;
}

IdentifierToken::~IdentifierToken()
{
}


BooleanToken::BooleanToken(bool value) : Token(kTokenBoolean)
{
	booleanValue = value;
}

BooleanToken::~BooleanToken()
{
}


IntegerToken::IntegerToken(int32 value) : Token(kTokenInteger)
{
	integerValue = value;
}

IntegerToken::~IntegerToken()
{
}


FloatToken::FloatToken(float value) : Token(kTokenFloat)
{
	floatValue = value;
}

FloatToken::~FloatToken()
{
}


StringToken::StringToken(const char *string) : Token(kTokenString)
{
	stringValue = string;
}

StringToken::~StringToken()
{
}


Evaluator::Evaluator(EvaluatorType type)
{
	evaluatorType = type;
}

Evaluator::Evaluator(const Evaluator& evaluator)
{
	evaluatorType = evaluator.evaluatorType;
}

Evaluator::~Evaluator()
{
}

Evaluator *Evaluator::Create(Unpacker& data, unsigned_int32 unpackFlags)
{
	switch (data.GetType())
	{
		case kEvaluatorValue:

			return (new ValueEvaluator);

		case kEvaluatorMember:

			return (new MemberEvaluator);

		case kEvaluatorArray:

			return (new ArrayEvaluator);

		case kEvaluatorBoolean:

			return (new BooleanEvaluator);

		case kEvaluatorInteger: 

			return (new IntegerEvaluator);
 
		case kEvaluatorFloat:
 
			return (new FloatEvaluator);

		case kEvaluatorString: 

			return (new StringEvaluator); 
 
		case kEvaluatorNegate:

			return (new NegateEvaluator);
 
		case kEvaluatorInvert:

			return (new InvertEvaluator);

		case kEvaluatorMultiply:

			return (new MultiplyEvaluator);

		case kEvaluatorDivide:

			return (new DivideEvaluator);

		case kEvaluatorModulo:

			return (new ModuloEvaluator);

		case kEvaluatorAdd:

			return (new AddEvaluator);

		case kEvaluatorSubtract:

			return (new SubtractEvaluator);

		case kEvaluatorAnd:

			return (new AndEvaluator);

		case kEvaluatorOr:

			return (new OrEvaluator);

		case kEvaluatorXor:

			return (new XorEvaluator);

		case kEvaluatorShiftLeft:

			return (new ShiftLeftEvaluator);

		case kEvaluatorShiftRight:

			return (new ShiftRightEvaluator);

		case kEvaluatorLess:

			return (new LessEvaluator);

		case kEvaluatorGreater:

			return (new GreaterEvaluator);

		case kEvaluatorLessEqual:

			return (new LessEqualEvaluator);

		case kEvaluatorGreaterEqual:

			return (new GreaterEqualEvaluator);

		case kEvaluatorEqual:

			return (new EqualEvaluator);

		case kEvaluatorNotEqual:

			return (new NotEqualEvaluator);
	}

	return (nullptr);
}

Evaluator *Evaluator::Clone(void) const
{
	Evaluator *root = Replicate();

	Evaluator *subnode = GetFirstSubnode();
	while (subnode)
	{
		Evaluator *clone = subnode->Clone();
		root->AppendSubnode(clone);

		subnode = subnode->Next();
	}

	return (root);
}

void Evaluator::PackType(Packer& data) const
{
	data << evaluatorType;
}

void Evaluator::Pack(Packer& data, unsigned_int32 packFlags) const
{
	const Evaluator *super = GetSuperNode();
	int32 superIndex = (super) ? super->evaluatorIndex : -1;

	data << superIndex;
}

void Evaluator::Unpack(Unpacker& data, unsigned_int32 unpackFlags)
{
	data >> superIndex;
}


ValueEvaluator::ValueEvaluator() : Evaluator(kEvaluatorValue)
{
}

ValueEvaluator::ValueEvaluator(EvaluatorType type) : Evaluator(type)
{
}

ValueEvaluator::ValueEvaluator(const char *name) : Evaluator(kEvaluatorValue)
{
	valueName = name;
}

ValueEvaluator::ValueEvaluator(EvaluatorType type, const char *name) : Evaluator(type)
{
	valueName = name;
}

ValueEvaluator::ValueEvaluator(const ValueEvaluator& valueEvaluator) : Evaluator(valueEvaluator)
{
	valueName = valueEvaluator.valueName;
}

ValueEvaluator::~ValueEvaluator()
{
}

Evaluator *ValueEvaluator::Replicate(void) const
{
	return (new ValueEvaluator(*this));
}

void ValueEvaluator::Pack(Packer& data, unsigned_int32 packFlags) const
{
	Evaluator::Pack(data, packFlags);

	data << valueName;
}

void ValueEvaluator::Unpack(Unpacker& data, unsigned_int32 unpackFlags)
{
	Evaluator::Unpack(data, unpackFlags);

	data >> valueName;
}

const Value *ValueEvaluator::Evaluate(const ScriptState *state)
{
	return (state->GetValue(valueName));
}


MemberEvaluator::MemberEvaluator() : ValueEvaluator(kEvaluatorMember)
{
}

MemberEvaluator::MemberEvaluator(const char *name) : ValueEvaluator(kEvaluatorMember, name)
{
}

MemberEvaluator::MemberEvaluator(const MemberEvaluator& memberEvaluator) : ValueEvaluator(memberEvaluator)
{
}

MemberEvaluator::~MemberEvaluator()
{
}

Evaluator *MemberEvaluator::Replicate(void) const
{
	return (new MemberEvaluator(*this));
}

const Value *MemberEvaluator::Evaluate(const ScriptState *state)
{
	const Value *value = ValueEvaluator::Evaluate(state);
	if (value)
	{
		const IntegerValue *subvalue = static_cast<const IntegerValue *>(GetFirstSubnode()->Evaluate(state));
		if (subvalue)
		{
			int32 index = subvalue->GetValue();

			ValueType type = value->GetValueType();
			if (type == kValueColor)
			{
				const ColorRGBA& c = static_cast<const ColorValue *>(value)->GetValue();

				if (index == 'r')
				{
					memberValue.SetValue(c.red);
					return (&memberValue);
				}

				if (index == 'g')
				{
					memberValue.SetValue(c.green);
					return (&memberValue);
				}

				if (index == 'b')
				{
					memberValue.SetValue(c.blue);
					return (&memberValue);
				}

				if (index == 'a')
				{
					memberValue.SetValue(c.alpha);
					return (&memberValue);
				}
			}
			else if ((type == kValueVector) || (type == kValuePoint))
			{
				const Vector3D& v = static_cast<const VectorValue *>(value)->GetValue();

				if (index == 'x')
				{
					memberValue.SetValue(v.x);
					return (&memberValue);
				}

				if (index == 'y')
				{
					memberValue.SetValue(v.y);
					return (&memberValue);
				}

				if (index == 'z')
				{
					memberValue.SetValue(v.z);
					return (&memberValue);
				}
			}
		}
	}

	return (nullptr);
}


ArrayEvaluator::ArrayEvaluator() : Evaluator(kEvaluatorArray)
{
}

ArrayEvaluator::ArrayEvaluator(const char *name) : Evaluator(kEvaluatorArray)
{
	valueName = name;
}

ArrayEvaluator::ArrayEvaluator(const ArrayEvaluator& arrayEvaluator) : Evaluator(arrayEvaluator)
{
	valueName = arrayEvaluator.valueName;
}

ArrayEvaluator::~ArrayEvaluator()
{
}

Evaluator *ArrayEvaluator::Replicate(void) const
{
	return (new ArrayEvaluator(*this));
}

void ArrayEvaluator::Pack(Packer& data, unsigned_int32 packFlags) const
{
	Evaluator::Pack(data, packFlags);

	data << valueName;
}

void ArrayEvaluator::Unpack(Unpacker& data, unsigned_int32 unpackFlags)
{
	Evaluator::Unpack(data, unpackFlags);

	data >> valueName;
}

const Value *ArrayEvaluator::Evaluate(const ScriptState *state)
{
	/*const Value *value = GetFirstSubnode()->Evaluate(state);
	if (value)
	{
		int32	index;

		switch (value->GetValueType())
		{
			case kValueBoolean:

				index = static_cast<const BooleanValue *>(value)->GetValue();
				break;

			case kValueInteger:

				index = static_cast<const IntegerValue *>(value)->GetValue();
				break;

			case kValueFloat:

				index = (int32) static_cast<const FloatValue *>(value)->GetValue();
				break;

			case kValueString:

				index = Text::StringToInteger(static_cast<const StringValue *>(value)->GetValue());
				break;

			default:

				return (nullptr);
		}

		return (state->GetValue(valueName));
	}*/

	return (nullptr);
}


BooleanEvaluator::BooleanEvaluator() : Evaluator(kEvaluatorBoolean)
{
}

BooleanEvaluator::BooleanEvaluator(bool value) :
		Evaluator(kEvaluatorBoolean),
		booleanValue(value)
{
}

BooleanEvaluator::BooleanEvaluator(const BooleanEvaluator& booleanEvaluator) :
		Evaluator(booleanEvaluator),
		booleanValue(booleanEvaluator.booleanValue)
{
}

BooleanEvaluator::~BooleanEvaluator()
{
}

Evaluator *BooleanEvaluator::Replicate(void) const
{
	return (new BooleanEvaluator(*this));
}

void BooleanEvaluator::Pack(Packer& data, unsigned_int32 packFlags) const
{
	Evaluator::Pack(data, packFlags);
	booleanValue.Pack(data, packFlags);
}

void BooleanEvaluator::Unpack(Unpacker& data, unsigned_int32 unpackFlags)
{
	Evaluator::Unpack(data, unpackFlags);
	booleanValue.Unpack(data, unpackFlags);
}

const Value *BooleanEvaluator::Evaluate(const ScriptState *state)
{
	return (&booleanValue);
}


IntegerEvaluator::IntegerEvaluator() : Evaluator(kEvaluatorInteger)
{
}

IntegerEvaluator::IntegerEvaluator(int32 value) :
		Evaluator(kEvaluatorInteger),
		integerValue(value)
{
}

IntegerEvaluator::IntegerEvaluator(const IntegerEvaluator& integerEvaluator) :
		Evaluator(integerEvaluator),
		integerValue(integerEvaluator.integerValue)
{
}

IntegerEvaluator::~IntegerEvaluator()
{
}

Evaluator *IntegerEvaluator::Replicate(void) const
{
	return (new IntegerEvaluator(*this));
}

void IntegerEvaluator::Pack(Packer& data, unsigned_int32 packFlags) const
{
	Evaluator::Pack(data, packFlags);
	integerValue.Pack(data, packFlags);
}

void IntegerEvaluator::Unpack(Unpacker& data, unsigned_int32 unpackFlags)
{
	Evaluator::Unpack(data, unpackFlags);
	integerValue.Unpack(data, unpackFlags);
}

const Value *IntegerEvaluator::Evaluate(const ScriptState *state)
{
	return (&integerValue);
}


FloatEvaluator::FloatEvaluator() : Evaluator(kEvaluatorFloat)
{
}

FloatEvaluator::FloatEvaluator(float value) :
		Evaluator(kEvaluatorFloat),
		floatValue(value)
{
}

FloatEvaluator::FloatEvaluator(const FloatEvaluator& floatEvaluator) :
		Evaluator(floatEvaluator),
		floatValue(floatEvaluator.floatValue)
{
}

FloatEvaluator::~FloatEvaluator()
{
}

Evaluator *FloatEvaluator::Replicate(void) const
{
	return (new FloatEvaluator(*this));
}

void FloatEvaluator::Pack(Packer& data, unsigned_int32 packFlags) const
{
	Evaluator::Pack(data, packFlags);
	floatValue.Pack(data, packFlags);
}

void FloatEvaluator::Unpack(Unpacker& data, unsigned_int32 unpackFlags)
{
	Evaluator::Unpack(data, unpackFlags);
	floatValue.Unpack(data, unpackFlags);
}

const Value *FloatEvaluator::Evaluate(const ScriptState *state)
{
	return (&floatValue);
}


StringEvaluator::StringEvaluator() : Evaluator(kEvaluatorString)
{
}

StringEvaluator::StringEvaluator(const char *value) :
		Evaluator(kEvaluatorString),
		stringValue(value)
{
}

StringEvaluator::StringEvaluator(const StringEvaluator& stringEvaluator) :
		Evaluator(stringEvaluator),
		stringValue(stringEvaluator.stringValue)
{
}

StringEvaluator::~StringEvaluator()
{
}

Evaluator *StringEvaluator::Replicate(void) const
{
	return (new StringEvaluator(*this));
}

void StringEvaluator::Pack(Packer& data, unsigned_int32 packFlags) const
{
	Evaluator::Pack(data, packFlags);
	stringValue.Pack(data, packFlags);
}

void StringEvaluator::Unpack(Unpacker& data, unsigned_int32 unpackFlags)
{
	Evaluator::Unpack(data, unpackFlags);
	stringValue.Unpack(data, unpackFlags);
}

const Value *StringEvaluator::Evaluate(const ScriptState *state)
{
	return (&stringValue);
}


OperatorEvaluator::OperatorEvaluator(EvaluatorType type) : Evaluator(type)
{
	outputValue = nullptr;
}

OperatorEvaluator::OperatorEvaluator(const OperatorEvaluator& operatorEvaluator) : Evaluator(operatorEvaluator)
{
	outputValue = nullptr;
}

OperatorEvaluator::~OperatorEvaluator()
{
	delete outputValue;
}

const Value *OperatorEvaluator::OutputBooleanValue(bool v)
{
	Value *output = outputValue;
	if (output)
	{
		if (output->GetValueType() == kValueBoolean)
		{
			static_cast<BooleanValue *>(output)->SetValue(v);
			return (output);
		}

		delete output;
	}

	output = new BooleanValue(v);
	outputValue = output;
	return (output);
}

const Value *OperatorEvaluator::OutputIntegerValue(int32 v)
{
	Value *output = outputValue;
	if (output)
	{
		if (output->GetValueType() == kValueInteger)
		{
			static_cast<IntegerValue *>(output)->SetValue(v);
			return (output);
		}

		delete output;
	}

	output = new IntegerValue(v);
	outputValue = output;
	return (output);
}

const Value *OperatorEvaluator::OutputFloatValue(float v)
{
	Value *output = outputValue;
	if (output)
	{
		if (output->GetValueType() == kValueFloat)
		{
			static_cast<FloatValue *>(output)->SetValue(v);
			return (output);
		}

		delete output;
	}

	output = new FloatValue(v);
	outputValue = output;
	return (output);
}

const Value *OperatorEvaluator::OutputStringValue(const char *v)
{
	Value *output = outputValue;
	if (output)
	{
		if (output->GetValueType() == kValueString)
		{
			static_cast<StringValue *>(output)->SetValue(v);
			return (output);
		}

		delete output;
	}

	output = new StringValue(v);
	outputValue = output;
	return (output);
}

const Value *OperatorEvaluator::OutputColorValue(const ColorRGBA& v)
{
	Value *output = outputValue;
	if (output)
	{
		if (output->GetValueType() == kValueColor)
		{
			static_cast<ColorValue *>(output)->SetValue(v);
			return (output);
		}

		delete output;
	}

	output = new ColorValue(v);
	outputValue = output;
	return (output);
}

const Value *OperatorEvaluator::OutputVectorValue(const Vector3D& v)
{
	Value *output = outputValue;
	if (output)
	{
		if (output->GetValueType() == kValueVector)
		{
			static_cast<VectorValue *>(output)->SetValue(v);
			return (output);
		}

		delete output;
	}

	output = new VectorValue(v);
	outputValue = output;
	return (output);
}

const Value *OperatorEvaluator::OutputPointValue(const Point3D& v)
{
	Value *output = outputValue;
	if (output)
	{
		if (output->GetValueType() == kValuePoint)
		{
			static_cast<PointValue *>(output)->SetValue(v);
			return (output);
		}

		delete output;
	}

	output = new PointValue(v);
	outputValue = output;
	return (output);
}


NegateEvaluator::NegateEvaluator() : OperatorEvaluator(kEvaluatorNegate)
{
}

NegateEvaluator::NegateEvaluator(const NegateEvaluator& negateEvaluator) : OperatorEvaluator(negateEvaluator)
{
}

NegateEvaluator::~NegateEvaluator()
{
}

Evaluator *NegateEvaluator::Replicate(void) const
{
	return (new NegateEvaluator(*this));
}

const Value *NegateEvaluator::Evaluate(const ScriptState *state)
{
	const Value *value = GetFirstSubnode()->Evaluate(state);
	if (value)
	{
		switch (value->GetValueType())
		{
			case kValueBoolean:

				return (OutputIntegerValue(-(int32) static_cast<const BooleanValue *>(value)->GetValue()));

			case kValueInteger:

				return (OutputIntegerValue(-static_cast<const IntegerValue *>(value)->GetValue()));

			case kValueFloat:

				return (OutputFloatValue(-static_cast<const FloatValue *>(value)->GetValue()));

			case kValueColor:

				return (OutputColorValue(-static_cast<const ColorValue *>(value)->GetValue()));

			case kValueVector:

				return (OutputVectorValue(-static_cast<const VectorValue *>(value)->GetValue()));

			case kValuePoint:

				return (OutputPointValue(-static_cast<const PointValue *>(value)->GetValue()));
		}
	}

	return (nullptr);
}


InvertEvaluator::InvertEvaluator() : OperatorEvaluator(kEvaluatorInvert)
{
}

InvertEvaluator::InvertEvaluator(const InvertEvaluator& invertEvaluator) : OperatorEvaluator(invertEvaluator)
{
}

InvertEvaluator::~InvertEvaluator()
{
}

Evaluator *InvertEvaluator::Replicate(void) const
{
	return (new InvertEvaluator(*this));
}

const Value *InvertEvaluator::Evaluate(const ScriptState *state)
{
	const Value *value = GetFirstSubnode()->Evaluate(state);
	if (value)
	{
		switch (value->GetValueType())
		{
			case kValueBoolean:

				return (OutputBooleanValue(!static_cast<const BooleanValue *>(value)->GetValue()));

			case kValueInteger:

				return (OutputIntegerValue(~static_cast<const IntegerValue *>(value)->GetValue()));
		}
	}

	return (nullptr);
}


MultiplyEvaluator::MultiplyEvaluator() : OperatorEvaluator(kEvaluatorMultiply)
{
}

MultiplyEvaluator::MultiplyEvaluator(const MultiplyEvaluator& multiplyEvaluator) : OperatorEvaluator(multiplyEvaluator)
{
}

MultiplyEvaluator::~MultiplyEvaluator()
{
}

Evaluator *MultiplyEvaluator::Replicate(void) const
{
	return (new MultiplyEvaluator(*this));
}

const Value *MultiplyEvaluator::Evaluate(const ScriptState *state)
{
	const Value *value1 = GetFirstSubnode()->Evaluate(state);
	if (value1)
	{
		const Value *value2 = GetLastSubnode()->Evaluate(state);
		if (value2)
		{
			ValueType type1 = value1->GetValueType();
			ValueType type2 = value2->GetValueType();

			if ((type1 == kValueBoolean) || (type1 == kValueInteger))
			{
				int32 v1 = (type1 == kValueBoolean) ? (int32) static_cast<const BooleanValue *>(value1)->GetValue() : static_cast<const IntegerValue *>(value1)->GetValue();

				switch (type2)
				{
					case kValueBoolean:
						return (OutputIntegerValue(v1 * (int32) static_cast<const BooleanValue *>(value2)->GetValue()));
					case kValueInteger:
						return (OutputIntegerValue(v1 * static_cast<const IntegerValue *>(value2)->GetValue()));
					case kValueFloat:
						return (OutputFloatValue((float) v1 * static_cast<const FloatValue *>(value2)->GetValue()));
					case kValueColor:
						return (OutputColorValue(static_cast<const ColorValue *>(value2)->GetValue() * (float) v1));
					case kValueVector:
						return (OutputVectorValue(static_cast<const VectorValue *>(value2)->GetValue() * (float) v1));
					case kValuePoint:
						return (OutputPointValue(static_cast<const PointValue *>(value2)->GetValue() * (float) v1));
				}
			}
			else if (type1 == kValueFloat)
			{
				float v1 = static_cast<const FloatValue *>(value1)->GetValue();

				switch (type2)
				{
					case kValueBoolean:
						return (OutputFloatValue(v1 * (float) static_cast<const BooleanValue *>(value2)->GetValue()));
					case kValueInteger:
						return (OutputFloatValue(v1 * (float) static_cast<const IntegerValue *>(value2)->GetValue()));
					case kValueFloat:
						return (OutputFloatValue(v1 * static_cast<const FloatValue *>(value2)->GetValue()));
					case kValueColor:
						return (OutputColorValue(static_cast<const ColorValue *>(value2)->GetValue() * v1));
					case kValueVector:
						return (OutputVectorValue(static_cast<const VectorValue *>(value2)->GetValue() * v1));
					case kValuePoint:
						return (OutputPointValue(static_cast<const PointValue *>(value2)->GetValue() * v1));
				}
			}
			else if (type1 == kValueColor)
			{
				const ColorRGBA& v1 = static_cast<const ColorValue *>(value1)->GetValue();

				switch (type2)
				{
					case kValueBoolean:
						return (OutputColorValue(v1 * (float) static_cast<const BooleanValue *>(value2)->GetValue()));
					case kValueInteger:
						return (OutputColorValue(v1 * (float) static_cast<const IntegerValue *>(value2)->GetValue()));
					case kValueFloat:
						return (OutputColorValue(v1 * static_cast<const FloatValue *>(value2)->GetValue()));
					case kValueColor:
						return (OutputColorValue(v1 * static_cast<const ColorValue *>(value2)->GetValue()));
				}
			}
			else if (type1 == kValueVector)
			{
				const Vector3D& v1 = static_cast<const VectorValue *>(value1)->GetValue();

				switch (type2)
				{
					case kValueBoolean:
						return (OutputVectorValue(v1 * (float) static_cast<const BooleanValue *>(value2)->GetValue()));
					case kValueInteger:
						return (OutputVectorValue(v1 * (float) static_cast<const IntegerValue *>(value2)->GetValue()));
					case kValueFloat:
						return (OutputVectorValue(v1 * static_cast<const FloatValue *>(value2)->GetValue()));
					case kValueVector:
						return (OutputVectorValue(v1 & static_cast<const VectorValue *>(value2)->GetValue()));
					case kValuePoint:
						return (OutputVectorValue(v1 & static_cast<const PointValue *>(value2)->GetValue()));
				}
			}
			else if (type1 == kValuePoint)
			{
				const Point3D& v1 = static_cast<const PointValue *>(value1)->GetValue();

				switch (type2)
				{
					case kValueBoolean:
						return (OutputPointValue(v1 * (float) static_cast<const BooleanValue *>(value2)->GetValue()));
					case kValueInteger:
						return (OutputPointValue(v1 * (float) static_cast<const IntegerValue *>(value2)->GetValue()));
					case kValueFloat:
						return (OutputPointValue(v1 * static_cast<const FloatValue *>(value2)->GetValue()));
					case kValueVector:
						return (OutputPointValue(v1 & static_cast<const VectorValue *>(value2)->GetValue()));
					case kValuePoint:
						return (OutputPointValue(v1 & static_cast<const PointValue *>(value2)->GetValue()));
				}
			}
		}
	}

	return (nullptr);
}


DivideEvaluator::DivideEvaluator() : OperatorEvaluator(kEvaluatorDivide)
{
}

DivideEvaluator::DivideEvaluator(const DivideEvaluator& divideEvaluator) : OperatorEvaluator(divideEvaluator)
{
}

DivideEvaluator::~DivideEvaluator()
{
}

Evaluator *DivideEvaluator::Replicate(void) const
{
	return (new DivideEvaluator(*this));
}

const Value *DivideEvaluator::Evaluate(const ScriptState *state)
{
	const Value *value1 = GetFirstSubnode()->Evaluate(state);
	if (value1)
	{
		const Value *value2 = GetLastSubnode()->Evaluate(state);
		if (value2)
		{
			ValueType type2 = value2->GetValueType();

			if (type2 == kValueInteger)
			{
				int32 v2 = static_cast<const IntegerValue *>(value2)->GetValue();

				switch (value1->GetValueType())
				{
					case kValueInteger:
						return (OutputIntegerValue((v2 != 0) ? static_cast<const IntegerValue *>(value1)->GetValue() / v2 : 0));
					case kValueFloat:
						return (OutputFloatValue(static_cast<const FloatValue *>(value1)->GetValue() / (float) v2));
					case kValueColor:
						return (OutputColorValue(static_cast<const ColorValue *>(value1)->GetValue() / (float) v2));
					case kValueVector:
						return (OutputVectorValue(static_cast<const VectorValue *>(value1)->GetValue() / (float) v2));
					case kValuePoint:
						return (OutputPointValue(static_cast<const PointValue *>(value1)->GetValue() / (float) v2));
				}
			}
			else if (type2 == kValueFloat)
			{
				float v2 = static_cast<const FloatValue *>(value2)->GetValue();

				switch (value1->GetValueType())
				{
					case kValueInteger:
						return (OutputFloatValue((float) static_cast<const IntegerValue *>(value1)->GetValue() / v2));
					case kValueFloat:
						return (OutputFloatValue(static_cast<const FloatValue *>(value1)->GetValue() / v2));
					case kValueColor:
						return (OutputColorValue(static_cast<const ColorValue *>(value1)->GetValue() / v2));
					case kValueVector:
						return (OutputVectorValue(static_cast<const VectorValue *>(value1)->GetValue() / v2));
					case kValuePoint:
						return (OutputPointValue(static_cast<const PointValue *>(value1)->GetValue() / v2));
				}
			}
		}
	}

	return (nullptr);
}


ModuloEvaluator::ModuloEvaluator() : OperatorEvaluator(kEvaluatorModulo)
{
}

ModuloEvaluator::ModuloEvaluator(const ModuloEvaluator& moduloEvaluator) : OperatorEvaluator(moduloEvaluator)
{
}

ModuloEvaluator::~ModuloEvaluator()
{
}

Evaluator *ModuloEvaluator::Replicate(void) const
{
	return (new ModuloEvaluator(*this));
}

const Value *ModuloEvaluator::Evaluate(const ScriptState *state)
{
	const Value *value1 = GetFirstSubnode()->Evaluate(state);
	if ((value1) && (value1->GetValueType() == kValueInteger))
	{
		const Value *value2 = GetLastSubnode()->Evaluate(state);
		if ((value2) && (value2->GetValueType() == kValueInteger))
		{
			int32 v1 = static_cast<const IntegerValue *>(value1)->GetValue();
			int32 v2 = static_cast<const IntegerValue *>(value2)->GetValue();
			return (OutputIntegerValue((v2 != 0) ? v1 % v2 : 0));
		}
	}

	return (nullptr);
}


AddEvaluator::AddEvaluator() : OperatorEvaluator(kEvaluatorAdd)
{
}

AddEvaluator::AddEvaluator(const AddEvaluator& addEvaluator) : OperatorEvaluator(addEvaluator)
{
}

AddEvaluator::~AddEvaluator()
{
}

Evaluator *AddEvaluator::Replicate(void) const
{
	return (new AddEvaluator(*this));
}

const Value *AddEvaluator::Evaluate(const ScriptState *state)
{
	const Value *value1 = GetFirstSubnode()->Evaluate(state);
	if (value1)
	{
		const Value *value2 = GetLastSubnode()->Evaluate(state);
		if (value2)
		{
			ValueType type1 = value1->GetValueType();
			ValueType type2 = value2->GetValueType();

			if (type1 == kValueBoolean)
			{
				int32 v1 = (int32) static_cast<const BooleanValue *>(value1)->GetValue();

				switch (type2)
				{
					case kValueBoolean:
						return (OutputIntegerValue(v1 + (int32) static_cast<const BooleanValue *>(value2)->GetValue()));
					case kValueInteger:
						return (OutputIntegerValue(v1 + static_cast<const IntegerValue *>(value2)->GetValue()));
					case kValueFloat:
						return (OutputFloatValue((float) v1 + static_cast<const FloatValue *>(value2)->GetValue()));
					case kValueString:
						return (OutputStringValue(String<kMaxStringValueLength>((int32) v1) += static_cast<const StringValue *>(value2)->GetValue()));
				}
			}
			else if (type1 == kValueInteger)
			{
				int32 v1 = static_cast<const IntegerValue *>(value1)->GetValue();

				switch (type2)
				{
					case kValueBoolean:
						return (OutputIntegerValue(v1 + (int32) static_cast<const BooleanValue *>(value2)->GetValue()));
					case kValueInteger:
						return (OutputIntegerValue(v1 + static_cast<const IntegerValue *>(value2)->GetValue()));
					case kValueFloat:
						return (OutputFloatValue((float) v1 + static_cast<const FloatValue *>(value2)->GetValue()));
					case kValueString:
						return (OutputStringValue(String<kMaxStringValueLength>(v1) += static_cast<const StringValue *>(value2)->GetValue()));
				}
			}
			else if (type1 == kValueFloat)
			{
				float v1 = static_cast<const FloatValue *>(value1)->GetValue();

				switch (type2)
				{
					case kValueBoolean:
						return (OutputFloatValue(v1 + (float) static_cast<const BooleanValue *>(value2)->GetValue()));
					case kValueInteger:
						return (OutputFloatValue(v1 + (float) static_cast<const IntegerValue *>(value2)->GetValue()));
					case kValueFloat:
						return (OutputFloatValue(v1 + static_cast<const FloatValue *>(value2)->GetValue()));
					case kValueString:
						return (OutputStringValue(String<kMaxStringValueLength>(v1) += static_cast<const StringValue *>(value2)->GetValue()));
				}
			}
			else if (type1 == kValueString)
			{
				const String<kMaxStringValueLength>& v1 = static_cast<const StringValue *>(value1)->GetValue();

				switch (type2)
				{
					case kValueBoolean:
						return (OutputStringValue(v1 + String<kMaxStringValueLength>((int32) static_cast<const BooleanValue *>(value2)->GetValue())));
					case kValueInteger:
						return (OutputStringValue(v1 + String<kMaxStringValueLength>(static_cast<const IntegerValue *>(value2)->GetValue())));
					case kValueFloat:
						return (OutputStringValue(v1 + String<kMaxStringValueLength>(static_cast<const FloatValue *>(value2)->GetValue())));
					case kValueString:
						return (OutputStringValue(v1 + static_cast<const StringValue *>(value2)->GetValue()));
				}
			}
			else if (type1 == kValueColor)
			{
				if (type2 == kValueColor)
				{
					const ColorRGBA& v1 = static_cast<const ColorValue *>(value1)->GetValue();
					const ColorRGBA& v2 = static_cast<const ColorValue *>(value2)->GetValue();
					return (OutputColorValue(v1 + v2));
				}
			}
			else if (type1 == kValueVector)
			{
				const Vector3D& v1 = static_cast<const VectorValue *>(value1)->GetValue();

				switch (type2)
				{
					case kValueVector:
						return (OutputVectorValue(v1 + static_cast<const VectorValue *>(value2)->GetValue()));
					case kValuePoint:
						return (OutputPointValue(static_cast<const PointValue *>(value2)->GetValue() + v1));
				}
			}
			else if (type1 == kValuePoint)
			{
				const Point3D& v1 = static_cast<const PointValue *>(value1)->GetValue();

				switch (type2)
				{
					case kValueVector:
						return (OutputPointValue(v1 + static_cast<const VectorValue *>(value2)->GetValue()));
					case kValuePoint:
						return (OutputPointValue(v1 + static_cast<const PointValue *>(value2)->GetValue()));
				}
			}
		}
	}

	return (nullptr);
}


SubtractEvaluator::SubtractEvaluator() : OperatorEvaluator(kEvaluatorSubtract)
{
}

SubtractEvaluator::SubtractEvaluator(const SubtractEvaluator& subtractEvaluator) : OperatorEvaluator(subtractEvaluator)
{
}

SubtractEvaluator::~SubtractEvaluator()
{
}

Evaluator *SubtractEvaluator::Replicate(void) const
{
	return (new SubtractEvaluator(*this));
}

const Value *SubtractEvaluator::Evaluate(const ScriptState *state)
{
	const Value *value1 = GetFirstSubnode()->Evaluate(state);
	if (value1)
	{
		const Value *value2 = GetLastSubnode()->Evaluate(state);
		if (value2)
		{
			ValueType type1 = value1->GetValueType();
			ValueType type2 = value2->GetValueType();

			if (type1 == kValueBoolean)
			{
				int32 v1 = (int32) static_cast<const BooleanValue *>(value1)->GetValue();

				switch (type2)
				{
					case kValueBoolean:
						return (OutputIntegerValue(v1 - (int32) static_cast<const BooleanValue *>(value2)->GetValue()));
					case kValueInteger:
						return (OutputIntegerValue(v1 - static_cast<const IntegerValue *>(value2)->GetValue()));
					case kValueFloat:
						return (OutputFloatValue((float) v1 - static_cast<const FloatValue *>(value2)->GetValue()));
				}
			}
			else if (type1 == kValueInteger)
			{
				int32 v1 = static_cast<const IntegerValue *>(value1)->GetValue();

				switch (type2)
				{
					case kValueBoolean:
						return (OutputIntegerValue(v1 - (int32) static_cast<const BooleanValue *>(value2)->GetValue()));
					case kValueInteger:
						return (OutputIntegerValue(v1 - static_cast<const IntegerValue *>(value2)->GetValue()));
					case kValueFloat:
						return (OutputFloatValue((float) v1 - static_cast<const FloatValue *>(value2)->GetValue()));
				}
			}
			else if (type1 == kValueFloat)
			{
				float v1 = static_cast<const FloatValue *>(value1)->GetValue();

				switch (type2)
				{
					case kValueBoolean:
						return (OutputFloatValue(v1 - (float) static_cast<const BooleanValue *>(value2)->GetValue()));
					case kValueInteger:
						return (OutputFloatValue(v1 - (float) static_cast<const IntegerValue *>(value2)->GetValue()));
					case kValueFloat:
						return (OutputFloatValue(v1 - static_cast<const FloatValue *>(value2)->GetValue()));
				}
			}
			else if (type1 == kValueColor)
			{
				if (type2 == kValueColor)
				{
					const ColorRGBA& v1 = static_cast<const ColorValue *>(value1)->GetValue();
					const ColorRGBA& v2 = static_cast<const ColorValue *>(value2)->GetValue();
					return (OutputColorValue(v1 - v2));
				}
			}
			else if (type1 == kValueVector)
			{
				const Vector3D& v1 = static_cast<const VectorValue *>(value1)->GetValue();

				switch (type2)
				{
					case kValueVector:
						return (OutputVectorValue(v1 - static_cast<const VectorValue *>(value2)->GetValue()));
					case kValuePoint:
						return (OutputVectorValue(v1 - static_cast<const PointValue *>(value2)->GetValue()));
				}
			}
			else if (type1 == kValuePoint)
			{
				const Point3D& v1 = static_cast<const PointValue *>(value1)->GetValue();

				switch (type2)
				{
					case kValueVector:
						return (OutputPointValue(v1 - static_cast<const VectorValue *>(value2)->GetValue()));
					case kValuePoint:
						return (OutputVectorValue(v1 - static_cast<const PointValue *>(value2)->GetValue()));
				}
			}
		}
	}

	return (nullptr);
}


AndEvaluator::AndEvaluator() : OperatorEvaluator(kEvaluatorAnd)
{
}

AndEvaluator::AndEvaluator(const AndEvaluator& andEvaluator) : OperatorEvaluator(andEvaluator)
{
}

AndEvaluator::~AndEvaluator()
{
}

Evaluator *AndEvaluator::Replicate(void) const
{
	return (new AndEvaluator(*this));
}

const Value *AndEvaluator::Evaluate(const ScriptState *state)
{
	const Value *value1 = GetFirstSubnode()->Evaluate(state);
	if (value1)
	{
		const Value *value2 = GetLastSubnode()->Evaluate(state);
		if (value2)
		{
			ValueType type1 = value1->GetValueType();
			ValueType type2 = value2->GetValueType();

			if (type1 == kValueBoolean)
			{
				bool v1 = static_cast<const BooleanValue *>(value1)->GetValue();

				switch (type2)
				{
					case kValueBoolean:
						return (OutputBooleanValue(v1 & static_cast<const BooleanValue *>(value2)->GetValue()));
					case kValueInteger:
						return (OutputIntegerValue((int32) v1 & static_cast<const IntegerValue *>(value2)->GetValue()));
				}
			}
			else if (type1 == kValueInteger)
			{
				int32 v1 = static_cast<const IntegerValue *>(value1)->GetValue();

				switch (type2)
				{
					case kValueBoolean:
						return (OutputIntegerValue(v1 & (int32) static_cast<const BooleanValue *>(value2)->GetValue()));
					case kValueInteger:
						return (OutputIntegerValue(v1 & static_cast<const IntegerValue *>(value2)->GetValue()));
				}
			}
		}
	}

	return (nullptr);
}


OrEvaluator::OrEvaluator() : OperatorEvaluator(kEvaluatorOr)
{
}

OrEvaluator::OrEvaluator(const OrEvaluator& orEvaluator) : OperatorEvaluator(orEvaluator)
{
}

OrEvaluator::~OrEvaluator()
{
}

Evaluator *OrEvaluator::Replicate(void) const
{
	return (new OrEvaluator(*this));
}

const Value *OrEvaluator::Evaluate(const ScriptState *state)
{
	const Value *value1 = GetFirstSubnode()->Evaluate(state);
	if (value1)
	{
		const Value *value2 = GetLastSubnode()->Evaluate(state);
		if (value2)
		{
			ValueType type1 = value1->GetValueType();
			ValueType type2 = value2->GetValueType();

			if (type1 == kValueBoolean)
			{
				bool v1 = static_cast<const BooleanValue *>(value1)->GetValue();

				switch (type2)
				{
					case kValueBoolean:
						return (OutputBooleanValue(v1 | static_cast<const BooleanValue *>(value2)->GetValue()));
					case kValueInteger:
						return (OutputIntegerValue((int32) v1 | static_cast<const IntegerValue *>(value2)->GetValue()));
				}
			}
			else if (type1 == kValueInteger)
			{
				int32 v1 = static_cast<const IntegerValue *>(value1)->GetValue();

				switch (type2)
				{
					case kValueBoolean:
						return (OutputIntegerValue(v1 | (int32) static_cast<const BooleanValue *>(value2)->GetValue()));
					case kValueInteger:
						return (OutputIntegerValue(v1 | static_cast<const IntegerValue *>(value2)->GetValue()));
				}
			}
		}
	}

	return (nullptr);
}


XorEvaluator::XorEvaluator() : OperatorEvaluator(kEvaluatorXor)
{
}

XorEvaluator::XorEvaluator(const XorEvaluator& xorEvaluator) : OperatorEvaluator(xorEvaluator)
{
}

XorEvaluator::~XorEvaluator()
{
}

Evaluator *XorEvaluator::Replicate(void) const
{
	return (new XorEvaluator(*this));
}

const Value *XorEvaluator::Evaluate(const ScriptState *state)
{
	const Value *value1 = GetFirstSubnode()->Evaluate(state);
	if (value1)
	{
		const Value *value2 = GetLastSubnode()->Evaluate(state);
		if (value2)
		{
			ValueType type1 = value1->GetValueType();
			ValueType type2 = value2->GetValueType();

			if (type1 == kValueBoolean)
			{
				bool v1 = static_cast<const BooleanValue *>(value1)->GetValue();

				switch (type2)
				{
					case kValueBoolean:
						return (OutputBooleanValue(v1 ^ static_cast<const BooleanValue *>(value2)->GetValue()));
					case kValueInteger:
						return (OutputIntegerValue((int32) v1 ^ static_cast<const IntegerValue *>(value2)->GetValue()));
				}
			}
			else if (type1 == kValueInteger)
			{
				int32 v1 = static_cast<const IntegerValue *>(value1)->GetValue();

				switch (type2)
				{
					case kValueBoolean:
						return (OutputIntegerValue(v1 ^ (int32) static_cast<const BooleanValue *>(value2)->GetValue()));
					case kValueInteger:
						return (OutputIntegerValue(v1 ^ static_cast<const IntegerValue *>(value2)->GetValue()));
				}
			}
		}
	}

	return (nullptr);
}


ShiftLeftEvaluator::ShiftLeftEvaluator() : OperatorEvaluator(kEvaluatorShiftLeft)
{
}

ShiftLeftEvaluator::ShiftLeftEvaluator(const ShiftLeftEvaluator& shiftLeftEvaluator) : OperatorEvaluator(shiftLeftEvaluator)
{
}

ShiftLeftEvaluator::~ShiftLeftEvaluator()
{
}

Evaluator *ShiftLeftEvaluator::Replicate(void) const
{
	return (new ShiftLeftEvaluator(*this));
}

const Value *ShiftLeftEvaluator::Evaluate(const ScriptState *state)
{
	const Value *value1 = GetFirstSubnode()->Evaluate(state);
	if (value1)
	{
		const Value *value2 = GetLastSubnode()->Evaluate(state);
		if (value2)
		{
			ValueType type1 = value1->GetValueType();
			ValueType type2 = value2->GetValueType();

			if (type1 == kValueBoolean)
			{
				int32 v1 = (int32) static_cast<const BooleanValue *>(value1)->GetValue();

				switch (type2)
				{
					case kValueBoolean:
						return (OutputIntegerValue(v1 << (int32) static_cast<const BooleanValue *>(value2)->GetValue()));
					case kValueInteger:
						return (OutputIntegerValue(v1 << static_cast<const IntegerValue *>(value2)->GetValue()));
				}
			}
			else if (type1 == kValueInteger)
			{
				int32 v1 = static_cast<const IntegerValue *>(value1)->GetValue();

				switch (type2)
				{
					case kValueBoolean:
						return (OutputIntegerValue(v1 << (int32) static_cast<const BooleanValue *>(value2)->GetValue()));
					case kValueInteger:
						return (OutputIntegerValue(v1 << static_cast<const IntegerValue *>(value2)->GetValue()));
				}
			}
		}
	}

	return (nullptr);
}


ShiftRightEvaluator::ShiftRightEvaluator() : OperatorEvaluator(kEvaluatorShiftRight)
{
}

ShiftRightEvaluator::ShiftRightEvaluator(const ShiftRightEvaluator& shiftRightEvaluator) : OperatorEvaluator(shiftRightEvaluator)
{
}

ShiftRightEvaluator::~ShiftRightEvaluator()
{
}

Evaluator *ShiftRightEvaluator::Replicate(void) const
{
	return (new ShiftRightEvaluator(*this));
}

const Value *ShiftRightEvaluator::Evaluate(const ScriptState *state)
{
	const Value *value1 = GetFirstSubnode()->Evaluate(state);
	if (value1)
	{
		const Value *value2 = GetLastSubnode()->Evaluate(state);
		if (value2)
		{
			ValueType type1 = value1->GetValueType();
			ValueType type2 = value2->GetValueType();

			if (type1 == kValueBoolean)
			{
				int32 v1 = (int32) static_cast<const BooleanValue *>(value1)->GetValue();

				switch (type2)
				{
					case kValueBoolean:
						return (OutputIntegerValue(v1 >> (int32) static_cast<const BooleanValue *>(value2)->GetValue()));
					case kValueInteger:
						return (OutputIntegerValue(v1 >> static_cast<const IntegerValue *>(value2)->GetValue()));
				}
			}
			else if (type1 == kValueInteger)
			{
				int32 v1 = static_cast<const IntegerValue *>(value1)->GetValue();

				switch (type2)
				{
					case kValueBoolean:
						return (OutputIntegerValue(v1 >> (int32) static_cast<const BooleanValue *>(value2)->GetValue()));
					case kValueInteger:
						return (OutputIntegerValue(v1 >> static_cast<const IntegerValue *>(value2)->GetValue()));
				}
			}
		}
	}

	return (nullptr);
}


LessEvaluator::LessEvaluator() : OperatorEvaluator(kEvaluatorLess)
{
}

LessEvaluator::LessEvaluator(const LessEvaluator& lessEvaluator) : OperatorEvaluator(lessEvaluator)
{
}

LessEvaluator::~LessEvaluator()
{
}

Evaluator *LessEvaluator::Replicate(void) const
{
	return (new LessEvaluator(*this));
}

const Value *LessEvaluator::Evaluate(const ScriptState *state)
{
	const Value *value1 = GetFirstSubnode()->Evaluate(state);
	if (value1)
	{
		const Value *value2 = GetLastSubnode()->Evaluate(state);
		if (value2)
		{
			ValueType type1 = value1->GetValueType();
			ValueType type2 = value2->GetValueType();

			if ((type1 == kValueBoolean) || (type1 == kValueInteger))
			{
				int32 v1 = (type1 == kValueBoolean) ? (int32) static_cast<const BooleanValue *>(value1)->GetValue() : static_cast<const IntegerValue *>(value1)->GetValue();

				switch (type2)
				{
					case kValueBoolean:
						return (OutputBooleanValue(v1 < (int32) static_cast<const BooleanValue *>(value2)->GetValue()));
					case kValueInteger:
						return (OutputBooleanValue(v1 < static_cast<const IntegerValue *>(value2)->GetValue()));
					case kValueFloat:
						return (OutputBooleanValue((float) v1 < static_cast<const FloatValue *>(value2)->GetValue()));
				}
			}
			else if (type1 == kValueFloat)
			{
				float v1 = static_cast<const FloatValue *>(value1)->GetValue();

				switch (type2)
				{
					case kValueBoolean:
						return (OutputBooleanValue(v1 < (float) static_cast<const BooleanValue *>(value2)->GetValue()));
					case kValueInteger:
						return (OutputBooleanValue(v1 < (float) static_cast<const IntegerValue *>(value2)->GetValue()));
					case kValueFloat:
						return (OutputBooleanValue(v1 < static_cast<const FloatValue *>(value2)->GetValue()));
				}
			}
		}
	}

	return (nullptr);
}


GreaterEvaluator::GreaterEvaluator() : OperatorEvaluator(kEvaluatorGreater)
{
}

GreaterEvaluator::GreaterEvaluator(const GreaterEvaluator& greaterEvaluator) : OperatorEvaluator(greaterEvaluator)
{
}

GreaterEvaluator::~GreaterEvaluator()
{
}

Evaluator *GreaterEvaluator::Replicate(void) const
{
	return (new GreaterEvaluator(*this));
}

const Value *GreaterEvaluator::Evaluate(const ScriptState *state)
{
	const Value *value1 = GetFirstSubnode()->Evaluate(state);
	if (value1)
	{
		const Value *value2 = GetLastSubnode()->Evaluate(state);
		if (value2)
		{
			ValueType type1 = value1->GetValueType();
			ValueType type2 = value2->GetValueType();

			if ((type1 == kValueBoolean) || (type1 == kValueInteger))
			{
				int32 v1 = (type1 == kValueBoolean) ? (int32) static_cast<const BooleanValue *>(value1)->GetValue() : static_cast<const IntegerValue *>(value1)->GetValue();

				switch (type2)
				{
					case kValueBoolean:
						return (OutputBooleanValue(v1 > (int32) static_cast<const BooleanValue *>(value2)->GetValue()));
					case kValueInteger:
						return (OutputBooleanValue(v1 > static_cast<const IntegerValue *>(value2)->GetValue()));
					case kValueFloat:
						return (OutputBooleanValue((float) v1 > static_cast<const FloatValue *>(value2)->GetValue()));
				}
			}
			else if (type1 == kValueFloat)
			{
				float v1 = static_cast<const FloatValue *>(value1)->GetValue();

				switch (type2)
				{
					case kValueBoolean:
						return (OutputBooleanValue(v1 > (float) static_cast<const BooleanValue *>(value2)->GetValue()));
					case kValueInteger:
						return (OutputBooleanValue(v1 > (float) static_cast<const IntegerValue *>(value2)->GetValue()));
					case kValueFloat:
						return (OutputBooleanValue(v1 > static_cast<const FloatValue *>(value2)->GetValue()));
				}
			}
		}
	}

	return (nullptr);
}


LessEqualEvaluator::LessEqualEvaluator() : OperatorEvaluator(kEvaluatorLessEqual)
{
}

LessEqualEvaluator::LessEqualEvaluator(const LessEqualEvaluator& lessEqualEvaluator) : OperatorEvaluator(lessEqualEvaluator)
{
}

LessEqualEvaluator::~LessEqualEvaluator()
{
}

Evaluator *LessEqualEvaluator::Replicate(void) const
{
	return (new LessEqualEvaluator(*this));
}

const Value *LessEqualEvaluator::Evaluate(const ScriptState *state)
{
	const Value *value1 = GetFirstSubnode()->Evaluate(state);
	if (value1)
	{
		const Value *value2 = GetLastSubnode()->Evaluate(state);
		if (value2)
		{
			ValueType type1 = value1->GetValueType();
			ValueType type2 = value2->GetValueType();

			if ((type1 == kValueBoolean) || (type1 == kValueInteger))
			{
				int32 v1 = (type1 == kValueBoolean) ? (int32) static_cast<const BooleanValue *>(value1)->GetValue() : static_cast<const IntegerValue *>(value1)->GetValue();

				switch (type2)
				{
					case kValueBoolean:
						return (OutputBooleanValue(v1 <= (int32) static_cast<const BooleanValue *>(value2)->GetValue()));
					case kValueInteger:
						return (OutputBooleanValue(v1 <= static_cast<const IntegerValue *>(value2)->GetValue()));
					case kValueFloat:
						return (OutputBooleanValue((float) v1 <= static_cast<const FloatValue *>(value2)->GetValue()));
				}
			}
			else if (type1 == kValueFloat)
			{
				float v1 = static_cast<const FloatValue *>(value1)->GetValue();

				switch (type2)
				{
					case kValueBoolean:
						return (OutputBooleanValue(v1 <= (float) static_cast<const BooleanValue *>(value2)->GetValue()));
					case kValueInteger:
						return (OutputBooleanValue(v1 <= (float) static_cast<const IntegerValue *>(value2)->GetValue()));
					case kValueFloat:
						return (OutputBooleanValue(v1 <= static_cast<const FloatValue *>(value2)->GetValue()));
				}
			}
		}
	}

	return (nullptr);
}


GreaterEqualEvaluator::GreaterEqualEvaluator() : OperatorEvaluator(kEvaluatorGreaterEqual)
{
}

GreaterEqualEvaluator::GreaterEqualEvaluator(const GreaterEqualEvaluator& greaterEqualEvaluator) : OperatorEvaluator(greaterEqualEvaluator)
{
}

GreaterEqualEvaluator::~GreaterEqualEvaluator()
{
}

Evaluator *GreaterEqualEvaluator::Replicate(void) const
{
	return (new GreaterEqualEvaluator(*this));
}

const Value *GreaterEqualEvaluator::Evaluate(const ScriptState *state)
{
	const Value *value1 = GetFirstSubnode()->Evaluate(state);
	if (value1)
	{
		const Value *value2 = GetLastSubnode()->Evaluate(state);
		if (value2)
		{
			ValueType type1 = value1->GetValueType();
			ValueType type2 = value2->GetValueType();

			if ((type1 == kValueBoolean) || (type1 == kValueInteger))
			{
				int32 v1 = (type1 == kValueBoolean) ? (int32) static_cast<const BooleanValue *>(value1)->GetValue() : static_cast<const IntegerValue *>(value1)->GetValue();

				switch (type2)
				{
					case kValueBoolean:
						return (OutputBooleanValue(v1 >= (int32) static_cast<const BooleanValue *>(value2)->GetValue()));
					case kValueInteger:
						return (OutputBooleanValue(v1 >= static_cast<const IntegerValue *>(value2)->GetValue()));
					case kValueFloat:
						return (OutputBooleanValue((float) v1 >= static_cast<const FloatValue *>(value2)->GetValue()));
				}
			}
			else if (type1 == kValueFloat)
			{
				float v1 = static_cast<const FloatValue *>(value1)->GetValue();

				switch (type2)
				{
					case kValueBoolean:
						return (OutputBooleanValue(v1 >= (float) static_cast<const BooleanValue *>(value2)->GetValue()));
					case kValueInteger:
						return (OutputBooleanValue(v1 >= (float) static_cast<const IntegerValue *>(value2)->GetValue()));
					case kValueFloat:
						return (OutputBooleanValue(v1 >= static_cast<const FloatValue *>(value2)->GetValue()));
				}
			}
		}
	}

	return (nullptr);
}


EqualEvaluator::EqualEvaluator() : OperatorEvaluator(kEvaluatorEqual)
{
}

EqualEvaluator::EqualEvaluator(const EqualEvaluator& equalEvaluator) : OperatorEvaluator(equalEvaluator)
{
}

EqualEvaluator::~EqualEvaluator()
{
}

Evaluator *EqualEvaluator::Replicate(void) const
{
	return (new EqualEvaluator(*this));
}

const Value *EqualEvaluator::Evaluate(const ScriptState *state)
{
	const Value *value1 = GetFirstSubnode()->Evaluate(state);
	if (value1)
	{
		const Value *value2 = GetLastSubnode()->Evaluate(state);
		if (value2)
		{
			ValueType type1 = value1->GetValueType();
			ValueType type2 = value2->GetValueType();

			if ((type1 == kValueBoolean) || (type1 == kValueInteger))
			{
				int32 v1 = (type1 == kValueBoolean) ? (int32) static_cast<const BooleanValue *>(value1)->GetValue() : static_cast<const IntegerValue *>(value1)->GetValue();

				switch (type2)
				{
					case kValueBoolean:
						return (OutputBooleanValue(v1 == (int32) static_cast<const BooleanValue *>(value2)->GetValue()));
					case kValueInteger:
						return (OutputBooleanValue(v1 == static_cast<const IntegerValue *>(value2)->GetValue()));
					case kValueFloat:
						return (OutputBooleanValue((float) v1 == static_cast<const FloatValue *>(value2)->GetValue()));
					case kValueString:
						return (OutputBooleanValue(Text::CompareText(Text::IntegerToString(v1), static_cast<const StringValue *>(value2)->GetValue())));
				}
			}
			else if (type1 == kValueFloat)
			{
				float v1 = static_cast<const FloatValue *>(value1)->GetValue();

				switch (type2)
				{
					case kValueBoolean:
						return (OutputBooleanValue(v1 == (float) static_cast<const BooleanValue *>(value2)->GetValue()));
					case kValueInteger:
						return (OutputBooleanValue(v1 == (float) static_cast<const IntegerValue *>(value2)->GetValue()));
					case kValueFloat:
						return (OutputBooleanValue(v1 == static_cast<const FloatValue *>(value2)->GetValue()));
					case kValueString:
						return (OutputBooleanValue(Text::CompareText(Text::FloatToString(v1), static_cast<const StringValue *>(value2)->GetValue())));
				}
			}
			else if (type1 == kValueString)
			{
				const char *v1 = static_cast<const StringValue *>(value1)->GetValue();

				switch (type2)
				{
					case kValueInteger:
						return (OutputBooleanValue(Text::CompareText(v1, Text::IntegerToString(static_cast<const IntegerValue *>(value2)->GetValue()))));
					case kValueFloat:
						return (OutputBooleanValue(Text::CompareText(v1, Text::FloatToString(static_cast<const FloatValue *>(value2)->GetValue()))));
					case kValueString:
						return (OutputBooleanValue(Text::CompareText(v1, static_cast<const StringValue *>(value2)->GetValue())));
				}
			}
			else if (type1 == kValueColor)
			{
				if (type2 == kValueColor)
				{
					const ColorRGBA& v1 = static_cast<const ColorValue *>(value1)->GetValue();
					return (OutputBooleanValue(v1 == static_cast<const ColorValue *>(value2)->GetValue()));
				}
			}
		}
	}

	return (nullptr);
}


NotEqualEvaluator::NotEqualEvaluator() : OperatorEvaluator(kEvaluatorNotEqual)
{
}

NotEqualEvaluator::NotEqualEvaluator(const NotEqualEvaluator& notEqualEvaluator) : OperatorEvaluator(notEqualEvaluator)
{
}

NotEqualEvaluator::~NotEqualEvaluator()
{
}

Evaluator *NotEqualEvaluator::Replicate(void) const
{
	return (new NotEqualEvaluator(*this));
}

const Value *NotEqualEvaluator::Evaluate(const ScriptState *state)
{
	const Value *value1 = GetFirstSubnode()->Evaluate(state);
	if (value1)
	{
		const Value *value2 = GetLastSubnode()->Evaluate(state);
		if (value2)
		{
			ValueType type1 = value1->GetValueType();
			ValueType type2 = value2->GetValueType();

			if ((type1 == kValueBoolean) || (type1 == kValueInteger))
			{
				int32 v1 = (type1 == kValueBoolean) ? (int32) static_cast<const BooleanValue *>(value1)->GetValue() : static_cast<const IntegerValue *>(value1)->GetValue();

				switch (type2)
				{
					case kValueBoolean:
						return (OutputBooleanValue(v1 != (int32) static_cast<const BooleanValue *>(value2)->GetValue()));
					case kValueInteger:
						return (OutputBooleanValue(v1 != static_cast<const IntegerValue *>(value2)->GetValue()));
					case kValueFloat:
						return (OutputBooleanValue((float) v1 != static_cast<const FloatValue *>(value2)->GetValue()));
					case kValueString:
						return (OutputBooleanValue(!Text::CompareText(Text::IntegerToString(v1), static_cast<const StringValue *>(value2)->GetValue())));
				}
			}
			else if (type1 == kValueFloat)
			{
				float v1 = static_cast<const FloatValue *>(value1)->GetValue();

				switch (type2)
				{
					case kValueBoolean:
						return (OutputBooleanValue(v1 != (float) static_cast<const BooleanValue *>(value2)->GetValue()));
					case kValueInteger:
						return (OutputBooleanValue(v1 != (float) static_cast<const IntegerValue *>(value2)->GetValue()));
					case kValueFloat:
						return (OutputBooleanValue(v1 != static_cast<const FloatValue *>(value2)->GetValue()));
					case kValueString:
						return (OutputBooleanValue(!Text::CompareText(Text::FloatToString(v1), static_cast<const StringValue *>(value2)->GetValue())));
				}
			}
			else if (type1 == kValueString)
			{
				const char *v1 = static_cast<const StringValue *>(value1)->GetValue();

				switch (type2)
				{
					case kValueInteger:
						return (OutputBooleanValue(!Text::CompareText(v1, Text::IntegerToString(static_cast<const IntegerValue *>(value2)->GetValue()))));
					case kValueFloat:
						return (OutputBooleanValue(!Text::CompareText(v1, Text::FloatToString(static_cast<const FloatValue *>(value2)->GetValue()))));
					case kValueString:
						return (OutputBooleanValue(!Text::CompareText(v1, static_cast<const StringValue *>(value2)->GetValue())));
				}
			}
			else if (type1 == kValueColor)
			{
				if (type2 == kValueColor)
				{
					const ColorRGBA& v1 = static_cast<const ColorValue *>(value1)->GetValue();
					return (OutputBooleanValue(v1 != static_cast<const ColorValue *>(value2)->GetValue()));
				}
			}
		}
	}

	return (nullptr);
}


ExpressionMethod::ExpressionMethod() : Method(kMethodExpression)
{
	evaluatorRoot = nullptr;
}

ExpressionMethod::ExpressionMethod(const ExpressionMethod& expressionMethod) : Method(expressionMethod)
{
	expressionText = expressionMethod.expressionText;

	const Evaluator *evaluator = expressionMethod.evaluatorRoot;
	evaluatorRoot = (evaluator) ? evaluator->Clone() : nullptr;
}

ExpressionMethod::~ExpressionMethod()
{
	delete evaluatorRoot;
}

Method *ExpressionMethod::Replicate(void) const
{
	return (new ExpressionMethod(*this));
}

void ExpressionMethod::Pack(Packer& data, unsigned_int32 packFlags) const
{
	Method::Pack(data, packFlags);

	if ((packFlags & kPackEditor) && (expressionText.Length() > 0))
	{
		PackHandle handle = data.BeginChunk('TEXT');
		data << expressionText;
		data.EndChunk(handle);
	}

	int32 evaluatorCount = 0;
	const Evaluator *evaluator = evaluatorRoot;
	while (evaluator)
	{
		evaluator->evaluatorIndex = evaluatorCount++;
		evaluator = evaluatorRoot->GetNextNode(evaluator);
	}

	PackHandle handle = data.BeginChunk('TREE');
	data << evaluatorCount;

	evaluator = evaluatorRoot;
	while (evaluator)
	{
		PackHandle section = data.BeginSection();
		evaluator->PackType(data);
		evaluator->Pack(data, packFlags);
		data.EndSection(section);

		evaluator = evaluatorRoot->GetNextNode(evaluator);
	}

	data.EndChunk(handle);
	data << TerminatorChunk;
}

void ExpressionMethod::Unpack(Unpacker& data, unsigned_int32 unpackFlags)
{
	Method::Unpack(data, unpackFlags);
	UnpackChunkList<ExpressionMethod>(data, unpackFlags);
}

bool ExpressionMethod::UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags)
{
	switch (chunkHeader->chunkType)
	{
		case 'TEXT':

			if (unpackFlags & kUnpackEditor)
			{
				data >> expressionText;
				return (true);
			}

			break;

		case 'TREE':
		{
			int32	evaluatorCount;

			data >> evaluatorCount;
			if (evaluatorCount != 0)
			{
				Evaluator **evaluatorTable = new Evaluator *[evaluatorCount];

				for (machine a = 0; a < evaluatorCount; a++)
				{
					unsigned_int32	size;

					data >> size;
					const void *mark = data.GetPointer();

					Evaluator *evaluator = Evaluator::Create(data, unpackFlags);
					if (evaluator)
					{
						evaluator->Unpack(++data, unpackFlags);
						evaluatorTable[a] = evaluator;
					}
					else
					{
						data.Skip(mark, size);
						evaluatorTable[a] = nullptr;
					}
				}

				for (machine a = 0; a < evaluatorCount; a++)
				{
					Evaluator *evaluator = evaluatorTable[a];
					if (evaluator)
					{
						int32 superIndex = evaluator->superIndex;
						if (superIndex >= 0)
						{
							Evaluator *super = evaluatorTable[superIndex];
							if (super)
							{
								super->AppendSubnode(evaluator);
							}
							else
							{
								delete evaluator;
								evaluatorTable[a] = nullptr;
							}
						}
						else
						{
							evaluatorRoot = evaluator;
						}
					}
				}

				delete[] evaluatorTable;
			}

			return (true);
		}
	}

	return (false);
}

void *ExpressionMethod::BeginSettingsUnpack(void)
{
	delete evaluatorRoot;
	evaluatorRoot = nullptr;

	expressionText.Purge();

	return (Method::BeginSettingsUnpack());
}

bool ExpressionMethod::TokenizeText(const char *text, List<Token> *tokenList)
{
	for (;;)
	{
		text += Data::GetWhitespaceLength(text);
		if (text[0] == 0)
		{
			return (true);
		}

		Token *token = nullptr;

		int32 c = *text;
		if (((unsigned_int32) (c - '0') < 10U) || (c == 39))
		{
			bool integer = true;
			for (const char *s = text;; s++)
			{
				int32 d = *s;
				if (d >= 33)
				{
					if (((unsigned_int32) (d - '0') < 10U) || (d == '_'))
					{
						continue;
					}

					if ((d == '.') || (d == 'e') || (d == 'E'))
					{
						integer = false;
					}
				}

				break;
			}

			if (integer)
			{
				int32	value;

				if (Int32DataType::ParseValue(text, &value) == kDataOkay)
				{
					token = new IntegerToken(value);
				}
			}
			else
			{
				float	value;

				if (FloatDataType::ParseValue(text, &value) == kDataOkay)
				{
					token = new FloatToken(value);
				}
			}
		}
		else if (((unsigned_int32) (c - 'A') < 26U) || ((unsigned_int32) (c - 'a') < 26U) || (c == '_'))
		{
			bool	value;
			int32	textLength;

			if (BoolDataType::ParseValue(text, &value) == kDataOkay)
			{
				token = new BooleanToken(value);
			}
			else if (Data::ReadIdentifier(text, &textLength) == kDataOkay)
			{
				String<>	string;

				string.SetLength(textLength);
				Data::ReadIdentifier(text, &textLength, string);
				text += textLength;

				token = new IdentifierToken(string);
			}
		}
		else if (c == 34)
		{
			String<>	string;

			if (StringDataType::ParseValue(text, &string) == kDataOkay)
			{
				token = new StringToken(string);
			}
		}
		else if (c == '(')
		{
			text++;
			token = new Token(kTokenLeftParen);
		}
		else if (c == ')')
		{
			text++;
			token = new Token(kTokenRightParen);
		}
		else if (c == '[')
		{
			text++;
			token = new Token(kTokenLeftBracket);
		}
		else if (c == ']')
		{
			text++;
			token = new Token(kTokenRightBracket);
		}
		else if (c == '~')
		{
			text++;
			token = new Token(kTokenBitwiseNot);
		}
		else if (c == '*')
		{
			text++;
			token = new Token(kTokenMultiply);
		}
		else if (c == '/')
		{
			text++;
			token = new Token(kTokenDivide);
		}
		else if (c == '%')
		{
			text++;
			token = new Token(kTokenModulo);
		}
		else if (c == '+')
		{
			text++;
			token = new Token(kTokenPlus);
		}
		else if (c == '-')
		{
			text++;
			token = new Token(kTokenMinus);
		}
		else if (c == '<')
		{
			int32 d = text[1];
			if (d == '<')
			{
				text += 2;
				token = new Token(kTokenShiftLeft);
			}
			else if (d == '=')
			{
				text += 2;
				token = new Token(kTokenLessEqual);
			}
			else
			{
				text++;
				token = new Token(kTokenLess);
			}
		}
		else if (c == '>')
		{
			int32 d = text[1];
			if (d == '>')
			{
				text += 2;
				token = new Token(kTokenShiftRight);
			}
			else if (d == '=')
			{
				text += 2;
				token = new Token(kTokenGreaterEqual);
			}
			else
			{
				text++;
				token = new Token(kTokenGreater);
			}
		}
		else if (c == '=')
		{
			if (text[1] == '=')
			{
				text += 2;
				token = new Token(kTokenEqual);
			}
		}
		else if (c == '!')
		{
			if (text[1] == '=')
			{
				text += 2;
				token = new Token(kTokenNotEqual);
			}
		}
		else if (c == '&')
		{
			text++;
			token = new Token(kTokenBitwiseAnd);
		}
		else if (c == '^')
		{
			text++;
			token = new Token(kTokenBitwiseXor);
		}
		else if (c == '|')
		{
			text++;
			token = new Token(kTokenBitwiseOr);
		}
		else if (c == '.')
		{
			text++;
			token = new Token(kTokenDot);
		}

		if (!token)
		{
			break;
		}

		tokenList->Append(token);
	}

	return (false);
}

Evaluator *ExpressionMethod::ParsePrimaryExpression(const Token *& token)
{
	Evaluator *evaluator = nullptr;

	switch (token->GetTokenType())
	{
		case kTokenIdentifier:
		{
			const Token *nextToken = token->Next();
			if (nextToken)
			{
				TokenType nextType = nextToken->GetTokenType();
				if (nextType == kTokenDot)
				{
					nextToken = nextToken->Next();
					if ((nextToken) && (nextToken->GetTokenType() == kTokenIdentifier))
					{
						const char *subname = static_cast<const IdentifierToken *>(nextToken)->GetIdentifier();
						unsigned_int32 c = subname[0];
						if ((c - 'a' < 26U) && (subname[1] == 0))
						{
							evaluator = new MemberEvaluator(static_cast<const IdentifierToken *>(token)->GetIdentifier());
							evaluator->AppendSubnode(new IntegerEvaluator(c));
							token = nextToken->Next();
						}
					}

					break;
				}
				else if (nextType == kTokenLeftBracket)
				{
					nextToken = nextToken->Next();
					if (nextToken)
					{
						Evaluator *subevaluator = ParseExpression(nextToken);
						if (subevaluator)
						{
							if ((nextToken) && (nextToken->GetTokenType() == kTokenRightBracket))
							{
								evaluator = new ArrayEvaluator(static_cast<const IdentifierToken *>(token)->GetIdentifier());
								evaluator->AppendSubnode(subevaluator);
								token = nextToken->Next();
							}
							else
							{
								delete subevaluator;
							}
						}
					}

					break;
				}
			}

			evaluator = new ValueEvaluator(static_cast<const IdentifierToken *>(token)->GetIdentifier());
			token = nextToken;
			break;
		}

		case kTokenBoolean:
		{
			evaluator = new BooleanEvaluator(static_cast<const BooleanToken *>(token)->GetBooleanValue());
			token = token->Next();
			break;
		}

		case kTokenInteger:
		{
			evaluator = new IntegerEvaluator(static_cast<const IntegerToken *>(token)->GetIntegerValue());
			token = token->Next();
			break;
		}

		case kTokenFloat:
		{
			evaluator = new FloatEvaluator(static_cast<const FloatToken *>(token)->GetFloatValue());
			token = token->Next();
			break;
		}

		case kTokenString:
		{
			evaluator = new StringEvaluator(static_cast<const StringToken *>(token)->GetStringValue());
			token = token->Next();
			break;
		}

		case kTokenLeftParen:
		{
			const Token *nextToken = token->Next();
			if (nextToken)
			{
				evaluator = ParseExpression(nextToken);
				if (evaluator)
				{
					if ((nextToken) && (nextToken->GetTokenType() == kTokenRightParen))
					{
						token = nextToken->Next();
					}
					else
					{
						delete evaluator;
						evaluator = nullptr;
					}
				}
			}

			break;
		}
	}

	return (evaluator);
}

Evaluator *ExpressionMethod::ParseUnaryExpression(const Token *& token)
{
	Evaluator *evaluator = ParsePrimaryExpression(token);
	if (!evaluator)
	{
		const Token *nextToken = token->Next();
		if (nextToken)
		{
			C4::TokenType type = token->GetTokenType();
			if (type == kTokenPlus)
			{
				evaluator = ParsePrimaryExpression(nextToken);
				if (evaluator)
				{
					token = nextToken;
				}
			}
			else if ((type == kTokenMinus) || (type == kTokenBitwiseNot))
			{
				Evaluator *subevaluator = ParsePrimaryExpression(nextToken);
				if (subevaluator)
				{
					if (type == kTokenMinus)
					{
						evaluator = new NegateEvaluator;
					}
					else
					{
						evaluator = new InvertEvaluator;
					}

					evaluator->AppendSubnode(subevaluator);
					token = nextToken;
				}
			}
		}
	}

	return (evaluator);
}

Evaluator *ExpressionMethod::ParseMultiplicativeExpression(const Token *& token, Evaluator *leftEvaluator)
{
	if (!leftEvaluator)
	{
		leftEvaluator = ParseUnaryExpression(token);
	}

	if ((leftEvaluator) && (token))
	{
		const Token *nextToken = token->Next();
		if (nextToken)
		{
			C4::TokenType type = token->GetTokenType();
			if ((type == kTokenMultiply) || (type == kTokenDivide) || (type == kTokenModulo))
			{
				Evaluator *rightEvaluator = ParseUnaryExpression(nextToken);
				if (rightEvaluator)
				{
					Evaluator	*evaluator;

					if (type == kTokenMultiply)
					{
						evaluator = new MultiplyEvaluator;
					}
					else if (type == kTokenDivide)
					{
						evaluator = new DivideEvaluator;
					}
					else
					{
						evaluator = new ModuloEvaluator;
					}

					evaluator->AppendSubnode(leftEvaluator);
					evaluator->AppendSubnode(rightEvaluator);

					token = nextToken;
					if (token)
					{
						return (ParseMultiplicativeExpression(token, evaluator));
					}

					return (evaluator);
				}
			}
		}
	}

	return (leftEvaluator);
}

Evaluator *ExpressionMethod::ParseAdditiveExpression(const Token *& token, Evaluator *leftEvaluator)
{
	if (!leftEvaluator)
	{
		leftEvaluator = ParseMultiplicativeExpression(token);
	}

	if ((leftEvaluator) && (token))
	{
		const Token *nextToken = token->Next();
		if (nextToken)
		{
			C4::TokenType type = token->GetTokenType();
			if ((type == kTokenPlus) || (type == kTokenMinus))
			{
				Evaluator *rightEvaluator = ParseMultiplicativeExpression(nextToken);
				if (rightEvaluator)
				{
					Evaluator	*evaluator;

					if (type == kTokenPlus)
					{
						evaluator = new AddEvaluator;
					}
					else
					{
						evaluator = new SubtractEvaluator;
					}

					evaluator->AppendSubnode(leftEvaluator);
					evaluator->AppendSubnode(rightEvaluator);

					token = nextToken;
					if (token)
					{
						return (ParseAdditiveExpression(token, evaluator));
					}

					return (evaluator);
				}
			}
		}
	}

	return (leftEvaluator);
}

Evaluator *ExpressionMethod::ParseShiftExpression(const Token *& token, Evaluator *leftEvaluator)
{
	if (!leftEvaluator)
	{
		leftEvaluator = ParseAdditiveExpression(token);
	}

	if ((leftEvaluator) && (token))
	{
		const Token *nextToken = token->Next();
		if (nextToken)
		{
			C4::TokenType type = token->GetTokenType();
			if ((type == kTokenShiftLeft) || (type == kTokenShiftRight))
			{
				Evaluator *rightEvaluator = ParseAdditiveExpression(nextToken);
				if (rightEvaluator)
				{
					Evaluator	*evaluator;

					if (type == kTokenShiftLeft)
					{
						evaluator = new ShiftLeftEvaluator;
					}
					else
					{
						evaluator = new ShiftRightEvaluator;
					}

					evaluator->AppendSubnode(leftEvaluator);
					evaluator->AppendSubnode(rightEvaluator);

					token = nextToken;
					if (token)
					{
						return (ParseShiftExpression(token, evaluator));
					}

					return (evaluator);
				}
			}
		}
	}

	return (leftEvaluator);
}

Evaluator *ExpressionMethod::ParseRelationalExpression(const Token *& token, Evaluator *leftEvaluator)
{
	if (!leftEvaluator)
	{
		leftEvaluator = ParseShiftExpression(token);
	}

	if ((leftEvaluator) && (token))
	{
		const Token *nextToken = token->Next();
		if (nextToken)
		{
			C4::TokenType type = token->GetTokenType();
			if ((type == kTokenLess) || (type == kTokenGreater) || (type == kTokenLessEqual) || (type == kTokenGreaterEqual))
			{
				Evaluator *rightEvaluator = ParseShiftExpression(nextToken);
				if (rightEvaluator)
				{
					Evaluator	*evaluator;

					if (type == kTokenLess)
					{
						evaluator = new LessEvaluator;
					}
					else if (type == kTokenGreater)
					{
						evaluator = new GreaterEvaluator;
					}
					else if (type == kTokenLessEqual)
					{
						evaluator = new LessEqualEvaluator;
					}
					else
					{
						evaluator = new GreaterEqualEvaluator;
					}

					evaluator->AppendSubnode(leftEvaluator);
					evaluator->AppendSubnode(rightEvaluator);

					token = nextToken;
					if (token)
					{
						return (ParseRelationalExpression(token, evaluator));
					}

					return (evaluator);
				}
			}
		}
	}

	return (leftEvaluator);
}

Evaluator *ExpressionMethod::ParseEqualityExpression(const Token *& token, Evaluator *leftEvaluator)
{
	if (!leftEvaluator)
	{
		leftEvaluator = ParseRelationalExpression(token);
	}

	if ((leftEvaluator) && (token))
	{
		const Token *nextToken = token->Next();
		if (nextToken)
		{
			C4::TokenType type = token->GetTokenType();
			if ((type == kTokenEqual) || (type == kTokenNotEqual))
			{
				Evaluator *rightEvaluator = ParseRelationalExpression(nextToken);
				if (rightEvaluator)
				{
					Evaluator	*evaluator;

					if (type == kTokenEqual)
					{
						evaluator = new EqualEvaluator;
					}
					else
					{
						evaluator = new NotEqualEvaluator;
					}

					evaluator->AppendSubnode(leftEvaluator);
					evaluator->AppendSubnode(rightEvaluator);

					token = nextToken;
					if (token)
					{
						return (ParseEqualityExpression(token, evaluator));
					}

					return (evaluator);
				}
			}
		}
	}

	return (leftEvaluator);
}

Evaluator *ExpressionMethod::ParseAndExpression(const Token *& token, Evaluator *leftEvaluator)
{
	if (!leftEvaluator)
	{
		leftEvaluator = ParseEqualityExpression(token);
	}

	if ((leftEvaluator) && (token))
	{
		const Token *nextToken = token->Next();
		if (nextToken)
		{
			C4::TokenType type = token->GetTokenType();
			if (type == kTokenBitwiseAnd)
			{
				Evaluator *rightEvaluator = ParseEqualityExpression(nextToken);
				if (rightEvaluator)
				{
					Evaluator *evaluator = new AndEvaluator;
					evaluator->AppendSubnode(leftEvaluator);
					evaluator->AppendSubnode(rightEvaluator);

					token = nextToken;
					if (token)
					{
						return (ParseAndExpression(token, evaluator));
					}

					return (evaluator);
				}
			}
		}
	}

	return (leftEvaluator);
}

Evaluator *ExpressionMethod::ParseXorExpression(const Token *& token, Evaluator *leftEvaluator)
{
	if (!leftEvaluator)
	{
		leftEvaluator = ParseAndExpression(token);
	}

	if ((leftEvaluator) && (token))
	{
		const Token *nextToken = token->Next();
		if (nextToken)
		{
			C4::TokenType type = token->GetTokenType();
			if (type == kTokenBitwiseXor)
			{
				Evaluator *rightEvaluator = ParseAndExpression(nextToken);
				if (rightEvaluator)
				{
					Evaluator *evaluator = new XorEvaluator;
					evaluator->AppendSubnode(leftEvaluator);
					evaluator->AppendSubnode(rightEvaluator);

					token = nextToken;
					if (token)
					{
						return (ParseXorExpression(token, evaluator));
					}

					return (evaluator);
				}
			}
		}
	}

	return (leftEvaluator);
}

Evaluator *ExpressionMethod::ParseOrExpression(const Token *& token, Evaluator *leftEvaluator)
{
	if (!leftEvaluator)
	{
		leftEvaluator = ParseXorExpression(token);
	}

	if ((leftEvaluator) && (token))
	{
		const Token *nextToken = token->Next();
		if (nextToken)
		{
			C4::TokenType type = token->GetTokenType();
			if (type == kTokenBitwiseOr)
			{
				Evaluator *rightEvaluator = ParseXorExpression(nextToken);
				if (rightEvaluator)
				{
					Evaluator *evaluator = new OrEvaluator;
					evaluator->AppendSubnode(leftEvaluator);
					evaluator->AppendSubnode(rightEvaluator);

					token = nextToken;
					if (token)
					{
						return (ParseOrExpression(token, evaluator));
					}

					return (evaluator);
				}
			}
		}
	}

	return (leftEvaluator);
}

void ExpressionMethod::SetExpressionText(const char *text)
{
	List<Token>		tokenList;

	delete evaluatorRoot;
	evaluatorRoot = nullptr;

	if (text[0] != 0)
	{
		expressionText = text;
	}
	else
	{
		expressionText.Purge();
	}

	if (TokenizeText(text, &tokenList))
	{
		const Token *token = tokenList.First();
		if (token)
		{
			evaluatorRoot = ParseExpression(token);
			if (token)
			{
				delete evaluatorRoot;
				evaluatorRoot = nullptr;
			}
		}
	}
}

void ExpressionMethod::Execute(const ScriptState *state)
{
	if (evaluatorRoot)
	{
		const Value *value = evaluatorRoot->Evaluate(state);
		if (value)
		{
			SetMethodResult(value->GetBooleanValue());

			Value *output = state->GetValue(GetOutputValueName());
			if (output)
			{
				output->SetValue(value);
			}
		}
	}

	CallCompletionProc();
}

// ZYUQURM
