/*
 * InsteonMessenger.h
 *
 *  Created on: 2014-11-11
 *      Author: etienne
 */

#ifndef INSTEONMESSENGER_H_
#define INSTEONMESSENGER_H_

#include "Serial/AutoResetEvent.h"
#include "InsteonNetworkBridge.h"
#include "InsteonNetwork.h"
#include "InsteonMessage.h"
#include "Defs.h"
#include <vector>
#include <mutex>
#include <list>
#include <set>

using namespace std;

class InsteonNetwork;
class WaitItem
{
public:
	WaitItem(uint8 messageId)
	{
		this->messageId = messageId;
		this->messageEvent = new AutoResetEvent(false);
		this->message = NULL;
	}

	~WaitItem()
	{
		delete messageEvent;
		delete message;
	}

	uint8 getMessageId() { return messageId; }
	AutoResetEvent getMessageEvent() { return messageEvent; }
	InsteonMessage* getMessage() { return message; }
	void setMessage(InsteonMessage *message) { this->message = message; }

private:
	uint8 messageId;
	AutoResetEvent *messageEvent;
	InsteonMessage *message;
};


// This class is responsible for processing raw messages into structured property lists and dispatching the result to individual device objects.
// The responsibilities of the messenger include:
//  - Owning the network bridge to the physical INSTEON network.
//  - Providing the ability to send messages to the controller for other classes in the module.
//  - Processing raw message bytes into structured property lists.
//  - Determining the logical device object to which the message is directed and dispatching the message to that object.
//  - Reporting back to the bridge whether or not each message is valid, and if valid the size in bytes of the message.
class InsteonMessenger : public IMessageProcessor
{
public:
	InsteonMessenger(InsteonNetwork *network);
	~InsteonMessenger();

	void close();
	void connect(InsteonConnection *connection);
	bool tryConnect(InsteonConnection *connection);

	void send(vector<uint8> message);
	void sendReceive(vector<uint8> message, uint8 receiveMessageId, map<PropertyKey, int32> &properties);

	EchoStatus trySend(vector<uint8> message);
	EchoStatus trySend(vector<uint8> message, bool retryOnNak);
	EchoStatus trySend(vector<uint8> message, bool retryOnNak, int32 echoLength);

	EchoStatus trySendEchoCommand(vector<uint8> message, bool retryOnNak, int32 echoLength, map<PropertyKey, int32> &properties);
	EchoStatus trySendReceive(vector<uint8> message, bool retryOnNak, uint8 receiveMessageId, map<PropertyKey, int32> &properties);
	bool verifyConnection();

	bool isConnected() { return bridge->isConnected(); }
	map<PropertyKey, int32> getControllerProperties() { return controllerProperties; }

private:
	InsteonNetwork *network;
    InsteonNetworkBridge *bridge;
    InsteonMessage *echoMessage = NULL;

    vector<WaitItem*> waitList;
    set<string> duplicates; // used to detect duplicate messages
    vector<uint8> sentMessage; // bytes of last sent message, used to match the echo
    bool echoCommand = false;

    map<PropertyKey, int32> controllerProperties;
    mutex lockBridge;
    mutex lockWaitList;
    mutex lockDuplicates;

    void duplicateMessageTimerCallback(string key);
    bool isDuplicateMessage(InsteonMessage *message);
    void updateWaitItems(InsteonMessage *message);
    void onMessage(InsteonMessage *message);

	bool processMessage(vector<uint8> data, uint32 offset, int32 &count);
	bool processEcho(vector<uint8> data, uint32 offset, int32 &count);
	void setEchoStatus(EchoStatus status);
};

#endif /* INSTEONMESSENGER_H_ */
