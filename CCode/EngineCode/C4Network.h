 

#ifndef C4Network_h
#define C4Network_h


//# \component	Network Manager
//# \prefix		NetworkMgr/


#include "C4Threads.h"


namespace C4
{
	typedef EngineResult	NetworkResult;


	enum : NetworkResult
	{
		kNetworkOkay				= kEngineOkay,
		kNetworkInitFailed			= (kManagerNetwork << 16) | 0x0001,
		kNetworkNoConnection		= (kManagerNetwork << 16) | 0x0002,
		kNetworkPacketTooLarge		= (kManagerNetwork << 16) | 0x0003,
		kNetworkBufferFull			= (kManagerNetwork << 16) | 0x0004,
		kNetworkNoPacket			= (kManagerNetwork << 16) | 0x0005,
		kNetworkResolveUnresolved	= (kManagerNetwork << 16) | 0x0006,
		kNetworkResolvePending		= (kManagerNetwork << 16) | 0x0007,
		kNetworkResolveFailed		= (kManagerNetwork << 16) | 0x0008,
		kNetworkResolveNotFound		= (kManagerNetwork << 16) | 0x0009
	};


	enum
	{
		kMaxHostNameLength			= 255,
		kMaxPacketDataSize			= 512,
		kMaxIncomingPacketCount		= 32,
		kMaxOutgoingPacketCount		= 32
	};


	enum
	{
		kPacketFlagFile		= 1 << 0
	};


	//# \enum	NetworkFail

	enum
	{
		kNetworkFailTimeout,			//## The remote machine did not respond and the connection attempt timed out.
		kNetworkFailWrongProtocol,		//## The remote machine is not using the same communications protocol that the local machine is using.
		kNetworkFailNotServer,			//## The remote machine is using the correct protocol, but it is not a server.
		kNetworkFailServerFull			//## The remote machine did not accept the connection because the maximum number of clients have already connected.
	};


	#if C4WINDOWS

		enum
		{
			kWindowsMessageResolve		= WM_APP
		};

	#endif


	enum PacketType
	{
		kPacketControl,
		kPacketAcknowledge,
		kPacketReliable,
		kPacketUnreliable,
		kPacketUnordered,
		kPacketConnectionless,
		kPacketTypeCount
	};


	enum
	{
		kPacketTypeShift				= 29,
		kPacketNumberFirstControl		= kPacketControl << kPacketTypeShift,
		kPacketNumberFirstAcknowledge	= kPacketAcknowledge << kPacketTypeShift,
		kPacketNumberFirstReliable		= kPacketReliable << kPacketTypeShift,
		kPacketNumberFirstUnreliable	= kPacketUnreliable << kPacketTypeShift,
		kPacketNumberUnordered			= kPacketUnordered << kPacketTypeShift,
		kPacketNumberConnectionless		= kPacketConnectionless << kPacketTypeShift,
		kPacketNumberRangeMask			= (1 << kPacketTypeShift) - 1,
		kPacketNumberSignBit			= (1 << kPacketTypeShift) / 2
	};


	enum
	{
		kPacketNumberDisconnect = kPacketNumberFirstControl,
		kPacketNumberConnectRequest,
		kPacketNumberConnectAccept,
		kPacketNumberConnectDeny
	};
 

	enum ConnectionStatus
	{ 
		kConnectionRequest,
		kConnectionDisconnect, 
		kConnectionOpen,
		kConnectionClosed
	}; 

 
	//# \enum	NetworkEvent 

	enum NetworkEvent
	{
		kNetworkEventOpen,			//## A new connection has been opened by means of the local machine accepting a connection request from a remote machine. 
		kNetworkEventAccept,		//## A connection request originating from the local machine has been accepted by a remote machine.
		kNetworkEventClose,			//## A remote machine has explicitly disconnected from the local machine.
		kNetworkEventFail,			//## An attempt to connection to a remote machine has failed.
		kNetworkEventTimeout		//## A remote machine has timed out due to a reliable packet not being acknowledged.
	};


	class PacketNumber
	{
		private:

			unsigned_int32	number;

		public:

			PacketNumber() = default;

			PacketNumber(unsigned_int32 n)
			{
				number = n;
			}

			unsigned_int32 GetNumber(void) const
			{
				return (number);
			}

			PacketNumber& operator =(unsigned_int32 n)
			{
				number = n;
				return (*this);
			}

			unsigned_int32 operator ++(int)
			{
				unsigned_int32 n = number;
				number = ((n + 1) & kPacketNumberRangeMask) | (n & ~kPacketNumberRangeMask);
				return (n);
			}

			unsigned_int32 operator --(int)
			{
				unsigned_int32 n = number;
				number = ((n - 1) & kPacketNumberRangeMask) | (n & ~kPacketNumberRangeMask);
				return (n);
			}

			unsigned_int32 operator +(int32 n) const
			{
				return (((number + n) & kPacketNumberRangeMask) | (number & ~kPacketNumberRangeMask));
			}

			int32 operator -(const PacketNumber& pn) const
			{
				return ((number - pn.number) & kPacketNumberRangeMask);
			}

			bool operator ==(const PacketNumber& pn) const
			{
				return (number == pn.number);
			}

			bool operator !=(const PacketNumber& pn) const
			{
				return (number != pn.number);
			}

			bool operator <(const PacketNumber& pn) const
			{
				return (((number - pn.number) & kPacketNumberSignBit) != 0);
			}

			bool operator >(const PacketNumber& pn) const
			{
				return ((number != pn.number) && (((number - pn.number) & kPacketNumberSignBit) == 0));
			}

			bool operator >=(const PacketNumber& pn) const
			{
				return (((number - pn.number) & kPacketNumberSignBit) == 0);
			}

			bool operator <=(const PacketNumber& pn) const
			{
				return ((number == pn.number) || (((number - pn.number) & kPacketNumberSignBit) != 0));
			}
	};


	//# \class	NetworkAddress		Encapsulates a socket address.
	//
	//# The $NetworkAddress$ class Encapsulates a socket address.
	//
	//# \def	class NetworkAddress
	//
	//# \operator	NetworkAddress& operator =(const NetworkAddress& na);
	//#				Standard assignment operator.
	//
	//# \operator	bool operator ==(const NetworkAddress& na) const;
	//#				Standard equality operator.
	//
	//# \operator	bool operator !=(const NetworkAddress& na) const;
	//#				Standard inequality operator.
	//
	//# \ctor	NetworkAddress();
	//# \ctor	explicit NetworkAddress(unsigned_int32 addr, unsigned_int16 pt = 0);
	//
	//# \param	addr	The 32-bit IP address.
	//# \param	pt		The 16-bit port number.
	//
	//# \desc
	//# The $NetworkAddress$ class holds the 32-bit IP address and 16-bit port number corresponding
	//# to a communications endpoint on the internet. The default constructor fills these address
	//# components with zeros.
	//
	//# \also	$@NetworkMgr@$


	//# \function	NetworkAddress::Set		Sets the IP address and port number.
	//
	//# \proto	NetworkAddress& Set(unsigned_int32 addr, unsigned_int16 pt = 0);
	//
	//# \param	addr	The 32-bit IP address.
	//# \param	pt		The 16-bit port number.
	//
	//# \desc
	//# The $Set$ function sets the IP address of a $NetworkAddress$ object to the address given by
	//# the $addr$ parameter. The port number is set to the value given by the $pt$ parameter.
	//
	//# \also	$@NetworkAddress::GetAddress@$
	//# \also	$@NetworkAddress::GetPort@$
	//# \also	$@NetworkAddress::SetPort@$


	//# \function	NetworkAddress::GetAddress		Returns the 32-bit IP address.
	//
	//# \proto	unsigned_int32 GetAddress(void) const;
	//
	//# \desc
	//# The $GetAddress$ function returns the 32-bit IP address stored in a $NetworkAddress$ object.
	//# The 16-bit port number can be retrieved using the $@NetworkAddress::GetPort@$ function.
	//
	//# \also	$@NetworkAddress::Set@$
	//# \also	$@NetworkAddress::GetPort@$
	//# \also	$@NetworkAddress::SetPort@$


