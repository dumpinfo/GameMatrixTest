 

#ifndef C4AudioCapture_h
#define C4AudioCapture_h


//# \component	Sound Manager
//# \prefix		SoundMgr/


#include "C4Sound.h"
#include "C4Messages.h"

#if C4WINDOWS && C4FASTBUILD

	#include "C4DirectSound.h"

#endif


namespace C4
{
	typedef EngineResult	AudioCaptureResult;


	enum : AudioCaptureResult
	{
		kAudioCaptureOkay			= kEngineOkay,
		kAudioCaptureUnavailable	= (kManagerAudioCapture << 16) | 0x0001
	};


	enum
	{
		kCaptureBufferFrameCount	= 1024,
		kCaptureBufferSize			= kCaptureBufferFrameCount * sizeof(Sample),
		kChatRingBufferSliceCount	= 4,
		kChatRingBufferFrameCount	= kCaptureBufferFrameCount * kChatRingBufferSliceCount,
		kChatRingBufferSize			= kCaptureBufferSize * kChatRingBufferSliceCount,

		kAudioMessageBlockSize		= 16,
		kAudioMessageBlockCount		= (kMaxMessageDataSize - 9) / (kAudioMessageBlockSize + 1),
		kAudioMessageFrameCount		= kAudioMessageBlockSize * kAudioMessageBlockCount,
		kMaxAudioMessageDataSize	= (kAudioMessageBlockSize + 1) * kAudioMessageBlockCount
	};


	enum
	{
		kAudioCaptureRecording		= 1 << 0,
		kAudioCapturePaused			= 1 << 1
	};


	class AudioMessage : public Message
	{
		friend class MessageMgr;

		private:

			mutable PlayerKey	playerKey;
			unsigned_int32		timeStamp;

			unsigned_int32		audioDataSize;
			unsigned_int32		audioBlockCount;
			int8				audioData[kMaxAudioMessageDataSize];

			AudioMessage();

		public:

			AudioMessage(unsigned_int32 time, int32 count, const Sample *data);
			~AudioMessage();

			PlayerKey GetPlayerKey(void) const
			{
				return (playerKey);
			}

			unsigned_int32 GetTimeStamp(void) const
			{
				return (timeStamp);
			}

			int32 GetSampleCount(void) const
			{
				return (audioBlockCount * kAudioMessageBlockSize);
			}

			void Compress(Compressor& data) const override;
			bool Decompress(Decompressor& data) override;

			bool HandleMessage(Player *sender) const override;

			unsigned_int32 GetAudioData(Sample *data, unsigned_int32 offset = 0) const;
	};


	class ChatStreamer : public SoundStreamer
	{
		private:

			unsigned_int32				chatStreamerState; 

			unsigned_int32				baseTimeStamp;
			unsigned_int32				dataTimeStamp; 
			volatile unsigned_int32		playTimeStamp;
 
		public:

			ChatStreamer(); 
			~ChatStreamer();
 
			SoundResult StartStream(void); 
			bool FillBuffer(unsigned_int32 bufferSize, Sample *buffer, int32 *count);

			void ReceiveAudioMessage(const AudioMessage *message);
	}; 


	//# \class	AudioCaptureMgr		The Audio Capture Manager class.
	//
	//# \def	class AudioCaptureMgr : public Manager<AudioCaptureMgr>
	//
	//# \desc
	//# The $AudioCaptureMgr$ class encapsulates the audio input functionality of the C4 Engine.
	//# The single instance of the Audio Capture Manager is constructed during an application's initialization
	//# and destroyed at termination.
	//#
	//# The Audio Capture Manager's member functions are accessed through the global pointer $TheAudioCaptureMgr$.


