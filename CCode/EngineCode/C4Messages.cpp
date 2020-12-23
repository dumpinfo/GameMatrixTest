 

#include "C4Messages.h"
#include "C4Application.h"
#include "C4Engine.h"
#include "C4Random.h"
#include "C4AudioCapture.h"


using namespace C4;


namespace
{
	const NetworkAddress kAddressServer(0, 0);
	const NetworkAddress kAddressSelf(0, 1);
}


MessageMgr *C4::TheMessageMgr = nullptr;


namespace C4
{
	template <> MessageMgr Manager<MessageMgr>::managerObject(0);
	template <> MessageMgr **Manager<MessageMgr>::managerPointer = &TheMessageMgr;

	template <> const char *const Manager<MessageMgr>::resultString[] =
	{
		nullptr
	};

	template <> const unsigned_int32 Manager<MessageMgr>::resultIdentifier[] =
	{
		0
	};

	template class Manager<MessageMgr>;

	template <> Heap EngineMemory<MessageMgr>::heap("MessageMgr", 8192, kHeapMutexless);
	template class EngineMemory<MessageMgr>;
}


Message::Message(MessageType type, unsigned_int32 flags)
{
	messageType = type;
	messageFlags = flags;
}

Message::~Message()
{
}

void Message::Compress(Compressor& data) const
{
}

bool Message::Decompress(Decompressor& data)
{
	return (true);
}

bool Message::HandleMessage(Player *sender) const
{
	return (false);
}


PingMessage::PingMessage() : Message(kMessagePing)
{
}

PingMessage::~PingMessage()
{
}


ServerQueryMessage::ServerQueryMessage() : Message(kMessageServerQuery)
{
}

ServerQueryMessage::ServerQueryMessage(unsigned_int32 prot) : Message(kMessageServerQuery)
{
	protocol = prot;
}

ServerQueryMessage::~ServerQueryMessage()
{
}

void ServerQueryMessage::Compress(Compressor& data) const
{
	data << protocol;
}

bool ServerQueryMessage::Decompress(Decompressor& data)
{
	data >> protocol;
	return (true);
}

 
ConnectMessage::ConnectMessage() : Message(kMessageConnect)
{
} 

ConnectMessage::ConnectMessage(PlayerKey key, const char *name) : Message(kMessageConnect) 
{
	playerKey = key;
	playerName = name; 
}
 
ConnectMessage::~ConnectMessage() 
{
}

void ConnectMessage::Compress(Compressor& data) const 
{
	data << (int16) playerKey;
	data << playerName;
}

bool ConnectMessage::Decompress(Decompressor& data)
{
	int16	player;

	data >> player;
	playerKey = player;

	data >> playerName;
	return (true);
}


DisconnectMessage::DisconnectMessage() : Message(kMessageDisconnect)
{
}

DisconnectMessage::DisconnectMessage(PlayerKey key) : Message(kMessageDisconnect)
{
	playerKey = key;
}

DisconnectMessage::~DisconnectMessage()
{
}

void DisconnectMessage::Compress(Compressor& data) const
{
	data << (int16) playerKey;
}

bool DisconnectMessage::Decompress(Decompressor& data)
{
	int16	player;

	data >> player;
	playerKey = player;
	return (true);
}


TimeoutMessage::TimeoutMessage() : Message(kMessageTimeout)
{
}

TimeoutMessage::TimeoutMessage(PlayerKey key) : Message(kMessageTimeout)
{
	playerKey = key;
}

TimeoutMessage::~TimeoutMessage()
{
}

void TimeoutMessage::Compress(Compressor& data) const
{
	data << (int16) playerKey;
}

bool TimeoutMessage::Decompress(Decompressor& data)
{
	int16	player;

	data >> player;
	playerKey = player;
	return (true);
}


ClientInfoMessage::ClientInfoMessage() : Message(kMessageClientInfo)
{
}

ClientInfoMessage::ClientInfoMessage(const char *name) : Message(kMessageClientInfo)
{
	playerName = name;
}

ClientInfoMessage::~ClientInfoMessage()
{
}

void ClientInfoMessage::Compress(Compressor& data) const
{
	data << playerName;
}

bool ClientInfoMessage::Decompress(Decompressor& data)
{
	data >> playerName;
	return (true);
}


PlayerKeyMessage::PlayerKeyMessage() : Message(kMessagePlayerKey)
{
}

PlayerKeyMessage::PlayerKeyMessage(PlayerKey key) : Message(kMessagePlayerKey)
{
	playerKey = key;
}

PlayerKeyMessage::~PlayerKeyMessage()
{
}

void PlayerKeyMessage::Compress(Compressor& data) const
{
	data << (int16) playerKey;
}

bool PlayerKeyMessage::Decompress(Decompressor& data)
{
	int16	player;

	data >> player;
	playerKey = player;
	return (true);
}


SynchronizeMessage::SynchronizeMessage() : Message(kMessageSynchronize)
{
}

SynchronizeMessage::~SynchronizeMessage()
{
}


ChatMessage::ChatMessage() : Message(kMessageChat)
{
}

