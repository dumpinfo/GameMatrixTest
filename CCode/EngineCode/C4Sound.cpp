 

#include "C4Sound.h"
#include "C4Engine.h"
#include "C4Compression.h"


using namespace C4;


namespace
{
	#if C4DESKTOP || C4MOBILE

		enum
		{
			kSoundOutputSampleRate	= 44100,
			kMaxFeedbackDelay		= 4689
		};

	#elif C4PS4 //[ PS4

		// -- PS4 code hidden --

	#elif C4PS3 //[ PS3

		// -- PS3 code hidden --

	#endif //]


	const float kMaxFeedbackTimeDelay = (float) kMaxFeedbackDelay * 1000.0F / (float) kSoundOutputSampleRate;
	const int32 kMaxFeedbackMixCount = (int32) PositiveCeil(kMaxReverbDecayTime / kMaxFeedbackTimeDelay);


	enum
	{
		kMixFractionSize	= 10,
		kMixFractionMax		= (1 << kMixFractionSize) - 1
	};


	const float kMixFractionMultiplier = (float) (1 << kMixFractionSize);
	const float kMixFractionReciprocal = 1.0F / kMixFractionMultiplier;
	const float kMinDistanceDelayPlayFrame = -(float) (1 << (30 - kMixFractionSize));


	enum
	{
		kAudioCompressionNone			= 0,
		kAudioCompressionGeneral		= 1
	};


	enum
	{
		kAudioIndependent				= 0,
		kAudioLeftDifference			= 1,
		kAudioRightDifference			= 2,
		kAudioAverageDifference			= 3,
		kAudioChannelDependencyCount	= 4
	};


	struct AudioLeftDifferenceCombiner
	{
		static void Output(int32 sample, int32 left, Sample *restrict audio)
		{
			WriteLittleEndianS16(&audio[0], left);
			WriteLittleEndianS16(&audio[1], (Sample) (left + sample));
		}
	};

	struct AudioRightDifferenceCombiner
	{
		static void Output(int32 sample, int32 right, Sample *restrict audio)
		{
			WriteLittleEndianS16(&audio[0], (Sample) (right - sample));
			WriteLittleEndianS16(&audio[1], right);
		}
	};

	struct AudioAverageDifferenceCombiner
	{
		static void Output(int32 sample, int32 average, Sample *restrict audio)
		{
			WriteLittleEndianS16(&audio[0], (Sample) (average - (sample >> 1)));
			WriteLittleEndianS16(&audio[1], (Sample) (average + ((sample + 1) >> 1)));
		}
	};


	const int16 adpcmTable[16] =
	{
		0x00E6, 0x00E6, 0x00E6, 0x00E6, 0x0133, 0x0199, 0x0200, 0x0266,
		0x0300, 0x0266, 0x0200, 0x0199, 0x0133, 0x00E6, 0x00E6, 0x00E6
	};


	struct RiffChunkHeader
	{
		unsigned_int32		type;
		unsigned_int32		size; 
	};
}
 

SoundMgr *C4::TheSoundMgr = nullptr; 


namespace C4 
{
	template <> SoundMgr Manager<SoundMgr>::managerObject(0); 
	template <> SoundMgr **Manager<SoundMgr>::managerPointer = &TheSoundMgr; 

	template <> const char *const Manager<SoundMgr>::resultString[] =
	{
		nullptr, 
		"SoundMgr initialization failed",
		"Sound load failed",
		"Sound play failed",
		"Sound invalid format",
		"Sound too large for non-stream"
	};

	template <> const unsigned_int32 Manager<SoundMgr>::resultIdentifier[] =
	{
		0, 'INIT', 'LOAD', 'PLAY', 'FINV', 'SIZE'
	};

	template class Manager<SoundMgr>;

	template <> Heap EngineMemory<Sound>::heap("Sound", MemoryMgr::CalculatePoolSize(kMaxSoundCount, sizeof(Sound)));
	template class EngineMemory<Sound>;
}


ResourceDescriptor SoundResource::descriptor("wav", 0, 8388608, "C4/missing");


AudioCompressor::AudioCompressor(const AudioTrackHeader *audioTrackHeader)
{
	channelCount = audioTrackHeader->audioChannelCount;

	int32 frameCount = audioTrackHeader->blockFrameCount;
	unsigned_int32 blockCodeSize = frameCount * sizeof(Sample);

	if (channelCount == 1)
	{
		compressorStorage = new char[blockCodeSize * 3];
		audioChannelCode[0] = reinterpret_cast<unsigned_int8 *>(compressorStorage);
		compressedCode = audioChannelCode[0] + blockCodeSize * 2;
	}
	else
	{
		compressorStorage = new char[blockCodeSize * 11];

		audioChannelData[0] = reinterpret_cast<Sample *>(compressorStorage);
		audioChannelData[1] = audioChannelData[0] + frameCount;
		audioDifferenceData = audioChannelData[1] + frameCount;

		audioChannelCode[0] = reinterpret_cast<unsigned_int8 *>(audioDifferenceData + frameCount);
		audioChannelCode[1] = audioChannelCode[0] + blockCodeSize * 2;
		audioDifferenceCode = audioChannelCode[1] + blockCodeSize * 2;

		compressedCode = audioDifferenceCode + blockCodeSize * 2;
	}
}

AudioCompressor::~AudioCompressor()
{
	delete[] compressorStorage;
}

unsigned_int32 AudioCompressor::EncodeDeltaRun(const Sample *sample, int32 count, int32 level, unsigned_int8 *restrict code)
{
	const unsigned_int8 *start = code;

	int32 remain = count;
	do
	{
		count = Min(remain, 4112);
		remain -= count;

		if (count < 17)
		{
			*code++ = (unsigned_int8) ((level << 4) | (count - 1));
		}
		else
		{
			int32 k = count - 17;
			code[0] = (unsigned_int8) (0x80 | (level << 4) | (k >> 8));
			code[1] = (unsigned_int8) k;
			code += 2;
		}

		if (level == 0)
		{
			int32 k = count & ~3;
			for (machine a = 0; a < k; a += 4)
			{
				int32 sample0 = sample[a - 1];
				int32 sample1 = sample[a];
				int32 sample2 = sample[a + 1];
				int32 sample3 = sample[a + 2];
				int32 sample4 = sample[a + 3];

				unsigned_int32 value = (sample1 - sample0) & 0x03;
				value |= ((sample2 - sample1) << 2) & 0x0C;
				value |= ((sample3 - sample2) << 4) & 0x30;
				value |= ((sample4 - sample3) << 6) & 0xC0;
				*code++ = (unsigned_int8) value;
			}

			if (count & 3)
			{
				unsigned_int32 value = (sample[count - 1] - sample[count - 2]) & 0x03;
				for (machine a = count - 2; a >= k; a--)
				{
					value = (value << 2) | ((sample[a] - sample[a - 1]) & 0x03);
				}

				*code++ = (unsigned_int8) value;
			}
		}
		else if (level == 1)
		{
			int32 k = count & ~1;
			for (machine a = 0; a < k; a += 2)
			{
				int32 sample0 = sample[a - 1];
				int32 sample1 = sample[a];
				int32 sample2 = sample[a + 1];

				unsigned_int32 value = (sample1 - sample0) & 0x0F;
				value |= ((sample2 - sample1) << 4) & 0xF0;
				*code++ = (unsigned_int8) value;
			}

			if (count & 1)
			{
				*code++ = (unsigned_int8) (sample[count - 1] - sample[count - 2]);
			}
		}
		else if (level == 2)
		{
			int32 k = count & ~3;
			for (machine a = 0; a < k; a += 4)
			{
				int32 sample0 = sample[a - 1];
				int32 sample1 = sample[a];
				int32 sample2 = sample[a + 1];
				int32 sample3 = sample[a + 2];
				int32 sample4 = sample[a + 3];

				unsigned_int32 value1 = ((sample1 - sample0) << 2) & 0xFC;
				value1 |= ((sample2 - sample1) >> 4) & 0x03;
				unsigned_int32 value2 = ((sample2 - sample1) << 4) & 0xF0;
				value2 |= ((sample3 - sample2) >> 2) & 0x0F;
				unsigned_int32 value3 = ((sample3 - sample2) << 6) & 0xC0;
				value3 |= (sample4 - sample3) & 0x3F;

				code[0] = (unsigned_int8) value1;
				code[1] = (unsigned_int8) value2;
				code[2] = (unsigned_int8) value3;
				code += 3;
			}

			for (machine a = k; a < count; a++)
			{
				*code++ = (unsigned_int8) (sample[a] - sample[a - 1]);
			}
		}
		else if (level == 3)
		{
			int32 sample0 = sample[-1];
			for (machine a = 0; a < count; a++)
			{
				int32 sample1 = sample[a];
				*code++ = (unsigned_int8) (sample1 - sample0);
				sample0 = sample1;
			}
		}
		else if (level == 4)
		{
			int32 k = count & ~1;
			for (machine a = 0; a < k; a += 2)
			{
				int32 sample0 = sample[a - 1];
				int32 sample1 = sample[a];
				int32 sample2 = sample[a + 1];

				int32 value1 = sample1 - sample0;
				int32 value2 = sample2 - sample1;
				code[0] = (unsigned_int8) value1;
				code[1] = (unsigned_int8) value2;
				code[2] = (unsigned_int8) (((value1 >> 8) & 0x0F) | ((value2 >> 4) & 0xF0));
				code += 3;
			}

			if (count & 1)
			{
				int32 value = sample[count - 1];
				code[0] = (unsigned_int8) value;
				code[1] = (unsigned_int8) (value >> 8);
				code += 2;
			}
		}
		else
		{
			for (machine a = 0; a < count; a++)
			{
				int32 value = sample[a];
				code[0] = (unsigned_int8) value;
				code[1] = (unsigned_int8) (value >> 8);
				code += 2;
			}
		}

		sample += count;
	} while (remain > 0);

	return ((unsigned_int32) (code - start));
}

unsigned_int32 AudioCompressor::CompressAudioChannel(const Sample *audio, int32 frameCount, unsigned_int8 *restrict code)
{
	enum
	{
		kLevelCount = 6
	};

	static const int8 minTransitionCount[kLevelCount - 1][kLevelCount] =
	{
		{ 7, 0, 0, 0, 0, 0},
		{10, 6, 0, 0, 0, 0},
		{ 3, 4, 8, 0, 0, 0},
		{ 2, 2, 4, 4, 0, 0},
		{ 2, 2, 2, 2, 3, 0}
	};

	int32 sample1 = audio[0];
	code[0] = (unsigned_int8) sample1;
	code[1] = (unsigned_int8) (sample1 >> 8);
	unsigned_int32 codeSize = 2;

	unsigned_int32 frameStart = 1;
	int32 level = kLevelCount - 1;
	int32 levelCount[kLevelCount] = {0};

	for (machine a = 1; a < frameCount; a++)
	{
		int32 sample2 = audio[a];
		int32 delta = sample2 - sample1;
		sample1 = sample2;

		int32 k = kLevelCount - 1;
		if ((unsigned_int32) (delta + 2) < 4U)
		{
			k = 0;
		}
		else if ((unsigned_int32) (delta + 8) < 16U)
		{
			k = 1;
		}
		else if ((unsigned_int32) (delta + 32) < 64U)
		{
			k = 2;
		}
		else if ((unsigned_int32) (delta + 128) < 256U)
		{
			k = 3;
		}
		else if ((unsigned_int32) (delta + 2048) < 4096U)
		{
			k = 4;
		}

		if (k > level)
		{
			if (levelCount[level] >= minTransitionCount[k - 1][level])
			{
				codeSize += EncodeDeltaRun(&audio[frameStart], a - frameStart, level, &code[codeSize]);

				level = k;
				frameStart = (unsigned_int32) a;

				for (machine i = 0; i < k; i++)
				{
					levelCount[i] = 0;
				}

				for (machine i = k; i < kLevelCount; i++)
				{
					levelCount[i] = 1;
				}

				continue;
			}

			level = k;
		}
		else if ((k < level) && (levelCount[k] >= minTransitionCount[level - 1][k]))
		{
			codeSize += EncodeDeltaRun(&audio[frameStart], a - frameStart, level, &code[codeSize]);

			level = k;
			frameStart = (unsigned_int32) a;

			for (machine i = 0; i < k; i++)
			{
				levelCount[i] = 0;
			}

			for (machine i = k; i < kLevelCount; i++)
			{
				levelCount[i] = 1;
			}

			continue;
		}

		for (machine i = 0; i < k; i++)
		{
			levelCount[i] = 0;
		}

		for (machine i = k; i < kLevelCount; i++)
		{
			levelCount[i]++;
		}
	}

	return (codeSize + EncodeDeltaRun(&audio[frameStart], frameCount - frameStart, level, &code[codeSize]));
}

unsigned_int32 AudioCompressor::CompressBlock(const Sample *audio, int32 frameCount, AudioBlockHeader *audioBlockHeader)
{
	if (channelCount == 1)
	{
		unsigned_int32 dataSize = CompressAudioChannel(audio, frameCount, audioChannelCode[0]);

		unsigned_int8 compression = kAudioCompressionGeneral;
		unsigned_int32 codeSize = Comp::CompressData(audioChannelCode[0], dataSize, compressedCode);
		if (codeSize == 0)
		{
			MemoryMgr::CopyMemory(audioChannelCode[0], compressedCode, dataSize);
			compression = kAudioCompressionNone;

			codeSize = (dataSize + 3) & ~3;
			for (unsigned_machine a = dataSize; a < codeSize; a++)
			{
				compressedCode[a] = 0;
			}
		}

		audioBlockHeader->audioBlockFlags = 0;
		audioBlockHeader->audioChannelData[0].audioChannelFlags = compression;
		audioBlockHeader->audioChannelData[0].audioCodeOffset = sizeof(AudioChannelData);
		audioBlockHeader->audioChannelData[0].audioCodeSize = codeSize;
		audioBlockHeader->audioChannelData[0].audioDataSize = dataSize;

		return (codeSize);
	}

	for (machine a = 0; a < frameCount; a++)
	{
		int32 left = audio[0];
		int32 right = audio[1];
		audioChannelData[0][a] = (Sample) left;
		audioChannelData[1][a] = (Sample) right;
		audioDifferenceData[a] = (Sample) (right - left);
		audio += 2;
	}

	unsigned_int32 leftDataSize = CompressAudioChannel(audioChannelData[0], frameCount, audioChannelCode[0]);
	unsigned_int32 rightDataSize = CompressAudioChannel(audioChannelData[1], frameCount, audioChannelCode[1]);
	unsigned_int32 differenceDataSize = CompressAudioChannel(audioDifferenceData, frameCount, audioDifferenceCode);

	unsigned_int32 independentDataSize = leftDataSize + rightDataSize;
	unsigned_int32 leftDifferenceDataSize = leftDataSize + differenceDataSize;
	unsigned_int32 rightDifferenceDataSize = rightDataSize + differenceDataSize;

	unsigned_int32 audioBlockFlags = kAudioIndependent;
	const unsigned_int8 *channelData1 = audioChannelCode[0];
	const unsigned_int8 *channelData2 = audioChannelCode[1];
	unsigned_int32 dataSize1 = leftDataSize;
	unsigned_int32 dataSize2 = rightDataSize;

	if ((leftDifferenceDataSize < independentDataSize) && (leftDifferenceDataSize <= rightDifferenceDataSize))
	{
		audioBlockFlags = kAudioLeftDifference;

		channelData2 = audioDifferenceCode;
		dataSize2 = differenceDataSize;
	}
	else if (rightDifferenceDataSize < independentDataSize)
	{
		audioBlockFlags = kAudioRightDifference;

		channelData1 = audioChannelCode[1];
		dataSize1 = rightDataSize;

		channelData2 = audioDifferenceCode;
		dataSize2 = differenceDataSize;
	}

	unsigned_int8 compression1 = kAudioCompressionGeneral;
	unsigned_int8 *code = compressedCode;

	unsigned_int32 codeSize1 = Comp::CompressData(channelData1, dataSize1, code);
	if (codeSize1 == 0)
	{
		MemoryMgr::CopyMemory(channelData1, code, dataSize1);
		compression1 = kAudioCompressionNone;

		codeSize1 = (dataSize1 + 3) & ~3;
		for (unsigned_machine a = dataSize1; a < codeSize1; a++)
		{
			code[a] = 0;
		}
	}

	unsigned_int8 compression2 = kAudioCompressionGeneral;
	code += codeSize1;

	unsigned_int32 codeSize2 = Comp::CompressData(channelData2, dataSize2, code);
	if (codeSize2 == 0)
	{
		MemoryMgr::CopyMemory(channelData2, code, dataSize2);
		compression2 = kAudioCompressionNone;

		codeSize2 = (dataSize2 + 3) & ~3;
		for (unsigned_machine a = dataSize2; a < codeSize2; a++)
		{
			code[a] = 0;
		}
	}

	audioBlockHeader->audioBlockFlags = audioBlockFlags;
	audioBlockHeader->audioChannelData[0].audioChannelFlags = compression1;
	audioBlockHeader->audioChannelData[0].audioCodeOffset = sizeof(AudioChannelData) * 2;
	audioBlockHeader->audioChannelData[0].audioCodeSize = codeSize1;
	audioBlockHeader->audioChannelData[0].audioDataSize = dataSize1;

	audioBlockHeader->audioChannelData[1].audioChannelFlags = compression2;
	audioBlockHeader->audioChannelData[1].audioCodeOffset = sizeof(AudioChannelData) + codeSize1;
	audioBlockHeader->audioChannelData[1].audioCodeSize = codeSize2;
	audioBlockHeader->audioChannelData[1].audioDataSize = dataSize2;

	return (codeSize1 + codeSize2);
}


AudioDecompressor::AudioDecompressor(const AudioTrackHeader *audioTrackHeader)
{
	channelCount = audioTrackHeader->audioChannelCount;

	if (channelCount == 1)
	{
		decompressorStorage = new char[audioTrackHeader->maxBlockCodeSize + audioTrackHeader->maxBlockDataSize];
		audioBlockHeader = reinterpret_cast<AudioBlockHeader *>(decompressorStorage);
		audioBlockData = reinterpret_cast<unsigned_int8 *>(decompressorStorage + audioTrackHeader->maxBlockCodeSize);
	}
	else
	{
		unsigned_int32 maxBlockDataSize = audioTrackHeader->maxBlockDataSize;
		decompressorStorage = new char[audioTrackHeader->maxBlockCodeSize + maxBlockDataSize + audioTrackHeader->blockFrameCount * sizeof(Sample)];

		audioBlockHeader = reinterpret_cast<AudioBlockHeader *>(decompressorStorage);
		audioBlockData = reinterpret_cast<unsigned_int8 *>(decompressorStorage + audioTrackHeader->maxBlockCodeSize);
		audioSampleBuffer = reinterpret_cast<Sample *>(audioBlockData + maxBlockDataSize);
	}
}

