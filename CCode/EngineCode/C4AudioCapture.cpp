 

#include "C4AudioCapture.h"
#include "C4Time.h"


using namespace C4;


namespace
{
	enum
	{
		kSoundInputSampleRate	= 8000,
		kAudioCaptureThreshold	= 2048
	};


	// DCT matrix for N = 16, multiplied by sqrt(2/N), 12-bit fraction.

	alignas(128) const int16 blockMatrix[kAudioMessageBlockSize * kAudioMessageBlockSize] =
	{
		0x05A8,  0x05A1,  0x058C,  0x0569,  0x0539,  0x04FD,  0x04B4,  0x045F,  0x0400,  0x0396,  0x0324,  0x02AA,  0x022A,  0x01A4,  0x011A,  0x008D,
		0x05A8,  0x0569,  0x04B4,  0x0396,  0x022A,  0x008D, -0x011A, -0x02AA, -0x0400, -0x04FD, -0x058C, -0x05A1, -0x0539, -0x045F, -0x0324, -0x01A4,
		0x05A8,  0x04FD,  0x0324,  0x008D, -0x022A, -0x045F, -0x058C, -0x0569, -0x0400, -0x01A4,  0x011A,  0x0396,  0x0539,  0x05A1,  0x04B4,  0x02AA,
		0x05A8,  0x045F,  0x011A, -0x02AA, -0x0539, -0x0569, -0x0324,  0x008D,  0x0400,  0x05A1,  0x04B4,  0x01A4, -0x022A, -0x04FD, -0x058C, -0x0396,
		0x05A8,  0x0396, -0x011A, -0x04FD, -0x0539, -0x01A4,  0x0324,  0x05A1,  0x0400, -0x008D, -0x04B4, -0x0569, -0x022A,  0x02AA,  0x058C,  0x045F,
		0x05A8,  0x02AA, -0x0324, -0x05A1, -0x022A,  0x0396,  0x058C,  0x01A4, -0x0400, -0x0569, -0x011A,  0x045F,  0x0539,  0x008D, -0x04B4, -0x04FD,
		0x05A8,  0x01A4, -0x04B4, -0x045F,  0x022A,  0x05A1,  0x011A, -0x04FD, -0x0400,  0x02AA,  0x058C,  0x008D, -0x0539, -0x0396,  0x0324,  0x0569,
		0x05A8,  0x008D, -0x058C, -0x01A4,  0x0539,  0x02AA, -0x04B4, -0x0396,  0x0400,  0x045F, -0x0324, -0x04FD,  0x022A,  0x0569, -0x011A, -0x05A1,
		0x05A8, -0x008D, -0x058C,  0x01A4,  0x0539, -0x02AA, -0x04B4,  0x0396,  0x0400, -0x045F, -0x0324,  0x04FD,  0x022A, -0x0569, -0x011A,  0x05A1,
		0x05A8, -0x01A4, -0x04B4,  0x045F,  0x022A, -0x05A1,  0x011A,  0x04FD, -0x0400, -0x02AA,  0x058C, -0x008D, -0x0539,  0x0396,  0x0324, -0x0569,
		0x05A8, -0x02AA, -0x0324,  0x05A1, -0x022A, -0x0396,  0x058C, -0x01A4, -0x0400,  0x0569, -0x011A, -0x045F,  0x0539, -0x008D, -0x04B4,  0x04FD,
		0x05A8, -0x0396, -0x011A,  0x04FD, -0x0539,  0x01A4,  0x0324, -0x05A1,  0x0400,  0x008D, -0x04B4,  0x0569, -0x022A, -0x02AA,  0x058C, -0x045F,
		0x05A8, -0x045F,  0x011A,  0x02AA, -0x0539,  0x0569, -0x0324, -0x008D,  0x0400, -0x05A1,  0x04B4, -0x01A4, -0x022A,  0x04FD, -0x058C,  0x0396,
		0x05A8, -0x04FD,  0x0324, -0x008D, -0x022A,  0x045F, -0x058C,  0x0569, -0x0400,  0x01A4,  0x011A, -0x0396,  0x0539, -0x05A1,  0x04B4, -0x02AA,
		0x05A8, -0x0569,  0x04B4, -0x0396,  0x022A, -0x008D, -0x011A,  0x02AA, -0x0400,  0x04FD, -0x058C,  0x05A1, -0x0539,  0x045F, -0x0324,  0x01A4,
		0x05A8, -0x05A1,  0x058C, -0x0569,  0x0539, -0x04FD,  0x04B4, -0x045F,  0x0400, -0x0396,  0x0324, -0x02AA,  0x022A, -0x01A4,  0x011A, -0x008D
	};

