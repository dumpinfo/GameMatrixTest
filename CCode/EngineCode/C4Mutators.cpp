 

#include "C4Mutators.h"
#include "C4Panels.h"


using namespace C4;


namespace C4
{
	template class Registrable<Mutator, MutatorRegistration>;
}


float (*const PulsateMutator::pulsateWaveFunc[kPulsateWaveCount])(float) =
{
	&PulsateMutator::SquareWaveFunc, &PulsateMutator::TriangleWaveFunc, &PulsateMutator::SineWaveFunc
};

ColorRGBA (*const PulsateMutator::pulsateBlendFunc[kPulsateBlendCount])(const ColorRGBA&, const ColorRGBA&, float) =
{
	&PulsateMutator::InterpolateBlendFunc, &PulsateMutator::AddBlendFunc, &PulsateMutator::MultiplyBlendFunc
};


MutatorRegistration::MutatorRegistration(MutatorType type, const char *name) : Registration<Mutator, MutatorRegistration>(type)
{
	mutatorName = name;
}

MutatorRegistration::~MutatorRegistration()
{
}


Mutator::Mutator(MutatorType type)
{
	mutatorType = type;
	mutatorKey[0] = 0;
	mutatorState = 0;

	targetWidget = nullptr;
}

Mutator::Mutator(const Mutator& mutator)
{
	mutatorType = mutator.mutatorType;
	mutatorKey = mutator.mutatorKey;
	mutatorState = mutator.mutatorState & ~kMutatorTerminated;

	targetWidget = nullptr;
}

Mutator::~Mutator()
{
}

Mutator *Mutator::New(MutatorType type)
{
	Type	data[2];

	data[0] = type;
	data[1] = 0;

	Unpacker unpacker(data);
	return (Create(unpacker));
}

Mutator *Mutator::CreateUnknown(Unpacker& data, unsigned_int32 unpackFlags)
{
	return (new UnknownMutator(data.GetType()));
}

bool Mutator::ValidWidget(const Widget *widget)
{
	return (true);
}

void Mutator::RegisterStandardMutators(void)
{
	const StringTable *table = TheInterfaceMgr->GetStringTable();

	static MutatorReg<UnknownMutator> unknownRegistration(kMutatorUnknown, table->GetString(StringID('MUTR', kMutatorUnknown)));
	static MutatorReg<PulsateMutator> pulsateRegistration(kMutatorPulsate, table->GetString(StringID('MUTR', kMutatorPulsate)));
	static MutatorReg<RandomizeMutator> randomizeRegistration(kMutatorRandomize, table->GetString(StringID('MUTR', kMutatorRandomize)));
	static MutatorReg<ScrollMutator> scrollRegistration(kMutatorScroll, table->GetString(StringID('MUTR', kMutatorScroll)));
	static MutatorReg<RotateMutator> rotateRegistration(kMutatorRotate, table->GetString(StringID('MUTR', kMutatorRotate)));
	static MutatorReg<ScaleMutator> scaleRegistration(kMutatorScale, table->GetString(StringID('MUTR', kMutatorScale)));
	static MutatorReg<FadeMutator> fadeRegistration(kMutatorFade, table->GetString(StringID('MUTR', kMutatorFade)));
	static MutatorReg<TickerMutator> tickerRegistration(kMutatorTicker, table->GetString(StringID('MUTR', kMutatorTicker)));
	static MutatorReg<AnimateMutator> animateRegistration(kMutatorAnimate, table->GetString(StringID('MUTR', kMutatorAnimate)));

	SetUnknownCreator(&CreateUnknown);
}

void Mutator::PackType(Packer& data) const
{
	data << mutatorType;
}

void Mutator::Pack(Packer& data, unsigned_int32 packFlags) const
{ 
	unsigned_int32 state = mutatorState;
	if (targetWidget->GetManipulator())
	{ 
		state &= ~kMutatorTerminated;
	} 

	data << ChunkHeader('STAT', 4);
	data << state; 

	if (mutatorKey[0] != 0) 
	{ 
		PackHandle handle = data.BeginChunk('KEY ');
		data << mutatorKey;
		data.EndChunk(handle);
	} 

	data << TerminatorChunk;
}

void Mutator::Unpack(Unpacker& data, unsigned_int32 unpackFlags)
{
	UnpackChunkList<Mutator>(data, unpackFlags);
}

bool Mutator::UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags)
{
	switch (chunkHeader->chunkType)
	{
		case 'STAT':

			data >> mutatorState;
			return (true);

		case 'KEY ':

			data >> mutatorKey;
			return (true);
	}

	return (false);
}

void *Mutator::BeginSettingsUnpack(void)
{
	mutatorKey[0] = 0;
	return (nullptr);
}

int32 Mutator::GetSettingCount(void) const
{
	return (4);
}

Setting *Mutator::GetSetting(int32 index) const
{
	const StringTable *table = TheInterfaceMgr->GetStringTable();

	if (index == 0)
	{
		const char *title = table->GetString(StringID('MUTR', 'SETT'));
		return (new HeadingSetting('MUTR', title));
	}

	if (index == 1)
	{
		const char *title = table->GetString(StringID('MUTR', 'IDNT'));
		return (new TextSetting('IDNT', mutatorKey, title, kMaxMutatorKeyLength));
	}

	if (index == 2)
	{
		const char *title = table->GetString(StringID('MUTR', 'DSAB'));
		return (new BooleanSetting('DSAB', ((mutatorState & kMutatorDisabled) != 0), title));
	}

	if (index == 3)
	{
		const char *title = table->GetString(StringID('MUTR', 'RVRS'));
		return (new BooleanSetting('RVRS', ((mutatorState & kMutatorReverse) != 0), title));
	}

	return (nullptr);
}

void Mutator::SetSetting(const Setting *setting)
{
	Type identifier = setting->GetSettingIdentifier();

	if (identifier == 'IDNT')
	{
		mutatorKey = static_cast<const TextSetting *>(setting)->GetText();
	}
	else if (identifier == 'DSAB')
	{
		if (static_cast<const BooleanSetting *>(setting)->GetBooleanValue())
		{
			mutatorState |= kMutatorDisabled;
		}
		else
		{
			mutatorState &= ~kMutatorDisabled;
		}
	}
	else if (identifier == 'RVRS')
	{
		if (static_cast<const BooleanSetting *>(setting)->GetBooleanValue())
		{
			mutatorState |= kMutatorReverse;
		}
		else
		{
			mutatorState &= ~kMutatorReverse;
		}
	}
}

void Mutator::SetMutatorState(unsigned_int32 state)
{
	mutatorState = state;
}

void Mutator::Preprocess(void)
{
}

void Mutator::Move(void)
{
}

void Mutator::Reset(void)
{
	mutatorState &= ~kMutatorTerminated;
}


UnknownMutator::UnknownMutator() : Mutator(kMutatorUnknown)
{
	unknownType = 0;
	unknownSize = 0;
	unknownData = nullptr;
}

UnknownMutator::UnknownMutator(MutatorType type) : Mutator(kMutatorUnknown)
{
	unknownType = type;
	unknownSize = 0;
	unknownData = nullptr;
}

UnknownMutator::UnknownMutator(const UnknownMutator& unknownMutator) : Mutator(unknownMutator)
{
	unknownType = unknownMutator.unknownType;
	unknownSize = unknownMutator.unknownSize;
	unknownData = new char[unknownSize];
	MemoryMgr::CopyMemory(unknownMutator.unknownData, unknownData, unknownSize);
}

UnknownMutator::~UnknownMutator()
{
	delete[] unknownData;
}

Mutator *UnknownMutator::Replicate(void) const
{
	return (new UnknownMutator(*this));
}

bool UnknownMutator::ValidWidget(const Widget *widget)
{
	return (false);
}

void UnknownMutator::PackType(Packer& data) const
{
	data << unknownType;
}

void UnknownMutator::Pack(Packer& data, unsigned_int32 packFlags) const
{
	data.WriteData(unknownData, unknownSize);
}