AudioDecompressor::~AudioDecompressor()
{
	delete[] decompressorStorage;
}

template <int stride> void AudioDecompressor::DecompressAudioMono(const unsigned_int8 *code, int32 sampleCount, Sample *restrict audio)
{
	Sample sample = code[0] | (reinterpret_cast<const int8 *>(code)[1] << 8);
	code += 2;

	WriteLittleEndianS16(&audio[0], sample);
	audio += stride;
	sampleCount--;

	while (sampleCount > 0)
	{
		int32 k = *code++;
		int32 level = (k >> 4) & 0x07;
		int32 count = (k & 0x0F) + 1;
		if (k & 0x80)
		{
			k = *code++;
			count = ((count << 8) | k) - 239;
		}

		switch (level)
		{
			case 0:
			{
				k = count & ~3;
				for (machine a = 0; a < k; a += 4)
				{
					int32 value = *code++;
					int32 delta1 = value << 30 >> 30;
					int32 delta2 = value << 28 >> 30;
					int32 delta3 = value << 26 >> 30;
					int32 delta4 = value << 24 >> 30;

					sample = (Sample) (sample + delta1);
					WriteLittleEndianS16(&audio[0], sample);
					sample = (Sample) (sample + delta2);
					WriteLittleEndianS16(&audio[stride], sample);
					sample = (Sample) (sample + delta3);
					WriteLittleEndianS16(&audio[stride * 2], sample);
					sample = (Sample) (sample + delta4);
					WriteLittleEndianS16(&audio[stride * 3], sample);
					audio += stride * 4;
				}

				if (count & 3)
				{
					int32 value = *code++;
					for (machine a = k; a < count; a++)
					{
						int32 delta = value << 30 >> 30;
						value >>= 2;

						sample = (Sample) (sample + delta);
						WriteLittleEndianS16(&audio[0], sample);
						audio += stride;
					}
				}

				break;
			}

			case 1:
			{
				k = count & ~1;
				for (machine a = 0; a < k; a += 2)
				{
					int32 value = *code++;
					int32 delta1 = value << 28 >> 28;
					int32 delta2 = value << 24 >> 28;

					sample = (Sample) (sample + delta1);
					WriteLittleEndianS16(&audio[0], sample);
					sample = (Sample) (sample + delta2);
					WriteLittleEndianS16(&audio[stride], sample);
					audio += stride * 2;
				}

				if (count & 1)
				{
					int32 delta = *reinterpret_cast<const int8 *>(code++);
					sample = (Sample) (sample + delta);
					WriteLittleEndianS16(&audio[0], sample);
					audio += stride;
				}

				break;
			}

			case 2:
			{
				k = count & ~3;
				for (machine a = 0; a < k; a += 4)
				{
					int32 value1 = code[0];
					int32 value2 = code[1];
					int32 value3 = code[2];
					code += 3;

					int32 delta1 = value1 << 24 >> 26;
					int32 delta2 = ((value1 << 30) | (value2 << 22)) >> 26;
					int32 delta3 = ((value2 << 28) | (value3 << 20)) >> 26;
					int32 delta4 = value3 << 26 >> 26;

					sample = (Sample) (sample + delta1);
					WriteLittleEndianS16(&audio[0], sample);
					sample = (Sample) (sample + delta2);
					WriteLittleEndianS16(&audio[stride], sample);
					sample = (Sample) (sample + delta3);
					WriteLittleEndianS16(&audio[stride * 2], sample);
					sample = (Sample) (sample + delta4);
					WriteLittleEndianS16(&audio[stride * 3], sample);
					audio += stride * 4;
				}

				for (machine a = k; a < count; a++)
				{
					int32 delta = *reinterpret_cast<const int8 *>(code++);
					sample = (Sample) (sample + delta);
					WriteLittleEndianS16(&audio[0], sample);
					audio += stride;
				}

				break;
			}

			case 3:
			{
				for (machine a = 0; a < count; a++)
				{
					int32 delta = reinterpret_cast<const int8 *>(code)[a];
					sample = (Sample) (sample + delta);
					WriteLittleEndianS16(&audio[0], sample);
					audio += stride;
				}

				code += count;
				break;
			}

			case 4:
			{
				k = count & ~1;
				for (machine a = 0; a < k; a += 2)
				{
					int32 value1 = code[0];
					int32 value2 = code[1];
					int32 value3 = code[2];
					code += 3;

					int32 delta1 = value1 | ((value3 << 28 >> 20) & ~0xFF);
					int32 delta2 = value2 | ((value3 << 24 >> 20) & ~0xFF);

					sample = (Sample) (sample + delta1);
					WriteLittleEndianS16(&audio[0], sample);
					sample = (Sample) (sample + delta2);
					WriteLittleEndianS16(&audio[stride], sample);
					audio += stride * 2;
				}

				if (count & 1)
				{
					sample = (Sample) (code[0] | (reinterpret_cast<const int8 *>(code)[1] << 8));
					WriteLittleEndianS16(&audio[0], sample);
					audio += stride;
					code += 2;
				}

				break;
			}

			default:
			{
				for (machine a = 0; a < count; a++)
				{
					sample = (Sample) (code[0] | (reinterpret_cast<const int8 *>(code)[1] << 8));
					WriteLittleEndianS16(&audio[0], sample);
					audio += stride;
					code += 2;
				}

				break;
			}
		}

		sampleCount -= count;
	}
}

template <class combiner> void AudioDecompressor::DecompressAudioStereo(const unsigned_int8 *code, int32 sampleCount, const Sample *channel, Sample *restrict audio)
{
	Sample sample = code[0] | (reinterpret_cast<const int8 *>(code)[1] << 8);
	code += 2;

	combiner::Output(sample, ReadLittleEndianS16(&channel[0]), audio);
	channel++;
	audio += 2;
	sampleCount--;

	while (sampleCount > 0)
	{
		int32 k = *code++;
		int32 level = (k >> 4) & 0x07;
		int32 count = (k & 0x0F) + 1;
		if (k & 0x80)
		{
			k = *code++;
			count = ((count << 8) | k) - 239;
		}

		switch (level)
		{
			case 0:
			{
				k = count & ~3;
				for (machine a = 0; a < k; a += 4)
				{
					int32 value = *code++;
					int32 delta1 = value << 30 >> 30;
					int32 delta2 = value << 28 >> 30;
					int32 delta3 = value << 26 >> 30;
					int32 delta4 = value << 24 >> 30;

					sample = (Sample) (sample + delta1);
					combiner::Output(sample, ReadLittleEndianS16(&channel[0]), audio);
					sample = (Sample) (sample + delta2);
					combiner::Output(sample, ReadLittleEndianS16(&channel[1]), audio + 2);
					sample = (Sample) (sample + delta3);
					combiner::Output(sample, ReadLittleEndianS16(&channel[2]), audio + 4);
					sample = (Sample) (sample + delta4);
					combiner::Output(sample, ReadLittleEndianS16(&channel[3]), audio + 6);
					channel += 4;
					audio += 8;
				}

				if (count & 3)
				{
					int32 value = *code++;
					for (machine a = k; a < count; a++)
					{
						int32 delta = value << 30 >> 30;
						value >>= 2;

						sample = (Sample) (sample + delta);
						combiner::Output(sample, ReadLittleEndianS16(&channel[0]), audio);
						channel++;
						audio += 2;
					}
				}

				break;
			}

			case 1:
			{
				k = count & ~1;
				for (machine a = 0; a < k; a += 2)
				{
					int32 value = *code++;
					int32 delta1 = value << 28 >> 28;
					int32 delta2 = value << 24 >> 28;

					sample = (Sample) (sample + delta1);
					combiner::Output(sample, ReadLittleEndianS16(&channel[0]), audio);
					sample = (Sample) (sample + delta2);
					combiner::Output(sample, ReadLittleEndianS16(&channel[1]), audio + 2);
					channel += 2;
					audio += 4;
				}

				if (count & 1)
				{
					int32 delta = *reinterpret_cast<const int8 *>(code++);
					sample = (Sample) (sample + delta);
					combiner::Output(sample, ReadLittleEndianS16(&channel[0]), audio);
					channel++;
					audio += 2;
				}

				break;
			}

			case 2:
			{
				k = count & ~3;
				for (machine a = 0; a < k; a += 4)
				{
					int32 value1 = code[0];
					int32 value2 = code[1];
					int32 value3 = code[2];
					code += 3;

					int32 delta1 = value1 << 24 >> 26;
					int32 delta2 = ((value1 << 30) | (value2 << 22)) >> 26;
					int32 delta3 = ((value2 << 28) | (value3 << 20)) >> 26;
					int32 delta4 = value3 << 26 >> 26;

					sample = (Sample) (sample + delta1);
					combiner::Output(sample, ReadLittleEndianS16(&channel[0]), audio);
					sample = (Sample) (sample + delta2);
					combiner::Output(sample, ReadLittleEndianS16(&channel[1]), audio + 2);
					sample = (Sample) (sample + delta3);
					combiner::Output(sample, ReadLittleEndianS16(&channel[2]), audio + 4);
					sample = (Sample) (sample + delta4);
					combiner::Output(sample, ReadLittleEndianS16(&channel[3]), audio + 6);
					channel += 4;
					audio += 8;
				}

				for (machine a = k; a < count; a++)
				{
					int32 delta = *reinterpret_cast<const int8 *>(code++);
					sample = (Sample) (sample + delta);
					combiner::Output(sample, ReadLittleEndianS16(&channel[0]), audio);
					channel++;
					audio += 2;
				}

				break;
			}

			case 3:
			{
				for (machine a = 0; a < count; a++)
				{
					int32 delta = reinterpret_cast<const int8 *>(code)[a];
					sample = (Sample) (sample + delta);
					combiner::Output(sample, ReadLittleEndianS16(&channel[0]), audio);
					channel++;
					audio += 2;
				}

				code += count;
				break;
			}

			case 4:
			{
				k = count & ~1;
				for (machine a = 0; a < k; a += 2)
				{
					int32 value1 = code[0];
					int32 value2 = code[1];
					int32 value3 = code[2];
					code += 3;

					int32 delta1 = value1 | ((value3 << 28 >> 20) & ~0xFF);
					int32 delta2 = value2 | ((value3 << 24 >> 20) & ~0xFF);

					sample = (Sample) (sample + delta1);
					combiner::Output(sample, ReadLittleEndianS16(&channel[0]), audio);
					sample = (Sample) (sample + delta2);
					combiner::Output(sample, ReadLittleEndianS16(&channel[1]), audio + 2);
					channel += 2;
					audio += 4;
				}

				if (count & 1)
				{
					sample = (Sample) (code[0] | (reinterpret_cast<const int8 *>(code)[1] << 8));
					combiner::Output(sample, ReadLittleEndianS16(&channel[0]), audio);
					channel++;
					audio += 2;
					code += 2;
				}

				break;
			}

			default:
			{
				for (machine a = 0; a < count; a++)
				{
					sample = (Sample) (code[0] | (reinterpret_cast<const int8 *>(code)[1] << 8));
					combiner::Output(sample, ReadLittleEndianS16(&channel[0]), audio);
					channel++;
					audio += 2;
					code += 2;
				}

				break;
			}
		}

		sampleCount -= count;
	}
}

void AudioDecompressor::DecompressBlock(Sample *audio, int32 frameCount)
{
	const AudioChannelData *channelData = audioBlockHeader->audioChannelData;

	if (channelCount == 1)
	{
		const unsigned_int8 *code = channelData[0].GetAudioCode();
		const unsigned_int8 *data = audioBlockData;

		if (channelData[0].audioChannelFlags == kAudioCompressionGeneral)
		{
			Comp::DecompressData(code, channelData[0].audioCodeSize, const_cast<unsigned_int8 *>(data));
		}
		else
		{
			data = code;
		}

		DecompressAudioMono<1>(data, frameCount, audio);
	}
	else
	{
		const unsigned_int8 *code1 = channelData[0].GetAudioCode();
		const unsigned_int8 *code2 = channelData[1].GetAudioCode();

		const unsigned_int8 *data1 = audioBlockData;
		const unsigned_int8 *data2 = data1 + channelData[0].audioDataSize;

		if (channelData[0].audioChannelFlags == kAudioCompressionGeneral)
		{
			Comp::DecompressData(code1, channelData[0].audioCodeSize, const_cast<unsigned_int8 *>(data1));
		}
		else
		{
			data1 = code1;
		}

		if (channelData[1].audioChannelFlags == kAudioCompressionGeneral)
		{
			Comp::DecompressData(code2, channelData[1].audioCodeSize, const_cast<unsigned_int8 *>(data2));
		}
		else
		{
			data2 = code2;
		}

		unsigned_int32 flags = audioBlockHeader->audioBlockFlags;
		if (flags == kAudioIndependent)
		{
			DecompressAudioMono<2>(data1, frameCount, audio);
			DecompressAudioMono<2>(data2, frameCount, audio + 1);
		}
		else
		{
			static void (*const decompressor[kAudioChannelDependencyCount - 1])(const unsigned_int8 *, int32, const Sample *, Sample *) =
			{
				&DecompressAudioStereo<AudioLeftDifferenceCombiner>, &DecompressAudioStereo<AudioRightDifferenceCombiner>, &DecompressAudioStereo<AudioAverageDifferenceCombiner>
			};

			DecompressAudioMono<1>(data1, frameCount, audioSampleBuffer);
			(*decompressor[flags - 1])(data2, frameCount, audioSampleBuffer, audio);
		}
	}
}


SoundResource::SoundResource(const char *name, ResourceCatalog *catalog) : Resource<SoundResource>(name, catalog)
{
}

SoundResource::~SoundResource()
{
}

void SoundResource::Preprocess(void)
{
	char *chunkData = static_cast<char *>(GetData());

	unsigned_int32 position = 12;
	unsigned_int32 resourceSize = GetSize() - sizeof(RiffChunkHeader);
	while (position < resourceSize)
	{
		RiffChunkHeader *chunkHeader = reinterpret_cast<RiffChunkHeader *>(&chunkData[position]);
		position += sizeof(ChunkHeader);

		if (chunkHeader->type == ' tmf')
		{
			WaveHeader *header = reinterpret_cast<WaveHeader *>(chunkHeader + 1);
			waveHeader = header;
		}
		else if (chunkHeader->type == 'atad')
		{
			sampleData = reinterpret_cast<Sample *>(chunkHeader + 1);
			sampleCount = chunkHeader->size / sizeof(Sample);
		}

		position += (chunkHeader->size + 1) & ~1;
	}
}

ResourceResult SoundResource::LoadWaveHeader(ResourceLoader *loader, WaveHeader *header) const
{
	unsigned_int32 size = loader->GetDataSize();

	unsigned_int32 position = 12;
	while (position < size)
	{
		RiffChunkHeader		chunkHeader;

		ResourceResult result = loader->Read(&chunkHeader, position, sizeof(RiffChunkHeader));
		if (result != kResourceOkay)
		{
			return (result);
		}

		position += sizeof(RiffChunkHeader);

		if (chunkHeader.type == ' tmf')
		{
			result = loader->Read(header, position, sizeof(WaveHeader));
			if (result != kResourceOkay)
			{
				return (result);
			}

			break;
		}

		position += (chunkHeader.size + 1) & ~1;
	}

	return (kResourceOkay);
}

bool SoundResource::DetermineStreaming(const char *name)
{
	bool streaming = false;

	SoundResource *resource = Get(name, kResourceDeferLoad);
	if (resource)
	{
		ResourceLoader		loader;
		WaveHeader			header;

		if (resource->OpenLoader(&loader) == kResourceOkay)
		{
			if (resource->LoadWaveHeader(&loader, &header) == kResourceOkay)
			{
				streaming = (header.format == WAVE_FORMAT_ADPCM);
			}
		}

		resource->Release();
	}

	return (streaming);
}


SoundLoader::SoundLoader()
{
	soundResource = nullptr;
}

SoundLoader::~SoundLoader()
{
	if (soundResource)
	{
		soundResource->CloseLoader(this);
		soundResource->Release();
	}
}

SoundResult SoundLoader::Open(const char *name)
{
	soundResource = SoundResource::Get(name, kResourceDeferLoad);
	if (!soundResource)
	{
		return (kSoundLoadFailed);
	}

	if (soundResource->OpenLoader(this) != kResourceOkay)
	{
		soundResource->Release();
		soundResource = nullptr;
		return (kSoundLoadFailed);
	}

	frameCount = 0;
	waveDataSize = 0;

	bool adpcmFlag = false;

	unsigned_int32 position = 12;
	unsigned_int32 dataSize = GetDataSize();
	while (position < dataSize)
	{
		RiffChunkHeader		chunkHeader;

		Read(&chunkHeader, position, sizeof(RiffChunkHeader));
		position += sizeof(RiffChunkHeader);

		unsigned_int32 type = chunkHeader.type;
		if (type == ' tmf')
		{
			adpcmFlag = true;
			Read(&adpcmHeader, position, Min(chunkHeader.size, sizeof(ADPCMWaveHeader)));

			if (adpcmHeader.format != WAVE_FORMAT_ADPCM)
			{
				return (kSoundFormatInvalid);
			}
		}
		else if (type == 'tcaf')
		{
			Read(&frameCount, position, 4);
		}
		else if (type == 'atad')
		{
			startPosition = position;
			waveDataSize = chunkHeader.size;
		}

		position += (chunkHeader.size + 1) & ~1;
	}

	if (adpcmFlag)
	{
		int32 blockSize = (((adpcmHeader.blockFrameCount - 2) >> 1) + 7) * adpcmHeader.numChannels;
		int32 count = waveDataSize / blockSize * adpcmHeader.blockFrameCount;

		if (frameCount == 0)
		{
			frameCount = count;
		}
		else
		{
			frameCount = Min(frameCount, count);
		}
	}

	return (kSoundOkay);
}


SoundStreamer::SoundStreamer()
{
	streamerState = 0;
	workBuffer = nullptr;
}

SoundStreamer::~SoundStreamer()
{
	delete[] workBuffer;
}

