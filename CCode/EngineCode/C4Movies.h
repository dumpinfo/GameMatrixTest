 

#ifndef C4Movies_h
#define C4Movies_h


//# \component	Movie Manager
//# \prefix		MovieMgr/


#include "C4Sources.h"
#include "C4Shaders.h"
#include "C4Panels.h"
#include "C4Sequence.h"


namespace C4
{
	typedef EngineResult		MovieResult;
	typedef Type				MovieTrackType;
	typedef unsigned_int32		MovieTime;


	enum : MovieResult
	{
		kMovieOkay							= kEngineOkay,
		kMovieLoadFailed					= (kManagerMovie << 16) | 0x0001,
		kMovieVideoTrackMissing				= (kManagerMovie << 16) | 0x0002,
		kMovieBadImportVideoSize			= (kManagerMovie << 16) | 0x0003,
		kMovieInconsistentImportVideoSize	= (kManagerMovie << 16) | 0x0004,
		kMovieBadImportAudioFormat			= (kManagerMovie << 16) | 0x0005,
		kMovieImportCancelled				= (kManagerMovie << 16) | 0x0006
	};


	enum : ProcessType
	{
		kProcessMovie		= 'MOVI'
	};


	enum : WidgetType
	{
		kWidgetMovie		= 'MOVI'
	};


	enum : MovieTrackType
	{
		kMovieTrackVideo	= 'VDEO',
		kMovieTrackAudio	= 'ADIO'
	};


	enum
	{
		kVideoAlphaChannel			= 1 << 0
	};


	enum
	{
		kVideoCompressBaseFrame		= 1 << 0,
		kVideoCompressDeltaFrame	= 1 << 1
	};


	//# \enum	MovieState

	enum MovieState
	{
		kMovieStopped				= 0,			//## The movie is stopped.
		kMoviePlaying				= 1				//## The movie is playing.
	};


	//# \enum	MovieFlags

	enum
	{
		kMovieInitialPlay			= 1 << 0,		//## Movie is initially playing.
		kMovieLoop					= 1 << 1		//## Movie loops when it reaches the end.
	};


	enum
	{
		kMovieTicksPerSecond		= 36000,
		kMovieTicksPerMillisecond	= kMovieTicksPerSecond / 1000
	};


	const float kMovieFloatTicksPerSecond = (float) kMovieTicksPerSecond;
	const float kMovieFloatSecondsPerTick = 1.0F / kMovieFloatTicksPerSecond;


	enum : FunctionType
	{
		kFunctionPlayMovieWidget		= 'PLMV',
		kFunctionStopMovieWidget		= 'STMV',
		kFunctionGetMovieWidgetTime		= 'GMVT',
		kFunctionSetMovieWidgetTime		= 'SMVT'
	}; 


	struct VideoChannelData 
	{
		unsigned_int32			videoChannelFlags; 
		float					quantizationScale;

		Rect					channelBoundingRect; 

		unsigned_int32			flatCodeOffset; 
		unsigned_int32			flatCodeSize; 
		unsigned_int32			flatDataSize;

		unsigned_int32			wlenCodeOffset;
		unsigned_int32			wlenCodeSize; 
		unsigned_int32			wlenDataSize;

		unsigned_int32			wavyCodeOffset;
		unsigned_int32			wavyCodeSize;
		unsigned_int32			wavyDataSize;

		const unsigned_int8 *GetFlatCode(void) const
		{
			return (reinterpret_cast<const unsigned_int8 *>(this) + flatCodeOffset);
		}

		const unsigned_int8 *GetWlenCode(void) const
		{
			return (reinterpret_cast<const unsigned_int8 *>(this) + wlenCodeOffset);
		}

		const unsigned_int8 *GetWavyCode(void) const
		{
			return (reinterpret_cast<const unsigned_int8 *>(this) + wavyCodeOffset);
		}
	};


	struct VideoFrameHeader
	{
		VideoChannelData		luminanceData;
		VideoChannelData		chrominanceData;
	};


	struct VideoAlphaFrameHeader : VideoFrameHeader
	{
		VideoChannelData		alphaData;
	};


	struct VideoFrameData
	{
		unsigned_int32			videoFrameDataOffset;
		unsigned_int32			videoFrameDataSize;
		int32					videoBaseFrameIndex;
	};


	struct VideoTrackHeader
	{
		unsigned_int32			videoTrackFlags;
		int32					videoFrameCount;

		Integer2D				videoFrameSize;
		MovieTime				videoFrameTime;
		int32					posterFrameIndex;

		unsigned_int32			maxFrameCodeSize;
		unsigned_int32			maxFrameDataSize;

		VideoFrameData			videoFrameData[1];
	};


	struct MovieTrackHeader
	{
		MovieTrackType			movieTrackType;
		unsigned_int32			movieTrackOffset;