void UnknownMutator::Unpack(Unpacker& data, unsigned_int32 unpackFlags)
{
	unsigned_int32 size = data.GetUnknownSize() - sizeof(MutatorType);
	unknownSize = size;
	unknownData = new char[size];
	data.ReadData(unknownData, size);
}

int32 UnknownMutator::GetSettingCount(void) const
{
	return (1);
}

Setting *UnknownMutator::GetSetting(int32 index) const
{
	if (index == 0)
	{
		const StringTable *table = TheInterfaceMgr->GetStringTable();
		const char *title = table->GetString(StringID('MUTR', kMutatorUnknown, 'TYPE'));
		return (new InfoSetting('TYPE', Text::TypeToHexCharString(unknownType), title));
	}

	return (nullptr);
}

void UnknownMutator::SetSetting(const Setting *setting)
{
}


PulsateMutator::PulsateMutator() : Mutator(kMutatorPulsate)
{
	pulsateColor.Set(1.0F, 1.0F, 1.0F, 1.0F);
	colorType = kWidgetColorDefault;

	pulsateWave = kPulsateWaveSquare;
	pulsateBlend = kPulsateBlendInterpolate;

	pulsateFrequency = 0.001F;
	pulsatePhaseShift = 0.0F;
	pulsateState = 0.0F;
}

PulsateMutator::PulsateMutator(const ColorRGBA& color, int32 wave, float frequency, float phaseShift) : Mutator(kMutatorPulsate)
{
	pulsateColor = color;
	colorType = kWidgetColorDefault;

	pulsateWave = Min(MaxZero(wave), kPulsateWaveCount - 1);
	pulsateBlend = kPulsateBlendInterpolate;

	pulsateFrequency = frequency;
	pulsatePhaseShift = phaseShift;
	pulsateState = 0.0F;
}

PulsateMutator::PulsateMutator(const PulsateMutator& pulsateMutator) : Mutator(pulsateMutator)
{
	pulsateColor = pulsateMutator.pulsateColor;
	colorType = pulsateMutator.colorType;

	pulsateWave = pulsateMutator.pulsateWave;
	pulsateBlend = pulsateMutator.pulsateBlend;

	pulsateFrequency = pulsateMutator.pulsateFrequency;
	pulsatePhaseShift = pulsateMutator.pulsatePhaseShift;
	pulsateState = 0.0F;
}

PulsateMutator::~PulsateMutator()
{
}

Mutator *PulsateMutator::Replicate(void) const
{
	return (new PulsateMutator(*this));
}

void PulsateMutator::Pack(Packer& data, unsigned_int32 packFlags) const
{
	Mutator::Pack(data, packFlags);

	data << ChunkHeader('COLR', sizeof(ColorRGBA));
	data << pulsateColor;

	data << ChunkHeader('CTYP', 4);
	data << colorType;

	data << ChunkHeader('WAVE', 4);
	data << pulsateWave;

	data << ChunkHeader('BLND', 4);
	data << pulsateBlend;

	data << ChunkHeader('FREQ', 4);
	data << pulsateFrequency;

	data << ChunkHeader('PHAS', 4);
	data << pulsatePhaseShift;

	if (!GetTargetWidget()->GetManipulator())
	{
		data << ChunkHeader('STAT', 4);
		data << pulsateState;
	}

	data << TerminatorChunk;
}

void PulsateMutator::Unpack(Unpacker& data, unsigned_int32 unpackFlags)
{
	Mutator::Unpack(data, unpackFlags);
	UnpackChunkList<PulsateMutator>(data, unpackFlags);
}

bool PulsateMutator::UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags)
{
	switch (chunkHeader->chunkType)
	{
		case 'COLR':

			data >> pulsateColor;
			return (true);

		case 'CTYP':

			data >> colorType;
			return (true);

		case 'WAVE':

			data >> pulsateWave;
			return (true);

		case 'BLND':

			data >> pulsateBlend;
			return (true);

		case 'FREQ':

			data >> pulsateFrequency;
			return (true);

		case 'PHAS':

			data >> pulsatePhaseShift;
			return (true);

		case 'STAT':

			data >> pulsateState;
			return (true);
	}

	return (false);
}

int32 PulsateMutator::GetSettingCount(void) const
{
	return (Mutator::GetSettingCount() + 7);
}

Setting *PulsateMutator::GetSetting(int32 index) const
{
	int32 count = Mutator::GetSettingCount();
	if (index < count)
	{
		return (Mutator::GetSetting(index));
	}

	const StringTable *table = TheInterfaceMgr->GetStringTable();

	if (index == count)
	{
		const char *title = table->GetString(StringID('MUTR', kMutatorPulsate, 'SETT'));
		return (new HeadingSetting(kMutatorPulsate, title));
	}

	if (index == count + 1)
	{
		const char *title = table->GetString(StringID('MUTR', kMutatorPulsate, 'COLR'));
		const char *picker = table->GetString(StringID('MUTR', kMutatorPulsate, 'PCOL'));
		return (new ColorSetting('COLR', pulsateColor, title, picker, kColorPickerAlpha));
	}

	if (index == count + 2)
	{
		int32 selection = 0;
		for (machine a = 1; a < Widget::kWidgetColorCount; a++)
		{
			if (colorType == Widget::widgetColorType[a])
			{
				selection = a;
				break;
			}
		}

		const char *title = table->GetString(StringID('WDGT', 'WDGT', 'CTYP'));
		MenuSetting *menu = new MenuSetting('CTYP', selection, title, Widget::kWidgetColorCount);
		menu->SetMenuItemString(0, table->GetString(StringID('WDGT', 'WDGT', 'CTYP', 'DFLT')));
		for (machine a = 1; a < Widget::kWidgetColorCount; a++)
		{
			menu->SetMenuItemString(a, table->GetString(StringID('WDGT', 'WDGT', 'CTYP', Widget::widgetColorType[a])));
		}

		return (menu);
	}

	if (index == count + 3)
	{
		const char *title = table->GetString(StringID('MUTR', kMutatorPulsate, 'WAVE'));
		MenuSetting *menu = new MenuSetting('WAVE', pulsateWave, title, kPulsateWaveCount);

		menu->SetMenuItemString(0, table->GetString(StringID('MUTR', kMutatorPulsate, 'WAVE', 'SQAR')));
		menu->SetMenuItemString(1, table->GetString(StringID('MUTR', kMutatorPulsate, 'WAVE', 'TRI ')));
		menu->SetMenuItemString(2, table->GetString(StringID('MUTR', kMutatorPulsate, 'WAVE', 'SINE')));

		return (menu);
	}

	if (index == count + 4)
	{
		const char *title = table->GetString(StringID('MUTR', kMutatorPulsate, 'BLND'));
		MenuSetting *menu = new MenuSetting('BLND', pulsateBlend, title, kPulsateBlendCount);

		menu->SetMenuItemString(0, table->GetString(StringID('MUTR', kMutatorPulsate, 'BLND', 'ITRP')));
		menu->SetMenuItemString(1, table->GetString(StringID('MUTR', kMutatorPulsate, 'BLND', 'ADD ')));
		menu->SetMenuItemString(2, table->GetString(StringID('MUTR', kMutatorPulsate, 'BLND', 'MULT')));

		return (menu);
	}

	if (index == count + 5)
	{
		const char *title = table->GetString(StringID('MUTR', kMutatorPulsate, 'FREQ'));
		return (new TextSetting('FREQ', pulsateFrequency * 1000.0F, title));
	}

	if (index == count + 6)
	{
		const char *title = table->GetString(StringID('MUTR', kMutatorPulsate, 'PHAS'));
		return (new TextSetting('PHAS', pulsatePhaseShift, title));
	}

	return (nullptr);
}