	alignas(16) const unsigned_int8 blockNormalizer[kAudioMessageBlockSize] =
	{
		20, 20, 20, 20, 21, 21, 21, 21, 22, 22, 23, 23, 24, 24, 24, 24
	};

	alignas(16) const unsigned_int8 blockDenormalizer[kAudioMessageBlockSize] =
	{
		0, 0, 0, 0, 1, 1, 1, 1, 2, 2, 3, 3, 4, 4, 4, 4
	};
}


AudioCaptureMgr *C4::TheAudioCaptureMgr = nullptr;


namespace C4
{
	template <> AudioCaptureMgr Manager<AudioCaptureMgr>::managerObject(0);
	template <> AudioCaptureMgr **Manager<AudioCaptureMgr>::managerPointer = &TheAudioCaptureMgr;

	template <> const char *const Manager<AudioCaptureMgr>::resultString[] =
	{
		nullptr,
		"Audio capture unavailable"
	};

	template <> const unsigned_int32 Manager<AudioCaptureMgr>::resultIdentifier[] =
	{
		0, 'AVAL'
	};

	template class Manager<AudioCaptureMgr>;
}


AudioMessage::AudioMessage() : Message(kMessageAudio, kMessageUnreliable | kMessageUnordered)
{
}

AudioMessage::AudioMessage(unsigned_int32 time, int32 count, const Sample *data) : Message(kMessageAudio, kMessageUnreliable | kMessageUnordered)
{
	playerKey = TheMessageMgr->GetLocalPlayerKey();
	timeStamp = time;
	audioBlockCount = count;

	const int16 *matrix = blockMatrix;
	int8 *output = audioData;

	for (machine a = 0; a < count; a++)
	{
		int32 y = data[0];
		for (unsigned_machine i = 1; i < kAudioMessageBlockSize; i++)
		{
			y += data[i];
		}

		output[1] = (int8) Min(Max(y >> 10, -128), 127);

		for (unsigned_machine j = 1; j < kAudioMessageBlockSize; j++)
		{
			y = data[0] * matrix[j];
			for (machine i = 1; i < kAudioMessageBlockSize; i++)
			{ 
				y += data[i] * matrix[i * kAudioMessageBlockSize + j];
			}
 
			output[j + 1] = (int8) Min(Max(y >> blockNormalizer[j], -128), 127);
		} 

		unsigned_machine size = 1;
		for (unsigned_machine j = kAudioMessageBlockSize; j > 1; j--) 
		{
			if (Abs(output[j]) > 1) 
			{ 
				size = j;
				break;
			}
		} 

		output[0] = (int8) size;
		output += size + 1;

		data += kAudioMessageBlockSize;
	}

	audioDataSize = (unsigned_int32) (output - audioData);
}

AudioMessage::~AudioMessage()
{
}

void AudioMessage::Compress(Compressor& data) const
{
	data << (int16) playerKey;
	data << timeStamp;

	data << (unsigned_int8) audioBlockCount;
	data << (unsigned_int16) audioDataSize;
	data.Write(audioData, audioDataSize);
}

bool AudioMessage::Decompress(Decompressor& data)
{
	int16			key;
	unsigned_int8	count;
	unsigned_int16	size;

	data >> key;
	playerKey = key;

	data >> timeStamp;
	data >> count;
	data >> size;

	if (((unsigned_int32) (count - 1) < kAudioMessageBlockCount) && ((unsigned_int32) (size - 1) < kMaxAudioMessageDataSize))
	{
		audioBlockCount = count;
		audioDataSize = size;
		data.Read(audioData, size);
		return (true);
	}

	return (false);
}

bool AudioMessage::HandleMessage(Player *sender) const
{
	Player *localPlayer = TheMessageMgr->GetLocalPlayer();

	if (TheMessageMgr->Server())
	{
		playerKey = sender->GetPlayerKey();

		const Channel *channel = sender->GetFirstOutgoingEdge();
		while (channel)
		{
			if (channel->Enabled())
			{
				Player *player = channel->GetFinishElement();
				if (player != localPlayer)
				{
					player->SendMessage(*this);
				}
				else
				{
					ChatStreamer *streamer = localPlayer->GetChatStreamer();
					if (!streamer)
					{
						streamer = localPlayer->CreateChatStreamer();
					}

					streamer->ReceiveAudioMessage(this);
				}
			}

			channel = channel->GetNextOutgoingEdge();
		}
	}
	else
	{
		Player *player = TheMessageMgr->GetPlayer(playerKey);
		if ((player) && (player != localPlayer))
		{
			ChatStreamer *streamer = player->GetChatStreamer();
			if (!streamer)
			{
				streamer = player->CreateChatStreamer();
			}

			streamer->ReceiveAudioMessage(this);
		}
	}

	return (true);
}

