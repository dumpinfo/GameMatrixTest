 

#ifndef C4Messages_h
#define C4Messages_h


//# \component	Message Manager
//# \prefix		MessageMgr/

//# \import		C4Network.h


#include "C4Files.h"
#include "C4Variables.h"
#include "C4Network.h"


namespace C4
{
	typedef unsigned_int32	MessageType;
	typedef unsigned_int32	ControllerMessageType;
	typedef unsigned_int32	ChannelType;
	typedef int32			PlayerKey;


	enum
	{
		kMaxGameNameLength				= 63,
		kMaxPlayerNameLength			= 63,
		kMaxCombinedMessageCount		= 15,
		kMaxCombinedMessageSize			= kMaxMessageSize,
		kMaxMessageDataSize				= kMaxCombinedMessageSize - 2,
		kMaxFileChunkSize				= (kMaxMessageDataSize - 2) & ~3,
		kMaxChatMessageLength			= kMaxMessageDataSize - sizeof(NetworkAddress) - 1,
		kMaxControllerMessageDataSize	= kMaxMessageDataSize - 3,
		kMaxSettingMessageDataSize		= kMaxControllerMessageDataSize - 8
	};


	//# \enum	MessageFlags

	enum
	{
		kMessageUnreliable			= 1 << 0,	//## The message should be sent as an unreliable packet. Messages having this flag set are not guaranteed to reach their destinations, but they do arrive in order.
		kMessageUnordered			= 1 << 1,	//## The message should be sent as an unordered packet. Messages having this flag set can arrive at their destinations out of order. The $kMessageUnreliable$ flag must also be set.
		kMessageCombineInhibit		= 1 << 2,	//## The message should not be combined with other messages into a single data packet. Messages having this flag set will always be transmitted as a distinct data packet.
		kMessageDestroyer			= 1 << 3,	//## The message is a controller message, and it causes the controller's target node to be destroyed. This flag is only useful for messages sent with the $@MessageMgr::SendMessageJournal@$ function, and it prevents the message from being deleted from the journal when the controller is destroyed.
		kMessageJournaled			= 1 << 4
	};


	enum : MessageType
	{
		kMessagePing,
		kMessageServerQuery,
		kMessageConnect,
		kMessageDisconnect,
		kMessageTimeout,
		kMessageClientInfo,
		kMessagePlayerKey,
		kMessageSynchronize,
		kMessageChat,
		kMessageAudio,
		kMessageName,
		kMessageFileRequest,
		kMessageFileReceive,
		kMessageFileError,
		kMessageFileCancel,
		kMessageFileChunk,
		kMessageController,
		kMessageBaseCount
	};


	enum
	{
		kPlayerNone			= 0,
		kPlayerServer		= -1,
		kPlayerSelf			= -2,
		kPlayerAll			= -3,
		kPlayerClients		= -4,
		kPlayerNew			= -5,
		kPlayerInvalid		= -32768
	};


	//# \enum	ConnectionEvent

	enum ConnectionEvent
	{
		kConnectionQueryReceived,			//## A request for server information has been received.
		kConnectionAttemptFailed,			//## An attempt to connect to a server has failed.
		kConnectionClientOpened,			//## A client machine has opened a connection to the server.
		kConnectionServerAccepted,			//## The server has accepted a connection request.
		kConnectionServerClosed,			//## The server has closed the connection.
		kConnectionClientClosed,			//## A client has closed the connection.
		kConnectionServerTimedOut,			//## The server connection has timed out.
		kConnectionClientTimedOut			//## A client connection has timed out.
	};


	//# \enum	PlayerEvent
 
	enum PlayerEvent
	{
		kPlayerConnected,					//## A new player connected to the game. 
		kPlayerDisconnected,				//## A player disconnected from the game.
		kPlayerTimedOut,					//## A player timed out and was disconnected from the game. 
		kPlayerInitialized,					//## A new player has been initialized and is ready to receive game state.
		kPlayerChatReceived,				//## A player has sent a chat message.
		kPlayerRenamed						//## A player has changed his name. 
	};
 
 
	//# \enum	GameEvent

	enum GameEvent
	{ 
		kGameSynchronized					//## The full game state has been received.
	};


	//# \enum	FileTransferResult

	enum FileTransferResult
	{
		kTransferOkay,						//## The file transfer succeeded.
		kTransferPending,					//## A file transfer is already in progress. Each player allows only one file transfer to be pending at a time.
		kTransferFileNotFound,				//## The file does not exist on the sending machine.
		kTransferFileUnreadable,			//## The file could not be read on the sending machine.
		kTransferFileUnwriteable,			//## The file could not be written on the receiving machine.
		kTransferConnectionLost				//## The connection was closed or timed out during the file transfer.
	};


	class Controller;
	class Player;
	class Sound;
	class SoundGroup;
	class ChatStreamer;
	class MessageMgr;


	//# \class	Compressor		Compresses data for a message.
	//
	//# \def	class Compressor
	//
	//# \ctor	Compressor(char *ptr);
	//
	//# $Compressor$ objects should be constructed only by the Message Manager.
	//
	//# \desc
	//# A $Compressor$ object is passed to the $@Message::Compress@$ function before the Message Manager
	//# sends a message. The message object should use the $<<$ operator or the $@Compressor::Write@$
	//# function to store its data in the $Compressor$ object's internal buffer. Data written by the
	//# $<<$ operator is always written in big endian order so that messages can be sent between machines
	//# having different native endians.
	//
	//# \important
	//# The total size of the data written to the $Compressor$ object should not exceed $kMaxMessageSize - 1$ bytes.
	//
	//# \operator	Compressor& operator <<(const char& x);
	//#				Writes a single 8-bit character to the internal buffer.
	//
	//# \operator	Compressor& operator <<(const unsigned_int8& x);
	//#				Writes a single 8-bit unsigned character to the internal buffer.
	//
	//# \operator	Compressor& operator <<(const int16& x);
	//#				Writes a single 16-bit integer to the internal buffer.
	//
	//# \operator	Compressor& operator <<(const unsigned_int16& x);
	//#				Writes a single 16-bit unsigned integer to the internal buffer.
	//
	//# \operator	Compressor& operator <<(const int32& x);
	//#				Writes a single 32-bit integer to the internal buffer.
	//
	//# \operator	Compressor& operator <<(const unsigned_int32& x);
	//#				Writes a single 32-bit unsigned integer to the internal buffer.
	//
	//# \operator	Compressor& operator <<(const int64& x);
	//#				Writes a single 64-bit integer to the internal buffer.
	//
	//# \operator	Compressor& operator <<(const unsigned_int64& x);
	//#				Writes a single 64-bit unsigned integer to the internal buffer.
	//
	//# \operator	Compressor& operator <<(const float& x);
	//#				Writes a single 32-bit floating-point value to the internal buffer.
	//
	//# \operator	Compressor& operator <<(const Vector2D& v);
	//#				Writes two 32-bit floating-point values to the internal buffer from the vector $v$.
	//
	//# \operator	Compressor& operator <<(const Vector3D& v);
	//#				Writes three 32-bit floating-point values to the internal buffer from the vector $v$.
	//
	//# \operator	Compressor& operator <<(const Vector4D& v);
	//#				Writes four 32-bit floating-point values to the internal buffer from the vector $v$.
	//
	//# \operator	Compressor& operator <<(const Quaternion& q);
	//#				Writes four 32-bit floating-point values to the internal buffer from the quaternion $q$.
	//
	//# \operator	Compressor& operator <<(const ColorRGB& c);
	//#				Writes three 32-bit floating-point values to the internal buffer from the color $c$.
	//
	//# \operator	Compressor& operator <<(const ColorRGBA& c);
	//#				Writes four 32-bit floating-point values to the internal buffer from the color $c$.
	//
	//# \operator	Compressor& operator <<(const char *text);
	//#				Writes a null-terminated text string to the internal buffer.
	//
	//# \also	$@Message::Compress@$
	//# \also	$@Decompressor@$


	//# \function	Compressor::GetSize		Returns the current size of the compressed data.
	//
	//# \proto	unsigned_int32 GetSize(void) const;
	//
	//# \desc
	//# The $GetSize$ function returns the total number of bytes written to the $@Compressor@$ object.
	//# When the $@Message::Compress@$ function is called, the $@Compressor@$ object passed in initially
	//# has a size of zero. No more than $kMaxMessageSize - 1$ bytes should be written to a $@Compressor@$ object.


	//# \function	Compressor::Write		Writes arbitrary data to the internal buffer.
	//
	//# \proto	Compressor& Write(const void *dataPtr, unsigned_int32 dataSize);
	//
	//# \desc
	//# The $Write$ function copies the number of bytes specified by the $dataSize$ parameter from the
	//# buffer specified by the $dataPtr$ parameter to the $@Compressor@$ object's internal buffer.
	//# No more than $kMaxMessageSize - 1$ total bytes should be written to a $@Compressor@$ object.
	//
	//# \special
	//# Data written to a $@Compressor@$ object by the $Write$ function is not converted to big endian
	//# order because its format is unknown. If cross-platform compatibility is of concern, care should
	//# be taken to ensure that data written by this function can be read by a receiving machine running
	//# on hardware having the opposite native endian.


	class Compressor
	{
		private:

			char			*pointer;
			unsigned_int32	size;

		public:

			Compressor(char *ptr)
			{
				pointer = ptr;
				size = 0;
			}

			unsigned_int32 GetSize(void) const
			{
				return (size);
			}

			Compressor& operator <<(const bool& x)
			{
				Assert(size < kMaxMessageDataSize, "Compressor::operator <<, message data size overflow\n");

				pointer[size] = (char) x;

				size += 1;
				return (*this);
			}

			Compressor& operator <<(const char& x)
			{
				Assert(size < kMaxMessageDataSize, "Compressor::operator <<, message data size overflow\n");

				pointer[size] = x;

				size += 1;
				return (*this);
			}

			Compressor& operator <<(const unsigned_int8& x)
			{
				Assert(size < kMaxMessageDataSize, "Compressor::operator <<, message data size overflow\n");

				*reinterpret_cast<unsigned_int8 *>(pointer + size) = x;

				size += 1;
				return (*this);
			}

			Compressor& operator <<(const int16& x)
			{
				Assert(size + 2 <= kMaxMessageDataSize, "Compressor::operator <<, message data size overflow\n");

				char *p = pointer + size;

				p[0] = (char) (x >> 8);
				p[1] = (char) x;

				size += 2;
				return (*this);
			}

			Compressor& operator <<(const unsigned_int16& x)
			{
				Assert(size + 2 <= kMaxMessageDataSize, "Compressor::operator <<, message data size overflow\n");

				unsigned_int8 *p = reinterpret_cast<unsigned_int8 *>(pointer + size);

				p[0] = (unsigned_int8) (x >> 8);
				p[1] = (unsigned_int8) x;

				size += 2;
				return (*this);
			}

			Compressor& operator <<(const int32& x)
			{
				Assert(size + 4 <= kMaxMessageDataSize, "Compressor::operator <<, message data size overflow\n");

				char *p = pointer + size;

				p[0] = (char) (x >> 24);
				p[1] = (char) (x >> 16);
				p[2] = (char) (x >> 8);
				p[3] = (char) x;

				size += 4;
				return (*this);
			}

			Compressor& operator <<(const unsigned_int32& x)
			{
				Assert(size + 4 <= kMaxMessageDataSize, "Compressor::operator <<, message data size overflow\n");

				unsigned_int8 *p = reinterpret_cast<unsigned_int8 *>(pointer + size);

				p[0] = (unsigned_int8) (x >> 24);
				p[1] = (unsigned_int8) (x >> 16);
				p[2] = (unsigned_int8) (x >> 8);
				p[3] = (unsigned_int8) x;

				size += 4;
				return (*this);
			}

			Compressor& operator <<(const int64& x)
			{
				Assert(size + 8 <= kMaxMessageDataSize, "Compressor::operator <<, message data size overflow\n");

				char *p = pointer + size;

				p[0] = (char) (x >> 56);
				p[1] = (char) (x >> 48);
				p[2] = (char) (x >> 40);
				p[3] = (char) (x >> 32);
				p[4] = (char) (x >> 24);
				p[5] = (char) (x >> 16);
				p[6] = (char) (x >> 8);
				p[7] = (char) x;

				size += 8;
				return (*this);
			}

			Compressor& operator <<(const unsigned_int64& x)
			{
				Assert(size + 8 <= kMaxMessageDataSize, "Compressor::operator <<, message data size overflow\n");

				unsigned_int8 *p = reinterpret_cast<unsigned_int8 *>(pointer + size);

				p[0] = (unsigned_int8) (x >> 56);
				p[1] = (unsigned_int8) (x >> 48);
				p[2] = (unsigned_int8) (x >> 40);
				p[3] = (unsigned_int8) (x >> 32);
				p[4] = (unsigned_int8) (x >> 24);
				p[5] = (unsigned_int8) (x >> 16);
				p[6] = (unsigned_int8) (x >> 8);
				p[7] = (unsigned_int8) x;

				size += 8;
				return (*this);
			}

			Compressor& operator <<(const float& x)
			{
				Assert(size + 4 <= kMaxMessageDataSize, "Compressor::operator <<, message data size overflow\n");

				char *p = pointer + size;

				#if C4BIGENDIAN

					p[0] = reinterpret_cast<const char *>(&x)[0];
					p[1] = reinterpret_cast<const char *>(&x)[1];
					p[2] = reinterpret_cast<const char *>(&x)[2];
					p[3] = reinterpret_cast<const char *>(&x)[3];

				#else

					p[0] = reinterpret_cast<const char *>(&x)[3];
					p[1] = reinterpret_cast<const char *>(&x)[2];
					p[2] = reinterpret_cast<const char *>(&x)[1];
					p[3] = reinterpret_cast<const char *>(&x)[0];

				#endif

				size += 4;
				return (*this);
			}

