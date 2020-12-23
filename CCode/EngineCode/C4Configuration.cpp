 

#include "C4Configuration.h"
#include "C4Plugins.h"
#include "C4Values.h"


using namespace C4;


namespace
{
	const float kSettingSpacing		= 18.0F;
	const float kValueNameWidth		= 144.0F;


	const TextureHeader indeterminantTextureHeader =
	{
		kTexture2D,
		kTextureForceHighQuality,
		kTextureSemanticDiffuse,
		kTextureSemanticTransparency,
		kTextureLA8,
		8, 8, 1,
		{kTextureRepeat, kTextureRepeat, kTextureRepeat},
		1
	};


	const unsigned_int8 indeterminantTextureImage[128] =
	{
		0xC0, 0xFF, 0xFF, 0x00, 0xFF, 0x00, 0xFF, 0x00, 0xFF, 0x00, 0xFF, 0x00, 0xC0, 0xFF, 0xC0, 0xFF,
		0xC0, 0xFF, 0xC0, 0xFF, 0xFF, 0x00, 0xFF, 0x00, 0xFF, 0x00, 0xFF, 0x00, 0xFF, 0x00, 0xC0, 0xFF,
		0xC0, 0xFF, 0xC0, 0xFF, 0xC0, 0xFF, 0xFF, 0x00, 0xFF, 0x00, 0xFF, 0x00, 0xFF, 0x00, 0xFF, 0x00,
		0xFF, 0x00, 0xC0, 0xFF, 0xC0, 0xFF, 0xC0, 0xFF, 0xFF, 0x00, 0xFF, 0x00, 0xFF, 0x00, 0xFF, 0x00,
		0xFF, 0x00, 0xFF, 0x00, 0xC0, 0xFF, 0xC0, 0xFF, 0xC0, 0xFF, 0xFF, 0x00, 0xFF, 0x00, 0xFF, 0x00,
		0xFF, 0x00, 0xFF, 0x00, 0xFF, 0x00, 0xC0, 0xFF, 0xC0, 0xFF, 0xC0, 0xFF, 0xFF, 0x00, 0xFF, 0x00,
		0xFF, 0x00, 0xFF, 0x00, 0xFF, 0x00, 0xFF, 0x00, 0xC0, 0xFF, 0xC0, 0xFF, 0xC0, 0xFF, 0xFF, 0x00,
		0xFF, 0x00, 0xFF, 0x00, 0xFF, 0x00, 0xFF, 0x00, 0xFF, 0x00, 0xC0, 0xFF, 0xC0, 0xFF, 0xC0, 0xFF
	};
}


Setting::Setting(SettingType type)
{
	settingType = type;
	settingValueName[0] = 0;
	settingInterface = nullptr;
}

Setting::Setting(SettingType type, Type identifier)
{
	settingType = type;
	settingIdentifier = identifier;
	settingValueName[0] = 0;
	settingInterface = nullptr;
}

Setting::Setting(const Setting& setting)
{
	settingType = setting.settingType;
	settingIdentifier = setting.settingIdentifier;
	settingValueName = setting.settingValueName;
	settingInterface = nullptr;
}

Setting::~Setting()
{
	delete settingInterface;
}

Setting *Setting::Create(Unpacker& data, unsigned_int32 unpackFlags)
{
	switch (data.GetType())
	{
		case kSettingHeading:

			return (new HeadingSetting);

		case kSettingInfo:

			return (new InfoSetting);

		case kSettingBoolean:

			return (new BooleanSetting);

		case kSettingInteger:

			return (new IntegerSetting);

		case kSettingPowerTwo:

			return (new PowerTwoSetting);

		case kSettingFloat:

			return (new FloatSetting);

		case kSettingText:

			return (new TextSetting);
 
		case kSettingMenu:

			return (new MenuSetting); 

		case kSettingColor: 

			return (new ColorSetting);
 
		case kSettingCheckColor:
 
			return (new CheckColorSetting); 

		case kSettingResource:

			return (new ResourceSetting); 

		case kSettingMultiResource:

			return (new MultiResourceSetting);
	}

	return (nullptr);
}

void Setting::PackType(Packer& data) const
{
	data << settingType;
}

void Setting::Pack(Packer& data, unsigned_int32 packFlags) const
{
	data << ChunkHeader('IDNT', 4);
	data << settingIdentifier;

	if (settingValueName[0] != 0)
	{
		PackHandle handle = data.BeginChunk('VALU');
		data << settingValueName;
		data.EndChunk(handle);
	}

	data << TerminatorChunk;
}

void Setting::Unpack(Unpacker& data, unsigned_int32 unpackFlags)
{
	UnpackChunkList<Setting>(data, unpackFlags);
}

bool Setting::UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags)
{
	switch (chunkHeader->chunkType)
	{
		case 'IDNT':

			data >> settingIdentifier;
			return (true);

		case 'VALU':

			data >> settingValueName;
			return (true);
	}

	return (false);
}

void *Setting::BeginSettingsUnpack(void)
{
	settingValueName[0] = 0;
	return (nullptr);
}

void Setting::Compress(Compressor& data) const
{
	data << settingIdentifier;
}

bool Setting::Decompress(Decompressor& data)
{
	data >> settingIdentifier;
	return (true);
}

void Setting::Copy(const Setting *setting)
{
}

bool Setting::SetValue(const Value *value)
{
	return (false);
}


SettingInterface::SettingInterface(Setting *setting, const char *title) : valueObserver(this, &SettingInterface::HandleValueEvent)
{
	settingData = setting;
	settingGroup = nullptr;
	settingTitle = title;
}

SettingInterface::~SettingInterface()
{
}

void SettingInterface::HandleValueEvent(Widget *widget, const WidgetEventData *eventData)
{
	if (eventData->eventType == kEventWidgetChange)
	{
		if ((static_cast<EditTextWidget *>(widget)->GetText()[0] != 0) && (!DeterminantValue()))
		{
			SetDeterminantValue();
		}
	}
}

void SettingInterface::BuildInterface(Widget *group, const ConfigurationWidget *configWidget)
{
	settingGroup = group;

	titleWidget = new TextWidget(Vector2D(configWidget->GetTitleColumnWidth() - 4.0F, kSettingSpacing), GetSettingTitle(), "font/Gui");
	titleWidget->SetWidgetPosition(Point3D(4.0F, 0.0F, 0.0F));
	group->AppendSubnode(titleWidget);

	if ((configWidget->GetConfigurationFlags() & kConfigurationScript) && (settingData->GetSettingType() != kSettingHeading))
	{
		valueWidget = new EditTextWidget(Vector2D(kValueNameWidth - 6.0F, kSettingSpacing - 5.0F), kMaxValueNameLength, "font/Gui");
		valueWidget->SetWidgetPosition(Point3D(configWidget->GetFullSettingWidth() - (kValueNameWidth - 3.0F), 0.0F, 0.0F));
		valueWidget->SetEditTextFlags(kEditTextOverflow | kEditTextRenderPlain);
		valueWidget->SetFilterProc(&EditTextWidget::IdentifierFilter);
		valueWidget->SetObserver(&valueObserver);
		group->AppendSubnode(valueWidget);
	}
	else
	{
		valueWidget = nullptr;
	}
}

void SettingInterface::UpdateInterface(const ConfigurationWidget *configWidget)
{
	titleWidget->SetWidgetSize(Vector2D(configWidget->GetTitleColumnWidth() - 4.0F, kSettingSpacing));

	if (valueWidget)
	{
		valueWidget->SetWidgetPosition(Point3D(configWidget->GetFullSettingWidth() - (kValueNameWidth - 3.0F), 0.0F, 0.0F));
		valueWidget->Invalidate();
	}
}

bool SettingInterface::DeterminantValue(void) const
{
	return (false);
}

void SettingInterface::SetDeterminantValue(void)
{
}

void SettingInterface::SetIndeterminantValue(void)
{
}

bool SettingInterface::ExtractCurrentSetting(void)
{
	if (valueWidget)
	{
		settingData->SetSettingValueName(valueWidget->GetText());
	}

	return (false);
}


HeadingSetting::HeadingSetting() : Setting(kSettingHeading)
{
}

HeadingSetting::HeadingSetting(Type identifier, const char *title) : Setting(kSettingHeading, identifier)
{
	SetSettingInterface(new HeadingSettingInterface(this, title));
}

HeadingSetting::HeadingSetting(const HeadingSetting& headingSetting) : Setting(headingSetting)
{
}

HeadingSetting::~HeadingSetting()
{
}

Setting *HeadingSetting::Replicate(void) const
{
	return (new HeadingSetting(*this));
}

bool HeadingSetting::operator ==(const Setting& setting) const
{
	return (setting.GetSettingType() == kSettingHeading);
}


HeadingSettingInterface::HeadingSettingInterface(HeadingSetting *setting, const char *title) : SettingInterface(setting, title)
{
}

HeadingSettingInterface::~HeadingSettingInterface()
{
}

void HeadingSettingInterface::BuildInterface(Widget *group, const ConfigurationWidget *configWidget)
{
	SettingInterface::BuildInterface(group, configWidget);

	quadWidget = new QuadWidget(Vector2D(configWidget->GetFullSettingWidth(), kSettingSpacing - 1.0F));

	const ColorRGBA& color = TheInterfaceMgr->GetInterfaceColor(kInterfaceColorWindowFrame);
	ColorRGBA top(color.red * 0.75F + 0.25F, color.green * 0.75F + 0.25F, color.blue * 0.75F + 0.25F, 1.0F);
	ColorRGBA bottom(color.red * 0.25F + 0.75F, color.green * 0.25F + 0.75F, color.blue * 0.25F + 0.75F, 1.0F);

	quadWidget->SetVertexColor(0, top);
	quadWidget->SetVertexColor(1, bottom);
	quadWidget->SetVertexColor(2, top);
	quadWidget->SetVertexColor(3, bottom);

	quadWidget->SetWidgetPosition(Point3D(0.0F, -2.0F, 0.0F));
	group->PrependSubnode(quadWidget);
}

void HeadingSettingInterface::UpdateInterface(const ConfigurationWidget *configWidget)
{
	SettingInterface::UpdateInterface(configWidget);
	quadWidget->SetWidgetSize(Vector2D(configWidget->GetFullSettingWidth(), kSettingSpacing - 1.0F));
}


InfoSetting::InfoSetting() : Setting(kSettingInfo)
{
}

InfoSetting::InfoSetting(Type identifier, const char *value, const char *title) : Setting(kSettingInfo, identifier)
{
	infoValue = value;

	SetSettingInterface(new InfoSettingInterface(this, title));
}

InfoSetting::InfoSetting(const InfoSetting& infoSetting) : Setting(infoSetting)
{
	infoValue = infoSetting.infoValue;
}

InfoSetting::~InfoSetting()
{
}

Setting *InfoSetting::Replicate(void) const
{
	return (new InfoSetting(*this));
}

bool InfoSetting::operator ==(const Setting& setting) const
{
	return (setting.GetSettingType() == kSettingInfo);
}


InfoSettingInterface::InfoSettingInterface(InfoSetting *setting, const char *title) : SettingInterface(setting, title)
{
}

InfoSettingInterface::~InfoSettingInterface()
{
}

void InfoSettingInterface::BuildInterface(Widget *group, const ConfigurationWidget *configWidget)
{
	SettingInterface::BuildInterface(group, configWidget);

	textWidget = new TextWidget(Vector2D(configWidget->GetValueColumnWidth() - 3.0F, kSettingSpacing), static_cast<InfoSetting *>(GetSettingData())->GetInfoValue(), "font/Gui");
	textWidget->SetWidgetPosition(Point3D(configWidget->GetValueColumnPosition() + 3.0F, 0.0F, 0.0F));
	group->AppendSubnode(textWidget);
}

void InfoSettingInterface::UpdateInterface(const ConfigurationWidget *configWidget)
{
	SettingInterface::UpdateInterface(configWidget);
	textWidget->SetWidgetSize(Vector2D(configWidget->GetValueColumnWidth(), kSettingSpacing));
}


BooleanSetting::BooleanSetting() : Setting(kSettingBoolean)
{
}

BooleanSetting::BooleanSetting(Type identifier, bool value, const char *title) : Setting(kSettingBoolean, identifier)
{
	booleanValue = value;

	SetSettingInterface(new BooleanSettingInterface(this, title));
}

BooleanSetting::BooleanSetting(const BooleanSetting& booleanSetting) : Setting(booleanSetting)
{
	booleanValue = booleanSetting.booleanValue;
}