unsigned_int32 AudioMessage::GetAudioData(Sample *data, unsigned_int32 offset) const
{
	const int16 *matrix = blockMatrix;
	const int8 *input = audioData;

	int32 count = audioBlockCount;
	for (machine a = 0; a < count; a++)
	{
		int32 size = *input++;
		for (machine i = 0; i < kAudioMessageBlockSize; i++)
		{
			int32 x = input[0] << 10;
			for (machine j = 1; j < size; j++)
			{
				x += (input[j] << blockDenormalizer[j]) * matrix[i * kAudioMessageBlockSize + j];
			}

			WriteLittleEndianS16(&data[offset & (kChatRingBufferFrameCount - 1)], (Sample) Min(Max(x >> 4, -32768), 32767));
			offset++;
		}

		input += size;
	}

	return ((unsigned_int32) (input - audioData));
}


ChatStreamer::ChatStreamer()
{
	AllocateStreamMemory(kChatRingBufferSize, kCaptureBufferSize);
}

ChatStreamer::~ChatStreamer()
{
}

SoundResult ChatStreamer::StartStream(void)
{
	chatStreamerState = 0;
	baseTimeStamp = 0;
	dataTimeStamp = 0;
	playTimeStamp = 0;

	MemoryMgr::ClearMemory(GetWorkBuffer(), kChatRingBufferSize);

	SetStreamChannelCount(1);
	SetStreamSampleRate(kSoundInputSampleRate);

	Pause();
	return (kSoundOkay);
}

bool ChatStreamer::FillBuffer(unsigned_int32 bufferSize, Sample *buffer, int32 *count)
{
	int32 frameCount = bufferSize / sizeof(Sample);
	*count = frameCount;

	unsigned_int32 timeStamp = playTimeStamp;
	Sample *sample = reinterpret_cast<Sample *>(GetWorkBuffer()) + ((timeStamp - baseTimeStamp) & (kChatRingBufferFrameCount - 1));

	for (machine a = 0; a < frameCount; a++)
	{
		buffer[a] = sample[a];
		sample[a] = 0;
	}

	playTimeStamp = timeStamp + kCaptureBufferFrameCount;
	if ((int32) (playTimeStamp - dataTimeStamp) >= kChatRingBufferFrameCount * 4)
	{
		Pause();
	}

	return (true);
}

void ChatStreamer::ReceiveAudioMessage(const AudioMessage *message)
{
	unsigned_int32	size;

	unsigned_int32 timeStamp = message->GetTimeStamp();

	if (chatStreamerState == 0)
	{
		chatStreamerState = 1;
		baseTimeStamp = timeStamp;
		dataTimeStamp = timeStamp;
		playTimeStamp = timeStamp;

		size = message->GetAudioData(reinterpret_cast<Sample *>(GetWorkBuffer()));
	}
	else
	{
		if ((unsigned_int32) (timeStamp - playTimeStamp) < (unsigned_int32) (kChatRingBufferFrameCount - kCaptureBufferFrameCount - message->GetSampleCount()))
		{
			size = message->GetAudioData(reinterpret_cast<Sample *>(GetWorkBuffer()), timeStamp - baseTimeStamp);
		}
		else
		{
			baseTimeStamp += timeStamp - playTimeStamp;
			playTimeStamp = timeStamp;
			size = message->GetAudioData(reinterpret_cast<Sample *>(GetWorkBuffer()), timeStamp);
		}

		if ((int32) (timeStamp - dataTimeStamp) > 0)
		{
			dataTimeStamp = timeStamp;
		}
	}

	TheAudioCaptureMgr->AddChatReceiveSize(size);

	Resume();
}


AudioCaptureMgr::AudioCaptureMgr(int)
{
}

AudioCaptureMgr::~AudioCaptureMgr()
{
}