void SoundStreamer::AllocateStreamMemory(unsigned_int32 workSize, unsigned_int32 streamSize)
{
	delete[] workBuffer;

	workBufferSize = workSize;
	streamBufferSize = streamSize;

	workSize = (workSize + 15) & ~15;
	streamSize = sizeof(StreamBufferHeader) + ((streamSize + 3) & ~3);

	workBuffer = new char[workSize + streamSize * 2];
	streamBuffer[0] = reinterpret_cast<StreamBufferHeader *>(workBuffer + workSize);
	streamBuffer[1] = reinterpret_cast<StreamBufferHeader *>(workBuffer + workSize + streamSize);
}

void SoundStreamer::ReleaseStreamMemory(void)
{
	delete[] workBuffer;
	workBuffer = nullptr;
}

int32 SoundStreamer::GetStreamFrameCount(void)
{
	return (0);
}

SoundResult SoundStreamer::StartStreamComponent(int32 index)
{
	return (StartStream());
}


WaveStreamer::WaveStreamer()
{
	currentSoundLoader = nullptr;
}

WaveStreamer::~WaveStreamer()
{
}

int32 WaveStreamer::DecompressMonoBlock(const char *input, Sample *output, int32 frameCount, const PredictorCoefficient *coefficient)
{
	int32 predictor = input[0];
	int32 c1 = coefficient[predictor].c1;
	int32 c2 = coefficient[predictor].c2;

	int32 delta = (input[2] << 8) | reinterpret_cast<const unsigned_int8 *>(input)[1];
	int32 sample1 = (input[4] << 8) | reinterpret_cast<const unsigned_int8 *>(input)[3];
	int32 sample2 = (input[6] << 8) | reinterpret_cast<const unsigned_int8 *>(input)[5];

	WriteLittleEndianS16(output, (Sample) sample2);
	WriteLittleEndianS16(++output, (Sample) sample1);

	input += 7;

	for (machine a = frameCount - 2; a > 0; a--)
	{
		int32 byte = *input++;
		int32 code = byte >> 4;

		int32 prediction = (sample1 * c1 + sample2 * c2) >> 8;
		sample2 = sample1;
		sample1 = Min(Max(code * delta + prediction, -32768), 32767);
		WriteLittleEndianS16(++output, (Sample) sample1);

		delta = Max((adpcmTable[code & 0x0F] * delta) >> 8, 16);

		if (--a <= 0)
		{
			break;
		}

		code = byte << 28 >> 28;

		prediction = (sample1 * c1 + sample2 * c2) >> 8;
		sample2 = sample1;
		sample1 = Min(Max(code * delta + prediction, -32768), 32767);
		WriteLittleEndianS16(++output, (Sample) sample1);

		delta = Max((adpcmTable[code & 0x0F] * delta) >> 8, 16);
	}

	return (frameCount);
}

int32 WaveStreamer::DecompressStereoBlock(const char *input, Sample *output, int32 frameCount, const PredictorCoefficient *coefficient)
{
	int32 predictorLeft = input[0];
	int32 predictorRight = input[1];
	int32 c1Left = coefficient[predictorLeft].c1;
	int32 c2Left = coefficient[predictorLeft].c2;
	int32 c1Right = coefficient[predictorRight].c1;
	int32 c2Right = coefficient[predictorRight].c2;

	int32 deltaLeft = (input[3] << 8) | reinterpret_cast<const unsigned_int8 *>(input)[2];
	int32 deltaRight = (input[5] << 8) | reinterpret_cast<const unsigned_int8 *>(input)[4];
	int32 sample1Left = (input[7] << 8) | reinterpret_cast<const unsigned_int8 *>(input)[6];
	int32 sample1Right = (input[9] << 8) | reinterpret_cast<const unsigned_int8 *>(input)[8];
	int32 sample2Left = (input[11] << 8) | reinterpret_cast<const unsigned_int8 *>(input)[10];
	int32 sample2Right = (input[13] << 8) | reinterpret_cast<const unsigned_int8 *>(input)[12];

	WriteLittleEndianS16(output, (Sample) sample2Left);
	WriteLittleEndianS16(++output, (Sample) sample2Right);
	WriteLittleEndianS16(++output, (Sample) sample1Left);
	WriteLittleEndianS16(++output, (Sample) sample1Right);

	input += 14;

	for (machine a = frameCount - 2; a > 0; a--)
	{
		int32 byte = *input++;
		int32 codeLeft = byte >> 4;
		int32 codeRight = byte << 28 >> 28;

		int32 prediction = (sample1Left * c1Left + sample2Left * c2Left) >> 8;
		sample2Left = sample1Left;
		sample1Left = Min(Max(codeLeft * deltaLeft + prediction, -32768), 32767);
		WriteLittleEndianS16(++output, (Sample) sample1Left);

		prediction = (sample1Right * c1Right + sample2Right * c2Right) >> 8;
		sample2Right = sample1Right;
		sample1Right = Min(Max(codeRight * deltaRight + prediction, -32768), 32767);
		WriteLittleEndianS16(++output, (Sample) sample1Right);

		deltaLeft = Max((adpcmTable[codeLeft & 0x0F] * deltaLeft) >> 8, 16);
		deltaRight = Max((adpcmTable[codeRight & 0x0F] * deltaRight) >> 8, 16);
	}

	return (frameCount);
}

SoundResult WaveStreamer::AddComponent(const char *name)
{
	SoundLoader *soundLoader = new SoundLoader;
	SoundResult result = soundLoader->Open(name);
	if (result != kSoundOkay)
	{
		delete soundLoader;
		return (result);
	}

	soundLoaderList.Append(soundLoader);

	if (!soundLoader->Previous())
	{
		const ADPCMWaveHeader *header = soundLoader->GetADPCMWaveHeader();

		int32 channelCount = header->numChannels;
		SetStreamChannelCount(channelCount);
		SetStreamSampleRate(header->sampleRate);

		blockFrameCount = header->blockFrameCount;
		compressedBlockSize = (((blockFrameCount - 2) >> 1) + 7) * channelCount;
		decompressedBlockSize = blockFrameCount * channelCount * sizeof(Sample);
		bufferBlockCount = kStreamBufferSize / decompressedBlockSize;
		decompressor = (channelCount == 1) ? &DecompressMonoBlock : &DecompressStereoBlock;

		AllocateStreamMemory(compressedBlockSize * bufferBlockCount, decompressedBlockSize * bufferBlockCount);
	}

	return (kSoundOkay);
}

int32 WaveStreamer::GetStreamFrameCount(void)
{
	int32 count = 0;

	SoundLoader *loader = soundLoaderList.First();
	while (loader)
	{
		count += loader->GetFrameCount();
		loader = loader->Next();
	}

	return (count);
}

SoundResult WaveStreamer::StartStream(void)
{
	currentSoundLoader = soundLoaderList.First();
	if (currentSoundLoader)
	{
		streamPosition = currentSoundLoader->GetStartPosition();
		frameNumber = 0;
		return (kSoundOkay);
	}

	return (kSoundPlayFailed);
}

SoundResult WaveStreamer::StartStreamComponent(int32 index)
{
	currentSoundLoader = soundLoaderList[index];
	if (currentSoundLoader)
	{
		streamPosition = currentSoundLoader->GetStartPosition();
		frameNumber = 0;
		return (kSoundOkay);
	}

	return (kSoundPlayFailed);
}

bool WaveStreamer::FillBuffer(unsigned_int32 bufferSize, Sample *buffer, int32 *count)
{
	bool result = true;
	int32 totalCount = 0;

	unsigned_int32 outputPosition = 0;
	for (;;)
	{
		int32 frameCount = currentSoundLoader->GetFrameCount();
		int32 blockCount = bufferSize / decompressedBlockSize;

		char *workBuffer = GetWorkBuffer();
		unsigned_int32 remainingSize = currentSoundLoader->GetWaveDataSize() + currentSoundLoader->GetStartPosition() - streamPosition;
		unsigned_int32 size = Min(compressedBlockSize * blockCount, remainingSize);
		currentSoundLoader->Read(workBuffer, streamPosition, size);
		streamPosition += size;

		const PredictorCoefficient *coefficient = currentSoundLoader->GetADPCMWaveHeader()->coefficient;

		unsigned_int32 inputPosition = 0;
		for (machine a = 0; a < blockCount; a++)
		{
			int32 decompressCount = (*decompressor)(&workBuffer[inputPosition], buffer + outputPosition / sizeof(Sample), Min(blockFrameCount, frameCount - frameNumber), coefficient);
			totalCount += decompressCount;
			outputPosition += decompressedBlockSize;

			if ((frameNumber += decompressCount) >= frameCount)
			{
				break;
			}

			inputPosition += compressedBlockSize;
		}

		if (frameNumber < frameCount)
		{
			break;
		}

		SoundLoader *loader = currentSoundLoader->Next();
		if (!loader)
		{
			result = false;
			break;
		}

		currentSoundLoader = loader;
		streamPosition = loader->GetStartPosition();
		bufferSize -= outputPosition;
		frameNumber = 0;
	}

	*count = totalCount;
	return (result);
}


SoundGroup::SoundGroup(SoundGroupType type, const char *name)
{
	soundGroupType = type;
	soundGroupName = name;
	soundGroupVolume = 1.0F;
	soundGroupFrequency = 1.0F;
}

SoundGroup::~SoundGroup()
{
}

void SoundGroup::SetVolume(float volume)
{
	soundGroupVolume = volume;

	Sound *sound = TheSoundMgr->loadedSoundList.First();
	while (sound)
	{
		sound->updateFlags |= kSoundUpdateVolume;
		sound = sound->Next();
	}
}

void SoundGroup::SetFrequency(float frequency)
{
	soundGroupFrequency = frequency;

	Sound *sound = TheSoundMgr->loadedSoundList.First();
	while (sound)
	{
		sound->updateFlags |= kSoundUpdateFrequency;
		sound = sound->Next();
	}
}


SoundRoom::SoundRoom(const Vector3D& size)
{
	tableIndex = -1;
	maxRoomMixCount = 0;

	roomSize = size;
	reflectionVolume = 0.0F;
	reflectionHFVolume = 1.0F;
	mediumHFAbsorption = 1.0F;
	reverbVolume = 0.0F;

	roomVolume[0] = 0.0F;
	roomVolume[1] = 0.0F;
}

SoundRoom::~SoundRoom()
{
}

void SoundRoom::Release(void)
{
	TheSoundMgr->releasedRoomList.Append(this);
}

void SoundRoom::SetReverbDecayTime(float time)
{
	if (time != 0.0F)
	{
		float count = time / kMaxFeedbackTimeDelay;
		reverbVolume = Pow(kMixFractionReciprocal, 1.0F / count);
		maxRoomMixCount = Min((int32) PositiveCeil(count), kMaxFeedbackMixCount);
	}
	else
	{
		reverbVolume = 0.0F;
		maxRoomMixCount = 0;
	}
}

void SoundRoom::SetRoomPosition(const Point3D& position)
{
	roomPosition = position;

	Point3D p = TheSoundMgr->GetListenerTransformable()->GetInverseWorldTransform() * position;
	float inverseDistance = InverseMag(p);
	float m = Fmin(inverseDistance, 1.0F);

	float d = p.x * inverseDistance;
	if (d > 0.0F)
	{
		roomVolume[0] = m * (1.0F - d);
		roomVolume[1] = m;
	}
	else
	{
		roomVolume[0] = m;
		roomVolume[1] = m * (1.0F + d);
	}
}


Sound::Sound()
{
	soundResource = nullptr;
	soundStreamer = nullptr;

	soundFlags = kSoundPersistent;
	soundPriority = 0;

	tableIndex = -1;
	soundState = kSoundUnloaded;
	mainReleaseFlag = false;

	startTime = 0;
	loopCount = 0;
	loopIndex = 0;
	loopProc = nullptr;

	variationState = 0;
	volumeVariationParams.completionProc = nullptr;
	frequencyVariationParams.completionProc = nullptr;

	soundProperty[kSoundVolume] = 1.0F;
	soundProperty[kSoundDirectVolume] = 1.0F;
	soundProperty[kSoundDirectHFVolume] = 1.0F;
	soundProperty[kSoundReflectionVolume] = 1.0F;
	soundProperty[kSoundReflectionHFVolume] = 1.0F;
	soundProperty[kSoundOuterConeVolume] = 0.0F;
	soundProperty[kSoundOuterConeHFVolume] = 1.0F;
	soundProperty[kSoundMinAttenDistance] = 0.0F;
	soundProperty[kSoundMaxAttenDistance] = 16.0F;
	soundProperty[kSoundInnerConeCosine] = 1.0F;
	soundProperty[kSoundOuterConeCosine] = 0.0F;
	soundProperty[kSoundFrequency] = 1.0F;

	soundTransformable = nullptr;
	soundVelocity.Set(0.0F, 0.0F, 0.0F);
	soundPathCount = 0;

	soundGroup = TheSoundMgr->GetDefaultSoundGroup();
}

Sound::~Sound()
{
	if (soundResource)
	{
		soundResource->Release();
	}

	if (!(soundFlags & kSoundStreamExternal))
	{
		delete soundStreamer;
	}
}

void Sound::Release(void)
{
	if (tableIndex < 0)
	{
		delete this;
	}
	else
	{
		soundState = kSoundReleased;
		streamReleaseFlag = (soundStreamer == nullptr);
		mixerReleaseFlag = false;

		Thread::Fence();
		mainReleaseFlag = true;
	}
}

void Sound::SetSoundRoom(SoundRoom *room)
{
	if (soundRoom != room)
	{
		soundRoom = room;
		updateFlags |= kSoundUpdateReflections;
	}
}

SoundResult Sound::Load(const char *name)
{
	if (!(soundFlags & kSoundStreamExternal))
	{
		delete soundStreamer;
	}

	soundStreamer = nullptr;

	soundResource = SoundResource::Get(name);
	if (!soundResource)
	{
		return (kSoundLoadFailed);
	}

	const WaveHeader *header = soundResource->GetWaveHeader();
	if ((header->format != WAVE_FORMAT_PCM) || (header->bitsPerSample != 16))
	{
		soundResource->Release();
		soundResource = nullptr;
		return (kSoundFormatInvalid);
	}

	channelCount = header->numChannels;
	sampleRate = header->sampleRate;
	sampleFrequency = sampleRate / (float) kSoundOutputSampleRate;

	soundSampleData = soundResource->GetSampleData();
	soundFrameCount = soundResource->GetSampleCount() / channelCount;

	if (soundFrameCount >= 0x00200000)
	{
		soundResource->Release();
		soundResource = nullptr;
		return (kSoundTooLarge);
	}

	soundState = kSoundStopped;
	soundFlags &= ~kSoundPersistent;

	TheSoundMgr->loadedSoundList.Append(this);
	return (kSoundOkay);
}

SoundResult Sound::Stream(SoundStreamer *streamer, bool external)
{
	unsigned_int32 flags = soundFlags;
	if (!(flags & kSoundStreamExternal))
	{
		delete soundStreamer;
	}

	soundStreamer = streamer;

	if (external)
	{
		soundFlags = flags | kSoundStreamExternal;
	}
	else
	{
		soundFlags = flags & ~kSoundStreamExternal;
	}

	soundState = kSoundStopped;
	soundFlags |= kSoundPersistent;

	TheSoundMgr->loadedSoundList.Append(this);
	return (kSoundOkay);
}

void Sound::FillStreamBuffer(SoundStreamer *streamer, StreamBufferHeader *buffer)
{
	buffer->frameCount = 0;
	buffer->loopFrame = 0x7FFFFFFF;
	buffer->finalFlag = false;

	unsigned_int32 bufferSize = streamer->GetStreamBufferSize();
	Sample *sampleData = buffer->GetSampleData();

	for (;;)
	{
		int32	frameCount;

		bool result = soundStreamer->FillBuffer(bufferSize, sampleData, &frameCount);
		buffer->frameCount += frameCount;

		if (!result)
		{
			if (loopCount > 0)
			{
				loopCount--;
			}

			if (loopCount != 0)
			{
				if (soundStreamer->StartStreamComponent(loopIndex) != kSoundOkay)
				{
					soundStreamer->StartStreamComponent(0);
				}

				buffer->loopFrame = buffer->frameCount;
				bufferSize -= frameCount * channelCount * sizeof(Sample);
				sampleData += frameCount * channelCount;

				if (bufferSize != 0)
				{
					continue;
				}
			}
			else
			{
				buffer->finalFlag = true;
			}
		}

		break;
	}

	Thread::Fence();
	buffer->readyFlag = true;
}

SoundResult Sound::Play(void)
{
	SoundResult result = kSoundOkay;

	if (soundState < kSoundPlaying)
	{
		if (soundState != kSoundDelaying)
		{
			variationState = 0;
		}

		mixFlag = false;
		loopFlag = false;
		pauseCount = 0;
		playFrame = 0;

		if (soundStreamer)
		{
			result = soundStreamer->StartStream();
			if (result != kSoundOkay)
			{
				return (result);
			}

			channelCount = soundStreamer->GetStreamChannelCount();
			sampleRate = soundStreamer->GetStreamSampleRate();
			sampleFrequency = sampleRate / (float) kSoundOutputSampleRate;

			playBuffer = 0;
			StreamBufferHeader *bufferHeader1 = soundStreamer->GetStreamBuffer(0);
			StreamBufferHeader *bufferHeader2 = soundStreamer->GetStreamBuffer(1);
			bufferHeader1->readyFlag = false;
			bufferHeader1->finalFlag = false;
			bufferHeader2->readyFlag = false;
			bufferHeader2->finalFlag = false;

			if (channelCount == 1)
			{
				soundMixData.sampleTableIndex = 0;
				float sample = (float) ReadLittleEndianS16(soundStreamer->GetStreamBuffer(0)->GetSampleData());
				soundMixData.sampleTableSum = sample * (float) kSampleHistoryCount;

				for (machine a = 0; a < kSampleHistoryCount; a++)
				{
					soundMixData.sampleTable[a] = sample;
				}
			}
		}
		else
		{
			if (startTime != 0)
			{
				playFrame = Min(startTime * sampleRate / 1000, soundFrameCount - 1);
			}

			if (channelCount == 1)
			{
				soundMixData.sampleTableIndex = 0;
				float sample = (float) ReadLittleEndianS16(&soundSampleData[playFrame]);
				soundMixData.sampleTableSum = sample * (float) kSampleHistoryCount;

				for (machine a = 0; a < kSampleHistoryCount; a++)
				{
					soundMixData.sampleTable[a] = sample;
				}

				if (((soundFlags & (kSoundSpatialized | kSoundDistanceDelay)) == (kSoundSpatialized | kSoundDistanceDelay)) && (playFrame == 0))
				{
					float distance = Magnitude(soundTransformable->GetWorldPosition() - TheSoundMgr->GetListenerTransformable()->GetWorldPosition());
					playFrame = (int32) Fmax(distance / TheSoundMgr->GetGlobalSoundSpeed() * sampleFrequency * -(float) kSoundOutputSampleRate, kMinDistanceDelayPlayFrame);
				}
			}
		}

		updateFlags = 0;

		UpdateVolume();
		soundMixData.directVolumeCurrent[0] = soundMixData.directVolumeFinal[0];
		soundMixData.directVolumeCurrent[1] = soundMixData.directVolumeFinal[1];
		soundMixData.reflectionVolumeCurrent = soundMixData.reflectionVolumeFinal;

		UpdateFrequency();
		soundMixData.frequencyCurrent = soundMixData.frequencyFinal;

		UpdateReflections();

		if (tableIndex < 0)
		{
			int32 index = TheSoundMgr->AddSound(this);
			if (index >= 0)
			{
				tableIndex = index;
				soundState = kSoundPlaying;
			}
			else
			{
				soundState = kSoundCompleted;
				waitMixStamp = TheSoundMgr->soundMixStamp;

				result = kSoundPlayFailed;
			}
		}
		else
		{
			soundState = kSoundPlaying;
		}
	}

	return (result);
}