			Compressor& operator <<(const Vector2D& v)
			{
				return (*this << v.x << v.y);
			}

			Compressor& operator <<(const Vector3D& v)
			{
				return (*this << v.x << v.y << v.z);
			}

			Compressor& operator <<(const Vector4D& v)
			{
				return (*this << v.x << v.y << v.z << v.w);
			}

			Compressor& operator <<(const Quaternion& q)
			{
				return (*this << q.x << q.y << q.z << q.w);
			}

			Compressor& operator <<(const ColorRGB& c)
			{
				return (*this << c.red << c.green << c.blue);
			}

			Compressor& operator <<(const ColorRGBA& c)
			{
				return (*this << c.red << c.green << c.blue << c.alpha);
			}

			Compressor& operator <<(const char *text)
			{
				Assert(size + Text::GetTextLength(text) + 1 <= kMaxMessageDataSize, "Compressor::operator <<, message data size overflow\n");

				size += Text::CopyText(text, pointer + size) + 1;
				return (*this);
			}

			Compressor& Write(const void *dataPtr, unsigned_int32 dataSize)
			{
				Assert(size + dataSize <= kMaxMessageDataSize, "Compressor::Write(), message data size overflow\n");

				MemoryMgr::CopyMemory(dataPtr, pointer + size, dataSize);
				size += dataSize;
				return (*this);
			}
	};


	//# \class	Decompressor		Decompresses data for a message.
	//
	//# \def	class Decompressor
	//
	//# \ctor	Decompressor(char *ptr);
	//
	//# $Decompressor$ objects should be constructed only by the Message Manager.
	//
	//# \desc
	//# A $Decompressor$ object is passed to the $@Message::Decompress@$ function after the Message Manager
	//# receives a message. The message object should use the $>>$ operator or the $@Decompressor::Read@$
	//# function to read its data from the $Decompressor$ object's internal buffer. Data read by the
	//# $>>$ operator is always converted from big endian order to the native endian of the local machine
	//# so that messages can be sent between machines having different native endians.
	//
	//# \operator	Decompressor& operator >>(char& x);
	//#				Reads a single 8-bit character from the internal buffer.
	//
	//# \operator	Decompressor& operator >>(unsigned_int8& x);
	//#				Reads a single 8-bit unsigned character from the internal buffer.
	//
	//# \operator	Decompressor& operator >>(int16& x);
	//#				Reads a single 16-bit integer from the internal buffer.
	//
	//# \operator	Decompressor& operator >>(unsigned_int16& x);
	//#				Reads a single 16-bit unsigned integer from the internal buffer.
	//
	//# \operator	Decompressor& operator >>(int32& x);
	//#				Reads a single 32-bit integer from the internal buffer.
	//
	//# \operator	Decompressor& operator >>(unsigned_int32& x);
	//#				Reads a single 32-bit unsigned integer from the internal buffer.
	//
	//# \operator	Decompressor& operator >>(int64& x);
	//#				Reads a single 64-bit integer from the internal buffer.
	//
	//# \operator	Decompressor& operator >>(unsigned_int64& x);
	//#				Reads a single 64-bit unsigned integer from the internal buffer.
	//
	//# \operator	Decompressor& operator >>(float& x);
	//#				Reads a single 32-bit floating-point value from the internal buffer.
	//
	//# \operator	Decompressor& operator >>(Vector2D& v);
	//#				Reads two 32-bit floating-point values from the internal buffer and stores them in the vector $v$.
	//
	//# \operator	Decompressor& operator >>(Vector3D& v);
	//#				Reads three 32-bit floating-point values from the internal buffer and stores them in the vector $v$.
	//
	//# \operator	Decompressor& operator >>(Vector4D& v);
	//#				Reads four 32-bit floating-point values from the internal buffer and stores them in the vector $v$.
	//
	//# \operator	Decompressor& operator >>(Quaternion& q);
	//#				Reads four 32-bit floating-point values from the internal buffer and stores them in the quaternion $q$.
	//
	//# \operator	Decompressor& operator >>(ColorRGB& c);
	//#				Reads three 32-bit floating-point values from the internal buffer and stores them in the color $c$.
	//
	//# \operator	Decompressor& operator >>(ColorRGBA& c);
	//#				Reads four 32-bit floating-point values from the internal buffer and stores them in the color $c$.
	//
	//# \operator	template <int32 len> Decompressor& operator >>(String<len>& text);
	//#				Reads a null-terminated text string from the internal buffer.
	//
	//# \also	$@Message::Decompress@$
	//# \also	$@Compressor@$


	//# \function	Decompressor::GetSize		Returns the total size of the compressed data.
	//
	//# \proto	unsigned_int32 GetSize(void) const;
	//
	//# \desc
	//# The $GetSize$ function returns the total number of bytes stored in the $@Decompressor@$ object.


	//# \function	Decompressor::Read		Reads arbitrary data from the internal buffer.
	//
	//# \proto	Decompressor& Read(void *dataPtr, unsigned_int32 dataSize);
	//
	//# \desc
	//# The $Read$ function copies the number of bytes specified by the $dataSize$ parameter from the
	//# $@Decompressor@$ object's internal buffer to the buffer specified by the $dataPtr$ parameter.
	//
	//# \special
	//# Data read from a $@Decompressor@$ object by the $Read$ function is not converted to the native
	//# endian of the local machine because its format is unknown. If cross-platform compatibility is of
	//# concern, care should be taken to ensure that data written by the $@Compressor::Write@$ function
	//# can be read by the $Decompressor::Read$ function on a receiving machine running on hardware having
	//# the opposite native endian.


	class Decompressor
	{
		private:

			const char		*pointer;
			unsigned_int32	size;

		public:

			Decompressor(const char *ptr)
			{
				pointer = ptr;
				size = 0;
			}

			unsigned_int32 GetSize(void) const
			{
				return (size);
			}

			Decompressor& operator >>(bool& x)
			{
				x = (pointer[size] != 0);

				size += 1;
				return (*this);
			}

			Decompressor& operator >>(char& x)
			{
				x = pointer[size];

				size += 1;
				return (*this);
			}

			Decompressor& operator >>(unsigned_int8& x)
			{
				x = *reinterpret_cast<const unsigned_int8 *>(pointer + size);

				size += 1;
				return (*this);
			}

			Decompressor& operator >>(int16& x)
			{
				const unsigned_int8 *p = reinterpret_cast<const unsigned_int8 *>(pointer + size);
				x = (int16) ((*reinterpret_cast<const int8 *>(p) << 8) | p[1]);

				size += 2;
				return (*this);
			}

			Decompressor& operator >>(unsigned_int16& x)
			{
				const unsigned_int8 *p = reinterpret_cast<const unsigned_int8 *>(pointer + size);
				x = (unsigned_int16) ((p[0] << 8) | p[1]);

				size += 2;
				return (*this);
			}

			Decompressor& operator >>(int32& x)
			{
				const unsigned_int8 *p = reinterpret_cast<const unsigned_int8 *>(pointer + size);
				x = (*reinterpret_cast<const int8 *>(p) << 24) | (p[1] << 16) | (p[2] << 8) | p[3];

				size += 4;
				return (*this);
			}

			Decompressor& operator >>(unsigned_int32& x)
			{
				const unsigned_int8 *p = reinterpret_cast<const unsigned_int8 *>(pointer + size);
				x = (p[0] << 24) | (p[1] << 16) | (p[2] << 8) | p[3];

				size += 4;
				return (*this);
			}

			Decompressor& operator >>(int64& x)
			{
				const unsigned_int8 *p = reinterpret_cast<const unsigned_int8 *>(pointer + size);
				x = ((int64) *reinterpret_cast<const int8 *>(p) << 56) | ((int64) p[1] << 48) | ((int64) p[2] << 40) | ((int64) p[3] << 32) | ((int64) p[4] << 24) | ((int64) p[5] << 16) | ((int64) p[6] << 8) | (int64) p[7];

				size += 8;
				return (*this);
			}

			Decompressor& operator >>(unsigned_int64& x)
			{
				const unsigned_int8 *p = reinterpret_cast<const unsigned_int8 *>(pointer + size);
				x = ((unsigned_int64) p[0] << 56) | ((unsigned_int64) p[1] << 48) | ((unsigned_int64) p[2] << 40) | ((unsigned_int64) p[3] << 32) | ((unsigned_int64) p[4] << 24) | ((unsigned_int64) p[5] << 16) | ((unsigned_int64) p[6] << 8) | (unsigned_int64) p[7];

				size += 8;
				return (*this);
			}

			Decompressor& operator >>(float& x)
			{
				const char *p = pointer + size;

				#if C4BIGENDIAN

					reinterpret_cast<char *>(&x)[0] = p[0];
					reinterpret_cast<char *>(&x)[1] = p[1];
					reinterpret_cast<char *>(&x)[2] = p[2];
					reinterpret_cast<char *>(&x)[3] = p[3];

				#else

					reinterpret_cast<char *>(&x)[3] = p[0];
					reinterpret_cast<char *>(&x)[2] = p[1];
					reinterpret_cast<char *>(&x)[1] = p[2];
					reinterpret_cast<char *>(&x)[0] = p[3];

				#endif

				size += 4;
				return (*this);
			}

			Decompressor& operator >>(Vector2D& v)
			{
				return (*this >> v.x >> v.y);
			}

			Decompressor& operator >>(Vector3D& v)
			{
				return (*this >> v.x >> v.y >> v.z);
			}

			Decompressor& operator >>(Vector4D& v)
			{
				return (*this >> v.x >> v.y >> v.z >> v.w);
			}

			Decompressor& operator >>(Quaternion& q)
			{
				return (*this >> q.x >> q.y >> q.z >> q.w);
			}

			Decompressor& operator >>(ColorRGB& c)
			{
				return (*this >> c.red >> c.green >> c.blue);
			}

			Decompressor& operator >>(ColorRGBA& c)
			{
				return (*this >> c.red >> c.green >> c.blue >> c.alpha);
			}

			template <int32 len> Decompressor& operator >>(String<len>& text)
			{
				size += Text::CopyText(pointer + size, text, len) + 1;
				return (*this);
			}

			Decompressor& Read(void *dataPtr, unsigned_int32 dataSize)
			{
				MemoryMgr::CopyMemory(pointer + size, dataPtr, dataSize);
				size += dataSize;
				return (*this);
			}
	};


	//# \class	Message		Base class for all message types.
	//
	//# \def	class Message : public ListElement<Message>, public Memory<MessageMgr>
	//
	//# \ctor	Message(MessageType type, unsigned_int32 flags = 0);
	//
	//# \param	type	The type of message.
	//# \param	flags	The message flags.
	//
	//# \desc
	//# Every type of message is encapsulated by a subclass of the $Message$ class. The $type$ parameter
	//# passed to the $Message$ constructor identifies the type of message being constructed and should
	//# be passed in from the constructor of a subclass.
	//#
	//# Each subclass of the $Message$ class must implement the following three functions.
	//
	//# <ol>
	//# <li>A default constructor that takes no parameters. When the Message Manager receives a message
	//# having an application-defined type, it calls the $@System/Application::CreateMessage@$ function with
	//# the message type, and this function should return a newly created $Message$ subclass corresponding
	//# to that type. The default constructor should not initialize any data fields because the Message Manager
	//# subsequently calls the object's $Decompress$ function to initialize the object using data from the
	//# received network packet.</li>
	//
	//# <li>A $Compress$ function. The Message Manager calls this function to compress and serialize the
	//# message into a buffer that will be sent as a data packet.</li>
	//
	//# <li>A $Decompress$ function. The Message Manager calls this function to decompress a data packet
	//# into the message object and validate its contents.</li>
	//# </ol>
	//
	//# Subclasses of the $Message$ class may have, in addition to the default constructor, constructors
	//# that take initializers. Such a constructor would be useful for creating a new message that is
	//# to be sent to another machine.
	//
	//# \base	Utilities/ListElement<Message>	Used internally by the Message Manager.
	//# \base	MemoryMgr/Memory<MessageMgr>	Memory for $Message$ objects is allocated in the Message Manager's heap.
	//
	//# \also	$@MessageMgr@$


	//# \function	Message::GetMessageType		Returns the dynamic type identifier of a message.
	//
	//# \proto	MessageType GetMessageType(void) const;
	//
	//# \desc
	//# The $GetMessageType$ function is typically called from the $@System/Application::CreateMessage@$ function
	//# to determine what type of message needs to be constructed or from the $@System/Application::ReceiveMessage@$
	//# function to determine what type of message is being received.
	//
	//# \also	$@System/Application::CreateMessage@$
	//# \also	$@System/Application::ReceiveMessage@$


	//# \function	Message::GetMessageFlags	Returns flags indicating whether a message requires special handling.
	//
	//# \proto	unsigned_int32 GetMessageFlags(void) const;
	//
	//# \desc
	//# The return value of the $GetMessageFlags$ function may be zero or a combination (through logical OR) of the
	//# following constants.
	//
	//# \table	MessageFlags
	//
	//# \also	$@Message::SetMessageFlags@$


	//# \function	Message::SetMessageFlags	Sets flags indicating whether a message requires special handling.
	//
	//# \proto	void SetMessageFlags(unsigned_int32 flags);
	//
	//# \param	flags	The new flags for this message.
	//
	//# \desc
	//# The $SetMessageFlags$ function sets flags for a message that indicate special properties.
	//# The $flags$ parameter may be zero or a combination (through logical OR) of the following constants.
	//
	//# \table	MessageFlags
	//
	//# When a new message is constructed, its flags are set to zero. Thus, all messages are sent
	//# reliably by default.
	//
	//# \also	$@Message::GetMessageFlags@$