void PulsateMutator::SetSetting(const Setting *setting)
{
	Type identifier = setting->GetSettingIdentifier();

	if (identifier == 'COLR')
	{
		pulsateColor = static_cast<const ColorSetting *>(setting)->GetColor();
	}
	else if (identifier == 'CTYP')
	{
		colorType = Widget::widgetColorType[static_cast<const MenuSetting *>(setting)->GetMenuSelection()];
	}
	else if (identifier == 'WAVE')
	{
		pulsateWave = static_cast<const MenuSetting *>(setting)->GetMenuSelection();
	}
	else if (identifier == 'BLND')
	{
		pulsateBlend = static_cast<const MenuSetting *>(setting)->GetMenuSelection();
	}
	else if (identifier == 'FREQ')
	{
		const char *text = static_cast<const TextSetting *>(setting)->GetText();
		pulsateFrequency = FmaxZero(Text::StringToFloat(text) * 0.001F);
	}
	else if (identifier == 'PHAS')
	{
		const char *text = static_cast<const TextSetting *>(setting)->GetText();
		pulsatePhaseShift = Text::StringToFloat(text);
	}
	else
	{
		Mutator::SetSetting(setting);
	}
}

float PulsateMutator::SquareWaveFunc(float t)
{
	return ((t > 0.5F) ? 1.0F : 0.0F);
}

float PulsateMutator::TriangleWaveFunc(float t)
{
	return (1.0F - 2.0F * Fabs(t - 0.5F));
}

float PulsateMutator::SineWaveFunc(float t)
{
	float s = Math::GetTrigTable()[(int32) (t * 128.0F) & 0x7F].y;
	return (s * s);
}

ColorRGBA PulsateMutator::InterpolateBlendFunc(const ColorRGBA& c1, const ColorRGBA& c2, float t)
{
	return (c1 * (1.0F - t) + c2 * t);
}

ColorRGBA PulsateMutator::AddBlendFunc(const ColorRGBA& c1, const ColorRGBA& c2, float t)
{
	return (c1 + c2 * t);
}

ColorRGBA PulsateMutator::MultiplyBlendFunc(const ColorRGBA& c1, const ColorRGBA& c2, float t)
{
	float u = 1.0F - t;
	return (ColorRGBA(c1.red * (u + c2.red * t), c1.green * (u + c2.green * t), c1.blue * (u + c2.blue * t), c1.alpha * (u + c2.alpha * t)));
}

void PulsateMutator::Move(void)
{
	float offset = pulsatePhaseShift;

	float state = pulsateState + pulsateFrequency * TheTimeMgr->GetSystemFloatDeltaTime() + offset;
	if (state > 1.0F)
	{
		state -= PositiveFloor(state);
	}
	else if (state < 0.0F)
	{
		state += PositiveFloor(-state) + 1.0F;
	}

	pulsateState = state - offset;
	float t = (*pulsateWaveFunc[pulsateWave])(state);

	Widget *widget = GetTargetWidget();
	widget->SetDynamicWidgetColor((*pulsateBlendFunc[pulsateBlend])(widget->GetWidgetColor(colorType), pulsateColor, t), colorType);
}

void PulsateMutator::Reset(void)
{
	pulsateState = 0.0F;
}


RandomizeMutator::RandomizeMutator() : Mutator(kMutatorRandomize)
{
}

RandomizeMutator::RandomizeMutator(const RandomizeMutator& randomizeMutator) : Mutator(randomizeMutator)
{
}

RandomizeMutator::~RandomizeMutator()
{
}

Mutator *RandomizeMutator::Replicate(void) const
{
	return (new RandomizeMutator(*this));
}

bool RandomizeMutator::ValidWidget(const Widget *widget)
{
	return (widget->GetWidgetType() == kWidgetImage);
}

void RandomizeMutator::Move(void)
{
	float x = Math::RandomFloat(1.0F);
	float y = Math::RandomFloat(1.0F);

	ImageWidget *image = static_cast<ImageWidget *>(GetTargetWidget());
	image->SetImageOffset(Vector2D(x, y));
}


ScrollMutator::ScrollMutator() : Mutator(kMutatorScroll)
{
	scrollSpeed.Set(0.01F, 0.01F);
	scrollOffset.Set(0.0F, 0.0F);
}

ScrollMutator::ScrollMutator(const Vector2D& speed) : Mutator(kMutatorScroll)
{
	scrollSpeed = speed;
	scrollOffset.Set(0.0F, 0.0F);
}

ScrollMutator::ScrollMutator(const ScrollMutator& scrollMutator) : Mutator(scrollMutator)
{
	scrollSpeed = scrollMutator.scrollSpeed;
	scrollOffset = scrollMutator.scrollOffset;
}

ScrollMutator::~ScrollMutator()
{
}

Mutator *ScrollMutator::Replicate(void) const
{
	return (new ScrollMutator(*this));
}

bool ScrollMutator::ValidWidget(const Widget *widget)
{
	return (widget->GetWidgetType() == kWidgetImage);
}

void ScrollMutator::Pack(Packer& data, unsigned_int32 packFlags) const
{
	Mutator::Pack(data, packFlags);

	data << ChunkHeader('SPED', sizeof(Vector2D));
	data << scrollSpeed;

	if (!GetTargetWidget()->GetManipulator())
	{
		data << ChunkHeader('OFST', sizeof(Vector2D));
		data << scrollOffset;
	}

	data << TerminatorChunk;
}

void ScrollMutator::Unpack(Unpacker& data, unsigned_int32 unpackFlags)
{
	Mutator::Unpack(data, unpackFlags);
	UnpackChunkList<ScrollMutator>(data, unpackFlags);
}

bool ScrollMutator::UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags)
{
	switch (chunkHeader->chunkType)
	{
		case 'SPED':

			data >> scrollSpeed;
			return (true);

		case 'OFST':

			data >> scrollOffset;
			return (true);
	}

	return (false);
}

int32 ScrollMutator::GetSettingCount(void) const
{
	return (Mutator::GetSettingCount() + 3);
}

Setting *ScrollMutator::GetSetting(int32 index) const
{
	int32 count = Mutator::GetSettingCount();
	if (index < count)
	{
		return (Mutator::GetSetting(index));
	}

	const StringTable *table = TheInterfaceMgr->GetStringTable();

	if (index == count)
	{
		const char *title = table->GetString(StringID('MUTR', kMutatorScroll, 'SETT'));
		return (new HeadingSetting(kMutatorScroll, title));
	}

	if (index == count + 1)
	{
		const char *title = table->GetString(StringID('MUTR', kMutatorScroll, 'SPDX'));
		return (new TextSetting('SPDX', scrollSpeed.x * 1000.0F, title));
	}

	if (index == count + 2)
	{
		const char *title = table->GetString(StringID('MUTR', kMutatorScroll, 'SPDY'));
		return (new TextSetting('SPDY', scrollSpeed.y * 1000.0F, title));
	}

	return (nullptr);
}

void ScrollMutator::SetSetting(const Setting *setting)
{
	Type identifier = setting->GetSettingIdentifier();

	if (identifier == 'SPDX')
	{
		const char *text = static_cast<const TextSetting *>(setting)->GetText();
		scrollSpeed.x = Text::StringToFloat(text) * 0.001F;
	}
	else if (identifier == 'SPDY')
	{
		const char *text = static_cast<const TextSetting *>(setting)->GetText();
		scrollSpeed.y = Text::StringToFloat(text) * 0.001F;
	}
	else
	{
		Mutator::SetSetting(setting);
	}
}

void ScrollMutator::Move(void)
{
	Widget *widget = GetTargetWidget();

	const Vector2D& size = widget->GetWidgetSize();
	float w = 1.0F / size.x;
	float h = 1.0F / size.y;

	float sx = scrollSpeed.x;
	float sy = scrollSpeed.y;
	if (GetMutatorState() & kMutatorReverse)
	{
		sx = -sx;
		sy = -sy;
	}

	float dt = TheTimeMgr->GetSystemFloatDeltaTime();

	float dx = scrollOffset.x + sx * w * dt;
	if (dx > 1.0F)
	{
		dx -= 1.0F;
	}
	else if (dx < -1.0F)
	{
		dx += 1.0F;
	}

	float dy = scrollOffset.y + sy * h * dt;
	if (dy > 1.0F)
	{
		dy -= 1.0F;
	}
	else if (dy < -1.0F)
	{
		dy += 1.0F;
	}

	scrollOffset.Set(dx, dy);

	ImageWidget *image = static_cast<ImageWidget *>(widget);
	image->SetImageOffset(-scrollOffset);
}

