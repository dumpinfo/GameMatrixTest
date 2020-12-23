 

#include "C4Sources.h"
#include "C4World.h"
#include "C4Configuration.h"


using namespace C4;


namespace C4
{
	template <> Heap EngineMemory<SourceRegion>::heap("SourceRegion", 16384, kHeapMutexless);
	template class EngineMemory<SourceRegion>;
}


SoundData::SoundData(const char *name)
{
	soundName = name;
}

SoundData::~SoundData()
{
}


Sound *SoundConduit::LoadSound(Source *source)
{
	Sound *sound = new Sound;

	const SourceObject *object = source->GetObject();
	unsigned_int32 flags = object->GetSourceFlags();

	const SoundData *soundData = object->GetFirstSound();
	if (soundData)
	{
		if (!(flags & kSourceStream))
		{
			if (sound->Load(soundData->GetSoundName()) != kSoundOkay)
			{
				sound->Load("C4/missing");
			}

			sound->SetSoundFlags(kSoundPersistent);
		}
		else
		{
			WaveStreamer *streamer = new WaveStreamer;
			if (sound->Stream(streamer) == kSoundOkay)
			{
				do
				{
					if (streamer->AddComponent(soundData->GetSoundName()) != kSoundOkay)
					{
						break;
					}

					soundData = soundData->Next();
				} while (soundData);
			}
		}
	}

	if (flags & kSourceLoop)
	{
		sound->SetLoopCount(kSoundLoopInfinite);
	}

	return (sound);
}

void SoundConduit::UnloadSound(Sound *sound)
{
	sound->Release();
}

void SoundConduit::PlaySound(Sound *sound)
{
	sound->Play();
}

void SoundConduit::StopSound(Sound *sound)
{
	sound->Stop();
}

void SoundConduit::SetSoundVolume(Sound *sound, float volume)
{
	sound->SetSoundProperty(kSoundVolume, volume);
}

void SoundConduit::SetSoundFrequency(Sound *sound, float frequency)
{
	sound->SetSoundProperty(kSoundFrequency, frequency);
}


SourceRegion::SourceRegion(OmniSource *source, Zone *zone)
{
	regionSource = source;
	primaryRegion = this;
 
	regionZone = zone;

	permeatedPortal = nullptr; 
	permeatedPosition = source->GetWorldPosition();
	permeatedPathLength = 0.0F; 

	audiblePosition = source->GetWorldPosition();
	audiblePathLength = 0.0F; 
}
 
SourceRegion::~SourceRegion() 
{
}

 
SourceObject::SourceObject(SourceType type) : Object(kObjectSource)
{
	sourceType = type;
	sourceFlags = kSourceInitialPlay;

	initialSourceVolume = 1.0F;
	initialSourceFrequency = 1.0F;

	soundGroupType = 0;
}

SourceObject::SourceObject(SourceType type, const char *name) : Object(kObjectSource)
{
	sourceType = type;
	sourceFlags = kSourceInitialPlay;

	initialSourceVolume = 1.0F;
	initialSourceFrequency = 1.0F;

	soundGroupType = 0;

	if (name)
	{
		soundList.Append(new SoundData(name));
	}
}

SourceObject::~SourceObject()
{
}

SourceObject *SourceObject::Create(Unpacker& data, unsigned_int32 unpackFlags)
{
	switch (data.GetType())
	{
		case kSourceAmbient:

			return (new AmbientSourceObject);

		case kSourceOmni:

			return (new OmniSourceObject);

		case kSourceDirected:

			return (new DirectedSourceObject);
	}

	return (nullptr);
}

void SourceObject::PackType(Packer& data) const
{
	Object::PackType(data);
	data << sourceType;
}

void SourceObject::Pack(Packer& data, unsigned_int32 packFlags) const
{
	data << ChunkHeader('FLAG', 4);
	data << sourceFlags;

	data << ChunkHeader('VOLU', 4);
	data << initialSourceVolume;

	data << ChunkHeader('FREQ', 4);
	data << initialSourceFrequency;

	data << ChunkHeader('GRUP', 4);
	data << soundGroupType;

	const SoundData *sound = soundList.First();
	while (sound)
	{
		PackHandle handle = data.BeginChunk('SOND');
		data << sound->GetSoundName();
		data.EndChunk(handle);

		sound = sound->Next();
	}

	data << TerminatorChunk;
}

void SourceObject::Unpack(Unpacker& data, unsigned_int32 unpackFlags)
{
	UnpackChunkList<SourceObject>(data, unpackFlags);
}

bool SourceObject::UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags)
{
	switch (chunkHeader->chunkType)
	{
		case 'FLAG':

			data >> sourceFlags;
			return (true);

		case 'VOLU':

			data >> initialSourceVolume;
			return (true);

		case 'FREQ':

			data >> initialSourceFrequency;
			return (true);

		case 'GRUP':

			data >> soundGroupType;
			return (true);

		case 'SOND':
		{
			ResourceName	name;

			data >> name;
			soundList.Append(new SoundData(name));
			return (true);
		}
	}

	return (false);
}

void *SourceObject::BeginSettingsUnpack(void)
{
	soundList.Purge();
	return (nullptr);
}

int32 SourceObject::GetCategoryCount(void) const
{
	return (1);
}

Type SourceObject::GetCategoryType(int32 index, const char **title) const
{
	if (index == 0)
	{
		*title = TheInterfaceMgr->GetStringTable()->GetString(StringID(kObjectSource));
		return (kObjectSource);
	}

	return (0);
}

int32 SourceObject::GetCategorySettingCount(Type category) const
{
	if (category == kObjectSource)
	{
		return (8);
	}

	return (0);
}