	//# \function	Message::Compress		Called by the Message Manager to compress and serialize message data into a memory buffer.
	//
	//# \proto	virtual void Compress(Compressor& data) const override;
	//
	//# \param	data	The $@Compressor@$ object that will hold the message's data.
	//
	//# \desc
	//# The Message Manager calls the $Compress$ function to compress and serialize the message data into
	//# a the buffer that will be transmitted as a data packet on the network. The implementation should
	//# compress the message data in whatever manner it wishes and write the data using the member functions
	//# of the $@Compressor@$ object referenced by the $data$ parameter. No more than $kMaxMessageSize - 1$
	//# bytes should be written to this object. (This length is one less than $kMaxMessageSize$ because the
	//# first byte is used as the message type identifier.)
	//
	//# \also	$@Message::Decompress@$


	//# \function	Message::Decompress		Called by the Message Manager to decompress and validate message data stored in a memory buffer.
	//
	//# \proto	virtual bool Decompress(Decompressor& data) override;
	//
	//# \param	data	The $@Decompressor@$ object that holds the message's data.
	//
	//# \desc
	//# The Message Manager calls the $Decompress$ function to decompress a data packet into a message
	//# object and validate its contents. The implementation should decompress the data using the member
	//# functions of the $@Decompressor@$ object referenced by the $data$ parameter. The size of the data
	//# stored in the object can be determined by calling $@Decompressor::GetSize@$. After decompressing
	//# the data, the $Decompress$ function should validate the contents of the message in whatever manner
	//# it can and return a boolean value indicating whether the data appears to be valid. If the
	//# $Decompress$ function returns $false$, then the Message Manager discards the message.
	//
	//# \also	$@Message::Compress@$


	//# \function	Message::HandleMessage		Called by the Message Manager to respond to a received message.
	//
	//# \proto	virtual bool HandleMessage(Player *sender) const override;
	//
	//# \param	sender		A pointer to the $@Player@$ instance who sent the message.
	//
	//# \desc
	//# When a message is received on any machine, the Message Manager calls the $HandleMessage$ function for
	//# the message object before attempting any other kind of processing for the message. The $HandleMessage$
	//# function can be overridden in a $Message$ subclass in order to provide code that responds to the
	//# particular message type. If the $HandleMessage$ function returns $true$, then the Message Manager
	//# considers the message handled, and no further processing is performed. If the $HandleMessage$ function
	//# returns $false$, then the Message Manager examines the message type and handles it accordingly.
	//# For any custom message types, the $@System/Application::ReceiveMessage@$ function is then called for
	//# the message object.
	//
	//# \also	$@System/Application::ReceiveMessage@$
	//# \also	$@System/Application::CreateMessage@$
	//# \also	$@Player@$


	class Message : public ListElement<Message>, public EngineMemory<MessageMgr>
	{
		friend class MessageMgr;

		private:

			MessageType					messageType;
			mutable unsigned_int32		messageFlags;

		protected:

			C4API Message(MessageType type, unsigned_int32 flags = 0);

		public:

			C4API virtual ~Message();

			MessageType GetMessageType(void) const
			{
				return (messageType);
			}

			unsigned_int32 GetMessageFlags(void) const
			{
				return (messageFlags);
			}

			void SetMessageFlags(unsigned_int32 flags)
			{
				messageFlags = flags;
			}

			C4API virtual void Compress(Compressor& data) const;
			C4API virtual bool Decompress(Decompressor& data);

			C4API virtual bool HandleMessage(Player *sender) const;
	};


	//# \class	PingMessage		Encapsulates a message that contains no data.
	//
	//# The $PingMessage$ class encapsulates a message that contains no data.
	//
	//# \def	class PingMessage : public Message
	//
	//# \ctor	PingMessage();
	//
	//# \desc
	//# A message of type $PingMessage$ is sent to a player in order to update the Network Manager's
	//# ping value for the corresponding machine. The Network Manager records a ping value for every
	//# reliable message that is sent. Sending a ping message is simply a way to guarantee that the ping
	//# value for a particular player is up to date.
	//
	//# \base	Message		All message types are subclasses of $Message$.
	//
	//# \also	$@Player::GetNetworkPing@$


	class PingMessage : public Message
	{
		public:

			C4API PingMessage();
			C4API ~PingMessage();
	};


	class ServerQueryMessage : public Message
	{
		friend class MessageMgr;

		private:

			unsigned_int32	protocol;

			ServerQueryMessage();

		public:

			ServerQueryMessage(unsigned_int32 prot);
			~ServerQueryMessage();

			unsigned_int32 GetProtocol(void) const
			{
				return (protocol);
			}

			void Compress(Compressor& data) const override;
			bool Decompress(Decompressor& data) override;
	};


	class ConnectMessage : public Message
	{
		friend class MessageMgr;

		private:

			PlayerKey						playerKey;
			String<kMaxPlayerNameLength>	playerName;

			ConnectMessage();

		public:

			ConnectMessage(PlayerKey key, const char *name);
			~ConnectMessage();

			PlayerKey GetPlayerKey(void) const
			{
				return (playerKey);
			}

			const char *GetPlayerName(void) const
			{
				return (playerName);
			}

			void Compress(Compressor& data) const override;
			bool Decompress(Decompressor& data) override;
	};


	class DisconnectMessage : public Message
	{
		friend class MessageMgr;

		private:

			PlayerKey		playerKey;

			DisconnectMessage();

		public:

			DisconnectMessage(PlayerKey key);
			~DisconnectMessage();

			PlayerKey GetPlayerKey(void) const
			{
				return (playerKey);
			}

			void Compress(Compressor& data) const override;
			bool Decompress(Decompressor& data) override;
	};


	class TimeoutMessage : public Message
	{
		friend class MessageMgr;

		private:

			PlayerKey		playerKey;

			TimeoutMessage();

		public:

			TimeoutMessage(PlayerKey key);
			~TimeoutMessage();

			PlayerKey GetPlayerKey(void) const
			{
				return (playerKey);
			}

			void Compress(Compressor& data) const override;
			bool Decompress(Decompressor& data) override;
	};


	class ClientInfoMessage : public Message
	{
		friend class MessageMgr;

		private:

			String<kMaxPlayerNameLength>	playerName;

			ClientInfoMessage();

		public:

			ClientInfoMessage(const char *name);
			~ClientInfoMessage();

			const char *GetPlayerName(void) const
			{
				return (playerName);
			}

			void Compress(Compressor& data) const override;
			bool Decompress(Decompressor& data) override;
	};


	class PlayerKeyMessage : public Message
	{
		friend class MessageMgr;

		private:

			PlayerKey		playerKey;

			PlayerKeyMessage();

		public:

			PlayerKeyMessage(PlayerKey key);
			~PlayerKeyMessage();

			PlayerKey GetPlayerKey(void) const
			{
				return (playerKey);
			}

			void Compress(Compressor& data) const override;
			bool Decompress(Decompressor& data) override;
	};


	class SynchronizeMessage : public Message
	{
		public:

			SynchronizeMessage();
			~SynchronizeMessage();
	};


	//# \class	ChatMessage		Encapsulates a message that contains a text string.
	//
	//# The $ChatMessage$ class encapsulates a message that contains a text string.
	//
	//# \def	class ChatMessage : public ReflectingMessage
	//
	//# \ctor	ChatMessage(const char *text);
	//
	//# \param	text	A pointer to the text string to be stored in the message object. This string
	//#					is truncated to the maximum length given by the $kMaxChatMessageLength$ constant.
	//
	//# \desc
	//# A message of type $ChatMessage$ is used to send a text message to other players in a game.
	//# The $ChatMessage$ object contains both the text message and the player ID of the machine from which
	//# it originated. When a chat message is received, the Message Manager calls the
	//# $@System/Application::HandlePlayerEvent@$ function with the $kPlayerChatReceived$ event to pass the
	//# text message to the application module. In this case, the $param$ parameter passed to the
	//# $HandlePlayerEvent$ function points to the chat text string.
	//
	//# \base	Message		All message types are subclasses of $Message$.
	//
	//# \also	$@System/Application::HandlePlayerEvent@$


	class ChatMessage : public Message
	{
		friend class MessageMgr;

		private:

			mutable PlayerKey				playerKey;
			String<kMaxChatMessageLength>	chatText;

			ChatMessage();

		public:

			C4API ChatMessage(const char *text);
			C4API ~ChatMessage();

			PlayerKey GetPlayerKey(void) const
			{
				return (playerKey);
			}

			const char *GetChatText(void) const
			{
				return (chatText);
			}

			void Compress(Compressor& data) const override;
			bool Decompress(Decompressor& data) override;

			bool HandleMessage(Player *sender) const override;
	};


	class NameMessage : public Message
	{
		friend class MessageMgr;

		private:

			PlayerKey						playerKey;
			String<kMaxPlayerNameLength>	playerName;

			NameMessage();

		public:

			C4API NameMessage(PlayerKey key, const char *name);
			C4API ~NameMessage();

			PlayerKey GetPlayerKey(void) const
			{
				return (playerKey);
			}

			const char *GetPlayerName(void) const
			{
				return (playerName);
			}

			void Compress(Compressor& data) const override;
			bool Decompress(Decompressor& data) override;

			bool HandleMessage(Player *sender) const override;
	};


	class FileRequestMessage : public Message
	{
		friend class MessageMgr;

		private:

			String<kMaxFileNameLength>	fileName;

			FileRequestMessage();

		public:

			FileRequestMessage(const char *name);
			~FileRequestMessage();

			const char *GetFileName(void) const
			{
				return (fileName);
			}

			void Compress(Compressor& data) const override;
			bool Decompress(Decompressor& data) override;

			bool HandleMessage(Player *sender) const override;
	};


	class FileReceiveMessage : public Message
	{
		friend class MessageMgr;

		private:

			unsigned_int64				fileSize;
			String<kMaxFileNameLength>	fileName;

			FileReceiveMessage();

		public:

			FileReceiveMessage(const char *name, unsigned_int64 size);
			~FileReceiveMessage();

			const char *GetFileName(void) const
			{
				return (fileName);
			}

			unsigned_int64 GetFileSize(void) const
			{
				return (fileSize);
			}

			void Compress(Compressor& data) const override;
			bool Decompress(Decompressor& data) override;

			bool HandleMessage(Player *sender) const override;
	};


	class FileErrorMessage : public Message
	{
		friend class MessageMgr;

		private:

			FileTransferResult		fileResult;

			FileErrorMessage();

		public:

			FileErrorMessage(FileTransferResult result);
			~FileErrorMessage();

			FileTransferResult GetResult(void) const
			{
				return (fileResult);
			}

			void Compress(Compressor& data) const override;
			bool Decompress(Decompressor& data) override;

			bool HandleMessage(Player *sender) const override;
	};


	class FileCancelMessage : public Message
	{
		public:

			FileCancelMessage();
			~FileCancelMessage();

			bool HandleMessage(Player *sender) const override;
	};


	class FileChunkMessage : public Message
	{
		friend class MessageMgr;

		private:

			unsigned_int32		chunkSize;
			char				chunkData[kMaxMessageDataSize];

			FileChunkMessage();

		public:

			FileChunkMessage(unsigned_int32 size);
			~FileChunkMessage();

			const void *GetChunkData(void) const
			{
				return (chunkData);
			}

			void *GetChunkData(void)
			{
				return (chunkData);
			}

			int32 GetChunkSize(void) const
			{
				return (chunkSize);
			}

			void Compress(Compressor& data) const override;
			bool Decompress(Decompressor& data) override;

			bool HandleMessage(Player *sender) const override;
	};


	//# \class	ControllerMessage		Base class for all controller message types.
	//
	//# \def	class ControllerMessage : public Message, public ListElement<ControllerMessage>
	//
	//# \ctor	ControllerMessage(ControllerMessageType type, int32 index, unsigned_int32 flags = 0);
	//
	//# \param	type	The type of controller message.
	//# \param	index	The World Manager controller index for this message. For a particular controller,
	//#					this index can be retrieved using the $@Controller/Controller::GetControllerIndex@$ function.
	//# \param	flags	The message flags.
	//
	//# \desc
	//# Controller messages are sent directly to $@Controller/Controller@$ objects in order to synchronize
	//# the state or motion of the scene graph nodes that they control. A $ControllerMessage$ object may
	//# contain no information beyond its type and controller index, or it may be subclassed so that extra
	//# data may be added to it.
	//
	//# The application module on the server machine typically constructs a controller message when it needs
	//# to inform client machines about a change in state for a particular controller and then calls either the
	//# $@MessageMgr::SendMessageAll@$ function or the $@MessageMgr::SendMessageJournal@$ function to distribute
	//# the message. The controller index given by the $index$ parameter identifies the particular controller
	//# object that will receive the message on each machine. On the receiving machines, the
	//# $@Controller/Controller::ReceiveMessage@$ function is called to process the controller message.
	//
	//# \base	Message										All message types are subclasses of $Message$.
	//# \base	Utilities/ListElement<ControllerMessage>	Used internally by the Message Manager.
	//
	//# \also	$@Controller/Controller::GetControllerIndex@$
	//# \also	$@Controller/Controller::CreateMessage@$
	//# \also	$@Controller/Controller::ReceiveMessage@$


	//# \function	ControllerMessage::GetControllerMessageType		Returns the dynamic type identifier of a controller message.
	//
	//# \proto	ControllerMessageType GetControllerMessageType(void) const;
	//
	//# \desc
	//# The $GetControllerMessageType$ function is typically called from the
	//# $@Controller/Controller::CreateMessage@$ function to determine what type of controller message
	//# needs to be constructed or from the $@Controller/Controller::ReceiveMessage@$ function to determine
	//# what type of message is being received.
	//
	//# \also	$@Controller/Controller::CreateMessage@$
	//# \also	$@Controller/Controller::ReceiveMessage@$


