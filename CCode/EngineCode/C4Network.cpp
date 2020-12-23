 

#include "C4Network.h"
#include "C4Engine.h"
#include "C4Random.h"


using namespace C4;


NetworkMgr *C4::TheNetworkMgr = nullptr;


namespace C4
{
	template <> NetworkMgr Manager<NetworkMgr>::managerObject(0);
	template <> NetworkMgr **Manager<NetworkMgr>::managerPointer = &TheNetworkMgr;

	template <> const char *const Manager<NetworkMgr>::resultString[] =
	{
		nullptr,
		"NetworkMgr initialization failed",
		"Network no connection",
		"Network packet too large",
		"Network buffer full",
		"Network no packet available",
		"Network unresolved domain",
		"Network domain resolve pending",
		"Network domain resolve failed",
		"Network domain not found"
	};

	template <> const unsigned_int32 Manager<NetworkMgr>::resultIdentifier[] =
	{
		0, 'INIT', 'CONN', 'PSIZ', 'FULL', 'PACK', 'UNRS', 'RPND', 'RFAL', 'RNFD'
	};

	template class Manager<NetworkMgr>;
}


void NetworkPacket::Encrypt(const void *data, unsigned_int32 size)
{
	const unsigned_int8 *source = static_cast<const unsigned_int8 *>(data);
	unsigned_int8 *destin = packetDataU8 + sizeof(PacketNumber);

	packetSize = size + sizeof(PacketNumber);

	unsigned_int32 number = packetNumber.GetNumber();
	unsigned_int32 r1 = number * 0xBC658A9D;
	unsigned_int32 r2 = number * 0x9DEA7405;
	unsigned_int32 c = ((r1 << 4) & 0xFFFF0000) | (r2 >> 16);

	switch ((r1 >> 28) & 3)
	{
		case 0:
		{
			for (unsigned_machine a = 0; a < size; a++)
			{
				unsigned_int32 x = source[a];
				x = ((x << 1) & 0xFE) | (x >> 7);
				x ^= (c >> (((3 - a) << 3) & 0x18)) & 0xFF;
				destin[a] = (unsigned_int8) x;
			}

			break;
		}

		case 1:
		{
			for (unsigned_machine a = 0; a < size; a++)
			{
				unsigned_int32 x = source[a];
				x = ((x << 3) & 0xF8) | (x >> 5);
				x ^= (c >> (((3 - a) << 3) & 0x18)) & 0xFF;
				destin[a] = (unsigned_int8) x;
			}

			break;
		}

		case 2:
		{
			for (unsigned_machine a = 0; a < size; a++)
			{
				unsigned_int32 x = source[a];
				x = ((x << 5) & 0xE0) | (x >> 3);
				x ^= (c >> (((3 - a) << 3) & 0x18)) & 0xFF;
				destin[a] = (unsigned_int8) x;
			}

			break;
		}

		case 3:
		{
			for (unsigned_machine a = 0; a < size; a++)
			{
				unsigned_int32 x = source[a];
				x = ((x << 7) & 0x80) | (x >> 1);
				x ^= (c >> (((3 - a) << 3) & 0x18)) & 0xFF;
				destin[a] = (unsigned_int8) x;
			} 

			break;
		} 
	}
} 

void NetworkPacket::Decrypt(NetworkPacket *output) const
{ 
	const unsigned_int8 *source = packetDataU8 + sizeof(PacketNumber);
	unsigned_int8 *destin = output->packetDataU8 + sizeof(PacketNumber); 
 
	unsigned_int32 size = packetSize - sizeof(PacketNumber);

	unsigned_int32 number = packetNumber.GetNumber();
	unsigned_int32 r1 = number * 0xBC658A9D; 
	unsigned_int32 r2 = number * 0x9DEA7405;
	unsigned_int32 c = ((r1 << 4) & 0xFFFF0000) | (r2 >> 16);

	switch ((r1 >> 28) & 3)
	{
		case 0:
		{
			for (unsigned_machine a = 0; a < size; a++)
			{
				unsigned_int32 x = source[a];
				x ^= (c >> (((3 - a) << 3) & 0x18)) & 0xFF;
				x = ((x << 7) & 0x80) | (x >> 1);
				destin[a] = (unsigned_int8) x;
			}

			break;
		}

		case 1:
		{
			for (unsigned_machine a = 0; a < size; a++)
			{
				unsigned_int32 x = source[a];
				x ^= (c >> (((3 - a) << 3) & 0x18)) & 0xFF;
				x = ((x << 5) & 0xE0) | (x >> 3);
				destin[a] = (unsigned_int8) x;
			}

			break;
		}

		case 2:
		{
			for (unsigned_machine a = 0; a < size; a++)
			{
				unsigned_int32 x = source[a];
				x ^= (c >> (((3 - a) << 3) & 0x18)) & 0xFF;
				x = ((x << 3) & 0xF8) | (x >> 5);
				destin[a] = (unsigned_int8) x;
			}

			break;
		}

		case 3:
		{
			for (unsigned_machine a = 0; a < size; a++)
			{
				unsigned_int32 x = source[a];
				x ^= (c >> (((3 - a) << 3) & 0x18)) & 0xFF;
				x = ((x << 1) & 0xFE) | (x >> 7);
				destin[a] = (unsigned_int8) x;
			}

			break;
		}
	}

	output->packetAddress = packetAddress;
	output->packetNumber = packetNumber;
	output->packetSize = packetSize;
	output->packetFlags = 0;
}


NetworkConnection::NetworkConnection(const NetworkAddress& address, ConnectionStatus status, unsigned_int32 packetNumber)
{
	connectionAddress = address;
	connectionStatus = status;
	connectionPing = -1;

	initialPacketNumber = packetNumber;
	SetInitialPacketNumbers(packetNumber);

	for (machine a = 0; a < kMaxIncomingPacketCount; a++)
	{
		incomingReliablePacketPool.Append(&incomingReliablePacket[a]);
	}

	for (machine a = 0; a < kMaxIncomingPacketCount; a++)
	{
		incomingUnreliablePacketPool.Append(&incomingUnreliablePacket[a]);
	}

	for (machine a = 0; a < kMaxOutgoingPacketCount; a++)
	{
		outgoingDataPacketPool.Append(&outgoingDataPacket[a]);
	}

	for (machine a = 0; a < kMaxOutgoingPacketCount; a++)
	{
		outgoingControlPacketPool.Append(&outgoingControlPacket[a]);
	}
}

NetworkConnection::~NetworkConnection()
{
}

void NetworkConnection::SetInitialPacketNumbers(unsigned_int32 number)
{
	incomingReliablePacketNumber = kPacketNumberFirstReliable | number;
	incomingUnreliablePacketNumber = kPacketNumberFirstUnreliable | number;
	outgoingReliablePacketNumber = kPacketNumberFirstReliable | number;
	outgoingUnreliablePacketNumber = kPacketNumberFirstUnreliable | number;
}