Setting *SourceObject::GetCategorySetting(Type category, int32 index, unsigned_int32 flags) const
{
	if (category == kObjectSource)
	{
		if (flags & kConfigurationScript)
		{
			return (nullptr);
		}

		const StringTable *table = TheInterfaceMgr->GetStringTable();

		if (index == 0)
		{
			const char *title = table->GetString(StringID(kObjectSource, 'SORC'));
			return (new HeadingSetting(kObjectSource, title));
		}

		if (index == 1)
		{
			const char *picker = table->GetString(StringID(kObjectSource, 'SORC', 'PICK'));

			if (GetSourceType() != kSourceAmbient)
			{
				const char *name = nullptr;
				const SoundData *data = GetFirstSound();
				if (data)
				{
					name = data->GetSoundName();
				}

				const char *title = table->GetString(StringID(kObjectSource, 'SORC', 'SNAM'));
				return (new ResourceSetting('SNAM', name, title, picker, SoundResource::GetDescriptor()));
			}

			const char *title = table->GetString(StringID(kObjectSource, 'SORC', 'LIST'));
			MultiResourceSetting *setting = new MultiResourceSetting('LIST', title, picker, SoundResource::GetDescriptor());

			const SoundData *data = GetFirstSound();
			while (data)
			{
				setting->AddResourceName(data->GetSoundName());
				data = data->Next();
			}

			return (setting);
		}

		if (index == 2)
		{
			int32 selection = 0;
			int32 count = 1;

			SoundGroupType type = soundGroupType;
			const SoundGroup *group = TheSoundMgr->GetFirstSoundGroup();
			while (group)
			{
				if (group->GetSoundGroupType() == type)
				{
					selection = count;
				}

				count++;
				group = group->Next();
			}

			const char *title = table->GetString(StringID(kObjectSource, 'SORC', 'GRUP'));
			MenuSetting *menu = new MenuSetting('GRUP', selection, title, count);

			menu->SetMenuItemString(0, table->GetString(StringID(kObjectSource, 'SORC', 'GRUP', 'DFLT')));

			count = 1;
			group = TheSoundMgr->GetFirstSoundGroup();
			while (group)
			{
				menu->SetMenuItemString(count, group->GetSoundGroupName());

				count++;
				group = group->Next();
			}

			return (menu);
		}

		if (index == 3)
		{
			const char *title = table->GetString(StringID(kObjectSource, 'SORC', 'VOLU'));
			return (new IntegerSetting('VOLU', (int32) (initialSourceVolume * 100.0 + 0.5F), title, 1, 100, 1));
		}

		if (index == 4)
		{
			const char *title = table->GetString(StringID(kObjectSource, 'SORC', 'FREQ'));
			return (new FloatSetting('FREQ', initialSourceFrequency, title, 0.25F, 4.0F, 0.05F));
		}

		if (index == 5)
		{
			const char *title = table->GetString(StringID(kObjectSource, 'SORC', 'STRM'));
			return (new BooleanSetting('STRM', ((sourceFlags & kSourceStream) != 0), title));
		}

		if (index == 6)
		{
			const char *title = table->GetString(StringID(kObjectSource, 'SORC', 'PLAY'));
			return (new BooleanSetting('PLAY', ((sourceFlags & kSourceInitialPlay) != 0), title));
		}

		if (index == 7)
		{
			const char *title = table->GetString(StringID(kObjectSource, 'SORC', 'LOOP'));
			return (new BooleanSetting('LOOP', ((sourceFlags & kSourceLoop) != 0), title));
		}
	}

	return (nullptr);
}

void SourceObject::SetCategorySetting(Type category, const Setting *setting)
{
	if (category == kObjectSource)
	{
		Type identifier = setting->GetSettingIdentifier();

		if (identifier == 'SNAM')
		{
			soundList.Purge();

			const char *name = static_cast<const ResourceSetting *>(setting)->GetResourceName();
			if (name[0] != 0)
			{
				soundList.Append(new SoundData(name));
			}
		}
		else if (identifier == 'LIST')
		{
			soundList.Purge();

			const MultiResourceSetting *multiResourceSetting = static_cast<const MultiResourceSetting *>(setting);
			int32 count = multiResourceSetting->GetResourceCount();
			for (machine a = 0; a < count; a++)
			{
				ResourceName name(multiResourceSetting->GetResourceName(a));
				if (name[0] != 0)
				{
					soundList.Append(new SoundData(name));
				}
			}
		}
		else if (identifier == 'GRUP')
		{
			int32 selection = static_cast<const MenuSetting *>(setting)->GetMenuSelection();
			if (selection == 0)
			{
				soundGroupType = 0;
			}
			else
			{
				int32 count = 1;
				const SoundGroup *group = TheSoundMgr->GetFirstSoundGroup();
				while (group)
				{
					if (count == selection)
					{
						soundGroupType = group->GetSoundGroupType();
						break;
					}

					count++;
					group = group->Next();
				}
			}
		}
		else if (identifier == 'VOLU')
		{
			int32 value = static_cast<const IntegerSetting *>(setting)->GetIntegerValue();
			initialSourceVolume = (float) value * 0.01F;
		}
		else if (identifier == 'FREQ')
		{
			initialSourceFrequency = static_cast<const FloatSetting *>(setting)->GetFloatValue();
		}
		else if (identifier == 'STRM')
		{
			if (static_cast<const BooleanSetting *>(setting)->GetBooleanValue())
			{
				sourceFlags |= kSourceStream;
			}
			else
			{
				sourceFlags &= ~kSourceStream;
			}
		}
		else if (identifier == 'PLAY')
		{
			if (static_cast<const BooleanSetting *>(setting)->GetBooleanValue())
			{
				sourceFlags |= kSourceInitialPlay;
			}
			else
			{
				sourceFlags &= ~kSourceInitialPlay;
			}
		}
		else if (identifier == 'LOOP')
		{
			if (static_cast<const BooleanSetting *>(setting)->GetBooleanValue())
			{
				sourceFlags |= kSourceLoop;
			}
			else
			{
				sourceFlags &= ~kSourceLoop;
			}
		}
	}
}


AmbientSourceObject::AmbientSourceObject() : SourceObject(kSourceAmbient)
{
	fadeTime = 0;
	loopIndex = 0;
}

AmbientSourceObject::AmbientSourceObject(const char *name) : SourceObject(kSourceAmbient, name)
{
	fadeTime = 0;
	loopIndex = 0;
}

AmbientSourceObject::~AmbientSourceObject()
{
}

void AmbientSourceObject::Pack(Packer& data, unsigned_int32 packFlags) const
{
	SourceObject::Pack(data, packFlags);

	data << ChunkHeader('FADE', 4);
	data << fadeTime;

	data << ChunkHeader('LIDX', 4);
	data << loopIndex;

	data << TerminatorChunk;
}

void AmbientSourceObject::Unpack(Unpacker& data, unsigned_int32 unpackFlags)
{
	SourceObject::Unpack(data, unpackFlags);
	UnpackChunkList<AmbientSourceObject>(data, unpackFlags);
}

bool AmbientSourceObject::UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags)
{
	switch (chunkHeader->chunkType)
	{
		case 'FADE':

			data >> fadeTime;
			return (true);

		case 'LIDX':

			data >> loopIndex;
			return (true);
	}

	return (false);
}