	//# \function	ControllerMessage::GetControllerIndex		Returns the World Manager controller index corresponding to a controller message.
	//
	//# \proto	int32 GetControllerIndex(void) const;
	//
	//# \desc
	//# The $GetControllerIndex$ function returns the controller index corresponding to the $ControllerMessage$
	//# object. The associated controller can be retrieved by calling the $@WorldMgr/World::GetController@$ function.
	//
	//# \also	$@WorldMgr/World::GetController@$


	//# \function	ControllerMessage::OverridesMessage		Returns a boolean value indicating whether a message overrides a previous message in the journal.
	//
	//# \proto	virtual bool OverridesMessage(const ControllerMessage *message) const;
	//
	//# \param	message		A journaled message previously sent to the same controller as the destination for this message.
	//
	//# \desc
	//# The $OverridesMessage$ function may be called one or more times when a journaled message is sent using the
	//# $@MessageMgr/MessageMgr::SendMessageJournal@$ function. Its purpose is to determine whether it overrides
	//# any journaled messages previously sent to the same controller. The $OverridesMessage$ function is called on
	//# the new message object once for each previous message object in the journal for the same controller. It should
	//# inspect the message object pointed to by the $message$ parameter and return $true$ if it overrides it.
	//# Returning $true$ will cause the previous message to be removed from the journal.
	//
	//# \also	$@MessageMgr::SendMessageJournal@$


	class ControllerMessage : public Message, public ListElement<ControllerMessage>
	{
		private:

			ControllerMessageType		controllerMessageType;
			int32						controllerIndex;

		public:

			C4API ControllerMessage(ControllerMessageType type, int32 index, unsigned_int32 flags = 0);
			C4API ~ControllerMessage();

			using ListElement<ControllerMessage>::Previous;
			using ListElement<ControllerMessage>::Next;

			ControllerMessageType GetControllerMessageType(void) const
			{
				return (controllerMessageType);
			}

			int32 GetControllerIndex(void) const
			{
				return (controllerIndex);
			}

			C4API void Compress(Compressor& data) const override;
			C4API bool Decompress(Decompressor& data) override;

			C4API bool HandleMessage(Player *sender) const override;

			C4API virtual void HandleControllerMessage(Controller *controller) const;
			C4API virtual bool OverridesMessage(const ControllerMessage *message) const;
	};


	class CombinedMessage : public ListElement<CombinedMessage>, public EngineMemory<MessageMgr>
	{
		private:

			unsigned_int32		combinedFlags;
			unsigned_int32		packetFlags;

			int32				messageCount;
			unsigned_int32		messageSize;

			char				messageBuffer[kMaxCombinedMessageSize];

		public:

			CombinedMessage(MessageType type, unsigned_int32 size, const char *data, unsigned_int32 flags);
			~CombinedMessage();

			int32 GetMessageCount(void) const
			{
				return (messageCount);
			}

			unsigned_int32 GetMessageSize(void) const
			{
				return (messageSize);
			}

			unsigned_int32 GetCombinedFlags(void) const
			{
				return (combinedFlags);
			}

			bool AddMessage(MessageType type, unsigned_int32 size, const char *data);

			NetworkResult Send(const NetworkAddress& address);
	};


	//# \class	StateSender		Encapsulates a function that sends initial game state in a multiplayer game.
	//
	//# \def	class StateSender : public ListElement<StateSender>
	//
	//# \ctor	StateSender(SenderProc *proc, void *cookie);
	//
	//# \param	proc	A pointer to the state-sending function.
	//# \param	cookie	A pointer to application-defined data that is passed to the state-sending function.
	//
	//# \desc
	//# The $StateSender$ class encapsulates a function that is called whenever a new client machine joins
	//# a multiplayer game. All state-sending functions are called immediately after the Message Manager
	//# sends the $kPlayerInitialized$ event to the $@System/Application::HandlePlayerEvent@$ function on
	//# the server machine. The function pointed to by the $proc$ parameter should have the following
	//# prototype.
	//
	//# \code	typedef void SenderProc(Player *to, void *);
	//
	//# The state-sending function should send any necessary messages to the player specified by the $to$
	//# parameter in order to synchronize some kind of initial game state.
	//#
	//# The World Manager installs a state-sending function that sends initial controller state messages
	//# to a new client. A controller can send initial game state messages by overriding the
	//# $@Controller/Controller::SendInitialStateMessages@$ function.
	//
	//# \base	Utilities/ListElement<StateSender>		Used internally by the Message Manager.
	//
	//# \also	$@MessageMgr::InstallStateSender@$
	//# \also	$@Controller/Controller::SendInitialStateMessages@$


	class StateSender : public ListElement<StateSender>
	{
		public:

			typedef void SenderProc(Player *to, void *);

		private:

			SenderProc		*senderProc;
			void			*senderCookie;

		public:

			C4API StateSender(SenderProc *proc, void *cookie);
			C4API ~StateSender();

			void SendState(Player *to) const
			{
				(*senderProc)(to, senderCookie);
			}
	};


	//# \class	SnapshotSender		The base class for objects that need to send snapshot data.
	//
	//# \def	class SnapshotSender : public ListElement<SnapshotSender>
	//
	//# \ctor	SnapshotSender();
	//
	//# \desc
	//# The $SnapshotSender$ class is the base class that objects should inherit from if they need to
	//# send snapshot data to client machines in a multiplayer game. Snapshots are sent at regular intervals
	//# by the Message Manager. When a snapshot is sent, the $@SnapshotSender::SendSnapshot@$ function is
	//# called for each snapshot sender that is currently registered. (A subclass of $SnapshotSender$ must
	//# override this function.) A snapshot sender is registered by calling the $@MessageMgr::AddSnapshotSender@$ function.
	//#
	//# The Message Manager regularly schedules opportunities to send a snapshot at the interval set by the
	//# $@MessageMgr::SetSnapshotInterval@$ function. A snapshot sender may send a snapshot at each opportunity,
	//# or it may send a snapshot only at each <i>n</i>-th opportunity, where <i>n</i> is a period set by the
	//# $SnapshotSender::SetSnapshotPeriod$ function. By default, the snapshot period is 1, meaning that a
	//# snapshot is sent at every opportunity scheduled by the Message Manager.
	//
	//# \base	Utilities/ListElement<SnapshotSender>	Used internally by the Message Manager.
	//
	//# \also	$@MessageMgr::AddSnapshotSender@$
	//# \also	$@MessageMgr::RemoveSnapshotSender@$
	//# \also	$@MessageMgr::GetSnapshotInterval@$
	//# \also	$@MessageMgr::GetSnapshotFrequency@$
	//# \also	$@MessageMgr::SetSnapshotInterval@$


	//# \function	SnapshotSender::SendSnapshot	Called when the Message Manager sends a snapshot.
	//
	//# \proto	virtual void SendSnapshot(void) = 0;
	//
	//# \desc
	//# The $SendSnapshot$ function is called by the Message Manager on the server machine at regular intervals
	//# for each snapshot sender. The frequency at which the $SendSnapshot$ function is called depends on the
	//# sender-specific snapshot period set by the $SnapshotSender::SetSnapshotPeriod$ function and the global
	//# snapshot interval set by the $@MessageMgr::SetSnapshotInterval@$ function.
	//#
	//# The implementation of the $SendSnapshot$ function should typically send messages to client machines using
	//# the $@MessageMgr::SendMessageClients@$ function, but it may send messages only to particular client machines
	//# instead if it's not necessary to send information to all clients. The $SendSnapshot$ function should not
	//# send messages that would be received by the server machine; in particular, it should not call the
	//# $@MessageMgr::SendMessageAll@$ function. Nor should the $SendSnapshot$ function send any messages that are journaled.
	//
	//# \also	$@SnapshotSender::GetSnapshotPeriod@$
	//# \also	$@SnapshotSender::SetSnapshotPeriod@$
	//# \also	$@SnapshotSender::ExpediteSnapshot@$
	//# \also	$@MessageMgr::AddSnapshotSender@$
	//# \also	$@MessageMgr::RemoveSnapshotSender@$
	//# \also	$@MessageMgr::GetSnapshotInterval@$
	//# \also	$@MessageMgr::GetSnapshotFrequency@$
	//# \also	$@MessageMgr::SetSnapshotInterval@$


	//# \function	SnapshotSender::GetSnapshotPeriod	Returns the snapshot period.
	//
	//# \proto	int32 GetSnapshotPeriod(void) const;
	//
	//# \desc
	//# The $GetSnapshotPeriod$ function returns the period for a snapshot sender. The period is measured
	//# as an integer number of snapshot sending opportunities. In general, a period of <i>n</i> means that
	//# a snapshot is sent every <i>n</i>-th time the Message Manager schedules an opportunity to send snapshots.
	//# A period of 1 means that a snapshot is sent as often as possible, and this is the initial value upon
	//# construction of a $SnapshotSender$ object.
	//
	//# \also	$@SnapshotSender::SetSnapshotPeriod@$
	//# \also	$@SnapshotSender::ExpediteSnapshot@$
	//# \also	$@SnapshotSender::SendSnapshot@$
	//# \also	$@MessageMgr::GetSnapshotInterval@$
	//# \also	$@MessageMgr::GetSnapshotFrequency@$
	//# \also	$@MessageMgr::SetSnapshotInterval@$


	//# \function	SnapshotSender::SetSnapshotPeriod	Sets the snapshot period.
	//
	//# \proto	void SetSnapshotPeriod(int32 period);
	//
	//# \param	period		The new snapshot period. This should be greater than or equal to 1.
	//
	//# \desc
	//# The $SetSnapshotPeriod$ function sets the period for a snapshot sender to that specified by the $period$
	//# parameter. The period is measured as an integer number of snapshot sending opportunities. In general, a
	//# period of <i>n</i> means that a snapshot is sent every <i>n</i>-th time the Message Manager schedules an
	//# opportunity to send snapshots. A period of 1 means that a snapshot is sent as often as possible, and this
	//# is the initial value upon construction of a $SnapshotSender$ object.
	//
	//# \also	$@SnapshotSender::GetSnapshotPeriod@$
	//# \also	$@SnapshotSender::ExpediteSnapshot@$
	//# \also	$@SnapshotSender::SendSnapshot@$
	//# \also	$@MessageMgr::GetSnapshotInterval@$
	//# \also	$@MessageMgr::GetSnapshotFrequency@$
	//# \also	$@MessageMgr::SetSnapshotInterval@$


	//# \function	SnapshotSender::ExpediteSnapshot	Causes a snapshot to be sent as soon as possible.
	//
	//# \proto	void ExpediteSnapshot(void);
	//
	//# \desc
	//# The $ExpediteSnapshot$ function causes a snapshot to be sent at the next opportunity schedule by the
	//# Message Manager without regard for the snapshot period. This function should be called when a major change
	//# is made to an object's state and that information needs to be transmitted to client machines sooner than
	//# it might ordinarily be done.
	//
	//# \also	$@SnapshotSender::GetSnapshotPeriod@$
	//# \also	$@SnapshotSender::SetSnapshotPeriod@$
	//# \also	$@SnapshotSender::SendSnapshot@$


	class SnapshotSender : public ListElement<SnapshotSender>
	{
		friend class MessageMgr;

		private:

			int32		snapshotPeriod;
			int32		snapshotCount;

		protected:

			C4API SnapshotSender();

		public:

			C4API virtual ~SnapshotSender();

			int32 GetSnapshotPeriod(void) const
			{
				return (snapshotPeriod);
			}

			void SetSnapshotPeriod(int32 period)
			{
				snapshotPeriod = period;
			}

			void ExpediteSnapshot(void)
			{
				snapshotCount = 1;
			}

			virtual void SendSnapshot(void) = 0;
	};


	//# \class	Channel		Represents a unidirectional voice chat channel.
	//
	//# \def	Channel : public GraphEdge<Player, Channel>
	//
	//# \ctor	Channel(Player *sender, Player *receiver);
	//
	//# \param	sender		The player that sends voice chat on the channel.
	//# \param	receiver	The player that receives voice chat on the channel.
	//
	//# \desc
	//# A $Channel$ object represents a connection between two players, a sender and a receiver, through which voice chat
	//# is sent. The connection is unidirectional, so the player specified by the $receiver$ parameter receives voice chat from
	//# the player specified by the $sender$ parameter, but the reverse is not true unless another $Channel$ object exists
	//# corresponding to the opposite direction.
	//
	//# The creation of a $Channel$ object itself establishes the connection in the player chat graph. Nothing needs to be
	//# done with the pointer to the object after the $new$ operator has been used to construct it. For example, the following
	//# single line of code is sufficient for creating a channel leading from player $p1$ to player $p2$.
	//
	//# \source
	//# new Channel(p1, p2);
	//
	//# \desc
	//# A channel object is automatically deleted if the player object at either end is deleted.
	//
	//# \base	Utilities/GraphEdge<Player, Channel>	A $Channel$ object corresponds to an edge in the player chat graph.
	//
	//# \also	$@Player@$


	class Channel : public GraphEdge<Player, Channel>
	{
		private:

			bool		enabledFlag;

		public:

			C4API Channel(Player *sender, Player *receiver);
			C4API ~Channel();

			bool Enabled(void) const
			{
				return (enabledFlag);
			}

			void Enable(void)
			{
				enabledFlag = true;
			}

			void Disable(void)
			{
				enabledFlag = false;
			}
	};


