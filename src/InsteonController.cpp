/*
 * InsteonController.cpp
 *
 *  Created on: 2014-11-13
 *      Author: etienne
 */

#include "InsteonController.h"
#include "Util/Utilities.h"
#include "Util/Log.h"
#include <boost/bind.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>

using namespace boost;

InsteonController::InsteonController(InsteonNetwork *network)
	: network (network),
	  address (new InsteonAddress(network->getMessenger()->getControllerProperties()[PropertyKey::Address])),
	  identity (new InsteonIdentity(
	                (uint8) network->getMessenger()->getControllerProperties()[PropertyKey::DevCat],
	                (uint8) network->getMessenger()->getControllerProperties()[PropertyKey::SubCat],
	                (uint8) network->getMessenger()->getControllerProperties()[PropertyKey::FirmwareVersion]
	                ))
{
	// 4 minutes
	boost::asio::io_service io_service;
	timer = new Timer(io_service, posix_time::seconds(4*60*1000), bind(&InsteonController::timer_Elapsed, this, _1, _2));
}

InsteonController::~InsteonController() {
	delete address;
	delete identity;
	delete timer;
}

void InsteonController::cancelLinkMode()
{
	if (!tryCancelLinkMode())
		throw ios_base::failure("Failed to cancel link mode");
}

void InsteonController::enterLinkMode(InsteonLinkMode mode, uint8 group)
{
	if (!tryEnterLinkMode(mode, group))
		throw ios_base::failure("Failed to enter link mode");
}

vector<InsteonDeviceLinkRecord> InsteonController::getLinks()
{
	vector<InsteonDeviceLinkRecord> links;
	if (!tryGetLinks(links))
		throw ios_base::failure("Failed to get links");
	return links;
}

void InsteonController::groupCommand(InsteonControllerGroupCommands command, uint8 group)
{
	if (command == InsteonControllerGroupCommands::StopDimming)
		throw invalid_argument("InsteonControllerGroupCommands can not be StopDimming");
	uint8 value = 0;
	if (command == InsteonControllerGroupCommands::On)
		value = 0xFF;
	groupCommand(command, group, value);
}

void InsteonController::groupCommand(InsteonControllerGroupCommands command, uint8 group, uint8 value)
{
	uint8 cmd = (uint8)command;
	vector<uint8> message = { 0x61, group, cmd, value };
	Log::writeLine(Severity_level::info, "Controller " + address->toString() + " GroupCommand(command:" + EnumString<InsteonControllerGroupCommands>::From(command) + ", group:" + Utilities::byteToString(group) + ", value:" + Utilities::byteToString(value) + ")");
	network->getMessenger()->send(message);
}

void InsteonController::onMessage(InsteonMessage *message)
{
	if (message->getMessageType() == InsteonMessageType::DeviceLink)
	{
		InsteonAddress *address = new InsteonAddress(message->getProperties()[PropertyKey::Address]);
		InsteonIdentity *identity = new InsteonIdentity((uint8)message->getProperties()[PropertyKey::DevCat], (uint8)message->getProperties()[PropertyKey::SubCat], (uint8)message->getProperties()[PropertyKey::FirmwareVersion]);
		InsteonDevice *device = network->getDevices()->add(address, identity);
		timer->stop();
		isInLinkingMode = false;
		if (linkingMode != InsteonLinkMode::Null)
		{
			if (linkingMode != InsteonLinkMode::Delete)
				onDeviceLinked(device);
			else
				onDeviceUnlinked(device);
		}
		else
		{
			onDeviceLinked(device);
		}
	}
}

void InsteonController::onDeviceLinked(InsteonDevice *device)
{
	deviceLinked(device);
}

void InsteonController::onDeviceLinkTimeout()
{
	deviceLinkTimeout();
}

void InsteonController::onDeviceUnlinked(InsteonDevice *device)
{
	deviceUnlinked(device);
}

void InsteonController::timer_Elapsed(boost::system::error_code err, Timer& t)
{
	if (err != boost::asio::error::operation_aborted)
	{
		isInLinkingMode = false;
		onDeviceLinkTimeout();
	}
}