	//# \function	AudioCaptureMgr::StartAudioCapture		Starts capturing audio from an input device.
	//
	//# \proto	AudioCaptureResult StartAudioCapture(bool paused = false);
	//
	//# \param	paused		If $true$, then the audio capture is started in the paused state.
	//
	//# \desc
	//# The $StartAudioCapture$ function allocates resources needed for capturing audio from an input device.
	//# If the $paused$ parameter is $false$, then audio capture begins immediately. If the $paused$ parameter is
	//# $true$, then audio capture does not begin until the $@AudioCaptureMgr::ResumeAudioCapture@$ function is called.
	//#
	//# If this function succeeds, then the return value is $kAudioCaptureOkay$. If audio capture is not available,
	//# then the return value is $kAudioCaptureUnavailable$.
	//#
	//# If the $StartAudioCapture$ function is called again after audio capture has already begun, then the
	//# function has no effect, and the return value is $kAudioCaptureOkay$. In this case, the $paused$ parameter
	//# is ignored.
	//
	//# \also	$@AudioCaptureMgr::StopAudioCapture@$
	//# \also	$@AudioCaptureMgr::PauseAudioCapture@$
	//# \also	$@AudioCaptureMgr::ResumeAudioCapture@$


	//# \function	AudioCaptureMgr::StopAudioCapture		Stops capturing audio from an input device.
	//
	//# \proto	void StopAudioCapture(void);
	//
	//# \desc
	//# The $StopAudioCapture$ function stops audio capture and deallocates the resources needed for capturing audio
	//# from an input device. If audio capture had not been previously started, then this function has no effect.
	//
	//# \also	$@AudioCaptureMgr::StartAudioCapture@$
	//# \also	$@AudioCaptureMgr::PauseAudioCapture@$
	//# \also	$@AudioCaptureMgr::ResumeAudioCapture@$


	//# \function	AudioCaptureMgr::PauseAudioCapture		Pauses audio capture.
	//
	//# \proto	void PauseAudioCapture(void);
	//
	//# \desc
	//# The $PauseAudioCapture$ function pauses audio capture without deallocating any resources needed for capturing
	//# audio. Audio capture is resumed by calling the $@AudioCaptureMgr::ResumeAudioCapture@$ function. If audio
	//# capture had not been previously started, then this function has no effect.
	//
	//# \also	$@AudioCaptureMgr::ResumeAudioCapture@$
	//# \also	$@AudioCaptureMgr::StartAudioCapture@$
	//# \also	$@AudioCaptureMgr::StopAudioCapture@$


	//# \function	AudioCaptureMgr::ResumeAudioCapture		Resumes audio capture.
	//
	//# \proto	void ResumeAudioCapture(void);
	//
	//# \desc
	//# The $ResumeAudioCapture$ function resumes audio capture without reallocating any resources needed for capturing
	//# audio. For this function to have any effect, either the $@AudioCaptureMgr::StartAudioCapture@$ function must have
	//# previously been called with $true$ specified for the $paused$ parameter or the $@AudioCaptureMgr::PauseAudioCapture@$
	//# must have previously been called when audio capture was enabled.
	//
	//# \also	$@AudioCaptureMgr::PauseAudioCapture@$
	//# \also	$@AudioCaptureMgr::StartAudioCapture@$
	//# \also	$@AudioCaptureMgr::StopAudioCapture@$


	//# \function	AudioCaptureMgr::SetAudioCaptureProc	Installs a callback procedure that receives raw captured audio data.
	//
	//# \proto	void SetAudioCaptureProc(AudioCaptureProc *proc, void *cookie = nullptr);
	//
	//# \param	proc	A pointer to the audio capture callback procedure.
	//# \param	cookie	The cookie that is passed to the callback procedure as its last parameter.
	//
	//# \desc
	//# The $SetAudioCaptureProc$ function installs a callback procedure that is periodically invoked when new audio data
	//# has been captured on the local machine. This function is provided so that an application may interpret the audio data
	//# in whatever way it wishes (e.g., speech recognition). The $AudioCaptureProc$ type is defined as follows.
	//
	//# \code	typedef void AudioCaptureProc(unsigned_int32 timeStamp, const Sample *sampleData, int32 sampleCount, void *cookie);
	//
	//# When the callback procedure is called, the $timeStamp$ parameter is set to the starting sample index. This value begins at
	//# zero and increases by the number of samples reported every time the callback procedure is called. When the time stamp
	//# reaches the size limit of a 32-bit integer, it wraps around. The $sampleData$ parameter contains a pointer to the raw audio
	//# data, in uncompressed mono PCM format with signed 16-bit integer samples. The $sampleCount$ parameter contains the number of
	//# audio samples. The $cookie$ parameter contains the value passed to the $cookie$ parameter of the $SetAudioCaptureProc$ function.