BooleanSetting::~BooleanSetting()
{
}

Setting *BooleanSetting::Replicate(void) const
{
	return (new BooleanSetting(*this));
}

void BooleanSetting::Pack(Packer& data, unsigned_int32 packFlags) const
{
	Setting::Pack(data, packFlags);

	data << booleanValue;
}

void BooleanSetting::Unpack(Unpacker& data, unsigned_int32 unpackFlags)
{
	Setting::Unpack(data, unpackFlags);

	data >> booleanValue;
}

void BooleanSetting::Compress(Compressor& data) const
{
	Setting::Compress(data);

	data << booleanValue;
}

bool BooleanSetting::Decompress(Decompressor& data)
{
	Setting::Decompress(data);

	data >> booleanValue;
	return (true);
}

void BooleanSetting::Copy(const Setting *setting)
{
	booleanValue = static_cast<const BooleanSetting *>(setting)->booleanValue;
}

bool BooleanSetting::operator ==(const Setting& setting) const
{
	if (setting.GetSettingType() == kSettingBoolean)
	{
		const BooleanSetting *booleanSetting = static_cast<const BooleanSetting *>(&setting);
		return (booleanSetting->GetBooleanValue() == booleanValue);
	}

	return (false);
}

bool BooleanSetting::SetValue(const Value *value)
{
	switch (value->GetValueType())
	{
		case kValueBoolean:

			booleanValue = static_cast<const BooleanValue *>(value)->GetValue();
			return (true);

		case kValueInteger:

			booleanValue = (static_cast<const IntegerValue *>(value)->GetValue() != 0);
			return (true);

		case kValueFloat:

			booleanValue = (static_cast<const FloatValue *>(value)->GetValue() != 0.0F);
			return (true);

		case kValueString:

			booleanValue = (static_cast<const StringValue *>(value)->GetValue()[0] != 0);
			return (true);
	}

	return (false);
}


BooleanSettingInterface::BooleanSettingInterface(BooleanSetting *setting, const char *title) :
		SettingInterface(setting, title),
		checkWidgetObserver(this, &BooleanSettingInterface::HandleCheckWidgetEvent)
{
}

BooleanSettingInterface::~BooleanSettingInterface()
{
}

void BooleanSettingInterface::HandleCheckWidgetEvent(Widget *widget, const WidgetEventData *eventData)
{
	if (eventData->eventType == kEventWidgetChange)
	{
		PostEvent();
	}
}

void BooleanSettingInterface::BuildInterface(Widget *group, const ConfigurationWidget *configWidget)
{
	SettingInterface::BuildInterface(group, configWidget);

	checkWidget = new CheckWidget(Vector2D(16.0F, kSettingSpacing));
	checkWidget->SetWidgetPosition(Point3D(configWidget->GetValueColumnPosition() + 4.0F, 0.0F, 0.0F));
	checkWidget->SetObserver(&checkWidgetObserver);
	group->PrependSubnode(checkWidget);
}

void BooleanSettingInterface::UpdateInterface(const ConfigurationWidget *configWidget)
{
	SettingInterface::UpdateInterface(configWidget);

	checkWidget->SetWidgetPosition(Point3D(configWidget->GetValueColumnPosition() + 4.0F, 0.0F, 0.0F));
	checkWidget->Invalidate();
}

bool BooleanSettingInterface::DeterminantValue(void) const
{
	return (checkWidget->GetValue() != kWidgetValueIndeterminant);
}

void BooleanSettingInterface::SetDeterminantValue(void)
{
	checkWidget->SetValue(static_cast<BooleanSetting *>(GetSettingData())->GetBooleanValue());
}

void BooleanSettingInterface::SetIndeterminantValue(void)
{
	checkWidget->SetValue(kWidgetValueIndeterminant);
}

bool BooleanSettingInterface::ExtractCurrentSetting(void)
{
	SettingInterface::ExtractCurrentSetting();

	int32 value = checkWidget->GetValue();
	if (value != kWidgetValueIndeterminant)
	{
		static_cast<BooleanSetting *>(GetSettingData())->SetBooleanValue(value != 0);
		return (true);
	}

	return (false);
}


SliderSettingInterface::SliderSettingInterface(Setting *setting, const char *title, const char *minLabel, const char *maxLabel) : SettingInterface(setting, title)
{
	if (minLabel)
	{
		minValueLabel = minLabel;
	}

	if (maxLabel)
	{
		maxValueLabel = maxLabel;
	}
}

SliderSettingInterface::~SliderSettingInterface()
{
}

void SliderSettingInterface::UpdateInterface(const ConfigurationWidget *configWidget)
{
	SettingInterface::UpdateInterface(configWidget);

	float valueWidth = configWidget->GetValueColumnWidth();
	float valuePosition = configWidget->GetValueColumnPosition();

	sliderWidget->SetWidgetSize(Vector2D(valueWidth - 65.0F, kSettingSpacing));
	sliderWidget->SetWidgetPosition(Point3D(valuePosition + 4.0F, -2.0F, 0.0F));
	sliderWidget->Invalidate();

	textWidget->SetWidgetPosition(Point3D(valuePosition + valueWidth - 55.0F, 0.0F, 0.0F));
	textWidget->Invalidate();
}

bool SliderSettingInterface::DeterminantValue(void) const
{
	return (sliderWidget->GetValue() != kWidgetValueIndeterminant);
}

void SliderSettingInterface::SetIndeterminantValue(void)
{
	sliderWidget->SetValue(kWidgetValueIndeterminant);
	textWidget->SetText(nullptr);
}


IntegerSetting::IntegerSetting() : Setting(kSettingInteger)
{
}

IntegerSetting::IntegerSetting(Type identifier, int32 value, const char *title, int32 min, int32 max, int32 step, const char *minLabel, const char *maxLabel) : Setting(kSettingInteger, identifier)
{
	integerValue = Min(Max(value, min), max);

	SetSettingInterface(new IntegerSettingInterface(this, title, min, max, step, minLabel, maxLabel));
}

IntegerSetting::IntegerSetting(const IntegerSetting& integerSetting) : Setting(integerSetting)
{
	integerValue = integerSetting.integerValue;
}

IntegerSetting::~IntegerSetting()
{
}

Setting *IntegerSetting::Replicate(void) const
{
	return (new IntegerSetting(*this));
}

void IntegerSetting::Pack(Packer& data, unsigned_int32 packFlags) const
{
	Setting::Pack(data, packFlags);

	data << integerValue;
}

void IntegerSetting::Unpack(Unpacker& data, unsigned_int32 unpackFlags)
{
	Setting::Unpack(data, unpackFlags);

	data >> integerValue;
}

void IntegerSetting::Compress(Compressor& data) const
{
	Setting::Compress(data);

	data << integerValue;
}

bool IntegerSetting::Decompress(Decompressor& data)
{
	Setting::Decompress(data);

	data >> integerValue;
	return (true);
}

void IntegerSetting::Copy(const Setting *setting)
{
	integerValue = static_cast<const IntegerSetting *>(setting)->integerValue;
}

bool IntegerSetting::operator ==(const Setting& setting) const
{
	if (setting.GetSettingType() == kSettingInteger)
	{
		const IntegerSetting *integerSetting = static_cast<const IntegerSetting *>(&setting);
		return (integerSetting->GetIntegerValue() == integerValue);
	}

	return (false);
}

bool IntegerSetting::SetValue(const Value *value)
{
	switch (value->GetValueType())
	{
		case kValueBoolean:

			integerValue = static_cast<const BooleanValue *>(value)->GetValue();
			return (true);

		case kValueInteger:

			integerValue = static_cast<const IntegerValue *>(value)->GetValue();
			return (true);

		case kValueFloat:

			integerValue = (int32) static_cast<const FloatValue *>(value)->GetValue();
			return (true);

		case kValueString:

			integerValue = Text::StringToInteger(static_cast<const StringValue *>(value)->GetValue());
			return (true);
	}

	return (false);
}


IntegerSettingInterface::IntegerSettingInterface(IntegerSetting *setting, const char *title, int32 min, int32 max, int32 step, const char *minLabel, const char *maxLabel) :
		SliderSettingInterface(setting, title, minLabel, maxLabel),
		sliderWidgetObserver(this, &IntegerSettingInterface::HandleSliderWidgetEvent),
		textWidgetObserver(this, &IntegerSettingInterface::HandleTextWidgetEvent)
{
	minValue = min;
	maxValue = max;
	stepValue = step;
}

IntegerSettingInterface::~IntegerSettingInterface()
{
}

void IntegerSettingInterface::SetValueText(int32 value) const
{
	if (value == minValue)
	{
		if (minValueLabel[0] != 0)
		{
			textWidget->SetText(minValueLabel);
			textWidget->SetEditTextFlags(textWidget->GetEditTextFlags() | kEditTextEditClear);
			return;
		}
	}
	else if (value == maxValue)
	{
		if (maxValueLabel[0] != 0)
		{
			textWidget->SetText(maxValueLabel);
			textWidget->SetEditTextFlags(textWidget->GetEditTextFlags() | kEditTextEditClear);
			return;
		}
	}

	textWidget->SetText(Text::IntegerToString(value));
	textWidget->SetEditTextFlags(textWidget->GetEditTextFlags() & ~kEditTextEditClear);
}

void IntegerSettingInterface::HandleSliderWidgetEvent(Widget *widget, const WidgetEventData *eventData)
{
	if (eventData->eventType == kEventWidgetChange)
	{
		SetValueText(static_cast<SliderWidget *>(widget)->GetValue() * stepValue + minValue);
		PostEvent();
	}
}

void IntegerSettingInterface::HandleTextWidgetEvent(Widget *widget, const WidgetEventData *eventData)
{
	if (eventData->eventType == kEventWidgetChange)
	{
		int32 integerValue = Text::StringToInteger(static_cast<EditTextWidget *>(widget)->GetText());
		integerValue = Min(Max(integerValue, minValue), maxValue);
		sliderWidget->SetValue((integerValue - minValue) / stepValue);

		PostEvent();
	}
}

void IntegerSettingInterface::BuildInterface(Widget *group, const ConfigurationWidget *configWidget)
{
	SliderSettingInterface::BuildInterface(group, configWidget);

	float valueWidth = configWidget->GetValueColumnWidth();
	float valuePosition = configWidget->GetValueColumnPosition();

	sliderWidget = new SliderWidget(Vector2D(valueWidth - 65.0F, kSettingSpacing));
	sliderWidget->SetWidgetPosition(Point3D(valuePosition + 4.0F, -2.0F, 0.0F));
	sliderWidget->SetMaxValue((maxValue - minValue) / stepValue);
	sliderWidget->SetObserver(&sliderWidgetObserver);
	group->PrependSubnode(sliderWidget);

	textWidget = new EditTextWidget(Vector2D(51.0F, kSettingSpacing - 5.0F), 8, "font/Gui");
	textWidget->SetWidgetPosition(Point3D(valuePosition + valueWidth - 55.0F, 0.0F, 0.0F));
	textWidget->SetFilterProc((minValue < 0) ? &EditTextWidget::SignedNumberFilter : &EditTextWidget::NumberFilter);
	textWidget->SetEditTextFlags(kEditTextChangeSelectAll | kEditTextRenderPlain);
	textWidget->SetObserver(&textWidgetObserver);
	group->InsertSubnodeAfter(textWidget, sliderWidget);
}

void IntegerSettingInterface::SetDeterminantValue(void)
{
	int32 integerValue = static_cast<IntegerSetting *>(GetSettingData())->GetIntegerValue();

	sliderWidget->SetValue((integerValue - minValue) / stepValue);
	SetValueText(integerValue);
}

bool IntegerSettingInterface::ExtractCurrentSetting(void)
{
	SettingInterface::ExtractCurrentSetting();

	int32 value = sliderWidget->GetValue();
	if (value != kWidgetValueIndeterminant)
	{
		static_cast<IntegerSetting *>(GetSettingData())->SetIntegerValue(value * stepValue + minValue);
		return (true);
	}

	return (false);
}


PowerTwoSetting::PowerTwoSetting() : Setting(kSettingPowerTwo)
{
}

PowerTwoSetting::PowerTwoSetting(Type identifier, int32 value, const char *title, int32 min, int32 max, const char *minLabel, const char *maxLabel) : Setting(kSettingPowerTwo, identifier)
{
	integerValue = Min(Max(value, min), max);

	SetSettingInterface(new PowerTwoSettingInterface(this, title, min, max, minLabel, maxLabel));
}