EngineResult AudioCaptureMgr::Construct(void)
{
	#if C4WINDOWS

		directSoundCapture = nullptr;

	#elif C4MACOS

		audioUnit = nullptr;

	#elif C4LINUX

		captureHandle = nullptr;

	#elif C4IOS //[ MOBILE

		// -- Mobile code hidden --

	#elif C4PS4 //[ PS4

		// -- PS4 code hidden --

	#elif C4PS3 //[ PS3

		// -- PS3 code hidden --

	#endif //]

	audioCaptureState = 0;

	audioAvailable[0] = false;
	audioAvailable[1] = false;
	audioBufferParity = 0;
	audioBuffer = nullptr;

	audioCaptureProc = nullptr;
	audioCaptureCookie = nullptr;

	chatReceiveTime = 0;
	chatReceiveFrame = 0;
	for (machine a = 0; a < kAudioInfoFrameCount; a++)
	{
		chatReceiveInfo[a].time = 0;
		chatReceiveInfo[a].size = 0;
	}

	return (kAudioCaptureOkay);
}

void AudioCaptureMgr::Destruct(void)
{
	if (audioBuffer)
	{
		StopAudioCapture();
	}

	Terminate();
}

AudioCaptureResult AudioCaptureMgr::Initialize(void)
{
	#if C4WINDOWS

		if (!directSoundCapture)
		{
			DSCBUFFERDESC			desc;
			WAVEFORMATEX			format;
			DSBPOSITIONNOTIFY		notifyData[2];
			IDirectSoundNotify		*notifyInterface;

			if (DirectSoundCaptureCreate8(&DSDEVID_DefaultVoiceCapture, &directSoundCapture, nullptr) != DS_OK)
			{
				return (kAudioCaptureUnavailable);
			}

			desc.dwSize = sizeof(DSCBUFFERDESC);
			desc.dwFlags = 0;
			desc.dwBufferBytes = kCaptureBufferSize * 2;
			desc.dwReserved = 0;
			desc.lpwfxFormat = &format;
			desc.dwFXCount = 0;
			desc.lpDSCFXDesc = nullptr;

			format.wFormatTag = WAVE_FORMAT_PCM;
			format.nChannels = 1;
			format.nSamplesPerSec = kSoundInputSampleRate;
			format.nAvgBytesPerSec = kSoundInputSampleRate * 2;
			format.nBlockAlign = 2;
			format.wBitsPerSample = 16;
			format.cbSize = 0;

			if (directSoundCapture->CreateCaptureBuffer(&desc, &captureBuffer, nullptr) != DS_OK)
			{
				directSoundCapture->Release();
				directSoundCapture = nullptr;
				return (kAudioCaptureUnavailable);
			}

			if (captureBuffer->QueryInterface(IID_IDirectSoundNotify, (void **) &notifyInterface) != S_OK)
			{
				captureBuffer->Release();
				directSoundCapture->Release();
				directSoundCapture = nullptr;
				return (kAudioCaptureUnavailable);
			}

			for (machine a = 0; a < 3; a++)
			{
				captureEvent[a] = CreateEventA(nullptr, false, false, nullptr);
			}

			notifyData[0].dwOffset = kCaptureBufferSize;
			notifyData[0].hEventNotify = captureEvent[0];
			notifyData[1].dwOffset = 0;
			notifyData[1].hEventNotify = captureEvent[1];

			notifyInterface->SetNotificationPositions(2, notifyData);
			notifyInterface->Release();

			captureThread = nullptr;
		}

	#elif C4MACOS

		if (!audioUnit)
		{
			AudioDeviceID					inputDeviceID;
			AudioObjectPropertyAddress		propertyAddress;
			AudioComponentDescription		componentDescription;
			AudioStreamBasicDescription		streamFormat;
			AURenderCallbackStruct			callbackProperty;
			Float64							sampleRate;

			propertyAddress.mSelector = kAudioHardwarePropertyDefaultInputDevice;
			propertyAddress.mScope = kAudioObjectPropertyScopeGlobal;
			propertyAddress.mElement = kAudioObjectPropertyElementMaster;

			UInt32 size = sizeof(AudioDeviceID);
			if (AudioObjectGetPropertyData(kAudioObjectSystemObject, &propertyAddress, 0, nullptr, &size, &inputDeviceID) != noErr)
			{
				return (kAudioCaptureUnavailable);
			}

			componentDescription.componentType = kAudioUnitType_Output;
			componentDescription.componentSubType = kAudioUnitSubType_HALOutput;
			componentDescription.componentManufacturer = kAudioUnitManufacturer_Apple;
			componentDescription.componentFlags = 0;
			componentDescription.componentFlagsMask = 0;

			AudioComponent component = AudioComponentFindNext(nullptr, &componentDescription);
			if ((!component) || (AudioComponentInstanceNew(component, &audioUnit) != noErr))
			{
				audioUnit = nullptr;
				return (kAudioCaptureUnavailable);
			}

			UInt32 enable = 1;
			UInt32 disable = 0;
			AudioUnitSetProperty(audioUnit, kAudioOutputUnitProperty_EnableIO, kAudioUnitScope_Input, 1, &enable, 4);
			AudioUnitSetProperty(audioUnit, kAudioOutputUnitProperty_EnableIO, kAudioUnitScope_Output, 0, &disable, 4);
			AudioUnitSetProperty(audioUnit, kAudioOutputUnitProperty_CurrentDevice, kAudioUnitScope_Global, 0, &inputDeviceID, sizeof(AudioDeviceID));

			size = sizeof(Float64);
			propertyAddress.mSelector = kAudioDevicePropertyNominalSampleRate;
			propertyAddress.mScope = kAudioDevicePropertyScopeInput;
			AudioObjectGetPropertyData(inputDeviceID, &propertyAddress, 0, nullptr, &size, &sampleRate);

			inputBufferFrequency = (Fixed) ((float) sampleRate / (float) kSoundInputSampleRate * 65536.0F);

			size = 4;
			propertyAddress.mSelector = kAudioDevicePropertyBufferFrameSize;
			AudioObjectGetPropertyData(inputDeviceID, &propertyAddress, 0, nullptr, &size, &inputBufferFrameCount);

			unsigned_int32 maxInputBufferFrameCount = (kCaptureBufferFrameCount * inputBufferFrequency) >> 16;
			if (inputBufferFrameCount > maxInputBufferFrameCount)
			{
				inputBufferFrameCount = maxInputBufferFrameCount;
				AudioObjectSetPropertyData(inputDeviceID, &propertyAddress, 0, nullptr, 4, &maxInputBufferFrameCount);
			}

			streamFormat.mSampleRate = sampleRate;
			streamFormat.mFormatID = kAudioFormatLinearPCM;
			streamFormat.mFormatFlags = kAudioFormatFlagIsFloat;
			streamFormat.mBytesPerPacket = 4;
			streamFormat.mFramesPerPacket = 1;
			streamFormat.mBytesPerFrame = 4;
			streamFormat.mChannelsPerFrame = 1;
			streamFormat.mBitsPerChannel = 32;
			streamFormat.mReserved = 0;
			AudioUnitSetProperty(audioUnit, kAudioUnitProperty_StreamFormat, kAudioUnitScope_Output, 1, &streamFormat, sizeof(AudioStreamBasicDescription));

			callbackProperty.inputProc = &CaptureCallback;
			callbackProperty.inputProcRefCon = this;
			AudioUnitSetProperty(audioUnit, kAudioOutputUnitProperty_SetInputCallback, kAudioUnitScope_Global, 0, &callbackProperty, sizeof(AURenderCallbackStruct));

			if (AudioUnitInitialize(audioUnit) != noErr)
			{
				AudioComponentInstanceDispose(audioUnit);
				audioUnit = nullptr;
				return (kAudioCaptureUnavailable);
			}
		}

	#elif C4LINUX

		if (!captureHandle)
		{
			snd_pcm_hw_params_t		*hwparams;

			if (snd_pcm_open(&captureHandle, "default", SND_PCM_STREAM_CAPTURE, 0) != 0)
			{
				captureHandle = nullptr;
				return (kAudioCaptureUnavailable);
			}

			snd_pcm_hw_params_malloc(&hwparams);
			snd_pcm_hw_params_any(captureHandle, hwparams);

			snd_pcm_hw_params_set_access(captureHandle, hwparams, SND_PCM_ACCESS_RW_INTERLEAVED);
			snd_pcm_hw_params_set_format(captureHandle, hwparams, SND_PCM_FORMAT_S16_LE);
			snd_pcm_hw_params_set_rate(captureHandle, hwparams, kSoundInputSampleRate, 0);
			snd_pcm_hw_params_set_channels(captureHandle, hwparams, 1);

			int error = snd_pcm_hw_params(captureHandle, hwparams);
			snd_pcm_hw_params_free(hwparams);

			if (error < 0)
			{
				snd_pcm_close(captureHandle);

				captureHandle = nullptr;
				return (kAudioCaptureUnavailable);
			}
		}

	#elif C4IOS //[ MOBILE

		// -- Mobile code hidden --

	#elif C4PS4 //[ PS4

		// -- PS4 code hidden --

	#elif C4PS3 //[ PS3

		// -- PS3 code hidden --

	#endif //]

	return (kAudioCaptureOkay);
}

