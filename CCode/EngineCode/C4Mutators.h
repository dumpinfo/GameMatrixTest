 

#ifndef C4Mutators_h
#define C4Mutators_h


//# \component	Interface Manager
//# \prefix		InterfaceMgr/


#include "C4Creation.h"
#include "C4Configurable.h"


namespace C4
{
	//# \tree	Mutator
	//
	//# \node	PulsateMutator
	//# \node	RandomizeMutator
	//# \node	ScrollMutator
	//# \node	RotateMutator
	//# \node	ScaleMutator
	//# \node	FadeMutator
	//# \node	TickerMutator
	//# \node	AnimateMutator


	enum
	{
		kMaxMutatorKeyLength	= 15
	};


	typedef Type	MutatorType;
	typedef Type	WidgetColorType;


	typedef String<kMaxMutatorKeyLength>	MutatorKey;


	enum : MutatorType
	{
		kMutatorUnknown			= '!UNK',
		kMutatorPulsate			= 'PULS',
		kMutatorRandomize		= 'RAND',
		kMutatorScroll			= 'SCRL',
		kMutatorRotate			= 'ROTR',
		kMutatorScale			= 'SCAL',
		kMutatorFade			= 'FADE',
		kMutatorTicker			= 'TICK',
		kMutatorAnimate			= 'ANIM'
	};


	//# \enum	MutatorState

	enum
	{
		kMutatorDisabled		= 1 << 0,		//## The mutator is disabled and will not be updated.
		kMutatorReverse			= 1 << 1,		//## The mutator should run in reverse (if supported).
		kMutatorTerminated		= 1 << 2		//## The mutator has finished running.
	};


	//# \enum	PulsateWave

	enum
	{
		kPulsateWaveSquare,						//## A square wave. The value is 0.0 for the first half of each cycle and 1.0 for the second half.
		kPulsateWaveTriangle,					//## A triangle wave. The value varies linearly from 0.0 to 1.0 in the first half of each cycle and back to 0.0 in the second half.
		kPulsateWaveSine,						//## A sine wave. The value varies as a sinusoid from 0.0 to 1.0 in the first half of each cycle and back to 0.0 in the second half.
		kPulsateWaveCount
	};


	enum
	{
		kPulsateBlendInterpolate,
		kPulsateBlendAdd,
		kPulsateBlendMultiply,
		kPulsateBlendCount
	};


	enum
	{
		kScaleMutatorScaleTexcoords		= 1 << 0,
		kScaleMutatorFlipHorizontal		= 1 << 1,
		kScaleMutatorFlipVertical		= 1 << 2,
		kScaleMutatorRestartable		= 1 << 3
	};


	enum
	{
		kFadeMutatorFinishHide			= 1 << 0,
		kFadeMutatorRestartable			= 1 << 1
	};


	enum
	{ 
		kTickerMutatorLooping			= 1 << 0,
		kTickerMutatorRestartable		= 1 << 1
	}; 

 
	enum
	{
		kAnimateMutatorLooping			= 1 << 0, 
		kAnimateMutatorRestartable		= 1 << 1
	}; 
 

	class Mutator;
	class Widget;
 

	//# \class	MutatorRegistration		Manages internal registration information for a custom mutator type.
	//
	//# The $MutatorRegistration$ class manages internal registration information for a custom mutator type.
	//
	//# \def	class MutatorRegistration : public Registration<Mutator, MutatorRegistration>
	//
	//# \ctor	MutatorRegistration(MutatorType type, const char *name);
	//
	//# \param	type		The mutator type.
	//# \param	name		The mutator name.
	//
	//# \desc
	//# The $MutatorRegistration$ class is abstract and serves as the common base class for the template class
	//# $@MutatorReg@$. A custom mutator is registered with the engine by instantiating an object of type
	//# $MutatorReg<classType>$, where $classType$ is the type of the mutator subclass being registered.
	//
	//# \base	System/Registration<Mutator, MutatorRegistration>		A mutator registration is a specific type of registration object.
	//
	//# \also	$@MutatorReg@$
	//# \also	$@Mutator@$


