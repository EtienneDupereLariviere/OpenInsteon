/*
 * InsteonDevice.cpp
 *
 *  Created on: 2014-11-14
 *      Author: etienne
 */

#include "InsteonDevice.h"
#include "Util/Log.h"
#include "Util/Utilities.h"
#include <boost/bind.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>

using namespace std;
using namespace boost;

AutoResetEvent InsteonDevice::pendingEvent(false);

InsteonDevice::InsteonDevice(InsteonNetwork *network, InsteonAddress *address, InsteonIdentity *identity)
{
	this->network = network;
	this->address = address;
	this->identity = identity;

	boost::asio::io_service io_service;
	this->ackTimer = new Timer(io_service, posix_time::seconds(deviceAckTimeout / 1000), bind(&InsteonDevice::pendingCommandTimerCallback, this, _1, _2));
}

InsteonDevice::~InsteonDevice() {
	delete address;
	delete identity;
	delete ackTimer;
}

void InsteonDevice::command(InsteonDeviceCommands cmd)
{
	if (cmd == InsteonDeviceCommands::DeviceOn)
		command(cmd, 0xFF);
	else
		command(cmd, 0x00);
}

void InsteonDevice::command(InsteonDeviceCommands command, uint8 value)
{
	if (!tryCommand(command, value))
		throw ios_base::failure("Failed to send command '" + EnumString<InsteonDeviceCommands>::From(command) + "' for device '" + address->toString() + "'");
}

vector<InsteonDeviceCommands> InsteonDevice::getCommands()
{
	vector<InsteonDeviceCommands> commands;
	switch (identity->getDevCat())
	{
		case 0x01: // SwitchLinc Dimmer, LampLinc, OutletLinc Dimmer, KeypadLinc Dimmer, ...
			commands.push_back(InsteonDeviceCommands::DeviceOn);
			commands.push_back(InsteonDeviceCommands::DeviceOff);
			commands.push_back(InsteonDeviceCommands::DeviceFastOn);
			commands.push_back(InsteonDeviceCommands::DeviceFastOff);
			commands.push_back(InsteonDeviceCommands::DeviceBrighten);
			commands.push_back(InsteonDeviceCommands::DeviceDim);
			commands.push_back(InsteonDeviceCommands::DeviceStartDimming);
			commands.push_back(InsteonDeviceCommands::DeviceStopDimming);
			break;

		case 0x02: // SwitchLinc On/Off, ApplianceLinc, OutletLinc, KeypadLinc On/Off, ...
		case 0x09: // Load Controller, ...
			commands.push_back(InsteonDeviceCommands::DeviceOn);
			commands.push_back(InsteonDeviceCommands::DeviceOff);
			break;

		case 0x00: // RemoteLinc, ...
		case 0x03: // PowerLinc, SmartLinc, ...
		case 0x05: // TempLinc, ...
		case 0x07: // IOLinc, ...
		case 0x10: // TriggerLinc, Motion sensors
		default:
			break;
	}
	return commands;
}

uint8 InsteonDevice::getOnLevel()
{
	uint8 value;
	if (!tryGetOnLevel(value))
		throw ios_base::failure("Failed to get level");
	return value;
}

vector<uint8> InsteonDevice::getStandardMessage(InsteonAddress *address, uint8 cmd1, uint8 cmd2)
{
	uint8 byte2, byte1, byte0;

	address->getByteIndex(2, byte2);
	address->getByteIndex(1, byte1);
	address->getByteIndex(0, byte0);
	vector<uint8> message = { 0x62, byte2, byte1, byte0, 0x0F, cmd1, cmd2 };
	return message;
}

void InsteonDevice::identify()
{
	delete identity;
	identity = new InsteonIdentity(0, 0, 0);
	command(InsteonDeviceCommands::IDRequest);
}

void InsteonDevice::onDeviceCommandTimeout()
{
	deviceCommandTimeout(this);
	network->getDevices()->onDeviceCommandTimeout(this);
}

void InsteonDevice::onDeviceIdentified()
{
	deviceIdentified(this);
	network->getDevices()->onDeviceIdentified(this);
}

void InsteonDevice::onDeviceStatusChanged(InsteonDeviceStatus status)
{
	deviceStatusChanged(this, status);
	network->getDevices()->onDeviceStatusChanged(this, status);
}