		const VideoTrackHeader *GetVideoTrackHeader(void) const
		{
			return (reinterpret_cast<const VideoTrackHeader *>(reinterpret_cast<const char *>(this) + movieTrackOffset));
		}

		const AudioTrackHeader *GetAudioTrackHeader(void) const
		{
			return (reinterpret_cast<const AudioTrackHeader *>(reinterpret_cast<const char *>(this) + movieTrackOffset));
		}
	};


	struct MovieResourceHeader
	{
		int32					endian;
		unsigned_int32			headerDataSize;
		int32					movieTrackCount;
	};


	class MovieResource : public Resource<MovieResource>
	{
		friend class Resource<MovieResource>;

		private:

			static C4API ResourceDescriptor		descriptor;

			~MovieResource();

		public:

			C4API MovieResource(const char *name, ResourceCatalog *catalog);

			const MovieResourceHeader *GetMovieResourceHeader(void) const
			{
				return (static_cast<const MovieResourceHeader *>(GetData()));
			}

			C4API ResourceResult LoadHeaderData(ResourceLoader *loader, MovieResourceHeader *resourceHeader, MovieTrackHeader **movieTrackHeader) const;
			ResourceResult LoadVideoFrameData(ResourceLoader *loader, MovieResourceHeader *resourceHeader, const VideoTrackHeader *videoTrackHeader, int32 frameIndex, VideoFrameHeader *videoFrameHeader) const;
			ResourceResult LoadAudioBlockData(ResourceLoader *loader, MovieResourceHeader *resourceHeader, const AudioTrackHeader *audioTrackHeader, int32 blockIndex, AudioBlockHeader *audioBlockHeader) const;

			C4API static void ReleaseHeaderData(MovieTrackHeader *movieTrackHeader);
	};


	class VideoCompressor
	{
		private:

			unsigned_int32		videoTrackFlags;
			Integer2D			videoFrameSize;

			int32				baseImageIndex;
			char				*compressorStorage;

			int8				*luminanceImage[3];
			int8				*chrominanceImage[3];
			int8				*alphaImage[3];

			unsigned_int8		*flatLuminCode;
			unsigned_int8		*wlenLuminCode;
			unsigned_int8		*wavyLuminCode;

			unsigned_int8		*flatChromCode;
			unsigned_int8		*wlenChromCode;
			unsigned_int8		*wavyChromCode;

			unsigned_int8		*flatAlphaCode;
			unsigned_int8		*wlenAlphaCode;
			unsigned_int8		*wavyAlphaCode;

			unsigned_int8		*compressedCode[2];

			int16				*blockData;

			static void FindLuminanceBounds(const int8 *image, const Integer2D& frameSize, Rect *bounds);
			static void FindChrominanceBounds(const int8 *image, const Integer2D& frameSize, Rect *bounds);
			static void FindDeltaLuminanceBounds(const int8 *image, const int8 *baseImage, const Integer2D& frameSize, Rect *bounds);
			static void FindDeltaChrominanceBounds(const int8 *image, const int8 *baseImage, const Integer2D& frameSize, Rect *bounds);

			static void LoadLuminanceBlock(const int8 *input, float (*restrict output)[8], int32 rowLength);
			static void LoadDeltaLuminanceBlock(const int8 *input, const int8 *base, float (*restrict output)[8], int32 rowLength);
			static void TransformQuantizeLuminanceBlock(const float (& input)[8][8], int16 *restrict output, const float (& quantizeMatrix)[8][8]);
			static void StoreLuminanceBlock(const float (& input)[8][8], int8 *restrict output, int32 rowLength);

			static void LoadChrominanceBlock(const int8 *input, float (*restrict blueOutput)[8], float (*restrict redOutput)[8], int32 rowLength);
			static void LoadDeltaChrominanceBlock(const int8 *input, const int8 *base, float (*restrict blueOutput)[8], float (*restrict redOutput)[8], int32 rowLength);
			static void TransformQuantizeChrominanceBlock(const float (& blueInput)[8][8], const float (& redInput)[8][8], int16 *restrict blueOutput, int16 *restrict redOutput, const float (& quantizeMatrix)[8][8]);
			static void StoreChrominanceBlock(const float (& blueInput)[8][8], const float (& redInput)[8][8], int8 *restrict output, int32 rowLength);

			static unsigned_int32 EncodeFlatRow(unsigned_int32 count, int16 *restrict data, unsigned_int8 *restrict code);
			static unsigned_int32 EncodeWavyBlock(const int16 *data, unsigned_int8 *restrict length, unsigned_int8 *restrict code);
			static unsigned_int32 EncodeWavyRow(unsigned_int32 count, const int16 *data, unsigned_int8 *restrict length, unsigned_int8 *restrict code);