	//# \function	NetworkAddress::GetPort		Returns the 16-bit port number.
	//
	//# \proto	unsigned_int16 GetPort(void) const;
	//
	//# \desc
	//# The $GetPort$ function returns the 16-bit port number stored in a $NetworkAddress$ object.
	//# The 32-bit IP address can be retrieved using the $@NetworkAddress::GetAddress@$ function.
	//
	//# \also	$@NetworkAddress::SetPort@$
	//# \also	$@NetworkAddress::GetAddress@$
	//# \also	$@NetworkAddress::Set@$


	//# \function	NetworkAddress::SetPort		Sets the 16-bit port number.
	//
	//# \proto	void SetPort(unsigned_int16 pt);
	//
	//# \desc
	//# The $SetPort$ function sets the port number of a $NetworkAddress$ object to the value given by
	//# the $pt$ parameter. The 32-bit IP address stored in the $NetworkAddress$ object is not modified.
	//
	//# \also	$@NetworkAddress::GetPort@$
	//# \also	$@NetworkAddress::GetAddress@$
	//# \also	$@NetworkAddress::Set@$


	class NetworkAddress
	{
		private:

			unsigned_int32		address;
			unsigned_int16		port;

		public:

			NetworkAddress()
			{
				address = 0;
				port = 0;
			}

			explicit NetworkAddress(unsigned_int32 addr, unsigned_int16 pt = 0)
			{
				address = addr;
				port = pt;
			}

			NetworkAddress& Set(unsigned_int32 addr, unsigned_int16 pt = 0)
			{
				address = addr;
				port = pt;
				return (*this);
			}

			unsigned_int32 GetAddress(void) const
			{
				return (address);
			}

			unsigned_int16 GetPort(void) const
			{
				return (port);
			}

			void SetPort(unsigned_int16 pt)
			{
				port = pt;
			}

			NetworkAddress& operator =(const NetworkAddress& na)
			{
				address = na.address;
				port = na.port;
				return (*this);
			}

			bool operator ==(const NetworkAddress& na) const
			{
				return ((address == na.address) && (port == na.port));
			}

			bool operator !=(const NetworkAddress& na) const
			{
				return ((address != na.address) || (port != na.port));
			}

			bool operator <(const NetworkAddress& na) const
			{
				return ((address < na.address) || ((address == na.address) && (port < na.port)));
			}

			bool operator >(const NetworkAddress& na) const
			{
				return ((address > na.address) || ((address == na.address) && (port > na.port)));
			}
	};


	class NetworkPacket : public ListElement<NetworkPacket>
	{
		public:

			NetworkAddress			packetAddress;
			PacketNumber			packetNumber;
			unsigned_int32			packetSize;
			unsigned_int32			packetFlags;

			List<NetworkPacket>		*packetPool;

			unsigned_int32			sentTime;
			unsigned_int32			resendTime;
			int32					resendCount;

			union
			{
				char				packetDataS8[kMaxPacketDataSize];
				unsigned_int8		packetDataU8[kMaxPacketDataSize];
				unsigned_int32		packetDataU32[kMaxPacketDataSize / 4];
			};

			PacketType GetPacketType(void) const
			{
				return (static_cast<PacketType>(packetNumber.GetNumber() >> kPacketTypeShift));
			}

			void Encrypt(const void *data, unsigned_int32 size);
			void Decrypt(NetworkPacket *output) const;
	};


	class NetworkConnection : public MapElement<NetworkConnection>
	{
		friend class NetworkSocket;
		friend class NetworkMgr;

		private:

			NetworkAddress			connectionAddress;
			ConnectionStatus		connectionStatus;
			volatile int32			connectionPing;

			unsigned_int32			controlSendTime;
			int32					controlSendCount;

			unsigned_int32			initialPacketNumber;

			PacketNumber			incomingReliablePacketNumber;
			PacketNumber			incomingUnreliablePacketNumber;
			PacketNumber			outgoingReliablePacketNumber;
			PacketNumber			outgoingUnreliablePacketNumber;

			List<NetworkPacket>		incomingReliablePacketPool;
			List<NetworkPacket>		incomingUnreliablePacketPool;
			List<NetworkPacket>		outgoingDataPacketPool;
			List<NetworkPacket>		outgoingControlPacketPool;
			List<NetworkPacket>		activeOutgoingPacketList;
			List<NetworkPacket>		pendingAcknowledgePacketList;

			NetworkPacket			incomingReliablePacket[kMaxIncomingPacketCount];
			NetworkPacket			incomingUnreliablePacket[kMaxIncomingPacketCount];
			NetworkPacket			outgoingDataPacket[kMaxOutgoingPacketCount];
			NetworkPacket			outgoingControlPacket[kMaxOutgoingPacketCount];

		public:

			typedef NetworkAddress KeyType;

			NetworkConnection(const NetworkAddress& address, ConnectionStatus status, unsigned_int32 packetNumber = 0);
			~NetworkConnection();

			const KeyType& GetKey(void) const
			{
				return (connectionAddress);
			}

			const NetworkAddress& GetAddress(void) const
			{
				return (connectionAddress);
			}

			ConnectionStatus GetStatus(void) const
			{
				return (connectionStatus);
			}

			void SetStatus(ConnectionStatus status)
			{
				connectionStatus = status;
			}

			int32 GetPing(void) const
			{
				return (connectionPing);
			}

			void SetInitialPacketNumbers(unsigned_int32 number);
	};


	class NetworkSocket
	{
		friend class NetworkMgr;

		private:

			NetworkAddress		socketAddress;

			#if C4WINDOWS

				SOCKET			socketHandle;
				WSAEVENT		socketEvent[2];

			#elif C4POSIX

				int				socketDesc;
				int				pipeDesc[2];
				volatile bool	sendBlocked;

			#elif C4PS4 //[ PS4

				// -- PS4 code hidden --

			#elif C4PS3 //[ PS3

				// -- PS3 code hidden --

			#endif //]

			Thread				*socketThread;

			static void SocketThread(const Thread *thread, void *cookie);

			bool Send(NetworkPacket *packet);
			bool Receive(NetworkPacket *packet);

		public:

			NetworkSocket();
			~NetworkSocket();

			NetworkResult Open(const NetworkAddress& address);
			void Close(void);

			bool Open(void) const
			{
				return (socketAddress.GetAddress() != 0);
			}

			void SendPackets(void);
			void ReceivePackets(void);
	};


	//# \class	DomainNameResolver		Represents a domain name resolution operation.
	//
	//# The $DomainNameResolver$ class represents an in-progress domain name resolution.
	//
	//# \def	class DomainNameResolver : public ListElement<DomainNameResolver>, public Completable<DomainNameResolver>
	//
	//# \ctor	DomainNameResolver();
	//
	//# The constructor has private access. All instances of $DomainNameResolver$ are created by the Network Manager.
	//
	//# \desc
	//# A $DomainNameResolver$ object represents a pending request for a domain name resolution. A domain name resolution is initiated by calling
	//# the $@NetworkMgr::ResolveAddress@$ function, which returns a pointer to a newly created $DomainNameResolver$ object. When the domain name
	//# is resolved, the completion procedure is called and is passed an instance of the $DomainNameResolver$ class. A subsequent call to the
	//# $@DomainNameResolver::GetAddress@$ function retrieves the IP address corresponding to the domain name.
	//
	//# \base	Utilities/ListElement<DomainNameResolver>	Used internally by the Network Manager.
	//# \base	Utilities/Completable<DomainNameResolver>	The completion procedure is invoked when the domain name has been resolved.
	//
	//# \also	$@NetworkMgr::ResolveAddress@$


	//# \function	DomainNameResolver::GetResolveResult		Returns the result code of a domain name resolution.
	//
	//# \proto	NetworkResult GetResolveResult(void) const;
	//
	//# \desc
	//# The $GetResolveResult$ function returns the current status of a $@DomainNameResolver@$ object. The return value is one of the following
	//# network result codes.
	//
	//# \value	kNetworkOkay				The domain name was successfully resolved. The IP address can be retrieved using the
	//#										$@DomainNameResolver::GetAddress@$ function.
	//# \value	kNetworkResolvePending		The domain name resolution is still in progress.
	//# \value	kNetworkResolveNotFound		The resolution operation completed, but the domain name could not be found.
	//
	//# \also	$@DomainNameResolver::GetAddress@$


