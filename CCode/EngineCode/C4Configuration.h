 

#ifndef C4Configuration_h
#define C4Configuration_h


//# \component	Interface Manager
//# \prefix		InterfaceMgr/

//# \import		C4ColorPicker.h


#include "C4Configurable.h"
#include "C4FilePicker.h"
#include "C4ColorPicker.h"
#include "C4Messages.h"


namespace C4
{
	//# \tree	Setting
	//
	//# \node	HeadingSetting
	//# \node	BooleanSetting
	//# \node	IntegerSetting
	//# \node	PowerTwoSetting
	//# \node	FloatSetting
	//# \node	TextSetting
	//# \node	MenuSetting
	//# \node	ColorSetting
	//# \sub
	//#		\node	CheckColorSetting
	//# \end
	//# \node	ResourceSetting
	//# \node	MultiResourceSetting


	enum
	{
		kMaxValueNameLength		= 15
	};


	typedef Type							SettingType;
	typedef String<kMaxValueNameLength>		ValueName;


	//# \enum	SettingType

	enum : SettingType
	{
		kSettingHeading					= 'HEAD',	//## Setting that simply displays a heading and has no value.
		kSettingInfo					= 'INFO',	//## Setting that displays information that cannot be edited.
		kSettingBoolean					= 'BOOL',	//## Boolean setting represented by a check box.
		kSettingInteger					= 'INT ',	//## Integer setting represented by a slider.
		kSettingPowerTwo				= 'POW2',	//## Power-of-two setting represented by a slider.
		kSettingFloat					= 'FLOT',	//## Floating-point setting represented by a slider.
		kSettingText					= 'TEXT',	//## Text setting represented by an editable text box.
		kSettingMenu					= 'MENU',	//## Multi-valued setting represented by a popup menu.
		kSettingColor					= 'COLR',	//## Color setting represented by a color box.
		kSettingCheckColor				= 'CCLR',	//## Color setting represented by a color box with a check box for enable/disable.
		kSettingResource				= 'RSRC',	//## Resource name setting represented by a text box and a browse button.
		kSettingMultiResource			= 'MULT'	//## Setting showing a list of resource names in a text box with a browse button.
	};


	enum
	{
		kMaxSettingTitleLength			= 255,
		kMaxInfoSettingLength			= 255,
		kMaxTextSettingLength			= 255
	};


	//# \enum	ResourceSettingFlags

	enum
	{
		kResourceSettingGenericPath		= 1 << 0,	//## Return the generic resource path instead of the virtual resource path.
		kResourceSettingImportCatalog	= 1 << 1	//## Show files in the Import folder instead of the Data folder.
	};


	enum
	{
		kConfigurationScript			= 1 << 0
	};


	enum : WidgetType
	{
		kWidgetConfiguration			= 'CNFG'
	};


	class Value;
	class FilePicker;
	class ColorPicker;
	class SettingInterface;
	class ConfigurationWidget;


	//# \class	Setting		The base class for all user-configurable settings. 
	//
	//# Every user-configurable setting is a subclass of the $Setting$ class.
	// 
	//# \def	class Setting : public ListElement<Setting>, public Packable
	// 
	//# \ctor	Setting(SettingType type, Type identifier);
	//
	//# \param	type			The setting type. 
	//# \param	identifier		The setting's unique identifier.
	// 
	//# \desc 
	//# The $Setting$ class is the base class for all user-configurable setting objects.
	//
	//# \table	SettingType
	// 
	//# \base	Utilities/ListElement<Setting>		Used internally by the Interface Manager.
	//# \base	ResourceMgr/Packable				A setting can be packed for storage in resources.
	//
	//# \also	$@Configurable@$


	//# \function	Setting::GetSettingType		Returns the setting type.
	//
	//# \proto	SettingType GetSettingType(void) const;
	//
	//# \desc
	//# The $GetSettingType$ function returns the type of a setting, which is one of the following constants.
	//
	//# \table	SettingType
	//
	//# \also	$@Setting::GetSettingIdentifier@$


	//# \function	Setting::GetSettingIdentifier		Returns the setting's unique identifier.
	//
	//# \proto	Type GetSettingIdentifier(void) const;
	//
	//# \desc
	//# The $GetSettingIdentifier$ returns the unique identifier that was passed to the
	//# $@Setting@$ constructor.
	//
	//# \also	$@Setting::GetSettingType@$


	class Setting : public ListElement<Setting>, public Packable
	{
		private:

			SettingType			settingType;
			Type				settingIdentifier;
			ValueName			settingValueName;

			SettingInterface	*settingInterface;

			void operator =(const Setting& setting) = delete;

			virtual Setting *Replicate(void) const = 0;

		protected:

			C4API Setting(SettingType type);
			C4API Setting(SettingType type, Type identifier);
			C4API Setting(const Setting& setting);

		public:

			C4API virtual ~Setting();

			SettingType GetSettingType(void) const
			{
				return (settingType);
			}

			Type GetSettingIdentifier(void) const
			{
				return (settingIdentifier);
			}

			const char *GetSettingValueName(void) const
			{
				return (settingValueName);
			}

			void SetSettingValueName(const char *name)
			{
				settingValueName = name;
			}

			SettingInterface *GetSettingInterface(void) const
			{
				return (settingInterface);
			}

			void SetSettingInterface(SettingInterface *intrface)
			{
				settingInterface = intrface;
			}

			Setting *Clone(void) const
			{
				return (Replicate());
			}

			static Setting *Create(Unpacker& data, unsigned_int32 unpackFlags = 0);

			C4API void PackType(Packer& data) const override;
			C4API void Pack(Packer& data, unsigned_int32 packFlags) const override;
			C4API void Unpack(Unpacker& data, unsigned_int32 unpackFlags) override;
			bool UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags);
			C4API void *BeginSettingsUnpack(void) override;

			C4API virtual void Compress(Compressor& data) const;
			C4API virtual bool Decompress(Decompressor& data);

			virtual void Copy(const Setting *setting);
			virtual bool operator ==(const Setting& setting) const = 0;

