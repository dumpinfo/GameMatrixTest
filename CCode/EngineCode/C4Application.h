#ifndef C4Application_h
#define C4Application_h


//# \component	System Utilities
//# \prefix		System/

//# \import		C4Network.h
//# \import		C4Messages.h


#include "C4Messages.h"


namespace C4
{
	//# \class	Application		The application module base class.
	//
	//# The $Application$ class is the application module base class.
	//
	//# \def	class Application : public Singleton<Application>
	//
	//# \desc
	//# The $Application$ class represents the application module and provides an interface through which the
	//# engine module can communicate with an application. When the application module is loaded, the engine
	//# calls the $Construct$ function defined in the dynamic link library. The $Construct$ function should
	//# return a pointer to a newly-constructed subclass of the $Application$ class.
	//#
	//# The $Application$ class's member functions are accessed through the global pointer $TheApplication$.
	//
	//# \base	Singleton<Application>	The application module is accessed through the global pointer $TheApplication$.


	//# \function	Application::HandleConnectionEvent		Called to handle a network connection event.
	//
	//# \proto	virtual void HandleConnectionEvent(ConnectionEvent event, const NetworkAddress& address, const void *param);
	//
	//# \param	event		The connection event type. See below for a list of possible types.
	//# \param	address		The network address for which the event occurred.
	//# \param	param		A pointer to parameter information for the event.
	//
	//# \desc
	//# The $HandleConnectionEvent$ function can be overridden by an application-defined subclass of the $Application$
	//# class. This function is called when an event pertaining to a particular connection occurs in a multiplayer game.
	//# The $event$ parameter contains one of the following constants.
	//
	//# \table	ConnectionEvent
	//
	//# The server machine can receive events pertaining to connections with any of the client machines. Each client
	//# machine only receives events pertaining to the connection with the server.
	//#
	//# If the $event$ parameter is $kConnectionAttemptFailed$, then the $param$ parameter points to a $int32$ value
	//# containing one of the following constants indicating the reason for the failure.
	//
	//# \table	NetworkFail
	//
	//# If the $event$ parameter is $kConnectionQueryReceived$, then the server has received a request for
	//# game information. In response, the event handler should send an application-defined message containing
	//# information about the server back to the machine from which the query originated. A connection with this
	//# machine will not normally exist, so the return message should be sent to the address given by the $address$
	//# parameter using the $@MessageMgr/MessageMgr::SendConnectionlessMessage@$ function.
	//
	//# \also	$@Application::HandlePlayerEvent@$
	//# \also	$@Application::HandleGameEvent@$
	//# \also	$@MessageMgr/MessageMgr::BroadcastServerQuery@$
	//# \also	$@MessageMgr/MessageMgr::SendConnectionlessMessage@$


	//# \function	Application::HandlePlayerEvent		Called to handle a player-related event.
	//
	//# \proto	virtual void HandlePlayerEvent(PlayerEvent event, Player *player, const void *param);
	//
	//# \param	event		The player event type. See below for a list of possible types.
	//# \param	player		The player for which the event occurred.
	//# \param	param		A pointer to parameter information for the event.
	//
	//# \desc
	//# The $HandlePlayerEvent$ function can be overridden by an application-defined subclass of the $Application$
	//# class. This function is called when an event pertaining to a particular player occurs in a multiplayer game.
	//# The $event$ parameter contains one of the following constants.
	//
	//# \table	PlayerEvent
	//
	//# If the $event$ parameter is $kPlayerChatReceived$, then the $param$ parameter is a pointer to the chat
	//# text string. This string will always have a maximum length of $kMaxChatMessageLength$ characters.
	//#
	//# If the $event$ parameter is $kPlayerInitialized$, then the server has finished initializing a new player
	//# in a multiplayer game, and it is about to start sending game state to that player. The event handler
	//# should respond to this event by sending one or more application-defined messages back to the player
	//# specified by the $player$ parameter in order to make sure it is ready to receive game state. For
	//# example, it is usually necessary to send information about what world is currently being played back
	//# to the new client at this point. After the $kPlayerInitialized$ is handled, the Message Manager
	//# allows all $@MessageMgr/StateSender@$ objects to send their game state, and this is followed by
	//# the message journal.
	//
	//# \also	$@Application::HandleConnectionEvent@$
	//# \also	$@Application::HandleGameEvent@$