	//# \function	DomainNameResolver::GetAddress		Returns the IP address found by a domain name resolution.
	//
	//# \proto	NetworkAddress GetAddress(void) const;
	//
	//# \desc
	//# The $GetAddress$ function returns a $@NetworkAddress@$ object containing the 32-bit IP address corresponding to a domain name that was passed
	//# to the $@NetworkMgr::ResolveAddress@$ function. The port number stored in the $@NetworkAddress@$ object is set to zero.
	//#
	//# If the domain name could not be resolved, then the IP address stored in the $@NetworkAddress@$ object is set to zero.
	//# The $@DomainNameResolver::GetResolveResult@$ function can be called to determine whether a domain name resolution was successful.
	//
	//# \also	$@DomainNameResolver::GetResolveResult@$


	//# \function	DomainNameResolver::Cancel		Cancels an in-progress domain name resolution.
	//
	//# \proto	void Cancel(void);
	//
	//# \desc
	//# The $Cancel$ function destroys the domain name resolver object. If a domain name resolution is in progress, then it is immediately canceled.


	class DomainNameResolver : public ListElement<DomainNameResolver>, public Completable<DomainNameResolver>
	{
		friend class NetworkMgr;

		private:

			volatile NetworkResult			resolveResult;

			#if C4WINDOWS

				friend class Engine;

				HANDLE						resolveHandle;

				union
				{
					HOSTENT					hostInfo;
					char					hostData[MAXGETHOSTSTRUCT];
				};

				static void ResolveCallback(HANDLE handle, int error);

			#elif C4MACOS || C4IOS

				CFHostRef					resolveHost;
				CFHostClientContext			resolveContext;

				static void ResolveCallback(CFHostRef host, CFHostInfoType type, const CFStreamError *error, void *cookie);

			#elif C4POSIX

				bool						resolveFlag;
				pthread_t					resolveThread;

				addrinfo					*resolveAddress;
				String<kMaxHostNameLength>	resolveName;

				static void *ResolveThread(void *cookie);

			#elif C4PS4 //[ PS4

				// -- PS4 code hidden --

			#endif //]

			DomainNameResolver();
			~DomainNameResolver();

			void ResolveAddress(const char *name);

		public:

			NetworkResult GetResolveResult(void) const
			{
				return (resolveResult);
			}

			void Cancel(void)
			{
				delete this;
			}

			C4API String<kMaxHostNameLength> GetName(void) const;
			C4API NetworkAddress GetAddress(void) const;
	};


	//# \class	NetworkMgr		The Network Manager class.
	//
	//# \def	class NetworkMgr : public Manager<NetworkMgr>
	//
	//# \desc
	//# The $NetworkMgr$ class encapsulates the low-level networking functionality of the C4 Engine.
	//# High-level networking functionality is exposed by the $@MessageMgr/MessageMgr@$ class.
	//# The single instance of the Network Manager is constructed during an application's initialization
	//# and destroyed at termination.
	//#
	//# The Network Manager's member functions are accessed through the global pointer $TheNetworkMgr$.
	//
	//# \also	$@NetworkAddress@$
	//# \also	$@DomainNameResolver@$


	//# \function	NetworkMgr::Initialize		Initializes the Network Manager.
	//
	//# \note
	//# This is a low-level function that is normally called exclusively by the Message Manager.
	//# Calling this function directly is not recommended if the Message Manager is being used.
	//
	//# \proto	NetworkResult Initialize(void);
	//
	//# \desc
	//# The $Initialize$ function attempts to initialize the underlying TCP/IP implementation and
	//# prepare the Network Manager for communications. The $Initialize$ function returns one of the
	//# following network result codes.
	//
	//# \value	kNetworkOkay 		The Network Manager was successfully initialized.
	//# \value	kNetworkInitFailed 	The Network Manager could not be initialized because the operating system returned an error.
	//
	//# \desc
	//# If the Network Manager has already been initialized at the time when $Initialize$ is called,
	//# then the return value is $kNetworkOkay$&mdash;duplicate calls to Initialize are harmless.
	//#
	//# The $@NetworkMgr::SetPortNumber@$ function should be called before the $Initialize$ function
	//# is called to set the port number on which communications will occur. On client machines, the port
	//# number may be set to zero to indicate that any available port number may be chosen by the TCP/IP
	//# implementation.
	//
	//# \also	$@NetworkMgr::Terminate@$
	//# \also	$@NetworkMgr::SetPortNumber@$
	//# \also	$@MessageMgr/MessageMgr::BeginMultiplayerGame@$


	//# \function	NetworkMgr::Terminate		Terminates the Network Manager.
	//
	//# \note
	//# This is a low-level function that is normally called exclusively by the Message Manager.
	//# Calling this function directly is not recommended if the Message Manager is being used.
	//
	//# \proto	void Terminate(void);
	//
	//# \desc
	//# The $Terminate$ function terminates all communications and releases any resources allocated
	//# by the underlying TCP/IP implementation. If the Network Manager has not been initialized at
	//# the time when $Terminate$ is called, then this function has no effect&mdash;duplicate calls
	//# to $Terminate$ are harmless.
	//
	//# \special
	//# After disconnecting from remote machines, a small amount of time should be allowed to pass
	//# before terminating the Network Manager. This is to allow enough time for the communications
	//# necessary for an orderly disconnection to occur. The number of connections to the local machine
	//# can be monitored by calling the $@NetworkMgr::GetConnectionCount@$ function. When the connection
	//# count becomes zero, it is safe to terminate the Network Manager.
	//
	//# \also	$@NetworkMgr::Initialize@$
	//# \also	$@NetworkMgr::GetConnectionCount@$
	//# \also	$@MessageMgr/MessageMgr::BeginSinglePlayerGame@$


	//# \div
	//# \function	NetworkMgr::GetLocalAddress		Returns the network address of the local machine.
	//
	//# \proto	const NetworkAddress& GetLocalAddress(void);
	//
	//# \desc
	//# The $GetLocalAddress$ function returns a $@NetworkAddress@$ object containing the 32-bit IP
	//# address of the local machine and the 16-bit port number currently in use. If the Network Manager
	//# has not been initialized, then the network address returned is filled with zeros.
	//
	//# \also	$@NetworkMgr::Initialize@$
	//# \also	$@NetworkMgr::Terminate@$
	//# \also	$@NetworkMgr::GetPortNumber@$
	//# \also	$@NetworkMgr::SetPortNumber@$


	//# \function	NetworkMgr::GetConnectionCount		Returns the number of connections that are currently established.
	//
	//# \proto	int32 GetConnectionCount(void);
	//
	//# \desc
	//# The $GetConnectionCount$ function returns the total number of connections that are currently
	//# established, including those that were initiated by the local machine and those that were
	//# initiated by remote machines. The number of connections initiated remotely that are accepted
	//# by the local machine can be limited by calling $@NetworkMgr::SetMaxConnectionCount@$.
	//#
	//# It may be useful to poll the $GetConnectionCount$ function after disconnecting from remote
	//# machines, but before terminating the Network Manager. When $GetConnectionCount$ returns zero,
	//# all connections have been terminated in an orderly fashion, and it is safe to call
	//# $@NetworkMgr::Terminate@$.
	//
	//# \also	$@NetworkMgr::Connect@$
	//# \also	$@NetworkMgr::Disconnect@$
	//# \also	$@NetworkMgr::GetMaxConnectionCount@$
	//# \also	$@NetworkMgr::SetMaxConnectionCount@$
	//# \also	$@NetworkMgr::Terminate@$