void ScrollMutator::Reset(void)
{
	scrollOffset.Set(0.0F, 0.0F);
}


RotateMutator::RotateMutator() : Mutator(kMutatorRotate)
{
	rotationAngle = 0.0F;
	rotationSpeed = 0.001F;
}

RotateMutator::RotateMutator(float speed) : Mutator(kMutatorRotate)
{
	rotationAngle = 0.0F;
	rotationSpeed = speed;
}

RotateMutator::RotateMutator(const RotateMutator& rotateMutator) : Mutator(rotateMutator)
{
	rotationAngle = 0.0F;
	rotationSpeed = rotateMutator.rotationSpeed;
}

RotateMutator::~RotateMutator()
{
}

Mutator *RotateMutator::Replicate(void) const
{
	return (new RotateMutator(*this));
}

bool RotateMutator::ValidWidget(const Widget *widget)
{
	return (widget->GetWidgetType() == kWidgetImage);
}

void RotateMutator::Pack(Packer& data, unsigned_int32 packFlags) const
{
	Mutator::Pack(data, packFlags);

	data << ChunkHeader('SPED', 4);
	data << rotationSpeed;

	if (!GetTargetWidget()->GetManipulator())
	{
		data << ChunkHeader('ANGL', 4);
		data << rotationAngle;
	}

	data << TerminatorChunk;
}

void RotateMutator::Unpack(Unpacker& data, unsigned_int32 unpackFlags)
{
	Mutator::Unpack(data, unpackFlags);
	UnpackChunkList<RotateMutator>(data, unpackFlags);
}

bool RotateMutator::UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags)
{
	switch (chunkHeader->chunkType)
	{
		case 'SPED':

			data >> rotationSpeed;
			return (true);

		case 'ANGL':

			data >> rotationAngle;
			return (true);
	}

	return (false);
}

int32 RotateMutator::GetSettingCount(void) const
{
	return (Mutator::GetSettingCount() + 2);
}

Setting *RotateMutator::GetSetting(int32 index) const
{
	int32 count = Mutator::GetSettingCount();
	if (index < count)
	{
		return (Mutator::GetSetting(index));
	}

	const StringTable *table = TheInterfaceMgr->GetStringTable();

	if (index == count)
	{
		const char *title = table->GetString(StringID('MUTR', kMutatorRotate, 'SETT'));
		return (new HeadingSetting(kMutatorRotate, title));
	}

	if (index == count + 1)
	{
		const char *title = table->GetString(StringID('MUTR', kMutatorRotate, 'SPED'));
		return (new TextSetting('SPED', rotationSpeed * 1000.0F, title));
	}

	return (nullptr);
}

void RotateMutator::SetSetting(const Setting *setting)
{
	Type identifier = setting->GetSettingIdentifier();

	if (identifier == 'SPED')
	{
		const char *text = static_cast<const TextSetting *>(setting)->GetText();
		rotationSpeed = Text::StringToFloat(text) * 0.001F;
	}
	else
	{
		Mutator::SetSetting(setting);
	}
}

void RotateMutator::Move(void)
{
	float speed = rotationSpeed;
	if (GetMutatorState() & kMutatorReverse)
	{
		speed = -speed;
	}

	float angle = rotationAngle + speed * TheTimeMgr->GetSystemFloatDeltaTime();
	if (angle > 1.0F)
	{
		angle -= 1.0F;
	}
	else if (angle < -1.0F)
	{
		angle += 1.0F;
	}

	rotationAngle = angle;

	ImageWidget *image = static_cast<ImageWidget *>(GetTargetWidget());
	image->SetImageRotation(angle * K::tau);
}

void RotateMutator::Reset(void)
{
	rotationAngle = 0.0F;
}


ScaleMutator::ScaleMutator() : Mutator(kMutatorScale)
{
	scaleMutatorFlags = 0;

	startScale.Set(1.0F, 1.0F);
	finishScale.Set(2.0F, 2.0F);

	scaleTime = 1000.0F;
	currentTime = 0.0F;
}

ScaleMutator::ScaleMutator(const Vector2D& start, const Vector2D& finish, float time) : Mutator(kMutatorScale)
{
	scaleMutatorFlags = 0;

	startScale = start;
	finishScale = finish;

	scaleTime = time;
	currentTime = 0.0F;
}

inline float ScaleMutator::GetInitialTime(void) const
{
	return ((GetMutatorState() & kMutatorReverse) ? scaleTime : 0.0F);
}

ScaleMutator::ScaleMutator(const ScaleMutator& scaleMutator) : Mutator(scaleMutator)
{
	scaleMutatorFlags = scaleMutator.scaleMutatorFlags;

	startScale = scaleMutator.startScale;
	finishScale = scaleMutator.finishScale;

	scaleTime = scaleMutator.scaleTime;
	currentTime = GetInitialTime();
}

ScaleMutator::~ScaleMutator()
{
}

Mutator *ScaleMutator::Replicate(void) const
{
	return (new ScaleMutator(*this));
}

bool ScaleMutator::ValidWidget(const Widget *widget)
{
	return (widget->GetBaseWidgetType() == kWidgetQuad);
}

void ScaleMutator::Pack(Packer& data, unsigned_int32 packFlags) const
{
	Mutator::Pack(data, packFlags);

	data << ChunkHeader('FLAG', 4);
	data << scaleMutatorFlags;

	data << ChunkHeader('SCAL', sizeof(Vector2D) * 2);
	data << startScale;
	data << finishScale;

	data << ChunkHeader('STIM', 4);
	data << scaleTime;

	float time = (!GetTargetWidget()->GetManipulator()) ? currentTime : GetInitialTime();
	data << ChunkHeader('CTIM', 4);
	data << time;

	data << TerminatorChunk;
}

void ScaleMutator::Unpack(Unpacker& data, unsigned_int32 unpackFlags)
{
	Mutator::Unpack(data, unpackFlags);
	UnpackChunkList<ScaleMutator>(data, unpackFlags);
}

bool ScaleMutator::UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags)
{
	switch (chunkHeader->chunkType)
	{
		case 'FLAG':

			data >> scaleMutatorFlags;
			return (true);

		case 'SCAL':

			data >> startScale;
			data >> finishScale;
			return (true);

		case 'STIM':

			data >> scaleTime;
			return (true);

		case 'CTIM':

			data >> currentTime;
			return (true);
	}

	return (false);
}

int32 ScaleMutator::GetSettingCount(void) const
{
	return (Mutator::GetSettingCount() + 10);
}