NetworkSocket::NetworkSocket()
{
	socketThread = nullptr;
}

NetworkSocket::~NetworkSocket()
{
}

NetworkResult NetworkSocket::Open(const NetworkAddress& address)
{
	#if C4WINDOWS

		SOCKADDR_IN		inetAddress;

		socketHandle = WSASocketW(AF_INET, SOCK_DGRAM, IPPROTO_UDP, nullptr, 0, 0);
		if (socketHandle == INVALID_SOCKET)
		{
			return (kNetworkInitFailed);
		}

		MemoryMgr::ClearMemory(&inetAddress, sizeof(SOCKADDR_IN));
		inetAddress.sin_family = AF_INET;
		inetAddress.sin_port = htons(address.GetPort());
		inetAddress.sin_addr.s_addr = htonl(address.GetAddress());

		if (bind(socketHandle, reinterpret_cast<SOCKADDR *>(&inetAddress), sizeof(SOCKADDR_IN)) != 0)
		{
			closesocket(socketHandle);
			return (kNetworkInitFailed);
		}

		socketAddress = address;
		if (address.GetPort() == 0)
		{
			int size = sizeof(SOCKADDR_IN);
			if (getsockname(socketHandle, reinterpret_cast<SOCKADDR *>(&inetAddress), &size) == 0)
			{
				socketAddress.SetPort(ntohs(inetAddress.sin_port));
			}
		}

		DWORD data = true;
		setsockopt(socketHandle, SOL_SOCKET, SO_BROADCAST, reinterpret_cast<char *>(&data), sizeof(DWORD));

		socketEvent[0] = WSACreateEvent();
		socketEvent[1] = WSACreateEvent();

		socketThread = new Thread(&SocketThread, this);
		WSAEventSelect(socketHandle, socketEvent[1], FD_READ | FD_WRITE);

	#elif C4POSIX

		sockaddr_in		inetAddress;

		socketDesc = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
		if (socketDesc == -1)
		{
			return (kNetworkInitFailed);
		}

		MemoryMgr::ClearMemory(&inetAddress, sizeof(sockaddr_in));

		#if !C4LINUX

			inetAddress.sin_len = sizeof(sockaddr_in);

		#endif

		inetAddress.sin_family = AF_INET;
		inetAddress.sin_port = htons(address.GetPort());
		inetAddress.sin_addr.s_addr = htonl(address.GetAddress());

		if (bind(socketDesc, reinterpret_cast<sockaddr *>(&inetAddress), sizeof(sockaddr_in)) != 0)
		{
			close(socketDesc);
			return (kNetworkInitFailed);
		}

		socketAddress = address;
		if (address.GetPort() == 0)
		{
			socklen_t size = sizeof(sockaddr_in);
			if (getsockname(socketDesc, reinterpret_cast<sockaddr *>(&inetAddress), &size) == 0)
			{
				socketAddress.SetPort(ntohs(inetAddress.sin_port));
			}
		}

		int broadcast = true;
		setsockopt(socketDesc, SOL_SOCKET, SO_BROADCAST, &broadcast, sizeof(int));

		int nonblocking = true;
		ioctl(socketDesc, FIONBIO, &nonblocking);

		(void) pipe(pipeDesc);
		socketThread = new Thread(&SocketThread, this);

	#elif C4PS4 //[ PS4

		// -- PS4 code hidden --

	#elif C4PS3 //[ PS3

		// -- PS3 code hidden --

	#endif //]

	return (kNetworkOkay);
}

void NetworkSocket::Close(void)
{
	if (Open())
	{
		#if C4WINDOWS

			WSAEventSelect(socketHandle, socketEvent[1], 0);
			WSASetEvent(socketEvent[0]);

		#elif C4POSIX

			char data = 0;
			(void) write(pipeDesc[1], &data, 1);

		#elif C4PS4 //[ PS4

			// -- PS4 code hidden --

		#endif //]

		delete socketThread;
		socketThread = nullptr;

		#if C4WINDOWS

			WSACloseEvent(socketEvent[1]);
			WSACloseEvent(socketEvent[0]);
			closesocket(socketHandle);

		#elif C4POSIX

			close(pipeDesc[1]);
			close(pipeDesc[0]);
			close(socketDesc);

		#elif C4PS4 //[ PS4

			// -- PS4 code hidden --

		#elif C4PS3 //[ PS3

			// -- PS3 code hidden --

		#endif //]

		socketAddress.Set(0);
	}
}

void NetworkSocket::SocketThread(const Thread *thread, void *cookie)
{
	Thread::SetThreadName("C4-NW Socket");

	NetworkSocket *networkSocket = static_cast<NetworkSocket *>(cookie);

	#if C4WINDOWS

		for (;;)
		{
			WSANETWORKEVENTS	events;

			DWORD d = WSAWaitForMultipleEvents(2, networkSocket->socketEvent, false, WSA_INFINITE, false);
			if (d == WSA_WAIT_EVENT_0)
			{
				break;
			}

			if (WSAEnumNetworkEvents(networkSocket->socketHandle, networkSocket->socketEvent[1], &events) == 0)
			{
				if (events.lNetworkEvents & FD_READ)
				{
					networkSocket->ReceivePackets();
				}

				if (events.lNetworkEvents & FD_WRITE)
				{
					networkSocket->SendPackets();
				}
			}
		}

	#elif C4POSIX

		fd_set	readSet;
		fd_set	writeSet;

		networkSocket->sendBlocked = false;

		FD_ZERO(&readSet);
		FD_ZERO(&writeSet);

		int socketDesc = networkSocket->socketDesc;
		int pipeDesc = networkSocket->pipeDesc[0];

		int num = Max(socketDesc, pipeDesc) + 1;
		for (;;)
		{
			FD_SET(socketDesc, &readSet);
			FD_SET(pipeDesc, &readSet);

			fd_set *ws = nullptr;
			if (networkSocket->sendBlocked)
			{
				networkSocket->sendBlocked = false;
				FD_SET(socketDesc, &writeSet);
				ws = &writeSet;
			}

			select(num, &readSet, ws, nullptr, nullptr);

			if (FD_ISSET(pipeDesc, &readSet))
			{
				break;
			}

			if (FD_ISSET(socketDesc, &readSet))
			{
				networkSocket->ReceivePackets();
			}

			if ((ws) && (FD_ISSET(socketDesc, ws)))
			{
				networkSocket->SendPackets();
			}
		}

	#elif C4PS4 //[ PS4

		// -- PS4 code hidden --

	#elif C4PS3 //[ PS3

		// -- PS3 code hidden --

	#endif //]
}