PowerTwoSetting::PowerTwoSetting(const PowerTwoSetting& powerTwoSetting) : Setting(powerTwoSetting)
{
	integerValue = powerTwoSetting.integerValue;
}

PowerTwoSetting::~PowerTwoSetting()
{
}

Setting *PowerTwoSetting::Replicate(void) const
{
	return (new PowerTwoSetting(*this));
}

void PowerTwoSetting::Pack(Packer& data, unsigned_int32 packFlags) const
{
	Setting::Pack(data, packFlags);

	data << integerValue;
}

void PowerTwoSetting::Unpack(Unpacker& data, unsigned_int32 unpackFlags)
{
	Setting::Unpack(data, unpackFlags);

	data >> integerValue;
}

void PowerTwoSetting::Compress(Compressor& data) const
{
	Setting::Compress(data);

	data << integerValue;
}

bool PowerTwoSetting::Decompress(Decompressor& data)
{
	Setting::Decompress(data);

	data >> integerValue;
	return (true);
}

void PowerTwoSetting::Copy(const Setting *setting)
{
	integerValue = static_cast<const PowerTwoSetting *>(setting)->integerValue;
}

bool PowerTwoSetting::operator ==(const Setting& setting) const
{
	if (setting.GetSettingType() == kSettingPowerTwo)
	{
		const PowerTwoSetting *powerTwoSetting = static_cast<const PowerTwoSetting *>(&setting);
		return (powerTwoSetting->GetIntegerValue() == integerValue);
	}

	return (false);
}

bool PowerTwoSetting::SetValue(const Value *value)
{
	switch (value->GetValueType())
	{
		case kValueBoolean:

			integerValue = static_cast<const BooleanValue *>(value)->GetValue();
			return (true);

		case kValueInteger:

			integerValue = static_cast<const IntegerValue *>(value)->GetValue();
			return (true);

		case kValueFloat:

			integerValue = (int32) static_cast<const FloatValue *>(value)->GetValue();
			return (true);

		case kValueString:

			integerValue = Text::StringToInteger(static_cast<const StringValue *>(value)->GetValue());
			return (true);
	}

	return (false);
}


PowerTwoSettingInterface::PowerTwoSettingInterface(PowerTwoSetting *setting, const char *title, int32 min, int32 max, const char *minLabel, const char *maxLabel) :
		SliderSettingInterface(setting, title, minLabel, maxLabel),
		sliderWidgetObserver(this, &PowerTwoSettingInterface::HandleSliderWidgetEvent),
		textWidgetObserver(this, &PowerTwoSettingInterface::HandleTextWidgetEvent)
{
	minValue = min;
	maxValue = max;
}

PowerTwoSettingInterface::~PowerTwoSettingInterface()
{
}

void PowerTwoSettingInterface::SetValueText(int32 value) const
{
	if (value == minValue)
	{
		if (minValueLabel[0] != 0)
		{
			textWidget->SetText(minValueLabel);
			textWidget->SetEditTextFlags(textWidget->GetEditTextFlags() | kEditTextEditClear);
			return;
		}
	}
	else if (value == maxValue)
	{
		if (maxValueLabel[0] != 0)
		{
			textWidget->SetText(maxValueLabel);
			textWidget->SetEditTextFlags(textWidget->GetEditTextFlags() | kEditTextEditClear);
			return;
		}
	}

	textWidget->SetText(Text::IntegerToString(value));
	textWidget->SetEditTextFlags(textWidget->GetEditTextFlags() & ~kEditTextEditClear);
}

void PowerTwoSettingInterface::HandleSliderWidgetEvent(Widget *widget, const WidgetEventData *eventData)
{
	if (eventData->eventType == kEventWidgetChange)
	{
		SetValueText(minValue * (1 << static_cast<SliderWidget *>(widget)->GetValue()));
		PostEvent();
	}
}

void PowerTwoSettingInterface::HandleTextWidgetEvent(Widget *widget, const WidgetEventData *eventData)
{
	if (eventData->eventType == kEventWidgetChange)
	{
		int32 integerValue = Text::StringToInteger(static_cast<EditTextWidget *>(widget)->GetText());
		integerValue = Min(Max(integerValue, minValue), maxValue);
		sliderWidget->SetValue(31 - Cntlz(integerValue / minValue));

		PostEvent();
	}
}

void PowerTwoSettingInterface::BuildInterface(Widget *group, const ConfigurationWidget *configWidget)
{
	SliderSettingInterface::BuildInterface(group, configWidget);

	float valueWidth = configWidget->GetValueColumnWidth();
	float valuePosition = configWidget->GetValueColumnPosition();

	sliderWidget = new SliderWidget(Vector2D(valueWidth - 65.0F, kSettingSpacing));
	sliderWidget->SetWidgetPosition(Point3D(valuePosition + 4.0F, -2.0F, 0.0F));
	sliderWidget->SetMaxValue(31 - Cntlz(maxValue / minValue));
	sliderWidget->SetObserver(&sliderWidgetObserver);
	group->PrependSubnode(sliderWidget);

	textWidget = new EditTextWidget(Vector2D(51.0F, kSettingSpacing - 5.0F), 8, "font/Gui");
	textWidget->SetWidgetPosition(Point3D(valuePosition + valueWidth - 55.0F, 0.0F, 0.0F));
	textWidget->SetFilterProc(&EditTextWidget::NumberFilter);
	textWidget->SetEditTextFlags(kEditTextChangeSelectAll | kEditTextRenderPlain);
	textWidget->SetObserver(&textWidgetObserver);
	group->InsertSubnodeAfter(textWidget, sliderWidget);
}

void PowerTwoSettingInterface::SetDeterminantValue(void)
{
	int32 integerValue = static_cast<PowerTwoSetting *>(GetSettingData())->GetIntegerValue();

	sliderWidget->SetValue(31 - Cntlz(integerValue / minValue));
	SetValueText(integerValue);
}

bool PowerTwoSettingInterface::ExtractCurrentSetting(void)
{
	SettingInterface::ExtractCurrentSetting();

	int32 value = sliderWidget->GetValue();
	if (value != kWidgetValueIndeterminant)
	{
		static_cast<IntegerSetting *>(GetSettingData())->SetIntegerValue(minValue * (1 << value));
		return (true);
	}

	return (false);
}


FloatSetting::FloatSetting() : Setting(kSettingFloat)
{
}

FloatSetting::FloatSetting(Type identifier, float value, const char *title, float min, float max, float step, const char *minLabel, const char *maxLabel) : Setting(kSettingFloat, identifier)
{
	floatValue = Clamp(value, min, max);

	SetSettingInterface(new FloatSettingInterface(this, title, min, max, step, minLabel, maxLabel));
}

FloatSetting::FloatSetting(const FloatSetting& floatSetting) : Setting(floatSetting)
{
	floatValue = floatSetting.floatValue;
}

FloatSetting::~FloatSetting()
{
}

Setting *FloatSetting::Replicate(void) const
{
	return (new FloatSetting(*this));
}

void FloatSetting::Pack(Packer& data, unsigned_int32 packFlags) const
{
	Setting::Pack(data, packFlags);

	data << floatValue;
}

void FloatSetting::Unpack(Unpacker& data, unsigned_int32 unpackFlags)
{
	Setting::Unpack(data, unpackFlags);

	data >> floatValue;
}

void FloatSetting::Compress(Compressor& data) const
{
	Setting::Compress(data);

	data << floatValue;
}

bool FloatSetting::Decompress(Decompressor& data)
{
	Setting::Decompress(data);

	data >> floatValue;
	return (true);
}

void FloatSetting::Copy(const Setting *setting)
{
	floatValue = static_cast<const FloatSetting *>(setting)->floatValue;
}

bool FloatSetting::operator ==(const Setting& setting) const
{
	if (setting.GetSettingType() == kSettingFloat)
	{
		const FloatSetting *floatSetting = static_cast<const FloatSetting *>(&setting);
		return (floatSetting->GetFloatValue() == floatValue);
	}

	return (false);
}

bool FloatSetting::SetValue(const Value *value)
{
	switch (value->GetValueType())
	{
		case kValueBoolean:

			floatValue = (float) static_cast<const BooleanValue *>(value)->GetValue();
			return (true);

		case kValueInteger:

			floatValue = (float) static_cast<const IntegerValue *>(value)->GetValue();
			return (true);

		case kValueFloat:

			floatValue = static_cast<const FloatValue *>(value)->GetValue();
			return (true);

		case kValueString:

			floatValue = Text::StringToFloat(static_cast<const StringValue *>(value)->GetValue());
			return (true);
	}

	return (false);
}


FloatSettingInterface::FloatSettingInterface(FloatSetting *setting, const char *title, float min, float max, float step, const char *minLabel, const char *maxLabel) :
		SliderSettingInterface(setting, title, minLabel, maxLabel),
		sliderWidgetObserver(this, &FloatSettingInterface::HandleSliderWidgetEvent),
		textWidgetObserver(this, &FloatSettingInterface::HandleTextWidgetEvent)
{
	minValue = min;
	maxValue = max;
	stepValue = step;
}

FloatSettingInterface::~FloatSettingInterface()
{
}

void FloatSettingInterface::SetValueText(float value) const
{
	if (value == minValue)
	{
		if (minValueLabel[0] != 0)
		{
			textWidget->SetText(minValueLabel);
			textWidget->SetEditTextFlags(textWidget->GetEditTextFlags() | kEditTextEditClear);
			return;
		}
	}
	else if (value == maxValue)
	{
		if (maxValueLabel[0] != 0)
		{
			textWidget->SetText(maxValueLabel);
			textWidget->SetEditTextFlags(textWidget->GetEditTextFlags() | kEditTextEditClear);
			return;
		}
	}

	textWidget->SetText(Text::FloatToString(value));
	textWidget->SetEditTextFlags(textWidget->GetEditTextFlags() & ~kEditTextEditClear);
}

void FloatSettingInterface::HandleSliderWidgetEvent(Widget *widget, const WidgetEventData *eventData)
{
	if (eventData->eventType == kEventWidgetChange)
	{
		SetValueText((float) static_cast<SliderWidget *>(widget)->GetValue() * stepValue + minValue);
		PostEvent();
	}
}

void FloatSettingInterface::HandleTextWidgetEvent(Widget *widget, const WidgetEventData *eventData)
{
	if (eventData->eventType == kEventWidgetChange)
	{
		float floatValue = Text::StringToFloat(static_cast<EditTextWidget *>(widget)->GetText());
		floatValue = Clamp(floatValue, minValue, maxValue);
		sliderWidget->SetValue((int32) ((floatValue - minValue) / stepValue + 0.5F));

		PostEvent();
	}
}

void FloatSettingInterface::BuildInterface(Widget *group, const ConfigurationWidget *configWidget)
{
	SliderSettingInterface::BuildInterface(group, configWidget);

	float valueWidth = configWidget->GetValueColumnWidth();
	float valuePosition = configWidget->GetValueColumnPosition();

	sliderWidget = new SliderWidget(Vector2D(valueWidth - 65.0F, kSettingSpacing));
	sliderWidget->SetWidgetPosition(Point3D(valuePosition + 4.0F, -2.0F, 0.0F));
	sliderWidget->SetMaxValue((int32) ((maxValue - minValue) / stepValue + 0.5F));
	sliderWidget->SetObserver(&sliderWidgetObserver);
	group->PrependSubnode(sliderWidget);

	textWidget = new EditTextWidget(Vector2D(51.0F, kSettingSpacing - 5.0F), 8, "font/Gui");
	textWidget->SetWidgetPosition(Point3D(valuePosition + valueWidth - 55.0F, 0.0F, 0.0F));
	textWidget->SetFilterProc(&EditTextWidget::FloatNumberFilter);
	textWidget->SetEditTextFlags(kEditTextChangeSelectAll | kEditTextRenderPlain);
	textWidget->SetObserver(&textWidgetObserver);
	group->InsertSubnodeAfter(textWidget, sliderWidget);
}

void FloatSettingInterface::SetDeterminantValue(void)
{
	float floatValue = static_cast<FloatSetting *>(GetSettingData())->GetFloatValue();

	sliderWidget->SetValue((int32) ((floatValue - minValue) / stepValue + 0.5F));
	SetValueText(floatValue);
}

bool FloatSettingInterface::ExtractCurrentSetting(void)
{
	SettingInterface::ExtractCurrentSetting();

	int32 value = sliderWidget->GetValue();
	if (value != kWidgetValueIndeterminant)
	{
		static_cast<FloatSetting *>(GetSettingData())->SetFloatValue((float) value * stepValue + minValue);
		return (true);
	}

	return (false);
}