			unsigned_int32 CompressLuminance(VideoFrameHeader *videoFrameHeader, float quantScale, unsigned_int32 flags, int32 index);
			unsigned_int32 CompressChrominance(VideoFrameHeader *videoFrameHeader, float quantScale, unsigned_int32 flags, int32 index, unsigned_int32 codeOffset);
			unsigned_int32 CompressAlpha(VideoAlphaFrameHeader *videoFrameHeader, float quantScale, unsigned_int32 flags, int32 index, unsigned_int32 codeOffset);

		public:

			C4API VideoCompressor(const VideoTrackHeader *videoTrackHeader);
			C4API ~VideoCompressor();

			void CommitBaseImage(void)
			{
				baseImageIndex ^= 1;
			}

			const unsigned_int8 *GetCompressedCode(bool delta) const
			{
				return (compressedCode[delta]);
			}

			C4API unsigned_int32 CompressFrame(const void *image, ImageFormat format, VideoFrameHeader *videoFrameHeader, float quantScale, unsigned_int32 flags);
	};


	class VideoDecompressor
	{
		friend class VideoCompressor;

		private:

			Integer2D				videoFrameSize;

			char					*decompressorStorage;

			Color1C					*luminanceImage[2];
			Color2C					*chrominanceImage[2];
			Color1C					*alphaImage[2];

			VideoFrameHeader		*videoFrameHeader;
			unsigned_int8			*videoFrameData;

			int16					*blockData;

			static void DequantizeInverseTransformLuminanceBlock(const int16 *input, float (*restrict output)[8], const float (& dequantizeMatrix)[8][8]);
			static void StoreLuminanceBlock(const float (& input)[8][8], Color1C *restrict output, int32 rowLength);
			static void StoreDeltaLuminanceBlock(const float (& input)[8][8], const Color1C *base, Color1C *restrict output, int32 rowLength);

			static void DequantizeInverseTransformChrominanceBlock(const int16 *blueInput, const int16 *redInput, float (*restrict blueOutput)[8], float (*restrict redOutput)[8], const float (& dequantizeMatrix)[8][8]);
			static void StoreChrominanceBlock(const float (& blueInput)[8][8], const float (& redInput)[8][8], Color2C *restrict output, int32 rowLength);
			static void StoreDeltaChrominanceBlock(const float (& blueInput)[8][8], const float (& redInput)[8][8], const Color2C *base, Color2C *restrict output, int32 rowLength);

			static unsigned_int32 DecodeFlatRow(unsigned_int32 count, const unsigned_int8 *code, int16 *restrict data);
			static unsigned_int32 DecodeWavyBlock(unsigned_int32 length, const unsigned_int8 *code, int16 *restrict data);
			static unsigned_int32 DecodeWavyRow(unsigned_int32 count, const unsigned_int8 *length, const unsigned_int8 *code, int16 *restrict data);

			static void ClearLuminance(Color1C *restrict image, int32 rowLength, const Rect& bounds);
			static void ClearChrominance(Color2C *restrict image, int32 rowLength, const Rect& bounds);
			static void CopyLuminance(const Color1C *base, Color1C *restrict image, int32 rowLength, const Rect& bounds);
			static void CopyChrominance(const Color2C *base, Color2C *restrict image, int32 rowLength, const Rect& bounds);

		public:

			VideoDecompressor(const VideoTrackHeader *videoTrackHeader);
			~VideoDecompressor();

			VideoFrameHeader *GetVideoFrameHeader(void) const
			{
				return (videoFrameHeader);
			}

			Color1C *GetLuminanceImage(void) const
			{
				return (luminanceImage[0]);
			}

			Color2C *GetChrominanceImage(void) const
			{
				return (chrominanceImage[0]);
			}

			Color1C *GetAlphaImage(void) const
			{
				return (alphaImage[0]);
			}

			void DecompressLuminance(bool delta);
			void DecompressChrominance(bool delta);
			void DecompressAlpha(bool delta);
	};


	//# \class	Movie		Encapsulates movie functionality.
	//
	//# \def	class Movie : public SoundStreamer, public SoundConduit, public ListElement<Movie>, public Completable<Movie>
	//
	//# \ctor	Movie();
	//
	//# \desc
	//# The $Movie$ class encapsulates low-level functionality involved in playing movies. Most direct interaction with
	//# $Movie$ objects is handled internally by the $@MovieWidget@$ class, but there are several functions of the $@Movie@$
	//# class that return useful information about a movie. A pointer to a $@Movie@$ object can be retrieved from a movie
	//# widget by calling the $@MovieWidget::GetMovieObject@$ function.
	//
	//# \base		SoundMgr/SoundStreamer			Movies can stream audio tracks into a sound object.
	//# \privbase	SoundConduit					Used internally for connecting movie audio to source nodes.
	//# \base		Utilities/ListElement<Movie>	Used internally by the Movie Manager.
	//# \base		Utilities/Completable<Movie>	The completion procedure is called when the movie finishes playing.