bool NetworkSocket::Send(NetworkPacket *packet)
{
	#if C4WINDOWS

		WSABUF			dataBuffer;
		SOCKADDR_IN		destAddress;
		DWORD			count;

		destAddress.sin_family = AF_INET;
		destAddress.sin_port = htons(packet->packetAddress.GetPort());
		destAddress.sin_addr.s_addr = htonl(packet->packetAddress.GetAddress());

		dataBuffer.len = packet->packetSize;
		dataBuffer.buf = packet->packetDataS8;

		WriteBigEndianU32(packet->packetDataU32, packet->packetNumber.GetNumber());
		return (WSASendTo(socketHandle, &dataBuffer, 1, &count, 0, reinterpret_cast<SOCKADDR *>(&destAddress), sizeof(SOCKADDR_IN), nullptr, nullptr) == 0);

	#elif C4POSIX

		sockaddr_in		destAddress;

		#if !C4LINUX

			destAddress.sin_len = sizeof(sockaddr_in);

		#endif

		destAddress.sin_family = AF_INET;
		destAddress.sin_port = htons(packet->packetAddress.GetPort());
		destAddress.sin_addr.s_addr = htonl(packet->packetAddress.GetAddress());

		WriteBigEndianU32(packet->packetDataU32, packet->packetNumber.GetNumber());
		bool result = (sendto(socketDesc, packet->packetDataU8, packet->packetSize, 0, reinterpret_cast<sockaddr *>(&destAddress), sizeof(sockaddr_in)) >= 0);
		if (!result)
		{
			int error = errno;
			if ((error == EWOULDBLOCK) || (error == ENOBUFS))
			{
				sendBlocked = true;
			}
		}

		return (result);

	#elif C4PS4 //[ PS4

		// -- PS4 code hidden --

	#elif C4PS3 //[ PS3

		// -- PS3 code hidden --

	#endif //]
}

bool NetworkSocket::Receive(NetworkPacket *packet)
{
	packet->packetDataU32[0] = 0xFFFFFFFF;

	#if C4WINDOWS

		for (;;)
		{
			WSABUF			dataBuffer;
			SOCKADDR_IN		sourceAddress;
			DWORD			dataSize;

			sourceAddress.sin_family = AF_INET;
			sourceAddress.sin_port = 0;
			sourceAddress.sin_addr.s_addr = INADDR_ANY;

			dataBuffer.len = kMaxPacketDataSize;
			dataBuffer.buf = packet->packetDataS8;

			DWORD flags = 0;
			INT sourceLen = sizeof(SOCKADDR_IN);
			if (WSARecvFrom(socketHandle, &dataBuffer, 1, &dataSize, &flags, reinterpret_cast<SOCKADDR *>(&sourceAddress), &sourceLen, nullptr, nullptr) != 0)
			{
				if (WSAGetLastError() == WSAEMSGSIZE)
				{
					continue;
				}

				return (false);
			}

			packet->packetAddress.Set(ntohl(sourceAddress.sin_addr.s_addr), ntohs(sourceAddress.sin_port));
			packet->packetNumber = ReadBigEndianU32(packet->packetDataU32);
			packet->packetSize = dataSize;
			break;
		}

	#elif C4POSIX

		sockaddr_in		sourceAddress;

		socklen_t sourceLen = sizeof(sockaddr_in);
		int dataSize = recvfrom(socketDesc, packet->packetDataU8, kMaxPacketDataSize, 0, reinterpret_cast<sockaddr *>(&sourceAddress), &sourceLen);
		if (dataSize < 0)
		{
			return (false);
		}

		packet->packetAddress.Set(ntohl(sourceAddress.sin_addr.s_addr), ntohs(sourceAddress.sin_port));
		packet->packetNumber = ReadBigEndianU32(packet->packetDataU32);
		packet->packetSize = dataSize;

	#elif C4PS4 //[ PS4

		// -- PS4 code hidden --

	#elif C4PS3 //[ PS3

		// -- PS3 code hidden --

	#endif //]

	return (true);
}

void NetworkSocket::SendPackets(void)
{
	TheNetworkMgr->outgoingMutex.Acquire();

	for (;;)
	{
		NetworkPacket *packet = TheNetworkMgr->activeOutgoingPacketList.First();
		if (!packet)
		{
			break;
		}

		if (Send(packet))
		{
			TheNetworkMgr->outgoingPacketCounter[packet->GetPacketType()]++;
			packet->packetPool->Append(packet);
		}
		else
		{
			if (packet->GetPacketType() == kPacketAcknowledge)
			{
				packet->packetPool->Append(packet);
			}

			break;
		}
	}

	unsigned_int32 time = TheTimeMgr->GetSystemAbsoluteTime();

	NetworkConnection *connection = TheNetworkMgr->connectionMap.First();
	while (connection)
	{
		for (;;)
		{
			NetworkPacket *packet = connection->activeOutgoingPacketList.First();
			if (!packet)
			{
				break;
			}

			if (Send(packet))
			{
				TheNetworkMgr->outgoingPacketCounter[packet->GetPacketType()]++;

				if (packet->GetPacketType() == kPacketReliable)
				{
					packet->sentTime = time;
					connection->pendingAcknowledgePacketList.Append(packet);
				}
				else
				{
					packet->packetPool->Append(packet);
				}
			}
			else
			{
				if (packet->GetPacketType() >= kPacketUnreliable)
				{
					packet->packetPool->Append(packet);
				}

				goto exit;
			}
		}

		connection = connection->Next();
	}

	exit:
	TheNetworkMgr->outgoingMutex.Release();
}