TextSetting::TextSetting() : Setting(kSettingText)
{
}

TextSetting::TextSetting(Type identifier, const char *text, const char *title, int32 maxLen, EditTextWidget::FilterProc *filterProc) : Setting(kSettingText, identifier)
{
	if (text)
	{
		textValue = text;
	}
	else
	{
		textValue[0] = 0;
	}

	SetSettingInterface(new TextSettingInterface(this, title, maxLen, filterProc));
}

TextSetting::TextSetting(Type identifier, float value, const char *title) :
		Setting(kSettingText, identifier),
		textValue(value)
{
	SetSettingInterface(new TextSettingInterface(this, title, 15, &EditTextWidget::FloatNumberFilter));
}

TextSetting::TextSetting(const TextSetting& textSetting) : Setting(textSetting)
{
	textValue = textSetting.textValue;
}

TextSetting::~TextSetting()
{
}

Setting *TextSetting::Replicate(void) const
{
	return (new TextSetting(*this));
}

void TextSetting::Pack(Packer& data, unsigned_int32 packFlags) const
{
	Setting::Pack(data, packFlags);

	data << textValue;
}

void TextSetting::Unpack(Unpacker& data, unsigned_int32 unpackFlags)
{
	Setting::Unpack(data, unpackFlags);

	data >> textValue;
}

void TextSetting::Compress(Compressor& data) const
{
	Setting::Compress(data);

	data << textValue;
}

bool TextSetting::Decompress(Decompressor& data)
{
	Setting::Decompress(data);

	data >> textValue;
	return (true);
}

void TextSetting::Copy(const Setting *setting)
{
	textValue = static_cast<const TextSetting *>(setting)->textValue;
}

bool TextSetting::operator ==(const Setting& setting) const
{
	if (setting.GetSettingType() == kSettingText)
	{
		const TextSetting *textSetting = static_cast<const TextSetting *>(&setting);
		return (Text::CompareText(textSetting->GetText(), textValue));
	}

	return (false);
}

bool TextSetting::SetValue(const Value *value)
{
	switch (value->GetValueType())
	{
		case kValueBoolean:

			textValue = (int32) static_cast<const BooleanValue *>(value)->GetValue();
			return (true);

		case kValueInteger:

			textValue = static_cast<const IntegerValue *>(value)->GetValue();
			return (true);

		case kValueFloat:

			textValue = static_cast<const FloatValue *>(value)->GetValue();
			return (true);

		case kValueString:

			textValue = static_cast<const StringValue *>(value)->GetValue();
			return (true);
	}

	return (false);
}


TextSettingInterface::TextSettingInterface(TextSetting *setting, const char *title, int32 maxLen, EditTextWidget::FilterProc *filterProc) :
		SettingInterface(setting, title),
		textWidgetObserver(this, &TextSettingInterface::HandleTextWidgetEvent)
{
	maxTextLength = maxLen;
	textFilterProc = filterProc;

	stripesWidget = nullptr;
}

TextSettingInterface::~TextSettingInterface()
{
}

void TextSettingInterface::HandleTextWidgetEvent(Widget *widget, const WidgetEventData *eventData)
{
	if (eventData->eventType == kEventWidgetChange)
	{
		delete stripesWidget;
		stripesWidget = nullptr;

		PostEvent();
	}
}

void TextSettingInterface::BuildInterface(Widget *group, const ConfigurationWidget *configWidget)
{
	SettingInterface::BuildInterface(group, configWidget);

	textWidget = new EditTextWidget(Vector2D(configWidget->GetValueColumnWidth() - 4.0F, kSettingSpacing - 5.0F), maxTextLength, "font/Gui");
	textWidget->SetWidgetPosition(Point3D(configWidget->GetValueColumnPosition() + 2.0F, 0.0F, 0.0F));
	textWidget->SetEditTextFlags(kEditTextOverflow | kEditTextRenderPlain);
	textWidget->SetFilterProc(textFilterProc);
	textWidget->SetObserver(&textWidgetObserver);
	group->PrependSubnode(textWidget);
}

void TextSettingInterface::UpdateInterface(const ConfigurationWidget *configWidget)
{
	SettingInterface::UpdateInterface(configWidget);

	textWidget->SetWidgetSize(Vector2D(configWidget->GetValueColumnWidth() - 4.0F, kSettingSpacing - 5.0F));
	textWidget->SetWidgetPosition(Point3D(configWidget->GetValueColumnPosition() + 2.0F, 0.0F, 0.0F));
	textWidget->Invalidate();

	if (stripesWidget)
	{
		stripesWidget->SetWidgetSize(textWidget->GetWidgetSize());
	}
}

bool TextSettingInterface::DeterminantValue(void) const
{
	return (!stripesWidget);
}

void TextSettingInterface::SetDeterminantValue(void)
{
	delete stripesWidget;
	stripesWidget = nullptr;

	textWidget->SetText(static_cast<TextSetting *>(GetSettingData())->GetText());
}

void TextSettingInterface::SetIndeterminantValue(void)
{
	if (!stripesWidget)
	{
		const Vector2D& size = textWidget->GetWidgetSize();

		stripesWidget = new ImageWidget(size, &indeterminantTextureHeader, indeterminantTextureImage);
		stripesWidget->SetImageScale(size * 0.125F);
		stripesWidget->Disable();

		textWidget->AppendNewSubnode(stripesWidget);
		textWidget->SetText(nullptr);
	}
}

bool TextSettingInterface::ExtractCurrentSetting(void)
{
	SettingInterface::ExtractCurrentSetting();

	if (!stripesWidget)
	{
		static_cast<TextSetting *>(GetSettingData())->SetText(textWidget->GetText());
		return (true);
	}

	return (false);
}


MenuSetting::MenuSetting() : Setting(kSettingMenu)
{
}

MenuSetting::MenuSetting(Type identifier, int32 selection, const char *title, int32 itemCount) : Setting(kSettingMenu, identifier)
{
	menuSelection = selection;

	SetSettingInterface(new MenuSettingInterface(this, title, itemCount));
}

MenuSetting::MenuSetting(const MenuSetting& menuSetting) : Setting(menuSetting)
{
	menuSelection = menuSetting.menuSelection;
}

MenuSetting::~MenuSetting()
{
}

Setting *MenuSetting::Replicate(void) const
{
	return (new MenuSetting(*this));
}

void MenuSetting::Pack(Packer& data, unsigned_int32 packFlags) const
{
	Setting::Pack(data, packFlags);

	data << menuSelection;
}

void MenuSetting::Unpack(Unpacker& data, unsigned_int32 unpackFlags)
{
	Setting::Unpack(data, unpackFlags);

	data >> menuSelection;
}

void MenuSetting::Compress(Compressor& data) const
{
	Setting::Compress(data);

	data << menuSelection;
}

bool MenuSetting::Decompress(Decompressor& data)
{
	Setting::Decompress(data);

	data >> menuSelection;
	return (true);
}

void MenuSetting::Copy(const Setting *setting)
{
	menuSelection = static_cast<const MenuSetting *>(setting)->menuSelection;
}

bool MenuSetting::operator ==(const Setting& setting) const
{
	if (setting.GetSettingType() == kSettingMenu)
	{
		const MenuSetting *menuSetting = static_cast<const MenuSetting *>(&setting);
		return (menuSetting->GetMenuSelection() == menuSelection);
	}

	return (false);
}

bool MenuSetting::SetValue(const Value *value)
{
	switch (value->GetValueType())
	{
		case kValueBoolean:

			menuSelection = static_cast<const BooleanValue *>(value)->GetValue();
			return (true);

		case kValueInteger:

			menuSelection = static_cast<const IntegerValue *>(value)->GetValue();
			return (true);

		case kValueFloat:

			menuSelection = (int32) static_cast<const FloatValue *>(value)->GetValue();
			return (true);

		case kValueString:

			menuSelection = Text::StringToInteger(static_cast<const StringValue *>(value)->GetValue());
			return (true);
	}

	return (false);
}


MenuSettingInterface::MenuSettingInterface(MenuSetting *setting, const char *title, int32 itemCount) :
		SettingInterface(setting, title),
		menuWidgetObserver(this, &MenuSettingInterface::HandleMenuWidgetEvent)
{
	menuItemCount = itemCount;
	menuString = new const char *[itemCount];
}

MenuSettingInterface::~MenuSettingInterface()
{
	delete[] menuString;
}

void MenuSettingInterface::HandleMenuWidgetEvent(Widget *widget, const WidgetEventData *eventData)
{
	if (eventData->eventType == kEventWidgetChange)
	{
		PostEvent();
	}
}

void MenuSettingInterface::BuildInterface(Widget *group, const ConfigurationWidget *configWidget)
{
	SettingInterface::BuildInterface(group, configWidget);

	menuWidget = new PopupMenuWidget(Vector2D(configWidget->GetValueColumnWidth(), 13.0F), "font/Gui");
	menuWidget->SetWidgetPosition(Point3D(configWidget->GetValueColumnPosition(), 0.0F, 0.0F));
	menuWidget->SetPopupMenuFlags(kPopupMenuRenderPlain);
	menuWidget->SetObserver(&menuWidgetObserver);
	group->PrependSubnode(menuWidget);

	for (machine a = 0; a < menuItemCount; a++)
	{
		menuWidget->AppendMenuItem(new MenuItemWidget(menuString[a]));
	}
}

void MenuSettingInterface::UpdateInterface(const ConfigurationWidget *configWidget)
{
	SettingInterface::UpdateInterface(configWidget);

	menuWidget->SetWidgetSize(Vector2D(configWidget->GetValueColumnWidth(), 13.0F));
	menuWidget->SetWidgetPosition(Point3D(configWidget->GetValueColumnPosition(), 0.0F, 0.0F));
	menuWidget->Invalidate();
}

bool MenuSettingInterface::DeterminantValue(void) const
{
	return (menuWidget->GetSelection() != kWidgetValueIndeterminant);
}

void MenuSettingInterface::SetDeterminantValue(void)
{
	menuWidget->SetSelection(static_cast<MenuSetting *>(GetSettingData())->GetMenuSelection());
}

void MenuSettingInterface::SetIndeterminantValue(void)
{
	menuWidget->SetSelection(kWidgetValueIndeterminant);
}

bool MenuSettingInterface::ExtractCurrentSetting(void)
{
	SettingInterface::ExtractCurrentSetting();

	int32 selection = menuWidget->GetSelection();
	if (selection != kWidgetValueIndeterminant)
	{
		static_cast<MenuSetting *>(GetSettingData())->SetMenuSelection(selection);
		return (true);
	}

	return (false);
}


ColorSetting::ColorSetting(SettingType type) : Setting(type)
{
}

ColorSetting::ColorSetting(SettingType type, Type identifier, const ColorRGBA& color) : Setting(type, identifier)
{
	colorValue = color;
}

ColorSetting::ColorSetting(Type identifier, const ColorRGBA& color, const char *title, const char *picker, unsigned_int32 flags) : Setting(kSettingColor, identifier)
{
	colorValue = color;

	SetSettingInterface(new ColorSettingInterface(this, title, picker, flags));
}

ColorSetting::ColorSetting(const ColorSetting& colorSetting) : Setting(colorSetting)
{
	colorValue = colorSetting.colorValue;
}

ColorSetting::~ColorSetting()
{
}

Setting *ColorSetting::Replicate(void) const
{
	return (new ColorSetting(*this));
}

void ColorSetting::Pack(Packer& data, unsigned_int32 packFlags) const
{
	Setting::Pack(data, packFlags);

	data << colorValue;
}

void ColorSetting::Unpack(Unpacker& data, unsigned_int32 unpackFlags)
{
	Setting::Unpack(data, unpackFlags);

	data >> colorValue;
}

void ColorSetting::Compress(Compressor& data) const
{
	Setting::Compress(data);

	data << colorValue;
}

bool ColorSetting::Decompress(Decompressor& data)
{
	Setting::Decompress(data);

	data >> colorValue;
	return (true);
}

void ColorSetting::Copy(const Setting *setting)
{
	colorValue = static_cast<const ColorSetting *>(setting)->colorValue;
}

bool ColorSetting::operator ==(const Setting& setting) const
{
	if (setting.GetSettingType() == GetSettingType())
	{
		const ColorSetting *colorSetting = static_cast<const ColorSetting *>(&setting);
		return (colorSetting->GetColor() == colorValue);
	}

	return (false);
}