Setting *ScaleMutator::GetSetting(int32 index) const
{
	int32 count = Mutator::GetSettingCount();
	if (index < count)
	{
		return (Mutator::GetSetting(index));
	}

	const StringTable *table = TheInterfaceMgr->GetStringTable();

	if (index == count)
	{
		const char *title = table->GetString(StringID('MUTR', kMutatorScale, 'SETT'));
		return (new HeadingSetting(kMutatorScale, title));
	}

	if (index == count + 1)
	{
		const char *title = table->GetString(StringID('MUTR', kMutatorScale, 'STRT'));
		return (new BooleanSetting('STRT', ((scaleMutatorFlags & kScaleMutatorRestartable) != 0), title));
	}

	if (index == count + 2)
	{
		if (GetTargetWidget()->GetWidgetType() == kWidgetImage)
		{
			const char *title = table->GetString(StringID('MUTR', kMutatorScale, 'TEXC'));
			return (new BooleanSetting('TEXC', ((scaleMutatorFlags & kScaleMutatorScaleTexcoords) != 0), title));
		}

		return (nullptr);
	}

	if (index == count + 3)
	{
		const char *title = table->GetString(StringID('MUTR', kMutatorScale, 'FLPH'));
		return (new BooleanSetting('FLPH', ((scaleMutatorFlags & kScaleMutatorFlipHorizontal) != 0), title));
	}

	if (index == count + 4)
	{
		const char *title = table->GetString(StringID('MUTR', kMutatorScale, 'FLPV'));
		return (new BooleanSetting('FLPV', ((scaleMutatorFlags & kScaleMutatorFlipVertical) != 0), title));
	}

	if (index == count + 5)
	{
		const char *title = table->GetString(StringID('MUTR', kMutatorScale, 'XSC1'));
		return (new TextSetting('XSC1', startScale.x, title));
	}

	if (index == count + 6)
	{
		const char *title = table->GetString(StringID('MUTR', kMutatorScale, 'YSC1'));
		return (new TextSetting('YSC1', startScale.y, title));
	}

	if (index == count + 7)
	{
		const char *title = table->GetString(StringID('MUTR', kMutatorScale, 'XSC2'));
		return (new TextSetting('XSC2', finishScale.x, title));
	}

	if (index == count + 8)
	{
		const char *title = table->GetString(StringID('MUTR', kMutatorScale, 'YSC2'));
		return (new TextSetting('YSC2', finishScale.y, title));
	}

	if (index == count + 9)
	{
		const char *title = table->GetString(StringID('MUTR', kMutatorScale, 'TIME'));
		return (new TextSetting('TIME', scaleTime * 0.001F, title));
	}

	return (nullptr);
}

void ScaleMutator::SetSetting(const Setting *setting)
{
	Type identifier = setting->GetSettingIdentifier();

	if (identifier == 'STRT')
	{
		if (static_cast<const BooleanSetting *>(setting)->GetBooleanValue())
		{
			scaleMutatorFlags |= kScaleMutatorRestartable;
		}
		else
		{
			scaleMutatorFlags &= ~kScaleMutatorRestartable;
		}
	}
	else if (identifier == 'TEXC')
	{
		if (static_cast<const BooleanSetting *>(setting)->GetBooleanValue())
		{
			scaleMutatorFlags |= kScaleMutatorScaleTexcoords;
		}
		else
		{
			scaleMutatorFlags &= ~kScaleMutatorScaleTexcoords;
		}
	}
	else if (identifier == 'FLPH')
	{
		if (static_cast<const BooleanSetting *>(setting)->GetBooleanValue())
		{
			scaleMutatorFlags |= kScaleMutatorFlipHorizontal;
		}
		else
		{
			scaleMutatorFlags &= ~kScaleMutatorFlipHorizontal;
		}
	}
	else if (identifier == 'FLPV')
	{
		if (static_cast<const BooleanSetting *>(setting)->GetBooleanValue())
		{
			scaleMutatorFlags |= kScaleMutatorFlipVertical;
		}
		else
		{
			scaleMutatorFlags &= ~kScaleMutatorFlipVertical;
		}
	}
	else if (identifier == 'XSC1')
	{
		const char *text = static_cast<const TextSetting *>(setting)->GetText();
		startScale.x = Text::StringToFloat(text);
	}
	else if (identifier == 'YSC1')
	{
		const char *text = static_cast<const TextSetting *>(setting)->GetText();
		startScale.y = Text::StringToFloat(text);
	}
	else if (identifier == 'XSC2')
	{
		const char *text = static_cast<const TextSetting *>(setting)->GetText();
		finishScale.x = Text::StringToFloat(text);
	}
	else if (identifier == 'YSC2')
	{
		const char *text = static_cast<const TextSetting *>(setting)->GetText();
		finishScale.y = Text::StringToFloat(text);
	}
	else if (identifier == 'TIME')
	{
		const char *text = static_cast<const TextSetting *>(setting)->GetText();
		scaleTime = Text::StringToFloat(text) * 1000.0F;
	}
	else
	{
		Mutator::SetSetting(setting);
	}
}

void ScaleMutator::SetMutatorState(unsigned_int32 state)
{
	Mutator::SetMutatorState(state);

	if ((state & (kMutatorDisabled | kMutatorTerminated)) == 0)
	{
		const Widget *widget = GetTargetWidget();
		if (widget)
		{
			PanelController *controller = widget->GetPanelController();
			if (controller)
			{
				controller->ExtendAnimationTime(scaleTime);
			}
		}

		if (scaleMutatorFlags & kScaleMutatorRestartable)
		{
			currentTime = (state & kMutatorReverse) ? scaleTime : 0.0F;
		}
	}
}

void ScaleMutator::UpdateScale(float time) const
{
	float t = time / scaleTime;
	Vector2D scale = startScale * (1.0F - t) + finishScale * t;

	QuadWidget *quadWidget = static_cast<QuadWidget *>(GetTargetWidget());
	quadWidget->SetQuadScale(scale);

	float dx = 0.0F;
	float dy = 0.0F;

	unsigned_int32 flags = scaleMutatorFlags;
	const Vector2D& size = quadWidget->GetWidgetSize();

	if (flags & kScaleMutatorFlipHorizontal)
	{
		dx = size.x * (1.0F - scale.x);
	}

	if (flags & kScaleMutatorFlipVertical)
	{
		dy = size.y * (1.0F - scale.y);
	}

	quadWidget->SetQuadOffset(Vector2D(dx, dy));

	if (flags & kScaleMutatorScaleTexcoords)
	{
		ImageWidget *imageWidget = static_cast<ImageWidget *>(quadWidget);
		imageWidget->SetImageScale(scale);
	}
}

void ScaleMutator::Preprocess(void)
{
	Mutator::Preprocess();
	UpdateScale(currentTime);
}

void ScaleMutator::Move(void)
{
	float time = currentTime;
	float dt = TheTimeMgr->GetSystemFloatDeltaTime();

	unsigned_int32 state = GetMutatorState();
	if (!(state & kMutatorReverse))
	{
		time += dt;
		if (time >= scaleTime)
		{
			time = scaleTime;
			Mutator::SetMutatorState(state | kMutatorTerminated);
		}
	}
	else
	{
		time -= dt;
		if (time <= 0.0F)
		{
			time = 0.0F;
			Mutator::SetMutatorState(state | kMutatorTerminated);
		}
	}

	UpdateScale(currentTime = time);
}

void ScaleMutator::Reset(void)
{
	Mutator::Reset();
	UpdateScale(currentTime = GetInitialTime());
}


FadeMutator::FadeMutator() : Mutator(kMutatorFade)
{
	fadeMutatorFlags = 0;

	fadeColor.Set(0.0F, 0.0F, 0.0F, 1.0F);
	colorType = kWidgetColorDefault;

	fadeTime = 1000.0F;
	currentTime = 0.0F;
}

FadeMutator::FadeMutator(const ColorRGBA& color, float time) : Mutator(kMutatorFade)
{
	fadeMutatorFlags = 0;

	fadeColor = color;
	colorType = kWidgetColorDefault;

	fadeTime = time;
	currentTime = 0.0F;
}

inline float FadeMutator::GetInitialTime(void) const
{
	return ((GetMutatorState() & kMutatorReverse) ? fadeTime : 0.0F);
}

FadeMutator::FadeMutator(const FadeMutator& fadeMutator) : Mutator(fadeMutator)
{
	fadeMutatorFlags = fadeMutator.fadeMutatorFlags;

	fadeColor = fadeMutator.fadeColor;
	colorType = fadeMutator.colorType;

	fadeTime = fadeMutator.fadeTime;
	currentTime = GetInitialTime();
}

FadeMutator::~FadeMutator()
{
}

Mutator *FadeMutator::Replicate(void) const
{
	return (new FadeMutator(*this));
}