void NetworkSocket::ReceivePackets(void)
{
	NetworkPacket	rawPacket;

	TheNetworkMgr->connectionMutex.Acquire();
	TheNetworkMgr->incomingMutex.Acquire();
	TheNetworkMgr->outgoingMutex.Acquire();

	bool send = false;
	unsigned_int32 time = TheTimeMgr->GetSystemAbsoluteTime();
	for (;;)
	{
		if (!Receive(&rawPacket))
		{
			break;
		}

		switch (rawPacket.GetPacketType())
		{
			case kPacketControl:
			{
				NetworkPacket *packet = TheNetworkMgr->incomingGeneralPacketPool.First();
				if (packet)
				{
					rawPacket.Decrypt(packet);
					packet->packetPool = &TheNetworkMgr->incomingGeneralPacketPool;
					TheNetworkMgr->activeIncomingPacketList.Append(packet);
					TheNetworkMgr->incomingPacketCounter[kPacketControl]++;
				}

				break;
			}

			case kPacketAcknowledge:
			{
				NetworkConnection *connection = TheNetworkMgr->GetConnection(rawPacket.packetAddress);
				if (connection)
				{
					unsigned_int32 n = rawPacket.packetNumber.GetNumber() & kPacketNumberRangeMask;
					NetworkPacket *sentPacket = connection->pendingAcknowledgePacketList.First();
					while (sentPacket)
					{
						if ((sentPacket->packetAddress == rawPacket.packetAddress) && ((sentPacket->packetNumber.GetNumber() & kPacketNumberRangeMask) == n))
						{
							connection->connectionPing = time - sentPacket->sentTime;
							connection->outgoingDataPacketPool.Append(sentPacket);
							break;
						}

						sentPacket = sentPacket->Next();
					}

					TheNetworkMgr->incomingPacketCounter[kPacketAcknowledge]++;
				}

				break;
			}

			case kPacketReliable:

				if (rawPacket.packetSize > 4)
				{
					NetworkConnection *connection = TheNetworkMgr->GetConnection(rawPacket.packetAddress);
					if ((connection) && (connection->GetStatus() == kConnectionOpen))
					{
						NetworkPacket *packet = connection->incomingReliablePacketPool.First();
						if (packet)
						{
							int32 delta = rawPacket.packetNumber - connection->incomingReliablePacketNumber;
							if (delta >= 0)
							{
								if (delta >= kMaxIncomingPacketCount)
								{
									break;
								}

								rawPacket.Decrypt(packet);
								packet->packetPool = &connection->incomingReliablePacketPool;
								TheNetworkMgr->activeIncomingPacketList.Append(packet);
								TheNetworkMgr->incomingPacketCounter[kPacketReliable]++;
							}

							NetworkPacket *acknowledgePacket = connection->outgoingControlPacketPool.First();
							if (acknowledgePacket)
							{
								acknowledgePacket->packetAddress = rawPacket.packetAddress;
								acknowledgePacket->packetNumber = (rawPacket.packetNumber.GetNumber() & kPacketNumberRangeMask) | kPacketNumberFirstAcknowledge;
								acknowledgePacket->packetSize = sizeof(PacketNumber);
								acknowledgePacket->packetPool = &connection->outgoingControlPacketPool;

								connection->activeOutgoingPacketList.Append(acknowledgePacket);
								send = true;
							}
						}
					}
				}
				else
				{
					TheNetworkMgr->badPacketCounter++;
				}

				break;

			case kPacketUnreliable:

				if (rawPacket.packetSize > 4)
				{
					NetworkConnection *connection = TheNetworkMgr->GetConnection(rawPacket.packetAddress);
					if ((connection) && (connection->GetStatus() == kConnectionOpen))
					{
						NetworkPacket *packet = connection->incomingUnreliablePacketPool.First();
						if (packet)
						{
							if (rawPacket.packetNumber >= connection->incomingUnreliablePacketNumber)
							{
								rawPacket.Decrypt(packet);
								packet->packetPool = &connection->incomingUnreliablePacketPool;
								TheNetworkMgr->activeIncomingPacketList.Append(packet);
								TheNetworkMgr->incomingPacketCounter[kPacketUnreliable]++;
							}
						}
					}
				}
				else
				{
					TheNetworkMgr->badPacketCounter++;
				}

				break;

			case kPacketUnordered:

				if (rawPacket.packetSize > 4)
				{
					NetworkConnection *connection = TheNetworkMgr->GetConnection(rawPacket.packetAddress);
					if ((connection) && (connection->GetStatus() == kConnectionOpen))
					{
						NetworkPacket *packet = connection->incomingUnreliablePacketPool.First();
						if (packet)
						{
							rawPacket.Decrypt(packet);
							packet->packetPool = &connection->incomingUnreliablePacketPool;
							TheNetworkMgr->activeIncomingPacketList.Append(packet);
							TheNetworkMgr->incomingPacketCounter[kPacketUnordered]++;
						}
					}
				}
				else
				{
					TheNetworkMgr->badPacketCounter++;
				}

				break;

			case kPacketConnectionless:

				if (rawPacket.packetAddress != socketAddress)
				{
					if ((rawPacket.packetSize > 4) && (rawPacket.packetNumber.GetNumber() == kPacketNumberConnectionless))
					{
						NetworkPacket *packet = TheNetworkMgr->incomingGeneralPacketPool.First();
						if (packet)
						{
							rawPacket.Decrypt(packet);
							packet->packetPool = &TheNetworkMgr->incomingGeneralPacketPool;
							TheNetworkMgr->activeIncomingPacketList.Append(packet);
							TheNetworkMgr->incomingPacketCounter[kPacketConnectionless]++;
						}
					}
					else
					{
						TheNetworkMgr->badPacketCounter++;
					}
				}

				break;

			default:

				TheNetworkMgr->badPacketCounter++;
				break;
		}
	}

	TheNetworkMgr->outgoingMutex.Release();
	TheNetworkMgr->incomingMutex.Release();
	TheNetworkMgr->connectionMutex.Release();

	if (send)
	{
		SendPackets();
	}
}


DomainNameResolver::DomainNameResolver()
{
	resolveResult = kNetworkResolveUnresolved;

	#if C4WINDOWS

		resolveHandle = INVALID_HANDLE_VALUE;

	#elif C4MACOS || C4IOS

		resolveHost = nullptr;

		resolveContext.version = 0;
		resolveContext.info = this;
		resolveContext.retain = nullptr;
		resolveContext.release = nullptr;
		resolveContext.copyDescription = nullptr;

	#elif C4POSIX

		resolveFlag = false;

	#elif C4PS4 //[ PS4

		// -- PS4 code hidden --

	#endif //]
}

DomainNameResolver::~DomainNameResolver()
{
	#if C4WINDOWS

		if ((resolveHandle != INVALID_HANDLE_VALUE) && (resolveResult == kNetworkResolvePending))
		{
			WSACancelAsyncRequest(resolveHandle);
		}

	#elif C4MACOS || C4IOS

		if (resolveHost != nullptr)
		{
			CFHostSetClient(resolveHost, nullptr, nullptr);
			CFHostUnscheduleFromRunLoop(resolveHost, CFRunLoopGetCurrent(), kCFRunLoopCommonModes);

			if (resolveResult == kNetworkResolvePending)
			{
				CFHostCancelInfoResolution(resolveHost, kCFHostAddresses);
			}

			CFRelease(resolveHost);
		}

	#elif C4POSIX

		if (resolveFlag)
		{
			if (resolveResult == kNetworkResolvePending)
			{
				pthread_cancel(resolveThread);
			}

			pthread_join(resolveThread, nullptr);

			addrinfo *info = resolveAddress;
			if (info)
			{
				freeaddrinfo(info);
			}
		}

	#elif C4PS4 //[ PS4

		// -- PS4 code hidden --

	#endif //]
}