bool ColorSetting::SetValue(const Value *value)
{
	if (value->GetValueType() == kValueColor)
	{
		colorValue = static_cast<const ColorValue *>(value)->GetValue();
		return (true);
	}

	return (false);
}


ColorSettingInterface::ColorSettingInterface(ColorSetting *setting, const char *title, const char *picker, unsigned_int32 flags) :
		SettingInterface(setting, title),
		colorWidgetObserver(this, &ColorSettingInterface::HandleColorWidgetEvent)
{
	pickerString = picker;
	pickerFlags = flags;

	stripesWidget = nullptr;
}

ColorSettingInterface::~ColorSettingInterface()
{
}

void ColorSettingInterface::HandleColorWidgetEvent(Widget *widget, const WidgetEventData *eventData)
{
	if (eventData->eventType == kEventWidgetChange)
	{
		delete stripesWidget;
		stripesWidget = nullptr;

		PostEvent();
	}
}

void ColorSettingInterface::BuildInterface(Widget *group, const ConfigurationWidget *configWidget)
{
	SettingInterface::BuildInterface(group, configWidget);

	colorWidget = new ColorWidget(Vector2D(24.0F, 11.0F));
	colorWidget->SetWidgetPosition(Point3D(configWidget->GetValueColumnPosition() + 4.0F, 1.0F, 0.0F));
	colorWidget->SetColorPickerFlags(pickerFlags);
	colorWidget->SetColorPickerTitle(pickerString);
	colorWidget->SetObserver(&colorWidgetObserver);
	group->PrependSubnode(colorWidget);
}

void ColorSettingInterface::UpdateInterface(const ConfigurationWidget *configWidget)
{
	SettingInterface::UpdateInterface(configWidget);

	colorWidget->SetWidgetPosition(Point3D(configWidget->GetValueColumnPosition() + 4.0F, 1.0F, 0.0F));
	colorWidget->Invalidate();
}

bool ColorSettingInterface::DeterminantValue(void) const
{
	return (!stripesWidget);
}

void ColorSettingInterface::SetDeterminantValue(void)
{
	delete stripesWidget;
	stripesWidget = nullptr;

	colorWidget->SetValue(static_cast<ColorSetting *>(GetSettingData())->GetColor());
}

void ColorSettingInterface::SetIndeterminantValue(void)
{
	if (!stripesWidget)
	{
		const Vector2D& size = colorWidget->GetWidgetSize();

		stripesWidget = new ImageWidget(size, &indeterminantTextureHeader, indeterminantTextureImage);
		stripesWidget->SetImageScale(size * 0.125F);
		stripesWidget->Disable();

		colorWidget->AppendNewSubnode(stripesWidget);
		colorWidget->SetValue(K::white);
	}
}

bool ColorSettingInterface::ExtractCurrentSetting(void)
{
	SettingInterface::ExtractCurrentSetting();

	if (!stripesWidget)
	{
		static_cast<ColorSetting *>(GetSettingData())->SetColor(colorWidget->GetValue());
		return (true);
	}

	return (false);
}


CheckColorSetting::CheckColorSetting() : ColorSetting(kSettingCheckColor)
{
}

CheckColorSetting::CheckColorSetting(Type identifier, bool check, const ColorRGBA& color, const char *title, const char *picker, unsigned_int32 flags) : ColorSetting(kSettingCheckColor, identifier, color)
{
	checkValue = check;

	SetSettingInterface(new CheckColorSettingInterface(this, title, picker, flags));
}

CheckColorSetting::CheckColorSetting(const CheckColorSetting& checkColorSetting) : ColorSetting(checkColorSetting)
{
	checkValue = checkColorSetting.checkValue;
}

CheckColorSetting::~CheckColorSetting()
{
}

Setting *CheckColorSetting::Replicate(void) const
{
	return (new CheckColorSetting(*this));
}

void CheckColorSetting::Pack(Packer& data, unsigned_int32 packFlags) const
{
	ColorSetting::Pack(data, packFlags);

	data << checkValue;
}

void CheckColorSetting::Unpack(Unpacker& data, unsigned_int32 unpackFlags)
{
	ColorSetting::Unpack(data, unpackFlags);

	data >> checkValue;
}

void CheckColorSetting::Compress(Compressor& data) const
{
	ColorSetting::Compress(data);

	data << checkValue;
}

bool CheckColorSetting::Decompress(Decompressor& data)
{
	ColorSetting::Decompress(data);

	data >> checkValue;
	return (true);
}

void CheckColorSetting::Copy(const Setting *setting)
{
	ColorSetting::Copy(setting);
	checkValue = static_cast<const CheckColorSetting *>(setting)->checkValue;
}

bool CheckColorSetting::operator ==(const Setting& setting) const
{
	if (ColorSetting::operator ==(setting))
	{
		const CheckColorSetting *checkColorSetting = static_cast<const CheckColorSetting *>(&setting);
		return (checkColorSetting->GetCheckValue() == checkValue);
	}

	return (false);
}


CheckColorSettingInterface::CheckColorSettingInterface(CheckColorSetting *setting, const char *title, const char *picker, unsigned_int32 flags) :
		SettingInterface(setting, title),
		checkWidgetObserver(this, &CheckColorSettingInterface::HandleCheckWidgetEvent),
		colorWidgetObserver(this, &CheckColorSettingInterface::HandleColorWidgetEvent)
{
	pickerString = picker;
	pickerFlags = flags;

	stripesWidget = nullptr;
}

CheckColorSettingInterface::~CheckColorSettingInterface()
{
}

void CheckColorSettingInterface::HandleCheckWidgetEvent(Widget *widget, const WidgetEventData *eventData)
{
	if (eventData->eventType == kEventWidgetChange)
	{
		PostEvent();
	}
}

void CheckColorSettingInterface::HandleColorWidgetEvent(Widget *widget, const WidgetEventData *eventData)
{
	if (eventData->eventType == kEventWidgetChange)
	{
		delete stripesWidget;
		stripesWidget = nullptr;

		checkWidget->SetValue(1);
		PostEvent();
	}
}

void CheckColorSettingInterface::BuildInterface(Widget *group, const ConfigurationWidget *configWidget)
{
	SettingInterface::BuildInterface(group, configWidget);

	checkWidget = new CheckWidget(Vector2D(16.0F, kSettingSpacing));
	checkWidget->SetWidgetPosition(Point3D(configWidget->GetValueColumnPosition() + 4.0F, 0.0F, 0.0F));
	checkWidget->SetObserver(&checkWidgetObserver);
	group->PrependSubnode(checkWidget);

	colorWidget = new ColorWidget(Vector2D(24.0F, 11.0F));
	colorWidget->SetWidgetPosition(Point3D(configWidget->GetValueColumnPosition() + 23.0F, 1.0F, 0.0F));
	colorWidget->SetColorPickerFlags(pickerFlags);
	colorWidget->SetColorPickerTitle(pickerString);
	colorWidget->SetObserver(&colorWidgetObserver);
	group->InsertSubnodeAfter(colorWidget, checkWidget);
}

void CheckColorSettingInterface::UpdateInterface(const ConfigurationWidget *configWidget)
{
	SettingInterface::UpdateInterface(configWidget);

	colorWidget->SetWidgetPosition(Point3D(configWidget->GetValueColumnPosition() + 23.0F, 1.0F, 0.0F));
	colorWidget->Invalidate();
}

bool CheckColorSettingInterface::DeterminantValue(void) const
{
	return (checkWidget->GetValue() != kWidgetValueIndeterminant);
}

void CheckColorSettingInterface::SetDeterminantValue(void)
{
	delete stripesWidget;
	stripesWidget = nullptr;

	checkWidget->SetValue(static_cast<CheckColorSetting *>(GetSettingData())->GetCheckValue());
	colorWidget->SetValue(static_cast<CheckColorSetting *>(GetSettingData())->GetColor());
}

void CheckColorSettingInterface::SetIndeterminantValue(void)
{
	checkWidget->SetValue(kWidgetValueIndeterminant);

	if (!stripesWidget)
	{
		const Vector2D& size = colorWidget->GetWidgetSize();

		stripesWidget = new ImageWidget(size, &indeterminantTextureHeader, indeterminantTextureImage);
		stripesWidget->SetImageScale(size * 0.125F);
		stripesWidget->Disable();

		colorWidget->AppendNewSubnode(stripesWidget);
		colorWidget->SetValue(K::white);
	}
}

bool CheckColorSettingInterface::ExtractCurrentSetting(void)
{
	SettingInterface::ExtractCurrentSetting();

	CheckColorSetting *setting = static_cast<CheckColorSetting *>(GetSettingData());

	int32 value = checkWidget->GetValue();
	if ((value == kWidgetValueIndeterminant) || (stripesWidget))
	{
		return (false);
	}

	setting->SetCheckValue(value != 0);
	setting->SetColor(colorWidget->GetValue());

	return (true);
}


FilePickerSettingInterface::FilePickerSettingInterface(Setting *setting, const char *title, const char *picker, const ResourceDescriptor *descriptor, const char *subdir, unsigned_int32 flags) : SettingInterface(setting, title)
{
	settingFlags = flags;
	pickerString = picker;
	resourceDescriptor = descriptor;

	if (subdir)
	{
		subdirectory = "/";
		subdirectory += subdir;
	}
	else
	{
		subdirectory[0] = 0;
	}

	stripesWidget = nullptr;
}

FilePickerSettingInterface::~FilePickerSettingInterface()
{
}

void FilePickerSettingInterface::UpdateInterface(const ConfigurationWidget *configWidget)
{
	SettingInterface::UpdateInterface(configWidget);

	float valueWidth = configWidget->GetValueColumnWidth();
	float valuePosition = configWidget->GetValueColumnPosition();

	browseWidget->SetWidgetPosition(Point3D(valuePosition + valueWidth - 18.0F, -2.0F, 0.0F));
	browseWidget->Invalidate();

	resourceWidget->SetWidgetSize(Vector2D(valueWidth - 22.0F, kSettingSpacing - 5.0F));
	resourceWidget->SetWidgetPosition(Point3D(valuePosition + 2.0F, 0.0F, 0.0F));
	resourceWidget->Invalidate();
}

bool FilePickerSettingInterface::DeterminantValue(void) const
{
	return (!stripesWidget);
}

void FilePickerSettingInterface::SetIndeterminantValue(void)
{
	if (!stripesWidget)
	{
		const Vector2D& size = resourceWidget->GetWidgetSize();

		stripesWidget = new ImageWidget(size, &indeterminantTextureHeader, indeterminantTextureImage);
		stripesWidget->SetImageScale(size * 0.125F);
		stripesWidget->Disable();

		resourceWidget->AppendNewSubnode(stripesWidget);
		resourceWidget->SetText(nullptr);
	}
}


ResourceSetting::ResourceSetting() : Setting(kSettingResource)
{
}

ResourceSetting::ResourceSetting(Type identifier, const char *name, const char *title, const char *picker, const ResourceDescriptor *descriptor, const char *subdir, unsigned_int32 flags) : Setting(kSettingResource, identifier)
{
	if (name)
	{
		resourceName = name;
	}
	else
	{
		resourceName[0] = 0;
	}

	SetSettingInterface(new ResourceSettingInterface(this, title, picker, descriptor, subdir, flags));
}

ResourceSetting::ResourceSetting(const ResourceSetting& resourceSetting) : Setting(resourceSetting)
{
	resourceName = resourceSetting.resourceName;
}

ResourceSetting::~ResourceSetting()
{
}

Setting *ResourceSetting::Replicate(void) const
{
	return (new ResourceSetting(*this));
}

void ResourceSetting::Pack(Packer& data, unsigned_int32 packFlags) const
{
	Setting::Pack(data, packFlags);

	data << resourceName;
}

void ResourceSetting::Unpack(Unpacker& data, unsigned_int32 unpackFlags)
{
	Setting::Unpack(data, unpackFlags);

	data >> resourceName;
}

void ResourceSetting::Compress(Compressor& data) const
{
	Setting::Compress(data);

	data << resourceName;
}

bool ResourceSetting::Decompress(Decompressor& data)
{
	Setting::Decompress(data);

	data >> resourceName;
	return (true);
}

void ResourceSetting::Copy(const Setting *setting)
{
	resourceName = static_cast<const ResourceSetting *>(setting)->resourceName;
}