	//# \function	MutatorRegistration::GetMutatorType		Returns the registered mutator type.
	//
	//# \proto	MutatorType GetMutatorType(void) const;
	//
	//# \desc
	//# The $GetMutatorType$ function returns the mutator type for a particular mutator registration.
	//# The mutator type is established when the mutator registration is constructed.
	//
	//# \also	$@MutatorRegistration::GetMutatorName@$


	//# \function	MutatorRegistration::GetMutatorName		Returns the human-readable mutator name.
	//
	//# \proto	const char *GetMutatorName(void) const;
	//
	//# \desc
	//# The $GetMutatorName$ function returns the human-readable mutator name for a particular mutator registration.
	//# The mutator name is established when the mutator registration is constructed.
	//
	//# \also	$@MutatorRegistration::GetMutatorType@$


	class MutatorRegistration : public Registration<Mutator, MutatorRegistration>
	{
		private:

			const char		*mutatorName;

		public:

			C4API MutatorRegistration(MutatorType type, const char *name);
			C4API ~MutatorRegistration();

			MutatorType GetMutatorType(void) const
			{
				return (GetRegistrableType());
			}

			const char *GetMutatorName(void) const
			{
				return (mutatorName);
			}

			virtual bool ValidWidget(const Widget *widget) const = 0;
	};


	//# \class	MutatorReg	 Represents a custom mutator type.
	//
	//# The $MutatorReg$ class represents a custom mutator type.
	//
	//# \def	template <class classType> class MutatorReg : public MutatorRegistration
	//
	//# \tparam	classType	The custom mutator class.
	//
	//# \ctor	MutatorReg(MutatorType type, const char *name);
	//
	//# \param	type		The mutator type.
	//# \param	name		The mutator name.
	//
	//# \desc
	//# The $MutatorReg$ template class is used to advertise the existence of a custom mutator type.
	//# The Interface Manager uses a mutator registration to construct a custom mutator. The act of instantiating a
	//# $MutatorReg$ object automatically registers the corresponding mutator type. The mutator type is unregistered
	//# when the $MutatorReg$ object is destroyed.
	//#
	//# No more than one mutator registration should be created for each distinct mutator type.
	//
	//# \base	MutatorRegistration		All specific mutator registration classes share the common base class $MutatorRegistration$.
	//
	//# \also	$@Mutator@$


	template <class classType> class MutatorReg : public MutatorRegistration
	{
		public:

			MutatorReg(MutatorType type, const char *name) : MutatorRegistration(type, name)
			{
			}

			Mutator *Create(void) const
			{
				return (new classType);
			}

			bool ValidWidget(const Widget *widget) const override
			{
				return ((GetMutatorName()) && (classType::ValidWidget(widget)));
			}
	};


	//# \class	Mutator		Used to modify a widget in some way over time.
	//
	//# The $Mutator$ class is used to modify a widget in some way over time.
	//
	//# \def	class Mutator : public ListElement<Mutator>, public Packable, public Configurable, public Registrable<Mutator, MutatorRegistration>
	//
	//# \ctor	Mutator(MutatorType type);
	//
	//# \param	type		The mutator type.
	//
	//# \desc
	//# The $Mutator$ class is the base class for all widget mutators. Every widget has a list of mutators that are applied
	//# to change its appearance over time. Mutators are added to a widget by calling the $@Widget::AddMutator@$ function.
	//
	//# \base	Utilities/ListElement<Mutator>						Used internally by the Interface Manager.
	//# \base	ResourceMgr/Packable								Mutators can be packed for storage in resources.
	//# \base	InterfaceMgr/Configurable							Mutators can define configurable parameters that are exposed
	//#																as user interface widgets in the Panel Editor.
	//# \base	System/Registrable<Mutator, MutatorRegistration>	Custom mutator types can be registered with the engine.
	//
	//# \also	$@MutatorReg@$
	//# \also	$@Widget@$
	//# \also	$@EffectMgr/PanelEffect@$
	//# \also	$@EffectMgr/PanelController@$
	//
	//# \wiki	Mutators	Mutators


	//# \function	Mutator::GetMutatorType		Returns the mutator type.
	//
	//# \proto	MutatorType GetMutatorType(void) const;
	//
	//# \desc
	//# The $GetMutatorType$ function returns the mutator type.