void AudioCaptureMgr::Terminate(void)
{
	#if C4WINDOWS

		if (directSoundCapture)
		{
			if (captureThread)
			{
				captureBuffer->Stop();

				SetEvent(captureEvent[2]);
				delete captureThread;
			}

			for (machine a = 2; a >= 0; a--)
			{
				CloseHandle(captureEvent[a]);
			}

			captureBuffer->Release();
			directSoundCapture->Release();
		}

	#elif C4MACOS

		if (audioUnit)
		{
			AudioComponentInstanceDispose(audioUnit);
		}

	#elif C4LINUX

		if (captureHandle)
		{
			captureExitFlag = true;
			delete captureThread;

			snd_pcm_close(captureHandle);
		}

	#elif C4IOS //[ MOBILE

		// -- Mobile code hidden --

	#elif C4PS4 //[ PS4

		// -- PS4 code hidden --

	#elif C4PS3 //[ PS3

		// -- PS3 code hidden --

	#endif //]
}

AudioCaptureResult AudioCaptureMgr::StartAudioCapture(bool paused)
{
	AudioCaptureResult result = Initialize();
	if (result != kAudioCaptureOkay)
	{
		return (result);
	}

	if (audioCaptureState == 0)
	{
		audioCaptureState = (paused) ? kAudioCaptureRecording | kAudioCapturePaused : kAudioCaptureRecording;
		baseTimeStamp = 0;

		audioBuffer = new Sample[kCaptureBufferFrameCount * 2];

		#if C4WINDOWS

			captureThread = new Thread(&CaptureThread, this);
			captureThread->SetThreadPriority(kThreadPriorityCritical);

			if (!paused)
			{
				captureBuffer->Start(DSCBSTART_LOOPING);
			}

		#elif C4MACOS

			inputBuffer = new float[inputBufferFrameCount];

			audioBufferList.mNumberBuffers = 1;
			audioBufferList.mBuffers[0].mNumberChannels = 1;
			audioBufferList.mBuffers[0].mDataByteSize = inputBufferFrameCount * sizeof(float);
			audioBufferList.mBuffers[0].mData = inputBuffer;

			audioWritePosition = 0;
			if (!paused)
			{
				AudioOutputUnitStart(audioUnit);
			}

		#elif C4LINUX

			captureExitFlag = false;
			captureThread = new Thread(&CaptureThread, this);
			captureThread->SetThreadPriority(kThreadPriorityCritical);

			if (!paused)
			{
				snd_pcm_start(captureHandle);
			}

		#elif C4IOS //[ MOBILE

			// -- Mobile code hidden --

		#elif C4PS4 //[ PS4

			// -- PS4 code hidden --

		#elif C4PS3 //[ PS3

			// -- PS3 code hidden --

		#endif //]
	}

	return (kAudioCaptureOkay);
}