int32 AmbientSourceObject::GetCategorySettingCount(Type category) const
{
	int32 count = SourceObject::GetCategorySettingCount(category);
	if (category == kObjectSource)
	{
		count += 2;
	}

	return (count);
}

Setting *AmbientSourceObject::GetCategorySetting(Type category, int32 index, unsigned_int32 flags) const
{
	if (category == kObjectSource)
	{
		int32 i = index - SourceObject::GetCategorySettingCount(kObjectSource);
		if (i >= 0)
		{
			if (flags & kConfigurationScript)
			{
				return (nullptr);
			}

			const StringTable *table = TheInterfaceMgr->GetStringTable();

			if (i == 0)
			{
				const char *title = table->GetString(StringID(kObjectSource, 'SORC', 'FADE'));
				return (new TextSetting('FADE', (float) fadeTime * 0.001F, title));
			}

			if (i == 1)
			{
				const char *title = table->GetString(StringID(kObjectSource, 'SORC', 'LIDX'));
				return (new TextSetting('LIDX', Text::IntegerToString(loopIndex), title, 2, &EditTextWidget::NumberFilter));
			}

			return (nullptr);
		}
	}

	return (SourceObject::GetCategorySetting(category, index, flags));
}

void AmbientSourceObject::SetCategorySetting(Type category, const Setting *setting)
{
	if (category == kObjectSource)
	{
		Type identifier = setting->GetSettingIdentifier();

		if (identifier == 'FADE')
		{
			const char *text = static_cast<const TextSetting *>(setting)->GetText();
			fadeTime = MaxZero((int32) (Text::StringToFloat(text) * 1000.0F));
		}
		else if (identifier == 'LIDX')
		{
			const char *text = static_cast<const TextSetting *>(setting)->GetText();
			loopIndex = Text::StringToInteger(text);
		}
		else
		{
			SourceObject::SetCategorySetting(kObjectSource, setting);
		}
	}
}


OmniSourceObject::OmniSourceObject() : SourceObject(kSourceOmni)
{
	reflectionVolume = 1.0F;
	reflectionHFVolume = 1.0F;

	SetSourceFlags(GetSourceFlags() | (kSourceDopplerShift | kSourceDistanceDelay | kSourceReverb));
}

OmniSourceObject::OmniSourceObject(SourceType type) : SourceObject(type)
{
	reflectionVolume = 1.0F;
	reflectionHFVolume = 1.0F;

	SetSourceFlags(GetSourceFlags() | (kSourceDopplerShift | kSourceDistanceDelay | kSourceReverb));
}

OmniSourceObject::OmniSourceObject(SourceType type, const char *name, float range) : SourceObject(type, name)
{
	sourceRange = range;
	reflectionVolume = 1.0F;
	reflectionHFVolume = 1.0F;

	SetSourceFlags(GetSourceFlags() | (kSourceDopplerShift | kSourceDistanceDelay | kSourceReverb));
}

OmniSourceObject::OmniSourceObject(const char *name, float range) : SourceObject(kSourceOmni, name)
{
	sourceRange = range;
	reflectionVolume = 1.0F;
	reflectionHFVolume = 1.0F;

	SetSourceFlags(GetSourceFlags() | (kSourceDopplerShift | kSourceDistanceDelay | kSourceReverb));
}

OmniSourceObject::~OmniSourceObject()
{
}

void OmniSourceObject::Pack(Packer& data, unsigned_int32 packFlags) const
{
	SourceObject::Pack(data, packFlags);

	data << ChunkHeader('RANG', 4);
	data << sourceRange;

	data << ChunkHeader('REFL', 8);
	data << reflectionVolume;
	data << reflectionHFVolume;

	data << TerminatorChunk;
}

void OmniSourceObject::Unpack(Unpacker& data, unsigned_int32 unpackFlags)
{
	SourceObject::Unpack(data, unpackFlags);
	UnpackChunkList<OmniSourceObject>(data, unpackFlags);
}

bool OmniSourceObject::UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags)
{
	switch (chunkHeader->chunkType)
	{
		case 'RANG':

			data >> sourceRange;
			return (true);

		case 'REFL':

			data >> reflectionVolume;
			data >> reflectionHFVolume;
			return (true);
	}

	return (false);
}

int32 OmniSourceObject::GetCategorySettingCount(Type category) const
{
	int32 count = SourceObject::GetCategorySettingCount(category);
	if (category == kObjectSource)
	{
		count += 8;
	}

	return (count);
}

Setting *OmniSourceObject::GetCategorySetting(Type category, int32 index, unsigned_int32 flags) const
{
	if (category == kObjectSource)
	{
		int32 i = index - SourceObject::GetCategorySettingCount(kObjectSource);
		if (i >= 0)
		{
			const StringTable *table = TheInterfaceMgr->GetStringTable();

			if (i == 0)
			{
				const char *title = table->GetString(StringID(kObjectSource, 'SORC', 'RAND'));
				return (new BooleanSetting('RAND', ((GetSourceFlags() & kSourceRandom) != 0), title));
			}

			if (i == 1)
			{
				const char *title = table->GetString(StringID(kObjectSource, 'SPAT'));
				return (new HeadingSetting('SPAT', title));
			}

			if (i == 2)
			{
				const char *title = table->GetString(StringID(kObjectSource, 'SPAT', 'DPLR'));
				return (new BooleanSetting('DPLR', ((GetSourceFlags() & kSourceDopplerShift) != 0), title));
			}

			if (i == 3)
			{
				const char *title = table->GetString(StringID(kObjectSource, 'SPAT', 'DIST'));
				return (new BooleanSetting('DIST', ((GetSourceFlags() & kSourceDistanceDelay) != 0), title));
			}

			if (i == 4)
			{
				const char *title = table->GetString(StringID(kObjectSource, 'SPAT', 'RVRB'));
				return (new BooleanSetting('RVRB', ((GetSourceFlags() & kSourceReverb) != 0), title));
			}

			if (i == 5)
			{
				const char *title = table->GetString(StringID(kObjectSource, 'SPAT', 'OBST'));
				return (new BooleanSetting('OBST', ((GetSourceFlags() & kSourceObstruction) != 0), title));
			}

			if (i == 6)
			{
				const char *title = table->GetString(StringID(kObjectSource, 'SPAT', 'REFL'));
				return (new IntegerSetting('REFL', (int32) (reflectionVolume * 100.0 + 0.5F), title, 0, 100, 1));
			}

			if (i == 7)
			{
				const char *title = table->GetString(StringID(kObjectSource, 'SPAT', 'REFH'));
				return (new IntegerSetting('REFH', (int32) (reflectionHFVolume * 100.0 + 0.5F), title, 0, 100, 1));
			}

			return (nullptr);
		}
	}

	return (SourceObject::GetCategorySetting(category, index, flags));
}