	//# \function	NetworkMgr::GetMaxConnectionCount		Returns the maximum number of connections that the Network Manager will allow at one time.
	//
	//# \proto	int32 GetMaxConnectionCount(void) const;
	//
	//# \desc
	//# The number returned by $GetMaxConnectionCount$ represents the limit on the number of connections
	//# that the Network Manager will accept from remote machines. This limit has no effect on the ability
	//# to make outgoing connections. If the total number of connections equals or exceeds the maximum
	//# number set by the $@NetworkMgr::SetMaxConnectionCount@$ function, then connections initiated
	//# remotely are denied, but connections initiated locally are still possible.
	//
	//# \also	$@NetworkMgr::SetMaxConnectionCount@$
	//# \also	$@NetworkMgr::GetConnectionCount@$
	//# \also	$@MessageMgr/MessageMgr::GetMaxPlayerCount@$
	//# \also	$@MessageMgr/MessageMgr::SetMaxPlayerCount@$


	//# \function	NetworkMgr::SetMaxConnectionCount		Sets the maximum number of connections that the Network Manager will allow at one time.
	//
	//# \proto	void SetMaxConnectionCount(int32 max);
	//
	//# \param	max		The maximum number of connections.
	//
	//# \desc
	//# The number passed to the $SetMaxConnectionCount$ function represents the limit on the number
	//# of connections that the Network Manager will accept from remote machines. This limit has no
	//# effect on the ability to make outgoing connections. If the total number of connections equals
	//# or exceeds the maximum number given by the $max$ parameter, then connections initiated remotely
	//# are denied, but connections initiated locally are still possible.
	//#
	//# The Message Manager automatically calls $SetMaxConnectionCount$ from within the
	//# $@MessageMgr/MessageMgr::BeginMultiplayerGame@$ function. The maximum number of connections set by the
	//# Message Manager is one less than the maximum number of players set by the
	//# $@MessageMgr/MessageMgr::SetMaxPlayerCount@$ function.
	//
	//# \also	$@NetworkMgr::GetMaxConnectionCount@$
	//# \also	$@NetworkMgr::GetConnectionCount@$
	//# \also	$@MessageMgr/MessageMgr::GetMaxPlayerCount@$
	//# \also	$@MessageMgr/MessageMgr::SetMaxPlayerCount@$
	//# \also	$@MessageMgr/MessageMgr::BeginMultiplayerGame@$


	//# \function	NetworkMgr::GetProtocol		Returns the protocol identifier used by the current application.
	//
	//# \proto	unsigned_int32 GetProtocol(void) const;
	//
	//# \desc
	//# The protocol identifier returned by the $GetProtocol$ function is a unique 32-bit quantity
	//# that identifies the current application. The Network Manager will only allow connections
	//# from remote machines that are using the same protocol identifier as the local machine.
	//#
	//# The high 16 bits of the protocol identifier represent the application, and the low 16 bits
	//# are used as a revision number. Every application must use its unique protocol identifier
	//# to prevent unintentional communications with other applications using the C4 Network Manager.
	//
	//# \also	$@NetworkMgr::SetProtocol@$


	//# \function	NetworkMgr::SetProtocol		Sets the protocol identifier used by the current application.
	//
	//# \proto	void SetProtocol(unsigned_int32 protocol);
	//
	//# \param	protocol	The 32-bit protocol identifier.
	//
	//# \desc
	//# The protocol identifier given by the $protocol$ parameter is a unique 32-bit quantity that
	//# identifies the current application. The Network Manager will only allow connections from
	//# remote machines that are using the same protocol identifier as the local machine.
	//#
	//# The high 16 bits of the protocol identifier represent the application, and the low 16 bits
	//# are used as a revision number. Every application must use its unique protocol identifier
	//# to prevent unintentional communications with other applications using the C4 Network Manager.
	//#
	//# The $SetProtocol$ function should be called before the Network Manager is initialized either
	//# by a direct call to $@NetworkMgr::Initialize@$ or indirectly through the Message Manager
	//# function $@MessageMgr/MessageMgr::BeginMultiplayerGame@$.
	//
	//# \also	$@NetworkMgr::GetProtocol@$
	//# \also	$@NetworkMgr::Initialize@$
	//# \also	$@MessageMgr/MessageMgr::BeginMultiplayerGame@$


	//# \function	NetworkMgr::GetPortNumber		Returns the port number on which the local machine will send and receive communications.
	//
	//# \proto	unsigned_int16 GetPortNumber(void) const;
	//
	//# \desc
	//# The port number returned by the $GetPortNumber$ function is the port number that will be
	//# utilized when the Network Manager is initialized. This number is not necessarily the port
	//# number in use by the Network Manager after initialization. To retrieve the actual port
	//# number in use by the local machine, call the $@NetworkMgr::GetLocalAddress@$ function.
	//#
	//# The port number that will be used by the Network Manager is set by calling the
	//# $@NetworkMgr::SetPortNumber@$ function.
	//
	//# \also	$@NetworkMgr::SetPortNumber@$
	//# \also	$@NetworkMgr::GetBroadcastPortNumber@$
	//# \also	$@NetworkMgr::SetBroadcastPortNumber@$
	//# \also	$@NetworkMgr::Initialize@$
	//# \also	$@NetworkMgr::GetLocalAddress@$


	//# \function	NetworkMgr::SetPortNumber		Sets the port number on which the local machine will send and receive communications.
	//
	//# \proto	void SetPortNumber(unsigned_int16 port);
	//
	//# \param	port		The 16-bit port number.
	//
	//# \desc
	//# The port number passed to the $SetPortNumber$ function is the port number that the Network Manager
	//# will attempt to use upon initialization. Servers should generally call $SetPortNumber$, specifying
	//# a port number on which client applications will expect it to be listening, before the Network
	//# Manager is initialized. Clients may specify a port number of zero, in which case the Network
	//# Manager allows the underlying TCP/IP implementation to select an available port. The port number
	//# is set to zero by default.
	//#
	//# Calls to $SetPortNumber$ that occur after the Network Manager has been initialized do not change
	//# the port on which communications occur. In order to make this change, one must terminate the
	//# Network Manager (by calling $@NetworkMgr::Terminate@$), set a new port number, and then
	//# reinitialize the Network Manager (by calling $@NetworkMgr::Initialize@$).
	//
	//# \also	$@NetworkMgr::GetPortNumber@$
	//# \also	$@NetworkMgr::GetBroadcastPortNumber@$
	//# \also	$@NetworkMgr::SetBroadcastPortNumber@$
	//# \also	$@NetworkMgr::Initialize@$
	//# \also	$@NetworkMgr::Terminate@$


	//# \function	NetworkMgr::GetBroadcastPortNumber		Returns the port number to which broadcasted messages are sent.
	//
	//# \proto	unsigned_int16 GetBroadcastPortNumber(void) const;
	//
	//# \desc
	//# The port number returned by the $GetBroadcastPortNumber$ function is the port number to which
	//# broadcasted messages are sent. The broadcast port number can be set by calling the
	//# $@NetworkMgr::SetBroadcastPortNumber@$ function.
	//
	//# \also	$@NetworkMgr::SetBroadcastPortNumber@$
	//# \also	$@NetworkMgr::BroadcastPacket@$


	//# \function	NetworkMgr::SetBroadcastPortNumber		Sets the port number to which broadcasted messages are sent.
	//
	//# \proto	void SetBroadcastPortNumber(unsigned_int16 port);
	//
	//# \param	port		The 16-bit port number.
	//
	//# \desc
	//# The port number passed to the $SetBroadcastPortNumber$ function is the port number to which
	//# broadcasted messages are sent. In order to be received, broadcasted messages must be sent to
	//# the port on which servers are listening. Thus, whatever port number is specified by a server
	//# when it calls $@NetworkMgr::SetPortNumber@$ is generally the same port number to which clients
	//# should broadcast.
	//
	//# \also	$@NetworkMgr::GetBroadcastPortNumber@$
	//# \also	$@NetworkMgr::BroadcastPacket@$
	//# \also	$@NetworkMgr::GetPortNumber@$
	//# \also	$@NetworkMgr::SetPortNumber@$


