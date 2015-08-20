/*
 * InsteonDevice.h
 *
 *  Created on: 2014-11-14
 *      Author: etienne
 */

#ifndef INSTEONDEVICE_H_
#define INSTEONDEVICE_H_

#include "Defs.h"
#include "Util/Timer.h"
#include "InsteonNetwork.h"
#include "InsteonAddress.h"
#include "InsteonIdentity.h"
#include "InsteonMessage.h"
#include "Serial/AutoResetEvent.h"
#include <boost/signals2.hpp>
#include <boost/asio.hpp>

// Represents an individual INSTEON device on the network.
class InsteonNetwork;
class InsteonDevice
{
public:
	boost::signals2::signal<void (InsteonDevice* device)> deviceCommandTimeout;
	boost::signals2::signal<void (InsteonDevice* device)> deviceIdentified;
	boost::signals2::signal<void (InsteonDevice* device, InsteonDeviceStatus status)> deviceStatusChanged;

	InsteonDevice(InsteonNetwork *network, InsteonAddress *address, InsteonIdentity *identity);
	~InsteonDevice();

	/*
	 * Sends an INSTEON command to the device.
	 */
	void command(InsteonDeviceCommands command);

	/*
	 * Sends an INSTEON command to the device.
	 */
	void command(InsteonDeviceCommands command, uint8 value);

	/*
	 * Returns the list of INSTEON commands supported by this device.
	 */
	vector<InsteonDeviceCommands> getCommands();

	/*
	 * Gets a value that indicates the on-level of the device.
	 */
	uint8 getOnLevel();

	/*
	 * Determines the type of INSTEON device by querying the device.
	 */
	void identify();

	/*
	 * Sends an INSTEON command to the device.
	 */
	bool tryCommand(InsteonDeviceCommands command);

	/*
	 * Sends an INSTEON command to the device.
	 */
	bool tryCommand(InsteonDeviceCommands command, uint8 value);

	/*
	 * Determines the type of INSTEON device by querying the device.
	 */
	bool tryIdentify();

	/*
	 * Removes links within both the INSTEON device and the INSTEON controller for the specified group.
	 */
	bool tryUnlink(uint8 group);

	/*
	 * Removes links within both the INSTEON device and the INSTEON controller for the specified group.
	 */
	void unlink(uint8 group);

	void onMessage(InsteonMessage *message);
	InsteonAddress* getAddress() { return address; }
	InsteonIdentity* getIdentity() { return identity; }

private:
	InsteonNetwork *network;
	InsteonAddress *address;
	InsteonIdentity *identity;
	Timer *ackTimer; // timeout to receive ACK from device


	InsteonDeviceCommands pendingCommand = InsteonDeviceCommands::DeviceCommandNull; // Gets the command that is currently pending on the device, or null if no command is pending.
	static AutoResetEvent pendingEvent;
	uint8 pendingValue = 0;
	int32 pendingRetry = 0; // retry count for pending command

	enum DimmerDirection { None, Up, Down };
	DimmerDirection dimmerDirection;
	mutex lockPendingEvent;

	/*
	 * Invoked when a pending command times out
	 */
	void pendingCommandTimerCallback(boost::system::error_code err, Timer& t);

	/*
	 * If a command is pending determines whether the current message completes the pending command
	 */
	void pendingCommandAck(InsteonMessage *message);

	/*
	 * Gets a value that indicates the on-level of the device.
	 */
	bool tryGetOnLevel(uint8 &value);

	/*
	 * Blocks the current thread if a command is pending, then sets the current command as the pending command (note does not apply to all commands)
	 */
	void waitAndSetPendingCommand(InsteonDeviceCommands command, uint8 value);


	vector<uint8> getStandardMessage(InsteonAddress *address, uint8 cmd1, uint8 cmd2);
	bool tryCommandInternal(InsteonDeviceCommands command, uint8 value, bool startNewTimer);
	void clearPendingCommand();

	void onDeviceCommandTimeout();
	void onDeviceIdentified();
	void onDeviceStatusChanged(InsteonDeviceStatus status);
	void onSetButtonPressed(InsteonMessage *message);
};

#endif /* INSTEONDEVICE_H_ */