void Sound::Stop(void)
{
	variationState = 0;

	if (soundFlags & kSoundPersistent)
	{
		if (tableIndex < 0)
		{
			soundState = kSoundStopped;
		}
		else
		{
			soundState = kSoundStopping;
			waitMixStamp = TheSoundMgr->soundMixStamp + 1;
		}
	}
	else
	{
		Release();
	}
}

void Sound::Delay(int32 time)
{
	delayTime = time;
	playFrame = 0;
	variationState = 0;
	soundState = kSoundDelaying;
}

void Sound::Pause(void)
{
	if (soundState == kSoundPlaying)
	{
		if (++pauseCount == 1)
		{
			soundState = kSoundPaused;
		}
	}
	else if (soundState == kSoundDelaying)
	{
		if (++pauseCount == 1)
		{
			soundState = kSoundDelayPaused;
		}
	}
}

void Sound::Resume(void)
{
	if (soundState == kSoundPaused)
	{
		if (--pauseCount == 0)
		{
			soundState = kSoundPlaying;
		}
	}
	else if (soundState == kSoundDelayPaused)
	{
		soundState = kSoundDelaying;
	}
}

void Sound::VaryVolume(float volume, int32 time, bool stop)
{
	if ((soundState >= kSoundDelaying) && (soundState <= kSoundPlaying))
	{
		if (time <= 0)
		{
			SetSoundProperty(kSoundVolume, volume);

			if (stop)
			{
				Stop();
			}
		}
		else
		{
			unsigned_int32 state = kSoundVaryingVolume;
			if (stop)
			{
				state |= kSoundVaryingVolumeStop;
			}

			variationState = (variationState & ~kSoundVaryingVolumeMask) | state;

			volumeVariationParams.targetValue = volume;
			volumeVariationParams.deltaValue = (volume - soundProperty[kSoundVolume]) / (float) time;
		}
	}
}

void Sound::VaryFrequency(float frequency, int32 time, bool stop)
{
	if ((soundState >= kSoundDelaying) && (soundState <= kSoundPlaying))
	{
		if (time <= 0)
		{
			SetSoundProperty(kSoundFrequency, frequency);

			if (stop)
			{
				Stop();
			}
		}
		else
		{
			unsigned_int32 state = kSoundVaryingFrequency;
			if (stop)
			{
				state |= kSoundVaryingFrequencyStop;
			}

			variationState = (variationState & ~kSoundVaryingFrequencyMask) | state;

			frequencyVariationParams.targetValue = frequency;
			frequencyVariationParams.deltaValue = (frequency - soundProperty[kSoundFrequency]) / (float) time;
		}
	}
}

inline float Sound::CalculateVolume(float distance) const
{
	float minAttenDistance = soundProperty[kSoundMinAttenDistance];
	float maxAttenDistance = soundProperty[kSoundMaxAttenDistance];
	float d = FmaxZero(Fmin(distance, maxAttenDistance) - minAttenDistance) / (maxAttenDistance - minAttenDistance);

	d -= 1.0F;
	d *= d;
	return (d * d);
}

void Sound::UpdateVolume(void)
{
	float masterVolume = soundProperty[kSoundVolume] * TheSoundMgr->GetMasterVolume();

	const SoundGroup *group = soundGroup;
	if (group)
	{
		masterVolume *= group->GetVolume();
	}

	if (soundFlags & kSoundSpatialized)
	{
		float	directHFVolume;
		float	speakerVolume[2];

		const SoundRoom *room = soundRoom;
		const Transformable *listenerTransformable = TheSoundMgr->GetListenerTransformable();

		int32 pathCount = soundPathCount;
		if (pathCount > 0)
		{
			directHFVolume = (room) ? 0.0F : 1.0F;
			speakerVolume[0] = 0.0F;
			speakerVolume[1] = 0.0F;

			machine pathIndex = 0;
			do
			{
				const SoundPathData *data = &soundPathData[pathIndex];

				Point3D position = listenerTransformable->GetInverseWorldTransform() * *data->soundPosition;
				float squaredDistance = SquaredMag(position);
				float inverseDistance = InverseSqrt(squaredDistance);

				float distance = squaredDistance * inverseDistance + data->soundPathLength;
				float volume = CalculateVolume(distance);

				float d = position.x * inverseDistance;
				if (d > 0.0F)
				{
					speakerVolume[0] = Fmax(speakerVolume[0], FmaxZero(1.0F - d) * volume);
					speakerVolume[1] = Fmax(speakerVolume[1], volume);
				}
				else
				{
					speakerVolume[0] = Fmax(speakerVolume[0], volume);
					speakerVolume[1] = Fmax(speakerVolume[1], FmaxZero(1.0F + d) * volume);
				}

				if (room)
				{
					directHFVolume = Fmax(directHFVolume, Fmin(Pow(room->GetMediumHFAbsorption(), distance), 1.0F));
				}
			} while (++pathIndex < pathCount);
		}
		else
		{
			Point3D position = listenerTransformable->GetInverseWorldTransform() * soundTransformable->GetWorldPosition();

			float r2 = Fmax(SquaredMag(position), K::min_float);
			float inverseDistance = InverseSqrt(r2);
			float distance = r2 * inverseDistance;
			float volume = CalculateVolume(distance);

			float d = position.x * inverseDistance;
			if (d > 0.0F)
			{
				speakerVolume[0] = FmaxZero(1.0F - d) * volume;
				speakerVolume[1] = volume;
			}
			else
			{
				speakerVolume[0] = volume;
				speakerVolume[1] = FmaxZero(1.0F + d) * volume;
			}

			directHFVolume = (room) ? Fmin(Pow(room->GetMediumHFAbsorption(), distance), 1.0F) : 1.0F;
		}

		float directVolume = masterVolume * soundProperty[kSoundDirectVolume];

		if (soundFlags & kSoundCones)
		{
			Point3D conePosition = soundTransformable->GetInverseWorldTransform() * listenerTransformable->GetWorldPosition();
			float cosine = conePosition.z * InverseMag(conePosition);

			float inner = soundProperty[kSoundInnerConeCosine];
			if (cosine < inner)
			{
				float outer = soundProperty[kSoundOuterConeCosine];
				if (cosine < outer)
				{
					directVolume *= soundProperty[kSoundOuterConeVolume];
					directHFVolume *= soundProperty[kSoundOuterConeHFVolume];
				}
				else
				{
					float t = (inner - cosine) / (inner - outer);
					directVolume *= (soundProperty[kSoundOuterConeVolume] - 1.0F) * t + 1.0F;
					directHFVolume *= (soundProperty[kSoundOuterConeHFVolume] - 1.0F) * t + 1.0F;
				}
			}
		}

		soundMixData.directVolumeFinal[0] = directVolume * speakerVolume[0];
		soundMixData.directVolumeFinal[1] = directVolume * speakerVolume[1];
		soundMixData.directHFVolume = directHFVolume * soundProperty[kSoundDirectHFVolume];

		soundMixData.reflectionVolumeFinal = masterVolume * soundProperty[kSoundReflectionVolume];
		soundMixData.reflectionHFVolume = soundProperty[kSoundReflectionHFVolume];
	}
	else
	{
		soundMixData.directVolumeFinal[0] = masterVolume;
		soundMixData.directVolumeFinal[1] = masterVolume;
	}
}

void Sound::UpdateFrequency(void)
{
	float frequency = sampleFrequency * soundProperty[kSoundFrequency];

	const SoundGroup *group = soundGroup;
	if (group)
	{
		frequency *= group->GetFrequency();
	}

	if ((soundFlags & (kSoundSpatialized | kSoundDopplerShift)) == (kSoundSpatialized | kSoundDopplerShift))
	{
		const Transformable *listenerTransformable = TheSoundMgr->GetListenerTransformable();
		if ((listenerTransformable) && (soundTransformable))
		{
			Vector3D dp = soundTransformable->GetWorldPosition() - listenerTransformable->GetWorldPosition();
			dp.Normalize();

			float c = TheSoundMgr->GetGlobalSoundSpeed();
			float doppler = (c + dp * TheSoundMgr->GetListenerVelocity()) / (c + dp * soundVelocity);
			frequency *= Clamp(doppler, 0.25F, 4.0F);
		}
	}

	soundMixData.frequencyFinal = frequency;
}

void Sound::UpdateReflections(void)
{
	soundMixData.reflectionData[0].soundRoom = nullptr;
	soundMixData.reflectionData[1].soundRoom = nullptr;

	SoundRoom *primaryRoom = soundRoom;
	if (primaryRoom)
	{
		static const float reflectionDistance[kSoundReflectionCount] =
		{
			0.25F, 0.1625F, 0.2075F, 0.3075F, 0.3925F, 0.4725F
		};

		soundMixData.reflectionData[0].soundRoom = primaryRoom;

		const Vector3D& size1 = primaryRoom->GetRoomSize();
		float x2 = size1.x * size1.x;
		float y2 = size1.y * size1.y;
		float z2 = size1.z * size1.z;
		float d = Sqrt(x2 + y2 + z2) * 1.5F;

		float unitDistanceFrameCount = TheSoundMgr->unitDistanceFrameCount;
		float rv = primaryRoom->GetReflectionVolume();
		float rhfv = primaryRoom->GetReflectionHFVolume();

		SoundMixData::ReflectionData *data = &soundMixData.reflectionData[0];
		for (machine a = 0; a < kSoundReflectionCount; a++)
		{
			float distance = d * reflectionDistance[a];
			data->reflectionDelay[a] = Min((int32) (distance * unitDistanceFrameCount), kRingBufferFrameCount - 1);
			data->reflectionVolume[a] = CalculateVolume(distance) * rv;
			data->reflectionHFVolume[a] = rhfv;
		}

		SoundRoom *secondaryRoom = primaryRoom->GetOutputRoom();
		if (secondaryRoom)
		{
			soundMixData.reflectionData[1].soundRoom = secondaryRoom;

			const Vector3D& size2 = secondaryRoom->GetRoomSize();
			x2 = size2.x * size2.x;
			y2 = size2.y * size2.y;
			z2 = size2.z * size2.z;
			d = Sqrt(x2 + y2 + z2) * 0.5F;

			rv = secondaryRoom->GetReflectionVolume();
			rhfv = secondaryRoom->GetReflectionHFVolume();
			float primaryDistance = Magnitude(primaryRoom->GetRoomPosition() - soundTransformable->GetWorldPosition());

			data = &soundMixData.reflectionData[1];
			for (machine a = 0; a < kSoundReflectionCount; a++)
			{
				float distance = primaryDistance + d * reflectionDistance[a];
				data->reflectionDelay[a] = Min((int32) (distance * unitDistanceFrameCount), kRingBufferFrameCount - 1);
				data->reflectionVolume[a] = CalculateVolume(distance) * rv;
				data->reflectionHFVolume[a] = rhfv;
			}
		}
	}
}

int32 Sound::GetDuration(void) const
{
	if (soundStreamer)
	{
		int32 count = soundStreamer->GetStreamFrameCount();
		return ((int32) ((float) count * 1000.0F / (float) soundStreamer->GetStreamSampleRate()));
	}

	return ((int32) ((float) soundFrameCount * 1000.0F / (sampleFrequency * (float) kSoundOutputSampleRate)));
}


SoundMgr::SoundMgr(int) : soundReverbObserver(this, &SoundMgr::HandleSoundReverbEvent)
{
}

SoundMgr::~SoundMgr()
{
}

EngineResult SoundMgr::Construct(void)
{
	soundOptionFlags = kSoundReverb;

	masterVolume = 1.0F;
	SetGlobalSoundSpeed(343.0F);

	listenerTransformable = nullptr;
	listenerVelocity.Set(0.0F, 0.0F, 0.0F);
	listenerRoom = nullptr;

	soundMixStamp = 0;
	ringBufferSliceIndex = 0;

	char *bufferStorage = new char[kRingBufferFrameCount * sizeof(StereoMixFrame) + kMaxRoomCount * sizeof(RoomMixBuffer)];
	MemoryMgr::ClearMemory(bufferStorage, kRingBufferFrameCount * sizeof(StereoMixFrame));
	stereoRingBuffer = reinterpret_cast<StereoMixFrame *>(bufferStorage);

	RoomMixBuffer *buffer = reinterpret_cast<RoomMixBuffer *>(stereoRingBuffer + kRingBufferFrameCount);
	for (machine a = 0; a < kMaxRoomCount; a++)
	{
		roomMixBuffer[a] = &buffer[a];
		activeRoomTable[a] = nullptr;
	}

	for (machine a = 0; a < kMaxSoundCount; a++)
	{
		activeSoundTable[a] = nullptr;
	}

	TheEngine->InitVariable("soundReverb", "1", kVariablePermanent, &soundReverbObserver);

	#if C4RECORDABLE

		recordFlag = false;

	#endif

	#if C4XAUDIO

		WAVEFORMATEX	format;

		if (FAILED(XAudio2Create(&xaudioObject)))
		{
			delete[] bufferStorage;
			return (kSoundInitFailed);
		}

		if (FAILED(xaudioObject->CreateMasteringVoice(&masteringVoice, 2, kSoundOutputSampleRate)))
		{
			xaudioObject->Release();
			delete[] bufferStorage;
			return (kSoundInitFailed);
		}

		voiceCallback.soundMgr = this;

		format.wFormatTag = WAVE_FORMAT_PCM;
		format.nChannels = 2;
		format.nSamplesPerSec = kSoundOutputSampleRate;
		format.nAvgBytesPerSec = kSoundOutputSampleRate * 4;
		format.nBlockAlign = 4;
		format.wBitsPerSample = 16;
		format.cbSize = 0;

		if (FAILED(xaudioObject->CreateSourceVoice(&sourceVoice, &format, XAUDIO2_VOICE_NOPITCH, 1.0F, &voiceCallback)))
		{
			masteringVoice->DestroyVoice();
			xaudioObject->Release();
			delete[] bufferStorage;
			return (kSoundInitFailed);
		}

		playBuffer[0] = new OutputSample[kOutputBufferFrameCount * 4];
		playBuffer[1] = playBuffer[0] + kOutputBufferFrameCount * 2;
		MemoryMgr::ClearMemory(playBuffer[0], kOutputBufferFrameCount * 4 * sizeof(OutputSample));

		new(soundSignal) Signal(3);
		new(soundThread) Thread(&SoundThread, this, 0, soundSignal);
		soundThread->SetThreadPriority(kThreadPriorityCritical);

		sourceBuffer[0].Flags = 0;
		sourceBuffer[0].AudioBytes = kStereoOutputBufferSize;
		sourceBuffer[0].pAudioData = reinterpret_cast<BYTE *>(playBuffer[0]);
		sourceBuffer[0].PlayBegin = 0;
		sourceBuffer[0].PlayLength = 0;
		sourceBuffer[0].LoopBegin = XAUDIO2_NO_LOOP_REGION;
		sourceBuffer[0].LoopLength = 0;
		sourceBuffer[0].LoopCount = 0;
		sourceBuffer[0].pContext = &sourceBuffer[0];

		sourceBuffer[1].Flags = 0;
		sourceBuffer[1].AudioBytes = kStereoOutputBufferSize;
		sourceBuffer[1].pAudioData = reinterpret_cast<BYTE *>(playBuffer[1]);
		sourceBuffer[1].PlayBegin = 0;
		sourceBuffer[1].PlayLength = 0;
		sourceBuffer[1].LoopBegin = XAUDIO2_NO_LOOP_REGION;
		sourceBuffer[1].LoopLength = 0;
		sourceBuffer[1].LoopCount = 0;
		sourceBuffer[1].pContext = &sourceBuffer[1];

		sourceVoice->SubmitSourceBuffer(&sourceBuffer[0]);
		sourceVoice->SubmitSourceBuffer(&sourceBuffer[1]);
		sourceVoice->Start(0);

	#elif C4MACOS

		AudioComponentDescription		componentDescription;
		AudioStreamBasicDescription		streamFormat;
		AURenderCallbackStruct			callbackProperty;

		NewAUGraph(&audioGraph);

		componentDescription.componentType = kAudioUnitType_Output;
		componentDescription.componentSubType = kAudioUnitSubType_DefaultOutput;
		componentDescription.componentManufacturer = kAudioUnitManufacturer_Apple;
		componentDescription.componentFlags = 0;
		componentDescription.componentFlagsMask = 0;

		if (AUGraphAddNode(audioGraph, &componentDescription, &audioNode) != noErr)
		{
			DisposeAUGraph(audioGraph);
			delete[] bufferStorage;
			return (kSoundInitFailed);
		}

		if (AUGraphOpen(audioGraph) != noErr)
		{
			DisposeAUGraph(audioGraph);
			delete[] bufferStorage;
			return (kSoundInitFailed);
		}

		AUGraphNodeInfo(audioGraph, audioNode, nullptr, &audioUnit);

		streamFormat.mSampleRate = (float) kSoundOutputSampleRate;
		streamFormat.mFormatID = kAudioFormatLinearPCM;
		streamFormat.mFormatFlags = kAudioFormatFlagIsFloat;
		streamFormat.mBytesPerPacket = 8;
		streamFormat.mFramesPerPacket = 1;
		streamFormat.mBytesPerFrame = 8;
		streamFormat.mChannelsPerFrame = 2;
		streamFormat.mBitsPerChannel = 32;
		streamFormat.mReserved = 0;
		AudioUnitSetProperty(audioUnit, kAudioUnitProperty_StreamFormat, kAudioUnitScope_Input, 0, &streamFormat, sizeof(AudioStreamBasicDescription));

		callbackProperty.inputProc = &SoundCallback;
		callbackProperty.inputProcRefCon = this;
		AudioUnitSetProperty(audioUnit, kAudioUnitProperty_SetRenderCallback, kAudioUnitScope_Input, 0, &callbackProperty, sizeof(AURenderCallbackStruct));

		UInt32 frameCount = kOutputBufferFrameCount;
		AudioUnitSetProperty(audioUnit, kAudioUnitProperty_MaximumFramesPerSlice, kAudioUnitScope_Global, 0, &frameCount, 4);

		if (AUGraphInitialize(audioGraph) != noErr)
		{
			DisposeAUGraph(audioGraph);
			delete[] bufferStorage;
			return (kSoundInitFailed);
		}

		playFrameIndex = 0;
		playBuffer[0] = new OutputSample[kOutputBufferFrameCount * 4];
		playBuffer[1] = playBuffer[0] + kOutputBufferFrameCount * 2;
		MemoryMgr::ClearMemory(playBuffer[0], kOutputBufferFrameCount * 4 * sizeof(OutputSample));

		if ((AudioOutputUnitStart(audioUnit) != noErr) || (AUGraphStart(audioGraph) != noErr))
		{
			delete[] playBuffer[0];
			AUGraphUninitialize(audioGraph);
			DisposeAUGraph(audioGraph);
			delete[] bufferStorage;
			return (kSoundInitFailed);
		}

	#elif C4LINUX

		snd_pcm_hw_params_t		*hwparams;
		snd_pcm_sw_params_t		*swparams;
		snd_pcm_uframes_t		bufferSize;

		if (snd_pcm_open(&soundHandle, "default", SND_PCM_STREAM_PLAYBACK, 0) != 0)
		{
			return (kSoundInitFailed);
		}

		snd_pcm_hw_params_malloc(&hwparams);
		snd_pcm_hw_params_any(soundHandle, hwparams);

		snd_pcm_hw_params_set_access(soundHandle, hwparams, SND_PCM_ACCESS_RW_INTERLEAVED);
		snd_pcm_hw_params_set_format(soundHandle, hwparams, SND_PCM_FORMAT_S16_LE);
		snd_pcm_hw_params_set_rate(soundHandle, hwparams, kSoundOutputSampleRate, 0);
		snd_pcm_hw_params_set_channels(soundHandle, hwparams, 2);
		snd_pcm_hw_params_set_period_size(soundHandle, hwparams, kOutputBufferFrameCount, 0);

		int dir = -1;
		unsigned int bufferTime = (unsigned int) ((kOutputBufferFrameCount * 1000000ULL + (kSoundOutputSampleRate - 1)) / kSoundOutputSampleRate);
		snd_pcm_hw_params_set_buffer_time_near(soundHandle, hwparams, &bufferTime, &dir);

		int error = snd_pcm_hw_params(soundHandle, hwparams);
		snd_pcm_hw_params_get_buffer_size(hwparams, &bufferSize);
		snd_pcm_hw_params_free(hwparams);

		if (error < 0)
		{
			snd_pcm_close(soundHandle);
			delete[] bufferStorage;
			return (kSoundInitFailed);
		}

		snd_pcm_sw_params_malloc(&swparams);
		snd_pcm_sw_params_current(soundHandle, swparams);
		snd_pcm_sw_params_set_avail_min(soundHandle, swparams, bufferSize - kOutputBufferFrameCount);
		error = snd_pcm_sw_params(soundHandle, swparams);
		snd_pcm_sw_params_free(swparams);

		if (error < 0)
		{
			snd_pcm_close(soundHandle);
			delete[] bufferStorage;
			return (kSoundInitFailed);
		}

		playBuffer = new OutputSample[kOutputBufferFrameCount * 2];
		MemoryMgr::ClearMemory(playBuffer, kOutputBufferFrameCount * 2 * sizeof(OutputSample));

		soundExitFlag = false;
		new(soundThread) Thread(&SoundThread, this);
		soundThread->SetThreadPriority(kThreadPriorityCritical);

	#elif C4IOS //[ MOBILE

		// -- Mobile code hidden --

	#elif C4PS4 //[ PS4

		// -- PS4 code hidden --

	#elif C4PS3 //[ PS3

		// -- PS3 code hidden --

	#endif //]

	new(streamSignal) Signal(2);
	new(streamThread) Thread(&StreamThread, this, 0, streamSignal);

	return (kSoundOkay);
}