void OmniSourceObject::SetCategorySetting(Type category, const Setting *setting)
{
	if (category == kObjectSource)
	{
		Type identifier = setting->GetSettingIdentifier();

		if (identifier == 'RAND')
		{
			if (static_cast<const BooleanSetting *>(setting)->GetBooleanValue())
			{
				SetSourceFlags(GetSourceFlags() | kSourceRandom);
			}
			else
			{
				SetSourceFlags(GetSourceFlags() & ~kSourceRandom);
			}
		}
		else if (identifier == 'DPLR')
		{
			if (static_cast<const BooleanSetting *>(setting)->GetBooleanValue())
			{
				SetSourceFlags(GetSourceFlags() | kSourceDopplerShift);
			}
			else
			{
				SetSourceFlags(GetSourceFlags() & ~kSourceDopplerShift);
			}
		}
		else if (identifier == 'DIST')
		{
			if (static_cast<const BooleanSetting *>(setting)->GetBooleanValue())
			{
				SetSourceFlags(GetSourceFlags() | kSourceDistanceDelay);
			}
			else
			{
				SetSourceFlags(GetSourceFlags() & ~kSourceDistanceDelay);
			}
		}
		else if (identifier == 'RVRB')
		{
			if (static_cast<const BooleanSetting *>(setting)->GetBooleanValue())
			{
				SetSourceFlags(GetSourceFlags() | kSourceReverb);
			}
			else
			{
				SetSourceFlags(GetSourceFlags() & ~kSourceReverb);
			}
		}
		else if (identifier == 'OBST')
		{
			if (static_cast<const BooleanSetting *>(setting)->GetBooleanValue())
			{
				SetSourceFlags(GetSourceFlags() | kSourceObstruction);
			}
			else
			{
				SetSourceFlags(GetSourceFlags() & ~kSourceObstruction);
			}
		}
		else if (identifier == 'REFL')
		{
			reflectionVolume = (float) static_cast<const IntegerSetting *>(setting)->GetIntegerValue() * 0.01F;
		}
		else if (identifier == 'REFH')
		{
			reflectionHFVolume = (float) static_cast<const IntegerSetting *>(setting)->GetIntegerValue() * 0.01F;
		}
		else
		{
			SourceObject::SetCategorySetting(kObjectSource, setting);
		}
	}
}

int32 OmniSourceObject::GetObjectSize(float *size) const
{
	size[0] = sourceRange;
	return (1);
}

void OmniSourceObject::SetObjectSize(const float *size)
{
	sourceRange = size[0];
}


DirectedSourceObject::DirectedSourceObject() : OmniSourceObject(kSourceDirected)
{
	outerConeVolume = 0.0F;
	outerConeHFVolume = 1.0F;
}

DirectedSourceObject::DirectedSourceObject(const char *name, float range, float apex) : OmniSourceObject(kSourceDirected, name, range)
{
	apexTangent = apex;

	outerConeVolume = 0.0F;
	outerConeHFVolume = 1.0F;
}

DirectedSourceObject::~DirectedSourceObject()
{
}

void DirectedSourceObject::Pack(Packer& data, unsigned_int32 packFlags) const
{
	OmniSourceObject::Pack(data, packFlags);

	data << ChunkHeader('APEX', 4);
	data << apexTangent;

	data << ChunkHeader('CONE', 8);
	data << outerConeVolume;
	data << outerConeHFVolume;

	data << TerminatorChunk;
}

void DirectedSourceObject::Unpack(Unpacker& data, unsigned_int32 unpackFlags)
{
	OmniSourceObject::Unpack(data, unpackFlags);
	UnpackChunkList<DirectedSourceObject>(data, unpackFlags);
}

bool DirectedSourceObject::UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags)
{
	switch (chunkHeader->chunkType)
	{
		case 'APEX':

			data >> apexTangent;
			return (true);

		case 'CONE':

			data >> outerConeVolume;
			data >> outerConeHFVolume;
			return (true);
	}

	return (false);
}

int32 DirectedSourceObject::GetCategorySettingCount(Type category) const
{
	int32 count = OmniSourceObject::GetCategorySettingCount(category);
	if (category == kObjectSource)
	{
		count += 2;
	}

	return (count);
}

Setting *DirectedSourceObject::GetCategorySetting(Type category, int32 index, unsigned_int32 flags) const
{
	if (category == kObjectSource)
	{
		int32 i = index - OmniSourceObject::GetCategorySettingCount(kObjectSource);
		if (i >= 0)
		{
			const StringTable *table = TheInterfaceMgr->GetStringTable();

			if (i == 0)
			{
				const char *title = table->GetString(StringID(kObjectSource, 'SPAT', 'CONV'));
				return (new IntegerSetting('CONV', (int32) (outerConeVolume * 100.0 + 0.5F), title, 0, 100, 1));
			}

			if (i == 1)
			{
				const char *title = table->GetString(StringID(kObjectSource, 'SPAT', 'CONH'));
				return (new IntegerSetting('CONH', (int32) (outerConeHFVolume * 100.0 + 0.5F), title, 0, 100, 1));
			}

			return (nullptr);
		}
	}

	return (OmniSourceObject::GetCategorySetting(category, index, flags));
}

void DirectedSourceObject::SetCategorySetting(Type category, const Setting *setting)
{
	if (category == kObjectSource)
	{
		Type identifier = setting->GetSettingIdentifier();

		if (identifier == 'CONV')
		{
			outerConeVolume = (float) static_cast<const IntegerSetting *>(setting)->GetIntegerValue() * 0.01F;
		}
		else if (identifier == 'CONH')
		{
			outerConeHFVolume = (float) static_cast<const IntegerSetting *>(setting)->GetIntegerValue() * 0.01F;
		}
		else
		{
			OmniSourceObject::SetCategorySetting(kObjectSource, setting);
		}
	}
}

int32 DirectedSourceObject::GetObjectSize(float *size) const
{
	size[0] = GetSourceRange();
	size[1] = apexTangent;
	return (2);
}

void DirectedSourceObject::SetObjectSize(const float *size)
{
	SetSourceRange(size[0]);
	apexTangent = size[1];
}


Source::Source(SourceType type) :
		Node(kNodeSource),
		playTask(&PlayTask, this),
		volumeTask(&VolumeTask, this),
		frequencyTask(&FrequencyTask, this)
{
	sourceType = type;

	sourcePriority = 0;
	sourceVolume = 1.0F;
	sourceFrequency = 1.0F;

	soundConduit = &sourceSoundConduit;
	sourceState = 0;
	soundObject = nullptr;
}