	//# \function	Application::HandleGameEvent		Called to handle an event pertaining to the entire game.
	//
	//# \proto	virtual void HandleGameEvent(GameEvent event, const void *param);
	//
	//# \param	event		The game event type. See below for a list of possible types.
	//# \param	param		A pointer to parameter information for the event.
	//
	//# \desc
	//# The $HandleGameEvent$ function can be overridden by an application-defined subclass of the $Application$
	//# class. This function is called when a globally applicable event occurs in a multiplayer game.
	//# The $event$ parameter contains one of the following constants.
	//
	//# \table	GameEvent
	//
	//# \also	$@Application::HandleConnectionEvent@$
	//# \also	$@Application::HandlePlayerEvent@$


	//# \div
	//# \function	Application::CreateMessage		Called to create an application-defined message.
	//
	//# \proto	virtual Message *CreateMessage(MessageType type, Decompressor& data) const;
	//
	//# \param	type	The type of message to construct.
	//# \param	data	A $@MessageMgr/Decompressor@$ object holding the raw message data.
	//
	//# \desc
	//# When the Message Manager receives a message with an application-defined type, it calls the
	//# $CreateMessage$ function to create the appropriate message object. The implementation of
	//# the $CreateMessage$ function should examine the message type passed in the $type$ parameter,
	//# create an instance of the corresponding subclass of the $@MessageMgr/Message@$ class, and return
	//# a pointer to it. If the message type is not recognized, then $CreateMessage$ should return $nullptr$.
	//#
	//# The Message Manager calls the message object's $@MessageMgr/Message::Decompress@$ function immediately
	//# after the object is created, so the $CreateMessage$ function should make no effort to initialize
	//# any components of the message object that it creates.
	//
	//# \also	$@Application::ReceiveMessage@$
	//# \also	$@MessageMgr/Message::HandleMessage@$
	//# \also	$@MessageMgr/Message@$


	//# \function	Application::ReceiveMessage		Called when an application-defined message is received.
	//
	//# \proto	virtual void ReceiveMessage(Player *sender, const NetworkAddress& address, const Message *message);
	//
	//# \param	sender		The player from whom the message originated.
	//# \param	address		The address of the machine from which the message originated.
	//#						This is only valid for connectionless messages.
	//# \param	message		The message being received.
	//
	//# \desc
	//# When the Message Manager receives a message with an application-defined type, it calls $ReceiveMessage$
	//# so that the application module may handle the message in whatever way it needs to. The
	//# implementation of the $ReceiveMessage$ function should determine the type of the message object
	//# pointed to by the $message$ parameter by calling $@MessageMgr/Message::GetMessageType@$ and then handle
	//# the message appropriately.
	//#
	//# If a connection exists with the sending machine, the $sender$ parameter contains a pointer to the sending
	//# player. (On client machines, this player will always represent the server.) If the received message was
	//# sent without a connection, then the $sender$ parameter is $nullptr$, and the network address of the
	//# sending machine is given by the $address$ parameter.
	//
	//# \also	$@Application::CreateMessage@$
	//# \also	$@MessageMgr/Message::HandleMessage@$
	//# \also	$@MessageMgr/Player@$
	//# \also	$@MessageMgr/Message@$
	//# \also	$@NetworkMgr/NetworkAddress@$


	//# \function	Application::ApplicationTask	Called once per application loop to allow the application module to perform periodic processing.
	//
	//# \proto	virtual void ApplicationTask(void);
	//
	//# \desc
	//# The $ApplicationTask$ function is called once per frame after input has been processed and before anything
	//# in the world is moved or rendered. The application module may perform any type of periodic processing that is
	//# necessary at this time. The default implementation of this function performs no action.


	class Application : public Singleton<Application>
	{
		private:

			#if C4DEBUG

				C4API virtual void DebugApplication(void);

			#elif C4OPTIMIZED

				C4API virtual void OptimizedApplication(void);

			#endif

		public:

			C4API Application();
			C4API virtual ~Application();

			C4API virtual void HandleConnectionEvent(ConnectionEvent event, const NetworkAddress& address, const void *param);
			C4API virtual void HandlePlayerEvent(PlayerEvent event, Player *player, const void *param);
			C4API virtual void HandleGameEvent(GameEvent event, const void *param);

			C4API virtual Message *CreateMessage(MessageType type, Decompressor& data) const;
			C4API virtual void ReceiveMessage(Player *sender, const NetworkAddress& address, const Message *message);

			C4API virtual EngineResult LoadWorld(const char *name);
			C4API virtual void UnloadWorld(void);

			C4API virtual void ApplicationTask(void);
	};


	C4API extern Application *TheApplication;
}


#endif

// ZYUQURM