void SoundMgr::Destruct(void)
{
	StopRecording();

	streamThread->~Thread();
	streamSignal->~Signal();

	#if C4XAUDIO

		soundThread->~Thread();
		soundSignal->~Signal();

		sourceVoice->Stop(0);
		delete[] playBuffer[0];

		sourceVoice->DestroyVoice();
		masteringVoice->DestroyVoice();
		xaudioObject->Release();

	#elif C4MACOS

		AUGraphStop(audioGraph);
		AudioOutputUnitStop(audioUnit);

		delete[] playBuffer[0];

		AUGraphUninitialize(audioGraph);
		AUGraphClose(audioGraph);
		DisposeAUGraph(audioGraph);

	#elif C4LINUX

		soundExitFlag = true;
		soundThread->~Thread();

		snd_pcm_drop(soundHandle);
		delete[] playBuffer;

		snd_pcm_close(soundHandle);

	#elif C4IOS //[ MOBILE

		// -- Mobile code hidden --

	#elif C4PS4 //[ PS4

		// -- PS4 code hidden --

	#elif C4PS3 //[ PS3

		// -- PS3 code hidden --

	#endif //]

	delete[] reinterpret_cast<char *>(stereoRingBuffer);

	releasedRoomList.Purge();
	loadedSoundList.Purge();

	TheResourceMgr->ReleaseCache(SoundResource::GetDescriptor());
}

#if C4XAUDIO

	void SoundMgr::VoiceCallback::OnVoiceProcessingPassStart(UINT32)
	{
	}

	void SoundMgr::VoiceCallback::OnVoiceProcessingPassEnd(void)
	{
	}

	void SoundMgr::VoiceCallback::OnStreamEnd(void)
	{
	}

	void SoundMgr::VoiceCallback::OnBufferStart(void *context)
	{
	}

	void SoundMgr::VoiceCallback::OnBufferEnd(void *context)
	{
		soundMgr->soundSignal->Trigger((context == &soundMgr->sourceBuffer[0]) ? 1 : 2);
	}

	void SoundMgr::VoiceCallback::OnLoopEnd(void *context)
	{
	}

	void SoundMgr::VoiceCallback::OnVoiceError(void *context, HRESULT)
	{
	}

#endif

void SoundMgr::HandleSoundReverbEvent(Variable *variable)
{
	unsigned_int32 flags = soundOptionFlags;

	if (variable->GetIntegerValue() != 0)
	{
		flags |= kSoundOptionReverb;
	}
	else
	{
		flags &= ~kSoundOptionReverb;
	}

	soundOptionFlags = flags;
}

int32 SoundMgr::MixStereoSamples_Mono_Constant(SoundMixData *mixData, const Sample *input, int32 inputFrameCount, int32& inputOffset, int32 outputFrameCount, int32 outputOffset)
{
	float directVolumeLeft = mixData->directVolumeCurrent[0];
	float directVolumeRight = mixData->directVolumeCurrent[1];

	Fixed ds = (Fixed) (mixData->frequencyFinal * kMixFractionMultiplier);
	Fixed offset = inputOffset << kMixFractionSize;

	StereoMixFrame *output = stereoRingBuffer;

	int32 count = 0;
	do
	{
		int32 sample1 = ReadLittleEndianS16(&input[offset >> kMixFractionSize]);
		int32 sample2 = ReadLittleEndianS16(&input[Min((offset >> kMixFractionSize) + 1, inputFrameCount - 1)]);

		Fixed param = offset & kMixFractionMax;
		float sample = (float) (sample1 + (((sample2 - sample1) * param) >> kMixFractionSize));

		float leftSample = sample * directVolumeLeft;
		float rightSample = sample * directVolumeRight;

		output[outputOffset].left += leftSample;
		output[outputOffset].right += rightSample;

		directVolumeLeft += mixData->directVolumeDelta[0];
		directVolumeRight += mixData->directVolumeDelta[1];

		count++;
		offset += ds;
		if ((offset >> kMixFractionSize) >= inputFrameCount)
		{
			break;
		}

		outputOffset++;

	} while (count < outputFrameCount);

	mixData->directVolumeCurrent[0] = directVolumeLeft;
	mixData->directVolumeCurrent[1] = directVolumeRight;

	inputOffset = offset >> kMixFractionSize;
	return (count);
}

int32 SoundMgr::MixStereoSamples_Mono_Variable(SoundMixData *mixData, const Sample *input, int32 inputFrameCount, int32& inputOffset, int32 outputFrameCount, int32 outputOffset)
{
	float directVolumeLeft = mixData->directVolumeCurrent[0];
	float directVolumeRight = mixData->directVolumeCurrent[1];

	float ds = mixData->frequencyCurrent * kMixFractionMultiplier;
	float alpha = mixData->frequencyAlpha;
	Fixed offset = inputOffset << kMixFractionSize;

	StereoMixFrame *output = stereoRingBuffer;

	int32 count = 0;
	do
	{
		int32 sample1 = ReadLittleEndianS16(&input[offset >> kMixFractionSize]);
		int32 sample2 = ReadLittleEndianS16(&input[Min((offset >> kMixFractionSize) + 1, inputFrameCount - 1)]);

		Fixed param = offset & kMixFractionMax;
		float sample = (float) (sample1 + (((sample2 - sample1) * param) >> kMixFractionSize));

		float leftSample = sample * directVolumeLeft;
		float rightSample = sample * directVolumeRight;

		output[outputOffset].left += leftSample;
		output[outputOffset].right += rightSample;

		directVolumeLeft += mixData->directVolumeDelta[0];
		directVolumeRight += mixData->directVolumeDelta[1];

		count++;
		offset += (Fixed) ds;
		if ((offset >> kMixFractionSize) >= inputFrameCount)
		{
			break;
		}

		ds *= alpha;
		outputOffset++;

	} while (count < outputFrameCount);

	mixData->directVolumeCurrent[0] = directVolumeLeft;
	mixData->directVolumeCurrent[1] = directVolumeRight;
	mixData->frequencyCurrent = ds * kMixFractionReciprocal;

	inputOffset = offset >> kMixFractionSize;
	return (count);
}

int32 SoundMgr::MixStereoSamples_Mono_Constant_Dry(SoundMixData *mixData, const Sample *input, int32 inputFrameCount, int32& inputOffset, int32 outputFrameCount, int32 outputOffset)
{
	float directVolumeLeft = mixData->directVolumeCurrent[0];
	float directVolumeRight = mixData->directVolumeCurrent[1];

	Fixed ds = (Fixed) (mixData->frequencyFinal * kMixFractionMultiplier);
	Fixed offset = inputOffset << kMixFractionSize;

	int32 count = 0;
	if (offset < 0)
	{
		count = Min(-offset / ds, outputFrameCount);

		directVolumeLeft += mixData->directVolumeDelta[0] * count;
		directVolumeRight += mixData->directVolumeDelta[1] * count;

		if (count == outputFrameCount)
		{
			mixData->directVolumeCurrent[0] = directVolumeLeft;
			mixData->directVolumeCurrent[1] = directVolumeRight;

			inputOffset = (offset + ds * outputFrameCount) >> kMixFractionSize;
			return (outputFrameCount);
		}
		else
		{
			offset = 0;
			outputOffset += count;
		}
	}

	StereoMixFrame *output = stereoRingBuffer;

	unsigned_int32 index = mixData->sampleTableIndex;
	do
	{
		int32 sample1 = ReadLittleEndianS16(&input[offset >> kMixFractionSize]);
		int32 sample2 = ReadLittleEndianS16(&input[Min((offset >> kMixFractionSize) + 1, inputFrameCount - 1)]);

		Fixed param = offset & kMixFractionMax;
		float sample = (float) (sample1 + (((sample2 - sample1) * param) >> kMixFractionSize));

		float *tableSample = &mixData->sampleTable[index];
		index = (index + 1) & (kSampleHistoryCount - 1);

		float sum = mixData->sampleTableSum - *tableSample + sample;
		mixData->sampleTableSum = sum;
		*tableSample = sample;

		sum *= 1.0F / (float) kSampleHistoryCount;
		sample = sum + (sample - sum) * mixData->directHFVolume;

		float leftSample = sample * directVolumeLeft;
		float rightSample = sample * directVolumeRight;

		output[outputOffset].left += leftSample;
		output[outputOffset].right += rightSample;

		directVolumeLeft += mixData->directVolumeDelta[0];
		directVolumeRight += mixData->directVolumeDelta[1];

		count++;
		offset += ds;
		if ((offset >> kMixFractionSize) >= inputFrameCount)
		{
			break;
		}

		outputOffset++;

	} while (count < outputFrameCount);

	mixData->directVolumeCurrent[0] = directVolumeLeft;
	mixData->directVolumeCurrent[1] = directVolumeRight;

	mixData->sampleTableIndex = index;
	inputOffset = offset >> kMixFractionSize;
	return (count);
}

int32 SoundMgr::MixStereoSamples_Mono_Constant_Wet(SoundMixData *mixData, const Sample *input, int32 inputFrameCount, int32& inputOffset, int32 outputFrameCount, int32 outputOffset)
{
	float directVolumeLeft = mixData->directVolumeCurrent[0];
	float directVolumeRight = mixData->directVolumeCurrent[1];
	float reflectionVolume = mixData->reflectionVolumeCurrent;

	Fixed ds = (Fixed) (mixData->frequencyFinal * kMixFractionMultiplier);
	Fixed offset = inputOffset << kMixFractionSize;

	int32 count = 0;
	if (offset < 0)
	{
		count = Min(-offset / ds, outputFrameCount);

		directVolumeLeft += mixData->directVolumeDelta[0] * count;
		directVolumeRight += mixData->directVolumeDelta[1] * count;
		reflectionVolume += mixData->reflectionVolumeDelta * count;

		if (count == outputFrameCount)
		{
			mixData->directVolumeCurrent[0] = directVolumeLeft;
			mixData->directVolumeCurrent[1] = directVolumeRight;
			mixData->reflectionVolumeCurrent = reflectionVolume;

			inputOffset = (offset + ds * outputFrameCount) >> kMixFractionSize;
			return (outputFrameCount);
		}
		else
		{
			offset = 0;
			outputOffset += count;
		}
	}

	StereoMixFrame *output = stereoRingBuffer;
	StereoMixFrame *reverb = mixData->reflectionData[0].roomMixBuffer->reverbBuffer;

	unsigned_int32 index = mixData->sampleTableIndex;

	if (!mixData->reflectionData[1].roomMixBuffer)
	{
		do
		{
			int32 sample1 = ReadLittleEndianS16(&input[offset >> kMixFractionSize]);
			int32 sample2 = ReadLittleEndianS16(&input[Min((offset >> kMixFractionSize) + 1, inputFrameCount - 1)]);

			Fixed param = offset & kMixFractionMax;
			float sample = (float) (sample1 + (((sample2 - sample1) * param) >> kMixFractionSize));

			float *tableSample = &mixData->sampleTable[index];
			index = (index + 1) & (kSampleHistoryCount - 1);

			float sum = mixData->sampleTableSum - *tableSample + sample;
			mixData->sampleTableSum = sum;
			*tableSample = sample;

			sum *= 1.0F / (float) kSampleHistoryCount;
			sample = sum + (sample - sum) * mixData->directHFVolume;

			float leftSample = sample * directVolumeLeft;
			float rightSample = sample * directVolumeRight;

			output[outputOffset].left += leftSample;
			output[outputOffset].right += rightSample;

			sum *= reflectionVolume;
			sample *= reflectionVolume;

			for (machine a = 0; a < kSoundReflectionCount; a++)
			{
				float t = mixData->reflectionData[0].reflectionHFVolume[a] * mixData->reflectionHFVolume;
				float s = sum + (sample - sum) * t;

				unsigned_int32 reflectOffset = (outputOffset + mixData->reflectionData[0].reflectionDelay[a]) & (kRingBufferFrameCount - 1);
				reverb[reflectOffset].left += s * mixData->reflectionData[0].reflectionVolume[a];
				reverb[reflectOffset].right += s * mixData->reflectionData[0].reflectionVolume[a];
			}

			directVolumeLeft += mixData->directVolumeDelta[0];
			directVolumeRight += mixData->directVolumeDelta[1];
			reflectionVolume += mixData->reflectionVolumeDelta;

			count++;
			offset += ds;
			if ((offset >> kMixFractionSize) >= inputFrameCount)
			{
				break;
			}

			outputOffset++;

		} while (count < outputFrameCount);
	}
	else
	{
		StereoMixFrame *reverb2 = mixData->reflectionData[1].roomMixBuffer->reverbBuffer;

		do
		{
			int32 sample1 = ReadLittleEndianS16(&input[offset >> kMixFractionSize]);
			int32 sample2 = ReadLittleEndianS16(&input[Min((offset >> kMixFractionSize) + 1, inputFrameCount - 1)]);

			Fixed param = offset & kMixFractionMax;
			float sample = (float) (sample1 + (((sample2 - sample1) * param) >> kMixFractionSize));

			float *tableSample = &mixData->sampleTable[index];
			index = (index + 1) & (kSampleHistoryCount - 1);

			float sum = mixData->sampleTableSum - *tableSample + sample;
			mixData->sampleTableSum = sum;
			*tableSample = sample;

			sum *= 1.0F / (float) kSampleHistoryCount;
			sample = sum + (sample - sum) * mixData->directHFVolume;

			float leftSample = sample * directVolumeLeft;
			float rightSample = sample * directVolumeRight;

			output[outputOffset].left += leftSample;
			output[outputOffset].right += rightSample;

			sum *= reflectionVolume;
			sample *= reflectionVolume;

			for (machine a = 0; a < kSoundReflectionCount; a++)
			{
				float t = mixData->reflectionData[0].reflectionHFVolume[a] * mixData->reflectionHFVolume;
				float s = sum + (sample - sum) * t;

				unsigned_int32 reflectOffset = (outputOffset + mixData->reflectionData[0].reflectionDelay[a]) & (kRingBufferFrameCount - 1);
				reverb[reflectOffset].left += s * mixData->reflectionData[0].reflectionVolume[a];
				reverb[reflectOffset].right += s * mixData->reflectionData[0].reflectionVolume[a];

				t = mixData->reflectionData[1].reflectionHFVolume[a] * mixData->reflectionHFVolume;
				s = sum + (sample - sum) * t;

				reflectOffset = (outputOffset + mixData->reflectionData[1].reflectionDelay[a]) & (kRingBufferFrameCount - 1);
				reverb2[reflectOffset].left += s * mixData->reflectionData[1].reflectionVolume[a];
				reverb2[reflectOffset].right += s * mixData->reflectionData[1].reflectionVolume[a];
			}

			directVolumeLeft += mixData->directVolumeDelta[0];
			directVolumeRight += mixData->directVolumeDelta[1];
			reflectionVolume += mixData->reflectionVolumeDelta;

			count++;
			offset += ds;
			if ((offset >> kMixFractionSize) >= inputFrameCount)
			{
				break;
			}

			outputOffset++;

		} while (count < outputFrameCount);
	}

	mixData->directVolumeCurrent[0] = directVolumeLeft;
	mixData->directVolumeCurrent[1] = directVolumeRight;
	mixData->reflectionVolumeCurrent = reflectionVolume;

	mixData->sampleTableIndex = index;
	inputOffset = offset >> kMixFractionSize;
	return (count);
}