	//# \class	Player		Represents a machine connected to a multiplayer game.
	//
	//# \def	class Player : public MapElement<Player>, public GraphElement<Player, Channel>, public LinkTarget<Player>, public Completable<Player>
	//
	//# \ctor	explicit Player(PlayerKey key);
	//
	//# \param	key		The unique player ID assigned to the player. The server always uses the special key $kPlayerServer$.
	//
	//# \desc
	//# $Player$ objects represent machines in a multiplayer game.
	//
	//# \note
	//# It is not necessary to explicitly construct $Player$ objects. The Message Manager creates new
	//# player objects automatically for itself and new clients as they join a game.
	//
	//# \desc
	//# The Message Manager maintains a list of players that can be accessed by calling the
	//# $@MessageMgr::GetFirstPlayer@$ function or $@MessageMgr::GetPlayer@$ function.
	//#
	//# Messages can be sent to a player by calling the $@Player::SendMessage@$ function. It is also
	//# possible to send messages by calling the $@MessageMgr::SendMessage@$ function, specifying the
	//# player key corresponding to the intended recipient.
	//
	//# \base	Utilities/MapElement<Player>				All $Player$ objects area stored in a map maintained by the Message Manager.
	//# \base	Utilities/GraphElement<Player, Channel>		$Player$ objects are stored in a graph structure that determines how voice chat messages are distributed.
	//# \base	Utilities/LinkTarget<Player>				Used internally by the Message Manager.
	//# \base	Utilities/Completable<Player>				Used for file transfers. The completion procedure is called when a file transfer completes or results in an error.
	//
	//# \also	$@MessageMgr@$
	//# \also	$@MessageMgr::SetPlayerCreator@$


	//# \function	Player::GetPlayerKey		Returns the key associated with a player.
	//
	//# \proto	PlayerKey GetPlayerKey(void) const;
	//
	//# \desc
	//# The $GetPlayerKey$ function returns the unique ID associated with a player. The player representing the
	//# server always has the special key $kPlayerServer$.
	//
	//# \also	$@MessageMgr::GetPlayer@$


	//# \function	Player::GetPlayerName		Returns the name of a player.
	//
	//# \proto	const char *GetPlayerName(void) const;
	//
	//# \desc
	//# The $GetPlayerName$ function returns a pointer to the name of a player.
	//
	//# \also	$@Player::SetPlayerName@$


	//# \function	Player::SetPlayerName		Sets the name of a player.
	//
	//# \proto	void SetPlayerName(const char *name);
	//
	//# \param	name	A pointer to the name of the player.
	//
	//# \desc
	//# The $SetPlayerName$ function sets the name of the player to the string specified by the $name$ parameter.
	//# The length of the text string is truncated to the maximum player name length defined by the constant
	//# $kMaxPlayerNameLength$.
	//
	//# \also	$@Player::GetPlayerName@$


	//# \function	Player::GetNetworkPing		Returns the most recent ping between the local machine and the machine associated with a player.
	//
	//# \proto	int32 GetNetworkPing(void) const;
	//
	//# \desc
	//# The Network Manager records a ping value each time a reliable message is sent through a network
	//# connection. Calling $GetNetworkPing$ returns the most recent ping value for a player. If messages are
	//# sent at int32 intervals, it may be desirable to send periodic messages of type $@PingMessage@$ in
	//# order to maintain an up-to-date ping value.
	//#
	//# Since client machines do not send messages directly to other clients, ping values are not available
	//# for these players and $GetNetworkPing$ will return $-1$. Client machines can only retrieve the ping
	//# value between themselves and the server.
	//
	//# \also	$@PingMessage@$


	//# \div
	//# \function	Player::SendMessage		Sends a message to a specific player in a multiplayer game.
	//
	//# \proto	void SendMessage(const Message& message);
	//
	//# \param	message		The message to be sent.
	//
	//# \desc
	//# The $SendMessage$ function sends the message given by the $message$ parameter to the machine
	//# represented by the $Player$ object. Messages may be sent only to remote machines for which an
	//# actual connection with the local machine exists. Since client machines are normally connected only
	//# to the server, messages may not be sent directly from client to client. A call to the $SendMessage$
	//# function has no effect if there is no connection corresponding to the player.
	//
	//# \also	$@MessageMgr::SendMessage@$
	//# \also	$@MessageMgr::SendMessageAll@$
	//# \also	$@Message@$


	//# \function	Player::RequestFile		Attempts to initiate a file transfer from a player.
	//
	//# \proto	void RequestFile(const char *name);
	//
	//# \param	name	A pointer to the name of the file to be transferred.
	//
	//# \desc
	//# A call to $RequestFile$ causes a message to be sent to a player to request the file whose
	//# name is given by the $name$ parameter. The $RequestFile$ function returns immediately and the
	//# Message Manager listens for a response to the request.
	//#
	//# If the request is accepted, the file transfer is initiated and the player will call its
	//# completion procedure when the transfer is complete. If the request is denied for some reason
	//# (for instance, the file does not exist), then an error message is registered and the player's
	//# completion procedure is invoked.
	//#
	//# The $@Utilities/Completable@$ base class of the $Player$ class is used to signal the completion
	//# of a file transfer or that an error arose during a file transfer. The completion procedure has the prototype
	//
	//# \code	void FileTransferComplete(Player *player, void *cookie);
	//
	//# where $player$ is the player from whom the file was requested, and $cookie$ is the user-defined
	//# pointer passed to the $@Utilities/Completable::SetCompletionProc@$ function. The implementation of
	//# this completion procedure should first call $@Player::GetFileTransferResult@$ to determine
	//# whether the file was successfully transferred or whether an error occurred. This function returns
	//# one of the following file transfer result codes.
	//
	//# \table	FileTransferResult
	//
	//# \also	$@Player::GetFileTransferResult@$
	//# \also	$@Player::SendingFile@$
	//# \also	$@Player::ReceivingFile@$
	//# \also	$@Player::GetReceiveFileSize@$
	//# \also	$@Player::GetReceiveFilePosition@$
	//# \also	$@Utilities/Completable@$


	//# \function	Player::SendingFile		Returns a boolean value indicating whether a file is currently being sent to a player.
	//
	//# \proto	bool SendingFile(void) const;
	//
	//# \desc
	//# The $SendingFile$ function returns $true$ if a file is currently being transferred from the local machine to
	//# the machine associated with the $Player$ object. If no such file transfer is in progress,
	//# $SendingFile$ returns $false$.
	//
	//# \also	$@Player::ReceivingFile@$
	//# \also	$@Player::RequestFile@$


	//# \function	Player::ReceivingFile	Returns a boolean value indicating whether a file is currently being received from a player.
	//
	//# \proto	bool ReceivingFile(void) const;
	//
	//# \desc
	//# The $ReceivingFile$ function returns $true$ if a file is currently being transferred from the machine
	//# associated with the $Player$ object to the local machine. If no such file transfer is in
	//# progress, $ReceivingFile$ returns false.
	//#
	//# A file transfer from a remote machine to the local machine is initiated by a call to the
	//# $@Player::RequestFile@$ function.
	//
	//# \also	$@Player::SendingFile@$
	//# \also	$@Player::RequestFile@$


	//# \function	Player::GetFileChunkSize		Returns the maximum size of each packet sent during a file transfer.
	//
	//# \proto	unsigned_int32 GetFileChunkSize(void) const;
	//
	//# \desc
	//# During a file transfer, several data packets, or "chunks", are sent to the receiving machine at
	//# one time. Each chunk is the same size (unless it's the last chunk to be sent). Calling
	//# $GetFileChunkSize$ retrieves the current size of each chunk. This size can be changed by calling
	//# the $@Player::SetFileChunkParams@$ function.
	//
	//# \also	$@Player::GetFileChunkCount@$
	//# \also	$@Player::SetFileChunkParams@$
	//# \also	$@Player::RequestFile@$


	//# \function	Player::GetFileChunkCount	Returns the number of packets sent at one time during a file transfer.
	//
	//# \proto	int32 GetFileChunkCount(void) const;
	//
	//# \desc
	//# During a file transfer, several data packets, or "chunks", are sent to the receiving machine at
	//# one time. Once the receiving machine has acknowledged all of the outstanding packets, the same
	//# number of packets is sent again, and the process repeats until the entire file has been transmitted.
	//# Calling $GetFileChunkCount$ retrieves the current number of data packets that are sent at one time.
	//# This number can be raised or lowered by calling the $@Player::SetFileChunkParams@$ function.
	//
	//# \also	$@Player::GetFileChunkSize@$
	//# \also	$@Player::SetFileChunkParams@$
	//# \also	$@Player::RequestFile@$


	//# \function	Player::SetFileChunkParams		Returns the number of packets sent at one time during a file transfer.
	//
	//# \proto	void SetFileChunkParams(unsigned_int32 size, int32 count);
	//
	//# \param	size	The maximum size of each packet. This value is clamped to the constant $kMaxFileChunkSize$.
	//# \param	count	The number of packets sent at one time.
	//
	//# \desc
	//# During a file transfer, several data packets, or "chunks", are sent to the receiving machine at
	//# one time. Each chunk is the same size (unless it's the last chunk to be sent). Once the receiving
	//# machine has acknowledged all of the outstanding packets, the same number of packets is sent again,
	//# and the process repeats until the entire file has been transmitted.
	//#
	//# The $SetFileChunkParams$ function sets the size of the data packets and the number of data packets
	//# that are sent at one time. Noisey connections may benefit from a smaller chunk size because this may
	//# reduce packet loss. The chunk count should be determined by the available bandwidth. Low bandwidth
	//# connections should use a low chunk count to avoid adversely affecting other communications.
	//
	//# \also	$@Player::GetFileChunkSize@$
	//# \also	$@Player::GetFileChunkCount@$
	//# \also	$@Player::RequestFile@$


	//# \function	Player::GetReceiveFileSize		Returns the size of the file being received from a player.
	//
	//# \proto	unsigned_int64 GetReceiveFileSize(void) const;
	//
	//# \desc
	//# If a file is currently being received from a player, then the total size of this file can be
	//# retrieved by calling the $GetReceiveFileSize$ function. If no file is currently being received,
	//# then the value returned by $GetReceiveFileSize$ is zero.
	//#
	//# The $@Player::ReceivingFile@$ function returns a boolean value indicating whether a file is
	//# currently being received.
	//
	//# \also	$@Player::GetReceiveFilePosition@$
	//# \also	$@Player::ReceivingFile@$
	//# \also	$@Player::RequestFile@$


	//# \function	Player::GetReceiveFilePosition		Returns the current position in the file being received from a player.
	//
	//# \proto	unsigned_int64 GetReceiveFilePosition(void) const;
	//
	//# \desc
	//# If a file is currently being received from a player, then the current write position of this
	//# file can be retrieved by calling the $GetReceiveFilePosition$ function. This function can be used in
	//# conjunction with the $@Player::GetReceiveFileSize@$ function to determine what percentage of a file
	//# has been downloaded. If no file is currently being received, then the value returned by
	//# $GetReceiveFilePosition$ is zero.
	//#
	//# The $@Player::ReceivingFile@$ function returns a boolean value indicating whether a file is
	//# currently being received.
	//
	//# \also	$@Player::GetReceiveFileSize@$
	//# \also	$@Player::ReceivingFile@$
	//# \also	$@Player::RequestFile@$


	//# \function	Player::GetFileTransferResult		Returns the result of the most recent file transfer from a player.
	//
	//# \proto	FileTransferResult GetFileTransferResult(void) const;
	//
	//# \desc
	//# The $GetFileTransferResult$ function is normally called from within a player's completion
	//# procedure to determine whether a file transfer completed successfully. This function can return
	//# one of the following constants
	//
	//# \table	FileTransferResult
	//
	//# A file transfer is initiated by calling $@Player::RequestFile@$. When a file transfer is in
	//# progress, the $GetFileTransferResult$ function always returns $kTransferPending$. Otherwise,
	//# $GetFileTransferResult$ returns the file transfer result code corresponding to the most recent
	//# file transfer. If no file transfer has ever taken place, then the return value is undefined.
	//
	//# \also	$@Player::RequestFile@$
	//# \also	$@Player::ReceivingFile@$


	//# \div
	//# \function	Player::GetChatSoundGroup		Returns the sound group to which a player's voice chat belongs.
	//
	//# \proto	SoundGroup *GetChatSoundGroup(void) const;
	//
	//# \desc
	//# The $GetChatSoundGroup$ function returns the sound group to which a player's voice chat belongs.
	//# If a player does not belong to any sound group, which is the initial state, then the return value is $nullptr$.
	//
	//# \also	$@Player::SetChatSoundGroup@$
	//# \also	$@Player::GetChatVolume@$
	//# \also	$@Player::SetChatVolume@$
	//# \also	$@SoundMgr/SoundGroup@$


	//# \function	Player::SetChatSoundGroup		Sets the sound group to which a player's voice chat belongs.
	//
	//# \proto	virtual void SetChatSoundGroup(SoundGroup *group);
	//
	//# \param	group	The sound group to which the voice chat belongs. This can be $nullptr$.
	//
	//# \desc
	//# The $SetChatSoundGroup$ function should be called shortly after a $Player$ object is constructed
	//# in order to set the sound group to which the player's voice chat belongs. The player's voice chat
	//# is affected by the volume of this sound group.
	//
	//# \also	$@Player::SetChatVolume@$
	//# \also	$@SoundMgr/SoundGroup@$


	//# \function	Player::GetChatVolume		Returns the volume for a particular player's voice chat.
	//
	//# \proto	float GetChatVolume(void) const;
	//
	//# \desc
	//# The $GetChatVolume$ function returns the volume for the voice chat from a particular player.
	//
	//# \also	$@Player::SetChatVolume@$
	//# \also	$@Player::GetChatSoundGroup@$
	//# \also	$@Player::SetChatSoundGroup@$