	//# \function	NetworkMgr::GetReliableResendTime		Returns the time interval between reliable packet transmission attempts.
	//
	//# \proto	unsigned_int32 GetReliableResendTime(void) const;
	//
	//# \desc
	//# When the Network Manager sends a reliable packet, it waits for a return receipt, or acknowledgement,
	//# from the receiving machine that confirms the packet's arrival at its destination. If an
	//# acknowledgement has not been received after a certain time period, the packet is retransmitted.
	//# The Network Manager makes a certain number of attempts before a connection is considered to have
	//# timed out. The $GetReliableResendTime$ function returns the time interval between attempts to send
	//# a reliable packet, in milliseconds. This time can be changed by calling $@NetworkMgr::SetReliableResendTime@$.
	//#
	//# When a connection times out, the Network Manager calls the currently installed network event
	//# procedure, if any, and then disconnects from the machine that timed out. The low-level network
	//# event procedure can be set using the $@NetworkMgr::SetNetworkEventProc@$ function, but this
	//# procedure should not be changed if the Message Manager is being used. Instead, the Message Manager
	//# will call the $@System/Application::HandleConnectionEvent@$ function when a connection times out.
	//
	//# \also	$@NetworkMgr::SetReliableResendTime@$
	//# \also	$@NetworkMgr::GetReliableResendCount@$
	//# \also	$@NetworkMgr::SetReliableResendCount@$
	//# \also	$@NetworkMgr::SetNetworkEventProc@$
	//# \also	$@System/Application::HandleConnectionEvent@$


	//# \function	NetworkMgr::SetReliableResendTime		Sets the time interval between reliable packet transmission attempts.
	//
	//# \proto	void SetReliableResendTime(unsigned_int32 time);
	//
	//# \param	time		The time between transmission attempts, in milliseconds.
	//
	//# \desc
	//# When the Network Manager sends a reliable packet, it waits for a return receipt, or acknowledgement,
	//# from the receiving machine that confirms the packet's arrival at its destination. If an
	//# acknowledgement has not been received after a certain time period, the packet is retransmitted.
	//# The Network Manager makes a certain number of attempts before a connection is considered to have
	//# timed out. The $SetReliableResendTime$ function sets the time interval between attempts to send a
	//# reliable packet.
	//#
	//# When a connection times out, the Network Manager calls the currently installed network event
	//# procedure, if any, and then disconnects from the machine that timed out. The low-level network
	//# event procedure can be set using the $@NetworkMgr::SetNetworkEventProc@$ function, but this
	//# procedure should not be changed if the Message Manager is being used. Instead, the Message Manager
	//# will call the $@System/Application::HandleConnectionEvent@$ function when a connection times out.
	//
	//# \also	$@NetworkMgr::GetReliableResendTime@$
	//# \also	$@NetworkMgr::GetReliableResendCount@$
	//# \also	$@NetworkMgr::SetReliableResendCount@$
	//# \also	$@NetworkMgr::SetNetworkEventProc@$
	//# \also	$@System/Application::HandleConnectionEvent@$


	//# \function	NetworkMgr::GetReliableResendCount		Returns the time interval between reliable packet transmission attempts.
	//
	//# \proto	int32 GetReliableResendCount(void) const;
	//
	//# \desc
	//# When the Network Manager sends a reliable packet, it waits for a return receipt, or acknowledgement,
	//# from the receiving machine that confirms the packet's arrival at its destination. If an
	//# acknowledgement has not been received after a certain time period, the packet is retransmitted.
	//# The Network Manager makes a certain number of attempts before a connection is considered to have
	//# timed out. The $GetReliableResendCount$ function returns the number of attempts that the Network
	//# Manager makes to send a reliable packet. This number can be changed by calling
	//# $@NetworkMgr::SetReliableResendCount@$.
	//#
	//# When a connection times out, the Network Manager calls the currently installed network event
	//# procedure, if any, and then disconnects from the machine that timed out. The low-level network
	//# event procedure can be set using the $@NetworkMgr::SetNetworkEventProc@$ function, but this
	//# procedure should not be changed if the Message Manager is being used. Instead, the Message Manager
	//# will call the $@System/Application::HandleConnectionEvent@$ function when a connection times out.
	//
	//# \also	$@NetworkMgr::SetReliableResendCount@$
	//# \also	$@NetworkMgr::GetReliableResendTime@$
	//# \also	$@NetworkMgr::SetReliableResendTime@$
	//# \also	$@NetworkMgr::SetNetworkEventProc@$
	//# \also	$@System/Application::HandleConnectionEvent@$


	//# \function	NetworkMgr::SetReliableResendCount		Sets the time interval between reliable packet transmission attempts.
	//
	//# \proto	void SetReliableResendCount(int32 count);
	//
	//# \param	count		The number of attempts that are made to transmit a reliable packet.
	//
	//# \desc
	//# When the Network Manager sends a reliable packet, it waits for a return receipt, or acknowledgement,
	//# from the receiving machine that confirms the packet's arrival at its destination. If an
	//# acknowledgement has not been received after a certain time period, the packet is retransmitted.
	//# The Network Manager makes a certain number of attempts before a connection is considered to have
	//# timed out. The $SetReliableResendCount$ function sets the number of attempts that the Network
	//# Manager makes to send a reliable packet.
	//#
	//# When a connection times out, the Network Manager calls the currently installed network event
	//# procedure, if any, and then disconnects from the machine that timed out. The low-level network
	//# event procedure can be set using the $@NetworkMgr::SetNetworkEventProc@$ function, but this
	//# procedure should not be changed if the Message Manager is being used. Instead, the Message Manager
	//# will call the $@System/Application::HandleConnectionEvent@$ function when a connection times out.
	//
	//# \also	$@NetworkMgr::GetReliableResendCount@$
	//# \also	$@NetworkMgr::GetReliableResendTime@$
	//# \also	$@NetworkMgr::SetReliableResendTime@$
	//# \also	$@NetworkMgr::SetNetworkEventProc@$
	//# \also	$@System/Application::HandleConnectionEvent@$


	//# \div
	//# \function	NetworkMgr::Connect		Attempts to establish a connection with another machine.
	//
	//# \note
	//# This is a low-level function that is normally called exclusively by the Message Manager.
	//# Calling this function directly is not recommended if the Message Manager is being used.
	//
	//# \proto	void Connect(const NetworkAddress& address);
	//
	//# \param	address		The address of the machine with which to establish a connection.
	//
	//# \desc
	//# The $Connect$ function initiates an attempt to connect to the remote server whose address is given
	//# by the $address$ parameter. This function returns immediately and the Network Manager begins
	//# listening for a response.
	//#
	//# If the connection is accepted by the server, then the Network Manager on both the client machine
	//# and the server machine calls the currently installed network event procedure, if any. The low-level
	//# network event procedure can be set using the $@NetworkMgr::SetNetworkEventProc@$ function, but this
	//# procedure should not be changed if the Message Manager is being used. Instead, the Message Manager
	//# will call the $@System/Application::HandleConnectionEvent@$ function when a connection is accepted.
	//#
	//# If the connection is denied by the server, or if the connection attempt times out, then the network
	//# event procedure is called with the $kNetworkEventFail$ event on the client machine. The value of
	//# the $param$ parameter passed to the network event procedure is one of the following constants indicating
	//# why the connection could not be made.
	//
	//# \table	NetworkFail
	//
	//# When the Message Manager receives this event, it calls the $@System/Application::HandleConnectionEvent@$
	//# function with the $kConnectionAttemptFailed$ event and passes the same failure code the application.
	//#
	//# The number of attempts to make a connection and the time interval between attempts are same as
	//# those used for reliable packet transmission. These values can be changed using the
	//# $@NetworkMgr::SetReliableResendCount@$ and $@NetworkMgr::SetReliableResendTime@$ functions.
	//
	//# \also	$@NetworkAddress@$
	//# \also	$@MessageMgr/MessageMgr@$
	//# \also	$@NetworkMgr::Disconnect@$
	//# \also	$@NetworkMgr::SetNetworkEventProc@$
	//# \also	$@MessageMgr/MessageMgr::Connect@$
	//# \also	$@System/Application::HandleConnectionEvent@$