	//# \function	Movie::GetVideoFrameCount		Returns the number of video frames in a movie.
	//
	//# \proto	int32 GetVideoFrameCount(void) const;
	//
	//# \desc
	//# The $GetVideoFrameCount$ function returns the number of video frames in a movie.
	//
	//# \also		$@Movie::GetVideoFrameSize@$
	//# \also		$@Movie::GetVideoFrameTime@$
	//# \also		$@Movie::GetMovieDuration@$


	//# \function	Movie::GetVideoFrameSize		Returns the dimensions of the video frame in a movie.
	//
	//# \proto	const Integer2D& GetVideoFrameSize(void) const;
	//
	//# \desc
	//# The $GetVideoFrameSize$ function returns the width and height, in pixels, of the video track in a movie.
	//
	//# \also		$@Movie::GetVideoFrameCount@$
	//# \also		$@Movie::GetVideoFrameTime@$


	//# \function	Movie::GetVideoFrameTime		Returns the duration of a single video frame in a movie.
	//
	//# \proto	MovieTime GetVideoFrameTime(void) const;
	//
	//# \desc
	//# The $GetVideoFrameTime$ function returns the duration of each video frame in a movie.
	//#
	//# The returned $MovieTime$ type represents times in integer units of 36000 ticks per second.
	//
	//# \also		$@Movie::GetVideoFrameSize@$
	//# \also		$@Movie::GetVideoFrameCount@$
	//# \also		$@Movie::GetMovieDuration@$


	//# \function	Movie::GetMovieDuration			Returns the duration of an entire movie.
	//
	//# \proto	MovieTime GetMovieDuration(void) const;
	//
	//# \desc
	//# The $GetMovieDuration$ function returns the duration an entire movie. This duration corresponds to the longest
	//# duration of all tracks contained in the movie, and not just the duration of the video track. It's possible for
	//# the audio track to be longer than the product of the number of video frames and the duration of each video frame.
	//#
	//# The returned $MovieTime$ type represents times in integer units of 36000 ticks per second.
	//
	//# \also		$@Movie::GetVideoFrameCount@$
	//# \also		$@Movie::GetVideoFrameTime@$


	//# \function	Movie::GetMovieState		Returns the current state of a movie.
	//
	//# \proto	MovieState GetMovieState(void) const;
	//
	//# \desc
	//# The $GetMovieState$ function returns the current state of a movie, which can be one of the following constants.
	//
	//# \table	MovieState
	//
	//# \also		$@Movie::GetMovieTime@$
	//# \also		$@Movie::SetMovieTime@$
	//# \also		$@MovieWidget::PlayMovie@$
	//# \also		$@MovieWidget::StopMovie@$


	//# \function	Movie::GetMovieTime			Returns the current play time of a movie.
	//
	//# \proto	MovieTime GetMovieTime(void) const;
	//
	//# \desc
	//# The $GetMovieTime$ function returns the current play time of a movie.
	//#
	//# The returned $MovieTime$ type represents times in integer units of 36000 ticks per second.
	//
	//# \also		$@Movie::SetMovieTime@$
	//# \also		$@MovieWidget::PlayMovie@$
	//# \also		$@MovieWidget::StopMovie@$


	//# \function	Movie::SetMovieTime			Sets the current play time of a movie.
	//
	//# \proto	void SetMovieTime(MovieTime time);
	//
	//# \param	time	The new time value.
	//
	//# \desc
	//# The $SetMovieTime$ function sets the current play time of a movie to that specified by the $time$ parameter.
	//# This function should be called only for movies that are currently stopped. A subsequent call to the
	//# $@MovieWidget::PlayMovie@$ function will cause the movie to start playing at the new time value.
	//#
	//# The $MovieTime$ type represents times in integer units of 36000 ticks per second.
	//
	//# \also		$@Movie::GetMovieTime@$
	//# \also		$@MovieWidget::PlayMovie@$
	//# \also		$@MovieWidget::StopMovie@$


	class Movie : public SoundStreamer, public SoundConduit, public ListElement<Movie>, public Completable<Movie>
	{
		friend class MovieMgr;

		private:

			MovieState						movieState;
			MovieTime						movieDuration;

			MovieTime						movieTime;
			int32							movieLoop;

			bool							conduitFlag;
			float							movieVolume;

			volatile bool					movieRenderFlag;
			volatile bool					movieUpdateFlag;

			int32							videoFrameIndex;
			int32							baseFrameIndex;
			int32							audioFrameIndex;

			MovieResource					*movieResource;
			MovieTrackHeader				*movieTrackHeader;
			const VideoTrackHeader			*videoTrackHeader;
			const AudioTrackHeader			*audioTrackHeader;

			Sound							*movieSound;

			Texture							*luminanceTexture;
			Texture							*chrominanceTexture;
			Texture							*alphaTexture;

			TextureHeader					luminanceTextureHeader;
			TextureHeader					chrominanceTextureHeader;
			TextureHeader					alphaTextureHeader;

			MovieResourceHeader				movieResourceHeader;
			ResourceLoader					movieLoader;

