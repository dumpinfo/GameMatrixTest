 

#include "C4Values.h"


using namespace C4;


Value::Value(ValueType type)
{
	valueType = type;
	valueScope = kValueScopeScript;
	valueName[0] = 0;
}

Value::Value(const Value& value)
{
	valueType = value.valueType;
	valueScope = value.valueScope;
	valueName = value.valueName;
}

Value::~Value()
{
}

Value *Value::Create(Unpacker& data)
{
	switch (data.GetType())
	{
		case kValueBoolean:

			return (new BooleanValue);

		case kValueInteger:

			return (new IntegerValue);

		case kValueFloat:

			return (new FloatValue);

		case kValueString:

			return (new StringValue);

		case kValueColor:

			return (new ColorValue);

		case kValueVector:

			return (new VectorValue);

		case kValuePoint:

			return (new PointValue);
	}

	return (nullptr);
}

Value *Value::New(ValueType type)
{
	Type	data[2];

	data[0] = type;
	data[1] = 0;

	Unpacker unpacker(data);
	return (Create(unpacker));
}

void Value::PackType(Packer& data) const
{
	data << valueType;
}

void Value::Pack(Packer& data, unsigned_int32 packFlags) const
{
	data << ChunkHeader('SCOP', 4);
	data << valueScope;

	PackHandle handle = data.BeginChunk('NAME');
	data << valueName;
	data.EndChunk(handle);

	data << TerminatorChunk;
}

void Value::Unpack(Unpacker& data, unsigned_int32 unpackFlags)
{
	UnpackChunkList<Value>(data, unpackFlags);
}

bool Value::UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags)
{
	switch (chunkHeader->chunkType)
	{
		case 'SCOP':

			data >> valueScope;
			return (true); 

		case 'NAME':
 
			data >> valueName;
			return (true); 
	}

	return (false); 
}
 
int32 Value::GetSettingCount(void) const 
{
	return (3);
}
 
Setting *Value::GetSetting(int32 index) const
{
	const StringTable *table = TheInterfaceMgr->GetStringTable();

	if (index == 0)
	{
		const char *title = table->GetString(StringID('VALU', 'NAME'));
		return (new TextSetting('NAME', valueName, title, kMaxValueNameLength, &EditTextWidget::IdentifierFilter));
	}

	if (index == 1)
	{
		int32 selection = 0;
		if (valueType == kValueInteger)
		{
			selection = 1;
		}
		else if (valueType == kValueFloat)
		{
			selection = 2;
		}
		else if (valueType == kValueString)
		{
			selection = 3;
		}
		else if (valueType == kValueColor)
		{
			selection = 4;
		}
		else if (valueType == kValueVector)
		{
			selection = 5;
		}
		else if (valueType == kValuePoint)
		{
			selection = 6;
		}

		const char *title = table->GetString(StringID('VALU', 'TYPE'));
		MenuSetting *menu = new MenuSetting('TYPE', selection, title, 7);

		menu->SetMenuItemString(0, table->GetString(StringID('VALU', 'TYPE', kValueBoolean)));
		menu->SetMenuItemString(1, table->GetString(StringID('VALU', 'TYPE', kValueInteger)));
		menu->SetMenuItemString(2, table->GetString(StringID('VALU', 'TYPE', kValueFloat)));
		menu->SetMenuItemString(3, table->GetString(StringID('VALU', 'TYPE', kValueString)));
		menu->SetMenuItemString(4, table->GetString(StringID('VALU', 'TYPE', kValueColor)));
		menu->SetMenuItemString(5, table->GetString(StringID('VALU', 'TYPE', kValueVector)));
		menu->SetMenuItemString(6, table->GetString(StringID('VALU', 'TYPE', kValuePoint)));

		return (menu);
	}

	if (index == 2)
	{
		int32 selection = 0;
		if (valueScope == kValueScopeController)
		{
			selection = 1;
		}
		else if (valueScope == kValueScopeObject)
		{
			selection = 2;
		}

		const char *title = table->GetString(StringID('VALU', 'SCOP'));
		MenuSetting *menu = new MenuSetting('SCOP', selection, title, 3);

		menu->SetMenuItemString(0, table->GetString(StringID('VALU', 'SCOP', kValueScopeScript)));
		menu->SetMenuItemString(1, table->GetString(StringID('VALU', 'SCOP', kValueScopeController)));
		menu->SetMenuItemString(2, table->GetString(StringID('VALU', 'SCOP', kValueScopeObject)));

		return (menu);
	}

	return (nullptr);
}