	//# \function	NetworkMgr::Disconnect		Terminates a connection with another machine.
	//
	//# \note
	//# This is a low-level function that is normally called exclusively by the Message Manager.
	//# Calling this function directly is not recommended if the Message Manager is being used.
	//
	//# \proto	void Disconnect(const NetworkAddress& address);
	//
	//# \param	address		The address of the machine with which to terminate a connection.
	//
	//# \desc
	//# The $Disconnect$ function terminates the connection with the machine whose address is given by
	//# the $address$ parameter. If no connection to this address exists, then the $Disconnect$ function
	//# has no effect.
	//#
	//# When a server receives a disconnect message from a client, the Network Manager on the server calls
	//# the currently installed network event procedure, if any. The low-level network event
	//# procedure can be set using the $@NetworkMgr::SetNetworkEventProc@$ function, but this procedure
	//# should not be changed if the Message Manager is being used. Instead, the Message Manager on the
	//# server and on each remaining client in the game calls the $@System/Application::HandlePlayerEvent@$
	//# function to inform the application modules on those machines that a player has departed.
	//#
	//# The Network Manager attempts to disconnect in an orderly fashion by listening for an
	//# acknowledgement to the disconnection message. The number of attempts to disconnect and the time
	//# interval between these attempts are the same as those used for reliable packet transmission.
	//# These values can be changed using the $@NetworkMgr::SetReliableResendCount@$ and
	//# $@NetworkMgr::SetReliableResendTime@$ functions.
	//
	//# \also	$@NetworkAddress@$
	//# \also	$@MessageMgr/MessageMgr@$
	//# \also	$@NetworkMgr::Connect@$
	//# \also	$@NetworkMgr::SetNetworkEventProc@$
	//# \also	$@MessageMgr/MessageMgr::Disconnect@$
	//# \also	$@System/Application::HandlePlayerEvent@$


	//# \function	NetworkMgr::ResolveAddress		Looks up the IP address corresponding to a domain name.
	//
	//# \proto	DomainNameResolver *ResolveAddress(const char *name);
	//
	//# \param	name	The domain name of the server whose IP address should be resolved.
	//
	//# \desc
	//# Calling the $ResolveAddress$ function causes an object of type $@DomainNameResolver@$ to be created
	//# and maintained by the Network Manager. Domain names are resolved asychronously, so the
	//# $ResolveAddress$ function always returns immediately. If the domain name resolution operation
	//# is successfully initiated, then the return value is a pointer to a $@DomainNameResolver@$ object;
	//# otherwise, the return value is $nullptr$.
	//#
	//# After a successful call to $ResolveAddress$, a completion procedure should be installed for the
	//# returned object. This completion procedure is invoked during the next call to
	//# $@NetworkMgr::NetworkTask@$ after the domain name resolution completes. The completion procedure
	//# can extract the resolved IP address by calling the $@DomainNameResolver::GetAddress@$ function.
	//# After the completion procedure is called, the $@DomainNameResolver@$ object is automatically destroyed.
	//#
	//# A domain name resolution can be canceled before its completion procedure is invoked by calling the
	//# $@DomainNameResolver::Cancel@$ function.
	//
	//# \special
	//# The $ResolveAddress$ function should not be called before the Network Manager has been successfully
	//# initialized by the $@NetworkMgr::Initialize@$ function or the $@MessageMgr/MessageMgr::BeginMultiplayerGame@$ function.
	//
	//# \also	$@DomainNameResolver@$


	//# \div
	//# \function	NetworkMgr::SendReliablePacket		Sends a reliable packet to another machine.
	//
	//# \note
	//# This is a low-level function that is normally called exclusively by the Message Manager.
	//# Calling this function directly is not recommended if the Message Manager is being used.
	//
	//# \proto	NetworkResult SendReliablePacket(const NetworkAddress& to, unsigned_int32 size, const void *data, unsigned_int32 flags = 0);
	//
	//# \param	to			The destination address.
	//# \param	size		The size of the data packet. This value may not exceed the maximum size given by the constant $kMaxMessageSize$.
	//# \param	data		A pointer to the data to be sent.
	//# \param	flags		Packet flags. This value is used internally and should be set to zero.
	//
	//# \desc
	//# The $SendReliablePacket$ function sends a data packet to the machine whose address is given by
	//# the $to$ parameter and listens for an acknowledgement of receipt. The return value is one of the
	//# following network result codes.
	//
	//# \value	kNetworkOkay 			The packet was successfully queued for transmission.
	//# \value	kNetworkNoConnection 	The operation failed because the address specified by the $to$
	//#									parameter does not correspond to any current network connection.
	//# \value	kNetworkPacketTooLarge 	The packet could not be sent because its size exceeds $kMaxMessageSize$.
	//# \value	kNetworkBufferFull 		The packet could not be sent because the outgoing packet buffers
	//#									are full. Try sending the packet again at a later time.
	//
	//# \desc
	//# A connection must be established with a machine before the $SendReliablePacket$ function can be
	//# used to send messages to it. Connections are established at the Network Manager level by calling
	//# the $@NetworkMgr::Connect@$ function.
	//#
	//# When the Network Manager sends a reliable packet, it waits for a return receipt, or acknowledgement,
	//# from the receiving machine that confirms the packet's arrival at its destination. If an
	//# acknowledgement has not been received after a certain time period, the packet is retransmitted.
	//# The Network Manager makes a certain number of attempts before a connection is considered to have
	//# timed out. The number of attempts and the time interval between those attempts can be set using
	//# the $@NetworkMgr::SetReliableResendCount@$ and $@NetworkMgr::SetReliableResendTime@$ functions.
	//#
	//# When a connection times out because a reliable packet was not acknowledged, the Network Manager
	//# calls the currently installed network event procedure, if any, and then disconnects from the
	//# machine that timed out. The low-level network event procedure can be set using the
	//# $@NetworkMgr::SetNetworkEventProc@$ function, but this procedure should not be changed if the
	//# Message Manager is being used. Instead, the Message Manager will call the
	//# $@System/Application::HandleConnectionEvent@$ function when a connection times out.
	//#
	//# Packet data is encrypted before it is transmitted.
	//
	//# \also	$@NetworkAddress@$
	//# \also	$@NetworkMgr::SendUnreliablePacket@$
	//# \also	$@NetworkMgr::SendUnorderedPacket@$
	//# \also	$@NetworkMgr::SendConnectionlessPacket@$
	//# \also	$@NetworkMgr::BroadcastPacket@$
	//# \also	$@NetworkMgr::ReceivePacket@$
	//# \also	$@NetworkMgr::Connect@$
	//# \also	$@NetworkMgr::SetReliableResendCount@$
	//# \also	$@NetworkMgr::SetReliableResendTime@$
	//# \also	$@System/Application::HandleConnectionEvent@$


	//# \function	NetworkMgr::SendUnreliablePacket		Sends an unreliable packet to another machine.
	//
	//# \note
	//# This is a low-level function that is normally called exclusively by the Message Manager.
	//# Calling this function directly is not recommended if the Message Manager is being used.
	//
	//# \proto	NetworkResult SendUnreliablePacket(const NetworkAddress& to, unsigned_int32 size, const void *data);
	//
	//# \param	to			The destination address.
	//# \param	size		The size of the data packet. This value may not exceed the maximum size given by the constant $kMaxMessageSize$.
	//# \param	data		A pointer to the data to be sent.
	//
	//# \desc
	//# The $SendUnreliablePacket$ function sends a data packet to the machine whose address is given by
	//# the $to$ parameter, but does not guarantee delivery. The return value is one of the following
	//# network result codes.
	//
	//# \value	kNetworkOkay 			The packet was successfully queued for transmission.
	//# \value	kNetworkNoConnection 	The operation failed because the address specified by the $to$
	//#									parameter does not correspond to any current network connection.
	//# \value	kNetworkPacketTooLarge 	The packet could not be sent because its size exceeds $kMaxMessageSize$.
	//# \value	kNetworkBufferFull 		The packet could not be sent because the outgoing packet buffers
	//#									are full. Try sending the packet again at a later time.
	//
	//# \desc
	//# A connection must be established with a machine before the $SendUnreliablePacket$ function can be
	//# used to send messages to it. Connections are established at the Network Manager level by calling
	//# the $@NetworkMgr::Connect@$ function.
	//#
	//# Unreliable packets are used for "fire and forget" messages. Once sent, an unreliable packet is no
	//# longer tracked by the sending machine, and the receiving machine does not return an acknowledgement.
	//# Delivery of an unreliable packet is not guaranteed, but the Network Manager does ensure that
	//# unreliable packets are received in the same order in which they were sent. (This is done by
	//# ignoring any unreliable packets sent earlier than the one most recently received.)
	//#
	//# Packet data is encrypted before it is transmitted.
	//
	//# \also	$@NetworkAddress@$
	//# \also	$@NetworkMgr::SendReliablePacket@$
	//# \also	$@NetworkMgr::SendUnorderedPacket@$
	//# \also	$@NetworkMgr::SendConnectionlessPacket@$
	//# \also	$@NetworkMgr::BroadcastPacket@$
	//# \also	$@NetworkMgr::ReceivePacket@$
	//# \also	$@NetworkMgr::Connect@$