	//# \function	Player::SetChatVolume		Sets the volume for a particular player's voice chat.
	//
	//# \proto	virtual void SetChatVolume(float volume);
	//
	//# \param	volume		The volume for the voice chat, in the range [0.0, 1.0].
	//
	//# \desc
	//# The $SetChatVolume$ function sets the volume for the voice chat from a particular player.
	//
	//# \also	$@Player::GetChatVolume@$
	//# \also	$@Player::GetChatSoundGroup@$
	//# \also	$@Player::SetChatSoundGroup@$


	class Player : public MapElement<Player>, public GraphElement<Player, Channel>, public LinkTarget<Player>, public Completable<Player>
	{
		friend class MessageMgr;

		private:

			PlayerKey						playerKey;
			NetworkAddress					playerAddress;

			List<CombinedMessage>			messageList;
			bool							sendEnabled;

			Sound							*chatSound;
			SoundGroup						*chatSoundGroup;
			ChatStreamer					*chatStreamer;
			float							chatVolume;

			String<kMaxPlayerNameLength>	playerName;

			unsigned_int64					sendFileSize;
			unsigned_int64					receiveFileSize;
			unsigned_int32					fileChunkSize;
			int32							fileChunkCount;

			FileTransferResult				receiveFileResult;
			String<kMaxFileNameLength>		receiveFileName;

			File							sendFile;
			File							receiveFile;

			void SetChatStreamer(ChatStreamer *streamer)
			{
				chatStreamer = streamer;
			}

			void AddMessage(const Message& message, unsigned_int32 size, const char *data);

		public:

			typedef PlayerKey KeyType;

			C4API explicit Player(PlayerKey key);
			C4API ~Player();

			using MapElement<Player>::Previous;
			using MapElement<Player>::Next;

			KeyType GetKey(void) const
			{
				return (playerKey);
			}

			PlayerKey GetPlayerKey(void) const
			{
				return (playerKey);
			}

			const NetworkAddress& GetPlayerAddress(void) const
			{
				return (playerAddress);
			}

			void SetPlayerAddress(const NetworkAddress& address)
			{
				playerAddress = address;
			}

			SoundGroup *GetChatSoundGroup(void) const
			{
				return (chatSoundGroup);
			}

			ChatStreamer *GetChatStreamer(void) const
			{
				return (chatStreamer);
			}

			float GetChatVolume(void) const
			{
				return (chatVolume);
			}

			const char *GetPlayerName(void) const
			{
				return (playerName);
			}

			void SetPlayerName(const char *name)
			{
				playerName = name;
			}

			int32 GetNetworkPing(void) const
			{
				return (TheNetworkMgr->GetPing(playerAddress));
			}

			bool SendingFile(void) const
			{
				return (sendFile.Open());
			}

			bool ReceivingFile(void) const
			{
				return (receiveFile.Open());
			}

			unsigned_int32 GetFileChunkSize(void) const
			{
				return (fileChunkSize);
			}

			int32 GetFileChunkCount(void) const
			{
				return (fileChunkCount);
			}

			void SetFileChunkParams(unsigned_int32 size, int32 count)
			{
				fileChunkSize = Min(size, kMaxFileChunkSize);
				fileChunkCount = count;
			}

			unsigned_int64 GetReceiveFileSize(void) const
			{
				return (receiveFile.Open() ? receiveFileSize : 0);
			}

			unsigned_int64 GetReceiveFilePosition(void) const
			{
				return (receiveFile.Open() ? receiveFile.GetPosition() : 0);
			}

			FileTransferResult GetFileTransferResult(void) const
			{
				return (receiveFileResult);
			}

			void StartSendingFile(const char *name);
			void StopSendingFile();
			void StartReceivingFile(const char *name, unsigned_int64 size);
			void StopReceivingFile(FileTransferResult result);
			void SendFileChunk(void);
			void ReceiveFileChunk(const FileChunkMessage *fcm);

			C4API void SendMessage(const Message& message);
			C4API void RequestFile(const char *name);

			C4API virtual ChatStreamer *CreateChatStreamer(void);
			C4API virtual void SetChatSoundGroup(SoundGroup *group);
			C4API virtual void SetChatVolume(float volume);
	};


	//# \class	MessageMgr		The Message Manager class.
	//
	//# \def	class MessageMgr : public Manager<MessageMgr>
	//
	//# \desc
	//# The $MessageMgr$ class encapsulates the high-level networking and multiplayer features of the C4 Engine.
	//# Low-level networking functionality is exposed by the $@NetworkMgr/NetworkMgr@$ class.
	//# The single instance of the Message Manager is constructed during an application's initialization
	//# and destroyed at termination.
	//#
	//# The Message Manager's member functions are accessed through the global pointer $TheMessageMgr$.
	//
	//# \also	$@Message@$
	//# \also	$@Player@$


	//# \function	MessageMgr::Server		Returns a boolean value indicating whether the local machine is the server.
	//
	//# \proto	bool Server(void) const;
	//
	//# \desc
	//# The $Server$ function returns $true$ if the Message Manager has been configured as the server in
	//# a multiplayer game or if the Message Manager is in single player mode.
	//#
	//# Upon initial construction, the Message Manager is in single player mode. The Message Manager can
	//# be configured as the server for a multiplayer game by calling $@MessageMgr::BeginMultiplayerGame@$ with
	//# the $host$ parameter set to $true$.
	//#
	//# The only case in which the $Server$ function returns $false$ is when the Message Manager has been
	//# configured as a client in a multiplayer game. This is done by calling $@MessageMgr::BeginMultiplayerGame@$
	//# with the $host$ parameter set to $false$.
	//
	//# \also	$@MessageMgr::BeginSinglePlayerGame@$
	//# \also	$@MessageMgr::BeginMultiplayerGame@$
	//# \also	$@MessageMgr::Multiplayer@$


	//# \function	MessageMgr::Multiplayer		Returns a boolean value indicating whether the Message Manager is configured for a multiplayer game.
	//
	//# \proto	bool Multiplayer(void) const;
	//
	//# \desc
	//# The $Multiplayer$ function returns $true$ if the Message Manager has been configured for a
	//# multiplayer game. If the Message Manager is in single player mode, $Multiplayer$ returns $false$.
	//
	//# \also	$@MessageMgr::BeginSinglePlayerGame@$
	//# \also	$@MessageMgr::BeginMultiplayerGame@$
	//# \also	$@MessageMgr::Server@$


	//# \function	MessageMgr::Synchronized	Returns a boolean value indicating whether a new client machine is fully synchronized with the server.
	//
	//# \proto	bool Synchronized(void) const;
	//
	//# \desc
	//# When a client machine connects to a server, the server sends it information about the game currently being
	//# played. Once all of the information has been sent, the server sends a special synchronization message to
	//# signal that the client may begin playing the game. In the time between joining a game and receiving the
	//# synchronization message from the server, the $Synchronized$ function returns $false$ when called on the
	//# client machine. After all of the game state has been received from the server, the $Synchronized$ function
	//# returns $true$.
	//
	//# \also	$@MessageMgr::BeginSinglePlayerGame@$
	//# \also	$@MessageMgr::BeginMultiplayerGame@$
	//# \also	$@MessageMgr::Server@$


	//# \function	MessageMgr::GetPlayerCount		Returns the current number of players connected to a multiplayer game.
	//
	//# \proto	int32 GetPlayerCount(void) const;
	//
	//# \desc
	//# The count returned by the $GetPlayerCount$ function includes the server, the local machine, and all other
	//# clients connected to a multiplayer game.
	//#
	//# The maximum number of players allowed in a game can be set with the
	//# $@MessageMgr::SetMaxPlayerCount@$ function.
	//
	//# \also	$@MessageMgr::GetMaxPlayerCount@$
	//# \also	$@MessageMgr::SetMaxPlayerCount@$


	//# \function	MessageMgr::GetMaxPlayerCount		Returns the maximum number of players that the server will allow in a multiplayer game.
	//
	//# \proto	int32 GetMaxPlayerCount(void) const;
	//
	//# \desc
	//# The count returned by the $GetMaxPlayerCount$ function represents the maximum number of players that will be
	//# allowed to enter a game that is hosted by the local machine. This count includes the server machine.
	//
	//# \also	$@MessageMgr::SetMaxPlayerCount@$
	//# \also	$@MessageMgr::GetPlayerCount@$


	//# \function	MessageMgr::SetMaxPlayerCount		Sets the maximum number of players that the server will allow in a multiplayer game.
	//
	//# \proto	void SetMaxPlayerCount(int32 count);
	//
	//# \param	count	The maximum number of players.
	//
	//# \desc
	//# The $SetMaxPlayerCount$ function should be called on the machine hosting a multiplayer game to set
	//# the maximum number of players allowable. The $SetMaxPlayerCount$ function should be called prior to calling the
	//# $@MessageMgr::BeginMultiplayerGame@$ function since it is this function that tells the Network Manager how
	//# many connections may be accepted.
	//
	//# \also	$@MessageMgr::GetMaxPlayerCount@$
	//# \also	$@MessageMgr::GetPlayerCount@$


	//# \div
	//# \function	MessageMgr::BeginSinglePlayerGame		Configures the Message Manager for a single player game.
	//
	//# \proto	void BeginSinglePlayerGame(void);
	//
	//# \desc
	//# Calling the $BeginSinglePlayerGame$ function places the Message Manager in single player mode.
	//#
	//# After calling $BeginSinglePlayerGame$, subsequent calls to $@MessageMgr::Multiplayer@$ return $false$,
	//# and subsequent calls to $@MessageMgr::Server@$ return $true$.
	//#
	//# The $BeginSinglePlayerGame$ function calls the Network Manager function $@NetworkMgr/NetworkMgr::Terminate@$
	//# to disable network communications. This has the effect of terminating any existing connections in a
	//# disorderly fashion, so any previous multiplayer game should be stopped with the $@MessageMgr::EndGame@$
	//# function first.
	//
	//# \also	$@MessageMgr::BeginMultiplayerGame@$
	//# \also	$@MessageMgr::EndGame@$
	//# \also	$@MessageMgr::Multiplayer@$
	//# \also	$@MessageMgr::Server@$
	//# \also	$@NetworkMgr/NetworkMgr::Terminate@$


	//# \function	MessageMgr::BeginMultiplayerGame		Configures the Message Manager for a multiplayer game.
	//
	//# \proto	NetworkResult BeginMultiplayerGame(bool host = true);
	//
	//# \param	host	Indicates whether the local machine is the server.
	//
	//# \desc
	//# Calling the $BeginMultiplayerGame$ function places the Message Manager in multiplayer mode and returns
	//# one of the following network result codes.
	//
	//# \value	kEngineOkay 		The Network Manager was successfully initialized and the Message Manager
	//#								has been placed in multiplayer mode.
	//# \value	kNetworkInitFailed 	The Network Manager could not be initialized because the operating
	//#								system returned an error. If this result code is returned, then the
	//#								Message Manager remains in single player mode.
	//
	//# \desc
	//# If the $host$ parameter is set to $true$, then the local machine is configured as the server.
	//# In this case, the $BeginMultiplayerGame$ function calls the Network Manager function
	//# $@NetworkMgr/NetworkMgr::SetMaxConnectionCount@$ to set the maximum number of connections that will be
	//# accepted by client machines. The number of connections allowed is one less than the current maximum
	//# player count since the server machine counts as the first player. The maximum player count should
	//# be set prior to calling $BeginMultiplayerGame$ by calling the $@MessageMgr::SetMaxPlayerCount@$ function.
	//#
	//# If the $host$ parameter is set to $false$, then the local machine is configured as a client.
	//# In this case, the $BeginMultiplayerGame$ function calls the Network Manager function
	//# $@NetworkMgr/NetworkMgr::SetMaxConnectionCount@$ with a maximum connection count of zero to prevent other
	//# machines from connecting to the local machine. The local machine is still able to make outgoing connections.
	//#
	//# After a successful call to $BeginMultiplayerGame$, subsequent calls to $@MessageMgr::Multiplayer@$
	//# return $true$, and subsequent calls to $@MessageMgr::Server@$ return the value given by the $host$
	//# parameter. If $BeginMultiplayerGame$ returns the result code $kNetworkInitFailed$, then the Message
	//# Manager is placed in single player mode.
	//#
	//# On a machine configured as a multiplayer server, the Message Manager calls the
	//# $@System/Application::HandleConnectionEvent@$ function with the $kConnectionQueryReceived$ event whenever
	//# it receives a broadcasted query asking servers to identify themselves.
	//
	//# \special
	//# A call to the $@MessageMgr::Connect@$ function while the Message Manager is in single player mode
	//# will cause an implicit call to $BeginMultiplayerGame$ to be made in an attempt to configure the local
	//# machine as a multiplayer client. If the Message Manager is already in multiplayer mode (as a client
	//# or as a server) when the $@MessageMgr::Connect@$ function is called, then the $BeginMultiplayerGame$
	//# function is not implicitly called.
	//#
	//# Before calling $BeginMultiplayerGame$, the Network Manager function $@NetworkMgr/NetworkMgr::SetPortNumber@$
	//# should be called to indicate what port is to be used for communications. For a server, the port
	//# number should be one on which clients expect the server to be listening. For a client, the port
	//# number may be zero, in which case the underlying TCP/IP implementation will choose an available
	//# port number automatically.
	//#
	//# The Network Manager function $@NetworkMgr/NetworkMgr::SetBroadcastPortNumber@$ should also be called to set
	//# the port on which broadcasts will be sent and received. For a server, the broadcast port number
	//# should be the same as the normal port number. For a client, the broadcast port number should
	//# specify the port on which it expects servers to be listening.
	//
	//# \also	$@MessageMgr::BeginSinglePlayerGame@$
	//# \also	$@MessageMgr::EndGame@$
	//# \also	$@MessageMgr::Connect@$
	//# \also	$@MessageMgr::Server@$
	//# \also	$@MessageMgr::Multiplayer@$
	//# \also	$@MessageMgr::SetMaxPlayerCount@$
	//# \also	$@NetworkMgr/NetworkMgr::SetMaxConnectionCount@$
	//# \also	$@NetworkMgr/NetworkMgr::SetPortNumber@$
	//# \also	$@NetworkMgr/NetworkMgr::SetBroadcastPortNumber@$
	//# \also	$@System/Application::HandleConnectionEvent@$