void Value::SetSetting(const Setting *setting)
{
	Type identifier = setting->GetSettingIdentifier();

	if (identifier == 'NAME')
	{
		valueName = static_cast<const TextSetting *>(setting)->GetText();
	}
	else if (identifier == 'TYPE')
	{
		static const ValueType type[7] =
		{
			kValueBoolean, kValueInteger, kValueFloat, kValueString, kValueColor, kValueVector, kValuePoint
		};

		int32 selection = static_cast<const MenuSetting *>(setting)->GetMenuSelection();
		valueType = type[selection];
	}
	else if (identifier == 'SCOP')
	{
		int32 selection = static_cast<const MenuSetting *>(setting)->GetMenuSelection();
		if (selection == 0)
		{
			valueScope = kValueScopeScript;
		}
		else if (selection == 1)
		{
			valueScope = kValueScopeController;
		}
		else
		{
			valueScope = kValueScopeObject;
		}
	}
}

bool Value::SetValue(bool v)
{
	return (false);
}

bool Value::SetValue(int32 v)
{
	return (false);
}

bool Value::SetValue(float v)
{
	return (false);
}

bool Value::SetValue(const char *v)
{
	return (false);
}

bool Value::SetValue(const ColorRGBA& v)
{
	return (false);
}

bool Value::SetValue(const Vector3D& v)
{
	return (false);
}

bool Value::SetValue(const Value *v)
{
	switch (v->GetValueType())
	{
		case kValueBoolean:

			return (SetValue(static_cast<const BooleanValue *>(v)->GetValue()));

		case kValueInteger:

			return (SetValue(static_cast<const IntegerValue *>(v)->GetValue()));

		case kValueFloat:

			return (SetValue(static_cast<const FloatValue *>(v)->GetValue()));

		case kValueString:

			return (SetValue(static_cast<const StringValue *>(v)->GetValue()));

		case kValueColor:

			return (SetValue(static_cast<const ColorValue *>(v)->GetValue()));

		case kValueVector:
		case kValuePoint:

			return (SetValue(static_cast<const VectorValue *>(v)->GetValue()));
	}

	return (false);
}


BooleanValue::BooleanValue(bool v) : Value(kValueBoolean)
{
	value = v;
}

BooleanValue::BooleanValue(const BooleanValue& booleanValue) : Value(booleanValue)
{
	value = booleanValue.value;
}

BooleanValue::~BooleanValue()
{
}

Value *BooleanValue::Replicate(void) const
{
	return (new BooleanValue(*this));
}

void BooleanValue::Pack(Packer& data, unsigned_int32 packFlags) const
{
	Value::Pack(data, packFlags);

	data << value;
}

void BooleanValue::Unpack(Unpacker& data, unsigned_int32 unpackFlags)
{
	Value::Unpack(data, unpackFlags);

	data >> value;
}

int32 BooleanValue::GetSettingCount(void) const
{
	return (Value::GetSettingCount() + 1);
}

Setting *BooleanValue::GetSetting(int32 index) const
{
	int32 count = Value::GetSettingCount();
	if (index < count)
	{
		return (Value::GetSetting(index));
	}

	if (index == count)
	{
		const StringTable *table = TheInterfaceMgr->GetStringTable();

		const char *title = table->GetString(StringID('VALU', 'INIT'));
		MenuSetting *menu = new MenuSetting('INIT', !value, title, 2);

		menu->SetMenuItemString(0, table->GetString(StringID('VALU', 'INIT', 'TRUE')));
		menu->SetMenuItemString(1, table->GetString(StringID('VALU', 'INIT', 'FALS')));

		return (menu);
	}

	return (nullptr);
}