bool InsteonController::tryCancelLinkMode()
{
	timer->stop();
	isInLinkingMode = false;
	linkingMode = InsteonLinkMode::Null;
	vector<uint8> message = { 0x65 };
	Log::writeLine(Severity_level::info, "Controller " + address->toString() + " CancelLinkMode");
	return network->getMessenger()->trySend(message) == EchoStatus::ACK;
}

bool InsteonController::tryEnterLinkMode(InsteonLinkMode mode, uint8 group)
{
	linkingMode = mode;
	vector<uint8> message = { 0x64, (uint8)mode, group };
	Log::writeLine(Severity_level::info, "Controller " + address->toString() + " EnterLinkMode(mode:" + EnumString<InsteonLinkMode>::From(mode) + ", group:" + Utilities::byteToString(group) + ")");
	if (network->getMessenger()->trySend(message) != EchoStatus::ACK)
		return false;
	timer->start();
	isInLinkingMode = true;
	return true;
}

bool InsteonController::tryGetLinks(vector<InsteonDeviceLinkRecord> &links)
{
	if (tryGetLinksInternal(links))
		return true;

	const int retryCount = 3;
	for (int retry = 0; retry < retryCount; ++retry)
	{
		Log::writeLine(Severity_level::warning, "Controller " + address->toString() + " GetLinks failed, retry " + to_string(retry) + " of " + to_string(retryCount));
		if (tryGetLinksInternal(links))
			return true;
	}

	Log::writeLine(Severity_level::error, "Controller " + address->toString() + " GetLinks failed");
	return false;
}

bool InsteonController::tryGetLinksInternal(vector<InsteonDeviceLinkRecord> &links)
{
	vector<InsteonDeviceLinkRecord> list;
	map<PropertyKey, int> properties;
	EchoStatus status = EchoStatus::None;

	Log::writeLine(Severity_level::info, "Controller " + address->toString() + " GetLinks");
	vector<uint8> message1 = { 0x69 };
	status = network->getMessenger()->trySendReceive(message1, false, 0x57, properties);
	if (status == EchoStatus::NAK)
	{
		Log::writeLine(Severity_level::info, "Controller " + address->toString() + " GetLinks returned no links, empty link table");
		return true;
	}
	else if (status == EchoStatus::ACK)
	{
		if (properties.size() > 0)
		{
			Log::writeLine(Severity_level::error, "Controller " + address->toString() + " null properties object");
			return false;
		}
		list.push_back(InsteonDeviceLinkRecord(properties));
	}
	else
	{
		return false; // echo was not ACK or NAK
	}

	Log::writeLine(Severity_level::info, "Controller " + address->toString() + " GetLinks");
	vector<uint8> message2 = { 0x6A };
	status = network->getMessenger()->trySendReceive(message2, false, 0x57, properties);
	while (status == EchoStatus::ACK)
	{
		if (properties.size() > 0)
		{
			Log::writeLine(Severity_level::error, "Controller " + address->toString() + " null properties object");
			return false;
		}
		list.push_back(InsteonDeviceLinkRecord(properties));
		status = network->getMessenger()->trySendReceive(message2, false, 0x57, properties);
	}

	if (status != EchoStatus::NAK)
		return false; // echo was not ACK or NAK

	links = list;
	Log::writeLine(Severity_level::info, "Controller " + address->toString() + " GetLinks returned " + to_string(links.size()) + " links");
	return true;
}

bool InsteonController::tryGroupCommand(InsteonControllerGroupCommands command, uint8 group)
{
	if (command == InsteonControllerGroupCommands::StopDimming)
		return false;
	uint8 value = 0;
	if (command == InsteonControllerGroupCommands::On)
		value = 0xFF;
	return tryGroupCommand(command, group, value);
}

bool InsteonController::tryGroupCommand(InsteonControllerGroupCommands command, uint8 group, uint8 value)
{
	uint8 cmd = (uint8)command;
	vector<uint8> message = { 0x61, group, cmd, value };
	Log::writeLine(Severity_level::info, "Controller " + address->toString() + " GroupCommand(command:" + EnumString<InsteonControllerGroupCommands>::From(command) + ", group:" + Utilities::byteToString(group) + ", value:" + Utilities::byteToString(value) + ")");
	return network->getMessenger()->trySend(message) == EchoStatus::ACK;
}