ChatMessage::ChatMessage(const char *text) : Message(kMessageChat)
{
	playerKey = TheMessageMgr->GetLocalPlayerKey();
	chatText = text;
}

ChatMessage::~ChatMessage()
{
}

void ChatMessage::Compress(Compressor& data) const
{
	data << (int16) playerKey;
	data << chatText;
}

bool ChatMessage::Decompress(Decompressor& data)
{
	int16	player;

	data >> player;
	playerKey = player;

	data >> chatText;
	return (true);
}

bool ChatMessage::HandleMessage(Player *sender) const
{
	if (TheMessageMgr->Server())
	{
		playerKey = sender->GetPlayerKey();
		TheMessageMgr->SendMessageClients(*this, sender);
	}

	Player *player = TheMessageMgr->GetPlayer(playerKey);
	if (player)
	{
		TheApplication->HandlePlayerEvent(kPlayerChatReceived, player, chatText);
	}

	return (true);
}


NameMessage::NameMessage() : Message(kMessageName)
{
}

NameMessage::NameMessage(PlayerKey key, const char *name) : Message(kMessageName)
{
	playerKey = key;
	playerName = name;
}

NameMessage::~NameMessage()
{
}

void NameMessage::Compress(Compressor& data) const
{
	data << (int16) playerKey;
	data << playerName;
}

bool NameMessage::Decompress(Decompressor& data)
{
	int16	player;

	data >> player;
	playerKey = player;

	data >> playerName;
	return (true);
}

bool NameMessage::HandleMessage(Player *sender) const
{
	Player *player = TheMessageMgr->GetPlayer(playerKey);
	if (player)
	{
		String<kMaxPlayerNameLength> prevName(player->GetPlayerName());
		player->SetPlayerName(playerName);

		TheApplication->HandlePlayerEvent(kPlayerRenamed, player, prevName);
		TheMessageMgr->SendMessageClients(*this);
	}

	return (true);
}


FileRequestMessage::FileRequestMessage() : Message(kMessageFileRequest)
{
}

FileRequestMessage::FileRequestMessage(const char *name) : Message(kMessageFileRequest)
{
	fileName = name;
}

FileRequestMessage::~FileRequestMessage()
{
}

void FileRequestMessage::Compress(Compressor& data) const
{
	data << fileName;
}

bool FileRequestMessage::Decompress(Decompressor& data)
{
	data >> fileName;
	return (true);
}

bool FileRequestMessage::HandleMessage(Player *sender) const
{
	sender->StartSendingFile(fileName);
	return (true);
}


FileReceiveMessage::FileReceiveMessage() : Message(kMessageFileReceive)
{
}

FileReceiveMessage::FileReceiveMessage(const char *name, unsigned_int64 size) : Message(kMessageFileReceive)
{
	fileSize = size;
	fileName = name;
}

FileReceiveMessage::~FileReceiveMessage()
{
}

void FileReceiveMessage::Compress(Compressor& data) const
{
	data << fileSize;
	data << fileName;
}

bool FileReceiveMessage::Decompress(Decompressor& data)
{
	data >> fileSize;
	data >> fileName;
	return (true);
}

bool FileReceiveMessage::HandleMessage(Player *sender) const
{
	sender->StartReceivingFile(fileName, fileSize);
	return (true);
}


FileErrorMessage::FileErrorMessage() : Message(kMessageFileError)
{
}

FileErrorMessage::FileErrorMessage(FileTransferResult result) : Message(kMessageFileError)
{
	fileResult = result;
}

FileErrorMessage::~FileErrorMessage()
{
}

void FileErrorMessage::Compress(Compressor& data) const
{
	data << (unsigned_int8) fileResult;
}

bool FileErrorMessage::Decompress(Decompressor& data)
{
	unsigned_int8	result;

	data >> result;
	fileResult = static_cast<FileTransferResult>(result);

	return (true);
}

bool FileErrorMessage::HandleMessage(Player *sender) const
{
	sender->StopReceivingFile(fileResult);
	return (true);
}


FileCancelMessage::FileCancelMessage() : Message(kMessageFileCancel)
{
}

FileCancelMessage::~FileCancelMessage()
{
}

bool FileCancelMessage::HandleMessage(Player *sender) const
{
	sender->StopSendingFile();
	return (true);
}


FileChunkMessage::FileChunkMessage() : Message(kMessageFileChunk)
{
}

FileChunkMessage::FileChunkMessage(unsigned_int32 size) : Message(kMessageFileChunk, kMessageCombineInhibit)
{
	chunkSize = size;
}

FileChunkMessage::~FileChunkMessage()
{
}

void FileChunkMessage::Compress(Compressor& data) const
{
	data << (unsigned_int16) chunkSize;
	data.Write(chunkData, chunkSize);
}

bool FileChunkMessage::Decompress(Decompressor& data)
{
	unsigned_int16	size;

	data >> size;
	if (size > kMaxFileChunkSize)
	{
		return (false);
	}

	chunkSize = size;
	data.Read(chunkData, size);
	return (true);
}