void AudioCaptureMgr::StopAudioCapture(void)
{
	if (audioCaptureState & kAudioCaptureRecording)
	{
		audioCaptureState = 0;

		#if C4WINDOWS

			captureBuffer->Stop();

			SetEvent(captureEvent[2]);
			delete captureThread;
			captureThread = nullptr;

		#elif C4MACOS

			AudioOutputUnitStop(audioUnit);

			delete[] inputBuffer;

		#elif C4LINUX

			snd_pcm_drop(captureHandle);

			captureExitFlag = true;
			delete captureThread;
			captureThread = nullptr;

		#elif C4IOS //[ MOBILE

			// -- Mobile code hidden --

		#elif C4PS4 //[ PS4

			// -- PS4 code hidden --

		#elif C4PS3 //[ PS3

			// -- PS3 code hidden --

		#endif //]

		audioAvailable[0] = false;
		audioAvailable[1] = false;
		audioBufferParity = 0;

		delete[] audioBuffer;
		audioBuffer = nullptr;
	}
}

void AudioCaptureMgr::PauseAudioCapture(void)
{
	unsigned_int32 state = audioCaptureState;
	if (state == kAudioCaptureRecording)
	{
		audioCaptureState = state | kAudioCapturePaused;

		#if C4WINDOWS

			captureBuffer->Stop();

		#elif C4MACOS

			AudioOutputUnitStop(audioUnit);

		#elif C4LINUX

			snd_pcm_drop(captureHandle);

		#elif C4IOS //[ MOBILE

			// -- Mobile code hidden --

		#elif C4PS4 //[ PS4

			// -- PS4 code hidden --

		#elif C4PS3 //[ PS3

			// -- PS3 code hidden --

		#endif //]
	}
}