int32 SoundMgr::MixStereoSamples_Mono_Variable_Dry(SoundMixData *mixData, const Sample *input, int32 inputFrameCount, int32& inputOffset, int32 outputFrameCount, int32 outputOffset)
{
	float directVolumeLeft = mixData->directVolumeCurrent[0];
	float directVolumeRight = mixData->directVolumeCurrent[1];

	float ds = mixData->frequencyCurrent * kMixFractionMultiplier;
	float alpha = mixData->frequencyAlpha;
	Fixed offset = inputOffset << kMixFractionSize;

	int32 count = 0;
	if (offset < 0)
	{
		do
		{
			directVolumeLeft += mixData->directVolumeDelta[0];
			directVolumeRight += mixData->directVolumeDelta[1];

			count++;
			offset += (Fixed) ds;
			ds *= alpha;
			outputOffset++;

			if (offset > 0)
			{
				break;
			}
		} while (count < outputFrameCount);
	}

	StereoMixFrame *output = stereoRingBuffer;

	unsigned_int32 index = mixData->sampleTableIndex;
	while (count < outputFrameCount)
	{
		int32 sample1 = ReadLittleEndianS16(&input[offset >> kMixFractionSize]);
		int32 sample2 = ReadLittleEndianS16(&input[Min((offset >> kMixFractionSize) + 1, inputFrameCount - 1)]);

		Fixed param = offset & kMixFractionMax;
		float sample = (float) (sample1 + (((sample2 - sample1) * param) >> kMixFractionSize));

		float *tableSample = &mixData->sampleTable[index];
		index = (index + 1) & (kSampleHistoryCount - 1);

		float sum = mixData->sampleTableSum - *tableSample + sample;
		mixData->sampleTableSum = sum;
		*tableSample = sample;

		sum *= 1.0F / (float) kSampleHistoryCount;
		sample = sum + (sample - sum) * mixData->directHFVolume;

		float leftSample = sample * directVolumeLeft;
		float rightSample = sample * directVolumeRight;

		output[outputOffset].left += leftSample;
		output[outputOffset].right += rightSample;

		directVolumeLeft += mixData->directVolumeDelta[0];
		directVolumeRight += mixData->directVolumeDelta[1];

		count++;
		offset += (Fixed) ds;
		if ((offset >> kMixFractionSize) >= inputFrameCount)
		{
			break;
		}

		ds *= alpha;
		outputOffset++;
	}

	mixData->directVolumeCurrent[0] = directVolumeLeft;
	mixData->directVolumeCurrent[1] = directVolumeRight;
	mixData->frequencyCurrent = ds * kMixFractionReciprocal;

	mixData->sampleTableIndex = index;
	inputOffset = offset >> kMixFractionSize;
	return (count);
}

int32 SoundMgr::MixStereoSamples_Mono_Variable_Wet(SoundMixData *mixData, const Sample *input, int32 inputFrameCount, int32& inputOffset, int32 outputFrameCount, int32 outputOffset)
{
	float directVolumeLeft = mixData->directVolumeCurrent[0];
	float directVolumeRight = mixData->directVolumeCurrent[1];
	float reflectionVolume = mixData->reflectionVolumeCurrent;

	float ds = mixData->frequencyCurrent * kMixFractionMultiplier;
	float alpha = mixData->frequencyAlpha;
	Fixed offset = inputOffset << kMixFractionSize;

	int32 count = 0;
	if (offset < 0)
	{
		do
		{
			directVolumeLeft += mixData->directVolumeDelta[0];
			directVolumeRight += mixData->directVolumeDelta[1];
			reflectionVolume += mixData->reflectionVolumeDelta;

			count++;
			offset += (Fixed) ds;
			ds *= alpha;
			outputOffset++;

			if (offset > 0)
			{
				break;
			}
		} while (count < outputFrameCount);
	}

	StereoMixFrame *output = stereoRingBuffer;
	StereoMixFrame *reverb = mixData->reflectionData[0].roomMixBuffer->reverbBuffer;

	unsigned_int32 index = mixData->sampleTableIndex;

	if (!mixData->reflectionData[1].roomMixBuffer)
	{
		while (count < outputFrameCount)
		{
			int32 sample1 = ReadLittleEndianS16(&input[offset >> kMixFractionSize]);
			int32 sample2 = ReadLittleEndianS16(&input[Min((offset >> kMixFractionSize) + 1, inputFrameCount - 1)]);

			Fixed param = offset & kMixFractionMax;
			float sample = (float) (sample1 + (((sample2 - sample1) * param) >> kMixFractionSize));

			float *tableSample = &mixData->sampleTable[index];
			index = (index + 1) & (kSampleHistoryCount - 1);

			float sum = mixData->sampleTableSum - *tableSample + sample;
			mixData->sampleTableSum = sum;
			*tableSample = sample;

			sum *= 1.0F / (float) kSampleHistoryCount;
			sample = sum + (sample - sum) * mixData->directHFVolume;

			float leftSample = sample * directVolumeLeft;
			float rightSample = sample * directVolumeRight;

			output[outputOffset].left += leftSample;
			output[outputOffset].right += rightSample;

			sum *= reflectionVolume;
			sample *= reflectionVolume;

			for (machine a = 0; a < kSoundReflectionCount; a++)
			{
				float t = mixData->reflectionData[0].reflectionHFVolume[a] * mixData->reflectionHFVolume;
				float s = sum + (sample - sum) * t;

				unsigned_int32 reflectOffset = (outputOffset + mixData->reflectionData[0].reflectionDelay[a]) & (kRingBufferFrameCount - 1);
				reverb[reflectOffset].left += s * mixData->reflectionData[0].reflectionVolume[a];
				reverb[reflectOffset].right += s * mixData->reflectionData[0].reflectionVolume[a];
			}

			directVolumeLeft += mixData->directVolumeDelta[0];
			directVolumeRight += mixData->directVolumeDelta[1];
			reflectionVolume += mixData->reflectionVolumeDelta;

			count++;
			offset += (Fixed) ds;
			if ((offset >> kMixFractionSize) >= inputFrameCount)
			{
				break;
			}

			ds *= alpha;
			outputOffset++;
		}
	}
	else
	{
		StereoMixFrame *reverb2 = mixData->reflectionData[1].roomMixBuffer->reverbBuffer;

		while (count < outputFrameCount)
		{
			int32 sample1 = ReadLittleEndianS16(&input[offset >> kMixFractionSize]);
			int32 sample2 = ReadLittleEndianS16(&input[Min((offset >> kMixFractionSize) + 1, inputFrameCount - 1)]);

			Fixed param = offset & kMixFractionMax;
			float sample = (float) (sample1 + (((sample2 - sample1) * param) >> kMixFractionSize));

			float *tableSample = &mixData->sampleTable[index];
			index = (index + 1) & (kSampleHistoryCount - 1);

			float sum = mixData->sampleTableSum - *tableSample + sample;
			mixData->sampleTableSum = sum;
			*tableSample = sample;

			sum *= 1.0F / (float) kSampleHistoryCount;
			sample = sum + (sample - sum) * mixData->directHFVolume;

			float leftSample = sample * directVolumeLeft;
			float rightSample = sample * directVolumeRight;

			output[outputOffset].left += leftSample;
			output[outputOffset].right += rightSample;

			sum *= reflectionVolume;
			sample *= reflectionVolume;

			for (machine a = 0; a < kSoundReflectionCount; a++)
			{
				float t = mixData->reflectionData[0].reflectionHFVolume[a] * mixData->reflectionHFVolume;
				float s = sum + (sample - sum) * t;

				unsigned_int32 reflectOffset = (outputOffset + mixData->reflectionData[0].reflectionDelay[a]) & (kRingBufferFrameCount - 1);
				reverb[reflectOffset].left += s * mixData->reflectionData[0].reflectionVolume[a];
				reverb[reflectOffset].right += s * mixData->reflectionData[0].reflectionVolume[a];

				t = mixData->reflectionData[1].reflectionHFVolume[a] * mixData->reflectionHFVolume;
				s = sum + (sample - sum) * t;

				reflectOffset = (outputOffset + mixData->reflectionData[1].reflectionDelay[a]) & (kRingBufferFrameCount - 1);
				reverb2[reflectOffset].left += s * mixData->reflectionData[1].reflectionVolume[a];
				reverb2[reflectOffset].right += s * mixData->reflectionData[1].reflectionVolume[a];
			}

			directVolumeLeft += mixData->directVolumeDelta[0];
			directVolumeRight += mixData->directVolumeDelta[1];
			reflectionVolume += mixData->reflectionVolumeDelta;

			count++;
			offset += (Fixed) ds;
			if ((offset >> kMixFractionSize) >= inputFrameCount)
			{
				break;
			}

			ds *= alpha;
			outputOffset++;
		}
	}

	mixData->directVolumeCurrent[0] = directVolumeLeft;
	mixData->directVolumeCurrent[1] = directVolumeRight;
	mixData->reflectionVolumeCurrent = reflectionVolume;
	mixData->frequencyCurrent = ds * kMixFractionReciprocal;

	mixData->sampleTableIndex = index;
	inputOffset = offset >> kMixFractionSize;
	return (count);
}

int32 SoundMgr::MixStereoSamples_Stereo_Constant(SoundMixData *mixData, const Sample *input, int32 inputFrameCount, int32& inputOffset, int32 outputFrameCount, int32 outputOffset)
{
	float directVolumeLeft = mixData->directVolumeCurrent[0];
	float directVolumeRight = mixData->directVolumeCurrent[1];

	Fixed ds = (Fixed) (mixData->frequencyFinal * kMixFractionMultiplier);
	Fixed offset = inputOffset << kMixFractionSize;

	StereoMixFrame *output = stereoRingBuffer;

	int32 count = 0;
	do
	{
		const Sample *source = &input[(offset >> (kMixFractionSize - 1)) & ~1];
		float leftSample = (float) ReadLittleEndianS16(&source[0]);
		float rightSample = (float) ReadLittleEndianS16(&source[1]);

		output[outputOffset].left += leftSample * directVolumeLeft;
		output[outputOffset].right += rightSample * directVolumeRight;

		directVolumeLeft += mixData->directVolumeDelta[0];
		directVolumeRight += mixData->directVolumeDelta[1];

		count++;
		offset += ds;
		if ((offset >> kMixFractionSize) >= inputFrameCount)
		{
			break;
		}

		outputOffset++;

	} while (count < outputFrameCount);

	mixData->directVolumeCurrent[0] = directVolumeLeft;
	mixData->directVolumeCurrent[1] = directVolumeRight;

	inputOffset = offset >> kMixFractionSize;
	return (count);
}

int32 SoundMgr::MixStereoSamples_Stereo_Variable(SoundMixData *mixData, const Sample *input, int32 inputFrameCount, int32& inputOffset, int32 outputFrameCount, int32 outputOffset)
{
	float directVolumeLeft = mixData->directVolumeCurrent[0];
	float directVolumeRight = mixData->directVolumeCurrent[1];

	float ds = mixData->frequencyCurrent * kMixFractionMultiplier;
	float alpha = mixData->frequencyAlpha;
	Fixed offset = inputOffset << kMixFractionSize;

	StereoMixFrame *output = stereoRingBuffer;

	int32 count = 0;
	do
	{
		const Sample *source = &input[(offset >> (kMixFractionSize - 1)) & ~1];
		float leftSample = (float) ReadLittleEndianS16(&source[0]);
		float rightSample = (float) ReadLittleEndianS16(&source[1]);

		output[outputOffset].left += leftSample * directVolumeLeft;
		output[outputOffset].right += rightSample * directVolumeRight;
		directVolumeLeft += mixData->directVolumeDelta[0];
		directVolumeRight += mixData->directVolumeDelta[1];

		count++;
		offset += (Fixed) ds;
		if ((offset >> kMixFractionSize) >= inputFrameCount)
		{
			break;
		}

		ds *= alpha;
		outputOffset++;

	} while (count < outputFrameCount);

	mixData->directVolumeCurrent[0] = directVolumeLeft;
	mixData->directVolumeCurrent[1] = directVolumeRight;
	mixData->frequencyCurrent = ds * kMixFractionReciprocal;

	inputOffset = offset >> kMixFractionSize;
	return (count);
}

void SoundMgr::MixSoundMono(Sound *sound, int32 outputOffset)
{
	const Sample *input = sound->soundSampleData;
	int32 inputFrameCount = sound->soundFrameCount;
	int32 inputOffset = sound->playFrame;

	int32 outputFrameCount = kOutputBufferFrameCount;

	float ratio = sound->soundMixData.frequencyFinal / sound->soundMixData.frequencyCurrent;
	if (Fabs(ratio - 1.0F) < 0.015625F)
	{
		MixProc mixProc = &SoundMgr::MixStereoSamples_Mono_Constant;
		if (sound->GetSoundFlags() & kSoundSpatialized)
		{
			mixProc = (sound->soundMixData.reflectionData[0].roomMixBuffer) ? &SoundMgr::MixStereoSamples_Mono_Constant_Wet : &SoundMgr::MixStereoSamples_Mono_Constant_Dry;
		}

		for (;;)
		{
			int32 mixFrameCount = (this->*mixProc)(&sound->soundMixData, input, inputFrameCount, inputOffset, outputFrameCount, outputOffset);

			outputOffset += mixFrameCount;
			outputFrameCount -= mixFrameCount;

			if (inputOffset >= inputFrameCount)
			{
				if (sound->loopCount > 0)
				{
					sound->loopCount--;
				}

				if (sound->loopCount != 0)
				{
					inputOffset = 0;
					sound->loopFlag = true;
					continue;
				}
			}

			break;
		}
	}
	else
	{
		sound->soundMixData.frequencyAlpha = Pow(ratio, 1.0F / (float) kOutputBufferFrameCount);

		MixProc mixProc = &SoundMgr::MixStereoSamples_Mono_Variable;
		if (sound->GetSoundFlags() & kSoundSpatialized)
		{
			mixProc = (sound->soundMixData.reflectionData[0].roomMixBuffer) ? &SoundMgr::MixStereoSamples_Mono_Variable_Wet : &SoundMgr::MixStereoSamples_Mono_Variable_Dry;
		}

		for (;;)
		{
			int32 mixFrameCount = (this->*mixProc)(&sound->soundMixData, input, inputFrameCount, inputOffset, outputFrameCount, outputOffset);

			outputOffset += mixFrameCount;
			outputFrameCount -= mixFrameCount;

			if (inputOffset >= inputFrameCount)
			{
				if (sound->loopCount > 0)
				{
					sound->loopCount--;
				}

				if (sound->loopCount != 0)
				{
					inputOffset = 0;
					sound->loopFlag = true;
					continue;
				}
			}

			break;
		}
	}

	sound->playFrame = inputOffset;
}

void SoundMgr::MixSoundStereo(Sound *sound, int32 outputOffset)
{
	const Sample *input = sound->soundSampleData;
	int32 inputFrameCount = sound->soundFrameCount;
	int32 inputOffset = sound->playFrame;

	int32 outputFrameCount = kOutputBufferFrameCount;

	float ratio = sound->soundMixData.frequencyFinal / sound->soundMixData.frequencyCurrent;
	if (Fabs(ratio - 1.0F) < 0.015625F)
	{
		for (;;)
		{
			int32 mixFrameCount = MixStereoSamples_Stereo_Constant(&sound->soundMixData, input, inputFrameCount, inputOffset, outputFrameCount, outputOffset);

			outputOffset += mixFrameCount;
			outputFrameCount -= mixFrameCount;

			if (inputOffset >= inputFrameCount)
			{
				if (sound->loopCount > 0)
				{
					sound->loopCount--;
				}

				if (sound->loopCount != 0)
				{
					inputOffset = 0;
					sound->loopFlag = true;
					continue;
				}
			}

			break;
		}
	}
	else
	{
		sound->soundMixData.frequencyAlpha = Pow(ratio, 1.0F / (float) kOutputBufferFrameCount);

		for (;;)
		{
			int32 mixFrameCount = MixStereoSamples_Stereo_Variable(&sound->soundMixData, input, inputFrameCount, inputOffset, outputFrameCount, outputOffset);

			outputOffset += mixFrameCount;
			outputFrameCount -= mixFrameCount;

			if (inputOffset >= inputFrameCount)
			{
				if (sound->loopCount > 0)
				{
					sound->loopCount--;
				}

				if (sound->loopCount != 0)
				{
					inputOffset = 0;
					sound->loopFlag = true;
					continue;
				}
			}

			break;
		}
	}

	sound->playFrame = inputOffset;
}