bool FileChunkMessage::HandleMessage(Player *sender) const
{
	sender->ReceiveFileChunk(this);
	return (true);
}


ControllerMessage::ControllerMessage(ControllerMessageType type, int32 index, unsigned_int32 flags) : Message(kMessageController, flags)
{
	controllerMessageType = type;
	controllerIndex = index;
}

ControllerMessage::~ControllerMessage()
{
}

void ControllerMessage::Compress(Compressor& data) const
{
	data << (unsigned_int8) controllerMessageType;
	data << (unsigned_int16) controllerIndex;
}

bool ControllerMessage::Decompress(Decompressor& data)
{
	return (true);
}

bool ControllerMessage::HandleMessage(Player *sender) const
{
	MessageMgr::ControllerMessageReceiveProc *receiver = TheMessageMgr->GetControllerMessageReceiver();
	if (receiver)
	{
		(*receiver)(this, TheMessageMgr->GetControllerMessageCookie());
	}

	return (true);
}

void ControllerMessage::HandleControllerMessage(Controller *controller) const
{
}

bool ControllerMessage::OverridesMessage(const ControllerMessage *message) const
{
	return (false);
}


CombinedMessage::CombinedMessage(MessageType type, unsigned_int32 size, const char *data, unsigned_int32 flags)
{
	combinedFlags = flags;
	packetFlags = (type == kMessageFileChunk) ? kPacketFlagFile : 0;

	messageCount = 1;
	messageSize = size + 2;

	messageBuffer[1] = (char) type;
	MemoryMgr::CopyMemory(data, &messageBuffer[2], size);
}

CombinedMessage::~CombinedMessage()
{
}

bool CombinedMessage::AddMessage(MessageType type, unsigned_int32 size, const char *data)
{
	if (messageCount < kMaxCombinedMessageCount)
	{
		unsigned_int32 offset = messageSize;
		unsigned_int32 newSize = offset + size + 1;
		if (newSize <= kMaxCombinedMessageSize)
		{
			messageCount++;
			messageSize = newSize;

			if (type == kMessageFileChunk)
			{
				packetFlags |= kPacketFlagFile;
			}

			messageBuffer[offset] = (char) type;
			MemoryMgr::CopyMemory(data, &messageBuffer[offset + 1], size);

			return (true);
		}
	}

	return (false);
}

NetworkResult CombinedMessage::Send(const NetworkAddress& address)
{
	messageBuffer[0] = (char) messageCount;

	if (!(combinedFlags & kMessageUnreliable))
	{
		return (TheNetworkMgr->SendReliablePacket(address, messageSize, messageBuffer, packetFlags));
	}

	if (combinedFlags & kMessageUnordered)
	{
		return (TheNetworkMgr->SendUnorderedPacket(address, messageSize, messageBuffer));
	}

	return (TheNetworkMgr->SendUnreliablePacket(address, messageSize, messageBuffer));
}


StateSender::StateSender(SenderProc *proc, void *cookie)
{
	senderProc = proc;
	senderCookie = cookie;
}

StateSender::~StateSender()
{
}


SnapshotSender::SnapshotSender()
{
	snapshotPeriod = 1;
}

SnapshotSender::~SnapshotSender()
{
}


Channel::Channel(Player *sender, Player *receiver) : GraphEdge<Player, Channel>(sender, receiver)
{
	enabledFlag = true;
}

Channel::~Channel()
{
}


Player::Player(PlayerKey key)
{
	playerKey = key;
	playerAddress.Set(0, 0);

	chatSound = nullptr;
	chatSoundGroup = nullptr;
	chatStreamer = nullptr;
	chatVolume = 1.0F;

	playerName[0] = 0;

	fileChunkSize = kMaxFileChunkSize;
	fileChunkCount = 4;
}

Player::~Player()
{
	if (receiveFile.Open())
	{
		receiveFile.Close();
		TheFileMgr->DeleteFile(receiveFileName);
		receiveFileResult = kTransferConnectionLost;
		CallCompletionProc();
	}
	else if (receiveFileResult == kTransferPending)
	{
		receiveFileResult = kTransferConnectionLost;
		CallCompletionProc();
	}

	if (chatSound)
	{
		chatSound->Release();
	}
}

void Player::AddMessage(const Message& message, unsigned_int32 size, const char *data)
{
	unsigned_int32 flags = message.GetMessageFlags();
	unsigned_int32 combinedFlags = flags & (kMessageUnreliable | kMessageUnordered);

	MessageType type = message.GetMessageType();
	if (!(flags & kMessageCombineInhibit))
	{
		CombinedMessage *combinedMessage = messageList.Last();
		if ((combinedMessage) && (combinedMessage->GetCombinedFlags() == combinedFlags) && (combinedMessage->AddMessage(type, size, data)))
		{
			return;
		}
	}

	CombinedMessage *combinedMessage = new CombinedMessage(type, size, data, combinedFlags);
	messageList.Append(combinedMessage);
}