bool ResourceSetting::operator ==(const Setting& setting) const
{
	if (setting.GetSettingType() == kSettingResource)
	{
		const ResourceSetting *resourceSetting = static_cast<const ResourceSetting *>(&setting);
		return (resourceSetting->GetResourceName() == resourceName);
	}

	return (false);
}

bool ResourceSetting::SetValue(const Value *value)
{
	if (value->GetValueType() == kValueString)
	{
		resourceName = static_cast<const StringValue *>(value)->GetValue();
		return (true);
	}

	return (false);
}


ResourceSettingInterface::ResourceSettingInterface(ResourceSetting *setting, const char *title, const char *picker, const ResourceDescriptor *descriptor, const char *subdir, unsigned_int32 flags) :
		FilePickerSettingInterface(setting, title, picker, descriptor, subdir, flags),
		browseWidgetObserver(this, &ResourceSettingInterface::HandleBrowseWidgetEvent),
		resourceWidgetObserver(this, &ResourceSettingInterface::HandleResourceWidgetEvent)
{
}

ResourceSettingInterface::~ResourceSettingInterface()
{
}

void ResourceSettingInterface::HandleBrowseWidgetEvent(Widget *widget, const WidgetEventData *eventData)
{
	if (eventData->eventType == kEventWidgetActivate)
	{
		FilePicker	*picker;

		const ResourceCatalog *catalog = (settingFlags & kResourceSettingImportCatalog) ? ThePluginMgr->GetImportCatalog() : TheResourceMgr->GetGenericCatalog();
		ResourceType type = resourceDescriptor->GetType();

		if (resourceDescriptor->DescribesType("tex"))
		{
			picker = new TexturePicker(type, pickerString, catalog, resourceDescriptor, subdirectory);
		}
		else if (resourceDescriptor->DescribesType("wav"))
		{
			picker = new SoundPicker(type, pickerString, catalog, resourceDescriptor, subdirectory);
		}
		else if (resourceDescriptor->DescribesType("wld"))
		{
			picker = new WorldPicker(type, pickerString, catalog, resourceDescriptor, subdirectory);
		}
		else
		{
			picker = new FilePicker(type, pickerString, catalog, resourceDescriptor, subdirectory);
		}

		picker->SetCompletionProc(&ResourcePickProc, this);
		widget->GetOwningWindow()->AddSubwindow(picker);
	}
}

void ResourceSettingInterface::HandleResourceWidgetEvent(Widget *widget, const WidgetEventData *eventData)
{
	if (eventData->eventType == kEventWidgetChange)
	{
		delete stripesWidget;
		stripesWidget = nullptr;

		PostEvent();
	}
}

void ResourceSettingInterface::ResourcePickProc(FilePicker *picker, void *cookie)
{
	ResourceSettingInterface *settingInterface = static_cast<ResourceSettingInterface *>(cookie);

	delete settingInterface->stripesWidget;
	settingInterface->stripesWidget = nullptr;

	EditTextWidget *widget = settingInterface->resourceWidget;
	widget->GetOwningWindow()->SetFocusWidget(widget);

	if (settingInterface->settingFlags & kResourceSettingGenericPath)
	{
		widget->SetText(picker->GetFileName());
	}
	else
	{
		widget->SetText(picker->GetResourceName());
	}

	settingInterface->PostEvent();
}

void ResourceSettingInterface::BuildInterface(Widget *group, const ConfigurationWidget *configWidget)
{
	FilePickerSettingInterface::BuildInterface(group, configWidget);

	float valueWidth = configWidget->GetValueColumnWidth();
	float valuePosition = configWidget->GetValueColumnPosition();

	browseWidget = new GuiButtonWidget(Vector2D(19.0F, 19.0F), Point2D(0.5625F, 0.5F), Point2D(0.6875F, 0.625F));
	browseWidget->SetWidgetPosition(Point3D(valuePosition + valueWidth - 18.0F, -2.0F, 0.0F));
	browseWidget->SetWidgetColor(TheInterfaceMgr->GetInterfaceColor(kInterfaceColorButton));
	browseWidget->SetObserver(&browseWidgetObserver);
	group->PrependSubnode(browseWidget);

	resourceWidget = new EditTextWidget(Vector2D(valueWidth - 22.0F, kSettingSpacing - 5.0F), kMaxResourcePathLength, "font/Gui");
	resourceWidget->SetWidgetPosition(Point3D(valuePosition + 2.0F, 0.0F, 0.0F));
	resourceWidget->SetEditTextFlags(kEditTextOverflow | kEditTextChangeSelectAll | kEditTextRenderPlain);
	resourceWidget->SetObserver(&resourceWidgetObserver);
	group->InsertSubnodeAfter(resourceWidget, browseWidget);
}

void ResourceSettingInterface::SetDeterminantValue(void)
{
	delete stripesWidget;
	stripesWidget = nullptr;

	resourceWidget->SetText(static_cast<ResourceSetting *>(GetSettingData())->GetResourceName());
}

bool ResourceSettingInterface::ExtractCurrentSetting(void)
{
	SettingInterface::ExtractCurrentSetting();

	if (!stripesWidget)
	{
		static_cast<ResourceSetting *>(GetSettingData())->SetResourceName(resourceWidget->GetText());
		return (true);
	}

	return (false);
}


MultiResourceSetting::MultiResourceSetting() : Setting(kSettingMultiResource)
{
}

MultiResourceSetting::MultiResourceSetting(Type identifier, const char *title, const char *picker, const ResourceDescriptor *descriptor, const char *subdir, unsigned_int32 flags) : Setting(kSettingMultiResource, identifier)
{
	resourceList = "";

	SetSettingInterface(new MultiResourceSettingInterface(this, title, picker, descriptor, subdir, flags));
}

MultiResourceSetting::MultiResourceSetting(const MultiResourceSetting& multiResourceSetting) : Setting(multiResourceSetting)
{
	resourceList = multiResourceSetting.resourceList;
}

MultiResourceSetting::~MultiResourceSetting()
{
}

Setting *MultiResourceSetting::Replicate(void) const
{
	return (new MultiResourceSetting(*this));
}

void MultiResourceSetting::Pack(Packer& data, unsigned_int32 packFlags) const
{
	Setting::Pack(data, packFlags);

	data << resourceList;
}

void MultiResourceSetting::Unpack(Unpacker& data, unsigned_int32 unpackFlags)
{
	Setting::Unpack(data, unpackFlags);

	data >> resourceList;
}

void MultiResourceSetting::Compress(Compressor& data) const
{
	String<kMaxSettingMessageDataSize - 1>	string;

	Setting::Compress(data);

	string = resourceList;
	data << string;
}

bool MultiResourceSetting::Decompress(Decompressor& data)
{
	String<kMaxSettingMessageDataSize - 1>	string;

	Setting::Decompress(data);

	data >> string;
	resourceList = string;

	return (true);
}

void MultiResourceSetting::Copy(const Setting *setting)
{
	resourceList = static_cast<const MultiResourceSetting *>(setting)->resourceList;
}

bool MultiResourceSetting::operator ==(const Setting& setting) const
{
	if (setting.GetSettingType() == kSettingMultiResource)
	{
		const MultiResourceSetting *multiResourceSetting = static_cast<const MultiResourceSetting *>(&setting);
		return (multiResourceSetting->GetResourceList() == resourceList);
	}

	return (false);
}

bool MultiResourceSetting::SetValue(const Value *value)
{
	if (value->GetValueType() == kValueString)
	{
		resourceList = static_cast<const StringValue *>(value)->GetValue();
		return (true);
	}

	return (false);
}

void MultiResourceSetting::AddResourceName(const char *name)
{
	if (resourceList[0] != 0)
	{
		resourceList += ';';
	}

	resourceList += name;
}

int32 MultiResourceSetting::GetResourceCount(void) const
{
	const char *s = resourceList;
	if (s[0] != 0)
	{
		int32 count = 1;
		for (machine a = 0;; a++)
		{
			char c = s[a];
			if (c == 0)
			{
				break;
			}

			if (c == ';')
			{
				count++;
			}
		}

		return (count);
	}

	return (0);
}

ResourceName MultiResourceSetting::GetResourceName(int32 index) const
{
	ResourceName name("");

	const char *text = resourceList;

	int32 count = 0;
	while (count < index)
	{
		for (machine a = 0;; a++)
		{
			char c = text[a];
			if (c == 0)
			{
				return (name);
			}

			if (c == ';')
			{
				count++;
				text += a + 1;
				break;
			}
		}
	}

	while (*text == 32)
	{
		text++;
	}

	int32 len = Text::FindChar(text, ';');
	if (len >= 0)
	{
		name.Set(text, len);
	}
	else
	{
		name = text;
	}

	return (name);
}


MultiResourceSettingInterface::MultiResourceSettingInterface(MultiResourceSetting *setting, const char *title, const char *picker, const ResourceDescriptor *descriptor, const char *subdir, unsigned_int32 flags) :
		FilePickerSettingInterface(setting, title, picker, descriptor, subdir, flags),
		browseWidgetObserver(this, &MultiResourceSettingInterface::HandleBrowseWidgetEvent),
		resourceWidgetObserver(this, &MultiResourceSettingInterface::HandleResourceWidgetEvent)
{
}

MultiResourceSettingInterface::~MultiResourceSettingInterface()
{
}

void MultiResourceSettingInterface::HandleBrowseWidgetEvent(Widget *widget, const WidgetEventData *eventData)
{
	if (eventData->eventType == kEventWidgetActivate)
	{
		FilePicker	*picker;

		const ResourceCatalog *catalog = (settingFlags & kResourceSettingImportCatalog) ? ThePluginMgr->GetImportCatalog() : TheResourceMgr->GetGenericCatalog();
		ResourceType type = resourceDescriptor->GetType();

		if (resourceDescriptor->DescribesType("tex"))
		{
			picker = new TexturePicker(type, pickerString, catalog, resourceDescriptor, subdirectory);
		}
		else if (resourceDescriptor->DescribesType("wav"))
		{
			picker = new SoundPicker(type, pickerString, catalog, resourceDescriptor, subdirectory);
		}
		else if (resourceDescriptor->DescribesType("wld"))
		{
			picker = new WorldPicker(type, pickerString, catalog, resourceDescriptor, subdirectory);
		}
		else
		{
			picker = new FilePicker(type, pickerString, catalog, resourceDescriptor, subdirectory);
		}

		picker->SetCompletionProc(&ResourcePickProc, this);
		widget->GetOwningWindow()->AddSubwindow(picker);
	}
}

void MultiResourceSettingInterface::HandleResourceWidgetEvent(Widget *widget, const WidgetEventData *eventData)
{
	if (eventData->eventType == kEventWidgetChange)
	{
		delete stripesWidget;
		stripesWidget = nullptr;

		PostEvent();
	}
}

void MultiResourceSettingInterface::ResourcePickProc(FilePicker *picker, void *cookie)
{
	MultiResourceSettingInterface *settingInterface = static_cast<MultiResourceSettingInterface *>(cookie);

	delete settingInterface->stripesWidget;
	settingInterface->stripesWidget = nullptr;

	String<> string(settingInterface->resourceWidget->GetText());
	if (string.Length() > 0)
	{
		string += ';';
	}

	if (settingInterface->settingFlags & kResourceSettingGenericPath)
	{
		string += picker->GetFileName();
	}
	else
	{
		string += picker->GetResourceName();
	}

	EditTextWidget *widget = settingInterface->resourceWidget;
	widget->GetOwningWindow()->SetFocusWidget(widget);
	widget->SetText(string);

	settingInterface->PostEvent();
}

void MultiResourceSettingInterface::BuildInterface(Widget *group, const ConfigurationWidget *configWidget)
{
	FilePickerSettingInterface::BuildInterface(group, configWidget);

	float valueWidth = configWidget->GetValueColumnWidth();
	float valuePosition = configWidget->GetValueColumnPosition();

	browseWidget = new GuiButtonWidget(Vector2D(19.0F, 19.0F), Point2D(0.5625F, 0.5F), Point2D(0.6875F, 0.625F));
	browseWidget->SetWidgetPosition(Point3D(valuePosition + valueWidth - 18.0F, -2.0F, 0.0F));
	browseWidget->SetWidgetColor(TheInterfaceMgr->GetInterfaceColor(kInterfaceColorButton));
	browseWidget->SetObserver(&browseWidgetObserver);
	group->PrependSubnode(browseWidget);

	resourceWidget = new EditTextWidget(Vector2D(valueWidth - 22.0F, kSettingSpacing - 5.0F), 1023, "font/Gui");
	resourceWidget->SetWidgetPosition(Point3D(valuePosition + 2.0F, 0.0F, 0.0F));
	resourceWidget->SetEditTextFlags(kEditTextOverflow | kEditTextChangeSelectAll | kEditTextRenderPlain);
	resourceWidget->SetObserver(&resourceWidgetObserver);
	group->InsertSubnodeAfter(resourceWidget, browseWidget);
}