Source::Source(SourceType type, bool persistent) :
		Node(kNodeSource),
		playTask(&PlayTask, this),
		volumeTask(&VolumeTask, this),
		frequencyTask(&FrequencyTask, this)
{
	sourceType = type;

	sourcePriority = 0;
	sourceVolume = 1.0F;
	sourceFrequency = 1.0F;

	soundConduit = &sourceSoundConduit;
	sourceState = (persistent) ? kSourcePersistent : 0;
	soundObject = nullptr;
}

Source::Source(const Source& source) :
		Node(source),
		playTask(&PlayTask, this),
		volumeTask(&VolumeTask, this),
		frequencyTask(&FrequencyTask, this)
{
	sourceType = source.sourceType;

	sourcePriority = source.sourcePriority;
	sourceVolume = source.sourceVolume;
	sourceFrequency = source.sourceFrequency;

	soundConduit = &sourceSoundConduit;
	sourceState = source.sourceState & kSourcePersistent;
	soundObject = nullptr;
}

Source::~Source()
{
	if (soundObject)
	{
		soundConduit->UnloadSound(soundObject);
	}
}

Source *Source::Create(Unpacker& data, unsigned_int32 unpackFlags)
{
	switch (data.GetType())
	{
		case kSourceAmbient:

			return (new AmbientSource);

		case kSourceOmni:

			return (new OmniSource);

		case kSourceDirected:

			return (new DirectedSource);
	}

	return (nullptr);
}

void Source::PackType(Packer& data) const
{
	Node::PackType(data);
	data << sourceType;
}

void Source::Pack(Packer& data, unsigned_int32 packFlags) const
{
	Node::Pack(data, packFlags);

	int32 priority = sourcePriority;
	if (priority != 0)
	{
		data << ChunkHeader('PRIO', 4);
		data << priority;
	}

	data << ChunkHeader('VOLU', 4);
	data << sourceVolume;

	data << ChunkHeader('FREQ', 4);
	data << sourceFrequency;

	unsigned_int32 state = sourceState;
	if (packFlags & kPackEditor)
	{
		state &= kSourcePersistent;
	}
	else
	{
		state &= ~kSourceEngaged;
	}

	data << ChunkHeader('STAT', 4);
	data << state;

	data << TerminatorChunk;
}

void Source::Unpack(Unpacker& data, unsigned_int32 unpackFlags)
{
	Node::Unpack(data, unpackFlags);
	UnpackChunkList<Source>(data, unpackFlags);
}

bool Source::UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags)
{
	switch (chunkHeader->chunkType)
	{
		case 'PRIO':

			data >> sourcePriority;
			return (true);

		case 'VOLU':

			data >> sourceVolume;
			return (true);

		case 'FREQ':

			data >> sourceFrequency;
			return (true);

		case 'STAT':

			data >> sourceState;
			return (true);
	}

	return (false);
}

inline void Source::LoadSound(void)
{
	soundObject = soundConduit->LoadSound(this);
}

inline void Source::UnloadSound(void)
{
	soundConduit->UnloadSound(soundObject);
	soundObject = nullptr;
}

inline void Source::PlaySound(void)
{
	soundConduit->PlaySound(soundObject);
}

inline void Source::StopSound(void)
{
	soundConduit->StopSound(soundObject);
}

void Source::Preprocess(void)
{
	Node::Preprocess();

	if (!GetManipulator())
	{
		const SourceObject *object = GetObject();

		unsigned_int32 state = sourceState;
		if (!(state & kSourceInitialized))
		{
			sourceState = state | kSourceInitialized;
			sourceVolume = object->GetInitialSourceVolume();
			sourceFrequency = object->GetInitialSourceFrequency();
		}

		if ((state & kSourcePlaying) || ((!(state & kSourceStopped)) && (object->GetSourceFlags() & kSourceInitialPlay)))
		{
			GetWorld()->AddDeferredTask(&playTask);
		}
	}
}

void Source::Neutralize(void)
{
	if (!GetManipulator())
	{
		Stop();
	}

	Node::Neutralize();
}

void Source::PlayTask(DeferredTask *task, void *cookie)
{
	static_cast<Source *>(cookie)->Play();
}

void Source::VolumeTask(DeferredTask *task, void *cookie)
{
	Source *source = static_cast<Source *>(cookie);

	float volume = source->varyVolumeValue;
	source->sourceVolume = volume;

	bool stop = source->varyVolumeStop;
	Sound *sound = source->soundObject;
	if (sound)
	{
		sound->SetVolumeVariationCompletionProc((stop) ? &SoundComplete : nullptr, source);
		sound->VaryVolume(volume, source->varyVolumeTime, stop);
	}
	else if (stop)
	{
		source->sourceState = (source->sourceState & ~kSourcePlaying) | kSourceStopped;
	}
}

void Source::FrequencyTask(DeferredTask *task, void *cookie)
{
	Source *source = static_cast<Source *>(cookie);

	float frequency = source->varyFrequencyValue;
	source->sourceFrequency = frequency;

	bool stop = source->varyFrequencyStop;
	Sound *sound = source->soundObject;
	if (sound)
	{
		sound->SetFrequencyVariationCompletionProc((stop) ? &SoundComplete : nullptr, source);
		sound->VaryFrequency(frequency, source->varyFrequencyTime, stop);
	}
	else if (stop)
	{
		source->sourceState = (source->sourceState & ~kSourcePlaying) | kSourceStopped;
	}
}

void Source::SoundComplete(Sound *sound, void *cookie)
{
	Source *source = static_cast<Source *>(cookie);

	unsigned_int32 state = source->sourceState;
	if (state & kSourcePersistent)
	{
		source->ResetSource();
		source->CallCompletionProc();
	}
	else
	{
		source->CallCompletionProc();
		delete source;
	}
}

void Source::SetSoundConduit(SoundConduit *conduit)
{
	if (conduit)
	{
		soundConduit = conduit;
	}
	else
	{
		soundConduit = &sourceSoundConduit;
	}
}

void Source::InitializeSound(void)
{
	soundObject->SetSoundPriority(sourcePriority);

	soundObject->SetTransformable(this);
	soundObject->SetCompletionProc(&SoundComplete, this);

	SoundGroupType type = GetObject()->GetSoundGroupType();
	if (type != 0)
	{
		soundObject->SetSoundGroup(TheSoundMgr->FindSoundGroup(type));
	}

	soundConduit->SetSoundVolume(soundObject, sourceVolume);
	soundConduit->SetSoundFrequency(soundObject, sourceFrequency);
}