String<kMaxHostNameLength> DomainNameResolver::GetName(void) const
{
	String<kMaxHostNameLength> name("");

	#if C4WINDOWS

		if (resolveResult == kNetworkOkay)
		{
			name = hostInfo.h_name;
		}

	#elif C4MACOS || C4IOS

		if (resolveResult == kNetworkOkay)
		{
			CFArrayRef array = CFHostGetNames(resolveHost, nullptr);
			if (array)
			{
				CFStringRef string = (CFStringRef) CFArrayGetValueAtIndex(array, 0);
				CFStringGetCString(string, name, kMaxHostNameLength, kCFStringEncodingASCII);
			}
		}

	#elif C4POSIX

		if (resolveResult == kNetworkOkay)
		{
			name = resolveAddress->ai_canonname;
		}

	#endif

	return (name);
}

NetworkAddress DomainNameResolver::GetAddress(void) const
{
	#if C4WINDOWS

		if (resolveResult == kNetworkOkay)
		{
			return (NetworkAddress(ntohl(*reinterpret_cast<unsigned_int32 *>(hostInfo.h_addr_list[0]))));
		}

	#elif C4MACOS || C4IOS

		if (resolveResult == kNetworkOkay)
		{
			CFArrayRef array = CFHostGetAddressing(resolveHost, nullptr);
			if (array)
			{
				CFIndex count = CFArrayGetCount(array);
				for (CFIndex index = 0; index < count; index++)
				{
					const sockaddr *address = reinterpret_cast<const sockaddr *>(CFDataGetBytePtr((CFDataRef) CFArrayGetValueAtIndex(array, index)));
					if (address->sa_family == AF_INET)
					{
						const sockaddr_in *inetAddress = reinterpret_cast<const sockaddr_in *>(address);
						return (NetworkAddress(ntohl(inetAddress->sin_addr.s_addr)));
					}
				}
			}
		}

	#elif C4POSIX

		if (resolveResult == kNetworkOkay)
		{
			const sockaddr *address = resolveAddress->ai_addr;
			if (address->sa_family == AF_INET)
			{
				const sockaddr_in *inetAddress = reinterpret_cast<const sockaddr_in *>(address);
				return (NetworkAddress(ntohl(inetAddress->sin_addr.s_addr)));
			}
		}

	#elif C4PS4 //[ PS4

		// -- PS4 code hidden --

	#endif //]

	return (NetworkAddress(0));
}

#if C4WINDOWS

	void DomainNameResolver::ResolveCallback(HANDLE handle, int error)
	{
		DomainNameResolver *resolver = TheNetworkMgr->GetFirstDomainNameResolver();
		while (resolver)
		{
			if (resolver->resolveHandle == handle)
			{
				if (error)
				{
					resolver->resolveResult = kNetworkResolveNotFound;
				}
				else
				{
					resolver->resolveResult = kNetworkOkay;
				}

				resolver->resolveHandle = INVALID_HANDLE_VALUE;
				break;
			}

			resolver = resolver->Next();
		}
	}

#elif C4MACOS || C4IOS

	void DomainNameResolver::ResolveCallback(CFHostRef host, CFHostInfoType type, const CFStreamError *error, void *cookie)
	{
		DomainNameResolver *resolver = static_cast<DomainNameResolver *>(cookie);
		if (error)
		{
			resolver->resolveResult = kNetworkResolveNotFound;
		}

		resolver->resolveResult = kNetworkOkay;
	}

#elif C4POSIX

	void *DomainNameResolver::ResolveThread(void *cookie)
	{
		addrinfo	info;

		DomainNameResolver *resolver = static_cast<DomainNameResolver *>(cookie);

		info.ai_flags = AI_CANONNAME;
		info.ai_family = AF_INET;
		info.ai_socktype = SOCK_DGRAM;
		info.ai_protocol = IPPROTO_UDP;

		resolver->resolveAddress = nullptr;

		if (getaddrinfo(resolver->resolveName, nullptr, &info, &resolver->resolveAddress) == 0)
		{
			resolver->resolveResult = kNetworkOkay;
		}
		else
		{
			resolver->resolveResult = kNetworkResolveNotFound;
		}

		return (nullptr);
	}

#elif C4PS4 //[ PS4

	// -- PS4 code hidden --

#endif //]

void DomainNameResolver::ResolveAddress(const char *name)
{
	#if C4WINDOWS

		resolveHandle = WSAAsyncGetHostByName(TheEngine->GetEngineWindow(), kWindowsMessageResolve, name, hostData, MAXGETHOSTSTRUCT);
		if (resolveHandle)
		{
			resolveResult = kNetworkResolvePending;
		}
		else
		{
			resolveResult = kNetworkResolveFailed;
		}

	#elif C4MACOS || C4IOS

		CFStringRef string = CFStringCreateWithCStringNoCopy(kCFAllocatorDefault, name, kCFStringEncodingASCII, kCFAllocatorNull);
		resolveHost = CFHostCreateWithName(kCFAllocatorDefault, string);
		CFRelease(string);

		NetworkResult result = kNetworkResolveFailed;

		if (resolveHost != nullptr)
		{
			if (CFHostSetClient(resolveHost, &ResolveCallback, &resolveContext))
			{
				CFHostScheduleWithRunLoop(resolveHost, CFRunLoopGetCurrent(), kCFRunLoopCommonModes);
				if (CFHostStartInfoResolution(resolveHost, kCFHostAddresses, nullptr))
				{
					result = kNetworkResolvePending;
				}
			}
		}

		resolveResult = result;

	#elif C4POSIX

		resolveResult = kNetworkResolvePending;
		resolveFlag = true;
		resolveName = name;

		pthread_create(&resolveThread, nullptr, &ResolveThread, this);

	#elif C4PS4 //[ PS4

		// -- PS4 code hidden --

	#endif //]
}


NetworkMgr::NetworkMgr(int)
{
}

NetworkMgr::~NetworkMgr()
{
}

EngineResult NetworkMgr::Construct(void)
{
	maxConnectionCount = 8;

	networkProtocol = 0x00000001;
	localPortNumber = 0;
	broadcastPortNumber = 0;

	reliableResendTime = 300;
	reliableResendCount = 15;

	networkEventProc = nullptr;

	ResetPackets();

	#if C4PS4 //[ PS4

		// -- PS4 code hidden --

	#elif C4PS3 //[ PS3

		// -- PS3 code hidden --

	#endif //]

	return (kEngineOkay);
}

void NetworkMgr::Destruct(void)
{
	Terminate();

	#if C4PS4 //[ PS4

		// -- PS4 code hidden --

	#elif C4PS3 //[ PS3

		// -- PS3 code hidden --

	#endif //]
}