void InsteonDevice::onSetButtonPressed(InsteonMessage *message)
{
	if (identity->isEmpty())
	{
		uint8 devCat = (uint8)message->getProperties()[PropertyKey::DevCat];
		uint8 subCat = (uint8)message->getProperties()[PropertyKey::DevCat];
		uint8 firmwareVersion = (uint8)message->getProperties()[PropertyKey::DevCat];
		identity = new InsteonIdentity(devCat, subCat, firmwareVersion);
	}
	onDeviceIdentified();
}

void InsteonDevice::onMessage(InsteonMessage *message)
{
	switch (message->getMessageType())
	{
		case InsteonMessageType::Ack:
			pendingCommandAck(message);
			break;

		case InsteonMessageType::OnCleanup:
			onDeviceStatusChanged(InsteonDeviceStatus::DeviceStatusOn);
			break;

		case InsteonMessageType::OffCleanup:
			onDeviceStatusChanged(InsteonDeviceStatus::DeviceStatusOff);
			break;

		case InsteonMessageType::FastOnCleanup:
			onDeviceStatusChanged(InsteonDeviceStatus::DeviceStatusOn);
			onDeviceStatusChanged(InsteonDeviceStatus::DeviceStatusFastOn);
			break;

		case InsteonMessageType::FastOffCleanup:
			onDeviceStatusChanged(InsteonDeviceStatus::DeviceStatusOff);
			onDeviceStatusChanged(InsteonDeviceStatus::DeviceStatusFastOff);
			break;

		case InsteonMessageType::IncrementBeginBroadcast:
			dimmerDirection = message->getProperties()[PropertyKey::IncrementDirection] != 0 ? DimmerDirection::Up : DimmerDirection::Down;
			break;

		case InsteonMessageType::IncrementEndBroadcast:
			if (dimmerDirection == DimmerDirection::Up)
			{
				onDeviceStatusChanged(InsteonDeviceStatus::DeviceStatusBrighten);
			}
			else if (dimmerDirection == DimmerDirection::Down)
			{
				onDeviceStatusChanged(InsteonDeviceStatus::DeviceStatusDim);
			}
			break;

		case InsteonMessageType::SetButtonPressed:
			onSetButtonPressed(message);
			break;
	}
}

int isInEnum(int32 x)
{
    for(uint32 i=0; i<(sizeof(deviceCommandsValues)/sizeof(*deviceCommandsValues)); i++)
    {
        if(deviceCommandsValues[i] == x){ return 1; }
    }
    return 0;
}

void InsteonDevice::pendingCommandAck(InsteonMessage *message)
{
	lockPendingEvent.lock();
	if (pendingCommand != InsteonDeviceCommands::DeviceCommandNull)
	{
		int32 cmd1 = message->getProperties()[PropertyKey::Cmd1];
		if (isInEnum(cmd1))
		{
			InsteonDeviceCommands command = (InsteonDeviceCommands)cmd1;
			if (pendingCommand == command)
			{
				pendingCommand = InsteonDeviceCommands::DeviceCommandNull;
				pendingValue = 0;
				ackTimer->stop(); // stop ACK timeout timer
				pendingEvent.set(); // unblock any thread that may be waiting on the pending command
			}
		}
	}
	lockPendingEvent.unlock();
}

void InsteonDevice::clearPendingCommand()
{
	lockPendingEvent.lock();
		pendingCommand = InsteonDeviceCommands::DeviceCommandNull;
		pendingValue = 0;
		ackTimer->stop(); // stop ACK timeout timer
		pendingEvent.set(); // unblock any thread that may be waiting on the pending command
	lockPendingEvent.unlock();
}

void InsteonDevice::pendingCommandTimerCallback(system::error_code err, Timer& t)
{
	lockPendingEvent.lock();
	if (err != boost::asio::error::operation_aborted)
	{
		bool retry = false;
		InsteonDeviceCommands command = InsteonDeviceCommands::DeviceOn;
		uint8 value = 0;
		int retryCount = 0;

		if (pendingCommand == InsteonDeviceCommands::DeviceCommandNull)
		{
			lockPendingEvent.unlock();
			return;
		}

		pendingRetry += 1;
		if (pendingRetry <= deviceCommandRetries)
		{
			retry = true;
			value = pendingValue;
			retryCount = pendingRetry;
		}
		else
		{
			retry = false;
			command = pendingCommand;
			pendingCommand = InsteonDeviceCommands::DeviceCommandNull;
			pendingValue = 0;
			pendingEvent.set(); // unblock any thread that may be waiting on the pending command
		}

		if (retry)
		{
			Log::writeLine(Severity_level::warning, "Device " + address->toString() + " Command " + EnumString<InsteonDeviceCommands>::From(command) + " timed out, retry " + to_string(retryCount) + " of " + to_string(deviceCommandRetries) + "...");
			tryCommandInternal(command, value, false);
		}
		else
		{
			Log::writeLine(Severity_level::error, "Device " + address->toString() + " Command " + EnumString<InsteonDeviceCommands>::From(command) + " timed out");
			onDeviceCommandTimeout();
		}
	}
	lockPendingEvent.unlock();
}