void BooleanValue::SetSetting(const Setting *setting)
{
	if (setting->GetSettingIdentifier() == 'INIT')
	{
		int32 selection = static_cast<const MenuSetting *>(setting)->GetMenuSelection();
		value = (selection == 0);
	}
	else
	{
		Value::SetSetting(setting);
	}
}

bool BooleanValue::GetBooleanValue(void) const
{
	return (value);
}

bool BooleanValue::SetValue(bool v)
{
	value = v;
	return (true);
}

bool BooleanValue::SetValue(int32 v)
{
	value = (v != 0);
	return (true);
}

bool BooleanValue::SetValue(float v)
{
	value = (v != 0.0F);
	return (true);
}

bool BooleanValue::SetValue(const char *v)
{
	value = (v[0] != 0);
	return (true);
}


IntegerValue::IntegerValue(int32 v) : Value(kValueInteger)
{
	value = v;
}

IntegerValue::IntegerValue(const IntegerValue& integerValue) : Value(integerValue)
{
	value = integerValue.value;
}

IntegerValue::~IntegerValue()
{
}

Value *IntegerValue::Replicate(void) const
{
	return (new IntegerValue(*this));
}

void IntegerValue::Pack(Packer& data, unsigned_int32 packFlags) const
{
	Value::Pack(data, packFlags);

	data << value;
}

void IntegerValue::Unpack(Unpacker& data, unsigned_int32 unpackFlags)
{
	Value::Unpack(data, unpackFlags);

	data >> value;
}

int32 IntegerValue::GetSettingCount(void) const
{
	return (Value::GetSettingCount() + 1);
}

Setting *IntegerValue::GetSetting(int32 index) const
{
	int32 count = Value::GetSettingCount();
	if (index < count)
	{
		return (Value::GetSetting(index));
	}

	if (index == count)
	{
		const StringTable *table = TheInterfaceMgr->GetStringTable();
		const char *title = table->GetString(StringID('VALU', 'INIT'));
		return (new TextSetting('INIT', Text::IntegerToString(value), title, 11, &EditTextWidget::SignedNumberFilter));
	}

	return (nullptr);
}

void IntegerValue::SetSetting(const Setting *setting)
{
	if (setting->GetSettingIdentifier() == 'INIT')
	{
		value = Text::StringToInteger(static_cast<const TextSetting *>(setting)->GetText());
	}
	else
	{
		Value::SetSetting(setting);
	}
}

bool IntegerValue::GetBooleanValue(void) const
{
	return (value != 0);
}

bool IntegerValue::SetValue(bool v)
{
	value = (int32) v;
	return (true);
}

bool IntegerValue::SetValue(int32 v)
{
	value = v;
	return (true);
}

bool IntegerValue::SetValue(float v)
{
	value = (int32) v;
	return (true);
}

bool IntegerValue::SetValue(const char *v)
{
	value = Text::StringToInteger(v);
	return (true);
}


FloatValue::FloatValue(float v) : Value(kValueFloat)
{
	value = v;
}

FloatValue::FloatValue(const FloatValue& floatValue) : Value(floatValue)
{
	value = floatValue.value;
}

FloatValue::~FloatValue()
{
}

Value *FloatValue::Replicate(void) const
{
	return (new FloatValue(*this));
}

void FloatValue::Pack(Packer& data, unsigned_int32 packFlags) const
{
	Value::Pack(data, packFlags);

	data << value;
}

void FloatValue::Unpack(Unpacker& data, unsigned_int32 unpackFlags)
{
	Value::Unpack(data, unpackFlags);

	data >> value;
}

int32 FloatValue::GetSettingCount(void) const
{
	return (Value::GetSettingCount() + 1);
}