void MultiResourceSettingInterface::SetDeterminantValue(void)
{
	delete stripesWidget;
	stripesWidget = nullptr;

	resourceWidget->SetText(static_cast<MultiResourceSetting *>(GetSettingData())->GetResourceList());
}

bool MultiResourceSettingInterface::ExtractCurrentSetting(void)
{
	SettingInterface::ExtractCurrentSetting();

	if (!stripesWidget)
	{
		static_cast<MultiResourceSetting *>(GetSettingData())->SetResourceList(resourceWidget->GetText());
		return (true);
	}

	return (false);
}


ConfigurationWidget::ConfigurationWidget() :
		RenderableWidget(kWidgetConfiguration, kRenderQuads),
		scrollObserver(this, &ConfigurationWidget::HandleScrollEvent),
		configurationVertexBuffer(kVertexBufferAttribute | kVertexBufferDynamic),
		borderVertexBuffer(kVertexBufferAttribute | kVertexBufferDynamic),
		borderTextureAttribute(&LineWidget::lineTextureHeader, LineWidget::lineTextureImage),
		borderRenderable(kRenderQuads),
		scrollWidget(Zero2D)
{
	colorOverrideFlags = 0;
	lineColor.Set(0.0F, 0.0F, 0.0F);
	backgroundColor.Set(1.0F, 1.0F, 1.0F);

	configurationSettingCount = 0;
	displaySettingIndex = 0;
	settingObserver = nullptr;

	SetActiveUpdateFlags(GetActiveUpdateFlags() | kUpdateStructure);
	SetWidgetUsage(kWidgetMouseWheel);

	SetDefaultColorType(kWidgetColorBorder);
	RenderableWidget::SetWidgetColor(K::black);
}

ConfigurationWidget::ConfigurationWidget(const Vector2D& size, float titleFraction, unsigned_int32 flags) :
		RenderableWidget(kWidgetConfiguration, kRenderQuads, size),
		scrollObserver(this, &ConfigurationWidget::HandleScrollEvent),
		configurationVertexBuffer(kVertexBufferAttribute | kVertexBufferDynamic),
		borderVertexBuffer(kVertexBufferAttribute | kVertexBufferDynamic),
		borderTextureAttribute(&LineWidget::lineTextureHeader, LineWidget::lineTextureImage),
		borderRenderable(kRenderQuads),
		scrollWidget(Vector2D(16.0F, size.y))
{
	configurationFlags = 0;
	colorOverrideFlags = 0;
	titleColumnFraction = titleFraction;
	lineColor.Set(0.0F, 0.0F, 0.0F);
	backgroundColor.Set(1.0F, 1.0F, 1.0F);

	configurationSettingCount = 0;
	displaySettingIndex = 0;
	settingObserver = nullptr;

	SetActiveUpdateFlags(GetActiveUpdateFlags() | kUpdateStructure);
	SetWidgetUsage(kWidgetMouseWheel);

	SetDefaultColorType(kWidgetColorBorder);
	RenderableWidget::SetWidgetColor(K::black);
}

ConfigurationWidget::ConfigurationWidget(const ConfigurationWidget& configurationWidget) :
		RenderableWidget(configurationWidget),
		scrollObserver(this, &ConfigurationWidget::HandleScrollEvent),
		configurationVertexBuffer(kVertexBufferAttribute | kVertexBufferDynamic),
		borderVertexBuffer(kVertexBufferAttribute | kVertexBufferDynamic),
		borderColorAttribute(kAttributeMutable),
		borderTextureAttribute(&LineWidget::lineTextureHeader, LineWidget::lineTextureImage),
		borderRenderable(kRenderQuads),
		scrollWidget(configurationWidget.scrollWidget.GetWidgetSize())
{
	configurationFlags = configurationWidget.configurationFlags;
	colorOverrideFlags = configurationWidget.colorOverrideFlags;
	titleColumnFraction = configurationWidget.titleColumnFraction;
	lineColor = configurationWidget.lineColor;
	backgroundColor = configurationWidget.backgroundColor;

	configurationSettingCount = 0;
	displaySettingIndex = 0;
	settingObserver = nullptr;

	SetActiveUpdateFlags(GetActiveUpdateFlags() | kUpdateStructure);
}

ConfigurationWidget::~ConfigurationWidget()
{
}

Widget *ConfigurationWidget::Replicate(void) const
{
	return (new ConfigurationWidget(*this));
}

void ConfigurationWidget::Pack(Packer& data, unsigned_int32 packFlags) const
{
	RenderableWidget::Pack(data, packFlags);

	data << ChunkHeader('FLAG', 4);
	data << configurationFlags;

	data << ChunkHeader('TFRC', 4);
	data << titleColumnFraction;

	data << ChunkHeader('OVER', 4);
	data << colorOverrideFlags;

	if (colorOverrideFlags & kWidgetOverrideLineColor)
	{
		data << ChunkHeader('LINC', sizeof(ColorRGBA));
		data << lineColor;
	}

	data << ChunkHeader('BGDC', sizeof(ColorRGBA));
	data << backgroundColor;

	data << TerminatorChunk;
}

void ConfigurationWidget::Unpack(Unpacker& data, unsigned_int32 unpackFlags)
{
	RenderableWidget::Unpack(data, unpackFlags);
	UnpackChunkList<ConfigurationWidget>(data, unpackFlags);
}

bool ConfigurationWidget::UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags)
{
	switch (chunkHeader->chunkType)
	{
		case 'FLAG':

			data >> configurationFlags;
			return (true);

		case 'TFRC':

			data >> titleColumnFraction;
			return (true);

		case 'OVER':

			data >> colorOverrideFlags;
			return (true);

		case 'LINC':

			data >> lineColor;
			return (true);

		case 'BGDC':

			data >> backgroundColor;
			return (true);
	}

	return (false);
}

void *ConfigurationWidget::BeginSettingsUnpack(void)
{
	colorOverrideFlags = 0;
	return (RenderableWidget::BeginSettingsUnpack());
}

int32 ConfigurationWidget::GetSettingCount(void) const
{
	return (RenderableWidget::GetSettingCount() + 5);
}

Setting *ConfigurationWidget::GetSetting(int32 index) const
{
	int32 count = RenderableWidget::GetSettingCount();
	if (index < count)
	{
		return (RenderableWidget::GetSetting(index));
	}

	const StringTable *table = TheInterfaceMgr->GetStringTable();

	if (index == count)
	{
		const char *title = table->GetString(StringID('WDGT', kWidgetConfiguration, 'SETT'));
		return (new HeadingSetting(kWidgetConfiguration, title));
	}

	if (index == count + 1)
	{
		const char *title = table->GetString(StringID('WDGT', kWidgetConfiguration, 'TFRC'));
		return (new FloatSetting('TFRC', titleColumnFraction, title, 0.1F, 0.9F, 0.05F));
	}

	if (index == count + 2)
	{
		const char *title = table->GetString(StringID('WDGT', kWidgetConfiguration, 'SCPT'));
		return (new BooleanSetting('SCPT', ((configurationFlags & kConfigurationScript) != 0), title));
	}

	if (index == count + 3)
	{
		const char *title = table->GetString(StringID('WDGT', kWidgetConfiguration, 'LINC'));
		const char *picker = table->GetString(StringID('WDGT', kWidgetConfiguration, 'LINP'));
		return (new CheckColorSetting('CFLC', ((colorOverrideFlags & kWidgetOverrideLineColor) != 0), lineColor, title, picker));
	}

	if (index == count + 4)
	{
		const char *title = table->GetString(StringID('WDGT', kWidgetConfiguration, 'BGDC'));
		const char *picker = table->GetString(StringID('WDGT', kWidgetConfiguration, 'BGDP'));
		return (new ColorSetting('CFBG', backgroundColor, title, picker, kColorPickerAlpha));
	}

	return (nullptr);
}

void ConfigurationWidget::SetSetting(const Setting *setting)
{
	Type identifier = setting->GetSettingIdentifier();

	if (identifier == 'TFRC')
	{
		titleColumnFraction = static_cast<const FloatSetting *>(setting)->GetFloatValue();
	}
	else if (identifier == 'SCPT')
	{
		if (static_cast<const BooleanSetting *>(setting)->GetBooleanValue())
		{
			configurationFlags |= kConfigurationScript;
		}
		else
		{
			configurationFlags &= ~kConfigurationScript;
		}

		CalculateColumnWidths();
	}
	else if (identifier == 'CFLC')
	{
		const CheckColorSetting *checkColorSetting = static_cast<const CheckColorSetting *>(setting);
		if (checkColorSetting->GetCheckValue() != 0)
		{
			colorOverrideFlags |= kWidgetOverrideHiliteColor;
			lineColor = checkColorSetting->GetColor();
		}
		else
		{
			colorOverrideFlags &= ~kWidgetOverrideHiliteColor;
			SetDefaultLineColor();
		}
	}
	else if (identifier == 'CFBG')
	{
		backgroundColor = static_cast<const ColorSetting *>(setting)->GetColor();
	}
	else
	{
		RenderableWidget::SetSetting(setting);
	}
}

const ColorRGBA& ConfigurationWidget::GetWidgetColor(WidgetColorType type) const
{
	if (type == kWidgetColorLine)
	{
		return (lineColor);
	}

	if (type == kWidgetColorBackground)
	{
		return (backgroundColor);
	}

	return (RenderableWidget::GetWidgetColor(type));
}

void ConfigurationWidget::SetWidgetColor(const ColorRGBA& color, WidgetColorType type)
{
	if (type == kWidgetColorLine)
	{
		lineColor = color;
		colorOverrideFlags |= kWidgetOverrideLineColor;
	}
	else if (type == kWidgetColorBackground)
	{
		backgroundColor = color;
	}

	RenderableWidget::SetWidgetColor(color, type);
}

void ConfigurationWidget::SetWidgetAlpha(float alpha, WidgetColorType type)
{
	if (type == kWidgetColorBackground)
	{
		backgroundColor.alpha = alpha;
	}

	RenderableWidget::SetWidgetAlpha(alpha, type);
}

void ConfigurationWidget::SetDynamicWidgetColor(const ColorRGBA& color, WidgetColorType type)
{
	if (type == kWidgetColorLine)
	{
		dynamicLineColor = color;
		SetBuildFlag();
	}
	else if (type == kWidgetColorBackground)
	{
		dynamicBackgroundColor = color;
		SetBuildFlag();
	}
	else
	{
		RenderableWidget::SetDynamicWidgetColor(color, type);
	}
}

void ConfigurationWidget::SetDynamicWidgetAlpha(float alpha, WidgetColorType type)
{
	if (type == kWidgetColorLine)
	{
		dynamicLineColor.alpha = alpha;
		SetBuildFlag();
	}
	else if (type == kWidgetColorBackground)
	{
		dynamicBackgroundColor.alpha = alpha;
		SetBuildFlag();
	}
	else
	{
		RenderableWidget::SetDynamicWidgetAlpha(alpha, type);
	}
}

void ConfigurationWidget::SetDefaultLineColor(void)
{
	lineColor = TheInterfaceMgr->GetInterfaceColor(kInterfaceColorWindowFrame).GetColorRGB() * 0.5F + ColorRGB(0.5F, 0.5F, 0.5F);
}

void ConfigurationWidget::CalculateColumnWidths(void)
{
	float width = FmaxZero(GetWidgetSize().x - 16.0F);
	fullSettingWidth = width;

	if (configurationFlags & kConfigurationScript)
	{
		width -= kValueNameWidth;
	}

	titleColumnWidth = PositiveFloor(width * titleColumnFraction);
	valueColumnPosition = titleColumnWidth + 1.0F;
	valueColumnWidth = FmaxZero(width - valueColumnPosition);
}

void ConfigurationWidget::SetWidgetSize(const Vector2D& size)
{
	RenderableWidget::SetWidgetSize(size);

	scrollWidget.SetWidgetSize(Vector2D(16.0F, size.y));
	scrollWidget.SetWidgetPosition(Point3D(FmaxZero(size.x - 16.0F), 0.0F, 0.0F));

	CalculateColumnWidths();
	Invalidate();

	const Setting *setting = settingList.First();
	while (setting)
	{
		SettingInterface *settingInterface = setting->GetSettingInterface();
		if (settingInterface)
		{
			settingInterface->UpdateInterface(this);
		}

		setting = setting->Next();
	}
}