			Storage<VideoDecompressor>		videoDecompressor;
			Storage<AudioDecompressor>		audioDecompressor;

			MovieResult RenderVideoFrame(void);
			void UpdateVideoFrame(void);

			int32 GetStreamFrameCount(void) override;
			SoundResult StartStream(void) override;
			bool FillBuffer(unsigned_int32 bufferSize, Sample *buffer, int32 *count) override;

			Sound *LoadSound(Source *source) override;
			void SetSoundVolume(Sound *sound, float volume) override;

		public:

			Movie(bool conduit = false);
			~Movie();

			int32 GetVideoFrameCount(void) const
			{
				return (videoTrackHeader->videoFrameCount);
			}

			const Integer2D& GetVideoFrameSize(void) const
			{
				return (videoTrackHeader->videoFrameSize);
			}

			MovieTime GetVideoFrameTime(void) const
			{
				return (videoTrackHeader->videoFrameTime);
			}

			MovieState GetMovieState(void) const
			{
				return (movieState);
			}

			MovieTime GetMovieDuration(void) const
			{
				return (movieDuration);
			}

			MovieTime GetMovieTime(void) const
			{
				return (movieTime);
			}

			void SetMovieTime(MovieTime time)
			{
				movieTime = time;
			}

			bool GetMovieLoop(void) const
			{
				return (movieLoop != 0);
			}

			void SetMovieLoop(bool loop)
			{
				movieLoop = loop;
			}

			float GetMovieVolume(void) const
			{
				return (movieVolume);
			}

			Texture *GetLuminanceTexture(void) const
			{
				return (luminanceTexture);
			}

			Texture *GetChrominanceTexture(void) const
			{
				return (chrominanceTexture);
			}

			Texture *GetAlphaTexture(void) const
			{
				return (alphaTexture);
			}

			MovieResult Load(const char *name);

			void SetMovieVolume(float volume);

			C4API void Play(void);
			C4API void Stop(void);

			C4API bool Update(void);
	};


	class MovieProcess final : public Process
	{
		private:

			Texture			*luminanceTexture;
			Texture			*chrominanceTexture;
			Texture			*alphaTexture;

			MovieProcess(const MovieProcess& movieProcess);

			Process *Replicate(void) const override;

		public:

			MovieProcess(Texture *luminance, Texture *chrominance, Texture *alpha = nullptr);
			~MovieProcess();

			int32 GetPortCount(void) const override;
			const char *GetPortName(int32 index) const override;

			void GenerateProcessData(const ShaderCompileData *compileData, ProcessData *data) const override;
			int32 GenerateShaderCode(const ShaderCompileData *compileData, const char **shaderCode) const override;
	};


	//# \class	MovieWidget		The interface widget that plays a movie.
	//
	//# The $MovieWidget$ class represents an interface widget that plays a movie.
	//
	//# \def	class MovieWidget final : public QuadWidget
	//
	//# \ctor	MovieWidget(const Vector2D& size, const char *name);
	//
	//# \param	size	The size of the quad in which the movie is rendered, in pixels.
	//# \param	name	The name of the movie resource that is played by the movie widget.
	//
	//# \desc
	//# The $MovieWidget$ class displays a movie.
	//
	//# \base	InterfaceMgr/QuadWidget		A $MovieWidget$ is a specialized $QuadWidget$.


	//# \function	MovieWidget::GetMovieName		Returns the name of the movie resource.
	//
	//# \proto	const char *GetMovieName(void) const;
	//
	//# \desc
	//# The $GetMovieName$ function returns the name of the movie resource that is played by the movie widget.
	//
	//# \also	$@MovieWidget::SetMovieName@$


	//# \function	MovieWidget::SetMovieName		Sets the name of the movie resource.
	//
	//# \proto	void SetMovieName(const char *name);
	//
	//# \param	name	The name of the movie resource that is played by the movie widget.
	//
	//# \desc
	//# The $SetMovieName$ function sets the name of the movie resource that is played by the movie widget.
	//
	//# \also	$@MovieWidget::GetMovieName@$


	//# \function	MovieWidget::GetMovieBlendState		Returns the blend state for a movie.
	//
	//# \proto	unsigned_int32 GetMovieBlendState(void) const;
	//
	//# \desc
	//# The $GetMovieBlendState$ function returns the blend state used by a movie widget when playing a movie that contains an alpha channel.
	//# See the $@MovieWidget::SetMovieBlendState@$ function for information about what values can be returned.
	//
	//# \also	$@MovieWidget::SetMovieBlendState@$