void Source::ResetSource(void)
{
	sourceState = (sourceState & ~(kSourcePlaying | kSourceEngaged)) | kSourceStopped;

	if (soundObject)
	{
		UnloadSound();
	}
}

void Source::SetSourceVolume(float volume)
{
	sourceVolume = volume;

	if (soundObject)
	{
		soundConduit->SetSoundVolume(soundObject, volume);
	}
}

void Source::SetSourceFrequency(float frequency)
{
	sourceFrequency = frequency;

	if (soundObject)
	{
		soundConduit->SetSoundFrequency(soundObject, frequency);
	}
}

void Source::VaryVolume(float volume, int32 time, bool stop)
{
	varyVolumeValue = volume;
	varyVolumeTime = time;
	varyVolumeStop = stop;
	TheTimeMgr->AddTask(&volumeTask);
}

void Source::VaryFrequency(float frequency, int32 time, bool stop)
{
	varyFrequencyValue = frequency;
	varyFrequencyTime = time;
	varyFrequencyStop = stop;
	TheTimeMgr->AddTask(&frequencyTask);
}


AmbientSource::AmbientSource() : Source(kSourceAmbient)
{
}

AmbientSource::AmbientSource(const char *name, bool persistent) : Source(kSourceAmbient, persistent)
{
	SetNewObject(new AmbientSourceObject(name));
}

AmbientSource::AmbientSource(const AmbientSource& ambientSource) : Source(ambientSource)
{
}

AmbientSource::~AmbientSource()
{
}

Node *AmbientSource::Replicate(void) const
{
	return (new AmbientSource(*this));
}

void AmbientSource::Play(void)
{
	sourceState = (sourceState | kSourcePlaying) & ~kSourceStopped;

	if (!soundObject)
	{
		LoadSound();
	}

	InitializeSound();

	const AmbientSourceObject *object = GetObject();
	soundObject->SetLoopIndex(object->GetLoopIndex());

	int32 fadeTime = object->GetFadeTime();
	if (fadeTime == 0)
	{
		PlaySound();
	}
	else
	{
		soundObject->SetSoundProperty(kSoundVolume, 0.0F);
		PlaySound();

		soundObject->SetVolumeVariationCompletionProc(nullptr);
		soundObject->VaryVolume(GetSourceVolume(), fadeTime);
	}
}

void AmbientSource::Stop(void)
{
	if (sourceState & kSourcePlaying)
	{
		if (GetObject()->GetSourceFlags() & kSourceLoop)
		{
			UnloadSound();
		}
		else
		{
			StopSound();
		}
	}

	ResetSource();
}

Sound *AmbientSource::ExtractSound(void)
{
	Sound *sound = soundObject;
	if (sound)
	{
		sound->SetCompletionProc(nullptr);
		soundObject = nullptr;
		ResetSource();
		return (sound);
	}

	return (nullptr);
}


OmniSource::OmniSource() : Source(kSourceOmni)
{
	sourceVelocity.Set(0.0F, 0.0F, 0.0F);
	sourceLifeTime = 0;
}

OmniSource::OmniSource(SourceType type) : Source(type)
{
	sourceVelocity.Set(0.0F, 0.0F, 0.0F);
	sourceLifeTime = 0;
}

OmniSource::OmniSource(SourceType type, bool persistent) : Source(type, persistent)
{
	sourceVelocity.Set(0.0F, 0.0F, 0.0F);
	sourceLifeTime = 0;
}

OmniSource::OmniSource(const char *name, float range, bool persistent) : Source(kSourceOmni, persistent)
{
	SetNewObject(new OmniSourceObject(name, range));

	sourceVelocity.Set(0.0F, 0.0F, 0.0F);
	sourceLifeTime = 0;
}

OmniSource::OmniSource(const OmniSource& omniSource) : Source(omniSource)
{
	sourceVelocity = omniSource.sourceVelocity;
	sourceLifeTime = 0;
}

OmniSource::~OmniSource()
{
}

Node *OmniSource::Replicate(void) const
{
	return (new OmniSource(*this));
}

void OmniSource::Pack(Packer& data, unsigned_int32 packFlags) const
{
	Source::Pack(data, packFlags);

	data << ChunkHeader('VELO', sizeof(Vector3D));
	data << sourceVelocity;

	if (sourceLifeTime != 0)
	{
		data << ChunkHeader('LIFE', 4);
		data << sourceLifeTime;
	}

	data << TerminatorChunk;
}

void OmniSource::Unpack(Unpacker& data, unsigned_int32 unpackFlags)
{
	Source::Unpack(data, unpackFlags);
	UnpackChunkList<OmniSource>(data, unpackFlags);
}

bool OmniSource::UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags)
{
	switch (chunkHeader->chunkType)
	{
		case 'VELO':

			data >> sourceVelocity;
			return (true);

		case 'LIFE':

			data >> sourceLifeTime;
			return (true);
	}

	return (false);
}

void OmniSource::Preprocess(void)
{
	sourceRoom = nullptr;
	sourceObstruction = nullptr;
	playRegionCount = 0;

	Source::Preprocess();

	if (!GetManipulator())
	{
		sourceRange = GetObject()->GetSourceRange();

		if (Playing())
		{
			SetActiveUpdateFlags(GetActiveUpdateFlags() | kUpdatePostprocess);
		}
	}
}

void OmniSource::SetSourceVelocity(const Vector3D& velocity)
{
	sourceVelocity = velocity;

	if (soundObject)
	{
		soundObject->SetVelocity(velocity);
	}
}

void OmniSource::ResetSource(void)
{
	Source::ResetSource();

	ListElement<OmniSource>::Detach();
	rootRegionList.Purge();

	sourceLifeTime = 0;
	playRegionCount = 0;

	SetActiveUpdateFlags(GetActiveUpdateFlags() & ~kUpdatePostprocess);
}