void Player::SendMessage(const Message& message)
{
	if (TheMessageMgr->GetLocalPlayer() == this)
	{
		TheMessageMgr->ReceiveMessage(kAddressSelf, &message);
	}
	else
	{
		char	data[kMaxMessageDataSize];

		Compressor compressor(data);
		message.Compress(compressor);

		AddMessage(message, compressor.GetSize(), data);
	}
}

void Player::StartSendingFile(const char *name)
{
	if (!sendFile.Open())
	{
		if (sendFile.Open(name) == kFileOkay)
		{
			sendFileSize = sendFile.GetSize();
			SendMessage(FileReceiveMessage(name, sendFileSize));

			String<255> text("Sending file ");
			text += name;
			text += " to ";
			text += MessageMgr::AddressToString(playerAddress, true);
			Engine::Report(text);
		}
		else
		{
			SendMessage(FileErrorMessage(kTransferFileNotFound));
		}
	}
}

void Player::StopSendingFile(void)
{
	sendFile.Close();
}

void Player::StartReceivingFile(const char *name, unsigned_int64 size)
{
	if (!receiveFile.Open())
	{
		if (receiveFileResult == kTransferPending)
		{
			if (receiveFile.Open(name, kFileCreate) == kFileOkay)
			{
				receiveFileSize = size;
				receiveFileName = name;
			}
			else
			{
				SendMessage(FileCancelMessage());

				receiveFileResult = kTransferFileUnwriteable;
				CallCompletionProc();
			}
		}
		else
		{
			SendMessage(FileCancelMessage());
		}
	}
}

void Player::StopReceivingFile(FileTransferResult result)
{
	if (receiveFile.Open())
	{
		receiveFile.Close();

		TheFileMgr->DeleteFile(receiveFileName);
		receiveFileResult = result;
		CallCompletionProc();
	}
	else if (receiveFileResult == kTransferPending)
	{
		receiveFileResult = result;
		CallCompletionProc();
	}
}

void Player::SendFileChunk(void)
{
	bool done = false;
	for (machine a = 0; a < fileChunkCount; a++)
	{
		unsigned_int32 size = (unsigned_int32) Min64(sendFileSize - sendFile.GetPosition(), fileChunkSize);
		FileChunkMessage fcm(size);

		if (sendFile.Read(fcm.GetChunkData(), size) == kFileOkay)
		{
			SendMessage(fcm);
			if (sendFile.GetPosition() == sendFileSize)
			{
				sendFile.Close();
				done = true;
			}
		}
		else
		{
			SendMessage(FileErrorMessage(kTransferFileUnreadable));

			sendFile.Close();
			done = true;
		}

		if (done)
		{
			break;
		}
	}
}

void Player::ReceiveFileChunk(const FileChunkMessage *fcm)
{
	if (receiveFile.Open())
	{
		if (receiveFile.Write(fcm->GetChunkData(), fcm->GetChunkSize()) == kFileOkay)
		{
			if (receiveFile.GetPosition() == receiveFileSize)
			{
				receiveFile.Close();
				receiveFileResult = kTransferOkay;
				CallCompletionProc();
			}
		}
		else
		{
			SendMessage(FileCancelMessage());
			StopReceivingFile(kTransferFileUnwriteable);
		}
	}
}

void Player::RequestFile(const char *name)
{
	SendMessage(FileRequestMessage(name));

	receiveFileSize = 0;
	receiveFileResult = kTransferPending;
}

ChatStreamer *Player::CreateChatStreamer(void)
{
	chatSound = new Sound;
	chatSound->SetSoundGroup(chatSoundGroup);
	chatStreamer = new ChatStreamer;

	chatSound->SetSoundProperty(kSoundVolume, chatVolume);
	chatSound->Stream(chatStreamer);
	chatSound->Play();

	return (chatStreamer);
}

void Player::SetChatSoundGroup(SoundGroup *group)
{
	chatSoundGroup = group;

	if (chatSound)
	{
		chatSound->SetSoundGroup(chatSoundGroup);
	}
}

void Player::SetChatVolume(float volume)
{
	chatVolume = volume;

	if (chatSound)
	{
		chatSound->SetSoundProperty(kSoundVolume, volume);
	}
}


MessageMgr::MessageMgr(int) :
		playerNameObserver(this, &MessageMgr::HandlePlayerNameEvent),
		maxPlayersObserver(this, &MessageMgr::HandleMaxPlayersEvent)
{
}

MessageMgr::~MessageMgr()
{
}

EngineResult MessageMgr::Construct(void)
{
	serverFlag = true;
	multiplayerFlag = false;
	synchronizedFlag = true;

	SetSnapshotInterval(50);

	chatSendTime = 0;
	chatLockoutTime = 1000;

	playerCreatorProc = nullptr;
	controllerMessageCreator = nullptr;
	controllerMessageReceiver = nullptr;

	TheNetworkMgr->SetNetworkEventProc(&HandleNetworkEvent);

	TheEngine->InitVariable("gameName", "Game", kVariablePermanent);
	TheEngine->InitVariable("playerName", "Player", kVariablePermanent, &playerNameObserver);
	TheEngine->InitVariable("maxPlayers", "8", kVariablePermanent, &maxPlayersObserver);

	return (kEngineOkay);
}