void AudioCaptureMgr::ResumeAudioCapture(void)
{
	unsigned_int32 state = audioCaptureState;
	if (state & kAudioCapturePaused)
	{
		audioCaptureState = state & ~kAudioCapturePaused;

		#if C4WINDOWS

			captureBuffer->Start(DSCBSTART_LOOPING);

		#elif C4MACOS

			AudioOutputUnitStart(audioUnit);

		#elif C4LINUX

			snd_pcm_start(captureHandle);

		#elif C4IOS //[ MOBILE

			// -- Mobile code hidden --

		#elif C4PS4 //[ PS4

			// -- PS4 code hidden --

		#elif C4PS3 //[ PS3

			// -- PS3 code hidden --

		#endif //]
	}
}

#if C4WINDOWS

	void AudioCaptureMgr::CaptureThread(const Thread *thread, void *cookie)
	{
		Thread::SetThreadName("C4-AC Capture");

		AudioCaptureMgr *audioCaptureMgr = static_cast<AudioCaptureMgr *>(cookie);

		for (;;)
		{
			void	*audioPtr1;
			void	*audioPtr2;
			DWORD	audioBytes1;
			DWORD	audioBytes2;

			unsigned_int32 index = WaitForMultipleObjectsEx(3, audioCaptureMgr->captureEvent, false, INFINITE, false) - WAIT_OBJECT_0;
			if (index == 2)
			{
				break;
			}

			if (!audioCaptureMgr->audioAvailable[index])
			{
				IDirectSoundCaptureBuffer *captureBuffer = audioCaptureMgr->captureBuffer;
				captureBuffer->Lock(index * kCaptureBufferSize, kCaptureBufferSize, &audioPtr1, &audioBytes1, &audioPtr2, &audioBytes2, 0);

				const Sample *source = static_cast<Sample *>(audioPtr1);
				Sample *destin = audioCaptureMgr->audioBuffer + index * kCaptureBufferFrameCount;

				int32 dmin = source[0];
				int32 dmax = dmin;
				destin[0] = (Sample) dmin;

				for (machine a = 1; a < kCaptureBufferFrameCount; a++)
				{
					int32 d = source[a];
					destin[a] = (Sample) d;
					dmin = Min(dmin, d);
					dmax = Max(dmax, d);
				}

				captureBuffer->Unlock(audioPtr1, audioBytes1, audioPtr2, audioBytes2);

				audioCaptureMgr->audioAboveThreshold[index] = ((dmax - dmin) > kAudioCaptureThreshold);

				Thread::Fence();
				audioCaptureMgr->audioAvailable[index] = true;
			}
		}
	}

#elif C4MACOS

	OSStatus AudioCaptureMgr::CaptureCallback(void *inRefCon, AudioUnitRenderActionFlags *ioActionFlags, const AudioTimeStamp *inTimeStamp, UInt32 inBusNumber, UInt32 inNumberFrames, AudioBufferList *ioData)
	{
		AudioCaptureMgr *audioCaptureMgr = static_cast<AudioCaptureMgr *>(inRefCon);

		if (inNumberFrames > audioCaptureMgr->inputBufferFrameCount)
		{
			return (kAudioUnitErr_TooManyFramesToProcess);
		}

		OSStatus result = AudioUnitRender(audioCaptureMgr->audioUnit, ioActionFlags, inTimeStamp, inBusNumber, inNumberFrames, &audioCaptureMgr->audioBufferList);
		if (result == noErr)
		{
			unsigned_int32 offset = audioCaptureMgr->audioWritePosition;
			unsigned_int32 index = offset / kCaptureBufferFrameCount;

			if (!audioCaptureMgr->audioAvailable[index])
			{
				const float *source = audioCaptureMgr->inputBuffer;
				Sample *destin = audioCaptureMgr->audioBuffer;

				unsigned_int32 ds = audioCaptureMgr->inputBufferFrequency;
				int32 frameCount = (inNumberFrames << 16) / ds;

				for (machine a = 0; a < frameCount; a++)
				{
					destin[offset] = (Sample) (source[(a * ds) >> 16] * 32767.0F);
					offset = (offset + 1) & (kCaptureBufferFrameCount * 2 - 1);
				}

				audioCaptureMgr->audioWritePosition = offset;
				if (offset / kCaptureBufferFrameCount != index)
				{
					const Sample *sample = audioCaptureMgr->audioBuffer + index * kCaptureBufferFrameCount;
					int32 dmin = sample[0];
					int32 dmax = dmin;

					for (machine a = 1; a < kCaptureBufferFrameCount; a++)
					{
						int32 d = sample[a];
						dmin = Min(dmin, d);
						dmax = Max(dmax, d);
					}

					audioCaptureMgr->audioAboveThreshold[index] = ((dmax - dmin) > kAudioCaptureThreshold);

					Thread::Fence();
					audioCaptureMgr->audioAvailable[index] = true;
				}
			}
		}

		return (result);
	}