void OmniSource::InitializeSound(void)
{
	Source::InitializeSound();

	const OmniSourceObject *object = GetObject();
	unsigned_int32 flags = object->GetSourceFlags();

	unsigned_int32 soundFlags = soundObject->GetSoundFlags() & ~(kSoundDopplerShift | kSoundDistanceDelay | kSoundReverb);

	if (flags & kSourceDopplerShift)
	{
		soundFlags |= kSoundDopplerShift;
	}

	if (flags & kSourceDistanceDelay)
	{
		soundFlags |= kSoundDistanceDelay;
	}

	if (flags & kSourceReverb)
	{
		soundFlags |= kSoundReverb;
	}

	soundObject->SetSoundFlags(soundFlags | kSoundSpatialized);

	soundObject->SetSoundProperty(kSoundDirectVolume, 1.0F);
	soundObject->SetSoundProperty(kSoundDirectHFVolume, 1.0F);
	soundObject->SetSoundProperty(kSoundReflectionVolume, object->GetReflectionVolume());
	soundObject->SetSoundProperty(kSoundReflectionHFVolume, object->GetReflectionHFVolume());
	soundObject->SetSoundProperty(kSoundMaxAttenDistance, object->GetSourceRange());

	soundObject->SetVelocity(sourceVelocity);
	soundObject->SetSoundRoom(sourceRoom);

	EndUpdate();

	if (flags & kSourceObstruction)
	{
		DetectObstruction();
	}
}

void OmniSource::Play(void)
{
	unsigned_int32 state = sourceState;
	if (!(state & kSourcePlaying))
	{
		sourceState = (state | kSourcePlaying) & ~kSourceStopped;

		if (soundObject)
		{
			sourceLifeTime = sourceDuration;
		}
	}

	SetActiveUpdateFlags(GetActiveUpdateFlags() | kUpdatePostprocess);
	GetWorld()->AddPlayingSource(this);
}

void OmniSource::Stop(void)
{
	if (sourceState & kSourceEngaged)
	{
		Disengage();
	}

	ResetSource();
}

bool OmniSource::Engage(void)
{
	unsigned_int32 flags = GetObject()->GetSourceFlags();
	if ((sourceLifeTime > 0) || (flags & kSourceLoop))
	{
		sourceState |= kSourceEngaged;
		if (!soundObject)
		{
			LoadSound();

			sourceDuration = soundObject->GetDuration();
			if (sourceLifeTime <= 0)
			{
				sourceLifeTime = sourceDuration;
			}
		}

		if (flags & kSourceLoop)
		{
			if (flags & kSourceRandom)
			{
				soundObject->SetStartTime(Math::Random(sourceDuration));
			}
		}
		else
		{
			soundObject->SetStartTime(sourceDuration - sourceLifeTime);
		}

		InitializeSound();
		PlaySound();
		return (true);
	}

	return (false);
}

void OmniSource::Disengage(void)
{
	sourceState &= ~kSourceEngaged;
	playRegionCount = 0;

	if (GetObject()->GetSourceFlags() & kSourceLoop)
	{
		UnloadSound();
	}
	else
	{
		StopSound();
	}
}

void OmniSource::BeginUpdate(void)
{
	sourceState &= ~kSourceAudible;
	playRegionCount = 0;
}

void OmniSource::EndUpdate(void)
{
	int32 count = playRegionCount;
	soundObject->SetSoundPathCount(count);

	for (machine a = 0; a < count; a++)
	{
		const SourceRegion *region = playRegion[a];
		soundObject->SetSoundPathData(a, &region->GetAudiblePosition(), region->GetAudiblePathLength());
	}
}

void OmniSource::Move(void)
{
	if (!(GetObject()->GetSourceFlags() & kSourceLoop))
	{
		if (soundObject)
		{
			int32 time = sourceLifeTime - TheTimeMgr->GetSystemDeltaTime();
			sourceLifeTime = time;

			if (time <= 0)
			{
				unsigned_int32 state = sourceState;
				if (!(state & kSourceEngaged))
				{
					sourceState = (state & ~kSourcePlaying) | kSourceStopped;
					CallCompletionProc();

					if (state & kSourcePersistent)
					{
						ListElement<OmniSource>::Detach();
					}
					else
					{
						delete this;
					}
				}
			}
		}
		else
		{
			LoadSound();
			sourceDuration = soundObject->GetDuration();
			sourceLifeTime = sourceDuration;
		}
	}
}

void OmniSource::CalculatePermeation(SourceRegion *region)
{
	Zone *zone = region->GetZone();
	zone->SetTraversalExclusionMask(zone->GetTraversalExclusionMask() | kZoneTraversalLocal);

	const Portal *portal = zone->GetFirstPortal();
	while (portal)
	{
		if ((portal->Enabled()) && (portal->GetPortalType() == kPortalDirect) && (!(portal->GetObject()->GetPortalFlags() & kPortalSourceInhibit)))
		{
			Zone *connectedZone = portal->GetConnectedZone();
			if ((connectedZone) && (connectedZone->GetTraversalExclusionMask() == 0))
			{
				if (!((portal->GetWorldPlane() ^ GetWorldPosition()) < 0.0F))
				{
					const Point3D& permeatedPosition = region->GetPermeatedPosition();
					Point3D portalPosition = portal->CalculateClosestBoundaryPoint(permeatedPosition);

					float length = Magnitude(portalPosition - permeatedPosition) + region->GetPermeatedPathLength();
					if (length < sourceRange)
					{
						SourceRegion *newRegion = new SourceRegion(this, connectedZone);
						newRegion->SetPermeatedPortal(portal, portalPosition, length);
						connectedZone->AddSourceRegion(newRegion);

						region->AppendSubnode(newRegion);
						CalculatePermeation(newRegion);
					}
				}
			}
		}

		portal = portal->Next();
	}

	zone->SetTraversalExclusionMask(zone->GetTraversalExclusionMask() & ~kZoneTraversalLocal);
}

bool OmniSource::EstablishRootRegions(Zone *zone, int32 maxDepth, int32 forcedDepth)
{
	if (forcedDepth < 0)
	{
		if (zone->GetObject()->InteriorPoint(zone->GetInverseWorldTransform() * GetWorldPosition()))
		{
			bool covered = false;

			if (maxDepth > 0)
			{
				Zone *subzone = zone->GetFirstSubzone();
				while (subzone)
				{
					covered |= EstablishRootRegions(subzone, maxDepth - 1, -1);
					subzone = subzone->Next();
				}
			}

			if (!covered)
			{
				RootSourceRegion *region = new RootSourceRegion(this, zone);
				rootRegionList.Append(region);
				zone->AddSourceRegion(region);
			}

			return (true);
		}
	}
	else
	{
		RootSourceRegion *region = new RootSourceRegion(this, zone);
		rootRegionList.Append(region);
		zone->AddSourceRegion(region);

		if (maxDepth > 0)
		{
			Zone *subzone = zone->GetFirstSubzone();
			while (subzone)
			{
				EstablishRootRegions(subzone, maxDepth - 1, forcedDepth - 1);
				subzone = subzone->Next();
			}
		}
	}

	return (false);
}