void MessageMgr::Destruct(void)
{
	playerMap.Purge();
	stateSenderList.Purge();
	snapshotSenderList.Purge();
	journaledMessageList.Purge();
}

String<31> MessageMgr::AddressToString(const NetworkAddress& address, bool includePort)
{
	unsigned_int32 addr = address.GetAddress();
	if (includePort)
	{
		int32 port = address.GetPort();
		return (String<31>((int32) (addr >> 24)) + "." + (int32) ((addr >> 16) & 255) + "." + (int32) ((addr >> 8) & 255) + "." + (int32) (addr & 255) + ":" + port);
	}

	return (String<31>((int32) (addr >> 24)) + "." + (int32) ((addr >> 16) & 255) + "." + (int32) ((addr >> 8) & 255) + "." + (int32) (addr & 255));
}

NetworkAddress MessageMgr::StringToAddress(const char *text)
{
	unsigned_int32 address = 0;
	unsigned_int32 port = 0;

	bool ip = true;
	unsigned_int32 part = 0;
	for (;;)
	{
		unsigned_int32 c = *text++;
		if (c == 0)
		{
			if (ip)
			{
				address = (address << 8) | (part & 255);
			}

			break;
		}
		else if (c == '.')
		{
			if (ip)
			{
				address = (address << 8) | (part & 255);
				part = 0;
			}
		}
		else if (c == ':')
		{
			if (ip)
			{
				address = (address << 8) | (part & 255);
				ip = false;
			}
		}
		else if (c - '0' < 10U)
		{
			if (ip)
			{
				part = part * 10 + c - 48;
			}
			else
			{
				port = port * 10 + c - 48;
			}
		}
	}

	return (NetworkAddress(address, (unsigned_int16) port));
}

Player *MessageMgr::NewPlayer(PlayerKey key)
{
	if (key == kPlayerNew)
	{
		do
		{
			key = Math::Random(32767) + 1;
		} while (playerMap.Find(key));
	}

	Player *player = (playerCreatorProc) ? (*playerCreatorProc)(key, playerCreatorCookie) : new Player(key);
	playerMap.Insert(player);
	return (player);
}

Player *MessageMgr::GetPlayer(const NetworkAddress& address) const
{
	if (address.GetAddress() != 0)
	{
		Player *player = playerMap.First();
		while (player)
		{
			if (player->GetPlayerAddress() == address)
			{
				return (player);
			}

			player = player->Next();
		}
	}
	else
	{
		if (address == kAddressSelf)
		{
			return (localPlayer);
		}

		if (address == kAddressServer)
		{
			return (GetPlayer(kPlayerServer));
		}
	}

	return (nullptr);
}

Player *MessageMgr::GetFirstClientPlayer(void) const
{
	Player *player = playerMap.First();
	while (player)
	{
		if (player->GetPlayerKey() > kPlayerNone)
		{
			return (player);
		}

		player = player->Next();
	}

	return (nullptr);
}

void MessageMgr::BeginSinglePlayerGame(void)
{
	TheNetworkMgr->Terminate();

	playerMap.Purge();
	journaledMessageList.Purge();

	serverFlag = true;
	multiplayerFlag = false;
	synchronizedFlag = true;

	Player *player = NewPlayer(kPlayerServer);
	player->SetPlayerName(TheEngine->GetVariable("playerName")->GetValue());
	localPlayer = player;
}

NetworkResult MessageMgr::BeginMultiplayerGame(bool host)
{
	TheNetworkMgr->Terminate();

	playerMap.Purge();
	journaledMessageList.Purge();

	TheNetworkMgr->SetMaxConnectionCount((host) ? maxPlayerCount - 1 : 0);
	NetworkResult result = TheNetworkMgr->Initialize();
	if (result == kNetworkOkay)
	{
		serverFlag = host;
		multiplayerFlag = true;
		synchronizedFlag = host;

		Player *player = NewPlayer((host) ? kPlayerServer : kPlayerNone);
		player->SetPlayerName(TheEngine->GetVariable("playerName")->GetValue());
		localPlayer = player;

		snapshotTime = snapshotInterval;
	}
	else
	{
		serverFlag = true;
		multiplayerFlag = false;
		synchronizedFlag = true;
	}

	return (result);
}

void MessageMgr::EndGame(void)
{
	DisconnectAll();

	playerMap.Purge();
	journaledMessageList.Purge();

	serverFlag = true;
	multiplayerFlag = false;
	synchronizedFlag = true;
}

void MessageMgr::AddSnapshotSender(SnapshotSender *sender)
{
	sender->ExpediteSnapshot();
	snapshotSenderList.Append(sender);
}

void MessageMgr::RemoveSnapshotSender(SnapshotSender *sender)
{
	if (snapshotSenderList.Member(sender))
	{
		snapshotSenderList.Remove(sender);
	}
}

void MessageMgr::SetSnapshotInterval(int32 time)
{
	snapshotInterval = time;
	snapshotFrequency = 1.0F / (float) time;
}