	//# \function	MovieWidget::SetMovieBlendState		Sets the blend state for a movie.
	//
	//# \proto	void SetMovieBlendState(unsigned_int32 state);
	//
	//# \param	state	The new movie blend state.
	//
	//# \desc
	//# The $SetMovieBlendState$ function sets the blend state used by a movie widget when playing a movie that contains an alpha channel.
	//# The $state$ parameter can be any of the same values that are accepted by the $@GraphicsMgr/Renderable::SetAmbientBlendState@$
	//# function. The default blend state is $kBlendInterpolate$.
	//#
	//# For movies that do not contain an alpha channel, the current movie blend state is ignored, and the movie is rendered as if the
	//# blend state is $kBlendReplace$.
	//
	//# \also	$@MovieWidget::GetMovieBlendState@$


	//# \function	MovieWidget::GetSourceConnectorKey		Returns the connector key that identifies the source node for audio output.
	//
	//# \proto	const ConnectorKey& GetCameraConnectorKey(void) const;
	//
	//# \desc
	//# The $GetSourceConnectorKey$ function returns the connector key that identifies the source node through which audio
	//# is played for the movie widget.
	//
	//# \also	$@MovieWidget::SetSourceConnectorKey@$


	//# \function	MovieWidget::SetSourceConnectorKey		Sets the connector key that identifies the source node for audio output.
	//
	//# \proto	void SetSourceConnectorKey(const ConnectorKey& key);
	//
	//# \param	key		Thew new source connector key.
	//
	//# \desc
	//# The $SetSourceConnectorKey$ function sets the connector key that identifies the source node through which audio
	//# is played for the movie widget. The $key$ parameter should identify a connector attached to the panel effect containing
	//# the movie widget, and that connector should be connected to a $@WorldMgr/Source@$ node.
	//#
	//# A connection to a source node is required in order for audio to be played by a movie widget inside a panel effect.
	//# For movie widgets in a desktop window, there is no panel effect node, and audio always plays if it is present in the movie.
	//#
	//# The initial value of the source connector key is the empty string.
	//
	//# \also	$@MovieWidget::GetSourceConnectorKey@$


	//# \function	MovieWidget::GetMovieObject			Returns the movie object associated with a movie widget.
	//
	//# \proto	Movie *GetMovieObject(void) const;
	//
	//# \desc
	//# The $GetMovieObject$ function returns the $@Movie@$ object associated with a movie widget.
	//
	//# \also	$@Movie@$


	//# \function	MovieWidget::PlayMovie		Plays the movie associated with a movie widget.
	//
	//# \proto	void PlayMovie(void);
	//
	//# \desc
	//# The $PlayMovie$ function plays the movie associated with a movie widget. A movie can be stopped with
	//# the $@MovieWidget::StopMovie@$ function.
	//#
	//# If a movie was previously stopped part way through, then calling the $PlayMovie$ function will resume
	//# playback at the point it was previously stopped. To reset the playback time to the beginning of the
	//# movie, call the $@MovieWidget::GetMovieObject@$ function to retrieve the $@Movie@$ object, and then
	//# call the $@Movie::SetMovieTime@$ function to set the play time to zero before playing the movie.
	//#
	//# If there is no movie associated with the movie widget, or there was an error loading the movie,
	//# then the $PlayMovie$ function has no effect.
	//
	//# \also	$@MovieWidget::StopMovie@$
	//# \also	$@Movie@$


	//# \function	MovieWidget::StopMovie		Stops the movie associated with a movie widget.
	//
	//# \proto	void StopMovie(void);
	//
	//# \desc
	//# The $StopMovie$ function stops the movie associated with a movie widget. A movie can be played with
	//# the $@MovieWidget::PlayMovie@$ function.
	//#
	//# If a movie is stopped part way through, then a future call to the $@MovieWidget::PlayMovie@$ function
	//# will resume playback at the point it was previously stopped. To reset the playback time to the beginning
	//# of the movie, call the $@MovieWidget::GetMovieObject@$ function to retrieve the $@Movie@$ object, and then
	//# call the $@Movie::SetMovieTime@$ function to set the play time to zero before playing the movie.
	//#
	//# If there is no movie associated with the movie widget, or there was an error loading the movie,
	//# then the $StopMovie$ function has no effect.
	//
	//# \also	$@MovieWidget::PlayMovie@$
	//# \also	$@Movie@$


	class MovieWidget final : public QuadWidget
	{
		friend class WidgetReg<MovieWidget>;

		private:

			ResourceName			movieName;
			unsigned_int32			movieFlags;
			unsigned_int32			movieBlendState;
			MovieTime				movieStartTime;

			ConnectorKey			sourceConnectorKey;

			Movie					*movieObject;
			MovieProcess			*movieProcess;
			Link<Node>				sourceNode;

			DeferredTask			playTask;

			VertexBuffer			vertexBuffer;
			List<Attribute>			attributeList;
			ShaderAttribute			shaderAttribute;

			MovieWidget();
			MovieWidget(const MovieWidget& movieWidget);

			Widget *Replicate(void) const override;

			static void PlayTask(DeferredTask *task, void *cookie);

			void ExtendAnimationTime(void);

		public:

			C4API MovieWidget(const Vector2D& size, const char *name);
			C4API ~MovieWidget();

			const char *GetMovieName(void) const
			{
				return (movieName);
			}

			void SetMovieName(const char *name)
			{
				movieName = name;
			}

			unsigned_int32 GetMovieFlags(void) const
			{
				return (movieFlags);
			}

			void SetMovieFlags(unsigned_int32 flags)
			{
				movieFlags = flags;
			}

			unsigned_int32 GetMovieBlendState(void) const
			{
				return (movieBlendState);
			}

			void SetMovieBlendState(unsigned_int32 state)
			{
				movieBlendState = state;
			}

			const ConnectorKey& GetSourceConnectorKey(void) const
			{
				return (sourceConnectorKey);
			}

			void SetSourceConnectorKey(const ConnectorKey& key)
			{
				sourceConnectorKey = key;
			}

			Movie *GetMovieObject(void) const
			{
				return (movieObject);
			}

			static void RegisterFunctions(ControllerRegistration *registration);

			void Pack(Packer& data, unsigned_int32 packFlags) const override;
			void Unpack(Unpacker& data, unsigned_int32 unpackFlags) override;
			bool UnpackChunk(const ChunkHeader *chunkHeader, Unpacker& data, unsigned_int32 unpackFlags);
			void *BeginSettingsUnpack(void) override;

			int32 GetSettingCount(void) const override;
			Setting *GetSetting(int32 index) const override;
			void SetSetting(const Setting *setting) override;

			void Preprocess(void) override;
			void Move(void) override;
			void Build(void) override;

			void HandleMouseEvent(const PanelMouseEventData *eventData) override;

			C4API void PlayMovie(void);
			C4API void StopMovie(void);
	};


	class PlayMovieWidgetFunction final : public WidgetFunction
	{
		private:

			PlayMovieWidgetFunction(const PlayMovieWidgetFunction& playMovieWidgetFunction);

			Function *Replicate(void) const override;

		public:

			PlayMovieWidgetFunction();
			~PlayMovieWidgetFunction();

			bool OverridesFunction(const Function *function) const override;
			void Execute(Controller *controller, FunctionMethod *method, const ScriptState *state) override;
	};


	class StopMovieWidgetFunction final : public WidgetFunction
	{
		private:

			StopMovieWidgetFunction(const StopMovieWidgetFunction& stopMovieWidgetFunction);

			Function *Replicate(void) const override;

		public:

			StopMovieWidgetFunction();
			~StopMovieWidgetFunction();

			bool OverridesFunction(const Function *function) const override;
			void Execute(Controller *controller, FunctionMethod *method, const ScriptState *state) override;
	};


	class GetMovieWidgetTimeFunction final : public WidgetFunction
	{
		private:

			GetMovieWidgetTimeFunction(const GetMovieWidgetTimeFunction& getMovieWidgetTimeFunction);

			Function *Replicate(void) const override;

		public:

			GetMovieWidgetTimeFunction();
			~GetMovieWidgetTimeFunction();

			void Execute(Controller *controller, FunctionMethod *method, const ScriptState *state) override;
	};


	class SetMovieWidgetTimeFunction final : public WidgetFunction
	{
		private:

			float		movieTime;

			SetMovieWidgetTimeFunction(const SetMovieWidgetTimeFunction& setMovieWidgetTimeFunction);

			Function *Replicate(void) const override;

		public:

			SetMovieWidgetTimeFunction(float time = 0.0F);
			~SetMovieWidgetTimeFunction();

			float GetMovieTime(void) const
			{
				return (movieTime);
			}

			void Pack(Packer& data, unsigned_int32 packFlags) const override;
			void Unpack(Unpacker& data, unsigned_int32 unpackFlags) override;

			void Compress(Compressor& data) const override;
			bool Decompress(Decompressor& data) override;

			int32 GetSettingCount(void) const override;
			Setting *GetSetting(int32 index) const override;
			void SetSetting(const Setting *setting) override;

			void Execute(Controller *controller, FunctionMethod *method, const ScriptState *state) override;
	};


	//# \class	MovieMgr		The Movie Manager class.
	//
	//# \def	class MovieMgr : public Manager<MovieMgr>
	//
	//# \desc
	//# The $MovieMgr$ class encapsulates the movie compression and playback functionality of the C4 Engine.
	//# The single instance of the Movie Manager is constructed during an application's initialization
	//# and destroyed at termination.
	//#
	//# The Movie Manager's member functions are accessed through the global pointer $TheMovieMgr$.
	//
	//# \also	$@Movie@$
	//# \also	$@MovieWidget@$