bool InsteonDevice::tryCommand(InsteonDeviceCommands command)
{
	if (command == InsteonDeviceCommands::DeviceOn)
		return tryCommand(command, 0xFF);
	else
		return tryCommand(command, 0x00);
}

bool InsteonDevice::tryCommand(InsteonDeviceCommands command, uint8 value)
{
	waitAndSetPendingCommand(command, value);
	return tryCommandInternal(command, value, true);
}

bool InsteonDevice::tryCommandInternal(InsteonDeviceCommands command, uint8 value, bool startNewTimer)
{
	vector<uint8> message = getStandardMessage(address, (uint8) command, value);
	Log::writeLine(Severity_level::info, "Device " + address->toString() + " Command(command:" + EnumString<InsteonDeviceCommands>::From(command) + ", value:" + Utilities::byteToString(value) + ")");

	EchoStatus status = network->getMessenger()->trySend(message);
	if (status == EchoStatus::ACK)
	{
		if(startNewTimer)
			ackTimer->start(); // start ACK timeout timer
		else
			ackTimer->continuePeriod(); // restart ACK timeout timer
		return true;
	}
	else
	{
		clearPendingCommand();
		return false;
	}
}

bool InsteonDevice::tryGetOnLevel(uint8 &value)
{

    InsteonDeviceCommands command = InsteonDeviceCommands::StatusRequest;
    waitAndSetPendingCommand(command, 0);
    Log::writeLine(Severity_level::info, "Device " + address->toString() + " GetOnLevel");
    vector<uint8> message = getStandardMessage(address, (uint8)command, 0);
    map<PropertyKey, int32> properties;
    EchoStatus status = network->getMessenger()->trySendReceive(message, true, 0x50, properties); // on-level returned in cmd2 of ACK
    if (status == EchoStatus::ACK && properties.size() > 0)
    {
        value = (uint8)properties[PropertyKey::Cmd2];
        Log::writeLine(Severity_level::info, "Device " + address->toString() + " GetOnLevel returning " + Utilities::byteToString(value));
        return true;
    }
    else
    {
        clearPendingCommand();
        value = 0;
        return false;
    }
}

bool InsteonDevice::tryIdentify()
{
	delete identity;
	identity = new InsteonIdentity(0, 0, 0);
	return tryCommand(InsteonDeviceCommands::IDRequest);
}

bool InsteonDevice::tryUnlink(uint8 group)
{
	if (network->getController()->tryEnterLinkMode(InsteonLinkMode::Delete, group))
		return tryCommand(InsteonDeviceCommands::EnterLinkingMode, group);
	else
		return false;
}

void InsteonDevice::waitAndSetPendingCommand(InsteonDeviceCommands command, uint8 value)
{
	InsteonDeviceCommands latchedPendingCommand;

	lockPendingEvent.lock();
		if (pendingCommand == InsteonDeviceCommands::DeviceCommandNull)
		{
			pendingCommand = command;
			pendingValue = value;
			pendingRetry = 0;
			lockPendingEvent.unlock();
			return;
		}
		latchedPendingCommand = pendingCommand;
	lockPendingEvent.unlock();

	// block current thread if a command is pending
	Log::writeLine(Severity_level::info, "Device " + address->toString() + " blocking command " + EnumString<InsteonDeviceCommands>::From(command) + " for pending command " + EnumString<InsteonDeviceCommands>::From(latchedPendingCommand));
	pendingEvent.reset();
	if (!pendingEvent.waitOne(deviceAckTimeout)) // wait at most deviceAckTimeout seconds
	{
		clearPendingCommand(); // break deadlock and warn
		Log::writeLine(Severity_level::warning, "Device " + address->toString() + " unblocking command " + EnumString<InsteonDeviceCommands>::From(command) + " for pending command " + EnumString<InsteonDeviceCommands>::From(latchedPendingCommand));
	}

	waitAndSetPendingCommand(command, value); // try again
}

void InsteonDevice::unlink(uint8 group)
{
	network->getController()->enterLinkMode(InsteonLinkMode::Delete, group);
	command(InsteonDeviceCommands::EnterUnlinkingMode, group);
}