void OmniSource::HandlePostprocessUpdate(void)
{
	rootRegionList.Purge();
	EstablishRootRegions(GetOwningZone(), GetMaxSubzoneDepth(), GetForcedSubzoneDepth());

	sourceRoom = nullptr;

	RootSourceRegion *region = rootRegionList.First();
	while (region)
	{
		Zone *zone = region->GetZone();
		zone->SetTraversalExclusionMask(kZoneTraversalGlobal);

		const AcousticsSpace *acousticsSpace = zone->GetConnectedAcousticsSpace();
		if (acousticsSpace)
		{
			sourceRoom = acousticsSpace->GetSoundRoom();
		}

		region = region->Next();
	}

	if (soundObject)
	{
		soundObject->SetSoundRoom(sourceRoom);
	}

	region = rootRegionList.First();
	while (region)
	{
		CalculatePermeation(region);
		region = region->Next();
	}

	region = rootRegionList.First();
	while (region)
	{
		region->GetZone()->SetTraversalExclusionMask(0);
		region = region->Next();
	}

	if ((sourceState & kSourceEngaged) && (GetObject()->GetSourceFlags() & kSourceObstruction))
	{
		DetectObstruction();
	}
}

void OmniSource::DetectObstruction(void)
{
	const AcousticsProperty *property = GetWorld()->DetectObstruction(GetWorldPosition());
	if (sourceObstruction != property)
	{
		sourceObstruction = property;

		const OmniSourceObject *object = GetObject();
		if (property)
		{
			soundObject->SetSoundProperty(kSoundDirectVolume, property->GetDirectVolume());
			soundObject->SetSoundProperty(kSoundDirectHFVolume, property->GetDirectHFVolume());
			soundObject->SetSoundProperty(kSoundReflectionVolume, object->GetReflectionVolume() * property->GetReflectionVolume());
			soundObject->SetSoundProperty(kSoundReflectionHFVolume, object->GetReflectionHFVolume() * property->GetReflectionHFVolume());
		}
		else
		{
			soundObject->SetSoundProperty(kSoundDirectVolume, 1.0F);
			soundObject->SetSoundProperty(kSoundDirectHFVolume, 1.0F);
			soundObject->SetSoundProperty(kSoundReflectionVolume, object->GetReflectionVolume());
			soundObject->SetSoundProperty(kSoundReflectionHFVolume, object->GetReflectionHFVolume());
		}
	}
}

void OmniSource::AddPlayRegion(SourceRegion *region, const Point3D& listenerPosition)
{
	region = region->GetPrimaryRegion();
	const Portal *portal = region->GetPermeatedPortal();
	if (portal)
	{
		int32 count = playRegionCount;
		if (count < kMaxSoundPathCount)
		{
			playRegion[count] = region;
			playRegionCount = count + 1;

			SourceRegion *baseRegion = region;
			baseRegion->SetAudibleSubregion(nullptr);

			const Point3D *p1 = &listenerPosition;
			float length = 0.0F;

			SourceRegion *superRegion = baseRegion->GetSuperNode()->GetPrimaryRegion();
			for (;;)
			{
				Point3D		q;

				SourceRegion *nextRegion = superRegion->GetSuperNode();
				if (nextRegion)
				{
					const Point3D& p2 = superRegion->GetPermeatedPosition();
					if (portal->CalculateClosestBoundaryPoint((p2 ^ *p1).Standardize(), &q))
					{
						length += Magnitude(q - *p1);
						baseRegion->SetAudiblePosition(q, length);
						superRegion->SetAudibleSubregion(baseRegion);

						p1 = &baseRegion->GetAudiblePosition();
						baseRegion = superRegion;
					}
				}
				else
				{
					const Point3D& p2 = GetWorldPosition();
					if (portal->CalculateClosestBoundaryPoint((p2 ^ *p1).Standardize(), &q))
					{
						length += Magnitude(q - *p1);
						baseRegion->SetAudiblePosition(q, length);
						length += Magnitude(p2 - q);
					}
					else
					{
						length += Magnitude(p2 - *p1);
						baseRegion->SetAudiblePosition(p2, length);
					}

					superRegion->SetAudibleSubregion(baseRegion);
					break;
				}

				portal = superRegion->GetPermeatedPortal();
				superRegion = nextRegion->GetPrimaryRegion();
			}

			SourceRegion *subregion = superRegion->GetAudibleSubregion();
			do
			{
				subregion->InvertAudiblePathLength(length);
				subregion = subregion->GetAudibleSubregion();
			} while (subregion);
		}

		const AcousticsSpace *prevSpace = region->GetZone()->GetConnectedAcousticsSpace();
		const SourceRegion *prevRegion = region;
		for (;;)
		{
			region = region->GetSuperNode();
			if (!region)
			{
				break;
			}

			region = region->GetPrimaryRegion();

			const AcousticsSpace *space = region->GetZone()->GetConnectedAcousticsSpace();
			if ((space) && (space != prevSpace))
			{
				SoundRoom *room = space->GetSoundRoom();
				room->SetOutputRoom((prevSpace) ? prevSpace->GetSoundRoom() : nullptr);
				room->SetRoomPosition(prevRegion->GetAudiblePosition());

				prevSpace = space;
				prevRegion = region;
			}
		}
	}
}


DirectedSource::DirectedSource() : OmniSource(kSourceDirected)
{
}

DirectedSource::DirectedSource(const char *name, float range, float apex, bool persistent) : OmniSource(kSourceDirected, persistent)
{
	SetNewObject(new DirectedSourceObject(name, range, apex));
}

DirectedSource::DirectedSource(const DirectedSource& directedSource) : OmniSource(directedSource)
{
}

DirectedSource::~DirectedSource()
{
}

Node *DirectedSource::Replicate(void) const
{
	return (new DirectedSource(*this));
}

void DirectedSource::InitializeSound(void)
{
	OmniSource::InitializeSound();
	soundObject->SetSoundFlags(soundObject->GetSoundFlags() | kSoundCones);

	const DirectedSourceObject *object = GetObject();
	float f = object->GetApexTangent();

	soundObject->SetSoundProperty(kSoundOuterConeCosine, f * InverseSqrt(f * f + 1.0F));
	soundObject->SetSoundProperty(kSoundOuterConeVolume, object->GetOuterConeVolume());
	soundObject->SetSoundProperty(kSoundOuterConeHFVolume, object->GetOuterConeHFVolume());
}

// ZYUQURM