	//# \function	MessageMgr::EndGame		Tells the Message Manager that a game has ended so it can clean up.
	//
	//# \proto	void EndGame(void);
	//
	//# \desc
	//# Calling the $EndGame$ function tells the Message Manager that the current game has ended so it can clean up.
	//# This function should be called when a game ends to close network connections and release Message Manager resources.
	//# All $@Player@$ objects are destroyed when $EndGame$ is called.
	//#
	//# After calling $EndGame$, subsequent calls to $@MessageMgr::Multiplayer@$ return $false$,
	//# and subsequent calls to $@MessageMgr::Server@$ return $true$.
	//
	//# \also	$@MessageMgr::BeginSinglePlayerGame@$
	//# \also	$@MessageMgr::BeginMultiplayerGame@$
	//# \also	$@MessageMgr::Multiplayer@$
	//# \also	$@MessageMgr::Server@$


	//# \function	MessageMgr::Connect		Establishes a connection with a server.
	//
	//# \proto	NetworkResult Connect(const NetworkAddress& address);
	//
	//# \param	address		The address of the server.
	//
	//# \desc
	//# Calling the $Connect$ function causes a connection request to be sent to the server at the network address
	//# given by the $address$ parameter. If the Message Manager is in single player mode, the $Connect$
	//# function attempts to configure the local machine as a multiplayer client by calling the
	//# $@MessageMgr::BeginMultiplayerGame@$ function with the $host$ parameter set to $false$.
	//# The $Connect$ function returns one of the following network result codes.
	//
	//# \value	kEngineOkay 		The Network Manager was successfully initialized and the connection request was submitted.
	//# \value	kNetworkInitFailed 	The Network Manager could not be initialized because the operating
	//#								system returned an error.
	//
	//# \desc
	//# If successful, the $Connect$ function returns immediately and the Network Manager listens for a
	//# response. The number of attempts to make a connection and the time interval between attempts are
	//# controlled by the reliable resend count and reliable resend time settings maintained by the
	//# Network Manager.
	//#
	//# If the connection is accepted by the server, a new $@Player@$ object is created to represent
	//# the server and this player is added to the Message Manager's player map. The Message Manager
	//# then calls the $@System/Application::HandleConnectionEvent@$ function with the $kConnectionServerAccepted$
	//# event to notify the application module that the connection has been established. The Message Manager will
	//# then make a call to the $@System/Application::HandlePlayerEvent@$ function with the $kPlayerConnected$ event
	//# once for each player already connected to the server to inform the application module about any
	//# additional players in the game. The Message Manager on the server machine and every other client
	//# machine connected to the game also calls $@System/Application::HandlePlayerEvent@$ one time to inform
	//# the application modules running on those machines that a new player has arrived.
	//#
	//# After the server accepts a connection and sends events about the players in a game, the Message Manager
	//# on the server calls the $@System/Application::HandlePlayerEvent@$ function with the $kPlayerInitialized$ event
	//# to inform the application module to optionally send any kind of game information messages needed before the
	//# current game state is sent. Then, the server sends the message journal to the new client.
	//# Finally, the server allows all $@StateSender@$ objects to send their game state.
	//#
	//# If the connection attempt fails or the connection request was denied by the server, the Message
	//# Manager calls the $@System/Application::HandleConnectionEvent@$ function with the $kConnectionAttemptFailed$
	//# event to notify the application module that the connection could not be established. The reason for the
	//# connection failure passed to this function is one of the following constants.
	//
	//# \table	NetworkFail
	//
	//# \also	$@MessageMgr::Disconnect@$
	//# \also	$@MessageMgr::DisconnectAll@$
	//# \also	$@MessageMgr::BeginMultiplayerGame@$
	//# \also	$@NetworkMgr/NetworkMgr::SetProtocol@$
	//# \also	$@System/Application::HandleConnectionEvent@$
	//# \also	$@System/Application::HandlePlayerEvent@$
	//# \also	$@NetworkMgr/NetworkAddress@$


	//# \function	MessageMgr::Disconnect		Terminates a connection with a server.
	//
	//# \proto	void Disconnect(Player *player);
	//
	//# \param	player		A pointer to a player from which to disconnect.
	//
	//# \desc
	//# When the $Disconnect$ function is called, the Message Manager terminates the connection with the
	//# machine associated with the $player$ parameter. On client machines, if this player does not represent
	//# the server, then the $Disconnect$ function has no effect.
	//#
	//# When a client disconnects from the server, the $@System/Application::HandleConnectionEvent@$ function is called
	//# with the $kConnectionClientClosed$ event to notify the application module that the connection to the server
	//# has been terminated. The Message Manager on the server and on each remaining client in the game calls the
	//# $@System/Application::HandlePlayerEvent@$ function with the $kPlayerDisconnected$ event to inform the
	//# application modules on those machines that a player has departed.
	//#
	//# When a server machine disconnects a client, the $@System/Application::HandleConnectionEvent@$ function is
	//# called with the $kConnectionServerClosed$ event on the client machine to inform the application module that
	//# is has been disconnected by the server.
	//
	//# \also	$@MessageMgr::Connect@$
	//# \also	$@MessageMgr::DisconnectAll@$
	//# \also	$@System/Application::HandleConnectionEvent@$
	//# \also	$@System/Application::HandlePlayerEvent@$


	//# \function	MessageMgr::DisconnectAll		Terminates all current connections.
	//
	//# \proto	void DisconnectAll(void);
	//
	//# \desc
	//# On a server machine, calling the $DisconnectAll$ function disconnects all current clients. When a
	//# server machine disconnects a client, the $@System/Application::HandleConnectionEvent@$ function is
	//# called with the $kConnectionServerClosed$ event on the client machine to inform the Application
	//# Module that is has been disconnected by the server.
	//#
	//# On client machines, calling the $DisconnectAll$ function disconnects from the server machine.
	//# This is equivalent to calling $@MessageMgr::Disconnect@$ with the player associated with the
	//# server machine. In this case, the $@System/Application::HandleConnectionEvent@$ function is called
	//# with the $kConnectionClientClosed$ event to notify the application module that the connection to the
	//# server has been terminated. The Message Manager on the server and on each remaining client in the game calls the
	//# $@System/Application::HandlePlayerEvent@$ function with the $kPlayerDisconnected$ event to inform the
	//# application modules on those machines that a player has departed.
	//#
	//# If there are no current connections, then calling $DisconnectAll$ has no effect.
	//
	//# \also	$@MessageMgr::Connect@$
	//# \also	$@MessageMgr::Disconnect@$
	//# \also	$@System/Application::HandleConnectionEvent@$
	//# \also	$@System/Application::HandlePlayerEvent@$


	//# \function	MessageMgr::SetPlayerCreator	Sets the creator function for Player objects.
	//
	//# \proto	void SetPlayerCreator(PlayerCreateProc *proc, void *cookie = nullptr);
	//
	//# \param	proc		A pointer to the player creator function.
	//# \param	cookie		The cookie that is passed to the player creator function.
	//
	//# \desc
	//# When the Message Manager needs to create a new $@Player@$ object, it first checks to see if a
	//# player creator function has been installed. If so, then it calls the player creator function,
	//# allowing the application to create an object of a type that is a subclass of the $Player$ class.
	//# Initially, there is no player creator function, so the Message Manager just creates an object
	//# whose type is the $Player$ base class.
	//#
	//# The $SetPlayerCreator$ function should be called by an application's startup code when it wants to
	//# use a subclass of the $Player$ class to represent players in a game. The $proc$ parameter should point
	//# to a function with the following signature.
	//
	//# \code	typedef Player *PlayerCreateProc(PlayerKey, void *);
	//
	//# The player key that is passed in the first parameter should be passed to the constructor for the $Player$
	//# base class. The second parameter is set to the value specified by the $cookie$ parameter of the
	//# $SetPlayerCreator$ function. Ordinarily, this callback should simply construct a new object having type
	//# that is a subclass of $Player$ and return it.
	//#
	//# The current player creator can be uninstalled by calling the $SetPlayerCreator$ function with a value
	//# of $nullptr$ for the $proc$ parameter.
	//
	//# \also	$@Player@$


	//# \function	MessageMgr::GetFirstPlayer		Returns the first player in the map of players maintained by the Message Manager.
	//
	//# \proto	Player *GetFirstPlayer(void) const;
	//
	//# \desc
	//# The map of players represents the machines connected to a multiplayer game. Calling the
	//# $GetFirstPlayer$ function retrieves a pointer to the first player in the map maintained by the
	//# Message Manager. Once the first player has been obtained, one may iterate through the entire
	//# map of players by using the $@Utilities/MapElement::Next@$ function.
	//
	//# \also	$@MessageMgr::GetLocalPlayer@$
	//# \also	$@MessageMgr::GetPlayer@$
	//# \also	$@Player@$


	//# \function	MessageMgr::GetLocalPlayer		Returns the player representing the local machine.
	//
	//# \proto	Player *GetLocalPlayer(void);
	//
	//# \desc
	//# The $GetLocalPlayer$ function returns the $@Player@$ object associated with the local machine.
	//
	//# \also	$@MessageMgr::GetFirstPlayer@$
	//# \also	$@MessageMgr::GetPlayer@$
	//# \also	$@Player@$


	//# \function	MessageMgr::GetPlayer		Returns the player having a specific key.
	//
	//# \proto	Player *GetPlayer(PlayerKey key) const;
	//
	//# \param	key		The key for the player to return. If the constant $kPlayerServer$ is specified,
	//#					then the player representing the server machine is returned.
	//
	//# \desc
	//# The $GetPlayer$ function attempts to locate the player whose key matches that given by
	//# the $key$ parameter. If the player exists, then a pointer to it is returned. Otherwise,
	//# $GetPlayer$ returns $nullptr$. The constant $kPlayerServer$ may be specified to retrieve the
	//# player representing the server on any machine in a multiplayer game.
	//#
	//# The entire map of players may be accessed by using the $@MessageMgr::GetFirstPlayer@$ function.
	//
	//# \also	$@MessageMgr::GetFirstPlayer@$
	//# \also	$@MessageMgr::GetLocalPlayer@$
	//# \also	$@Player@$


	//# \function	MessageMgr::GetSnapshotInterval		Returns the time in between snapshots.
	//
	//# \proto	int32 GetSnapshotInterval(void) const;
	//
	//# \desc
	//# The $GetSnapshotInterval$ function returns the time, in milliseconds, that the Message Manager
	//# waits after sending a state snapshot before the next snapshot is sent.
	//
	//# \also	$@MessageMgr::GetSnapshotFrequency@$
	//# \also	$@MessageMgr::SetSnapshotInterval@$
	//# \also	$@SnapshotSender@$


	//# \function	MessageMgr::GetSnapshotFrequency	Returns the snapshot frequency.
	//
	//# \proto	float GetSnapshotFrequency(void) const;
	//
	//# \desc
	//# The $GetSnapshotFrequency$ function returns the snapshot frequency, in kilohertz. This value
	//# is simply the reciprocal of the snapshot time interval specified in milliseconds.
	//
	//# \also	$@MessageMgr::GetSnapshotInterval@$
	//# \also	$@MessageMgr::SetSnapshotInterval@$
	//# \also	$@SnapshotSender@$


	//# \function	MessageMgr::SetSnapshotInterval		Sets the time in between snapshots.
	//
	//# \proto	void SetSnapshotInterval(int32 time) const;
	//
	//# \param	time	The time between snapshots, in milliseconds.
	//
	//# \desc
	//# The $SetSnapshotInterval$ function sets the time, in millseconds, that the Message Manager
	//# waits after sending a state snapshot before the next snapshot is sent. The default snapshot
	//# interval is 50 millseconds, corresponding to 20 snapshots per second.
	//
	//# \also	$@MessageMgr::GetSnapshotInterval@$
	//# \also	$@MessageMgr::GetSnapshotFrequency@$
	//# \also	$@SnapshotSender@$


	//# \function	MessageMgr::InstallStateSender		Installs a state-sending function.
	//
	//# \proto	void InstallStateSender(StateSender *sender);
	//
	//# \param	sender		A pointer to a $@StateSender@$ object encapsulating the state-sending function.
	//
	//# \desc
	//# The $InstallStateSender$ function installs a state-sending function that is called whenever a new
	//# client machine joins a multiplayer game. A state-sending function is uninstalled by destroying the
	//# $StateSender$ object.
	//
	//# \also	$@StateSender@$


	//# \function	MessageMgr::AddSnapshotSender		Registers a snapshot-sending object.
	//
	//# \proto	void AddSnapshotSender(SnapshotSender *sender);
	//
	//# \param	sender		A pointer to a $@SnapshotSender@$ object.
	//
	//# \desc
	//# The $AddSnapshotSender$ function registers a snapshot-sending object that is able to send snapshot
	//# messages whenever the Message Manager sends a snapshot to client machines in a multiplayer game.
	//# When the Message Manager sends a snapshot, the $@SnapshotSender::SendSnapshot@$ function of the
	//# object specified by the $sender$ parameter is called.
	//#
	//# A snapshot-sending function is unregistered by either destroying the $SnapshotSender$ object or
	//# calling the $@MessageMgr::RemoveSnapshotSender@$ function.
	//
	//# \also	$@MessageMgr::RemoveSnapshotSender@$
	//# \also	$@SnapshotSender@$