	//# \function	Mutator::GetMutatorState	Returns the mutator state.
	//
	//# \proto	unsigned_int32 GetMutatorState(void) const;
	//
	//# \desc
	//# The $GetMutatorState$ function returns the mutator flags, which can be a combination (through
	//# logical OR) of the following constants.
	//
	//# \table	MutatorState
	//
	//# \also	$@Mutator::SetMutatorState@$


	//# \function	Mutator::SetMutatorState	Sets the mutator state.
	//
	//# \proto	void SetMutatorState(unsigned_int32 state);
	//
	//# \param	state	The new mutator state.
	//
	//# \desc
	//# The $SetMutatorState$ function sets the mutator state to the value specified by the $state$ parameter,
	//# which can be a combination (through logical OR) of the following constants.
	//
	//# \table	MutatorState
	//
	//# \also	$@Mutator::GetMutatorState@$


	//# \function	Mutator::GetMutatorKey		Returns the mutator key.
	//
	//# \proto	const MutatorKey& GetMutatorKey(void) const;
	//
	//# \desc
	//# The $GetMutatorKey$ function returns the mutator key. The mutator key is a string having up to 15
	//# single-byte characters that can be used to identify one or more mutators in a panel. The initial
	//# key for a mutator is the empty string.
	//
	//# \also	$@Mutator::SetMutatorKey@$


	//# \function	Mutator::SetMutatorKey		Sets the mutator key.
	//
	//# \proto	void SetMutatorKey(const char *key);
	//
	//# \param	key		The new mutator key. This is a string up to 15 bytes in length, not counting the null terminator.
	//
	//# \desc
	//# The $SetMutatorKey$ function sets the mutator key to the string specified by the $key$ parameter.
	//# The mutator key is a string having up to 15 single-byte characters that can be used to identify
	//# one or more mutators in a panel. The initial key for a mutator is the empty string.
	//
	//# \also	$@Mutator::GetMutatorKey@$


	class Mutator : public ListElement<Mutator>, public Packable, public Configurable, public Registrable<Mutator, MutatorRegistration>
	{
		friend class Widget;

		private:

			MutatorType			mutatorType;
			unsigned_int32		mutatorState;
			MutatorKey			mutatorKey;

			Widget				*targetWidget;

			C4API virtual Mutator *Replicate(void) const = 0;

			static Mutator *CreateUnknown(Unpacker& data, unsigned_int32 unpackFlags);

		protected:

			C4API Mutator(MutatorType type);
			C4API Mutator(const Mutator& mutator);

		public:

			C4API virtual ~Mutator();

			MutatorType GetMutatorType(void) const
			{
				return (mutatorType);
			}

			unsigned_int32 GetMutatorState(void) const
			{
				return (mutatorState);
			}

			const MutatorKey& GetMutatorKey(void) const
			{
				return (mutatorKey);
			}

			void SetMutatorKey(const char *key)
			{
				mutatorKey = key;
			}

			Widget *GetTargetWidget(void) const
			{
				return (targetWidget);
			}

			Mutator *Clone(void) const
			{
				return (Replicate());
			}

			C4API static Mutator *New(MutatorType type);

			C4API static bool ValidWidget(const Widget *widget);
			static void RegisterStandardMutators(void);

			C4API void PackType(Packer& data) const override;
			C4API void Pack(Packer& data, unsigned_int32 packFlags) const override;
			C4API void Unpack(Unpacker& data, unsigned_int32 unpackFlags) override;
			bool UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags);
			C4API void *BeginSettingsUnpack(void) override;

			C4API int32 GetSettingCount(void) const override;
			C4API Setting *GetSetting(int32 index) const override;
			C4API void SetSetting(const Setting *setting) override;

			C4API virtual void SetMutatorState(unsigned_int32 state);