			C4API virtual bool SetValue(const Value *value);
	};


	class SettingInterface : public ExclusiveObservable<SettingInterface>
	{
		friend class ConfigurationWidget;

		private:

			Setting								*settingData;

			Widget								*settingGroup;
			TextWidget							*titleWidget;

			EditTextWidget						*valueWidget;
			WidgetObserver<SettingInterface>	valueObserver;

			String<kMaxSettingTitleLength>		settingTitle;

			void HandleValueEvent(Widget *widget, const WidgetEventData *eventData);

		protected:

			C4API SettingInterface(Setting *setting, const char *title);

			C4API virtual void BuildInterface(Widget *group, const ConfigurationWidget *configWidget);
			C4API virtual void UpdateInterface(const ConfigurationWidget *configWidget);

		public:

			C4API virtual ~SettingInterface();

			Setting *GetSettingData(void) const
			{
				return (settingData);
			}

			const char *GetValueName(void) const
			{
				return ((valueWidget) ? valueWidget->GetText() : nullptr);
			}

			void SetValueName(const char *name)
			{
				if (valueWidget)
				{
					valueWidget->SetText(name);
				}
			}

			const char *GetSettingTitle(void) const
			{
				return (settingTitle);
			}

			void UpdateCurrentSetting(void)
			{
				if (DeterminantValue())
				{
					SetDeterminantValue();
				}
			}

			C4API virtual bool DeterminantValue(void) const;
			C4API virtual void SetDeterminantValue(void);
			C4API virtual void SetIndeterminantValue(void);

			C4API virtual bool ExtractCurrentSetting(void);
	};


	//# \class	HeadingSetting		A setting that simply displays a heading and has no value.
	//
	//# The $HeadingSetting$ class is used to display a heading in a list of settings.
	//
	//# \def	class HeadingSetting : public Setting
	//
	//# \ctor	HeadingSetting(Type identifier, const char *title);
	//
	//# \param	identifier		The setting's unique identifier.
	//# \param	title			The title of the setting.
	//
	//# \desc
	//# The $HeadingSetting$ class represents a setting that simply displays a heading (specified
	//# by the $title$ parameter) and has no value. Even though the setting doesn't need to be
	//# identified in the $@Configurable::SetSetting@$ function, it should still have a unique
	//# identifier specified by the $identifier$ parameter.
	//
	//# \base	Setting		A $HeadingSetting$ is a specific type of setting.


	class HeadingSetting : public Setting
	{
		friend class Setting;

		private:

			HeadingSetting();
			HeadingSetting(const HeadingSetting& headingSetting);

			Setting *Replicate(void) const override;

		public:

			C4API HeadingSetting(Type identifier, const char *title);
			C4API ~HeadingSetting();

			bool operator ==(const Setting& setting) const;
	};


	class HeadingSettingInterface : public SettingInterface
	{
		private:

			QuadWidget		*quadWidget;

			void BuildInterface(Widget *group, const ConfigurationWidget *configWidget) override;
			void UpdateInterface(const ConfigurationWidget *configWidget) override;

		public:

			HeadingSettingInterface(HeadingSetting *setting, const char *title);
			~HeadingSettingInterface();
	};


	//# \class	InfoSetting		A setting that displays information that cannot be edited.
	//
	//# The $InfoSetting$ class is used to display information that cannot be edited.
	//
	//# \def	class InfoSetting : public Setting
	//
	//# \ctor	InfoSetting(Type identifier, const char *value, const char *title);
	//
	//# \param	identifier		The setting's unique identifier.
	//# \param	value			The information displayed by the setting.
	//# \param	title			The title of the setting.
	//
	//# \desc
	//# The $InfoSetting$ class represents a setting that displays the information specified by the
	//# $value$ parameter without allowing it to be edited. Even though the setting doesn't need to be
	//# identified in the $@Configurable::SetSetting@$ function, it should still have a unique
	//# identifier specified by the $identifier$ parameter.
	//
	//# \base	Setting		An $InfoSetting$ is a specific type of setting.


	class InfoSetting : public Setting
	{
		friend class Setting;

		private:

			String<kMaxInfoSettingLength>	infoValue;

			InfoSetting();
			InfoSetting(const InfoSetting& infoSetting);

			Setting *Replicate(void) const override;

		public:

			C4API InfoSetting(Type identifier, const char *value, const char *title);
			C4API ~InfoSetting();

			const char *GetInfoValue(void) const
			{
				return (infoValue);
			}

			bool operator ==(const Setting& setting) const;
	};


	class InfoSettingInterface : public SettingInterface
	{
		private:

			TextWidget		*textWidget;

			void BuildInterface(Widget *group, const ConfigurationWidget *configWidget) override;
			void UpdateInterface(const ConfigurationWidget *configWidget) override;

		public:

			InfoSettingInterface(InfoSetting *setting, const char *title);
			~InfoSettingInterface();
	};


	//# \class	BooleanSetting		A boolean setting represented by a check box.
	//
	//# The $BooleanSetting$ class is used for a boolean setting represented by a check box.
	//
	//# \def	class BooleanSetting : public Setting
	//
	//# \ctor	BooleanSetting(Type identifier, bool value, const char *title);
	//
	//# \param	identifier		The setting's unique identifier.
	//# \param	value			The initial value of the setting.
	//# \param	title			The title of the setting.
	//
	//# \desc
	//# The $BooleanSetting$ class represents a setting that displays a check box and
	//# has a boolean value.
	//
	//# \base	Setting		A $BooleanSetting$ is a specific type of setting.


	//# \function	BooleanSetting::GetBooleanValue		Returns the boolean value stored in the setting.
	//
	//# \proto	bool GetBooleanValue(void) const;
	//
	//# \desc
	//# The $GetBooleanValue$ function returns the boolean value stored in the setting object.


	class BooleanSetting : public Setting
	{
		friend class Setting;

		private:

			bool		booleanValue;

			BooleanSetting();
			BooleanSetting(const BooleanSetting& booleanSetting);

			Setting *Replicate(void) const override;

		public:

			C4API BooleanSetting(Type identifier, bool value, const char *title);
			C4API ~BooleanSetting();

			bool GetBooleanValue(void) const
			{
				return (booleanValue);
			}

			void SetBooleanValue(bool value)
			{
				booleanValue = value;
			}

			void Pack(Packer& data, unsigned_int32 packFlags) const override;
			void Unpack(Unpacker& data, unsigned_int32 unpackFlags) override;

			void Compress(Compressor& data) const override;
			bool Decompress(Decompressor& data) override;

			void Copy(const Setting *setting) override;
			bool operator ==(const Setting& setting) const override;

			bool SetValue(const Value *value) override;
	};


	class BooleanSettingInterface : public SettingInterface
	{
		private:

			CheckWidget									*checkWidget;
			WidgetObserver<BooleanSettingInterface>		checkWidgetObserver;

			void HandleCheckWidgetEvent(Widget *widget, const WidgetEventData *eventData);

			void BuildInterface(Widget *group, const ConfigurationWidget *configWidget) override;
			void UpdateInterface(const ConfigurationWidget *configWidget) override;

		public:

			BooleanSettingInterface(BooleanSetting *setting, const char *title);
			~BooleanSettingInterface();

			void SetBooleanValue(int32 value)
			{
				checkWidget->SetValue(value);
			}

			bool DeterminantValue(void) const override;
			void SetDeterminantValue(void) override;
			void SetIndeterminantValue(void) override;

			bool ExtractCurrentSetting(void) override;
	};


	class SliderSettingInterface : public SettingInterface
	{
		protected:

			SliderWidget			*sliderWidget;
			EditTextWidget			*textWidget;

			String<>				minValueLabel;
			String<>				maxValueLabel;

			SliderSettingInterface(Setting *setting, const char *title, const char *minLabel, const char *maxLabel);

			void UpdateInterface(const ConfigurationWidget *configWidget) override;

		public:

			~SliderSettingInterface();

			bool DeterminantValue(void) const override;
			void SetIndeterminantValue(void) override;
	};


	//# \class	IntegerSetting		An integer setting represented by a slider.
	//
	//# The $IntegerSetting$ class is used for an integer setting represented by a slider.
	//
	//# \def	class IntegerSetting : public Setting
	//
	//# \ctor	IntegerSetting(Type identifier, int32 value, const char *title, int32 min, int32 max, int32 step,
	//# \ctor2	const char *minLabel = nullptr, const char *maxLabel = nullptr);
	//
	//# \param	identifier		The setting's unique identifier.
	//# \param	value			The initial value of the setting.
	//# \param	title			The title of the setting.
	//# \param	min				The minimum value allowed for the setting.
	//# \param	max				The maximum value allowed for the setting.
	//# \param	step			The smallest increment allowed between the minimum and maximum values.
	//# \param	minLabel		An optional label to display in place of the actual value when the setting's value is equal to the minimum value.
	//# \param	maxLabel		An optional label to display in place of the actual value when the setting's value is equal to the maximum value.
	//
	//# \desc
	//# The $IntegerSetting$ class represents a setting that displays a slider and has an
	//# integer value restricted to a given range. A text box is also displayed, allowing
	//# the user to enter a value directly. The value of the setting is always equal to the minimum
	//# value specified by the $min$ parameter plus a multiple of the $step$ parameter, but is never
	//# greater than the maaximum value specified by the $max$ parameter. If the user enters a
	//# number in the text box, then it is rounded down to the nearest valid value.
	//#
	//# If either the $minLabel$ or $maxLabel$ parameters is not $nullptr$, then they specify strings that are
	//# displayed instead of the integer value of the setting whenever the setting's value is equal to the
	//# minimum or maximum allowable value.
	//
	//# \base	Setting		An $IntegerSetting$ is a specific type of setting.


	//# \function	IntegerSetting::GetIntegerValue		Returns the integer value stored in the setting.
	//
	//# \proto	int32 GetIntegerValue(void) const;
	//
	//# \desc
	//# The $GetIntegerValue$ function returns the integer value stored in the setting object.


	class IntegerSetting : public Setting
	{
		friend class Setting;

		private:

			int32		integerValue;

			IntegerSetting();
			IntegerSetting(const IntegerSetting& integerSetting);

			Setting *Replicate(void) const override;

		public:

			C4API IntegerSetting(Type identifier, int32 value, const char *title, int32 min, int32 max, int32 step, const char *minLabel = nullptr, const char *maxLabel = nullptr);
			C4API ~IntegerSetting();

			int32 GetIntegerValue(void) const
			{
				return (integerValue);
			}

			void SetIntegerValue(int32 value)
			{
				integerValue = value;
			}

			void Pack(Packer& data, unsigned_int32 packFlags) const override;
			void Unpack(Unpacker& data, unsigned_int32 unpackFlags) override;

			void Compress(Compressor& data) const override;
			bool Decompress(Decompressor& data) override;

			void Copy(const Setting *setting) override;
			bool operator ==(const Setting& setting) const override;

			bool SetValue(const Value *value) override;
	};


	class IntegerSettingInterface : public SliderSettingInterface
	{
		private:

			int32		minValue;
			int32		maxValue;
			int32		stepValue;

			WidgetObserver<IntegerSettingInterface>		sliderWidgetObserver;
			WidgetObserver<IntegerSettingInterface>		textWidgetObserver;

			void SetValueText(int32 value) const;

			void HandleSliderWidgetEvent(Widget *widget, const WidgetEventData *eventData);
			void HandleTextWidgetEvent(Widget *widget, const WidgetEventData *eventData);

			void BuildInterface(Widget *group, const ConfigurationWidget *configWidget) override;

		public:

			IntegerSettingInterface(IntegerSetting *setting, const char *title, int32 min, int32 max, int32 step, const char *minLabel, const char *maxLabel);
			~IntegerSettingInterface();

			void SetDeterminantValue(void) override;
			bool ExtractCurrentSetting(void) override;
	};


	//# \class	PowerTwoSetting		A power-of-two setting represented by a slider.
	//
	//# The $PowerTwoSetting$ class is used for a power-of-two setting represented by a slider.
	//
	//# \def	class PowerTwoSetting : public Setting
	//
	//# \ctor	PowerTwoSetting(Type identifier, int32 value, const char *title, int32 min, int32 max);
	//# \ctor2	const char *minLabel = nullptr, const char *maxLabel = nullptr);
	//
	//# \param	identifier		The setting's unique identifier.
	//# \param	value			The initial value of the setting.
	//# \param	title			The title of the setting.
	//# \param	min				The minimum value allowed for the setting.
	//# \param	max				The maximum value allowed for the setting.
	//# \param	minLabel		An optional label to display in place of the actual value when the setting's value is equal to the minimum value.
	//# \param	maxLabel		An optional label to display in place of the actual value when the setting's value is equal to the maximum value.
	//
	//# \desc
	//# The $PowerTwoSetting$ class represents a setting that displays a slider and has an
	//# integer value restricted to a power of two within a given range. A text box is also displayed,
	//# allowing the user to enter a value directly. The value of the setting is always equal to a power
	//# of two between the minimum value specified by the $min$ parameter and the maaximum value
	//# specified by the $max$ parameter. If the user enters a number in the text box, then it is
	//# rounded down to the nearest valid value.
	//#
	//# If either the $minLabel$ or $maxLabel$ parameters is not $nullptr$, then they specify strings that are
	//# displayed instead of the integer value of the setting whenever the setting's value is equal to the
	//# minimum or maximum allowable value.
	//
	//# \base	Setting		A $PowerTwoSetting$ is a specific type of setting.


	//# \function	PowerTwoSetting::GetIntegerValue	Returns the integer value stored in the setting.
	//
	//# \proto	int32 GetIntegerValue(void) const;
	//
	//# \desc
	//# The $GetIntegerValue$ function returns the integer value stored in the setting object.


	class PowerTwoSetting : public Setting
	{
		friend class Setting;

		private:

			int32		integerValue;

			PowerTwoSetting();
			PowerTwoSetting(const PowerTwoSetting& powerTwoSetting);

			Setting *Replicate(void) const override;

		public:

			C4API PowerTwoSetting(Type identifier, int32 value, const char *title, int32 min, int32 max, const char *minLabel = nullptr, const char *maxLabel = nullptr);
			C4API ~PowerTwoSetting();

			int32 GetIntegerValue(void) const
			{
				return (integerValue);
			}

			void SetIntegerValue(int32 value)
			{
				integerValue = value;
			}

			void Pack(Packer& data, unsigned_int32 packFlags) const override;
			void Unpack(Unpacker& data, unsigned_int32 unpackFlags) override;

			void Compress(Compressor& data) const override;
			bool Decompress(Decompressor& data) override;

			void Copy(const Setting *setting) override;
			bool operator ==(const Setting& setting) const override;

			bool SetValue(const Value *value) override;
	};


	class PowerTwoSettingInterface : public SliderSettingInterface
	{
		private:

			int32		minValue;
			int32		maxValue;

			WidgetObserver<PowerTwoSettingInterface>	sliderWidgetObserver;
			WidgetObserver<PowerTwoSettingInterface>	textWidgetObserver;

			void SetValueText(int32 value) const;

			void HandleSliderWidgetEvent(Widget *widget, const WidgetEventData *eventData);
			void HandleTextWidgetEvent(Widget *widget, const WidgetEventData *eventData);

			void BuildInterface(Widget *group, const ConfigurationWidget *configWidget) override;

		public:

			PowerTwoSettingInterface(PowerTwoSetting *setting, const char *title, int32 min, int32 max, const char *minLabel, const char *maxLabel);
			~PowerTwoSettingInterface();

			void SetDeterminantValue(void) override;
			bool ExtractCurrentSetting(void) override;
	};


	//# \class	FloatSetting	A floating-point setting represented by a slider.
	//
	//# The $FloatSetting$ class is used for a floating-point setting represented by a slider.
	//
	//# \def	class FloatSetting : public Setting
	//
	//# \ctor	FloatSetting(Type identifier, float value, const char *title, float min, float max, float step);
	//# \ctor2	const char *minLabel = nullptr, const char *maxLabel = nullptr);
	//
	//# \param	identifier		The setting's unique identifier.
	//# \param	value			The initial value of the setting.
	//# \param	title			The title of the setting.
	//# \param	min				The minimum value allowed for the setting.
	//# \param	max				The maximum value allowed for the setting.
	//# \param	step			The smallest increment allowed between the minimum and maximum values.
	//# \param	minLabel		An optional label to display in place of the actual value when the setting's value is equal to the minimum value.
	//# \param	maxLabel		An optional label to display in place of the actual value when the setting's value is equal to the maximum value.
	//
	//# \desc
	//# The $FloatSetting$ class represents a setting that displays a slider and has an
	//# floating-point value restricted to a given range. A text box is also displayed, allowing
	//# the user to enter a value directly. The value of the setting is always equal to the minimum
	//# value specified by the $min$ parameter plus an integer multiple of the $step$ parameter, but
	//# is never greater than the maaximum value specified by the $max$ parameter. If the user enters a
	//# number in the text box, then it is rounded to the nearest valid value.
	//#
	//# If either the $minLabel$ or $maxLabel$ parameters is not $nullptr$, then they specify strings that are
	//# displayed instead of the floating-point value of the setting whenever the setting's value is equal to the
	//# minimum or maximum allowable value.
	//
	//# \base	Setting		A $FloatSetting$ is a specific type of setting.


	//# \function	FloatSetting::GetFloatValue		Returns the floating-point value stored in the setting.
	//
	//# \proto	float GetFloatValue(void) const;
	//
	//# \desc
	//# The $GetFloatValue$ function returns the floating-point value stored in the setting object.


	class FloatSetting : public Setting
	{
		friend class Setting;

		private:

			float		floatValue;

			FloatSetting();
			FloatSetting(const FloatSetting& floatSetting);

			Setting *Replicate(void) const override;

		public:

			C4API FloatSetting(Type identifier, float value, const char *title, float min, float max, float step, const char *minLabel = nullptr, const char *maxLabel = nullptr);
			C4API ~FloatSetting();

			float GetFloatValue(void) const
			{
				return (floatValue);
			}

			void SetFloatValue(float value)
			{
				floatValue = value;
			}

			void Pack(Packer& data, unsigned_int32 packFlags) const override;
			void Unpack(Unpacker& data, unsigned_int32 unpackFlags) override;

			void Compress(Compressor& data) const override;
			bool Decompress(Decompressor& data) override;

			void Copy(const Setting *setting) override;
			bool operator ==(const Setting& setting) const override;

			bool SetValue(const Value *value) override;
	};


	class FloatSettingInterface : public SliderSettingInterface
	{
		private:

			float		minValue;
			float		maxValue;
			float		stepValue;

			WidgetObserver<FloatSettingInterface>	sliderWidgetObserver;
			WidgetObserver<FloatSettingInterface>	textWidgetObserver;

			void SetValueText(float value) const;

			void HandleSliderWidgetEvent(Widget *widget, const WidgetEventData *eventData);
			void HandleTextWidgetEvent(Widget *widget, const WidgetEventData *eventData);

			void BuildInterface(Widget *group, const ConfigurationWidget *configWidget) override;

		public:

			FloatSettingInterface(FloatSetting *setting, const char *title, float min, float max, float step, const char *minLabel, const char *maxLabel);
			~FloatSettingInterface();

			void SetDeterminantValue(void) override;
			bool ExtractCurrentSetting(void) override;
	};


	//# \class	TextSetting		A text setting represented by an editable text box.
	//
	//# The $TextSetting$ class is used for a text setting represented by an editable text box.
	//
	//# \def	class TextSetting : public Setting
	//
	//# \ctor	TextSetting(Type identifier, const char *text, const char *title, int32 maxLen,
	//# \ctor2	EditTextWidget::FilterProc *filterProc = nullptr);
	//
	//# \param	identifier		The setting's unique identifier.
	//# \param	text			The initial text for the setting.
	//# \param	title			The title of the setting.
	//# \param	maxLen			The maximum number of characters that can be entered into the text box.
	//# \param	filterProc		A character filter function. If this is $nullptr$, then all characters are allowed.
	//
	//# \desc
	//# The $TextSetting$ class represents a setting that displays a text box and has a
	//# string value. The maximum length of the string is specified by the $maxLen$ parameter,
	//# and the characters allowed in the string can be controlled by specifying a filter with the
	//# $filterProc$ parameter. If no filter is specified, then all characters are allowed.
	//
	//# \base	Setting		A $TextSetting$ is a specific type of setting.


	//# \function	TextSetting::GetText		Returns the text stored in the setting.
	//
	//# \proto	const char *GetText(void) const;
	//
	//# \desc
	//# The $GetText$ function returns a pointer to the text string stored in the setting object.


	class TextSetting : public Setting
	{
		friend class Setting;

		private:

			String<kMaxTextSettingLength>		textValue;

			TextSetting();
			TextSetting(const TextSetting& textSetting);

			Setting *Replicate(void) const override;

		public:

			C4API TextSetting(Type identifier, const char *text, const char *title, int32 maxLen, EditTextWidget::FilterProc *filterProc = nullptr);
			C4API TextSetting(Type identifier, float value, const char *title);
			C4API ~TextSetting();

			const char *GetText(void) const
			{
				return (textValue);
			}

			void SetText(const char *text)
			{
				textValue = text;
			}

			void Pack(Packer& data, unsigned_int32 packFlags) const override;
			void Unpack(Unpacker& data, unsigned_int32 unpackFlags) override;

			void Compress(Compressor& data) const override;
			bool Decompress(Decompressor& data) override;

			void Copy(const Setting *setting) override;
			bool operator ==(const Setting& setting) const override;

			bool SetValue(const Value *value) override;
	};


	class TextSettingInterface : public SettingInterface
	{
		private:

			int32									maxTextLength;
			EditTextWidget::FilterProc				*textFilterProc;

			EditTextWidget							*textWidget;
			ImageWidget								*stripesWidget;

			WidgetObserver<TextSettingInterface>	textWidgetObserver;

			void HandleTextWidgetEvent(Widget *widget, const WidgetEventData *eventData);

			void BuildInterface(Widget *group, const ConfigurationWidget *configWidget) override;
			void UpdateInterface(const ConfigurationWidget *configWidget) override;

		public:

			TextSettingInterface(TextSetting *setting, const char *title, int32 maxLen, EditTextWidget::FilterProc *filterProc = nullptr);
			~TextSettingInterface();

			bool DeterminantValue(void) const override;
			void SetDeterminantValue(void) override;
			void SetIndeterminantValue(void) override;

			bool ExtractCurrentSetting(void) override;
	};


	//# \class	MenuSetting		A multi-valued setting represented by a popup menu.
	//
	//# The $MenuSetting$ class is used for a multi-valued setting represented by a popup menu.
	//
	//# \def	class MenuSetting : public Setting
	//
	//# \ctor	MenuSetting(Type identifier, int32 selection, const char *title, int32 itemCount);
	//
	//# \param	identifier		The setting's unique identifier.
	//# \param	selection		The initial selection for the setting.
	//# \param	title			The title of the setting.
	//# \param	itemCount		The number of items that will appear in the menu.
	//
	//# \desc
	//# The $MenuSetting$ class represents a setting that displays a popup menu and has an
	//# integer value in the range [0,&nbsp;<i>n</i>&nbsp;&minus;&nbsp;1], where <i>n</i> is
	//# the number of menu items specified by the $itemCount$ parameter.
	//#
	//# After a $MenuSetting$ is created, the $@MenuSetting::SetMenuItemString@$ function should
	//# be called for each menu item to specify its text.
	//
	//# \base	Setting		A $MenuSetting$ is a specific type of setting.


	//# \function	MenuSetting::SetMenuItemString		Sets the name of a single menu item.
	//
	//# \proto	void SetMenuItemString(int32 index, const char *string);
	//
	//# \param	index		The index of the menu item whose name is being set.
	//# \param	string		The name of the menu item.
	//
	//# \desc
	//# The $SetMenuItemString$ function is used to specify the text for a particular menu item
	//# belonging to a $MenuSetting$ object. This function should be called for each menu item
	//# after a $MenuSetting$ object has been created with the $index$ parameter ranging from
	//# 0 to <i>n</i>&nbsp;&minus;&nbsp;1, where <i>n</i> is the number of menu items specified
	//# by the $itemCount$ parameter of the $MenuSetting$ constructor.


	//# \function	MenuSetting::GetMenuSelection		Returns the menu selection for the setting.
	//
	//# \proto	int32 GetMenuSelection(void) const;
	//
	//# \desc
	//# The $GetMenuSelection$ function returns the index of the menu selection stored in the setting object.


	class MenuSetting : public Setting
	{
		friend class Setting;

		private:

			int32		menuSelection;

			MenuSetting();
			MenuSetting(const MenuSetting& menuSetting);

			Setting *Replicate(void) const override;

		public:

			C4API MenuSetting(Type identifier, int32 selection, const char *title, int32 itemCount);
			C4API ~MenuSetting();

			int32 GetMenuSelection(void) const
			{
				return (menuSelection);
			}

			void SetMenuSelection(int32 selection)
			{
				menuSelection = selection;
			}

			void SetMenuItemString(int32 index, const char *string);

			void Pack(Packer& data, unsigned_int32 packFlags) const override;
			void Unpack(Unpacker& data, unsigned_int32 unpackFlags) override;

			void Compress(Compressor& data) const override;
			bool Decompress(Decompressor& data) override;

			void Copy(const Setting *setting) override;
			bool operator ==(const Setting& setting) const override;

			bool SetValue(const Value *value) override;
	};


	class MenuSettingInterface : public SettingInterface
	{
		private:

			int32				menuItemCount;
			const char			**menuString;

			PopupMenuWidget							*menuWidget;
			WidgetObserver<MenuSettingInterface>	menuWidgetObserver;

			void HandleMenuWidgetEvent(Widget *widget, const WidgetEventData *eventData);

			void BuildInterface(Widget *group, const ConfigurationWidget *configWidget) override;
			void UpdateInterface(const ConfigurationWidget *configWidget) override;

		public:

			MenuSettingInterface(MenuSetting *setting, const char *title, int32 itemCount);
			~MenuSettingInterface();

			void SetMenuItemString(int32 index, const char *string)
			{
				menuString[index] = string;
			}

			bool DeterminantValue(void) const override;
			void SetDeterminantValue(void) override;
			void SetIndeterminantValue(void) override;

			bool ExtractCurrentSetting(void) override;
	};


	inline void MenuSetting::SetMenuItemString(int32 index, const char *string)
	{
		static_cast<MenuSettingInterface *>(GetSettingInterface())->SetMenuItemString(index, string);
	}


	//# \class	ColorSetting		A color setting represented by a color box.
	//
	//# The $ColorSetting$ class is used for a color setting represented by a color box.
	//
	//# \def	class ColorSetting : public Setting
	//
	//# \ctor	ColorSetting(Type identifier, const ColorRGBA& color, const char *title, const char *picker, unsigned_int32 flags = 0);
	//
	//# \param	identifier		The setting's unique identifier.
	//# \param	color			The initial color for the setting.
	//# \param	title			The title of the setting.
	//# \param	picker			The title of the color picker dialog.
	//# \param	flags			The flags passed to the color picker dialog.
	//
	//# \desc
	//# The $ColorSetting$ class represents a setting that displays a color box and has an
	//# RGBA color value. The $flags$ parameter specifies the color picker flags that are
	//# passed to the $@InterfaceMgr/ColorPicker@$ constructor when the user clicks on the
	//# color box. The flags can be zero or the following value.
	//
	//# \table	ColorPickerFlags
	//
	//# \base	Setting		A $ColorSetting$ is a specific type of setting.


	//# \function	ColorSetting::GetColor		Returns the color stored in the setting.
	//
	//# \proto	const ColorRGBA& GetColor(void) const;
	//
	//# \desc
	//# The $GetColor$ function returns the RGBA color value stored in the setting object.
	//
	//# \also	$@Math/ColorRGBA@$


	class ColorSetting : public Setting
	{
		friend class Setting;

		private:

			ColorRGBA		colorValue;

			Setting *Replicate(void) const override;

		protected:

			ColorSetting(SettingType type = kSettingColor);
			ColorSetting(SettingType type, Type identifier, const ColorRGBA& color);
			ColorSetting(const ColorSetting& colorSetting);

		public:

			C4API ColorSetting(Type identifier, const ColorRGBA& color, const char *title, const char *picker, unsigned_int32 flags = 0);
			C4API ~ColorSetting();

			const ColorRGBA& GetColor(void) const
			{
				return (colorValue);
			}

			void SetColor(const ColorRGBA& color)
			{
				colorValue = color;
			}

			void Pack(Packer& data, unsigned_int32 packFlags) const override;
			void Unpack(Unpacker& data, unsigned_int32 unpackFlags) override;

			void Compress(Compressor& data) const override;
			bool Decompress(Decompressor& data) override;

			void Copy(const Setting *setting) override;
			bool operator ==(const Setting& setting) const override;

			bool SetValue(const Value *value) override;
	};


	class ColorSettingInterface : public SettingInterface
	{
		private:

			String<kMaxSettingTitleLength>			pickerString;
			unsigned_int32							pickerFlags;

			ColorWidget								*colorWidget;
			ImageWidget								*stripesWidget;

			WidgetObserver<ColorSettingInterface>	colorWidgetObserver;

			void HandleColorWidgetEvent(Widget *widget, const WidgetEventData *eventData);

			void BuildInterface(Widget *group, const ConfigurationWidget *configWidget) override;
			void UpdateInterface(const ConfigurationWidget *configWidget) override;

		public:

			ColorSettingInterface(ColorSetting *setting, const char *title, const char *picker, unsigned_int32 flags = 0);
			~ColorSettingInterface();

			bool DeterminantValue(void) const override;
			void SetDeterminantValue(void) override;
			void SetIndeterminantValue(void) override;

			bool ExtractCurrentSetting(void) override;
	};


	//# \class	CheckColorSetting		A color setting represented by a color box with a check box for enable/disable.
	//
	//# The $CheckColorSetting$ class is used for a color setting represented by a color box with a check box for enable/disable.
	//
	//# \def	class CheckColorSetting : public ColorSetting
	//
	//# \ctor	CheckColorSetting(Type identifier, bool check, const ColorRGBA& color, const char *title, const char *picker, unsigned_int32 flags = 0);
	//
	//# \param	identifier		The setting's unique identifier.
	//# \param	check			The initial value for the checkbox.
	//# \param	color			The initial color for the setting.
	//# \param	title			The title of the setting.
	//# \param	picker			The title of the color picker dialog.
	//# \param	flags			The flags passed to the color picker dialog.
	//
	//# \desc
	//# The $CheckColorSetting$ class represents a setting that displays a color box and has an
	//# RGBA color value. This setting also displays a check box and contains an extra boolean value.
	//
	//# \base	ColorSetting	A $CheckColorSetting$ is a special type of $@ColorSetting@$.


	//# \function	CheckColorSetting::GetCheckValue	Returns the check value stored in the setting.
	//
	//# \proto	bool GetCheckValue(void) const;
	//
	//# \desc
	//# The $GetCheckValue$ function returns the boolean value stored in the setting object.


	class CheckColorSetting : public ColorSetting
	{
		friend class Setting;

		private:

			bool		checkValue;

			CheckColorSetting();
			CheckColorSetting(const CheckColorSetting& checkColorSetting);

			Setting *Replicate(void) const override;

		public:

			C4API CheckColorSetting(Type identifier, bool check, const ColorRGBA& color, const char *title, const char *picker, unsigned_int32 flags = 0);
			C4API ~CheckColorSetting();

			bool GetCheckValue(void) const
			{
				return (checkValue);
			}

			void SetCheckValue(bool value)
			{
				checkValue = value;
			}

			void Pack(Packer& data, unsigned_int32 packFlags) const override;
			void Unpack(Unpacker& data, unsigned_int32 unpackFlags) override;

			void Compress(Compressor& data) const override;
			bool Decompress(Decompressor& data) override;

			void Copy(const Setting *setting) override;
			bool operator ==(const Setting& setting) const override;
	};


	class CheckColorSettingInterface : public SettingInterface
	{
		private:

			String<kMaxSettingTitleLength>	pickerString;
			unsigned_int32					pickerFlags;

			CheckWidget						*checkWidget;
			ColorWidget						*colorWidget;
			ImageWidget						*stripesWidget;

			WidgetObserver<CheckColorSettingInterface>		checkWidgetObserver;
			WidgetObserver<CheckColorSettingInterface>		colorWidgetObserver;

			void HandleCheckWidgetEvent(Widget *widget, const WidgetEventData *eventData);
			void HandleColorWidgetEvent(Widget *widget, const WidgetEventData *eventData);

			void BuildInterface(Widget *group, const ConfigurationWidget *configWidget) override;
			void UpdateInterface(const ConfigurationWidget *configWidget) override;

		public:

			CheckColorSettingInterface(CheckColorSetting *setting, const char *title, const char *picker, unsigned_int32 flags = 0);
			~CheckColorSettingInterface();

			bool DeterminantValue(void) const override;
			void SetDeterminantValue(void) override;
			void SetIndeterminantValue(void) override;

			bool ExtractCurrentSetting(void) override;
	};


	class FilePickerSettingInterface : public SettingInterface
	{
		protected:

			ResourceName					subdirectory;
			unsigned_int32					settingFlags;

			String<kMaxSettingTitleLength>	pickerString;
			const ResourceDescriptor		*resourceDescriptor;

			EditTextWidget					*resourceWidget;
			GuiButtonWidget					*browseWidget;
			ImageWidget						*stripesWidget;

			FilePickerSettingInterface(Setting *setting, const char *title, const char *picker, const ResourceDescriptor *descriptor, const char *subdir, unsigned_int32 flags);

			void UpdateInterface(const ConfigurationWidget *configWidget) override;

		public:

			~FilePickerSettingInterface();

			bool DeterminantValue(void) const override;
			void SetIndeterminantValue(void) override;
	};


	//# \class	ResourceSetting		A resource name setting represented by a text box and a browse button.
	//
	//# The $ResourceSetting$ class is used for a resource name setting represented by a text box and a browse button.
	//
	//# \def	class ResourceSetting : public Setting
	//
	//# \ctor	ResourceSetting(Type identifier, const char *name, const char *title, const char *picker,
	//# \ctor2	const ResourceDescriptor *descriptor, const char *subdir = nullptr, unsigned_int32 flags = 0);
	//
	//# \param	identifier		The setting's unique identifier.
	//# \param	name			The initial resource name for the setting.
	//# \param	title			The title of the setting.
	//# \param	picker			The title of the file picker dialog.
	//# \param	descriptor		A pointer to the resource descriptor for the type of resource that can be chosen with the setting.
	//# \param	subdir			A subdirectory within the main resource directory to which the resource selection should be restricted.
	//# \param	flags			Flags that affect the behavior of the setting.
	//
	//# \desc
	//# The $ResourceSetting$ class represents a setting that displays a text box and has a
	//# resource name value. A button is also displayed that causes a file picker dialog to appear
	//# when clicked by the user. When a file is chosen through the file picker, the returned resource
	//# name is the virtual path to the resource if the $flags$ parameter is 0. If the following value
	//# is specified for the $flags$ parameter, then the generic path to the resource is returned.
	//
	//# \table	ResourceSettingFlags
	//
	//# \base	Setting		A $ResourceSetting$ is a specific type of setting.


	//# \function	ResourceSetting::GetResourceName		Returns the resource name stored in the setting.
	//
	//# \proto	const ResourceName& GetResourceName(void) const;
	//
	//# \desc
	//# The $GetResourceName$ function returns the resource name stored in the setting object.


	class ResourceSetting : public Setting
	{
		friend class Setting;

		private:

			ResourcePath		resourceName;

			ResourceSetting();
			ResourceSetting(const ResourceSetting& resourceSetting);

			Setting *Replicate(void) const override;

		public:

			C4API ResourceSetting(Type identifier, const char *name, const char *title, const char *picker, const ResourceDescriptor *descriptor, const char *subdir = nullptr, unsigned_int32 flags = 0);
			C4API ~ResourceSetting();

			const ResourcePath& GetResourceName(void) const
			{
				return (resourceName);
			}

			void SetResourceName(const char *name)
			{
				resourceName = name;
			}

			void Pack(Packer& data, unsigned_int32 packFlags) const override;
			void Unpack(Unpacker& data, unsigned_int32 unpackFlags) override;

			void Compress(Compressor& data) const override;
			bool Decompress(Decompressor& data) override;

			void Copy(const Setting *setting) override;
			bool operator ==(const Setting& setting) const override;

			bool SetValue(const Value *value) override;
	};


	class ResourceSettingInterface : public FilePickerSettingInterface
	{
		private:

			WidgetObserver<ResourceSettingInterface>	browseWidgetObserver;
			WidgetObserver<ResourceSettingInterface>	resourceWidgetObserver;

			void HandleBrowseWidgetEvent(Widget *widget, const WidgetEventData *eventData);
			void HandleResourceWidgetEvent(Widget *widget, const WidgetEventData *eventData);
			static void ResourcePickProc(FilePicker *picker, void *cookie);

			void BuildInterface(Widget *group, const ConfigurationWidget *configWidget) override;

		public:

			ResourceSettingInterface(ResourceSetting *setting, const char *title, const char *picker, const ResourceDescriptor *descriptor, const char *subdir, unsigned_int32 flags);
			~ResourceSettingInterface();

			void SetDeterminantValue(void) override;
			bool ExtractCurrentSetting(void) override;
	};


	//# \class	MultiResourceSetting		A setting showing a list of resource names in a text box with a browse button.
	//
	//# The $MultiResourceSetting$ class is used for a setting showing a list of resource names in a text box with a browse button.
	//
	//# \def	class MultiResourceSetting : public Setting
	//
	//# \ctor	ResourceSetting(Type identifier, const char *title, const char *picker,
	//# \ctor2	const ResourceDescriptor *descriptor, const char *subdir = nullptr, unsigned_int32 flags = 0);
	//
	//# \param	identifier		The setting's unique identifier.
	//# \param	title			The title of the setting.
	//# \param	picker			The title of the file picker dialog.
	//# \param	descriptor		A pointer to the resource descriptor for the type of resource that can be chosen with the setting.
	//# \param	subdir			A subdirectory within the main resource directory to which the resource selection should be restricted.
	//# \param	flags			Flags that affect the behavior of the setting.
	//
	//# \desc
	//# The $MultiResourceSetting$ class represents a setting that displays a text box and has a
	//# value which is a list of resource names. A button is also displayed that causes a file picker dialog
	//# to appear when clicked by the user. When a file is chosen through the file picker, the returned resource
	//# name is the virtual path to the resource if the $flags$ parameter is 0. If the following value
	//# is specified for the $flags$ parameter, then the generic path to the resource is returned.
	//
	//# \table	ResourceSettingFlags
	//
	//# When new files are chosen in the file picker, they are added to the list of resource names.
	//
	//# \base	Setting		A $MultiResourceSetting$ is a specific type of setting.


	//# \function	MultiResourceSetting::GetResourceCount		Returns the number of resource names stored in the setting.
	//
	//# \proto	int32 GetResourceCount(void) const;
	//
	//# \desc
	//# The $GetResourceCount$ function returns the number of resource names stored in the setting object.
	//# The individual resource names can be retrieved using the $@MultiResourceSetting::GetResourceName@$ function.
	//
	//# \also	$@MultiResourceSetting::GetResourceName@$


	//# \function	MultiResourceSetting::GetResourceName		Returns an individual resource name stored in the setting.
	//
	//# \proto	ResourceName GetResourceName(int32 index) const;
	//
	//# \desc
	//# The $GetResourceName$ function returns the individual resource name corresponding to the
	//# index specified by the $index$ parameter. The value of $index$ should be in the range
	//# [0,&nbsp;<i>n</i>&nbsp;&minus;&nbsp;1], where <i>n</i> is the number of resource names
	//# returned by the $@MultiResourceSetting::GetResourceCount@$ function.
	//
	//# \also	$@MultiResourceSetting::GetResourceName@$


	class MultiResourceSetting : public Setting
	{
		friend class Setting;

		private:

			String<>		resourceList;

			MultiResourceSetting();
			MultiResourceSetting(const MultiResourceSetting& multiResourceSetting);

			Setting *Replicate(void) const override;

		public:

			C4API MultiResourceSetting(Type identifier, const char *title, const char *picker, const ResourceDescriptor *descriptor, const char *subdir = nullptr, unsigned_int32 flags = 0);
			C4API ~MultiResourceSetting();

			const String<>& GetResourceList(void) const
			{
				return (resourceList);
			}

			void SetResourceList(const char *list)
			{
				resourceList = list;
			}

			void Pack(Packer& data, unsigned_int32 packFlags) const override;
			void Unpack(Unpacker& data, unsigned_int32 unpackFlags) override;

			void Compress(Compressor& data) const override;
			bool Decompress(Decompressor& data) override;

			void Copy(const Setting *setting) override;
			bool operator ==(const Setting& setting) const override;

			bool SetValue(const Value *value) override;

			C4API void AddResourceName(const char *name);

			C4API int32 GetResourceCount(void) const;
			C4API ResourceName GetResourceName(int32 index) const;
	};


	class MultiResourceSettingInterface : public FilePickerSettingInterface
	{
		private:

			WidgetObserver<MultiResourceSettingInterface>	browseWidgetObserver;
			WidgetObserver<MultiResourceSettingInterface>	resourceWidgetObserver;

			void HandleBrowseWidgetEvent(Widget *widget, const WidgetEventData *eventData);
			void HandleResourceWidgetEvent(Widget *widget, const WidgetEventData *eventData);
			static void ResourcePickProc(FilePicker *picker, void *cookie);

			void BuildInterface(Widget *group, const ConfigurationWidget *configWidget) override;

		public:

			MultiResourceSettingInterface(MultiResourceSetting *setting, const char *title, const char *picker, const ResourceDescriptor *descriptor, const char *subdir, unsigned_int32 flags);
			~MultiResourceSettingInterface();

			void SetDeterminantValue(void) override;
			bool ExtractCurrentSetting(void) override;
	};


	//# \class	ConfigurationWidget		The interface widget that displays a configuration table.
	//
	//# The $ConfigurationWidget$ class represents an interface widget that displays a configuration table.
	//
	//# \def	class ConfigurationWidget final : public RenderableWidget
	//
	//# \ctor	ConfigurationWidget(const Vector2D& size, float titleFraction, unsigned_int32 flags = 0);
	//
	//# \param	size			The size of the configuration widget, in pixels.
	//# \param	titleFraction	The fraction of the horizontal width dedicated to displaying setting titles.
	//# \param	flags			The configuration widget flags. This is used internally and should be set to zero.
	//
	//# \desc
	//# The $ConfigurationWidget$ class displays a configuration table that is used to show property settings to the user.
	//#
	//# The default widget color corresponds to the $kWidgetColorBorder$ color type and controls the color of the configuration
	//# table's outer border. Other color types supported by the configuration widget are $kWidgetColorLine$ and $kWidgetColorBackground$.
	//
	//# \base	RenderableWidget	All rendered interface widgets are subclasses of $RenderableWidget$.
	//
	//# \also	$@Configurable@$
	//# \also	$@Setting@$


	//# \function	ConfigurationWidget::BuildConfiguration		Builds the configuration interface for the settings belonging to a configurable object.
	//
	//# \proto	void BuildConfiguration(const Configurable *configurable);
	//
	//# \param	configurable	A pointer to the configurable object.
	//
	//# \desc
	//# The $BuildConfiguration$ function builds the table of widgets for the settings belonging to the configurable
	//# object specified by the $configurable$ parameter. During this process, the $@Configurable::GetSettingCount@$
	//# and $@Configurable::GetSetting@$ functions are called for the configurable object, and the appropriate widgets
	//# are created for each setting.
	//#
	//# The $BuildConfiguration$ function can be called multiple times for the same configuration widget with different
	//# values for the $configurable$ parameter. In this case, settings with unique identifiers returned from later
	//# configurable objects cause new widgets to be added to the bottom of the table. However, if a setting having an
	//# identifier matching one of the existing entries of the table is returned, then the existing widgets are reused,
	//# and the separate settings are combined into a single interface. Settings using the same identifier must have the
	//# same type. If the values of the separate settings are the same, then that value is displayed in the combined setting,
	//# but if the values are different, then an indeterminate state is displayed until the user makes a change to the setting.
	//#
	//# In the particular case that the $BuildConfiguration$ function is called multiple times for configurable objects
	//# having the same dynamic type, the settings returned by later objects will always match the identifiers of the
	//# existing settings, and the values shown in the configuration widget will reflect the common values shared by all
	//# of the configurable objects.
	//#
	//# In order to rebuild the contents of a configuration widget from scratch (without combining the new settings with
	//# the existing settings), the $@ConfigurationWidget::ReleaseConfiguration@$ function must first be called to release
	//# the widgets for the existing settings. A subsequent call to the $BuildConfiguration$ function will then build new
	//# widgets for the configurable object passed to it.
	//
	//# \also	$@ConfigurationWidget::CommitConfiguration@$
	//# \also	$@ConfigurationWidget::ReleaseConfiguration@$
	//# \also	$@Configurable@$
	//# \also	$@Setting@$


	//# \function	ConfigurationWidget::CommitConfiguration	Commits the new settings for a configurable object.
	//
	//# \proto	void CommitConfiguration(Configurable *configurable) const;
	//
	//# \param	configurable	A pointer to the configurable object.
	//
	//# \desc
	//# The $CommitConfiguration$ function commits the settings stored in a configuration widget for the configurable
	//# object specified by the $configurable$ parameter. During this process, the $@Configurable::SetSetting@$ function
	//# is called for the configurable object once for each setting that does not have an indeterminate value. (A setting
	//# can only have an indeterminate value if the $@ConfigurationWidget::BuildConfiguration@$ function was called multiple
	//# times, and a setting with the same identifier was returned by multiple objects without having the same value in
	//# all cases.)
	//#
	//# The $CommitConfiguration$ function may be called multiple times for the same configuration widget with different
	//# values for the $configurable$ parameter. If the configurable objects have different dynamic types, this can cause
	//# the $@Configurable::SetSetting@$ function to be called to commit a particular setting for a configurable object
	//# that did not previously return the setting through the $@Configurable::GetSetting@$ function, but such unrecognized
	//# settings should be ignored by the configurable object.
	//
	//# \also	$@ConfigurationWidget::BuildConfiguration@$
	//# \also	$@ConfigurationWidget::ReleaseConfiguration@$
	//# \also	$@Configurable@$
	//# \also	$@Setting@$


	//# \function	ConfigurationWidget::ReleaseConfiguration	Releases the configuration interface.
	//
	//# \proto	void ReleaseConfiguration(void);
	//
	//# \desc
	//# The $ReleaseConfiguration$ function releases the per-setting interface widgets currently displayed by a configuration
	//# widget and returned the configuration widget to its initial empty state. It's necessary to call this function only in
	//# order to rebuild the interface widgets from scratch.
	//
	//# \also	$@ConfigurationWidget::BuildConfiguration@$
	//# \also	$@ConfigurationWidget::CommitConfiguration@$
	//# \also	$@Configurable@$
	//# \also	$@Setting@$


	class ConfigurationWidget final : public RenderableWidget
	{
		friend class WidgetReg<ConfigurationWidget>;

		private:

			struct ConfigurationVertex
			{
				Point2D		position;
				ColorRGBA	color;
			};

			unsigned_int32							configurationFlags;
			unsigned_int32							colorOverrideFlags;
			float									titleColumnFraction;

			ColorRGBA								lineColor;
			ColorRGBA								backgroundColor;

			float									fullSettingWidth;
			float									titleColumnWidth;
			float									valueColumnWidth;
			float									valueColumnPosition;

			int32									configurationSettingCount;
			int32									displaySettingCount;
			int32									displaySettingIndex;

			List<Setting>							settingList;
			SettingInterface::ObserverType			*settingObserver;

			ColorRGBA								dynamicBackgroundColor;
			ColorRGBA								dynamicLineColor;

			WidgetObserver<ConfigurationWidget>		scrollObserver;

			VertexBuffer							configurationVertexBuffer;

			VertexBuffer							borderVertexBuffer;
			List<Attribute>							borderAttributeList;
			DiffuseAttribute						borderColorAttribute;
			DiffuseTextureAttribute					borderTextureAttribute;
			Renderable								borderRenderable;

			Widget									settingGroup;
			ScrollWidget							scrollWidget;

			ConfigurationWidget();
			ConfigurationWidget(const ConfigurationWidget& configurationWidget);

			Widget *Replicate(void) const override;

			void SetDefaultLineColor(void);
			void CalculateColumnWidths(void);

			void HandleStructureUpdate(void) override;

			void HandleScrollEvent(Widget *widget, const WidgetEventData *eventData);

		public:

			C4API ConfigurationWidget(const Vector2D& size, float titleFraction, unsigned_int32 flags = 0);
			C4API ~ConfigurationWidget();

			unsigned_int32 GetConfigurationFlags(void) const
			{
				return (configurationFlags);
			}

			void SetConfigurationFlags(unsigned_int32 flags)
			{
				configurationFlags = flags;
			}

			float GetFullSettingWidth(void) const
			{
				return (fullSettingWidth);
			}

			float GetTitleColumnWidth(void) const
			{
				return (titleColumnWidth);
			}

			float GetValueColumnWidth(void) const
			{
				return (valueColumnWidth);
			}

			float GetValueColumnPosition(void) const
			{
				return (valueColumnPosition);
			}

			Setting *GetFirstSetting(void) const
			{
				return (settingList.First());
			}

			Setting *GetLastSetting(void) const
			{
				return (settingList.Last());
			}

			void Pack(Packer& data, unsigned_int32 packFlags) const override;
			void Unpack(Unpacker& data, unsigned_int32 unpackFlags) override;
			bool UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags);
			void *BeginSettingsUnpack(void) override;

			int32 GetSettingCount(void) const override;
			Setting *GetSetting(int32 index) const override;
			void SetSetting(const Setting *setting) override;

			const ColorRGBA& GetWidgetColor(WidgetColorType type = kWidgetColorDefault) const override;
			void SetWidgetColor(const ColorRGBA& color, WidgetColorType type = kWidgetColorDefault) override;
			void SetWidgetAlpha(float alpha, WidgetColorType type = kWidgetColorDefault) override;
			void SetDynamicWidgetColor(const ColorRGBA& color, WidgetColorType type = kWidgetColorDefault) override;
			void SetDynamicWidgetAlpha(float alpha, WidgetColorType type = kWidgetColorDefault) override;

			void SetWidgetSize(const Vector2D& size) override;
			void Preprocess(void) override;

			void Build(void) override;
			void Render(List<Renderable> *renderList) override;

			void HandleMouseEvent(const PanelMouseEventData *eventData) override;

			C4API void SetObserver(SettingInterface::ObserverType *observer);

			C4API Setting *FindSetting(Type identifier) const;

			C4API void BuildConfiguration(const Configurable *configurable);
			C4API void CommitConfiguration(Configurable *configurable) const;
			C4API void BuildCategoryConfiguration(const Configurable *configurable, Type category);
			C4API void CommitCategoryConfiguration(Configurable *configurable, Type category) const;
			C4API void ReleaseConfiguration(void);
	};


	template <class observerType> class ConfigurationObserver : public ExclusiveObserver<observerType, SettingInterface>
	{
		public:

			ConfigurationObserver(observerType *observer, void (observerType::*callback)(SettingInterface *)) : ExclusiveObserver<observerType, SettingInterface>(observer, callback)
			{
			}
	};
}


#endif

// ZYUQURM
