/*
 * InsteonMessenger.cpp
 *
 *  Created on: 2014-11-11
 *      Author: Étienne Dupéré Larivière
 */

#include "InsteonMessenger.h"
#include "InsteonMessageProcessor.h"
#include "Util/Log.h"
#include "Util/Utilities.h"
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/thread/thread.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>

using namespace std;
using namespace boost;

InsteonMessenger::InsteonMessenger(InsteonNetwork *network)
{
	if (network == NULL)
		throw invalid_argument( "received null network" );

	this->network = network;
	this->bridge = new InsteonNetworkBridge(this);
}

InsteonMessenger::~InsteonMessenger()
{
	delete bridge;
}

void InsteonMessenger::close()
{
	lockBridge.lock();
		bridge->close();
	lockBridge.unlock();

	network->disconnect();
}

void InsteonMessenger::connect(InsteonConnection *connection)
{
	lockBridge.lock();
		bridge->connect(connection, controllerProperties);
	lockBridge.unlock();

	Log::writeLine(Severity_level::info, "Connected to '" + connection->toString() + "'");

	vector<uint8> message = { 0x6B, 0x48 }; // disable deadman
	send(message);
}

void InsteonMessenger::duplicateMessageTimerCallback(string key)
{
	lockDuplicates.lock();
		if (duplicates.find(key) != duplicates.end())
			duplicates.erase(key);
	lockDuplicates.unlock();
}

bool InsteonMessenger::isDuplicateMessage(InsteonMessage *message)
{
	lockDuplicates.lock();
		// determine if message key matches an entry in the list
		for(auto it=duplicates.begin(); it!=duplicates.end(); ++it)
		{
			if (message->getKey() == *it)
			{
				lockDuplicates.unlock();
				return true;
			}
		}

		// create a new duplicate entry and timer
		asio::io_service io;
		asio::deadline_timer dt(io, posix_time::seconds(1));
		dt.async_wait(bind(&InsteonMessenger::duplicateMessageTimerCallback, this, message->getKey()));
		thread timerThread(bind(&asio::io_service::run, &io));

		duplicates.insert(message->getKey());
	lockDuplicates.unlock();

	return false;
}

void InsteonMessenger::onMessage(InsteonMessage *message)
{
	map<PropertyKey, int32> properties = message->getProperties();
	if (properties.find(PropertyKey::FromAddress) != properties.end())
	{
		int32 address = properties[PropertyKey::FromAddress];
		if (network->getDevices()->containsKey(address))
		{
			Log::writeLine(Severity_level::info, "Device " + InsteonAddress::format(address) + " received message " + message->toString());
			InsteonDevice *device = network->getDevices()->find(address);
			device->onMessage(message);
		}
		else if (message->getMessageType() == InsteonMessageType::SetButtonPressed)
		{
			// don't warn about SetButtonPressed message from unknown devices, because it may be from a device about to be added
		}
		else if (network->autoAdd)
		{
			Log::writeLine(Severity_level::info, "Unknown device " + InsteonAddress::format(address)  + " received message " + message->toString() + ", adding device");
			InsteonDevice *device = network->getDevices()->add(new InsteonAddress(address), new InsteonIdentity(0, 0, 0));
			device->onMessage(message);
		}
		else
		{
			Log::writeLine(Severity_level::warning, "Unknown device " + InsteonAddress::format(address)  + " received message " + message->toString());
		}
	}
	else
	{
		Log::writeLine(Severity_level::info, "Controller received message " + message->toString());
		network->getController()->onMessage(message);
	}
}

void InsteonMessenger::send(vector<uint8> message)
{
	if (trySend(message, true) != EchoStatus::ACK)
		throw ios_base::failure("Failed to send message '" + Utilities::byteArrayToString(message) + "'.");
}

void InsteonMessenger::sendReceive(vector<uint8> message, uint8 receiveMessageId, map<PropertyKey, int32> &properties)
{
	if (trySendReceive(message, true, receiveMessageId, properties) != EchoStatus::ACK)
		throw ios_base::failure("Failed to send message '" + Utilities::byteArrayToString(message) + "'.");
}

bool InsteonMessenger::tryConnect(InsteonConnection *connection)
{
	if (connection != NULL)
	{
		try
		{
			Log::writeLine(Severity_level::info, "Trying connection '" + connection->toString() + "'...");

			lockBridge.lock();
				bridge->connect(connection, controllerProperties);
			lockBridge.unlock();
			Log::writeLine(Severity_level::info, "Connected to '" + connection->toString() + "'");

			vector<uint8> message = { 0x6B, 0x48 }; // disable deadman
			trySend(message);

			return true;
		}
		catch (const std::exception& ex)
		{
			lockBridge.unlock();
			string error(ex.what());
			Log::writeLine(Severity_level::error, "Could not connect to '" + connection->toString() + "'. Error: " + error);
		}
	}
	return false;
}

EchoStatus InsteonMessenger::trySend(vector<uint8> message)
{
	return trySend(message, true);
}

EchoStatus InsteonMessenger::trySend(vector<uint8> message, bool retryOnNak)
{
	return trySend(message, retryOnNak, message.size());
}