			C4API virtual void Preprocess(void);
			C4API virtual void Move(void);
			C4API virtual void Reset(void);
	};


	class UnknownMutator : public Mutator
	{
		friend class Mutator;
		friend class MutatorReg<UnknownMutator>;

		private:

			MutatorType			unknownType;
			unsigned_int32		unknownSize;
			char				*unknownData;

			UnknownMutator();
			UnknownMutator(MutatorType type);
			UnknownMutator(const UnknownMutator& unknownMutator);

			Mutator *Replicate(void) const override;

		public:

			~UnknownMutator();

			static bool ValidWidget(const Widget *widget);

			void PackType(Packer& data) const override;
			void Pack(Packer& data, unsigned_int32 packFlags) const override;
			void Unpack(Unpacker& data, unsigned_int32 unpackFlags) override;

			int32 GetSettingCount(void) const override;
			Setting *GetSetting(int32 index) const override;
			void SetSetting(const Setting *setting) override;
	};


	//# \class	PulsateMutator		Causes one of a widget's colors to pulsate.
	//
	//# The $PulsateMutator$ class causes one of a widget's colors to pulsate.
	//
	//# \def	class PulsateMutator final : public Mutator
	//
	//# \ctor	PulsateMutator(const ColorRGBA& color, int32 wave, float frequency, float phaseShift = 0.0F);
	//
	//# \param	color		The pulsate color. The widget color varies between its original value and this color.
	//# \param	wave		The type of waveform. See below for possible values.
	//# \param	frequency	The pulsate frequency, in full periods per millisecond.
	//# \param	phaseShift	The phase shift, in units of full periods. This should be in the range [0,&nbsp;1).
	//
	//# \desc
	//# The $PulsateMutator$ class varies the color of a widget back and forth between its original value and the color specified
	//# by the $color$ parameter. The interpolation can be based on one of the following types of waves specified by the $wave$ parameter.
	//
	//# \table	PulsateWave
	//
	//# \base	Mutator		A $PulsateMutator$ is a specific type of mutator.


	class PulsateMutator final : public Mutator
	{
		friend class MutatorReg<PulsateMutator>;

		private:

			ColorRGBA			pulsateColor;
			WidgetColorType		colorType;

			int32				pulsateWave;
			int32				pulsateBlend;

			float				pulsateFrequency;
			float				pulsatePhaseShift;
			float				pulsateState;

			static float (*const pulsateWaveFunc[kPulsateWaveCount])(float);
			static ColorRGBA (*const pulsateBlendFunc[kPulsateBlendCount])(const ColorRGBA&, const ColorRGBA&, float);

			PulsateMutator();
			PulsateMutator(const PulsateMutator& pulsateMutator);

			Mutator *Replicate(void) const override;

			static float SquareWaveFunc(float t);
			static float TriangleWaveFunc(float t);
			static float SineWaveFunc(float t);

			static ColorRGBA InterpolateBlendFunc(const ColorRGBA& c1, const ColorRGBA& c2, float t);
			static ColorRGBA AddBlendFunc(const ColorRGBA& c1, const ColorRGBA& c2, float t);
			static ColorRGBA MultiplyBlendFunc(const ColorRGBA& c1, const ColorRGBA& c2, float t);

		public:

			C4API PulsateMutator(const ColorRGBA& color, int32 wave, float frequency, float phaseShift = 0.0F);
			C4API ~PulsateMutator();

			const ColorRGBA& GetPulsateColor(void) const
			{
				return (pulsateColor);
			}

			void SetPulsateColor(const ColorRGBA& color)
			{
				pulsateColor = color;
			}

			WidgetColorType GetColorType(void) const
			{
				return (colorType);
			}

			void SetColorType(WidgetColorType type)
			{
				colorType = type;
			}

			int32 GetPulsateWave(void) const
			{
				return (pulsateWave);
			}

			void SetPulsateWave(int32 wave)
			{
				pulsateWave = Min(MaxZero(wave), kPulsateWaveCount - 1);
			}

			int32 GetPulsateBlend(void) const
			{
				return (pulsateBlend);
			}

			void SetPulsateBlend(int32 blend)
			{
				pulsateBlend = Min(MaxZero(blend), kPulsateBlendCount - 1);
			}

			float GetPulsateFrequency(void) const
			{
				return (pulsateFrequency);
			}

			void SetPulsateFrequency(float frequency)
			{
				pulsateFrequency = frequency;
			}

			float GetPulsatePhaseShift(void) const
			{
				return (pulsatePhaseShift);
			}

			void SetPulsatePhaseShift(float phaseShift)
			{
				pulsatePhaseShift = phaseShift;
			}

			float GetPulsateState(void) const
			{
				return (pulsateState);
			}

			void SetPulsateState(float state)
			{
				pulsateState = state;
			}

			void Pack(Packer& data, unsigned_int32 packFlags) const override;
			void Unpack(Unpacker& data, unsigned_int32 unpackFlags) override;
			bool UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags);

			int32 GetSettingCount(void) const override;
			Setting *GetSetting(int32 index) const override;
			void SetSetting(const Setting *setting) override;

			void Move(void) override;
			C4API void Reset(void) override;
	};


	//# \class	RandomizeMutator	Causes a widget's texture coordinates to change randomly.
	//
	//# The $RandomizeMutator$ class causes a widget's texture coordinates to change randomly.
	//
	//# \def	class RandomizeMutator final : public Mutator
	//
	//# \ctor	RandomizeMutator();
	//
	//# \desc
	//# The $RandomizeMutator$ class continually assigns a random offset to the texture coordinates of an image widget.
	//# This can be useful for producing effects like television static with a noise texture.
	//
	//# \base	Mutator		A $RandomizeMutator$ is a specific type of mutator.


	class RandomizeMutator final : public Mutator
	{
		private:

			RandomizeMutator(const RandomizeMutator& randomizeMutator);

			Mutator *Replicate(void) const override;

		public:

			C4API RandomizeMutator();
			C4API ~RandomizeMutator();

			static bool ValidWidget(const Widget *widget);

			void Move(void) override;
	};


	//# \class	ScrollMutator	Causes a widget's texture coordinates to scroll.
	//
	//# The $ScrollMutator$ class causes a widget's texture coordinates to scroll.
	//
	//# \def	class ScrollMutator final : public Mutator
	//
	//# \ctor	ScrollMutator(const Vector2D& speed);
	//
	//# \param	speed	The scrolling speed, in pixels per millisecond.
	//
	//# \desc
	//# The $ScrollMutator$ class continually scrolls the texture coordinates of an image widget at the
	//# linear rate specified by the $speed$ parameter.
	//
	//# \base	Mutator		A $ScrollMutator$ is a specific type of mutator.


	class ScrollMutator final : public Mutator
	{
		friend class MutatorReg<ScrollMutator>;

		private:

			Vector2D	scrollSpeed;
			Vector2D	scrollOffset;

			ScrollMutator();
			ScrollMutator(const ScrollMutator& scrollMutator);

			Mutator *Replicate(void) const override;

		public:

			C4API ScrollMutator(const Vector2D& speed);
			C4API ~ScrollMutator();

			const Vector2D& GetScrollSpeed(void) const
			{
				return (scrollSpeed);
			}

			void SetScrollSpeed(const Vector2D& speed)
			{
				scrollSpeed = speed;
			}

			const Vector2D& GetScrollOffset(void) const
			{
				return (scrollOffset);
			}

			void SetScrollOffset(const Vector2D& offset)
			{
				scrollOffset = offset;
			}

			static bool ValidWidget(const Widget *widget);

			void Pack(Packer& data, unsigned_int32 packFlags) const override;
			void Unpack(Unpacker& data, unsigned_int32 unpackFlags) override;
			bool UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags);

			int32 GetSettingCount(void) const override;
			Setting *GetSetting(int32 index) const override;
			void SetSetting(const Setting *setting) override;

			void Move(void) override;
			C4API void Reset(void) override;
	};


	//# \class	RotateMutator	Causes a widget's texture coordinates to rotate.
	//
	//# The $ScrollMutator$ class causes a widget's texture coordinates to rotate.
	//
	//# \def	class RotateMutator final : public Mutator
	//
	//# \ctor	RotateMutator(float speed);
	//
	//# \param	speed	The rotation speed, in revolutions per millisecond.
	//
	//# \desc
	//# The $RotateMutator$ class rotates the texture coordinates of an image widget at the angular rate specified
	//# by the $speed$ parameter. A positive speed rotates counterclockwise, and a negative speed rotates clockwise.
	//
	//# \base	Mutator		A $RotateMutator$ is a specific type of mutator.


	class RotateMutator final : public Mutator
	{
		friend class MutatorReg<RotateMutator>;

		private:

			float		rotationSpeed;
			float		rotationAngle;

			RotateMutator();
			RotateMutator(const RotateMutator& rotateMutator);

			Mutator *Replicate(void) const override;

		public:

			C4API RotateMutator(float speed);
			C4API ~RotateMutator();

			float GetRotationSpeed(void) const
			{
				return (rotationSpeed);
			}

			void SetRotationSpeed(float speed)
			{
				rotationSpeed = speed;
			}

			float GetRotationAngle(void) const
			{
				return (rotationAngle);
			}

			void SetRotationAngle(float angle)
			{
				rotationAngle = angle;
			}

			static bool ValidWidget(const Widget *widget);

			void Pack(Packer& data, unsigned_int32 packFlags) const override;
			void Unpack(Unpacker& data, unsigned_int32 unpackFlags) override;
			bool UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags);

			int32 GetSettingCount(void) const override;
			Setting *GetSetting(int32 index) const override;
			void SetSetting(const Setting *setting) override;

			void Move(void) override;
			C4API void Reset(void) override;
	};


	//# \class	ScaleMutator	Causes a widget to change scale.
	//
	//# The $ScaleMutator$ class causes a widget to change scale.
	//
	//# \def	class ScaleMutator final : public Mutator
	//
	//# \ctor	ScaleMutator(const Vector2D& start, const Vector2D& finish, float time);
	//
	//# \param	start		The start scale factor.
	//# \param	finish		The finish scale factor.
	//# \param	time		The time over which the widget is scaled from start to finish, in milliseconds.
	//
	//# \desc
	//# The $ScaleMutator$ class scales an image widget from the scale specified by the $start$ parameter to the
	//# scale specified by the $finish$ parameter over the time specified by the $time$ parameter, where the scales
	//# represent multiples of the widget's original size.
	//
	//# \base	Mutator		A $ScaleMutator$ is a specific type of mutator.


	class ScaleMutator final : public Mutator
	{
		friend class MutatorReg<ScaleMutator>;

		private:

			unsigned_int32	scaleMutatorFlags;

			Vector2D		startScale;
			Vector2D		finishScale;

			float			scaleTime;
			float			currentTime;

			ScaleMutator();
			ScaleMutator(const ScaleMutator& scaleMutator);

			Mutator *Replicate(void) const override;

			float GetInitialTime(void) const;
			void UpdateScale(float time) const;

		public:

			C4API ScaleMutator(const Vector2D& start, const Vector2D& finish, float time);
			C4API ~ScaleMutator();

			unsigned_int32 GetScaleMutatorFlags(void) const
			{
				return (scaleMutatorFlags);
			}

			void SetScaleMutatorFlags(unsigned_int32 flags)
			{
				scaleMutatorFlags = flags;
			}

			const Vector2D& GetStartScale(void) const
			{
				return (startScale);
			}

			void SetStartScale(const Vector2D& scale)
			{
				startScale = scale;
			}

			const Vector2D& GetFinishScale(void) const
			{
				return (finishScale);
			}

			void SetFinishScale(const Vector2D& scale)
			{
				finishScale = scale;
			}

			float GetScaleTime(void) const
			{
				return (scaleTime);
			}

			void SetScaleTime(float time)
			{
				scaleTime = time;
			}

			float GetCurrentTime(void) const
			{
				return (currentTime);
			}

			void SetCurrentTime(float time)
			{
				currentTime = time;
			}

			static bool ValidWidget(const Widget *widget);

			void Pack(Packer& data, unsigned_int32 packFlags) const override;
			void Unpack(Unpacker& data, unsigned_int32 unpackFlags) override;
			bool UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags);

			int32 GetSettingCount(void) const override;
			Setting *GetSetting(int32 index) const override;
			void SetSetting(const Setting *setting) override;

			C4API void SetMutatorState(unsigned_int32 state) override;

			void Preprocess(void) override;
			void Move(void) override;
			C4API void Reset(void) override;
	};


	//# \class	FadeMutator		Causes one of a widget's colors to fade to a specific value.
	//
	//# The $FadeMutator$ class causes one of a widget's colors to fade to a specific value.
	//
	//# \def	class FadeMutator final : public Mutator
	//
	//# \ctor	FadeMutator(const ColorRGBA& color, float time);
	//
	//# \param	color		The final fade color.
	//# \param	time		The time over which the fade occurs, in milliseconds.
	//
	//# \desc
	//# The $FadeMutator$ class causes the color of a widget to smoothly change to the value specified by the
	//# $color$ parameter over the time specified by the $time$ parameter.
	//
	//# \base	Mutator		A $FadeMutator$ is a specific type of mutator.


	class FadeMutator final : public Mutator
	{
		friend class MutatorReg<FadeMutator>;

		private:

			unsigned_int32		fadeMutatorFlags;
			ColorRGBA			fadeColor;
			WidgetColorType		colorType;

			float				fadeTime;
			float				currentTime;

			FadeMutator();
			FadeMutator(const FadeMutator& fadeMutator);

			Mutator *Replicate(void) const override;

			float GetInitialTime(void) const;
			void UpdateColor(float time) const;

		public:

			C4API FadeMutator(const ColorRGBA& color, float time);
			C4API ~FadeMutator();

			unsigned_int32 GetFadeMutatorFlags(void) const
			{
				return (fadeMutatorFlags);
			}

			void SetFadeMutatorFlags(unsigned_int32 flags)
			{
				fadeMutatorFlags = flags;
			}

			const ColorRGBA& GetFadeColor(void) const
			{
				return (fadeColor);
			}

			void SetFadeColor(const ColorRGBA& color)
			{
				fadeColor = color;
			}

			WidgetColorType GetColorType(void) const
			{
				return (colorType);
			}

			void SetColorType(WidgetColorType type)
			{
				colorType = type;
			}

			float GetFadeTime(void) const
			{
				return (fadeTime);
			}

			void SetFadeTime(float time)
			{
				fadeTime = time;
			}

			float GetCurrentTime(void) const
			{
				return (currentTime);
			}

			void SetCurrentTime(float time)
			{
				currentTime = time;
			}

			void Pack(Packer& data, unsigned_int32 packFlags) const override;
			void Unpack(Unpacker& data, unsigned_int32 unpackFlags) override;
			bool UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags);

			int32 GetSettingCount(void) const override;
			Setting *GetSetting(int32 index) const override;
			void SetSetting(const Setting *setting) override;

			C4API void SetMutatorState(unsigned_int32 state) override;

			void Preprocess(void) override;
			void Move(void) override;
			C4API void Reset(void) override;
	};


	//# \class	TickerMutator		Causes a text widget's contents to scroll horizontally.
	//
	//# The $TickerMutator$ class causes a text widget's contents to scroll horizontally.
	//
	//# \def	class TickerMutator final : public Mutator
	//
	//# \ctor	TickerMutator(float start, float finish, float time);
	//
	//# \param	start		The start offset of the text, in pixels.
	//# \param	finish		The finish offset of the text, in pixels.
	//# \param	time		The time over which the text is scrolled from start to finish, in milliseconds.
	//
	//# \desc
	//# The $TickerMutator$ class scrolls text in a text widget like a stock ticker. The text must be
	//# displayed as a single line.
	//
	//# \base	Mutator		A $TickerMutator$ is a specific type of mutator.


	class TickerMutator final : public Mutator
	{
		friend class MutatorReg<TickerMutator>;

		private:

			unsigned_int32	tickerMutatorFlags;

			float			startPosition;
			float			finishPosition;

			float			scrollSpeed;
			float			currentPosition;

			TickerMutator();
			TickerMutator(const TickerMutator& tickerMutator);

			Mutator *Replicate(void) const override;

			float GetInitialPosition(void) const;
			void UpdatePosition(float position) const;

		public:

			C4API TickerMutator(float start, float finish, float time);
			C4API ~TickerMutator();

			unsigned_int32 GetTickerMutatorFlags(void) const
			{
				return (tickerMutatorFlags);
			}

			void SetTickerMutatorFlags(unsigned_int32 flags)
			{
				tickerMutatorFlags = flags;
			}

			float GetStartPosition(void) const
			{
				return (startPosition);
			}

			void SetStartPosition(float position)
			{
				startPosition = position;
			}

			float GetFinishPosition(void) const
			{
				return (finishPosition);
			}

			void SetFinishPosition(float position)
			{
				finishPosition = position;
			}

			float GetScrollSpeed(void) const
			{
				return (scrollSpeed);
			}

			void SetScrollSpeed(float speed)
			{
				scrollSpeed = speed;
			}

			float GetCurrentPosition(void) const
			{
				return (currentPosition);
			}

			void SetCurrentPosition(float position)
			{
				currentPosition = position;
			}

			static bool ValidWidget(const Widget *widget);

			void Pack(Packer& data, unsigned_int32 packFlags) const override;
			void Unpack(Unpacker& data, unsigned_int32 unpackFlags) override;
			bool UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags);

			int32 GetSettingCount(void) const override;
			Setting *GetSetting(int32 index) const override;
			void SetSetting(const Setting *setting) override;

			C4API void SetMutatorState(unsigned_int32 state) override;

			void Preprocess(void) override;
			void Move(void) override;
			C4API void Reset(void) override;
	};


	//# \class	AnimateMutator		Causes an image widget's texture coordinates to animate through a grid.
	//
	//# The $AnimateMutator$ class causes an image widget's texture coordinates to animate through a grid.
	//
	//# \def	class AnimateMutator final : public Mutator
	//
	//# \ctor	AnimateMutator(int32 x, int32 y, int32 total, float rate);
	//
	//# \param	x			The number of grid cells in the texture map horizontally.
	//# \param	y			The number of grid cells in the texture map vertically.
	//# \param	total		The total number of grid cells that contain animation frames.
	//# \param	rate		The animation rate, in frames per millisecond.
	//
	//# \desc
	//# The $AnimateMutator$ class plays a sequence of frames from a texture map in an image widget.
	//# The $x$ and $y$ parameters specify how many equal-sized grid cells exist in the texture map,
	//# and the $total$ parameter specifies the total number of grid cells that should be used in the
	//# animation, starting at the upper-left corner and proceeding to the right through each row and
	//# downward through the rows.
	//
	//# \base	Mutator		A $AnimateMutator$ is a specific type of mutator.


	class AnimateMutator final : public Mutator
	{
		friend class MutatorReg<AnimateMutator>;

		private:

			unsigned_int32	animateMutatorFlags;

			int32			frameCountX;
			int32			frameCountY;
			int32			totalFrameCount;

			float			frameRate;
			float			currentFrame;

			AnimateMutator();
			AnimateMutator(const AnimateMutator& animateMutator);

			Mutator *Replicate(void) const override;

			float GetInitialFrame(void) const;
			void UpdateFrame(float frame) const;

		public:

			C4API AnimateMutator(int32 x, int32 y, int32 total, float rate);
			C4API ~AnimateMutator();

			unsigned_int32 GetAnimateMutatorFlags(void) const
			{
				return (animateMutatorFlags);
			}

			void SetAnimateMutatorFlags(unsigned_int32 flags)
			{
				animateMutatorFlags = flags;
			}

			int32 GetFrameCountX(void) const
			{
				return (frameCountX);
			}

			int32 GetFrameCountY(void) const
			{
				return (frameCountY);
			}

			int32 GetTotalFrameCount(void) const
			{
				return (totalFrameCount);
			}

			void SetFrameCount(int32 x, int32 y, int32 total)
			{
				frameCountX = x;
				frameCountY = y;
				totalFrameCount = total;
			}

			float GetFrameRate(void) const
			{
				return (frameRate);
			}

			void SetFrameRate(float rate)
			{
				frameRate = rate;
			}

			float GetCurrentFrame(void) const
			{
				return (currentFrame);
			}

			void SetCurrentFrame(float frame)
			{
				currentFrame = frame;
			}

			static bool ValidWidget(const Widget *widget);

			void Pack(Packer& data, unsigned_int32 packFlags) const override;
			void Unpack(Unpacker& data, unsigned_int32 unpackFlags) override;
			bool UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags);

			int32 GetSettingCount(void) const override;
			Setting *GetSetting(int32 index) const override;
			void SetSetting(const Setting *setting) override;

			C4API void SetMutatorState(unsigned_int32 state) override;

			void Preprocess(void) override;
			void Move(void) override;
			C4API void Reset(void) override;
	};
}


#endif

// ZYUQURM