	//# \function	MovieMgr::StartRecording		Starts recording video output to movie sequence files.
	//
	//# \proto	EngineResult StartRecording(int32 rate, const char *name);
	//
	//# \param	rate	The rate at which frames are captured, in frames per second. For best results, this number should evenly divide 36000.
	//# \param	name	The base name (including directory path) of the sequence files that will be generated, without any extension.
	//
	//# \desc
	//# The $StartRecording$ function starts recording the rendered video output of the engine at the frame rate specified by
	//# the $rate$ parameter. The video frames are stored in one or more movie sequence files that are named by taking the path
	//# specified by the $name$ parameter, appending a number representing the index of the sequence file, and then appending
	//# the extension $.seq$. Each sequence file is filled with as many frames as will fit within a 1.5 GB limit, and then a new
	//# sequence file is started.
	//#
	//# If the call to the $StartRecording$ function is successful, then the return value is $kEngineOkay$. Otherwise, a nonzero
	//# result code is returned. If the $StartRecording$ function is called while recording is already in progress, then the function
	//# has no effect, and the return value is $kEngineOkay$.
	//#
	//# The Movie Manager continues recording video until the $@MovieMgr::StopRecording@$ function is called.
	//
	//# \also	$@MovieMgr::StopRecording@$
	//# \also	$@SoundMgr/SoundMgr::StartRecording@$
	//# \also	$@SoundMgr/SoundMgr::StopRecording@$
	//
	//# \wiki	Recording_Movies	Recording Movies


	//# \function	MovieMgr::StopRecording		Stops recording video output.
	//
	//# \proto	void StopRecording(void);
	//
	//# \desc
	//# The $StopRecording$ function stops video output recording that was previously started with the $@MovieMgr::StartRecording@$ function.
	//# If video recording is not in progress when the $StopRecording$ function is called, then the function has no effect.
	//
	//# \also	$@MovieMgr::StartRecording@$
	//# \also	$@SoundMgr/SoundMgr::StartRecording@$
	//# \also	$@SoundMgr/SoundMgr::StopRecording@$
	//
	//# \wiki	Recording_Movies	Recording Movies


	//# \function	MovieMgr::FormatMovieTime		Formats a movie time value as a string showing seconds, minutes, etc.
	//
	//# \proto	static String<15> FormatMovieTime(MovieTime time);
	//
	//# \param	time	The movie time value to format as a string.
	//
	//# \desc
	//# The $FormatMovieTime$ function takes the movie time value specified by the $time$ parameter and translates it into
	//# a string containing hours, minutes, seconds, and tenths of a second. The returned string is always exactly ten
	//# characters in length, and it has the format "HH:MM:SS.S".


	class MovieMgr : public Manager<MovieMgr>
	{
		private:

			enum
			{
				kMovieRecordBufferCount		= 4,
				kMovieRecordBufferMask		= kMovieRecordBufferCount - 1
			};

			List<Movie>							renderList;
			Mutex								renderMutex;

			Storage<Signal>						movieSignal;
			Storage<Thread>						movieThread;

			static void MovieThread(const Thread *thread, void *cookie);

			#if C4RECORDABLE

				int32							recordFrameCount;
				Integer2D						recordFrameSize;

				int32							recordFileCount;
				unsigned_int32					recordFileSize;

				MovieTime						recordTime;
				MovieTime						recordFrameDuration;

				SequenceHeader					recordSequenceHeader;

				int8							*recordLuminanceBuffer;
				int8							*recordChrominanceBuffer;

				Render::PixelBufferObject		recordBuffer[kMovieRecordBufferCount];
				const Color4C					*volatile recordImage[kMovieRecordBufferCount];
				volatile int8					recordStage[kMovieRecordBufferCount];
				volatile bool					recordExtractFlag[kMovieRecordBufferCount];
				volatile int32					recordFrameIndex;

				Storage<Signal>					recordSignal;
				Storage<Thread>					recordThread;

				Storage<File>					recordFile;
				String<kMaxFileNameLength>		recordFileName;

				FileResult NewRecordFile(void);

				static void RecordThread(const Thread *thread, void *cookie);

			#endif

		public:

			MovieMgr(int);
			~MovieMgr();

			EngineResult Construct(void);
			void Destruct(void);

			static void ExtractLuminanceImage(const Integer2D& size, const Color4C *color, int8 *restrict luminance);
			static void ExtractChrominanceImage(const Integer2D& size, const Color4C *color, int8 *restrict chrominance, bool reverse = false);
			static void ExtractAlphaImage(const Integer2D& size, const Color4C *color, int8 *restrict alpha);

			void SubmitMovieRenderTask(Movie *movie);

			C4API EngineResult StartRecording(int32 rate, const char *name);
			C4API void StopRecording(void);

			#if C4RECORDABLE

				bool Recording(void) const
				{
					return (recordFrameCount >= 0);
				}

				void RecordTask(void);

			#else

				bool Recording(void) const
				{
					return (false);
				}

				void RecordTask(void)
				{
				}

			#endif

			C4API static String<15> FormatMovieTime(MovieTime time);
	};


	C4API extern MovieMgr *TheMovieMgr;
}


#endif

// ZYUQURM