void FadeMutator::Pack(Packer& data, unsigned_int32 packFlags) const
{
	Mutator::Pack(data, packFlags);

	data << ChunkHeader('FLAG', 4);
	data << fadeMutatorFlags;

	data << ChunkHeader('COLR', sizeof(ColorRGBA));
	data << fadeColor;

	data << ChunkHeader('CTYP', 4);
	data << colorType;

	data << ChunkHeader('FTIM', 4);
	data << fadeTime;

	float time = (!GetTargetWidget()->GetManipulator()) ? currentTime : GetInitialTime();
	data << ChunkHeader('CTIM', 4);
	data << time;

	data << TerminatorChunk;
}

void FadeMutator::Unpack(Unpacker& data, unsigned_int32 unpackFlags)
{
	Mutator::Unpack(data, unpackFlags);
	UnpackChunkList<FadeMutator>(data, unpackFlags);
}

bool FadeMutator::UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags)
{
	switch (chunkHeader->chunkType)
	{
		case 'FLAG':

			data >> fadeMutatorFlags;
			return (true);

		case 'COLR':

			data >> fadeColor;
			return (true);

		case 'CTYP':

			data >> colorType;
			return (true);

		case 'FTIM':

			data >> fadeTime;
			return (true);

		case 'CTIM':

			data >> currentTime;
			return (true);
	}

	return (false);
}

int32 FadeMutator::GetSettingCount(void) const
{
	return (Mutator::GetSettingCount() + 6);
}

Setting *FadeMutator::GetSetting(int32 index) const
{
	int32 count = Mutator::GetSettingCount();
	if (index < count)
	{
		return (Mutator::GetSetting(index));
	}

	const StringTable *table = TheInterfaceMgr->GetStringTable();

	if (index == count)
	{
		const char *title = table->GetString(StringID('MUTR', kMutatorFade, 'SETT'));
		return (new HeadingSetting(kMutatorFade, title));
	}

	if (index == count + 1)
	{
		const char *title = table->GetString(StringID('MUTR', kMutatorFade, 'STRT'));
		return (new BooleanSetting('STRT', ((fadeMutatorFlags & kFadeMutatorRestartable) != 0), title));
	}

	if (index == count + 2)
	{
		const char *title = table->GetString(StringID('MUTR', kMutatorFade, 'PICK'));
		const char *picker = table->GetString(StringID('MUTR', kMutatorFade, 'COLR'));
		return (new ColorSetting('COLR', fadeColor, title, picker, kColorPickerAlpha));
	}

	if (index == count + 3)
	{
		int32 selection = 0;
		for (machine a = 1; a < Widget::kWidgetColorCount; a++)
		{
			if (colorType == Widget::widgetColorType[a])
			{
				selection = a;
				break;
			}
		}

		const char *title = table->GetString(StringID('WDGT', 'WDGT', 'CTYP'));
		MenuSetting *menu = new MenuSetting('CTYP', selection, title, Widget::kWidgetColorCount);
		menu->SetMenuItemString(0, table->GetString(StringID('WDGT', 'WDGT', 'CTYP', 'DFLT')));
		for (machine a = 1; a < Widget::kWidgetColorCount; a++)
		{
			menu->SetMenuItemString(a, table->GetString(StringID('WDGT', 'WDGT', 'CTYP', Widget::widgetColorType[a])));
		}

		return (menu);
	}

	if (index == count + 4)
	{
		const char *title = table->GetString(StringID('MUTR', kMutatorFade, 'TIME'));
		return (new TextSetting('TIME', fadeTime * 0.001F, title));
	}

	if (index == count + 5)
	{
		const char *title = table->GetString(StringID('MUTR', kMutatorFade, 'FHID'));
		return (new BooleanSetting('FHID', ((fadeMutatorFlags & kFadeMutatorFinishHide) != 0), title));
	}

	return (nullptr);
}

void FadeMutator::SetSetting(const Setting *setting)
{
	Type identifier = setting->GetSettingIdentifier();

	if (identifier == 'STRT')
	{
		if (static_cast<const BooleanSetting *>(setting)->GetBooleanValue())
		{
			fadeMutatorFlags |= kFadeMutatorRestartable;
		}
		else
		{
			fadeMutatorFlags &= ~kFadeMutatorRestartable;
		}
	}
	else if (identifier == 'COLR')
	{
		fadeColor = static_cast<const ColorSetting *>(setting)->GetColor();
	}
	else if (identifier == 'CTYP')
	{
		colorType = Widget::widgetColorType[static_cast<const MenuSetting *>(setting)->GetMenuSelection()];
	}
	else if (identifier == 'TIME')
	{
		const char *text = static_cast<const TextSetting *>(setting)->GetText();
		fadeTime = Text::StringToFloat(text) * 1000.0F;
	}
	else if (identifier == 'FHID')
	{
		if (static_cast<const BooleanSetting *>(setting)->GetBooleanValue())
		{
			fadeMutatorFlags |= kFadeMutatorFinishHide;
		}
		else
		{
			fadeMutatorFlags &= ~kFadeMutatorFinishHide;
		}
	}
	else
	{
		Mutator::SetSetting(setting);
	}
}

void FadeMutator::SetMutatorState(unsigned_int32 state)
{
	Mutator::SetMutatorState(state);

	if ((state & (kMutatorDisabled | kMutatorTerminated)) == 0)
	{
		Widget *widget = GetTargetWidget();
		if (widget)
		{
			PanelController *controller = widget->GetPanelController();
			if (controller)
			{
				controller->ExtendAnimationTime(fadeTime);
			}
		}

		if (fadeMutatorFlags & kFadeMutatorRestartable)
		{
			currentTime = (state & kMutatorReverse) ? fadeTime : 0.0F;

			if ((widget) && (!widget->GetManipulator()) && (fadeMutatorFlags & kFadeMutatorFinishHide))
			{
				widget->SetWidgetState(widget->GetWidgetState() & ~kWidgetDisabled);
			}
		}
	}
}

void FadeMutator::UpdateColor(float time) const
{
	Widget *widget = GetTargetWidget();

	float t = time / fadeTime;
	widget->SetDynamicWidgetColor(widget->GetWidgetColor(colorType) * (1.0F - t) + fadeColor * t, colorType);
}

void FadeMutator::Preprocess(void)
{
	Mutator::Preprocess();
	UpdateColor(currentTime);
}

void FadeMutator::Move(void)
{
	float time = currentTime;
	float dt = TheTimeMgr->GetSystemFloatDeltaTime();

	unsigned_int32 state = GetMutatorState();
	if (!(state & kMutatorReverse))
	{
		time += dt;
		if (time >= fadeTime)
		{
			time = fadeTime;
			Mutator::SetMutatorState(state | kMutatorTerminated);

			if (fadeMutatorFlags & kFadeMutatorFinishHide)
			{
				Widget *widget = GetTargetWidget();
				if (!widget->GetManipulator())
				{
					widget->SetWidgetState(widget->GetWidgetState() | kWidgetDisabled);
				}
			}
		}
	}
	else
	{
		time -= dt;
		if (time <= 0.0F)
		{
			time = 0.0F;
			Mutator::SetMutatorState(state | kMutatorTerminated);
		}
	}

	UpdateColor(currentTime = time);
}

void FadeMutator::Reset(void)
{
	Mutator::Reset();
	UpdateColor(currentTime = GetInitialTime());
}


TickerMutator::TickerMutator() : Mutator(kMutatorTicker)
{
	tickerMutatorFlags = 0;

	startPosition = 0.0F;
	finishPosition = 0.0F;

	scrollSpeed = 0.001F;
	currentPosition = 0.0F;
}

TickerMutator::TickerMutator(float start, float finish, float speed) : Mutator(kMutatorTicker)
{
	tickerMutatorFlags = 0;

	startPosition = start;
	finishPosition = finish;

	scrollSpeed = speed;
	currentPosition = 0.0F;
}

float TickerMutator::GetInitialPosition(void) const
{
	return ((GetMutatorState() & kMutatorReverse) ? finishPosition : startPosition);
}