EchoStatus InsteonMessenger::trySend(vector<uint8> message, bool retryOnNak, int32 echoLength)
{
	EchoStatus status = EchoStatus::None;

	lockBridge.lock();
		sentMessage = message;
		try
		{
			status = bridge->send(message, retryOnNak, echoLength);
		}
		catch (const std::exception& ex)
		{
			lockBridge.unlock();
			string error(ex.what());
			Log::writeLine(Severity_level::error, "Invalid_argument... " + error);
		}
		sentMessage.clear();

	lockBridge.unlock();

	if (status == EchoStatus::None)
	{
		Log::writeLine(Severity_level::error, "No response from serial port");
		network->onDisconnected();
	}

	return status;
}

EchoStatus InsteonMessenger::trySendEchoCommand(vector<uint8> message, bool retryOnNak, int32 echoLength, map<PropertyKey, int32> &properties)
{
	echoMessage = NULL;

	echoCommand = true;
	EchoStatus status = trySend(message, retryOnNak, echoLength);
	echoCommand = false;

	if(echoMessage != NULL)
	{
		properties = echoMessage->getProperties();
		delete echoMessage;
	}

	echoMessage = NULL;
	return status;
}

EchoStatus InsteonMessenger::trySendReceive(vector<uint8> message, bool retryOnNak, uint8 receiveMessageId, map<PropertyKey, int32> &properties)
{
	WaitItem item(receiveMessageId);

	lockWaitList.lock();
		auto it = waitList.emplace(waitList.end(), &item);
	lockWaitList.unlock();

	EchoStatus status = trySend(message, retryOnNak);
	if (status == EchoStatus::ACK)
	{
		if (item.getMessage() == NULL)
			item.getMessageEvent().waitOne(sendReceiveTimeout);
		if (item.getMessage() != NULL)
			properties = item.getMessage()->getProperties();
		else
			Log::writeLine(Severity_level::error, "Did not receive expected message reply; SentMessage='" + Utilities::byteArrayToString(message) + "', ExpectedReceiveMessageId=" + Utilities::byteToString(receiveMessageId) + ", Timeout=" + to_string(sendReceiveTimeout) + "ms");
	}

	lockWaitList.lock();
		waitList.erase(it);
	lockWaitList.unlock();

	return status;
}

void InsteonMessenger::updateWaitItems(InsteonMessage *message)
{
	lockWaitList.lock();
		bool isItemUpdate = false;
		for (uint32 i = 0; i < waitList.size(); ++i)
		{
			WaitItem *item = waitList[i];
			if (message->getMessageId() == item->getMessageId())
				if (item->getMessage() == NULL)
				{
					isItemUpdate = true;
					item->setMessage(message);
					item->getMessageEvent().set();
				}
		}

		if(!isItemUpdate)
			delete message;
	lockWaitList.unlock();
}

bool InsteonMessenger::verifyConnection()
{
	if (!bridge->isConnected())
			return false;

	vector<uint8> message = { 0x60 };
	map<PropertyKey, int32> properties;
	EchoStatus status = trySendEchoCommand(message, true, 7, properties);
	if (status == EchoStatus::ACK || status == EchoStatus::NAK)
		return true;

	Log::writeLine(Severity_level::error, "Verify connection failed");
	network->onDisconnected();
	return false;
}

bool InsteonMessenger::processMessage(vector<uint8> data, uint32 offset, int32 &count)
{
	InsteonMessage *message;
	if (InsteonMessageProcessor::processMessage(data, offset, count, message))
	{
		if (!isDuplicateMessage(message))
		{
			Log::writeLine(Severity_level::info, "PROCESSOR: Message '" + Utilities::byteArrayToString(data, offset, count) + "' processed...\r\n" + message->toString("Log"));
			onMessage(message);
			updateWaitItems(message);
		}
		else
		{
			Log::writeLine(Severity_level::info, "PROCESSOR: Message '" + Utilities::byteArrayToString(data, offset, count) + "' duplicate ignored...\r\n" + message->toString("Log"));
			delete message;
		}
		return true;
	}
	else
	{
		return false;
	}
}

bool InsteonMessenger::processEcho(vector<uint8> data, uint32 offset, int32 &count)
{
	vector<uint8> message = Utilities::arraySubset(data, offset, sentMessage.size());
	if (echoCommand)
	{
		if (InsteonMessageProcessor::processMessage(data, offset, count, echoMessage))
		{
			Log::writeLine(Severity_level::info, "PROCESSOR: Echo '" + Utilities::byteArrayToString(data, offset, count) + "' processed...\r\n" + echoMessage->toString("Log"));
			return true;
		}
		else
		{
			return false;
		}
	}
	else if (Utilities::arraySequenceEquals(sentMessage, message))
	{
		count = sentMessage.size();
		Log::writeLine(Severity_level::info, "PROCESSOR: Echo '" + Utilities::byteArrayToString(data, offset, count) + "' matched");
		return true;
	}
	else
	{
		count = 0;
		return false;
	}
}

void InsteonMessenger::setEchoStatus(EchoStatus status)
{

}