void SoundMgr::MixSoundStreamMono(Sound *sound, int32 outputOffset)
{
	int32 bufferIndex = sound->playBuffer;
	StreamBufferHeader *header = sound->GetSoundStreamer()->GetStreamBuffer(bufferIndex);

	if (header->readyFlag)
	{
		const Sample *input = header->GetSampleData();
		int32 inputFrameCount = header->frameCount;
		int32 inputOffset = sound->playFrame;

		int32 outputFrameCount = kOutputBufferFrameCount;

		float ratio = sound->soundMixData.frequencyFinal / sound->soundMixData.frequencyCurrent;
		if (Fabs(ratio - 1.0F) < 0.015625F)
		{
			MixProc mixProc = &SoundMgr::MixStereoSamples_Mono_Constant;
			if (sound->GetSoundFlags() & kSoundSpatialized)
			{
				mixProc = (sound->soundMixData.reflectionData[0].roomMixBuffer) ? &SoundMgr::MixStereoSamples_Mono_Constant_Wet : &SoundMgr::MixStereoSamples_Mono_Constant_Dry;
			}

			do
			{
				int32 mixFrameCount = (this->*mixProc)(&sound->soundMixData, input, inputFrameCount, inputOffset, outputFrameCount, outputOffset);

				outputOffset += mixFrameCount;
				outputFrameCount -= mixFrameCount;

				if (inputOffset >= header->loopFrame)
				{
					sound->loopFlag = true;
					header->loopFrame = 0x7FFFFFFF;
				}

				if (inputOffset >= inputFrameCount)
				{
					if (header->finalFlag)
					{
						sound->soundState = kSoundCompleted;
						sound->waitMixStamp = soundMixStamp;
						break;
					}

					header->readyFlag = false;
					bufferIndex = 1 - bufferIndex;
					inputOffset = 0;

					header = sound->GetSoundStreamer()->GetStreamBuffer(bufferIndex);
					if (!header->readyFlag)
					{
						break;
					}

					input = header->GetSampleData();
					inputFrameCount = header->frameCount;
				}
			} while (outputFrameCount > 0);
		}
		else
		{
			sound->soundMixData.frequencyAlpha = Pow(ratio, 1.0F / (float) kOutputBufferFrameCount);

			MixProc mixProc = &SoundMgr::MixStereoSamples_Mono_Variable;
			if (sound->GetSoundFlags() & kSoundSpatialized)
			{
				mixProc = (sound->soundMixData.reflectionData[0].roomMixBuffer) ? &SoundMgr::MixStereoSamples_Mono_Variable_Wet : &SoundMgr::MixStereoSamples_Mono_Variable_Dry;
			}

			do
			{
				int32 mixFrameCount = (this->*mixProc)(&sound->soundMixData, input, inputFrameCount, inputOffset, outputFrameCount, outputOffset);

				outputOffset += mixFrameCount;
				outputFrameCount -= mixFrameCount;

				if (inputOffset >= header->loopFrame)
				{
					sound->loopFlag = true;
					header->loopFrame = 0x7FFFFFFF;
				}

				if (inputOffset >= inputFrameCount)
				{
					if (header->finalFlag)
					{
						sound->soundState = kSoundCompleted;
						sound->waitMixStamp = soundMixStamp;
						break;
					}

					header->readyFlag = false;
					bufferIndex = 1 - bufferIndex;
					inputOffset = 0;

					header = sound->GetSoundStreamer()->GetStreamBuffer(bufferIndex);
					if (!header->readyFlag)
					{
						break;
					}

					input = header->GetSampleData();
					inputFrameCount = header->frameCount;
				}
			} while (outputFrameCount > 0);
		}

		sound->playFrame = inputOffset;
		sound->playBuffer = bufferIndex;
	}
}

void SoundMgr::MixSoundStreamStereo(Sound *sound, int32 outputOffset)
{
	int32 bufferIndex = sound->playBuffer;
	StreamBufferHeader *header = sound->GetSoundStreamer()->GetStreamBuffer(bufferIndex);

	if (header->readyFlag)
	{
		const Sample *input = header->GetSampleData();
		int32 inputFrameCount = header->frameCount;
		int32 inputOffset = sound->playFrame;

		unsigned_int32 outputFrameCount = kOutputBufferFrameCount;

		float ratio = sound->soundMixData.frequencyFinal / sound->soundMixData.frequencyCurrent;
		if (Fabs(ratio - 1.0F) < 0.015625F)
		{
			do
			{
				int32 mixFrameCount = MixStereoSamples_Stereo_Constant(&sound->soundMixData, input, inputFrameCount, inputOffset, outputFrameCount, outputOffset);

				outputOffset += mixFrameCount;
				outputFrameCount -= mixFrameCount;

				if (inputOffset >= header->loopFrame)
				{
					sound->loopFlag = true;
					header->loopFrame = 0x7FFFFFFF;
				}

				if (inputOffset >= inputFrameCount)
				{
					if (header->finalFlag)
					{
						sound->soundState = kSoundCompleted;
						sound->waitMixStamp = soundMixStamp;
						break;
					}

					header->readyFlag = false;
					bufferIndex = 1 - bufferIndex;
					inputOffset = 0;

					header = sound->GetSoundStreamer()->GetStreamBuffer(bufferIndex);
					if (!header->readyFlag)
					{
						break;
					}

					input = header->GetSampleData();
					inputFrameCount = header->frameCount;
				}
			} while (outputFrameCount > 0);
		}
		else
		{
			sound->soundMixData.frequencyAlpha = Pow(ratio, 1.0F / (float) kOutputBufferFrameCount);

			do
			{
				int32 mixFrameCount = MixStereoSamples_Stereo_Variable(&sound->soundMixData, input, inputFrameCount, inputOffset, outputFrameCount, outputOffset);

				outputOffset += mixFrameCount;
				outputFrameCount -= mixFrameCount;

				if (inputOffset >= header->loopFrame)
				{
					sound->loopFlag = true;
					header->loopFrame = 0x7FFFFFFF;
				}

				if (inputOffset >= inputFrameCount)
				{
					if (header->finalFlag)
					{
						sound->soundState = kSoundCompleted;
						sound->waitMixStamp = soundMixStamp;
						break;
					}

					header->readyFlag = false;
					bufferIndex = 1 - bufferIndex;
					inputOffset = 0;

					header = sound->GetSoundStreamer()->GetStreamBuffer(bufferIndex);
					if (!header->readyFlag)
					{
						break;
					}

					input = header->GetSampleData();
					inputFrameCount = header->frameCount;
				}
			} while (outputFrameCount > 0);
		}

		sound->playFrame = inputOffset;
		sound->playBuffer = bufferIndex;
	}
}

void SoundMgr::MixRoomEffects(const SoundRoom *soundRoom, RoomMixBuffer *mixBuffer, int32 outputOffset)
{
	static const int32 kFeedbackOffset[kRoomFeedbackBufferCount] =
	{
		kMaxFeedbackDelay * 35 / 100, kMaxFeedbackDelay * 63 / 100, kMaxFeedbackDelay * 85 / 100, kMaxFeedbackDelay
	};

	StereoMixFrame *output = stereoRingBuffer;
	StereoMixFrame *reverb = mixBuffer->reverbBuffer;
	float reverbVolume = soundRoom->reverbVolume;

	float leftVolume = soundRoom->roomVolume[0];
	float rightVolume = soundRoom->roomVolume[1];

	int32 count = 0;
	do
	{
		float leftSample = reverb[outputOffset].left;
		float rightSample = reverb[outputOffset].right;

		for (machine a = 0; a < kRoomFeedbackBufferCount; a++)
		{
			StereoMixFrame *input = mixBuffer->feedbackBuffer[a];
			leftSample = input[outputOffset].left - leftSample;
			rightSample = input[outputOffset].right - rightSample;

			StereoMixFrame *feedback = &input[(outputOffset + kFeedbackOffset[a]) & (kRingBufferFrameCount - 1)];
			feedback->left += reverb[outputOffset].left + input[outputOffset].left * reverbVolume;
			feedback->right += reverb[outputOffset].right + input[outputOffset].right * reverbVolume;

			input[outputOffset].left = 0.0F;
			input[outputOffset].right = 0.0F;
		}

		output[outputOffset].left += leftSample * leftVolume;
		output[outputOffset].right += rightSample * rightVolume;
		reverb[outputOffset].left = 0.0F;
		reverb[outputOffset].right = 0.0F;

		outputOffset++;
	} while (++count < kOutputBufferFrameCount);
}

void SoundMgr::AllocateListenerRoomMixBuffer(SoundRoom *soundRoom)
{
	int32 roomIndex = -1;
	int32 minMixCount = 0x7FFF;
	for (machine a = 0; a < kMaxRoomCount; a++)
	{
		const SoundRoom *room = activeRoomTable[a];
		if (!room)
		{
			roomIndex = a;
			break;
		}

		int32 count = room->roomMixCount;
		if (count < minMixCount)
		{
			minMixCount = count;
			roomIndex = a;
		}
	}

	SoundRoom *room = activeRoomTable[roomIndex];
	if (room)
	{
		room->tableIndex = -1;
	}

	MemoryMgr::ClearMemory(roomMixBuffer[roomIndex], sizeof(RoomMixBuffer));
	activeRoomTable[roomIndex] = soundRoom;
	soundRoom->tableIndex = roomIndex;
	soundRoom->roomMixCount = 0;
}

int32 SoundMgr::AllocateSoundRoomMixBuffer(SoundRoom *soundRoom)
{
	if (!soundRoom->GetOwningList())
	{
		int32 index = soundRoom->tableIndex;
		if (index >= 0)
		{
			return (index);
		}

		for (machine a = 0; a < kMaxRoomCount; a++)
		{
			const SoundRoom *room = activeRoomTable[a];
			if (!room)
			{
				MemoryMgr::ClearMemory(roomMixBuffer[a], sizeof(RoomMixBuffer));
				activeRoomTable[a] = soundRoom;
				soundRoom->tableIndex = a;
				return (a);
			}
		}
	}

	return (-1);
}

void SoundMgr::MixSounds(OutputSample *outputSample)
{
	bool reverbFlag = ((soundOptionFlags & kSoundOptionReverb) != 0);
	if (reverbFlag)
	{
		SoundRoom *primaryRoom = listenerRoom;
		if ((primaryRoom) && (primaryRoom->tableIndex < 0))
		{
			AllocateListenerRoomMixBuffer(primaryRoom);
		}
	}

	int32 sliceIndex = ringBufferSliceIndex;
	int32 outputOffset = kOutputBufferFrameCount * sliceIndex;
	ringBufferSliceIndex = (sliceIndex + 1) & (kRingBufferSliceCount - 1);

	for (machine index = 0; index < kMaxSoundCount; index++)
	{
		Sound *sound = activeSoundTable[index];
		if (sound)
		{
			if (sound->soundState == kSoundPlaying)
			{
				sound->soundMixData.directVolumeDelta[0] = (sound->soundMixData.directVolumeFinal[0] - sound->soundMixData.directVolumeCurrent[0]) / kOutputBufferFrameCount;
				sound->soundMixData.directVolumeDelta[1] = (sound->soundMixData.directVolumeFinal[1] - sound->soundMixData.directVolumeCurrent[1]) / kOutputBufferFrameCount;

				unsigned_int32 flags = sound->GetSoundFlags();
				if ((flags & kSoundSpatialized) && (sound->channelCount == 1))
				{
					sound->soundMixData.reflectionVolumeDelta = (sound->soundMixData.reflectionVolumeFinal - sound->soundMixData.reflectionVolumeCurrent) / kOutputBufferFrameCount;

					sound->soundMixData.reflectionData[0].roomMixBuffer = nullptr;
					sound->soundMixData.reflectionData[1].roomMixBuffer = nullptr;

					if ((flags & kSoundReverb) && (reverbFlag))
					{
						SoundRoom *soundRoom = sound->soundMixData.reflectionData[0].soundRoom;
						if (soundRoom)
						{
							int32 roomIndex = AllocateSoundRoomMixBuffer(soundRoom);
							if (roomIndex >= 0)
							{
								sound->soundMixData.reflectionData[0].roomMixBuffer = roomMixBuffer[roomIndex];
								soundRoom->roomMixCount = soundRoom->maxRoomMixCount;

								soundRoom = sound->soundMixData.reflectionData[1].soundRoom;
								if (soundRoom)
								{
									roomIndex = AllocateSoundRoomMixBuffer(soundRoom);
									if (roomIndex >= 0)
									{
										sound->soundMixData.reflectionData[1].roomMixBuffer = roomMixBuffer[roomIndex];
										soundRoom->roomMixCount = soundRoom->maxRoomMixCount;
									}
								}
							}
						}
					}

					const SoundStreamer *streamer = sound->soundStreamer;
					if (!streamer)
					{
						MixSoundMono(sound, outputOffset);
						if (sound->playFrame >= sound->soundFrameCount)
						{
							sound->soundState = kSoundCompleted;
							sound->waitMixStamp = soundMixStamp;
						}
					}
					else if (!streamer->Paused())
					{
						MixSoundStreamMono(sound, outputOffset);
					}

					sound->soundMixData.reflectionVolumeCurrent = sound->soundMixData.reflectionVolumeFinal;
				}
				else
				{
					const SoundStreamer *streamer = sound->soundStreamer;
					if (!streamer)
					{
						if (sound->channelCount == 1)
						{
							MixSoundMono(sound, outputOffset);
						}
						else
						{
							MixSoundStereo(sound, outputOffset);
						}

						if (sound->playFrame >= sound->soundFrameCount)
						{
							sound->soundState = kSoundCompleted;
							sound->waitMixStamp = soundMixStamp;
						}
					}
					else if (!streamer->Paused())
					{
						if (sound->channelCount == 1)
						{
							MixSoundStreamMono(sound, outputOffset);
						}
						else
						{
							MixSoundStreamStereo(sound, outputOffset);
						}
					}
				}

				sound->soundMixData.directVolumeCurrent[0] = sound->soundMixData.directVolumeFinal[0];
				sound->soundMixData.directVolumeCurrent[1] = sound->soundMixData.directVolumeFinal[1];
				sound->soundMixData.frequencyCurrent = sound->soundMixData.frequencyFinal;

				Thread::Fence();
				sound->mixFlag = true;
			}
			else if (sound->mainReleaseFlag)
			{
				sound->mixerReleaseFlag = true;
			}
		}
	}

	for (machine a = 0; a < kMaxRoomCount; a++)
	{
		SoundRoom *room = activeRoomTable[a];
		if (room)
		{
			MixRoomEffects(room, roomMixBuffer[a], outputOffset);

			if (--room->roomMixCount < 0)
			{
				room->tableIndex = -1;
				activeRoomTable[a] = nullptr;
			}
		}
	}

	OutputSoundData(&stereoRingBuffer[outputOffset].left, outputSample);

	soundMixStamp++;
}

void SoundMgr::OutputSoundData(float *inputSample, OutputSample *outputSample)
{
	#if C4RECORDABLE

		if (!recordFlag)
		{

	#endif

		#if C4WINDOWS || C4LINUX || C4IOS

			#if C4SIMD

				vec_float *input = reinterpret_cast<vec_float *>(inputSample);
				vec_int16 *output = reinterpret_cast<vec_int16 *>(outputSample);

				const vec_float zero = VecFloatGetZero();

				for (machine a = 0; a < kOutputBufferFrameCount / 4; a++)
				{
					vec_int32 sample1 = VecConvertInt32(input[0]);
					vec_int32 sample2 = VecConvertInt32(input[1]);
					*output++ = VecInt32PackSaturate(sample1, sample2);
					input[0] = zero;
					input[1] = zero;
					input += 2;
				}

			#else

				for (machine a = 0; a < kOutputBufferFrameCount * 2; a++)
				{
					int32 sample = (int32) *inputSample;
					sample = Min(Max(sample, -32768), 32767);
					*outputSample++ = (Sample) sample;
					*inputSample++ = 0.0F;
				}

			#endif

		#elif C4MACOS || C4PS4 || C4PS3

			#if C4SIMD

				vec_float *input = reinterpret_cast<vec_float *>(inputSample);
				vec_float *output = reinterpret_cast<vec_float *>(outputSample);

				const vec_float zero = VecFloatGetZero();
				const vec_float scale = VecLoadVectorConstant<0x38000000>();

				for (machine a = 0; a < kOutputBufferFrameCount / 2; a++)
				{
					*output++ = VecMul(*input, scale);
					*input++ = zero;
				}

			#else

				for (machine a = 0; a < kOutputBufferFrameCount * 2; a++)
				{
					*outputSample++ = *inputSample * 3.0517578125e-5F;
					*inputSample++ = 0.0F;
				}

			#endif

		#endif

	#if C4RECORDABLE

		}
		else
		{
			int32 recordIndex = recordSliceIndex;
			Sample *recordSample = recordBuffer + recordIndex * (kOutputBufferFrameCount * 2);

			#if C4WINDOWS || C4LINUX || C4IOS

				#if C4SIMD

					vec_float *input = reinterpret_cast<vec_float *>(inputSample);
					vec_int16 *output = reinterpret_cast<vec_int16 *>(outputSample);
					vec_int16 *record = reinterpret_cast<vec_int16 *>(recordSample);

					const vec_float zero = VecFloatGetZero();

					for (machine a = 0; a < kOutputBufferFrameCount / 4; a++)
					{
						vec_int32 sample1 = VecConvertInt32(input[0]);
						vec_int32 sample2 = VecConvertInt32(input[1]);
						vec_int16 packedSample = VecInt32PackSaturate(sample1, sample2);
						*output++ = packedSample;
						*record++ = packedSample;
						input[0] = zero;
						input[1] = zero;
						input += 2;
					}

				#else

					for (machine a = 0; a < kOutputBufferFrameCount * 2; a++)
					{
						int32 sample = (int32) *inputSample;
						sample = Min(Max(sample, -32768), 32767);
						*outputSample++ = (Sample) sample;
						*recordSample++ = (Sample) sample;
						*inputSample++ = 0.0F;
					}

				#endif

			#elif C4MACOS || C4PS4 || C4PS3

				#if C4SIMD

					vec_float *input = reinterpret_cast<vec_float *>(inputSample);
					vec_float *output = reinterpret_cast<vec_float *>(outputSample);
					vec_int16 *record = reinterpret_cast<vec_int16 *>(recordSample);

					const vec_float zero = VecFloatGetZero();
					const vec_float scale = VecLoadVectorConstant<0x38000000>();

					for (machine a = 0; a < kOutputBufferFrameCount / 4; a++)
					{
						vec_int32 sample1 = VecConvertInt32(input[0]);
						vec_int32 sample2 = VecConvertInt32(input[1]);
						*record++ = VecInt32PackSaturate(sample1, sample2);

						output[0] = VecMul(input[0], scale);
						output[1] = VecMul(input[1], scale);
						input[0] = zero;
						input[1] = zero;
						input += 2;
						output += 2;
					}

				#else

					for (machine a = 0; a < kOutputBufferFrameCount * 2; a++)
					{
						int32 sample = (int32) *inputSample;
						sample = Min(Max(sample, -32768), 32767);
						WriteLittleEndianS16(recordSample++, (Sample) sample);

						*outputSample++ = *inputSample * 3.0517578125e-5F;
						*inputSample++ = 0.0F;
					}

				#endif

			#endif

			recordIndex++;
			recordSliceIndex = recordIndex & (kRecordBufferSliceCount - 1);

			if ((recordIndex & (kRecordBufferSliceCount / 2 - 1)) == 0)
			{
				recordBufferFull[recordIndex >> kRecordBufferLogSliceCount] = true;
				recordSignal->Trigger(1);
			}
		}

	#endif
}