void ConfigurationWidget::Preprocess(void)
{
	if (!(colorOverrideFlags & kWidgetOverrideLineColor))
	{
		SetDefaultLineColor();
	}

	dynamicLineColor = lineColor;
	dynamicBackgroundColor = backgroundColor;

	settingGroup.SetWidgetState(settingGroup.GetWidgetState() | kWidgetNonpersistent);
	AppendSubnode(&settingGroup);

	scrollWidget.SetWidgetSize(Vector2D(16.0F, GetWidgetSize().y));
	scrollWidget.SetWidgetPosition(Point3D(FmaxZero(GetWidgetSize().x - 16.0F), 0.0F, 0.0F));
	scrollWidget.SetWidgetState(scrollWidget.GetWidgetState() | kWidgetNonpersistent);
	scrollWidget.SetObserver(&scrollObserver);
	AppendSubnode(&scrollWidget);

	CalculateColumnWidths();
	RenderableWidget::Preprocess();

	SetVertexBuffer(kVertexBufferAttributeArray, &configurationVertexBuffer, sizeof(ConfigurationVertex));
	SetVertexAttributeArray(kArrayPosition, 0, 2);
	SetVertexAttributeArray(kArrayColor, sizeof(Point2D), 4);

	InitRenderable(&borderRenderable);
	borderRenderable.SetVertexCount(16);
	borderRenderable.SetVertexBuffer(kVertexBufferAttributeArray, &borderVertexBuffer, sizeof(BorderVertex));
	borderRenderable.SetVertexAttributeArray(kArrayPosition, 0, 2);
	borderRenderable.SetVertexAttributeArray(kArrayTexcoord, sizeof(Point2D), 2);
	borderVertexBuffer.Establish(sizeof(BorderVertex) * 16);

	borderAttributeList.Append(&borderColorAttribute);
	borderAttributeList.Append(&borderTextureAttribute);
	borderRenderable.SetMaterialAttributeList(&borderAttributeList);
}

void ConfigurationWidget::HandleStructureUpdate(void)
{
	displaySettingCount = (int32) ((GetWidgetSize().y + 1.0F) / kSettingSpacing);
	scrollWidget.SetPageDistance(Max(displaySettingCount - 1, 1));

	int32 maxIndex = MaxZero(configurationSettingCount - displaySettingCount);
	scrollWidget.SetMaxValue(maxIndex);

	displaySettingIndex = Min(displaySettingIndex, maxIndex);
	scrollWidget.SetValue(displaySettingIndex);

	int32 vertexCount = MaxZero(displaySettingCount - 1) * 4 + 8;
	if (configurationFlags & kConfigurationScript)
	{
		vertexCount += 4;
	}

	SetVertexCount(vertexCount);
	configurationVertexBuffer.Establish(sizeof(ConfigurationVertex) * vertexCount);
	InvalidateVertexData();

	float y = 2.0F;
	machine index = 0;

	Widget *widget = settingGroup.GetFirstSubnode();
	while (widget)
	{
		if ((unsigned_int32) (index - displaySettingIndex) < (unsigned_int32) displaySettingCount)
		{
			widget->Show();
		}
		else
		{
			widget->Hide();
		}

		widget->SetWidgetPosition(Point3D(0.0F, y, 0.0F));

		y += kSettingSpacing;
		index++;

		widget = widget->Next();
	}

	settingGroup.SetWidgetPosition(Point3D(0.0F, -kSettingSpacing * (float) displaySettingIndex, 0.0F));
	settingGroup.Invalidate();

	SetBuildFlag();
}

void ConfigurationWidget::Build(void)
{
	borderColorAttribute.SetDiffuseColor(RenderableWidget::GetWidgetColor());

	volatile ConfigurationVertex *restrict configurationVertex = configurationVertexBuffer.BeginUpdate<ConfigurationVertex>();

	float w = GetFullSettingWidth();
	float h = GetWidgetSize().y;

	configurationVertex[0].position.Set(0.0F, 0.0F);
	configurationVertex[1].position.Set(0.0F, h);
	configurationVertex[2].position.Set(w, h);
	configurationVertex[3].position.Set(w, 0.0F);

	for (machine a = 0; a < 4; a++)
	{
		configurationVertex[a].color = dynamicBackgroundColor;
	}

	configurationVertex += 4;
	float x = w;

	if (configurationFlags & kConfigurationScript)
	{
		x -= kValueNameWidth;

		configurationVertex[0].position.Set(x, 0.0F);
		configurationVertex[1].position.Set(x, h);
		configurationVertex[2].position.Set(x + 1.0F, h);
		configurationVertex[3].position.Set(x + 1.0F, 0.0F);

		for (machine a = 0; a < 4; a++)
		{
			configurationVertex[a].color = dynamicLineColor;
		}

		configurationVertex += 4;
	}

	x = GetTitleColumnWidth();

	configurationVertex[0].position.Set(x, 0.0F);
	configurationVertex[1].position.Set(x, h);
	configurationVertex[2].position.Set(x + 1.0F, h);
	configurationVertex[3].position.Set(x + 1.0F, 0.0F);

	for (machine a = 0; a < 4; a++)
	{
		configurationVertex[a].color = dynamicLineColor;
	}

	configurationVertex += 4;
	float y = kSettingSpacing - 1.0F;

	for (machine a = 1; a < displaySettingCount; a++)
	{
		configurationVertex[0].position.Set(0.0F, y);
		configurationVertex[1].position.Set(0.0F, y + 1.0F);
		configurationVertex[2].position.Set(w, y + 1.0F);
		configurationVertex[3].position.Set(w, y);

		for (machine b = 0; b < 4; b++)
		{
			configurationVertex[b].color = dynamicLineColor;
		}

		configurationVertex += 4;
		y += kSettingSpacing;
	}

	configurationVertexBuffer.EndUpdate();

	Box2D box(Zero2D, Point2D(Fmax(GetWidgetSize().x, 16.0F), h));

	volatile BorderVertex *restrict borderVertex = borderVertexBuffer.BeginUpdate<BorderVertex>();
	BuildBorder(box, borderVertex);
	borderVertexBuffer.EndUpdate();
}

void ConfigurationWidget::Render(List<Renderable> *renderList)
{
	RenderableWidget::Render(renderList);
	renderList->Append(&borderRenderable);
}

void ConfigurationWidget::HandleScrollEvent(Widget *widget, const WidgetEventData *eventData)
{
	if (eventData->eventType == kEventWidgetChange)
	{
		displaySettingIndex = static_cast<ScrollWidget *>(widget)->GetValue();
		Invalidate();
	}
}

void ConfigurationWidget::HandleMouseEvent(const PanelMouseEventData *eventData)
{
	if (eventData->eventType == kEventMouseWheel)
	{
		int32 maxIndex = configurationSettingCount - displaySettingCount;
		if (maxIndex > 0)
		{
			int32 index = Min(MaxZero(displaySettingIndex - (int32) eventData->wheelDelta.y), maxIndex);
			if (index != displaySettingIndex)
			{
				displaySettingIndex = index;
				Invalidate();
			}
		}
	}
}

void ConfigurationWidget::SetObserver(SettingInterface::ObserverType *observer)
{
	settingObserver = observer;

	Setting *setting = settingList.First();
	while (setting)
	{
		setting->GetSettingInterface()->SetObserver(observer);
		setting = setting->Next();
	}
}

Setting *ConfigurationWidget::FindSetting(Type identifier) const
{
	Setting *setting = settingList.First();
	while (setting)
	{
		if (setting->GetSettingIdentifier() == identifier)
		{
			return (setting);
		}

		setting = setting->Next();
	}

	return (nullptr);
}

void ConfigurationWidget::BuildConfiguration(const Configurable *configurable)
{
	Setting *previousSetting = nullptr;
	int32 settingCount = configurable->GetSettingCount();
	for (machine a = 0; a < settingCount; a++)
	{
		Setting *setting = configurable->GetSetting(a);
		if (setting)
		{
			Type identifier = setting->GetSettingIdentifier();
			Setting *existingSetting = settingList.First();
			while (existingSetting)
			{
				if (existingSetting->GetSettingIdentifier() == identifier)
				{
					if (!(*existingSetting == *setting))
					{
						existingSetting->GetSettingInterface()->SetIndeterminantValue();
					}

					previousSetting = existingSetting;
					delete setting;
					goto next;
				}

				existingSetting = existingSetting->Next();
			}

			Widget *subgroup = new Widget;
			setting->GetSettingInterface()->BuildInterface(subgroup, this);

			if (previousSetting)
			{
				settingList.InsertAfter(setting, previousSetting);
				settingGroup.InsertSubnodeAfter(subgroup, previousSetting->GetSettingInterface()->settingGroup);
				subgroup->Preprocess();
			}
			else
			{
				settingList.Append(setting);
				settingGroup.AppendNewSubnode(subgroup);
			}

			previousSetting = setting;
		}

		next:;
	}

	int32 totalCount = 0;
	SettingInterface::ObserverType *observer = settingObserver;

	const Setting *setting = settingList.First();
	while (setting)
	{
		totalCount++;

		SettingInterface *settingInterface = setting->GetSettingInterface();
		settingInterface->UpdateCurrentSetting();

		if (observer)
		{
			settingInterface->SetObserver(observer);
		}

		setting = setting->Next();
	}

	configurationSettingCount = totalCount;
	displaySettingIndex = 0;

	Invalidate();
}

void ConfigurationWidget::CommitConfiguration(Configurable *configurable) const
{
	void *cookie = configurable->BeginSettings();

	Setting *setting = settingList.First();
	while (setting)
	{
		if (setting->GetSettingInterface()->ExtractCurrentSetting())
		{
			configurable->SetSetting(setting);
		}

		setting = setting->Next();
	}

	configurable->EndSettings(cookie);
}

void ConfigurationWidget::BuildCategoryConfiguration(const Configurable *configurable, Type category)
{
	Setting *previousSetting = nullptr;
	int32 settingCount = configurable->GetCategorySettingCount(category);
	for (machine a = 0; a < settingCount; a++)
	{
		Setting *setting = configurable->GetCategorySetting(category, a, configurationFlags);
		if (setting)
		{
			Type identifier = setting->GetSettingIdentifier();
			Setting *existingSetting = settingList.First();
			while (existingSetting)
			{
				if (existingSetting->GetSettingIdentifier() == identifier)
				{
					if (!(*existingSetting == *setting))
					{
						existingSetting->GetSettingInterface()->SetIndeterminantValue();
					}

					previousSetting = existingSetting;
					delete setting;
					goto next;
				}

				existingSetting = existingSetting->Next();
			}

			Widget *subgroup = new Widget;
			setting->GetSettingInterface()->BuildInterface(subgroup, this);

			if (previousSetting)
			{
				settingList.InsertAfter(setting, previousSetting);
				settingGroup.InsertSubnodeAfter(subgroup, previousSetting->GetSettingInterface()->settingGroup);
				subgroup->Preprocess();
			}
			else
			{
				settingList.Append(setting);
				settingGroup.AppendNewSubnode(subgroup);
			}

			previousSetting = setting;
		}

		next:;
	}

	int32 totalCount = 0;
	SettingInterface::ObserverType *observer = settingObserver;

	const Setting *setting = settingList.First();
	while (setting)
	{
		totalCount++;

		SettingInterface *settingInterface = setting->GetSettingInterface();
		settingInterface->UpdateCurrentSetting();

		if (observer)
		{
			settingInterface->SetObserver(observer);
		}

		setting = setting->Next();
	}

	configurationSettingCount = totalCount;
	displaySettingIndex = 0;

	Invalidate();
}

void ConfigurationWidget::CommitCategoryConfiguration(Configurable *configurable, Type category) const
{
	void *cookie = configurable->BeginSettings();

	Setting *setting = settingList.First();
	while (setting)
	{
		if (setting->GetSettingInterface()->ExtractCurrentSetting())
		{
			configurable->SetCategorySetting(category, setting);
		}

		setting = setting->Next();
	}

	configurable->EndSettings(cookie);
}

void ConfigurationWidget::ReleaseConfiguration(void)
{
	settingGroup.PurgeSubtree();
	settingList.Purge();

	configurationSettingCount = 0;
	displaySettingIndex = 0;

	Invalidate();
}

// ZYUQURM