NetworkResult NetworkMgr::Initialize(void)
{
	if (networkSocket.Open())
	{
		return (kNetworkOkay);
	}

	#if C4WINDOWS

		WSADATA		data;

		if (WSAStartup(MAKEWORD(2, 0), &data) == 0)
		{
			SOCKADDR_IN		broadcast;
			DWORD			size;
			char			name[256];

			gethostname(name, 255);
			const HOSTENT *hostInfo = gethostbyname(name);
			if (hostInfo)
			{
				NetworkAddress localAddress(ntohl(*(unsigned_int32 *) hostInfo->h_addr_list[0]), localPortNumber);
				if (networkSocket.Open(localAddress) == kNetworkOkay)
				{
					if (WSAIoctl(networkSocket.socketHandle, SIO_GET_BROADCAST_ADDRESS, nullptr, 0, &broadcast, sizeof(SOCKADDR_IN), &size, nullptr, nullptr) == 0)
					{
						broadcastAddress = ntohl(broadcast.sin_addr.s_addr);
					}
					else
					{
						broadcastAddress = INADDR_BROADCAST;
					}

					return (kNetworkOkay);
				}
			}

			WSACleanup();
		}

		return (kNetworkInitFailed);

	#elif C4POSIX

		ifaddrs		*addressList;
		in_addr_t	inetAddress;
		in_addr_t	subnetMask;

		if (getifaddrs(&addressList) != 0)
		{
			return (kNetworkInitFailed);
		}

		const ifaddrs *address = addressList;
		while (address)
		{
			u_int flags = address->ifa_flags;
			if ((flags & (IFF_UP | IFF_LOOPBACK)) == IFF_UP)
			{
				const sockaddr *addr = address->ifa_addr;
				if ((addr) && (addr->sa_family == AF_INET))
				{
					inetAddress = ntohl(reinterpret_cast<const sockaddr_in *>(addr)->sin_addr.s_addr);

					const sockaddr *mask = address->ifa_netmask;
					subnetMask = (mask) ? ntohl(reinterpret_cast<const sockaddr_in *>(mask)->sin_addr.s_addr) : 0;
					break;
				}
			}

			address = address->ifa_next;
		}

		freeifaddrs(addressList);

		if (!address)
		{
			return (kNetworkInitFailed);
		}

		broadcastAddress = (inetAddress & subnetMask) | ~subnetMask;

		NetworkAddress localAddress(inetAddress, localPortNumber);
		if (networkSocket.Open(localAddress) != kNetworkOkay)
		{
			return (kNetworkInitFailed);
		}

		return (kNetworkOkay);

	#elif C4PS4 //[ PS4

		// -- PS4 code hidden --

	#elif C4PS3 //[ PS3

		// -- PS3 code hidden --

	#endif //]
}

void NetworkMgr::Terminate(void)
{
	if (networkSocket.Open())
	{
		resolverList.Purge();

		NetworkTask();
		networkSocket.Close();

		#if C4WINDOWS

			WSACleanup();

		#elif C4PS3 //[ PS3

			// -- PS3 code hidden --

		#endif //]

		connectionMap.Purge();
		ResetPackets();
	}
}

DomainNameResolver *NetworkMgr::ResolveAddress(const char *name)
{
	DomainNameResolver *resolver = new DomainNameResolver;
	resolver->ResolveAddress(name);

	if (resolver->GetResolveResult() != kNetworkResolveFailed)
	{
		resolverList.Append(resolver);
		return (resolver);
	}

	delete resolver;
	return (nullptr);
}

int32 NetworkMgr::GetPing(const NetworkAddress& address)
{
	connectionMutex.Acquire();

	NetworkConnection *connection = GetConnection(address);
	int32 ping = (connection) ? connection->GetPing() : -1;

	connectionMutex.Release();
	return (ping);
}

int32 NetworkMgr::GetPacketCount(const NetworkAddress& address, unsigned_int32 flags)
{
	const NetworkConnection *connection = GetConnection(address);
	if (connection)
	{
		int32 count = 0;
		outgoingMutex.Acquire();

		NetworkPacket *packet = connection->activeOutgoingPacketList.First();
		while (packet)
		{
			if ((packet->packetAddress == address) && ((packet->packetFlags & flags) == flags))
			{
				count++;
			}

			packet = packet->Next();
		}

		packet = connection->pendingAcknowledgePacketList.First();
		while (packet)
		{
			if ((packet->packetAddress == address) && ((packet->packetFlags & flags) == flags))
			{
				count++;
			}

			packet = packet->Next();
		}

		outgoingMutex.Release();
		return (count);
	}

	return (0);
}

void NetworkMgr::Connect(const NetworkAddress& address)
{
	connectionMutex.Acquire();

	NetworkConnection *connection = GetConnection(address);
	if (!connection)
	{
		connection = new NetworkConnection(address, kConnectionRequest);
		connection->controlSendTime = TheTimeMgr->GetSystemAbsoluteTime() - reliableResendTime;
		connection->controlSendCount = 0;
		connectionMap.Insert(connection);
	}

	connectionMutex.Release();
}

void NetworkMgr::Disconnect(const NetworkAddress& address)
{
	connectionMutex.Acquire();

	NetworkConnection *connection = GetConnection(address);
	if (connection)
	{
		connection->SetStatus(kConnectionDisconnect);
		connection->controlSendTime = TheTimeMgr->GetSystemAbsoluteTime() - reliableResendTime;
		connection->controlSendCount = 0;
	}

	connectionMutex.Release();
}

int32 NetworkMgr::GetConnectionCount(void)
{
	connectionMutex.Acquire();
	int32 count = connectionMap.GetElementCount();
	connectionMutex.Release();
	return (count);
}

NetworkResult NetworkMgr::SendControlPacket(const NetworkAddress& address, unsigned_int32 number, unsigned_int32 *param)
{
	NetworkResult result = kNetworkOkay;
	bool send = false;

	outgoingMutex.Acquire();

	NetworkPacket *packet = outgoingControlPacketPool.First();
	if (packet)
	{
		packet->packetAddress = address;
		packet->packetNumber = number;
		packet->packetSize = sizeof(PacketNumber);
		packet->packetFlags = 0;
		packet->packetPool = &outgoingControlPacketPool;

		if (param)
		{
			unsigned_int32	data;

			WriteBigEndianU32(&data, *param);
			packet->Encrypt(&data, 4);
		}

		activeOutgoingPacketList.Append(packet);
		send = true;
	}
	else
	{
		result = kNetworkBufferFull;
	}

	outgoingMutex.Release();

	if (send)
	{
		networkSocket.SendPackets();
	}

	return (result);
}