void MessageMgr::HandleNetworkEvent(NetworkEvent event, const NetworkAddress& address, unsigned_int32 param)
{
	switch (event)
	{
		case kNetworkEventOpen:
		{
			Player *player = TheMessageMgr->NewPlayer(kPlayerNew);
			player->SetPlayerAddress(address);

			TheApplication->HandleConnectionEvent(kConnectionClientOpened, address, nullptr);
			break;
		}

		case kNetworkEventAccept:
		{
			if (!TheMessageMgr->Server())
			{
				Player *player = TheMessageMgr->NewPlayer(kPlayerServer);
				player->SetPlayerAddress(address);

				player->SendMessage(ClientInfoMessage(TheMessageMgr->GetLocalPlayer()->GetPlayerName()));
				TheApplication->HandleConnectionEvent(kConnectionServerAccepted, address, nullptr);
			}
			else
			{
				TheNetworkMgr->Disconnect(address);
			}

			break;
		}

		case kNetworkEventClose:
		{
			Player *player = TheMessageMgr->GetPlayer(address);
			if (player)
			{
				if (TheMessageMgr->Server())
				{
					TheMessageMgr->SendMessageAll(DisconnectMessage(player->GetPlayerKey()));
					TheApplication->HandleConnectionEvent(kConnectionClientClosed, address, nullptr);
				}
				else
				{
					TheApplication->HandleConnectionEvent(kConnectionServerClosed, address, nullptr);
				}
			}

			break;
		}

		case kNetworkEventFail:
		{
			TheApplication->HandleConnectionEvent(kConnectionAttemptFailed, address, &param);
			break;
		}

		case kNetworkEventTimeout:
		{
			Player *player = TheMessageMgr->GetPlayer(address);
			if (player)
			{
				if (TheMessageMgr->Server())
				{
					TheMessageMgr->SendMessageAll(TimeoutMessage(player->GetPlayerKey()));
					TheApplication->HandleConnectionEvent(kConnectionClientTimedOut, address, nullptr);
				}
				else
				{
					TheApplication->HandleConnectionEvent(kConnectionServerTimedOut, address, nullptr);
				}
			}

			break;
		}
	}
}

Message *MessageMgr::CreateMessage(MessageType type, Decompressor& data)
{
	switch (type)
	{
		case kMessagePing:

			return (new PingMessage);

		case kMessageServerQuery:

			return (new ServerQueryMessage);

		case kMessageConnect:

			return (new ConnectMessage);

		case kMessageDisconnect:

			return (new DisconnectMessage);

		case kMessageTimeout:

			return (new TimeoutMessage);

		case kMessageClientInfo:

			return (new ClientInfoMessage);

		case kMessagePlayerKey:

			return (new PlayerKeyMessage);

		case kMessageSynchronize:

			return (new SynchronizeMessage);

		case kMessageChat:

			return (new ChatMessage);

		case kMessageAudio:

			return (new AudioMessage);

		case kMessageName:

			return (new NameMessage);

		case kMessageFileRequest:

			return (new FileRequestMessage);

		case kMessageFileReceive:

			return (new FileReceiveMessage);

		case kMessageFileError:

			return (new FileErrorMessage);

		case kMessageFileCancel:

			return (new FileCancelMessage);

		case kMessageFileChunk:

			return (new FileChunkMessage);

		case kMessageController:

			if (controllerMessageCreator)
			{
				unsigned_int8	controllerMessageType;
				unsigned_int16	controllerIndex;

				data >> controllerMessageType;
				data >> controllerIndex;

				return ((*controllerMessageCreator)(controllerMessageType, controllerIndex, data, controllerMessageCookie));
			}

			break;

		default:

			return (TheApplication->CreateMessage(type, data));
	}

	return (nullptr);
}