	//# \function	NetworkMgr::SendUnorderedPacket		Sends an unordered packet to another machine.
	//
	//# \note
	//# This is a low-level function that is normally called exclusively by the Message Manager.
	//# Calling this function directly is not recommended if the Message Manager is being used.
	//
	//# \proto	NetworkResult SendUnorderedPacket(const NetworkAddress& to, unsigned_int32 size, const void *data);
	//
	//# \param	to			The destination address.
	//# \param	size		The size of the data packet. This value may not exceed the maximum size given by the constant $kMaxMessageSize$.
	//# \param	data		A pointer to the data to be sent.
	//
	//# \desc
	//# The $SendUnorderedPacket$ function sends a data packet to the machine whose address is given by
	//# the $to$ parameter, but does not guarantee delivery and does not guarantee in-order delivery. The return
	//# value is one of the following network result codes.
	//
	//# \value	kNetworkOkay 			The packet was successfully queued for transmission.
	//# \value	kNetworkNoConnection 	The operation failed because the address specified by the $to$
	//#									parameter does not correspond to any current network connection.
	//# \value	kNetworkPacketTooLarge 	The packet could not be sent because its size exceeds $kMaxMessageSize$.
	//# \value	kNetworkBufferFull 		The packet could not be sent because the outgoing packet buffers
	//#									are full. Try sending the packet again at a later time.
	//
	//# \desc
	//# A connection must be established with a machine before the $SendUnorderedPacket$ function can be
	//# used to send messages to it. Connections are established at the Network Manager level by calling
	//# the $@NetworkMgr::Connect@$ function.
	//#
	//# Unordered packets are used for "fire and forget" messages for which the order of receipt is not important.
	//# Once sent, an unordered packet is no longer tracked by the sending machine, and the receiving machine does
	//# not return an acknowledgement. Delivery of an unordered packet is not guaranteed, and unlike unreliable packets,
	//# the Network Manager does not ensure that unordered packets are received in the same order in which they were sent.
	//#
	//# Packet data is encrypted before it is transmitted.
	//
	//# \also	$@NetworkAddress@$
	//# \also	$@NetworkMgr::SendReliablePacket@$
	//# \also	$@NetworkMgr::SendUnreliablePacket@$
	//# \also	$@NetworkMgr::SendConnectionlessPacket@$
	//# \also	$@NetworkMgr::BroadcastPacket@$
	//# \also	$@NetworkMgr::ReceivePacket@$
	//# \also	$@NetworkMgr::Connect@$


	//# \function	NetworkMgr::SendConnectionlessPacket		Sends an unreliable data packet to another machine without establishing a connection.
	//
	//# \note
	//# This is a low-level function that is normally called exclusively by the Message Manager.
	//# Calling this function directly is not recommended if the Message Manager is being used.
	//
	//# \proto	NetworkResult SendConnectionlessPacket(const NetworkAddress& to, unsigned_int32 size, const void *data);
	//
	//# \param	to			The destination address.
	//# \param	size		The size of the data packet. This value may not exceed the maximum size given by the constant $kMaxMessageSize$.
	//# \param	data		A pointer to the data to be sent.
	//
	//# \desc
	//# The $SendConnectionlessPacket$ function sends an unreliable data packet to the machine whose
	//# address is given by the $to$ parameter. It is not necessary to establish a connection with the
	//# destination machine in order to send packets using $SendConnectionlessPacket$. The return value
	//# is one of the following network result codes.
	//
	//# \value	kNetworkOkay 			The packet was successfully queued for transmission.
	//# \value	kNetworkPacketTooLarge 	The packet could not be sent because its size exceeds $kMaxMessageSize$.
	//# \value	kNetworkBufferFull 		The packet could not be sent because the outgoing packet buffers
	//#									are full. Try sending the packet again at a later time.
	//
	//# \desc
	//# Delivery of a connectionless packet is not guaranteed, and connectionless packets may be received
	//# in an order different from which they were sent.
	//#
	//# Packet data is encrypted before it is transmitted.
	//
	//# \also	$@NetworkAddress@$
	//# \also	$@NetworkMgr::SendReliablePacket@$
	//# \also	$@NetworkMgr::SendUnreliablePacket@$
	//# \also	$@NetworkMgr::SendUnorderedPacket@$
	//# \also	$@NetworkMgr::BroadcastPacket@$
	//# \also	$@NetworkMgr::ReceivePacket@$


	//# \function	NetworkMgr::BroadcastPacket		Broadcasts a data packet on the LAN.
	//
	//# \note
	//# This is a low-level function that is normally called exclusively by the Message Manager.
	//# Calling this function directly is not recommended if the Message Manager is being used.
	//
	//# \proto	NetworkResult BroadcastPacket(unsigned_int32 size, const void *data);
	//
	//# \param	size	The size of the data packet. This value may not exceed the maximum size given by the constant $kMaxMessageSize$.
	//# \param	data	A pointer to the data to be sent.
	//
	//# \desc
	//# The $BroadcastPacket$ function broadcasts a connectionless data packet to the local area network.
	//# The return value is one of the following network result codes.
	//
	//# \value	kNetworkOkay 			The packet was successfully queued for transmission.
	//# \value	kNetworkPacketTooLarge 	The packet could not be sent because its size exceeds $kMaxMessageSize$.
	//# \value	kNetworkBufferFull 		The packet could not be sent because the outgoing packet buffers
	//#									are full. Try sending the packet again at a later time.
	//
	//# \desc
	//# Delivery of a connectionless packet is not guaranteed, and connectionless packets may be received
	//# in an order different from which they were sent.
	//#
	//# Broadcasted packets are sent to the broadcast port number, which can be set using the
	//# $@NetworkMgr::SetBroadcastPortNumber@$ function.
	//#
	//# Packet data is encrypted before it is transmitted.
	//
	//# \also	$@NetworkAddress@$
	//# \also	$@NetworkMgr::GetBroadcastPortNumber@$
	//# \also	$@NetworkMgr::SetBroadcastPortNumber@$
	//# \also	$@NetworkMgr::SendReliablePacket@$
	//# \also	$@NetworkMgr::SendUnreliablePacket@$
	//# \also	$@NetworkMgr::SendUnorderedPacket@$
	//# \also	$@NetworkMgr::SendConnectionlessPacket@$
	//# \also	$@NetworkMgr::ReceivePacket@$