TickerMutator::TickerMutator(const TickerMutator& tickerMutator) : Mutator(tickerMutator)
{
	tickerMutatorFlags = tickerMutator.tickerMutatorFlags;

	startPosition = tickerMutator.startPosition;
	finishPosition = tickerMutator.finishPosition;

	scrollSpeed = tickerMutator.scrollSpeed;
	currentPosition = GetInitialPosition();
}

TickerMutator::~TickerMutator()
{
}

Mutator *TickerMutator::Replicate(void) const
{
	return (new TickerMutator(*this));
}

bool TickerMutator::ValidWidget(const Widget *widget)
{
	return (widget->GetWidgetType() == kWidgetText);
}

void TickerMutator::Pack(Packer& data, unsigned_int32 packFlags) const
{
	Mutator::Pack(data, packFlags);

	data << ChunkHeader('FLAG', 4);
	data << tickerMutatorFlags;

	data << ChunkHeader('POSI', 8);
	data << startPosition;
	data << finishPosition;

	data << ChunkHeader('SPED', 4);
	data << scrollSpeed;

	float position = (!GetTargetWidget()->GetManipulator()) ? currentPosition : GetInitialPosition();
	data << ChunkHeader('CPOS', 4);
	data << position;

	data << TerminatorChunk;
}

void TickerMutator::Unpack(Unpacker& data, unsigned_int32 unpackFlags)
{
	Mutator::Unpack(data, unpackFlags);
	UnpackChunkList<TickerMutator>(data, unpackFlags);
}

bool TickerMutator::UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags)
{
	switch (chunkHeader->chunkType)
	{
		case 'FLAG':

			data >> tickerMutatorFlags;
			return (true);

		case 'POSI':

			data >> startPosition;
			data >> finishPosition;
			return (true);

		case 'SPED':

			data >> scrollSpeed;
			return (true);

		case 'CPOS':

			data >> currentPosition;
			return (true);
	}

	return (false);
}

int32 TickerMutator::GetSettingCount(void) const
{
	return (Mutator::GetSettingCount() + 6);
}

Setting *TickerMutator::GetSetting(int32 index) const
{
	int32 count = Mutator::GetSettingCount();
	if (index < count)
	{
		return (Mutator::GetSetting(index));
	}

	const StringTable *table = TheInterfaceMgr->GetStringTable();

	if (index == count)
	{
		const char *title = table->GetString(StringID('MUTR', kMutatorTicker, 'SETT'));
		return (new HeadingSetting(kMutatorTicker, title));
	}

	if (index == count + 1)
	{
		const char *title = table->GetString(StringID('MUTR', kMutatorTicker, 'STRT'));
		return (new BooleanSetting('STRT', ((tickerMutatorFlags & kTickerMutatorRestartable) != 0), title));
	}

	if (index == count + 2)
	{
		const char *title = table->GetString(StringID('MUTR', kMutatorTicker, 'LOOP'));
		return (new BooleanSetting('LOOP', ((tickerMutatorFlags & kTickerMutatorLooping) != 0), title));
	}

	if (index == count + 3)
	{
		const char *title = table->GetString(StringID('MUTR', kMutatorTicker, 'SPOS'));
		return (new TextSetting('SPOS', startPosition, title));
	}

	if (index == count + 4)
	{
		const char *title = table->GetString(StringID('MUTR', kMutatorTicker, 'FPOS'));
		return (new TextSetting('FPOS', finishPosition, title));
	}

	if (index == count + 5)
	{
		const char *title = table->GetString(StringID('MUTR', kMutatorTicker, 'SPED'));
		return (new TextSetting('SPED', scrollSpeed * 1000.0F, title));
	}

	return (nullptr);
}

void TickerMutator::SetSetting(const Setting *setting)
{
	Type identifier = setting->GetSettingIdentifier();

	if (identifier == 'STRT')
	{
		if (static_cast<const BooleanSetting *>(setting)->GetBooleanValue())
		{
			tickerMutatorFlags |= kTickerMutatorRestartable;
		}
		else
		{
			tickerMutatorFlags &= ~kTickerMutatorRestartable;
		}
	}
	else if (identifier == 'LOOP')
	{
		if (static_cast<const BooleanSetting *>(setting)->GetBooleanValue())
		{
			tickerMutatorFlags |= kTickerMutatorLooping;
		}
		else
		{
			tickerMutatorFlags &= ~kTickerMutatorLooping;
		}
	}
	else if (identifier == 'SPOS')
	{
		const char *text = static_cast<const TextSetting *>(setting)->GetText();
		startPosition = Text::StringToFloat(text);
	}
	else if (identifier == 'FPOS')
	{
		const char *text = static_cast<const TextSetting *>(setting)->GetText();
		finishPosition = Text::StringToFloat(text);
	}
	else if (identifier == 'SPED')
	{
		const char *text = static_cast<const TextSetting *>(setting)->GetText();
		scrollSpeed = Text::StringToFloat(text) * 0.001F;
	}
	else
	{
		Mutator::SetSetting(setting);
	}
}

void TickerMutator::SetMutatorState(unsigned_int32 state)
{
	Mutator::SetMutatorState(state);

	if ((state & (kMutatorDisabled | kMutatorTerminated)) == 0)
	{
		if (tickerMutatorFlags & kTickerMutatorRestartable)
		{
			currentPosition = (state & kMutatorReverse) ? finishPosition : startPosition;
		}
	}
}

void TickerMutator::UpdatePosition(float position) const
{
	TextWidget *widget = static_cast<TextWidget *>(GetTargetWidget());
	widget->SetTextRenderOffset(Vector3D(position, 0.0F, 0.0F));
}

void TickerMutator::Preprocess(void)
{
	Mutator::Preprocess();
	UpdatePosition(currentPosition);
}

void TickerMutator::Move(void)
{
	float dt = TheTimeMgr->GetSystemFloatDeltaTime();
	float speed = Fabs(scrollSpeed);
	float position = currentPosition;
	float start = startPosition;
	float finish = finishPosition;

	unsigned_int32 state = GetMutatorState();
	bool reverse = ((state & kMutatorReverse) != 0);

	if (finish < start)
	{
		reverse = !reverse;
		start = finish;
		finish = startPosition;
	}

	if (!reverse)
	{
		position += speed * dt;
		if (position >= finish)
		{
			if (tickerMutatorFlags & kTickerMutatorLooping)
			{
				position = start;
			}
			else
			{
				position = finish;
				Mutator::SetMutatorState(state | kMutatorTerminated);
			}
		}
	}
	else
	{
		position -= speed * dt;
		if (position <= start)
		{
			if (tickerMutatorFlags & kTickerMutatorLooping)
			{
				position = finish;
			}
			else
			{
				position = start;
				Mutator::SetMutatorState(state | kMutatorTerminated);
			}
		}
	}

	UpdatePosition(currentPosition = position);
}

void TickerMutator::Reset(void)
{
	Mutator::Reset();
	UpdatePosition(currentPosition = GetInitialPosition());
}


AnimateMutator::AnimateMutator() : Mutator(kMutatorAnimate)
{
	animateMutatorFlags = 0;

	frameCountX = 1;
	frameCountY = 1;
	totalFrameCount = 1;

	frameRate = 0.001F;
	currentFrame = 0.0F;
}

AnimateMutator::AnimateMutator(int32 x, int32 y, int32 total, float rate) : Mutator(kMutatorAnimate)
{
	animateMutatorFlags = 0;

	frameCountX = x;
	frameCountY = y;
	totalFrameCount = total;

	frameRate = rate;
	currentFrame = 0.0F;
}

inline float AnimateMutator::GetInitialFrame(void) const
{
	return ((GetMutatorState() & kMutatorReverse) ? (float) totalFrameCount : 0.0F);
}