void MessageMgr::ReceiveMessage(const NetworkAddress& address, const Message *message)
{
	Player *sender = GetPlayer(address);
	if ((sender) && (message->HandleMessage(sender)))
	{
		return;
	}

	MessageType type = message->GetMessageType();
	switch (type)
	{
		case kMessagePing:

			break;

		case kMessageServerQuery:

			if ((serverFlag) && (multiplayerFlag))
			{
				const ServerQueryMessage *m = static_cast<const ServerQueryMessage *>(message);
				if (m->GetProtocol() == TheNetworkMgr->GetProtocol())
				{
					TheApplication->HandleConnectionEvent(kConnectionQueryReceived, address, nullptr);
				}
			}

			break;

		case kMessageConnect:
		{
			const ConnectMessage *m = static_cast<const ConnectMessage *>(message);

			PlayerKey key = m->GetPlayerKey();
			Player *player = GetPlayer(key);
			if (!player)
			{
				player = NewPlayer(key);
			}

			player->SetPlayerName(m->GetPlayerName());
			TheApplication->HandlePlayerEvent(kPlayerConnected, player, nullptr);
			break;
		}

		case kMessageDisconnect:
		{
			const DisconnectMessage *m = static_cast<const DisconnectMessage *>(message);

			Player *player = GetPlayer(m->GetPlayerKey());
			if (player)
			{
				TheApplication->HandlePlayerEvent(kPlayerDisconnected, player, nullptr);
				delete player;
			}

			break;
		}

		case kMessageTimeout:
		{
			const TimeoutMessage *m = static_cast<const TimeoutMessage *>(message);

			Player *player = GetPlayer(m->GetPlayerKey());
			if (player)
			{
				TheApplication->HandlePlayerEvent(kPlayerTimedOut, player, nullptr);
				delete player;
			}

			break;
		}

		case kMessageClientInfo:

			if ((sender) && (serverFlag))
			{
				const ClientInfoMessage *m = static_cast<const ClientInfoMessage *>(message);

				sender->SetPlayerName(m->GetPlayerName());
				TheApplication->HandlePlayerEvent(kPlayerConnected, sender, nullptr);

				sender->SendMessage(PlayerKeyMessage(sender->GetPlayerKey()));

				ConnectMessage newPlayerInfo(sender->GetPlayerKey(), sender->GetPlayerName());

				Player *player = playerMap.First();
				while (player)
				{
					if (player != sender)
					{
						PlayerKey key = player->GetPlayerKey();
						sender->SendMessage(ConnectMessage(key, player->GetPlayerName()));

						if (key != kPlayerServer)
						{
							player->SendMessage(newPlayerInfo);
						}
					}

					player = player->Next();
				}

				TheApplication->HandlePlayerEvent(kPlayerInitialized, sender, nullptr);

				const Message *journaledMessage = journaledMessageList.First();
				while (journaledMessage)
				{
					sender->SendMessage(*journaledMessage);
					journaledMessage = journaledMessage->Next();
				}

				const StateSender *stateSender = stateSenderList.First();
				while (stateSender)
				{
					stateSender->SendState(sender);
					stateSender = stateSender->Next();
				}

				sender->SendMessage(SynchronizeMessage());
			}

			break;

		case kMessagePlayerKey:
		{
			Player *player = localPlayer;
			if (player)
			{
				const PlayerKeyMessage *m = static_cast<const PlayerKeyMessage *>(message);
				player->playerKey = m->GetPlayerKey();
				playerMap.Insert(player);
			}

			break;
		}

		case kMessageSynchronize:

			synchronizedFlag = true;
			TheApplication->HandleGameEvent(kGameSynchronized, nullptr);
			break;

		default:

			TheApplication->ReceiveMessage(sender, address, message);
			break;
	}
}

void MessageMgr::SendMessage(PlayerKey key, const Message& message)
{
	Player *player = GetPlayer(key);
	if (player)
	{
		player->SendMessage(message);
	}
	else if (key == kPlayerServer)
	{
		if (serverFlag)
		{
			ReceiveMessage(kAddressServer, &message);
		}
	}
	else if (key == kPlayerSelf)
	{
		ReceiveMessage(kAddressSelf, &message);
	}
	else if (key == kPlayerAll)
	{
		SendMessageAll(message);
	}
	else if (key == kPlayerClients)
	{
		SendMessageClients(message);
	}
}

void MessageMgr::SendMessageAll(const Message& message, bool self)
{
	char	data[kMaxMessageDataSize + 1];

	if (serverFlag)
	{
		Player *player = GetFirstClientPlayer();
		if (player)
		{
			Compressor compressor(data);
			message.Compress(compressor);
			unsigned_int32 size = compressor.GetSize();

			do
			{
				player->AddMessage(message, size, data);
				player = player->Next();
			} while (player);
		}
	}
	else
	{
		Player *player = GetPlayer(kPlayerServer);
		if (player)
		{
			Compressor compressor(data);
			message.Compress(compressor);

			player->AddMessage(message, compressor.GetSize(), data);
		}
	}

	if (self)
	{
		ReceiveMessage(kAddressSelf, &message);
	}
}

void MessageMgr::SendMessageClients(const Message& message, const Player *exclude)
{
	if (serverFlag)
	{
		Player *player = GetFirstClientPlayer();
		if (player)
		{
			char	data[kMaxMessageDataSize + 1];

			Compressor compressor(data);
			message.Compress(compressor);
			unsigned_int32 size = compressor.GetSize();

			do
			{
				if (player != exclude)
				{
					player->AddMessage(message, size, data);
				}

				player = player->Next();
			} while (player);
		}
	}
}

void MessageMgr::SendMessageJournal(ControllerMessage *message)
{
	journaledMessageList.Append(message);
	message->messageFlags |= kMessageJournaled;

	SendMessageAll(*message);
}

bool MessageMgr::SendChatMessage(const char *text)
{
	if (chatSendTime <= 0)
	{
		chatSendTime = chatLockoutTime;

		ChatMessage message(text);
		SendMessage(kPlayerServer, message);

		if (!serverFlag)
		{
			ReceiveMessage(kAddressSelf, &message);
		}

		return (true);
	}

	return (false);
}

void MessageMgr::SendConnectionlessMessage(const NetworkAddress& to, const Message& message)
{
	char	data[kMaxCombinedMessageSize];

	data[0] = 1;
	data[1] = (char) message.GetMessageType();

	Compressor compressor(data + 2);
	message.Compress(compressor);

	TheNetworkMgr->SendConnectionlessPacket(to, compressor.GetSize() + 2, data);
}