Setting *FloatValue::GetSetting(int32 index) const
{
	int32 count = Value::GetSettingCount();
	if (index < count)
	{
		return (Value::GetSetting(index));
	}

	if (index == count)
	{
		const StringTable *table = TheInterfaceMgr->GetStringTable();
		const char *title = table->GetString(StringID('VALU', 'INIT'));
		return (new TextSetting('INIT', value, title));
	}

	return (nullptr);
}

void FloatValue::SetSetting(const Setting *setting)
{
	if (setting->GetSettingIdentifier() == 'INIT')
	{
		value = Text::StringToFloat(static_cast<const TextSetting *>(setting)->GetText());
	}
	else
	{
		Value::SetSetting(setting);
	}
}

bool FloatValue::GetBooleanValue(void) const
{
	return (value != 0.0F);
}

bool FloatValue::SetValue(bool v)
{
	value = (float) v;
	return (true);
}

bool FloatValue::SetValue(int32 v)
{
	value = (float) v;
	return (true);
}

bool FloatValue::SetValue(float v)
{
	value = v;
	return (true);
}

bool FloatValue::SetValue(const char *v)
{
	value = Text::StringToFloat(v);
	return (true);
}


StringValue::StringValue() : Value(kValueString)
{
	value[0] = 0;
}

StringValue::StringValue(const char *v) : Value(kValueString)
{
	value = v;
}

StringValue::StringValue(const StringValue& stringValue) : Value(stringValue)
{
	value = stringValue.value;
}

StringValue::~StringValue()
{
}

Value *StringValue::Replicate(void) const
{
	return (new StringValue(*this));
}

void StringValue::Pack(Packer& data, unsigned_int32 packFlags) const
{
	Value::Pack(data, packFlags);

	data << value;
}

void StringValue::Unpack(Unpacker& data, unsigned_int32 unpackFlags)
{
	Value::Unpack(data, unpackFlags);

	data >> value;
}

int32 StringValue::GetSettingCount(void) const
{
	return (Value::GetSettingCount() + 1);
}

Setting *StringValue::GetSetting(int32 index) const
{
	int32 count = Value::GetSettingCount();
	if (index < count)
	{
		return (Value::GetSetting(index));
	}

	if (index == count)
	{
		const StringTable *table = TheInterfaceMgr->GetStringTable();
		const char *title = table->GetString(StringID('VALU', 'INIT'));
		return (new TextSetting('INIT', value, title, kMaxStringValueLength));
	}

	return (nullptr);
}

void StringValue::SetSetting(const Setting *setting)
{
	if (setting->GetSettingIdentifier() == 'INIT')
	{
		value = static_cast<const TextSetting *>(setting)->GetText();
	}
	else
	{
		Value::SetSetting(setting);
	}
}

bool StringValue::GetBooleanValue(void) const
{
	return (value[0] != 0);
}

bool StringValue::SetValue(bool v)
{
	value = (int32) v;
	return (true);
}

bool StringValue::SetValue(int32 v)
{
	value = v;
	return (true);
}

bool StringValue::SetValue(float v)
{
	value = v;
	return (true);
}

bool StringValue::SetValue(const char *v)
{
	value = v;
	return (true);
}


ColorValue::ColorValue() : Value(kValueColor)
{
	value.Set(0.0F, 0.0F, 0.0F, 1.0F);
}

ColorValue::ColorValue(const ColorRGBA& v) : Value(kValueColor)
{
	value = v;
}

ColorValue::ColorValue(const ColorValue& colorValue) : Value(colorValue)
{
	value = colorValue.value;
}

ColorValue::~ColorValue()
{
}

Value *ColorValue::Replicate(void) const
{
	return (new ColorValue(*this));
}

void ColorValue::Pack(Packer& data, unsigned_int32 packFlags) const
{
	Value::Pack(data, packFlags);

	data << value;
}

void ColorValue::Unpack(Unpacker& data, unsigned_int32 unpackFlags)
{
	Value::Unpack(data, unpackFlags);

	data >> value;
}