AnimateMutator::AnimateMutator(const AnimateMutator& animateMutator) : Mutator(animateMutator)
{
	animateMutatorFlags = animateMutator.animateMutatorFlags;

	frameCountX = animateMutator.frameCountX;
	frameCountY = animateMutator.frameCountY;
	totalFrameCount = animateMutator.totalFrameCount;

	frameRate = animateMutator.frameRate;
	currentFrame = GetInitialFrame();
}

AnimateMutator::~AnimateMutator()
{
}

Mutator *AnimateMutator::Replicate(void) const
{
	return (new AnimateMutator(*this));
}

bool AnimateMutator::ValidWidget(const Widget *widget)
{
	return (widget->GetWidgetType() == kWidgetImage);
}

void AnimateMutator::Pack(Packer& data, unsigned_int32 packFlags) const
{
	Mutator::Pack(data, packFlags);

	data << ChunkHeader('FLAG', 4);
	data << animateMutatorFlags;

	data << ChunkHeader('DATA', 12);
	data << frameCountX;
	data << frameCountY;
	data << totalFrameCount;

	data << ChunkHeader('RATE', 4);
	data << frameRate;

	float frame = (!GetTargetWidget()->GetManipulator()) ? currentFrame : GetInitialFrame();
	data << ChunkHeader('FRAM', 4);
	data << frame;

	data << TerminatorChunk;
}

void AnimateMutator::Unpack(Unpacker& data, unsigned_int32 unpackFlags)
{
	Mutator::Unpack(data, unpackFlags);
	UnpackChunkList<AnimateMutator>(data, unpackFlags);
}

bool AnimateMutator::UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags)
{
	switch (chunkHeader->chunkType)
	{
		case 'FLAG':

			data >> animateMutatorFlags;
			return (true);

		case 'DATA':

			data >> frameCountX;
			data >> frameCountY;
			data >> totalFrameCount;
			return (true);

		case 'RATE':

			data >> frameRate;
			return (true);

		case 'FRAM':

			data >> currentFrame;
			return (true);
	}

	return (false);
}

int32 AnimateMutator::GetSettingCount(void) const
{
	return (Mutator::GetSettingCount() + 7);
}

Setting *AnimateMutator::GetSetting(int32 index) const
{
	int32 count = Mutator::GetSettingCount();
	if (index < count)
	{
		return (Mutator::GetSetting(index));
	}

	const StringTable *table = TheInterfaceMgr->GetStringTable();

	if (index == count)
	{
		const char *title = table->GetString(StringID('MUTR', kMutatorAnimate, 'SETT'));
		return (new HeadingSetting(kMutatorAnimate, title));
	}

	if (index == count + 1)
	{
		const char *title = table->GetString(StringID('MUTR', kMutatorAnimate, 'STRT'));
		return (new BooleanSetting('STRT', ((animateMutatorFlags & kAnimateMutatorRestartable) != 0), title));
	}

	if (index == count + 2)
	{
		const char *title = table->GetString(StringID('MUTR', kMutatorAnimate, 'LOOP'));
		return (new BooleanSetting('LOOP', ((animateMutatorFlags & kAnimateMutatorLooping) != 0), title));
	}

	if (index == count + 3)
	{
		const char *title = table->GetString(StringID('MUTR', kMutatorAnimate, 'XCNT'));
		return (new TextSetting('XCNT', Text::IntegerToString(frameCountX), title, 3, &EditTextWidget::NumberFilter));
	}

	if (index == count + 4)
	{
		const char *title = table->GetString(StringID('MUTR', kMutatorAnimate, 'YCNT'));
		return (new TextSetting('YCNT', Text::IntegerToString(frameCountY), title, 3, &EditTextWidget::NumberFilter));
	}

	if (index == count + 5)
	{
		const char *title = table->GetString(StringID('MUTR', kMutatorAnimate, 'TOTL'));
		return (new TextSetting('TOTL', Text::IntegerToString(totalFrameCount), title, 4, &EditTextWidget::NumberFilter));
	}

	if (index == count + 6)
	{
		const char *title = table->GetString(StringID('MUTR', kMutatorAnimate, 'RATE'));
		return (new TextSetting('RATE', frameRate * 1000.0F, title));
	}

	return (nullptr);
}

void AnimateMutator::SetSetting(const Setting *setting)
{
	Type identifier = setting->GetSettingIdentifier();

	if (identifier == 'STRT')
	{
		if (static_cast<const BooleanSetting *>(setting)->GetBooleanValue())
		{
			animateMutatorFlags |= kAnimateMutatorRestartable;
		}
		else
		{
			animateMutatorFlags &= ~kAnimateMutatorRestartable;
		}
	}
	else if (identifier == 'LOOP')
	{
		if (static_cast<const BooleanSetting *>(setting)->GetBooleanValue())
		{
			animateMutatorFlags |= kAnimateMutatorLooping;
		}
		else
		{
			animateMutatorFlags &= ~kAnimateMutatorLooping;
		}
	}
	else if (identifier == 'XCNT')
	{
		const char *text = static_cast<const TextSetting *>(setting)->GetText();
		frameCountX = Max(Text::StringToInteger(text), 1);
	}
	else if (identifier == 'YCNT')
	{
		const char *text = static_cast<const TextSetting *>(setting)->GetText();
		frameCountY = Max(Text::StringToInteger(text), 1);
	}
	else if (identifier == 'TOTL')
	{
		const char *text = static_cast<const TextSetting *>(setting)->GetText();
		totalFrameCount = Max(Text::StringToInteger(text), 1);
	}
	else if (identifier == 'RATE')
	{
		const char *text = static_cast<const TextSetting *>(setting)->GetText();
		frameRate = Text::StringToFloat(text) * 0.001F;
	}
	else
	{
		Mutator::SetSetting(setting);
	}
}

void AnimateMutator::SetMutatorState(unsigned_int32 state)
{
	Mutator::SetMutatorState(state);

	if ((state & (kMutatorDisabled | kMutatorTerminated)) == 0)
	{
		if (animateMutatorFlags & kAnimateMutatorRestartable)
		{
			currentFrame = (state & kMutatorReverse) ? (float) totalFrameCount : 0.0F;
		}
	}
}

void AnimateMutator::UpdateFrame(float frame) const
{
	int32 position = Min(MaxZero((int32) frame), totalFrameCount - 1);
	int32 j = position / frameCountX;
	int32 i = position - j * frameCountX;

	float w = 1.0F / (float) frameCountX;
	float h = 1.0F / (float) frameCountY;

	float s = (float) i * w;
	float t = (float) (frameCountY - 1 - j) * h;

	ImageWidget *widget = static_cast<ImageWidget *>(GetTargetWidget());
	widget->SetImageScale(Vector2D(w, h));
	widget->SetImageOffset(Vector2D(s, t));
}

void AnimateMutator::Preprocess(void)
{
	Mutator::Preprocess();
	UpdateFrame(currentFrame);
}

void AnimateMutator::Move(void)
{
	float dt = TheTimeMgr->GetSystemFloatDeltaTime();
	float rate = Fabs(frameRate);
	float frame = currentFrame;
	float count = (float) totalFrameCount;

	unsigned_int32 state = GetMutatorState();
	bool reverse = ((state & kMutatorReverse) != 0);

	if (!reverse)
	{
		frame += rate * dt;
		if (frame >= count)
		{
			if (animateMutatorFlags & kAnimateMutatorLooping)
			{
				frame -= count;
			}
			else
			{
				frame = count;
				Mutator::SetMutatorState(state | kMutatorTerminated);
			}
		}
	}
	else
	{
		frame -= rate * dt;
		if (frame < 0.0F)
		{
			if (animateMutatorFlags & kAnimateMutatorLooping)
			{
				frame += count;
			}
			else
			{
				frame = 0.0F;
				Mutator::SetMutatorState(state | kMutatorTerminated);
			}
		}
	}

	UpdateFrame(currentFrame = frame);
}

void AnimateMutator::Reset(void)
{
	Mutator::Reset();
	UpdateFrame(currentFrame = GetInitialFrame());
}

// ZYUQURM