NetworkResult NetworkMgr::SendReliablePacket(const NetworkAddress& to, unsigned_int32 size, const void *data, unsigned_int32 flags)
{
	NetworkResult result = kNetworkOkay;
	bool send = false;

	connectionMutex.Acquire();

	NetworkConnection *connection = GetConnection(to);
	if (connection)
	{
		if (size <= kMaxMessageSize)
		{
			outgoingMutex.Acquire();

			NetworkPacket *packet = connection->outgoingDataPacketPool.First();
			if (packet)
			{
				packet->packetAddress = to;
				packet->packetNumber = connection->outgoingReliablePacketNumber++;
				packet->packetFlags = flags;
				packet->packetPool = &connection->outgoingDataPacketPool;

				packet->resendTime = reliableResendTime;
				packet->resendCount = 0;

				packet->Encrypt(data, size);

				connection->activeOutgoingPacketList.Append(packet);
				send = true;
			}
			else
			{
				result = kNetworkBufferFull;
			}

			outgoingMutex.Release();
		}
		else
		{
			result = kNetworkPacketTooLarge;
		}
	}
	else
	{
		result = kNetworkNoConnection;
	}

	connectionMutex.Release();

	if (send)
	{
		networkSocket.SendPackets();
	}

	return (result);
}

NetworkResult NetworkMgr::SendUnreliablePacket(const NetworkAddress& to, unsigned_int32 size, const void *data)
{
	NetworkResult result = kNetworkOkay;
	bool send = false;

	connectionMutex.Acquire();

	NetworkConnection *connection = GetConnection(to);
	if (connection)
	{
		if (size <= kMaxMessageSize)
		{
			outgoingMutex.Acquire();

			NetworkPacket *packet = connection->outgoingDataPacketPool.First();
			if (packet)
			{
				packet->packetAddress = to;
				packet->packetNumber = connection->outgoingUnreliablePacketNumber++;
				packet->packetFlags = 0;
				packet->packetPool = &connection->outgoingDataPacketPool;

				packet->Encrypt(data, size);

				connection->activeOutgoingPacketList.Append(packet);
				send = true;
			}
			else
			{
				result = kNetworkBufferFull;
			}

			outgoingMutex.Release();
		}
		else
		{
			result = kNetworkPacketTooLarge;
		}
	}
	else
	{
		result = kNetworkNoConnection;
	}

	connectionMutex.Release();

	if (send)
	{
		networkSocket.SendPackets();
	}

	return (result);
}

NetworkResult NetworkMgr::SendUnorderedPacket(const NetworkAddress& to, unsigned_int32 size, const void *data)
{
	NetworkResult result = kNetworkOkay;
	bool send = false;

	connectionMutex.Acquire();

	NetworkConnection *connection = GetConnection(to);
	if (connection)
	{
		if (size <= kMaxMessageSize)
		{
			outgoingMutex.Acquire();

			NetworkPacket *packet = connection->outgoingDataPacketPool.First();
			if (packet)
			{
				packet->packetAddress = to;
				packet->packetNumber = kPacketNumberUnordered;
				packet->packetFlags = 0;
				packet->packetPool = &connection->outgoingDataPacketPool;

				packet->Encrypt(data, size);

				connection->activeOutgoingPacketList.Append(packet);
				send = true;
			}
			else
			{
				result = kNetworkBufferFull;
			}

			outgoingMutex.Release();
		}
		else
		{
			result = kNetworkPacketTooLarge;
		}
	}
	else
	{
		result = kNetworkNoConnection;
	}

	connectionMutex.Release();

	if (send)
	{
		networkSocket.SendPackets();
	}

	return (result);
}

NetworkResult NetworkMgr::SendConnectionlessPacket(const NetworkAddress& to, unsigned_int32 size, const void *data)
{
	NetworkResult result = kNetworkOkay;
	bool send = false;

	if (size <= kMaxMessageSize)
	{
		outgoingMutex.Acquire();

		NetworkPacket *packet = outgoingDataPacketPool.First();
		if (packet)
		{
			packet->packetAddress = to;
			packet->packetNumber = kPacketNumberConnectionless;
			packet->packetFlags = 0;
			packet->packetPool = &outgoingDataPacketPool;

			packet->Encrypt(data, size);

			activeOutgoingPacketList.Append(packet);
			send = true;
		}
		else
		{
			result = kNetworkBufferFull;
		}

		outgoingMutex.Release();
	}
	else
	{
		result = kNetworkPacketTooLarge;
	}

	if (send)
	{
		networkSocket.SendPackets();
	}

	return (result);
}

NetworkResult NetworkMgr::ReceivePacket(NetworkAddress *from, unsigned_int32 *size, void *data)
{
	NetworkResult result = kNetworkNoPacket;

	connectionMutex.Acquire();
	incomingMutex.Acquire();

	NetworkPacket *packet = activeIncomingPacketList.First();
	while (packet)
	{
		NetworkPacket *next = packet->Next();

		PacketType type = packet->GetPacketType();
		bool good = (type >= kPacketUnordered);
		if (!good)
		{
			NetworkConnection *connection = GetConnection(packet->packetAddress);
			if (connection)
			{
				if (type == kPacketReliable)
				{
					PacketNumber number = connection->incomingReliablePacketNumber;
					if (packet->packetNumber == number)
					{
						good = true;
						connection->incomingReliablePacketNumber = number + 1;
					}
					else if (packet->packetNumber > number)
					{
						goto loop;
					}
				}
				else if (type == kPacketUnreliable)
				{
					PacketNumber number = connection->incomingUnreliablePacketNumber;
					if (packet->packetNumber >= number)
					{
						good = true;
						connection->incomingUnreliablePacketNumber = number + 1;
					}
				}
			}
		}

		packet->packetPool->Append(packet);

		if (good)
		{
			unsigned_int32 s = packet->packetSize - sizeof(PacketNumber);
			MemoryMgr::CopyMemory(packet->packetDataU8 + sizeof(PacketNumber), data, s);
			*from = packet->packetAddress;
			*size = s;

			result = kNetworkOkay;
			break;
		}

		loop:
		packet = next;
	}

	incomingMutex.Release();
	connectionMutex.Release();
	return (result);
}

void NetworkMgr::FlushPackets(const NetworkAddress& address)
{
	incomingMutex.Acquire();

	NetworkPacket *packet = activeIncomingPacketList.First();
	while (packet)
	{
		NetworkPacket *next = packet->Next();

		if (packet->packetAddress == address)
		{
			packet->packetPool->Append(packet);
		}

		packet = next;
	}

	incomingMutex.Release();
	outgoingMutex.Acquire();

	packet = activeOutgoingPacketList.First();
	while (packet)
	{
		NetworkPacket *next = packet->Next();

		if (packet->packetAddress == address)
		{
			packet->packetPool->Append(packet);
		}

		packet = next;
	}

	outgoingMutex.Release();
}

void NetworkMgr::ResetPackets(void)
{
	for (machine a = 0; a < kPacketTypeCount; a++)
	{
		incomingPacketCounter[a] = 0;
		outgoingPacketCounter[a] = 0;
	}

	badPacketCounter = 0;

	for (machine a = 0; a < kMaxIncomingPacketCount; a++)
	{
		incomingGeneralPacketPool.Append(&incomingGeneralPacket[a]);
	}

	for (machine a = 0; a < kMaxOutgoingPacketCount; a++)
	{
		outgoingDataPacketPool.Append(&outgoingDataPacket[a]);
	}

	for (machine a = 0; a < kMaxOutgoingPacketCount; a++)
	{
		outgoingControlPacketPool.Append(&outgoingControlPacket[a]);
	}
}