	//# \function	NetworkMgr::ReceivePacket		Retrieves the next available incoming data packet.
	//
	//# \note
	//# This is a low-level function that is normally called exclusively by the Message Manager.
	//# Calling this function directly is not recommended if the Message Manager is being used.
	//
	//# \proto	NetworkResult ReceivePacket(NetworkAddress *from, unsigned_int32 *size, void *data);
	//
	//# \param	from		A pointer to a $@NetworkAddress@$ structure that will be filled with the
	//#						address of the machine from which the message originated.
	//# \param	size		A pointer to an $unsigned_int32$ that will be filled with the size of the message.
	//# \param	data		A pointer to a buffer that will receive the packet data. This buffer
	//#						must be at least $kMaxMessageSize$ bytes in length.
	//
	//# \desc
	//# The $ReceivePacket$ function retrieves the least recently received incoming data packet from
	//# any network address. The return value is one of the following network result codes.
	//
	//# \value	kNetworkOkay 		A packet was received and has been copied into the buffer pointed
	//#								to by the $data$ parameter.
	//# \value	kNetworkNoPacket 	There is no incoming packet to be received.
	//
	//# \desc
	//# The packet data is decrypted before being copied into the buffer pointed to by the $data$ parameter.
	//# The size that is written to the variable pointed to by the $size$ parameter is always at least
	//# one byte and at most $kMaxMessageSize$ bytes.
	//
	//# \also	$@NetworkAddress@$
	//# \also	$@NetworkMgr::SendReliablePacket@$
	//# \also	$@NetworkMgr::SendUnreliablePacket@$
	//# \also	$@NetworkMgr::SendConnectionlessPacket@$
	//# \also	$@NetworkMgr::BroadcastPacket@$


	//# \div
	//# \function	NetworkMgr::SetNetworkEventProc		Installs a callback function that is called when a network event occurs.
	//
	//# \note
	//# This is a low-level function that is normally called exclusively by the Message Manager.
	//# Calling this function directly is not recommended if the Message Manager is being used.
	//
	//# \proto	void SetNetworkEventProc(NetworkEventProc *proc);
	//
	//# \param	proc	A pointer to a network event procedure. This parameter may be $nullptr$, in
	//#					which case no network event procedure is called when a network event occurs.
	//
	//# \desc
	//# The Network Manager generates a network event when a network connection is created or destroyed.
	//# When a network event occurs, the Network Manager calls the currently installed network event procedure.
	//# The following are the possible network events.
	//
	//# \table	NetworkEvent
	//
	//# The $NetworkEventProc$ type is defined as follows.
	//
	//# \code	typedef void NetworkEventProc(NetworkEvent, const NetworkAddress&, unsigned_int32);
	//
	//# The first parameter passed to this function is set to one of the above network event constants. The second
	//# parameter is the address of the machine to which the network event pertains. The third parameter is currently
	//# used only for the $kNetworkEventFailed$ event and supplies one of the following reasons for the connection failure.
	//
	//# \table	NetworkFail


	//# \div
	//# \function	NetworkMgr::NetworkTask		Called once per application loop to allow the Network Manager
	//#											to perform internal processing.
	//
	//# \proto	void NetworkTask(void);
	//
	//# \desc
	//# The $NetworkTask$ function performs internal processing necessary for reliable packet transport
	//# and asynchronous network operations. This function is called automatically by the engine module.


	class NetworkMgr : public Manager<NetworkMgr>
	{
		friend class NetworkSocket;

		public:

			typedef void NetworkEventProc(NetworkEvent, const NetworkAddress&, unsigned_int32);

		private:

			NetworkSocket				networkSocket;

			int32						maxConnectionCount;

			unsigned_int32				networkProtocol;
			unsigned_int16				localPortNumber;
			unsigned_int16				broadcastPortNumber;
			unsigned_int32				broadcastAddress;

			unsigned_int32				reliableResendTime;
			int32						reliableResendCount;

			NetworkEventProc			*networkEventProc;

			volatile int32				incomingPacketCounter[kPacketTypeCount];
			volatile int32				outgoingPacketCounter[kPacketTypeCount];
			volatile int32				badPacketCounter;

			Mutex						connectionMutex;
			Mutex						incomingMutex;
			Mutex						outgoingMutex;

			Map<NetworkConnection>		connectionMap;
			List<DomainNameResolver>	resolverList;

			List<NetworkPacket>			incomingGeneralPacketPool;
			List<NetworkPacket>			outgoingDataPacketPool;
			List<NetworkPacket>			outgoingControlPacketPool;
			List<NetworkPacket>			activeIncomingPacketList;
			List<NetworkPacket>			activeOutgoingPacketList;

			NetworkPacket				incomingGeneralPacket[kMaxIncomingPacketCount];
			NetworkPacket				outgoingDataPacket[kMaxOutgoingPacketCount];
			NetworkPacket				outgoingControlPacket[kMaxOutgoingPacketCount];

			NetworkConnection *GetConnection(const NetworkAddress& address) const
			{
				return (connectionMap.Find(address));
			}

			NetworkResult SendControlPacket(const NetworkAddress& address, unsigned_int32 number, unsigned_int32 *param = 0);
			void FlushPackets(const NetworkAddress& address);
			void ResetPackets(void);

		public:

			NetworkMgr(int);
			~NetworkMgr();

			EngineResult Construct(void);
			void Destruct(void);

			int32 GetMaxConnectionCount(void) const
			{
				return (maxConnectionCount);
			}

			void SetMaxConnectionCount(int32 max)
			{
				maxConnectionCount = max;
			}

			unsigned_int32 GetProtocol(void) const
			{
				return (networkProtocol);
			}

			void SetProtocol(unsigned_int32 protocol)
			{
				networkProtocol = protocol;
			}

			unsigned_int16 GetPortNumber(void) const
			{
				return (localPortNumber);
			}

			void SetPortNumber(unsigned_int16 port)
			{
				localPortNumber = port;
			}

			unsigned_int16 GetBroadcastPortNumber(void) const
			{
				return (broadcastPortNumber);
			}

			void SetBroadcastPortNumber(unsigned_int16 port)
			{
				broadcastPortNumber = port;
			}

			unsigned_int32 GetReliableResendTime(void) const
			{
				return (reliableResendTime);
			}

			void SetReliableResendTime(unsigned_int32 time)
			{
				reliableResendTime = time;
			}

			int32 GetReliableResendCount(void) const
			{
				return (reliableResendCount);
			}

			void SetReliableResendCount(int32 count)
			{
				reliableResendCount = count;
			}

			void SetNetworkEventProc(NetworkEventProc *proc)
			{
				networkEventProc = proc;
			}

			int32 GetIncomingPacketCounter(int32 index) const
			{
				return (incomingPacketCounter[index]);
			}

			int32 GetOutgoingPacketCounter(int32 index) const
			{
				return (outgoingPacketCounter[index]);
			}

			int32 GetBadPacketCounter(void) const
			{
				return (badPacketCounter);
			}

			const NetworkAddress& GetLocalAddress(void) const
			{
				return (networkSocket.socketAddress);
			}

			DomainNameResolver *GetFirstDomainNameResolver(void) const
			{
				return (resolverList.First());
			}

			C4API NetworkResult Initialize(void);
			C4API void Terminate(void);

			C4API DomainNameResolver *ResolveAddress(const char *name);

			C4API int32 GetPing(const NetworkAddress& address);
			C4API int32 GetPacketCount(const NetworkAddress& address, unsigned_int32 flags = 0);

			C4API void Connect(const NetworkAddress& address);
			C4API void Disconnect(const NetworkAddress& address);

			C4API int32 GetConnectionCount(void);

			C4API NetworkResult SendReliablePacket(const NetworkAddress& to, unsigned_int32 size, const void *data, unsigned_int32 flags = 0);
			C4API NetworkResult SendUnreliablePacket(const NetworkAddress& to, unsigned_int32 size, const void *data);
			C4API NetworkResult SendUnorderedPacket(const NetworkAddress& to, unsigned_int32 size, const void *data);
			C4API NetworkResult SendConnectionlessPacket(const NetworkAddress& to, unsigned_int32 size, const void *data);
			C4API NetworkResult ReceivePacket(NetworkAddress *from, unsigned_int32 *size, void *data);

			NetworkResult BroadcastPacket(unsigned_int32 size, const void *data)
			{
				return (SendConnectionlessPacket(NetworkAddress(broadcastAddress, broadcastPortNumber), size, data));
			}

			C4API void NetworkTask(void);
	};


	const unsigned_int32 kMaxMessageSize = kMaxPacketDataSize - sizeof(PacketNumber);


	C4API extern NetworkMgr *TheNetworkMgr;
}


#endif

// ZYUQURM