#elif C4LINUX

	void AudioCaptureMgr::CaptureThread(const Thread *thread, void *cookie)
	{
		Thread::SetThreadName("C4-AC Capture");

		AudioCaptureMgr *audioCaptureMgr = static_cast<AudioCaptureMgr *>(cookie);

		for (machine index = 0;; index ^= 1)
		{
			snd_pcm_wait(audioCaptureMgr->captureHandle, kCaptureBufferFrameCount * 1000 / kSoundInputSampleRate);
			if (audioCaptureMgr->captureExitFlag)
			{
				break;
			}

			Sample *sample = audioCaptureMgr->audioBuffer + index * kCaptureBufferFrameCount;

			int result = snd_pcm_readi(audioCaptureMgr->captureHandle, sample, kCaptureBufferFrameCount);
			if (result > 0)
			{
				int32 dmin = sample[0];
				int32 dmax = dmin;

				for (machine a = 1; a < kCaptureBufferFrameCount; a++)
				{
					int32 d = sample[a];
					dmin = Min(dmin, d);
					dmax = Max(dmax, d);
				}

				audioCaptureMgr->audioAboveThreshold[index] = ((dmax - dmin) > kAudioCaptureThreshold);

				Thread::Fence();
				audioCaptureMgr->audioAvailable[index] = true;
			}
			else
			{
				if (audioCaptureMgr->captureExitFlag)
				{
					break;
				}

				snd_pcm_recover(audioCaptureMgr->captureHandle, result, true);
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

unsigned_int32 AudioCaptureMgr::GetChatReceiveRate(void) const
{
	unsigned_int32 size = chatReceiveInfo[0].size;
	for (machine a = 1; a < kAudioInfoFrameCount; a++)
	{
		size += chatReceiveInfo[a].size;
	}

	unsigned_int32 frame = chatReceiveFrame;
	unsigned_int32 time = chatReceiveInfo[frame].time - chatReceiveInfo[(frame + 1) & (kAudioInfoFrameCount - 1)].time;

	return (size * 1000 / Max(time, 1));
}

void AudioCaptureMgr::AudioCaptureTask(void)
{
	unsigned_int32 parity = audioBufferParity;
	for (machine a = 0; a < 2; a++)
	{
		if (audioAvailable[parity])
		{
			unsigned_int32 timeStamp = baseTimeStamp;
			const Sample *buffer = audioBuffer + parity * kCaptureBufferFrameCount;

			if (audioAboveThreshold[parity])
			{
				for (unsigned_int32 frame = 0; frame < kCaptureBufferFrameCount; frame += kAudioMessageFrameCount)
				{
					int32 count = Min(kCaptureBufferFrameCount - frame, kAudioMessageFrameCount) / kAudioMessageBlockSize;
					TheMessageMgr->SendMessage(kPlayerServer, AudioMessage(timeStamp + frame, count, buffer + frame));
				}
			}

			if (audioCaptureProc)
			{
				(*audioCaptureProc)(timeStamp, buffer, kCaptureBufferFrameCount, audioCaptureCookie);
			}

			audioAvailable[parity] = false;
			audioBufferParity = (unsigned_int8) (parity ^ 1);
			baseTimeStamp = timeStamp + kCaptureBufferFrameCount;
			break;
		}

		parity ^= 1;
	}

	unsigned_int32 time = chatReceiveTime + TheTimeMgr->GetSystemDeltaTime();
	chatReceiveTime = time;

	unsigned_int32 frame = (chatReceiveFrame + 1) & (kAudioInfoFrameCount - 1);
	chatReceiveFrame = frame;

	chatReceiveInfo[frame].time = time;
	chatReceiveInfo[frame].size = 0;
}

// ZYUQURM