int32 ColorValue::GetSettingCount(void) const
{
	return (Value::GetSettingCount() + 1);
}

Setting *ColorValue::GetSetting(int32 index) const
{
	int32 count = Value::GetSettingCount();
	if (index < count)
	{
		return (Value::GetSetting(index));
	}

	if (index == count)
	{
		const StringTable *table = TheInterfaceMgr->GetStringTable();
		const char *title = table->GetString(StringID('VALU', 'INIT'));
		const char *picker = table->GetString(StringID('VALU', 'INIT', 'COLR'));
		return (new ColorSetting('INIT', value, title, picker, kColorPickerAlpha));
	}

	return (nullptr);
}

void ColorValue::SetSetting(const Setting *setting)
{
	if (setting->GetSettingIdentifier() == 'INIT')
	{
		value = static_cast<const ColorSetting *>(setting)->GetColor();
	}
	else
	{
		Value::SetSetting(setting);
	}
}

bool ColorValue::GetBooleanValue(void) const
{
	return ((value.red != 0.0F) || (value.green != 0.0F) || (value.blue != 0.0F) || (value.alpha != 0.0F));
}

bool ColorValue::SetValue(const ColorRGBA& v)
{
	value = v;
	return (true);
}


VectorValue::VectorValue() : Value(kValueVector)
{
	value.Set(0.0F, 0.0F, 0.0F);
}

VectorValue::VectorValue(const Vector3D& v) : Value(kValueVector)
{
	value = v;
}

VectorValue::VectorValue(ValueType type) : Value(type)
{
	value.Set(0.0F, 0.0F, 0.0F);
}

VectorValue::VectorValue(ValueType type, const Vector3D& v) : Value(type)
{
	value = v;
}

VectorValue::VectorValue(const VectorValue& vectorValue) : Value(vectorValue)
{
	value = vectorValue.value;
}

VectorValue::~VectorValue()
{
}

Value *VectorValue::Replicate(void) const
{
	return (new VectorValue(*this));
}

void VectorValue::Pack(Packer& data, unsigned_int32 packFlags) const
{
	Value::Pack(data, packFlags);

	data << value;
}

void VectorValue::Unpack(Unpacker& data, unsigned_int32 unpackFlags)
{
	Value::Unpack(data, unpackFlags);

	data >> value;
}

int32 VectorValue::GetSettingCount(void) const
{
	return (Value::GetSettingCount() + 3);
}

Setting *VectorValue::GetSetting(int32 index) const
{
	int32 count = Value::GetSettingCount();
	if (index < count)
	{
		return (Value::GetSetting(index));
	}

	const StringTable *table = TheInterfaceMgr->GetStringTable();

	index -= count;
	if (index < 3)
	{
		Type identifier = 'VEC0' + index;
		const char *title = table->GetString(StringID('VALU', identifier));
		return (new TextSetting(identifier, value[index], title));
	}

	return (nullptr);
}

void VectorValue::SetSetting(const Setting *setting)
{
	Type identifier = setting->GetSettingIdentifier() - 'VEC0';
	if (identifier < 3U)
	{
		value[identifier] = Text::StringToFloat(static_cast<const TextSetting *>(setting)->GetText());
	}
	else
	{
		Value::SetSetting(setting);
	}
}

bool VectorValue::GetBooleanValue(void) const
{
	return ((value.x != 0.0F) || (value.y != 0.0F) || (value.z != 0.0F));
}

bool VectorValue::SetValue(const Vector3D& v)
{
	value = v;
	return (true);
}


PointValue::PointValue() : VectorValue(kValuePoint)
{
}

PointValue::PointValue(const Point3D& v) : VectorValue(kValuePoint, v)
{
}

PointValue::PointValue(const PointValue& pointValue) : VectorValue(pointValue)
{
}

PointValue::~PointValue()
{
}

Value *PointValue::Replicate(void) const
{
	return (new PointValue(*this));
}

// ZYUQURM