#if C4XAUDIO

	void SoundMgr::SoundThread(const Thread *thread, void *cookie)
	{
		Thread::SetThreadName("C4-SD Mixer");

		SoundMgr *soundMgr = static_cast<SoundMgr *>(cookie);

		for (;;)
		{
			int32 index = soundMgr->soundSignal->Wait();
			if (index == 0)
			{
				break;
			}

			index--;
			soundMgr->MixSounds(soundMgr->playBuffer[index]);
			soundMgr->sourceVoice->SubmitSourceBuffer(&soundMgr->sourceBuffer[index]);
		}
	}

#elif C4MACOS

	OSStatus SoundMgr::SoundCallback(void *inRefCon, AudioUnitRenderActionFlags *ioActionFlags, const AudioTimeStamp *inTimeStamp, UInt32 inBusNumber, UInt32 inNumberFrames, AudioBufferList *ioData)
	{
		if (inNumberFrames > kOutputBufferFrameCount)
		{
			return (kAudioUnitErr_TooManyFramesToProcess);
		}

		SoundMgr *soundMgr = static_cast<SoundMgr *>(inRefCon);

		unsigned_int32 beginFrame = soundMgr->playFrameIndex;
		unsigned_int32 endFrame = beginFrame + inNumberFrames;

		if (beginFrame <= kOutputBufferFrameCount)
		{
			if (endFrame > kOutputBufferFrameCount)
			{
				soundMgr->MixSounds(soundMgr->playBuffer[1]);
			}

			MemoryMgr::CopyMemory(soundMgr->playBuffer[0] + beginFrame * 2, ioData->mBuffers[0].mData, (endFrame - beginFrame) * (sizeof(OutputSample) * 2));
		}
		else if (endFrame > kOutputBufferFrameCount * 2)
		{
			soundMgr->MixSounds(soundMgr->playBuffer[0]);
			unsigned_int32 sampleCount = (kOutputBufferFrameCount * 2 - beginFrame) * 2;
			OutputSample *output = static_cast<OutputSample *>(ioData->mBuffers[0].mData);
			MemoryMgr::CopyMemory(soundMgr->playBuffer[0] + beginFrame * 2, output, sampleCount * sizeof(OutputSample));

			endFrame -= kOutputBufferFrameCount * 2;
			MemoryMgr::CopyMemory(soundMgr->playBuffer[0], output + sampleCount, endFrame * 2 * sizeof(OutputSample));
		}
		else
		{
			MemoryMgr::CopyMemory(soundMgr->playBuffer[0] + beginFrame * 2, ioData->mBuffers[0].mData, (endFrame - beginFrame) * (sizeof(OutputSample) * 2));
		}

		soundMgr->playFrameIndex = endFrame;
		return (noErr);
	}

#elif C4LINUX

	void SoundMgr::SoundThread(const Thread *thread, void *cookie)
	{
		Thread::SetThreadName("C4-SD Mixer");

		SoundMgr *soundMgr = static_cast<SoundMgr *>(cookie);

		for (;;)
		{
			snd_pcm_wait(soundMgr->soundHandle, -1);

			if (soundMgr->soundExitFlag)
			{
				break;
			}

			soundMgr->MixSounds(soundMgr->playBuffer);

			int result = snd_pcm_writei(soundMgr->soundHandle, soundMgr->playBuffer, kOutputBufferFrameCount);
			if (result < 0)
			{
				if (soundMgr->soundExitFlag)
				{
					break;
				}

				snd_pcm_recover(soundMgr->soundHandle, result, true);
			}
		}
	}

#elif C4IOS //[ MOBILE

	// -- Mobile code hidden --

#elif C4PS4 //[ PS4

	// -- PS4 code hidden --

#elif C4PS3 //[ PS3

	// -- PS3 code hidden --

#endif //]

void SoundMgr::SetMasterVolume(float volume)
{
	masterVolume = volume;

	Sound *sound = loadedSoundList.First();
	while (sound)
	{
		sound->updateFlags |= kSoundUpdateVolume;
		sound = sound->Next();
	}
}

void SoundMgr::StopAllSounds(void)
{
	Sound *sound = loadedSoundList.First();
	while (sound)
	{
		SoundState state = sound->GetSoundState();
		if ((state >= kSoundDelaying) && (state <= kSoundPaused))
		{
			sound->Stop();
		}

		sound = sound->Next();
	}
}

void SoundMgr::PauseAllSounds(void)
{
	Sound *sound = loadedSoundList.First();
	while (sound)
	{
		sound->Pause();
		sound = sound->Next();
	}
}

void SoundMgr::ResumeAllSounds(void)
{
	Sound *sound = loadedSoundList.First();
	while (sound)
	{
		sound->Resume();
		sound = sound->Next();
	}
}

void SoundMgr::SetGlobalSoundSpeed(float speed)
{
	globalSoundSpeed = speed;
	unitDistanceFrameCount = (float) kSoundOutputSampleRate / speed;
}

void SoundMgr::SetListenerRoom(SoundRoom *room)
{
	if (listenerRoom != room)
	{
		listenerRoom = room;
		if (room)
		{
			room->outputRoom = nullptr;
			room->roomVolume[0] = 1.0F;
			room->roomVolume[1] = 1.0F;
		}
	}
}

void SoundMgr::StreamThread(const Thread *thread, void *cookie)
{
	Thread::SetThreadName("C4-SD Stream");

	SoundMgr *soundMgr = static_cast<SoundMgr *>(cookie);

	for (;;)
	{
		for (machine a = 0; a < kMaxSoundCount; a++)
		{
			Sound *sound = soundMgr->activeSoundTable[a];
			if (sound)
			{
				if (sound->soundState == kSoundPlaying)
				{
					if (sound->Streaming())
					{
						SoundStreamer *streamer = sound->GetSoundStreamer();
						StreamBufferHeader *buffer1 = streamer->GetStreamBuffer(0);
						StreamBufferHeader *buffer2 = streamer->GetStreamBuffer(1);

						if ((!buffer1->readyFlag) && (!buffer2->finalFlag))
						{
							sound->FillStreamBuffer(streamer, buffer1);
						}

						if ((!buffer2->readyFlag) && (!buffer1->finalFlag))
						{
							sound->FillStreamBuffer(streamer, buffer2);
						}
					}
				}
				else if (sound->mainReleaseFlag)
				{
					sound->streamReleaseFlag = true;
				}
			}
		}

		if (soundMgr->streamSignal->Wait() == 0)
		{
			break;
		}
	}
}

EngineResult SoundMgr::StartRecording(const char *name)
{
	#if C4RECORDABLE

		if (!recordFlag)
		{
			FileMgr::CreateDirectoryPath(name);

			String<kMaxFileNameLength> path(name);
			path += SoundResource::GetDescriptor()->GetExtension();

			new(recordFile) File;
			FileResult result = recordFile->Open(path, kFileCreate);
			if (result == kFileOkay)
			{
				WriteBigEndianU32(&recordHeader.riffTag, 'RIFF');
				WriteBigEndianU32(&recordHeader.waveTag, 'WAVE');
				WriteBigEndianU32(&recordHeader.fmtTag, 'fmt ');
				WriteLittleEndianU32(&recordHeader.fmtLength, 16);
				WriteLittleEndianU16(&recordHeader.format, WAVE_FORMAT_PCM);
				WriteLittleEndianU16(&recordHeader.numChannels, 2);
				WriteLittleEndianU32(&recordHeader.sampleRate, kSoundOutputSampleRate);
				WriteLittleEndianU32(&recordHeader.bytesPerSec, kSoundOutputSampleRate * 4);
				WriteLittleEndianU16(&recordHeader.blockAlign, 4);
				WriteLittleEndianU16(&recordHeader.bitsPerSample, 16);
				WriteBigEndianU32(&recordHeader.dataTag, 'data');

				recordHeader.fileLength = 0;
				recordHeader.dataLength = 0;
				recordFile->Write(&recordHeader, sizeof(WaveFileHeader));

				recordBufferFull[0] = false;
				recordBufferFull[1] = false;
				recordSliceIndex = 0;
				recordBuffer = new Sample[kRecordBufferSampleCount];

				new(recordSignal) Signal(2);
				new(recordThread) Thread(&RecordThread, this, 0, recordSignal);

				Thread::Fence();
				recordFlag = true;
			}
			else
			{
				recordFile->~File();
				return (result);
			}
		}

	#endif

	return (kEngineOkay);
}

void SoundMgr::StopRecording(void)
{
	#if C4RECORDABLE

		if (recordFlag)
		{
			recordFlag = false;
			Thread::Fence();

			recordThread->~Thread();
			recordSignal->~Signal();

			delete[] recordBuffer;

			int32 size = (int32) recordFile->GetPosition();
			WriteLittleEndianU32(&recordHeader.fileLength, size - 8);
			WriteLittleEndianU32(&recordHeader.dataLength, size - 44);

			recordFile->SetPosition(0);
			recordFile->Write(&recordHeader, sizeof(WaveFileHeader));

			recordFile->~File();
		}

	#endif
}

#if C4RECORDABLE

	void SoundMgr::RecordThread(const Thread *thread, void *cookie)
	{
		Thread::SetThreadName("C4-SD Record");

		SoundMgr *soundMgr = static_cast<SoundMgr *>(cookie);

		int32 parity = 0;
		for (;;)
		{
			int32 index = soundMgr->recordSignal->Wait();

			for (;;)
			{
				if (!soundMgr->recordBufferFull[parity])
				{
					break;
				}

				soundMgr->recordBufferFull[parity] = false;

				const char *buffer = reinterpret_cast<char *>(soundMgr->recordBuffer);
				soundMgr->recordFile->Write(buffer + (-parity & kRecordOutputBufferSize), kRecordOutputBufferSize);
				parity ^= 1;
			}

			if (index == 0)
			{
				break;
			}
		}
	}

#endif

int32 SoundMgr::AddSound(Sound *sound)
{
	int32	lowerPlayFrame, equalPlayFrame;

	int32 soundPriority = sound->GetSoundPriority();
	int32 lowerPriority = soundPriority;

	int32 lowerReplaceIndex = -1;
	int32 equalReplaceIndex = -1;

	const SoundResource *soundResource = sound->GetSoundResource();

	for (machine a = 0; a < kMaxSoundCount; a++)
	{
		Sound *activeSound = activeSoundTable[a];
		if (!activeSound)
		{
			activeSoundTable[a] = sound;
			return (a);
		}

		if ((!(activeSound->Streaming())) && (activeSound->GetLoopCount() != kSoundLoopInfinite))
		{
			int32 activePriority = activeSound->GetSoundPriority();
			int32 activePlayFrame = activeSound->GetPlayFrame();

			if (activePriority == soundPriority)
			{
				if ((activeSound->soundResource == soundResource) && (activePlayFrame > activeSound->sampleRate >> 1))
				{
					if ((equalReplaceIndex < 0) || (activePlayFrame > equalPlayFrame))
					{
						equalReplaceIndex = a;
						equalPlayFrame = activePlayFrame;
					}
				}
			}
			else if (activePriority == lowerPriority)
			{
				if (activePlayFrame > lowerPlayFrame)
				{
					lowerReplaceIndex = a;
					lowerPlayFrame = activePlayFrame;
					lowerPriority = activePriority;
				}
			}
			else if (activePriority < lowerPriority)
			{
				lowerReplaceIndex = a;
				lowerPlayFrame = activePlayFrame;
				lowerPriority = activePriority;
			}
		}
	}

	int32 replaceIndex = (lowerReplaceIndex >= 0) ? lowerReplaceIndex : equalReplaceIndex;
	if (replaceIndex >= 0)
	{
		Sound *activeSound = activeSoundTable[replaceIndex];
		activeSound->soundState = kSoundCompleted;
		activeSound->waitMixStamp = soundMixStamp + 1;
		activeSound->tableIndex = -1;

		activeSoundTable[replaceIndex] = sound;
		return (replaceIndex);
	}

	return (-1);
}

void SoundMgr::SoundTask(void)
{
	int32 dt = TheTimeMgr->GetSystemDeltaTime();
	float fdt = TheTimeMgr->GetSystemFloatDeltaTime();

	Sound *sound = loadedSoundList.First();
	while (sound)
	{
		Sound *next = sound->Next();

		if (!sound->mainReleaseFlag)
		{
			switch (sound->soundState)
			{
				case kSoundStopping:

					if (soundMixStamp - sound->waitMixStamp < 0)
					{
						break;
					}

					sound->soundState = kSoundStopped;
					// fall through

				case kSoundStopped:
				{
					int32 index = sound->tableIndex;
					if (index >= 0)
					{
						activeSoundTable[index] = nullptr;
						sound->tableIndex = -1;
					}

					break;
				}

				case kSoundDelaying:

					if ((sound->delayTime -= dt) <= 0)
					{
						sound->Play();
					}

					break;

				case kSoundPlaying:
				{
					if (sound->Streaming())
					{
						const SoundStreamer *streamer = sound->GetSoundStreamer();
						StreamBufferHeader *buffer1 = streamer->GetStreamBuffer(0);
						StreamBufferHeader *buffer2 = streamer->GetStreamBuffer(1);

						if ((!buffer1->readyFlag) || (!buffer2->readyFlag))
						{
							streamSignal->Trigger(1);
						}
					}

					if (sound->loopFlag)
					{
						sound->loopFlag = false;
						if (sound->loopProc)
						{
							(*sound->loopProc)(sound, sound->loopCookie);
						}
					}

					bool spatializedFlag = ((sound->GetSoundFlags() & kSoundSpatialized) != 0);
					bool updateVolumeFlag = ((sound->updateFlags & kSoundUpdateVolume) != 0) | spatializedFlag;
					bool updateFrequencyFlag = ((sound->updateFlags & kSoundUpdateFrequency) != 0) | spatializedFlag;

					unsigned_int32 variationState = sound->variationState;
					if (variationState != 0)
					{
						if (variationState & kSoundVaryingVolume)
						{
							updateVolumeFlag = true;
							if (!(variationState & kSoundVaryingVolumeComplete))
							{
								float target = sound->volumeVariationParams.targetValue;
								float delta = sound->volumeVariationParams.deltaValue;

								float volume = sound->soundProperty[kSoundVolume] + fdt * delta;
								if ((delta >= 0.0F) ? (volume >= target) : (volume <= target))
								{
									volume = target;
									sound->variationState = variationState | kSoundVaryingVolumeComplete;
									sound->mixFlag = false;
								}

								sound->soundProperty[kSoundVolume] = volume;
							}
							else if (sound->mixFlag)
							{
								Sound::VariationProc *proc = sound->volumeVariationParams.completionProc;
								if (proc)
								{
									(*proc)(sound, sound->volumeVariationParams.completionCookie);
								}

								if (variationState & kSoundVaryingVolumeStop)
								{
									sound->Stop();
								}
								else
								{
									sound->variationState &= ~kSoundVaryingVolumeMask;
								}
							}
						}

						if (variationState & kSoundVaryingFrequency)
						{
							updateFrequencyFlag = true;
							if (!(variationState & kSoundVaryingFrequencyComplete))
							{
								float target = sound->frequencyVariationParams.targetValue;
								float delta = sound->frequencyVariationParams.deltaValue;

								float frequency = sound->soundProperty[kSoundFrequency] + fdt * delta;
								if ((delta >= 0.0F) ? (frequency >= target) : (frequency <= target))
								{
									frequency = target;
									sound->variationState = variationState | kSoundVaryingFrequencyComplete;
									sound->mixFlag = false;
								}

								sound->soundProperty[kSoundFrequency] = frequency;
							}
							else if (sound->mixFlag)
							{
								Sound::VariationProc *proc = sound->frequencyVariationParams.completionProc;
								if (proc)
								{
									(*proc)(sound, sound->frequencyVariationParams.completionCookie);
								}

								if (variationState & kSoundVaryingFrequencyStop)
								{
									sound->Stop();
								}
								else
								{
									sound->variationState &= ~kSoundVaryingFrequencyMask;
								}
							}
						}
					}

					if (updateVolumeFlag)
					{
						sound->UpdateVolume();
					}

					if (updateFrequencyFlag)
					{
						sound->UpdateFrequency();
					}

					if (sound->updateFlags & kSoundUpdateReflections)
					{
						sound->UpdateReflections();
					}

					sound->updateFlags = 0;
					break;
				}

				case kSoundCompleted:

					if (soundMixStamp - sound->waitMixStamp >= 0)
					{
						sound->soundState = kSoundStopped;

						int32 index = sound->tableIndex;
						if (index >= 0)
						{
							activeSoundTable[index] = nullptr;
							sound->tableIndex = -1;
						}

						bool persistent = ((sound->soundFlags & kSoundPersistent) != 0);
						sound->CallCompletionProc();

						if (!persistent)
						{
							delete sound;
						}
					}

					break;
			}
		}
		else if (sound->streamReleaseFlag)
		{
			if (sound->mixerReleaseFlag)
			{
				int32 index = sound->tableIndex;
				if (index >= 0)
				{
					activeSoundTable[index] = nullptr;
				}

				delete sound;
			}
		}
		else
		{
			streamSignal->Trigger(1);
		}

		sound = next;
	}

	SoundRoom *room = releasedRoomList.First();
	while (room)
	{
		SoundRoom *next = room->Next();

		if (room->tableIndex < 0)
		{
			delete room;
		}

		room = next;
	}
}

// ZYUQURM