void NetworkMgr::NetworkTask(void)
{
	if (!networkSocket.Open())
	{
		return;
	}

	DomainNameResolver *resolver = resolverList.First();
	while (resolver)
	{
		DomainNameResolver *next = resolver->Next();

		if (resolver->GetResolveResult() != kNetworkResolvePending)
		{
			resolver->CallCompletionProc();
			delete resolver;
		}

		resolver = next;
	}

	for (machine a = 0; a < kPacketTypeCount; a++)
	{
		incomingPacketCounter[a] = 0;
		outgoingPacketCounter[a] = 0;
	}

	badPacketCounter = 0;

	connectionMutex.Acquire();
	incomingMutex.Acquire();

	NetworkPacket *packet = activeIncomingPacketList.First();
	while (packet)
	{
		NetworkPacket *next = packet->Next();

		if (packet->GetPacketType() == kPacketControl)
		{
			NetworkConnection *connection = GetConnection(packet->packetAddress);
			unsigned_int32 param = ReadBigEndianU32(packet->packetDataU32 + sizeof(PacketNumber) / 4);

			switch (packet->packetNumber.GetNumber())
			{
				case kPacketNumberDisconnect:
				{
					if (connection)
					{
						while (next)
						{
							if (next->packetAddress != connection->GetAddress())
							{
								break;
							}

							next = next->Next();
						}

						incomingMutex.Release();
						FlushPackets(connection->GetAddress());
						incomingMutex.Acquire();

						if (connection->GetStatus() != kConnectionDisconnect)
						{
							SendControlPacket(connection->GetAddress(), kPacketNumberDisconnect);

							if (networkEventProc)
							{
								(*networkEventProc)(kNetworkEventClose, connection->GetAddress(), 0);
							}
						}

						delete connection;
					}
					else
					{
						SendControlPacket(packet->packetAddress, kPacketNumberDisconnect);
					}

					break;
				}

				case kPacketNumberConnectRequest:
				{
					if (param != networkProtocol)
					{
						param = kNetworkFailWrongProtocol;
						SendControlPacket(packet->packetAddress, kPacketNumberConnectDeny, &param);
					}
					else if (connectionMap.GetElementCount() >= maxConnectionCount)
					{
						param = (maxConnectionCount == 0) ? kNetworkFailNotServer : kNetworkFailServerFull;
						SendControlPacket(packet->packetAddress, kPacketNumberConnectDeny, &param);
					}
					else if (!connection)
					{
						unsigned_int32 num = Math::Random(65536);
						num = (num | (num << 16)) & kPacketNumberRangeMask;
						connection = new NetworkConnection(packet->packetAddress, kConnectionOpen, num);
						connectionMap.Insert(connection);

						SendControlPacket(connection->GetAddress(), kPacketNumberConnectAccept, &num);

						if (networkEventProc)
						{
							(*networkEventProc)(kNetworkEventOpen, connection->GetAddress(), 0);
						}
					}
					else
					{
						SendControlPacket(connection->GetAddress(), kPacketNumberConnectAccept, &connection->initialPacketNumber);
					}

					break;
				}

				case kPacketNumberConnectAccept:
				{
					if ((connection) && (connection->GetStatus() == kConnectionRequest))
					{
						connection->SetInitialPacketNumbers(param);
						connection->SetStatus(kConnectionOpen);

						if (networkEventProc)
						{
							(*networkEventProc)(kNetworkEventAccept, connection->GetAddress(), 0);
						}
					}

					break;
				}

				case kPacketNumberConnectDeny:
				{
					if ((connection) && (connection->GetStatus() == kConnectionRequest))
					{
						while (next)
						{
							if (next->packetAddress != connection->GetAddress())
							{
								break;
							}

							next = next->Next();
						}

						incomingMutex.Release();
						FlushPackets(connection->GetAddress());
						incomingMutex.Acquire();

						if (networkEventProc)
						{
							(*networkEventProc)(kNetworkEventFail, connection->GetAddress(), param);
						}

						delete connection;
					}

					break;
				}

				default:
				{
					badPacketCounter++;
					break;
				}
			}

			packet->packetPool->Append(packet);
		}

		packet = next;
	}

	incomingMutex.Release();
	outgoingMutex.Acquire();

	unsigned_int32 time = TheTimeMgr->GetSystemAbsoluteTime();
	bool send = false;

	NetworkConnection *connection = connectionMap.First();
	while (connection)
	{
		NetworkConnection *nextConnection = connection->Next();

		ConnectionStatus status = connection->GetStatus();
		if (status < kConnectionOpen)
		{
			if (time - connection->controlSendTime >= reliableResendTime)
			{
				connection->controlSendTime = time;
				if (status == kConnectionRequest)
				{
					if (++connection->controlSendCount < reliableResendCount)
					{
						SendControlPacket(connection->GetAddress(), kPacketNumberConnectRequest, &networkProtocol);
					}
					else
					{
						FlushPackets(connection->GetAddress());

						if (networkEventProc)
						{
							(*networkEventProc)(kNetworkEventFail, connection->GetAddress(), kNetworkFailTimeout);
						}

						delete connection;
					}
				}
				else
				{
					if (++connection->controlSendCount < reliableResendCount)
					{
						SendControlPacket(connection->GetAddress(), kPacketNumberDisconnect);
					}
					else
					{
						FlushPackets(connection->GetAddress());
						delete connection;
					}
				}
			}
		}
		else
		{
			packet = connection->pendingAcknowledgePacketList.First();
			while (packet)
			{
				NetworkPacket *next = packet->Next();

				if (time - packet->sentTime >= packet->resendTime)
				{
					if (++packet->resendCount < reliableResendCount)
					{
						packet->resendTime += packet->resendTime >> 2;
						connection->activeOutgoingPacketList.Append(packet);
						send = true;
					}
					else
					{
						packet->packetPool->Append(packet);
						NetworkConnection *lostConnection = GetConnection(packet->packetAddress);
						if (lostConnection)
						{
							FlushPackets(lostConnection->GetAddress());
							if (networkEventProc)
							{
								(*networkEventProc)(kNetworkEventTimeout, lostConnection->GetAddress(), 0);
							}

							delete lostConnection;
							break;
						}
					}
				}

				packet = next;
			}
		}

		connection = nextConnection;
	}

	outgoingMutex.Release();
	connectionMutex.Release();

	if (send)
	{
		networkSocket.SendPackets();
	}
}

// ZYUQURM