	class AudioCaptureMgr : public Manager<AudioCaptureMgr>
	{
		public:

			typedef void AudioCaptureProc(unsigned_int32, const Sample *, int32, void *);

		private:

			enum
			{
				kAudioInfoFrameCount = 64
			};

			struct AudioInfo
			{
				unsigned_int32		time;
				unsigned_int32		size;
			};

			#if C4WINDOWS

				IDirectSoundCapture			*directSoundCapture;
				IDirectSoundCaptureBuffer	*captureBuffer;

				HANDLE						captureEvent[3];
				Thread						*captureThread;

				static void CaptureThread(const Thread *thread, void *cookie);

			#elif C4MACOS

				AudioUnit					audioUnit;

				float						*inputBuffer;
				UInt32						inputBufferFrameCount;
				Fixed						inputBufferFrequency;

				unsigned_int32				audioWritePosition;
				AudioBufferList				audioBufferList;

				static OSStatus CaptureCallback(void *inRefCon, AudioUnitRenderActionFlags *ioActionFlags, const AudioTimeStamp *inTimeStamp, UInt32 inBusNumber, UInt32 inNumberFrames, AudioBufferList *ioData);

			#elif C4LINUX

				snd_pcm_t					*captureHandle;

				volatile bool				captureExitFlag;
				Thread						*captureThread;

				static void CaptureThread(const Thread *thread, void *cookie);

			#elif C4IOS //[ MOBILE

				// -- Mobile code hidden --

			#elif C4PS4 //[ PS4

				// -- PS4 code hidden --

			#elif C4PS3 //[ PS3

				// -- PS3 code hidden --

			#endif //]

			volatile unsigned_int32		audioCaptureState;

			volatile bool				audioAvailable[2];
			volatile bool				audioAboveThreshold[2];
			unsigned_int8				audioBufferParity;

			unsigned_int32				baseTimeStamp;
			Sample						*audioBuffer;

			AudioCaptureProc			*audioCaptureProc;
			void						*audioCaptureCookie;

			unsigned_int32				chatReceiveTime;
			unsigned_int32				chatReceiveFrame;
			AudioInfo					chatReceiveInfo[kAudioInfoFrameCount];

			AudioCaptureResult Initialize(void);
			void Terminate(void);

		public:

			AudioCaptureMgr(int);
			~AudioCaptureMgr();

			EngineResult Construct(void);
			void Destruct(void);

			void AddChatReceiveSize(unsigned_int32 size)
			{
				chatReceiveInfo[chatReceiveFrame].size += size;
			}

			void SetAudioCaptureProc(AudioCaptureProc *proc, void *cookie = nullptr)
			{
				audioCaptureProc = proc;
				audioCaptureCookie = cookie;
			}

			C4API AudioCaptureResult StartAudioCapture(bool paused = false);
			C4API void StopAudioCapture(void);

			C4API void PauseAudioCapture(void);
			C4API void ResumeAudioCapture(void);

			C4API unsigned_int32 GetChatReceiveRate(void) const;

			void AudioCaptureTask(void);
	};


	C4API extern AudioCaptureMgr *TheAudioCaptureMgr;
}


#endif

// ZYUQURM