	//# \function	MessageMgr::RemoveSnapshotSender		Registers a snapshot-sending object.
	//
	//# \proto	void RemoveSnapshotSender(SnapshotSender *sender);
	//
	//# \param	sender		A pointer to a $@SnapshotSender@$ object.
	//
	//# \desc
	//# The $RemoveSnapshotSender$ function unregisters a snapshot-sending object. After this function is
	//# called, the snapshot-sending object specified by the $sender$ parameter no longer has its
	//# $@SnapshotSender::SendSnapshot@$ function called when snapshots are sent by the Message Manager.
	//
	//# \also	$@MessageMgr::AddSnapshotSender@$
	//# \also	$@SnapshotSender@$


	//# \function	MessageMgr::SendMessage		Sends a message to a player in a multiplayer game.
	//
	//# \proto	void SendMessage(PlayerKey key, const Message& message);
	//
	//# \param	key			The key of the player that the message will be sent to.
	//# \param	message		The message to be sent.
	//
	//# \desc
	//# The $SendMessage$ function locates the player whose key matches the $key$ parameter and sends
	//# the message given by the $message$ parameter to that player. If no player with the specified key
	//# exists, then $SendMessage$ returns without sending the message.
	//#
	//# The $key$ parameter may be the actual key of a player, or it may be one of the following
	//# special constants.
	//
	//# \value	kPlayerServer 	The message is sent to the server. If the local machine is the server,
	//#							then the message is received immediately and is processed before
	//#							$SendMessage$ returns.
	//# \value	kPlayerSelf 	The message is sent to the local machine. The message is received
	//#							immediately and is processed before $SendMessage$ returns.
	//# \value	kPlayerAll 		The message is sent to all players and to the local machine. Specifying
	//#							this key is equivalent to calling the $@MessageMgr::SendMessageAll@$ function.
	//# \value	kPlayerClients	The message is sent to all client machines. This may only be specified
	//#							if the sending machine is the server, and it is equivalent to calling
	//#							the $@MessageMgr::SendMessageClients@$ function.
	//
	//# \desc
	//# Messages may be sent only to remote machines for which an actual connection with the local machine
	//# exists. Since client machines are normally connected only to the server, messages may not be sent
	//# directly from client to client. A call to the $SendMessage$ function has no effect if there is no
	//# connection corresponding to the recipient.
	//
	//# \special
	//# If a pointer to a $@Player@$ object is available, it is generally better to call
	//# $@Player::SendMessage@$ to send a message to that player instead of calling
	//# $MessageMgr::SendMessage$ with the player's key.
	//
	//# \also	$@MessageMgr::SendMessageAll@$
	//# \also	$@MessageMgr::SendMessageClients@$
	//# \also	$@MessageMgr::SendConnectionlessMessage@$
	//# \also	$@Player::SendMessage@$


	//# \function	MessageMgr::SendMessageAll		Sends a message to all players in a multiplayer game.
	//
	//# \proto	void SendMessageAll(const Message& message, bool self = true);
	//
	//# \param	message		The message to be sent.
	//# \param	self		A boolean value that indicates whether the message should also be received on the local machine.
	//
	//# \desc
	//# The $SendMessageAll$ function sends the message given by the $message$ parameter to every player.
	//# If the $self$ parameter is $true$, then the message is also received on the local machine. In this case,
	//# the message is received and processed on the local machine before $SendMessageAll$ returns.
	//#
	//# When a client machine calls $SendMessageAll$, the message is sent only to the server since that is
	//# the only machine with which a connection exists.
	//
	//# \also	$@MessageMgr::SendMessage@$
	//# \also	$@MessageMgr::SendMessageClients@$
	//# \also	$@MessageMgr::SendMessageJournal@$


	//# \function	MessageMgr::SendMessageClients		Sends a message from the server to all clients in a multiplayer game.
	//
	//# \proto	void SendMessageClients(const Message& message, const Player *exclude = nullptr);
	//
	//# \param	message		The message to be sent.
	//# \param	exclude		An optional pointer to a player that the message will not be sent to.
	//
	//# \desc
	//# The $SendMessageClients$ function sends the message given by the $message$ parameter to every client machine.
	//# This function should only be called on the server machine and has no effect if called from a client machine.
	//# If the $exclude$ parameter is not $nullptr$, then the message will not be sent to the client corresponding
	//# to the excluded player.
	//
	//# \also	$@MessageMgr::SendMessageAll@$
	//# \also	$@MessageMgr::SendMessage@$


	//# \function	MessageMgr::SendMessageJournal		Sends a controller message to all player in a multiplayer game and adds it to the message journal.
	//
	//# \proto	void SendMessageJournal(ControllerMessage *message);
	//
	//# \param	message		The message to be sent.
	//
	//# \desc
	//# The $SendMessageJournal$ function sends the controller message given by the $message$ parameter to
	//# every player and to the local machine. The message is received and processed on the local machine before
	//# $SendMessageJournal$ returns. The message is also added to the message journal, and ownership of the
	//# $@ControllerMessage@$ object is transferred to the Message Manager.
	//#
	//# A client machine should not call the $SendMessageJournal$ function.
	//
	//# \also	$@MessageMgr::SendMessageAll@$
	//# \also	$@MessageMgr::SendMessage@$


	//# \function	MessageMgr::SendConnectionlessMessage		Sends a message to a remote machine for which no connection exists.
	//
	//# \proto	void SendConnectionlessMessage(const NetworkAddress& to, const Message& message);
	//
	//# \param	to			The address of the recipient of this message.
	//# \param	message		The message to be sent.
	//
	//# \desc
	//# The $SendConnectionlessMessage$ function sends a connectionless data packet to the machine whose
	//# address is given by the $address$ parameter. This function is normally called in response to a
	//# $kConnectionQueryReceived$ event received by the $@System/Application::HandleConnectionEvent@$
	//# to return information about the server to another machine that broadcasted a server query.
	//#
	//# Connectionless data packets are sent using the low-level Network Manager function
	//# $@NetworkMgr/NetworkMgr::SendConnectionlessPacket@$ and are not guaranteed to be received.
	//
	//# \also	$@MessageMgr::BroadcastServerQuery@$
	//# \also	$@System/Application::HandleConnectionEvent@$


	//# \function	MessageMgr::BroadcastServerQuery		Locates servers on a LAN.
	//
	//# \proto	void BroadcastServerQuery(void) const;
	//
	//# \desc
	//# Calling the $BroadcastServerQuery$ function causes a message to be broadcasted to the local area
	//# network asking servers to identify themselves. When a server receives this message, it calls the
	//# $@System/Application::HandleConnectionEvent@$ function with the $kConnectionQueryReceived$ event
	//# to inform the application module that it should send back information about the server.
	//#
	//# Only servers using the same protocol as that used by the local machine will receive the
	//# broadcasted query. The local machine's protocol can be set by calling the Network Manager function
	//# $@NetworkMgr/NetworkMgr::SetProtocol@$.
	//
	//# \also	$@NetworkMgr/NetworkMgr::SetProtocol@$
	//# \also	$@System/Application::HandleConnectionEvent@$


	//# \div
	//# \function	MessageMgr::AddressToString		Generates a text string corresponding to a network address.
	//
	//# \proto	static String<31> AddressToString(const NetworkAddress& address, bool includePort = false);
	//
	//# \param	address			The address for which to generate a text string.
	//# \param	includePort		Indicates whether the port number should be included in the text string.
	//
	//# \desc
	//# The $AddressToString$ function generates a text representation of the network address given by the
	//# $address$ parameter. If the $includePort$ parameter is $true$, then the returned string has the
	//# form $ip1.ip2.ip3.ip4:port$. If $includePort$ is $false$, then the returned string does not include
	//# the port number and has the form $ip1.ip2.ip3.ip4$.
	//
	//# \also	$@MessageMgr::StringToAddress@$
	//# \also	$@NetworkMgr/NetworkAddress@$
	//# \also	$@Utilities/String@$


	//# \function	MessageMgr::StringToAddress		Reads a network address from a text string.
	//
	//# \proto	static NetworkAddress StringToAddress(const char *text);
	//
	//# \param	text		A pointer to the text string to interpret.
	//
	//# \desc
	//# The $StringToAddress$ function interprets the string given by the $text$ parameter and constructs
	//# a $@NetworkMgr/NetworkAddress@$ object that holds the corresponding numerical address. The
	//# $StringToAddress$ function recognizes text strings of the form $ip1.ip2.ip3.ip4:port$. If the
	//# port component is not present, then the port number in the returned address is set to zero.
	//
	//# \also	$@MessageMgr::AddressToString@$
	//# \also	$@NetworkMgr/NetworkAddress@$


	class MessageMgr : public Manager<MessageMgr>
	{
		friend class Player;

		public:

			typedef Player *PlayerCreateProc(PlayerKey, void *);

			typedef ControllerMessage *ControllerMessageCreateProc(ControllerMessageType, int32, Decompressor&, void *);
			typedef void ControllerMessageReceiveProc(const ControllerMessage *, void *);

		private:

			bool								serverFlag;
			bool								multiplayerFlag;
			bool								synchronizedFlag;

			int32								maxPlayerCount;
			Map<Player>							playerMap;
			Link<Player>						localPlayer;

			int32								chatSendTime;
			int32								chatLockoutTime;

			int32								snapshotTime;
			int32								snapshotInterval;
			float								snapshotFrequency;

			List<StateSender>					stateSenderList;
			List<SnapshotSender>				snapshotSenderList;
			List<Message>						journaledMessageList;

			PlayerCreateProc					*playerCreatorProc;
			void								*playerCreatorCookie;

			ControllerMessageCreateProc			*controllerMessageCreator;
			ControllerMessageReceiveProc		*controllerMessageReceiver;
			void								*controllerMessageCookie;

			VariableObserver<MessageMgr>		playerNameObserver;
			VariableObserver<MessageMgr>		maxPlayersObserver;

			Player *NewPlayer(PlayerKey key = kPlayerNew);
			Player *GetPlayer(const NetworkAddress& address) const;
			Player *GetFirstClientPlayer(void) const;

			static void HandleNetworkEvent(NetworkEvent event, const NetworkAddress& address, unsigned_int32 param);

			Message *CreateMessage(MessageType type, Decompressor& data);
			void ReceiveMessage(const NetworkAddress& address, const Message *message);

			void HandlePlayerNameEvent(Variable *variable);
			void HandleMaxPlayersEvent(Variable *variable);

		public:

			MessageMgr(int);
			~MessageMgr();

			EngineResult Construct(void);
			void Destruct(void);

			bool Server(void) const
			{
				return (serverFlag);
			}

			bool Multiplayer(void) const
			{
				return (multiplayerFlag);
			}

			bool Synchronized(void) const
			{
				return (synchronizedFlag);
			}

			int32 GetPlayerCount(void) const
			{
				return (playerMap.GetElementCount());
			}

			int32 GetMaxPlayerCount(void) const
			{
				return (maxPlayerCount);
			}

			void SetMaxPlayerCount(int32 count)
			{
				maxPlayerCount = count;
			}

			Player *GetFirstPlayer(void) const
			{
				return (playerMap.First());
			}

			Player *GetLocalPlayer(void)
			{
				return (localPlayer);
			}

			PlayerKey GetLocalPlayerKey(void) const
			{
				return ((localPlayer) ? localPlayer->GetPlayerKey() : kPlayerNone);
			}

			Player *GetPlayer(PlayerKey key) const
			{
				return (playerMap.Find(key));
			}

			int32 GetChatLockoutTime(void) const
			{
				return (chatLockoutTime);
			}

			void SetChatLockoutTime(int32 time)
			{
				chatLockoutTime = time;
			}

			int32 GetSnapshotInterval(void) const
			{
				return (snapshotInterval);
			}

			float GetSnapshotFrequency(void) const
			{
				return (snapshotFrequency);
			}

			void InstallStateSender(StateSender *sender)
			{
				stateSenderList.Append(sender);
			}

			void SetPlayerCreator(PlayerCreateProc *proc, void *cookie = nullptr)
			{
				playerCreatorProc = proc;
				playerCreatorCookie = cookie;
			}

			ControllerMessageReceiveProc *GetControllerMessageReceiver(void) const
			{
				return (controllerMessageReceiver);
			}

			void *GetControllerMessageCookie(void) const
			{
				return (controllerMessageCookie);
			}

			void SetControllerMessageProcs(ControllerMessageCreateProc *creator, ControllerMessageReceiveProc *receiver, void *cookie = nullptr)
			{
				controllerMessageCreator = creator;
				controllerMessageReceiver = receiver;
				controllerMessageCookie = cookie;
			}

			C4API static String<31> AddressToString(const NetworkAddress& address, bool includePort = false);
			C4API static NetworkAddress StringToAddress(const char *text);

			C4API void BeginSinglePlayerGame(void);
			C4API NetworkResult BeginMultiplayerGame(bool host = true);
			C4API void EndGame(void);

			C4API void AddSnapshotSender(SnapshotSender *sender);
			C4API void RemoveSnapshotSender(SnapshotSender *sender);
			C4API void SetSnapshotInterval(int32 time);

			C4API void SendMessage(PlayerKey key, const Message& message);
			C4API void SendMessageAll(const Message& message, bool self = true);
			C4API void SendMessageClients(const Message& message, const Player *exclude = nullptr);
			C4API void SendMessageJournal(ControllerMessage *message);
			C4API bool SendChatMessage(const char *text);

			C4API void SendConnectionlessMessage(const NetworkAddress& to, const Message& message);
			C4API void BroadcastServerQuery(void) const;

			C4API NetworkResult Connect(const NetworkAddress& address);
			C4API void Disconnect(Player *player);
			C4API void DisconnectAll(void);

			C4API void ReceiveTask(void);
			C4API void SendTask(void);
	};


	C4API extern MessageMgr *TheMessageMgr;
}


#endif

// ZYUQURM