void MessageMgr::BroadcastServerQuery(void) const
{
	char	data[kMaxCombinedMessageSize];

	ServerQueryMessage message(TheNetworkMgr->GetProtocol());

	data[0] = 1;
	data[1] = (char) message.GetMessageType();

	Compressor compressor(data + 2);
	message.Compress(compressor);

	TheNetworkMgr->BroadcastPacket(compressor.GetSize() + 2, data);
}

NetworkResult MessageMgr::Connect(const NetworkAddress& address)
{
	if (!multiplayerFlag)
	{
		NetworkResult result = BeginMultiplayerGame(false);
		if (result != kNetworkOkay)
		{
			return (result);
		}
	}

	TheNetworkMgr->Connect(address);
	return (kNetworkOkay);
}

void MessageMgr::Disconnect(Player *player)
{
	if (serverFlag)
	{
		TheNetworkMgr->Disconnect(player->GetPlayerAddress());
		delete player;
	}
	else if (player->GetPlayerKey() == kPlayerServer)
	{
		TheNetworkMgr->Disconnect(player->GetPlayerAddress());
		playerMap.Purge();
	}
}

void MessageMgr::DisconnectAll(void)
{
	if (serverFlag)
	{
		Player *player = GetFirstClientPlayer();
		while (player)
		{
			Player *next = player->Next();
			TheNetworkMgr->Disconnect(player->GetPlayerAddress());
			delete player;
			player = next;
		}
	}
	else
	{
		Player *player = GetPlayer(kPlayerServer);
		if (player)
		{
			TheNetworkMgr->Disconnect(player->GetPlayerAddress());
			delete player;
		}
	}
}

void MessageMgr::HandlePlayerNameEvent(Variable *variable)
{
	Player *player = GetLocalPlayer();
	if (player)
	{
		const char *prevName = player->GetPlayerName();
		const char *newName = variable->GetValue();
		if (!Text::CompareText(prevName, newName))
		{
			SendMessage(kPlayerServer, NameMessage(player->GetPlayerKey(), newName));
		}
	}
}

void MessageMgr::HandleMaxPlayersEvent(Variable *variable)
{
	maxPlayerCount = Max(variable->GetIntegerValue(), 1);
}

void MessageMgr::ReceiveTask(void)
{
	static char			data[kMaxMessageSize + 1];
	NetworkAddress		address;
	unsigned_int32		size;

	TheNetworkMgr->NetworkTask();

	while (TheNetworkMgr->ReceivePacket(&address, &size, data) == kNetworkOkay)
	{
		int32 messageCount = *reinterpret_cast<unsigned_int8 *>(data);
		if (messageCount <= kMaxCombinedMessageCount)
		{
			data[size] = 0;
			int32 position = 1;

			for (machine a = 0; a < messageCount; a++)
			{
				MessageType type = *reinterpret_cast<unsigned_int8 *>(&data[position]);
				Decompressor decompressor(&data[++position]);

				Message *message = CreateMessage(type, decompressor);
				if (!message)
				{
					break;
				}

				if (message->Decompress(decompressor))
				{
					position += decompressor.GetSize();
					ReceiveMessage(address, message);
					delete message;
				}
				else
				{
					String<63> text("Invalid message: ");
					text += AddressToString(address, true);
					Engine::Report(text);

					delete message;
					break;
				}
			}
		}
	}
}

void MessageMgr::SendTask(void)
{
	int32 dt = TheTimeMgr->GetSystemDeltaTime();
	if (chatSendTime > 0)
	{
		chatSendTime -= dt;
	}

	if (serverFlag)
	{
		int32 time = (snapshotTime -= dt);
		if (time < 0)
		{
			snapshotTime = snapshotInterval;

			SnapshotSender *sender = snapshotSenderList.First();
			while (sender)
			{
				int32 count = sender->snapshotCount;
				if (--count <= 0)
				{
					sender->snapshotCount = sender->snapshotPeriod;

					SnapshotSender *next = sender->Next();
					sender->SendSnapshot();
					sender = next;
					continue;
				}

				sender->snapshotCount = count;
			}
		}
	}

	Player *player = playerMap.First();
	while (player)
	{
		if ((player->SendingFile()) && (TheNetworkMgr->GetPacketCount(player->GetPlayerAddress(), kPacketFlagFile) == 0))
		{
			player->SendFileChunk();
		}

		player->sendEnabled = true;
		player = player->Next();
	}

	for (;;)
	{
		int32 sendCount = 0;

		player = playerMap.First();
		while (player)
		{
			if (player->sendEnabled)
			{
				CombinedMessage *combinedMessage = player->messageList.First();
				if (combinedMessage)
				{
					if (combinedMessage->Send(player->GetPlayerAddress()) != kNetworkBufferFull)
					{
						delete combinedMessage;
						sendCount++;
					}
					else
					{
						player->sendEnabled = false;
					}
				}
			}

			player = player->Next();
		}

		if (sendCount == 0)
		{
			break;
		}
	}
}

// ZYUQURM
